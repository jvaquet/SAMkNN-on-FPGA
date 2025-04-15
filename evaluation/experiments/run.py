import os
import sys
import subprocess
import shutil
import datetime
import time

from experiments import EXPERIMENTS

EXPERIMENTS_PATH = '/home/petalinux/experiments'
DATASETS_PATH = '/home/petalinux/datasets'
CONFIGS_PATH = '/home/petalinux/configs'
RESULTS_PATH = '/home/petalinux/results/fpga/interleaved'

def reboot():
    time.sleep(30)
    print(f'[{datetime.datetime.now()}] Rebooting in 30 seconds...', flush=True)
    os.system('reboot')
    sys.exit(0)

def prepare_experiment(experiment_idx):
    print(f'[{datetime.datetime.now()}] Preparing experiment {experiment_idx}...', flush=True)
    # Read experiment
    dataset = EXPERIMENTS[experiment_idx][0]
    n_bits = EXPERIMENTS[experiment_idx][1]
    n_runs = EXPERIMENTS[experiment_idx][2]
    approx = EXPERIMENTS[experiment_idx][3]

    # Read dims for dataset
    dims_path = os.path.join(DATASETS_PATH, dataset, '.DIMS')
    with open(dims_path) as f:
        n_dims = int(f.read())

    # Read classes for dataset
    dims_path = os.path.join(DATASETS_PATH, dataset, '.CLASSES')
    with open(dims_path) as f:
        n_classes = int(f.read())

    # Copy config to /boot
    config_path = os.path.join(CONFIGS_PATH, f'{n_dims}dim_{n_classes}cls_{n_bits}bit_{n_runs}run{"" if approx else "_exact"}')
    shutil.copy(os.path.join(config_path, 'BOOT.BIN'), '/boot')
    shutil.copy(os.path.join(config_path, 'boot.scr'), '/boot')
    shutil.copy(os.path.join(config_path, 'uImage'), '/boot')

def run_experiment(experiment_idx):
    # Read experiment 
    dataset = EXPERIMENTS[experiment_idx][0]
    n_bits = EXPERIMENTS[experiment_idx][1]
    n_runs = EXPERIMENTS[experiment_idx][2]
    approx = EXPERIMENTS[experiment_idx][3]

    print(f'[{datetime.datetime.now()}] Running experiment:  {dataset} with {n_bits} bits, {n_runs} runs, {"approx" if approx else "exact"} ({experiment_idx}/{len(EXPERIMENTS)})...', flush=True)

    # Create results folder
    results_dir = os.path.join(RESULTS_PATH, dataset)
    os.makedirs(results_dir, exist_ok=True)
    
    # Prepare paths and args for experiment
    executable = os.path.join(EXPERIMENTS_PATH, 'SAMkNN')
    dataset_dir = os.path.join(DATASETS_PATH, dataset)
    tmp_dir = os.path.join(EXPERIMENTS_PATH, 'tmp')
    os.makedirs(tmp_dir, exist_ok=True)

    # Run experiment
    command = [executable, dataset_dir, tmp_dir, str(n_bits)]
    exit_code = subprocess.call(command, stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT, shell=False)

    print(f'[{datetime.datetime.now()}] Done running experiment. Exit code: {exit_code}', flush=True)

    shutil.move(os.path.join(tmp_dir, 'runtime'), os.path.join(results_dir, f'time_{n_bits}_{n_runs}{"" if approx else "_exact"}'))
    shutil.move(os.path.join(tmp_dir, 'predictions'), os.path.join(results_dir, f'predictions_{n_bits}_{n_runs}{"" if approx else "_exact"}'))

def main():
    print(f'[{datetime.datetime.now()}] Starting run script in 10 seconds...', flush=True)
    time.sleep(10)

    # Make sure to stop on stop signal
    stop_path = os.path.join(EXPERIMENTS_PATH, '.STOP')
    if os.path.exists(stop_path):
        print(f'[{datetime.datetime.now()}] Stop file is present. Exiting.', flush=True)
        sys.exit(0)

    # Read out, current experiment
    experiment_counter_path = os.path.join(EXPERIMENTS_PATH, '.EXPERIMENT')

    # If no experiment is active, 0th
    if not os.path.exists(experiment_counter_path):
        print(f'[{datetime.datetime.now()}] Preparing 0th experiment...', flush=True)
        with open(experiment_counter_path, 'w') as f:
            f.write('0')
        prepare_experiment(0)
        reboot()
        
    # Get active experiment
    with open(experiment_counter_path, 'r') as f:
        cur_experiment = int(f.read())

    # Run experiment
    run_experiment(cur_experiment)

    # Check, if this was last experiment
    if cur_experiment >= len(EXPERIMENTS)-1:
        print(f'[{datetime.datetime.now()}] Finished running all experiments. Stopping.', flush=True)
        with open(stop_path, 'w') as f:
            f.write('DONE')
        sys.exit(0)

    # Prepare next experiment
    prepare_experiment(cur_experiment+1)
    with open(experiment_counter_path, 'w') as f:
        f.write(str(cur_experiment+1))

    reboot()


if __name__ == '__main__':
    main()