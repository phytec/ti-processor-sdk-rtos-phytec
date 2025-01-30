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

#include "MMALIB_LINALG_matrixTranspose_ixX_oxX_idat.h"

static matrixTranspose_ixX_oxX_testParams_t    testParams[]=
{
   /********************************************
    {
    testPattern,
    staticIn,
    staticOut,
    dataType,
    heightIn, widthIn, strideIn,
    heightOut, widthOut, strideOut,
    numReps, testID
    },
    *********************************************/
   
#if ((TEST_CATEGORY == 1000) || (TEST_CASE == 1000))
   {
      RANDOM_SIGNED,
      NULL,
      NULL,
      MMALIB_INT32,
      MMALIB_MMA_SIZE_32_BIT*16, MMALIB_MMA_SIZE_32_BIT*8, MMALIB_MMA_SIZE_32_BIT*8*sizeof(int32_t),
      MMALIB_MMA_SIZE_32_BIT*8, MMALIB_MMA_SIZE_32_BIT*16, MMALIB_MMA_SIZE_32_BIT*16*sizeof(int32_t),
      MMALIB_TEST_OUTPUT_MSMC,
      1, 1000
   },
#endif
#if ((TEST_CATEGORY == 1000) || (TEST_CASE == 1001))
   {
      RANDOM_SIGNED,
      NULL,
      NULL,
      MMALIB_INT16,
      MMALIB_MMA_SIZE_16_BIT*16, MMALIB_MMA_SIZE_16_BIT*8, MMALIB_MMA_SIZE_16_BIT*8*sizeof(int16_t),
      MMALIB_MMA_SIZE_16_BIT*8, MMALIB_MMA_SIZE_16_BIT*16, MMALIB_MMA_SIZE_16_BIT*16*sizeof(int16_t),
      MMALIB_TEST_OUTPUT_MSMC,
      1, 1001
   },
#endif
#if ((TEST_CATEGORY == 1000) || (TEST_CASE == 1002))
   {
      RANDOM_SIGNED,
      NULL,
      NULL,
      MMALIB_INT8,
      MMALIB_MMA_SIZE_8_BIT*8, MMALIB_MMA_SIZE_8_BIT*8, MMALIB_MMA_SIZE_8_BIT*8*sizeof(int8_t),
      MMALIB_MMA_SIZE_8_BIT*8, MMALIB_MMA_SIZE_8_BIT*8, MMALIB_MMA_SIZE_8_BIT*8*sizeof(int8_t),
      MMALIB_TEST_OUTPUT_MSMC,
      1, 1002
   },
#endif
};

/*
 *  Sends the test parameter structure and number of tests
 */
void matrixTranspose_ixX_oxX_getTestParams(matrixTranspose_ixX_oxX_testParams_t * *params, int32_t *numTests)
{
   *params = testParams;
   *numTests = sizeof(testParams) / sizeof(matrixTranspose_ixX_oxX_testParams_t);
}

/* ======================================================================== */
/*                       End of file                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2016 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

