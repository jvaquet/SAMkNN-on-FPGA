
#include "../samknn_config.h"
#include "../samknn_types.h"
#include "knn_out.h"
#include "k_best_combiner.h"
#include "../common/best_class.h"
#include "../common/correct_class.h"
#include "prediction_histories.h"


void get_output(label_t labels_stm[K_NEIGHBORS], dist_t dists_stm[K_NEIGHBORS],
		label_t labels_ltm[K_NEIGHBORS],  dist_t dists_ltm[K_NEIGHBORS],
		label_t label_reference,
		label_t& prediction) {

	label_t labels_com[K_NEIGHBORS];
	k_best_combiner(labels_stm, dists_stm,
			labels_ltm, dists_ltm,
			labels_com);

	label_t label_stm, label_ltm, label_com;
	best_class(labels_stm, label_stm);
	best_class(labels_ltm, label_ltm);
	best_class(labels_com, label_com);

	bool correct_stm, correct_ltm, correct_com;
	correct_class(label_stm, label_reference, correct_stm);
	correct_class(label_ltm, label_reference, correct_ltm);
	correct_class(label_com, label_reference, correct_com);

	PredictionHistories::get_label(label_stm, correct_stm,
			label_ltm, correct_ltm,
			label_com, correct_com,
			prediction);
}
