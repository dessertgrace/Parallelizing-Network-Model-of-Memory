#!/usr/bin/env python

import pickle
import matplotlib.pyplot as plt
import numpy as np


def strongScalePlots():
    # STRONG SCALING

    # open pickled data
    with open('ns_round_robin/strongScaleData.pkl', 'rb') as f:
        roundRobinSS = pickle.load(f)
    with open('ns_layer_based/strongScalePlot1.pkl', 'rb') as f:
        layerBasedSS = pickle.load(f)

    numDirs = [4, 8, 12, 16, 20, 24, 36, 64, 128, 256]

    # plot and save plot
    plt.figure(figsize=[7, 4])
    # plt.grid(1, axis='y')
    plt.grid(True)
    plt.yscale('log')
    # plt.xscale('log')
    plt.plot(numDirs, roundRobinSS, '-o')
    plt.plot(numDirs, layerBasedSS, '-o')
    xTicks = [4, 16, 36, 64, 128, 256]
    plt.xticks(xTicks, xTicks)
    yTicks = [10,25,50,100,200,400,800,1600]
    # yTicks = [10, 200, 400, 1000, 1600]
    plt.yticks(yTicks, yTicks)  # np.arange(1, round(max(layerBasedSS)/10)*10, round(max(layerBasedSS)/10)*10/10)
    legendEntries = ["Round Robin", "Layer Based"]  # ', "ACC lesion"]
    plt.legend(legendEntries, loc='best')
    plt.ylabel("Max Rank Runtime (secs)")
    plt.xlabel("Num Processors")
    plt.title("Strong Scaling of 25x25 Network Simulation")
    # plt.show()
    plt.savefig("strongScalingCombined_logY.svg")
    plt.savefig("strongScalingCombined_logY.jpeg")

    plt.figure(figsize=[7, 4])
    # plt.grid(1, axis='y')
    plt.grid(True)
    plt.yscale('linear')
    plt.xscale('linear')
    plt.plot(numDirs, roundRobinSS, '-o')
    plt.plot(numDirs, layerBasedSS, '-o')
    xTicks = [4, 16, 36, 64, 128, 256]
    plt.xticks(xTicks, xTicks)
    # yTicks = [10, 25, 50, 100, 200, 400, 800, 1600]
    yTicks = [10, 200, 400, 1000, 1600]
    plt.yticks(yTicks, yTicks)  # np.arange(1, round(max(layerBasedSS)/10)*10, round(max(layerBasedSS)/10)*10/10)
    legendEntries = ["Round Robin", "Layer Based"]  # ', "ACC lesion"]
    plt.legend(legendEntries, loc='best')
    plt.ylabel("Max Rank Runtime (secs)")
    plt.xlabel("Num Processors")
    plt.title("Strong Scaling of 25x25 Network Simulation")
    # plt.show()
    plt.savefig("strongScalingCombined.svg")
    plt.savefig("strongScalingCombined.jpeg")

    plt.figure(figsize=[7, 4])
    # plt.grid(1, axis='y')
    plt.grid(True)
    plt.yscale('log')
    plt.xscale('log')
    plt.plot(numDirs, roundRobinSS, '-o')
    plt.plot(numDirs, layerBasedSS, '-o')
    # xTicks = [4, 16, 36, 64, 128, 256]
    plt.xticks(numDirs, numDirs)
    yTicks = [10, 25, 50, 100, 200, 400, 800, 1600]
    # yTicks = [10, 200, 400, 1000, 1600]
    plt.yticks(yTicks, yTicks)  # np.arange(1, round(max(layerBasedSS)/10)*10, round(max(layerBasedSS)/10)*10/10)
    legendEntries = ["Round Robin", "Layer Based"]  # ', "ACC lesion"]
    plt.legend(legendEntries, loc='best')
    plt.ylabel("Max Rank Runtime (secs)")
    plt.xlabel("Num Processors")
    plt.title("Strong Scaling of 25x25 Network Simulation")
    # plt.show()
    plt.savefig("strongScalingCombined_logLog.svg")
    plt.savefig("strongScalingCombined_logLog.jpeg")

    # with open('strongScaleData.pkl', 'wb') as f:
    #     pickle.dump(maxTimes, f)


