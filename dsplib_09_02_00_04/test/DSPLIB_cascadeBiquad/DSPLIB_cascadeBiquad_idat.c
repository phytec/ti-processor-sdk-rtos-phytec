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

#include "DSPLIB_cascadeBiquad_idat.h"

static cascadeBiquad_testParams_t testParams[] = {
/********************************************
 {
 testPattern,
 staticIn,
 filter,
 staticOut,
 dataType,
 dataSize,
 dataPitchIn,//in bytes
 dataPitchOut, // in bytes
 batchSize,
 filterSize,
 shift,
 outputDataLocation,
 numReps, testID
 },
 *********************************************/

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 1) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*filterCoeff
        NULL,                    //*filterVar
        NULL,                    //*staticOut
        DSPLIB_FLOAT32,          // dataType
        64,                      // dataSize
        2 * 4,                   // dataPitchIn
        2 * 4,                   // dataPitchOut
        2 * 4,                   // filterVarPitch
        2,                       // numChannels
        3,                       // numStages
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        1                        // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 2) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*filterCoeff
        NULL,                    //*filterVar
        NULL,                    //*staticOut
        DSPLIB_FLOAT32,          // dataType
        64,                      // dataSize
        4 * 4,                   // dataPitchIn
        4 * 4,                   // dataPitchOut
        4 * 4,                   // filterVarPitch
        4,                       // numChannels
        3,                       // numStages
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        2                        // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (EXTENDED_TEST_CASE == 3) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*filterCoeff
        NULL,                    //*filterVar
        NULL,                    //*staticOut
        DSPLIB_FLOAT32,          // dataType
        64,                      // dataSize
        8 * 4,                   // dataPitchIn
        8 * 4,                   // dataPitchOut
        8 * 4,                   // filterVarPitch
        8,                       // numChannels
        3,                       // numStages
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        3                        // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (EXTENDED_TEST_CASE == 4) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*filterCoeff
        NULL,                    //*filterVar
        NULL,                    //*staticOut
        DSPLIB_FLOAT32,          // dataType
        64,                      // dataSize
        16 * 4,                  // dataPitchIn
        16 * 4,                  // dataPitchOut
        16 * 4,                  // filterVarPitch
        16,                      // numChannels
        3,                       // numStages
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        4                        // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (EXTENDED_TEST_CASE == 5) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*filterCoeff
        NULL,                    //*filterVar
        NULL,                    //*staticOut
        DSPLIB_FLOAT32,          // dataType
        64,                      // dataSize
        32 * 4,                  // dataPitchIn
        32 * 4,                  // dataPitchOut
        32 * 4,                  // filterVarPitch
        32,                      // numChannels
        3,                       // numStages
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        5                        // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (EXTENDED_TEST_CASE == 6) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*filterCoeff
        NULL,                    //*filterVar
        NULL,                    //*staticOut
        DSPLIB_FLOAT32,          // dataType
        128,                     // dataSize
        32 * 4,                  // dataPitchIn
        32 * 4,                  // dataPitchOut
        32 * 4,                  // filterVarPitch
        32,                      // numChannels
        3,                       // numStages
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        6                        // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (EXTENDED_TEST_CASE == 7) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*filterCoeff
        NULL,                    //*filterVar
        NULL,                    //*staticOut
        DSPLIB_FLOAT32,          // dataType
        256,                     // dataSize
        32 * 4,                  // dataPitchIn
        32 * 4,                  // dataPitchOut
        32 * 4,                  // filterVarPitch
        32,                      // numChannels
        3,                       // numStages
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        7                        // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (EXTENDED_TEST_CASE == 8) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*filterCoeff
        NULL,                    //*filterVar
        NULL,                    //*staticOut
        DSPLIB_FLOAT32,          // dataType
        512,                     // dataSize
        32 * 4,                  // dataPitchIn
        32 * 4,                  // dataPitchOut
        32 * 4,                  // filterVarPitch
        32,                      // numChannels
        3,                       // numStages
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        8                        // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (EXTENDED_TEST_CASE == 9) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*filterCoeff
        NULL,                    //*filterVar
        NULL,                    //*staticOut
        DSPLIB_FLOAT32,          // dataType
        512,                     // dataSize
        40 * 4,                  // dataPitchIn
        40 * 4,                  // dataPitchOut
        40 * 4,                  // filterVarPitch
        40,                      // numChannels
        3,                       // numStages
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        9                        // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (EXTENDED_TEST_CASE == 10) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*filterCoeff
        NULL,                    //*filterVar
        NULL,                    //*staticOut
        DSPLIB_FLOAT32,          // dataType
        64,                      // dataSize
        2 * 4,                   // dataPitchIn
        2 * 4,                   // dataPitchOut
        2 * 4,                   // filterVarPitch
        2,                       // numChannels
        7,                       // numStages
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        10                       // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (EXTENDED_TEST_CASE == 11) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*filterCoeff
        NULL,                    //*filterVar
        NULL,                    //*staticOut
        DSPLIB_FLOAT32,          // dataType
        64,                      // dataSize
        16 * 4,                  // dataPitchIn
        16 * 4,                  // dataPitchOut
        16 * 4,                  // filterVarPitch
        16,                      // numChannels
        7,                       // numStages
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        11                       // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (EXTENDED_TEST_CASE == 12) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*filterCoeff
        NULL,                    //*filterVar
        NULL,                    //*staticOut
        DSPLIB_FLOAT32,          // dataType
        64,                      // dataSize
        32 * 4,                  // dataPitchIn
        32 * 4,                  // dataPitchOut
        32 * 4,                  // filterVarPitch
        32,                      // numChannels
        7,                       // numStages
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        12                       // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (EXTENDED_TEST_CASE == 13) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*filterCoeff
        NULL,                    //*filterVar
        NULL,                    //*staticOut
        DSPLIB_FLOAT32,          // dataType
        128,                     // dataSize
        32 * 4,                  // dataPitchIn
        32 * 4,                  // dataPitchOut
        32 * 4,                  // filterVarPitch
        32,                      // numChannels
        7,                       // numStages
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        13                       // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (EXTENDED_TEST_CASE == 14) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*filterCoeff
        NULL,                    //*filterVar
        NULL,                    //*staticOut
        DSPLIB_FLOAT32,          // dataType
        256,                     // dataSize
        40 * 4,                  // dataPitchIn
        40 * 4,                  // dataPitchOut
        40 * 4,                  // filterVarPitch
        40,                      // numChannels
        7,                       // numStages
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        14                       // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (EXTENDED_TEST_CASE == 15) || (TEST_CATEGORY == 100))
    {
        RANDOM,
        NULL,                    //*staticIn
        NULL,                    //*filterCoeff
        NULL,                    //*filterVar
        NULL,                    //*staticOut
        DSPLIB_FLOAT32,          // dataType
        512,                     // dataSize
        40 * 4,                  // dataPitchIn
        40 * 4,                  // dataPitchOut
        40 * 4,                  // filterVarPitch
        40,                      // numChannels
        7,                       // numStages
        DSPLIB_TEST_OUTPUT_MSMC, // outputDataLocation
        1,                       // numReps
        15                       // testID
    },
#endif

};

/*
 *  Sends the test parameter structure and number of tests
 */
void cascadeBiquad_getTestParams(cascadeBiquad_testParams_t **params, int32_t *numTests)
{
   *params   = testParams;
   *numTests = sizeof(testParams) / sizeof(cascadeBiquad_testParams_t);
}

/* ======================================================================== */
/*                       End of file                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2016 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
