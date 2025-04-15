#ifndef KN_UNIT_H
#define KN_UNIT_H

#include "../samknn_config.h"
#include "../samknn_types.h"
#include "k_best_collector.h"
#include "../memory/memory.h"

namespace KNUnit {
    void run(datapoint_t ref_datapoint[N_DATAPOINT_DIMENSIONS], 
        memory_idx_t address_start, memory_idx_t address_end,
        dist_t dists[K_NEIGHBORS], label_t labels[K_NEIGHBORS]);
    void clean(datapoint_t ref_datapoint[N_DATAPOINT_DIMENSIONS], label_t ref_label, 
        memory_idx_t address_start, memory_idx_t address_end,
        memory_idx_t clean_address_start, memory_idx_t clean_address_end,
        bool& something_invalidated);
    void predict_correct(datapoint_t ref_datapoint[N_DATAPOINT_DIMENSIONS], label_t ref_label,
        memory_idx_t address_start, memory_idx_t address_end,
        bool &correct);
};

#endif