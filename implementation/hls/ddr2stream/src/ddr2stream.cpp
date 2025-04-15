#include "ddr2stream.h"

void ddr2stream(uint32_t* in_data, hls::stream<uint32_t> &out_stream, const uint32_t n_elements) {
#pragma HLS INTERFACE mode=axis register_mode=both port=out_stream register

#pragma HLS INTERFACE mode=m_axi bundle=DDR port=in_data

#pragma HLS INTERFACE mode=s_axilite bundle=control port=in_data
#pragma HLS INTERFACE mode=s_axilite bundle=control port=n_elements
#pragma HLS INTERFACE mode=s_axilite bundle=control port=return

	for (uint32_t i = 0; i < n_elements; i++) {
#pragma HLS PIPELINE II=1
		out_stream << in_data[i];
	}
}
