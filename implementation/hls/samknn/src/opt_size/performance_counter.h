#ifndef PERFORMANCE_COUNTER
#define PERFORMANCE_COUNTER

#include "../samknn.h"

namespace PerformanceCounter {
    void add(bool scores[MAX_CANDIDATE_SIZES], memory_idx_t address,
            memory_idx_t candidate_scores[MAX_CANDIDATE_SIZES], memory_idx_t cutoff_addresses[MAX_CANDIDATE_SIZES]);
    void get_optimal_size(memory_idx_t& optimal_size,
            memory_idx_t candidate_scores[MAX_CANDIDATE_SIZES], 
            candidate_size_idx_t n_candidate_sizes, memory_idx_t candidate_sizes[MAX_CANDIDATE_SIZES]);
    void reset();

    #if MAX_ACC_APPROX
    void append(candidate_size_idx_t idx, bool correct);
    void pop(candidate_size_idx_t idx, bool &correct);
    #endif


    extern memory_idx_t start_addresses[MAX_CANDIDATE_SIZES];
    extern memory_idx_t candidate_scores[MAX_CANDIDATE_SIZES];

    #if MAX_ACC_APPROX
    extern bool prediction_histories[MAX_CANDIDATE_SIZES][MEM_SIZE];
    extern memory_idx_t write_ptr[MAX_CANDIDATE_SIZES];
    extern memory_idx_t read_ptr[MAX_CANDIDATE_SIZES];
    #endif
};

#endif