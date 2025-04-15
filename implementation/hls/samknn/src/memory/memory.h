#ifndef MEMORY
#define MEMORY

#include "../samknn_config.h"
#include "../samknn_types.h"

namespace Memory {

    extern memory_idx_t stm_pointer;
    extern memory_idx_t ltm_pointer;
    extern datapoint_t data[2][MEM_SIZE][N_DATAPOINT_DIMENSIONS]; 
    extern label_t labels[2][MEM_SIZE];
    extern bool invalid_flags[MEM_SIZE];
    extern memory_idx_t n_ltm_samples_per_class[N_CLASSES];
    extern bool active_buffer;

    void init();
    void append_stm(datapoint_t data[N_DATAPOINT_DIMENSIONS], label_t label);
    void invalidate(memory_idx_t address);
    void clean(memory_idx_t& n_stm_samples_cleaned);
    void stm_to_ltm(memory_idx_t stm_split_pointer);
    void cluster_down();

};


#endif