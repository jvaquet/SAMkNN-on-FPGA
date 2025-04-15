
#include "../samknn_config.h"
#include "../samknn_types.h"
#include "k_best_combiner.h"

void k_best_combiner(label_t labels0[K_NEIGHBORS],
		dist_t dists0[K_NEIGHBORS],
		label_t labels1[K_NEIGHBORS],
		dist_t dists1[K_NEIGHBORS],
		label_t labels_out[K_NEIGHBORS]) {

	#pragma HLS INLINE

	neighbor_idx_t idx0 = 0;
	neighbor_idx_t idx1 = 0;

	k_best_combiner_combine:
	for(neighbor_idx_t idx_out = 0; idx_out < K_NEIGHBORS; idx_out++) {
		if(dists0[idx0] < dists1[idx1]) {
			labels_out[idx_out] = labels0[idx0];
			idx0 ++;
		} else {
			labels_out[idx_out] = labels1[idx1];
			idx1 ++;
		}
	}
}
