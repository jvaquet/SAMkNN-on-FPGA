#include "hls_stream.h"
#include <cstdint>

void stream2ddr(hls::stream<uint32_t> &in_stream, uint32_t* out_data, const uint32_t n_elements);
