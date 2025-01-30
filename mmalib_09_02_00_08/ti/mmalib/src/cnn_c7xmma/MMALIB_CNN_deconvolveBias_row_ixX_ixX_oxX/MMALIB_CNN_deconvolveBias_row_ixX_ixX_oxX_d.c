/******************************************************************************
 *                                                                             *
 * module name       :MMALIB                                                   *
 *                                                                             *
 * module descripton :Matrix Multiply Accelerator Library module for C7x+MMA   *
 *                                                                             *
 * Copyright (C) 2017-2018 Texas Instruments Incorporated - http://www.ti.com/ *
 * ALL RIGHTS RESERVED                                                         *
 *                                                                             *
 ******************************************************************************/

/**
******************************************************************************
*  @file     MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_d.c
*
*  @brief    File to demnostrate usage of kernel
*
*
*******************************************************************************
*/

// include MMALIB
#include <mmalib.h>

// include test infrastructure provided by MMALIB
#include "../../test/MMALIB_test.h"

// include test data for this kernel
#include "MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_idat.h"

// use volatile to keep compiler from removing this operation
int16_t volatile volatileSum = 0;

__attribute__ ((section (".msmcData"), aligned (64)))
int8_t msmcBuffer[MMALIB_L3_RESULTS_BUFFER_SIZE];
__attribute__ ((section (".ddrData"), aligned (64)))
int8_t ddrBuffer[4 * 1024 * 1024];

void MMALIB_debugPrintMatrix_int16 (int16_t                    *matrix,
                                    const MMALIB_bufParams2D_t *params)
{
   uint32_t x, y;
   int16_t *xPtr;
   int16_t *yPtr = matrix;

   // stride_y is stored in bytes, but easier to use in elements
   uint32_t stride_y_elements =
       params->stride_y / MMALIB_sizeof (params->data_type);

   for (y = 0; y < params->dim_y; y++) {
      xPtr = yPtr;
      printf ("%p |", xPtr);
      for (x = 0; x < params->dim_x; x++) {
         printf ("%3d ", *(xPtr));
         xPtr++;
      }
      printf ("%s", "|\n");
      yPtr += stride_y_elements;
   }

   return;
}

/* MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_d:  Test Driver Routine */
int MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_d (uint32_t *pProfile,
                                                 uint8_t   LevelOfFeedback)
{

   int32_t  currentTestFail;               // status of current test case
   int32_t  tpi        = 0;                // test parameter index
   int32_t  fail       = 0;                // fail flag
   uint64_t archCycles = 0, estCycles = 0; // performance estimates
   uint32_t num_pts = 0;

   /* Test Parameters */

   MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_testParams_t *prm = NULL;

   MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_getTestParams (&prm, &test_cases);

   MMALIB_kernelHandle handle     = NULL;
   int32_t             handleSize = 0;

   MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_InitArgs    kerInitArgs;
   MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_ExecInArgs  kerExecInArgs;
   MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_ExecOutArgs kerExecOutArgs;

   /* Initialize profiling */

   TI_profile_init ("deconvolveBias_row_ixX_ixX_oxX");

   /* Run each test case */

   for (tpi = 0; tpi < test_cases; tpi++) {

      /* Initialize status flags */

      int32_t status_nat_vs_opt =
          TI_TEST_KERNEL_PASS; /* Test status : Natural c vs. Optimized */
      int32_t status_opt_vs_ref =
          TI_TEST_KERNEL_PASS; /* Test status : Natural c vs. Static Reference
                                */
      /* int32_t status_nat_vs_ref = MMALIB_SUCCESS; */
      MMALIB_STATUS currTestStatus = MMALIB_STATUS_NOT_SET;
      currentTestFail = 0;

      /* Setup test case parameters */

      uint32_t inp0Size = 0, inp1Size = 0, biasSize = 0, outSize = 0;

      handleSize = MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_getHandleSize (
          &kerInitArgs);
      handle = malloc (handleSize);

      int32_t inWidth = prm[tpi].inWidth;
      /* int32_t inputBlockWidth     = inWidth + prm[tpi].pad; */
      int32_t inChOffset          = prm[tpi].inChOffset;
      int32_t numInChannels       = prm[tpi].numInChannels;
      int32_t numOutChannels      = prm[tpi].numOutChannels;
      int32_t expectedStatusCode  = prm[tpi].expectedStatusCode;
      int32_t numGroups           = prm[tpi].numGroups;
      int32_t subMChannels        = prm[tpi].subMChannels;
      int32_t kernelWidth         = prm[tpi].kernelWidth;
      int32_t kernelHeight        = prm[tpi].kernelHeight;
      int32_t dilationWidth       = prm[tpi].dilationWidth;
      int32_t dilationHeight      = prm[tpi].dilationHeight;
      int32_t validColsIn         = prm[tpi].validColsIn;
      int32_t strideWidth         = prm[tpi].strideWidth;
      int32_t strideHeight        = prm[tpi].strideHeight;
      int32_t numOfOutputChKerBuf = 0;

      // FIXME: Need better interface instead of having to change in driver file
      if (strideWidth == 2 && strideHeight == 2 && kernelWidth == 4 &&
          kernelHeight == 4) {
         numOfOutputChKerBuf = (prm[tpi].numOfOutputChKerBuf << 2);
      }
      else if (strideWidth == 2 && strideHeight == 2 && kernelWidth == 2 &&
               kernelHeight == 2) {

         numOfOutputChKerBuf = (prm[tpi].numOfOutputChKerBuf << 2);
      }
      else if (strideWidth == 2 && strideHeight == 2 && kernelWidth == 8 &&
               kernelHeight == 8) {
         numOfOutputChKerBuf = (prm[tpi].numOfOutputChKerBuf << 2);
      }
      else {
         numOfOutputChKerBuf = prm[tpi].numOfOutputChKerBuf;
      }

      int32_t kDim           = prm[tpi].kDim;
      int32_t pitchA         = prm[tpi].pitchA;
      int32_t pitchC         = prm[tpi].pitchC;
      uint8_t dataTypeA      = prm[tpi].dataTypeA;
      uint8_t dataTypeB      = prm[tpi].dataTypeB;
      uint8_t dataTypeC      = prm[tpi].dataTypeC;
      uint8_t dataTypeBias   = prm[tpi].dataTypeBias;
      uint8_t dataTypeS      = prm[tpi].dataTypeS;
      uint8_t activationType = prm[tpi].activationType;
      int32_t pad            = prm[tpi].pad;
      uint8_t mode           = prm[tpi].mode;
      /* int32_t circularOffset    = prm[tpi].circularOffset; */
      int32_t maxHeight         = prm[tpi].maxHeight;
      int32_t totalN            = prm[tpi].totalN;
      int32_t subN              = prm[tpi].subN;
      int32_t validColsPerRowIn = prm[tpi].validColsPerRowIn;
      int32_t validRowsIn       = prm[tpi].validRowsIn;
      int32_t outputPitchPerRow = prm[tpi].outputPitchPerRow;
      int32_t inputPitchPerRow  = prm[tpi].inputPitchPerRow;
      int32_t testNum           = prm[tpi].testID;
      bool    preProcessFlag    = prm[tpi].preProcessFlag;
      /* int32_t circularOffset          = prm[tpi].circularOffset; */

      int32_t numBytes = 0;
      int32_t MMA_SIZE = 0;

      if (dataTypeC == MMALIB_INT8 || dataTypeC == MMALIB_UINT8) {
         numBytes = 1;
         MMA_SIZE = MMALIB_MMA_SIZE_8_BIT;
      }
      else if (dataTypeC == MMALIB_INT16 || dataTypeC == MMALIB_UINT16) {
         numBytes = 2;
         MMA_SIZE = MMALIB_MMA_SIZE_16_BIT;
      }
      else {
         // TODO
      }

      int32_t validColsOut = 0;
      /* int32_t validColsOutLast = 0; */

      int32_t validColsPerRow = 0;
      /* Compute buffer sizes */

      /**************************************************************/
      /* Handle scale and shift values based on quantization method */
      /**************************************************************/

      MMALIB_quantMethod_type quantMethod = prm[tpi].quantMethod;
      uint32_t                scaleSize   = 0;
      uint32_t                shiftSize   = 0;

      // Includes both shift and scale values and hence twice the size
      if (quantMethod == MMALIB_QUANT_PER_CHANNEL) {
         scaleSize = numOutChannels * numGroups << 2;
         if (numOutChannels % MMA_SIZE != 0) {
            scaleSize += (numOutChannels % MMA_SIZE) * numBytes;
         }
         shiftSize = numOutChannels * numGroups << 2;
         if (numOutChannels % MMA_SIZE != 0) {
            shiftSize += (numOutChannels % MMA_SIZE) * numBytes;
         }
      }
      else {
         scaleSize = 1;
         shiftSize = 1;
      }

#if MMALIB_DEBUGPRINT
      printf ("pitchA %d, inChOffset %d, pitchC %d, inp0Size %d, inp1Size %d,"
              "outSize %d\n",
              pitchA, inChOffset, pitchC, inp0Size, inp1Size, outSize);
      printf ("numOfOutputChKerBuf %d, numInChannels %d, numOutChannels %d, "
              "totalN %d,"
              "subN %d\n",
              numOfOutputChKerBuf, numInChannels, numOutChannels, totalN, subN);
#endif

      // coefficient matrix
      inp0Size = (numOfOutputChKerBuf * pitchA) * numGroups;
      inp1Size = numInChannels * (inChOffset) *numBytes * numGroups;
      biasSize = numOfOutputChKerBuf * numGroups;
      outSize  = numOutChannels * pitchC * (totalN / subN) * numGroups;

      if (numOfOutputChKerBuf % MMA_SIZE != 0) {
         biasSize += (numOutChannels % MMA_SIZE);
      }
      /* Allocate buffers for each test vector */
      /* Setup data buffers */

      int8_t *src0 = (int8_t *) TI_memalign (numBytes * MMA_SIZE, inp0Size);
      int8_t *src1 = (int8_t *) TI_memalign (inChOffset * numBytes, inp1Size);
      int8_t *src2 = (int8_t *) TI_memalign (MMA_SIZE * numBytes * 4,
                                             biasSize * numBytes * 4);
      void   *src3 = TI_memalign (MMALIB_L2DATA_ALIGNMENT, scaleSize);
      void   *src4 = TI_memalign (MMALIB_L2DATA_ALIGNMENT, shiftSize);
      int8_t *dst  = NULL;

      if (prm[tpi].outputDataLocation == 0) {
         dst = (int8_t *) TI_memalign (numBytes * MMA_SIZE, outSize);
      }
      else if (prm[tpi].outputDataLocation == 1) {
         dst = (int8_t *) msmcBuffer;
      }
#if !defined(PERFORMANCE_TEST) && !defined(COMPILER_TEST)
      /* int8_t *dst_cn = (int8_t *) malloc (outSize); */
      int8_t *dst_cn = (int8_t *) ddrBuffer;
#endif

      /* int8_t *dst_deconv = (int8_t *) TI_memalign (numBytes * MMA_SIZE,
       * outSize); */

      int32_t MCount = 0;

#if MMALIB_DEBUGPRINT
      printf ("MMALIB_DEBUGPRINT test_case %d  src1: %p  src0 %p dst %p \n",
              tpi, src1, src0, dst);
#if !defined(PERFORMANCE_TEST) && !defined(COMPILER_TEST)
      printf ("MMALIB_DEBUGPRINT test_case %d  dst_cn %p\n", tpi, dst_cn);
#endif
#endif
      /* Only run the test if the buffer allocations fit in the heap */
      if (src0 && src1 && src2 && src3 && src4 && dst
          && dst_cn
      ) {
         MMALIB_bufParams2D_t src0_addr; // paramsWgt
         MMALIB_bufParams2D_t src1_addr; // input
         MMALIB_bufParams1D_t src2_addr; // bias
         MMALIB_bufParams1D_t src3_addr; // scale
         MMALIB_bufParams2D_t dst_addr;  // output after deinterleaving columns

         /* Fill input arrays according to desired test pattern */

         if (numBytes == 1) { // 8-bit test cases
            if ((preProcessFlag && prm[tpi].staticKernel8Bit != NULL) &&
                ((kernelHeight == 4 && kernelWidth == 4) &&
                 (strideHeight == 2 && strideWidth == 2))) {
               MMALIB_CNN_deconvolveBias_row_4x4Stride2PreProcessParameters (
                   numInChannels, pitchA,
                   numOutChannels, // before pre-processing
                   numGroups, MMALIB_MMA_SIZE_8_BIT, prm[tpi].staticKernel8Bit,
                   src0);

               MMALIB_CNN_deconvolveBias_row_4x4Stride2PreProcessBias (
                   prm[tpi].staticBias32Bit, src2, numOutChannels, numGroups,
                   MMALIB_MMA_SIZE_8_BIT);

               if (quantMethod == MMALIB_QUANT_PER_CHANNEL) {
                  MMALIB_CNN_deconvolveBias_row_4x4Stride2PreProcessScaleAndShift (
                      prm[tpi].staticScale8Bit, prm[tpi].staticShift8Bit, src3,
                      src4, numOutChannels, numGroups);
               }
            }
            else if ((preProcessFlag && prm[tpi].staticKernel8Bit != NULL) &&
                     ((kernelHeight == 2 && kernelWidth == 2) &&
                      (strideHeight == 2 && strideWidth == 2))) {
               MMALIB_CNN_deconvolveBias_row_2x2Stride2PreProcessParameters (
                   kDim, numInChannels, pitchA,
                   numOutChannels, // before pre-processing
                   numGroups, MMALIB_MMA_SIZE_8_BIT, prm[tpi].staticKernel8Bit,
                   src0);
            }
            else if ((preProcessFlag && prm[tpi].staticKernel8Bit != NULL) &&
                     ((kernelHeight == 8 && kernelWidth == 8) &&
                      (strideHeight == 2 && strideWidth == 2))) {
               MMALIB_CNN_deconvolveBias_row_8x8Stride2PreProcessParameters (
                   kDim, numInChannels, pitchA,
                   numOutChannels, // before pre-processing
                   numGroups, MMALIB_MMA_SIZE_8_BIT, prm[tpi].staticKernel8Bit,
                   src0);
            }
            else {
               TI_fillBuffer (prm[tpi].testPattern, (uint8_t) 255, src0,
                              prm[tpi].staticKernel8Bit, kDim,
                              numOfOutputChKerBuf *
                                  numGroups, // expected with pre-processing
                              pitchA, sizeof (int8_t), testPatternString);

               // Fill Bias buffer
               TI_fillBuffer (prm[tpi].testPattern, (uint8_t) 255, src2,
                              prm[tpi].staticBias32Bit, biasSize, 1, biasSize,
                              sizeof (int32_t), testPatternString);
            }

#if GENERATE_STATIC_DATA
            MMALIB_bufParams3D_t kernelPrint;
            kernelPrint.data_type = MMALIB_INT8;
            kernelPrint.dim_x =
                kDim; // prm[tpi].kernelWidth * prm[tpi].kernelHeight;
            kernelPrint.dim_y = numOfOutputChKerBuf; // prm[tpi].numInChannels;
            kernelPrint.dim_z = 1;
            kernelPrint.stride_y = pitchA;
            kernelPrint.stride_z = 0; // kernelMatrixCols;
            printf ("static int8_t staticRefKernel[] =\n");
            MMALIB_debugPrint3DVReg (src0, &kernelPrint);
#endif

            // Fill Input buffer
            TI_fillBuffer (prm[tpi].testPattern, (uint8_t) 255, src1,
                           prm[tpi].staticIn8Bit, inChOffset,
                           numInChannels * numGroups, inChOffset,
                           sizeof (int8_t), testPatternString);

#if GENERATE_STATIC_DATA
            MMALIB_bufParams3D_t inputPrint;
            inputPrint.data_type = MMALIB_INT8;
            inputPrint.dim_x     = inChOffset;
            inputPrint.dim_y     = numInChannels;
            inputPrint.dim_z     = 1;
            inputPrint.stride_y  = inChOffset;
            inputPrint.stride_z  = 0;
            printf ("static int8_t staticRefIn[] =\n");
            MMALIB_debugPrint3DVReg (src1, &inputPrint);
#endif
         }
         else if (numBytes == 2) { // 16-bit test cases
            if ((preProcessFlag && prm[tpi].staticKernel16Bit != NULL) &&
                ((kernelHeight == 4 && kernelWidth == 4) &&
                 (strideHeight == 2 && strideWidth == 2))) {
               MMALIB_CNN_deconvolveBias_row_4x4Stride2PreProcessParameters (
                   numInChannels, pitchA,
                   numOutChannels, // before pre-processing
                   numGroups, MMALIB_MMA_SIZE_16_BIT,
                   prm[tpi].staticKernel16Bit, src0);

               MMALIB_CNN_deconvolveBias_row_4x4Stride2PreProcessBias (
                   prm[tpi].staticBias64Bit, src2, numOutChannels, numGroups,
                   MMALIB_MMA_SIZE_16_BIT);
            }
            else if ((preProcessFlag && prm[tpi].staticKernel16Bit != NULL) &&
                     ((kernelHeight == 2 && kernelWidth == 2) &&
                      (strideHeight == 2 && strideWidth == 2))) {
               MMALIB_CNN_deconvolveBias_row_2x2Stride2PreProcessParameters (
                   kDim, numInChannels, pitchA,
                   numOutChannels, // before pre-processing
                   numGroups, MMALIB_MMA_SIZE_16_BIT,
                   prm[tpi].staticKernel16Bit, src0);
            }

            else if ((preProcessFlag && prm[tpi].staticKernel16Bit != NULL) &&
                     ((kernelHeight == 8 && kernelWidth == 8) &&
                      (strideHeight == 2 && strideWidth == 2))) {
               MMALIB_CNN_deconvolveBias_row_8x8Stride2PreProcessParameters (
                   kDim, numInChannels, pitchA,
                   numOutChannels, // before pre-processing,
                   numGroups, MMALIB_MMA_SIZE_16_BIT,
                   prm[tpi].staticKernel16Bit, src0);
            }

            else {
               TI_fillBuffer (prm[tpi].testPattern, (uint8_t) 255, src0,
                              prm[tpi].staticKernel16Bit, kDim,
                              numOfOutputChKerBuf *
                                  numGroups, // expected with pre-processing
                              pitchA, sizeof (int16_t), testPatternString);

               // Fill Bias buffer
               TI_fillBuffer (prm[tpi].testPattern, (uint8_t) 255, src2,
                              prm[tpi].staticBias64Bit, biasSize, 1, biasSize,
                              sizeof (int64_t), testPatternString);
            }

#if GENERATE_STATIC_DATA
            MMALIB_bufParams3D_t kernelPrint;
            kernelPrint.data_type = MMALIB_INT16;
            kernelPrint.dim_x =
                kDim; // prm[tpi].kernelWidth * prm[tpi].kernelHeight;
            kernelPrint.dim_y = numOfOutputChKerBuf; // prm[tpi].numInChannels;
            kernelPrint.dim_z = 1;
            kernelPrint.stride_y = pitchA;
            kernelPrint.stride_z = 0; // kernelMatrixCols;
            printf ("static int8_t staticRefKernel[] =\n");
            MMALIB_debugPrint3DVReg (src0, &kernelPrint);
#endif
            // Fill Input buffer
            TI_fillBuffer (prm[tpi].testPattern, (uint8_t) 255, src1,
                           prm[tpi].staticIn16Bit, inChOffset,
                           numInChannels * numGroups, inChOffset * numBytes,
                           sizeof (int16_t), testPatternString);

#if GENERATE_STATIC_DATA
            MMALIB_bufParams3D_t inputPrint;
            inputPrint.data_type = MMALIB_INT16;
            inputPrint.dim_x     = inChOffset;
            inputPrint.dim_y     = numInChannels;
            inputPrint.dim_z     = 1;
            inputPrint.stride_y  = inChOffset * numBytes;
            inputPrint.stride_z  = 0;
            printf ("static int8_t staticRefIn[] =\n");
            MMALIB_debugPrint3DVReg (src1, &inputPrint);
#endif
         }

         if (!preProcessFlag || quantMethod == MMALIB_QUANT_PER_KERNEL) {
            // scale and shift Buffer
            TI_fillBuffer (prm[tpi].testPattern, (uint8_t) 255, src3,
                           prm[tpi].staticScale8Bit, scaleSize, 1, scaleSize,
                           sizeof (uint8_t), testPatternString);

            TI_fillBuffer (prm[tpi].testPattern, (uint8_t) 255, src4,
                           prm[tpi].staticShift8Bit, shiftSize, 1, shiftSize,
                           sizeof (uint8_t), testPatternString);
         }

         /* Allocate problem dimensions to buffer */

         src0_addr.dim_x     = kDim;
         src0_addr.dim_y     = numOfOutputChKerBuf * numGroups;
         src0_addr.stride_y  = pitchA;
         src0_addr.data_type = dataTypeA;

         src1_addr.dim_x     = inChOffset;
         src1_addr.dim_y     = numInChannels * numGroups;
         src1_addr.stride_y  = inChOffset;
         src1_addr.data_type = dataTypeB;

         // allocate Bias per output channel
         src2_addr.dim_x = numOfOutputChKerBuf;
         if (numBytes == 1) {
            src2_addr.data_type = MMALIB_INT32;
         }
         else if (numBytes == 2) {
            src2_addr.data_type = MMALIB_INT64;
         }

         // allocate  scale per output channel
         src3_addr.dim_x     = numOutChannels;
         src3_addr.data_type = dataTypeS;

         dst_addr.dim_x     = (pitchC);
         dst_addr.dim_y     = (numOutChannels * numGroups);
         dst_addr.stride_y  = (pitchC);
         dst_addr.data_type = dataTypeC;

         /* Assign to Init Args and Interface file for deconvolution */

         kerInitArgs.Fc                = kernelWidth;
         kerInitArgs.Fr                = kernelHeight;
         kerInitArgs.dilationX         = dilationWidth;
         kerInitArgs.dilationY         = dilationHeight;
         kerInitArgs.strideX           = strideWidth;
         kerInitArgs.strideY           = strideHeight;
         kerInitArgs.validColsIn       = validColsIn;
         kerInitArgs.subMChannels      = subMChannels;
         kerInitArgs.inWidth           = inWidth;
         kerInitArgs.pad               = pad;
         kerInitArgs.maxHeight         = maxHeight;
         kerInitArgs.inChOffset        = inChOffset;
         kerInitArgs.No                = numOutChannels;
         kerInitArgs.numGroups         = numGroups;
         kerInitArgs.activationType    = activationType;
         kerInitArgs.mode              = mode;
         kerInitArgs.validColsPerRowIn = validColsPerRowIn;
         kerInitArgs.validRowsIn       = validRowsIn;
         kerInitArgs.outputPitchPerRow = outputPitchPerRow;
         kerInitArgs.inputPitchPerRow  = inputPitchPerRow;

         kerExecInArgs.quantMethod = quantMethod;

         /* This for creating the predicate buffers */
         int32_t totalBytes = 0;

#if MMALIB_DEBUGPRINT
         printf ("\nAfter MMALIB_CNN_generateFillSeamPredicateRegisters"
                 "totalBytes %d\n",
                 totalBytes);
#endif
         /* Initialize kernel */
         kerInitArgs.funcStyle = MMALIB_FUNCTION_OPTIMIZED;
         /* kerInitArgs.funcStyle = MMALIB_FUNCTION_NATC; */
         TI_profile_start (TI_PROFILE_KERNEL_INIT);
         MMALIB_asm (" MARK 0");
         currTestStatus = MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_init (
             handle, &src0_addr, &src1_addr, &src2_addr, &src3_addr, &dst_addr,
             &kerInitArgs);
#if MMALIB_DEBUGPRINT
         printf ("\nAfter init status_init: %d\n",currTestStatus);
#endif

         MMALIB_asm (" MARK 1");
         TI_profile_stop ();

         /* Test optimized kernel */

#if MMALIB_DEBUGPRINT
         printf ("\nEntering:MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX");
#endif
         int32_t iterN           = 0;
         int32_t validOutputRows = 0;
         int32_t NCount          = 0;
      if ( currTestStatus == MMALIB_SUCCESS) 
      {   
         for (NCount = 0; NCount < totalN; NCount += subN) {
            int8_t *src1_Iter = src1;
            for (MCount = 0; MCount < numOutChannels; MCount += subMChannels) {
               kerExecInArgs.subMChannels = subMChannels;
               if ((totalN / subN) != 1 &&
                   (NCount == totalN - subN)) { // last iteration
                  // poor-man's way of simulating last call
                  /* kerExecInArgs.validColsIn = validColsIn - 10; */
                  kerExecInArgs.validColsIn = validColsIn;
               }
               else {
                  kerExecInArgs.validColsIn = validColsIn;
               }

               int8_t *dst_Iter = dst + MCount * (pitchC) +
                                  (numOutChannels * iterN * (pitchC));

               int8_t *src0_Iter = src0;

               TI_profile_start (TI_PROFILE_KERNEL_OPT);
               MMALIB_asm (" MARK 2");
               currTestStatus = MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_exec (
                   handle, src0_Iter, src1_Iter, src2, src3, src4, dst_Iter,
                   &kerExecInArgs, &kerExecOutArgs);
               MMALIB_asm (" MARK 3");
               TI_profile_stop ();

               /* printf ("dst_iter matrix below\n"); */
               /* MMALIB_debugPrintMatrix_int16 ((int16_t *) dst_Iter,
                * &dst_addr); */

               if ((totalN / subN) != 1 &&
                   (NCount == totalN - subN)) { // last iteration
                  /* validColsOutLast = kerExecOutArgs.validColsOut; */
               }
               else {
                  validColsOut = kerExecOutArgs.validColsOut;
               }
            }
            iterN++;
         }
         __SE0_CLOSE ();
         __SE1_CLOSE ();
      }
      else 
      {
         MMALIB_PRINTF ("Opt. code init error: %d\n", currTestStatus);
      }
         // run warm instruction cache test
      if(currTestStatus == MMALIB_SUCCESS)
      {
         currTestStatus = MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_init (
             handle, &src0_addr, &src1_addr, &src2_addr, &src3_addr, &dst_addr,
             &kerInitArgs);

         iterN           = 0;
         validOutputRows = 0;
         NCount          = 0;
         if(currTestStatus == MMALIB_SUCCESS)
         {
            for (NCount = 0; NCount < totalN; NCount += subN) 
            {
               int8_t *src1_Iter = src1;
               for (MCount = 0; MCount < numOutChannels; MCount += subMChannels) {
                  kerExecInArgs.subMChannels = subMChannels;
                  if ((totalN / subN) != 1 &&
                     (NCount == totalN - subN)) { // last iteration
                     // poor-man's way of simulating last call
                     /* kerExecInArgs.validColsIn = validColsIn - 10; */
                     kerExecInArgs.validColsIn = validColsIn;
                  }
                  else {
                     kerExecInArgs.validColsIn = validColsIn;
                  }

               int8_t *dst_Iter = dst + MCount * (pitchC) +
                                  (numOutChannels * iterN * (pitchC));
               int8_t *src0_Iter = src0;

                  TI_profile_start (TI_PROFILE_KERNEL_OPT_WARM);
                  MMALIB_asm (" MARK 4");
                  currTestStatus= MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_exec (
                     handle, src0_Iter, src1_Iter, src2, src3, src4, dst_Iter,
                     &kerExecInArgs, &kerExecOutArgs);

               MMALIB_asm (" MARK 5");
               TI_profile_stop ();
               if ((totalN / subN) != 1 && (NCount == totalN - subN)) {
                  // last iteration
                  // TBD
                  /* validColsOutLast = kerExecOutArgs.validColsOut; */
               }
               else {
                  validColsOut = kerExecOutArgs.validColsOut;
               }
            }
            iterN++;
         }

         __SE0_CLOSE ();
         __SE1_CLOSE ();

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

            // dummy store of outSum to insure that the compiler does not remove
            // it.
            volatileSum = outSum;
         }
         else 
         {
            MMALIB_PRINTF("MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX__init_checkParams failed!%s", "\n");
         }
         currTestStatus = MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_init (
             handle, &src0_addr, &src1_addr, &src2_addr, &src3_addr, &dst_addr,
             &kerInitArgs);

         iterN           = 0;
         validOutputRows = 0;
         NCount          = 0;

         // run warm instruction cache test
         if(currTestStatus == MMALIB_SUCCESS) 
         {
            for (NCount = 0; NCount < totalN; NCount += subN) {
               int8_t *src1_Iter = src1;
               for (MCount = 0; MCount < numOutChannels; MCount += subMChannels) {
                  kerExecInArgs.subMChannels = subMChannels;
                  if ((totalN / subN) != 1 &&
                     (NCount == totalN - subN)) { // last iteration
                     // poor-man's way of simulating last call
                     /* kerExecInArgs.validColsIn = validColsIn - 10; */
                     kerExecInArgs.validColsIn = validColsIn;
                  }
                  else {
                     kerExecInArgs.validColsIn = validColsIn;
                  }
                  int8_t *dst_Iter = dst + MCount * (pitchC) +
                                    (numOutChannels * iterN * (pitchC));

               int8_t *src0_Iter = src0;

                  TI_profile_start (TI_PROFILE_KERNEL_OPT_WARMWRB);
                  MMALIB_asm (" MARK 6");
                  currTestStatus = MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_exec (
                     handle, src0_Iter, src1_Iter, src2, src3, src4, dst_Iter,
                     &kerExecInArgs, &kerExecOutArgs);

               MMALIB_asm (" MARK 7");
               TI_profile_stop ();
               if ((totalN / subN) != 1 && (NCount == totalN - subN)) {
                  // last iteration
                  /* validColsOutLast = kerExecOutArgs.validColsOut; */
               }
               else {
                  validColsOut = kerExecOutArgs.validColsOut;
               }
            }
            iterN++;
         }

         __SE0_CLOSE ();
         __SE1_CLOSE ();
         }
         else 
         {
            MMALIB_PRINTF("MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_init_checkParams failed!%s", "\n");
         }
      }

#if !defined(PERFORMANCE_TEST) && !defined(COMPILER_TEST) && !defined(MMALIB_SKIP_CN)

         /* Test _cn kernel */
         kerInitArgs.funcStyle = MMALIB_FUNCTION_NATC;
         currTestStatus = MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_init (
             handle, &src0_addr, &src1_addr, &src2_addr, &src3_addr, &dst_addr,
             &kerInitArgs);

         iterN           = 0;
         validOutputRows = 0;
         if(currTestStatus == MMALIB_SUCCESS)
         {
            for (NCount = 0; NCount < totalN; NCount += subN) 
            {
               int8_t *src1_Iter = src1;

            for (MCount = 0; MCount < numOutChannels; MCount += subMChannels) {
               kerExecInArgs.subMChannels = subMChannels;
               if ((totalN / subN) != 1 &&
                   (NCount == totalN - subN)) { // last iteration
                  // poor-man's way of simulating last call
                  /* kerExecInArgs.validColsIn = validColsIn - 10; */
                  kerExecInArgs.validColsIn = validColsIn;
               }
               else {
                  kerExecInArgs.validColsIn = validColsIn;
               }

               int8_t *dst_cn_Iter = dst_cn + MCount * (pitchC) +
                                     (iterN * numOutChannels * pitchC);
               int8_t *src0_Iter = src0;

               TI_profile_start (TI_PROFILE_KERNEL_CN);

                  MMALIB_asm (" MARK 12");
                  currTestStatus = MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_exec (
                     handle, src0_Iter, src1_Iter, src2, src3, src4, dst_cn_Iter,
                     &kerExecInArgs, &kerExecOutArgs);
                  MMALIB_asm (" MARK 13");
                  TI_profile_stop ();

               /* printf ("dst_cn_iter matrix below\n"); */
               /* MMALIB_debugPrintMatrix_int8 ((int8_t *) dst_cn_Iter,
                * &dst_addr); */

                  if ((totalN / subN) != 1 &&
                     (NCount == totalN - subN)) { // last iteration
                     /* validColsOutLast = kerExecOutArgs.validColsOut; */
                  }
                  else {
                     validColsOut = kerExecOutArgs.validColsOut;
                  }
               }
               iterN++;
            }
         }
         else 
         {
            MMALIB_PRINTF("MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_init failed with status = %d!\n", currTestStatus);
         }

         /* Compare natural C Output and Optimized Output */
      if (currTestStatus == MMALIB_SUCCESS)
      {
         if ((strideWidth == 2 && strideHeight == 2 && kernelWidth == 4 &&
              kernelHeight == 4) ||
             (strideWidth == 2 && strideHeight == 2 && kernelWidth == 2 &&
              kernelHeight == 2) ||
             (strideWidth == 2 && strideHeight == 2 && kernelWidth == 8 &&
              kernelHeight == 8)) {
            uint32_t validColsOutDeconv = 0;
            validColsOutDeconv          = validColsOut << 1;

            status_nat_vs_opt = TI_compare_mem_2D (
                (void *) dst, (void *) dst_cn, 0, 0, validColsOutDeconv,
                (numOutChannels << 1) * (iterN) * (numGroups),
                /* (numOutChannels << 1), */
                (pitchC >> 1), numBytes);
         }
      } 
      else 
      {
         status_nat_vs_opt = TI_TEST_KERNEL_PASS;
      }
#else
         /* Set to pass since it wasn't supposed to run. */
         status_nat_vs_opt = TI_TEST_KERNEL_PASS;
#endif //#if !defined(PERFORMANCE_TEST) && !defined(COMPILER_TEST)
         if (numBytes == 1) {
            if (prm[tpi].staticOut8Bit != NULL && currTestStatus == MMALIB_SUCCESS) 
            {

#if MMALIB_DEBUGPRINT
               printf ("dst %p prm[tpi].staticOut8Bit %p validColsOut %d"
                       "numOutChannels %d pitchC %d\n",
                       dst, prm[tpi].staticOut8Bit, validColsOut,
                       numOutChannels, pitchC);
#endif
               if ((strideWidth == 2 && strideHeight == 2 && kernelWidth == 4 &&
                    kernelHeight == 4) ||
                   (strideWidth == 2 && strideHeight == 2 && kernelWidth == 2 &&
                    kernelHeight == 2) ||
                   (strideWidth == 2 && strideHeight == 2 && kernelWidth == 8 &&
                    kernelHeight == 8)) {
                  uint32_t validColsOutDeconv = 0;
                  validColsOutDeconv          = validColsOut << 1;
#if MMALIB_DEBUGPRINT
                  printf ("validOutputRows %d ValidColsOutDeconv %d \n",
                          validOutputRows, validColsOutDeconv);
#endif
                  status_opt_vs_ref = TI_compare_mem_roi2DStatic (
                      (void *) dst, (void *) prm[tpi].staticOutMMALIB8Bit, 0, 0,
                      (validColsOutDeconv),
                      (numOutChannels << 1) * iterN * numGroups, (pitchC >> 1),
                      numBytes);
               }

#if MMALIB_DEBUGPRINT
               printf ("reaching comparison for host\n");
#endif
            }
            else {
               status_opt_vs_ref = TI_TEST_KERNEL_PASS;
            }
         }
         else if (numBytes == 2) {
            if (prm[tpi].staticOut16Bit != NULL && currTestStatus == MMALIB_SUCCESS) {
               if ((strideWidth == 2 && strideHeight == 2 && kernelWidth == 4 &&
                    kernelHeight == 4) ||
                   (strideWidth == 2 && strideHeight == 2 && kernelWidth == 2 &&
                    kernelHeight == 2) ||
                   (strideWidth == 2 && strideHeight == 2 && kernelWidth == 8 &&
                    kernelHeight == 8)) {
                  uint32_t validColsOutDeconv = 0;
                  validColsOutDeconv          = validColsOut << 1;

#if MMALIB_DEBUGPRINT
                  printf ("validOutputRows %d ValidColsOutDeconv %d \n",
                          validOutputRows, validColsOutDeconv);
#endif

                  status_opt_vs_ref = TI_compare_mem_roi2DStatic (
                      (void *) dst, (void *) prm[tpi].staticOutMMALIB16Bit, 0,
                      0, (validColsOutDeconv),
                      /* numOutChannels*iterN, */
                      (numOutChannels << 1) * iterN * numGroups, (pitchC >> 1),
                      numBytes);
               }

#if MMALIB_DEBUGPRINT
               printf ("\nreaching comparison for host\n");
#endif
            }
            else {
               status_opt_vs_ref = TI_TEST_KERNEL_PASS;
               /* status_nat_vs_ref = TI_TEST_KERNEL_PASS; */
            }
         }

         /* Set the 'fail' flag based on test vector comparison results */
         currentTestFail =
             ((status_nat_vs_opt == TI_TEST_KERNEL_FAIL) ||
              (status_opt_vs_ref == TI_TEST_KERNEL_FAIL) ||
              (currTestStatus != prm[tpi].expectedStatusCode)) ? 1: 0;

         fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;

#if MMALIB_DEBUGPRINT
         printf ("%s: status_nat_vs_opt = %d\n", __FUNCTION__,
                 status_nat_vs_opt);
         printf ("%s: status_opt_vs_ref = %d\n", __FUNCTION__,
                 status_opt_vs_ref);
         printf ("%s: currTestStatus      = %d\n", __FUNCTION__, currTestStatus);

#endif
         pProfile[3 * tpi] =
             (int32_t) TI_profile_get_cycles (TI_PROFILE_KERNEL_OPT);
         pProfile[3 * tpi + 1] =
             (int32_t) TI_profile_get_cycles (TI_PROFILE_KERNEL_OPT_WARM);
         pProfile[3 * tpi + 2] =
             (int32_t) TI_profile_get_cycles (TI_PROFILE_KERNEL_OPT_WARMWRB);

         if (kerInitArgs.strideX == 1) {
            num_pts = numOutChannels * validColsOut;
         }

         if (kerInitArgs.strideX > 1 && kerInitArgs.Fc == 1) {
            num_pts = numOutChannels * validOutputRows * validColsPerRow;
         }
         else if (kerInitArgs.strideX > 1) {
            num_pts = numOutChannels * validOutputRows * validColsPerRow;
         }

         /* MMALIB_CNN_deconvolveBiasBiasReLUCompute_ixX_ixX_oxX_perfEst ( */
         /*     &src0_addr, */
         /*     &src1_addr, */
         /*     &dst_addr, */
         /*     &kerInitArgs, */
         /*     &kerExecInArgs, */
         /*     &kerExecOutArgs, */
         /*     iterN, */
         /*     &archCycles, */
         /*     &estCycles); */

         if (kerInitArgs.strideX == 1) {
            sprintf (
                desc,
                "%s generated input | Input Channels = %d, Output per ker "
                "Channels= %d, total Output Channels= %d, totalNCols = %d, "
                "Output number Cols = %d, kernel width = %d strideWidth %d "
                "dilation width = %d Predicate Bytes %d",
                testPatternString, numInChannels, subMChannels, numOutChannels,
                totalN, validColsOut, kernelWidth, strideWidth, dilationWidth,
                totalBytes);
         }

         if (kerInitArgs.strideX > 1 && kerInitArgs.Fc) {
            sprintf (
                desc,
                "%s generated input | Input Channels = %d, Output per ker "
                "Channels= %d, total Output Channels= %d, totalNCols = %d, "
                "validColsOut = %d, kernel width = %d strideWidth %d dilation "
                "width = %d",
                testPatternString, numInChannels, subMChannels, numOutChannels,
                totalN, validColsOut, kernelWidth, strideWidth, dilationWidth);
         }
         else if (kerInitArgs.strideX > 1) {
            sprintf (
                desc,
                "%s generated input | Input Channels = %d, Output per ker "
                "Channels= %d, total Output Channels= %d, totalNCols = %d, "
                "Output number Rows = %d, Output number Cols = %d, kernel "
                "width = %d strideWidth %d dilation width = %d",
                testPatternString, numInChannels, subMChannels, numOutChannels,
                totalN, validOutputRows, validColsPerRow, kernelWidth,
                strideWidth, dilationWidth);
         }

         TI_profile_add_test (testNum, num_pts, archCycles, estCycles,
                              currentTestFail, desc);
#if GENERATE_STATIC_DATA
         MMALIB_bufParams3D_t outputPrint;
         outputPrint.dim_x    = validColsOut;
         outputPrint.dim_y    = numOutChannels * (iterN);
         outputPrint.dim_z    = 1;
         outputPrint.stride_y = pitchC;
         outputPrint.stride_z = 0;
         if (kerInitArgs.strideX > 1) {
            outputPrint.dim_x    = validColsPerRow;
            outputPrint.dim_y    = validOutputRows;
            outputPrint.dim_z    = numOutChannels * iterN;
            outputPrint.stride_y = outputPitchPerRow * numBytes;
            outputPrint.stride_z = pitchC;
         }
         if (numBytes == 1) {
            outputPrint.data_type = MMALIB_UINT8;
            printf ("static int8_t staticRefOutput[] =\n");
            MMALIB_debugPrint3DVReg (dst, &outputPrint);
         }
         else {
            outputPrint.data_type = MMALIB_INT16;
            printf ("static int8_t staticRefOutput[] =\n");
         }
#endif
      }
      else {
         /* Display the error printout for this test vector before moving on to
          * the next test vector */
         TI_profile_skip_test (desc);
      }

      /* Free buffers for each test vector */
/*       #if !defined(PERFORMANCE_TEST) && !defined(COMPILER_TEST)
              free (dst_cn);
        #endif */
      if (prm[tpi].outputDataLocation == MMALIB_TEST_OUTPUT_HEAP) {
         // only malloc'd data can be freed
         TI_align_free (dst);
      }

      TI_align_free (src1);
      TI_align_free (src0);

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
      TI_memError ("MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX");
      return 1;
   }
   else
#else
   printf ("_HOST_BUILD is defined.\n");
#endif //_HOST_BUILD
   {
      return MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_d (&pProfile[0], 0);
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
/*  End of file:  MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_d.c */
/* ======================================================================== */
