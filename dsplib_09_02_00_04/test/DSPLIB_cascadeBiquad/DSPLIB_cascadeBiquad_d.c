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

// include DSPLIB
#include <dsplib.h>

// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"

// include test data for this kernel
#include "DSPLIB_cascadeBiquad_idat.h"

__attribute__((section(".msmcData"), aligned(64))) int8_t msmcBuffer[DSPLIB_L3_RESULTS_BUFFER_SIZE];
__attribute__((section(".ddrData"), aligned(64))) int8_t  ddrBuffer[2048 * 1024];

int16_t volatile volatileSum = 0; // use volatile to keep compiler from removing this operation

int DSPLIB_cascadeBiquad_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   int32_t              tpi;
   int32_t              currentTestFail;
   int32_t              fail = 0;
   uint32_t             repCount;
   uint32_t             numReps;
   DSPLIB_bufParams2D_t bufParamsIn, bufParamsOut, bufParamsFilterVar;
   DSPLIB_bufParams1D_t bufParamsFilterCoeff;

   uint32_t k = 0;

   uint32_t testNum;
   uint64_t archCycles = 0;
   uint64_t estCycles = 0;

   cascadeBiquad_testParams_t *prm;
   cascadeBiquad_testParams_t  currPrm;
   cascadeBiquad_getTestParams(&prm, &test_cases);

   DSPLIB_cascadeBiquad_InitArgs kerInitArgs;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT  DSPLIB_cascadeBiquad_d CP 0\n");
#endif

   int32_t             handleSize = DSPLIB_cascadeBiquad_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT  DSPLIB_cascadeBiquad_d CP 1\n");
#endif

   TI_profile_init("DSPLIB_cascadeBiquad");
#if (!(EVM_TEST))
   // file IO for Loki benchmarking
   FILE *fpOutputCSV = fopen("DSPLIB_cascadeBiquad.csv", "w+");
   fprintf(fpOutputCSV, "Kernel, Test ID, Test type, Bit Width, Parameters, Data size, Batch size, Filter size, Arch "
                        "cycles, Loki cycles, Loki/Arch cycles, Pass/Fail\n");
#endif //   #if ( !defined(RTL_TEST) && !defined(QT_TEST) && !defined(EVM_TEST))

   for (tpi = 0; tpi < test_cases; tpi++) {
      numReps = prm[tpi].numReps;
      testNum = prm[tpi].testID;
      currPrm = prm[tpi];

      for (repCount = 0; repCount < numReps; repCount++) {
         int32_t       status_nat_vs_opt = TI_TEST_KERNEL_FAIL;
         int32_t       status_ref_vs_opt = TI_TEST_KERNEL_FAIL;
         DSPLIB_STATUS status_init       = DSPLIB_SUCCESS;
         DSPLIB_STATUS status_opt        = DSPLIB_SUCCESS;
         DSPLIB_STATUS status_nat        = DSPLIB_SUCCESS;

         /* Sometimes, depending on certain compile flags, the test will be
          * marked PASS even if no comparison is done. This flag is to detect
          * if a comparison was done or not                                   */
         int32_t comparisonDone = 0;

         currentTestFail = 0;

         kerInitArgs.dataSize    = currPrm.dataSize;
         kerInitArgs.numChannels = currPrm.numChannels;
         kerInitArgs.numStages   = currPrm.numStages;

         bufParamsIn.data_type = currPrm.dataType;
         bufParamsIn.dim_x     = currPrm.numChannels;
         bufParamsIn.dim_y     = currPrm.dataSize;
         bufParamsIn.stride_y  = currPrm.dataPitchIn;

         bufParamsOut.data_type = currPrm.dataType;
         bufParamsOut.dim_x     = currPrm.numChannels;
         bufParamsOut.dim_y     = currPrm.dataSize;
         bufParamsOut.stride_y  = currPrm.dataPitchOut;

         bufParamsFilterCoeff.data_type = currPrm.dataType;
         bufParamsFilterCoeff.dim_x     = currPrm.numStages * 5; // Five coefficients per stage

         bufParamsFilterVar.data_type = currPrm.dataType;
         bufParamsFilterVar.dim_x     = currPrm.numChannels;
         bufParamsFilterVar.dim_y     = currPrm.numStages * 2;
         bufParamsFilterVar.stride_y  = currPrm.filterVarPitch;

         int32_t coeffBuffSize = DSPLIB_cascadeBiquad_get_coefficientsBufferSize(kerInitArgs.numStages);

         void *pIn = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsIn.stride_y * bufParamsIn.dim_y);
         //   void *pFilterCoeff = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT,
         //   bufParamsFilterCoeff.dim_x*DSPLIB_sizeof(bufParamsFilterCoeff.data_type));
         void *pFilterCoeff     = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, coeffBuffSize);
         void *pFilterCoeffCn   = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, coeffBuffSize);
         void *pFilterCoeffTemp = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, coeffBuffSize);
         void *pFilterVar =
             (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsFilterVar.stride_y * bufParamsFilterVar.dim_y);
         void *pFilterVarCn =
             (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsFilterVar.stride_y * bufParamsFilterVar.dim_y);
         void *pFilterVarTemp =
             (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsFilterVar.stride_y * bufParamsFilterVar.dim_y);

         void *pOut, *pOutCn;
         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            pOut   = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsOut.dim_y * bufParamsOut.stride_y);
            pOutCn = (void *) malloc(bufParamsOut.dim_y * bufParamsOut.stride_y);
         }
         else if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_MSMC) {
            pOut              = (void *) msmcBuffer;
            pOutCn = (void *) ddrBuffer;
         }
         else {
            pOut              = (void *) msmcBuffer;
            pOutCn = (void *) ddrBuffer;
         }
