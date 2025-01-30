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
#include "VXLIB_median.h"
#include "VXLIB_median_idat.h"
#include "VXLIB_types.h"
#include <vxlib.h>

#define DSPLIB_ALIGN_SHIFT_64BYTES 6

#define VXLIB_CALC_STRIDE(BYTES, ALIGN_SHIFT) (((((BYTES) -1) >> (ALIGN_SHIFT)) + 1) << (ALIGN_SHIFT))

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

int16_t volatile volatileSum = 0; // use volatile to keep compiler from removing this operation

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_median_d */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method performs regerssion testing for the VXLIB_median kernel
int VXLIB_median_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   int32_t  tpi;             // iteration variable for test input
   int32_t  currentTestFail; // status of current test case
   int32_t  fail = 0;        // pass/fail flag
   uint32_t repCount;        // iteration variable for repetition count
   uint32_t numReps;         // number of times to repeat test cases
   uint32_t testNum;         // test number

   // input and output buffer parameter variables
   VXLIB_bufParams2D_t bufParamsIn;
   VXLIB_bufParams2D_t bufParamsMask;
   VXLIB_bufParams2D_t bufParamsOut;
   VXLIB_bufParams2D_t bufParamsScratch;

#if defined(EVM_TEST)
   uint32_t k = 0;
#endif

   // performance estimation
   uint64_t archCycles, estCycles;

   // data structure to hold test cases parameters
   VXLIB_median_testParams_t *prm;
   VXLIB_median_testParams_t  currPrm;

   // get total number of test cases
   VXLIB_median_getTestParams(&prm, &test_cases);

   // initialization args for kernel
   VXLIB_median_InitArgs kerInitArgs;

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT  VXLIB_median_d CP 0\n");
#endif

   // create kernel's handle
   int32_t            handleSize = VXLIB_median_getHandleSize(&kerInitArgs);
   VXLIB_kernelHandle handle     = malloc(handleSize);

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT  VXLIB_median_d CP 1\n");
#endif

   TI_profile_init("VXLIB_median");

#if !defined(EVM_TEST)
   // file IO for benchmarking
   //  FILE *fpOutputCSV = fopen("VXLIB_median.csv", "w+");
   //  fprintf(fpOutputCSV, "Kernel, Test ID, Test type, Bit Width, Parameters, width, height, Arch"
   //                       "cycles, Loki cycles, Loki/Arch cycles, Pass/Fail\n");
#endif // #if defined(EVM_TEST)

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

         bufParamsIn.data_type   = currPrm.dTypeIn;
         bufParamsIn.dim_x       = currPrm.width;
         bufParamsIn.dim_y       = currPrm.height;
         int32_t  memory_padding = 0;
         uint64_t strideIn =
             VXLIB_CALC_STRIDE(bufParamsIn.dim_x * VXLIB_sizeof(bufParamsIn.data_type), VXLIB_ALIGN_SHIFT_64BYTES);
         if (strideIn % 512 == 0) {
            strideIn += memory_padding;
         }
         //  bufParamsIn.stride_y = currPrm.strideIn;
         bufParamsIn.stride_y = strideIn;

         bufParamsMask.data_type = currPrm.dTypeMask;
         bufParamsMask.dim_x     = currPrm.N;
         bufParamsMask.dim_y     = currPrm.M;
         bufParamsMask.stride_y  = currPrm.strideMask;

         bufParamsOut.data_type = currPrm.dTypeOut;
         bufParamsOut.dim_x     = currPrm.width - (currPrm.N - 1);
         bufParamsOut.dim_y     = currPrm.height - (currPrm.M - 1);
         uint64_t strideOut =
             VXLIB_CALC_STRIDE(bufParamsOut.dim_x * VXLIB_sizeof(bufParamsOut.data_type), VXLIB_ALIGN_SHIFT_64BYTES);
         //  bufParamsOut.stride_y  = currPrm.strideOut;
         if (strideOut % 512 == 0) {
            strideOut += memory_padding;
         }
         bufParamsOut.stride_y = strideOut;

         /*****************************************/
         /* Create buffers for input and outputs  */
         /*****************************************/

         size_t inp0Size = bufParamsIn.dim_y * bufParamsIn.stride_y;
         size_t inp1Size = bufParamsMask.dim_y * bufParamsMask.stride_y;
         size_t outSize  = bufParamsOut.dim_y * bufParamsOut.stride_y;

         void *pIn   = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, inp0Size);
         void *pMask = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, inp1Size);
