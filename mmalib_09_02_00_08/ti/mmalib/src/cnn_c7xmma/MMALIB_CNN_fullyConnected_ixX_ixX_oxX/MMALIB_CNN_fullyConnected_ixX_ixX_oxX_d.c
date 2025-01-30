/*******************************************************************************
 **+--------------------------------------------------------------------------+**
 **|                            **** |**
 **|                            **** |**
 **|                            ******o*** |**
 **|                      ********_///_**** |**
 **|                      ***** /_//_/ **** |**
 **|                       ** ** (__/ **** |**
 **|                           ********* |**
 **|                            **** |**
 **|                            *** |**
 **| |**
 **|         Copyright (c) 2016 Texas Instruments Incorporated |**
 **|                        ALL RIGHTS RESERVED |**
 **| |**
 **| Permission to use, copy, modify, or distribute this software, |**
 **| whether in part or in whole, for any purpose is forbidden without |**
 **| a signed licensing agreement and NDA from Texas Instruments |**
 **| Incorporated (TI). |**
 **| |**
 **| TI makes no representation or warranties with respect to the |**
 **| performance of this computer program, and specifically disclaims |**
 **| any responsibility for any damages, special or consequential, |**
 **| connected with the use of this program. |**
 **| |**
 **+--------------------------------------------------------------------------+**
 *******************************************************************************/

// include MMALIB
#include <mmalib.h>

// include test infrastructure provided by MMALIB
#include "../../test/MMALIB_test.h"

// include test data for this kernel
#include "MMALIB_CNN_fullyConnected_ixX_ixX_oxX_idat.h"

// use volatile to keep compiler from removing this operation
int16_t volatile volatileSum = 0;

__attribute__ ((section (".msmcData"), aligned (64)))
int8_t msmcBuffer[MMALIB_L3_RESULTS_BUFFER_SIZE];
__attribute__ ((section (".ddrData"), aligned (64)))
int8_t ddrBuffer[2048 * 1024];