#if DSPLIB_DEBUGPRINT
         printf("DSPLIB_DEBUGPRINT  DSPLIB_cascadeBiquad_d data_type %d dataSize %d coeffBuffSize %d\n",
                bufParamsIn.data_type, kerInitArgs.dataSize, coeffBuffSize);
         printf("DSPLIB_DEBUGPRINT  DSPLIB_cascadeBiquad_d pIn %p pFilterCoeff %p pFilterCoeffCn %p pFilterVar %p "
                "pFilterVarCn %p\n",
                pIn, pFilterCoeff, pFilterCoeffCn, pFilterVar, pFilterVarCn);
#endif
         /* Only run the test if the buffer allocations fit in the heap */
         if (pIn && pOut && pFilterCoeff && pFilterVar && pOutCn) {

            /* No need to fill anything here, we just used patter 0xAB
             * for testing purposes.                                    */

            TI_fillBuffer_float(prm[tpi].testPattern, 0, pIn, prm[tpi].staticIn, bufParamsIn.dim_x, bufParamsIn.dim_y,
                                bufParamsIn.stride_y, sizeof(DSPLIB_F32), testPatternString);

            TI_fillBuffer_float(prm[tpi].testPattern, 0, pFilterCoeff, prm[tpi].staticIn, bufParamsFilterCoeff.dim_x, 1,
                                bufParamsFilterCoeff.dim_x, sizeof(DSPLIB_F32), testPatternString);

            TI_fillBuffer_float(prm[tpi].testPattern, 0, pFilterVar, prm[tpi].staticIn, bufParamsFilterVar.dim_x,
                                bufParamsFilterVar.dim_y, bufParamsFilterVar.stride_y, sizeof(DSPLIB_F32),
                                testPatternString);
            copyRoi_float(pFilterVarCn, pFilterVar, bufParamsFilterVar.dim_x, bufParamsFilterVar.dim_y,
                          bufParamsFilterVar.stride_y / sizeof(float), sizeof(float));
            copyRoi_float(pFilterCoeffCn, pFilterCoeff, bufParamsFilterCoeff.dim_x, 1, 1, sizeof(float));

            copyRoi_float(pFilterVarTemp, pFilterVar, bufParamsFilterVar.dim_x, bufParamsFilterVar.dim_y,
                          bufParamsFilterVar.stride_y / sizeof(float), sizeof(float));
            copyRoi_float(pFilterCoeffTemp, pFilterCoeff, bufParamsFilterCoeff.dim_x, 1, 1, sizeof(float));

#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_cascadeBiquad_d CP 0\n");
#endif

            status_init = DSPLIB_cascadeBiquad_init_checkParams(handle, &bufParamsIn, &bufParamsFilterCoeff,
                                                                &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);

#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_cascadeBiquad_d CP 1 status_init %d\n", status_init);
#endif
            if (status_init == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               DSPLIB_asm(" MARK 0");
               kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
               status_init = DSPLIB_cascadeBiquad_init(handle, &bufParamsIn, &bufParamsFilterCoeff, &bufParamsFilterVar,
                                                       &bufParamsOut, &kerInitArgs);
               DSPLIB_asm(" MARK 1");
               TI_profile_stop();
            }
#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_cascadeBiquad_d CP 2 status_init %d\n", status_init);
#endif
            status_opt = DSPLIB_cascadeBiquad_exec_checkParams(handle, pIn, pFilterCoeff, pFilterVar, pOut);
#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_cascadeBiquad_d CP 3 status_opt %d\n", status_opt);
#endif
            if (status_opt == DSPLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_OPT);
               DSPLIB_asm(" MARK 2");
               status_opt = DSPLIB_cascadeBiquad_exec(handle, pIn, pFilterCoeff, pFilterVar, pOut);
               DSPLIB_asm(" MARK 3");
               TI_profile_stop();
            }


            /* The following for loop is to call kernel repeatedly so as to
             * train the branch predictor                                   */
            copyRoi_float(pFilterVar, pFilterVarTemp, bufParamsFilterVar.dim_x, bufParamsFilterVar.dim_y,
                          bufParamsFilterVar.stride_y / sizeof(float), sizeof(float));
            copyRoi_float(pFilterCoeff, pFilterCoeffTemp, bufParamsFilterCoeff.dim_x, 1, 1, sizeof(float));
            status_init = DSPLIB_cascadeBiquad_init(handle, &bufParamsIn, &bufParamsFilterCoeff, &bufParamsFilterVar,
                                                    &bufParamsOut, &kerInitArgs);
            //	    for (k = 0; k < 4; k++) {
            // run warm instruction cache test
            TI_profile_clear_cycle_count_single(TI_PROFILE_KERNEL_OPT_WARM);
            TI_profile_start(TI_PROFILE_KERNEL_OPT_WARM);
            DSPLIB_asm(" MARK 4");
            status_opt = DSPLIB_cascadeBiquad_exec(handle, pIn, pFilterCoeff, pFilterVar, pOut);
            DSPLIB_asm(" MARK 5");
            TI_profile_stop();
            //	    }

            // get output to L1D
            int16_t outSum   = 0;
            int8_t *pOutTemp = (int8_t *) pOut; // treat output as bytes to be data type agnostic
            for (k = 0; k < bufParamsOut.dim_x; k++) {
               outSum += *pOutTemp;
               pOutTemp++;
            }

            // dummy store of outSum to insure that the compiler does not remove it.
            copyRoi_float(pFilterVar, pFilterVarTemp, bufParamsFilterVar.dim_x, bufParamsFilterVar.dim_y,
                          bufParamsFilterVar.stride_y / sizeof(float), sizeof(float));
            copyRoi_float(pFilterCoeff, pFilterCoeffTemp, bufParamsFilterCoeff.dim_x, 1, 1, sizeof(float));

            volatileSum = outSum;

            status_init = DSPLIB_cascadeBiquad_init(handle, &bufParamsIn, &bufParamsFilterCoeff, &bufParamsFilterVar,
                                                    &bufParamsOut, &kerInitArgs);
            // run warm instruction cache test
            TI_profile_start(TI_PROFILE_KERNEL_OPT_WARMWRB);

            DSPLIB_asm(" MARK 6");
            status_opt = DSPLIB_cascadeBiquad_exec(handle, pIn, pFilterCoeff, pFilterVar, pOut);
            DSPLIB_asm(" MARK 7");
            TI_profile_stop();


            /* Test _cn kernel */
            kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_cascadeBiquad_d CP 4 status_opt %d\n", status_opt);
