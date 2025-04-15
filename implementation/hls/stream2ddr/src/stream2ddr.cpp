#include "stream2ddr.h"

void stream2ddr(hls::stream<uint32_t> &in_stream, uint32_t* out_data, const uint32_t n_elements) {
#pragma HLS INTERFACE mode=axis register_mode=both port=in_stream register

#pragma HLS INTERFACE mode=m_axi bundle=DDR port=out_data

#pragma HLS INTERFACE mode=s_axilite bundle=control port=out_data
#pragma HLS INTERFACE mode=s_axilite bundle=control port=n_elements
#pragma HLS INTERFACE mode=s_axilite bundle=control port=return

	for (uint32_t i = 0; i < n_elements; i++) {
#pragma HLS PIPELINE II=1
		in_stream >> out_data[i];
	}
}
