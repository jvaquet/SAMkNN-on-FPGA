# SAM-kNN on FPGA
This repository contains an FPGA implementation of the [SAM-kNN](https://github.com/vlosing/SAMkNN) model for stream classification on a Zybo Z7-20 FPGA development board.

Further, tooling is provided to easily build, deploy and test the model with different parameters and compare it to the reference implementation.

## Setup

### Requirements
To run the experiments in this repository, the following is required
* A Zybo Z7-20 development board
* `git`
* `cmake` >= 3.22.1
* An arm cpp compiler such as `arm-linux-gnueabihf-g++` >= 11.4.0
* `vitis_hls` v2023.2
* `vivado` v2023.2
* `petalinux` v2024.1
* `python` >= 3.7 with 
    * `numpy` >= 1.21
    * `scikit-multiflow` >= 0.5.3

### Setup Script 
To setup the project, simply run the included `implementation/setup.py`. If you are using an arm cpp compiler different from `arm-linux-gnueabihf-g++` you need to change this at the top of the script. The setup script will:

1. Download and preprocess the datasets used for evaluation
2. Cross-compile the host executable for the Zybo Z7 Board
3. Use `vitis_hls` to generate all custom IP Cores
4. Use `vivado` to generate and export a hardware platform
5. Use `petalinux` to build a linux distribution for the Zybo Z7 Board

After running the script, you need to:
1. Flash the image `artifacts/image/petalinux-sdimage.wic` to the Board's SD Card
2. Run on the Zybo Z7 board (as described [here](https://github.com/fporrmann/CLAP?tab=readme-ov-file#allow-users-to-access-the-uio-devices-without-root-permissions)):
```
echo "SUBSYSTEM==\"uio\", GROUP=\"users\", MODE=\"0666\"" | sudo tee /etc/udev/rules.d/uio.rules
sudo udevadm trigger
```

## Performing Experiments
To perform experiments, you first need to generate the required hardware. Then the hardware needs to be installed on the Zybo Z7 Board and the experiment run. To do this in bulk, there are two scripts.


### Bulk building Hardware
To bulk build Hardware, first edit the `implementation/build_bulk_config.py` to configure which hardware to build:
```
CONFIGS = [
    #(N_DATAPOINT_DIMENSIONS, N_CLASSES, DATAPOINT_BITS, N_PARALLEL_RUNS, MAX_ACC_APPROX)
    (2, 4, 8, 2, True),
    ...
]
```
Each entry in the list defines one hardware configuration:
* `N_DATAPOINT_DIMENSIONS` defines the dimensionality of the dataset 
* `N_CLASSES` defines the number of classes for the dataset 
* `DATAPOINT_BITS` defines the bit length of each datapoint, only fixed point arithmetic is used 
* `N_PARALLEL_RUNS` defines how much the main kNN unit is parallelized, there is a tradeoff between speedup, computation overhead and area usage 
* `MAX_ACC_APPROX` use faster (but inexact) approximation to determine best STM size 

Once configured, you can simply build all the hardware, by executing the `implementation/build_bulk.py` script. The resulting configuration files are saved under `evaluation/configs`.

There are further configuration options for this SAMkNN implementation, namely: `K_NEIGHBORS`, `MEM_SIZE`, `MIN_STM_SIZE` and `MAX_LTM_SIZE`. For our experiment, however we kept these constant in order to stay comparable to the original SAM-kNN paper. The implementation however can easily be changed in the `samknn_config.h` header file.


### Bulk running Experiments
To bulk run experiments, first copy all contents of the `evaluation` directory to the Zybo Z7 Board's home.

Configure the experiments to run in the `experiments/experiments.py` file, as shown:
```
EXPERIMETNS = [
    #(DATASET, DATAPOINT_BITS, N_PARALLEL_RUNS, MAX_ACC_APPROX)
    ('electricity', 8, 1, True),
    ...
]
```
* `DATASET` defines which dataset to use
* `DATAPOINT_BITS`, `N_PARALLEL_RUNS` and `MAX_ACC_APPROX` define the hardware config to be used. (Dataset dimensions and number of classes are loaded from the dataset itself)

As changing hardware configs require the board to restart, add `experiments/run.py` script to the board's autostart as follows:
TODO

To start the experiment, just reboot the board. While running the experiments, the board will reboot several times until all experiments are done. The experiment results are stored in the `results` folder.

To start a new batch of experiments, edit `experiments/experiments.py` accordingly. Then delete 
* `experiments/.STOP` 
* `experiments/.EXPERIMENT_TODO` 

files and reboot the board.


## Reference implementation testing
Results for the reference implementation in `skmultiflow` can be generated using the `evaluation/reference.py` script. In the top, change the device name of the device the reference is taken on. There you can also toggle on power usage recording for battery powered devices.

Once configured, simply run the script and the results are stored in `evaluation/results`.