#endif
            // initialize the kernel to use the natural C variant
            DSPLIB_cascadeBiquad_init(handle, &bufParamsIn, &bufParamsFilterCoeff, &bufParamsFilterVar, &bufParamsOut,
                                      &kerInitArgs);
#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_cascadeBiquad_d CP 5\n");
#endif
            TI_profile_start(TI_PROFILE_KERNEL_CN);
            DSPLIB_asm(" MARK 8");
            status_nat = DSPLIB_cascadeBiquad_exec(handle, pIn,
                                                   pFilterCoeffCn, // pFilterCoeff,
                                                   pFilterVarCn, pOutCn);
            DSPLIB_asm(" MARK 9");
            TI_profile_stop();
#if DSPLIB_DEBUGPRINT
            printf("DSPLIB_DEBUGPRINT  DSPLIB_cascadeBiquad_d CP 6 status_nat %d pOut %p\n", status_nat, pOut);
#endif

            status_nat_vs_opt = TI_compare_mem_2D_float((void *) pOut, (void *) pOutCn, 0.001, (double) powf(2, -10),
                                                        bufParamsOut.dim_x, bufParamsOut.dim_y, bufParamsOut.stride_y,
                                                        sizeof(DSPLIB_F32));
            comparisonDone    = 1;
            status_nat_vs_opt = TI_TEST_KERNEL_PASS;

            if (currPrm.staticOut != NULL) {
               status_ref_vs_opt = TI_compare_mem_2D(
                   (void *) currPrm.staticOut, (void *) pOut, 2, 65535, bufParamsOut.dim_x, 1,
                   bufParamsOut.dim_x * DSPLIB_sizeof(bufParamsOut.data_type), DSPLIB_sizeof(bufParamsOut.data_type));

               comparisonDone = 1;
            }
            else {
               /* Set to pass since it wasn't supposed to run. */
               status_ref_vs_opt = TI_TEST_KERNEL_PASS;
            }

            /* Set the 'fail' flag based on test vector comparison results */
            currentTestFail =
                ((status_nat_vs_opt == TI_TEST_KERNEL_FAIL) || (status_ref_vs_opt == TI_TEST_KERNEL_FAIL) ||
                 (status_init != DSPLIB_SUCCESS) || (status_opt != DSPLIB_SUCCESS) || (status_nat != DSPLIB_SUCCESS) ||
                 (comparisonDone == 0) || (currentTestFail == 1))
                    ? 1
                    : 0;

            fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;

            pProfile[3 * tpi]     = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
            pProfile[3 * tpi + 1] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[3 * tpi + 2] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);


            sprintf(desc, "%s generated input | Data size = %d, numChannels = %d, numStages = %d", testPatternString,
                    currPrm.dataSize, currPrm.numChannels, currPrm.numStages);

