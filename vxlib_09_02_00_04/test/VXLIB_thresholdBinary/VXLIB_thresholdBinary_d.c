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
#include "VXLIB_thresholdBinary.h"
#include "VXLIB_thresholdBinary_idat.h"
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

int16_t volatile volatileSum = 0; // use volatile to keep compiler from removing this operation

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_thresholdBinary_d                                                                                                        */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method performs regerssion testing for the VXLIB_thresholdBinary kernel
int VXLIB_thresholdBinary_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
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

#if defined(EVM_TEST)
   uint32_t k = 0;
#endif

   // performance estimation
   uint64_t archCycles, estCycles;

   // data structure to hold test cases parameters
   VXLIB_thresholdBinary_testParams_t *prm;
   VXLIB_thresholdBinary_testParams_t  currPrm;

   // get total number of test cases
   VXLIB_thresholdBinary_getTestParams(&prm, &test_cases);

   // initialization args for kernel
   VXLIB_thresholdBinary_InitArgs kerInitArgs;

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT  VXLIB_thresholdBinary_d CP 0\n");
#endif

   // create kernel's handle
   int32_t            handleSize = VXLIB_thresholdBinary_getHandleSize(&kerInitArgs);
   VXLIB_kernelHandle handle     = malloc(handleSize);

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT  VXLIB_thresholdBinary_d CP 1\n");
#endif

   TI_profile_init("VXLIB_thresholdBinary");

#if !defined(EVM_TEST)
   // file IO for benchmarking
   // FILE *fpOutputCSV = fopen("VXLIB_thresholdBinary.csv", "w+");
   // fprintf(fpOutputCSV, "Kernel, Test ID, Test type, Bit Width, Parameters, width, height, Arch"
   //                      "cycles, Loki cycles, Loki/Arch cycles, Pass/Fail\n");
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

         bufParamsIn.data_type = currPrm.dTypeIn;
         bufParamsIn.dim_x     = currPrm.width;
         bufParamsIn.dim_y     = currPrm.height;
         bufParamsIn.stride_y  = currPrm.strideIn;

         bufParamsOut.data_type = currPrm.dTypeOut;
         bufParamsOut.dim_x     = currPrm.width;
         bufParamsOut.dim_y     = currPrm.height;
         bufParamsOut.stride_y  = currPrm.strideOut;

         int64_t thresholdVal   = currPrm.thresholdVal;
         int64_t trueVal  = currPrm.trueVal;
         int64_t falseVal = currPrm.falseVal;
         /*****************************************/
         /* Create buffers for input and outputs  */
         /*****************************************/

         size_t inpSize = bufParamsIn.dim_y * bufParamsIn.stride_y;
         size_t outSize  = bufParamsOut.dim_y * bufParamsOut.stride_y;

         void *pIn = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, inpSize);
         void *pOut = NULL, *pOutCn = NULL;

         void *pThresholdVal, *pTrueVal, *pFalseVal;

         if (currPrm.outputDataLocation == VXLIB_TEST_OUTPUT_HEAP) {
            pOut   = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, outSize);
            pOutCn = (void *) malloc(outSize);
         }
         else if (currPrm.outputDataLocation == VXLIB_TEST_OUTPUT_MSMC) {
            pOut = (void *) msmcBuffer;
            pOutCn = (void *) ddrBuffer;
         }
         // only run the test if the buffer allocations fit in the heap
         if (pIn && pOut && pOutCn) {

            // fill buffers
            TI_fillBuffer(prm[tpi].testPattern, 0, pIn, prm[tpi].staticIn, bufParamsIn.dim_x, bufParamsIn.dim_y,
                          bufParamsIn.stride_y, VXLIB_sizeof(currPrm.dTypeIn), testPatternString);



            pTrueVal  = (void *) &trueVal;
            pFalseVal = (void *) &falseVal;
            pThresholdVal   = (void *) &thresholdVal;
#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_thresholdBinary_d CP 0\n");
#endif

#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_thresholdBinary_d CP 1 status_init %d\n", status_init);
#endif
            // check parameters for initializing the kernel and initialize kernel if successful
            status_init = VXLIB_thresholdBinary_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
            if (status_init == VXLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               VXLIB_asm(" MARK 0");
               kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;
               status_init = VXLIB_thresholdBinary_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
               VXLIB_asm(" MARK 1");
               TI_profile_stop();
            }
            else {
               printf("status_init failed: %d", status_init);
            }

            // check execution parameters and call optimized kernel if sucessful
            status_opt = VXLIB_thresholdBinary_exec_checkParams(handle, pIn, pOut, pThresholdVal, pTrueVal, pFalseVal);
            if (status_opt == VXLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_OPT);
               VXLIB_asm(" MARK 2");
               status_opt = VXLIB_thresholdBinary_exec(handle, pIn, pOut, pThresholdVal, pTrueVal, pFalseVal);
               VXLIB_asm(" MARK 3");
               TI_profile_stop();
            }
            else {
               printf("status_opt failed: %d", status_opt);
            }

