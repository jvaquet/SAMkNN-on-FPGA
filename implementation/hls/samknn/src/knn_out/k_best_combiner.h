
#include "../samknn_config.h"
#include "../samknn_types.h"

void k_best_combiner(label_t labels0[K_NEIGHBORS],
		dist_t dists0[K_NEIGHBORS],
		label_t labels1[K_NEIGHBORS],
		dist_t dists1[K_NEIGHBORS],
		label_t labels_out[K_NEIGHBORS]);