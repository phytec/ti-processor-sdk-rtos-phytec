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

// include MMALIB
#include <mmalib.h>

// include test infrastructure provided by MMALIB
#include "../../test/MMALIB_test.h"

// include test data for this kernel
#include "MMALIB_LINALG_matrixTranspose_ixX_oxX_idat.h"

__attribute__((section(".msmcData"), aligned(64))) int8_t msmcBuffer[MMALIB_L3_RESULTS_BUFFER_SIZE];

__attribute__ ((section (".ddrData"), aligned (64))) int8_t ddrBuffer[2048 * 1024];

int MMALIB_LINALG_matrixTranspose_ixX_oxX_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   int32_t  tpi;
   int32_t  currentTestFail;
   int32_t  fail = 0;
   uint32_t repCount;
   uint32_t numReps;
   MMALIB_bufParams2D_t bufParamsIn, bufParamsOut;
   int32_t k;
   int32_t inSize, outSize;
   uint32_t testNum;
   uint64_t archCycles, estCycles;

   matrixTranspose_ixX_oxX_testParams_t   *prm;
   matrixTranspose_ixX_oxX_testParams_t   currPrm;
   matrixTranspose_ixX_oxX_getTestParams(&prm, &test_cases);

   MMALIB_LINALG_matrixTranspose_ixX_oxX_InitArgs    kerInitArgs;

   int32_t handleSize = MMALIB_LINALG_matrixTranspose_ixX_oxX_getHandleSize(&kerInitArgs);
   MMALIB_kernelHandle handle = malloc(handleSize);

   TI_profile_init("MMALIB_LINALG_matrixTranspose_ixX_oxX");

   for (tpi=0; tpi < test_cases; tpi++) {
      numReps = prm[tpi].numReps;
      testNum = prm[tpi].testID;
      currPrm = prm[tpi];

      for (repCount = 0; repCount < numReps; repCount++){
         int32_t      status_nat_vs_opt = TI_TEST_KERNEL_FAIL;
         int32_t      status_ref_vs_opt = TI_TEST_KERNEL_FAIL;
         MMALIB_STATUS   status_init = MMALIB_SUCCESS;
         MMALIB_STATUS   status_opt = MMALIB_SUCCESS;
         MMALIB_STATUS   status_nat = MMALIB_SUCCESS;

         /* Sometimes, depending on certain compile flags, the test will be
          * marked PASS even if no comparison is done. This flag is to detect
          * if a comparison was done or not                                   */
         int32_t      comparisonDone = 0;

         currentTestFail = 0;

         inSize = currPrm.strideIn*currPrm.heightIn;
         outSize = currPrm.strideOut*currPrm.heightOut;
         bufParamsIn.dim_x = currPrm.widthIn;
         bufParamsIn.dim_y = currPrm.heightIn;
         bufParamsIn.stride_y = currPrm.strideIn;
         bufParamsIn.data_type = currPrm.dataType;
         bufParamsOut.dim_x = currPrm.widthOut;
         bufParamsOut.dim_y = currPrm.heightOut;
         bufParamsOut.stride_y = currPrm.strideOut;
         bufParamsOut.data_type = currPrm.dataType;

         void *pIn    = (void *)TI_memalign(MMALIB_L2DATA_ALIGNMENT, inSize);

         void *pOut = NULL;
         void *pOutCn = NULL;

         if (currPrm.outputDataLocation == MMALIB_TEST_OUTPUT_HEAP) {
            pOut   = (void *)TI_memalign(MMALIB_L2DATA_ALIGNMENT, outSize);
            pOutCn = (void *)malloc(outSize);
         } 
         else if(currPrm.outputDataLocation == MMALIB_TEST_OUTPUT_MSMC) {
            pOut   = (void *)msmcBuffer;
            pOutCn = (void *)ddrBuffer;
         } else {
            pOut   = (void *)msmcBuffer;
            pOutCn = (void *)ddrBuffer;
         }

         /* Only run the test if the buffer allocations fit in the heap */
         if( pIn && pOut && pOutCn ) {

            TI_fillBuffer(prm[tpi].testPattern,
                          0,
                          pIn, prm[tpi].staticIn,
                          currPrm.widthIn, currPrm.heightIn,
                          currPrm.strideIn,
                          MMALIB_sizeof(currPrm.dataType),
                          testPatternString);

            status_init = MMALIB_LINALG_matrixTranspose_ixX_oxX_init_checkParams(
                          handle,
                          &bufParamsIn,
                          &bufParamsOut,
                          &kerInitArgs);
            if (status_init == MMALIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               MMALIB_asm(" MARK 0");
               kerInitArgs.funcStyle = MMALIB_FUNCTION_OPTIMIZED;
               status_init = MMALIB_LINALG_matrixTranspose_ixX_oxX_init(handle,
                                                                        &bufParamsIn,
                                                                        &bufParamsOut,
                                                                        &kerInitArgs);
               MMALIB_asm(" MARK 1");
               TI_profile_stop();
            }

            status_opt = MMALIB_LINALG_matrixTranspose_ixX_oxX_exec_checkParams(handle,
                                                                                pIn,
                                                                                pOut);

            if(status_opt == MMALIB_SUCCESS){
               TI_profile_start(TI_PROFILE_KERNEL_OPT);
               MMALIB_asm(" MARK 2");
               status_opt = MMALIB_LINALG_matrixTranspose_ixX_oxX_exec(handle,
                                                                       pIn,
                                                                       pOut);
               MMALIB_asm(" MARK 3");
               TI_profile_stop();
            }

            /* The following for loop is to call kernel repeatedly so as to
             * train the branch predictor                                   */
            for (k = 0; k < 4; k++) {
               // run warm instruction cache test
               TI_profile_clear_cycle_count_single(TI_PROFILE_KERNEL_OPT_WARM);
               TI_profile_start(TI_PROFILE_KERNEL_OPT_WARM);
               MMALIB_asm(" MARK 4");
               status_opt = MMALIB_LINALG_matrixTranspose_ixX_oxX_exec(handle,
                                                                       pIn,
                                                                       pOut);
               MMALIB_asm(" MARK 5");
               TI_profile_stop();
            }

            // get output to L1D
            int16_t outSum = 0;
            int16_t volatile volatileSum = 0;  // use volatile to keep compiler from removing this operation
            int8_t *pOutTemp = (int8_t *)pOut;       // treat output as bytes to be data type agnostic
            for (k = 0; k < outSize; k++) {
                outSum += *pOutTemp;
                pOutTemp++;
            }

            // dummy store of outSum to insure that the compiler does not remove it.
            volatileSum = outSum;

            // run warm instruction cache test
            TI_profile_start(TI_PROFILE_KERNEL_OPT_WARMWRB);

            MMALIB_asm(" MARK 6");
            status_opt = MMALIB_LINALG_matrixTranspose_ixX_oxX_exec(handle, pIn, pOut);
            MMALIB_asm(" MARK 7");
            TI_profile_stop();

            /* Test _cn kernel */
            kerInitArgs.funcStyle = MMALIB_FUNCTION_NATC;

            // initialize the kernel to use the natural C variant
            MMALIB_LINALG_matrixTranspose_ixX_oxX_init(handle,
                                                       &bufParamsIn,
                                                       &bufParamsOut,
                                                       &kerInitArgs);

            TI_profile_start(TI_PROFILE_KERNEL_CN);
            MMALIB_asm(" MARK 8");
            status_nat = MMALIB_LINALG_matrixTranspose_ixX_oxX_exec(handle,
                                                                    pIn, pOutCn);
            MMALIB_asm(" MARK 9");
            TI_profile_stop();

            status_nat_vs_opt = TI_compare_mem_2D((void *)pOut, (void *)pOutCn,
                                                  0, 65535, currPrm.widthOut,
                                                  currPrm.heightOut,
                                                  currPrm.strideOut,
                                                  MMALIB_sizeof(bufParamsOut.data_type));
            comparisonDone = 1;

            if( currPrm.staticOut != NULL ) {
                status_ref_vs_opt = TI_compare_mem_2D((void *)currPrm.staticOut, (void *)pOut,
                                                      0, 65535, currPrm.widthOut,
                                                      currPrm.heightOut,
                                                      currPrm.strideOut,
                                                      MMALIB_sizeof(bufParamsOut.data_type));
                comparisonDone = 1;
            } else {
               /* Set to pass since it wasn't supposed to run. */
               status_ref_vs_opt = TI_TEST_KERNEL_PASS;
            }

            /* Set the 'fail' flag based on test vector comparison results */
            currentTestFail = ((status_nat_vs_opt == TI_TEST_KERNEL_FAIL) ||
                               (status_ref_vs_opt == TI_TEST_KERNEL_FAIL) ||
                               (status_init != MMALIB_SUCCESS) ||
                               (status_opt  != MMALIB_SUCCESS) ||
                               (status_nat  != MMALIB_SUCCESS) ||
                               (comparisonDone == 0) ||
                               (currentTestFail == 1)) ? 1 : 0;

            fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;

#if MMALIB_DEBUGPRINT
            printf("%s: status_nat_vs_opt = %d\n", __FUNCTION__, status_nat_vs_opt);
            printf("%s: status_ref_vs_opt = %d\n", __FUNCTION__, status_ref_vs_opt);
            printf("%s: status_init       = %d\n", __FUNCTION__, status_init);
            printf("%s: status_opt        = %d\n", __FUNCTION__, status_opt);
            printf("%s: status_nat        = %d\n", __FUNCTION__, status_nat);
#endif

            pProfile[3*tpi]   = (int32_t)TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
            pProfile[3*tpi+1] = (int32_t)TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[3*tpi+2] = (int32_t)TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);

            sprintf(desc, "%s generated input | M=%d, N=%d",
                    testPatternString, currPrm.heightIn, currPrm.widthIn);

            MMALIB_LINALG_matrixTranspose_ixX_oxX_perfEst(handle,
                                                          &bufParamsIn,
                                                          &bufParamsOut,
                                                          &archCycles,
                                                          &estCycles);

            TI_profile_add_test(testNum++,
                                currPrm.widthIn*currPrm.heightIn,
                                archCycles,
                                estCycles,
                                currentTestFail,
                                desc);
         } else {
            sprintf(desc, "M=%d, N=%d", currPrm.heightIn, currPrm.widthIn);
            TI_profile_skip_test(desc);
         } // end of memory allocation successful?

         /* Free buffers for each test vector */
         TI_align_free(pIn);
         if (currPrm.outputDataLocation == MMALIB_TEST_OUTPUT_HEAP){
            // only malloc'd data can be freed
            free(pOutCn);
            TI_align_free(pOut);
         }

      } // end repetitions
   } // end idat test cases

   free(handle);

   return fail;
}

int test_main(uint32_t *pProfile) {
#if !defined(_HOST_BUILD)
   if( TI_cache_init()) {
      TI_memError("MMALIB_LINALG_matrixTranspose_ixX_oxX");
      return 1;
   } else
#else
      printf("_HOST_BUILD is defined.\n");
#endif
   {
      return MMALIB_LINALG_matrixTranspose_ixX_oxX_d(&pProfile[0], 0);
   }
}

/* Main call for individual test projects */
int main()
{
   int fail = 1;

   uint32_t profile[256*3];

   MMALIB_TEST_init();

   fail = test_main(&profile[0]);

#if !defined(NO_PRINTF)
   if(fail == 0)
      printf("Test Pass!\n");
   else
      printf("Test Fail!\n");

   int i;
   for(i = 0; i < test_cases; i++){
      printf("Test %4d: Cold Cycles = %8d, Warm Cycles = %8d, Warm Cycles WRB = %8d\n", i, profile[3*i], profile[3*i+1], profile[3*i+2]);
   }
#endif

   return fail;
}

/* ======================================================================== */
/*  End of file:  MMALIB_LINALG_matrixTranspose_ixX_oxX_d.c                          */
/* ======================================================================== */
