#include <iostream>

#include <CLAP.hpp>
#include <IP_Cores/HLSCore.hpp>

#include <fstream>
#include <string>
#include <chrono>

// The DDR is located at 0x20000000
static constexpr uint64_t DDR_BASE_ADDR = 0x20000000;
// The size of the DDR is 512MB
static constexpr uint64_t DDR_SIZE = 0x20000000;

// The HLS core control registers are located at 0x40000000
static constexpr uint64_t BASE_ADDR_CORE_DATA_IN = 0x40000000;
static constexpr uint64_t BASE_ADDR_CORE_LABELS_IN = 0x40010000;
static constexpr uint64_t BASE_ADDR_CORE_PREDICTIONS_OUT = 0x40020000;

// The HLS core control register ofsets for the different paramters
// Get these offsets from the exported IP core (drivers/*/src/xstream2ddr_hw.h)

static constexpr uint64_t CORE_ADDR_DATA = 0x10;
static constexpr uint64_t CORE_ADDR_N_ELEMENTS = 0x1c;


int main(int argc, char* argv[])
{

	// Read cmdline args
	if (argc != 4) {
		std::cout << "Usage: SAMKNN <IN_DIR> <OUT_DIR> <N_BITS>" << std::endl;
		return -1;
	}

	std::string in_dir = argv[1];
	std::string out_dir = argv[2];
	uint32_t n_bits = std::stoi(argv[3]);



	try
	{
		// Create an XDMA object
		clap::CLAPPtr pClap = clap::CLAP::Create<clap::backends::PetaLinuxBackend>();
		// Add a DDR memory region to the XDMA
		pClap->AddMemoryRegion(clap::CLAP::MemoryType::DDR, DDR_BASE_ADDR, DDR_SIZE);

		// Create an HLS core object, whose control registers are located at HLS_TEST_CORE_BASE_ADDR
		// with the name "HLS_Test".
		clap::HLSCore hls_write_data(pClap, BASE_ADDR_CORE_DATA_IN, "HLS_Write_Data");
		clap::HLSCore hls_write_labels(pClap, BASE_ADDR_CORE_LABELS_IN, "HLS_Write_Labels");
		clap::HLSCore hls_read_predictions(pClap, BASE_ADDR_CORE_PREDICTIONS_OUT, "HLS_Read_Predictions");

		// Load and parse dataset meta
		std::ifstream in_dims(in_dir + "/.DIMS");
		std::ifstream in_samples(in_dir + "/.SAMPLES");

		uint32_t n_dims, n_samples;
		in_dims >> n_dims;
		in_samples >> n_samples;

		// Create host side buffers
		clap::CLAPBuffer<uint32_t> data_in(n_samples*n_dims, 0);
		clap::CLAPBuffer<uint32_t> labels_in(n_samples, 0);
		clap::CLAPBuffer<uint32_t> predictions_out(n_samples, 0);

		// Load Dataset itself
		std::ifstream infile_data(in_dir + "/data");
		std::ifstream infile_label(in_dir + "/labels");

    	double data;
		uint32_t label;

		for (uint32_t i = 0; i < n_samples; i++) {
			// Read input files
			for (uint32_t k = 0; k < n_dims; k++) {
				infile_data >> data;
				data_in[(i*n_dims)+k] = data * (1 << n_bits);
			}

			infile_label >> label;
			labels_in[i] = label;
		}


		// Allocate memory for the data on the devices DDR
		clap::Memory data_in_buf = pClap->AllocMemoryDDR(n_samples*n_dims, sizeof(uint32_t));
		clap::Memory labels_in_buf = pClap->AllocMemoryDDR(n_samples, sizeof(uint32_t));
		clap::Memory predictions_out_buf = pClap->AllocMemoryDDR(n_samples, sizeof(uint32_t));

		// Set the addresses of the input and output memory used in the HLS core.
		hls_write_data.SetDataAddr(CORE_ADDR_DATA, data_in_buf);
		hls_write_labels.SetDataAddr(CORE_ADDR_DATA, labels_in_buf);
		hls_read_predictions.SetDataAddr(CORE_ADDR_DATA, predictions_out_buf);

		// Set the number of elements to process, as this is a plain value, instead of a memory
		// address the actual value is passed.
		hls_write_data.SetDataAddr(CORE_ADDR_N_ELEMENTS, n_samples*n_dims);
		hls_write_labels.SetDataAddr(CORE_ADDR_N_ELEMENTS, n_samples);
		hls_read_predictions.SetDataAddr(CORE_ADDR_N_ELEMENTS, n_samples);

		// Write data to device memory
		pClap->Write(data_in_buf, data_in);
		pClap->Write(labels_in_buf, labels_in);

		// Enable interrupts to get done signal
		hls_write_data.EnableInterrupts(2);
		hls_write_labels.EnableInterrupts(2);
		hls_read_predictions.EnableInterrupts(2);

		// Start all cores
		std::chrono::steady_clock::time_point time_begin = std::chrono::steady_clock::now();
		hls_write_data.Start();
		hls_write_labels.Start();
		hls_read_predictions.Start();

		std::cout << "Started all cores..." << std::endl;

		// Wait for all cores to finish
		hls_write_data.WaitForFinish();
		hls_write_labels.WaitForFinish();
		hls_read_predictions.WaitForFinish();
		std::chrono::steady_clock::time_point time_end = std::chrono::steady_clock::now();

		std::cout << "All cores finished" << std::endl;


		// Readback the result data from the device memory.
		pClap->Read(predictions_out_buf, predictions_out);

		// Save predictions
		std::cout << "Writing Results..." << std::endl;
    	std::ofstream outfile(out_dir + "/predictions");
		for (const uint32_t& d : predictions_out) {
			outfile << d << std::dec << std::endl;
		}
		outfile << std::flush;

		// Save elapsed time
    	std::ofstream outfile_time(out_dir + "/runtime");
		outfile_time << std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_begin).count() << std::dec << std::endl;
		outfile_time << std::flush;
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return -1;
	}

	return 0;
}