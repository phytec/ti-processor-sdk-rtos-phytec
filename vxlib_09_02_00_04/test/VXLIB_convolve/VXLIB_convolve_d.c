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
#include "VXLIB_convolve_idat.h"
#include "VXLIB_types.h"
#include <common/VXLIB_types.h>
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

int16_t volatile volatileSum = 0; // use volatile to keep compiler from removing this operation

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_convolve_d */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method performs regerssion testing for the VXLIB_convolve kernel
int VXLIB_convolve_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   int32_t  tpi;             // iteration variable for test input
   int32_t  currentTestFail; // status of current test case
   int32_t  fail = 0;        // pass/fail flag
   uint32_t repCount;        // iteration variable for repetition count
   uint32_t numReps;         // number of times to repeat test cases
   uint32_t testNum;         // test number

   // input and output buffer parameter variables
   VXLIB_bufParams2D_t bufParamsIn;
   VXLIB_bufParams2D_t bufParamsOut;
   VXLIB_bufParams2D_t bufParamsFilter;

#if defined(EVM_TEST)
   uint32_t k = 0;
#endif

   // Unused for now; TBD: performance estimation
   uint64_t archCycles, estCycles;

   // data structure to hold test cases parameters
   VXLIB_convolve_testParams_t *prm;
   VXLIB_convolve_testParams_t  currPrm;

   // get total number of test cases
   VXLIB_convolve_getTestParams(&prm, &test_cases);

   // initialization args for kernel
   VXLIB_convolve_InitArgs kerInitArgs;

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT  VXLIB_convolve_d CP 0\n");
#endif

   // create kernel's handle
   int32_t            handleSize = VXLIB_convolve_getHandleSize(&kerInitArgs);
   VXLIB_kernelHandle handle     = malloc(handleSize);

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT  VXLIB_convolve_d CP 1\n");
#endif

   TI_profile_init("VXLIB_convolve");

   /* #if !defined(EVM_TEST) */
   /* file IO for benchmarking */
   FILE *fpOutputCSV = fopen("VXLIB_convolve.csv", "w+");
   fprintf(fpOutputCSV, "Kernel, Test ID, dTypeIn, dTypeFilter, dTypeOut, width, height, strideIn, strideOut, "
                        "filterWidth, filterHeight, scale, padLeft, "
                        "padRight, padTop, "
                        "padBottom, Loki cycles\n");
   /* #endif // #if defined(EVM_TEST) */

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

         /*******************************************/
         /* Assign kernel initialization parameters */
         /*******************************************/

         kerInitArgs.filterHeight = currPrm.filterHeight;
         kerInitArgs.filterWidth  = currPrm.filterWidth;
         kerInitArgs.scale        = currPrm.scale;

         kerInitArgs.padLeft   = currPrm.padLeft; // supports non-padded implementation currently
         kerInitArgs.padRight  = currPrm.padRight;
         kerInitArgs.padTop    = currPrm.padTop;
         kerInitArgs.padBottom = currPrm.padBottom;

         bool isNotPadded =
             (currPrm.padLeft == 0) && (currPrm.padRight == 0) && (currPrm.padTop == 0) && (currPrm.padBottom == 0);
         /*****************************************/
         /* Assign input output buffer parameters */
         /*****************************************/

         bufParamsIn.data_type = currPrm.dTypeIn;
         bufParamsIn.dim_x     = currPrm.width;
         bufParamsIn.dim_y     = currPrm.height;
         bufParamsIn.stride_y  = currPrm.strideIn;

         bufParamsFilter.data_type = currPrm.dTypeFilter;
         bufParamsFilter.dim_x     = currPrm.filterWidth;
         bufParamsFilter.dim_y     = currPrm.filterHeight;
         bufParamsFilter.stride_y  = currPrm.filterWidth * sizeof(int16_t);

         bufParamsOut.data_type = currPrm.dTypeOut;
         bufParamsOut.stride_y  = currPrm.strideOut;

         // output size differs based on padding or non padding
         if (isNotPadded) {

            bufParamsOut.dim_x = currPrm.width - currPrm.filterWidth + 1;
            bufParamsOut.dim_y = currPrm.height - currPrm.filterHeight + 1;
         }

         else {

            bufParamsOut.dim_x = currPrm.width;
            bufParamsOut.dim_y = currPrm.height;
         }

         /*****************************************/
         /* Create buffers for input and outputs  */
         /*****************************************/

         size_t inpSize    = bufParamsIn.dim_y * bufParamsIn.stride_y;
         size_t filterSize = bufParamsFilter.dim_y * bufParamsFilter.stride_y;
         size_t outSize    = bufParamsOut.dim_y * bufParamsOut.stride_y;

         void *pIn     = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, inpSize);
         void *pFilter = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, filterSize);
         void *pOut = NULL, *pOutCn = NULL;
         if (currPrm.outputDataLocation == VXLIB_TEST_OUTPUT_HEAP) {
            pOut   = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, outSize);
            pOutCn = (void *) malloc(outSize);
         }
         else if (currPrm.outputDataLocation == VXLIB_TEST_OUTPUT_MSMC) {
            pOut = (void *) msmcBuffer;
            pOutCn = (void *) ddrBuffer;
         }

         // only run the test if the buffer allocations fit in the heap
         if (pIn && pFilter && pOut && pOutCn) {

            // fill buffers
            TI_fillBuffer(prm[tpi].testPattern, 0, pIn, prm[tpi].staticIn, bufParamsIn.dim_x, bufParamsIn.dim_y,
                          bufParamsIn.stride_y, VXLIB_sizeof(currPrm.dTypeIn), testPatternString);

            TI_fillBuffer(prm[tpi].testPattern, 0, pFilter, prm[tpi].staticFilter, bufParamsFilter.dim_x,
                          bufParamsFilter.dim_y, bufParamsFilter.stride_y, VXLIB_sizeof(currPrm.dTypeFilter),
                          testPatternString);

#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_convolve_d CP 0\n");
#endif

#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_convolve_d CP 1 status_init %d\n", status_init);
#endif
            // check parameters for initializing the kernel and initialize kernel if successful
            status_init =
                VXLIB_convolve_init_checkParams(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
            if (status_init == VXLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               VXLIB_asm(" MARK 0");
               kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;
               status_init = VXLIB_convolve_init(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
               VXLIB_asm(" MARK 1");
               TI_profile_stop();
            }
            else {
               printf("status_init failed: %d", status_init);
            }

            // check execution parameters and call optimized kernel if sucessful
            status_opt = VXLIB_convolve_exec_checkParams(handle, pIn, pFilter, pOut);
            if (status_opt == VXLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_OPT);
               VXLIB_asm(" MARK 2");
               status_opt = VXLIB_convolve_exec(handle, pIn, pFilter, pOut);
               VXLIB_asm(" MARK 3");
               TI_profile_stop();
            }
            else {
               printf("status_opt failed: %d", status_opt);
            }

#if defined(EVM_TEST)
            // the following for loop is to call kernel repeatedly so as to train the branch predictor
            for (k = 0; k < (VXLIB_WARM_REPS); k++) {
               // run warm instruction cache test
               TI_profile_clear_cycle_count_single(TI_PROFILE_KERNEL_OPT_WARM);
               TI_profile_start(TI_PROFILE_KERNEL_OPT_WARM);
               VXLIB_asm(" MARK 4");
               status_opt = VXLIB_convolve_exec(handle, pIn, pFilter, pOut);
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
            status_opt = VXLIB_convolve_exec(handle, pIn, pFilter, pOut);
            VXLIB_asm(" MARK 7");
            TI_profile_stop();
#endif // if defined(EVM_TEST)

            /**************************************/
            /* Test natural C code for the kernel */
            /**************************************/

            // initialize the kernel function style to natural C
            kerInitArgs.funcStyle = VXLIB_FUNCTION_NATC;

            // call init once again to use the natural C variant
            VXLIB_convolve_init(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);

            TI_profile_start(TI_PROFILE_KERNEL_CN);
            VXLIB_asm(" MARK 8");
            status_nat = VXLIB_convolve_exec(handle, pIn, pFilter, pOutCn);
            VXLIB_asm(" MARK 9");
            TI_profile_stop();
#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_convolve_d CP 6 status_nat %d\n", status_nat);
#endif

            /*********************************************************************/
            /* Compare results with natural C output and static reference output */
            /*********************************************************************/

            /* printf("\natc output compare\n"); */

            status_nat_vs_opt =
                TI_compare_mem_2D((void *) pOut, (void *) pOutCn, 0, 0, bufParamsOut.dim_x, bufParamsOut.dim_y,
                                  bufParamsOut.stride_y, VXLIB_sizeof(currPrm.dTypeOut));

            comparisonDone = 1;
#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_convolve_d CP 7 comparisonDone %d status_nat_vs_opt %d\n", comparisonDone,
                   status_nat_vs_opt);
#endif

            if (currPrm.staticOut != NULL) {
               /* printf("\nref output compare\n"); */
               status_ref_vs_opt = TI_compare_mem_roi2DStatic(
                   (void *) pOut, (void *) currPrm.staticOut, 0, 0, bufParamsOut.dim_x, bufParamsOut.dim_y,
                   bufParamsOut.stride_y, VXLIB_sizeof(bufParamsOut.data_type));
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
            printf("VXLIB_DEBUGPRINT  VXLIB_convolve_d CP 8 fail %d\n", fail);
#endif

            // update profiling information
            pProfile[3 * tpi]     = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
            pProfile[3 * tpi + 1] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[3 * tpi + 2] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);

            sprintf(desc, "%s generated input | width = %ld, height = %ld", testPatternString, currPrm.width,
                    currPrm.height);

            /* #if !defined(EVM_TEST) */
            // write to CSV, must happen prior to write to screen because TI_profile_formula_add clears values in
            // counters
            fprintf(fpOutputCSV,
                    "convolve, %d, %d, %d, %d, %lu, %lu, %lu, %lu, %lu, %lu, %lu, %lu, %lu, %lu, %lu, %lu\n", testNum,
                    VXLIB_sizeof(bufParamsIn.data_type) * 8, VXLIB_sizeof(bufParamsFilter.data_type) * 8,
                    VXLIB_sizeof(bufParamsOut.data_type) * 8, currPrm.width, currPrm.height, currPrm.strideIn,
                    currPrm.strideOut, currPrm.filterWidth, currPrm.filterHeight, currPrm.scale, currPrm.padLeft,
                    currPrm.padRight, currPrm.padTop, currPrm.padBottom, cycles[TI_PROFILE_KERNEL_OPT]);
            /* #endif // #if !defined(EVM_TEST) */

            VXLIB_convolve_perfEst(handle, &archCycles, &estCycles);
            TI_profile_add_test(testNum++, currPrm.width * currPrm.height, archCycles, estCycles, currentTestFail,
                                desc);
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

         TI_align_free(pFilter);
         TI_align_free(pIn);

      } // end repetitions
   }    // end idat test cases

   free(handle);

   // close results CSV
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
      TI_memError("VXLIB_convolve");
      return 1;
   }
   else
