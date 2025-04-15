import os
import subprocess
import shutil

from build_bulk_config import CONFIGS

def write_config(params):
    with open('hls/samknn/src/samknn_config.h', 'w') as f:
        f.write(f'''#ifndef SAMKNN_CONFIG_H
#define SAMKNN_CONFIG_H

// Input data parameters
#define DATAPOINT_BITS {params[2]}
#define N_DATAPOINT_DIMENSIONS {params[0]}
#define N_CLASSES {params[1]}

// Model specific parameters
#define K_NEIGHBORS 5
#define MEM_SIZE 5000
#define MIN_STM_SIZE 50
#define MAX_LTM_SIZE 2000

// Optimization Parameters
#define N_PARALLEL_RUNS {params[3]}
#define MAX_ACC_APPROX {'true' if params[4] else 'false'}

#endif 
''')


def main():
    for config in CONFIGS:
        cfg_name = f'{config[0]}dim_{config[1]}cls_{config[2]}bit_{config[3]}run{"" if config[4] else "_exact"}'
        print(f'Starting {cfg_name}...')
        
        # Modify Config
        print(f'Writing Config... ', end='', flush=True)
        write_config(config)
        print('Done.')

        # Generate IP Core
        print(f'Generating IP Core... ', end='', flush=True)
        with open('logs/vitis.log', 'w') as logfile:
            exit_code = subprocess.call('vitis_hls generate_ip.tcl'.split(' '), stdout=logfile, stderr=subprocess.STDOUT, cwd='hls/samknn')
        
        if exit_code != 0:
            print('ERROR! Skipping to next.')
            continue
        
        print('Done.')

        # Save IP core
        print('Saving IP Core... ', end='', flush=True)
        shutil.move('hls/samknn/samknn/solution1/impl/ip/xilinx_com_hls_samknn_top_1_0.zip', f'../artifacts/ip_cores/{cfg_name}.zip')
        
        # Move and unzip IP Core
        if os.path.exists('vivado/ip_repo/samknn.zip'):
            os.remove('vivado/ip_repo/samknn.zip')
        if os.path.exists('vivado/ip_repo/samknn'):
            shutil.rmtree('vivado/ip_repo/samknn')
        shutil.copy(f'../artifacts/ip_cores/{cfg_name}.zip', 'vivado/ip_repo/samknn.zip')

        exit_code = subprocess.call('unzip -d samknn samknn.zip'.split(' '), stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, cwd='vivado/ip_repo')
        print('Done.')

        # Run Vivado
        print('Generating hardware... ', end='', flush=True)
        with open('logs/vivado.log', 'w') as logfile:
            exit_code = subprocess.call('vivado -mode tcl -script export_hardware.tcl samknn.xpr'.split(' '), stdout=logfile, stderr=subprocess.STDOUT, cwd='vivado/samknn')

        if exit_code != 0:
            print('ERROR! Skipping to next.')
            continue
        
        print('Done.')

        # Move platform XSA
        print('Saving XSA File... ', end='', flush=True)
        shutil.move('vivado/samknn/platform.xsa', f'../artifacts/xsa_files/{cfg_name}.xsa')
        print('Done.')
        
        # Generate Boot Files
        print('Generating boot files... ', end='', flush=True)
        with open('logs/petalinux.log', 'w') as logfile:
            exit_code = subprocess.call(f'petalinux-config --get-hw-description=../../../artifacts/xsa_files/{cfg_name}.xsa --silentconfig'.split(' '), stdout=logfile, stderr=subprocess.STDOUT, cwd='petalinux/SAMkNN')
            exit_code = subprocess.call('petalinux-build -c kernel'.split(' '), stdout=logfile, stderr=subprocess.STDOUT, cwd='petalinux/SAMkNN')
            exit_code = subprocess.call('petalinux-package --boot --fpga --u-boot --dtb --boot-script --force --fsbl'.split(' '), stdout=logfile, stderr=subprocess.STDOUT, cwd='petalinux/SAMkNN')
        print('Done.')

        # Save boot files
        print('Saving boot files... ', end='', flush=True)
        os.makedirs(f'../evaluation/configs/{cfg_name}', exist_ok=True)
        shutil.copy('petalinux/SAMkNN/images/linux/uImage', f'../evaluation/configs/{cfg_name}/uImage')
        shutil.copy('petalinux/SAMkNN/images/linux/boot.scr', f'../evaluation/configs/{cfg_name}/boot.scr')
        shutil.copy('petalinux/SAMkNN/images/linux/BOOT.BIN', f'../evaluation/configs/{cfg_name}/BOOT.BIN')
        print('Done.')

if __name__ == '__main__':
    main()