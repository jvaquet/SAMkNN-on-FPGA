#include "samknn.h"
#include "./memory/memory.h"
#include "./knn_out/knn_out.h"
#include "./knn_out/prediction_histories.h"
#include "./opt_size/candidate_size_gen.h"
#include "./opt_size/performance_counter.h"
#include "./kn_unit/kn_unit.h"
#include "./common/best_class.h"
#include "./common/correct_class.h"
#include "hls_stream.h"
#include <cstdint>

void samknn_top(hls::stream<uint32_t> &data, hls::stream<uint32_t> &labels, hls::stream<uint32_t> &predictions) {

    #pragma HLS INTERFACE mode=axis register_mode=both port=data register
    #pragma HLS INTERFACE mode=axis register_mode=both port=labels register
    #pragma HLS INTERFACE mode=axis register_mode=both port=predictions register

    #pragma HLS interface mode=ap_ctrl_none port=return

    // Initialize everything
    Memory::init();
    PerformanceCounter::reset();

    label_t cur_label, cur_prediction;
    datapoint_t cur_datapoint[N_DATAPOINT_DIMENSIONS];
    dimension_idx_t cur_dim;

    samknn_top_loop:
    //while(!data.empty()) { //For testing
    while(true) {
        samknn_top_read_datapoint:
        for(dimension_idx_t cur_dim = 0; cur_dim < N_DATAPOINT_DIMENSIONS; cur_dim++) {
            cur_datapoint[cur_dim] = data.read();
        }
        cur_label = labels.read();
        predict_train(cur_datapoint, cur_label, cur_prediction);
        predictions.write(cur_prediction);
    }
}

