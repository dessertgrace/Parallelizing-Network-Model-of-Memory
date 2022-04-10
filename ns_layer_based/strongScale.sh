#!/bin/bash
#SBATCH --job-name=ss
#SBATCH --output=ss_%j.out
#SBATCH --nodes=4
#SBATCH --ntasks=256
#SBATCH --cpus-per-task=1
#SBATCH --time=2:00:00
#SBATCH -p development
#SBATCH -A TG-MDE210001

./multi_ns_parallel -p b_01 -t FATAL -f -n 4 10
./multi_ns_parallel -p b_01 -t FATAL -f -n 8 10
./multi_ns_parallel -p b_01 -t FATAL -f -n 12 10
./multi_ns_parallel -p b_01 -t FATAL -f -n 16 10
./multi_ns_parallel -p b_01 -t FATAL -f -n 20 10
./multi_ns_parallel -p b_01 -t FATAL -f -n 24 10
./multi_ns_parallel -p b_01 -t FATAL -f -n 36 10
./multi_ns_parallel -p b_01 -t FATAL -f -n 64 10
./multi_ns_parallel -p b_01 -t FATAL -f -n 128 10
./multi_ns_parallel -p b_01 -t FATAL -f -n 256 10

# submit python parse output script to get timings from output files
# to open correct files, get last X directories in the out directory in order and parse total times from 0_0.raw end
python parseStrongScaleData.py
