import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import sys


def percent_error(filename, plot):
    # Read csv file and convert to numpy arrays
    rawData = pd.read_csv(filename, header=None)
    rawDataArr = pd.DataFrame(rawData).to_numpy()
    rawDataArr_rows = np.shape(rawDataArr)[0]

    # In the case where there is 1 input vector for the kernel
    if rawDataArr_rows is 3:
        # Create input array, C7x output array, and the numpy output array
        input0 = rawDataArr[0][0 : rawDataArr[0].size - 1]
        c7Output = rawDataArr[1][0 : rawDataArr[1].size - 1]
        pyOutput = rawDataArr[2][0 : rawDataArr[2].size - 1]

        # If data is not already sorted, sort in ascending order for plot
        sortIndices = np.argsort(input0)
        input0 = np.array(input0)[sortIndices]
        c7Output = np.array(c7Output)[sortIndices]
        pyOutput = np.array(pyOutput)[sortIndices]

    # In the case there is 2 input vectors for the kernel
    if rawDataArr_rows is 4:
        # Create the two input arrays, C7x output array, and the numpy output array
        input0 = rawDataArr[0][0 : rawDataArr[0].size - 1]
        input1 = rawDataArr[1][0 : rawDataArr[1].size - 1]
        c7Output = rawDataArr[2][0 : rawDataArr[2].size - 1]
        pyOutput = rawDataArr[3][0 : rawDataArr[3].size - 1]

    # Calculate percent error
    percentError = np.array(
        [((abs(i - j) / i) * 100) for i, j in zip(pyOutput, c7Output)]
    )
    averagePercentError = np.average(percentError)
    print("Average Percent Error: {}".format(averagePercentError))

    # Only plot for kernels with one input
    if plot and rawDataArr_rows is 3:
        plt.plot(input0, percentError, "bo")
        plt.title("exp() percent error")
        plt.xlabel("x")
        plt.ylabel("% error")
        plt.show()


def main():
    argumentLength = len(sys.argv)
    # Ex: python3 data_analysis ../MATHLIB_exp/test_data/outputdata1.csv 1
    filename = sys.argv[1]  # "Ex ../MATHLIB_exp/test_data/outputdata1.csv"
    plot = sys.argv[2]  # 1 to plot

    percent_error(filename, plot)


if __name__ == "__main__":
    main()
