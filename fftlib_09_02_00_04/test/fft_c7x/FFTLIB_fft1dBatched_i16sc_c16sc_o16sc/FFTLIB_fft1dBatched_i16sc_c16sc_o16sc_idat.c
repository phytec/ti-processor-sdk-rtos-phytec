/*******************************************************************************
 **+--------------------------------------------------------------------------+**
 **|                            **** |**
 **|                            **** |**
 **|                            ******o*** |**
 **|                      ********_///_**** |**
 **|                      ***** /_//_/ **** |**
 **|                       ** ** (__/ **** |**
 **|                           ********* |**
 **|                            **** |**
 **|                            *** |**
 **| |**
 **|         Copyright (c) 2017 Texas Instruments Incorporated |**
 **|                        ALL RIGHTS RESERVED |**
 **| |**
 **| Permission to use, copy, modify, or distribute this software, |**
 **| whether in part or in whole, for any purpose is forbidden without |**
 **| a signed licensing agreement and NDA from Texas Instruments |**
 **| Incorporated (TI). |**
 **| |**
 **| TI makes no representation or warranties with respect to the |**
 **| performance of this computer program, and specifically disclaims |**
 **| any responsibility for any damages, special or consequential, |**
 **| connected with the use of this program. |**
 **| |**
 **+--------------------------------------------------------------------------+**
 *******************************************************************************/
#include "FFTLIB_fft1dBatched_i16sc_c16sc_o16sc_idat.h"
/* #include "../../common/TI_memory.h" */


__attribute__((section (".staticData")))
     static uint32_t shiftVector16Point[] = { 0, 0 };

__attribute__((section (".staticData")))
     static uint32_t shiftVector32Point[] = { 0, 0 };

__attribute__((section (".staticData")))
     static uint32_t shiftVector64Point[] = { 0, 0, 0 };

__attribute__((section (".staticData")))
     static uint32_t shiftVector128Point[] = { 0, 0, 0 };

__attribute__((section (".staticData")))
     static uint32_t shiftVector256Point[] = { 0, 0, 0, 0 };

__attribute__((section (".staticData")))
     static uint32_t shiftVector512Point[] = { 0, 0, 0, 0 };

__attribute__((section (".staticData")))
     static uint32_t shiftVector1024Point[] = { 0, 0, 0, 0, 0 };

__attribute__((section (".staticData")))
     static uint32_t shiftVector2048Point[] = { 0, 0, 0, 0, 0 };

__attribute__((section (".staticData")))
     static uint32_t shiftVector4096Point[] = { 0, 0, 0, 0, 0, 0 };

__attribute__((section (".staticData")))
     static uint32_t shiftVector8192Point[] = { 0, 0, 0, 0, 0, 0 };