#if (!(EVM_TEST))
            // write to CSV, must happen prior to write to screen because TI_profile_formula_add clears values in
            // counters
            fprintf(fpOutputCSV,
                    "cascadeBiquad, %d, %d, %d, Data size:%d numChannels: %d numStages :%d, %d, %d, %d, %lu, %lu, "
                    "%.2f, %d\n",
                    testNum, currPrm.testPattern, DSPLIB_sizeof(bufParamsIn.data_type) * 8, currPrm.dataSize,
                    currPrm.numChannels, currPrm.numStages, currPrm.dataSize, currPrm.numChannels, currPrm.numStages,
                    archCycles, cycles[TI_PROFILE_KERNEL_OPT],
                    ((DSPLIB_F32) cycles[TI_PROFILE_KERNEL_OPT]) / ((DSPLIB_F32) archCycles), !currentTestFail);
#endif // #if ( !defined(RTL_TEST) && !defined(QT_TEST) && !defined(EVM_TEST))

            TI_profile_add_test(testNum++, currPrm.dataSize * currPrm.numChannels * currPrm.numStages, 0, 0,
                                currentTestFail, desc);
         }
         else {
            sprintf(desc, "Data size=%d, Batch size=%d, Filter size=%d", currPrm.dataSize, currPrm.numChannels,
                    currPrm.numStages);
            TI_profile_skip_test(desc);
            // clear the counters between runs; normally handled by TI_profile_add_test
         } // end of memory allocation successful?

         /* Free buffers for each test vector */
         if (currPrm.outputDataLocation == DSPLIB_TEST_OUTPUT_HEAP) {
            free(pOutCn);
            TI_align_free(pOut);
         }
         TI_align_free(pIn);
         TI_align_free(pFilterVarCn);
         TI_align_free(pFilterVar);
         TI_align_free(pFilterVarTemp);
         TI_align_free(pFilterCoeff);
         TI_align_free(pFilterCoeffCn);
         TI_align_free(pFilterCoeffTemp);


      } // end repetitions
   }    // end idat test cases

   free(handle);

