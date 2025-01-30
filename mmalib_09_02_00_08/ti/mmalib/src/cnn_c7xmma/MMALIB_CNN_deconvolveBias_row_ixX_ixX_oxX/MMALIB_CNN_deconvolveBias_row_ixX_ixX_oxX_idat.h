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
*  @file     MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_idat.h
*
*  @brief    File to have static test data related structure
*
*  @version  0.1 - Jan 2018 : Initial Version - Template [PKS]
*
*******************************************************************************
*/

#ifndef _MMALIB_CNN_DECONVOLVEBIAS_ROW_IXX_IXX_OXX_IDAT_H_
#define _MMALIB_CNN_DECONVOLVEBIAS_ROW_IXX_IXX_OXX_IDAT_H_

#include <mmalib.h>

// include test infrastructure provided by MMALIB
#include "../../test/MMALIB_test.h"

typedef struct {
   uint8_t testPattern; /* 0: constant, 1: sequential, 2: random, 3: static
                           array, 4: file, etc */
   void *  staticIn8Bit;
   void *  staticKernel8Bit;
   void *  staticBias32Bit;
   void *  staticOutMMALIB8Bit;
   void *  staticOut8Bit;
   void *  staticIn16Bit;
   void *  staticKernel16Bit;
   void *  staticBias64Bit;
   void *  staticOutMMALIB16Bit;
   void *  staticOut16Bit;
   void *  staticScale8Bit;
   void *  staticShift8Bit;
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
   int32_t expectedStatusCode;
   int32_t numGroups;
   MMALIB_quantMethod_type quantMethod;
   int8_t  activationType;
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
   int32_t                 outputDataLocation; // 0 -> L2SRAM, 1-> MSMC
   bool                    preProcessFlag;
   int32_t                 testID;
} MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_testParams_t;

void MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_getTestParams (
    MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_testParams_t **params,
    int32_t *                                     numTests);

#endif /* define _MMALIB_CNN_DECONVOLVEBIAS_ROW_IXX_IXX_OXX_IDAT_H_ */

/* ======================================================================== */
/*  End of file:  MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_idat.h */
/* ======================================================================== */