#if (__C7X_VEC_SIZE_BITS__ == 256)
         uint32_t eleCountBytes = 32;
         uint32_t eleCount      = eleCountBytes / VXLIB_sizeof(bufParamsIn.data_type);
#elif (__C7X_VEC_SIZE_BITS__ == 512)
         uint32_t eleCountBytes = 64;
         uint32_t eleCount      = eleCountBytes / VXLIB_sizeof(bufParamsIn.data_type);
#else
#error invalid target
#endif

         if (!(currPrm.kernelType)) {
            bufParamsScratch.data_type = currPrm.dTypeIn;
            bufParamsScratch.dim_x     = 1;
            bufParamsScratch.dim_y     = 1;
            // uint64_t strideScratch     = VXLIB_CALC_STRIDE(
            //     bufParamsScratch.dim_x * VXLIB_sizeof(bufParamsScratch.data_type), VXLIB_ALIGN_SHIFT_64BYTES);
            bufParamsScratch.stride_y = bufParamsScratch.dim_y * VXLIB_sizeof(bufParamsScratch.data_type);
         }
         else {
            bufParamsScratch.data_type = currPrm.dTypeIn;
            bufParamsScratch.dim_x     = bufParamsIn.dim_x;
            bufParamsScratch.dim_y     = (currPrm.M + 1) * currPrm.N;
            uint64_t strideScratch     = VXLIB_CALC_STRIDE(
                bufParamsScratch.dim_x * VXLIB_sizeof(bufParamsScratch.data_type), VXLIB_ALIGN_SHIFT_64BYTES);
            if (strideScratch % 512 == 0) {
               strideScratch += memory_padding;
            }
            bufParamsScratch.stride_y = strideScratch;
         }

         uint64_t scratchSizeBytes = bufParamsScratch.dim_y * bufParamsScratch.stride_y;
         //  printf("##### scratchSizeBytes: %d\n", scratchSizeBytes);
         void *pScratch = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, scratchSizeBytes);

         //  printf("pIn : %p pMask: %p\n", pIn, pMask);
         void *pOut = NULL, *pOutCn = NULL;
         if (currPrm.outputDataLocation == VXLIB_TEST_OUTPUT_HEAP) {
            pOut   = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, outSize);
            pOutCn = (void *) malloc(outSize);
         }
         else if (currPrm.outputDataLocation == VXLIB_TEST_OUTPUT_MSMC) {
            pOut = (void *) msmcBuffer;
            pOutCn = (void *) ddrBuffer;
         }

         // assign policy of current test case
         kerInitArgs.kernelType = currPrm.kernelType;

         // only run the test if the buffer allocations fit in the heap
         if (pIn && pMask && pOut && pOutCn && pScratch) {

            // fill buffers
            TI_fillBuffer(prm[tpi].testPattern, 0, pIn, prm[tpi].staticIn, bufParamsIn.dim_x, bufParamsIn.dim_y,
                          bufParamsIn.stride_y, VXLIB_sizeof(currPrm.dTypeIn), testPatternString);

            uint32_t trueCount = 0;
            if (currPrm.testPattern == STATIC) {
               TI_fillBuffer(prm[tpi].testPattern, 0, pMask, prm[tpi].staticMask, bufParamsMask.dim_x,
                             bufParamsMask.dim_y, bufParamsMask.stride_y, VXLIB_sizeof(currPrm.dTypeMask),
                             testPatternString);
            }
            else {
               int32_t ii, jj;
               if (VXLIB_sizeof(currPrm.dTypeMask) == 1) {
                  uint8_t *pMaskFill  = (uint8_t *) pMask;
                  int32_t  strideMask = bufParamsMask.stride_y / VXLIB_sizeof(currPrm.dTypeMask);

                  for (ii = 0; ii < (int32_t) bufParamsMask.dim_y; ii++) {
                     for (jj = 0; jj < (int32_t) bufParamsMask.dim_x; jj++) {
                        if (rand() > RAND_MAX / 2) {
                           pMaskFill[jj + ii * strideMask] = 0XFF;
                           trueCount++;
                        }
                        else {
                           pMaskFill[jj + ii * strideMask] = 0X00;
                        }
                     }
                  }
                  if (trueCount < 1) {
                     pMaskFill[0] = 0xFF;
                     trueCount    = 1;
                  }
               }
               else {
                  uint16_t *pMaskFill  = (uint16_t *) pMask;
                  int32_t   strideMask = bufParamsMask.stride_y / VXLIB_sizeof(currPrm.dTypeMask);
                  for (ii = 0; ii < (int32_t) bufParamsMask.dim_y; ii++) {
                     for (jj = 0; jj < (int32_t) bufParamsMask.dim_x; jj++) {
                        if (rand() > RAND_MAX / 2) {
                           pMaskFill[jj + ii * strideMask] = 0XFFFF;
                           trueCount++;
                        }
                        else {
                           pMaskFill[jj + ii * strideMask] = 0X0000;
                        }
                     }
                  }
                  if (trueCount < 1) {
                     pMaskFill[0] = 0xFFFF;
                     trueCount    = 1;
                  }
               }

               memset(pMask, 0xFF, inp1Size);
               trueCount = currPrm.M * currPrm.N;
            }
            // printf("##### TRUE CNT: %u\n", trueCount);

#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_median_d CP 0\n");
#endif

#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_median_d CP 1 status_init %d\n", status_init);
#endif
            // check parameters for initializing the kernel and initialize kernel if successful
            status_init = VXLIB_median_init_checkParams(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut,
                                                        &bufParamsScratch, &kerInitArgs);
            if (status_init == VXLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               VXLIB_asm(" MARK 0");
               kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;
               status_init = VXLIB_median_init(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut, &bufParamsScratch,
                                               &kerInitArgs);
               VXLIB_asm(" MARK 1");
               TI_profile_stop();
            }
            else {
               printf("status_init failed: %d", status_init);
            }

            // check execution parameters and call optimized kernel if sucessful
            status_opt = VXLIB_median_exec_checkParams(handle, pIn, pMask, pOut, pScratch);
            if (status_opt == VXLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_OPT);
               VXLIB_asm(" MARK 2");
               status_opt = VXLIB_median_exec(handle, pIn, pMask, pOut, pScratch);
               VXLIB_asm(" MARK 3");
               TI_profile_stop();
            }
            else {
               printf("status_opt failed: %d", status_opt);
            }

