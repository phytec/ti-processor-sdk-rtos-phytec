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

// include VXLIB
#include <vxlib.h>

// include test infrastructure provided by VXLIB
#include "../common/VXLIB_test.h"

// include test data for this kernel
#include "TI_test.h"
#include "VXLIB_convertDepth_idat.h"

#ifdef WIN32
int8_t msmcBuffer[VXLIB_L3_RESULTS_BUFFER_SIZE];
int8_t ddrBuffer[2048 * 1024];
#else
__attribute__((section(".msmcData"), aligned(64))) int8_t msmcBuffer[VXLIB_L3_RESULTS_BUFFER_SIZE];
__attribute__((section(".ddrData"), aligned(64))) int8_t  ddrBuffer[2048 * 1024];

#endif // WIN32

int16_t volatile volatileSum = 0; // use volatile to keep compiler from removing this operation

int VXLIB_convertDepth_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   int32_t             tpi;
   int32_t             currentTestFail;
   int32_t             fail = 0;
   uint32_t            repCount;
   uint32_t            numReps;
   VXLIB_bufParams2D_t bufParamsIn0, bufParamsOut;
   uint32_t            k = 0;

   uint32_t testNum;
   // uint64_t archCycles, estCycles;

   VXLIB_convertDepth_testParams_t *prm;
   VXLIB_convertDepth_testParams_t  currPrm;
   VXLIB_convertDepth_getTestParams(&prm, &test_cases);

   VXLIB_convertDepth_InitArgs kerInitArgs;

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT  VXLIB_convertDepth_d CP 0\n");
#endif

   int32_t            handleSize = VXLIB_convertDepth_getHandleSize(&kerInitArgs);
   VXLIB_kernelHandle handle     = malloc(handleSize);

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT  VXLIB_convertDepth_d CP 1\n");
#endif

   TI_profile_init("VXLIB_convertDepth");

#if !defined(EVM_TEST)
   // file IO for benchmarking
   FILE *fpOutputCSV = fopen("VXLIB_convertDepth.csv", "w+");
   fprintf(fpOutputCSV, "Kernel, Test ID, Test type, Bit Width, Parameters, width, height, Arch"
                        "cycles, Loki cycles, Loki/Arch cycles, Pass/Fail\n");
