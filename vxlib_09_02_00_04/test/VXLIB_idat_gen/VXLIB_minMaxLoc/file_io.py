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


def write_attributes_scalar(f, attributes, stringToWrite):
    f.write("#ifdef WIN32\n")
    f.write(stringToWrite)
    f.write("#else\n")
    f.write(attributes + stringToWrite)
    f.write("#endif\n")

def write_header_file(in0, minVal, maxVal, minCount, maxCount, minLoc, maxLoc, testCase, headerFileName):
    testId = testCase["ID"]
    dType = testCase["dType"]
    height = testCase["height"]
    width = testCase["width"]

    # dTypeOut = "uint8_t" if dTypeIn == "int16_t" else "int16_t"

    attributesIn = '__attribute__((section(".staticData"))) static '
    attributesOut = '__attribute__((section(".staticData"))) static '

    outputDir = "../../VXLIB_minMaxLoc/test_data/"

    with open(outputDir + headerFileName, "w") as f:
        write_attributes(
            f,
            attributesIn,
            dType + " " + "staticRefInCase" + str(testId) + "[]=\n",
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
            attributesOut,
            dType + " " + "staticRefMinValCase" + str(testId) + "[]=\n",
        )
        print("{:>2}".format(minVal), file=f)
        print(" ", file=f)
        f.write("\n};\n\n")
        f.close()

    with open(outputDir + headerFileName, "a") as f:
        write_attributes(
            f,
            attributesOut,
            dType + " " + "staticRefMaxValCase" + str(testId) + "[]=\n",
        )
        print("{:>2}".format(maxVal), file=f)
        print(" ", file=f)
        f.write("\n};\n\n")
        f.close()

    with open(outputDir + headerFileName, "a") as f:
        write_attributes(
            f,
            attributesOut,
            "uint32_t" + " " + "staticRefMinCountCase" + str(testId) + "[]=\n",
        )
        print("{:>2}".format(minCount), file=f)
        print(" ", file=f)
        f.write("\n};\n\n")
        f.close()

    with open(outputDir + headerFileName, "a") as f:
        write_attributes(
            f,
            attributesOut,
            "uint32_t" + " " + "staticRefMaxCountCase" + str(testId) + "[]=\n",
        )
        print("{:>2}".format(maxCount), file=f)
        print(" ", file=f)
        f.write("\n};\n\n")
        f.close()

    with open(outputDir + headerFileName, "a") as f:
        write_attributes(
            f,
            attributesOut,
            "uint32_t" + " " + "staticRefMinLocCase" + str(testId) + "[]=\n",
        )
        for dimX in range(len(minLoc)):
            print("{:>2}".format(minLoc[dimX]), file=f, end=", ")
        print(" ", file=f)
        f.write("\n};\n\n")
        f.close()

    with open(outputDir + headerFileName, "a") as f:
        write_attributes(
            f,
            attributesOut,
            "uint32_t" + " " + "staticRefMaxLocCase" + str(testId) + "[]=\n",
        )
        for dimX in range(len(maxLoc)):
            print("{:>2}".format(maxLoc[dimX]), file=f, end=", ")
        print(" ", file=f)
        f.write("\n};\n\n")
        f.close()


    # with open(outputDir + headerFileName, "a") as f:
    #     write_attributes(
    #         f,
    #         attributesOut,
    #         dTypeOut + " " + "staticRefOutCase" + str(testId) + "[]=\n",
    #     )
    #     for dimY in range(0, height):
    #         for dimX in range(0, width):
    #             print("{:>2}".format(out[dimY, dimX]), file=f, end=", ")
    #         print(" ", file=f)
    #     f.write("\n};")
    #     f.write("\n\n")
    #     f.close()

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
