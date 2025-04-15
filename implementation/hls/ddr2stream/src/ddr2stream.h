#include "hls_stream.h"
#include <cstdint>

void ddr2stream(uint32_t* in_data, hls::stream<uint32_t> &out_stream, const uint32_t n_elements);
