#ifndef SAMKNN_TYPES_H
#define SAMKNN_TYPES_H

#include "ap_int.h"
#include "ap_fixed.h"
#include "samknn_config.h"


constexpr int calculate_bits(int max_value) {
    int bits = 0;
    while ((1 << bits) < max_value)
        bits++;
    return bits;
}

// Inferred Hyperparamters
constexpr int MAX_CANDIDATE_SIZES = (int) (calculate_bits(MEM_SIZE) - calculate_bits(MIN_STM_SIZE) + 1); 

// Input data types
typedef ap_uint<DATAPOINT_BITS> datapoint_t;
constexpr int LABEL_BITS = calculate_bits(N_CLASSES);
typedef ap_uint<LABEL_BITS> label_t;

// Distance types
typedef ap_uint<(DATAPOINT_BITS+1)*2+calculate_bits(N_DATAPOINT_DIMENSIONS)> dist_t;
#define MAX_DIST -1

// Index types
constexpr int NEIGHBOR_IDX_BITS = calculate_bits(K_NEIGHBORS+1);
typedef ap_uint<NEIGHBOR_IDX_BITS> neighbor_idx_t;

constexpr int MEMORY_IDX_BITS = calculate_bits(MEM_SIZE+1);
typedef ap_uint<MEMORY_IDX_BITS> memory_idx_t;

constexpr int DIMENSION_IDX_BITS = calculate_bits(N_DATAPOINT_DIMENSIONS+1);
typedef ap_uint<DIMENSION_IDX_BITS> dimension_idx_t;

constexpr int CLASS_IDX_BITS = calculate_bits(N_CLASSES+1);
typedef ap_uint<CLASS_IDX_BITS> class_idx_t;

constexpr int N_PARALLEL_RUNS_IDX_BITS = calculate_bits(N_PARALLEL_RUNS+1);
typedef ap_uint<N_PARALLEL_RUNS_IDX_BITS> parallel_runs_idx_t;


constexpr int N_CANDIDATE_SIZES_BITS = calculate_bits(MAX_CANDIDATE_SIZES+1);
typedef ap_uint<N_CANDIDATE_SIZES_BITS> candidate_size_idx_t;



// Intermediate types
typedef ap_uint<DATAPOINT_BITS*2> centroid_acc_t;
typedef ap_int<DATAPOINT_BITS+2> dist_1dim_t; // Add 1 bit for sign and 1 bit for additional length
typedef ap_ufixed<2*MEMORY_IDX_BITS,MEMORY_IDX_BITS> normalized_score_t; // I guess this is a reasonable bit length as we need to store at most this integer part an then normalize it.


#endif 