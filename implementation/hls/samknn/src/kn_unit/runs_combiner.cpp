
#include "../samknn_config.h"
#include "../samknn_types.h"
#include "runs_combiner.h"

void runs_combiner(label_t labels[N_PARALLEL_RUNS][K_NEIGHBORS],
		dist_t dists[N_PARALLEL_RUNS][K_NEIGHBORS],
		label_t labels_out[K_NEIGHBORS],
		dist_t dists_out[K_NEIGHBORS]) {

	#pragma HLS INLINE

	neighbor_idx_t idcs[N_PARALLEL_RUNS] = {0};
    #pragma HLS array_partition variable=idcs complete dim=1

	runs_combiner_combine:
	for(neighbor_idx_t idx_out = 0; idx_out < K_NEIGHBORS; idx_out++) {
		#pragma HLS UNROLL
		dist_t closest_dist = MAX_DIST;
		parallel_runs_idx_t closest_dist_idx = 0;

		runs_combiner_combine_find_closest:
		for(parallel_runs_idx_t i = 0; i < N_PARALLEL_RUNS; i++) {
			#pragma HLS UNROLL
			if(dists[i][idcs[i]] < closest_dist) {
				closest_dist = dists[i][idcs[i]];
				closest_dist_idx = i;
			}
		}

		labels_out[idx_out] = labels[closest_dist_idx][idcs[closest_dist_idx]];
		dists_out[idx_out] = dists[closest_dist_idx][idcs[closest_dist_idx]];
		idcs[closest_dist_idx]++;
	}
}