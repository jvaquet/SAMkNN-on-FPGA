
#include "../samknn_config.h"
#include "../samknn_types.h"
#include "calc_dist.h"
#include "farthest_correct.h"
#include "k_best_collector.h"
#include "runs_combiner.h"
#include "kn_unit.h"
#include "../common/best_class.h"
#include "../common/correct_class.h"
#include "../memory/memory.h"
#include "../opt_size/performance_counter.h"


void KNUnit::run(datapoint_t ref_datapoint[N_DATAPOINT_DIMENSIONS], 
        memory_idx_t address_start, memory_idx_t address_end,
        dist_t dists[K_NEIGHBORS], label_t labels[K_NEIGHBORS]) {
    #pragma HLS INLINE off

    #if N_PARALLEL_RUNS==1

    // Reset K best collectors
    kn_unit_run_reset: 
    for(neighbor_idx_t i = 0; i < K_NEIGHBORS; i++) {
        #pragma HLS unroll
        labels[i] = 0;
        dists[i] = MAX_DIST;
    }

    kn_unit_run_calc: 
    for(memory_idx_t address = address_start; address < address_end; address++) {
        #pragma HLS loop_tripcount min=0 max=MEM_SIZE avg=MEM_SIZE/2

        // Calculate distance for current address
        dist_t cur_dist;
        calc_dist(ref_datapoint, Memory::data[Memory::active_buffer][address], cur_dist);
        KBestCollector::append(cur_dist, Memory::labels[Memory::active_buffer][address],
                dists, labels);
    }

    #else

    dist_t dists_collectors[N_PARALLEL_RUNS][K_NEIGHBORS];
    label_t label_collectors[N_PARALLEL_RUNS][K_NEIGHBORS];
    #pragma HLS array_partition variable=dists_collectors complete dim=1
    #pragma HLS array_partition variable=label_collectors complete dim=1

    for(memory_idx_t i = 0; i < N_PARALLEL_RUNS; i++) {
        for(memory_idx_t k = 0; k < K_NEIGHBORS; k++) { 
            dists_collectors[i][k] = MAX_DIST;
            label_collectors[i][k] = 0;
        }
    }

    kn_unit_run_calc: 
    for(memory_idx_t address = address_start; address < address_end; address+=N_PARALLEL_RUNS) {
        #pragma HLS loop_tripcount min=0 max=MEM_SIZE/N_PARALLEL_RUNS avg=(MEM_SIZE/2)/N_PARALLEL_RUNS

        // Calculate distances for current address
        for(parallel_runs_idx_t i = 0; i < N_PARALLEL_RUNS; i++) {
            #pragma HLS UNROLL
            memory_idx_t cur_address = address+i;
            if(cur_address < address_end) {
                dist_t cur_dist;
                calc_dist(ref_datapoint, Memory::data[Memory::active_buffer][cur_address], cur_dist);
                KBestCollector::append(cur_dist, Memory::labels[Memory::active_buffer][cur_address],
                        dists_collectors[i], label_collectors[i]);
            }
        }
    }

    // Combine
    runs_combiner(label_collectors,
		dists_collectors,
		labels, dists);
    #endif
}

void KNUnit::predict_correct(datapoint_t ref_datapoint[N_DATAPOINT_DIMENSIONS],  label_t ref_label, 
        memory_idx_t address_start, memory_idx_t address_end,
        bool &correct) {

    #pragma HLS INLINE
        
    dist_t dists[K_NEIGHBORS];
    label_t labels[K_NEIGHBORS];

    run(ref_datapoint, 
        address_start, address_end, 
        dists, labels);

    label_t pred_label;
    best_class(labels, pred_label);
    correct_class(pred_label, ref_label, correct);
}


void KNUnit::clean(datapoint_t ref_datapoint[N_DATAPOINT_DIMENSIONS], label_t ref_label, 
        memory_idx_t address_start, memory_idx_t address_end,
        memory_idx_t clean_address_start, memory_idx_t clean_address_end,
        bool& something_invalidated) {

    #pragma HLS INLINE

    dist_t dists[K_NEIGHBORS];
    label_t labels[K_NEIGHBORS];

    run(ref_datapoint, 
        address_start, address_end,
        dists, labels);

    // Determine farthest correct datapoint
    dist_t max_dist;
    bool clean_needed;
    farthest_correct(ref_label, labels, dists, max_dist, clean_needed);

    // If no correct datapoint, cancel cleaning
    if(!clean_needed) {
        something_invalidated = false;
        return;
    }

    // Invalidate affected datapoints
    bool cur_something_invalidated = false;

    knunit_clean_loop:
    for(memory_idx_t address = clean_address_start; address < clean_address_end; address++) {
        #pragma HLS loop_tripcount min=0 max=MEM_SIZE avg=MEM_SIZE/2
        //#pragma HLS PIPELINE

        // If label is correct, skip
        if(Memory::labels[Memory::active_buffer][address] == ref_label) {
            continue;
        }

        dist_t cur_dist;
        calc_dist(ref_datapoint, Memory::data[Memory::active_buffer][address], cur_dist);

        // If point is further away, skip
        if(cur_dist > max_dist) {
            continue;
        }

        // Invalidate Datapoint
        Memory::invalidate(address);
        cur_something_invalidated = true;
    }

    something_invalidated = cur_something_invalidated;
}
