
#include "../samknn_config.h"
#include "../samknn_types.h"
#include "farthest_correct.h"

void farthest_correct(label_t ref_label, label_t labels[K_NEIGHBORS], 
		dist_t dists[K_NEIGHBORS], dist_t& max_dist, bool& clean_needed) {
	#pragma HLS INLINE

	dist_t cur_max_dist = 0;
	dist_t correct_found = false;

	farthest_correct_loop:
	for(neighbor_idx_t i = 0; i < K_NEIGHBORS; i++) {
		#pragma HLS unroll
		if(ref_label == labels[i] && dists[i] >= cur_max_dist) {
			cur_max_dist = dists[i];
			correct_found = true;
		}
	}

	max_dist = cur_max_dist;
	clean_needed = correct_found;
}
