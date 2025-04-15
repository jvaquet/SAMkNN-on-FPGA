
#include "../samknn_config.h"
#include "../samknn_types.h"
#include "prediction_histories.h"

namespace PredictionHistories {

	void push(bool correct_stm, bool correct_ltm, bool correct_com) {

		//#pragma HLS INLINE

		score_stm += correct_stm;
		score_ltm += correct_ltm;
		score_com += correct_com;

		hist_stm[write_ptr] = correct_stm;
		hist_ltm[write_ptr] = correct_ltm;
		hist_com[write_ptr] = correct_com;

		write_ptr = write_ptr+1 % MEM_SIZE;
	}

	void pop() {
		//#pragma HLS INLINE
		score_stm -= hist_stm[read_ptr];
		score_ltm -= hist_ltm[read_ptr];
		score_com -= hist_com[read_ptr];

		read_ptr = read_ptr+1 % MEM_SIZE;
	}

	void get_label(label_t label_stm, bool correct_stm,
			label_t label_ltm, bool correct_ltm,
			label_t label_com, bool correct_com,
			label_t& prediction) {
		//#pragma HLS INLINE

		// Select correct label
		if (score_ltm > score_stm && score_ltm > score_com) {
			prediction = label_ltm;
		} else if (score_stm > score_ltm && score_stm > score_com){
			prediction = label_stm;
		} else {
			prediction = label_com;
		}
		
		// Update histories
		push(correct_stm, correct_ltm, correct_com);
	}

	void shorten_history(memory_idx_t shorten_amount) {
		prediction_histories_pop:
		for(memory_idx_t i = 0; i < shorten_amount; i++) {
			#pragma HLS loop_tripcount min=MIN_STM_SIZE max=MEM_SIZE-MIN_STM_SIZE avg=MIN_STM_SIZE*2
			pop();
		}
	}

	memory_idx_t write_ptr = 0;
	memory_idx_t read_ptr = 0;
	memory_idx_t score_stm = 0;
	memory_idx_t score_ltm = 0;
	memory_idx_t score_com = 0;

	bool hist_stm[MEM_SIZE];
	bool hist_ltm[MEM_SIZE];
	bool hist_com[MEM_SIZE];


}