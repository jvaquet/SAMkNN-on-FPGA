#ifndef KNN_OUT
#define KNN_OUT


#include "../samknn_config.h"
#include "../samknn_types.h"

void get_output(label_t labels_stm[K_NEIGHBORS], dist_t dists_stm[K_NEIGHBORS],
	label_t labels_ltm[K_NEIGHBORS],  dist_t dists_ltm[K_NEIGHBORS],
	label_t label_reference,
	label_t& prediction);


#endif