import os
import subprocess
import shutil
import numpy as np

CPP_COMPILER = 'arm-linux-gnueabihf-g++'

### Dataset preprocessing functions
def normalize(data):
    max = np.max(data)
    min = np.min(data)
    return (data-min)/(max-min)

def save_dataset(name, data, labels, norm=True):
    os.makedirs(f'../evaluation/datasets/{name}', exist_ok=True)

    if norm:
        data = normalize(data)

    np.savetxt(f'../evaluation/datasets/{name}/data', data, fmt='%.10f')
    np.savetxt(f'../evaluation/datasets/{name}/labels', labels, fmt='%i')

    np.savetxt(f'../evaluation/datasets/{name}/.SAMPLES', np.array(data.shape[0])[None], fmt='%i')
    np.savetxt(f'../evaluation/datasets/{name}/.DIMS', np.array(data.shape[1])[None], fmt='%i')
    np.savetxt(f'../evaluation/datasets/{name}/.CLASSES', np.unique(labels).shape, fmt='%i')

def parse_moa_dataset(file, dims):
    data = np.loadtxt(file, delimiter=',', comments='@', usecols=range(dims))
    labels_str = np.loadtxt(file, delimiter=',', comments='@', usecols=dims)
    labels = np.zeros(labels_str.shape)

    for i, label in enumerate(np.unique(labels_str)):
        labels[labels_str == label] = i

    return data, labels

def preprocess_datasets():
    data = np.loadtxt('../evaluation/driftDatasets/artificial/sea/SEA_training_data.csv', delimiter=',')
    labels = np.loadtxt('../evaluation/driftDatasets/artificial/sea/SEA_training_class.csv')
    save_dataset('seaConcepts', data, labels)

    data = np.loadtxt('../evaluation/driftDatasets/artificial/hyperplane/rotatingHyperplane.data')
    labels = np.loadtxt('../evaluation/driftDatasets/artificial/hyperplane/rotatingHyperplane.labels')
    save_dataset('rotatingHyperplane', data, labels)

    data = np.loadtxt('../evaluation/driftDatasets/artificial/rbf/movingRBF.data')
    labels = np.loadtxt('../evaluation/driftDatasets/artificial/rbf/movingRBF.labels')
    save_dataset('movingRBF', data, labels)

    data = np.loadtxt('../evaluation/driftDatasets/artificial/rbf/interchangingRBF.data')
    labels = np.loadtxt('../evaluation/driftDatasets/artificial/rbf/interchangingRBF.labels')
    save_dataset('interchangingRBF', data, labels)

    data = np.loadtxt('../evaluation/driftDatasets/artificial/movingSquares/movingSquares.data')
    labels = np.loadtxt('../evaluation/driftDatasets/artificial/movingSquares/movingSquares.labels')
    save_dataset('movingSquares', data, labels)

    data = np.loadtxt('../evaluation/driftDatasets/artificial/chess/transientChessboard.data')
    labels = np.loadtxt('../evaluation/driftDatasets/artificial/chess/transientChessboard.labels')
    save_dataset('transientChessboard', data, labels)

    data = np.loadtxt('../evaluation/driftDatasets/artificial/mixedDrift/mixedDrift.data')
    labels = np.loadtxt('../evaluation/driftDatasets/artificial/mixedDrift/mixedDrift.labels')
    save_dataset('mixedDrift', data, labels)

    data = np.loadtxt('../evaluation/driftDatasets/realWorld/weather/NEweather_data.csv', delimiter=',')
    labels = np.loadtxt('../evaluation/driftDatasets/realWorld/weather/NEweather_class.csv')
    save_dataset('weather', data, labels)

    data = np.loadtxt('../evaluation/driftDatasets/realWorld/Elec2/elec2_data.dat')
    labels = np.loadtxt('../evaluation/driftDatasets/realWorld/Elec2/elec2_label.dat')
    save_dataset('electricity', data, labels)

    data = np.loadtxt('../evaluation/driftDatasets/realWorld/poker/poker.data')
    labels = np.loadtxt('../evaluation/driftDatasets/realWorld/poker/poker.labels')
    save_dataset('poker', data, labels)

    data = np.loadtxt('../evaluation/driftDatasets/realWorld/outdoor/outdoorStream.data')
    labels = np.loadtxt('../evaluation/driftDatasets/realWorld/outdoor/outdoorStream.labels')
    save_dataset('outdoor', data, labels)
    