static fft1dBatched_i16sc_c16sc_o16sc_testParams_t testParams[] = {
/* #if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 0)) */
/*    { */
/*     RANDOM_SIGNED,                                    // test pattern */
/*     NULL,                                             // staticIn0 */
/*     shiftVector16Point,                               // shiftVector */
/*     NULL,                                             // staticIn1 */
/*     2,                                                // numShifts */
/*     32,                                               // numChannels */
/*     16,                                               // numPoints */
/*     0,                                                // test ID */
/*     }, */
/* #endif */

/* #if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 1)) */
/*    { */
/*     RANDOM_SIGNED,                                    // test pattern */
/*     NULL,                                             // staticIn0 */
/*     shiftVector16Point,                               // shiftVector */
/*     NULL,                                             // staticIn1 */
/*     2,                                                // numShifts */
/*     64,                                               // numChannels */
/*     16,                                               // numPoints */
/*     1,                                                // test ID */
/*     }, */
/* #endif */

/* #if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 2)) */
/*    { */
/*     RANDOM_SIGNED,                                    // test pattern */
/*     NULL,                                             // staticIn0 */
/*     shiftVector16Point,                               // shiftVector */
/*     NULL,                                             // staticIn1 */
/*     2,                                                // numShifts */
/*     128,                                              // numChannels */
/*     16,                                               // numPoints */
/*     2,                                                // test ID */
/*     }, */
/* #endif */

/* #if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 3)) */
/*    { */
/*     RANDOM_SIGNED,                                    // test pattern */
/*     NULL,                                             // staticIn0 */
/*     shiftVector16Point,                               // shiftVector */
/*     NULL,                                             // staticIn1 */
/*     2,                                                // numShifts */
/*     256,                                              // numChannels */
/*     16,                                               // numPoints */
/*     3,                                                // test ID */
/*     }, */
/* #endif */

/* #if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 4)) */
/*    { */
/*     RANDOM_SIGNED,                                    // test pattern */
/*     NULL,                                             // staticIn0 */
/*     shiftVector16Point,                               // shiftVector */
/*     NULL,                                             // staticIn1 */
/*     2,                                                // numShifts */
/*     512,                                              // numChannels */
/*     16,                                               // numPoints */
/*     4,                                                // test ID */
/*     }, */
/* #endif */

/* #if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 5)) */
/*    { */
/*     RANDOM_SIGNED,                                    // test pattern */
/*     NULL,                                             // staticIn0 */
/*     shiftVector32Point,                               // shiftVector */
/*     NULL,                                             // staticIn1 */
/*     2,                                                // numShifts */
/*     32,                                               // numChannels */
/*     32,                                               // numPoints */
/*     5,                                                // test ID */
/*     }, */
/* #endif */

/* #if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 6)) */
/*    { */
/*     RANDOM_SIGNED,                                    // test pattern */
/*     NULL,                                             // staticIn0 */
/*     shiftVector32Point,                               // shiftVector */
/*     NULL,                                             // staticIn1 */
/*     2,                                                // numShifts */
/*     64,                                               // numChannels */
/*     32,                                               // numPoints */
/*     6,                                                // test ID */
/*     }, */
/* #endif */

/* #if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 7)) */
/*    { */
/*     RANDOM_SIGNED,                                    // test pattern */
/*     NULL,                                             // staticIn0 */
/*     shiftVector32Point,                               // shiftVector */
/*     NULL,                                             // staticIn1 */
/*     2,                                                // numShifts */
/*     128,                                              // numChannels */
/*     32,                                               // numPoints */
/*     7,                                                // test ID */
/*     }, */
/* #endif */

/* #if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 8)) */
/*    { */
/*     RANDOM_SIGNED,                                    // test pattern */
/*     NULL,                                             // staticIn0 */
/*     shiftVector32Point,                               // shiftVector */
/*     NULL,                                             // staticIn1 */
/*     2,                                                // numShifts */
/*     256,                                              // numChannels */
/*     32,                                               // numPoints */
/*     8,                                                // test ID */
/*     }, */
/* #endif */

/* #if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 9)) */
/*    { */
/*     RANDOM_SIGNED,                                    // test pattern */
/*     NULL,                                             // staticIn0 */
/*     shiftVector32Point,                               // shiftVector */
/*     NULL,                                             // staticIn1 */
/*     2,                                                // numShifts */
/*     512,                                              // numChannels */
/*     32,                                               // numPoints */
/*     9,                                                // test ID */
/*     }, */
/* #endif */

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 10))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector64Point,                               // shiftVector
    NULL,                                             // staticIn1
    3,                                                // numShifts
    32,                                               // numChannels
    64,                                               // numPoints
    10,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 11))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector64Point,                               // shiftVector
    NULL,                                             // staticIn1
    3,                                                // numShifts
    64,                                               // numChannels
    64,                                               // numPoints
    11,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 12))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector64Point,                               // shiftVector
    NULL,                                             // staticIn1
    3,                                                // numShifts
    128,                                              // numChannels
    64,                                               // numPoints
    12,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 13))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector64Point,                               // shiftVector
    NULL,                                             // staticIn1
    3,                                                // numShifts
    256,                                              // numChannels
    64,                                               // numPoints
    13,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 14))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector64Point,                               // shiftVector
    NULL,                                             // staticIn1
    3,                                                // numShifts
    512,                                              // numChannels
    64,                                               // numPoints
    14,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 15))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector128Point,                              // shiftVector
    NULL,                                             // staticIn1
    3,                                                // numShifts
    16,                                               // numChannels
    128,                                              // numPoints
    15,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 16))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector128Point,                              // shiftVector
    NULL,                                             // staticIn1
    3,                                                // numShifts
    32,                                               // numChannels
    128,                                              // numPoints
    16,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 17))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector128Point,                              // shiftVector
    NULL,                                             // staticIn1
    3,                                                // numShifts
    64,                                               // numChannels
    128,                                              // numPoints
    17,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 18))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector128Point,                              // shiftVector
    NULL,                                             // staticIn1
    3,                                                // numShifts
    128,                                              // numChannels
    128,                                              // numPoints
    18,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 19))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector128Point,                              // shiftVector
    NULL,                                             // staticIn1
    3,                                                // numShifts
    256,                                              // numChannels
    128,                                              // numPoints
    19,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 20))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector256Point,                              // shiftVector
    NULL,                                             // staticIn1
    4,                                                // numShifts
    8,                                                // numChannels
    256,                                              // numPoints
    20,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 21))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector256Point,                              // shiftVector
    NULL,                                             // staticIn1
    4,                                                // numShifts
    16,                                               // numChannels
    256,                                              // numPoints
    21,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 22))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector256Point,                              // shiftVector
    NULL,                                             // staticIn1
    4,                                                // numShifts
    32,                                               // numChannels
    256,                                              // numPoints
    22,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 23))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector256Point,                              // shiftVector
    NULL,                                             // staticIn1
    4,                                                // numShifts
    64,                                               // numChannels
    256,                                              // numPoints
    23,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 24))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector256Point,                              // shiftVector
    NULL,                                             // staticIn1
    4,                                                // numShifts
    128,                                              // numChannels
    256,                                              // numPoints
    24,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 25))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector512Point,                              // shiftVector
    NULL,                                             // staticIn1
    4,                                                // numShifts
    4,                                                // numChannels
    512,                                              // numPoints
    25,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 26))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector512Point,                              // shiftVector
    NULL,                                             // staticIn1
    4,                                                // numShifts
    8,                                                // numChannels
    512,                                              // numPoints
    26,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 27))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector512Point,                              // shiftVector
    NULL,                                             // staticIn1
    4,                                                // numShifts
    16,                                               // numChannels
    512,                                              // numPoints
    27,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 28))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector512Point,                              // shiftVector
    NULL,                                             // staticIn1
    4,                                                // numShifts
    32,                                               // numChannels
    512,                                              // numPoints
    28,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 29))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector512Point,                              // shiftVector
    NULL,                                             // staticIn1
    4,                                                // numShifts
    64,                                               // numChannels
    512,                                              // numPoints
    29,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 30))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector1024Point,                             // shiftVector
    NULL,                                             // staticIn1
    5,                                                // numShifts
    2,                                                // numChannels
    1024,                                             // numPoints
    30,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 31))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector1024Point,                             // shiftVector
    NULL,                                             // staticIn1
    5,                                                // numShifts
    4,                                                // numChannels
    1024,                                             // numPoints
    31,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 32))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector1024Point,                             // shiftVector
    NULL,                                             // staticIn1
    5,                                                // numShifts
    8,                                                // numChannels
    1024,                                             // numPoints
    32,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 33))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector1024Point,                             // shiftVector
    NULL,                                             // staticIn1
    5,                                                // numShifts
    16,                                               // numChannels
    1024,                                             // numPoints
    33,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 34))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector1024Point,                             // shiftVector
    NULL,                                             // staticIn1
    5,                                                // numShifts
    32,                                               // numChannels
    1024,                                             // numPoints
    34,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 35))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector2048Point,                             // shiftVector
    NULL,                                             // staticIn1
    5,                                                // numShifts
    1,                                                // numChannels
    2048,                                             // numPoints
    35,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 36))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector2048Point,                             // shiftVector
    NULL,                                             // staticIn1
    5,                                                // numShifts
    2,                                                // numChannels
    2048,                                             // numPoints
    36,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 37))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector2048Point,                             // shiftVector
    NULL,                                             // staticIn1
    5,                                                // numShifts
    4,                                                // numChannels
    2048,                                             // numPoints
    37,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 38))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector2048Point,                             // shiftVector
    NULL,                                             // staticIn1
    5,                                                // numShifts
    8,                                                // numChannels
    2048,                                             // numPoints
    38,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 39))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector2048Point,                             // shiftVector
    NULL,                                             // staticIn1
    5,                                                // numShifts
    16,                                               // numChannels
    2048,                                             // numPoints
    39,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 40))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector4096Point,                             // shiftVector
    NULL,                                             // staticIn1
    6,                                                // numShifts
    1,                                                // numChannels
    4096,                                             // numPoints
    40,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 41))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector4096Point,                             // shiftVector
    NULL,                                             // staticIn1
    6,                                                // numShifts
    2,                                                // numChannels
    4096,                                             // numPoints
    41,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 42))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector4096Point,                             // shiftVector
    NULL,                                             // staticIn1
    6,                                                // numShifts
    4,                                                // numChannels
    4096,                                             // numPoints
    42,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 43))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector4096Point,                             // shiftVector
    NULL,                                             // staticIn1
    6,                                                // numShifts
    8,                                                // numChannels
    4096,                                             // numPoints
    43,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 44))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector8192Point,                             // shiftVector
    NULL,                                             // staticIn1
    6,                                                // numShifts
    1,                                                // numChannels
    8192,                                             // numPoints
    44,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 45))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector8192Point,                             // shiftVector
    NULL,                                             // staticIn1
    6,                                                // numShifts
    2,                                                // numChannels
    8192,                                             // numPoints
    45,                                               // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 46))
   {
    RANDOM_SIGNED,                                    // test pattern
    NULL,                                             // staticIn0
    shiftVector8192Point,                             // shiftVector
    NULL,                                             // staticIn1
    6,                                                // numShifts
    4,                                                // numChannels
    8192,                                             // numPoints
    46,                                               // test ID
    },
#endif

};

/*
 *  Sends the test parameter structure and number of tests
 */
void
fft1dBatched_i16sc_c16sc_o16sc_getTestParams (fft1dBatched_i16sc_c16sc_o16sc_testParams_t ** params, int32_t * numTests)
{
   *params = testParams;
   *numTests = sizeof (testParams) / sizeof (fft1dBatched_i16sc_c16sc_o16sc_testParams_t);
}

/* ======================================================================== */
/*                       End of file                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2017 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
