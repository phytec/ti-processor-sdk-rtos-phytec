/******************************************************************************/
/*!
 * \file MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_d.c
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
#include "MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_idat.h"


__attribute__((section(".msmcData"), aligned(64))) int8_t msmcBuffer[MMALIB_L3_RESULTS_BUFFER_SIZE];

__attribute__ ((section (".ddrData"), aligned (64))) int8_t ddrBuffer[2048 * 1024];


/* MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_d:  Test Driver Routine */
int MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_d (uint32_t *pProfile,
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

   matrixMatrixMultiplyBias_ixX_ixX_oxX_testParams_t *prm;
   matrixMatrixMultiplyBias_ixX_ixX_oxX_testParams_t  currPrm;
   matrixMatrixMultiplyBias_ixX_ixX_oxX_getTestParams (&prm, &test_cases);

   /* Function call and execution parameters */

   MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_InitArgs    kerInitArgs;

   /* Initialize profiling */

   TI_profile_init ("MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX");

   /* Run each test case */

   for (tpi = 0; tpi < test_cases; tpi++) {
      numReps = prm[tpi].numReps;
      testNum = prm[tpi].testID;

      currPrm = prm[tpi];

      int32_t handleSize =
          MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_getHandleSize (
              &kerInitArgs);
      MMALIB_kernelHandle handle = malloc (handleSize);

      for (repCount = 0; repCount < numReps; repCount++) {

         /* Initialize status flags */

         int32_t status_nat_vs_opt =
             TI_TEST_KERNEL_FAIL; /* Test status : Natural c vs. Optimized */
         int32_t status_ref_vs_opt =
             TI_TEST_KERNEL_FAIL; /* Test status : Static Reference vs.
                                     Optimized */
         MMALIB_STATUS status_init = MMALIB_SUCCESS;
         MMALIB_STATUS status_opt  = MMALIB_SUCCESS;
         MMALIB_STATUS status_nat  = MMALIB_SUCCESS;

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

            if(currPrm.bTranspose == MMALIB_LINALG_MATRIXMATRIXMULTIPLY_IXX_IXX_OXX_B_TRANSPOSED)
            {
               currPrm.heightIn1 = Nrand;
               currPrm.widthIn1  = Krand;
            }
            else
            {
               currPrm.heightIn1 = Krand;
               currPrm.widthIn1  = Nrand;
            }
            currPrm.strideIn1 = MMALIB_CALC_STRIDE (
                currPrm.widthIn1 * MMALIB_sizeof (currPrm.dataTypeIn1),
                MMALIB_ALIGN_SHIFT_64BYTES);

            currPrm.heightOut = Mrand;
            currPrm.widthOut  = Nrand;
            currPrm.strideOut = MMALIB_CALC_STRIDE (
                currPrm.widthOut * MMALIB_sizeof (currPrm.dataTypeOut),
                MMALIB_ALIGN_SHIFT_64BYTES);
         }

         /* Compute buffer sizes */

         uint32_t inp0_size = currPrm.numChannels * currPrm.strideIn0 * currPrm.heightIn0;
         uint32_t inp1_size = currPrm.numChannels * currPrm.strideIn1 * currPrm.heightIn1;
         uint32_t inp2_size;
         if(currPrm.biasOrder == 1)
         {
            inp2_size =
             currPrm.numChannels * currPrm.heightOut * MMALIB_sizeof (currPrm.dataTypeIn2);
         }
         else
         {
            inp2_size =
             currPrm.numChannels * currPrm.widthOut * MMALIB_sizeof (currPrm.dataTypeIn2);
         }

         uint32_t outp_size = currPrm.numChannels * currPrm.strideOut * currPrm.heightOut;
         uint32_t num_pts =
             currPrm.numChannels * currPrm.heightIn0 * currPrm.widthIn0 * currPrm.widthIn1;

         /**************************************************************/
         /* Handle scale and shift values based on quantization method */
         /**************************************************************/

         int8_t      scaleShiftEnable = currPrm.scaleShiftEnable;
         int8_t      scaleShiftOrder  = currPrm.scaleShiftOrder;
         uint32_t    scaleSize   = 0;
         uint32_t    shiftSize   = 0;

         // Includes both shift and scale values and hence twice the size
         if (scaleShiftEnable == 1) {
            if(scaleShiftOrder == 1)
            {
               scaleSize = currPrm.heightOut;
               shiftSize = currPrm.heightOut;
            }
            else
            {
               scaleSize = currPrm.widthOut;
               shiftSize = currPrm.widthOut;
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
         void *src1     = TI_memalign (MMALIB_L2DATA_ALIGNMENT, inp1_size);
         void *src2     = TI_memalign (MMALIB_L2DATA_ALIGNMENT, inp2_size);
         void *src3     = TI_memalign (MMALIB_L2DATA_ALIGNMENT, currPrm.numChannels * scaleSize);
         void *src4     = TI_memalign (MMALIB_L2DATA_ALIGNMENT, currPrm.numChannels * shiftSize);
         void *dst      = NULL;

         if (currPrm.outputDataLocation == MMALIB_TEST_OUTPUT_HEAP){
            dst        = TI_memalign(MMALIB_ALIGN_128BYTES, outp_size);
         } else if(currPrm.outputDataLocation == MMALIB_TEST_OUTPUT_MSMC) {
            dst        = (void *)msmcBuffer;
         } else {
            dst        = NULL;
         }


         void *dst_cn = NULL;
         if (prm[tpi].outputDataLocation == MMALIB_TEST_OUTPUT_HEAP) {
            dst_cn = malloc (outp_size);
         }
         else {
            dst_cn = (void *) ddrBuffer;
         }

         MMALIB_DEBUGPRINTFN (1,
                              "src0 = %p, src1 = %p, src2 = %p, src3 = %p, "
                              "src4 = %p, dst = %p\n",
                              src0, src1, src2, src3, src4, dst);
         MMALIB_DEBUGPRINTFN (1,
                              "inp0_size = %d, inp1_size = %d, inp2_size = %d, scaleSize = %d, "
                              "shiftSize= %d, outp_size = %d\n",
                              inp0_size, inp1_size, inp2_size, scaleSize, shiftSize,
                              outp_size);

         /**************************************
          * Initialization arguments to kernel *
          **************************************/

         kerInitArgs.activationType =
             currPrm.activationType;            // activation type applied
         kerInitArgs.pSatMax = currPrm.pSatMax; // Parameterized saturation max
         kerInitArgs.pSatMin = currPrm.pSatMin; // Parameterized saturation min

         kerInitArgs.bTranspose = currPrm.bTranspose;
         kerInitArgs.biasOrder = currPrm.biasOrder;
         kerInitArgs.scaleShiftOrder = currPrm.scaleShiftOrder;
         kerInitArgs.scaleAndShiftFlag = currPrm.scaleShiftEnable;

         /* Only run the test if the buffer allocations fit in the heap */
         if (src0 && src1 && src2 && src3 && src4 && dst
             && dst_cn
         ) {
            MMALIB_bufParams3D_t src0_addr, src1_addr, dst_addr;
            MMALIB_bufParams2D_t src2_addr;
            MMALIB_bufParams2D_t src3_addr;

            src0_addr.dim_x     = currPrm.widthIn0;
            src0_addr.dim_y     = currPrm.heightIn0;
            src0_addr.stride_y  = currPrm.strideIn0;
            src0_addr.dim_z     = currPrm.numChannels;
            src0_addr.stride_z  = currPrm.heightIn0 * currPrm.strideIn0;
            src0_addr.data_type = currPrm.dataTypeIn0;

            src1_addr.dim_x     = currPrm.widthIn1;
            src1_addr.dim_y     = currPrm.heightIn1;
            src1_addr.stride_y  = currPrm.strideIn1;
            src1_addr.dim_z     = currPrm.numChannels;
            src1_addr.stride_z  = currPrm.heightIn1 * currPrm.strideIn1;
            src1_addr.data_type = currPrm.dataTypeIn1;

            if(currPrm.biasOrder == 1)
            {
               src2_addr.dim_x  = currPrm.heightOut;
            }
            else
            {
               src2_addr.dim_x  = currPrm.widthOut;
            }
            src2_addr.dim_y     = currPrm.numChannels;
            src2_addr.stride_y  = src2_addr.dim_x * MMALIB_sizeof (currPrm.dataTypeIn2);
            src2_addr.data_type = currPrm.dataTypeIn2;

            src3_addr.dim_x     = scaleSize;
            src3_addr.dim_y     = currPrm.numChannels;
            src3_addr.stride_y  = src3_addr.dim_x * MMALIB_sizeof (currPrm.dataTypeIn3);
            src3_addr.data_type = currPrm.dataTypeIn3;

            dst_addr.dim_x     = currPrm.widthOut;
            dst_addr.dim_y     = currPrm.heightOut;
            dst_addr.stride_y  = currPrm.strideOut;
            dst_addr.dim_z     = currPrm.numChannels;
            dst_addr.stride_z  = currPrm.heightOut * currPrm.strideOut;
            dst_addr.data_type = currPrm.dataTypeOut;

            uint32_t ch=0;
            /* Fill A input matrix according to desired test pattern */
            for (ch=0; ch < currPrm.numChannels; ch++)
            {
               TI_fillBuffer (
                  currPrm.testPattern, (uint8_t) 255, ((int8_t*)src0 + ch*src0_addr.stride_z), ((int8_t*)currPrm.staticIn0 + ch*src0_addr.stride_z),
                  currPrm.widthIn0, currPrm.heightIn0, currPrm.strideIn0,
                  MMALIB_sizeof (currPrm.dataTypeIn0), testPatternString);

               /* Fill B input matrix according to desired test pattern */
               TI_fillBuffer (
                  currPrm.testPattern, (uint8_t) 255, ((int8_t*)src1 + ch*src1_addr.stride_z), ((int8_t*)currPrm.staticIn1 + ch*src1_addr.stride_z),
                  currPrm.widthIn1, currPrm.heightIn1, currPrm.strideIn1,
                  MMALIB_sizeof (currPrm.dataTypeIn1), testPatternString);
            }

 
            // the full range of random values does not make sense for bias, scale, or shift
            uint8_t biasPattern, scalePattern, shiftPattern;
            if( (currPrm.testPattern == RANDOM_SIGNED) ||
                (currPrm.testPattern == RANDOM) ){
               biasPattern  = RANDOM_SMALL;
               scalePattern = RANDOM_SMALL;
               shiftPattern = RANDOM_SMALL;
            } else {
               biasPattern  = currPrm.testPattern;
               scalePattern = currPrm.testPattern;
               shiftPattern = currPrm.testPattern;
            }

            // copy/generate the bias vector
            if(currPrm.testPattern == STATIC && currPrm.staticIn2 == NULL)
            {
               TI_align_free (src2);
               src2 = NULL;
            }
            else
            {
               for (ch=0; ch < currPrm.numChannels; ch++)
               {
                  if(currPrm.biasOrder == 1)
                  {
                     TI_fillBuffer (
                        biasPattern, (uint8_t) 255, ((int8_t*)src2 + ch*src2_addr.stride_y), ((int8_t*)currPrm.staticIn2 + ch*src2_addr.stride_y),
                        (currPrm.heightOut), 1,
                        (currPrm.heightOut * MMALIB_sizeof (currPrm.dataTypeIn2)), // not used here since height is '1'
                        MMALIB_sizeof (currPrm.dataTypeIn2), testPatternString);
                  }
                  else
                  {
                     TI_fillBuffer (
                        biasPattern, (uint8_t) 255, ((int8_t*)src2 + ch*src2_addr.stride_y), ((int8_t*)currPrm.staticIn2 + ch*src2_addr.stride_y),
                        (currPrm.widthOut), 1,
                        (currPrm.widthOut * MMALIB_sizeof (currPrm.dataTypeIn2)), // not used here since height is '1'
                        MMALIB_sizeof (currPrm.dataTypeIn2), testPatternString);
                  }
               }
            }

            for (ch=0; ch < currPrm.numChannels; ch++)
            {
               // copy/generate the scale vector
               TI_fillBuffer(scalePattern,
                           (uint8_t)12,
                           ((int8_t*)src3 + ch*src3_addr.stride_y), ((int8_t*)currPrm.staticScale + ch*src3_addr.stride_y),
                           scaleSize, 1, 0,
                           MMALIB_sizeof(src3_addr.data_type), testPatternString);
            
               // copy/generate the shift vector
               TI_fillBuffer(shiftPattern,
                           (uint8_t)4,
                           ((int8_t*)src4 + ch*src3_addr.stride_y), ((int8_t*)currPrm.staticShift + ch*src3_addr.stride_y),
                           shiftSize, 1, 0,
                           sizeof(uint8_t), testPatternString);
            }


            /* Initialize kernel */
            status_init =
               MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_init_checkParams (
               handle, &src0_addr, &src1_addr, &src2_addr, &dst_addr,
               &kerInitArgs);

            if (status_init == MMALIB_SUCCESS) {
               TI_profile_start (TI_PROFILE_KERNEL_INIT);
               MMALIB_asm (" MARK 0");
               kerInitArgs.funcStyle = MMALIB_FUNCTION_OPTIMIZED;
               status_init = MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_init (
                   handle, &src0_addr, &src1_addr, &src2_addr, &src3_addr,
                   &dst_addr, &kerInitArgs);
               MMALIB_asm (" MARK 1");
               TI_profile_stop ();
            }

            /* Test optimized kernel */
            status_opt =
                MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_exec_checkParams (
                    handle, src0, src1, src2, dst);
            if (status_opt == MMALIB_SUCCESS) {
               TI_profile_start (TI_PROFILE_KERNEL_OPT);
               MMALIB_asm (" MARK 2");
               status_opt = MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_exec (
                     handle, src0, src1, src2, src3, src4, dst);
               MMALIB_asm (" MARK 3");
               TI_profile_stop ();
            }
            /* run warm instruction cache test */
            TI_profile_start (TI_PROFILE_KERNEL_OPT_WARM);
            MMALIB_asm (" MARK 4");
            status_opt = MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_exec (
                  handle, src0, src1, src2, src3, src4, dst);
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
            status_opt = MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_exec (
                  handle, src0, src1, src2, src3, src4, dst);
            MMALIB_asm (" MARK 7");
            TI_profile_stop ();

            /* With RTL, we do not want to consume too many cycles. So, omit
             * running natural C version */
            /* Test _cn kernel */
            kerInitArgs.funcStyle = MMALIB_FUNCTION_NATC;

            MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_init (
                handle, &src0_addr, &src1_addr, &src2_addr, &src3_addr,
                &dst_addr, &kerInitArgs);

            TI_profile_start (TI_PROFILE_KERNEL_CN);
            MMALIB_asm (" MARK 8");
            status_nat = MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_exec (
                  handle, src0, src1, src2, src3, src4, dst_cn);
            MMALIB_asm (" MARK 9");
            TI_profile_stop ();

            /* Compare natural C Output and Optimized Output */
            status_nat_vs_opt = TI_compare_mem_3D (
                dst, dst_cn, 0, 0, currPrm.widthOut, currPrm.heightOut, currPrm.numChannels,
                currPrm.strideOut, currPrm.heightOut * currPrm.strideOut, MMALIB_sizeof (dst_addr.data_type));

            MMALIB_asm (" MARK 10");
            /* If static output is available, then additionally compares
             * optimized C output with static reference output data */
            if (currPrm.staticOut != NULL) {

#if MMALIB_DEBUGPRINT
               MMALIB_PRINTF("Checking ref vs opt%s", "\n");
#endif
               MMALIB_asm (" MARK 11");
               status_ref_vs_opt = TI_compare_mem_roi2DStatic (
                   (void *) dst, (void *) currPrm.staticOut, 0, 0,
                   currPrm.widthOut, currPrm.heightOut, currPrm.strideOut,
                   MMALIB_sizeof (dst_addr.data_type));
            }
            else {
               /* Set to pass since it wasn't supposed to run. */
               status_ref_vs_opt = TI_TEST_KERNEL_PASS;
            }
            MMALIB_asm (" MARK 12");
            /* Set the 'fail' flag based on test vector comparison results */
            currentTestFail =
                ((status_nat_vs_opt == TI_TEST_KERNEL_FAIL) ||
                 (status_ref_vs_opt == TI_TEST_KERNEL_FAIL) ||
                 (status_init != MMALIB_SUCCESS) ||
                 (status_opt != MMALIB_SUCCESS) ||
                 (status_nat != MMALIB_SUCCESS) || (currentTestFail == 1))
                    ? 1
                    : 0;

            fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;
            MMALIB_asm (" MARK 13");

#if MMALIB_DEBUGPRINT
            if(currentTestFail == 1){
               MMALIB_PRINTF("Failed%s", "\n");
               MMALIB_PRINTF("   status_ref_vs_opt: %d\n", status_ref_vs_opt);
               MMALIB_PRINTF("   status_nat_vs_opt: %d\n", status_nat_vs_opt);
               MMALIB_PRINTF("   status_init: %d\n", status_init);
               MMALIB_PRINTF("   status_opt: %d\n", status_opt);
               MMALIB_PRINTF("   status_nat: %d\n", status_nat);
            }
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
            if(currPrm.bTranspose == MMALIB_LINALG_MATRIXMATRIXMULTIPLYBIAS_IXX_IXX_OXX_B_TRANSPOSED)
            {
               sprintf(desc, "%s generated input | dataType=%d, m=%d, k=%d, n=%d",
                     testPatternString, dst_addr.data_type, currPrm.heightIn0, currPrm.widthIn0, currPrm.heightIn1);
            }
            else
            {
               sprintf(desc, "%s generated input | dataType=%d, m=%d, k=%d, n=%d",
                     testPatternString, dst_addr.data_type, currPrm.heightIn0, currPrm.widthIn0, currPrm.widthIn1);
            }

            MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_perfEst (
                handle, &src0_addr, &src1_addr, &dst_addr, &idealCycles,
                &archCycles, &estCycles, &caseNumber);

            MMALIB_asm (" MARK 18");
            TI_profile_add_test (testNum++, num_pts, archCycles, estCycles,
                                 currentTestFail, desc);
#if MMALIB_DEBUGPRINT
            if (currentTestFail) {
               MMALIB_PRINTF("\nA is: %s", "\n");
               MMALIB_debugPrintMatrix3D ((char *) src0, &src0_addr);
               MMALIB_PRINTF("B is: %s", "\n");
               MMALIB_debugPrintMatrix3D ((char *) src1, &src1_addr);
               if(src2){
                  MMALIB_PRINTF ("Bias is: %s", "\n");
                  MMALIB_debugPrintMatrix ((char *) src2, &src2_addr);
               }
               MMALIB_PRINTF ("Scale is: %s", "\n");
               MMALIB_debugPrintMatrix ((char *) src3, &src3_addr);
               src3_addr.data_type = MMALIB_UINT8;
               MMALIB_PRINTF ("Shift is: %s", "\n");
               MMALIB_debugPrintMatrix ((char *) src4, &src3_addr);

               if( currPrm.staticOut != NULL ) {
                  MMALIB_bufParams3D_t staticOut_addr = dst_addr;
                  staticOut_addr.stride_y = staticOut_addr.dim_x * MMALIB_sizeof(staticOut_addr.data_type);
                  staticOut_addr.stride_z = staticOut_addr.dim_y * staticOut_addr.stride_y;
                  MMALIB_PRINTF("The reference output is %s", "\n");
                  MMALIB_debugPrintBufParams3D(&staticOut_addr);
                  MMALIB_debugPrintMatrix3D((void *)currPrm.staticOut, &staticOut_addr);
               }


               MMALIB_PRINTF ("\nThe natural c output is: %s", "\n");
               MMALIB_debugPrintMatrix3D ((char *) dst_cn, &dst_addr);
               MMALIB_PRINTF ("The optimized output is: %s", "\n");
               MMALIB_debugPrintMatrix3D ((char *) dst, &dst_addr);
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
         TI_align_free (src2);
         TI_align_free (src3);
         TI_align_free (src4);
         MMALIB_asm (" MARK 22");
         if (currPrm.outputDataLocation == MMALIB_TEST_OUTPUT_HEAP){
            // only malloc'd data can be freed
            TI_align_free(dst);
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
      TI_memError ("MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX");
      return 1;
   }
   else
#else
   printf ("_HOST_BUILD is defined.\n");
#endif //_HOST_BUILD
   {
      return MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_d (&pProfile[0], 0);
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
/*  End of file:  MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_d.c */
/* ======================================================================== */
