#include "../src/samknn.h"
#include "../src/samknn_config.h"
#include "../src/samknn_types.h"
#include "hls_stream.h"
#include <fstream>
#include <cstdint>

int e2e_test_1000() {
    printf("E2E Test 1000 started...\n");

    // Define input files
    std::ifstream infile_data("/home/jonas/uni/study/ma/repar/vitis/test/dataset/movingSquares.data");
    std::ifstream infile_label("/home/jonas/uni/study/ma/repar/vitis/test/dataset/movingSquares.labels");

    // Define variables
    double x, y;
    datapoint_t datapoint[2];
    label_t label, prediction;

    hls::stream<uint32_t> data;
    hls::stream<uint32_t> labels;
    hls::stream<uint32_t> predictions;

    for (int i = 0; i < 1000; i++) {
        // Read input files
        infile_data >> x >> y;
        data.write(x * (1 << DATAPOINT_BITS));
        data.write(y * (1 << DATAPOINT_BITS));
        infile_label >> label;
        labels.write(label);
    }


    // Use model
    samknn_top(data, labels, predictions);


    int score = 0;
    std::ifstream infile_label2("/home/jonas/uni/study/ma/repar/vitis/test/dataset/movingSquares.labels");
    for (int i = 0; i < 1000; i++) {
        // Read input files
        infile_label2 >> label;
        prediction = predictions.read();
        if(label == prediction) score++;
    }

    printf("Simulation complete. Score: %i/1000\n", score);
    return score < 900;
}