#if defined(EVM_TEST)
            //  the following for loop is to call kernel repeatedly so as to train the branch predictor
            for (k = 0; k < (VXLIB_WARM_REPS); k++) {
               // for (k = 0; k < (4); k++) {
               // run warm instruction cache test
               TI_profile_clear_cycle_count_single(TI_PROFILE_KERNEL_OPT_WARM);
               TI_profile_start(TI_PROFILE_KERNEL_OPT_WARM);
               VXLIB_asm(" MARK 4");
               status_opt = VXLIB_median_exec(handle, pIn, pMask, pOut, pScratch);
               VXLIB_asm(" MARK 5");
               TI_profile_stop();
            }

            // get output to L1D
            int16_t outSum   = 0;
            int8_t *pOutTemp = (int8_t *) pOut; // treat output as bytes to be data type agnostic
            for (k = 0; k < bufParamsOut.dim_x; k++) {
               outSum += *pOutTemp;
               pOutTemp++;
            }

            // dummy store of outSum to insure that the compiler does not remove it.
            volatileSum = outSum;

            // run warm instruction cache test
            TI_profile_start(TI_PROFILE_KERNEL_OPT_WARMWRB);

            VXLIB_asm(" MARK 6");
            status_opt = VXLIB_median_exec(handle, pIn, pMask, pOut, pScratch);
            VXLIB_asm(" MARK 7");
            TI_profile_stop();
