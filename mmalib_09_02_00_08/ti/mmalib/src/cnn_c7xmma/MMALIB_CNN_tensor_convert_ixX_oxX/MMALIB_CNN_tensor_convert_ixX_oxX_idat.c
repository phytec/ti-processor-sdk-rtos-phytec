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
*  @file     MMALIB_CNN_tensor_convert_ixX_oxX_idat.c
*
*  @brief    File to have static test data related content
*
*  @version  0.1 - June 2021 : Initial Version - Template [PKS]
*
*******************************************************************************
*/

#include "MMALIB_CNN_tensor_convert_ixX_oxX_idat.h"


#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 100) || (TEST_CATEGORY == 200) || (TEST_CASE == 7))
#include "test_data/MMALIB_CNN_tensor_convert_idat_testcase7.h"
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 100) || (TEST_CATEGORY == 200) || (TEST_CASE == 48))
#include "test_data/MMALIB_CNN_tensor_convert_idat_testcase48.h"
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 100) || (TEST_CATEGORY == 200) || (TEST_CASE == 100))
#include "test_data/MMALIB_CNN_tensor_convert_idat_testcase100.h"
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 100) || (TEST_CATEGORY == 200) || (TEST_CASE == 110))
#include "test_data/MMALIB_CNN_tensor_convert_idat_testcase110.h"
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 100) || (TEST_CATEGORY == 200) || (TEST_CASE == 125))
#include "test_data/MMALIB_CNN_tensor_convert_idat_testcase125.h"
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 5000) || (TEST_CASE == 136))
#include "test_data/MMALIB_CNN_tensor_convert_idat_testcase136.h"
#endif


