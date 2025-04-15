
#include "../samknn_config.h"
#include "../samknn_types.h"
#include "calc_dist.h"
#include <hls_math.h>

void calc_dist(datapoint_t ref_datapoint[N_DATAPOINT_DIMENSIONS], datapoint_t datapoint[N_DATAPOINT_DIMENSIONS], dist_t& dist) {
	#pragma HLS INLINE
	dist_t cur_dist = 0.0;
	dist_1dim_t cur_dim_dist;

	calc_dist_loop:
	for(dimension_idx_t i = 0; i < N_DATAPOINT_DIMENSIONS; i++) {
		#pragma HLS UNROLL
		cur_dim_dist = ref_datapoint[i] - datapoint[i];
		cur_dist += cur_dim_dist*cur_dim_dist;
	}
	dist = cur_dist;
}