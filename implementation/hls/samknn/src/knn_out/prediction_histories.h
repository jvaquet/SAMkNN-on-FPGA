#ifndef PREDICTION_HISTORIES
#define PREDICTION_HISTORIES


#include "../samknn_config.h"
#include "../samknn_types.h"

namespace PredictionHistories {
	// Histories
	extern bool hist_stm[MEM_SIZE];
	extern bool hist_ltm[MEM_SIZE];
	extern bool hist_com[MEM_SIZE];

	// Read and Write ptrs
	extern memory_idx_t write_ptr;
	extern memory_idx_t read_ptr;

	// Scores
	extern memory_idx_t score_stm;
	extern memory_idx_t score_ltm;
	extern memory_idx_t score_com;


	// Helper functions
	void push(bool correct_stm, bool correct_ltm, bool correct_com);
	void pop();
	void get_label(label_t label_stm, bool correct_stm,
		label_t label_ltm, bool correct_ltm,
		label_t label_com, bool correct_com,
		label_t& prediction);
	void shorten_history(memory_idx_t shorten_amount);

};

#endif