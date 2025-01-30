/******************************************************************************/
/*!
 * \file MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_d.c
 */
/* Copyright (C) 2017 Texas Instruments Incorporated - http://www.ti.com/
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

// include MMALIB
#include <mmalib.h>

// include test infrastructure provided by MMALIB
#include "../../common/c71/MMALIB_debug.h"
#include "../../test/MMALIB_test.h"

// include test data for this kernel
#include "MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_idat.h"

__attribute__ ((section (".msmcData"), aligned (64))) int8_t msmcBuffer[MMALIB_L3_RESULTS_BUFFER_SIZE];

__attribute__ ((section (".ddrData"), aligned (64))) int8_t ddrBuffer[2048 * 1024];

/* MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_d:  Test Driver Routine */
int MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_d (uint32_t *pProfile,
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

   uint64_t idealCycles = 0;
   uint64_t archCycles  = 0;
   uint64_t estCycles   = 0;
   int32_t  caseNumber  = 0;

   /* Test Parameters */

   MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_testParams_t *prm;
   MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_testParams_t  currPrm;
   MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_getTestParams (&prm, &test_cases);
   
   /* Function call and execution parameters */

   MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_InitArgs    kerInitArgs;
   MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_ExecInArgs  kerExecInArgs;
   MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_ExecOutArgs kerExecOutArgs;

   /* Initialize profiling */

   TI_profile_init ("MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX");

   /* Run each test case */

   for (tpi = 0; tpi < test_cases; tpi++) {
      numReps = prm[tpi].numReps;
      testNum = prm[tpi].testID;

      currPrm = prm[tpi];

      int32_t handleSize =
          MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_getHandleSize (
              &kerInitArgs);
      MMALIB_kernelHandle handle = malloc (handleSize);

      for (repCount = 0; repCount < numReps; repCount++) {

         /* Initialize status flags */

         int32_t status_nat_vs_opt =
             TI_TEST_KERNEL_FAIL; /* Test status : Natural c vs. Optimized */
         int32_t status_ref_vs_opt =
             TI_TEST_KERNEL_FAIL; /* Test status : Static Reference vs.
                                     Optimized */
         MMALIB_STATUS currTestStatus = MMALIB_STATUS_NOT_SET;

         // dynamic range testing
         int64_t dynamicRangeMax_nat = 0;
         int64_t dynamicRangeMin_nat = 0;
         int64_t dynamicRangeMax_opt = 0;
         int64_t dynamicRangeMin_opt = 0;
         currentTestFail = 0;

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

         int32_t numBytes  = 1;
         int32_t MMA_SIZE  = 0;
         uint8_t dataTypeC = currPrm.dataTypeOut;
         if (dataTypeC == MMALIB_INT8 || dataTypeC == MMALIB_UINT8) {
            MMA_SIZE = MMALIB_MMA_SIZE_8_BIT;
            numBytes = 1;
         }
         else if (dataTypeC == MMALIB_INT16 || dataTypeC == MMALIB_UINT16) {
            MMA_SIZE = MMALIB_MMA_SIZE_16_BIT;
            numBytes = 2;
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
         uint32_t inp2_size =
             currPrm.widthOut * MMALIB_sizeof (currPrm.dataTypeIn2);

         uint32_t outp_size = currPrm.strideOut * currPrm.heightOut;
         uint32_t num_pts =
             currPrm.heightIn0 * currPrm.widthIn0 * currPrm.widthIn1;

         /**************************************************************/
         /* Handle scale and shift values based on quantization method */
         /**************************************************************/

         MMALIB_quantMethod_type quantMethod = currPrm.quantMethod;
         uint32_t                scaleSize   = 0;
         uint32_t                shiftSize   = 0;

         // Includes both shift and scale values and hence twice the size
         if (quantMethod == MMALIB_QUANT_PER_CHANNEL) {
            scaleSize = currPrm.widthOut;
            if (currPrm.widthOut % MMA_SIZE != 0) {
               scaleSize += (currPrm.widthOut % MMA_SIZE) * numBytes;
            }
            shiftSize = currPrm.widthOut;
            if (currPrm.widthOut % MMA_SIZE != 0) {
               shiftSize += (currPrm.widthOut % MMA_SIZE) * numBytes;
            }
         }
         else {
            scaleSize = 1;
            shiftSize = 1;
         }

         /*****************************************/
         /* Allocate buffers for each test vector */
         /*****************************************/

         void *src0     = TI_memalign (MMALIB_L2DATA_ALIGNMENT, inp0_size);
#if 0
         void *src1     = TI_memalign (MMALIB_L2DATA_ALIGNMENT, inp1_size);
#endif // #if 0, WFL: think this needs to be moved lower down
         void *src1     = NULL;
         void *src1_nat = TI_memalign (MMALIB_L2DATA_ALIGNMENT, inp1_size);
         void *src2     = TI_memalign (MMALIB_L2DATA_ALIGNMENT, inp2_size);
         void *src3     = TI_memalign (MMALIB_L2DATA_ALIGNMENT, scaleSize);
         void *src4     = TI_memalign (MMALIB_L2DATA_ALIGNMENT, shiftSize);
         void *dst      = NULL;

         if (currPrm.outputDataLocation == MMALIB_TEST_OUTPUT_HEAP) {
            dst = TI_memalign (MMALIB_L2DATA_ALIGNMENT, outp_size);
         }
         else if (currPrm.outputDataLocation == MMALIB_TEST_OUTPUT_MSMC) {
            dst = (int8_t *) msmcBuffer;
         } else {
            dst = NULL;
         }

         void *dst_cn = NULL;
         if (prm[tpi].outputDataLocation == MMALIB_TEST_OUTPUT_HEAP) {
            dst_cn = malloc (outp_size);
         }
         else {
            dst_cn = (void *) ddrBuffer;
         }

         MMALIB_DEBUGPRINTFN (1,
                              "src0 = %p, src1_nat = %p, src2 = %p, src3 = %p, "
                              "src4 = %p, dst = %p\n",
                              src0, src1_nat, src2, src3, src4, dst);
         MMALIB_DEBUGPRINTFN (1,
                              "inp0_size = %d, inp1_size = %d, scaleSize = %d, "
                              "shiftSize= %d, outp_size = %d\n",
                              inp0_size, inp1_size, scaleSize, shiftSize,
                              outp_size);


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

         kerInitArgs.Ni = currPrm.widthIn0; // Number of input channels
         /* kerInitArgs.shift = currPrm.shiftOut; // Output shift applied */

         kerInitArgs.activationType =
             currPrm.activationType;            // activation type applied
         kerInitArgs.pSatMax = currPrm.pSatMax; // Parameterized saturation max
         kerInitArgs.pSatMin = currPrm.pSatMin; // Parameterized saturation min
         kerExecInArgs.quantMethod = quantMethod; // Quantization method

         /**************************
          * Multipass flags           *
          **************************/

         kerInitArgs.multiPassX = currPrm.multiPassX;
         kerInitArgs.multiPassH = currPrm.multiPassH;

         kerExecInArgs.multiPassX = currPrm.multiPassX;
         kerExecInArgs.multiPassH = currPrm.multiPassH;

         kerExecInArgs.firstCall =
             true; // set to true by default since first init is
                   // outside compute loop in multi-pass case
         kerExecInArgs.lastCall = false; // same comments as firstCall
         kerExecInArgs.writeCBuffer =
             true; // set to true to handle when multipass is disabled

         kerExecInArgs.enableDynamicRange = currPrm.enableDynamicRange;
         kerExecInArgs.initDynamicRange   = currPrm.initDynamicRange;

         /* Only run the test if the buffer allocations fit in the heap */
         if (src0 && src1_nat && src2 && src3 && src4 && dst
             && dst_cn
         ) {
            MMALIB_bufParams2D_t src0_addr, src1_addr, dst_addr;
            MMALIB_bufParams1D_t src2_addr;
            MMALIB_bufParams1D_t src3_addr;

            // non-multi-pass case
            if (!kerExecInArgs.multiPassX && !kerExecInArgs.multiPassH) {
               src0_addr.dim_x     = currPrm.widthIn0;
               src0_addr.dim_y     = currPrm.heightIn0;
               src0_addr.stride_y  = currPrm.strideIn0;
               src0_addr.data_type = currPrm.dataTypeIn0;

               src1_addr.dim_x     = currPrm.widthIn1;
               src1_addr.dim_y     = currPrm.heightIn1;
               src1_addr.stride_y  = currPrm.strideIn1;
               src1_addr.data_type = currPrm.dataTypeIn1;

               src2_addr.dim_x     = currPrm.widthOut;
               src2_addr.data_type = currPrm.dataTypeIn2;

               src3_addr.dim_x     = currPrm.widthOut;
               src3_addr.data_type = currPrm.dataTypeIn3;

               dst_addr.dim_x     = currPrm.widthOut;
               dst_addr.dim_y     = currPrm.heightOut;
               dst_addr.stride_y  = currPrm.strideOut;
               dst_addr.data_type = currPrm.dataTypeOut;
            } else { // multi-pass case; currently does not support cases where X
                   // does not fit and H fits since that may not be plausible

               if (kerExecInArgs.multiPassX) {
                  src0_addr.dim_x = NUM_K_MULTIPASS;
               } else {
                  src0_addr.dim_x = currPrm.widthIn0;
               }

               src0_addr.dim_y     = currPrm.heightIn0;
               src0_addr.stride_y  = currPrm.strideIn0;
               src0_addr.data_type = currPrm.dataTypeIn0;

               src1_addr.dim_x     = currPrm.widthIn1;
               src1_addr.dim_y     = NUM_K_MULTIPASS;
               src1_addr.stride_y  = currPrm.strideIn1;
               src1_addr.data_type = currPrm.dataTypeIn1;

               src2_addr.dim_x     = currPrm.widthOut;
               src2_addr.data_type = currPrm.dataTypeIn2;

               dst_addr.dim_x     = currPrm.widthOut;
               dst_addr.dim_y     = currPrm.heightOut;
               dst_addr.stride_y  = currPrm.strideOut;
               dst_addr.data_type = currPrm.dataTypeOut;
            }

            MMALIB_CNN_fullyConnectedBias_processWeights_Args pWeightsArgs;
            pWeightsArgs.funcStyle     = MMALIB_FUNCTION_OPTIMIZED;
            pWeightsArgs.dim_x         = src1_addr.dim_x;
            pWeightsArgs.dim_y         = src1_addr.dim_y;
            pWeightsArgs.stride_y      = src1_addr.stride_y;
            pWeightsArgs.data_type     = src1_addr.data_type;
#if defined (__SE_FEAT_SPARSITY)
            pWeightsArgs.packetizeMode = 1;
            kerInitArgs.packetizeMode  = pWeightsArgs.packetizeMode;
#endif

            /* Fill input arrays according to desired test pattern */
            TI_fillBuffer (
                currPrm.testPattern, (uint8_t) 255, src0, currPrm.staticIn0,
                currPrm.widthIn0, currPrm.heightIn0, currPrm.strideIn0,
                MMALIB_sizeof (currPrm.dataTypeIn0), testPatternString);

            // this covers the case where the data coming into the function is not reordered for fully connected processing
            if (currPrm.preProcessFlag && currPrm.staticIn1 != NULL) {
               MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_reorderWeights (
                   MMA_SIZE, currPrm.heightIn1, currPrm.widthIn1,
                   currPrm.strideIn1, currPrm.staticIn1, src1_nat); //WFL: changed destination from src1 to src1_nat
            } else {
               // Filter matrix that is already pre-processed
               TI_fillBuffer (
                   currPrm.testPattern, (uint8_t) 255, src1_nat, currPrm.staticIn1,
                   (currPrm.heightIn1 * MMA_SIZE),
                   (currPrm.widthIn1) / (MMA_SIZE), currPrm.strideIn1,
                   MMALIB_sizeof (currPrm.dataTypeIn1), testPatternString);
            } //WFL: moved the closing of the else to here

            // this has to be calculated here because we need to examine the data in order to determine how much it
            // can be compressed.
            int32_t weight_size = MMALIB_CNN_fullyConnectedBias_processWeights_getMemorySize(&pWeightsArgs, src1_nat);

            //src1 = TI_memalign (MMALIB_L2DATA_ALIGNMENT, weight_size);
            src1     = TI_memalign (MMALIB_L2DATA_ALIGNMENT, weight_size+1); // +1 for error checker down below

            // debugging
            //memset(src1, 7, weight_size+1);            

            MMALIB_STATUS status_reorder = MMALIB_CNN_fullyConnectedBias_processWeights_reorder( 
                  &pWeightsArgs,
                  src1_nat,//currPrm.staticIn1,
                  src1
                );
          
#if defined (__SE_FEAT_SPARSITY)
            if (pWeightsArgs.packetizeMode) {
                // Check that the final byte of the buffer is written to and
                // that the next byte of the buffer is not.
                int8_t val0;
                int8_t val1;
                val0 = ((int8_t*)src1)[weight_size-1];
                val1 = ((int8_t*)src1)[weight_size  ]+1;
                ((int8_t*)src1)[weight_size-1] = val0+1;
                ((int8_t*)src1)[weight_size  ] = val1;

                // double checking the reorder function; not strictly necessary; goes with code above 
                MMALIB_CNN_fullyConnectedBias_processWeights_reorder( 
                  &pWeightsArgs,
                  src1_nat,//currPrm.staticIn1,
                  src1
                );

                if (((int8_t*)src1)[weight_size-1] != val0) {
                    MMALIB_DEBUGPRINTFN(1, "val0 mismatch: (%d vs %d)\n",((int8_t*)src1)[weight_size-1],val0);
                }
                if (((int8_t*)src1)[weight_size  ] != val1) {
                    MMALIB_DEBUGPRINTFN(1, "val1 mismatch: (%d vs %d)\n",((int8_t*)src1)[weight_size  ],val1);
                }

            } else
#endif
            {
                if ((uint32_t)weight_size != inp1_size) {
                    MMALIB_DEBUGPRINTFN(1, "weight size is incorrect!!!! (%d vs %d)\n",weight_size,inp1_size);
                }
            }

            if (status_reorder != MMALIB_SUCCESS) {
                MMALIB_DEBUGPRINTFN(1, "reorder failed: %d\n", status_reorder);
                fail = 1;
                return fail;
            }

#if defined (__SE_FEAT_SPARSITY)
            if (!pWeightsArgs.packetizeMode)
#endif
            {
              int32_t comp_status = TI_compare_mem_2D (
                        src1, src1_nat, 0, 0,
                        (currPrm.heightIn1 * MMA_SIZE),
                        (currPrm.widthIn1) / (MMA_SIZE),
                        currPrm.strideIn1,
                        MMALIB_sizeof (currPrm.dataTypeIn1)
                      );
              if (comp_status != TI_TEST_KERNEL_PASS) {
#if defined(MMALIB_DEBUGPRINT)
                printf("Reorder created invalid weights matrix\n");
#endif
                fail = 1;
                return fail;
              }
            }

            //} WFL: moved this closing bracket earlier

            // Bias vector
            TI_fillBuffer (
                currPrm.testPattern, (uint8_t) 255, src2, currPrm.staticIn2,
                (currPrm.widthOut), 1,
                (currPrm.widthOut), // not used here since height is '1'
                MMALIB_sizeof (currPrm.dataTypeIn2), testPatternString);

            // scale buffer
            TI_fillBuffer (prm[tpi].testPattern, (uint8_t) 2, src3,
                           currPrm.staticScale, scaleSize, 1, scaleSize,
                           sizeof (uint8_t), testPatternString);

            // shift Buffer
            TI_fillBuffer (prm[tpi].testPattern, (uint8_t) 12, src4,
                           currPrm.staticShift, shiftSize, 1, shiftSize,
                           sizeof (uint8_t), testPatternString);

            /* Genrate predicate buffer */
            /**
             * @todo Optimize generation only last block when multiPassX is true
             * to save memory footprint of predicate buffer
             *
             */

            /* MMALIB_CNN_generateFillBiasPredicateRegisters (
             *     handle, currPrm.widthIn0, currPrm.heightIn1, MMA_SIZE); */

            /* Initialize kernel */
            // status_init =
            //   MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_init_checkParams (
            //   handle, &src0_addr, &src1_addr, &src2_addr, &dst_addr,
            //   &kerInitArgs);
            

         
         TI_profile_start (TI_PROFILE_KERNEL_INIT);
         MMALIB_asm (" MARK 0");
         kerInitArgs.funcStyle = MMALIB_FUNCTION_OPTIMIZED;
         currTestStatus = MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_init (
                   handle, &src0_addr, &src1_addr, &src2_addr, &src3_addr,
                   &dst_addr, &kerInitArgs);
         MMALIB_asm (" MARK 1");
         TI_profile_stop ();
            
         if (currTestStatus == MMALIB_SUCCESS) {
            /* Test optimized kernel */
            currTestStatus =
                MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_exec_checkParams (
                    handle, src0, src1, src2, dst);
            if (currTestStatus == MMALIB_SUCCESS) {
               TI_profile_start (TI_PROFILE_KERNEL_OPT);
               MMALIB_asm (" MARK 2");

               if (!kerExecInArgs.multiPassH) {
                  currTestStatus = MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_exec (
                      handle, src0, src1, src2, src3, src4, dst, &kerExecInArgs,
                      &kerExecOutArgs);
               }
               else if (kerExecInArgs.multiPassH) {
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
                        kerExecInArgs.firstCall = true;
                        kerExecInArgs.lastCall  = false;
                     }
                     else {
                        kerExecInArgs.firstCall = false;
                        kerExecInArgs.lastCall  = false;
                     }

                     // Is this last call to kernel
                     if (i == (numCalls - 1)) {
                        kerExecInArgs.lastCall = true;

                        if (kerExecInArgs.multiPassX) {
                           src0_addr.dim_x =
                               (currPrm.widthIn0) % (NUM_K_MULTIPASS);
                        }
                        else {
                           src0_addr.dim_x = currPrm.widthIn0;
                        }

                        src1_addr.dim_y =
                            (currPrm.heightIn1) % (NUM_K_MULTIPASS);

                        kerExecInArgs.writeCBuffer = true;

                        currTestStatus =
                            MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_init (
                                handle, &src0_addr, &src1_addr, &src2_addr,
                                &src3_addr, &dst_addr, &kerInitArgs);
                     }
                     else {
                        kerExecInArgs.writeCBuffer = false;
                     }

                     if (kerExecInArgs.multiPassX) {
                        currTestStatus =
                            MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_exec (
                                handle,
                                (void *) ((int8_t *) src0 +
                                          i * (NUM_K_MULTIPASS) *MMALIB_sizeof (
                                                  src0_addr.data_type)),
                                (void *) ((int8_t *) src1 +
                                          i * ((NUM_K_MULTIPASS * MMA_SIZE)) *
                                              MMALIB_sizeof (
                                                  src1_addr.data_type)),
                                src2, src3, src4, dst, &kerExecInArgs,
                                &kerExecOutArgs);
                     }
                     else if (!kerExecInArgs.multiPassX) {
                        currTestStatus =
                            MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_exec (
                                handle, (void *) src0,
                                (void *) ((int8_t *) src1 +
                                          i * ((NUM_K_MULTIPASS * MMA_SIZE)) *
                                              MMALIB_sizeof (
                                                  src1_addr.data_type)),
                                src2, src3, src4, dst, &kerExecInArgs,
                                &kerExecOutArgs);
                     }
                  }
               }
               MMALIB_asm (" MARK 3");
               TI_profile_stop ();
               dynamicRangeMin_opt = kerExecOutArgs.dynamicRangeMin;
               dynamicRangeMax_opt = kerExecOutArgs.dynamicRangeMax;
            }
         }
            /* run warm instruction cache test */
         if(currTestStatus == MMALIB_SUCCESS)
         {
            TI_profile_start (TI_PROFILE_KERNEL_OPT_WARM);
            MMALIB_asm (" MARK 4");
            if (!kerExecInArgs.multiPassH) {
               currTestStatus = MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_exec (
                   handle, src0, src1, src2, src3, src4, dst, &kerExecInArgs,
                   &kerExecOutArgs);
            }
            else if (kerExecInArgs.multiPassH) {
               int32_t numCalls    = (currPrm.heightIn1) / (NUM_K_MULTIPASS);
               int32_t remNumCalls = (currPrm.heightIn1) % (NUM_K_MULTIPASS);

               if (remNumCalls) {
                  numCalls += 1;
               }

               int32_t i = 0;

               for (i = 0; i < numCalls; i++) {

                  // Is this first call to kernel
                  if (!i) {
                     kerExecInArgs.firstCall = true;
                     kerExecInArgs.lastCall  = false;
                  }
                  else {
                     kerExecInArgs.firstCall = false;
                     kerExecInArgs.lastCall  = false;
                  }

                  // Is this last call to kernel
                  if (i == (numCalls - 1)) {
                     kerExecInArgs.lastCall = true;
                     /* src0_addr.dim_x = (currPrm.widthIn0) %
                      * (NUM_K_MULTIPASS); */
                     src1_addr.dim_y = (currPrm.heightIn1) % (NUM_K_MULTIPASS);

                     kerExecInArgs.writeCBuffer = true;

                     currTestStatus =
                         MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_init (
                             handle, &src0_addr, &src1_addr, &src2_addr,
                             &src3_addr, &dst_addr, &kerInitArgs);
                  }
                  else {
                     kerExecInArgs.writeCBuffer = false;
                  }

                  currTestStatus = MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_exec (
                      handle, src0,
                      (void *) ((int8_t *) src1 +
                                i * ((NUM_K_MULTIPASS * src1_addr.dim_x)) *
                                    MMALIB_sizeof (src1_addr.data_type)),
                      src2, src3, src4, dst, &kerExecInArgs, &kerExecOutArgs);
               }
            }
            MMALIB_asm (" MARK 5");
            TI_profile_stop ();
            // get output to L1D
            int32_t row, col;
            int16_t outSum = 0;
            int16_t volatile volatileSum =
                0; // use volatile to keep compiler from removing this operation
            int8_t *pDst = (int8_t *)
                dst; // treat output as bytes to be data type agnostic
            for (row = 0; row < dst_addr.dim_y; row++) {
               for (col = 0; col < dst_addr.stride_y; col++) {
                  outSum += *pDst;
                  pDst++;
               }
            }

            /* dummy store of outSum to insure that the compiler does not remove
             * it. */
            volatileSum = outSum;

            /* run warm instruction cache test */
            TI_profile_start (TI_PROFILE_KERNEL_OPT_WARMWRB);
            MMALIB_asm (" MARK 6");
            if(currTestStatus == MMALIB_SUCCESS) {
            if (!kerExecInArgs.multiPassH) {
               currTestStatus = MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_exec (
                   handle, src0, src1, src2, src3, src4, dst, &kerExecInArgs,
                   &kerExecOutArgs);
            }
            else if (kerExecInArgs.multiPassH) {
               int32_t numCalls    = (currPrm.heightIn1) / (NUM_K_MULTIPASS);
               int32_t remNumCalls = (currPrm.heightIn1) % (NUM_K_MULTIPASS);

               if (remNumCalls) {
                  numCalls += 1;
               }

               int32_t i = 0;

               for (i = 0; i < numCalls; i++) {

                  /* Is this first call to kernel */
                  if (!i) {
                     kerExecInArgs.firstCall = true;
                     kerExecInArgs.lastCall  = false;
                  }
                  else {
                     kerExecInArgs.firstCall = false;
                     kerExecInArgs.lastCall  = false;
                  }

                  /* Is this last call to kernel */
                  if (i == (numCalls - 1)) {
                     kerExecInArgs.lastCall = true;
                     /* src0_addr.dim_x = (currPrm.widthIn0) %
                      * (NUM_K_MULTIPASS); */
                     src1_addr.dim_y = (currPrm.heightIn1) % (NUM_K_MULTIPASS);

                     kerExecInArgs.writeCBuffer = true;

                     currTestStatus =
                         MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_init (
                             handle, &src0_addr, &src1_addr, &src2_addr,
                             &src3_addr, &dst_addr, &kerInitArgs);
                  }
                  else {
                     kerExecInArgs.writeCBuffer = false;
                  }

                  currTestStatus = MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_exec (
                      handle, src0,
                      (void *) ((int8_t *) src1 +
                                i * ((NUM_K_MULTIPASS * src1_addr.dim_x)) *
                                    MMALIB_sizeof (src1_addr.data_type)),
                      dst, src2, src3, src4, &kerExecInArgs, &kerExecOutArgs);
               }
            }

            MMALIB_asm (" MARK 7");
            TI_profile_stop ();
             }
      }

         if(currTestStatus == MMALIB_SUCCESS) 
         {
            /* With RTL, we do not want to consume too many cycles. So, omit
             * running natural C version */
            /* Test _cn kernel */
            kerInitArgs.funcStyle = MMALIB_FUNCTION_NATC;

            // initialize the kernel to use the natural C variant

            if (kerExecInArgs.multiPassH) {
               src0_addr.dim_x =
                   (NUM_K_MULTIPASS); // reset after optimized code overwrite
               src1_addr.dim_y =
                   (NUM_K_MULTIPASS); // reset after optimized code overwrite
            }
            else {
               src1_addr.dim_y = currPrm.heightIn1;
            }
            MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_init (
                handle, &src0_addr, &src1_addr, &src2_addr, &src3_addr,
                &dst_addr, &kerInitArgs);

            
            TI_profile_start (TI_PROFILE_KERNEL_CN);
            MMALIB_asm (" MARK 8");
            if (currTestStatus == MMALIB_SUCCESS) {
            if (!kerExecInArgs.multiPassH) {
               currTestStatus = MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_exec (
                   handle, src0, src1_nat, src2, src3, src4, dst_cn, &kerExecInArgs,
                   &kerExecOutArgs);
            }
            else if (kerExecInArgs.multiPassH) {
               /* currently, multiPassH is true if multiPassX is true;
                * this is expected at test case-level input */
               int32_t numCalls    = (currPrm.heightIn1) / (NUM_K_MULTIPASS);
               int32_t remNumCalls = (currPrm.heightIn1) % (NUM_K_MULTIPASS);

               if (remNumCalls) {
                  numCalls += 1;
               }

               int32_t i = 0;

               for (i = 0; i < numCalls; i++) {
                  kerExecInArgs.blockOffsetA = (i * NUM_K_MULTIPASS);
                  /* Is this first call to kernel */
                  if (!i) {
                     kerExecInArgs.firstCall = true;
                     kerExecInArgs.lastCall  = false;
                  }
                  else {
                     kerExecInArgs.firstCall = false;
                     kerExecInArgs.lastCall  = false;
                  }

                  /* Is this last call to kernel */
                  if (i == (numCalls - 1)) {
                     kerExecInArgs.lastCall = true;

                     src1_addr.dim_y = (currPrm.heightIn1) % (NUM_K_MULTIPASS);

                     kerExecInArgs.writeCBuffer = true;
                     currTestStatus =
                         MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_init (
                             handle, &src0_addr, &src1_addr, &src2_addr,
                             &src3_addr, &dst_addr, &kerInitArgs);
                  }
                  else {
                     kerExecInArgs.writeCBuffer = false;
                  }

                  if (kerExecInArgs.multiPassX) {
                     currTestStatus =
                         MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_exec (
                             handle,
                             (void *) ((int8_t *) src0 +
                                       i * (NUM_K_MULTIPASS) *MMALIB_sizeof (
                                               src0_addr.data_type)),
                             (void *) ((int8_t *) src1_nat +
                                       i * ((NUM_K_MULTIPASS * MMA_SIZE)) *
                                           MMALIB_sizeof (src1_addr.data_type)),
                             src2, src3, src4, dst_cn, &kerExecInArgs,
                             &kerExecOutArgs);
                  }
                  else if (!kerExecInArgs.multiPassX) {
                     currTestStatus =
                         MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_exec (
                             handle, (void *) src0,
                             (void *) ((int8_t *) src1_nat +
                                       i * ((NUM_K_MULTIPASS * MMA_SIZE)) *
                                           MMALIB_sizeof (src1_addr.data_type)),
                             src2, src3, src4, dst_cn, &kerExecInArgs,
                             &kerExecOutArgs);
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
               if(currPrm.expectedStatusCode == currTestStatus) {
                  status_nat_vs_opt = TI_TEST_KERNEL_PASS;
               }
          }
            dynamicRangeMin_nat = kerExecOutArgs.dynamicRangeMin;
            dynamicRangeMax_nat = kerExecOutArgs.dynamicRangeMax;
            //               printf("dynamicRangeMin_nat = %ld\n",
            //               dynamicRangeMin_nat); printf("dynamicRangeMax_nat =
            //               %ld\n", dynamicRangeMax_nat);

            /* Compare natural C Output and Optimized Output */
         if(currTestStatus==MMALIB_SUCCESS){
            status_nat_vs_opt = TI_compare_mem_2D (
                dst, dst_cn, 0, 0, currPrm.widthOut, currPrm.heightOut,
                currPrm.strideOut, MMALIB_sizeof (dst_addr.data_type));
         }
         else{
            status_nat_vs_opt = TI_TEST_KERNEL_PASS;
         }
            MMALIB_DEBUGPRINTFN (
                1, "Without dynamic range, status_nat_vs_opt = %d.\n",
                status_nat_vs_opt);
            MMALIB_DEBUGPRINTFN (
                1, "dynamicRangeMin_nat = %ld, dynamicRangeMin_opt = %ld\n",
                dynamicRangeMin_nat, dynamicRangeMin_opt);
            MMALIB_DEBUGPRINTFN (
                1, "dynamicRangeMax_nat = %ld, dynamicRangeMax_opt = %ld\n",
                dynamicRangeMax_nat, dynamicRangeMax_opt);
            //            printf("dynamicRangeMin_nat = %ld, dynamicRangeMin_opt
            //            = %ld\n", dynamicRangeMin_nat, dynamicRangeMin_opt);
            //            printf("dynamicRangeMax_nat = %ld, dynamicRangeMax_opt
            //            = %ld\n", dynamicRangeMax_nat, dynamicRangeMax_opt);
            if (prm[tpi].enableDynamicRange == MMALIB_DYNAMIC_RANGE_ENABLE) {
               if (dynamicRangeMin_nat != dynamicRangeMin_opt) {
                  status_nat_vs_opt = TI_TEST_KERNEL_FAIL;
                  MMALIB_DEBUGPRINTFN (
                      1,
                      "Dynamic range minimum error.  dynamicRangeMin_nat = "
                      "%ld, dynamicRangeMin_opt = %ld\n",
                      dynamicRangeMin_nat, dynamicRangeMin_opt);
               }
               if (dynamicRangeMax_nat != dynamicRangeMax_opt) {
                  status_nat_vs_opt = TI_TEST_KERNEL_FAIL;
                  MMALIB_DEBUGPRINTFN (
                      1,
                      "Dynamic range maximum error.  dynamicRangeMax_nat = "
                      "%ld, dynamicRangeMax_opt = %ld\n",
                      dynamicRangeMax_nat, dynamicRangeMax_opt);
               }
            }
         
            MMALIB_asm (" MARK 10");
            /* If static output is available, then additionally compares
             * optimized C output with static reference output data */

            if (currPrm.staticOut != NULL && currTestStatus == MMALIB_SUCCESS) {
#if MMALIB_DEBUGPRINT
               printf ("Checking ref vs opt\n");
#endif
               MMALIB_asm (" MARK 11");
               status_ref_vs_opt = TI_compare_mem_roi2DStatic (
                   (void *) dst, (void *) currPrm.staticOut, 0, 0,
                   currPrm.widthOut, currPrm.heightOut, currPrm.strideOut,
                   MMALIB_sizeof (dst_addr.data_type));
            }
            else { 
               status_ref_vs_opt = TI_TEST_KERNEL_PASS;
            }
               if (currPrm.enableDynamicRange == MMALIB_DYNAMIC_RANGE_ENABLE) {
                  if (currPrm.staticInDynamicRangeMin != NULL &&
                      currPrm.staticInDynamicRangeMax != NULL) {
                     if (((int64_t *) currPrm.staticInDynamicRangeMin)[0] !=
                         dynamicRangeMin_opt) {
                        status_ref_vs_opt = TI_TEST_KERNEL_FAIL;
                        printf ("Dynamic range minimum error.  "
                                "dynamicRangeMin_ref = "
                                "%ld, dynamicRangeMin_opt = %ld\n",
                                *(int64_t *) currPrm.staticInDynamicRangeMin,
                                dynamicRangeMin_opt);
                     }
                     if (((int64_t *) currPrm.staticInDynamicRangeMax)[0] !=
                         dynamicRangeMax_opt) {
                        status_ref_vs_opt = TI_TEST_KERNEL_FAIL;
                        printf ("Dynamic range maximum error.  "
                                "dynamicRangeMax_ref = "
                                "%ld, dynamicRangeMax_opt = %ld\n",
                                *(int64_t *) currPrm.staticInDynamicRangeMax,
                                dynamicRangeMax_opt);
                     }
                  }
                  MMALIB_DEBUGPRINTFN (
                      1,
                      "dynamicRangeMin_ref = "
                      "%ld, dynamicRangeMin_opt = %ld\n",
                      *(int64_t *) currPrm.staticInDynamicRangeMin,
                      dynamicRangeMin_opt);
                  MMALIB_DEBUGPRINTFN (
                      1,
                      "dynamicRangeMax_ref = "
                      "%ld, dynamicRangeMax_opt = %ld\n",
                      *(int64_t *) currPrm.staticInDynamicRangeMax,
                      dynamicRangeMax_opt);
               }
            MMALIB_asm (" MARK 12");
            /* Set the 'fail' flag based on test vector comparison results */
            currentTestFail =
                ((status_nat_vs_opt == TI_TEST_KERNEL_FAIL) ||
                 (status_ref_vs_opt == TI_TEST_KERNEL_FAIL) ||
                 (currTestStatus!= currPrm.expectedStatusCode))  ? 1: 0;

            fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;
            MMALIB_asm (" MARK 13");

#if MMALIB_DEBUGPRINT
            printf ("%s: status_nat_vs_opt = %d\n", __FUNCTION__,
                    status_nat_vs_opt);
            printf ("%s: status_ref_vs_opt = %d\n", __FUNCTION__,
                    status_ref_vs_opt);
            printf ("%s: currTestStatus       = %d\n", __FUNCTION__, currTestStatus);
#endif

            MMALIB_asm (" MARK 14");
            pProfile[3 * tpi] =
                (int32_t) TI_profile_get_cycles (TI_PROFILE_KERNEL_OPT);
            pProfile[3 * tpi + 1] =
                (int32_t) TI_profile_get_cycles (TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[3 * tpi + 2] =
                (int32_t) TI_profile_get_cycles (TI_PROFILE_KERNEL_OPT_WARMWRB);
            MMALIB_asm (" MARK 15");

            /* Profile display and preparation for cycle estimation */
            sprintf (desc, "%s generated input | dataType=%d, m=%d, k=%d, n=%d",
                     testPatternString, dst_addr.data_type, currPrm.heightIn0,
                     currPrm.heightIn1, currPrm.widthOut);

            src1_addr.dim_y =
                currPrm.heightIn1; // reset after exec code overwrite
            MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_perfEst (
                handle, &src0_addr, &src1_addr, &dst_addr, &idealCycles,
                &archCycles, &estCycles, &caseNumber);

            MMALIB_asm (" MARK 18");
            TI_profile_add_test (testNum++, num_pts, archCycles, estCycles,
                                 currentTestFail, desc);
#if MMALIB_DEBUGPRINT
            if (currentTestFail) {
               printf ("\nA is: \n");
               MMALIB_debugPrintMatrix ((char *) src0, &src0_addr);
               printf ("B is: \n");
               MMALIB_debugPrintMatrix ((char *) src1_nat, &src1_addr);
               printf ("\nThe correct output is: \n");
               MMALIB_debugPrintMatrix ((char *) dst_cn, &dst_addr);
               printf ("The computed output is: \n");
               MMALIB_debugPrintMatrix ((char *) dst, &dst_addr);
            }
#endif
         }
         else {
            /* Display the error printout for this test vector before moving on
             * to the next test vector */
            MMALIB_asm (" MARK 19");
            sprintf (desc, "m=%d, k=%d, n=%d", currPrm.heightIn0,
                     currPrm.widthIn0, currPrm.widthIn1);
            TI_profile_skip_test (desc);
            MMALIB_asm (" MARK 20");
         } // end of memory allocation successful? */
         MMALIB_asm (" MARK 21");
         /* Free buffers for each test vector */
         TI_align_free (src0);
         TI_align_free (src1);
         TI_align_free (src1_nat);
         TI_align_free (src2);
         TI_align_free (src3);
         TI_align_free (src4);
         MMALIB_asm (" MARK 22");
         if (currPrm.outputDataLocation == MMALIB_TEST_OUTPUT_HEAP) {
            /* only malloc'd data can be freed */
            TI_align_free (dst);
         }
         else {
         }
         MMALIB_asm (" MARK 23");
         if (prm[tpi].outputDataLocation == MMALIB_TEST_OUTPUT_HEAP) {
            free (dst_cn);
         }
         else {
         }

      } // end repetitions
      free (handle);
      MMALIB_asm (" MARK 26");
   } // end idat test cases

   /* Provide memory requirements */
   // TI_kernel_memory();
   MMALIB_asm (" MARK 27");
   return fail;
}

int test_main (uint32_t *pProfile)
{
#if !defined(_HOST_BUILD)
   if (TI_cache_init ()) {
      TI_memError ("MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX");
      return 1;
   }
   else
#else
   printf ("_HOST_BUILD is defined.\n");
#endif //_HOST_BUILD
   {
      return MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_d (&pProfile[0], 0);
   }
}

/* Main call for individual test projects */
int main ()
{
   int fail = 1;

   uint32_t profile[256 * 3];

   MMALIB_TEST_init ();

   fail = test_main (&profile[0]);

   /* #if __TI_COMPILER_VERSION__ > 1003999
    *    printf ("TI_COMP_VER > 1003999\n");
    * #else
    *    printf ("TI_COMP_VER <= 1003999\n");
    * #endif */
   MMALIB_asm (" MARK 28");
#if !defined(NO_PRINTF)

   if (fail == 0)
      printf ("Test Pass!\n");
   else
      printf ("Test Fail!\n");

   MMALIB_asm (" MARK 29");
   int i;
   for (i = 0; i < test_cases; i++) {
      printf ("Test %4d: Cold Cycles = %8d, Warm Cycles = %8d, Warm Cycles WRB "
              "= %8d\n",
              i, profile[3 * i], profile[3 * i + 1], profile[3 * i + 2]);
   }
   MMALIB_asm (" MARK 30");
#endif
   MMALIB_asm (" MARK 31");
   return fail;
}

/* ======================================================================== */
/*  End of file:  MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_d.c */
/* ======================================================================== */
