import skmultiflow
import numpy as np
import os
import time
import threading

DEVICE = 'my_device'
RECORD_POWER = False

def record_power():
    while not record_power_stop.is_set():
        time.sleep(0.1)
        try:
            with open('/sys/class/power_supply/BAT1/current_now', 'r') as c:
                with open('/sys/class/power_supply/BAT1/voltage_now', 'r') as v:
                    current_muA = int(c.read())
                    voltage_muV = int(v.read())
                    power_W = current_muA * voltage_muV / 1e12
                    record_power_results.append(power_W)
        except Exception as e:
            print(f"Error reading power supply data: {e}")

def start_power_recording():
    global recording_thread
    global record_power_results
    global record_power_stop

    record_power_stop = threading.Event()
    record_power_stop.clear()
    record_power_results = []
    recording_thread = threading.Thread(target=record_power)
    recording_thread.start()

def stop_power_recording():
    record_power_stop.set()
    recording_thread.join()
    return record_power_results

def run_dataset(dataset, maxAcc=False, weighted=False):
    # Load Dataset into stream
    data = np.loadtxt(f'datasets/{dataset}/data')
    labels = np.loadtxt(f'datasets/{dataset}/labels')
    stream = skmultiflow.data.DataStream(data, labels)

    # Initialize model and evaluator
    stm_size_option = 'maxACC' if maxAcc else 'maxACCApprox'
    weighting = 'distance' if weighted else 'uniform'
    samknn = skmultiflow.lazy.SAMKNNClassifier(stm_size_option=stm_size_option, weighting=weighting)
    evaluator = skmultiflow.evaluation.EvaluatePrequential(pretrain_size=0, max_samples=1e9, batch_size=1,
                                n_wait=1e9, output_file=None, metrics=['accuracy', 'running_time'], restart_stream=False)

    # Run evaluation
    if RECORD_POWER:
        start_power_recording()
    evaluator.evaluate(stream=stream, model=samknn)
    if RECORD_POWER:
        powers = stop_power_recording()

    # Read accuracy and runtime from evaluator
    accuracy = evaluator.get_mean_measurements()[0].accuracy_score()
    runtime = int(evaluator.running_time_measurements[0]._total_time * 1000)

    # Write back results
    os.makedirs(f'results/reference/{DEVICE}/{dataset}', exist_ok=True)

    suffix = '_exact' if maxAcc else ''

    np.savetxt(f'results/reference/{DEVICE}/{dataset}/time{suffix}', np.array(runtime)[None], fmt='%i')
    np.savetxt(f'results/reference/{DEVICE}/{dataset}/acc{suffix}', np.array(accuracy)[None], fmt='%.5f')

    if RECORD_POWER:
        # Automatically report power
        np.savetxt(f'results/reference/{DEVICE}/{dataset}/pwr{suffix}', [np.mean(powers), np.max(powers)], fmt='%.5f')

def main():
    datasets = os.listdir('datasets')
    for dataset in datasets:
        print(f'Evaluating reference implementation for {dataset}...')
        run_dataset(dataset)
        print(f'Evaluating reference implementation for {dataset} with maxACC...')
        run_dataset(dataset, maxAcc=True)
        print(f'Evaluating reference implementation for {dataset}, weighted...')
        run_dataset(dataset, weighted=True)
        print(f'Evaluating reference implementation for {dataset}, weighted with maxACC...')
        run_dataset(dataset, weighted=True, maxAcc=True)

if __name__ == '__main__':
    main()
