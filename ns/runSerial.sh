#!/bin/bash
#SBATCH --job-name=serial
#SBATCH --output=serial_%j.out
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --time=4:00:00
#SBATCH -p normal
#SBATCH -A TG-MDE210001

./multi_ns -p b_01 -t FATAL -f 1
