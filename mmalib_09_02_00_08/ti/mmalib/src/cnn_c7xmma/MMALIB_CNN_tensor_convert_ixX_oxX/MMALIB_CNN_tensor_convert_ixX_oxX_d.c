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
#include "MMALIB_CNN_tensor_convert_ixX_oxX_idat.h"

/* #include "MMALIB_CNN_tensor_convert_ixX_oxX.h" */

// use volatile to keep compiler from removing this operation
int16_t volatile volatileSum = 0;

__attribute__ ((section (".msmcData"), aligned (64))) int8_t msmcBuffer[MMALIB_L3_RESULTS_BUFFER_SIZE];

__attribute__ ((section (".ddrData"), aligned (64))) int8_t ddrBuffer[2048 * 1024];


/* MMALIB_CNN_tensor_convert_ixX_oxX_d:  Test Driver Routine */
int MMALIB_CNN_tensor_convert_ixX_oxX_d (uint32_t *pProfile,
                                         uint8_t   LevelOfFeedback)
{

   int32_t  tpi;             // test parameter index
   int32_t  currentTestFail; // status of current test case
   int32_t  fail = 0;        // fail flag
   uint32_t repCount; // number of times to repeat a test; useful for random
                      // data and random sizes (set to 1 for static)
   uint32_t numReps;  // Number of times a given test cases is repeated
   uint32_t testNum;  // test case id

   /* variables for calculating performance estimates */

   uint64_t archCycles = 0, estCycles = 0, idealCycles = 0;
   /* int32_t  caseNumber; */

   /* Test Parameters */

   MMALIB_CNN_tensor_convert_ixX_oxX_testParams_t *prm;
   MMALIB_CNN_tensor_convert_ixX_oxX_testParams_t  currPrm;
   MMALIB_CNN_tensor_convert_ixX_oxX_getTestParams (&prm, &test_cases);
   
   MMALIB_CNN_tensor_convert_ixX_oxX_InitArgs    kerInitArgs;
   MMALIB_CNN_tensor_convert_ixX_oxX_ExecInArgs  kerExecInArgs;
   MMALIB_CNN_tensor_convert_ixX_oxX_ExecOutArgs kerExecOutArgs;

   /* Initialize profiling */

   TI_profile_init ("MMALIB_CNN_tensor_convert_ixX_oxX");

   /* Run each test case */

   for (tpi = 0; tpi < test_cases; tpi++) {

      numReps = prm[tpi].numReps;
      testNum = prm[tpi].testID;
      currPrm = prm[tpi];

      int32_t handleSize =
          MMALIB_CNN_tensor_convert_ixX_oxX_getHandleSize (&kerInitArgs);
      MMALIB_kernelHandle handle = malloc (handleSize);

      for (repCount = 0; repCount < numReps; repCount++) {

         /* Initialize status flags */

         int32_t status_nat_vs_opt =
             TI_TEST_KERNEL_PASS; /* Test status : Natural c vs. Optimized *
                                     since we are not running this */
         int32_t status_ref_vs_opt =
             TI_TEST_KERNEL_FAIL; /* Test status : Static Reference vs.
                                   * Optimized
                                   */
         MMALIB_STATUS currTestStatus = MMALIB_STATUS_NOT_SET;
         currentTestFail           = 0;

         /**************************************
          * Initialization arguments to kernel *
          **************************************/

         int32_t numChannels = currPrm.numChannels;
         int32_t height      = currPrm.height;
         int32_t width       = currPrm.width;
         /* int32_t inScale   = currPrm.inScale; */
         /* int32_t outScale   = currPrm.outScale; */
         /* int32_t inZf   = currPrm.inZf; */
         /* int32_t outZf   = currPrm.outZf; */
         int32_t inPad  = currPrm.inPad;
         int32_t outPad = currPrm.outPad;
         /* float                   inZf            = currPrm.inZf; */
         /* float                   outZf           = currPrm.outZf; */
         int32_t              numRowsQuant    = currPrm.numRowsQuant;
         MMALIB_tensor_format tensorFormatIn  = currPrm.tensorFormatIn;
         MMALIB_tensor_format tensorFormatOut = currPrm.tensorFormatOut;
         int32_t              num_pts         = numChannels * height * width;
         /* MMALIB_quantMethod_type quantMethod     = currPrm.quantMethod; */

         /* Determine MMA_SIZE of test case */

         uint32_t inp_size = 0;
         if (currPrm.tensorFormatIn == MMALIB_CHW) {
            inp_size = currPrm.strideInZ * numChannels;
         }
         else {
            inp_size = currPrm.strideInZ * height;
         }

         uint32_t scale_size = 0;
         uint32_t shift_size = 0;

         uint32_t q_size = 0;

         /* if (quantMethod == MMALIB_QUANT_PER_CHANNEL) { */
         /*    scale_size = currPrm.width * MMALIB_sizeof
          * (currPrm.dataTypeScale); */
         /*    shift_size = currPrm.width * sizeof (uint8_t); */
         /*    // q1_size and q2_size TBD */
         /* } */
         /* else */
         { // scale/shift is scalar
            scale_size = 1 * MMALIB_sizeof (currPrm.dataTypeScale);
            shift_size = 1 * sizeof (uint8_t);

            if (currPrm.dataTypeScale == MMALIB_FLOAT32) {
               q_size = 1 * MMALIB_sizeof (currPrm.dataTypeScale);
            }
            else {
               // 8-bit scale => 32-bit bias for MMA
               // 16-bit scale => 64-bit bias for MMA
               q_size = 4 * MMALIB_sizeof (currPrm.dataTypeScale);
            }
         }
         uint32_t outp_size = 0;
         if (currPrm.tensorFormatOut == MMALIB_CHW) {
            outp_size = currPrm.strideOutZ * numChannels;
         }
         else {
            outp_size = currPrm.strideOutZ * height;
         }

         /* Allocate buffers for each test vector */

         void *src   = TI_memalign (MMALIB_L2DATA_ALIGNMENT, inp_size);
         void *scale = TI_memalign (MMALIB_L2DATA_ALIGNMENT, scale_size);
         void *shift = TI_memalign (MMALIB_L2DATA_ALIGNMENT, shift_size);
         void *q = TI_memalign (MMALIB_L2DATA_ALIGNMENT, q_size);

         void *dst = NULL;

         if (currPrm.outputDataLocation == MMALIB_TEST_OUTPUT_HEAP) {
            dst = TI_memalign (MMALIB_L2DATA_ALIGNMENT, outp_size);
         }
         else if (currPrm.outputDataLocation == MMALIB_TEST_OUTPUT_MSMC) {
            dst = (int8_t *) msmcBuffer;
         } else {
            dst = NULL;
         }

         MMALIB_DEBUGPRINTFN (1, "src = %p, dst = %p\n", src, dst);
         MMALIB_DEBUGPRINTFN (1, "inp_size = %d, outp_size = %d\n", inp_size,
                              outp_size);

         void *dst_cn = NULL;
         if (prm[tpi].outputDataLocation == MMALIB_TEST_OUTPUT_HEAP) {
            dst_cn = malloc (outp_size);
         }
         else {
            dst_cn = (void *) ddrBuffer;
         }

         if (src && scale && shift && dst
             && dst_cn
         ) {

            MMALIB_bufParams3D_t src_addr, dst_addr;
            MMALIB_bufParams1D_t scale_addr;
            /* MMALIB_bufParams1D_t shift_addr; */

            /* Fill input arrays according to desired test pattern */
            if (tensorFormatIn == MMALIB_CHW) {
               TI_fillBuffer3D (
                   currPrm.testPattern, (uint8_t) 255, src, currPrm.staticIn,
                   currPrm.width, currPrm.height, currPrm.numChannels,
                   currPrm.strideInY, currPrm.strideInZ,
                   MMALIB_sizeof (currPrm.dataTypeIn), testPatternString);
            }
            else {
               TI_fillBuffer3D (
                   currPrm.testPattern, (uint8_t) 255, src, currPrm.staticIn,
                   currPrm.numChannels, currPrm.width, currPrm.height,
                   currPrm.strideInY, currPrm.strideInZ,
                   MMALIB_sizeof (currPrm.dataTypeIn), testPatternString);
            }

            // scale buffer
            TI_fillBuffer (
                prm[tpi].testPattern, (uint8_t) 1, scale, currPrm.staticScale,
                1, // warning hard-coding for now since scale is scalar
                1, scale_size, MMALIB_sizeof (currPrm.dataTypeScale),
                testPatternString);

            // shift Buffer
            TI_fillBuffer (prm[tpi].testPattern, (uint8_t) 1, shift,
                           currPrm.staticShift, shift_size, 1, shift_size,
                           sizeof (uint8_t), testPatternString);

            if (currPrm.dataTypeScale == MMALIB_FLOAT32) {
               float staticQ = (*(float *) currPrm.staticQ1) *
                               (*(float *) currPrm.staticQ2);
               TI_fillBuffer (
                   prm[tpi].testPattern, (uint8_t) 1, q, &staticQ,
                   1, // warning hard-coding for now since Q1 is scalar
                   1, q_size, MMALIB_sizeof (currPrm.dataTypeScale),
                   testPatternString);
            }
            else {
               if (currPrm.dataTypeScale == MMALIB_INT8) {
                  int32_t r    = 0;
                  int32_t sum  = 0;
                  int32_t temp = 0;

                  int32_t staticQ = 0;
                  for (r = 0; r < numRowsQuant; r++) {
                     temp = (int32_t) (((int8_t *) currPrm.staticQ2)[r]);
                     sum += temp;
                  }
                  if (numRowsQuant > 0) {
                     staticQ = (*(int8_t *) currPrm.staticQ1) * sum;
                  }
                  TI_fillBuffer (
                      prm[tpi].testPattern, (uint8_t) 1, q, &staticQ, 1,
                      1, // since we support only per kernel quant.
                      q_size, 4 * MMALIB_sizeof (currPrm.dataTypeScale),
                      testPatternString);
               }
               else {
                  int64_t r    = 0;
                  int64_t sum  = 0;
                  int64_t temp = 0;

                  int64_t staticQ = 0;
                  for (r = 0; r < numRowsQuant; r++) {
                     temp = (int64_t) (((int16_t *) currPrm.staticQ2)[r]);
                     sum += temp;
                  }
                  if (numRowsQuant > 0) {
                     staticQ = (*(int16_t *) currPrm.staticQ1) * sum;
                  }
                  TI_fillBuffer (
                      prm[tpi].testPattern, (uint8_t) 1, q, &staticQ, 1,
                      1, // since we support only per kernel quant.
                      q_size, 4 * MMALIB_sizeof (currPrm.dataTypeScale),
                      testPatternString);
               }
            }


            if (tensorFormatIn == MMALIB_CHW) {
               src_addr.dim_x = currPrm.width;
               src_addr.dim_y = currPrm.height;
               src_addr.dim_z = currPrm.numChannels;
            }
            else {
               src_addr.dim_x = currPrm.numChannels;
               src_addr.dim_y = currPrm.width;
               src_addr.dim_z = currPrm.height;
            }
            src_addr.stride_y  = currPrm.strideInY;
            src_addr.stride_z  = currPrm.strideInZ;
            src_addr.data_type = currPrm.dataTypeIn;

            scale_addr.dim_x     = currPrm.width;
            scale_addr.data_type = currPrm.dataTypeScale;

            /* shift_addr.dim_x     = currPrm.width; */
            /* shift_addr.data_type = MMALIB_UINT8; */

            if (tensorFormatOut == MMALIB_CHW) {
               dst_addr.dim_x = currPrm.width;
               dst_addr.dim_y = currPrm.height;
               dst_addr.dim_z = currPrm.numChannels;
            }
            else {
               dst_addr.dim_x = currPrm.numChannels;
               dst_addr.dim_y = currPrm.width;
               dst_addr.dim_z = currPrm.height;
            }
            dst_addr.stride_y  = currPrm.strideOutY;
            dst_addr.stride_z  = currPrm.strideOutZ;
            dst_addr.data_type = currPrm.dataTypeOut;

            // Assign to Init Args and Interface file
            /* kerInitArgs.numChannels = numChannels; */
            /* kerInitArgs.width       = width; */
            /* kerInitArgs.height      = height; */
            kerInitArgs.inPad  = inPad;
            kerInitArgs.outPad = outPad;
/* kerInitArgs.inZf   = inZf; */
/* kerInitArgs.outZf  = outZf; */
/* kerInitArgs.inScale         = scale; */
            kerInitArgs.tensorFormatIn  = tensorFormatIn;
            kerInitArgs.tensorFormatOut = tensorFormatOut;

            {
               kerInitArgs.funcStyle = MMALIB_FUNCTION_OPTIMIZED;
               /* kerInitArgs.funcStyle = MMALIB_FUNCTION_NATC; */
               TI_profile_start (TI_PROFILE_KERNEL_INIT);
               MMALIB_asm (" MARK 0");
                currTestStatus = MMALIB_CNN_tensor_convert_ixX_oxX_init (
                   handle, &src_addr, &scale_addr, &dst_addr, &kerInitArgs);
               MMALIB_DEBUGPRINTFN (1, " currTestStatus: %d\n",  currTestStatus);

               if (currPrm.tensorFormatIn == MMALIB_CHW) {
                  kerExecInArgs.srcDimZ = currPrm.numChannels;
                  kerExecInArgs.srcDimY = currPrm.height;
               }
               else {
                  kerExecInArgs.srcDimZ = currPrm.height;
                  kerExecInArgs.srcDimY = currPrm.width;
               }
               MMALIB_asm (" MARK 1");
               TI_profile_stop ();

               if ( currTestStatus == MMALIB_SUCCESS) {
                  TI_profile_start (TI_PROFILE_KERNEL_OPT);
                  MMALIB_asm (" MARK 2");
                   currTestStatus = MMALIB_CNN_tensor_convert_ixX_oxX_exec (
                      handle, src, scale, (uint8_t *) shift, q, dst,
                      &kerExecInArgs, &kerExecOutArgs);
                  MMALIB_asm (" MARK 3");
                  TI_profile_stop ();
               }
               else {
                  MMALIB_PRINTF ("Opt. code init error: %d\n", currTestStatus);
               }
               /* MMALIB_PRINTF ("dst matrix below:\n"); */
               /* /\* dst_addr.dim_x = currPrm.width + currPrm.outPad; *\/ */
               /* MMALIB_debugPrintMatrix_u16 ((uint16_t *) dst, &dst_addr); */
            }

         if(currTestStatus == MMALIB_SUCCESS)
         {
            /* run warm instruction cache test */
            TI_profile_start (TI_PROFILE_KERNEL_OPT_WARM);
            MMALIB_asm (" MARK 4");
            currTestStatus = MMALIB_CNN_tensor_convert_ixX_oxX_exec (
                handle, src, scale, (uint8_t *) shift, q, dst, &kerExecInArgs,
                &kerExecOutArgs);

            MMALIB_asm (" MARK 5");
            TI_profile_stop ();

            // get output to L1D
            uint32_t row    = 0;
            int32_t  col    = 0;
            int16_t  outSum = 0;
            // treat output as bytes to be data type agnostic
            int8_t *pDst = (int8_t *) dst;
            for (row = 0; row < dst_addr.dim_y; row++) {
               for (col = 0; col < dst_addr.stride_y; col++) {
                  outSum += *pDst;
                  pDst++;
               }
            }

            /* dummy store of outSum to insure that the compiler does not remove it. */
            volatileSum = outSum;

            /* run warm instruction cache test */
            TI_profile_start (TI_PROFILE_KERNEL_OPT_WARMWRB);
            MMALIB_asm (" MARK 6");
            if(currTestStatus == MMALIB_SUCCESS) 
            {
               currTestStatus = MMALIB_CNN_tensor_convert_ixX_oxX_exec (
                handle, src, scale, (uint8_t *) shift, q, dst, &kerExecInArgs,
                &kerExecOutArgs);

            MMALIB_asm (" MARK 7");
            TI_profile_stop ();
            }
         }
         
            /* Test _cn kernel */
            // initialize the kernel to use the natural C variant
            kerInitArgs.funcStyle = MMALIB_FUNCTION_NATC;
         if(currTestStatus == MMALIB_SUCCESS) {
            currTestStatus = MMALIB_CNN_tensor_convert_ixX_oxX_init (
                          handle, &src_addr, &scale_addr, &dst_addr, &kerInitArgs);

            if (currPrm.tensorFormatIn == MMALIB_CHW) {
               kerExecInArgs.srcDimZ = currPrm.numChannels;
               kerExecInArgs.srcDimY = currPrm.height;
            }
            else {
               kerExecInArgs.srcDimZ = currPrm.height;
               kerExecInArgs.srcDimY = currPrm.width;
            }

            if (currTestStatus == MMALIB_SUCCESS) {
               TI_profile_start (TI_PROFILE_KERNEL_CN);
               MMALIB_asm (" MARK 8");
               currTestStatus = MMALIB_CNN_tensor_convert_ixX_oxX_exec (
                   handle, src, scale, (uint8_t *) shift, q, dst_cn,
                   &kerExecInArgs, &kerExecOutArgs);

               MMALIB_asm (" MARK 9");
               TI_profile_stop ();
            }
            else {
               MMALIB_PRINTF ("NatC init error: %d\n", currTestStatus);
            }
          } else {
               if(currPrm.expectedStatusCode != MMALIB_SUCCESS) {
                  status_nat_vs_opt = TI_TEST_KERNEL_PASS;
               }
          }
            /* MMALIB_PRINTF ("dst_cn matrix below:\n"); */
            /* dst_addr.dim_x = currPrm.width + currPrm.outPad; */
            /* MMALIB_debugPrintMatrix_i8 ((int8_t *) dst_cn, &dst_addr); */

            /* Compare natural C Output and Optimized Output */
         if(currTestStatus==MMALIB_SUCCESS)
         {
            if (currPrm.tensorFormatOut == MMALIB_CHW) {
               status_nat_vs_opt = TI_compare_mem_3D (
                   dst, dst_cn, 0, 0, currPrm.width + outPad, currPrm.height,
                   currPrm.numChannels, currPrm.strideOutY, currPrm.strideOutZ,
                   MMALIB_sizeof (dst_addr.data_type));
            }
            else {
               status_nat_vs_opt = TI_compare_mem_3D (
                   dst, dst_cn, 0, 0, currPrm.numChannels,
                   currPrm.width + outPad, currPrm.height, currPrm.strideOutY,
                   currPrm.strideOutZ, MMALIB_sizeof (dst_addr.data_type));
            }
         }
         else {
            if(currPrm.expectedStatusCode  == currTestStatus) {
               status_nat_vs_opt = TI_TEST_KERNEL_PASS;
            }
         }

            /* If static output is available, then additionally compares
             * optimized C output with static reference output data */
            if ((currPrm.staticOut != NULL) && (currTestStatus == MMALIB_SUCCESS)) {

               /* #if MMALIB_DEBUGPRINT */
               /* MMALIB_PRINTF ("Checking ref vs opt\n"); */
               /* #endif */
               if (currPrm.tensorFormatOut == MMALIB_CHW) {
                  status_ref_vs_opt = TI_compare_mem_roi3DStatic (
                      (void *) dst, (void *) currPrm.staticOut, 0, 0,
                      currPrm.width + currPrm.outPad, currPrm.height,
                      currPrm.numChannels, currPrm.strideOutY,
                      currPrm.strideOutZ, MMALIB_sizeof (dst_addr.data_type));
               }
               else {
                  status_ref_vs_opt = TI_compare_mem_roi3DStatic (
                      (void *) dst, (void *) currPrm.staticOut, 0, 0,
                      currPrm.numChannels, currPrm.width + currPrm.outPad,
                      currPrm.height, currPrm.strideOutY, currPrm.strideOutZ,
                      MMALIB_sizeof (dst_addr.data_type));
               }
               /* MMALIB_debugPrintMatrix_i8 ((uint8_t *) dst, */
               /*                             &dst_addr); */
            }
           
            else {
               if(currPrm.expectedStatusCode != MMALIB_SUCCESS) {
                     status_ref_vs_opt = TI_TEST_KERNEL_PASS;
               }
            }
            /* Set the 'fail' flag based on test vector comparison results */
            currentTestFail =
                ((status_nat_vs_opt == TI_TEST_KERNEL_FAIL) ||
                 (status_ref_vs_opt == TI_TEST_KERNEL_FAIL) ||
                 (currTestStatus!= currPrm.expectedStatusCode))
                    ? 1
                    : 0;

            fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;
#if MMALIB_DEBUGPRINT
            MMALIB_PRINTF ("%s: status_nat_vs_opt = %d\n", __FUNCTION__,
                           status_nat_vs_opt);
            MMALIB_PRINTF ("%s: status_ref_vs_opt = %d\n", __FUNCTION__,
                           status_ref_vs_opt);
            MMALIB_PRINTF ("%s:currTestStatus       = %d\n", __FUNCTION__,
                           currTestStatus);
           
#endif

            pProfile[3 * tpi] =
                (int32_t) TI_profile_get_cycles (TI_PROFILE_KERNEL_OPT);
            pProfile[3 * tpi + 1] =
                (int32_t) TI_profile_get_cycles (TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[3 * tpi + 2] =
                (int32_t) TI_profile_get_cycles (TI_PROFILE_KERNEL_OPT_WARMWRB);

            /* Profile display and preparation for cycle estimation */
            sprintf (
                desc, "%s generated input | dataTypeOut=%d, c=%d, h=%d, w=%d",
                testPatternString, dst_addr.data_type, currPrm.numChannels,
                currPrm.height, currPrm.width);
            MMALIB_CNN_tensor_convert_ixX_oxX_perfEst (handle, &src_addr,
                                                       &dst_addr, &idealCycles,
                                                       &archCycles, &estCycles);
            TI_profile_add_test (testNum++, num_pts, archCycles, estCycles,
                                 currentTestFail, desc);
         }
         else {
            /* Display the error printout for this test vector before moving on
             * to the next test vector */
            sprintf (desc, "c=%d, h*w=%d ", currPrm.height, currPrm.width);
            TI_profile_skip_test (desc);
         } // end of memory allocation successful? */

         /* Free buffers for each test vector */
         TI_align_free (src);
         TI_align_free (scale);
         TI_align_free (shift);
         TI_align_free (q);

         if (currPrm.outputDataLocation == MMALIB_TEST_OUTPUT_HEAP) {
            /* only malloc'd data can be freed */
            TI_align_free (dst);
         }
         else {
         }

         if (prm[tpi].outputDataLocation == MMALIB_TEST_OUTPUT_HEAP) {
            free (dst_cn);
         }
         else {
         }

      } // end repetitions
      free (handle);
   } // end idat test cases

   /* Provide memory requirements */
   // TI_kernel_memory();
   return fail;
}

int test_main (uint32_t *pProfile)
{
#if !defined(_HOST_BUILD)
   if (TI_cache_init ()) {
      TI_memError ("MMALIB_CNN_tensor_convert_ixX_oxX");
      return 1;
   }
   else
#else
   printf ("_HOST_BUILD is defined.\n");
#endif //_HOST_BUILD
   {
      return MMALIB_CNN_tensor_convert_ixX_oxX_d (&pProfile[0], 0);
   }
}

/* Main call for individual test projects */
int main ()
{
   int fail = 1;

   uint32_t profile[256 * 3];

   MMALIB_TEST_init ();

   fail = test_main (&profile[0]);

#if !defined(NO_PRINTF)
   if (fail == 0)
      printf ("Test Pass!\n");
   else
      printf ("Test Fail!\n");

   int i;
   for (i = 0; i < test_cases; i++) {
      printf (
          "Test %4d: Cold Cycles = %8d, Warm Cycles = %8d, Warm Cycles WRB = "
          "%8d\n",
          i, profile[3 * i], profile[3 * i + 1], profile[3 * i + 2]);
   }
#endif

   return fail;
}

/* ======================================================================== */
/*  End of file:  MMALIB_CNN_tensor_convert_ixX_oxX_d.c */
/* ======================================================================== */
