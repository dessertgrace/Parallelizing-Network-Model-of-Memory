#!/bin/sh

#SBATCH --job-name=ws_array         # Job name
#SBATCH --nodes=1                   # Use one node
#SBATCH --ntasks=1                  # Run a single task
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=16gb          # Memory per processor
#SBATCH --time=08:00:00             # Time limit hrs:min:sec
#SBATCH --output=ws_rray_%A-%a.out  # Standard output and error log
#SBATCH --array=0-5                 # Array range
#SBATCH -p normal
#SBATCH -A TG-MDE210001

./multi_ns -p ws_0$SLURM_ARRAY_TASK_ID -t FATAL -f 1