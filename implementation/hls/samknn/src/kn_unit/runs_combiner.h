
#include "../samknn_config.h"
#include "../samknn_types.h"

void runs_combiner(label_t labels[N_PARALLEL_RUNS][K_NEIGHBORS],
		dist_t dists[N_PARALLEL_RUNS][K_NEIGHBORS],
		label_t labels_out[K_NEIGHBORS],
		dist_t dists_out[K_NEIGHBORS]);