/*****************************************************************************
 * Copyright (C) 2023 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Includes                                                                   */
/*                                                                            */
/******************************************************************************/

#include "MATHLIB_memory.h"
#include "MATHLIB_profile.h"
#include "MATHLIB_sqrt/MATHLIB_sqrt_scalar.h"
#include "MATHLIB_sqrt_testParams.h"
#include "MATHLIB_types.h"
#include <c7x.h>
#include <cstdint>
#include <mathlib.h>
#include <stdio.h>

/******************************************************************************/
/*                                                                            */
/* Defines                                                                    */
/*                                                                            */
/******************************************************************************/

#ifdef WIN32
int8_t msmcBuffer[MATHLIB_L3_RESULTS_BUFFER_SIZE];
int8_t ddrBuffer[2048 * 1024];
#else
__attribute__((section(".msmcData"), aligned(64))) int8_t msmcBuffer[MATHLIB_L3_RESULTS_BUFFER_SIZE];
__attribute__((section(".ddrData"), aligned(64))) int8_t  ddrBuffer[2048 * 1024];

#endif // WIN32

/******************************************************************************/
/*                                                                            */
/* MATHLIB_sqrt_runTest                                                         */
/*                                                                            */
/******************************************************************************/

// this method tests the kernel for a given test case
template <typename T>
int32_t MATHLIB_sqrt_runTest(MATHLIB_sqrt_testParams_t *pCurrPrm, size_t length, T *pSrc, T *pDst, uint32_t *pProfile)
{

   // variables
   uint32_t           testNum;                                 // test number
   uint64_t           archCycles = 0;                          // theoretical maximum for C7x
   uint64_t           estCycles  = 0;                          // estimated cycle count based on asm report
   int32_t            status     = MATHLIB_TEST_KERNEL_FAIL;   // pass or fail flag for test
   MATHLIB_TestMetric testMetric = MATHLIB_ABSOLUTE_TOLERANCE; // tolerance type

   // assign test case number to testID
   testNum = pCurrPrm->testID;

   // create buffers in L2SRAM for test case
   pSrc = (T *) MATHLIB_memalign(MATHLIB_L2DATA_ALIGNMENT, length * sizeof(T));
   pDst = (T *) MATHLIB_memalign(MATHLIB_L2DATA_ALIGNMENT, length * sizeof(T));

   // fill buffer with test case data from DDR
   MATHLIB_fillBuffer((T *) pCurrPrm->staticIn, pSrc, length);

   // cold run
   MATHLIB_profile_start(MATHLIB_PROFILE_KERNEL_OPT_COLD);
   MATHLIB_sqrt(length, pSrc, pDst);
   MATHLIB_profile_stop();

   // warm run
   MATHLIB_profile_start(MATHLIB_PROFILE_KERNEL_OPT_WARM);
   MATHLIB_sqrt(length, pSrc, pDst);
   MATHLIB_profile_stop();

   // sanity check
   status = MATHLIB_compare_mem(pDst, (T *) pCurrPrm->staticOut, length, 10e-5, testMetric);

   // free L2SRAM memory
   MATHLIB_align_free(pSrc);
   MATHLIB_align_free(pDst);

   // print test case results
   sprintf(desc, "dataType=%d, length=%ld", pCurrPrm->dType, pCurrPrm->length);
   MATHLIB_profile_add_test(testNum++, length, archCycles, estCycles, status, desc);

   return status;
}

/******************************************************************************/
/*                                                                            */
/* MATHLIB_sqrt_testBench                                                       */
/*                                                                            */
/******************************************************************************/

