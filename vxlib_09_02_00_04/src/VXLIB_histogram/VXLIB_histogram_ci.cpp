/******************************************************************************
 * Copyright (C) 2023 Texas Instruments Incorporated - http://www.ti.com/
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

/**********************************************************************************************************************/
/*                                                                                                                    */
/* INCLUDES                                                                                                           */
/*                                                                                                                    */
/**********************************************************************************************************************/

#include "VXLIB_histogram_priv.h"

/**********************************************************************************************************************/
/*                                                                                                                    */
/* DEFINES                                                                                                            */
/*                                                                                                                    */
/**********************************************************************************************************************/
#define SE_PARAM_BASE (0x0000)
#define SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE1_PARAM_OFFSET (SE0_PARAM_OFFSET + VXLIB_SE_PARAM_SIZE)
#define SA0_PARAM_OFFSET (SE1_PARAM_OFFSET + VXLIB_SE_PARAM_SIZE)

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_histogram_init_ci                                                                                            */
/*                                                                                                                    */
/**********************************************************************************************************************/

void VXLIB_histogram_val_init(uint16_t numBins)
{
   ulong8   vSeed;
   uint32_t row_offset;

   row_offset = 0;
   for (int32_t j = 0; j < numBins; j += 2) {
      // Let the seed be 0, here one quanta will be 2, 32-bit elements
      vSeed = (ulong8) (0x0000000000000000);

      // Update the quanta in all N ways
      __lookup_init(__LUT_SET0, __as_ushort32(vSeed), row_offset);

      // Increment the row_offset by 2 as there are 2, 32-bit bins every row in a 64-bit bank
      row_offset += 2;
   }
}

