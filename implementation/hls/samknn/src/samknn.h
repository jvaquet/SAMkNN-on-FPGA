#ifndef SAMKNN_H
#define SAMKNN_H

#include "samknn_config.h"
#include "samknn_types.h"
#include "./kn_unit/kn_unit.h"
#include "./memory/memory.h"
#include "./knn_out/knn_out.h"
#include "hls_stream.h"
#include <cstdint>

void samknn_top(hls::stream<uint32_t> &data, hls::stream<uint32_t> &labels, hls::stream<uint32_t> &predictions);

void predict_train(datapoint_t datapoint[N_DATAPOINT_DIMENSIONS], label_t ref_label, label_t& label);

namespace Test {
    extern int cur_iteration;
}

#endif 