void predict_train(datapoint_t datapoint[N_DATAPOINT_DIMENSIONS], label_t ref_label, label_t& label) {
    
    // (1) Make predictions using all three memories 

    // Define Memory ranges
    memory_idx_t address_stm_start = 0;
    memory_idx_t address_stm_end = Memory::stm_pointer;
    memory_idx_t address_ltm_start = Memory::ltm_pointer+1;
    memory_idx_t address_ltm_end = MEM_SIZE;

    // Predict STM/ LTM
    label_t stm_labels[K_NEIGHBORS], ltm_labels[K_NEIGHBORS];
    dist_t stm_dists[K_NEIGHBORS], ltm_dists[K_NEIGHBORS];

    KNUnit::run(datapoint, address_stm_start, address_stm_end,
            stm_dists, stm_labels);
    KNUnit::run(datapoint, address_ltm_start, address_ltm_end,
            ltm_dists, ltm_labels);    

    // Combine predictions
    label_t prediction;
    get_output(stm_labels, stm_dists, ltm_labels, ltm_dists,
			ref_label, prediction);

    // (2) Append Datapoint to STM
    Memory::append_stm(datapoint, ref_label);

    // (3) Prevent STM and LTM from colliding
    if(Memory::stm_pointer > Memory::ltm_pointer) {

        if((MEM_SIZE - Memory::ltm_pointer) < MAX_LTM_SIZE) {
            memory_idx_t enlarge_ltm_by = MAX_LTM_SIZE - (MEM_SIZE - Memory::ltm_pointer);
            Memory::stm_to_ltm(enlarge_ltm_by);
            PredictionHistories::shorten_history(enlarge_ltm_by);
            PerformanceCounter::reset();
        }

        Memory::cluster_down();
    }

    // (4) Clean LTM using latest sample
    address_stm_start = 0;
    memory_idx_t address_stm_end_exclude_latest = Memory::stm_pointer-1;
    address_ltm_start = Memory::ltm_pointer+1;
    address_ltm_end = MEM_SIZE;

    bool clean_needed;
    KNUnit::clean(datapoint, ref_label, 
            address_stm_start, address_stm_end_exclude_latest,
            address_ltm_start, address_ltm_end, clean_needed);

    if(clean_needed) {
        memory_idx_t n_stm_samples_cleaned;
        Memory::clean(n_stm_samples_cleaned);
    }
    

    // (5) Try out and apply new optimal size
    // Generate candidate sizes
    candidate_size_idx_t n_candidate_sizes;
    memory_idx_t candidate_sizes[MAX_CANDIDATE_SIZES];
    memory_idx_t cur_ltm_size = Memory::stm_pointer;
    generate_candidate_sizes(cur_ltm_size, n_candidate_sizes, candidate_sizes);

    // Return, if no candidate sizes need to be tested
    if (n_candidate_sizes < 2) {
        label = prediction;
        return;
    }

    // Update all relevant performance counters
    samknn_update_performance_counters:
    for(candidate_size_idx_t i = 0; i < MAX_CANDIDATE_SIZES; i++) {
        if(i < n_candidate_sizes) {
            memory_idx_t start_addr = cur_ltm_size - candidate_sizes[i];

            if(PerformanceCounter::start_addresses[i] == start_addr) {
                // If counter is present, update with latest sample
                bool correct_pred;
                KNUnit::predict_correct(datapoint, ref_label, 
                        start_addr, address_stm_end_exclude_latest,
                        correct_pred);
                
                PerformanceCounter::candidate_scores[i] += correct_pred;
                #if MAX_ACC_APPROX
                PerformanceCounter::append(i, correct_pred);
                #endif

            #if MAX_ACC_APPROX
            } else if(PerformanceCounter::start_addresses[i] == start_addr-1) {
                // If counter is off by one, update with latest sample and remove first entry from history
                PerformanceCounter::start_addresses[i]++;

                bool correct_pred;
                KNUnit::predict_correct(datapoint, ref_label, 
                        start_addr, address_stm_end_exclude_latest,
                        correct_pred);

                PerformanceCounter::candidate_scores[i] += correct_pred;

                PerformanceCounter::append(i, correct_pred);

                bool correct_first;
                PerformanceCounter::pop(i, correct_first);
                PerformanceCounter::candidate_scores[i] -= correct_first;
            #endif

            } else {
                // If counter is absent, create from scratch
                PerformanceCounter::candidate_scores[i] = 0;

                samknn_create_performance_counter:
                for(memory_idx_t cur_end_addr = start_addr+K_NEIGHBORS; cur_end_addr <= address_stm_end; cur_end_addr++) {
                    bool correct_pred;
                    KNUnit::predict_correct(Memory::data[Memory::active_buffer][cur_end_addr], Memory::labels[Memory::active_buffer][cur_end_addr], 
                            start_addr, cur_end_addr-1,
                            correct_pred);
                    PerformanceCounter::candidate_scores[i] += correct_pred;
                    #if MAX_ACC_APPROX
                    PerformanceCounter::append(i, correct_pred);
                    #endif
                }
                PerformanceCounter::start_addresses[i] = start_addr;
            }
        } else {
            PerformanceCounter::candidate_scores[i] = 0;
        }
    }

    memory_idx_t optimal_stm_size;
    PerformanceCounter::get_optimal_size(optimal_stm_size, PerformanceCounter::candidate_scores, n_candidate_sizes, candidate_sizes);

    if (optimal_stm_size != Memory::stm_pointer) {
        memory_idx_t stm_split_pointer = Memory::stm_pointer - optimal_stm_size;

        memory_idx_t address_stm_keep_start = stm_split_pointer;
        memory_idx_t address_stm_keep_end = Memory::stm_pointer;
        memory_idx_t address_stm_clean_start = 0;
        memory_idx_t address_stm_clean_end = stm_split_pointer;

        bool clean_needed = false;

        samknn_clean_stm_for_transfer:
        for (memory_idx_t address = address_stm_keep_start; address < address_stm_keep_end; address++) {
            #pragma HLS loop_tripcount min=0 max=MEM_SIZE avg=MEM_SIZE/2

            label_t cur_label;
            bool cur_clean_needed;
            KNUnit::clean(Memory::data[Memory::active_buffer][address], Memory::labels[Memory::active_buffer][address], 
                    address_stm_keep_start, address_stm_keep_end,
                    address_stm_clean_start, address_stm_clean_end,
                    cur_clean_needed);
            clean_needed |= cur_clean_needed;
        }

        memory_idx_t stm_samples_cleaned = 0;
        if (clean_needed) {
            Memory::clean(stm_samples_cleaned);
        }

        Memory::stm_to_ltm(stm_split_pointer - stm_samples_cleaned);
        PredictionHistories::shorten_history(stm_split_pointer);
        PerformanceCounter::reset();
    }

    label = prediction;
}
