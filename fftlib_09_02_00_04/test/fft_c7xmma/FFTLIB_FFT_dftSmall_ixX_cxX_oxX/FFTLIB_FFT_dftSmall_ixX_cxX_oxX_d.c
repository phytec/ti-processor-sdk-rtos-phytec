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

// include FFTLIB
#include <fftlib.h>

// include test infrastructure provided by FFTLIB
#include "FFTLIB_test.h"

// include test data for this kernel
#include "FFTLIB_FFT_dftSmall_ixX_cxX_oxX_idat.h"

__attribute__ ((section (".ddrData"), aligned (64))) int8_t ddrBuffer[2048 * 1024];
int16_t volatile volatileSum = 0;  // use volatile to keep compiler from removing this operation

int FFTLIB_FFT_dftSmall_ixX_cxX_oxX_d(
   uint32_t *pProfile,
   uint8_t LevelOfFeedback)
{
   int32_t  tpi;
   int32_t  currentTestFail;
   int32_t  fail = 0;
   uint32_t repCount;
   uint32_t numReps;
   FFTLIB_bufParams1D_t bufParamsX, bufParamsY, bufParamsW;
   uint32_t k;
   uint32_t testNum;
   uint64_t archCycles, estCycles;

   dftSmall_ixX_cxX_oxX_testParams_t   *prm;
   dftSmall_ixX_cxX_oxX_testParams_t   currPrm;
   dftSmall_ixX_cxX_oxX_getTestParams(&prm, &test_cases);

   FFTLIB_FFT_dftSmall_ixX_cxX_oxX_InitArgs    kerInitArgs;

   int32_t handleSize = FFTLIB_FFT_dftSmall_ixX_cxX_oxX_getHandleSize(&kerInitArgs);
   FFTLIB_kernelHandle handle = malloc(handleSize);

   TI_profile_init("FFTLIB_FFT_dftSmall_ixX_cxX_oxX");
#if ( !defined(RTL_TEST) && !defined(QT_TEST) && !defined(EVM_TEST))
   // file IO for Loki benchmarking
   FILE *fpOutputCSV = fopen("FFTLIB_FFT_dftSmall_ixX_cxX_oxX.csv", "w+");
   fprintf(fpOutputCSV, "Kernel, Test ID, Test type, Bit Width, Parameters, FFT size, Batch size, Interleave, Arch cycles, Loki cycles, Loki/Arch cycles, Pass/Fail\n");
#endif //   #if ( !defined(RTL_TEST) && !defined(QT_TEST) && !defined(EVM_TEST))

   for (tpi=0; tpi < test_cases; tpi++) {
      numReps = prm[tpi].numReps;
      testNum = prm[tpi].testID;
      currPrm = prm[tpi];

      for (repCount = 0; repCount < numReps; repCount++){
	 int32_t      status_nat_vs_opt = TI_TEST_KERNEL_FAIL;
	 int32_t      status_ref_vs_opt = TI_TEST_KERNEL_FAIL;
	 FFTLIB_STATUS   status_init = FFTLIB_SUCCESS;
	 FFTLIB_STATUS   status_opt = FFTLIB_SUCCESS;
	 FFTLIB_STATUS   status_nat = FFTLIB_SUCCESS;

	 /* Sometimes, depending on certain compile flags, the test will be
	  * marked PASS even if no comparison is done. This flag is to detect
	  * if a comparison was done or not                                   */
	 int32_t      comparisonDone = 0;

	 currentTestFail = 0;

	 kerInitArgs.fftSize = currPrm.fftSize;
	 kerInitArgs.batchSize = currPrm.batchSize;
	 for (k = 0; k < FFTLIB_FFT_DFTSMALL_IXX_CXX_OXX_NUMSHIFTS; k++) {
	     kerInitArgs.shiftVector[k] = currPrm.shiftVector[k];
	 }
	 kerInitArgs.interleave = currPrm.interleave;
	 bufParamsX.data_type = currPrm.dataType;
	 bufParamsY.data_type = currPrm.dataType;;
	 bufParamsW.data_type = currPrm.dataType;;
	 FFTLIB_FFT_dftSmall_ixX_cxX_oxX_getSizes(&kerInitArgs,
						  &bufParamsX,
						  &bufParamsW,
						  &bufParamsY);

	 void *pX   = (void *) TI_memalign(FFTLIB_L2DATA_ALIGNMENT, bufParamsX.dim_x*FFTLIB_sizeof(currPrm.dataType));
	 void *pW   = (void *) TI_memalign(FFTLIB_L2DATA_ALIGNMENT,bufParamsW.dim_x*FFTLIB_sizeof(currPrm.dataType));
	 void *pY   = (void *) TI_memalign(FFTLIB_L2DATA_ALIGNMENT, bufParamsY.dim_x*FFTLIB_sizeof(currPrm.dataType));
//	 void *pYCn = (void *) malloc(bufParamsX.dim_x*FFTLIB_sizeof(currPrm.dataType));
	 void *pYCn = (void *) ddrBuffer;

	 /* Only run the test if the buffer allocations fit in the heap */
	 if( pX && pW && pY && pYCn ) {

	    TI_fillBuffer(prm[tpi].testPattern,
			  0,
			  pX,
			  prm[tpi].staticIn,
			  currPrm.batchSize*currPrm.fftSize*2,
			  1,
			  currPrm.batchSize*currPrm.fftSize*2*FFTLIB_sizeof(currPrm.dataType),
			  FFTLIB_sizeof(currPrm.dataType),
			  testPatternString);

	    FFTLIB_FFT_dftSmall_ixX_cxX_oxX_twGen(&kerInitArgs,
						  pW,
						  &bufParamsW);

	    status_init = FFTLIB_FFT_dftSmall_ixX_cxX_oxX_init_checkParams(handle,
									   &bufParamsX,
									   &bufParamsW,
									   &bufParamsY,
									   &kerInitArgs);
	    if (status_init == FFTLIB_SUCCESS) {
	       TI_profile_start(TI_PROFILE_KERNEL_INIT);
	       FFTLIB_asm(" MARK 0");
	       kerInitArgs.funcStyle = FFTLIB_FUNCTION_OPTIMIZED;
	       status_init = FFTLIB_FFT_dftSmall_ixX_cxX_oxX_init(handle,
								  &bufParamsX,
								  &bufParamsW,
								  &bufParamsY,
								  &kerInitArgs);
	       FFTLIB_asm(" MARK 1");
	       TI_profile_stop();
	    }

	    status_opt = FFTLIB_FFT_dftSmall_ixX_cxX_oxX_exec_checkParams(handle,
									  pX,
									  pW,
									  pY);
	    if(status_opt == FFTLIB_SUCCESS){
	       TI_profile_start(TI_PROFILE_KERNEL_OPT);
	       FFTLIB_asm(" MARK 2");
	       status_opt = FFTLIB_FFT_dftSmall_ixX_cxX_oxX_exec(handle,
								 pX,
								 pW,
								 pY);
	       FFTLIB_asm(" MARK 3");
	       TI_profile_stop();
	    }

#if (defined(RTL_TEST) || defined(QT_TEST) || defined(EVM_TEST) )
	    /* The following for loop is to call kernel repeatedly so as to
	     * train the branch predictor                                   */
	    for (k = 0; k < 4; k++) {
	       // run warm instruction cache test
	       TI_profile_clear_cycle_count_single(TI_PROFILE_KERNEL_OPT_WARM);
	       TI_profile_start(TI_PROFILE_KERNEL_OPT_WARM);
	       FFTLIB_asm(" MARK 4");
	       status_opt = FFTLIB_FFT_dftSmall_ixX_cxX_oxX_exec(handle,
								 pX,
								 pW,
								 pY);
	       FFTLIB_asm(" MARK 5");
	       TI_profile_stop();
	    }

	    // get output to L1D
	    int16_t outSum = 0;
	    int8_t *pYTemp = (int8_t *)pY;       // treat output as bytes to be data type agnostic
	    for (k = 0; k < currPrm.fftSize*currPrm.batchSize*2; k++) {
		outSum += *pYTemp;
		pYTemp++;
	    }

	    // dummy store of outSum to insure that the compiler does not remove it.
	    volatileSum = outSum;

	    // run warm instruction cache test
	    TI_profile_start(TI_PROFILE_KERNEL_OPT_WARMWRB);

	    FFTLIB_asm(" MARK 6");
	    status_opt = FFTLIB_FFT_dftSmall_ixX_cxX_oxX_exec(handle,
							      pX,
							      pW,
							      pY);
	    FFTLIB_asm(" MARK 7");
	    TI_profile_stop();
#endif

#if (!defined(RTL_TEST))
	    /* Test _cn kernel */
	    kerInitArgs.funcStyle = FFTLIB_FUNCTION_NATC;

	    // initialize the kernel to use the natural C variant
	    FFTLIB_FFT_dftSmall_ixX_cxX_oxX_init(handle,
						 &bufParamsX,
						 &bufParamsW,
						 &bufParamsY,
						 &kerInitArgs);

	    TI_profile_start(TI_PROFILE_KERNEL_CN);
	    FFTLIB_asm(" MARK 8");
	    status_nat = FFTLIB_FFT_dftSmall_ixX_cxX_oxX_exec(handle,
							      pX, pW, pYCn);
	    FFTLIB_asm(" MARK 9");
	    TI_profile_stop();

	    status_nat_vs_opt = TI_compare_mem_2D((void *)pY,
						  (void *)pYCn,
						  50,
						  65535,
						  currPrm.batchSize*currPrm.fftSize*2,
						  1,
						  currPrm.batchSize*currPrm.fftSize*2*FFTLIB_sizeof(bufParamsY.data_type),
						  FFTLIB_sizeof(bufParamsY.data_type));
	    comparisonDone = 1;
#else
	    status_nat_vs_opt = TI_TEST_KERNEL_PASS;
#endif

	    if( currPrm.staticOut != NULL ) {
		status_ref_vs_opt = TI_compare_mem_2D((void *)currPrm.staticOut,
						      (void *)pY,
						      50,
						      65535,
						      currPrm.batchSize*currPrm.fftSize*2,
						      1,
						      currPrm.batchSize*currPrm.fftSize*2*FFTLIB_sizeof(bufParamsY.data_type),
						      FFTLIB_sizeof(bufParamsY.data_type));
		comparisonDone = 1;
	    } else {
	       /* Set to pass since it wasn't supposed to run. */
	       status_ref_vs_opt = TI_TEST_KERNEL_PASS;
	    }

	    /* Set the 'fail' flag based on test vector comparison results */
	    currentTestFail = ((status_nat_vs_opt == TI_TEST_KERNEL_FAIL) ||
			       (status_ref_vs_opt == TI_TEST_KERNEL_FAIL) ||
			       (status_init != FFTLIB_SUCCESS) ||
			       (status_opt  != FFTLIB_SUCCESS) ||
			       (status_nat  != FFTLIB_SUCCESS) ||
			       (comparisonDone == 0) ||
			       (currentTestFail == 1)) ? 1 : 0;

	    fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;

	    pProfile[3*tpi]   = (int32_t)TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
	    pProfile[3*tpi+1] = (int32_t)TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
	    pProfile[3*tpi+2] = (int32_t)TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);
#if defined(QT_TEST)
	    __ESTP_GS = pProfile[3*tpi];
	    __FSR     = pProfile[3*tpi+1];
	    __ESTP_S  = pProfile[3*tpi+2];
	    __GPLY    = !fail;
	    __DBGCTXT = testNum;
#endif // #if defined(QT_TEST)

#if !defined(RTL_TEST)
	    sprintf(desc, "%s generated input | fftSize=%d, batchSize=%d",
		    testPatternString, currPrm.fftSize, currPrm.batchSize);

	    FFTLIB_FFT_dftSmall_ixX_cxX_oxX_perfEst(handle,
						    &bufParamsX,
						    &bufParamsY,
						    &bufParamsW,
						    currPrm.fftSize,
						    currPrm.batchSize,
						    &archCycles,
						    &estCycles);

#if ( !defined(RTL_TEST) && !defined(QT_TEST) && !defined(EVM_TEST))
	    // write to CSV, must happen prior to write to screen because TI_profile_formula_add clears values in counters
	    fprintf(fpOutputCSV, "DFT small, %d, %d, %d, FFT:%d Batch:%d Int/Deint: %d, %d, %d, %d, %lu, %lu, %.2f, %d\n", testNum, currPrm.testPattern, FFTLIB_sizeof(bufParamsY.data_type)*8, currPrm.fftSize, currPrm.batchSize, currPrm.interleave, currPrm.fftSize, currPrm.batchSize, currPrm.interleave, archCycles, cycles[TI_PROFILE_KERNEL_OPT], ((FFTLIB_F32)cycles[TI_PROFILE_KERNEL_OPT])/((FFTLIB_F32)archCycles),!currentTestFail);
#endif // #if ( !defined(RTL_TEST) && !defined(QT_TEST) && !defined(EVM_TEST))

	    TI_profile_add_test(testNum++,
				currPrm.fftSize*currPrm.batchSize,
				archCycles,
				estCycles,
				currentTestFail,
				desc);
	 } else {
	    sprintf(desc, "fftSize=%d, batchSize=%d", currPrm.fftSize, currPrm.batchSize);
	    TI_profile_skip_test(desc);
#else
	    // clear the counters between runs; normally handled by TI_profile_add_test
	    TI_profile_clear_run_stats ();
#endif
	 } // end of memory allocation successful?

	 /* Free buffers for each test vector */
	 //free(pYCn);
	 TI_align_free(pX);
	 TI_align_free(pW);
	 TI_align_free(pY);

#if defined(RTL_TEST)
	 // for RTL testing, request to return after first fail
	 if(fail){
	    free(handle);
	    return fail;
	 }
#endif
      } // end repetitions
   } // end idat test cases

   free(handle);

#if ( !defined(RTL_TEST) && !defined(QT_TEST) && !defined(EVM_TEST))
   /* Close results CSV */
   fclose(fpOutputCSV);
#endif //#if ( !defined(RTL_TEST) && !defined(QT_TEST) && !defined(EVM_TEST))

   return fail;
}

int test_main(uint32_t *pProfile) {
#if !defined(_HOST_BUILD)
   if( TI_cache_init()) {
      TI_memError("FFTLIB_FFT_dftSmall_ixX_cxX_oxX");
      return 1;
   } else
#else
      printf("_HOST_BUILD is defined.\n");
#endif
   {
      return FFTLIB_FFT_dftSmall_ixX_cxX_oxX_d(&pProfile[0],
					       0);
   }
}

/* Main call for individual test projects */
#if !defined(__ONESHOTTEST) && !defined(RTL_TEST)
int main()
{
   int fail = 1;

   uint32_t profile[256*3];

   FFTLIB_TEST_init();

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
#endif

/* ======================================================================== */
/*  End of file:  FFTLIB_FFT_dftSmall_ixX_cxX_oxX_d.c                          */
/* ======================================================================== */
