#!/bin/bash
#SBATCH --job-name=ss
#SBATCH --output=ss_%j.out
#SBATCH --nodes=16
#SBATCH --ntasks=1088
#SBATCH --cpus-per-task=1
#SBATCH --time=4:00:00
#SBATCH -p development
#SBATCH -A 

./multi_ns_parallel -p ns_01 -t FATAL -f -n 4 1
./multi_ns_parallel -p ns_01 -t FATAL -f -n 8 1
#./multi_ns_parallel -p ns_01 -t FATAL -f -n 8 1
#./multi_ns_parallel -p ns_01 -t FATAL -f -n 12 1
#./multi_ns_parallel -p ns_01 -t FATAL -f -n 16 1

# submit python parse output script to get timings from output files
# to open correct files, get last X directories in the out directory in order and parse total times from 0_0.raw end
./parseStrongScaleData