### Main setup function
def main():

    # Make working directories
    os.makedirs('../artifacts/ip_cores', exist_ok=True)
    os.makedirs('../artifacts/xsa_files', exist_ok=True)
    os.makedirs('../artifacts/image', exist_ok=True)
    os.makedirs('vivado/ip_repo', exist_ok=True)
    os.makedirs('logs', exist_ok=True)

    # Preprocess Datasets
    if os.path.exists('../evaluation/datasets'):
        print(f'Skipping Dataset Preparation... ', flush=True)
    else:
        print(f'Preparing Datasets... ', end='', flush=True)
        with open('logs/git.log', 'w') as logfile:
            # Clone drift datasets
            exit_code = subprocess.call('git clone https://github.com/vlosing/driftDatasets.git'.split(' '), stdout=logfile, stderr=subprocess.STDOUT, cwd='../evaluation')
            if exit_code != 0:
                print('Error: Failed to clone driftDatasets!', flush=True)
                return
        preprocess_datasets()
        print('Done.', flush=True)

    # Build Host code
    if os.path.exists('../evaluation/experiments/SAMkNN'):
        print('Skipped building Host Code.', flush=True)
    else:
        print(f'Building Host Code... ', end='', flush=True)
        with open('logs/host_build.log', 'w') as logfile:
            
            # Configure and build program
            exit_code = subprocess.call('cmake . -B build'.split(' '), stdout=logfile, stderr=subprocess.STDOUT, cwd='host/samknn')
            exit_code = subprocess.call(f'cmake . -B build -DCMAKE_CXX_COMPILER={CPP_COMPILER}'.split(' '), stdout=logfile, stderr=subprocess.STDOUT, cwd='host/samknn')
            exit_code = subprocess.call('cmake --build build --config Release'.split(' '), stdout=logfile, stderr=subprocess.STDOUT, cwd='host/samknn')
                
        # Copy executable
        shutil.copy('host/samknn/build/SAMkNN', '../evaluation/experiments/SAMkNN')
        print('Done.', flush=True)
    
    # Build IP Cores
    print('Building IP Cores:', flush=True)

    # ddr2stream
    if os.path.exists('vivado/ip_repo/ddr2stream'):
        print('Skipping ddr2stream.', flush=True)
    else:
        # Generate IP Core
        print(f'Generating ddr2stream... ', end='', flush=True)
        with open('logs/vitis.log', 'w') as logfile:
            exit_code = subprocess.call('vitis_hls generate_ip.tcl'.split(' '), stdout=logfile, stderr=subprocess.STDOUT, cwd='hls/ddr2stream')
        if exit_code != 0:
            print('Error: Failed to generate.')
            return

        # Save IP core
        if os.path.exists('ip_cores/ddr2stream.zip'):
            os.remove('ip_cores/ddr2stream.zip')
        if os.path.exists('ip_cores/ddr2stream'):
            shutil.rmtree('ip_cores/ddr2stream')
        shutil.move('hls/ddr2stream/ddr2stream/solution1/impl/ip/xilinx_com_hls_ddr2stream_1_0.zip', f'vivado/ip_repo/ddr2stream.zip')

        exit_code = subprocess.call('unzip -d ddr2stream ddr2stream.zip'.split(' '), stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, cwd='vivado/ip_repo')

        print('Done.', flush=True)


    # stream2ddr
    if os.path.exists('vivado/ip_repo/stream2ddr'):
        print('Skipping stream2ddr.', flush=True)
    else:
        # Generate IP Core
        print(f'Generating stream2ddr... ', end='', flush=True)
        with open('logs/vitis.log', 'w') as logfile:
            exit_code = subprocess.call('vitis_hls generate_ip.tcl'.split(' '), stdout=logfile, stderr=subprocess.STDOUT, cwd='hls/stream2ddr')
        if exit_code != 0:
            print('Error: Failed to generate.')
            return

        # Save IP core
        if os.path.exists('ip_cores/stream2ddr.zip'):
            os.remove('ip_cores/stream2ddr.zip')
        if os.path.exists('ip_cores/stream2ddr'):
            shutil.rmtree('ip_cores/stream2ddr')
        shutil.move('hls/stream2ddr/stream2ddr/solution1/impl/ip/xilinx_com_hls_stream2ddr_1_0.zip', f'vivado/ip_repo/stream2ddr.zip')

        exit_code = subprocess.call('unzip -d stream2ddr stream2ddr.zip'.split(' '), stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, cwd='vivado/ip_repo')

        print('Done.', flush=True)


    # samknn
    if os.path.exists('vivado/ip_repo/samknn'):
        print('Skipping samknn.', flush=True)
    else:
        # Generate IP Core
        print(f'Generating samknn... ', end='', flush=True)
        with open('logs/vitis.log', 'w') as logfile:
            exit_code = subprocess.call('vitis_hls generate_ip.tcl'.split(' '), stdout=logfile, stderr=subprocess.STDOUT, cwd='hls/samknn')
        if exit_code != 0:
            print('Error: Failed to generate.')
            return

        # Save IP core
        if os.path.exists('vivado/ip_repo/samknn.zip'):
            os.remove('vivado/ip_repo/samknn.zip')
        if os.path.exists('vivado/ip_repo/samknn'):
            shutil.rmtree('vivado/ip_repo/samknn')
        shutil.move('hls/samknn/samknn/solution1/impl/ip/xilinx_com_hls_samknn_top_1_0.zip', f'vivado/ip_repo/samknn.zip')

        exit_code = subprocess.call('unzip -d samknn samknn.zip'.split(' '), stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, cwd='vivado/ip_repo')

        print('Done.', flush=True)


    # Run Vivado
    print('Generating hardware... ', end='', flush=True)
    with open('logs/vivado.log', 'w') as logfile:
        exit_code = subprocess.call('vivado -mode tcl -script make_wrapper.tcl samknn.xpr'.split(' '), stdout=logfile, stderr=subprocess.STDOUT, cwd='vivado/samknn')
        exit_code = subprocess.call('vivado -mode tcl -script export_hardware.tcl samknn.xpr'.split(' '), stdout=logfile, stderr=subprocess.STDOUT, cwd='vivado/samknn')

    if exit_code != 0:
        print('Error: Could not generate Hardware.')
        return
        
    # Move platform XSA
    shutil.move('vivado/samknn/platform.xsa', f'../artifacts/xsa_files/samknn.xsa')
    print('Done.', flush=True)


    # Generate Boot Files
    print('Generating boot files... ', end='', flush=True)
    with open('logs/petalinux.log', 'w') as logfile:
        exit_code = subprocess.call(f'petalinux-config --get-hw-description=../../../artifacts/xsa_files/samknn.xsa --silentconfig'.split(' '), stdout=logfile, stderr=subprocess.STDOUT, cwd='petalinux/SAMkNN')
        exit_code = subprocess.call('petalinux-build'.split(' '), stdout=logfile, stderr=subprocess.STDOUT, cwd='petalinux/SAMkNN')
        exit_code = subprocess.call('petalinux-package --boot --fpga --u-boot --dtb --boot-script --force --fsbl'.split(' '), stdout=logfile, stderr=subprocess.STDOUT, cwd='petalinux/SAMkNN')
        exit_code = subprocess.call('petalinux-package --wic'.split(' '), stdout=logfile, stderr=subprocess.STDOUT, cwd='petalinux/SAMkNN')
    shutil.copy('petalinux/SAMkNN/images/linux/petalinux-sdimage.wic', '../artifacts/image/petalinux-sdimage.wic')
    print('Done.')

if __name__ == '__main__':
    main()