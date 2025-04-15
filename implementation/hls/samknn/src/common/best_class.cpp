
#include "../samknn_config.h"
#include "../samknn_types.h"
#include "best_class.h"

void best_class(label_t labels[K_NEIGHBORS], label_t& label) {

	#pragma HLS INLINE

	neighbor_idx_t hist[N_CLASSES];

	// Init doubles estimated LUTs?!
	best_class_hist_init:
	for(class_idx_t i = 0; i < N_CLASSES; i++) {
		#pragma HLS UNROLL
		hist[i] = 0;
	}


	neighbor_idx_t max_count = 0;
	label_t max_label = 0;

	best_class_hist_populate:
	for(neighbor_idx_t i = 0; i < K_NEIGHBORS; i++) {
		#pragma HLS UNROLL
		hist[labels[i]]++;
		if(hist[labels[i]] > max_count) {
			max_count = hist[labels[i]];
			max_label = labels[i];
		}
	}

    label = max_label;
}
