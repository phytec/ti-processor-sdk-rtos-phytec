from __future__ import print_function
from subprocess import call
from sys import platform

import sys
import numpy as np
import csv
import argparse


def gen_ifdefs(idatFile, testCase):
    """Generate C++ ifdefs macros for a given test case

    Args:
        param1 (file object): Input file object to idat.c file
        param2 (dict object): Dict object with test case parameter

    """
    testId = testCase["ID"]
    demoCase = testCase["demoCase"]

    headerFileName = "staticRefCase" + str(testId) + ".h"

    if demoCase == "true":
        idatFile.write(
            "#if (defined(ALL_TEST_CASES) || (TEST_CASE == %d) || defined(DEMO_CASE))\n"
            % (int(testId))
        )
    else:
        idatFile.write(
            "#if (defined(ALL_TEST_CASES) || (TEST_CASE == %d))\n" % (int(testId))
        )

    idatFile.write('#include "test_data/%s"\n' % (headerFileName))
    idatFile.write("#endif\n\n")


def shiftAndRound(inVal, shift):
    np.right_shift(inVal, shift - 1, out=inVal, where=shift != 0)
    np.add(inVal, 1, out=inVal, where=shift != 0)
    np.right_shift(inVal, 1, out=inVal, where=shift != 0)
    return inVal


def resolveDTypeLimits(dType):

    # integer data type support
    if dType == "uint8_t" or dType == "int8_t":
        bits = 8
    elif dType == "uint16_t" or dType == "int16_t":
        bits = 16
    elif dType == "uint32_t" or dType == "int32_t":
        bits = 32
    if dType[0] == "u":
        minVal = 0
        maxVal = (2 ** bits) - 1
    else:
        minVal = -(2 ** (bits - 1))
        maxVal = 2 ** (bits - 1) - 1

    # single precision data type support
    if dType == "float":
        minVal = np.finfo(np.float).min
        maxVal = np.finfo(np.float).max
        
    return [minVal, maxVal]


def resolveNumpyDType(dType):
    if dType == "uint8_t":
        return np.uint8
    elif dType == "int8_t":
        return np.int8
    elif dType == "uint16_t":
        return np.uint16
    elif dType == "int16_t":
        return np.int16
    elif dType == "uint32_t":
        return np.uint32
    elif dType == "int32_t":
        return np.int32
    elif dType == "float":
        return np.float32


def resolveVXLIBDType(dType):
    if dType == "uint8_t":
        return "VXLIB_UINT8"
    elif dType == "int8_t":
        return "VXLIB_INT8"
    elif dType == "uint16_t":
        return "VXLIB_UINT16"
    elif dType == "int16_t":
        return "VXLIB_INT16"
    elif dType == "uint32_t":
        return "VXLIB_UINT32"
    elif dType == "int32_t":
        return "VXLIB_INT32"
    elif dType == "float":
        return "VXLIB_FLOAT32"


def getCmdLineArgs():

    parser = argparse.ArgumentParser()

    parser.add_argument(
        "-a",
        action="store_true",
        default=False,
        dest="allCases",
        help="Set to generate all cases",
    )

    myArgs = parser.parse_args()

    return myArgs