#endif // #if defined(EVM_TEST)

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

         bufParamsIn0.data_type = currPrm.dataType;
         bufParamsIn0.dim_x     = currPrm.width;
         bufParamsIn0.dim_y     = currPrm.height;
         bufParamsIn0.stride_y  = currPrm.strideIn;

         bufParamsOut.data_type = (currPrm.dataType == VXLIB_UINT8) ? VXLIB_INT16 : VXLIB_UINT8;
         bufParamsOut.dim_x     = currPrm.width;
         bufParamsOut.dim_y     = currPrm.height;
         bufParamsOut.stride_y  = currPrm.strideOut;

         kerInitArgs.overFlowPolicy = currPrm.overFlowPolicy;
         kerInitArgs.shift          = currPrm.shift;

         // Here, stride is in bytes
         uint32_t inp0SizeBytes = bufParamsIn0.dim_y * bufParamsIn0.stride_y;
         uint32_t outSizeBytes  = bufParamsOut.dim_y * bufParamsOut.stride_y;

         void *pIn0 = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, inp0SizeBytes);

         void *pOut = NULL, *pOutCn = NULL;
         if (currPrm.outputDataLocation == VXLIB_TEST_OUTPUT_HEAP) {
            pOut   = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, outSizeBytes);
            pOutCn = (void *) malloc(outSizeBytes);
         }
         else if (currPrm.outputDataLocation == VXLIB_TEST_OUTPUT_MSMC) {
            pOut = (void *) msmcBuffer;
            pOutCn = (void *) ddrBuffer;
         }
         else {
            pOut = (void *) msmcBuffer;
            pOutCn = (void *) ddrBuffer;
         }
         /* Only run the test if the buffer allocations fit in the heap */
         if (pIn0 && pOut && pOutCn) {

            /* No need to fill anything here, we just used patter 0xAB
             * for testing purposes.                                    */

            if (currPrm.dataType == VXLIB_UINT8 || currPrm.dataType == VXLIB_INT16) {
               TI_fillBuffer(prm[tpi].testPattern, 0, pIn0, prm[tpi].staticIn0, bufParamsIn0.dim_x, bufParamsIn0.dim_y,
                             bufParamsIn0.stride_y, VXLIB_sizeof(currPrm.dataType), testPatternString);
            }
            else {
            }

#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_convertDepth_d CP 0\n");
#endif

#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_convertDepth_d CP 1 status_init %d\n", status_init);
#endif
            if (status_init == VXLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               VXLIB_asm(" MARK 0");
               kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;
               status_init           = VXLIB_convertDepth_init(handle, &bufParamsIn0, &bufParamsOut, &kerInitArgs);
               VXLIB_asm(" MARK 1");
               TI_profile_stop();
            }
#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_convertDepth_d CP 2 status_init %d\n", status_init);
#endif
            status_opt = VXLIB_convertDepth_exec_checkParams(handle, pIn0, pOut);
#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_convertDepth_d CP 3 status_opt %d\n", status_opt);
#endif
            if (status_opt == VXLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_OPT);
               VXLIB_asm(" MARK 2");
               status_opt = VXLIB_convertDepth_exec(handle, pIn0, pOut);
               VXLIB_asm(" MARK 3");
               TI_profile_stop();
            }

            /* The following for loop is to call kernel repeatedly so as to
             * train the branch predictor                                   */
            for (k = 0; k < (VXLIB_WARM_REPS); k++) {
               // run warm instruction cache test
               TI_profile_clear_cycle_count_single(TI_PROFILE_KERNEL_OPT_WARM);
               TI_profile_start(TI_PROFILE_KERNEL_OPT_WARM);
               VXLIB_asm(" MARK 4");
               status_opt = VXLIB_convertDepth_exec(handle, pIn0, pOut);
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
            status_opt = VXLIB_convertDepth_exec(handle, pIn0, pOut);
            VXLIB_asm(" MARK 7");
            TI_profile_stop();

            /* Test _cn kernel */

            kerInitArgs.funcStyle = VXLIB_FUNCTION_NATC;
#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_convertDepth_d CP 4 status_opt %d\n", status_opt);
#endif
            // initialize the kernel to use the natural C variant
            VXLIB_convertDepth_init(handle, &bufParamsIn0, &bufParamsOut, &kerInitArgs);
#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_convertDepth_d CP 5\n");
#endif
            TI_profile_start(TI_PROFILE_KERNEL_CN);
            VXLIB_asm(" MARK 8");
            status_nat = VXLIB_convertDepth_exec(handle, pIn0, pOutCn);
            VXLIB_asm(" MARK 9");
            TI_profile_stop();
#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_convertDepth_d CP 6 status_nat %d\n", status_nat);
#endif
            if (currPrm.dataType == VXLIB_UINT8 || currPrm.dataType == VXLIB_INT16) {
               status_nat_vs_opt =
                   TI_compare_mem_2D((void *) pOut, (void *) pOutCn, 0, 0, bufParamsOut.dim_x, bufParamsOut.dim_y,
                                     bufParamsOut.stride_y, VXLIB_sizeof(bufParamsOut.data_type));
            }
            else {
            }

            comparisonDone = 1;
#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_convertDepth_d CP 7 comparisonDone %d status_nat_vs_opt %d\n",
                   comparisonDone, status_nat_vs_opt);
#endif

            if (currPrm.staticOut != NULL) {
               if (currPrm.dataType == VXLIB_UINT8 || currPrm.dataType == VXLIB_INT16) {
                  status_ref_vs_opt = TI_compare_mem_roi2DStatic(
                      (void *) pOut, (void *) currPrm.staticOut, 0, 0, bufParamsOut.dim_x, bufParamsOut.dim_y,
                      bufParamsOut.stride_y, VXLIB_sizeof(bufParamsOut.data_type));
               }
               else {
               }

               comparisonDone = 1;
            }
            else {
               /* Set to pass since it wasn't supposed to run. */
               status_ref_vs_opt = TI_TEST_KERNEL_PASS;
            }