// this method performs regression testing for the MATHLIB_sqrt kernel
int MATHLIB_sqrt_testBench(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   // variables to keep track of test cases
   size_t                     tpi = 0; // test case iteration variable
   MATHLIB_sqrt_testParams_t *prm;     // pointer to test params struct
   MATHLIB_sqrt_testParams_t  currPrm; // current test case

   // variables to track test cases status
   int32_t status          = MATHLIB_TEST_KERNEL_FAIL; // fail or pass flag
   int32_t currentTestFail = 0;                        // status of current test case
   int32_t fail            = 0;                        // track number failing test cases

   // kernel-specific variables
   size_t length = 0;    // vector length
   void  *pSrc   = NULL; // pointer to input vector0; assigned later
   void  *pDst   = NULL; // pointer to output vector; assigned later

   // get total number of test case
   MATHLIB_sqrt_getTestParams(&prm, &test_cases);

   // name of profile
   MATHLIB_profile_init((char *) "MATHLIB_sqrt");

   // loop through all test cases
   for (tpi = 0; tpi < (size_t) test_cases; tpi++) {

      currPrm = prm[tpi];
      length  = currPrm.length;

      // determine datatype of test case and call function to run test case
      if (currPrm.dType == MATHLIB_FLOAT32) {

         status = MATHLIB_sqrt_runTest(&currPrm, length, (float *) pSrc, (float *) pDst, &pProfile[0]);
      }

      // update profiling information
      pProfile[MATHLIB_PROFILE_TEST_TYPES * tpi] =
          (int32_t) MATHLIB_profile_get_cycles(MATHLIB_PROFILE_KERNEL_OPT_COLD);
      pProfile[MATHLIB_PROFILE_TEST_TYPES * tpi + 1] =
          (int32_t) MATHLIB_profile_get_cycles(MATHLIB_PROFILE_KERNEL_OPT_WARM);

      // update status of current test case and updated fail counter appropriately
      currentTestFail = (status == MATHLIB_TEST_KERNEL_FAIL) ? 1 : 0;
      fail += currentTestFail;
   }

   return fail;
}

int coverage_test_main(uint32_t *pProfile)
{
   int32_t                   testNum           = 1000;
   int32_t                   currentTestPass   = 0;
   MATHLIB_STATUS            status;
   int                       fail              = 0;
   size_t length = 16;
    
   float * pSrc = (float *) MATHLIB_memalign(MATHLIB_L2DATA_ALIGNMENT, length * sizeof(float));
   float * pDst = (float *) MATHLIB_memalign(MATHLIB_L2DATA_ALIGNMENT, length * sizeof(float));
 
     while (testNum <=1003){
   switch (testNum) {
      case 1000:
         status = MATHLIB_sqrt(16, (float *) NULL, pDst);
         currentTestPass        = (status == MATHLIB_ERR_NULL_POINTER);
         break;
      case 1001:
         status = MATHLIB_sqrt(16, pSrc, (float *) NULL);
         currentTestPass        = (status == MATHLIB_ERR_NULL_POINTER);
         break;
      case 1002:
         status = MATHLIB_sqrt(0, pSrc, pDst);
         currentTestPass        = (status == MATHLIB_ERR_INVALID_DIMENSION);
         break;
      case 1003:
         status = MATHLIB_sqrt_sp(16, pSrc, pDst);
         currentTestPass        = (status == MATHLIB_SUCCESS);
         break;
      default:
         break;
   }
   
   fail = ((fail == 1) || (currentTestPass == 0)) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   MATHLIB_profile_add_test(testNum++, 0, 0, 0, currentTestPass, desc);
 
   }
   MATHLIB_align_free(pSrc);
   MATHLIB_align_free(pDst);
   return fail;
}

/******************************************************************************/
/*                                                                            */
/* main                                                                       */
/*                                                                            */
/******************************************************************************/

int main(void)
{
   // variables
   uint32_t profile[MATHLIB_PROFILE_MAX_TESTS * MATHLIB_PROFILE_TEST_TYPES]; // collect profiling data for test cases
   int      fail = 0;                                                        // track the number of failed test cases

   MATHLIB_test_init();

   // call test bench to run the regression test
   fail = MATHLIB_sqrt_testBench(&profile[0], 0);

   // check if regression passed or failed
   if (fail == 0) {
      printf("Test Pass!\n");
   }
   else {
      printf("Test Fail! %d tests fail\n", fail);
   }

   fail = coverage_test_main(&profile[0]);
   if (fail == 0)
      printf("Test Pass!\n");
   else
      printf("Test Fail!\n");
   return fail;
   
}
