# Parallelizing a Neural Network Model of Systems Memory Consolidation and Reconsolidation

> **Authors**: Minhaj Hussain and Grace Dessert

> Helfer, Peter, and Thomas R. Shultz. “A Computational Model of Systems Memory Consolidation and Reconsolidation.” Hippocampus 30, no. 7 (July 2020): 659–77. https://doi.org/10.1002/hipo.23187.

## Serial Code

The build and run instructions that came with the original implementation can be found in `README_old`. That document lays out in more detail the `props` system that is used to define parameters for specific runs, which will not be reiterated here. The steps to build and run the serial implementation on `stampede2` (that conveniently has the necessary GNU Make system, C++ compiler, Python interpreter, and GNUPlot) are as follows:

### Build
1. Clone the repository and `cd` into it.
2. Build the utility library:

```bash
cd lib
make
```
**Note**: The utility library require the `boost` C++ library which we have included in the repo.

3. Build the main program:
```bash
cd ../ns
make
```

This will create executables `ns`, `mat` and `columns` in the `/ns` directory. `ns` is the main executable.


### Run
Simulations are run from within the `ns` directory using the `multi_ns` `Python` script. This uses `Python`'s `subprocess` module to spawn and execute instances of the network model. For our parallel implementation we will need to modify how `ns` is invoked, however this works for serial execution. Run:

```bash
./multi_ns -p ns_01 -t FATAL -f 10
```

`-p ns_01` runs simulations as specified in the properties file(s) matching props/ns_01*.props. `-t FATAL` instructs that only fatal errors be logged (**Note**: you need to specifiy a `-t` level, with a choice of  "FLOW", "DEBUG3", "DEBUG2", "DEBUG1", "DEBUG", "INFO1", "INFO", "WARN", "ERROR", "FATAL". I'm not clear on what they all do.). `-f` specifies that the graphs generated are saved to a file (in `.svg` format) instead of displayed on the screen. Finally `10` specifies that 10 runs are executed and the results are averaged. Unless you specify a directory using the `-d` option, `multi_ns` will create an output directory using the current date and time as name in `./out/` into which all raw and post-processed data as well as graphs will be saved.