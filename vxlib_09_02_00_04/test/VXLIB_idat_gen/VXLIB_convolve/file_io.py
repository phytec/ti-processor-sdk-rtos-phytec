from __future__ import print_function
from subprocess import call
from sys import platform

import sys
import numpy as np
import csv
import os

def write_attributes(f, attributes, stringToWrite):
    f.write("#ifdef WIN32\n")
    f.write(stringToWrite)
    f.write("#else\n")
    f.write(attributes + stringToWrite)
    f.write("#endif\n")
    f.write("{\n")


def write_header_file(inp, convFilter, out, testCase, headerFileName):
    testId = testCase["ID"]
    width = testCase["width"]
    height = testCase["height"]
    dTypeIn = testCase["dTypeIn"]
    dTypeFilter = testCase["dTypeFilter"]
    dTypeOut = testCase["dTypeOut"]
    padLeft = testCase["padLeft"]
    padRight = testCase["padRight"]
    padTop = testCase["padTop"]
    padBottom = testCase["padBottom"]
    filterHeight = testCase["filterHeight"]
    filterWidth = testCase["filterWidth"]
    scale = testCase["scale"]
    
    outHeight = height
    outWidth = width

    isNotPadded = (padLeft == 0) and (padRight == 0) and (padTop == 0) and (padBottom == 0)
    
    if (isNotPadded):
        outHeight = height - filterHeight + 1
        outWidth = width - filterWidth + 1
        
    attributesIn = '__attribute__((section(".staticData"))) static '
    attributesFilter = '__attribute__((section(".staticData"))) static '
    attributesOut = '__attribute__((section(".staticData"))) static '

    outputDir = "../../VXLIB_convolve/test_data/"

    if not os.path.exists(outputDir):
        os.mkdir(outputDir)
        
    with open(outputDir + headerFileName, "w") as f:
        write_attributes(
            f,
            attributesIn,
            dTypeIn + " " + "staticRefInCase" + str(testId) + "[]=\n",
        )
        for dimY in range(0, height):
            for dimX in range(0, width):
                print("{:>2}".format(inp[dimY, dimX]), file=f, end=", ")
            print(" ", file=f)
        f.write("\n};")
        f.write("\n\n")
        f.close()

    with open(outputDir + headerFileName, "a") as f:
        write_attributes(
            f,
            attributesFilter,
            dTypeFilter + " " + "staticRefFilterCase" + str(testId) + "[]=\n",
        )
        for dimY in range(0, filterHeight):
            for dimX in range(0, filterWidth):
                print("{:>2}".format(convFilter[dimY, dimX]), file=f, end=", ")
            print(" ", file=f)
        f.write("\n};")
        f.write("\n\n")
        f.close()

    with open(outputDir + headerFileName, "a") as f:
        write_attributes(
            f,
            attributesOut,
            dTypeOut + " " + "staticRefOutCase" + str(testId) + "[]=\n",
        )
        for dimY in range(0, outHeight):
            for dimX in range(0, outWidth):
                print("{:>2}".format(out[dimY, dimX]), file=f, end=", ")
            print(" ", file=f)
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