// this method initializes the kernel-specific parameters
// mainly, the streaming engine and streaming address generators
template <uint32_t dTypeIn, uint32_t dTypeOut>
VXLIB_STATUS VXLIB_histogram_init_ci(VXLIB_kernelHandle              handle,
                                     const VXLIB_bufParams2D_t      *bufParamsIn,
                                     const VXLIB_bufParams1D_t      *bufParamsOut,
                                     const VXLIB_histogram_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_histogram_PrivArgs *pKerPrivArgs = (VXLIB_histogram_PrivArgs *) handle;

   // obtain image size and compute number of blocks to process
   size_t   width            = pKerPrivArgs->width;
   size_t   height           = pKerPrivArgs->height;
   size_t   elemCount        = VXLIB_max_simd<dTypeOut>::value;
   size_t   wBlocks          = (width + (elemCount - 1)) / (elemCount);
   size_t   numBlocks        = height * wBlocks;
   uint16_t numBins          = pKerInitArgs->numBins;
   size_t   strideInElements = pKerPrivArgs->strideInElements;
   pKerPrivArgs->numBlocks   = numBlocks;

   // structs to hold SE and SA parameters
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se1Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;
   __SA_VECLEN  SA_VECLEN;

   uint8_t *pBlock = pKerPrivArgs->bufPblock; // address to retrieve to store SE/SA params

   if (VXLIB_HISTOGRAM_I8U_O32U) {
      SE_ELETYPE = c7x::se_eletype<c7x::uchar_vec>::value;
      SE_VECLEN  = c7x::se_veclen<c7x::int_vec>::value;
      SA_VECLEN  = c7x::sa_veclen<c7x::int_vec>::value;

      if (width == strideInElements) {
         se0Params.ELETYPE = SE_ELETYPE;
         se0Params.VECLEN  = SE_VECLEN;
         se0Params.DIMFMT  = __SE_DIMFMT_1D;
         se0Params.PROMOTE = __SE_PROMOTE_4X_ZEROEXT;
         se0Params.ICNT0   = width * height;
      }

      else {
         se0Params.ELETYPE = SE_ELETYPE;
         se0Params.VECLEN  = SE_VECLEN;
         se0Params.DIMFMT  = __SE_DIMFMT_2D;
         se0Params.PROMOTE = __SE_PROMOTE_4X_ZEROEXT;

         se0Params.ICNT0 = width;
         se0Params.ICNT1 = height;
         se0Params.DIM1  = strideInElements;
      }

      sa0Params.VECLEN = SA_VECLEN;
      sa0Params.DIMFMT = __SA_DIMFMT_1D;
      sa0Params.ICNT0  = numBins;
   }

   /**************************/
   /* Store SE and SA params */
   /**************************/

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET) = sa0Params;

   /******************************************/
   /* Initialize LUT for !C7504 C7x variants */
   /******************************************/

   __sLTCRFlags_t set0flags;

   // 16 way lut for 16-bit data with 64 entries (6-bit index)
   set0flags.INTERPOLATION = __LUT_INTERP_OFF;        // NA, applicable only in LUT mode
   set0flags.SATURATION    = __LUT_SAT_ON;            // Saturate after reaching limit
   set0flags.SIGNUNSIGN    = __LUT_UNSIGNED;          // data elements are unsigned
   set0flags.ELETYPE       = __LUT_ELETYPE_32BIT;     // This is the bit-depth of each bin
   set0flags.NUMTBLS       = __LUT_NUM_TBL_16;        // 16 parallel lookup every cycle
   set0flags.TBLSIZE       = __LUT_TBL_SIZE_16KBYTES; // Total table size
   set0flags.WEIGHTSIZE    = __LUT_WSIZE_8BIT;        // Set weight size as 8-bit
   set0flags.PROMOTION     = __LUT_PROMOTE_OFF;       // NA, applicable only in LUT mode

   // Set configuration register for SET0
   __LUT_SET_LTCR(__LUT_SET0, __lut_set_param(&set0flags));

   // Start the table at offset 0 from the beginning of L1D-SRAM
   __LUT_SET_LTBR(__LUT_SET0, 0x0000);

   // Enable set 1
   __LUT_SET_LTER(__LUT_ENABLE_0);

   VXLIB_histogram_val_init(numBins);

   /* Generate predicate buffer */
   if (VXLIB_HISTOGRAM_I8U_O32U) {

      if (width != strideInElements) {

         int32_t blockCounter = 0;

         uint64_t *predRegister = (uint64_t *) pKerPrivArgs->bufPredicateStore;

         uint32_t i              = 0;
         int32_t  j              = 0;
         uint64_t predictedValue = 0x0;

         while (blockCounter < wBlocks) {
            for (i = 0; i < elemCount; i++) {
               if (j < width) {
                  predictedValue |= (uint64_t) ((uint64_t) 0xF << (i * 4));
                  j++;
               }
               else {
                  j++;
               }
            }
            *predRegister = predictedValue;

            predRegister++;
            blockCounter++;
            predictedValue = 0x0;
         }

         SE_ELETYPE = c7x::se_eletype<c7x::ulong_vec>::value;
         SE_VECLEN  = c7x::se_veclen<c7x::ulong_vec>::value;

         se1Params.ELETYPE = __SE_ELETYPE_64BIT;
         se1Params.VECLEN  = __SE_VECLEN_1ELEM;
         se1Params.DIMFMT  = __SE_DIMFMT_3D;
         se1Params.ICNT0   = 1;
         se1Params.ICNT1   = wBlocks;
         se1Params.ICNT2   = height;
         se1Params.DIM1    = 1;
         se1Params.DIM2    = 0;

         *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET) = se1Params;
      }

      else if (width == strideInElements) {

         uint64_t *predRegister = (uint64_t *) pKerPrivArgs->bufPredicateStore;

         uint32_t i              = 0;
         int32_t  j              = 0;
         uint64_t predictedValue = 0x0;
         size_t   widthLastBlock = (width * height) % elemCount;

         for (i = 0; i < elemCount; i++) {
            if (j < widthLastBlock) {
               predictedValue |= (uint64_t) ((uint64_t) 0xF << (i * 4));
               j++;
            }
            else {
               j++;
            }
         }
         *predRegister = predictedValue;

         se1Params.ELETYPE = __SE_ELETYPE_64BIT;
         se1Params.VECLEN  = __SE_VECLEN_1ELEM;
         se1Params.DIMFMT  = __SE_DIMFMT_1D;
         se1Params.ICNT0   = 1;

         *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET) = se1Params;
      }
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_histogram_init_ci                                                                */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS
VXLIB_histogram_init_ci<VXLIB_HISTOGRAM_DTYPE_I8U_O32U>(VXLIB_kernelHandle              handle,
                                                        const VXLIB_bufParams2D_t      *bufParamsIn0,
                                                        const VXLIB_bufParams1D_t      *bufParamsOut,
                                                        const VXLIB_histogram_InitArgs *pKerInitArgs);

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_histogram_exec_ci                                                                                            */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method generates a histogram distribution based on pixel values
template <typename dTypeIn, typename dTypeOut>
VXLIB_STATUS VXLIB_histogram_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_histogram_PrivArgs *pKerPrivArgs = (VXLIB_histogram_PrivArgs *) handle;

   // structs to retrieve SE and SA paramters
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;

   // create local pointers
   dTypeIn *restrict pInLocal   = (dTypeIn *) pIn;
   dTypeOut *restrict pOutLocal = (dTypeOut *) pOut;
   uint8_t *restrict pPredicate = (uint8_t *) pKerPrivArgs->bufPredicateStore;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_histogram_exec_ci\n");
