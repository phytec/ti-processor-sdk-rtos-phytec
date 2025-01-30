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

#include "MMALIB_LINALG_pointwiseMatrixMatrixMultiply_ixX_ixX_oxX_idat.h"


static pointwiseMatrixMatrixMultiply_ixX_ixX_oxX_testParams_t    testParams[]=
{
   /********************************************
    {
    testPattern,
    staticIn0, staticIn1, staticOut
    heightIn0 (M), widthIn0 (N), strideIn0
    heightIn1 (M), widthIn1 (N), strideIn1
    heightOut (M), widthOut (N), strideOut,
    shiftOut, numRepetitions
    },
    *********************************************/
   
#if ((TEST_CATEGORY == 1000) || (TEST_CASE == 1000))
   {
      MMALIB_PERFORMANCE_TEST_PATTERN,
      NULL, MMALIB_INT8, NULL, MMALIB_INT8, NULL, MMALIB_INT8,
      16*MMALIB_MMA_SIZE_8_BIT, 1*MMALIB_MMA_SIZE_8_BIT, (1*MMALIB_MMA_SIZE_8_BIT)*sizeof(int8_t),
      16*MMALIB_MMA_SIZE_8_BIT, 1*MMALIB_MMA_SIZE_8_BIT, (1*MMALIB_MMA_SIZE_8_BIT)*sizeof(int8_t),
      16*MMALIB_MMA_SIZE_8_BIT, 1*MMALIB_MMA_SIZE_8_BIT, (1*MMALIB_MMA_SIZE_8_BIT)*sizeof(int8_t),
      10, MMALIB_TEST_OUTPUT_MSMC,1,1000
   },
#endif
#if ((TEST_CATEGORY == 1000) || (TEST_CASE == 1001))
   {
      MMALIB_PERFORMANCE_TEST_PATTERN,
      NULL, MMALIB_INT32, NULL, MMALIB_INT32, NULL, MMALIB_INT32,
      16*MMALIB_MMA_SIZE_32_BIT, 4*MMALIB_MMA_SIZE_32_BIT, (4*MMALIB_MMA_SIZE_32_BIT)*sizeof(int32_t),
      16*MMALIB_MMA_SIZE_32_BIT, 4*MMALIB_MMA_SIZE_32_BIT, (4*MMALIB_MMA_SIZE_32_BIT)*sizeof(int32_t),
      16*MMALIB_MMA_SIZE_32_BIT, 4*MMALIB_MMA_SIZE_32_BIT, (4*MMALIB_MMA_SIZE_32_BIT)*sizeof(int32_t),
      20, MMALIB_TEST_OUTPUT_MSMC,1,1001
   },
#endif
};

/*
 *  Sends the test parameter structure and number of tests
 */
void pointwiseMatrixMatrixMultiply_ixX_ixX_oxX_getTestParams(pointwiseMatrixMatrixMultiply_ixX_ixX_oxX_testParams_t * *params, int32_t *numTests)
{
   *params = testParams;
   *numTests = sizeof(testParams) / sizeof(pointwiseMatrixMatrixMultiply_ixX_ixX_oxX_testParams_t);
}

/* ======================================================================== */
/*                       End of file                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2016 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

