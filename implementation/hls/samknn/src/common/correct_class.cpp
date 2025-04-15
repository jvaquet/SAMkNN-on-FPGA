
#include "../samknn_config.h"
#include "../samknn_types.h"
#include "correct_class.h"

void correct_class(label_t label, label_t truth, bool& correct) {
	#pragma HLS INLINE
	correct = label == truth;
}