#endif // if defined(EVM_TEST)

            /**************************************/
            /* Test natural C code for the kernel */
            /**************************************/

            // initialize the kernel function style to natural C
            kerInitArgs.funcStyle = VXLIB_FUNCTION_NATC;

            // call init once again to use the natural C variant
            VXLIB_median_init(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut, &bufParamsScratch, &kerInitArgs);

            TI_profile_start(TI_PROFILE_KERNEL_CN);
            VXLIB_asm(" MARK 8");
            status_nat = VXLIB_median_exec(handle, pIn, pMask, pOutCn, pScratch);
            VXLIB_asm(" MARK 9");
            TI_profile_stop();
#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_median_d CP 6 status_nat %d\n", status_nat);
#endif

            /*********************************************************************/
            /* Compare results with natural C output and static reference output */
            /*********************************************************************/
            // printf("NAT VS OPT\n");
            status_nat_vs_opt =
                TI_compare_mem_2D((void *) pOut, (void *) pOutCn, 0, 0, bufParamsOut.dim_x, bufParamsOut.dim_y,
                                  bufParamsOut.stride_y, VXLIB_sizeof(currPrm.dTypeOut));

            comparisonDone = 1;
#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_median_d CP 7 comparisonDone %d status_nat_vs_opt %d\n", comparisonDone,
                   status_nat_vs_opt);
#endif

            if (currPrm.staticOut != NULL) {
               // printf("REF VS OPT\n");
               status_ref_vs_opt = TI_compare_mem_roi2DStatic(
                   (void *) pOut, (void *) currPrm.staticOut, 0, 0, bufParamsOut.dim_x, bufParamsOut.dim_y,
                   bufParamsOut.stride_y, VXLIB_sizeof(bufParamsOut.data_type));
               comparisonDone = 1;
#if VXLIB_DEBUGPRINT
               printf("VXLIB_DEBUGPRINT  VXLIB_median_d CP 7 comparisonDone %d status_ref_vs_opt %d\n", comparisonDone,
                      status_ref_vs_opt);
#endif
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
            printf("VXLIB_DEBUGPRINT  VXLIB_median_d CP 8 fail %d\n", fail);
#endif

            // update profiling information
            pProfile[3 * tpi]     = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
            pProfile[3 * tpi + 1] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[3 * tpi + 2] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);

            if (!currPrm.kernelType) {
               sprintf(desc, "%s |R|%ld|C|%ld|IMPL|0|TRUECNT|9|M|%ld|N|%ld|Type|%u|", testPatternString, currPrm.height,
                       currPrm.width, currPrm.M, currPrm.N, currPrm.dTypeIn);
            }
            else {
               sprintf(desc, "%s |R|%ld|C|%ld|IMPL|1|TRUECNT|%d|M|%ld|N|%ld|DType|%u|", testPatternString,
                       currPrm.height, currPrm.width, trueCount, currPrm.M, currPrm.N, currPrm.dTypeIn);
            }