#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_convertDepth_d CP 8 status_nat_vs_opt %d status_ref_vs_opt %d "
                   "currentTestFail %d\n",
                   status_nat_vs_opt, status_ref_vs_opt, currentTestFail);
            printf("VXLIB_DEBUGPRINT  VXLIB_convertDepth_d CP 8 status_init %d status_opt %d status_nat %d\n",
                   status_init, status_opt, status_nat);
#endif
            /* Set the 'fail' flag based on test vector comparison results */
            currentTestFail =
                ((status_nat_vs_opt == TI_TEST_KERNEL_FAIL) || (status_ref_vs_opt == TI_TEST_KERNEL_FAIL) ||
                 (status_init != VXLIB_SUCCESS) || (status_opt != VXLIB_SUCCESS) || (status_nat != VXLIB_SUCCESS) ||
                 (comparisonDone == 0) || (currentTestFail == 1))
                    ? 1
                    : 0;

            fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;
#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_convertDepth_d CP 8 fail %d\n", fail);
#endif

            pProfile[3 * tpi]     = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
            pProfile[3 * tpi + 1] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[3 * tpi + 2] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);

            sprintf(desc, "%s generated input | height = %d, width = %d", testPatternString, currPrm.height,
                    currPrm.width);

#if !defined(EVM_TEST)
            // write to CSV, must happen prior to write to screen because TI_profile_formula_add clears values in
            // counters
            // fprintf(fpOutputCSV, "convertDepth, %d, %d, %d, %lu, %lu, %lu, %lu, %.2f, %d\n", testNum,
            // currPrm.testPattern,
            //         VXLIB_sizeof(bufParamsIn0.data_type) * 8, currPrm.width, currPrm.height, archCycles,
            //         cycles[TI_PROFILE_KERNEL_OPT],
            //         ((VXLIB_F32) cycles[TI_PROFILE_KERNEL_OPT]) / ((VXLIB_F32) archCycles), !currentTestFail);
#endif // #if !defined(EVM_TEST)

            TI_profile_add_test(testNum++, currPrm.height, currPrm.width, 0, currentTestFail, desc);
         }
         else {
            // sprintf(desc, "Data size=%d", currPrm.dataSize);
            TI_profile_skip_test(desc);

            // clear the counters between runs; normally handled by TI_profile_matMul_test
            TI_profile_clear_run_stats();

         } // end of memory allocation successful?

         /* Free buffers for each test vector */
         if (currPrm.outputDataLocation == VXLIB_TEST_OUTPUT_HEAP) {
            free(pOutCn);
            TI_align_free(pOut);
         }
         TI_align_free(pIn0);

      } // end repetitions
   }    // end idat test cases

   free(handle);

   /* Close results CSV */
#if !defined(EVM_TEST)
   fclose(fpOutputCSV);
#endif // #if !defined(EVM_TEST)

   return fail;
}

int test_main(uint32_t *pProfile)
{
#if !defined(_HOST_BUILD)
   if (TI_cache_init()) {
      TI_memError("VXLIB_convertDepth");
      return 1;
   }
   else
#else
   printf("_HOST_BUILD is defined.\n");
#endif
   {
      return VXLIB_convertDepth_d(&pProfile[0], 0);
   }
}


