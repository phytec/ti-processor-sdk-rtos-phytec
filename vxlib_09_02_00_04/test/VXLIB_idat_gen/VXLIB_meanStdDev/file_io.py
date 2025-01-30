from __future__ import print_function
from subprocess import call
from sys import platform

import sys
import numpy as np
import csv


def write_attributes(f, attributes, stringToWrite):
    f.write("#ifdef WIN32\n")
    f.write(stringToWrite)
    f.write("#else\n")
    f.write(attributes + stringToWrite)
    f.write("#endif\n")
    f.write("{\n")


def write_header_file(in0, pixelsProcessedIn, currentSumIn, currentSqSumIn, out0, out1, pixelsProcessedOut, currentSumOut, currentSqSumOut, testCase, headerFileName):
    testId = testCase["ID"]
    width = testCase["width"]
    height = testCase["height"]
    dTypeIn = testCase["dTypeIn"]
    dTypeOut = testCase["dTypeOut"]

    if dTypeIn == "uint8_t":
        dTypeAcc = "uint32_t"

    else:
        dTypeAcc = "uint64_t"
        
    attributesIn = '__attribute__((section(".staticData"))) static '
    attributesOut = '__attribute__((section(".staticData"))) static '

    outputDir = "../../VXLIB_meanStdDev/test_data/"

    with open(outputDir + headerFileName, "w") as f:
        write_attributes(
            f,
            attributesIn,
            dTypeIn + " " + "staticRefInCase" + str(testId) + "[]=\n",
        )
        for dimY in range(0, height):
            for dimX in range(0, width):
                print("{:>2}".format(in0[dimY, dimX]), file=f, end=", ")
            print(" ", file=f)
        f.write("\n};")
        f.write("\n\n")
        f.close()

    with open(outputDir + headerFileName, "a") as f:
        write_attributes(
            f,
            attributesIn,
            "uint32_t" + " " + "staticRefPixelsProcessedInCase" + str(testId) + "[]=\n",
        )
        print("{:>2}".format(pixelsProcessedIn), file=f, end=", ")
        f.write("\n};")
        f.write("\n\n")
        f.close()

    with open(outputDir + headerFileName, "a") as f:
        write_attributes(
            f,
            attributesIn,
            dTypeAcc + " " + "staticRefCurrentSumInCase" + str(testId) + "[]=\n",
        )
        print("{:>2}".format(currentSumIn), file=f, end=", ")
        f.write("\n};")
        f.write("\n\n")
        f.close()

    with open(outputDir + headerFileName, "a") as f:
        write_attributes(
            f,
            attributesIn,
            dTypeAcc + " " + "staticRefCurrentSqSumInCase" + str(testId) + "[]=\n",)
        print("{:>2}".format(currentSqSumIn), file=f, end=", ")
        f.write("\n};")
        f.write("\n\n")
        f.close()

    with open(outputDir + headerFileName, "a") as f:
        write_attributes(
            f,
            attributesOut,
            "float" + " " + "staticRefOut0Case" + str(testId) + "[]=\n",
        )
        print("{:>2}".format(out0), file=f, end=", ")
        f.write("\n};")
        f.write("\n\n")
        f.close()

    with open(outputDir + headerFileName, "a") as f:
        write_attributes(
            f,
            attributesOut,
            "float" + " " + "staticRefOut1Case" + str(testId) + "[]=\n",
        )
        print("{:>2}".format(out1), file=f, end=", ")
        f.write("\n};")
        f.write("\n\n")
        f.close()

    with open(outputDir + headerFileName, "a") as f:
        write_attributes(
            f,
            attributesIn,
            "uint32_t" + " " + "staticRefPixelsProcessedOutCase" + str(testId) + "[]=\n",
        )
        print("{:>2}".format(pixelsProcessedOut), file=f, end=", ")
        f.write("\n};")
        f.write("\n\n")
        f.close()

    with open(outputDir + headerFileName, "a") as f:
        write_attributes(
            f,
            attributesIn,
            dTypeAcc + " " + "staticRefCurrentSumOutCase" + str(testId) + "[]=\n",
        )
        print("{:>2}".format(currentSumOut), file=f, end=", ")
        f.write("\n};")
        f.write("\n\n")
        f.close()

    with open(outputDir + headerFileName, "a") as f:
        write_attributes(
            f,
            attributesIn,
            dTypeAcc + " " + "staticRefCurrentSqSumOutCase" + str(testId) + "[]=\n",)
        print("{:>2}".format(currentSqSumOut), file=f, end=", ")
        f.write("\n};")
        f.write("\n\n")
        f.close()

    if platform == "linux" or platform == "linux2":
        call(
            [
                "indent",
                "-nut",
                "-i3",
                # "-c55",
                "-l100",
                outputDir + headerFileName,
            ]
        )
        call(["rm", outputDir + headerFileName + "~"])  # remove backup file
