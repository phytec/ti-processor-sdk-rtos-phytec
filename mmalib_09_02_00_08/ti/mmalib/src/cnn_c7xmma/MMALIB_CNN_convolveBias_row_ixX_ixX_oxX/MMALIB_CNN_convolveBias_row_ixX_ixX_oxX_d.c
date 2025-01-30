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
 *  @file     MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_d.c
 *
 *  @brief    File to demnostrate usage of kernel
 *
 *  @version  0.1 - Jan 2018 : Initial Version - [Asheesh Bhardwaj]
 *  @version  0.2 - Feb 2018 : Support for 8 bit [Asheesh Bhardwaj]
 *  @version  0.3 - March 2018: Support for 16 bit, templates and 3x3 stride by 2 [Asheesh Bhardwaj]
 *  @version  0.4 - Oct 2020: Support for scale and shift [Asheesh Bhardwaj]
 *  @version  0.5 - Sept 2021: Support for predicated load and group convolution [Asheesh Bhardwaj]
 *
 *******************************************************************************
 */

// include MMALIB
#include <mmalib.h>

// include test infrastructure provided by MMALIB
#include "../../test/MMALIB_test.h"

// include test data for this kernel
#include "MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_idat.h"

__attribute__ ((section (".msmcData"), aligned (64))) int8_t msmcBuffer[MMALIB_L3_RESULTS_BUFFER_SIZE];

__attribute__ ((section (".ddrData"), aligned (64))) int8_t ddrBuffer[2048 * 1024];

int16_t volatile volatileSum = 0; // use volatile to keep compiler from removing this operation, move to global to prevent compiler warning of unused variable

#define GENERATE_STATIC_DATA 0

/* MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_d:  Test Driver Routine */
int MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_d (uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   int32_t  tpi; /* test parameter index */
   int32_t  fail = 0;
   int32_t  overall_fail = 0;
   uint64_t idealCycles, archCycles, estCycles;
   int64_t  repCount;  /* number of times to repeat a test; useful for random data and random sizes (set to 1 for static) */
   int64_t  numReps;
   uint32_t num_pts = 0;
   /* Test Parameters */
   MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_testParams_t *prm;

   MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_getTestParams (&prm, &test_cases);

   MMALIB_kernelHandle handle;
   int32_t handleSize;

   MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_InitArgs kerInitArgs;
   MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_ExecInArgs kerExecInArgs;
   MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_ExecOutArgs kerExecOutArgs;

   /* Initialize profiling */
   TI_profile_init("convolveBias_row_ixX_ixX_oxX");

   /* Run each test case */
   for (tpi = 0; tpi < test_cases; tpi++) {

      // initialize status flags 
      int32_t status_nat_vs_opt = TI_TEST_KERNEL_FAIL; // test status : natural C vs. optimized C 
      int32_t status_opt_vs_ref = TI_TEST_KERNEL_FAIL; // test status : optimized C vs. static reference
      MMALIB_STATUS currTestStatus = MMALIB_STATUS_NOT_SET;  /* Test current testcase status */

      // dynamic range testing
      int64_t dynamicRangeMax_nat = 0;
      int64_t dynamicRangeMin_nat = 0;
      int64_t dynamicRangeMax_opt = 0;
      int64_t dynamicRangeMin_opt = 0;
      int64_t dynamicRangeMax_ref = 0;
      int64_t dynamicRangeMin_ref = 0;
      uint32_t inp0Size, inp1Size, outSize, biasSize, scaleSize, shiftSize;
      numReps = 1; //prm[tpi].numReps;

      int32_t inWidth               = prm[tpi].inWidth;
      int32_t inputBlockWidth       = inWidth + prm[tpi].pad;
      int32_t inChOffset            = prm[tpi].inChOffset;
      int32_t numInChannels         = prm[tpi].numInChannels;
      int32_t numOutChannels        = prm[tpi].numOutChannels;
      int32_t numOfOutputChKerBuf   = prm[tpi].numOfOutputChKerBuf;
      int32_t subMChannels          = prm[tpi].subMChannels;
      int32_t kernelWidth           = prm[tpi].kernelWidth;
      int32_t kernelHeight          = prm[tpi].kernelHeight;
      int32_t dilationWidth         = prm[tpi].dilationWidth;
      int32_t dilationHeight        = prm[tpi].dilationHeight;
      int32_t validColsIn           = prm[tpi].validColsIn;
      int32_t strideWidth           = prm[tpi].strideWidth;
      int32_t strideHeight          = prm[tpi].strideHeight;
      int32_t kDim                  = prm[tpi].kDim;
      int32_t pitchA                = prm[tpi].pitchA;
      int32_t pitchC                = prm[tpi].pitchC;
      uint8_t dataTypeA             = prm[tpi].dataTypeA;
      uint8_t dataTypeB             = prm[tpi].dataTypeB;
      uint8_t dataTypeC             = prm[tpi].dataTypeC;
      uint8_t dataTypeBias          = prm[tpi].dataTypeBias;
      uint8_t dataTypeS             = prm[tpi].dataTypeS;
      uint8_t activationType        = prm[tpi].activationType;
      int32_t pSatMax               = prm[tpi].pSatMax; // Parameterized saturation max
      int32_t pSatMin               = prm[tpi].pSatMin; // Parameterized saturation min
      int32_t pad                   = prm[tpi].pad;
      uint8_t quantMethod           = prm[tpi].quantMethod;
      int32_t bias                  = prm[tpi].biasB;
      int32_t expectedStatusCode    = prm[tpi].expectedStatusCode;
      uint8_t mode                  = prm[tpi].mode;
      int32_t circularOffset        = prm[tpi].circularOffset;
      int32_t maxHeight             = prm[tpi].maxHeight;
      int32_t totalN                = prm[tpi].totalN;
      int32_t subN                  = prm[tpi].subN;
      int32_t validColsPerRowIn     = prm[tpi].validColsPerRowIn;
      int32_t validRowsIn           = prm[tpi].validRowsIn;
      int32_t outputPitchPerRow     = prm[tpi].outputPitchPerRow;
      int32_t inputPitchPerRow      = prm[tpi].inputPitchPerRow;
      int32_t validColsInlast       = prm[tpi].validColsInlast;
      int32_t validRowsInlast       = prm[tpi].validRowsInlast;
      int32_t validColsPerRowInlast = prm[tpi].validColsPerRowInlast;
      int32_t spadOffset            = prm[tpi].spadOffset;
      int32_t spadOffsetExec        = prm[tpi].spadOffsetExec;
      int32_t padLeft               = prm[tpi].padLeft;
      int32_t padRight              = prm[tpi].padRight;
      int32_t padTop                = prm[tpi].padTop;
      int32_t padBottom             = prm[tpi].padBottom;
      int32_t validColsOutBottom    = prm[tpi].validColsOutBottom;
      int32_t padFillValue          = prm[tpi].padFillValue;
      int32_t numGroupsPerKernel    = prm[tpi].numGroupsPerKernel;
      int8_t  packetizeMode        = prm[tpi].packetizeMode;


      MMALIB_enableDynamicRange_type enableDynamicRange =  prm[tpi].enableDynamicRange;
      MMALIB_initDynamicRange_type  initDynamicRange =  prm[tpi].initDynamicRange;

      int32_t testNum  = prm[tpi].testID;
      int32_t numBytes = 1;
      int32_t MMA_SIZE = MMALIB_MMA_SIZE_8_BIT;
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

      MMALIB_CNN_convolveBias_row_processWeights_Args weightArgs;
      weightArgs.funcStyle          = MMALIB_FUNCTION_OPTIMIZED;
      weightArgs.Fr                 = kernelHeight;
      weightArgs.Fc                 = kernelWidth;
      weightArgs.numInChPerGroup    = numInChannels;
      weightArgs.subMChannels       = subMChannels;
      weightArgs.No                 = subMChannels * numGroupsPerKernel;
      weightArgs.numGroupsPerKernel = numGroupsPerKernel;
      weightArgs.pitchA             = pitchA;
      weightArgs.data_type          = dataTypeC;

#if defined (__SE_FEAT_SPARSITY)
      // int8_t packetizeMode = 1;
      weightArgs.packetizeMode      = packetizeMode;
#endif

      handleSize = MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_getHandleSize (&kerInitArgs);
      MMALIB_DEBUGPRINTFN(1, "After MMALIB_CNN Handle Size before %d\n", handleSize);
      handle     = malloc (handleSize);

      for(repCount = 0; repCount < numReps; repCount++){
         int32_t strideShiftW = 0;
         if (strideWidth == 2)
            strideShiftW = 1;
         if (strideWidth == 4)
            strideShiftW = 2;

         int32_t strideShiftH = 0;
         if (strideHeight == 2)
            strideShiftH = 1;
         if (strideHeight == 4)
            strideShiftH = 2;

         int32_t validColsOut =
            ((validColsIn - inputBlockWidth * (kernelHeight * dilationHeight - 1)) >>
             strideShiftH) >>
            strideShiftW;
         int32_t validColsPerRow = 0;

         // compute buffer sizes
         inp0Size = numOfOutputChKerBuf * pitchA * numGroupsPerKernel;

         if (mode == MMALIB_LINEAR && strideWidth == 1 && padLeft > 0) {
            inp1Size = (numInChannels * inChOffset * numGroupsPerKernel + padLeft) * numBytes;
         } else {
            inp1Size = numInChannels * inChOffset * numGroupsPerKernel * numBytes;
         }
         outSize  = numOutChannels * pitchC * numGroupsPerKernel * (totalN / subN);
         biasSize = numOutChannels * numGroupsPerKernel;

         if (quantMethod == 1) {
            scaleSize = numOutChannels * numGroupsPerKernel;
            shiftSize = numOutChannels * numGroupsPerKernel;
         } else {
            scaleSize = 1;
            shiftSize = 1;
         }

         // allocate buffers for each test vector */
         MMALIB_DEBUGPRINTFN(1, "pitchA %d, inChOffset %d, pitchC %d, inp0Size %d, inp1Size %d, outSize %d\n",
               pitchA,
               inChOffset,
               pitchC,
               inp0Size,
               inp1Size,
               outSize);
         MMALIB_DEBUGPRINTFN(1, "numOfOutputChKerBuf %d, numInChannels %d, numOutChannels %d, totalN %d, "
               "subN %d\n",
               numOfOutputChKerBuf,
               numInChannels,
               numOutChannels,
               totalN,
               subN);

         // int8_t *src0_nat = (int8_t *) TI_memalign (numBytes * MMA_SIZE, inp0Size);
         // running out of L2 memory; need to keep src0_nat in DDR; MMALIB_CNN_convolveBias_row_processWeights_reorder call will move (while formatting) it to L2
         int8_t *src0_nat = (int8_t *) ddrBuffer;
         int8_t *src0_reordered = NULL;
         int8_t *src0 = NULL;
         int8_t *src1 = NULL; //(int8_t *)TI_memalign(inChOffset*numBytes, inp1Size);

         int32_t featureMapPadOffset = 0; 
         // assumption strided flow will follow the circular mode of addressing aligning the pointer to memory offset
         if (strideWidth == 2 &&  kernelWidth ==3) {
            featureMapPadOffset = numBytes * padLeft;
         } else {}
         inp1Size += featureMapPadOffset;


#if defined (__SE_FEAT_SPARSITY)
         int8_t *src0_depac     = NULL;
         if (packetizeMode && weightArgs.funcStyle == MMALIB_FUNCTION_OPTIMIZED) {
            src0_depac = (int8_t *) TI_memalign (numBytes * MMA_SIZE, inp0Size);
         }
#endif // #if defined (__SE_FEAT_SPARSITY)

         if (mode == MMALIB_LINEAR) {
            src1 = (int8_t *) TI_memalign (MMA_SIZE * numBytes, inp1Size);

         } else if (mode == MMALIB_SE_CIRCULAR) {
            src1 = (int8_t *) TI_memalign (inChOffset * numBytes, inp1Size);
         }

         int8_t *pSrc1 = src1;
         // assumption strided flow will follow the circular mode of addressing aligning the pointer to memory offset
         if (strideWidth == 2 &&  kernelWidth ==3) {
            pSrc1 = src1 +  featureMapPadOffset;
         } else {}

         // bias buffer
         int8_t *src2 = NULL;

         if (dataTypeBias == MMALIB_INT32 && bias == 1 ) {
            src2 = (int8_t *) TI_memalign (MMA_SIZE * numBytes*4, biasSize*numBytes*4);
         } else if (dataTypeBias == MMALIB_INT64  && bias == 1) {
            src2 = (int8_t *) TI_memalign (MMA_SIZE * numBytes*4, biasSize*numBytes*4);
         } else {

         }

         // scale buffer
         int8_t *src3 = NULL;
         src3 = (int8_t *) TI_memalign (MMA_SIZE, scaleSize);

         // shift buffer
         uint8_t *src4 = NULL;
         src4 = (uint8_t *) TI_memalign (MMA_SIZE, shiftSize);

         int64_t *dynMinPtr = NULL;
         int64_t *dynMaxPtr = NULL;
         if (enableDynamicRange == MMALIB_DYNAMIC_RANGE_ENABLE && prm[tpi].dynamicRangeMin != NULL) {
            dynMinPtr = (int64_t *) TI_memalign (MMA_SIZE, sizeof(int64_t));
            dynMaxPtr = (int64_t *) TI_memalign (MMA_SIZE, sizeof(int64_t));
         }

         int8_t *dst = NULL;
         if (prm[tpi].outputDataLocation == MMALIB_TEST_OUTPUT_HEAP){
            dst = (int8_t *) TI_memalign (numBytes * MMA_SIZE, outSize);
         }
         else if (prm[tpi].outputDataLocation == MMALIB_TEST_OUTPUT_MSMC) {
            //dst = (int8_t *) TI_memalign (numBytes * MMA_SIZE, outSize);
            dst = (int8_t *) msmcBuffer;
         } else {
            dst = NULL;
         }

#if !defined(PERFORMANCE_TEST) && !defined(COMPILER_TEST)
         int8_t *dst_cn = NULL;
         if (prm[tpi].outputDataLocation == MMALIB_TEST_OUTPUT_HEAP) {
            dst_cn = (int8_t *) malloc (outSize);
         } else {
            //dst_cn = (int8_t *) ddrBuffer;
            // also allocated src0_nat to ddrBuffer with size inp0Size, so need to offset this allocation
            dst_cn = (int8_t *) ddrBuffer + inp0Size;
         }
#endif

         int32_t MCount, NCount;

         MMALIB_DEBUGPRINTFN(1, "MMALIB_DEBUGPRINT test_case %d  test ID %d src2: %p pSrc1: %p  src0 %p src3: %p dst %p \n",
               tpi,
               testNum,
               src2,
               pSrc1,
               src0_nat,
               src3,
               dst);
#if !defined(PERFORMANCE_TEST) && !defined(COMPILER_TEST)
         MMALIB_DEBUGPRINTFN(1, "MMALIB_DEBUGPRINT test_case %d  dst_cn %p\n", tpi, dst_cn);
#endif

         // only run the test if the buffer allocations fit in the heap */
         if (src0_nat && src1 && dst) {
            MMALIB_bufParams2D_t src0_addr; // paramsWgt
            MMALIB_bufParams2D_t src1_addr; // input
            MMALIB_bufParams2D_t src2_addr; // bias
            MMALIB_bufParams1D_t src3_addr; // scale
            MMALIB_bufParams3D_t dst_addr;

            // allocate coefficient buffers
            src0_addr.dim_x     = kDim;
            src0_addr.dim_y     = numOfOutputChKerBuf*numGroupsPerKernel;
            src0_addr.stride_y  = pitchA;
            src0_addr.data_type = dataTypeA;

            // allocate input channels
            src1_addr.dim_x     = inChOffset;
            src1_addr.dim_y     = numInChannels*numGroupsPerKernel;
            src1_addr.stride_y  = inChOffset*MMALIB_sizeof(dataTypeB);
            src1_addr.data_type = dataTypeB;

            // allocate bias per output channel
            src2_addr.dim_x     = numOutChannels;
            src2_addr.dim_y     = 1;
            src2_addr.stride_y  = numOutChannels;
            src2_addr.data_type = dataTypeBias;

            // allocate scale per output channel
            src3_addr.dim_x     = numOutChannels;
            src3_addr.data_type = dataTypeS;

            // allocate output channels
            dst_addr.dim_x     = pitchC / numBytes;
            dst_addr.dim_y     = numOutChannels;
            dst_addr.stride_y  = pitchC;
            dst_addr.dim_z     = numGroupsPerKernel;
            dst_addr.stride_z  = numOutChannels*pitchC;//numOutChannels*pitchC = pitchG
            dst_addr.data_type = dataTypeC;

            // fill input arrays according to desired test pattern
            TI_fillBuffer (prm[tpi].testPattern,
                  (uint8_t) 255,
                  src0_nat,
                  prm[tpi].staticKernel,
                  kDim,
                  numOfOutputChKerBuf*numGroupsPerKernel,
                  pitchA,
                  MMALIB_sizeof(src0_addr.data_type),
                  testPatternString);

            // fill input buffer
            TI_fillBuffer (prm[tpi].testPattern,
                  (uint8_t) 255,
                  pSrc1,
                  prm[tpi].staticIn,
                  inChOffset,
                  numInChannels*numGroupsPerKernel,
                  inChOffset*numBytes,
                  MMALIB_sizeof(src1_addr.data_type),
                  testPatternString);

            if (bias == 1) {
               // fill bias buffer
               TI_fillBuffer (prm[tpi].testPattern,
                     (uint8_t) 255,
                     src2,
                     prm[tpi].staticBias,
                     biasSize,
                     1,
                     biasSize,
                     MMALIB_sizeof(src2_addr.data_type),
                     testPatternString);
            } else {}

            // fill scale buffer
            TI_fillBuffer (prm[tpi].testPattern,
                  (uint8_t) 255,
                  src3,
                  prm[tpi].staticScale,
                  scaleSize,
                  1,
                  scaleSize,
                  MMALIB_sizeof(src3_addr.data_type),
                  testPatternString);

            // fill shift buffer
            TI_fillBuffer (prm[tpi].testPattern,
                  (uint8_t) 255,
                  src4,
                  prm[tpi].staticShift,
                  shiftSize,
                  1,
                  shiftSize,
                  sizeof (uint8_t),
                  testPatternString);

            if (enableDynamicRange == MMALIB_DYNAMIC_RANGE_ENABLE && prm[tpi].dynamicRangeMin != NULL) {
               // dynamic min/max range buffer
               TI_fillBuffer (prm[tpi].testPattern,
                     (uint8_t) 255,
                     dynMinPtr,
                     prm[tpi].dynamicRangeMin,
                     1,
                     1,
                     1,
                     sizeof (int64_t),
                     testPatternString);
               TI_fillBuffer (prm[tpi].testPattern,
                     (uint8_t) 255,
                     dynMaxPtr,
                     prm[tpi].dynamicRangeMax,
                     1,
                     1,
                     1,
                     sizeof (int64_t),
                     testPatternString);
               dynamicRangeMin_ref = *dynMinPtr;
               dynamicRangeMax_ref = *dynMaxPtr;
            } else {}

            int32_t weightBytes = MMALIB_CNN_convolveBias_row_processWeights_getMemorySize(&weightArgs, src0_nat);

            src0_reordered = (int8_t *) TI_memalign (numBytes * MMA_SIZE, weightBytes);

            int32_t status_reordered_weights = MMALIB_ERR_FAILURE;
            if(src0_reordered == NULL){
               // fail for unsuccessful allocation
               status_reordered_weights = MMALIB_ERR_NULL_POINTER;
            } else {
               status_reordered_weights = MMALIB_CNN_convolveBias_row_processWeights_reorder(
                     &weightArgs,
                     src0_nat, // natural order for kernel weights
                     src0_reordered);
            }

            if (status_reordered_weights == MMALIB_SUCCESS){
               src0 = src0_reordered;
            } else {
               MMALIB_PRINTF("MMALIB_CNN_convolveBias_row_processWeights_reorder failed with status = %d!\n", status_reordered_weights);
               return 1;
            }

#if defined(__SE_FEAT_SPARSITY)
            if (packetizeMode && weightArgs.funcStyle == MMALIB_FUNCTION_OPTIMIZED) {
               int32_t status_sparse_dec = MMALIB_CNN_convolveBias_row_processWeights_depacketize(
                     &weightArgs,
                     src0_reordered,
                     src0_depac);

               int32_t status_sparse_vs_prov = TI_compare_mem_2D ((void *) src0_nat,
                     (void *) src0_depac,
                     0,
                     0,
                     kDim,
                     numOfOutputChKerBuf*numGroupsPerKernel,
                     pitchA,
                     numBytes);
               MMALIB_DEBUGPRINTFN(1, "===================================================================%s", "\n");
               MMALIB_DEBUGPRINTFN(1, "Reaching comparison for Compare sparsity output and original output%s", "\n");
               MMALIB_DEBUGPRINTFN(1, "   status_sparse_vs_prov = %d\n", status_sparse_vs_prov);
               MMALIB_DEBUGPRINTFN(1, "===================================================================%s", "\n");
               MMALIB_DEBUGPRINTFN(1, "orig = %llx, sparse = %llx, dec = %llx\n",src0,src0_reordered,src0_depac);
               if (status_sparse_vs_prov == TI_TEST_KERNEL_FAIL) {
                  return 1;
               }
            }

#endif // #if defined(__SE_FEAT_SPARSITY)

            // assign to init args and interface file
            kerInitArgs.Fc                   = kernelWidth;
            kerInitArgs.Fr                   = kernelHeight;
            kerInitArgs.dilationX            = dilationWidth;
            kerInitArgs.dilationY            = dilationHeight;
            kerInitArgs.strideX              = strideWidth;
            kerInitArgs.strideY              = strideHeight;
            kerInitArgs.validColsIn          = validColsIn;
            kerInitArgs.subMChannels         = subMChannels;
            kerInitArgs.inWidth              = inWidth;
            kerInitArgs.pad                  = pad;
            kerInitArgs.maxHeight            = maxHeight;
            kerInitArgs.inChOffset           = inChOffset;
            kerInitArgs.No                   = numOutChannels;
            kerInitArgs.bias                 = bias;
            kerInitArgs.activationType       = activationType;
            kerInitArgs.pSatMax              = pSatMax; // Parameterized saturation max
            kerInitArgs.pSatMin              = pSatMin; // Parameterized saturation min
            kerInitArgs.mode                 = mode;
            kerInitArgs.validColsPerRowIn    = validColsPerRowIn;
            kerInitArgs.validRowsIn          = validRowsIn;
            kerInitArgs.outputPitchPerRow    = outputPitchPerRow;
            kerInitArgs.inputPitchPerRow     = inputPitchPerRow;
            kerInitArgs.col                  = spadOffset;
            kerInitArgs.padLeft              = padLeft;
            kerInitArgs.padRight             = padRight;
            kerInitArgs.padTop               = padTop;
            kerInitArgs.padBottom            = padBottom;
            kerInitArgs.validColsOutBottom   = validColsOutBottom;
            kerExecInArgs.enableDynamicRange = enableDynamicRange;
            kerExecInArgs.initDynamicRange   = initDynamicRange;
            kerExecInArgs.quantMethod        = quantMethod;
            kerExecInArgs.padFillValue       = padFillValue;
            kerInitArgs.numGroupsPerKernel   = numGroupsPerKernel;

#if defined(__SE_FEAT_SPARSITY)
            kerInitArgs.packetizeMode        = packetizeMode;
#endif
            int32_t iterN, validOutputRows;

            /* Initialize kernel */
            kerInitArgs.funcStyle = MMALIB_FUNCTION_OPTIMIZED;
            TI_profile_start (TI_PROFILE_KERNEL_INIT);
            MMALIB_asm (" MARK 0");
            currTestStatus = MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_init (
                  handle, &src0_addr, &src1_addr, &src2_addr, &src3_addr, &dst_addr, &kerInitArgs);

            MMALIB_asm (" MARK 1");
            TI_profile_stop ();

            /* Test optimized kernel */
            iterN = 0;

            if(currTestStatus == MMALIB_SUCCESS){
               MMALIB_DEBUGPRINTFN(1, "Entering: MMALIB_CNN_convolveBias_row_ixX_ixX_oxX with currTestStatus = %d\n", currTestStatus);
               for (NCount = 0; NCount < totalN; NCount += subN) {
                  int8_t *src1_Iter = pSrc1 + NCount*numBytes;// + (rowOffset - 1)*numBytes*inWidth;// + spadOffset*numBytes;
                  if (mode == MMALIB_SE_CIRCULAR)
                     src1_Iter = src1_Iter + circularOffset * numBytes;
                  for (MCount = 0; MCount < numOutChannels; MCount += subMChannels) {
                     kerExecInArgs.subMChannels      = subMChannels;
                     kerExecInArgs.validColsIn       = validColsInlast;
                     kerExecInArgs.validColsPerRowIn = validColsPerRowInlast;
                     kerExecInArgs.validRowsIn       = validRowsInlast;
                     kerExecInArgs.col               = spadOffsetExec;
                     int8_t *dst_iter = dst + MCount * pitchC + numOutChannels * iterN * pitchC;
                     MMALIB_DEBUGPRINTFN(1, "src1_Iter %p dst_iter %p dst %p src0 %p\n",
                           src1_Iter,
                           dst_iter,
                           dst,
                           src0);
                     MMALIB_DEBUGPRINTFN(1, "subMChannels %d, validColsIn %d MCount %d NCount %d subN %d\n",
                           subMChannels,
                           validColsIn,
                           MCount,
                           NCount,
                           subN);

                     TI_profile_start (TI_PROFILE_KERNEL_OPT);
                     MMALIB_asm (" MARK 2");
                     currTestStatus = MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_exec (
                           handle, src0, src1_Iter, src2, src3, src4, dst_iter, &kerExecInArgs, &kerExecOutArgs);
                     MMALIB_asm (" MARK 3");
                     TI_profile_stop ();
                     validColsOut    = kerExecOutArgs.validColsOut;
                     validColsPerRow = kerExecOutArgs.validColsPerRowOut;
                     dynamicRangeMin_opt = kerExecOutArgs.dynamicRangeMin;
                     dynamicRangeMax_opt = kerExecOutArgs.dynamicRangeMax;
                  }

#if MMALIB_DEBUGPRINT
                  validColsOut = kerExecOutArgs.validColsOut;
                  MMALIB_DEBUGPRINTFN(1, "OptC: valid cols out %d itenN %d\n", validColsOut, iterN);
#endif
                  iterN++;
               }
               __SE0_CLOSE ();
               __SE1_CLOSE ();

               validOutputRows = kerExecOutArgs.validRowsOut;

               MMALIB_DEBUGPRINTFN(1, "\n validColsOut after optimized C %d validColsPerRow %d "
                     "outputPitchPerRow %d validOutputRows%d\n",
                     validColsOut,
                     validColsPerRow,
                     outputPitchPerRow,
                     validOutputRows);
            } else {
               MMALIB_PRINTF("MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_init failed with status = %d!\n", currTestStatus);
            }

            // run warm instruction cache test
            if (currTestStatus == MMALIB_SUCCESS)
            {
               /* Initialize kernel */
               kerInitArgs.funcStyle = MMALIB_FUNCTION_OPTIMIZED;
               TI_profile_start (TI_PROFILE_KERNEL_INIT);
               MMALIB_asm (" MARK 4");
               currTestStatus = MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_init (handle, &src0_addr, &src1_addr,  &src2_addr, &src3_addr, &dst_addr, &kerInitArgs);

               MMALIB_asm (" MARK 5");
               TI_profile_stop ();

#if defined(VMIN_DDR_MEMORY_RANGE)
               int64_t vminRepCount;
               int64_t vminNumReps = 6000000000;
               for(vminRepCount = 0; vminRepCount < vminNumReps; vminRepCount++){
#endif // VMIN_DDR_MEMORY_RANGE
                  /* Test optimized kernel */
                  iterN = 0;
                  if(currTestStatus == MMALIB_SUCCESS){
                     for (NCount = 0; NCount < totalN; NCount += subN) {
                        int8_t *src1_Iter = pSrc1 + NCount * numBytes;
                        if (mode == MMALIB_SE_CIRCULAR)
                           src1_Iter = src1_Iter + circularOffset * numBytes;
                        for (MCount = 0; MCount < numOutChannels; MCount += subMChannels) {
                           kerExecInArgs.subMChannels      = subMChannels;
                           kerExecInArgs.validColsIn       = validColsInlast;
                           kerExecInArgs.validColsPerRowIn = validColsPerRowInlast;
                           kerExecInArgs.validRowsIn       = validRowsInlast;
                           kerExecInArgs.col               = spadOffsetExec;
                           int8_t *dst_iter =
                              dst + MCount * pitchC + numOutChannels * iterN * pitchC;
                           TI_profile_start (TI_PROFILE_KERNEL_OPT_WARM);
                           MMALIB_asm (" MARK 6");
                           currTestStatus = MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_exec (handle, src0, src1_Iter, src2, src3, src4, dst_iter, &kerExecInArgs, &kerExecOutArgs);
                           MMALIB_asm (" MARK 7");
                           TI_profile_stop ();
                           validColsOut    = kerExecOutArgs.validColsOut;
                           validColsPerRow = kerExecOutArgs.validColsPerRowOut;
                        }
                        iterN++;
                     }
                     __SE0_CLOSE ();
                     __SE1_CLOSE ();
#if defined(VMIN_DDR_MEMORY_RANGE)
                  }
#endif // VMIN_DDR_MEMORY_RANGE          

                  // get output to L1D
                  int32_t row, col;
                  int16_t outSum = 0;
                  int8_t *pDst = (int8_t *) dst; // treat output as bytes to be data type agnostic
                  for (row = 0; row < dst_addr.dim_y; row++) {
                     for (col = 0; col < dst_addr.stride_y; col++) {
                        outSum += *pDst;
                        pDst++;
                     }
                  }

                  // dummy store of outSum to insure that the compiler does not remove it.
                  volatileSum += outSum;

               } else {
                  MMALIB_PRINTF("MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_init failed with status = %d!\n", currTestStatus);
               }



               /* Initialize kernel */
               kerInitArgs.funcStyle = MMALIB_FUNCTION_OPTIMIZED;
               TI_profile_start (TI_PROFILE_KERNEL_INIT);
               MMALIB_asm (" MARK 8");
               currTestStatus = MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_init (
                     handle, &src0_addr, &src1_addr,  &src2_addr, &src3_addr, &dst_addr, &kerInitArgs);
               MMALIB_asm (" MARK 9");
               TI_profile_stop ();

               /* Test optimized kernel */

               iterN = 0;

               if(currTestStatus == MMALIB_SUCCESS){
                  for (NCount = 0; NCount < totalN; NCount += subN) {
                     int8_t *src1_Iter = pSrc1 + NCount * numBytes;
                     if (mode == MMALIB_SE_CIRCULAR)
                        src1_Iter = src1_Iter + circularOffset * numBytes;
                     for (MCount = 0; MCount < numOutChannels; MCount += subMChannels) {
                        kerExecInArgs.subMChannels      = subMChannels;
                        kerExecInArgs.validColsIn       = validColsInlast;
                        kerExecInArgs.validColsPerRowIn = validColsPerRowInlast;
                        kerExecInArgs.validRowsIn       = validRowsInlast;
                        kerExecInArgs.col               = spadOffsetExec;
                        int8_t *dst_iter =
                           dst + MCount * pitchC + numOutChannels * iterN * pitchC;

                        TI_profile_start (TI_PROFILE_KERNEL_OPT_WARMWRB);
                        MMALIB_asm (" MARK 10");
                        currTestStatus = MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_exec (
                              handle, src0, src1_Iter, src2, src3, src4, dst_iter, &kerExecInArgs, &kerExecOutArgs);
                        MMALIB_asm (" MARK 11");
                        TI_profile_stop ();
                        validColsOut    = kerExecOutArgs.validColsOut;
                        validColsPerRow = kerExecOutArgs.validColsPerRowOut;
                     }
                     iterN++;
                  }
                  __SE0_CLOSE ();
                  __SE1_CLOSE ();
               } else {
                  MMALIB_PRINTF("MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_init failed with status = %d!\n", currTestStatus);
               }
            }

#if !defined(PERFORMANCE_TEST) && !defined(COMPILER_TEST) && !(defined(RTL_TEST))
            /* Test _cn kernel */
            kerInitArgs.funcStyle = MMALIB_FUNCTION_NATC;
            currTestStatus = MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_init (
                  handle, &src0_addr, &src1_addr,  &src2_addr, &src3_addr, &dst_addr, &kerInitArgs);
            iterN = 0;
            if(currTestStatus == MMALIB_SUCCESS){
               for (NCount = 0; NCount < totalN; NCount += subN) {
                  int8_t *src1_Iter = pSrc1 + NCount * numBytes;
                  if (mode == MMALIB_SE_CIRCULAR)
                     src1_Iter = src1_Iter + circularOffset * numBytes;
                  for (MCount = 0; MCount < numOutChannels; MCount += subMChannels) {
                     kerExecInArgs.subMChannels      = subMChannels;
                     kerExecInArgs.validColsIn       = validColsInlast;
                     kerExecInArgs.validColsPerRowIn = validColsPerRowInlast;
                     kerExecInArgs.validRowsIn       = validRowsInlast;
                     kerExecInArgs.col               = spadOffsetExec;
                     int8_t *dst_cn_iter =
                        dst_cn + MCount * pitchC + numOutChannels * iterN * pitchC;

                     MMALIB_DEBUGPRINTFN(1, "src1_Iter %p dst_cn_iter %p src0 %p\n", src1_Iter, dst_cn_iter, src0);
                     MMALIB_DEBUGPRINTFN(1, "subMChannels %d, validColsIn %d MCount %d NCount %d subN %d\n",
                           subMChannels,
                           validColsIn,
                           MCount,
                           NCount,
                           subN);
                     TI_profile_start (TI_PROFILE_KERNEL_CN);
                     MMALIB_asm (" MARK 4");
                     currTestStatus = MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_exec (
                           handle, src0_nat, src1_Iter, src2, src3, src4, dst_cn_iter, &kerExecInArgs, &kerExecOutArgs);
                     MMALIB_asm (" MARK 5");
                     TI_profile_stop ();
                     validColsPerRow = kerExecOutArgs.validColsPerRowOut;
                     validColsOut    = kerExecOutArgs.validColsOut;
                     dynamicRangeMin_nat = kerExecOutArgs.dynamicRangeMin;
                     dynamicRangeMax_nat = kerExecOutArgs.dynamicRangeMax;
                  }
                  MMALIB_DEBUGPRINTFN(1, "NatC: valid cols out %d itenN %d\n", kerExecOutArgs.validColsOut, iterN);
                  iterN++;
               }
               validOutputRows = kerExecOutArgs.validRowsOut;
               MMALIB_DEBUGPRINTFN(1, "validColsOut after refrence C %d validOutputRows %d validColsPerRow %d \n",
                     validColsOut,
                     validOutputRows,
                     validColsPerRow);
            } else {
               MMALIB_PRINTF("MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_init failed with status = %d!\n", currTestStatus);
            }


            // compare natural C output and optimized output 
            if (currTestStatus == MMALIB_SUCCESS){
               if (kerInitArgs.strideX == 1) {
                  status_nat_vs_opt = TI_compare_mem_3D ((void *) dst,
                        (void *) dst_cn,
                        0,
                        0,
                        validColsOut,
                        numOutChannels * iterN,
                        numGroupsPerKernel,
                        pitchC,
                        numOutChannels*pitchC,
                        numBytes);
               } else if (kerInitArgs.strideX > 1) {
                  status_nat_vs_opt = TI_compare_mem_3D ((void *) dst,
                        (void *) dst_cn,
                        0,
                        0,
                        validColsPerRow,
                        validOutputRows,
                        numOutChannels * iterN,
                        outputPitchPerRow,
                        pitchC,
                        numBytes);
               }

               MMALIB_DEBUGPRINTFN(1, "Compared natural C to optimized output. status_nat_vs_opt = %d\n", status_nat_vs_opt);

               MMALIB_DEBUGPRINTFN(1, "Without dynamic range, status_nat_vs_opt = %d.\n", status_nat_vs_opt);
               MMALIB_DEBUGPRINTFN(1, "dynamicRangeMin_nat = %ld, dynamicRangeMin_opt = %ld\n", dynamicRangeMin_nat, dynamicRangeMin_opt);
               MMALIB_DEBUGPRINTFN(1, "dynamicRangeMax_nat = %ld, dynamicRangeMax_opt = %ld\n", dynamicRangeMax_nat, dynamicRangeMax_opt);
               if(prm[tpi].enableDynamicRange == MMALIB_DYNAMIC_RANGE_ENABLE){
                  if(dynamicRangeMin_nat != dynamicRangeMin_opt){
                     status_nat_vs_opt = TI_TEST_KERNEL_FAIL;
                     MMALIB_DEBUGPRINTFN(1, "Dynamic range minimum error.  dynamicRangeMin_nat = %ld, dynamicRangeMin_opt = %ld\n", dynamicRangeMin_nat, dynamicRangeMin_opt);
                  }
                  if(dynamicRangeMax_nat != dynamicRangeMax_opt){
                     status_nat_vs_opt = TI_TEST_KERNEL_FAIL;
                     MMALIB_DEBUGPRINTFN(1, "Dynamic range maximum error.  dynamicRangeMax_nat = %ld, dynamicRangeMax_opt = %ld\n", dynamicRangeMax_nat, dynamicRangeMax_opt);
                  }
               }
            } else {
               if(expectedStatusCode == currTestStatus) {
                  status_nat_vs_opt = TI_TEST_KERNEL_PASS;
               }
            }
#else // #if !defined(PERFORMANCE_TEST) && !defined(COMPILER_TEST) && !(defined(RTL_TEST))
      // set to pass since it wasn't supposed to run.
            status_nat_vs_opt = TI_TEST_KERNEL_PASS;
#endif //#if !defined(PERFORMANCE_TEST)  && !defined(COMPILER_TEST)
 
           if ((prm[tpi].staticOut != NULL) && (currTestStatus == MMALIB_SUCCESS)) {
               MMALIB_DEBUGPRINTFN(1, "dst %p prm[tpi].staticOut %p validColsOut %d numOutChannels "
                     "%d pitchC %d outputPitchPerRow %d validColsPerRow %d\n",
                     dst,
                     prm[tpi].staticOut,
                     validColsOut,
                     numOutChannels,
                     pitchC,
                     outputPitchPerRow,
                     validColsPerRow);
               if (kerInitArgs.strideX == 1) {
                  status_opt_vs_ref = TI_compare_mem_3DDualStride((void *) dst,
                        (void *) prm[tpi].staticOut,
                        0,//tolerance,
                        0,// wrap_exception,
                        validColsOut,// dim_x,
                        numOutChannels,// dim_y,
                        numGroupsPerKernel,// dim_z,
                        pitchC,// strideA_y,
                        numOutChannels*pitchC,// strideA_z,
                        validColsOut*numBytes,// strideB_y,
                        numOutChannels*validColsOut*numBytes,// strideB_z,
                        numBytes// elementSize
                        );
               } else if (kerInitArgs.strideX > 1) {
                  status_opt_vs_ref = TI_compare_mem_3DDualStride((void *) dst,
                        (void *) prm[tpi].staticOut,
                        0,//tolerance,
                        0,// wrap_exception,
                        validColsPerRow,// dim_x,
                        validOutputRows,// dim_y,
                        numOutChannels * iterN,// dim_z,
                        outputPitchPerRow,// strideA_y,
                        pitchC,// strideA_z,
                        validColsPerRow*numBytes,// strideB_y,
                        validOutputRows*validColsPerRow*numBytes,// strideB_z,
                        numBytes// elementSize
                        );
               }

               if(prm[tpi].enableDynamicRange == MMALIB_DYNAMIC_RANGE_ENABLE){
                  MMALIB_DEBUGPRINTFN(1, "dynamicRangeMin_ref = %ld, dynamicRangeMin_opt = %ld\n", dynamicRangeMin_ref, dynamicRangeMin_opt);
                  MMALIB_DEBUGPRINTFN(1, "dynamicRangeMax_ref = %ld, dynamicRangeMax_opt = %ld\n", dynamicRangeMax_ref, dynamicRangeMax_opt);

                  if(dynamicRangeMin_ref != dynamicRangeMin_opt){
                     status_opt_vs_ref = TI_TEST_KERNEL_FAIL;
                     MMALIB_DEBUGPRINTFN(1, "Dynamic range minimum error.  dynamicRangeMin_ref = %ld, dynamicRangeMin_opt = %ld\n", dynamicRangeMin_ref, dynamicRangeMin_opt);
                  }
                  if(dynamicRangeMax_ref != dynamicRangeMax_opt){
                     status_opt_vs_ref = TI_TEST_KERNEL_FAIL;
                     MMALIB_DEBUGPRINTFN(1, "Dynamic range maximum error.  dynamicRangeMax_ref = %ld, dynamicRangeMax_opt = %ld\n", dynamicRangeMax_ref, dynamicRangeMax_opt);
                  }
               }

            } else {
               status_opt_vs_ref = TI_TEST_KERNEL_PASS;
            }

            // set the 'fail' flag based on test vector comparison results
            fail = ((status_nat_vs_opt == TI_TEST_KERNEL_FAIL) ||
                  (status_opt_vs_ref == TI_TEST_KERNEL_FAIL) ||
                  (currTestStatus != expectedStatusCode)) ? 1 : 0;

            overall_fail = overall_fail || fail;

            MMALIB_DEBUGPRINTFN(1, "%s: status_nat_vs_opt = %d\n", __FUNCTION__, status_nat_vs_opt);
            MMALIB_DEBUGPRINTFN(1, "%s: status_opt_vs_ref = %d\n", __FUNCTION__, status_opt_vs_ref);
            MMALIB_DEBUGPRINTFN(1, "%s: currTestStatus   = %d\n", __FUNCTION__, currTestStatus);

            pProfile[3 * tpi] = (int32_t) TI_profile_get_cycles (TI_PROFILE_KERNEL_OPT);
            pProfile[3 * tpi + 1] =
               (int32_t) TI_profile_get_cycles (TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[3 * tpi + 2] =
               (int32_t) TI_profile_get_cycles (TI_PROFILE_KERNEL_OPT_WARMWRB);

            if (kerInitArgs.strideX == 1 ||  (kerInitArgs.strideX > 1 && kerInitArgs.Fc == 1)) {
               idealCycles = ((kernelWidth * kernelHeight * numInChannels) * numOutChannels *
                     validColsOut * iterN) /
                  (MMA_SIZE * MMA_SIZE);
               num_pts = numOutChannels * validColsOut * iterN;
            } else if (kerInitArgs.strideX > 1) {
               idealCycles = ((kernelWidth * kernelHeight * numInChannels) * numOutChannels *
                     validOutputRows * validColsPerRow * iterN) /
                  (MMA_SIZE * MMA_SIZE);
               num_pts = numOutChannels * validOutputRows * validColsPerRow * iterN;
            }
            if (idealCycles == 0)
               idealCycles = 1;

            // setting ideal cycles to be zero until the right formula is identified for the bias
            idealCycles = 0;
            MMA_CNNLIB_convolveBias_ixX_ixX_oxX_perfEst (&src0_addr,
                  &src1_addr,
                  &dst_addr,
                  &kerInitArgs,
                  &kerExecInArgs,
                  &kerExecOutArgs,
                  iterN,
                  &archCycles,
                  &estCycles);

                  if (kerInitArgs.strideX == 1 || (kerInitArgs.strideX > 1 && kerInitArgs.Fc == 1)) {
                     sprintf (desc,
                           "%s generated input | Input Channels = %d, Output per ker Channels= "
                           "%d, total Output Channels= %d, totalNCols = %d, "
                           "Output number Cols = %d, kernel width = %d stride width = %d, stride height = %d, dilation "
                           "width = %d",
                           testPatternString,
                           numInChannels,
                           subMChannels,
                           numOutChannels,
                           totalN,
                           validColsOut,
                           kernelWidth,
                           strideWidth,
                           strideHeight,
                           dilationWidth);
                  }
                  else if (kerInitArgs.strideX > 1) {
                     sprintf (desc,
                           "%s generated input | Input Channels = %d, Output per ker Channels= "
                           "%d, total Output Channels= %d, totalNCols = %d, "
                           "Output number Rows = %d, Output number Cols = %d, kernel width = "
                           "%d stride width = %d stride height = %d dilation width = %d",
                           testPatternString,
                           numInChannels,
                           subMChannels,
                           numOutChannels,
                           totalN,
                           validOutputRows,
                           validColsPerRow,
                           kernelWidth,
                           strideWidth,
                           strideHeight,
                           dilationWidth);
                  }

                  TI_profile_add_test (
                        testNum, num_pts, archCycles, estCycles, fail, desc);
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
                     MMALIB_DEBUGPRINTFN(1, "static int8_t staticRefOutput[] =\n");
                     MMALIB_debugPrint3DVReg(dst, &outputPrint);
                  }
                  else {
                     outputPrint.data_type = MMALIB_INT16;
                     MMALIB_DEBUGPRINTFN(1, "static int8_t staticRefOutput[] =\n");
                  }
#endif
#if MMALIB_DEBUGPRINT
                  MMALIB_bufParams2D_t dst_addrDebug;
                  dst_addrDebug.dim_x    = validColsOut;
                  dst_addrDebug.dim_y    = numOutChannels * (iterN)*numGroupsPerKernel;
                  dst_addrDebug.stride_y = pitchC;
                  if (numBytes == 1)
                     dst_addrDebug.data_type = MMALIB_INT8;
                  else if (numBytes == 2)
                     dst_addrDebug.data_type = MMALIB_INT16;
                  MMALIB_DEBUGPRINTFN(1, "The computed output is: %s", "\n");
                  MMALIB_debugPrintMatrix ((char *) dst, &dst_addrDebug);

                  if(prm[tpi].staticOut != NULL){
                     MMALIB_DEBUGPRINTFN(1, "The static output (golden) reference is: %s", "\n");
                     dst_addrDebug.stride_y = validColsOut;
                     MMALIB_debugPrintMatrix ((char *) prm[tpi].staticOut, &dst_addrDebug);
                  }
                  MMALIB_DEBUGPRINTFN(1, "The natural C output is: %s", "\n");
                  MMALIB_debugPrintMatrix ((char *) dst_cn, &dst_addrDebug);
#endif // MMALIB_DEBUGPRINT
         } else {
            /* Display the error printout for this test vector before moving on to the next
             * test vector */
            TI_profile_skip_test (desc);
         }

         // free buffers for each test vector
#if !defined(PERFORMANCE_TEST) && !defined(COMPILER_TEST)
         if (prm[tpi].outputDataLocation == MMALIB_TEST_OUTPUT_HEAP) {
            free (dst_cn);
         } else {}
#endif

         if (prm[tpi].outputDataLocation == MMALIB_TEST_OUTPUT_HEAP) {
            /* only malloc'd data can be freed */
            TI_align_free (dst);
         }
         else {
         }

         // src0_nat is now in self-managed DDR buffer, so does not need to be freed
         //TI_align_free (src0_nat);
         TI_align_free (src0_reordered);

#if defined (__SE_FEAT_SPARSITY)
         if (packetizeMode) {
            TI_align_free (src0_depac);
         } else {}
#endif

         TI_align_free(src1);

         if (src2 != NULL) {
            TI_align_free (src2);
         }
         TI_align_free (src3);
         TI_align_free (src4);
      } // end repCount

      free (handle);
   }

   return overall_fail;
}

int test_main (uint32_t *pProfile)
{
   if (TI_cache_init ()) {
      TI_memError ("MMALIB_CNN_convolveBias_row_ixX_ixX_oxX");
      return 1;
   } else {
      return MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_d (&pProfile[0], 0);
   }
}

/* Main call for individual test projects */

int main()
{
   int fail = 1;

   uint32_t profile[1024 * 3];

   MMALIB_TEST_init();

   fail = test_main(&profile[0]);

#if !defined(NO_PRINTF) //&& !defined(EVM_TEST)
   if (fail == 0)
      printf ("Test Pass!\n");
   else
      printf ("Test Fail!\n");

   int i;
   for (i = 0; i < test_cases; i++) {
      printf ("Test %4d: Cold Cycles = %8d, Warm Cycles = %8d, Warm Cycles WRB = %8d\n",
            i,
            profile[3 * i],
            profile[3 * i + 1],
            profile[3 * i + 2]);
   }
#endif

   return fail;
}
/* ======================================================================== */
/*  End of file:  MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_d.c                   */
/* ======================================================================== */
