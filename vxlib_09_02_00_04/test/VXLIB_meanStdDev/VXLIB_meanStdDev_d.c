/******************************************************************************
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

/**********************************************************************************************************************/
/*                                                                                                                    */
/* INCLUDES                                                                                                           */
/*                                                                                                                    */
/**********************************************************************************************************************/

#include "../common/VXLIB_test.h"
#include "TI_memory.h"
#include "TI_test.h"
#include "VXLIB_bufParams.h"
#include "VXLIB_meanStdDev.h"
#include "VXLIB_meanStdDev_idat.h"
#include "VXLIB_types.h"
#include <vxlib.h>

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Global variables                                                                                                   */
/*                                                                                                                    */
/**********************************************************************************************************************/

#ifdef WIN32
int8_t msmcBuffer[VXLIB_L3_RESULTS_BUFFER_SIZE];
int8_t ddrBuffer[2048 * 1024];
#else
__attribute__((section(".msmcData"), aligned(64))) int8_t msmcBuffer[VXLIB_L3_RESULTS_BUFFER_SIZE];
__attribute__((section(".ddrData"), aligned(64))) int8_t  ddrBuffer[2048 * 1024];

#endif // WIN32

int16_t volatile volatileSum0 = 0; // use volatile to keep compiler from removing this operation
int16_t volatile volatileSum1 = 0;

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_meanStdDev_d */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method performs regerssion testing for the VXLIB_meanStdDev kernel
int VXLIB_meanStdDev_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   int32_t  tpi;             // iteration variable for test input
   int32_t  currentTestFail; // status of current test case
   int32_t  fail = 0;        // pass/fail flag
   uint32_t repCount;        // iteration variable for repetition count
   uint32_t numReps;         // number of times to repeat test cases
   uint32_t testNum;         // test number

   // input and output buffer parameter variables
   VXLIB_bufParams2D_t bufParamsIn;

   uint32_t k = 0;

   // Unused for now; TBD: performance estimation
   // uint64_t archCycles, estCycles;

   // data structure to hold test cases parameters
   VXLIB_meanStdDev_testParams_t *prm;
   VXLIB_meanStdDev_testParams_t  currPrm;

   // get total number of test cases
   VXLIB_meanStdDev_getTestParams(&prm, &test_cases);

   // initialization args for kernel
   VXLIB_meanStdDev_InitArgs kerInitArgs;

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT  VXLIB_meanStdDev_d CP 0\n");
#endif

   // create kernel's handle
   int32_t            handleSize = VXLIB_meanStdDev_getHandleSize(&kerInitArgs);
   VXLIB_kernelHandle handle     = malloc(handleSize);

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT  VXLIB_meanStdDev_d CP 1\n");
#endif

   TI_profile_init("VXLIB_meanStdDev");

   // file IO for benchmarking
   FILE *fpOutputCSV = fopen("VXLIB_meanStdDev.csv", "w+");
   fprintf(fpOutputCSV, "Kernel, Test ID, dTypeIn, dTypeOut, width, height, Loki cycles\n");

   // iterate through test cases
   for (tpi = 0; tpi < test_cases; tpi++) {

      numReps = prm[tpi].numReps;
      testNum = prm[tpi].testID;
      currPrm = prm[tpi];

      for (repCount = 0; repCount < numReps; repCount++) {
         int32_t      status_nat_vs_opt = TI_TEST_KERNEL_FAIL;
         int32_t      status_ref_vs_opt = TI_TEST_KERNEL_FAIL;
         VXLIB_STATUS status_init       = VXLIB_SUCCESS;
         VXLIB_STATUS status_opt        = VXLIB_SUCCESS;
         VXLIB_STATUS status_nat        = VXLIB_SUCCESS;

         /* Sometimes, depending on certain compile flags, the test will be
          * marked PASS even if no comparison is done. This flag is to detect
          * if a comparison was done or not                                   */
         int32_t comparisonDone = 0;

         currentTestFail = 0;

         /*****************************************/
         /* Assign input output buffer parameters */
         /*****************************************/

         bufParamsIn.data_type = currPrm.dTypeIn;
         bufParamsIn.dim_x     = currPrm.width;
         bufParamsIn.dim_y     = currPrm.height;
         bufParamsIn.stride_y  = currPrm.strideIn;

         size_t inpSize = bufParamsIn.dim_y * bufParamsIn.stride_y;
         size_t outSize = sizeof(float);

         size_t paramSize = (bufParamsIn.data_type == VXLIB_UINT8) ? sizeof(uint32_t) : sizeof(uint64_t);

         /*****************************************/
         /* Create buffers for input and outputs  */
         /*****************************************/

         // input
         void *pIn              = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, inpSize);
         void *pPixelsProcessed = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, outSize);
         void *pCurrentSum      = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, paramSize);
         void *pCurrentSqSum    = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, paramSize);

         // input, output
         void *pPixelsProcessedCn = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, outSize);
         void *pCurrentSumCn      = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, paramSize);
         void *pCurrentSqSumCn    = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, paramSize);

         // output
         void *pOut0   = NULL;
         void *pOut1   = NULL;
         void *pOut0Cn = NULL;
         void *pOut1Cn = NULL;

         if (currPrm.outputDataLocation == VXLIB_TEST_OUTPUT_HEAP) {
            pOut0   = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, outSize);
            pOut1   = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, outSize);
            pOut0Cn = (void *) malloc(outSize);
            pOut1Cn = (void *) malloc(outSize);
         }
         else if (currPrm.outputDataLocation == VXLIB_TEST_OUTPUT_MSMC) {
            pOut0 = (void *) msmcBuffer;
            pOut1 = (void *) (msmcBuffer + sizeof(float));
            pOut0Cn = (void *) ddrBuffer;
            pOut1Cn = (void *) (ddrBuffer + sizeof(float));
         }

         // only run the test if the buffer allocations fit in the heap
         if (pIn && pPixelsProcessed && pCurrentSqSum && pCurrentSum && pOut0 && pOut1 && pOut0Cn && pOut1Cn) {

            // fill buffers
            TI_fillBuffer(prm[tpi].testPattern, 0, pIn, prm[tpi].staticIn, bufParamsIn.dim_x, bufParamsIn.dim_y,
                          bufParamsIn.stride_y, VXLIB_sizeof(currPrm.dTypeIn), testPatternString);

            TI_fillBuffer(prm[tpi].testPattern, 0, pPixelsProcessed, prm[tpi].staticPixelsProcessedIn, 1, 1,
                          sizeof(uint32_t), VXLIB_sizeof(VXLIB_UINT32), testPatternString);

            TI_fillBuffer(prm[tpi].testPattern, 0, pPixelsProcessedCn, prm[tpi].staticPixelsProcessedIn, 1, 1,
                          sizeof(uint32_t), VXLIB_sizeof(VXLIB_UINT32), testPatternString);

            if (bufParamsIn.data_type == VXLIB_UINT8) {

               TI_fillBuffer(prm[tpi].testPattern, 0, pCurrentSum, prm[tpi].staticCurrentSumIn, 1, 1, sizeof(uint32_t),
                             VXLIB_sizeof(VXLIB_UINT32), testPatternString);

               TI_fillBuffer(prm[tpi].testPattern, 0, pCurrentSqSum, prm[tpi].staticCurrentSqSumIn, 1, 1,
                             sizeof(uint32_t), VXLIB_sizeof(VXLIB_UINT32), testPatternString);

               TI_fillBuffer(prm[tpi].testPattern, 0, pCurrentSumCn, prm[tpi].staticCurrentSumIn, 1, 1,
                             sizeof(uint32_t), VXLIB_sizeof(VXLIB_UINT32), testPatternString);

               TI_fillBuffer(prm[tpi].testPattern, 0, pCurrentSqSumCn, prm[tpi].staticCurrentSqSumIn, 1, 1,
                             sizeof(uint32_t), VXLIB_sizeof(VXLIB_UINT32), testPatternString);
            }

            else {

               TI_fillBuffer(prm[tpi].testPattern, 0, pCurrentSum, prm[tpi].staticCurrentSumIn, 1, 1, sizeof(uint64_t),
                             VXLIB_sizeof(VXLIB_UINT64), testPatternString);

               TI_fillBuffer(prm[tpi].testPattern, 0, pCurrentSqSum, prm[tpi].staticCurrentSqSumIn, 1, 1,
                             sizeof(uint64_t), VXLIB_sizeof(VXLIB_UINT64), testPatternString);

               TI_fillBuffer(prm[tpi].testPattern, 0, pCurrentSumCn, prm[tpi].staticCurrentSumIn, 1, 1,
                             sizeof(uint64_t), VXLIB_sizeof(VXLIB_UINT64), testPatternString);

               TI_fillBuffer(prm[tpi].testPattern, 0, pCurrentSqSumCn, prm[tpi].staticCurrentSqSumIn, 1, 1,
                             sizeof(uint64_t), VXLIB_sizeof(VXLIB_UINT64), testPatternString);
            }

#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_meanStdDev_d CP 0\n");
#endif

#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_meanStdDev_d CP 1 status_init %d\n", status_init);
#endif
            // check parameters for initializing the kernel and initialize kernel if successful
            status_init = VXLIB_meanStdDev_init_checkParams(handle, &bufParamsIn, &kerInitArgs);
            if (status_init == VXLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               VXLIB_asm(" MARK 0");
               kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;
               status_init           = VXLIB_meanStdDev_init(handle, &bufParamsIn, &kerInitArgs);
               VXLIB_asm(" MARK 1");
               TI_profile_stop();
            }
            else {
               printf("status_init failed: %d", status_init);
            }

            // check execution parameters and call optimized kernel if sucessful
            status_opt = VXLIB_meanStdDev_exec_checkParams(handle, pIn, pOut0, pOut1, pPixelsProcessed, pCurrentSum,
                                                           pCurrentSqSum);
            if (status_opt == VXLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_OPT);
               VXLIB_asm(" MARK 2");
               status_opt =
                   VXLIB_meanStdDev_exec(handle, pIn, pOut0, pOut1, pPixelsProcessed, pCurrentSum, pCurrentSqSum);
               VXLIB_asm(" MARK 3");
               TI_profile_stop();
            }
            else {
               printf("status_opt failed: %d", status_opt);
            }

            // the following for loop is to call kernel repeatedly so as to train the branch predictor
            for (k = 0; k < (VXLIB_WARM_REPS); k++) {
               // run warm instruction cache test
               TI_profile_clear_cycle_count_single(TI_PROFILE_KERNEL_OPT_WARM);
               TI_profile_start(TI_PROFILE_KERNEL_OPT_WARM);
               VXLIB_asm(" MARK 4");
               status_opt =
                   VXLIB_meanStdDev_exec(handle, pIn, pOut0, pOut1, pPixelsProcessed, pCurrentSum, pCurrentSqSum);
               VXLIB_asm(" MARK 5");
               TI_profile_stop();

               // reset buffers
               *((uint32_t *) pPixelsProcessed) = *((uint32_t *) currPrm.staticPixelsProcessedIn);
               if (bufParamsIn.data_type == VXLIB_UINT8) {
                  *((uint32_t *) pCurrentSum)   = *((uint32_t *) currPrm.staticCurrentSumIn);
                  *((uint32_t *) pCurrentSqSum) = *((uint32_t *) currPrm.staticCurrentSqSumIn);
               }

               else {
                  *((uint64_t *) pCurrentSum)   = *((uint64_t *) currPrm.staticCurrentSumIn);
                  *((uint64_t *) pCurrentSqSum) = *((uint64_t *) currPrm.staticCurrentSqSumIn);
               }
            }

            // get output to L1D
            int16_t outSum0   = 0;
            int16_t outSum1   = 0;
            int8_t *pOutTemp0 = (int8_t *) pOut0;
            int8_t *pOutTemp1 = (int8_t *) pOut1; // treat output as bytes to be data type agnostic
            for (k = 0; k < 1; k++) {
               outSum0 += *pOutTemp0;
               pOutTemp0++;
               outSum1 += *pOutTemp1;
               pOutTemp1++;
            }

            // dummy store of outSum to insure that the compiler does not remove it.
            volatileSum0 = outSum0;
            volatileSum1 = outSum1;

            // run warm instruction cache test
            TI_profile_start(TI_PROFILE_KERNEL_OPT_WARMWRB);

            VXLIB_asm(" MARK 6");
            status_opt = VXLIB_meanStdDev_exec(handle, pIn, pOut0, pOut1, pPixelsProcessed, pCurrentSum, pCurrentSqSum);
            VXLIB_asm(" MARK 7");
            TI_profile_stop();

            /**************************************/
            /* Test natural C code for the kernel */
            /**************************************/

            // initialize the kernel function style to natural C
            kerInitArgs.funcStyle = VXLIB_FUNCTION_NATC;

            // call init once again to use the natural C variant
            VXLIB_meanStdDev_init(handle, &bufParamsIn, &kerInitArgs);

            TI_profile_start(TI_PROFILE_KERNEL_CN);
            VXLIB_asm(" MARK 8");
            status_nat = VXLIB_meanStdDev_exec(handle, pIn, pOut0Cn, pOut1Cn, pPixelsProcessedCn, pCurrentSumCn,
                                               pCurrentSqSumCn);
            VXLIB_asm(" MARK 9");
            TI_profile_stop();