#if !defined(EVM_TEST)
            // write to CSV, must happen prior to write to screen because TI_profile_formula_add clears values in
            // counters
            /* fprintf(fpOutputCSV, "add, %d, %d, %d, %lu, %lu, %lu, %lu, %.2f, %d\n", testNum, currPrm.testPattern, */
            /*         VXLIB_sizeof(bufParamsIn.data_type) * 8, currPrm.width, currPrm.height, archCycles, */
            /*         cycles[TI_PROFILE_KERNEL_OPT], */
            /*         ((VXLIB_F32) cycles[TI_PROFILE_KERNEL_OPT]) / ((VXLIB_F32) archCycles), !currentTestFail); */
#endif // #if !defined(EVM_TEST)
            VXLIB_median_perfEst(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut, &bufParamsScratch, &kerInitArgs, &archCycles, &estCycles);
            TI_profile_add_test(testNum++, currPrm.width * currPrm.height, archCycles, estCycles, currentTestFail, desc);
         }
         else {
            sprintf(desc, "width =%ld, height = %ld", currPrm.width, currPrm.height);
            TI_profile_skip_test(desc);

         } // end of memory allocation successful?

         /*************************************/
         /* Free buffers for each test vector */
         /*************************************/

         if (currPrm.outputDataLocation == VXLIB_TEST_OUTPUT_HEAP) {
            free(pOutCn);
            TI_align_free(pOut);
         }
         else if (currPrm.outputDataLocation == VXLIB_TEST_OUTPUT_MSMC) {
         }

         TI_align_free(pIn);
         TI_align_free(pMask);
         TI_align_free(pScratch);

         //  printf("After free pIn: %p pMask: %p pOut: %p\n", pIn, pMask, pOut);

      } // end repetitions
   }    // end idat test cases

   free(handle);

   /* Close results CSV */
   /* fclose(fpOutputCSV); */

   return fail;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* coverage_test_main */
/*                                                                                                                    */
/**********************************************************************************************************************/

