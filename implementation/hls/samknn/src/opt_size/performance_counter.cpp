#include "../samknn.h"
#include "performance_counter.h"

void PerformanceCounter::add(bool scores[MAX_CANDIDATE_SIZES], memory_idx_t address,
            memory_idx_t candidate_scores[MAX_CANDIDATE_SIZES], memory_idx_t cutoff_addresses[MAX_CANDIDATE_SIZES]) {
    #pragma HLS INLINE
    performance_counter_add:
    for(candidate_size_idx_t i = 0; i < MAX_CANDIDATE_SIZES; i++) {
        #pragma HLS UNROLL
        if(cutoff_addresses[i] <= address) {
            candidate_scores[i] += scores[i];
        }
    }
}

void PerformanceCounter::get_optimal_size(memory_idx_t& optimal_size,
            memory_idx_t candidate_scores[MAX_CANDIDATE_SIZES], 
            candidate_size_idx_t n_candidate_sizes, memory_idx_t candidate_sizes[MAX_CANDIDATE_SIZES]) {
    #pragma HLS INLINE
    normalized_score_t best_normalized_score = 0;
    memory_idx_t best_size = 1;

    performance_counter_get_optimal_size_loop:
    for(candidate_size_idx_t i = 0; i < n_candidate_sizes; i++) {
        normalized_score_t cur_normalized_score = (normalized_score_t) candidate_scores[i] / (candidate_sizes[i]-K_NEIGHBORS);
        if(cur_normalized_score > best_normalized_score) {
            best_normalized_score = cur_normalized_score;
            best_size = candidate_sizes[i];
        }
    }
    optimal_size = best_size;
}

void PerformanceCounter::reset() {
    performance_counter_init:
    for(candidate_size_idx_t i = 0; i < MAX_CANDIDATE_SIZES; i++) {
        PerformanceCounter::start_addresses[i] = MEM_SIZE;
        PerformanceCounter::candidate_scores[i] = 0;

        #if MAX_ACC_APPROX
        PerformanceCounter::write_ptr[i] = 0;
        PerformanceCounter::read_ptr[i] = 0;
        #pragma HLS array_partition variable=PerformanceCounter::write_ptr complete
        #pragma HLS array_partition variable=PerformanceCounter::read_ptr complete
        #pragma HLS array_partition variable=PerformanceCounter::prediction_histories complete dim=1
        #endif
    }
}


#if MAX_ACC_APPROX

void PerformanceCounter::append(candidate_size_idx_t idx, bool correct) {
    #pragma HLS INLINE
    PerformanceCounter::prediction_histories[idx][PerformanceCounter::write_ptr[idx]] = correct;
    PerformanceCounter::write_ptr[idx]++;
}

void PerformanceCounter::pop(candidate_size_idx_t idx, bool &correct) {
    #pragma HLS INLINE
    correct = PerformanceCounter::prediction_histories[idx][PerformanceCounter::read_ptr[idx]];
    PerformanceCounter::read_ptr[idx]++;
}

#endif

namespace PerformanceCounter {
    memory_idx_t start_addresses[MAX_CANDIDATE_SIZES];
    memory_idx_t candidate_scores[MAX_CANDIDATE_SIZES];

    #if MAX_ACC_APPROX
    bool prediction_histories[MAX_CANDIDATE_SIZES][MEM_SIZE];
    memory_idx_t write_ptr[MAX_CANDIDATE_SIZES];
    memory_idx_t read_ptr[MAX_CANDIDATE_SIZES];
    #endif
}