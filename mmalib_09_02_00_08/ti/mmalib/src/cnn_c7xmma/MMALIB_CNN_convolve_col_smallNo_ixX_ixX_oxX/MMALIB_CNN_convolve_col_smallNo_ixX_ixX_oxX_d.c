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

/********************************************************************************
 *  @file     MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_d.c
 *
 *  @brief    File to demnostrate usage of kernel
 *
 *  @version  0.1 - Feb 2018 : Initial Version
 *
 ********************************************************************************/

// include MMALIB
#include <mmalib.h>

// include test infrastructure provided by MMALIB
#include "../../test/MMALIB_test.h"

// include test data for this kernel
#include "MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_idat.h"

#define FALSE 0
#define TRUE  1
   #define FORCE_EVEN_GROUPS TRUE

__attribute__((section(".msmcData"), aligned(64))) int8_t msmcBuffer[MMALIB_L3_RESULTS_BUFFER_SIZE];

__attribute__((section(".ddrData"),  aligned(64))) int8_t ddrBuffer[2048 * 1024];

int16_t volatile volatileSum = 0; // use volatile to keep compiler from removing this operation, move to global to prevent compiler warning of unused variable

// function for creating filter coefficients with random values for randomly sized test cases
void MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_generateRandomKernels(void **pKernels,
                                                                       const MMALIB_bufParams2D_t * pKernels_addr,
                                                                       const MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_InitArgs *pKerInitArgs)
{
   int32_t no, c, grp;
   int32_t Ni = pKerInitArgs->Ni;
   int32_t No = pKerInitArgs->No;
   int32_t Fr = pKerInitArgs->Fr;
   int32_t Fc = pKerInitArgs->Fc;
   int32_t numGroupsPerKernel = pKerInitArgs->numGroupsPerKernel;
   int32_t numBiasVals =  pKerInitArgs->numBiasVals;
   int32_t elementSize = MMALIB_sizeof(pKernels_addr->data_type);
   
   // make a set of random kernels stored in the normal order, including bias
   int32_t kernelRowElements = Ni*Fr*Fc + numBiasVals;
   *pKernels = TI_memalign(MMALIB_ALIGN_128BYTES, numGroupsPerKernel * No * pKernels_addr->stride_y);
 
   void *kernels = *pKernels;
   if(elementSize == 1){
      int8_t   *ptr = (int8_t *)kernels;
      for(grp = 0; grp < numGroupsPerKernel; grp++){
         for(no = 0; no < No; no++){
            for(c = 0; c < kernelRowElements; c++){
               ptr[c] = rand();
            }
            ptr += pKernels_addr->stride_y;
         }
      }
   } else if (elementSize == 2){
      int16_t *ptr = (int16_t *)kernels;
      for(grp = 0; grp < numGroupsPerKernel; grp++){
         for(no = 0; no < No; no++){
            for(c = 0; c < kernelRowElements; c++){
               ptr[c] = (rand() << 1) | (rand() & 0x00000001);
            }
            ptr += pKernels_addr->stride_y / elementSize;
         }
      }
   }
   
#if MMALIB_DEBUGPRINT
   MMALIB_PRINTF("Natural order kernels:%s", "\n");
   if(elementSize == 1){
      int8_t *debugPtr = (int8_t *)kernels;
      for(no = 0; no < No; no++){
         for(c = 0; c < kernelRowElements; c++){
            MMALIB_PRINTF("%d, ", debugPtr[no*kernelRowElements + c]);
         }
         MMALIB_PRINTF("%s", "\n");
      }
   } else if (elementSize == 2){
      int16_t *debugPtr = (int16_t *)kernels;
      for(no = 0; no < No; no++){
         for(c = 0; c < kernelRowElements; c++){
            MMALIB_PRINTF("%d, ", debugPtr[no*kernelRowElements + c]);
         }
         MMALIB_PRINTF("%s", "\n");
      }
   }
#endif
   
}


int MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   int32_t  tpi;       /* test parameter index */
   int32_t  currentTestFail;
   int32_t  fail = 0;  /* fail flag */
   int32_t  repCount;  /* number of times to repeat a test; useful for random data and random sizes (set to 1 for static) */
   int32_t numReps;
   int32_t testNum;
   
   // variables for calculating performance estimates
   uint64_t archCycles, estCycles;
   uint32_t numPts = 0;
   
   /* Test Parameters */
   MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_testParams_t *prm;
   MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_testParams_t currPrm;
   MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_getTestParams(&prm, &test_cases);
   
   MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_InitArgs    kerInitArgs;
   MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_ExecInArgs  kerExecInArgs;
   MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_ExecOutArgs kerExecOutArgs_ci;
   MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_ExecOutArgs kerExecOutArgs_cn;

   /* Initialize profiling */
   TI_profile_init("MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX");
   
   /* Run each test case */
   for ( tpi = 0; tpi < test_cases; tpi++ ) {
      /* Number of repetitions for the test */
      numReps = prm[tpi].numReps;
      testNum = prm[tpi].testID;
      currPrm = prm[tpi];
      
      int32_t handleSize = MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_getHandleSize(&kerInitArgs);
      MMALIB_kernelHandle handle = (MMALIB_kernelHandle)malloc(handleSize);

      for(repCount = 0; repCount < numReps; repCount++){
         /* Initialize status flags */
         int32_t status_nat_vs_opt = TI_TEST_KERNEL_FAIL;        /* Test status : Natural c vs. Optimized */
         int32_t status_ref_vs_opt = TI_TEST_KERNEL_FAIL;        /* Test status : Optimized c vs. Static Reference */   
         MMALIB_STATUS currTestStatus = MMALIB_STATUS_NOT_SET;  /* Test current testcase status*/
         currentTestFail = 0;
    
         // dynamic range testing
         int64_t dynamicRangeMax_nat = 0;
         int64_t dynamicRangeMin_nat = 0;
         int64_t dynamicRangeMax_opt = 0;
         int64_t dynamicRangeMin_opt = 0;
         
         /* Sometimes, depending on certain compile flags, the test will be
          * marked PASS even if no comparison is done. This flag is to detect
          * if a comparison was done or not                                   */
         int32_t comparisonDone = 0;
         /* Special handling for random dimensions */
         if(prm[tpi].testPattern == RANDOM_DIMENSION){
            // set data type to random data
            currPrm.testPattern = RANDOM_SIGNED;
            currPrm.numGroupsPerKernel  = TI_randomDimensionLessThan(prm[tpi].numGroupsPerKernel);

            if(prm[tpi].strideX == 1 ){
               // value in 3, 5, 7(8-bit only)
               currPrm.kernelWidth  = 2*TI_randomDimensionLessThan((prm[tpi].dataTypeB == MMALIB_INT8) ? 4 : 3) + 1;
               currPrm.kernelHeight = currPrm.kernelWidth;
               currPrm.dilationX = TI_randomDimensionLessThan(prm[tpi].dilationX + 1);
               currPrm.dilationY = currPrm.dilationX;  // enforce dilationX == dilationY
               currPrm.Lr = TI_randomDimensionLessThan(prm[tpi].Lr) + currPrm.dilationX*(currPrm.kernelHeight-1) + 1; // offset to ensure output exists
               currPrm.Lc = TI_randomDimensionLessThan(prm[tpi].Lc) + currPrm.dilationX*(currPrm.kernelWidth-1) + 1;  // offset to ensure output exists
            } else if(prm[tpi].strideX == 2) {
               currPrm.kernelWidth  = 3;
               currPrm.kernelHeight = 3;
               if (currPrm.numGroupsPerKernel > 1) {
                  currPrm.Lr = ((TI_randomDimensionLessThan(prm[tpi].Lr) + currPrm.kernelHeight) >> 1) << 1; // offset to ensure output exists, must be even
               } else {
                  currPrm.Lr = TI_randomDimensionLessThan(prm[tpi].Lr) + currPrm.kernelHeight; // offset to ensure output exists, not required to be even if there's only one group (?)
               }
               currPrm.Lc = TI_randomDimensionLessThan(prm[tpi].Lc) + currPrm.kernelWidth;  // offset to ensure output exists
            }
         
            currPrm.numInChannels = TI_randomDimensionLessThan((MMALIB_MMA_SIZE_8_BIT/MMALIB_sizeof(prm[tpi].dataTypeA)) / (currPrm.kernelWidth*currPrm.kernelHeight) + 1);
            currPrm.numOutChannels = currPrm.numInChannels;  // enforce Ni==No
            currPrm.qShift = TI_randomDimensionLessThan(prm[tpi].qShift);
            currPrm.numBiasVals = TI_randomDimensionLessThan(MMALIB_MMA_SIZE_8_BIT/MMALIB_sizeof(prm[tpi].dataTypeA) - currPrm.numInChannels*currPrm.kernelWidth*currPrm.kernelHeight);
            currPrm.biasB = TI_randomDimensionLessThan(prm[tpi].biasB);
         }
        
         uint32_t inp0Size, inp1Size, outSize;
         // HACK ALERT - setting this parameter allows testing for non-continguous groups for all devices except C7100, which requires contiguous groups; hack here because it is not implemented in the idat yet
         // assumed to represent a number of extra rows in the memory storage of the input feature map
         int32_t gapRowsBetweenGroups   = 0;

         kerInitArgs.Ni                  = currPrm.numInChannels;
         kerInitArgs.No                  = currPrm.numOutChannels;
         kerInitArgs.Fr                  = currPrm.kernelWidth;
         kerInitArgs.Fc                  = currPrm.kernelHeight;
         kerInitArgs.strideX             = currPrm.strideX;
         kerInitArgs.strideY             = currPrm.strideY;
         kerInitArgs.dilationX           = currPrm.dilationX;
         kerInitArgs.dilationY           = currPrm.dilationY;
         int32_t eFc                     = kerInitArgs.dilationX*(kerInitArgs.Fc-1) + 1;  // equivalent filter width for dilation
         int32_t eFr                     = kerInitArgs.dilationY*(kerInitArgs.Fr-1) + 1;  // equivalent filter height for dilation
         uint8_t dataTypeA               = currPrm.dataTypeA;
         uint8_t dataTypeB               = currPrm.dataTypeB;
         uint8_t dataTypeC               = currPrm.dataTypeC;
         kerInitArgs.activationType      = currPrm.activationType;
         kerInitArgs.shift               = currPrm.qShift;
         kerInitArgs.shiftMethod         = currPrm.shiftMethod;
         kerInitArgs.bias                = currPrm.biasB;
         kerInitArgs.biasDataType        = dataTypeB;
         kerInitArgs.numBiasVals         = currPrm.numBiasVals;
         kerInitArgs.featureWidth        = currPrm.Lc;
         kerInitArgs.blockFeatureHeight  = currPrm.Lr;

         kerInitArgs.blockFeaturePitch   = kerInitArgs.featureWidth * MMALIB_sizeof(dataTypeB) * kerInitArgs.Ni;
         kerInitArgs.columnOffset        = MMALIB_MMA_SIZE_8_BIT / MMALIB_sizeof(dataTypeB) * 2 * kerInitArgs.strideX; // should make this an idat.c parameter
         kerInitArgs.inPairOffset        = MMALIB_MMA_SIZE_8_BIT / MMALIB_sizeof(dataTypeB) * kerInitArgs.strideX;     // should make this an idat.c parameter
         kerInitArgs.inChOffset          = kerInitArgs.featureWidth; // temporary until it's implemented as an idat.c parameter
         kerInitArgs.outPairOffset       = MMALIB_MMA_SIZE_8_BIT / MMALIB_sizeof(dataTypeC);
         kerInitArgs.numGroupsPerKernel  = currPrm.numGroupsPerKernel;
         kerInitArgs.topPad              = currPrm.topPad;
         kerInitArgs.bottomPad           = currPrm.bottomPad;
         kerInitArgs.leftPad             = currPrm.leftPad;
         kerInitArgs.rightPad            = currPrm.rightPad;
         int32_t Mr                      = MMALIB_ceilingDiv(currPrm.Lr + kerInitArgs.topPad + kerInitArgs.bottomPad - eFr + 1, currPrm.strideY);
         int32_t Mc                      = MMALIB_ceilingDiv(currPrm.Lc + kerInitArgs.leftPad + kerInitArgs.rightPad - eFc + 1, currPrm.strideX);
         // HACK ALERT - change this value here to test for non-continguous groups
         gapRowsBetweenGroups   = 0;
         // groupOffset is a public parameter when in-kernel padding is provided because the dimension exists in the SE programming. 
         kerInitArgs.groupOffset         = (kerInitArgs.blockFeatureHeight + gapRowsBetweenGroups) * kerInitArgs.blockFeaturePitch / MMALIB_sizeof(dataTypeB);
         kerExecInArgs.padFillValue      = currPrm.padFillValue;
         kerInitArgs.pSatMin             = currPrm.pSatMin;
         kerInitArgs.pSatMax             = currPrm.pSatMax;
         kerExecInArgs.enableDynamicRange = currPrm.enableDynamicRange;
         kerExecInArgs.initDynamicRange = currPrm.initDynamicRange;
         
         MMALIB_bufParams2D_t weights_addr;             // filter coefficients in packed, natural order
         MMALIB_bufParams2D_t bias_addr;                // bias values in packed, natural order
         
         MMALIB_bufParams2D_t src0_addr;                // filter coefficients
         MMALIB_bufParams2D_t src1_addr, src1_lddr;     // input feature maps

         MMALIB_bufParams3D_t dst_addr;                 // output feature maps, expand to 3D to support groups
         
         /* Compute data sizes */
         MMALIB_bufParams2D_t *pBias_addr;
         if(currPrm.staticBias == NULL){
            // bias is merged with weights
            // kernel filter weights
            weights_addr.dim_x     = kerInitArgs.Ni * kerInitArgs.Fc * kerInitArgs.Fr + kerInitArgs.numBiasVals;
            weights_addr.dim_y     = kerInitArgs.No * kerInitArgs.numGroupsPerKernel;
            weights_addr.data_type = dataTypeA;
            weights_addr.stride_y  = weights_addr.dim_x*MMALIB_sizeof(weights_addr.data_type);
            pBias_addr = NULL;
         } else {
            // kernel filter weights
            weights_addr.dim_x     = kerInitArgs.Ni * kerInitArgs.Fc * kerInitArgs.Fr;
            weights_addr.dim_y     = kerInitArgs.No * kerInitArgs.numGroupsPerKernel;
            weights_addr.data_type = dataTypeA;
            weights_addr.stride_y  = weights_addr.dim_x*MMALIB_sizeof(weights_addr.data_type);
            
            bias_addr.dim_x        = kerInitArgs.numBiasVals;
            bias_addr.dim_y        = kerInitArgs.No * kerInitArgs.numGroupsPerKernel;
            bias_addr.data_type    = dataTypeA;
            bias_addr.stride_y     = bias_addr.dim_x*MMALIB_sizeof(bias_addr.data_type);
            
            pBias_addr = &bias_addr;
         }
         
         
         // calculate the buffer parameters for the reordered weights
         MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_reorderWeights_Args reorderWeightsArgs;
         reorderWeightsArgs.dataType = dataTypeA;
         reorderWeightsArgs.Ni = kerInitArgs.Ni;
         reorderWeightsArgs.No = kerInitArgs.No;
         reorderWeightsArgs.Fr = kerInitArgs.Fr;
         reorderWeightsArgs.Fc = kerInitArgs.Fc;
         reorderWeightsArgs.strideX = kerInitArgs.strideX;
         reorderWeightsArgs.strideY = kerInitArgs.strideY;
         reorderWeightsArgs.dilationX = kerInitArgs.dilationX;
         reorderWeightsArgs.featureWidth = kerInitArgs.featureWidth;
         reorderWeightsArgs.numBiasVals = kerInitArgs.numBiasVals;
#if FORCE_EVEN_GROUPS == FALSE  
         reorderWeightsArgs.numGroupsPerKernel = kerInitArgs.numGroupsPerKernel;
#else     
         if (currPrm.numGroupsPerKernel == 1){
            // we don't need an even number of groups when there is 1 group
            reorderWeightsArgs.numGroupsPerKernel = kerInitArgs.numGroupsPerKernel;
         } else {
            // "hack" to make sure we allocate memory for an even number of groups
            if (currPrm.numGroupsPerKernel > 1){
               reorderWeightsArgs.numGroupsPerKernel  = MMALIB_ceilingDiv(currPrm.numGroupsPerKernel, 2) << 1;
            } else {
               reorderWeightsArgs.numGroupsPerKernel  = currPrm.numGroupsPerKernel;
            }
         }
#endif
  
         reorderWeightsArgs.topPad    = kerInitArgs.topPad;
         reorderWeightsArgs.bottomPad = kerInitArgs.bottomPad;
         reorderWeightsArgs.leftPad   = kerInitArgs.leftPad;
         reorderWeightsArgs.rightPad  = kerInitArgs.rightPad;
         // calculate the memory size for allocation
         inp0Size = MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_reorderWeights_getMemorySize(&reorderWeightsArgs);
         // fill the bufParams for src0
         MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_reorderWeights_fillBufParams(&reorderWeightsArgs, &src0_addr);
         
#if MMALIB_DEBUGPRINT
         MMALIB_PRINTF("weights_addr: %s", "\n");
         MMALIB_debugPrintBufParams(&weights_addr);
         
         if(pBias_addr != NULL){
            MMALIB_PRINTF("bias_addr: %s", "\n");
            MMALIB_debugPrintBufParams(&bias_addr);
         }
         
         MMALIB_PRINTF("src0_addr: %s", "\n");
         MMALIB_debugPrintBufParams(&src0_addr);
#endif
         
         // currently assuming the feature map as 2D
         src1_addr.dim_x                = kerInitArgs.Ni*currPrm.Lc;  // input feature map
         int32_t LrWithGapRows          = currPrm.Lr + gapRowsBetweenGroups;
         src1_addr.dim_y                = LrWithGapRows*kerInitArgs.numGroupsPerKernel;
#if FORCE_EVEN_GROUPS == FALSE 
         src1_addr.dim_y                = LrWithGapRows*kerInitArgs.numGroupsPerKernel;
#else
         if (currPrm.numGroupsPerKernel == 1){
            src1_addr.dim_y                = LrWithGapRows*kerInitArgs.numGroupsPerKernel;
         } else {
            // make sure the numGroupsPerKernel is even, 
            src1_addr.dim_y                = LrWithGapRows*2*MMALIB_ceilingDiv(kerInitArgs.numGroupsPerKernel,2);
         }
#endif        
         src1_addr.data_type            = dataTypeB;
         src1_addr.stride_y             = kerInitArgs.blockFeaturePitch;
         // make sure the numGroupsPerKernel is even, but for loading source data we need the actual data size.
         src1_lddr = src1_addr;
         src1_lddr.dim_y                = currPrm.Lr*kerInitArgs.numGroupsPerKernel;
        
         
         dst_addr.dim_x                 = Mc;
         dst_addr.dim_y                 = kerInitArgs.No *  Mr;
         dst_addr.dim_z                 = kerInitArgs.numGroupsPerKernel;
         dst_addr.data_type             = dataTypeC;
#if defined(MINIMUM_OUTPUT_SPACE)
         dst_addr.stride_y              = Mc * MMALIB_sizeof(dataTypeC);  // pack output rows for minimum storage space
#else
         dst_addr.stride_y              = MMALIB_CALC_STRIDE(Mc * MMALIB_sizeof(dataTypeC), MMALIB_ALIGN_SHIFT_64BYTES);  // align every row to 64 bytes;
#endif
         dst_addr.stride_z              = kerInitArgs.No * dst_addr.stride_y * Mr;
         outSize                        = dst_addr.dim_z * dst_addr.stride_z;
         
         /* Allocate buffers for each test vector */
         // this works-around Valgrind warnings for predicated SA-based loads.  This should eventually be fixed in compiler/HE models.
         inp0Size                     =  MMALIB_CALC_STRIDE(inp0Size+64, MMALIB_ALIGN_SHIFT_64BYTES);
         void *src0                   = TI_memalign(MMALIB_ALIGN_128BYTES, inp0Size);  // weights
         

         // when pad is an option, the alignment of the "imaginary" starting point including zeros inserted by SE must be 64-byte aligned with 128-byte alignment preferred
         int32_t featureMapPadOffset    = MMALIB_sizeof(src1_addr.data_type) * (kerInitArgs.leftPad + kerInitArgs.topPad*src1_addr.dim_x);
         inp1Size                       = src1_addr.dim_y*src1_addr.stride_y + featureMapPadOffset;
         // make inp1Size a multiple of 64 bytes
         inp1Size                       = MMALIB_ceilingDiv(inp1Size, 64) * 64;
         void *src1                     = TI_memalign(MMALIB_ALIGN_128BYTES, inp1Size);
         // move pointer to the actual data leaving the imaginary starting point aligned to 128-bytes
         void *pSrc1 = src1 + featureMapPadOffset;
         
         uint8_t *shiftValues;
         if (currPrm.staticShiftValues != NULL) {
            shiftValues = (uint8_t *)TI_memalign(MMALIB_ALIGN_128BYTES, kerInitArgs.numGroupsPerKernel);
            int32_t i;
            for(i = 0; i < kerInitArgs.numGroupsPerKernel; i++){
               shiftValues[i] = currPrm.staticShiftValues[i];
            }
            
            // this is a cheap hack until the driver file supports per group shift
            // this is to allow the dynamic range comparison logic to work
            currPrm.qShift = shiftValues[0];

         } else {
            shiftValues = NULL;
         }
         
         int32_t *biasBValues = NULL;
         if (currPrm.staticBiasBValues != NULL) {
            biasBValues = (int32_t *)TI_memalign(MMALIB_ALIGN_128BYTES, kerInitArgs.numGroupsPerKernel*sizeof(int32_t));
            int32_t i;
            for(i = 0; i < kerInitArgs.numGroupsPerKernel; i++){
               biasBValues[i] = currPrm.staticBiasBValues[i];
            }
         } else {
            biasBValues = NULL;
         }
         
         void * dst;
      
         if (currPrm.outputDataLocation == MMALIB_TEST_OUTPUT_HEAP){
            dst                        = TI_memalign(MMALIB_ALIGN_128BYTES, outSize);
         } 
         else if(currPrm.outputDataLocation == MMALIB_TEST_OUTPUT_MSMC) {
            dst                        = (void *)msmcBuffer;
         } else { 
            dst = NULL;
         } 

         uint32_t i;
         // initialize all kernel coefficient memory even if it won't be used to address valgrind warnings
         for(i = 0; i < inp0Size; i++){
            ((uint8_t *)src0)[i] = 5;
         }
         for(i = 0; i < inp1Size; i++){
            ((uint8_t *)src1)[i] = 6;
         }
         
#if MMALIB_DEBUGPRINT
         //uint32_t i;
         for(i = 0; i < outSize; i++){
            ((uint8_t *)dst)[i] = 7;
         }

#if 0         
         MMALIB_PRINTF("dst before computation%s", "\n");
         for(i = 0; i < outSize; i++){
            if ((i+1) % 64 > 0) {
               MMALIB_PRINTF("%4u ", ((uint8_t *)dst)[i]);
            } else {
               MMALIB_PRINTF("%4u\n", ((uint8_t *)dst)[i]);
            }
         }
#endif
#endif
         
         
         void *dst_cn = NULL;
         if (currPrm.outputDataLocation == MMALIB_TEST_OUTPUT_HEAP){
            dst_cn = TI_memalign(MMALIB_ALIGN_128BYTES, outSize);
         } else {
            dst_cn = (void *) ddrBuffer;
         }
         
#if MMALIB_DEBUGPRINT
         MMALIB_PRINTF("src0_addr: %s", "\n");
         MMALIB_debugPrintBufParams(&src0_addr);
         
         MMALIB_PRINTF("src1_addr: %s", "\n");
         MMALIB_debugPrintBufParams(&src1_addr);
         
         MMALIB_PRINTF("dst_addr: %s", "\n");
         MMALIB_debugPrintBufParams3D(&dst_addr);
         
         MMALIB_PRINTF("inp0Size: %d, inp1Size: %d, outSize: %d, totalSize(KB): %d\n", inp0Size, inp1Size, outSize, (inp0Size + inp1Size + 2*outSize)/1024 );
         MMALIB_PRINTF("MMALIB_DEBUGPRINT test_case 0x%d  src1: 0x%p  pSrc1: %p  src0 %p dst 0x%p\n", tpi, src1, pSrc1, src0, dst);
         MMALIB_PRINTF("MMALIB_DEBUGPRINT dst_cn = 0x%p\n", dst_cn);
#endif
         
         /* Only run the test if the buffer allocations fit in the heap */
       if
            (src0 && src1 && dst && dst_cn)
         {
            /* copy static test data into memory, or generate random test data */
            // copy/generate the filter coefficients
            reorderWeightsArgs.blockFeatureHeight = kerInitArgs.blockFeatureHeight;

            if( currPrm.staticKernel != NULL ) {
               
#if MMALIB_DEBUGPRINT
               MMALIB_PRINTF("Static Weights%s", "\n");
               MMALIB_debugPrintBufParams(&weights_addr);
               MMALIB_debugPrintMatrix(currPrm.staticKernel, &weights_addr);
#endif
               if(tpi % 2 == 0){  // test various use cases for reorderWeights
                  MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_reorderWeights_exec(REORDER_WEIGHTS_AND_BIAS, &reorderWeightsArgs, &weights_addr, currPrm.staticKernel, pBias_addr, currPrm.staticBias, &src0_addr, src0);
               } else {
                  MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_reorderWeights_exec(REORDER_WEIGHTS, &reorderWeightsArgs, &weights_addr, currPrm.staticKernel, pBias_addr, currPrm.staticBias, &src0_addr, src0);
                  MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_reorderWeights_exec(REORDER_BIAS, &reorderWeightsArgs, &weights_addr, currPrm.staticKernel, pBias_addr, currPrm.staticBias, &src0_addr, src0);
               }
               MMALIB_DEBUGPRINTFN(1, "currPrm.staticBias = %p\n", currPrm.staticBias);
               
#if MMALIB_DEBUGPRINT
               MMALIB_PRINTF("Reordered kernel coefficients%s", "\n");
               MMALIB_debugPrintBufParams(&src0_addr);
               MMALIB_debugPrintMatrix(src0, &src0_addr);
#endif
               
            } else {
               // generate a random set of normal order filter coefficients
               void *normalOrderKernels;
               MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_generateRandomKernels(&normalOrderKernels, &weights_addr, &kerInitArgs);
               
               // random data for kernel coefficients and bias
               MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_reorderWeights_exec(REORDER_WEIGHTS_AND_BIAS, &reorderWeightsArgs, &weights_addr, normalOrderKernels, NULL, NULL, &src0_addr, src0);
               
               // free the memory for the normal order kernels
               free(normalOrderKernels);
            }
            
            // copy/generate the input feature maps
            // in devices with on the fly padding, we support a 3D arrangement of the input feature map data (i.e. a dimension is assigned to groups)
            // src1_lddr describes data as stored in program memory, which is a 2D buffer
            // TI_fillBuffer3D applies stride on the output and we need a stride_z, so we have to kind of hack it...
            TI_fillBuffer3D(currPrm.testPattern, 
                           (uint8_t) 255, 
                           pSrc1, currPrm.staticIn,
                           src1_lddr.dim_x, currPrm.Lr, kerInitArgs.numGroupsPerKernel,
                           kerInitArgs.blockFeaturePitch, kerInitArgs.groupOffset*MMALIB_sizeof(src1_addr.data_type),
                           MMALIB_sizeof(src1_lddr.data_type), testPatternString);
 
            /**********************************************************************************************************
             *
             * optimized C
             *
             *********************************************************************************************************/
            // initialize the kernel
            TI_profile_start(TI_PROFILE_KERNEL_INIT);
            MMALIB_asm(" MARK 0");
            kerInitArgs.funcStyle = MMALIB_FUNCTION_OPTIMIZED;
            //               MMALIB_DEBUGPRINT_STACK_PTR;
            currTestStatus = MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_init(handle,
                                                                               &src0_addr,
                                                                               &src1_addr,
                                                                               &dst_addr,
                                                                               &kerInitArgs);
            
            MMALIB_DEBUGPRINTFN(1, "currTestStatus = %d\n", currTestStatus);

            MMALIB_asm(" MARK 1");
            TI_profile_stop();
            if(currTestStatus == MMALIB_SUCCESS){
               /* Test optimized kernel */
               MMALIB_DEBUGPRINTFN(1,"Starting optimized-C computation.  currTestStatus=%d.\n", currTestStatus);
               kerExecInArgs.blockFeatureWidth = src1_addr.dim_x;
               TI_profile_start(TI_PROFILE_KERNEL_OPT);
               MMALIB_asm(" MARK 2");
               //               MMALIB_DEBUGPRINT_STACK_PTR;
               currTestStatus = MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_exec(handle, src0, pSrc1, shiftValues, biasBValues, dst,
                                                                                  &kerExecInArgs,
                                                                                     &kerExecOutArgs_ci);
               MMALIB_asm(" MARK 3");
               TI_profile_stop();
               
                  dynamicRangeMin_opt = kerExecOutArgs_ci.dynamicRangeMin;
                  dynamicRangeMax_opt = kerExecOutArgs_ci.dynamicRangeMax;
               
            if(currTestStatus == MMALIB_SUCCESS)
            {
               // call init again to get first-call cycles
               currTestStatus = MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_init(handle,
                                                                                  &src0_addr,
                                                                                  &src1_addr,
                                                                                  &dst_addr,
                                                                                  &kerInitArgs);
               
               // run warm instruction cache test
               TI_profile_start(TI_PROFILE_KERNEL_OPT_WARM);
               MMALIB_asm(" MARK 4");
               currTestStatus = MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_exec(handle,
                                                                                  src0,
                                                                                  pSrc1,
                                                                                  shiftValues,
                                                                                  biasBValues,
                                                                                  dst,
                                                                                  &kerExecInArgs,
                                                                                  &kerExecOutArgs_ci);
               MMALIB_asm(" MARK 5");
               TI_profile_stop();
               
               // get output to L1D
               uint32_t row, col;
               int16_t outSum = 0;
               //int16_t volatile volatileSum = 0;  // use volatile to keep compiler from removing this operation
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
               // call init again to get first-call cycles
               if(currTestStatus == MMALIB_SUCCESS) {
                  currTestStatus = MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_init(handle,
                                                                                    &src0_addr,
                                                                                    &src1_addr,
                                                                                    &dst_addr,
                                                                                    &kerInitArgs);
                  
                  // run warm instruction cache test
                  TI_profile_start(TI_PROFILE_KERNEL_OPT_WARMWRB);
                  MMALIB_asm(" MARK 6");
                  currTestStatus = MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_exec(handle,
                                                                                    src0,
                                                                                    pSrc1,
                                                                                    shiftValues,
                                                                                    biasBValues,
                                                                                    dst,
                                                                                    &kerExecInArgs,
                                                                                    &kerExecOutArgs_ci);
                  MMALIB_asm(" MARK 7");
                  TI_profile_stop();
               }
            }
            } else {
               MMALIB_PRINTF("MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_init_checkParams failed!%s", "\n");
            }
          
            
            /**********************************************************************************************************
             *
             * natural C
             *
             *********************************************************************************************************/
            MMALIB_DEBUGPRINTFN(1, "Starting natural C computation.%s", "\n");

            /* Test _cn kernel */
            kerInitArgs.funcStyle = MMALIB_FUNCTION_NATC;
            
            // initialize the kernel to use the natural C variant
            if(currTestStatus == MMALIB_SUCCESS) {
               currTestStatus = MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_init(handle,
                                                                                 &src0_addr,
                                                                                 &src1_addr,
                                                                                 &dst_addr,
                                                                                 &kerInitArgs);
               TI_profile_start(TI_PROFILE_KERNEL_CN);
               MMALIB_asm(" MARK 8");

               if(currTestStatus == MMALIB_SUCCESS){
                  kerExecInArgs.blockFeatureWidth = src1_addr.dim_x;
                  currTestStatus = MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_exec(handle,
                                                                                    src0,
                                                                                    pSrc1,
                                                                                    shiftValues,
                                                                                    biasBValues,
                                                                                    dst_cn,
                                                                                    &kerExecInArgs,
                                                                                    &kerExecOutArgs_cn);
                  dynamicRangeMin_nat = kerExecOutArgs_cn.dynamicRangeMin;
                  dynamicRangeMax_nat = kerExecOutArgs_cn.dynamicRangeMax;
                  
                  MMALIB_asm(" MARK 9");
                  TI_profile_stop();
                  MMALIB_DEBUGPRINTFN(1, "Finished natural C computation.%s", "\n");
            
                  /**********************************************************************************************************
                  *
                  * compare outputs
                  *
                  *********************************************************************************************************/
                  
                  /* Compare natural C Output and Optimized Output */
                  status_nat_vs_opt = TI_compare_mem_3D(dst, dst_cn, 0, 0, dst_addr.dim_x, dst_addr.dim_y, dst_addr.dim_z, dst_addr.stride_y, dst_addr.stride_z, MMALIB_sizeof(dst_addr.data_type));
                  MMALIB_DEBUGPRINTFN(1, "Without dynamic range, status_nat_vs_opt = %d.\n", status_nat_vs_opt);
                  MMALIB_DEBUGPRINTFN(1, "dynamicRangeMin_nat = %ld, dynamicRangeMin_opt = %ld\n", dynamicRangeMin_nat, dynamicRangeMin_opt);
                  MMALIB_DEBUGPRINTFN(1, "dynamicRangeMax_nat = %ld, dynamicRangeMax_opt = %ld\n", dynamicRangeMax_nat, dynamicRangeMax_opt);
                  if(currPrm.enableDynamicRange == MMALIB_DYNAMIC_RANGE_ENABLE){
                     if(dynamicRangeMin_nat != dynamicRangeMin_opt){
                        status_nat_vs_opt = TI_TEST_KERNEL_FAIL;
                        MMALIB_DEBUGPRINTFN(1, "Dynamic range minimum error.  dynamicRangeMin_nat = %ld, dynamicRangeMin_opt = %ld\n", dynamicRangeMin_nat, dynamicRangeMin_opt);
                     }
                     if(dynamicRangeMax_nat != dynamicRangeMax_opt){
                        status_nat_vs_opt = TI_TEST_KERNEL_FAIL;
                        MMALIB_DEBUGPRINTFN(1, "Dynamic range maximum error.  dynamicRangeMax_nat = %ld, dynamicRangeMax_opt = %ld\n", dynamicRangeMax_nat, dynamicRangeMax_opt);
                  }
               }
                  comparisonDone = 1;
               } else {
                  MMALIB_PRINTF("MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_init_checkParams failed!%s", "\n");
               }
            } else {
               if(currPrm.expectedStatusCode == currTestStatus) {
                  status_nat_vs_opt = TI_TEST_KERNEL_PASS;
               }
            }
            
            if( currPrm.staticOut != NULL && currTestStatus == MMALIB_SUCCESS) {
               /* Compare natural C output and static output */
#if MMALIB_DEBUGPRINT && !defined(PERFORMANCE_TEST)
               int32_t status_nat_vs_ref = TI_compare_mem_roi3DStatic(dst_cn, currPrm.staticOut, 0, 0, dst_addr.dim_x, dst_addr.dim_y, dst_addr.dim_z, dst_addr.stride_y, dst_addr.stride_z, MMALIB_sizeof(dst_addr.data_type));
               MMALIB_PRINTF(" status_nat_vs_ref: %d\n", status_nat_vs_ref);
               if(currPrm.enableDynamicRange == MMALIB_DYNAMIC_RANGE_ENABLE){
                  if( currPrm.dynamicRangeMin != NULL ){
                     //if(MMALIB_TEST_compareDynamicRange(currPrm.dynamicRangeMin, dynamicRangeMin_nat, currPrm.dataTypeC, currPrm.qShift, currPrm.activationType)){
                     if(*((int64_t *)(currPrm.dynamicRangeMin)) != dynamicRangeMin_opt){
                        status_nat_vs_ref = TI_TEST_KERNEL_FAIL;
                        MMALIB_DEBUGPRINTFN(1, "Dynamic range minimum error.  dynamicRangeMin_nat = %ld, currPrm.dynamicRangeMin = %ld\n", dynamicRangeMin_nat, *((int64_t *)(currPrm.dynamicRangeMin)));
                        //                     if(dynamicRangeMin_nat != *((int64_t *)(currPrm.dynamicRangeMin))){
                        //                        status_nat_vs_ref = TI_TEST_KERNEL_FAIL;
                        //                        MMALIB_DEBUGPRINTFN(1, "Dynamic range minimum error.  dynamicRangeMin_nat = %ld, currPrm.dynamicRangeMin = %ld\n", dynamicRangeMin_nat, *((int64_t *)(currPrm.dynamicRangeMin)));
                     }
                  }
                  if( currPrm.dynamicRangeMax != NULL ){
                     //if(MMALIB_TEST_compareDynamicRange(currPrm.dynamicRangeMax, dynamicRangeMax_nat, currPrm.dataTypeC, currPrm.qShift, currPrm.activationType)){
                     if(*((int64_t *)(currPrm.dynamicRangeMax)) != dynamicRangeMax_opt){
                        status_nat_vs_ref = TI_TEST_KERNEL_FAIL;
                        MMALIB_DEBUGPRINTFN(1, "Dynamic range maximum error.  dynamicRangeMax_nat = %ld, currPrm.dynamicRangeMax = %ld\n", dynamicRangeMax_nat, *((int64_t *)(currPrm.dynamicRangeMax)));
                     }
                  }
               }
#endif
               
               /* Compare optimized C output and static output (instead of natural) */
               status_ref_vs_opt = TI_compare_mem_roi3DStatic(dst, currPrm.staticOut, 0, 0, dst_addr.dim_x, dst_addr.dim_y, dst_addr.dim_z, dst_addr.stride_y, dst_addr.stride_z, MMALIB_sizeof(dst_addr.data_type));
               MMALIB_DEBUGPRINTFN(1, "Without dynamic range, status_ref_vs_opt = %d.\n", status_ref_vs_opt);
               if(currPrm.enableDynamicRange == MMALIB_DYNAMIC_RANGE_ENABLE){
                  if( currPrm.dynamicRangeMin != NULL ){
                     //if(MMALIB_TEST_compareDynamicRange(currPrm.dynamicRangeMin, dynamicRangeMin_opt, currPrm.dataTypeC, currPrm.qShift, currPrm.activationType)){
                     if(*((int64_t *)(currPrm.dynamicRangeMin)) != dynamicRangeMin_opt){
                        status_ref_vs_opt = TI_TEST_KERNEL_FAIL;
                        MMALIB_DEBUGPRINTFN(1, "Dynamic range minimum error.  dynamicRangeMin_opt = %ld, currPrm.dynamicRangeMin = %ld\n", dynamicRangeMin_opt, *((int64_t *)(currPrm.dynamicRangeMin)));
                     }
                  }
                  if( currPrm.dynamicRangeMax != NULL ){
                     //if(MMALIB_TEST_compareDynamicRange(currPrm.dynamicRangeMax, dynamicRangeMax_opt, currPrm.dataTypeC, currPrm.qShift, currPrm.activationType)){
                     if(*((int64_t *)(currPrm.dynamicRangeMax)) != dynamicRangeMax_opt){
                        status_ref_vs_opt = TI_TEST_KERNEL_FAIL;
                        MMALIB_DEBUGPRINTFN(1, "Dynamic range maximum error.  dynamicRangeMax_opt = %ld, currPrm.dynamicRangeMax = %ld\n", dynamicRangeMax_opt, *((int64_t *)(currPrm.dynamicRangeMax)));
                     }
                  }
               }
               comparisonDone = 1;
               MMALIB_DEBUGPRINTFN(1, "status_ref_vs_opt = %d.\n", status_ref_vs_opt);
            } else {
               status_ref_vs_opt = TI_TEST_KERNEL_PASS;
            }
            
            /* Set the 'fail' flag based on test vector comparison results */
            currentTestFail = ((status_ref_vs_opt == TI_TEST_KERNEL_FAIL) ||
                               (status_nat_vs_opt == TI_TEST_KERNEL_FAIL) ||
                               (comparisonDone == 0 && currTestStatus == MMALIB_SUCCESS) ||
                               (currTestStatus != currPrm.expectedStatusCode)) ? 1 : 0;

            fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;
#if MMALIB_DEBUGPRINT
            if(currentTestFail == 1){
               MMALIB_PRINTF("Failed%s", "\n");
               MMALIB_PRINTF("   status_ref_vs_opt: %d\n", status_ref_vs_opt);
               MMALIB_PRINTF("   status_nat_vs_opt: %d\n", status_nat_vs_opt);
               MMALIB_PRINTF("   currTestStatus: %d\n", currTestStatus);
               MMALIB_PRINTF("   comparison done: %d\n", comparisonDone);
               
               MMALIB_PRINTF(" Mr = %d, Mc = %d\n", Mr, Mc);
               
               MMALIB_PRINTF("Weights%s", "\n");
               MMALIB_debugPrintBufParams(&src0_addr);
               MMALIB_debugPrintMatrix(src0, &src0_addr);
               
               MMALIB_PRINTF("Input feature maps%s", "\n");
               MMALIB_debugPrintBufParams(&src1_addr);
               MMALIB_debugPrintMatrix(pSrc1, &src1_addr);
               
               if( currPrm.staticOut != NULL ) {
                  MMALIB_bufParams3D_t staticOut_addr = dst_addr;
                  staticOut_addr.stride_y = staticOut_addr.dim_x * MMALIB_sizeof(staticOut_addr.data_type);
                  staticOut_addr.stride_z = staticOut_addr.stride_y * staticOut_addr.dim_y;
                  MMALIB_PRINTF("dst_ref%s", "\n");
                  MMALIB_debugPrintBufParams3D(&staticOut_addr);
                  MMALIB_debugPrintMatrix3D((void *)currPrm.staticOut, &staticOut_addr);
               }
               
               MMALIB_PRINTF("dst_cn%s", "\n");
               MMALIB_debugPrintBufParams3D(&dst_addr);
               MMALIB_debugPrintMatrix3D((void *)dst_cn, &dst_addr);
               MMALIB_PRINTF("dst%s", "\n");
               MMALIB_debugPrintBufParams3D(&dst_addr);
               MMALIB_debugPrintMatrix3D((void *)dst, &dst_addr);
            }

#endif // MMALIB_DEBUGPRINT
            
            pProfile[3*tpi]   = (int32_t)TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
            pProfile[3*tpi+1] = (int32_t)TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[3*tpi+2] = (int32_t)TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);
            
            
            MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_perfEst(handle,
                                                                &src0_addr,
                                                                &src1_addr,
                                                                &dst_addr,
                                                                &kerInitArgs,
                                                                &kerExecInArgs,
                                                                &kerExecOutArgs_ci,
                                                                &archCycles,
                                                                &estCycles);
            
            sprintf(desc, "%s generated input | ByteWidth = %d, Groups=%d, Ni=%d, No=%d, Fr=%d, Fc=%d, Stride=%d, Dilation=%d, Lr=%d, Lc=%d, biasB=%d, numBiasVals=%d, shift=%d, ReLU=%s, MMA Ops=%d",
                    testPatternString,
                    MMALIB_sizeof(dst_addr.data_type),
                    kerInitArgs.numGroupsPerKernel,
                    kerInitArgs.Ni,
                    kerInitArgs.No,
                    kerInitArgs.Fr,
                    kerInitArgs.Fc,
                    kerInitArgs.strideX,
                    kerInitArgs.dilationX,
                    currPrm.Lr,
                    currPrm.Lc,
                    kerInitArgs.bias,
                    kerInitArgs.numBiasVals,
                    kerInitArgs.shift,
                    ((kerInitArgs.activationType==MMALIB_SATURATION) ? "OFF":"ON"),
                    kerInitArgs.Fr*(2*kerInitArgs.numGroupsPerKernel*kerInitArgs.Fr - 5));
            sprintf(desc, "%s, leftPad=%d, rightPad=%d, topPad=%d, bottomPad=%d", desc, kerInitArgs.leftPad, kerInitArgs.rightPad, kerInitArgs.topPad, kerInitArgs.bottomPad);
            TI_profile_add_test(testNum++, numPts, archCycles, estCycles, currentTestFail, desc);
         }
         else {
            /* Display the error printout for this test vector before moving on to the next test vector */
            TI_profile_skip_test(desc);
         }
         
         
         /* Free buffers for each test vector */
         if (currPrm.outputDataLocation == MMALIB_TEST_OUTPUT_HEAP){
            TI_align_free(dst_cn);
         } else {}
         if (currPrm.outputDataLocation == MMALIB_TEST_OUTPUT_HEAP){
            // only malloc'd data can be freed
            TI_align_free(dst);
         }

         TI_align_free(src1);
         TI_align_free(src0);
         if (biasBValues != NULL){
            TI_align_free(biasBValues);
         }
      } // end repetitions
      free(handle);

   } // end idat test cases
   
   return fail;
}


int test_main(uint32_t *pProfile) {
   if( TI_cache_init()) {
      TI_memError("MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX");
      return 1;
   } else
   {
      return  MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_d(pProfile, 0);
   }
}

/* Main call for individual test projects */

int main()
{
   int fail = 1;
   
   uint32_t profile[MMALIB_MAX_TEST_PER_KERNEL*3];

   MMALIB_TEST_init();
   
   fail = test_main(&profile[0]);
   
#if !defined(NO_PRINTF)
   if(fail == 0){
      printf("Test Pass!%s", "\n");
   } else {
      printf("Test Fail!%s", "\n");
   }
   
   int i;
   for(i = 0; i < test_cases; i++){
      printf("Test %4d: Cold Cycles = %8d, Warm Cycles = %8d, Warm Cycles WRB = %8d\n", i, profile[3*i], profile[3*i+1], profile[3*i+2]);
   }
#endif  // NO_PRINTF
   
   return fail;
}


/* ======================================================================== */
/*  End of file:  MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_d.c           */
/* ======================================================================== */
