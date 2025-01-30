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
*  @file     MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_idat.h
*
*  @brief    File to have static test data related structure
*
*  @version  0.1 - Jan 2018 : Initial Version [AB]
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
    void  *staticIn;
    void  *staticKernel;
    void  *staticBias;
    void  *staticOut;
    void  *staticScale;
    void  *staticShift;
    void  *dynamicRangeMin;
    void  *dynamicRangeMax;
    int32_t kernelWidth;
    int32_t kernelHeight;
    int32_t strideWidth;
    int32_t strideHeight;
    int32_t dilationHeight;
    int32_t dilationWidth;
    int32_t inWidth;
    int32_t pad;
    int32_t maxHeight;
    int32_t inChOffset;
    int32_t validColsIn;
    int32_t kDim;
    int32_t pitchA;
    int32_t numOfOutputChKerBuf;
    int32_t pitchC;
    int32_t subMChannels;
    int32_t numInChannels;
    int32_t numOutChannels;
   // uint8_t qShift;
    uint8_t quantMethod;
    int32_t biasB;
    int32_t expectedStatusCode;    // input test status code
    int8_t  activationType;
    /*! @brief Max value for parameterized saturation */
    int32_t pSatMin;
    /*! @brief Min value for parameterized saturation */
    int32_t pSatMax;
    uint8_t dataTypeA;
    uint8_t dataTypeB;
    uint8_t dataTypeC;
    uint8_t dataTypeBias;
    uint8_t dataTypeS;
    uint8_t mode;
    int32_t circularOffset;
    int32_t totalN;
    int32_t subN;
    int32_t validColsPerRowIn;
    int32_t validRowsIn;
    int32_t outputPitchPerRow;
    int32_t inputPitchPerRow;
    int32_t validColsInlast;
    int32_t validColsPerRowInlast;
    int32_t validRowsInlast;
    int32_t spadOffset;
    int32_t spadOffsetExec;
    int32_t row;
    int32_t padLeft;
    int32_t padRight;
    int32_t padTop;
    int32_t padBottom;
    int32_t padFillValue;
    int32_t validColsOutBottom;
    int32_t numGroupsPerKernel;
    int8_t  packetizeMode;
    MMALIB_enableDynamicRange_type enableDynamicRange;
    MMALIB_initDynamicRange_type initDynamicRange;
    int32_t outputDataLocation; // 0 -> L2SRAM, 1-> MSMC
    int32_t testID;
} MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_testParams_t;

void MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_getTestParams(MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_testParams_t * *params, int32_t *numTests);

#endif /* define _MMALIB_CNN_CONVOLVE_ROW_IXX_IXX_OXX_IDAT_H_ */

/* ======================================================================== */
/*  End of file:  MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_idat.h                */
/* ======================================================================== */
