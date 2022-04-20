# Parallelizing a Neural Network Model of Systems Memory Consolidation and Reconsolidation

> **Authors**: Minhaj Hussain and Grace Dessert

> Helfer, Peter, and Thomas R. Shultz. “A Computational Model of Systems Memory Consolidation and Reconsolidation.” Hippocampus 30, no. 7 (July 2020): 659–77. https://doi.org/10.1002/hipo.23187.
___
The final report is in the root directory of the repository, `Final_Report.pdf`.

---
**NOTE** : All `make` commands (for serial and parallel implementations) can be run with the commandline argument normalize=1. This will compile the program with a basic synaptic input normalization mechanism turned on, which produces some more physiologically realistic behavior with larger network sizes. By default, normalize=0. All performance data was generated using executables built with normalize=0.

---
## Serial Code

The build and run instructions that came with the original implementation can be found in `README_old`. That document lays out in more detail the `props` system that is used to define parameters for specific runs, which will not be reiterated here. The steps to build and run the serial implementation on `stampede2` (that conveniently has the necessary GNU Make system, C++ compiler, Python interpreter, and GNUPlot) are as follows:

### Build
1. Clone the repository and `cd` into it.
2. Build the utility library:

```bash
cd lib
make
```
**Note**: The utility library requires the `boost` C++ library which we have included in the repo.

3. Build the main program:
```bash
cd ../ns
make
```

This will create executables `ns`, `mat` and `columns` in the `/ns` directory. `ns` is the main executable.


### Run
Simulations are run from within the `ns` directory using the `multi_ns` `Python` script. Note: you may need to make the file executable by running `chmod +x multi_ns` before being able to run the commands below. The script uses `Python`'s `subprocess` module to spawn and execute instances of the network model. For our parallel implementation we will need to modify how `ns` is invoked, however this works for serial execution. Run:

```bash
./multi_ns -p ns_01 -t FATAL -f 10
```

`-p ns_01` runs simulations as specified in the properties file(s) matching props/ns_01*.props. `-t FATAL` instructs that only fatal errors be logged (**Note**: you need to specifiy a `-t` level, with a choice of  "FLOW", "DEBUG3", "DEBUG2", "DEBUG1", "DEBUG", "INFO1", "INFO", "WARN", "ERROR", "FATAL". I'm not clear on what they all do.). `-f` specifies that the graphs generated are saved to a file (in `.svg` format) instead of displayed on the screen. Finally `10` specifies that 10 runs are executed and the results are averaged. Unless you specify a directory using the `-d` option, `multi_ns` will create an output directory using the current date and time as name in `./out/` into which all raw and post-processed data as well as graphs will be saved.

This generates plots showing the network performance on recall tasks with the whole network intact (intact_score), with the HPC region inactivated during each recall test (hpc_score), and with the ACC region inactivated during each recall test (acc_score).

To generate serial run time data with the 25x25 neuron-per-layer network, we ran:

```bash
./multi_ns -p b_01 -t FATAL -f 1
```
on a KNL compute node on Stampede2 (`-N 1 --ntasks=1`).

 ---
## Parallel Code

All the following has been tested on `Stampede2`.

### Build
As with the serial implementation:
1. Clone the repository and `cd` into it.
2. Build the utility library:

```bash
cd lib
make
```

3. Build the main program:
```bash
cd ../ns_layer_based or cd ../ns_round_robin
make
```

This will create executables `ns`, `mat` and `columns` in the `/ns_layer_based` and `/ns_round_robin` directories, respectively. `ns` is the main executable.

### Run
Simulations are run from within either the `ns_round_robin` or `ns_layer_based` directory using the `multi_ns_parallel` `Python` script. Note: you may need to make the file executable by running `chmod +x multi_ns_parallel` before being able to run the commands below. The script uses `Python`'s `os` module, specifically the `os.system()` method to execute `mpirun` commands and instantiate and run parallel instances of the network model. For example:

```bash
./multi_ns_parallel -n 4 -p b_01 -t FATAL --caseID=ss -f 1
```

will execute a single simulation of the 25x25 neuron-per-layer network as used for strong scaling runs in our report on 4 cores. **NOTE**: you may issue this command on Stampede2 directly in your shell terminal in an _interactive session_ with at least an `-n 4` or `--ntasks=4` allocation or in a `slurm` bash script submitted from a login node using `sbatch` (again with at least an `-n 4` or `--ntasks=4` allocation).

`-p b_01` runs simulations as specified in the properties file(s) matching props/ns_01*.props. `-t FATAL` instructs that only fatal errors be logged (**Note**: you need to specifiy a `-t` level, with a choice of  "FLOW", "DEBUG3", "DEBUG2", "DEBUG1", "DEBUG", "INFO1", "INFO", "WARN", "ERROR", "FATAL". I'm not clear on what they all do.). `-f` specifies that the graphs generated are saved to a file (in `.svg` format) instead of displayed on the screen. Finally `1` specifies that only 1 run is executed (values are averaged over multiple runs, i.e. if you ran with a number `>1`). `--caseId=ss` instructs `multi_ns_parallel` to create an output directory using the current date and time as name in `./out/ss/` into which all raw and post-processed data will be saved.
