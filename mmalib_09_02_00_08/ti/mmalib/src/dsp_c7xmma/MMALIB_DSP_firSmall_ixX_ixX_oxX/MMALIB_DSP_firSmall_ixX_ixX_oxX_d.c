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
#include "MMALIB_DSP_firSmall_ixX_ixX_oxX_idat.h"


__attribute__((section(".msmcData"), aligned(64))) int8_t msmcBuffer[MMALIB_L3_RESULTS_BUFFER_SIZE];
__attribute__ ((section (".ddrData"), aligned (64))) int8_t ddrBuffer[2048 * 1024];
int16_t volatile volatileSum = 0;  // use volatile to keep compiler from removing this operation

int MMALIB_DSP_firSmall_ixX_ixX_oxX_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   int32_t  tpi;
   int32_t  currentTestFail;
   int32_t  fail = 0;
   uint32_t repCount;
   uint32_t numReps;
   MMALIB_bufParams1D_t bufParamsIn, bufParamsFilter, bufParamsOut;
   int32_t MMA_SIZE;
   uint32_t k;
   uint32_t testNum;
   uint64_t archCycles, estCycles;

   firSmall_ixX_ixX_oxX_testParams_t   *prm;
   firSmall_ixX_ixX_oxX_testParams_t   currPrm;
   firSmall_ixX_ixX_oxX_getTestParams(&prm, &test_cases);

   MMALIB_DSP_firSmall_ixX_ixX_oxX_InitArgs    kerInitArgs;

   int32_t handleSize = MMALIB_DSP_firSmall_ixX_ixX_oxX_getHandleSize(&kerInitArgs);
   MMALIB_kernelHandle handle = malloc(handleSize);

   TI_profile_init("MMALIB_DSP_firSmall_ixX_ixX_oxX");

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

         kerInitArgs.dataSize = currPrm.dataSize;
         kerInitArgs.batchSize = currPrm.batchSize;
         kerInitArgs.filterSize = currPrm.filterSize;
         kerInitArgs.shift = currPrm.shift;

         MMA_SIZE = MMALIB_MMA_SIZE_16_BIT;
         if (currPrm.dataType == MMALIB_INT32) {
            MMA_SIZE = MMALIB_MMA_SIZE_32_BIT;
         } else if (currPrm.dataType == MMALIB_INT8) {
            MMA_SIZE = MMALIB_MMA_SIZE_8_BIT;
         }

         bufParamsIn.data_type = currPrm.dataType;
         bufParamsOut.data_type = currPrm.dataType;
         bufParamsFilter.data_type = currPrm.dataType;
         MMALIB_DSP_firSmall_ixX_ixX_oxX_getSizes(&kerInitArgs,
                                             &bufParamsIn,
                                             &bufParamsFilter,
                                             &bufParamsOut);

         void *pIn    = (void *) TI_memalign(MMALIB_L2DATA_ALIGNMENT, bufParamsIn.dim_x*MMALIB_sizeof(bufParamsIn.data_type));
         void *pFilter = (void *) TI_memalign(MMALIB_L2DATA_ALIGNMENT, bufParamsFilter.dim_x*MMALIB_sizeof(bufParamsFilter.data_type));
         void *pOut, *pOutCn;
         if (currPrm.outputDataLocation == MMALIB_TEST_OUTPUT_HEAP) {
            pOut   = (void *) TI_memalign(MMALIB_L2DATA_ALIGNMENT, bufParamsOut.dim_x*MMALIB_sizeof(bufParamsOut.data_type));
            pOutCn   = (void *) malloc(bufParamsOut.dim_x*MMALIB_sizeof(bufParamsOut.data_type));
         } else if(currPrm.outputDataLocation == MMALIB_TEST_OUTPUT_MSMC) {
            pOut   = (void *)msmcBuffer;
            pOutCn = (void *)ddrBuffer;
         } else {
            pOut   = (void *)msmcBuffer;
            pOutCn = (void *)ddrBuffer;
         }

         /* Only run the test if the buffer allocations fit in the heap */
         if( pIn && pOut && pFilter && pOutCn ) {

            /* No need to fill anything here, we just used patter 0xAB
             * for testing purposes.                                    */
            TI_fillBuffer(CONSTANT,
                          0xAB,
                          pIn, NULL,
                          bufParamsIn.dim_x,
                          1,
                          bufParamsIn.dim_x*MMALIB_sizeof(currPrm.dataType),
                          MMALIB_sizeof(currPrm.dataType),
                          testPatternString);

            TI_fillBuffer(prm[tpi].testPattern,
                          0,
                          pIn, prm[tpi].staticIn,
                          currPrm.dataSize*currPrm.batchSize,
                          1,
                          currPrm.dataSize*currPrm.batchSize*MMALIB_sizeof(currPrm.dataType),
                          MMALIB_sizeof(currPrm.dataType),
                          testPatternString);

            TI_fillBuffer(CONSTANT,
                          0,
                          pFilter, NULL,
                          bufParamsFilter.dim_x,
                          1,
                          bufParamsFilter.dim_x*MMALIB_sizeof(currPrm.dataType),
                          MMALIB_sizeof(currPrm.dataType),
                          testPatternString);

            TI_fillBuffer(prm[tpi].testPattern,
                          0,
                          (void *)(((int8_t *)pFilter)+MMA_SIZE*MMALIB_sizeof(currPrm.dataType)),
                          prm[tpi].filter,
                          currPrm.filterSize,
                          1,
                          currPrm.filterSize*MMALIB_sizeof(currPrm.dataType),
                          MMALIB_sizeof(currPrm.dataType),
                          testPatternString);

            status_init = MMALIB_DSP_firSmall_ixX_ixX_oxX_init_checkParams(
                          handle,
                          &bufParamsIn,
                          &bufParamsFilter,
                          &bufParamsOut,
                          &kerInitArgs);
            if (status_init == MMALIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               MMALIB_asm(" MARK 0");
               kerInitArgs.funcStyle = MMALIB_FUNCTION_OPTIMIZED;
               status_init = MMALIB_DSP_firSmall_ixX_ixX_oxX_init(handle,
                                                             &bufParamsIn,
                                                             &bufParamsFilter,
                                                             &bufParamsOut,
                                                             &kerInitArgs);
               MMALIB_asm(" MARK 1");
               TI_profile_stop();
               status_opt = MMALIB_DSP_firSmall_ixX_ixX_oxX_exec_checkParams(handle,
                                                                     pIn,
                                                                     pFilter,
                                                                     pOut);
            }
            else
            {
               status_opt = MMALIB_ERR_FAILURE;
            }

            if(status_opt == MMALIB_SUCCESS){
               TI_profile_start(TI_PROFILE_KERNEL_OPT);
               MMALIB_asm(" MARK 2");
               status_opt = MMALIB_DSP_firSmall_ixX_ixX_oxX_exec(handle,
                                                            pIn,
                                                            pFilter,
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
               status_opt = MMALIB_DSP_firSmall_ixX_ixX_oxX_exec(handle,
                                                            pIn,
                                                            pFilter,
                                                            pOut);
               MMALIB_asm(" MARK 5");
               TI_profile_stop();
            }

            // get output to L1D
            int16_t outSum = 0;
            int8_t *pOutTemp = (int8_t *)pOut;       // treat output as bytes to be data type agnostic
            for (k = 0; k < bufParamsOut.dim_x; k++) {
                outSum += *pOutTemp;
                pOutTemp++;
            }

            // dummy store of outSum to insure that the compiler does not remove it.
            volatileSum = outSum;

            // run warm instruction cache test
            TI_profile_start(TI_PROFILE_KERNEL_OPT_WARMWRB);

            MMALIB_asm(" MARK 6");
            status_opt = MMALIB_DSP_firSmall_ixX_ixX_oxX_exec(handle,
                                                         pIn,
                                                         pFilter,
                                                         pOut);
            MMALIB_asm(" MARK 7");
            TI_profile_stop();

            /* Test _cn kernel */
            kerInitArgs.funcStyle = MMALIB_FUNCTION_NATC;

            // initialize the kernel to use the natural C variant
            MMALIB_DSP_firSmall_ixX_ixX_oxX_init(handle,
                                            &bufParamsIn,
                                            &bufParamsFilter,
                                            &bufParamsOut,
                                            &kerInitArgs);

            TI_profile_start(TI_PROFILE_KERNEL_CN);
            MMALIB_asm(" MARK 8");
            status_nat = MMALIB_DSP_firSmall_ixX_ixX_oxX_exec(handle,
                                                         pIn,
                                                         pFilter,
                                                         pOutCn);
            MMALIB_asm(" MARK 9");
            TI_profile_stop();

            status_nat_vs_opt = TI_compare_mem_2D((void *)pOut, (void *)pOutCn,
                                                  2, 65535,
                                                  bufParamsOut.dim_x,
                                                  1,
                                                  bufParamsOut.dim_x*MMALIB_sizeof(bufParamsOut.data_type),
                                                  MMALIB_sizeof(bufParamsOut.data_type));
            comparisonDone = 1;

            if( currPrm.staticOut != NULL ) {
               status_ref_vs_opt = TI_compare_mem_2D((void *)currPrm.staticOut, (void *)pOut,
                                                     2, 65535,
                                                     bufParamsOut.dim_x,
                                                     1,
                                                     bufParamsOut.dim_x*MMALIB_sizeof(bufParamsOut.data_type),
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

            pProfile[3*tpi]   = (int32_t)TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
            pProfile[3*tpi+1] = (int32_t)TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[3*tpi+2] = (int32_t)TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);

            sprintf(desc, "%s generated input | Data size=%d, Batch size=%d, Filter size=%d",
                    testPatternString, currPrm.dataSize, currPrm.batchSize, currPrm.filterSize);

            MMALIB_DSP_firSmall_ixX_ixX_oxX_perfEst(handle,
                                               &bufParamsIn,
                                               &bufParamsFilter,
                                               &bufParamsOut,
                                               &archCycles,
                                               &estCycles);

            TI_profile_add_test(testNum++,
                                currPrm.dataSize*currPrm.batchSize*currPrm.filterSize,
                                archCycles,
                                estCycles,
                                currentTestFail,
                                desc);
         } else {
            sprintf(desc, "Data size=%d, Batch size=%d, Filter size=%d", currPrm.dataSize, currPrm.batchSize, currPrm.filterSize);
            TI_profile_skip_test(desc);
         } // end of memory allocation successful?

         /* Free buffers for each test vector */
         if (currPrm.outputDataLocation == MMALIB_TEST_OUTPUT_HEAP) {
            free(pOutCn);
            TI_align_free(pOut);
         }
         TI_align_free(pIn);

      } // end repetitions
   } // end idat test cases

   free(handle);

   return fail;
}

int test_main(uint32_t *pProfile) {
#if !defined(_HOST_BUILD)
   if( TI_cache_init()) {
      TI_memError("MMALIB_DSP_firSmall_ixX_ixX_oxX");
      return 1;
   } else
#else
      printf("_HOST_BUILD is defined.\n");
#endif
   {
      return MMALIB_DSP_firSmall_ixX_ixX_oxX_d(&pProfile[0], 0);
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
/*  End of file:  MMALIB_DSP_firSmall_ixX_ixX_oxX_d.c                          */
/* ======================================================================== */