int coverage_test_main()
{
   int32_t               testNum         = 1000;
   int32_t               currentTestFail = 0;
   VXLIB_STATUS          status_nat, status_init;
   VXLIB_STATUS          status_opt;
   VXLIB_bufParams2D_t   bufParamsIn, bufParamsMask, bufParamsOut, bufParamsScratch;
   VXLIB_median_InitArgs kerInitArgs;
   int32_t               handleSize = VXLIB_median_getHandleSize(&kerInitArgs);
   VXLIB_kernelHandle    handle     = malloc(handleSize);
   int                   fail       = 0;

   void *pIn                  = (void *) malloc(16);
   void *pMask                = (void *) malloc(16);
   void *pOut                 = (void *) malloc(16);
   void *pScratch             = (void *) malloc(16);
   bufParamsIn.data_type      = VXLIB_UINT8;
   bufParamsMask.data_type    = VXLIB_UINT8;
   bufParamsOut.data_type     = VXLIB_UINT8;
   bufParamsScratch.data_type = VXLIB_UINT8;

   while (testNum <= 1013) {
      currentTestFail = 0;
      switch (testNum) {

      case 1000:
         status_init     = VXLIB_median_init_checkParams(NULL, &bufParamsIn, &bufParamsMask, &bufParamsOut,
                                                         &bufParamsScratch, &kerInitArgs);
         currentTestFail = (status_init != VXLIB_ERR_NULL_POINTER) ? 1 : 0;
         break;

      case 1001:
         status_init     = VXLIB_median_init_checkParams(handle, NULL, &bufParamsMask, &bufParamsOut, &bufParamsScratch,
                                                         &kerInitArgs);
         currentTestFail = (status_init != VXLIB_ERR_NULL_POINTER) ? 1 : 0;
         break;

      case 1002:
         status_init =
             VXLIB_median_init_checkParams(handle, &bufParamsIn, NULL, &bufParamsOut, &bufParamsScratch, &kerInitArgs);
         currentTestFail = (status_init != VXLIB_ERR_NULL_POINTER) ? 1 : 0;
         break;
         break;

      case 1003:
         status_init =
             VXLIB_median_init_checkParams(handle, &bufParamsIn, &bufParamsMask, NULL, &bufParamsScratch, &kerInitArgs);
         currentTestFail = (status_init != VXLIB_ERR_NULL_POINTER) ? 1 : 0;
         break;
         break;

      case 1004:
         status_init =
             VXLIB_median_init_checkParams(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut, NULL, &kerInitArgs);
         currentTestFail = (status_init != VXLIB_ERR_NULL_POINTER) ? 1 : 0;
         break;

      case 1005:
         status_init     = VXLIB_median_init_checkParams(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut,
                                                         &bufParamsScratch, NULL);
         currentTestFail = (status_init != VXLIB_ERR_NULL_POINTER) ? 1 : 0;
         break;

      case 1006:
         bufParamsIn.dim_x      = 8;
         bufParamsIn.stride_y   = 4;
         bufParamsMask.dim_x    = 8;
         bufParamsMask.stride_y = 4;
         bufParamsOut.dim_x     = 8;
         bufParamsOut.stride_y  = 4;
         status_init            = VXLIB_median_init_checkParams(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut,
                                                                &bufParamsScratch, &kerInitArgs);
         currentTestFail        = (status_init != VXLIB_ERR_INVALID_DIMENSION) ? 1 : 0;

         bufParamsIn.stride_y = 8;
         status_init          = VXLIB_median_init_checkParams(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut,
                                                              &bufParamsScratch, &kerInitArgs);
         currentTestFail      = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_DIMENSION)) ? 1 : 0;

         bufParamsMask.stride_y = 8;
         status_init            = VXLIB_median_init_checkParams(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut,
                                                                &bufParamsScratch, &kerInitArgs);
         currentTestFail        = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_DIMENSION)) ? 1 : 0;
         break;
      case 1007:
         bufParamsIn.dim_x      = 8;
         bufParamsIn.dim_y      = 8;
         bufParamsIn.stride_y   = 8;
         bufParamsMask.dim_x    = 3;
         bufParamsMask.dim_y    = 3;
         bufParamsMask.stride_y = 3;
         bufParamsOut.dim_x     = 8;
         bufParamsOut.dim_y     = 6;
         bufParamsOut.stride_y  = 8;
         status_init            = VXLIB_median_init_checkParams(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut,
                                                                &bufParamsScratch, &kerInitArgs);
         currentTestFail        = (status_init != VXLIB_ERR_INVALID_DIMENSION) ? 1 : 0;
         bufParamsOut.dim_x = 6;
         bufParamsOut.dim_y = 8;
         status_init        = VXLIB_median_init_checkParams(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut,
                                                            &bufParamsScratch, &kerInitArgs);
         currentTestFail    = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_DIMENSION)) ? 1 : 0;
         break;

      case 1008:
         bufParamsIn.dim_x      = 8;
         bufParamsIn.dim_y      = 8;
         bufParamsIn.stride_y   = 8;
         bufParamsMask.dim_x    = 3;
         bufParamsMask.dim_y    = 0;
         bufParamsMask.stride_y = 3;
         bufParamsOut.dim_x     = 6;
         bufParamsOut.dim_y     = 9;
         bufParamsOut.stride_y  = 10;
         status_init            = VXLIB_median_init_checkParams(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut,
                                                                &bufParamsScratch, &kerInitArgs);
         currentTestFail        = (status_init != VXLIB_ERR_INVALID_DIMENSION) ? 1 : 0;

         bufParamsMask.dim_x = 0;
         bufParamsMask.dim_y = 3;
         bufParamsOut.dim_x  = 9;
         bufParamsOut.dim_y  = 6;
         status_init         = VXLIB_median_init_checkParams(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut,
                                                             &bufParamsScratch, &kerInitArgs);
         currentTestFail     = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_DIMENSION)) ? 1 : 0;
         break;

      case 1009:
         bufParamsIn.dim_x      = 16;
         bufParamsIn.dim_y      = 16;
         bufParamsIn.stride_y   = 16;
         bufParamsMask.dim_x    = 9;
         bufParamsMask.dim_y    = 10;
         bufParamsMask.stride_y = 10;
         bufParamsOut.dim_x     = 8;
         bufParamsOut.dim_y     = 7;
         bufParamsOut.stride_y  = 20;
         status_init            = VXLIB_median_init_checkParams(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut,
                                                                &bufParamsScratch, &kerInitArgs);
         currentTestFail        = (status_init != VXLIB_ERR_INVALID_DIMENSION) ? 1 : 0;

         bufParamsMask.dim_x = 10;
         bufParamsMask.dim_y = 9;
         bufParamsOut.dim_x  = 7;
         bufParamsOut.dim_y  = 8;
         status_init         = VXLIB_median_init_checkParams(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut,
                                                             &bufParamsScratch, &kerInitArgs);
         currentTestFail     = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_DIMENSION)) ? 1 : 0;
         break;

      case 1010:
         bufParamsIn.data_type  = VXLIB_UINT64;
         bufParamsIn.dim_x      = 16;
         bufParamsIn.dim_y      = 16;
         bufParamsIn.stride_y   = 128;
         bufParamsMask.dim_x    = 16;
         bufParamsMask.dim_y    = 16;
         bufParamsMask.stride_y = 128;
         bufParamsOut.dim_x     = 16;
         bufParamsOut.dim_y     = 16;
         bufParamsOut.stride_y  = 128;
         status_init            = VXLIB_median_init_checkParams(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut,
                                                                &bufParamsScratch, &kerInitArgs);
         currentTestFail        = (status_init != VXLIB_ERR_INVALID_TYPE) ? 1 : 0;

         kerInitArgs.funcStyle = VXLIB_FUNCTION_NATC;
         status_init =
             VXLIB_median_init(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut, &bufParamsScratch, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_TYPE)) ? 1 : 0;

         kerInitArgs.funcStyle  = VXLIB_FUNCTION_OPTIMIZED;
         kerInitArgs.kernelType = 0;
         status_init =
             VXLIB_median_init(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut, &bufParamsScratch, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_TYPE)) ? 1 : 0;

         kerInitArgs.kernelType = 1;
         status_init =
             VXLIB_median_init(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut, &bufParamsScratch, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_TYPE)) ? 1 : 0;
         break;

      case 1011:
         status_init     = VXLIB_median_exec_checkParams(NULL, pIn, pMask, pOut, pScratch);
         currentTestFail = (status_init != VXLIB_ERR_NULL_POINTER) ? 1 : 0;

         status_init     = VXLIB_median_exec_checkParams(handle, NULL, pMask, pOut, pScratch);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_NULL_POINTER)) ? 1 : 0;

         status_init     = VXLIB_median_exec_checkParams(handle, pIn, NULL, pOut, pScratch);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_NULL_POINTER)) ? 1 : 0;

         status_init     = VXLIB_median_exec_checkParams(handle, pIn, pMask, NULL, pScratch);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_NULL_POINTER)) ? 1 : 0;

         status_init     = VXLIB_median_exec_checkParams(handle, pIn, pMask, pOut, NULL);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_NULL_POINTER)) ? 1 : 0;
         break;

      case 1012:
         bufParamsIn.data_type      = VXLIB_FLOAT16;
         bufParamsMask.data_type    = VXLIB_INT16;
         bufParamsOut.data_type     = VXLIB_INT16;
         bufParamsScratch.data_type = VXLIB_INT16;

         status_init =
             VXLIB_median_init(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut, &bufParamsScratch, &kerInitArgs);
         currentTestFail = ((status_init != VXLIB_ERR_INVALID_TYPE)) ? 1 : 0;

         bufParamsIn.data_type = VXLIB_UINT24;
         status_init =
             VXLIB_median_init(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut, &bufParamsScratch, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_TYPE)) ? 1 : 0;

         bufParamsIn.data_type = VXLIB_INT32;
         status_init =
             VXLIB_median_init(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut, &bufParamsScratch, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_TYPE)) ? 1 : 0;

         bufParamsIn.data_type = VXLIB_UINT32;
         status_init =
             VXLIB_median_init(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut, &bufParamsScratch, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_TYPE)) ? 1 : 0;

         bufParamsIn.data_type = VXLIB_FLOAT32;
         status_init =
             VXLIB_median_init(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut, &bufParamsScratch, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_TYPE)) ? 1 : 0;

         bufParamsIn.data_type = VXLIB_INT64;
         status_init =
             VXLIB_median_init(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut, &bufParamsScratch, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_TYPE)) ? 1 : 0;

         bufParamsIn.data_type = VXLIB_UINT64;
         status_init =
             VXLIB_median_init(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut, &bufParamsScratch, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_TYPE)) ? 1 : 0;

         bufParamsIn.data_type = VXLIB_FLOAT64;
         status_init =
             VXLIB_median_init(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut, &bufParamsScratch, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_TYPE)) ? 1 : 0;

         bufParamsIn.data_type = VXLIB_INT128;
         status_init =
             VXLIB_median_init(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut, &bufParamsScratch, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_TYPE)) ? 1 : 0;

         bufParamsIn.data_type = VXLIB_UINT128;
         status_init =
             VXLIB_median_init(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut, &bufParamsScratch, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_TYPE)) ? 1 : 0;

         bufParamsIn.data_type = 14;
         status_init =
             VXLIB_median_init(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut, &bufParamsScratch, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_TYPE)) ? 1 : 0;
         break;

      case 1013: {

         uint8_t *pMaskFill = (uint8_t *) pMask;
		 int i;
         for(i = 0; i < 16; i++) {
            pMaskFill[i] = 0;
         }

         bufParamsIn.data_type  = VXLIB_UINT8;
         kerInitArgs.funcStyle  = VXLIB_FUNCTION_NATC;
         kerInitArgs.kernelType = 1;
         bufParamsMask.dim_x    = 4;
         bufParamsMask.dim_y    = 4;
         bufParamsMask.stride_y = 4;
         bufParamsIn.dim_x      = 8;
         bufParamsIn.dim_y      = 8;
         bufParamsOut.dim_x     = 5;
         bufParamsOut.dim_y     = 5;

         status_init =
             VXLIB_median_init(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut, &bufParamsScratch, &kerInitArgs);
         if (status_init == VXLIB_SUCCESS) {
            status_nat = VXLIB_median_exec(handle, pIn, pMask, pOut, pScratch);
         }

         kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;
         status_init =
             VXLIB_median_init(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut, &bufParamsScratch, &kerInitArgs);
         if (status_init == VXLIB_SUCCESS) {
            status_opt = VXLIB_median_exec(handle, pIn, pMask, pOut, pScratch);
         }

         currentTestFail = ((status_nat != VXLIB_ERR_FAILURE) || (status_opt != VXLIB_ERR_FAILURE)) ? 1 : 0;
         break;
      }

      default:
         break;
      }

      fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;
      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum++, 0, 0, 0, currentTestFail, desc);
   }
   free(pIn);
   free(pMask);
   free(pOut);
   free(pScratch);
   free(handle);
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
      TI_memError("VXLIB_median");
      return 1;
   }
   else
#else
   printf("_HOST_BUILD is defined.\n");
#endif
   {
      return VXLIB_median_d(&pProfile[0], 0);
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

   uint32_t profile[256 * 3] = {0};

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

   fail = coverage_test_main();
   if (fail == 0) {
      printf("Coverage Test Pass!\n");
   }
   else {
      printf("Coverage Test Fail!!\n");
   }
   return fail;
}
#endif