#else
   printf("_HOST_BUILD is defined.\n");
#endif
   {
      return VXLIB_convolve_d(&pProfile[0], 0);
   }
}

int coverage_test_main()
{
   int32_t             testNum         = 1000;
   int32_t             currentTestFail = 0;
   VXLIB_STATUS        status_nat, status_nat1, status_nat2, status_nat3;
   VXLIB_STATUS        status_opt, status_opt1, status_opt2, status_opt3, status_opt4;
   VXLIB_bufParams2D_t bufParamsIn0, bufParamsOut, bufParamsFilter;

   VXLIB_convolve_InitArgs kerInitArgs;
   int32_t            handleSize = VXLIB_convolve_getHandleSize(&kerInitArgs);
   VXLIB_kernelHandle handle     = malloc(handleSize);

   int   fail = 0;
   void *pIn0 = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, 16 * 64);
   void *pFilter = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, 16 * 64);
   void *pOut = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, 16 * 64);

   kerInitArgs.padLeft        = 0;
   kerInitArgs.padRight       = 0;
   kerInitArgs.padTop         = 0;
   kerInitArgs.padBottom      = 0;
   kerInitArgs.filterHeight   = 3;
   kerInitArgs.filterWidth    = 3;
   kerInitArgs.scale          = 30;

   
   bufParamsIn0.dim_x     = 16;
   bufParamsIn0.dim_x     = 16;
   bufParamsIn0.stride_y  = 64;
   bufParamsIn0.data_type = VXLIB_UINT8;

   bufParamsOut.dim_x     = 16;
   bufParamsOut.dim_x     = 16;
   bufParamsOut.stride_y  = 64;
   bufParamsOut.data_type = VXLIB_UINT8;

   bufParamsFilter.data_type = VXLIB_INT16;
   bufParamsFilter.dim_x     = 3;
   bufParamsFilter.dim_y     = 3;
   bufParamsFilter.stride_y  = 3;

   while (testNum <= 1007) {

      switch (testNum) {
         /* Init Check Params  Coverage */
      case 1000:
         bufParamsIn0.dim_x     = 16;
         bufParamsIn0.dim_y     = 16;
         bufParamsIn0.stride_y  = 64;
         bufParamsIn0.data_type = VXLIB_INT16;

         bufParamsOut.dim_x     = 16;
         bufParamsOut.dim_y     = 16;
         bufParamsOut.stride_y  = 64;
         bufParamsOut.data_type = VXLIB_INT16;

         kerInitArgs.funcStyle = VXLIB_FUNCTION_NATC;
         status_nat  = VXLIB_convolve_init_checkParams(NULL, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);

         kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;
         status_opt1           = VXLIB_convolve_init_checkParams(NULL, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         currentTestFail       = ((status_nat  != VXLIB_ERR_NULL_POINTER) || (status_opt1 != VXLIB_ERR_NULL_POINTER));
         break;
      case 1001:
         /* Init Check Params  Coverage */
         // Invalid Dtype
         bufParamsIn0.dim_x    = 16;
         bufParamsIn0.dim_y    = 16;
         bufParamsIn0.stride_y = 64;

         bufParamsOut.dim_x     = 16;
         bufParamsOut.dim_y     = 16;
         bufParamsOut.stride_y  = 64;
         bufParamsIn0.data_type = VXLIB_UINT8;
         bufParamsFilter.data_type = VXLIB_INT16;
         bufParamsOut.data_type = VXLIB_INT32;

         kerInitArgs.funcStyle = VXLIB_FUNCTION_NATC;
         status_nat1           = VXLIB_convolve_init_checkParams(handle, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);

         bufParamsIn0.data_type = VXLIB_INT8;
         bufParamsFilter.data_type = VXLIB_INT32;
         bufParamsOut.data_type = VXLIB_UINT16;

         status_nat2           = VXLIB_convolve_init_checkParams(handle, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;

         bufParamsIn0.data_type = VXLIB_INT16;
         bufParamsFilter.data_type = VXLIB_INT16;
         bufParamsOut.data_type = VXLIB_UINT8;
         status_opt             = VXLIB_convolve_init_checkParams(handle, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         currentTestFail        = ((status_nat1 != VXLIB_ERR_CONVOLVE_INVALID_TYPE_COMBINATION) || (status_nat2 != VXLIB_ERR_CONVOLVE_INVALID_TYPE_COMBINATION) ||
                                   (status_opt != VXLIB_ERR_CONVOLVE_INVALID_TYPE_COMBINATION));
         break;
      case 1002:
         /* Init Check Params  Coverage */
         // Invalid shift
         bufParamsIn0.dim_x    = 16;
         bufParamsIn0.dim_y    = 16;
         bufParamsIn0.stride_y = 64;

         bufParamsOut.dim_x     = 16;
         bufParamsOut.dim_y     = 16;
         bufParamsOut.stride_y  = 64;
         bufParamsIn0.data_type = VXLIB_UINT8;
         bufParamsOut.data_type = VXLIB_INT16;
         bufParamsFilter.data_type = VXLIB_INT16;

         kerInitArgs.scale          = 33U; 

         kerInitArgs.funcStyle = VXLIB_FUNCTION_NATC;

         status_nat = VXLIB_convolve_init_checkParams(handle, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);

         bufParamsIn0.dim_x    = 16;
         bufParamsIn0.dim_y    = 16;
         bufParamsIn0.stride_y = 64;

         bufParamsOut.dim_x    = 16;
         bufParamsOut.dim_y    = 16;
         bufParamsOut.stride_y = 64;

         kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;
         status_opt      = VXLIB_convolve_init_checkParams(handle, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         currentTestFail = ((status_nat != VXLIB_ERR_CONVOLVE_INVALID_SHIFT) || (status_opt != VXLIB_ERR_CONVOLVE_INVALID_SHIFT));
         break;
      case 1003:
         /* Init Check Params  Coverage */
         // Invalid pad
         bufParamsIn0.dim_x    = 16;
         bufParamsIn0.dim_y    = 16;
         bufParamsIn0.stride_y = 64;

         bufParamsOut.dim_x     = 16;
         bufParamsOut.dim_y     = 16;
         bufParamsOut.stride_y  = 64;
         bufParamsIn0.data_type = VXLIB_UINT8;
         bufParamsOut.data_type = VXLIB_INT16;
         bufParamsFilter.data_type = VXLIB_INT16;

         kerInitArgs.padLeft        = 0;
         kerInitArgs.padRight       = 0;
         kerInitArgs.padTop         = 12;
         kerInitArgs.padBottom      = 0;
         kerInitArgs.filterHeight   = 3;
         kerInitArgs.filterWidth    = 3;
         kerInitArgs.scale          = 30;
   

         kerInitArgs.funcStyle = VXLIB_FUNCTION_NATC;

         status_nat = VXLIB_convolve_init_checkParams(handle, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);

         bufParamsIn0.dim_x    = 16;
         bufParamsIn0.dim_y    = 16;
         bufParamsIn0.stride_y = 64;

         bufParamsOut.dim_x    = 16;
         bufParamsOut.dim_y    = 16;
         bufParamsOut.stride_y = 64;

         kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;
         status_opt      = VXLIB_convolve_init_checkParams(handle, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         currentTestFail = ((status_nat != VXLIB_ERR_CONVOLVE_PADDED_NOT_IMPLEMENTED) || (status_opt != VXLIB_ERR_CONVOLVE_PADDED_NOT_IMPLEMENTED));
         break;
      case 1004:
         /* Init Check Params  Coverage */
         // Invalid dim
         bufParamsIn0.data_type = VXLIB_UINT8;
         bufParamsOut.data_type = VXLIB_INT16;
         bufParamsFilter.data_type = VXLIB_INT16;

         kerInitArgs.padLeft        = 0;
         kerInitArgs.padRight       = 0;
         kerInitArgs.padTop         = 0;
         kerInitArgs.padBottom      = 0;
         kerInitArgs.filterHeight   = 3;
         kerInitArgs.filterWidth    = 3;
         kerInitArgs.scale          = 30;

         kerInitArgs.funcStyle = VXLIB_FUNCTION_NATC;
         bufParamsIn0.dim_x    = 16;
         bufParamsIn0.dim_y    = 16;
         bufParamsIn0.stride_y = 64;

         bufParamsOut.dim_x    = 17;
         bufParamsOut.dim_y    = 16;
         bufParamsOut.stride_y = 64;
         status_nat            = VXLIB_convolve_init_checkParams(handle, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);

         bufParamsIn0.dim_x    = 16;
         bufParamsIn0.dim_y    = 16;
         bufParamsIn0.stride_y = 64;

         bufParamsOut.dim_x    = 16;
         bufParamsOut.dim_y    = 19;
         bufParamsOut.stride_y = 64;
         status_nat1            = VXLIB_convolve_init_checkParams(handle, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);

         bufParamsIn0.dim_x    = 16;
         bufParamsIn0.dim_y    = 16;
         bufParamsIn0.stride_y = 13;

         bufParamsOut.dim_x    = 16;
         bufParamsOut.dim_y    = 14;
         bufParamsOut.stride_y = 64;
         status_nat2            = VXLIB_convolve_init_checkParams(handle, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);

         kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;
         bufParamsIn0.dim_x    = 16;
         bufParamsIn0.dim_y    = 16;
         bufParamsIn0.stride_y = 64;

         bufParamsOut.dim_x    = 16;
         bufParamsOut.dim_y    = 14;
         bufParamsOut.stride_y = 13;

         status_opt = VXLIB_convolve_init_checkParams(handle, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         
         bufParamsIn0.dim_x    = 2;
         bufParamsIn0.dim_y    = 16;
         bufParamsIn0.stride_y = 64;

         bufParamsOut.dim_x    = 2;
         bufParamsOut.dim_y    = 14;
         bufParamsOut.stride_y = 64;

         status_opt1 = VXLIB_convolve_init_checkParams(handle, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         
         bufParamsIn0.dim_x    = 16;
         bufParamsIn0.dim_y    = 2;
         bufParamsIn0.stride_y = 64;

         bufParamsOut.dim_x    = 16;
         bufParamsOut.dim_y    = 0;
         bufParamsOut.stride_y = 64;

         status_opt2 = VXLIB_convolve_init_checkParams(handle, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         
         currentTestFail =
             ((status_nat  != VXLIB_ERR_CONVOLVE_INVALID_DIMENSION) || (status_opt  != VXLIB_ERR_CONVOLVE_INVALID_DIMENSION) ||
              (status_nat1 != VXLIB_ERR_CONVOLVE_INVALID_DIMENSION) || (status_opt1 != VXLIB_ERR_CONVOLVE_INVALID_DIMENSION) ||
              (status_nat2 != VXLIB_ERR_CONVOLVE_INVALID_DIMENSION) || (status_opt2 != VXLIB_ERR_CONVOLVE_INVALID_DIMENSION));
         break;
      case 1005:
         /* Init   Coverage */
         // Invalid Dtype
         bufParamsIn0.dim_x    = 16;
         bufParamsIn0.dim_y    = 16;
         bufParamsIn0.stride_y = 64;

         bufParamsOut.dim_x     = 16;
         bufParamsOut.dim_y     = 16;
         bufParamsOut.stride_y  = 64;
         bufParamsIn0.data_type = VXLIB_INT64;
         bufParamsOut.data_type = VXLIB_FLOAT16;

         kerInitArgs.funcStyle = VXLIB_FUNCTION_NATC;
         status_nat1           = VXLIB_convolve_init(handle, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);

         bufParamsIn0.data_type = VXLIB_FLOAT16;
         bufParamsOut.data_type = VXLIB_UINT24;

         status_nat2 = VXLIB_convolve_init(handle, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);

         bufParamsIn0.data_type = VXLIB_INT32;
         bufParamsOut.data_type = VXLIB_UINT32;

         status_nat3 = VXLIB_convolve_init(handle, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);

         kerInitArgs.funcStyle  = VXLIB_FUNCTION_OPTIMIZED;
         bufParamsIn0.data_type = VXLIB_FLOAT32;
         bufParamsOut.data_type = 32;
         status_opt1            = VXLIB_convolve_init(handle, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);

         bufParamsIn0.data_type = VXLIB_FLOAT64;
         bufParamsOut.data_type = VXLIB_UINT64;
         status_opt2            = VXLIB_convolve_init(handle, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);

         bufParamsIn0.data_type = VXLIB_UINT128;
         bufParamsOut.data_type = VXLIB_INT128;
         status_opt3            = VXLIB_convolve_init(handle, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);

         bufParamsIn0.data_type = VXLIB_UINT16;
         bufParamsOut.data_type = VXLIB_INT8;
         status_opt            = VXLIB_convolve_init(handle, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);

         currentTestFail = ((status_nat1 != VXLIB_ERR_INVALID_TYPE) || (status_opt1 != VXLIB_ERR_INVALID_TYPE) ||
                            (status_nat2 != VXLIB_ERR_INVALID_TYPE) || (status_opt2 != VXLIB_ERR_INVALID_TYPE) ||
                            (status_nat3 != VXLIB_ERR_INVALID_TYPE) || (status_opt3 != VXLIB_ERR_INVALID_TYPE) ||
                            (status_opt  != VXLIB_ERR_INVALID_TYPE));
         break;
      case 1006:
         /* exec CheckPArams   Coverage */
         // NULL ptr
         bufParamsIn0.dim_x    = 16;
         bufParamsIn0.dim_y    = 16;
         bufParamsIn0.stride_y = 64;

         bufParamsOut.dim_x    = 16;
         bufParamsOut.dim_y    = 16;
         bufParamsOut.stride_y = 64;

         bufParamsIn0.data_type = VXLIB_UINT8;
         bufParamsOut.data_type = VXLIB_INT16;
         bufParamsFilter.data_type = VXLIB_INT16;

         kerInitArgs.funcStyle = VXLIB_FUNCTION_NATC;
         status_nat1           = VXLIB_convolve_exec_checkParams(NULL, pIn0, pFilter, pOut);

         status_nat2 = VXLIB_convolve_exec_checkParams(handle, NULL, pFilter, pOut);

         kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;

         status_opt1     = VXLIB_convolve_exec_checkParams(handle, pIn0, NULL, pOut);
         status_opt2     = VXLIB_convolve_exec_checkParams(handle, pIn0, pFilter, NULL);
         currentTestFail = ((status_nat1 != VXLIB_ERR_NULL_POINTER) || (status_nat2 != VXLIB_ERR_NULL_POINTER) ||
                            (status_nat2 != VXLIB_ERR_NULL_POINTER) || (status_opt1 != VXLIB_ERR_NULL_POINTER));
         break;
      case 1007:
                  /* exec CheckPArams   Coverage */
         // NULL ptr
         bufParamsIn0.dim_x    = 16;
         bufParamsIn0.dim_y    = 16;
         bufParamsIn0.stride_y = 64;

         bufParamsOut.dim_x    = 16;
         bufParamsOut.dim_y    = 16;
         bufParamsOut.stride_y = 64;
         
         kerInitArgs.padLeft        = 0;
         kerInitArgs.padRight       = 2;
         kerInitArgs.padTop         = 0;
         kerInitArgs.padBottom      = 0;
         kerInitArgs.filterHeight   = 3;
         kerInitArgs.filterWidth    = 3;
         kerInitArgs.scale          = 30;

         bufParamsIn0.data_type = VXLIB_UINT8;
         bufParamsOut.data_type = VXLIB_INT16;
         bufParamsFilter.data_type = VXLIB_INT16;

         kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;
         /* 3x3 */
         status_opt            = VXLIB_convolve_init(handle, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);

         kerInitArgs.padLeft        = 0;
         kerInitArgs.padRight       = 2;
         kerInitArgs.padTop         = 0;
         kerInitArgs.padBottom      = 0;
         kerInitArgs.filterHeight   = 5;
         kerInitArgs.filterWidth    = 5;
         kerInitArgs.scale          = 30;
         /* 5x5 */
         status_opt1           = VXLIB_convolve_init(handle, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         
         kerInitArgs.padLeft        = 0;
         kerInitArgs.padRight       = 2;
         kerInitArgs.padTop         = 0;
         kerInitArgs.padBottom      = 0;
         kerInitArgs.filterHeight   = 7;
         kerInitArgs.filterWidth    = 7;
         kerInitArgs.scale          = 30;         
         /* 7x7 */         
         status_opt2           = VXLIB_convolve_init(handle, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         
         kerInitArgs.padLeft        = 0;
         kerInitArgs.padRight       = 2;
         kerInitArgs.padTop         = 0;
         kerInitArgs.padBottom      = 0;
         kerInitArgs.filterHeight   = 9;
         kerInitArgs.filterWidth    = 9;
         kerInitArgs.scale          = 30;            
         /* 9x9 */         
         status_opt3           = VXLIB_convolve_init(handle, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);

         kerInitArgs.padLeft        = 0;
         kerInitArgs.padRight       = 2;
         kerInitArgs.padTop         = 0;
         kerInitArgs.padBottom      = 0;
         kerInitArgs.filterHeight   = 13;
         kerInitArgs.filterWidth    = 13;
         kerInitArgs.scale          = 30;            
         /* 9x9 */         
         status_opt4           = VXLIB_convolve_init(handle, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);

         kerInitArgs.funcStyle = VXLIB_FUNCTION_NATC;
         status_nat            = VXLIB_convolve_init(handle, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);

         kerInitArgs.padLeft        = 0;
         kerInitArgs.padRight       = 0;
         kerInitArgs.padTop         = 0;
         kerInitArgs.padBottom      = 1;
         kerInitArgs.filterHeight   = 3;
         kerInitArgs.filterWidth    = 3;
         kerInitArgs.scale          = 30;

         bufParamsIn0.data_type = VXLIB_UINT8;
         bufParamsOut.data_type = VXLIB_INT16;
         bufParamsFilter.data_type = VXLIB_INT16;

         kerInitArgs.funcStyle = VXLIB_FUNCTION_NATC;
         status_nat            = VXLIB_convolve_init(handle, &bufParamsIn0, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
         status_nat            = VXLIB_convolve_exec(handle, pIn0, pFilter, pOut);

         currentTestFail = ((status_opt  != VXLIB_ERR_CONVOLVE_PADDED_NOT_IMPLEMENTED) || (status_opt1 != VXLIB_ERR_CONVOLVE_PADDED_NOT_IMPLEMENTED) ||
                            (status_nat  != VXLIB_ERR_CONVOLVE_PADDED_NOT_IMPLEMENTED) || (status_opt2 != VXLIB_ERR_CONVOLVE_PADDED_NOT_IMPLEMENTED) ||
                            (status_opt3 != VXLIB_ERR_CONVOLVE_PADDED_NOT_IMPLEMENTED) || (status_opt4 != VXLIB_ERR_CONVOLVE_PADDED_NOT_IMPLEMENTED));
         break;
      default:
         break;
      }

      fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;

      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum++, 0, 0, 0, currentTestFail, desc);
   }
   TI_align_free(pIn0);
   TI_align_free(pFilter);
   TI_align_free(pOut);

   free(handle);

   return fail;
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

   fail = coverage_test_main();
   if (fail == 0)
      printf("Test Pass!\n");
   else
      printf("Test Fail!\n");
   return fail;
}
#endif
