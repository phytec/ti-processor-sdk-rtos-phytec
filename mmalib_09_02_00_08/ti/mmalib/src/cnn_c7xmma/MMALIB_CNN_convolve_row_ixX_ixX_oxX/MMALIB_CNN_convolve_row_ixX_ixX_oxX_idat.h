/******************************************************************************
*                                                                             *
* module name       :MMALIB                                                   *
*                                                                             *
* module descripton :Matrix Multiply Accelerator Library module for C7x+MMA   *
*                                                                             *
* Copyright (C) 2017-2018 Texas Instruments Incorporated - http://www.ti.com/ *
* ALL RIGHTS RESERVED                                                         *
*                                                                             *
******************************************************************************/

/**
******************************************************************************
*  @file     MMALIB_CNN_convolve_row_ixX_ixX_oxX_idat.h
*
*  @brief    File to have static test data related structure
*
*  @version  0.1 - Jan 2018 : Initial Version [Asheesh Bhardwaj]
*
*******************************************************************************
*/

#ifndef MMALIB_CNN_CONVOLVE_ROW_IXX_IXX_OXX_IDAT_H_
#define MMALIB_CNN_CONVOLVE_ROW_IXX_IXX_OXX_IDAT_H_

#include <mmalib.h>

// include test infrastructure provided by MMALIB
#include "../../test/MMALIB_test.h"

typedef struct {
    uint8_t testPattern;   /* 0: constant, 1: sequential, 2: random, 3: static array, 4: file, etc */
    void  *staticIn;           // static feature map buffer
    void  *staticKernel;       // static kernel buffer
    void  *staticOut;          // static output buffer
    int32_t kernelWidth;
    int32_t kernelHeight;
    int32_t strideWidth;
    int32_t strideHeight;          // stride height should be same as stride width
    int32_t dilationHeight;        // dilation height should be same as dilation width
    int32_t dilationWidth;
    int32_t inWidth;               // width of input feature map
    int32_t pad;                   // pad value on one side of feature map (kernel width - 1)/2
    int32_t maxHeight;             // maximum height of the feature map
    int32_t inChOffset;            // size of buffer for input feature map in terms of elements. The buffer can be circular or linear
    int32_t validColsIn;           // number of pixels of input feature map including pad for one kernel call
    int32_t kDim;                  // number of input channels * filter width * filter height + bias
    int32_t pitchA;                // pitch of filter coefficient buffer per output channel
    int32_t numOfOutputChKerBuf;   // number of output rows in the coefficient buffer. This is same as subMChannels
    int32_t pitchC;                // total pitch for output pixels for a feature map
    int32_t subMChannels;          // number of output channels for a given kernel call
    int32_t numInChannels;         // total number of input channels
    int32_t numOutChannels;        // total number of output channels
    uint8_t qShift;                // shift factor for converting to output data types of 8 or 16 bit
    int32_t biasB;                 // constant Bias value for a given layer, other part of bias is part of coefficient buffer
    int32_t expectedStatusCode;    // input test status code
    int8_t  activationType;        // ReLU or Saturation
    uint8_t dataTypeA;             // data type of filter coefficients
    uint8_t dataTypeB;             // data type of input feature map
    uint8_t dataTypeC;             // data type of output
    uint8_t mode;                  // circular or linear addressing of feature map buffer
    int32_t circularOffset;        // starting offset for circular buffer of input feature ma, only for testing purpose. default use 0.
    int32_t totalN;                // total number of output columns to be processed for a feature map
    int32_t subN;                  // number of output pixels to be processed in one kernel call
    int32_t validColsPerRowIn;     // total pixels per row including left pad for strided convolution
    int32_t validRowsIn;           // total rows for strided flow including pad
    int32_t outputPitchPerRow;     // pitch of output rows for strided convolution
    int32_t inputPitchPerRow;      // pitch of input rows for strided convolution
    int32_t validColsInlast;       // initialize this same as validColsIn
    int32_t validColsPerRowInlast; // initialize this same as validColsPerRowIn
    int32_t validRowsInlast;       // initialize this same as validRowsIn
    int32_t numGroupsPerKernel;    // number of groups per kernel call default = 1
    int32_t outputDataLocation;    // 0 -> L2SRAM, 1-> MSMC
    int32_t testID;
} convolve_row_ixX_ixX_oxX_testParams_t;

void convolve_row_ixX_ixX_oxX_getTestParams(convolve_row_ixX_ixX_oxX_testParams_t * *params, int32_t *numTests);

#endif /* define _MMALIB_CNN_CONVOLVE_ROW_IXX_IXX_OXX_IDAT_H_ */

/* ======================================================================== */
/*  End of file:  MMALIB_CNN_convolve_row_ixX_ixX_oxX_idat.h                */
/* ======================================================================== */
