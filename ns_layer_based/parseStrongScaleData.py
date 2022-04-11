#!/usr/bin/env python
# Grace Dessert, April 7 2022
# BME 590L Dr. Randles
# parse strong scale data and create plots
# NOTE based on how the outputs are extracted, only run one simulation at a time!

import os
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import pickle


def main():
    # strong scale cases
    numDirs = [4, 8, 12, 16, 20, 24, 36, 64, 128, 256]
    numTrials = 10

    # open last X directories in out directory
    dirsInOut = os.listdir('out/ss/')
    dirsInOut.sort()
    print(dirsInOut)
    # plot out file
    plotFile = "strongScalePlot.svg"

    # for each get times from file '$TRIALNUM_0.raw', where $TRIALNUM goes from 0 to #TRIALS-1
    maxTimes = []
    for i in range(len(numDirs)):
        propDir = os.listdir('out/ss/'+dirsInOut[-i-1])
        maxTimeAvg = []
        for k in range(numTrials):
            fileName = "out/ss/" + dirsInOut[-i-1] + "/" + propDir[0] + "/" + str(k) + "_0.raw"
            if os.path.exists(fileName):
                print("Opening file: ", fileName)
                with open(fileName, 'r') as file:
                    for line in file:
                        if line.startswith("timing"):
                            last_line = line.split(" ")
                            maxTimeAvg.append(float(last_line[1]))
                            break
            else:
                print("NOT FOUND file: ", fileName)
        maxTime = sum(maxTimeAvg)/len(maxTimeAvg)
        maxTimes.append(maxTime)
    maxTimes.reverse()

    if len(maxTimes) != len(numDirs):
        print('ERROR: number of outputs found is not equal to number of runs expected')

    with open("strongScaleData.pkl", 'wb') as f:
        pickle.dump(maxTimes, f)
        
    #with open('saved_dictionary.pkl', 'rb') as f:
    #    loaded_dict = pickle.load(f)

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