#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_meanStdDev_d CP 6 status_nat %d\n", status_nat);
#endif

            /*********************************************************************/
            /* Compare results with natural C output and static reference output */
            /*********************************************************************/
            int32_t status_nat_vs_opt0, status_nat_vs_opt1, status_nat_vs_opt2, status_nat_vs_opt3, status_nat_vs_opt4;

            status_nat_vs_opt0 = TI_compare_mem_2D_float((void *) pOut0, (void *) pOut0Cn, 0.001, (double) powf(2, -10),
                                                         1, 1, 0, VXLIB_sizeof(currPrm.dTypeOut));

            status_nat_vs_opt1 = TI_compare_mem_2D_float((void *) pOut1, (void *) pOut1Cn, 0.001, (double) powf(2, -10),
                                                         1, 1, 0, VXLIB_sizeof(currPrm.dTypeOut));

            status_nat_vs_opt2 = TI_compare_mem_2D((void *) pPixelsProcessed, (void *) pPixelsProcessedCn, 0, 0, 1, 1,
                                                   0, VXLIB_sizeof(VXLIB_UINT32));

            if (bufParamsIn.data_type == VXLIB_UINT8) {

               status_nat_vs_opt3 = TI_compare_mem_2D((void *) pCurrentSum, (void *) pCurrentSumCn, 0, 0, 1, 1, 0,
                                                      VXLIB_sizeof(VXLIB_UINT32));

               status_nat_vs_opt4 = TI_compare_mem_2D((void *) pCurrentSqSum, (void *) pCurrentSqSumCn, 0, 0, 1, 1, 0,
                                                      VXLIB_sizeof(VXLIB_UINT32));
            }

            else {

               status_nat_vs_opt3 = TI_compare_mem_2D((void *) pCurrentSum, (void *) pCurrentSumCn, 0, 0, 1, 1, 0,
                                                      VXLIB_sizeof(VXLIB_UINT64));

               status_nat_vs_opt4 = TI_compare_mem_2D((void *) pCurrentSqSum, (void *) pCurrentSqSumCn, 0, 0, 1, 1, 0,
                                                      VXLIB_sizeof(VXLIB_UINT64));
            }

            status_nat_vs_opt =
                status_nat_vs_opt0 & status_nat_vs_opt1 & status_nat_vs_opt2 & status_nat_vs_opt3 & status_nat_vs_opt4;

            comparisonDone = 1;