#if defined(EVM_TEST)
            // the following for loop is to call kernel repeatedly so as to train the branch predictor
            for (k = 0; k < (VXLIB_WARM_REPS); k++) {
            // for (k = 0; k < (4); k++) {
               // run warm instruction cache test
               TI_profile_clear_cycle_count_single(TI_PROFILE_KERNEL_OPT_WARM);
               TI_profile_start(TI_PROFILE_KERNEL_OPT_WARM);
               VXLIB_asm(" MARK 4");
               status_opt = VXLIB_thresholdBinary_exec(handle, pIn, pOut, pThresholdVal, pTrueVal, pFalseVal);
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
               status_opt = VXLIB_thresholdBinary_exec(handle, pIn, pOut, pThresholdVal, pTrueVal, pFalseVal);
            VXLIB_asm(" MARK 7");
            TI_profile_stop();
#endif // if defined(EVM_TEST)

            /**************************************/
            /* Test natural C code for the kernel */
            /**************************************/

            // initialize the kernel function style to natural C
            kerInitArgs.funcStyle = VXLIB_FUNCTION_NATC;

            // call init once again to use the natural C variant
            VXLIB_thresholdBinary_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

            TI_profile_start(TI_PROFILE_KERNEL_CN);
            VXLIB_asm(" MARK 8");

               status_nat = VXLIB_thresholdBinary_exec(handle, pIn, pOutCn, pThresholdVal, pTrueVal, pFalseVal);
            VXLIB_asm(" MARK 9");
            TI_profile_stop();
#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_thresholdBinary_d CP 6 status_nat %d\n", status_nat);
#endif

            /*********************************************************************/
            /* Compare results with natural C output and static reference output */
            /*********************************************************************/

            status_nat_vs_opt =
                TI_compare_mem_2D((void *) pOut, (void *) pOutCn, 0, 0, bufParamsOut.dim_x, bufParamsOut.dim_y,
                                  bufParamsOut.stride_y, VXLIB_sizeof(currPrm.dTypeOut));

            comparisonDone = 1;
#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_thresholdBinary_d CP 7 comparisonDone %d status_nat_vs_opt %d\n", comparisonDone,
                   status_nat_vs_opt);
#endif

            if (currPrm.staticOut != NULL) {
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
            printf("VXLIB_DEBUGPRINT  VXLIB_thresholdBinary_d CP 8 fail %d\n", fail);
#endif

            // update profiling information
            pProfile[3 * tpi]     = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
            pProfile[3 * tpi + 1] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[3 * tpi + 2] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);

            sprintf(desc, "%s | R = %ld | C = %ld | Datatype = %d", 
                    testPatternString, currPrm.height,
                    currPrm.width,bufParamsIn.data_type );

#if !defined(EVM_TEST)
            // write to CSV, must happen prior to write to screen because TI_profile_formula_add clears values in
            // counters
            /* fprintf(fpOutputCSV, "add, %d, %d, %d, %lu, %lu, %lu, %lu, %.2f, %d\n", testNum, currPrm.testPattern, */
            /*         VXLIB_sizeof(bufParamsIn0.data_type) * 8, currPrm.width, currPrm.height, archCycles, */
            /*         cycles[TI_PROFILE_KERNEL_OPT], */
            /*         ((VXLIB_F32) cycles[TI_PROFILE_KERNEL_OPT]) / ((VXLIB_F32) archCycles), !currentTestFail); */