int coverage_test_main()
{
   int32_t             testNum         = 1000;
   int32_t             currentTestFail = 0;
   VXLIB_STATUS        status_nat, status_nat1, status_nat2, status_nat3;
   VXLIB_STATUS        status_opt, status_opt1, status_opt2, status_opt3;
   VXLIB_bufParams2D_t bufParamsIn0, bufParamsOut;

   VXLIB_convertDepth_InitArgs kerInitArgs;
   int32_t                        handleSize = VXLIB_convertDepth_getHandleSize(&kerInitArgs);
   VXLIB_kernelHandle             handle     = malloc(handleSize);

   int   fail = 0;
   void *pIn0 = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, 128 * 64);
   void *pOut = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, 128 * 64);

   bufParamsIn0.dim_x         = 16;
   bufParamsIn0.dim_x         = 16;
   bufParamsIn0.stride_y      = 64;
   bufParamsIn0.data_type     = VXLIB_UINT8;

   bufParamsOut.dim_x     = 16;
   bufParamsOut.dim_x     = 16;
   bufParamsOut.stride_y  = 64;
   bufParamsOut.data_type = VXLIB_UINT8;

   while (testNum <= 1005) {

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
         status_nat  = VXLIB_convertDepth_init_checkParams(NULL, &bufParamsIn0, &bufParamsOut, &kerInitArgs);
         status_nat1 = VXLIB_convertDepth_init_checkParams(handle, NULL, &bufParamsOut, &kerInitArgs);

         kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;
         status_opt1           = VXLIB_convertDepth_init_checkParams(handle, &bufParamsIn0, &bufParamsOut, NULL);
         status_opt2           = VXLIB_convertDepth_init_checkParams(handle, &bufParamsIn0, NULL, &kerInitArgs);
         currentTestFail       = ((status_nat  != VXLIB_ERR_NULL_POINTER) || (status_opt1 != VXLIB_ERR_NULL_POINTER) ||
                                  (status_nat1 != VXLIB_ERR_NULL_POINTER) || (status_opt2 != VXLIB_ERR_NULL_POINTER));
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
         bufParamsIn0.data_type = VXLIB_INT8;
         bufParamsOut.data_type = VXLIB_UINT16;

         kerInitArgs.funcStyle = VXLIB_FUNCTION_NATC;
         status_nat1 = VXLIB_convertDepth_init_checkParams(handle, &bufParamsIn0, &bufParamsOut, &kerInitArgs);
         kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;

         bufParamsIn0.data_type = VXLIB_UINT16;
         bufParamsOut.data_type = VXLIB_INT8;

         status_opt      = VXLIB_convertDepth_init_checkParams(handle, &bufParamsIn0, &bufParamsOut, &kerInitArgs);
         currentTestFail = ((status_nat1 != VXLIB_ERR_INVALID_TYPE) || (status_opt != VXLIB_ERR_INVALID_TYPE));
         break;
      case 1002:
         /* Init Check Params  Coverage */
         // Invalid Dimension
         bufParamsIn0.dim_x    = 17;
         bufParamsIn0.dim_y    = 16;
         bufParamsIn0.stride_y = 64;

         bufParamsOut.dim_x     = 16;
         bufParamsOut.dim_y     = 16;
         bufParamsOut.stride_y  = 64;
         bufParamsIn0.data_type = VXLIB_UINT8;
         bufParamsOut.data_type = VXLIB_UINT8;

         kerInitArgs.funcStyle = VXLIB_FUNCTION_NATC;

         status_nat = VXLIB_convertDepth_init_checkParams(handle, &bufParamsIn0, &bufParamsOut, &kerInitArgs);

         bufParamsIn0.dim_x    = 16;
         bufParamsIn0.dim_y    = 17;
         bufParamsIn0.stride_y = 64;

         bufParamsOut.dim_x    = 16;
         bufParamsOut.dim_y    = 16;
         bufParamsOut.stride_y = 64;

         status_opt      = VXLIB_convertDepth_init_checkParams(handle, &bufParamsIn0, &bufParamsOut, &kerInitArgs);
         currentTestFail = ((status_nat != VXLIB_ERR_INVALID_DIMENSION) || (status_opt != VXLIB_ERR_INVALID_DIMENSION));
         break;
      case 1003:

         bufParamsIn0.data_type = VXLIB_UINT8;
         bufParamsOut.data_type = VXLIB_UINT8;

         kerInitArgs.funcStyle = VXLIB_FUNCTION_NATC;
         bufParamsIn0.dim_x    = 16;
         bufParamsIn0.dim_y    = 16;
         bufParamsIn0.stride_y = 13;

         bufParamsOut.dim_x    = 16;
         bufParamsOut.dim_y    = 16;
         bufParamsOut.stride_y = 64;
         status_nat = VXLIB_convertDepth_init_checkParams(handle, &bufParamsIn0, &bufParamsOut, &kerInitArgs);

         kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;
         bufParamsIn0.dim_x    = 16;
         bufParamsIn0.dim_y    = 16;
         bufParamsIn0.stride_y = 64;

         bufParamsOut.dim_x    = 16;
         bufParamsOut.dim_y    = 16;
         bufParamsOut.stride_y = 13;

         status_opt = VXLIB_convertDepth_init_checkParams(handle, &bufParamsIn0, &bufParamsOut, &kerInitArgs);
         currentTestFail = ((status_nat != VXLIB_ERR_INVALID_DIMENSION) || (status_opt != VXLIB_ERR_INVALID_DIMENSION));
         break;
      case 1004:
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
         status_nat1           = VXLIB_convertDepth_init(handle, &bufParamsIn0, &bufParamsOut, &kerInitArgs);

         bufParamsIn0.data_type = VXLIB_UINT16;
         bufParamsOut.data_type = VXLIB_UINT24;

         status_nat2 = VXLIB_convertDepth_init(handle, &bufParamsIn0, &bufParamsOut, &kerInitArgs);

         bufParamsIn0.data_type = VXLIB_INT32;
         bufParamsOut.data_type = VXLIB_UINT32;

         status_nat3 = VXLIB_convertDepth_init(handle, &bufParamsIn0, &bufParamsOut, &kerInitArgs);

         kerInitArgs.funcStyle  = VXLIB_FUNCTION_OPTIMIZED;
         bufParamsIn0.data_type = VXLIB_FLOAT32;
         bufParamsOut.data_type = 32;
         status_opt1            = VXLIB_convertDepth_init(handle, &bufParamsIn0, &bufParamsOut, &kerInitArgs);

         bufParamsIn0.data_type = VXLIB_FLOAT64;
         bufParamsOut.data_type = VXLIB_UINT64;
         status_opt2            = VXLIB_convertDepth_init(handle, &bufParamsIn0, &bufParamsOut, &kerInitArgs);

         bufParamsIn0.data_type = VXLIB_UINT128;
         bufParamsOut.data_type = VXLIB_INT128;
         status_opt3            = VXLIB_convertDepth_init(handle, &bufParamsIn0, &bufParamsOut, &kerInitArgs);

         bufParamsIn0.data_type = VXLIB_INT8;
         bufParamsOut.data_type = VXLIB_UINT8;
         status_opt             = VXLIB_convertDepth_init(handle, &bufParamsIn0, &bufParamsOut, &kerInitArgs);

         currentTestFail = ((status_opt != VXLIB_ERR_INVALID_TYPE) || (status_nat1 != VXLIB_ERR_INVALID_TYPE) ||
                            (status_opt1 != VXLIB_ERR_INVALID_TYPE) || (status_nat2 != VXLIB_ERR_INVALID_TYPE) ||
                            (status_opt2 != VXLIB_ERR_INVALID_TYPE) || (status_nat3 != VXLIB_ERR_INVALID_TYPE) ||
                            (status_opt3 != VXLIB_ERR_INVALID_TYPE));
         break;
       case 1005:
         /* exec CheckPArams   Coverage */
         // NULL ptr
         bufParamsIn0.dim_x    = 16;
         bufParamsIn0.dim_y    = 16;
         bufParamsIn0.stride_y = 64;

         bufParamsOut.dim_x    = 16;
         bufParamsOut.dim_y    = 16;
         bufParamsOut.stride_y = 64;

         bufParamsIn0.data_type = VXLIB_INT16;
         bufParamsOut.data_type = VXLIB_UINT8;

         kerInitArgs.funcStyle = VXLIB_FUNCTION_NATC;
         status_nat1           = VXLIB_convertDepth_exec_checkParams(NULL, pIn0, pOut);

         status_nat2 = VXLIB_convertDepth_exec_checkParams(handle, NULL, pOut);

         kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;

         status_opt1     = VXLIB_convertDepth_exec_checkParams(handle, pIn0, NULL);
         currentTestFail = ((status_nat1 != VXLIB_ERR_NULL_POINTER) || (status_nat2 != VXLIB_ERR_NULL_POINTER) ||
                            (status_opt1 != VXLIB_ERR_NULL_POINTER));
         break;         
      default:
         break;
      }

      fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;

      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum++, 0, 0, 0, currentTestFail, desc);
   }
   TI_align_free(pIn0);
   TI_align_free(pOut);

   free(handle);

   return fail;
}


/* Main call for inaddidual test projects */
#if !defined(__ONESHOTTEST) && !defined(RTL_TEST)
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