#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_meanStdDev_d CP 7 comparisonDone %d status_nat_vs_opt %d\n", comparisonDone,
                   status_nat_vs_opt);
#endif
            int32_t status_ref_vs_opt0, status_ref_vs_opt1, status_ref_vs_opt2, status_ref_vs_opt3, status_ref_vs_opt4;

            if ((currPrm.staticOut0 != NULL) && (currPrm.staticOut1 != NULL)) {
               status_ref_vs_opt0 =
                   TI_compare_mem_2D_float((void *) pOut0, (void *) currPrm.staticOut0, 0.001, (double) powf(2, -10), 1,
                                           1, 0, VXLIB_sizeof(currPrm.dTypeOut));
               status_ref_vs_opt1 =
                   TI_compare_mem_2D_float((void *) pOut1, (void *) currPrm.staticOut1, 0.001, (double) powf(2, -10), 1,
                                           1, 0, VXLIB_sizeof(currPrm.dTypeOut));
               status_ref_vs_opt2 =
                   TI_compare_mem_2D((void *) pPixelsProcessed, (void *) currPrm.staticPixelsProcessedOut, 0, 0, 1, 1,
                                     0, VXLIB_sizeof(VXLIB_UINT32));

               if (bufParamsIn.data_type == VXLIB_UINT8) {

                  status_ref_vs_opt3 = TI_compare_mem_2D((void *) pCurrentSum, (void *) currPrm.staticCurrentSumOut, 0,
                                                         0, 1, 1, 0, VXLIB_sizeof(VXLIB_UINT32));
                  status_ref_vs_opt4 = TI_compare_mem_2D((void *) pCurrentSqSum, (void *) currPrm.staticCurrentSqSumOut,
                                                         0, 0, 1, 1, 0, VXLIB_sizeof(VXLIB_UINT32));
               }

               else {
                  status_ref_vs_opt3 = TI_compare_mem_2D((void *) pCurrentSum, (void *) currPrm.staticCurrentSumOut, 0,
                                                         0, 1, 1, 0, VXLIB_sizeof(VXLIB_UINT64));
                  status_ref_vs_opt4 = TI_compare_mem_2D((void *) pCurrentSqSum, (void *) currPrm.staticCurrentSqSumOut,
                                                         0, 0, 1, 1, 0, VXLIB_sizeof(VXLIB_UINT64));
               }

               // status of all ref vs opt comparisons
               status_ref_vs_opt = status_ref_vs_opt0 & status_ref_vs_opt1 & status_ref_vs_opt2 & status_ref_vs_opt3 &
                                   status_ref_vs_opt4;
               comparisonDone = 1;
            }
            else {
               // set to pass since it wasn't supposed to run.
               status_ref_vs_opt = TI_TEST_KERNEL_PASS;
            }

            // set the 'fail' flag based on test vector comparison results
            currentTestFail =
                ((status_nat_vs_opt == TI_TEST_KERNEL_FAIL) || (status_ref_vs_opt == TI_TEST_KERNEL_FAIL) ||
                 (status_init != VXLIB_SUCCESS) || (status_opt != VXLIB_SUCCESS) || (status_nat != VXLIB_SUCCESS) ||
                 (comparisonDone == 0) || (currentTestFail == 1))
                    ? 1
                    : 0;

            fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;
