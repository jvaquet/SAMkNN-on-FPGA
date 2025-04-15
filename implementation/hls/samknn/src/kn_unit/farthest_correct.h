
#include "../samknn_config.h"
#include "../samknn_types.h"

void farthest_correct(label_t ref_label, label_t labels[K_NEIGHBORS], 
		dist_t dists[K_NEIGHBORS], dist_t& max_dist, bool& clean_needed);