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

#include "MATHLIB_lut.h"
#include "MATHLIB_memory.h"
#include "MATHLIB_pow_testParams.h"
#include "MATHLIB_profile.h"
#include "MATHLIB_types.h"
#include <c7x.h>
#include <cstdint>
#include <mathlib.h>
#include <stdio.h>
#include <cfloat>

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
/* MATHLIB_pow_runTest                                                         */
/*                                                                            */
/******************************************************************************/

// this method tests the kernel for a given test case
template <typename T>
int32_t
MATHLIB_pow_runTest(MATHLIB_pow_testParams_t *pCurrPrm, size_t length, T *pSrc0, T *pSrc1, T *pDst, uint32_t *pProfile)
{

   // variables
   uint32_t           testNum;                               // test number
   uint64_t           archCycles = 0;                        // theoretical maximum for C7x
   uint64_t           estCycles  = 0;                        // estimated cycle count based on asm report
   int32_t            status     = MATHLIB_TEST_KERNEL_FAIL; // pass or fail flag for test
   MATHLIB_TestMetric testMetric = MATHLIB_PERCENT_ERROR;    // tolerance type

   // assign test case number to testID
   testNum = pCurrPrm->testID;

   // create buffers in L2SRAM for test case
   pSrc0 = (T *) MATHLIB_memalign(MATHLIB_L2DATA_ALIGNMENT, length * sizeof(T));
   pSrc1 = (T *) MATHLIB_memalign(MATHLIB_L2DATA_ALIGNMENT, length * sizeof(T));
   pDst  = (T *) MATHLIB_memalign(MATHLIB_L2DATA_ALIGNMENT, length * sizeof(T));

   // fill buffer with test case data from DDR
   MATHLIB_fillBuffer((T *) pCurrPrm->staticIn0, pSrc0, length);
   MATHLIB_fillBuffer((T *) pCurrPrm->staticIn1, pSrc1, length);

   // cold run
   MATHLIB_profile_start(MATHLIB_PROFILE_KERNEL_OPT_COLD);
   MATHLIB_pow(length, pSrc0, pSrc1, pDst);
   MATHLIB_profile_stop();

   // warm run
   MATHLIB_profile_start(MATHLIB_PROFILE_KERNEL_OPT_WARM);
   MATHLIB_pow(length, pSrc0, pSrc1, pDst);
   MATHLIB_profile_stop();

   // sanity check
   status = MATHLIB_compare_mem(pDst, (T *) pCurrPrm->staticOut, length, MATHLIB_POW_PERCENT_ERROR, testMetric);

   // free L2SRAM memory
   MATHLIB_align_free(pSrc0);
   MATHLIB_align_free(pSrc1);
   MATHLIB_align_free(pDst);

   // print test case results
   sprintf(desc, "dataType=%d, length=%ld", pCurrPrm->dType, pCurrPrm->length);
   MATHLIB_profile_add_test(testNum++, length, archCycles, estCycles, status, desc);

   return status;
}

/******************************************************************************/
/*                                                                            */
/* MATHLIB_pow_testBench                                                       */
/*                                                                            */
/******************************************************************************/