#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_meanStdDev_d CP 8 fail %d\n", fail);
#endif

            // update profiling information
            pProfile[3 * tpi]     = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
            pProfile[3 * tpi + 1] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[3 * tpi + 2] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);

            sprintf(desc, "%s generated input | width = %ld, height = %ld", testPatternString, currPrm.width,
                    currPrm.height);

            // write to CSV, must happen prior to write to screen because TI_profile_formula_add clears values in
            // counters
            fprintf(fpOutputCSV, "meanStdDev, %d, %d, %d, %lu, %lu, %lu\n", testNum,
                    VXLIB_sizeof(bufParamsIn.data_type) * 8, VXLIB_sizeof(VXLIB_FLOAT32) * 8, currPrm.width,
                    currPrm.height, cycles[TI_PROFILE_KERNEL_OPT]);

            TI_profile_add_test(testNum++, currPrm.width, currPrm.height, 0, currentTestFail, desc);
         }
         else {
            sprintf(desc, "width =%ld, height = %ld", currPrm.width, currPrm.height);
            TI_profile_skip_test(desc);

         } // end of memory allocation successful?

         /*************************************/
         /* Free buffers for each test vector */
         /*************************************/

         if (currPrm.outputDataLocation == VXLIB_TEST_OUTPUT_HEAP) {
            free(pOut1Cn);
            free(pOut0Cn);
            TI_align_free(pOut1);
            TI_align_free(pOut0);
         }

         TI_align_free(pCurrentSqSumCn);
         TI_align_free(pCurrentSumCn);
         TI_align_free(pPixelsProcessedCn);

         TI_align_free(pCurrentSqSum);
         TI_align_free(pCurrentSum);
         TI_align_free(pPixelsProcessed);

         TI_align_free(pIn);

      } // end repetitions
   }    // end idat test cases

   free(handle);

   /* Close results CSV */
   fclose(fpOutputCSV);

   return fail;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* test_main                                                                                                          */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method calls the testbench function
int test_main(uint32_t *pProfile)
{
#if !defined(_HOST_BUILD)
   if (TI_cache_init()) {
      TI_memError("VXLIB_meanStdDev");
      return 1;
   }
   else
#else
   printf("_HOST_BUILD is defined.\n");
#endif
   {
      return VXLIB_meanStdDev_d(&pProfile[0], 0);
   }
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* main                                                                                                               */
/*                                                                                                                    */
/**********************************************************************************************************************/

#if !defined(__ONESHOTTEST)
int main()
{
   int fail = 1;

   uint32_t profile[256 * 3];

   VXLIB_TEST_init();

   fail = test_main(&profile[0]);

#if !defined(NO_PRINTF)
   if (fail == 0)
      printf("Test Pass!\n");
   else
      printf("Test Fail!\n");

   int i;
   for (i = 0; i < test_cases; i++) {
      printf("Test %4d: Cold Cycles = %8d, Warm Cycles = %8d, Warm Cycles WRB = %8d\n", i, profile[3 * i],
             profile[3 * i + 1] / (VXLIB_WARM_REPS), profile[3 * i + 2]);
   }
#endif

   return fail;
}
#endif
