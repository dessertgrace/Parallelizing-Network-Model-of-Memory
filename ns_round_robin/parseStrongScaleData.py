#!/usr/bin/env python
# Grace Dessert, April 7 2022
# BME 590L Dr. Randles
# parse strong scale data and create plots
# NOTE based on how the outputs are extracted, only run one simulation at a time!

import os
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt


def main():
    # strong scale cases
    numDirs = [4, 8, 12, 16, 20, 24, 36, 64, 128, 256]

    # open last X directories in out directory
    dirsInOut = os.listdir('out/')
    dirsInOut.sort()
    print(dirsInOut)
    # plot out file
    plotFile = "strongScalePlot.svg"

    # for each get time from file '0_0.raw'
    maxTimes = []
    for i in range(len(numDirs)):
	propDir = os.listdir('out/'+dirsInOut[-i-1])
        fileName = "out/" + dirsInOut[-i-1] + "/" + propDir[0] + "/0_0.raw"
	print("Opening file: ", fileName)
        with open(fileName, 'r') as file:
            for line in file:
                if line.startswith("timing"):
                    last_line = line.split(" ")
                    maxTime = float(last_line[1])
                    maxTimes.append(maxTime)
                    break
    maxTimes.reverse()

    if len(maxTimes) != len(numDirs):
        print('ERROR: number of outputs found is not equal to number of runs expected')

    # plot and save plot
    plt.figure(figsize=[9, 5])
    plt.grid(1, axis='y')
    plt.plot(numDirs, maxTimes, '-o')

    # legendEntries = ["Baseline", "HPC lesion", "ACC lesion"]
    # plt.legend(legendEntries, loc='best')

    plt.ylabel("Max Rank Runtime")
    plt.xlabel("Num Processors")
    # plt.show()
    plt.savefig(plotFile)


if __name__ == "__main__":
    main()