// this method performs regression testing for the MATHLIB_exp kernel
int MATHLIB_pow_testBench(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   // variables to keep track of test cases
   size_t                    tpi = 0; // test case iteration variable
   MATHLIB_pow_testParams_t *prm;     // pointer to test params struct
   MATHLIB_pow_testParams_t  currPrm; // current test case

   // variables to track test cases status
   int32_t status          = MATHLIB_TEST_KERNEL_FAIL; // fail or pass flag
   int32_t currentTestFail = 0;                        // status of current test case
   int32_t fail            = 0;                        // track number failing test cases

   // kernel-specific variables
   size_t length = 0;    // vector length
   void  *pSrc0  = NULL; // pointer to input vector0; assigned later
   void  *pSrc1  = NULL; // pointer to input vector1; assigned later
   void  *pDst   = NULL; // pointer to output vector; assigned later

   // get total number of test case
   MATHLIB_pow_getTestParams(&prm, &test_cases);

   // name of profile
   MATHLIB_profile_init((char *) "MATHLIB_pow");

   // initialize LUT
   MATHLIB_LUTInit();

   // loop through all test cases
   for (tpi = 0; tpi < (size_t) test_cases; tpi++) {

      currPrm = prm[tpi];
      length  = currPrm.length;

      // determine datatype of test case and call function to run test case
      if (currPrm.dType == MATHLIB_FLOAT32) {

         status = MATHLIB_pow_runTest(&currPrm, length, (float *) pSrc0, (float *) pSrc1, (float *) pDst, &pProfile[0]);
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
   
   float * pSrc0 = (float *) MATHLIB_memalign(MATHLIB_L2DATA_ALIGNMENT, length * sizeof(float));
   float * pSrc1 = (float *) MATHLIB_memalign(MATHLIB_L2DATA_ALIGNMENT, length * sizeof(float));
   float * pDst = (float *) MATHLIB_memalign(MATHLIB_L2DATA_ALIGNMENT, length * sizeof(float));
   
   while (testNum <=1011){
   switch (testNum) {
      case 1000:
         status = MATHLIB_pow(16, (float *) NULL, pSrc1, pDst);
         currentTestPass        = (status == MATHLIB_ERR_NULL_POINTER);
         break;
      case 1001:
         status = MATHLIB_pow(16, pSrc0, (float *) NULL, pDst);
         currentTestPass        = (status == MATHLIB_ERR_NULL_POINTER);
         break;
      case 1002:
         status = MATHLIB_pow(16, pSrc0, pSrc1, (float *) NULL);
         currentTestPass        = (status == MATHLIB_ERR_NULL_POINTER);
         break;
      case 1003:
         status = MATHLIB_pow(0, pSrc0, pSrc1, pDst);
         currentTestPass        = (status == MATHLIB_ERR_INVALID_DIMENSION);
         break;
      case 1004:
         pSrc0[0] = FLT_MAX;
         pSrc1[0] = FLT_MAX;
         status = MATHLIB_pow_sp(1, pSrc0, pSrc1, pDst);
         currentTestPass        = (status == MATHLIB_SUCCESS);
      case 1005:
         pSrc0[0] = -1.0;
         pSrc1[0] = 5.0;
         status = MATHLIB_pow_sp(1, pSrc0, pSrc1, pDst);
         currentTestPass        = (status == MATHLIB_SUCCESS);
         break; 
      case 1006:
         pSrc0[0] = -1.0;
         pSrc1[0] = 5.660;
         status = MATHLIB_pow_sp(1, pSrc0, pSrc1, pDst);
         currentTestPass        = (status == MATHLIB_SUCCESS);
         break; 
      case 1007:
         pSrc0[0] = 0.0;
         pSrc1[0] = 1.0;
         status = MATHLIB_pow_sp(1, pSrc0, pSrc1, pDst);
         currentTestPass        = (status == MATHLIB_SUCCESS);
         break;
      case 1008:
         pSrc0[0] = 0.0;
         pSrc1[0] = -1.0;
         status = MATHLIB_pow_sp(1, pSrc0, pSrc1, pDst);
         currentTestPass        = (status == MATHLIB_SUCCESS);
         break;
      case 1009:
         pSrc0[0] = -88.33654475f;
         pSrc1[0] = -88.33654475f;
         status = MATHLIB_pow_sp(1, pSrc0, pSrc1, pDst);
         currentTestPass        = (status == MATHLIB_SUCCESS);
         break;
      case 1010:
         pSrc0[0] = -1.0;
         pSrc1[0] = 6.0;
         status = MATHLIB_pow_sp(1, pSrc0, pSrc1, pDst);
         currentTestPass        = (status == MATHLIB_SUCCESS);
         break;
      case 1011:
         pSrc0[0] = -1.0;
         pSrc1[0] = 0.0;
         status = MATHLIB_pow_sp(1, pSrc0, pSrc1, pDst);
         currentTestPass        = (status == MATHLIB_SUCCESS);
         break;
      default:
         break;
   }
   
   fail = ((fail == 1) || (currentTestPass == 0)) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   MATHLIB_profile_add_test(testNum++, 0, 0, 0, currentTestPass, desc);
 
   }
   MATHLIB_align_free(pSrc0);
   MATHLIB_align_free(pSrc1);
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
   fail = MATHLIB_pow_testBench(&profile[0], 0);

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