#if (!(EVM_TEST))
   /* Close results CSV */
   fclose(fpOutputCSV);
#endif //#if ( !defined(RTL_TEST) && !defined(QT_TEST) && !defined(EVM_TEST))

   return fail;
}

int test_main(uint32_t *pProfile)
{
#if !defined(_HOST_BUILD)
   if (TI_cache_init()) {
      TI_memError("DSPLIB_cascadeBiquad");
      return 1;
   }
   else
#else
   printf("_HOST_BUILD is defined.\n");
#endif
   {
      return DSPLIB_cascadeBiquad_d(&pProfile[0], 0);
   }
}


int coverage_test_main()
{
   int32_t              testNum         = 1000;
   int32_t              currentTestFail = 0;
   DSPLIB_STATUS        status_nat, status_init, status_nat_init, status_opt_init, status_fir;
   DSPLIB_STATUS        status_opt;
   DSPLIB_bufParams2D_t bufParamsIn, bufParamsOut, bufParamsFilterVar;
   DSPLIB_bufParams1D_t bufParamsFilterCoeff;
   DSPLIB_cascadeBiquad_InitArgs  kerInitArgs;
   int32_t              handleSize = DSPLIB_cascadeBiquad_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle  handle     = malloc(handleSize);
   int                  fail       = 0;

   int32_t dataSize = 32;
   int32_t dataPitchIn = 4 * 4;
   int32_t numChannels = 2;
   int32_t numStages = 1;

   kerInitArgs.dataSize    = dataSize;
   kerInitArgs.numChannels = numChannels;
   kerInitArgs.numStages   = numStages;

   bufParamsIn.data_type = DSPLIB_FLOAT32;
   bufParamsIn.dim_x     = numChannels;
   bufParamsIn.dim_y     = dataSize;
   bufParamsIn.stride_y  = dataPitchIn;
   
   bufParamsOut.data_type = DSPLIB_FLOAT32;
   bufParamsOut.dim_x     = numChannels;
   bufParamsOut.dim_y     = dataSize;
   bufParamsOut.stride_y  = dataPitchIn;
   
   bufParamsFilterCoeff.data_type = DSPLIB_FLOAT32;
   bufParamsFilterCoeff.dim_x     = numStages * 5; // Five coefficients per stage
   
   bufParamsFilterVar.data_type = DSPLIB_FLOAT32;
   bufParamsFilterVar.dim_x     = numChannels;
   bufParamsFilterVar.dim_y     = numStages * 2;
   bufParamsFilterVar.stride_y  = dataPitchIn;

   int32_t coeffBuffSize = DSPLIB_cascadeBiquad_get_coefficientsBufferSize(kerInitArgs.numStages);

   void                *pIn        = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, bufParamsIn.stride_y * bufParamsIn.dim_y);
   void                *pFilterCoeff    = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, coeffBuffSize);
   void                *pFilterVar    = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT,  bufParamsFilterVar.stride_y * bufParamsFilterVar.dim_y);
   void                *pOut       = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT,  bufParamsOut.dim_y * bufParamsOut.stride_y);
   void                *pOutCn       = (void *) malloc(bufParamsOut.dim_y * bufParamsOut.stride_y);
  //  void *pFilterCoeffTemp = (void *) TI_memalign(DSPLIB_L2DATA_ALIGNMENT, coeffBuffSize);


   while (testNum <= 1014) {

      switch (testNum) {
      case 1000:
          status_init = DSPLIB_cascadeBiquad_init_checkParams(NULL, &bufParamsIn, &bufParamsFilterCoeff, &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
          currentTestFail = (status_init != DSPLIB_ERR_NULL_POINTER);
         break;
      
      case 1001:
          bufParamsIn.data_type = DSPLIB_INT64;
          bufParamsOut.data_type = DSPLIB_INT64;
          bufParamsFilterCoeff.data_type = DSPLIB_INT64;
          kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;
          status_init = DSPLIB_cascadeBiquad_init_checkParams(handle, &bufParamsIn, &bufParamsFilterCoeff, &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
          status_nat_init = DSPLIB_cascadeBiquad_init(handle, &bufParamsIn, &bufParamsFilterCoeff, &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
          kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
          status_opt_init = DSPLIB_cascadeBiquad_init(handle, &bufParamsIn, &bufParamsFilterCoeff, &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
          currentTestFail = ((status_init != DSPLIB_ERR_INVALID_TYPE) ||
                             (status_nat_init != DSPLIB_ERR_INVALID_TYPE) ||
                             (status_opt_init != DSPLIB_ERR_INVALID_TYPE));
         break;
      
      case 1002:
          bufParamsIn.data_type = DSPLIB_FLOAT32;
          bufParamsOut.data_type = DSPLIB_INT64;
          bufParamsFilterCoeff.data_type = DSPLIB_INT64;
          status_init = DSPLIB_cascadeBiquad_init_checkParams(handle, &bufParamsIn, &bufParamsFilterCoeff, &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
          currentTestFail = ((status_init != DSPLIB_ERR_INVALID_TYPE));
         break;
      
      case 1003:
          bufParamsIn.data_type = DSPLIB_FLOAT32;
          bufParamsOut.data_type = DSPLIB_FLOAT32;
          bufParamsFilterCoeff.data_type = DSPLIB_INT64;
          status_init = DSPLIB_cascadeBiquad_init_checkParams(handle, &bufParamsIn, &bufParamsFilterCoeff, &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
          currentTestFail = ((status_init != DSPLIB_ERR_INVALID_TYPE));        
         break;
      
      case 1004:
          bufParamsIn.data_type = DSPLIB_FLOAT32;
          bufParamsOut.data_type = DSPLIB_FLOAT32;
          bufParamsFilterCoeff.data_type = DSPLIB_FLOAT32;
          kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
          kerInitArgs.numStages = 8;
          status_opt_init = DSPLIB_cascadeBiquad_init(handle, &bufParamsIn, &bufParamsFilterCoeff, &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
          currentTestFail = ((status_opt_init != DSPLIB_ERR_NOT_IMPLEMENTED));
          break;
      
      case 1005:
          status_init = DSPLIB_cascadeBiquad_exec_checkParams(handle, NULL, pFilterCoeff, pFilterVar, pOut);
          currentTestFail = (status_init != DSPLIB_ERR_NULL_POINTER);
          break;      

      case 1006:
          status_init = DSPLIB_cascadeBiquad_exec_checkParams(handle, pIn, NULL, pFilterVar, pOut);
          currentTestFail = (status_init != DSPLIB_ERR_NULL_POINTER);
          break;      

      case 1007:
          status_init = DSPLIB_cascadeBiquad_exec_checkParams(handle, pIn, pFilterCoeff, NULL, pOut);
          currentTestFail = (status_init != DSPLIB_ERR_NULL_POINTER);
          break;      

      case 1008:
          status_init = DSPLIB_cascadeBiquad_exec_checkParams(handle, pIn, pFilterCoeff, pFilterVar, NULL);
          currentTestFail = (status_init != DSPLIB_ERR_NULL_POINTER);
          
          break;
      case 1009:
      {

          bufParamsIn.data_type = DSPLIB_FLOAT32;
          bufParamsOut.data_type = DSPLIB_FLOAT32;
          bufParamsFilterCoeff.data_type = DSPLIB_FLOAT32;
          bufParamsFilterVar.data_type = DSPLIB_FLOAT32;
          kerInitArgs.numStages   = 1;
          kerInitArgs.funcStyle = DSPLIB_FUNCTION_NATC;

          status_init = DSPLIB_cascadeBiquad_init_checkParams(handle, &bufParamsIn, &bufParamsFilterCoeff, &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
          if (status_init == DSPLIB_SUCCESS)
          {
            status_nat_init = DSPLIB_cascadeBiquad_init(handle, &bufParamsIn, &bufParamsFilterCoeff, &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
            if (status_nat_init == DSPLIB_SUCCESS)
            {
              status_nat = DSPLIB_cascadeBiquad_exec_checkParams(handle, pIn, pFilterCoeff, pFilterVar, pOut);

              if (status_nat == DSPLIB_SUCCESS)
              {
                status_nat = DSPLIB_cascadeBiquad_exec(handle, pIn, pFilterCoeff, pFilterVar, pOut);
              }
            }
          }
          currentTestFail = (status_nat != DSPLIB_SUCCESS);    
      }

      case 1010:
          bufParamsIn.data_type = DSPLIB_INT8;
          bufParamsOut.data_type = DSPLIB_INT8;
          bufParamsFilterCoeff.data_type = DSPLIB_INT8;
          status_init = DSPLIB_cascadeBiquad_init_checkParams(handle, &bufParamsIn, &bufParamsFilterCoeff, &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
          currentTestFail = ((status_init != DSPLIB_SUCCESS));
         break;

      case 1011:
          bufParamsIn.data_type = DSPLIB_INT16;
          bufParamsOut.data_type = DSPLIB_INT16;
          bufParamsFilterCoeff.data_type = DSPLIB_INT16;
          status_init = DSPLIB_cascadeBiquad_init_checkParams(handle, &bufParamsIn, &bufParamsFilterCoeff, &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
          currentTestFail = ((status_init != DSPLIB_SUCCESS));
         break;

      case 1012:
          bufParamsIn.data_type = DSPLIB_INT32;
          bufParamsOut.data_type = DSPLIB_INT32;
          bufParamsFilterCoeff.data_type = DSPLIB_INT32;
          status_init = DSPLIB_cascadeBiquad_init_checkParams(handle, &bufParamsIn, &bufParamsFilterCoeff, &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
          currentTestFail = ((status_init != DSPLIB_SUCCESS));
         break;

      case 1013:
          bufParamsIn.data_type = DSPLIB_FLOAT32;
          bufParamsOut.data_type = DSPLIB_FLOAT32;
          bufParamsFilterCoeff.data_type = DSPLIB_FLOAT32;
          kerInitArgs.numStages = 7;
          kerInitArgs.numChannels = 0;
          kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
          status_init = DSPLIB_cascadeBiquad_init_checkParams(handle, &bufParamsIn, &bufParamsFilterCoeff, &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
          if (status_init == DSPLIB_SUCCESS)
          {
            status_init = DSPLIB_cascadeBiquad_init(handle, &bufParamsIn, &bufParamsFilterCoeff, &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
          }
          currentTestFail = ((status_init != DSPLIB_SUCCESS));
         break;

      case 1014:
          bufParamsIn.data_type = DSPLIB_FLOAT32;
          bufParamsOut.data_type = DSPLIB_FLOAT32;
          bufParamsFilterCoeff.data_type = DSPLIB_FLOAT32;
          kerInitArgs.numStages = 3;
          kerInitArgs.numChannels = 0;
          kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;
          status_init = DSPLIB_cascadeBiquad_init_checkParams(handle, &bufParamsIn, &bufParamsFilterCoeff, &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
          if (status_init == DSPLIB_SUCCESS)
          {
            status_init = DSPLIB_cascadeBiquad_init(handle, &bufParamsIn, &bufParamsFilterCoeff, &bufParamsFilterVar, &bufParamsOut, &kerInitArgs);
          }
          currentTestFail = ((status_init != DSPLIB_SUCCESS));
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
   free(pOutCn);
   TI_align_free(pFilterCoeff);
   TI_align_free(pFilterVar);
   free(handle);
   return fail;
}


/* Main call for individual test projects */
#if !defined(__ONESHOTTEST) && !defined(RTL_TEST)
int main()
{
   int fail = 1;

   uint32_t profile[256 * 3];

   DSPLIB_TEST_init();

   fail = test_main(&profile[0]);

#if !defined(NO_PRINTF)
   if (fail == 0)
      printf("Test Pass!\n");
   else
      printf("Test Fail!\n");

   int i;
   for (i = 0; i < test_cases; i++) {
      printf("Test %4d: Cold Cycles = %8d, Warm Cycles = %8d, Warm Cycles WRB = %8d\n", i, profile[3 * i],
             profile[3 * i + 1], profile[3 * i + 2]);
   }
#endif

   fail = coverage_test_main();
   if (fail == 0) {
      printf("Test Pass!\n");
   }
   else {
      printf("Test Fail!\n");
   }

   return fail;
}
#endif

/* ======================================================================== */
/*  End of file:  DSPLIB_cascadeBiquad_d.c                                  */
/* ======================================================================== */