#endif // #if !defined(EVM_TEST)

            VXLIB_thresholdBinary_perfEst(handle, &archCycles, &estCycles);
            TI_profile_add_test(testNum++, currPrm.width * currPrm.height, archCycles, estCycles, currentTestFail, desc);
         }
         else {
            sprintf(desc, "width =%ld, height = %ld ", currPrm.width, currPrm.height);
            TI_profile_skip_test(desc);

         } // end of memory allocation successful?

         /*************************************/
         /* Free buffers for each test vector */
         /*************************************/

         if (currPrm.outputDataLocation == VXLIB_TEST_OUTPUT_HEAP) {
            free(pOutCn);
            TI_align_free(pOut);
         }
         else if (currPrm.outputDataLocation == VXLIB_TEST_OUTPUT_MSMC){
         }
         TI_align_free(pIn);
      } // end repetitions
   }    // end idat test cases

   free(handle);

   /* Close results CSV */
   /* fclose(fpOutputCSV); */

   return fail;
}
/**********************************************************************************************************************/

/*                                                                                                                    */

/* coverage_test_main                                                                                                          */

/*                                                                                                                    */

/**********************************************************************************************************************/

int coverage_test_main()
{
   int32_t                  testNum         = 1000;
   int32_t                  currentTestFail = 0;
   VXLIB_STATUS            status_nat;
   VXLIB_STATUS            status_opt;
   VXLIB_bufParams2D_t bufParamsIn, bufParamsOut;
   VXLIB_thresholdBinary_InitArgs kerInitArgs;
   int32_t                  handleSize = VXLIB_thresholdBinary_getHandleSize(&kerInitArgs);
   VXLIB_kernelHandle      handle     = malloc(handleSize);
   int                      fail       = 0;
 
   void *pIn  = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, 32);
   void *pOut = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, 32);
   void *pThresholdVal = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, 1);
   void *pTrueVal = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, 1);
   void *pFalseVal = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, 1);

   while (testNum <= 1013) {

   bufParamsIn.data_type  = VXLIB_UINT8;
   bufParamsIn.dim_x      = 32;
   bufParamsIn.dim_y      = 32;
   bufParamsIn.stride_y   = 32;
   bufParamsOut.data_type = VXLIB_UINT8;
   bufParamsOut.dim_x     = 32;
   bufParamsOut.dim_y     = 32;
   bufParamsOut.stride_y  = 32;


      switch (testNum) {

      case 1000: 

         kerInitArgs.funcStyle  = VXLIB_FUNCTION_OPTIMIZED;
         status_opt             = VXLIB_thresholdBinary_init_checkParams(NULL,  &bufParamsIn, &bufParamsOut, &kerInitArgs);
         currentTestFail        = (status_opt != VXLIB_ERR_NULL_POINTER) ? 1 : 0;

         status_opt             = VXLIB_thresholdBinary_init_checkParams(handle, NULL, &bufParamsOut, &kerInitArgs);
         currentTestFail        = ((currentTestFail == 1) || (status_opt != VXLIB_ERR_NULL_POINTER)) ? 1 : 0;

         status_opt             = VXLIB_thresholdBinary_init_checkParams(handle,  &bufParamsIn, NULL, &kerInitArgs);
         currentTestFail        = ((currentTestFail == 1) || (status_opt != VXLIB_ERR_NULL_POINTER)) ? 1 : 0;

         status_opt             = VXLIB_thresholdBinary_init_checkParams(handle,  &bufParamsIn, &bufParamsOut, NULL);
         currentTestFail        = ((currentTestFail == 1) || (status_opt != VXLIB_ERR_NULL_POINTER)) ? 1 : 0;
         break;
      case 1001:

         bufParamsOut.dim_x     = 16;
         bufParamsOut.dim_y     = 16;
         bufParamsOut.stride_y  = 16;
         kerInitArgs.funcStyle  = VXLIB_FUNCTION_OPTIMIZED;
         status_opt             = VXLIB_thresholdBinary_init_checkParams(handle, &bufParamsIn, &bufParamsOut,&kerInitArgs);
         currentTestFail        = (status_opt != VXLIB_ERR_INVALID_DIMENSION) ? 1 : 0;
         break;

      case 1002:

         bufParamsIn.stride_y   = 16;
         kerInitArgs.funcStyle  = VXLIB_FUNCTION_OPTIMIZED;
         status_opt             = VXLIB_thresholdBinary_init_checkParams(handle, &bufParamsIn, &bufParamsOut,&kerInitArgs);
         currentTestFail        = (status_opt != VXLIB_ERR_NOT_EQUAL_WIDTH_STRIDE) ? 1 : 0;
         break;

      case 1003:

         bufParamsOut.stride_y  = 16;
         kerInitArgs.funcStyle  = VXLIB_FUNCTION_OPTIMIZED;
         status_opt             = VXLIB_thresholdBinary_init_checkParams(handle, &bufParamsIn, &bufParamsOut,&kerInitArgs);
         currentTestFail        = (status_opt != VXLIB_ERR_NOT_EQUAL_WIDTH_STRIDE) ? 1 : 0;
         break;

      case 1004:

         bufParamsIn.data_type  = VXLIB_UINT32;
         bufParamsOut.data_type = VXLIB_UINT32;
         bufParamsIn.stride_y   = 128;
         bufParamsOut.stride_y  = 128;
         kerInitArgs.funcStyle  = VXLIB_FUNCTION_OPTIMIZED;
         status_opt             = VXLIB_thresholdBinary_init_checkParams(handle, &bufParamsIn, &bufParamsOut,&kerInitArgs);      
         currentTestFail        = (status_opt != VXLIB_ERR_INVALID_TYPE) ? 1 : 0;
         break;

      case 1005:

         bufParamsIn.data_type  = VXLIB_UINT24;
         bufParamsOut.data_type = VXLIB_FLOAT16;
         kerInitArgs.funcStyle  = VXLIB_FUNCTION_NATC;
         status_nat             = VXLIB_thresholdBinary_init(handle, &bufParamsIn, &bufParamsOut,&kerInitArgs);   
         kerInitArgs.funcStyle  = VXLIB_FUNCTION_OPTIMIZED;
         status_opt             = VXLIB_thresholdBinary_init(handle, &bufParamsIn, &bufParamsOut,&kerInitArgs);      
         currentTestFail        = ((status_opt != VXLIB_ERR_INVALID_TYPE) || (status_nat != VXLIB_ERR_INVALID_TYPE));
         break;

      case 1006:

         kerInitArgs.funcStyle  = VXLIB_FUNCTION_OPTIMIZED;
         status_opt             = VXLIB_thresholdBinary_exec_checkParams(NULL, pIn, pOut, pThresholdVal, pTrueVal, pFalseVal);
         kerInitArgs.funcStyle  = VXLIB_FUNCTION_NATC;
         status_nat             = VXLIB_thresholdBinary_exec_checkParams(handle, pIn, pOut, NULL, pTrueVal, pFalseVal);
         currentTestFail        = ((status_opt != VXLIB_ERR_NULL_POINTER) || (status_nat != VXLIB_ERR_NULL_POINTER));
         break;

      case 1007:

         kerInitArgs.funcStyle  = VXLIB_FUNCTION_OPTIMIZED;
         status_opt             = VXLIB_thresholdBinary_exec_checkParams(handle, NULL, pOut, pThresholdVal, pTrueVal, pFalseVal);
         kerInitArgs.funcStyle  = VXLIB_FUNCTION_NATC;
         status_nat             = VXLIB_thresholdBinary_exec_checkParams(handle, pIn, pOut, pThresholdVal, NULL, pFalseVal);
         currentTestFail        = ((status_opt != VXLIB_ERR_NULL_POINTER) || (status_nat != VXLIB_ERR_NULL_POINTER));
         break;

      case 1008:

         kerInitArgs.funcStyle  = VXLIB_FUNCTION_OPTIMIZED;
         status_opt             = VXLIB_thresholdBinary_exec_checkParams(handle, pIn, NULL, pThresholdVal, pTrueVal, pFalseVal);
         kerInitArgs.funcStyle  = VXLIB_FUNCTION_NATC;
         status_nat             = VXLIB_thresholdBinary_exec_checkParams(handle, pIn, pOut, pThresholdVal, pTrueVal, NULL);
         currentTestFail        = ((status_opt != VXLIB_ERR_NULL_POINTER) || (status_nat != VXLIB_ERR_NULL_POINTER));
         break; 


      case 1009:

         bufParamsIn.data_type  = VXLIB_UINT32;
         bufParamsOut.data_type = VXLIB_INT32;
         kerInitArgs.funcStyle  = VXLIB_FUNCTION_OPTIMIZED;
         status_opt             = VXLIB_thresholdBinary_init(handle, &bufParamsIn, &bufParamsOut,&kerInitArgs);      
         currentTestFail        = (status_opt != VXLIB_ERR_INVALID_TYPE) ? 1 : 0;
         break;


      case 1010:

         bufParamsIn.data_type  = VXLIB_UINT64;
         bufParamsOut.data_type = VXLIB_INT64;
         kerInitArgs.funcStyle  = VXLIB_FUNCTION_OPTIMIZED;
         status_opt             = VXLIB_thresholdBinary_init(handle, &bufParamsIn, &bufParamsOut,&kerInitArgs);      
         currentTestFail        = (status_opt != VXLIB_ERR_INVALID_TYPE) ? 1 : 0;
         break;

      case 1011:

         bufParamsIn.data_type  = VXLIB_UINT128;
         bufParamsOut.data_type = VXLIB_INT128;
         kerInitArgs.funcStyle  = VXLIB_FUNCTION_OPTIMIZED;
         status_opt             = VXLIB_thresholdBinary_init(handle, &bufParamsIn, &bufParamsOut,&kerInitArgs);      
         currentTestFail        = (status_opt != VXLIB_ERR_INVALID_TYPE) ? 1 : 0;
         break;

      case 1012:

         bufParamsIn.data_type  = VXLIB_FLOAT32;
         bufParamsOut.data_type = VXLIB_FLOAT64;
         kerInitArgs.funcStyle  = VXLIB_FUNCTION_OPTIMIZED;
         status_opt             = VXLIB_thresholdBinary_init(handle, &bufParamsIn, &bufParamsOut,&kerInitArgs);      
         currentTestFail        = (status_opt != VXLIB_ERR_INVALID_TYPE) ? 1 : 0;
      case 1013:

         bufParamsIn.data_type  = VXLIB_FLOAT32;
         bufParamsOut.data_type = 45;
         kerInitArgs.funcStyle  = VXLIB_FUNCTION_OPTIMIZED;
         status_opt             = VXLIB_thresholdBinary_init(handle, &bufParamsIn, &bufParamsOut,&kerInitArgs);      
         currentTestFail        = (status_opt != VXLIB_ERR_INVALID_TYPE) ? 1 : 0;
         break;
      default:
         break;
      }

   fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;
   sprintf(desc, "%s", "COVERAGE TEST");
   TI_profile_add_test(testNum++, 0, 0, 0, currentTestFail, desc);
   }
   TI_align_free(pIn);
   TI_align_free(pOut);
   free(handle);
   TI_align_free(pThresholdVal);
   TI_align_free(pTrueVal);
   TI_align_free(pFalseVal);
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
      TI_memError("VXLIB_thresholdBinary");
      return 1;
   }
   else
#else
   printf("_HOST_BUILD is defined.\n");
#endif
   {
      return VXLIB_thresholdBinary_d(&pProfile[0], 0);
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
   if(fail == 0)
      printf("Coverage Test Pass!\n");
   else
      printf("Coverage Test Fail!!\n");
   return fail;
}
#endif