/* MMALIB_CNN_fullyConnected_ixX_ixX_oxX_d:  Test Driver Routine */
int MMALIB_CNN_fullyConnected_ixX_ixX_oxX_d (uint32_t *pProfile,
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

   uint64_t archCycles, estCycles;
   int32_t  caseNumber;

   /* Test Parameters */

   MMALIB_CNN_fullyConnected_ixX_ixX_oxX_testParams_t *prm;
   MMALIB_CNN_fullyConnected_ixX_ixX_oxX_testParams_t  currPrm;
   MMALIB_CNN_fullyConnected_ixX_ixX_oxX_getTestParams (&prm, &test_cases);

   /* Function call and execution parameters */

   MMALIB_CNN_fullyConnected_ixX_ixX_oxX_InitArgs kerInitArgs;
   MMALIB_CNN_fullyConnected_ixX_ixX_oxX_ExecArgs kerExecArgs;

   /* Initialize profiling */

   TI_profile_init ("MMALIB_CNN_fullyConnected_ixX_ixX_oxX");

   /* Run each test case */

   for (tpi = 0; tpi < test_cases; tpi++) {
      numReps = prm[tpi].numReps;
      testNum = prm[tpi].testID;
      currPrm = prm[tpi];

      int32_t handleSize = MMALIB_CNN_fullyConnected_ixX_ixX_oxX_getHandleSize (&kerInitArgs);
      MMALIB_kernelHandle handle = malloc (handleSize);

      for (repCount = 0; repCount < numReps; repCount++) {

         /* Initialize status flags */

         int32_t status_nat_vs_opt = TI_TEST_KERNEL_FAIL; /* Test status : Natural c vs. Optimized */
         int32_t status_ref_vs_opt = TI_TEST_KERNEL_FAIL; /* Test status : Static Reference vs. Optimized */
         MMALIB_STATUS currTestStatus = MMALIB_STATUS_NOT_SET; /* Test current testcase status*/
         currentTestFail           = 0;

         /* Special handling for random dimensions */
         if (prm[tpi].testPattern == RANDOM_DIMENSION) {
            // set data type to random data
            currPrm.testPattern = RANDOM_SIGNED;

            // values provided in prm are considered max values
            uint32_t Mrand =
                TI_randomDimensionLessThan ((uint32_t) prm[tpi].heightIn0);
            uint32_t Krand =
                TI_randomDimensionLessThan ((uint32_t) prm[tpi].widthIn0);
            uint32_t Nrand =
                TI_randomDimensionLessThan ((uint32_t) prm[tpi].widthIn1);

            currPrm.heightIn0 = Mrand;
            currPrm.widthIn0  = Krand;
            currPrm.strideIn0 = MMALIB_CALC_STRIDE (
                currPrm.widthIn0 * MMALIB_sizeof (currPrm.dataTypeIn0),
                MMALIB_ALIGN_SHIFT_64BYTES);

            currPrm.heightIn1 = Krand;
            currPrm.widthIn1  = Nrand;
            currPrm.strideIn1 = MMALIB_CALC_STRIDE (
                currPrm.widthIn1 * MMALIB_sizeof (currPrm.dataTypeIn1),
                MMALIB_ALIGN_SHIFT_64BYTES);
            currPrm.heightOut = Mrand;
            currPrm.widthOut  = Nrand;
            currPrm.strideOut = MMALIB_CALC_STRIDE (
                currPrm.widthOut * MMALIB_sizeof (currPrm.dataTypeOut),
                MMALIB_ALIGN_SHIFT_64BYTES);
         }

         /* Determine MMA_SIZE of test case */

         int32_t MMA_SIZE  = 0;
         uint8_t dataTypeC = currPrm.dataTypeOut;
         if (dataTypeC == MMALIB_INT8 || dataTypeC == MMALIB_UINT8) {
            MMA_SIZE = MMALIB_MMA_SIZE_8_BIT;
         }
         else if (dataTypeC == MMALIB_INT16 || dataTypeC == MMALIB_UINT16) {
            MMA_SIZE = MMALIB_MMA_SIZE_16_BIT;
         }
         else {
            // No else here
         }

         /* Compute buffer sizes */

         uint32_t inp0_size = currPrm.strideIn0 * currPrm.heightIn0;

         /*******************************************************
          * Since filter matrix requires special pre-processing *
          *******************************************************/

         uint32_t numRowsIn1    = (currPrm.widthIn1) / (MMA_SIZE);
         uint32_t remNumRowsIn1 = (currPrm.widthIn1) % (MMA_SIZE);
         if (remNumRowsIn1) {
            numRowsIn1 += 1;
         }
         uint32_t inp1_size = currPrm.strideIn1 * numRowsIn1;

         uint32_t outp_size = currPrm.strideOut * currPrm.heightOut;
         uint32_t num_pts =
             currPrm.heightIn0 * currPrm.widthIn0 * currPrm.widthIn1;

         /* Allocate buffers for each test vector */

         void *src0 = TI_memalign (MMALIB_L2DATA_ALIGNMENT, inp0_size);
         void *src1 = TI_memalign (MMALIB_L2DATA_ALIGNMENT, inp1_size);
         void *dst  = NULL;

         if (currPrm.outputDataLocation == MMALIB_TEST_OUTPUT_HEAP) {
            dst = TI_memalign (MMALIB_L2DATA_ALIGNMENT, outp_size);
         }
         else if (currPrm.outputDataLocation == MMALIB_TEST_OUTPUT_MSMC) {
            dst = (void *) msmcBuffer;
         }

         MMALIB_DEBUGPRINTFN (1, "src0 = %p, src1 = %p, dst = %p\n", src0, src1,
                              dst);
         MMALIB_DEBUGPRINTFN (
             1, "inp0_size = %d, inp1_size = %d, outp_size = %d\n", inp0_size,
             inp1_size, outp_size);

         void *dst_cn = NULL;
         if (prm[tpi].outputDataLocation == MMALIB_TEST_OUTPUT_HEAP) {
            dst_cn = malloc (outp_size);
         }
         else {
            dst_cn = (void *) ddrBuffer;
         }

         /* Buffer to hold accumulated values for natural C code when using
          * multi-pass */

         if (currPrm.multiPassH) {
            // Since MMA's C buffer size is 64 * 64 * 128 (512K) bytes in
            // hardware
            kerInitArgs.bufCAccum_cn =
                (void *) ((int8_t *) msmcBuffer + outp_size);
         }
         else {
            kerInitArgs.bufCAccum_cn = NULL;
         }

         /**************************************
          * Initialization arguments to kernel *
          **************************************/

         kerInitArgs.Ni    = currPrm.widthIn0; // Number of input channels
         kerInitArgs.shift = currPrm.shiftOut; // Output shift applied
         kerInitArgs.bias  = currPrm.biasB;    // scalar bias value appended
         kerInitArgs.activationType =
             currPrm.activationType;               // activation type applied
         kerInitArgs.kDimRows = currPrm.heightIn1; // Ni+Bias rows value

         /**************************
          * Multipass flags	   *
          **************************/

         kerInitArgs.multiPassX = currPrm.multiPassX;
         kerInitArgs.multiPassH = currPrm.multiPassH;

         kerExecArgs.multiPassX = currPrm.multiPassX;
         kerExecArgs.multiPassH = currPrm.multiPassH;

         kerExecArgs.firstCall =
             true; // set to true by default since first init is
                   // outside compute loop in multi-pass case
         kerExecArgs.lastCall = false; // same comments as firstCall
         kerExecArgs.writeCBuffer =
             true; // set to true to handle when multipass is dsiabled

         /* Only run the test if the buffer allocations fit in the heap */

         if (src0 && src1 && dst
             && dst_cn
         ) {
            MMALIB_bufParams2D_t src0_addr, src1_addr, dst_addr;

            /* Fill input arrays according to desired test pattern */
            TI_fillBuffer (
                currPrm.testPattern, (uint8_t) 255, src0, currPrm.staticIn0,
                currPrm.widthIn0, currPrm.heightIn0, currPrm.strideIn0,
                MMALIB_sizeof (currPrm.dataTypeIn0), testPatternString);

            if (currPrm.preProcessFlag && currPrm.staticIn1 != NULL) {
               MMALIB_CNN_fullyConnected_ixX_ixX_oxX_reorderWeights (
                   MMA_SIZE, currPrm.heightIn1, currPrm.widthIn1,
                   currPrm.strideIn1, currPrm.staticIn1, src1);
            }
            else {
               // Filter matrix that is already pre-processed
               TI_fillBuffer (
                   currPrm.testPattern, (uint8_t) 255, src1, currPrm.staticIn1,
                   (currPrm.heightIn1 * MMA_SIZE),
                   (currPrm.widthIn1) / (MMA_SIZE), currPrm.strideIn1,
                   MMALIB_sizeof (currPrm.dataTypeIn1), testPatternString);
            }

            // non-multi-pass case
            if (!kerExecArgs.multiPassX && !kerExecArgs.multiPassH) {
               src0_addr.dim_x     = currPrm.widthIn0;
               src0_addr.dim_y     = currPrm.heightIn0;
               src0_addr.stride_y  = currPrm.strideIn0;
               src0_addr.data_type = currPrm.dataTypeIn0;

               src1_addr.dim_x     = currPrm.widthIn1;
               src1_addr.dim_y     = currPrm.heightIn1;
               src1_addr.stride_y  = currPrm.strideIn1;
               src1_addr.data_type = currPrm.dataTypeIn1;

               dst_addr.dim_x     = currPrm.widthOut;
               dst_addr.dim_y     = currPrm.heightOut;
               dst_addr.stride_y  = currPrm.strideOut;
               dst_addr.data_type = currPrm.dataTypeOut;
            }

            else { // multi-pass case; currently does not support cases where X
                   // does not fit and H fits since that may not be plausible

               if (kerExecArgs.multiPassX) {
                  src0_addr.dim_x = NUM_K_MULTIPASS;
               }
               else {
                  src0_addr.dim_x = currPrm.widthIn0;
               }

               src0_addr.dim_y     = currPrm.heightIn0;
               src0_addr.stride_y  = currPrm.strideIn0;
               src0_addr.data_type = currPrm.dataTypeIn0;

               src1_addr.dim_x     = currPrm.widthIn1;
               src1_addr.dim_y     = NUM_K_MULTIPASS;
               src1_addr.stride_y  = currPrm.strideIn1;
               src1_addr.data_type = currPrm.dataTypeIn1;

               dst_addr.dim_x     = currPrm.widthOut;
               dst_addr.dim_y     = currPrm.heightOut;
               dst_addr.stride_y  = currPrm.strideOut;
               dst_addr.data_type = currPrm.dataTypeOut;
            }

            /* Genrate predicate buffer */
            /**
             * @todo Optimize generation only last block when multiPassX is true
             * to save memory footprint of predicate buffer
             *
             */

            MMALIB_CNN_generateFillBiasPredicateRegisters (
                handle, currPrm.widthIn0, currPrm.heightIn1, MMA_SIZE);

            /* Initialize kernel */
            currTestStatus =
                MMALIB_CNN_fullyConnected_ixX_ixX_oxX_init_checkParams (
                    handle, &src0_addr, &src1_addr, &dst_addr, &kerInitArgs);

            if (currTestStatus == MMALIB_SUCCESS) {
               TI_profile_start (TI_PROFILE_KERNEL_INIT);
               MMALIB_asm (" MARK 0");
               kerInitArgs.funcStyle = MMALIB_FUNCTION_OPTIMIZED;
               currTestStatus = MMALIB_CNN_fullyConnected_ixX_ixX_oxX_init (
                   handle, &src0_addr, &src1_addr, &dst_addr, &kerInitArgs);
               MMALIB_asm (" MARK 1");
               TI_profile_stop ();
            }

            /* Test optimized kernel */
            currTestStatus =
                MMALIB_CNN_fullyConnected_ixX_ixX_oxX_exec_checkParams (
                    handle, src0, src1, dst);
            if (currTestStatus == MMALIB_SUCCESS)
            {
               TI_profile_start (TI_PROFILE_KERNEL_OPT);
               MMALIB_asm (" MARK 2");
               /* printf ("\nChecking predicate buffer in driver before opt
                  exec\n"); n             * uint64_t *ptr; ptr = 0x64E001AC;
                  printf ("\nptr:%016llx %p\n", *ptr, ptr); */
               if (!kerExecArgs.multiPassH) {
                  currTestStatus = MMALIB_CNN_fullyConnected_ixX_ixX_oxX_exec (
                      handle, src0, src1, dst, &kerExecArgs);
               }
               else if (kerExecArgs.multiPassH) {
                  // currently, multiPassH is true if multiPassX is true;
                  // this is expected at test case-level input
                  int32_t numCalls    = (currPrm.heightIn1) / (NUM_K_MULTIPASS);
                  int32_t remNumCalls = (currPrm.heightIn1) % (NUM_K_MULTIPASS);

                  if (remNumCalls) {
                     numCalls += 1;
                  }

                  int32_t i = 0;
                  for (i = 0; i < numCalls; i++) {

                     // Is this first call to kernel
                     if (!i) {
                        kerExecArgs.firstCall = true;
                        kerExecArgs.lastCall  = false;
                     }
                     else {
                        kerExecArgs.firstCall = false;
                        kerExecArgs.lastCall  = false;
                     }

                     // Is this last call to kernel
                     if (i == (numCalls - 1)) {
                        kerExecArgs.lastCall = true;

                        if (kerExecArgs.multiPassX) {
                           src0_addr.dim_x =
                               (currPrm.widthIn0) % (NUM_K_MULTIPASS);
                        }
                        else {
                           src0_addr.dim_x = currPrm.widthIn0;
                        }

                        src1_addr.dim_y =
                            (currPrm.heightIn1) % (NUM_K_MULTIPASS);

                        kerExecArgs.writeCBuffer = true;

                        currTestStatus =
                            MMALIB_CNN_fullyConnected_ixX_ixX_oxX_init (
                                handle, &src0_addr, &src1_addr, &dst_addr,
                                &kerInitArgs);
                     }
                     else {
                        kerExecArgs.writeCBuffer = false;
                     }

                     if (kerExecArgs.multiPassX) {
                        currTestStatus =
                            MMALIB_CNN_fullyConnected_ixX_ixX_oxX_exec (
                                handle,
                                (void *) ((int8_t *) src0 +
                                          i * (NUM_K_MULTIPASS) *MMALIB_sizeof (
                                                  src0_addr.data_type)),
                                (void *) ((int8_t *) src1 +
                                          i * ((NUM_K_MULTIPASS * MMA_SIZE)) *
                                              MMALIB_sizeof (
                                                  src1_addr.data_type)),
                                dst, &kerExecArgs);
                     }
                     else if (!kerExecArgs.multiPassX) {
                        currTestStatus =
                            MMALIB_CNN_fullyConnected_ixX_ixX_oxX_exec (
                                handle, (void *) src0,
                                (void *) ((int8_t *) src1 +
                                          i * ((NUM_K_MULTIPASS * MMA_SIZE)) *
                                              MMALIB_sizeof (
                                                  src1_addr.data_type)),
                                dst, &kerExecArgs);
                     }
                  }
               }
               MMALIB_asm (" MARK 3");
               TI_profile_stop ();
            }

            /* #if 1 */
         if(currTestStatus == MMALIB_SUCCESS)
         {
            if (kerExecArgs.multiPassH) {

               if (kerExecArgs.multiPassX) {
                  src0_addr.dim_x = NUM_K_MULTIPASS;
               }
               else {
                  src0_addr.dim_x = currPrm.widthIn0;
               }

               src0_addr.dim_y     = currPrm.heightIn0;
               src0_addr.stride_y  = currPrm.strideIn0;
               src0_addr.data_type = currPrm.dataTypeIn0;

               src1_addr.dim_x     = currPrm.widthIn1;
               src1_addr.dim_y     = NUM_K_MULTIPASS;
               src1_addr.stride_y  = currPrm.strideIn1;
               src1_addr.data_type = currPrm.dataTypeIn1;

               dst_addr.dim_x     = currPrm.widthOut;
               dst_addr.dim_y     = currPrm.heightOut;
               dst_addr.stride_y  = currPrm.strideOut;
               dst_addr.data_type = currPrm.dataTypeOut;
            }
            /* run warm instruction cache test */
            TI_profile_start (TI_PROFILE_KERNEL_OPT_WARM);
            MMALIB_asm (" MARK 4");
            if (!kerExecArgs.multiPassH) {
               currTestStatus = MMALIB_CNN_fullyConnected_ixX_ixX_oxX_exec (
                   handle, src0, src1, dst, &kerExecArgs);
            }
            else if (kerExecArgs.multiPassH) {
               kerExecArgs.firstCall    = true;
               kerExecArgs.lastCall     = false;
               kerExecArgs.writeCBuffer = false;
               currTestStatus = MMALIB_CNN_fullyConnected_ixX_ixX_oxX_init (
                   handle, &src0_addr, &src1_addr, &dst_addr, &kerInitArgs);
               // currently, multiPassH is true if multiPassX is true;
               // this is expected at test case-level input
               int32_t numCalls    = (currPrm.heightIn1) / (NUM_K_MULTIPASS);
               int32_t remNumCalls = (currPrm.heightIn1) % (NUM_K_MULTIPASS);

               if (remNumCalls) {
                  numCalls += 1;
               }

               int32_t i = 0;
               for (i = 0; i < numCalls; i++) {

                  // Is this first call to kernel
                  if (!i) {
                     kerExecArgs.firstCall = true;
                     kerExecArgs.lastCall  = false;
                  }
                  else {
                     kerExecArgs.firstCall = false;
                     kerExecArgs.lastCall  = false;
                  }

                  // Is this last call to kernel
                  if (i == (numCalls - 1)) {
                     kerExecArgs.lastCall = true;

                     if (kerExecArgs.multiPassX) {
                        src0_addr.dim_x =
                            (currPrm.widthIn0) % (NUM_K_MULTIPASS);
                     }
                     else {
                        src0_addr.dim_x = currPrm.widthIn0;
                     }

                     src1_addr.dim_y = (currPrm.heightIn1) % (NUM_K_MULTIPASS);

                     kerExecArgs.writeCBuffer = true;

                     currTestStatus = MMALIB_CNN_fullyConnected_ixX_ixX_oxX_init (
                         handle, &src0_addr, &src1_addr, &dst_addr,
                         &kerInitArgs);
                  }
                  else {
                     kerExecArgs.writeCBuffer = false;
                  }

                  if (kerExecArgs.multiPassX) {
                     currTestStatus = MMALIB_CNN_fullyConnected_ixX_ixX_oxX_exec (
                         handle,
                         (void *) ((int8_t *) src0 +
                                   i * (NUM_K_MULTIPASS) *MMALIB_sizeof (
                                           src0_addr.data_type)),
                         (void *) ((int8_t *) src1 +
                                   i * ((NUM_K_MULTIPASS * MMA_SIZE)) *
                                       MMALIB_sizeof (src1_addr.data_type)),
                         dst, &kerExecArgs);
                  }
                  else if (!kerExecArgs.multiPassX) {
                     currTestStatus = MMALIB_CNN_fullyConnected_ixX_ixX_oxX_exec (
                         handle, (void *) src0,
                         (void *) ((int8_t *) src1 +
                                   i * ((NUM_K_MULTIPASS * MMA_SIZE)) *
                                       MMALIB_sizeof (src1_addr.data_type)),
                         dst, &kerExecArgs);
                  }
               }
            }
            MMALIB_asm (" MARK 5");
            TI_profile_stop ();
            if (kerExecArgs.multiPassH) {

               if (kerExecArgs.multiPassX) {
                  src0_addr.dim_x = NUM_K_MULTIPASS;
               }
               else {
                  src0_addr.dim_x = currPrm.widthIn0;
               }

               src0_addr.dim_y     = currPrm.heightIn0;
               src0_addr.stride_y  = currPrm.strideIn0;
               src0_addr.data_type = currPrm.dataTypeIn0;

               src1_addr.dim_x     = currPrm.widthIn1;
               src1_addr.dim_y     = NUM_K_MULTIPASS;
               src1_addr.stride_y  = currPrm.strideIn1;
               src1_addr.data_type = currPrm.dataTypeIn1;

               dst_addr.dim_x     = currPrm.widthOut;
               dst_addr.dim_y     = currPrm.heightOut;
               dst_addr.stride_y  = currPrm.strideOut;
               dst_addr.data_type = currPrm.dataTypeOut;
            }
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

            /* dummy store of outSum to insure that the compiler does not remove
             * it. */
            volatileSum = outSum;
            if(currTestStatus == MMALIB_SUCCESS)
            {
               /* run warm instruction cache test */
               TI_profile_start (TI_PROFILE_KERNEL_OPT_WARMWRB);
               MMALIB_asm (" MARK 6");
               if (!kerExecArgs.multiPassH) {
                  currTestStatus = MMALIB_CNN_fullyConnected_ixX_ixX_oxX_exec (
                   handle, src0, src1, dst, &kerExecArgs);
               }
               else if (kerExecArgs.multiPassH) {
                  kerExecArgs.firstCall    = true;
                  kerExecArgs.lastCall     = false;
                  kerExecArgs.writeCBuffer = false;
                  currTestStatus = MMALIB_CNN_fullyConnected_ixX_ixX_oxX_init (
                   handle, &src0_addr, &src1_addr, &dst_addr, &kerInitArgs);
                  // currently, multiPassH is true if multiPassX is true;
                  // this is expected at test case-level input
                  int32_t numCalls    = (currPrm.heightIn1) / (NUM_K_MULTIPASS);
                  int32_t remNumCalls = (currPrm.heightIn1) % (NUM_K_MULTIPASS);

                  if (remNumCalls) {
                   numCalls += 1;
                  }

                  int32_t i = 0;
                  for (i = 0; i < numCalls; i++) {

                     // Is this first call to kernel
                     if (!i) {
                        kerExecArgs.firstCall = true;
                        kerExecArgs.lastCall  = false;
                     }
                     else {
                        kerExecArgs.firstCall = false;
                        kerExecArgs.lastCall  = false;
                     }

                     // Is this last call to kernel
                     if (i == (numCalls - 1)) {
                        kerExecArgs.lastCall = true;

                        if (kerExecArgs.multiPassX) {
                           src0_addr.dim_x =
                            (currPrm.widthIn0) % (NUM_K_MULTIPASS);
                        }
                        else {
                           src0_addr.dim_x = currPrm.widthIn0;
                        }

                        src1_addr.dim_y = (currPrm.heightIn1) % (NUM_K_MULTIPASS);

                        kerExecArgs.writeCBuffer = true;

                        currTestStatus = MMALIB_CNN_fullyConnected_ixX_ixX_oxX_init (
                         handle, &src0_addr, &src1_addr, &dst_addr,
                         &kerInitArgs);
                     }
                     else {
                        kerExecArgs.writeCBuffer = false;
                     }

                     if (kerExecArgs.multiPassX) {
                        currTestStatus = MMALIB_CNN_fullyConnected_ixX_ixX_oxX_exec (
                         handle,
                         (void *) ((int8_t *) src0 +
                                   i * (NUM_K_MULTIPASS) *MMALIB_sizeof (
                                           src0_addr.data_type)),
                         (void *) ((int8_t *) src1 +
                                   i * ((NUM_K_MULTIPASS * MMA_SIZE)) *
                                       MMALIB_sizeof (src1_addr.data_type)),
                         dst, &kerExecArgs);
                     }
                     else if (!kerExecArgs.multiPassX) {
                        currTestStatus = MMALIB_CNN_fullyConnected_ixX_ixX_oxX_exec (
                         handle, (void *) src0,
                         (void *) ((int8_t *) src1 +
                                   i * ((NUM_K_MULTIPASS * MMA_SIZE)) *
                                       MMALIB_sizeof (src1_addr.data_type)),
                         dst, &kerExecArgs);
                     }
                  }
               }
               MMALIB_asm (" MARK 7");
               TI_profile_stop ();
            }
         }

         if(currTestStatus == MMALIB_SUCCESS) 
         {
            /* Test _cn kernel */
            kerInitArgs.funcStyle = MMALIB_FUNCTION_NATC;

            // initialize the kernel to use the natural C variant
            if (kerExecArgs.multiPassH) {
               src0_addr.dim_x =
                   (NUM_K_MULTIPASS); // reset after optimized code overwrite
               src1_addr.dim_y =
                   (NUM_K_MULTIPASS); // reset after optimized code overwrite
            }
            else {
               src1_addr.dim_y = currPrm.heightIn1;
            }
            currTestStatus = MMALIB_CNN_fullyConnected_ixX_ixX_oxX_init (
                handle, &src0_addr, &src1_addr, &dst_addr, &kerInitArgs);

            TI_profile_start (TI_PROFILE_KERNEL_CN);
            MMALIB_asm (" MARK 8");
            if(currTestStatus == MMALIB_SUCCESS){
               if (!kerExecArgs.multiPassH) {
                  currTestStatus = MMALIB_CNN_fullyConnected_ixX_ixX_oxX_exec (
                   handle, src0, src1, dst_cn, &kerExecArgs);
               }
               else if (kerExecArgs.multiPassH) {
                  /* currently, multiPassH is true if multiPassX is true;
                  * this is expected at test case-level input */
                  int32_t numCalls    = (currPrm.heightIn1) / (NUM_K_MULTIPASS);
                  int32_t remNumCalls = (currPrm.heightIn1) % (NUM_K_MULTIPASS);

                  if (remNumCalls) {
                     numCalls += 1;
                  }  

                  int32_t i = 0;

                  for (i = 0; i < numCalls; i++) {
                     kerExecArgs.blockOffsetA = (i * NUM_K_MULTIPASS);
                     /* Is this first call to kernel */
                     if (!i) {
                        kerExecArgs.firstCall = true;
                        kerExecArgs.lastCall  = false;
                     }
                     else {
                        kerExecArgs.firstCall = false;
                        kerExecArgs.lastCall  = false;
                     }

                     /* Is this last call to kernel */
                     if (i == (numCalls - 1)) {
                        kerExecArgs.lastCall = true;

                        src1_addr.dim_y = (currPrm.heightIn1) % (NUM_K_MULTIPASS);

                        kerExecArgs.writeCBuffer = true;

                        currTestStatus = MMALIB_CNN_fullyConnected_ixX_ixX_oxX_init (
                         handle, &src0_addr, &src1_addr, &dst_addr,
                         &kerInitArgs);
                     }
                     else {
                        kerExecArgs.writeCBuffer = false;
                     }

                     if (kerExecArgs.multiPassX) {
                        currTestStatus = MMALIB_CNN_fullyConnected_ixX_ixX_oxX_exec (
                         handle,
                         (void *) ((int8_t *) src0 +
                                   i * (NUM_K_MULTIPASS) *MMALIB_sizeof (
                                           src0_addr.data_type)),
                         (void *) ((int8_t *) src1 +
                                   i * ((NUM_K_MULTIPASS * MMA_SIZE)) *
                                       MMALIB_sizeof (src1_addr.data_type)),
                         dst_cn, &kerExecArgs);
                     }
                     else if (!kerExecArgs.multiPassX) {
                        currTestStatus = MMALIB_CNN_fullyConnected_ixX_ixX_oxX_exec (
                         handle, (void *) src0,
                         (void *) ((int8_t *) src1 +
                                   i * ((NUM_K_MULTIPASS * MMA_SIZE)) *
                                       MMALIB_sizeof (src1_addr.data_type)),
                         dst_cn, &kerExecArgs);
                     }
                  }
               }
               MMALIB_asm (" MARK 9");
               TI_profile_stop ();
            }
            else {
               MMALIB_PRINTF ("NatC init error: %d\n", currTestStatus);
            }
         }else {
            status_nat_vs_opt = TI_TEST_KERNEL_PASS;
         }
         
            /* Compare natural C Output and Optimized Output */
            if(currTestStatus == MMALIB_SUCCESS) 
            {
               status_nat_vs_opt = TI_compare_mem_2D (
                  dst, dst_cn, 0, 0, currPrm.widthOut, currPrm.heightOut,
                  currPrm.strideOut, MMALIB_sizeof (dst_addr.data_type));
            }else {
               if(currPrm.expectedStatusCode == currTestStatus) {
                  status_nat_vs_opt = TI_TEST_KERNEL_PASS;
               }
            }

            /* If static output is available, then additionally compares
             * optimized C output with static reference output data */
            if (currPrm.staticOut != NULL && currTestStatus == MMALIB_SUCCESS) {

#if MMALIB_DEBUGPRINT
               printf ("Checking ref vs opt\n");
#endif

               status_ref_vs_opt = TI_compare_mem_roi2DStatic (
                   (void *) dst, (void *) currPrm.staticOut, 0, 0,
                   currPrm.widthOut, currPrm.heightOut, currPrm.strideOut,
                   MMALIB_sizeof (dst_addr.data_type));
            }
            else {
               /* Set to pass since it wasn't supposed to run. */
                  status_ref_vs_opt = TI_TEST_KERNEL_PASS;
              
            }

            /* Set the 'fail' flag based on test vector comparison results */
            currentTestFail =
                ((status_nat_vs_opt == TI_TEST_KERNEL_FAIL) ||
                 (status_ref_vs_opt == TI_TEST_KERNEL_FAIL) ||
                 (currTestStatus != currPrm.expectedStatusCode)) ? 1 : 0; 
                 
            fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;

#if MMALIB_DEBUGPRINT
            printf ("%s: status_nat_vs_opt = %d\n", __FUNCTION__,
                    status_nat_vs_opt);
            printf ("%s: status_ref_vs_opt = %d\n", __FUNCTION__,
                    status_ref_vs_opt);
            printf ("%s: currTestStatus       = %d\n", __FUNCTION__, currTestStatus);
#endif

            pProfile[3 * tpi] =
                (int32_t) TI_profile_get_cycles (TI_PROFILE_KERNEL_OPT);
            pProfile[3 * tpi + 1] =
                (int32_t) TI_profile_get_cycles (TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[3 * tpi + 2] =
                (int32_t) TI_profile_get_cycles (TI_PROFILE_KERNEL_OPT_WARMWRB);

            /* Profile display and preparation for cycle estimation */
            sprintf (desc, "%s generated input | dataType=%d, m=%d, k=%d, n=%d",
                     testPatternString, dst_addr.data_type, currPrm.heightIn0,
                     currPrm.heightIn1, currPrm.widthOut);

            src1_addr.dim_y =
                currPrm.heightIn1; // reset after exec code overwrite
            MMALIB_CNN_fullyConnected_ixX_ixX_oxX_perfEst (
                handle, &src0_addr, &src1_addr, &dst_addr, &archCycles,
                &estCycles, &caseNumber);

            TI_profile_add_test (testNum++, num_pts, archCycles, estCycles,
                                 currentTestFail, desc);
         }
         else {
            /* Display the error printout for this test vector before moving on
             * to the next test vector */
            sprintf (desc, "m=%d, k=%d, n=%d", currPrm.heightIn0,
                     currPrm.widthIn0, currPrm.widthIn1);
            TI_profile_skip_test (desc);
         } // end of memory allocation successful? */

         /* Free buffers for each test vector */
         TI_align_free (src1);
         TI_align_free (src0);
         if (currPrm.outputDataLocation == MMALIB_TEST_OUTPUT_HEAP) {
            /* only malloc'd data can be freed */
            TI_align_free (dst);
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
      TI_memError ("MMALIB_CNN_fullyConnected_ixX_ixX_oxX");
      return 1;
   }
   else
#else
   printf ("_HOST_BUILD is defined.\n");
#endif //_HOST_BUILD
   {
      return MMALIB_CNN_fullyConnected_ixX_ixX_oxX_d (&pProfile[0], 0);
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
      printf ("Test %4d: Cold Cycles = %8d, Warm Cycles = %8d, Warm Cycles WRB "
              "= %8d\n",
              i, profile[3 * i], profile[3 * i + 1], profile[3 * i + 2]);
   }
#endif

   return fail;
}

/* ======================================================================== */
/*  End of file:  MMALIB_CNN_fullyConnected_ixX_ixX_oxX_d.c */
/* ======================================================================== */
