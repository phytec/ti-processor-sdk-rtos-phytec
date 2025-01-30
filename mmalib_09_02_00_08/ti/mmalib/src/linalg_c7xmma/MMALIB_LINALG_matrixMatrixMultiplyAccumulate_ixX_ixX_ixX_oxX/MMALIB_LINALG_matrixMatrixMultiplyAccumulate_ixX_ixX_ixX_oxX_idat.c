/*******************************************************************************
 **+--------------------------------------------------------------------------+**
 **|                            ****                                          |**
 **|                            ****                                          |**
 **|                            ******o***                                    |**
 **|                      ********_///_****                                   |**
 **|                      ***** /_//_/ ****                                   |**
 **|                       ** ** (__/ ****                                    |**
 **|                           *********                                      |**
 **|                            ****                                          |**
 **|                            ***                                           |**
 **|                                                                          |**
 **|         Copyright (c) 2016 Texas Instruments Incorporated                |**
 **|                        ALL RIGHTS RESERVED                               |**
 **|                                                                          |**
 **| Permission to use, copy, modify, or distribute this software,            |**
 **| whether in part or in whole, for any purpose is forbidden without        |**
 **| a signed licensing agreement and NDA from Texas Instruments              |**
 **| Incorporated (TI).                                                       |**
 **|                                                                          |**
 **| TI makes no representation or warranties with respect to the             |**
 **| performance of this computer program, and specifically disclaims         |**
 **| any responsibility for any damages, special or consequential,            |**
 **| connected with the use of this program.                                  |**
 **|                                                                          |**
 **+--------------------------------------------------------------------------+**
 *******************************************************************************/

#include "MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_idat.h"


static matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_testParams_t    testParams[]=
{
   /********************************************
    {
    testPattern,
    staticIn0, staticIn1, staticIn2, staticOut
    heightIn0 (M), widthIn0 (K), strideIn0
    heightIn1 (K), widthIn1 (N), strideIn1
    heightOut (M), widthOut (N), strideOut,
    heightOut (M), widthOut (N), strideOut,
    shiftOut, numReps
    },
    *********************************************/
   
#if ((TEST_CATEGORY == 1000) || (TEST_CASE == 1000))
   {  // memory size = 113 KB
      MMALIB_PERFORMANCE_TEST_PATTERN,
      NULL, MMALIB_INT8, NULL, MMALIB_INT8, NULL, MMALIB_INT8, NULL, MMALIB_INT8,
      1*MMALIB_MMA_SIZE_8_BIT+1, 4*MMALIB_MMA_SIZE_8_BIT+1, MMALIB_CALC_STRIDE((4*MMALIB_MMA_SIZE_8_BIT+1)*sizeof(int8_t), MMALIB_ALIGN_SHIFT_64BYTES),
      4*MMALIB_MMA_SIZE_8_BIT+1, 4*MMALIB_MMA_SIZE_8_BIT+1, MMALIB_CALC_STRIDE((4*MMALIB_MMA_SIZE_8_BIT+1)*sizeof(int8_t), MMALIB_ALIGN_SHIFT_64BYTES),
      1*MMALIB_MMA_SIZE_8_BIT+1, 4*MMALIB_MMA_SIZE_8_BIT+1, MMALIB_CALC_STRIDE((4*MMALIB_MMA_SIZE_8_BIT+1)*sizeof(int8_t), MMALIB_ALIGN_SHIFT_64BYTES),
      1*MMALIB_MMA_SIZE_8_BIT+1, 4*MMALIB_MMA_SIZE_8_BIT+1, MMALIB_CALC_STRIDE((4*MMALIB_MMA_SIZE_8_BIT+1)*sizeof(int8_t), MMALIB_ALIGN_SHIFT_64BYTES),
      11,   // shiftOut
      MMALIB_TEST_OUTPUT_MSMC,
      1,    // numReps
      1000   //testID
   },
#endif
#if ((TEST_CATEGORY == 1000) || (TEST_CASE == 1001))
   {  // memory size = 134 KB
      MMALIB_PERFORMANCE_TEST_PATTERN,
      NULL, MMALIB_INT8, NULL, MMALIB_INT8, NULL, MMALIB_INT8, NULL, MMALIB_INT8,
      1*MMALIB_MMA_SIZE_8_BIT-1, 16*MMALIB_MMA_SIZE_8_BIT-1, MMALIB_CALC_STRIDE((16*MMALIB_MMA_SIZE_8_BIT-1)*sizeof(int8_t), MMALIB_ALIGN_SHIFT_64BYTES),
      16*MMALIB_MMA_SIZE_8_BIT-1, 1*MMALIB_MMA_SIZE_8_BIT-1, MMALIB_CALC_STRIDE((1*MMALIB_MMA_SIZE_8_BIT-1)*sizeof(int8_t), MMALIB_ALIGN_SHIFT_64BYTES),
      1*MMALIB_MMA_SIZE_8_BIT-1, 1*MMALIB_MMA_SIZE_8_BIT-1, MMALIB_CALC_STRIDE((1*MMALIB_MMA_SIZE_8_BIT-1)*sizeof(int8_t), MMALIB_ALIGN_SHIFT_64BYTES),
      1*MMALIB_MMA_SIZE_8_BIT-1, 1*MMALIB_MMA_SIZE_8_BIT-1, MMALIB_CALC_STRIDE((1*MMALIB_MMA_SIZE_8_BIT-1)*sizeof(int8_t), MMALIB_ALIGN_SHIFT_64BYTES),
      11,   // shiftOut
      MMALIB_TEST_OUTPUT_MSMC,
      1,    // numReps
      1001   //testID
   },
#endif
#if ((TEST_CATEGORY == 1000) || (TEST_CASE == 1002))
   {  // memory size = 204 KB
      MMALIB_PERFORMANCE_TEST_PATTERN,
      NULL, MMALIB_INT16, NULL, MMALIB_INT16, NULL, MMALIB_INT16, NULL, MMALIB_INT16,
      1*MMALIB_MMA_SIZE_16_BIT+1, 4*MMALIB_MMA_SIZE_16_BIT+1, MMALIB_CALC_STRIDE((4*MMALIB_MMA_SIZE_16_BIT+1)*sizeof(int16_t), MMALIB_ALIGN_SHIFT_64BYTES),
      4*MMALIB_MMA_SIZE_16_BIT+1, 16*MMALIB_MMA_SIZE_16_BIT+1, MMALIB_CALC_STRIDE((16*MMALIB_MMA_SIZE_16_BIT+1)*sizeof(int16_t), MMALIB_ALIGN_SHIFT_64BYTES),
      1*MMALIB_MMA_SIZE_16_BIT+1, 16*MMALIB_MMA_SIZE_16_BIT+1, MMALIB_CALC_STRIDE((16*MMALIB_MMA_SIZE_16_BIT+1)*sizeof(int16_t), MMALIB_ALIGN_SHIFT_64BYTES),
      1*MMALIB_MMA_SIZE_16_BIT+1, 16*MMALIB_MMA_SIZE_16_BIT+1, MMALIB_CALC_STRIDE((16*MMALIB_MMA_SIZE_16_BIT+1)*sizeof(int16_t), MMALIB_ALIGN_SHIFT_64BYTES),
      22,   // shiftOut
      MMALIB_TEST_OUTPUT_MSMC,
      1,    // numReps
      1002   //testID
   },
#endif
#if ((TEST_CATEGORY == 1000) || (TEST_CASE == 1003))
   {  // memory size = 309 KB
      MMALIB_PERFORMANCE_TEST_PATTERN,
      NULL, MMALIB_INT32, NULL, MMALIB_INT32, NULL, MMALIB_INT32, NULL, MMALIB_INT32,
      1*MMALIB_MMA_SIZE_32_BIT+1, 16*MMALIB_MMA_SIZE_32_BIT+1, MMALIB_CALC_STRIDE((16*MMALIB_MMA_SIZE_32_BIT+1)*sizeof(int32_t), MMALIB_ALIGN_SHIFT_64BYTES),
      16*MMALIB_MMA_SIZE_32_BIT+1, 16*MMALIB_MMA_SIZE_32_BIT+1, MMALIB_CALC_STRIDE((16*MMALIB_MMA_SIZE_32_BIT+1)*sizeof(int32_t), MMALIB_ALIGN_SHIFT_64BYTES),
      1*MMALIB_MMA_SIZE_32_BIT+1, 16*MMALIB_MMA_SIZE_32_BIT+1, MMALIB_CALC_STRIDE((16*MMALIB_MMA_SIZE_32_BIT+1)*sizeof(int32_t), MMALIB_ALIGN_SHIFT_64BYTES),
      1*MMALIB_MMA_SIZE_32_BIT+1, 16*MMALIB_MMA_SIZE_32_BIT+1, MMALIB_CALC_STRIDE((16*MMALIB_MMA_SIZE_32_BIT+1)*sizeof(int32_t), MMALIB_ALIGN_SHIFT_64BYTES),
      44,   // shiftOut
      MMALIB_TEST_OUTPUT_MSMC,
      1,    // numReps
      1003   //testID
   },
#endif
};

/*
 *  Sends the test parameter structure and number of tests
 */
void matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_getTestParams(matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_testParams_t * *params, int32_t *numTests)
{
   *params = testParams;
   *numTests = sizeof(testParams) / sizeof(matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_testParams_t);
}

/* ======================================================================== */
/*                       End of file                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2016 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

