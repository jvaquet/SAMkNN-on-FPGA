
#include "../samknn_config.h"
#include "../samknn_types.h"
#include "k_best_collector.h"


void KBestCollector::append(dist_t dist, label_t label,
            dist_t dists[K_NEIGHBORS], label_t labels[K_NEIGHBORS]) {
    #pragma HLS INLINE

    neighbor_idx_t insert_idx = K_NEIGHBORS;

    k_best_collector_append_find_pos:
    for(neighbor_idx_t i = 0; i < K_NEIGHBORS; i++) {
        #pragma HLS UNROLL
        if(dist < dists[i]) {
            insert_idx = i;
            break;
        }
    }

    if(insert_idx == K_NEIGHBORS) {
        return;
    }


    k_best_collector_append_insert:
    for(neighbor_idx_t i = K_NEIGHBORS-1; i > 0; i--) {
        #pragma HLS UNROLL
        if(i > insert_idx) {
            dists[i] = dists[i-1]; 
            labels[i] = labels[i-1]; 
        }
    }

    dists[insert_idx] = dist;
    labels[insert_idx] = label;
}
