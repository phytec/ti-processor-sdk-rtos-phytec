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


def write_header_file(inp, out, testCase, headerFileName):
    testId = testCase["ID"]
    widthIn = testCase["widthIn"]
    heightIn = testCase["heightIn"]
    widthOut = testCase["widthOut"]
    heightOut = testCase["heightOut"]
    dTypeIn = testCase["dTypeIn"]
    dTypeOut = testCase["dTypeOut"]
    padLeft = testCase["padLeft"]
    padRight = testCase["padRight"]
    padTop = testCase["padTop"]
    padBottom = testCase["padBottom"]

    filterDim = 5

    outHeight = min(heightOut, (heightIn - filterDim - 1))
    outWidth = min(widthOut, (widthIn - filterDim - 1))

    attributesIn = '__attribute__((section(".staticData"))) static '
    attributesOut = '__attribute__((section(".staticData"))) static '

    outputDir = "../../VXLIB_halfScaleGaussian/test_data/"

    with open(outputDir + headerFileName, "w") as f:
        write_attributes(
            f,
            attributesIn,
            dTypeIn + " " + "staticRefInCase" + str(testId) + "[]=\n",
        )
        for dimY in range(0, heightIn):
            for dimX in range(0, widthIn):
                print("{:>2}".format(inp[dimY, dimX]), file=f, end=", ")
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
