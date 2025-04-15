#include "../samknn.h"

void generate_candidate_sizes(memory_idx_t cur_ltm_size, candidate_size_idx_t& n_candidate_sizes, memory_idx_t candidate_sizes[MAX_CANDIDATE_SIZES]) {
	#pragma HLS INLINE

	memory_idx_t candidate_size = cur_ltm_size;
	candidate_size_idx_t n_candidate_sizes_count = 0;

	candidate_size_gen_loop:
	for(candidate_size_idx_t i = 0; i < MAX_CANDIDATE_SIZES; i++) {
		#pragma HLS unroll
		if(candidate_size >= MIN_STM_SIZE) {
			candidate_sizes[i] = candidate_size;
			n_candidate_sizes_count++;
		} else {
			candidate_sizes[i] = 0;
		}
		candidate_size /= 2;
	}
	n_candidate_sizes = n_candidate_sizes_count;
}