static MMALIB_CNN_tensor_convert_ixX_oxX_testParams_t testParams[] =  {

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 100) || (TEST_CATEGORY == 200) || (TEST_CASE == 7))
   {
      STATIC,                                                 // test pattern
      staticRefInputCase7,                                    // Input tensor
      MMALIB_INT8,                                            // datatypeIn
      staticRefInputScaleCase7,                               // scale vector/scalar
      MMALIB_FLOAT32,                                         // dataTypeScale
      staticRefInputShiftCase7,                               // shift vector/scalar
      staticRefInputQ1Case7,                                  // Q1 vector/scalar
      staticRefInputQ2Case7,                                  // Q2 vector/scalar
      staticRefOutputCase7,                                   // output tensor
      MMALIB_FLOAT32,                                         // datatypOutC
      MMALIB_SUCCESS,                                        // expectedStatusCode;
      55,                                                     // width
      55,                                                     // height
      20,                                                     // numChannels
      64 * sizeof(int8_t),                                    // strideInY
      4096 * sizeof(int8_t),                                  // strideInZ
      64 * sizeof(float),                                     // strideOutY
      4096 * sizeof(float),                                   // strideOutZ
      0,                                                      // inPad
      1,                                                      // outPad
      0,                                                      // numRowsQuant
      MMALIB_CHW,                                             // tensorFormatIn
      MMALIB_CHW,                                             // tensorFormatOut
      MMALIB_QUANT_PER_KERNEL,                                // quantization method
      MMALIB_TEST_OUTPUT_MSMC,                                // output data location
      1,                                                      // num. of repetitions
      7,                                                      // test ID
   },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 100) || (TEST_CATEGORY == 200) || (TEST_CASE == 48))
   {
      STATIC,                                                 // test pattern
      staticRefInputCase48,                                   // Input tensor
      MMALIB_UINT8,                                           // datatypeIn
      staticRefInputScaleCase48,                              // scale vector/scalar
      MMALIB_FLOAT32,                                         // dataTypeScale
      staticRefInputShiftCase48,                              // shift vector/scalar
      staticRefInputQ1Case48,                                 // Q1 vector/scalar
      staticRefInputQ2Case48,                                 // Q2 vector/scalar
      staticRefOutputCase48,                                  // output tensor
      MMALIB_FLOAT32,                                         // datatypOutC
      MMALIB_SUCCESS,                                        // expectedStatusCode;
      80,                                                     // width
      12,                                                     // height
      64,                                                     // numChannels
      80 * sizeof(uint8_t),                                   // strideInY
      960 * sizeof(uint8_t),                                  // strideInZ
      64 * sizeof(float),                                     // strideOutY
      5120 * sizeof(float),                                   // strideOutZ
      0,                                                      // inPad
      0,                                                      // outPad
      0,                                                      // numRowsQuant
      MMALIB_CHW,                                             // tensorFormatIn
      MMALIB_HWC,                                             // tensorFormatOut
      MMALIB_QUANT_PER_KERNEL,                                // quantization method
      MMALIB_TEST_OUTPUT_MSMC,                                // output data location
      1,                                                      // num. of repetitions
      48,                                                     // test ID
   },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 100) || (TEST_CATEGORY == 200) || (TEST_CASE == 100))
   {
      STATIC,                                                 // test pattern
      staticRefInputCase100,                                  // Input tensor
      MMALIB_FLOAT32,                                         // datatypeIn
      staticRefInputScaleCase100,                             // scale vector/scalar
      MMALIB_FLOAT32,                                         // dataTypeScale
      staticRefInputShiftCase100,                             // shift vector/scalar
      staticRefInputQ1Case100,                                // Q1 vector/scalar
      staticRefInputQ2Case100,                                // Q2 vector/scalar
      staticRefOutputCase100,                                 // output tensor
      MMALIB_UINT8,                                           // datatypOutC
      MMALIB_SUCCESS,                                        // expectedStatusCode;
      92,                                                     // width
      86,                                                     // height
      8,                                                      // numChannels
      92 * sizeof(float),                                     // strideInY
      7912 * sizeof(float),                                   // strideInZ
      96 * sizeof(uint8_t),                                   // strideOutY
      8256 * sizeof(uint8_t),                                 // strideOutZ
      0,                                                      // inPad
      2,                                                      // outPad
      0,                                                      // numRowsQuant
      MMALIB_CHW,                                             // tensorFormatIn
      MMALIB_CHW,                                             // tensorFormatOut
      MMALIB_QUANT_PER_KERNEL,                                // quantization method
      MMALIB_TEST_OUTPUT_MSMC,                                // output data location
      1,                                                      // num. of repetitions
      100,                                                    // test ID
   },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 100) || (TEST_CATEGORY == 200) || (TEST_CASE == 110))
   {
      STATIC,                                                 // test pattern
      staticRefInputCase110,                                  // Input tensor
      MMALIB_FLOAT32,                                         // datatypeIn
      staticRefInputScaleCase110,                             // scale vector/scalar
      MMALIB_FLOAT32,                                         // dataTypeScale
      staticRefInputShiftCase110,                             // shift vector/scalar
      staticRefInputQ1Case110,                                // Q1 vector/scalar
      staticRefInputQ2Case110,                                // Q2 vector/scalar
      staticRefOutputCase110,                                 // output tensor
      MMALIB_INT8,                                            // datatypOutC
      MMALIB_SUCCESS,                                        // expectedStatusCode;
      92,                                                     // width
      86,                                                     // height
      8,                                                      // numChannels
      8 * sizeof(float),                                      // strideInY
      736 * sizeof(float),                                    // strideInZ
      94 * sizeof(int8_t),                                    // strideOutY
      8084 * sizeof(int8_t),                                  // strideOutZ
      0,                                                      // inPad
      2,                                                      // outPad
      0,                                                      // numRowsQuant
      MMALIB_HWC,                                             // tensorFormatIn
      MMALIB_CHW,                                             // tensorFormatOut
      MMALIB_QUANT_PER_KERNEL,                                // quantization method
      MMALIB_TEST_OUTPUT_MSMC,                                // output data location
      1,                                                      // num. of repetitions
      110,                                                    // test ID
   },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 100) || (TEST_CATEGORY == 200) || (TEST_CASE == 125))
   {
      STATIC,                                                 // test pattern
      staticRefInputCase125,                                  // Input tensor
      MMALIB_FLOAT32,                                         // datatypeIn
      staticRefInputScaleCase125,                             // scale vector/scalar
      MMALIB_FLOAT32,                                         // dataTypeScale
      staticRefInputShiftCase125,                             // shift vector/scalar
      staticRefInputQ1Case125,                                // Q1 vector/scalar
      staticRefInputQ2Case125,                                // Q2 vector/scalar
      staticRefOutputCase125,                                 // output tensor
      MMALIB_UINT8,                                           // datatypOutC
      MMALIB_SUCCESS,                                        // expectedStatusCode;
      16,                                                     // width
      1,                                                      // height
      1,                                                      // numChannels
      16 * sizeof(float),                                     // strideInY
      16 * sizeof(float),                                     // strideInZ
      16 * sizeof(uint8_t),                                   // strideOutY
      16 * sizeof(uint8_t),                                   // strideOutZ
      0,                                                      // inPad
      2,                                                      // outPad
      0,                                                      // numRowsQuant
      MMALIB_CHW,                                             // tensorFormatIn
      MMALIB_CHW,                                             // tensorFormatOut
      MMALIB_QUANT_PER_KERNEL,                                // quantization method
      MMALIB_TEST_OUTPUT_MSMC,                                // output data location
      1,                                                      // num. of repetitions
      125,                                                    // test ID
   },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 5000) || (TEST_CASE == 136))
   {
      STATIC,                                                 // test pattern
      staticRefInputCase136,                                  // Input tensor
      MMALIB_FLOAT32,                                         // datatypeIn
      staticRefInputScaleCase136,                             // scale vector/scalar
      MMALIB_FLOAT32,                                         // dataTypeScale
      staticRefInputShiftCase136,                             // shift vector/scalar
      staticRefInputQ1Case136,                                // Q1 vector/scalar
      staticRefInputQ2Case136,                                // Q2 vector/scalar
      staticRefOutputCase136,                                 // output tensor
      MMALIB_INT32,                                           // datatypOutC
      MMALIB_ERR_TENSOR_CONVERT_UNSUPPORTED_TYPE_COMBINATION,// expectedStatusCode;
      16,                                                     // width
      1,                                                      // height
      1,                                                      // numChannels
      16 * sizeof(float),                                     // strideInY
      16 * sizeof(float),                                     // strideInZ
      16 * sizeof(int32_t),                                   // strideOutY
      16 * sizeof(int32_t),                                   // strideOutZ
      0,                                                      // inPad
      2,                                                      // outPad
      0,                                                      // numRowsQuant
      MMALIB_CHW,                                             // tensorFormatIn
      MMALIB_CHW,                                             // tensorFormatOut
      MMALIB_QUANT_PER_KERNEL,                                // quantization method
      MMALIB_TEST_OUTPUT_MSMC,                                // output data location
      1,                                                      // num. of repetitions
      136,                                                    // test ID
   },
#endif

};

/*
 *  Sends the test parameter structure and number of tests
 */
void MMALIB_CNN_tensor_convert_ixX_oxX_getTestParams (
    MMALIB_CNN_tensor_convert_ixX_oxX_testParams_t **params,
    int32_t *                                 numTests)
{
   *params = testParams;
   *numTests =
       sizeof (testParams) / sizeof (MMALIB_CNN_tensor_convert_ixX_oxX_testParams_t);
}

/* ======================================================================== */
/*                       End of file                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2016 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