#endif

   // address of SE and SA parameters
   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   // retrieve SE and SA parameters
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);

   // open SEs to fetch Input samples
   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pPredicate, se1Params);

   // open SA0 to write output samples
   __SA0_OPEN(sa0Params);

   size_t   numBlocks = pKerPrivArgs->numBlocks;
   uint16_t numBins   = pKerPrivArgs->pKerInitArgs.numBins;

   c7x::uint_vec lowerBound = pKerPrivArgs->pKerInitArgs.offset;
   c7x::uint_vec upperBound = pKerPrivArgs->pKerInitArgs.range + pKerPrivArgs->pKerInitArgs.offset;
   c7x::uint_vec numBinsVec = pKerPrivArgs->pKerInitArgs.numBins;
   c7x::uint_vec rangeVec   = pKerPrivArgs->pKerInitArgs.range;

   // derive c7x vector type from template typename
   typedef typename c7x::make_full_vector<dTypeOut>::type vec;

   uint32_t maxSIMD = c7x::max_simd<dTypeOut>::value;

   /**************************/
   /* Populate Histogram     */
   /**************************/

   // Linear images
   if (pKerPrivArgs->width == pKerPrivArgs->strideInElements) {
      // When width * height is not a multiple of SIMD, need to use predicate buffer on last block to mask DECDIM 0s
      if ((pKerPrivArgs->width * pKerPrivArgs->height) % maxSIMD) {
         size_t linearBlocks = (pKerPrivArgs->width * pKerPrivArgs->height) / maxSIMD;
         for (uint32_t counter = 0; counter < linearBlocks; counter++) {
            vec inVec = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0

            __vpred cmp_lower = __cmp_le_pred(lowerBound, inVec);
            __vpred cmp_upper = __cmp_lt_pred(inVec, upperBound);
            __vpred condHist  = __and(cmp_lower, cmp_upper);

            vec histIndex = ((inVec - lowerBound) * numBinsVec) / rangeVec;

            vec inHist = __select(condHist, histIndex, numBinsVec);

            __hist(__LUT_SET0, inHist);
         }
         // Predicate buffer to mask DECDIM 0s on last block
         vec     inVec  = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0
         __vpred inPred = _mvrp(c7x::strm_eng<1, ulong>::get_adv());

         __vpred cmp_lower = __cmp_le_pred(lowerBound, inVec);
         __vpred cmp_upper = __cmp_lt_pred(inVec, upperBound);
         __vpred condHist  = __and(cmp_lower, cmp_upper);
         condHist          = __and(condHist, inPred);

         vec histIndex = ((inVec - lowerBound) * numBinsVec) / rangeVec;

         vec inHist = __select(condHist, histIndex, numBinsVec);

         __hist(__LUT_SET0, inHist);
      }
      // Width * height is a SIMD multiple, can process entire image in one loop
      else {
         size_t linearBlocks = (pKerPrivArgs->width * pKerPrivArgs->height) / maxSIMD;
         for (uint32_t counter = 0; counter < linearBlocks; counter++) {
            vec     inVec     = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0
            __vpred cmp_lower = __cmp_le_pred(lowerBound, inVec);
            __vpred cmp_upper = __cmp_lt_pred(inVec, upperBound);
            __vpred condHist  = __and(cmp_lower, cmp_upper);

            vec histIndex = ((inVec - lowerBound) * numBinsVec) / rangeVec;

            vec inHist = __select(condHist, histIndex, numBinsVec);

            __hist(__LUT_SET0, inHist);
         }
      }
   }
   // Not linear image (width != stride)
   else {
      for (uint32_t counter = 0; counter < numBlocks; counter++) {
         vec     inVec  = c7x::strm_eng<0, vec>::get_adv();          // fetch input 0 via SE0
         __vpred inPred = _mvrp(c7x::strm_eng<1, ulong>::get_adv()); // Predicate buffer to mask DECDIM 0s on last block

         __vpred cmp_lower = __cmp_le_pred(lowerBound, inVec);
         __vpred cmp_upper = __cmp_lt_pred(inVec, upperBound);
         __vpred condHist  = __and(cmp_lower, cmp_upper);
         condHist          = __and(condHist, inPred);

         vec histIndex = ((inVec - lowerBound) * numBinsVec) / rangeVec;

         // numBinsVec is chosen if the pixel does not meet the criteria to be in the distribution
         // bins are from bin[0] - bin[numBins -1] and thus numBins will not be a valid index and will not populate
         // histogram
         vec inHist = __select(condHist, histIndex, numBinsVec);

         __hist(__LUT_SET0, inHist);
      }
   }

   /**************************/
   /* Store Histogram        */
   /**************************/

   // Store histogram in outBuffer if last call to exec function for image
   if (pKerPrivArgs->pKerInitArgs.lastBlock) {

      uint32_t saStoreIter = (numBins / maxSIMD);
      if (numBins % maxSIMD) {
         saStoreIter++;
      }
      c7x::uint_vec bin;
      uint16_t      binCount = 0;

      for (int i = 0; i < saStoreIter; i++) {
         bin = 0;
         for (int j = 0; j < maxSIMD; j++) {
            c7x::uint_vec vHist = __lookup_read_uint(__LUT_SET0, (c7x::uint_vec)(binCount));
            // Horizontal add all bins to one index
            bin.s[j] = __horizontal_add(vHist);
            binCount++;
         }

         // After reaching a SIMD number of bins, store vector
         __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();
         vec    *addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, addr, bin);
      }
   }

   // close SE/SA
   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();

   return VXLIB_SUCCESS;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_histogram_exec_ci                                                                */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS VXLIB_histogram_exec_ci<VXLIB_HISTOGRAM_TYPENAME_I8U_O32U>(VXLIB_kernelHandle handle,
                                                                                 void *restrict pIn,
                                                                                 void *restrict pOut);

void VXLIB_histogram_perfEst(VXLIB_kernelHandle handle, size_t *archCycles, size_t *estCycles)
{

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_histogram_PrivArgs *pKerPrivArgs = (VXLIB_histogram_PrivArgs *) handle;

   // obtain loop count for compute loop
   size_t numBlocks   = pKerPrivArgs->numBlocks;
   size_t overheadCnt = 17;                // profiled code before entering compute loop
   *archCycles        = 7 + numBlocks * 2; // obtained from asm
   *estCycles         = overheadCnt + *archCycles;
}