def weakScalePlots():
    # open pickled data
    with open('ns_round_robin/weakScalePlot1.pkl', 'rb') as f:
        roundRobinSS = pickle.load(f)
    with open('ns_layer_based/weakScalePlot1.pkl', 'rb') as f:
        layerBasedSS = pickle.load(f)

    numDirs = [4, 8, 12, 16, 20, 24, 36, 64, 128, 256]
    # plot and save plot
    plt.figure(figsize=[7, 4])
    # plt.grid(1, axis='y')
    plt.grid(True)
    plt.yscale('log')
    # plt.xscale('log')
    plt.plot(numDirs, roundRobinSS, '-o')
    plt.plot(numDirs, layerBasedSS, '-o')
    xTicks = [4, 16, 36, 64, 128, 256]
    plt.xticks(xTicks, xTicks)
    yTicks = [1,10,25,50,100,200,400]
    # yTicks = [10, 200, 400, 1000, 1600]
    plt.yticks(yTicks, yTicks)  # np.arange(1, round(max(layerBasedSS)/10)*10, round(max(layerBasedSS)/10)*10/10)
    legendEntries = ["Round Robin", "Layer Based"]  # ', "ACC lesion"]
    plt.legend(legendEntries, loc='best')
    plt.ylabel("Max Rank Runtime (secs)")
    plt.xlabel("Num Processors (with linear scaled problem size)")
    plt.title("Linear Weak Scaling")
    # plt.show()
    plt.savefig("weakScalingCombined_logY.svg")
    plt.savefig("weakScalingCombined_logY.jpeg")

    plt.figure(figsize=[7, 4])
    # plt.grid(1, axis='y')
    plt.grid(True)
    plt.yscale('linear')
    plt.xscale('linear')
    plt.plot(numDirs, roundRobinSS, '-o')
    plt.plot(numDirs, layerBasedSS, '-o')
    xTicks = [4, 16, 36, 64, 128, 256]
    plt.xticks(xTicks, xTicks)
    yTicks = [1,50,100,200,210]
    # yTicks = [10, 200, 400, 1000, 1600]
    plt.yticks(yTicks, yTicks)  # np.arange(1, round(max(layerBasedSS)/10)*10, round(max(layerBasedSS)/10)*10/10)
    legendEntries = ["Round Robin", "Layer Based"]  # ', "ACC lesion"]
    plt.legend(legendEntries, loc='best')
    plt.ylabel("Max Rank Runtime (secs)")
    plt.xlabel("Num Processors (with linear scaled problem size)")
    plt.title("Linear Weak Scaling")
    # plt.show()
    plt.savefig("weakScalingCombined.svg")
    plt.savefig("weakScalingCombined.jpeg")

    plt.figure(figsize=[7, 4])
    # plt.grid(1, axis='y')
    plt.grid(True)
    plt.yscale('log')
    plt.xscale('log')
    plt.plot(numDirs, roundRobinSS, '-o')
    plt.plot(numDirs, layerBasedSS, '-o')
    xTicks = [4, 8, 12, 16, 20, 24, 36, 64, 128, 256]
    plt.xticks(xTicks, xTicks)
    # yTicks = [10, 25, 50, 100, 200, 400, 800, 1600]
    yTicks = [0.5, 1, 5, 10, 25, 50, 100, 200, 400]
    plt.yticks(yTicks, yTicks)  # np.arange(1, round(max(layerBasedSS)/10)*10, round(max(layerBasedSS)/10)*10/10)
    legendEntries = ["Round Robin", "Layer Based"]  # ', "ACC lesion"]
    plt.legend(legendEntries, loc='best')
    plt.ylabel("Max Rank Runtime (secs)")
    plt.xlabel("Num Processors (with linear scaled problem size)")
    plt.title("Linear Weak Scaling")
    # plt.show()
    plt.savefig("weakScalingCombined_logLog.svg")
    plt.savefig("weakScalingCombined_logLog.jpeg")


if __name__ == "__main__":
    # strongScalePlots()
    weakScalePlots()
