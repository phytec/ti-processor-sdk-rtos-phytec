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


#include "FFTLIB_ifft1d_i32fc_c32fc_o32fc_idat.h"
/* #include "../../common/TI_memory.h" */


#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 1))
#include "test_data/extended_test_cases/staticRefCase1.h"
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 2))
#include "test_data/extended_test_cases/staticRefCase2.h"
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 3))
#include "test_data/extended_test_cases/staticRefCase3.h"
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 4))
#include "test_data/extended_test_cases/staticRefCase4.h"
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 5))
#include "test_data/extended_test_cases/staticRefCase5.h"
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 6))
#include "test_data/extended_test_cases/staticRefCase6.h"
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 7))
#include "test_data/extended_test_cases/staticRefCase7.h"
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 8))
#include "test_data/extended_test_cases/staticRefCase8.h"
#endif

static ifft1d_i32fc_c32fc_o32fc_testParams_t testParams[] = {
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 1))
   {
    STATIC,                                           // test pattern
    staticRefIn1,                                     // staticIn0
    staticRefOut1,                                    // staticOut
    64,                                               // numPoints
    1,                                                // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 2))
   {
    STATIC,                                           // test pattern
    staticRefIn2,                                     // staticIn0
    staticRefOut2,                                    // staticOut
    128,                                              // numPoints
    2,                                                // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 3))
   {
    STATIC,                                           // test pattern
    staticRefIn3,                                     // staticIn0
    staticRefOut3,                                    // staticOut
    256,                                              // numPoints
    3,                                                // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 4))
   {
    STATIC,                                           // test pattern
    staticRefIn4,                                     // staticIn0
    staticRefOut4,                                    // staticOut
    512,                                              // numPoints
    4,                                                // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 5))
   {
    STATIC,                                           // test pattern
    staticRefIn5,                                     // staticIn0
    staticRefOut5,                                    // staticOut
    1024,                                             // numPoints
    5,                                                // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 6))
   {
    STATIC,                                           // test pattern
    staticRefIn6,                                     // staticIn0
    staticRefOut6,                                    // staticOut
    2048,                                             // numPoints
    6,                                                // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 7))
   {
    STATIC,                                           // test pattern
    staticRefIn7,                                     // staticIn0
    staticRefOut7,                                    // staticOut
    4096,                                             // numPoints
    7,                                                // test ID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 200) || (TEST_CATEGORY == 100) || (TEST_CASE == 8))
   {
    STATIC,                                           // test pattern
    staticRefIn8,                                     // staticIn0
    staticRefOut8,                                    // staticOut
    8192,                                             // numPoints
    8,                                                // test ID
    },
#endif

};

/*
 *  Sends the test parameter structure and number of tests
 */
void
ifft1d_i32fc_c32fc_o32fc_getTestParams (ifft1d_i32fc_c32fc_o32fc_testParams_t ** params, int32_t * numTests)
{
   *params = testParams;
   *numTests = sizeof (testParams) / sizeof (ifft1d_i32fc_c32fc_o32fc_testParams_t);
}

/* ======================================================================== */
/*                       End of file                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2017 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
