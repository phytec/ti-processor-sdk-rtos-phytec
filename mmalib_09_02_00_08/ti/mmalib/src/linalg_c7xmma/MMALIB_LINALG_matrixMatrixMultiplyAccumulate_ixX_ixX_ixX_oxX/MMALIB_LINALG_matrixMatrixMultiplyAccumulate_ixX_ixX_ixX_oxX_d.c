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
#include "MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_idat.h"

__attribute__((section(".msmcData"), aligned(64))) int8_t msmcBuffer[MMALIB_L3_RESULTS_BUFFER_SIZE];

__attribute__ ((section (".ddrData"), aligned (64))) int8_t ddrBuffer[2048 * 1024];

void MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_perfEst(MMALIB_kernelHandle handle,
									  const MMALIB_bufParams2D_t *src0_addr,
									  const MMALIB_bufParams2D_t *src1_addr,
									  const MMALIB_bufParams2D_t *src2_addr,
									  const MMALIB_bufParams2D_t *dst_addr,
									  uint64_t *archCycles,
									  uint64_t *estCycles,
									  int32_t *caseNumber);

/* MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_d:  Test Driver Routine */
int MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   int32_t  tpi;       /* test parameter index */
   int32_t  currentTestFail;
   int32_t  fail = 0;  /* fail flag */
   uint32_t  repCount; /* number of times to repeat a test; useful for random data and random sizes (set to 1 for static) */
   uint32_t numReps;
   uint32_t testNum;

   // variables for calculating performance estimates
   uint64_t archCycles, estCycles;
   int32_t caseNumber;

   /* Test Parameters */
   matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_testParams_t   *prm;
   matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_testParams_t   currPrm;
   matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_getTestParams(&prm, &test_cases);

   /* Function call parameters */
   MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_InitArgs    kerInitArgs;

   /* Initialize profiling */
   TI_profile_init("MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX");

   /* Run each test case */
   for( tpi=0; tpi < test_cases; tpi++ ) {
      /* Number of repetitions for the test */
      numReps = prm[tpi].numReps;
      testNum = prm[tpi].testID;

      currPrm = prm[tpi];
      int32_t handleSize = MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_getHandleSize(&kerInitArgs);
      MMALIB_kernelHandle handle = malloc(handleSize);

      for(repCount = 0; repCount < numReps; repCount++){
	 /* Initialize status flags */
	 int32_t      status_nat_vs_opt = TI_TEST_KERNEL_FAIL; /* Test status : Natural c vs. Optimized */
	 int32_t      status_ref_vs_opt = TI_TEST_KERNEL_FAIL; /* Test status : Static Reference vs. Optimized */
	 MMALIB_STATUS   status_init = MMALIB_SUCCESS;
	 MMALIB_STATUS   status_opt = MMALIB_SUCCESS;
	 MMALIB_STATUS   status_nat = MMALIB_SUCCESS;
	 currentTestFail = 0;

	 /* Sometimes, depending on certain compile flags, the test will be
	  * marked PASS even if no comparison is done. This flag is to detect
	  * if a comparison was done or not                                   */
	 int32_t comparisonDone = 0;

	 /* Special handling for random dimensions */
	 if(prm[tpi].testPattern == RANDOM_DIMENSION){
	    // set data type to random data
	    currPrm.testPattern = RANDOM_SIGNED;

	    // values provided in prm are considered max values
	    uint32_t Mrand = TI_randomDimensionLessThan((uint32_t)prm[tpi].heightIn0);
	    uint32_t Krand = TI_randomDimensionLessThan((uint32_t)prm[tpi].widthIn0);
	    uint32_t Nrand = TI_randomDimensionLessThan((uint32_t)prm[tpi].widthIn1);

	    currPrm.heightIn0 = Mrand;
	    currPrm.widthIn0  = Krand;
	    currPrm.strideIn0 = currPrm.widthIn0 * MMALIB_sizeof(currPrm.dataTypeIn0);
	    currPrm.heightIn1 = Krand;
	    currPrm.widthIn1  = Nrand;
	    currPrm.strideIn1 = currPrm.widthIn1 * MMALIB_sizeof(currPrm.dataTypeIn1);
	    currPrm.heightIn2 = Mrand;
	    currPrm.widthIn2  = Nrand;
	    currPrm.strideIn2 = currPrm.widthIn2 * MMALIB_sizeof(currPrm.dataTypeIn2);
	    currPrm.heightOut = Mrand;
	    currPrm.widthOut  = Nrand;
	    currPrm.strideOut = currPrm.widthOut * MMALIB_sizeof(currPrm.dataTypeOut);
	 }

	 /* Compute buffer sizes */
	 uint32_t    inp0Size =   currPrm.strideIn0 * currPrm.heightIn0;
	 uint32_t    inp1Size =   currPrm.strideIn1 * currPrm.heightIn1;
	 uint32_t    inp2Size =   currPrm.strideIn2 * currPrm.heightIn2;
	 uint32_t    outSize =   currPrm.strideOut * currPrm.heightOut;
	 uint32_t    num_pts   =   currPrm.heightIn0 * currPrm.widthIn0 * currPrm.widthIn1;

	 /* Allocate buffers for each test vector */
	 void *src0    =  TI_memalign(MMALIB_L2DATA_ALIGNMENT, inp0Size);
         void *src1    = TI_memalign(MMALIB_ALIGN_128BYTES, inp1Size);  // input feature maps
         void *src2    =  TI_memalign(MMALIB_L2DATA_ALIGNMENT, inp2Size);

	 void * dst = NULL;
         if (currPrm.outputDataLocation == MMALIB_TEST_OUTPUT_HEAP){
            dst        = TI_memalign(MMALIB_ALIGN_128BYTES, outSize);
         }
         else if(currPrm.outputDataLocation == MMALIB_TEST_OUTPUT_MSMC) {
            dst        = (void *)msmcBuffer;
         } else {
            dst        = NULL;
         }

	 void *dst_cn = NULL;
	 if (prm[tpi].outputDataLocation == MMALIB_TEST_OUTPUT_HEAP) {
	    dst_cn  =  malloc(outSize);
	 } else {
	    dst_cn = (void *) ddrBuffer;
	 }

	 /* Output shift applied by MMA */
	 kerInitArgs.shift = currPrm.shiftOut;

	 /* Only run the test if the buffer allocations fit in the heap */
	 if( src0 && src1 && dst && dst_cn ) {
	    MMALIB_bufParams2D_t    src0_addr, src1_addr, src2_addr, dst_addr;

	    /* Fill input arrays according to desired test pattern */
	    TI_fillBuffer(currPrm.testPattern,
			  (uint8_t)255,
			  src0, currPrm.staticIn0,
			  currPrm.widthIn0, currPrm.heightIn0, currPrm.strideIn0,
			  MMALIB_sizeof(currPrm.dataTypeIn0), testPatternString);

	    // height of second matrix should be same as width of first
	    TI_fillBuffer(currPrm.testPattern,
			  (uint8_t)255,
			  src1, currPrm.staticIn1,
			  currPrm.widthIn1, currPrm.heightIn1, currPrm.strideIn1,
			  MMALIB_sizeof(currPrm.dataTypeIn1), testPatternString);

	    // size of third input matrix should be same as the output
	    TI_fillBuffer(currPrm.testPattern,
			  (uint8_t)255,
			  src2, currPrm.staticIn2,
			  currPrm.widthIn2, currPrm.heightIn2, currPrm.strideIn2,
			  MMALIB_sizeof(currPrm.dataTypeIn2), testPatternString);

	    src0_addr.dim_x = currPrm.widthIn0;
	    src0_addr.dim_y = currPrm.heightIn0;
	    src0_addr.stride_y = currPrm.strideIn0;
	    src0_addr.data_type = currPrm.dataTypeIn0;

	    src1_addr.dim_x = currPrm.widthIn1;
	    src1_addr.dim_y = currPrm.heightIn1;
	    src1_addr.stride_y = currPrm.strideIn1;
	    src1_addr.data_type = currPrm.dataTypeIn1;

	    src2_addr.dim_x = currPrm.widthIn2;
	    src2_addr.dim_y = currPrm.heightIn2;
	    src2_addr.stride_y = currPrm.strideIn2;
	    src2_addr.data_type = currPrm.dataTypeIn2;

	    dst_addr.dim_x = currPrm.widthOut;
	    dst_addr.dim_y = currPrm.heightOut;
	    dst_addr.stride_y = currPrm.strideOut;
	    dst_addr.data_type = currPrm.dataTypeOut;

	    /* Initialize kernel */
	    status_init = MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_init_checkParams(handle, &src0_addr, &src1_addr,
													&src2_addr, &dst_addr, &kerInitArgs);
	    if(status_init == MMALIB_SUCCESS){
	       TI_profile_start(TI_PROFILE_KERNEL_INIT);
	       MMALIB_asm(" MARK 0");
	       kerInitArgs.funcStyle = MMALIB_FUNCTION_OPTIMIZED;
	       status_init = MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_init(handle,
											       &src0_addr,
											       &src1_addr,
											       &src2_addr,
											       &dst_addr,
											       &kerInitArgs);
	       MMALIB_asm(" MARK 1");
	       TI_profile_stop();
	    }

	    /* Test optimized kernel */
	    status_opt = MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_exec_checkParams(handle, src0, src1, src2, dst);
	    if(status_opt == MMALIB_SUCCESS){
	       TI_profile_start(TI_PROFILE_KERNEL_OPT);
	       MMALIB_asm(" MARK 2");
	       status_opt = MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_exec(handle, src0, src1, src2, dst);
	       MMALIB_asm(" MARK 3");
	       TI_profile_stop();
	    }
	    // run warm instruction cache test
	    TI_profile_start(TI_PROFILE_KERNEL_OPT_WARM);
	    MMALIB_asm(" MARK 4");
	    status_opt = MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_exec(handle,
											   src0,
											   src1,
											   src2,
											   dst);
	    MMALIB_asm(" MARK 5");
	    TI_profile_stop();

	    // get output to L1D
	    int32_t row, col;
	    int16_t outSum = 0;
	    int16_t volatile volatileSum = 0;  // use volatile to keep compiler from removing this operation
	    int8_t *pDst = (int8_t *)dst;       // treat output as bytes to be data type agnostic
	    for(row = 0; row < dst_addr.dim_y; row++)
       {
          for(col = 0; col < dst_addr.stride_y; col++)
          {
             outSum += *pDst;
             pDst++;
          }
       }

	    // dummy store of outSum to insure that the compiler does not remove it.
	    volatileSum = outSum;

	    // run warm instruction cache test
	    TI_profile_start(TI_PROFILE_KERNEL_OPT_WARMWRB);
	    MMALIB_asm(" MARK 6");
	    status_opt = MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_exec(handle,
											   src0,
											   src1,
											   src2,
											   dst);
	    MMALIB_asm(" MARK 7");
	    TI_profile_stop();

	    /* Test _cn kernel */
	    kerInitArgs.funcStyle = MMALIB_FUNCTION_NATC;

	    // initialize the kernel to use the natural C variant
	    MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_init(handle,
									      &src0_addr,
									      &src1_addr,
									      &src2_addr,
									      &dst_addr,
									      &kerInitArgs);

	    TI_profile_start(TI_PROFILE_KERNEL_CN);
	    MMALIB_asm(" MARK 8");
	    status_nat = MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_exec(handle,
											   src0,
											   src1,
											   src2,
											   dst_cn);
	    MMALIB_asm(" MARK 9");
	    TI_profile_stop();

	    /* Compare natural C Output and Optimized Output */
	    status_nat_vs_opt = TI_compare_mem_2D(dst, dst_cn, 0, 0, currPrm.widthOut, currPrm.heightOut, currPrm.strideOut, MMALIB_sizeof(dst_addr.data_type));
	    comparisonDone = 1;

	    /* If static output is available, then additionally compares optimized C output with static reference output data */
	    if( currPrm.staticOut != NULL ) {
	       status_ref_vs_opt = TI_compare_mem_roi2DStatic((void *) dst, (void *) currPrm.staticOut, 0, 0, currPrm.widthOut, currPrm.heightOut, currPrm.strideOut, MMALIB_sizeof(dst_addr.data_type));
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
	    printf("%s: comparison done   = %d\n", __FUNCTION__, comparisonDone);
#endif

	    pProfile[3*tpi]   = (int32_t)TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
	    pProfile[3*tpi+1] = (int32_t)TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
	    pProfile[3*tpi+2] = (int32_t)TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);

	    /* Profile display and preparation for cycle estimation */
	    sprintf(desc, "%s generated input | dataType=%d, m=%d, k=%d, n=%d",
		    testPatternString, dst_addr.data_type, currPrm.heightIn0, currPrm.widthIn0, currPrm.widthIn1);

	    MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_perfEst(handle, &src0_addr, &src1_addr, &src2_addr, &dst_addr, &archCycles, &estCycles, &caseNumber);

	    TI_profile_add_test(testNum++, num_pts, archCycles, estCycles, currentTestFail, desc);

#if MMALIB_DEBUGPRINT
	    if(currentTestFail){
	       printf("\nA is: \n");
	       MMALIB_debugPrintMatrix(src0, &src0_addr);
	       printf("B is: \n");
	       MMALIB_debugPrintMatrix(src1, &src1_addr);
	       printf("D is: \n");
	       MMALIB_debugPrintMatrix(src2, &src2_addr);
	       printf("\nThe natural-c output is: \n");
	       MMALIB_debugPrintMatrix(dst_cn, &dst_addr);
	       printf("The computed output is: \n");
	       MMALIB_debugPrintMatrix(dst, &dst_addr);
	    }
#endif
	 } else {
	    /* Display the error printout for this test vector before moving on to the next test vector */
	    sprintf(desc, "m=%d, k=%d, n=%d",
		    currPrm.heightIn0, currPrm.widthIn0, currPrm.widthIn1);
	    TI_profile_skip_test(desc);
	 } // end of memory allocation successful?

	 /* Free buffers for each test vector */
	 if (prm[tpi].outputDataLocation == MMALIB_TEST_OUTPUT_HEAP) {
	    free(dst_cn);
	 } else {}
         if (currPrm.outputDataLocation == MMALIB_TEST_OUTPUT_HEAP){
            // only malloc'd data can be freed
            TI_align_free(dst);
         }
	 
         TI_align_free(src2);
         TI_align_free(src1);
	TI_align_free(src0);

      } // end repetitions
	 free(handle);
   } // end idat test cases

   /* Provide memory requirements */
   //TI_kernel_memory();

   return fail;
}

int test_main(uint32_t *pProfile) {

   if( TI_cache_init()) {
      TI_memError("MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX");
      return 1;
   } else {
      return MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_d(&pProfile[0], 0);
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
/*  End of file:  MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_d.c                          */
/* ======================================================================== */
