#!/bin/bash
#SBATCH --job-name=ws
#SBATCH --output=ws_%j.out
#SBATCH --nodes=4
#SBATCH --ntasks=256
#SBATCH --cpus-per-task=1
#SBATCH --time=10:00:00
#SBATCH -p normal
#SBATCH -A TG-MDE210001

./multi_ns_parallel -p b_04 -t FATAL --caseID ws -f -n 4 1
./multi_ns_parallel -p b_04 -t FATAL --caseID ws -f -n 8 1
./multi_ns_parallel -p b_12 -t FATAL --caseID ws -f -n 12 1
./multi_ns_parallel -p b_16 -t FATAL --caseID ws -f -n 16 1
./multi_ns_parallel -p b_20 -t FATAL --caseID ws -f -n 20 1
./multi_ns_parallel -p b_24 -t FATAL --caseID ws -f -n 24 1
./multi_ns_parallel -p b_36 -t FATAL --caseID ws -f -n 36 1
./multi_ns_parallel -p b_64 -t FATAL --caseID ws -f -n 64 1
./multi_ns_parallel -p b_128 -t FATAL --caseID ws -f -n 128 1
./multi_ns_parallel -p b_256 -t FATAL --caseID ws -f -n 256 1

# submit python parse output script to get timings from output files
# to open correct files, get last X directories in the out directory in order and parse total times from 0_0.raw end
python parseWeakScaleData.py
