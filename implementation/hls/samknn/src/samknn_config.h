#ifndef SAMKNN_CONFIG_H
#define SAMKNN_CONFIG_H

// Input data parameters
#define DATAPOINT_BITS 8
#define N_DATAPOINT_DIMENSIONS 2
#define N_CLASSES 4

// Model specific parameters
#define K_NEIGHBORS 5
#define MEM_SIZE 5000
#define MIN_STM_SIZE 50
#define MAX_LTM_SIZE 2000

// Optimization Parameters
#define N_PARALLEL_RUNS 1
#define MAX_ACC_APPROX true

#endif 
