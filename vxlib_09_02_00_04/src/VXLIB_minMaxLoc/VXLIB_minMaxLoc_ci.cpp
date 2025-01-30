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

#include "VXLIB_minMaxLoc_priv.h"

/**********************************************************************************************************************/
/*                                                                                                                    */
/* DEFINES                                                                                                            */
/*                                                                                                                    */
/**********************************************************************************************************************/


#define SE_PARAM_BASE (0x0000)
#define SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE1_PARAM_OFFSET (SE0_PARAM_OFFSET + VXLIB_SE_PARAM_SIZE)
#define SE0_COUNT_PARAM_OFFSET (SE1_PARAM_OFFSET + VXLIB_SE_PARAM_SIZE)
#define SA0_COUNT_PARAM_OFFSET (SE0_COUNT_PARAM_OFFSET + VXLIB_SE_PARAM_SIZE)
#define SE0_LOC_PARAM_OFFSET (SA0_COUNT_PARAM_OFFSET + VXLIB_SA_PARAM_SIZE)
#define SA0_LOC_PARAM_OFFSET (SE0_LOC_PARAM_OFFSET + VXLIB_SE_PARAM_SIZE)

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_minMaxLoc_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/
#if (__C7X_VEC_SIZE_BITS__ == 256)
const c7x::uint_vec referenceRowIndex = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7);
#elif (__C7X_VEC_SIZE_BITS__ == 512)
const c7x::uint_vec referenceRowIndex = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
#else
#error invalid target
#endif

/* Global variable  */
__vpred predSelectMask;

template <uint32_t dType> static inline __vpred generateSelectMask(uint32_t selectEleCount);

template <> inline __vpred generateSelectMask<VXLIB_MINMAXLOC_DTYPE_8U>(uint32_t selectEleCount)
{
   return __mask_char(selectEleCount);
}
template <> inline __vpred generateSelectMask<VXLIB_MINMAXLOC_DTYPE_8S>(uint32_t selectEleCount)
{
   return __mask_char(selectEleCount);
}
template <> inline __vpred generateSelectMask<VXLIB_MINMAXLOC_DTYPE_16U>(uint32_t selectEleCount)
{
   return __mask_short(selectEleCount);
}
template <> inline __vpred generateSelectMask<VXLIB_MINMAXLOC_DTYPE_16S>(uint32_t selectEleCount)
{
   return __mask_short(selectEleCount);
}

template <uint32_t dType> inline void setSEPromoteMode(__SE_TEMPLATE_v1 *seParams);

template <> inline void setSEPromoteMode<VXLIB_MINMAXLOC_DTYPE_8U>(__SE_TEMPLATE_v1 *seParams)
{
   seParams->PROMOTE = __SE_PROMOTE_4X_ZEROEXT;
}
template <> inline void setSEPromoteMode<VXLIB_MINMAXLOC_DTYPE_8S>(__SE_TEMPLATE_v1 *seParams)
{
   seParams->PROMOTE = __SE_PROMOTE_4X_SIGNEXT;
}
template <> inline void setSEPromoteMode<VXLIB_MINMAXLOC_DTYPE_16U>(__SE_TEMPLATE_v1 *seParams)
{
   seParams->PROMOTE = __SE_PROMOTE_2X_ZEROEXT;
}
template <> inline void setSEPromoteMode<VXLIB_MINMAXLOC_DTYPE_16S>(__SE_TEMPLATE_v1 *seParams)
{
   seParams->PROMOTE = __SE_PROMOTE_2X_SIGNEXT;
}

// this method initializes the kernel-specific parameters
// mainly, the streaming engine and streaming address generators
template <uint32_t dType>
VXLIB_STATUS VXLIB_minMaxLoc_init_ci(VXLIB_kernelHandle              handle,
                                     VXLIB_bufParams2D_t *           bufParamsIn,
                                     VXLIB_bufParams1D_t *           bufParamsMinLoc,
                                     VXLIB_bufParams1D_t *           bufParamsMaxLoc,
                                     const VXLIB_minMaxLoc_InitArgs *pKerInitArgs)
{
   VXLIB_DEBUGPRINTFN(0, "%s\n", "Enter Function");
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_minMaxLoc_PrivArgs *pKerPrivArgs = (VXLIB_minMaxLoc_PrivArgs *) handle;

   // obtain image size and compute number of blocks to process
   uint32_t width            = pKerPrivArgs->width;
   uint32_t height           = pKerPrivArgs->height;
   uint32_t strideInElements = pKerPrivArgs->strideInElements;
   uint32_t eleCount         = VXLIB_max_simd<dType>::value;

   uint8_t *pBlock = pKerPrivArgs->bufPblock; // address to retrieve to store SE/SA params

   typedef typename VXLIB_vec_type<dType>::type vec;

   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<vec>::value;

   /*  ---------------------------------------------------------------------------------------------------
                 SE PARAM SETTINGS FOR MAX/MIN VALUE
   ------------------------------------------------------------------------------------------------------- */

   // structs to hold SE and SA parameters
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se1Params = __gen_SE_TEMPLATE_v1();

   uint32_t oneVecBlocks       = width / eleCount;
   uint32_t firstBlockWidth    = oneVecBlocks * eleCount;
   uint32_t remainingCols      = width - firstBlockWidth;
   predSelectMask              = generateSelectMask<dType>(remainingCols);
   uint32_t firstBlockLoopCnt  = oneVecBlocks * (height / 2);
   uint32_t secondBlockLoopCnt = VXLIB_ceilingDiv(remainingCols, eleCount) * (height / 2);

   pKerPrivArgs->oneVecBlocks       = oneVecBlocks;
   pKerPrivArgs->remainingCols      = remainingCols;
   pKerPrivArgs->firstBlockWidth    = firstBlockWidth;
   pKerPrivArgs->firstBlockLoopCnt  = firstBlockLoopCnt;
   pKerPrivArgs->secondBlockLoopCnt = secondBlockLoopCnt;
   pKerPrivArgs->isOddHeight        = ((height % 2) == 0) ? 0u : 1u;

   se0Params.ICNT0   = (oneVecBlocks > 0) ? firstBlockWidth : eleCount;
   se0Params.ICNT1   = VXLIB_ceilingDiv(height, 2);
   se0Params.DIM1    = strideInElements * 2;
   se0Params.DIMFMT  = __SE_DIMFMT_2D;
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;

   se1Params.ICNT0         = eleCount;
   se1Params.ICNT1         = VXLIB_ceilingDiv(height, 2);
   se1Params.DIM1          = strideInElements * 2;
   se1Params.DIMFMT        = __SE_DIMFMT_2D;
   se1Params.ELETYPE       = SE_ELETYPE;
   se1Params.VECLEN        = SE_VECLEN;
   se1Params.DECDIM2       = __SE_DECDIM_DIM2;
   se1Params.DECDIM2_WIDTH = remainingCols;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET) = se0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET) = se1Params;

   /*  ---------------------------------------------------------------------------------------------------
                 SE/SA PARAM SETTINGS FOR MAX/MIN COUNT
   ------------------------------------------------------------------------------------------------------- */

   __SE_TEMPLATE_v1 se0CountParams = __gen_SE_TEMPLATE_v1();
   se0CountParams.ICNT0            = width;
   se0CountParams.ICNT1            = height;
   se0CountParams.DIM1             = strideInElements;
   se0CountParams.ICNT2            = 2;
   se0CountParams.DIM2             = 0;
   se0CountParams.DIMFMT           = __SE_DIMFMT_3D;
   se0CountParams.ELETYPE          = SE_ELETYPE;
   se0CountParams.VECLEN           = SE_VECLEN;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_COUNT_PARAM_OFFSET) = se0CountParams;

   __SA_TEMPLATE_v1 sa0CountParams = __gen_SA_TEMPLATE_v1();
   sa0CountParams.ICNT0            = width;
   sa0CountParams.ICNT1            = height;
   sa0CountParams.DIM1             = strideInElements;
   sa0CountParams.ICNT2            = 2;
   sa0CountParams.DIM2             = 0;
   sa0CountParams.DIMFMT           = __SA_DIMFMT_3D;
   sa0CountParams.VECLEN           = SA_VECLEN;

   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_COUNT_PARAM_OFFSET) = sa0CountParams;

   /*  ---------------------------------------------------------------------------------------------------
                 SE/SA PARAM SETTINGS FOR MAX/MIN LOCATION
   ------------------------------------------------------------------------------------------------------- */

   typedef typename VXLIB_vec_type<VXLIB_UINT32>::type vecIndexType;

   __SE_VECLEN SE_VECLEN_32BIT = c7x::se_veclen<vecIndexType>::value;
   __SA_VECLEN SA_VECLEN_32BIT = c7x::sa_veclen<vecIndexType>::value;

   __SE_TEMPLATE_v1 se0NewLocParams = __gen_SE_TEMPLATE_v1();
   se0NewLocParams.ICNT0            = width;
   se0NewLocParams.ICNT1            = height;
   se0NewLocParams.DIM1             = strideInElements;
   se0NewLocParams.ICNT2            = 2;
   se0NewLocParams.DIM2             = 0;
   se0NewLocParams.DIMFMT           = __SE_DIMFMT_3D;
   se0NewLocParams.ELETYPE          = SE_ELETYPE;
   se0NewLocParams.VECLEN           = SE_VECLEN_32BIT;
   setSEPromoteMode<dType>(&se0NewLocParams);

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_LOC_PARAM_OFFSET) = se0NewLocParams;

   __SA_TEMPLATE_v1 sa0NewLocParams = __gen_SA_TEMPLATE_v1();
   sa0NewLocParams.ICNT0            = width;
   sa0NewLocParams.ICNT1            = height;
   sa0NewLocParams.DIM1             = strideInElements;
   sa0NewLocParams.ICNT2            = 2;
   sa0NewLocParams.DIM2             = 0;
   sa0NewLocParams.DIMFMT           = __SA_DIMFMT_3D;
   sa0NewLocParams.VECLEN           = SA_VECLEN_32BIT;

   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_LOC_PARAM_OFFSET) = sa0NewLocParams;

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_minMaxLoc_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS VXLIB_minMaxLoc_init_ci<VXLIB_MINMAXLOC_DTYPE_8U>(VXLIB_kernelHandle              handle,
                                                                        VXLIB_bufParams2D_t *           bufParamsIn,
                                                                        VXLIB_bufParams1D_t *           bufParamsMinLoc,
                                                                        VXLIB_bufParams1D_t *           bufParamsMaxLoc,
                                                                        const VXLIB_minMaxLoc_InitArgs *pKerInitArgs);

template VXLIB_STATUS VXLIB_minMaxLoc_init_ci<VXLIB_MINMAXLOC_DTYPE_8S>(VXLIB_kernelHandle              handle,
                                                                        VXLIB_bufParams2D_t *           bufParamsIn,
                                                                        VXLIB_bufParams1D_t *           bufParamsMinLoc,
                                                                        VXLIB_bufParams1D_t *           bufParamsMaxLoc,
                                                                        const VXLIB_minMaxLoc_InitArgs *pKerInitArgs);

template VXLIB_STATUS VXLIB_minMaxLoc_init_ci<VXLIB_MINMAXLOC_DTYPE_16U>(VXLIB_kernelHandle   handle,
                                                                         VXLIB_bufParams2D_t *bufParamsIn,
                                                                         VXLIB_bufParams1D_t *bufParamsMinLoc,
                                                                         VXLIB_bufParams1D_t *bufParamsMaxLoc,
                                                                         const VXLIB_minMaxLoc_InitArgs *pKerInitArgs);

template VXLIB_STATUS VXLIB_minMaxLoc_init_ci<VXLIB_MINMAXLOC_DTYPE_16S>(VXLIB_kernelHandle   handle,
                                                                         VXLIB_bufParams2D_t *bufParamsIn,
                                                                         VXLIB_bufParams1D_t *bufParamsMinLoc,
                                                                         VXLIB_bufParams1D_t *bufParamsMaxLoc,
                                                                         const VXLIB_minMaxLoc_InitArgs *pKerInitArgs);

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_minMaxLoc_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template <typename dType> inline uint32_t countEle(__vpred predIn);
template <> inline uint32_t               countEle<uint8_t>(__vpred predIn) { return __popcount_char(predIn); }
template <> inline uint32_t               countEle<int8_t>(__vpred predIn) { return __popcount_char(predIn); }
template <> inline uint32_t               countEle<uint16_t>(__vpred predIn) { return __popcount_short(predIn); }
template <> inline uint32_t               countEle<int16_t>(__vpred predIn) { return __popcount_short(predIn); }
template <> inline uint32_t               countEle<uint32_t>(__vpred predIn) { return __popcount_int(predIn); }
template <> inline uint32_t               countEle<int32_t>(__vpred predIn) { return __popcount_int(predIn); }

template <typename INVECTYPE>
static inline void locUpdateMinMax(INVECTYPE     vIn,
                                   __vpred       predSA,
                                   INVECTYPE     numericLimitVec32BIT,
                                   INVECTYPE     comparisonVec,
                                   c7x::uint_vec rowIdxTrackVec,
                                   c7x::uint_vec colIdxTrackVec,
                                   uint32_t      halfEleCount,
                                   uint32_t *    locCapacity,
                                   uint32_t *&   pLocStore)
{
   typedef typename c7x::element_type_of<INVECTYPE>::type signType;

   vIn = __select(predSA, vIn, numericLimitVec32BIT);

   __vpred  predMin     = __cmp_eq_pred(comparisonVec, vIn);
   uint32_t minEleFound = countEle<signType>(predMin);

   c7x::uint_vec rowIdxMin = c7x::as_uint_vec(__gather_set_bits(predMin, c7x::as_char_vec(rowIdxTrackVec)));
   c7x::uint_vec idx_lo    = __shuffle_stride2_low_low(colIdxTrackVec, rowIdxMin);
   c7x::uint_vec idx_hi    = __shuffle_stride2_high_high(colIdxTrackVec, rowIdxMin);

   uint32_t minEleCount_lo = __min(minEleFound, halfEleCount);
   minEleCount_lo          = __min(minEleCount_lo, *locCapacity);
   *locCapacity            = *locCapacity - minEleCount_lo;

   uint32_t minEleCount_hi = minEleFound - minEleCount_lo;
   minEleCount_hi          = __min(minEleCount_hi, *locCapacity);
   *locCapacity            = *locCapacity - minEleCount_hi;

   __vpred predMin_lo = __mask_int(minEleCount_lo * 2u);
   __vpred predMin_hi = __mask_int(minEleCount_hi * 2u);

   __vstore_pred(predMin_lo, (c7x::uint_vec *) pLocStore, idx_lo);
   pLocStore = pLocStore + (minEleCount_lo * 2u);

   __vstore_pred(predMin_hi, (c7x::uint_vec *) pLocStore, idx_hi);
   pLocStore = pLocStore + (minEleCount_hi * 2u);
}

// this method performs pixel-wise addition of two input images
template <typename dType>
VXLIB_STATUS VXLIB_minMaxLoc_exec_ci(VXLIB_kernelHandle handle,
                                     void *restrict     pIn,
                                     void *restrict     pMinVal,
                                     void *restrict     pMaxVal,
                                     void *restrict     pMinCount,
                                     void *restrict     pMaxCount,
                                     void *restrict     pMinLocCapacity,
                                     void *restrict     pMaxLocCapacity,
                                     void *restrict     pMinLoc,
                                     void *restrict     pMaxLoc,
                                     void *restrict     pStartX,
                                     void *restrict     pStartY)
{
   VXLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_minMaxLoc_PrivArgs *pKerPrivArgs = (VXLIB_minMaxLoc_PrivArgs *) handle;

   typedef typename c7x::make_full_vector<dType>::type vec;

   uint32_t eleCount = c7x::element_count_of<vec>::value;
   // create local pointers
   dType restrict *pInLocal          = (dType *) pIn;
   dType restrict *pMinValLocal      = (dType *) pMinVal;
   dType restrict *pMaxValLocal      = (dType *) pMaxVal;
   uint32_t restrict *pMinCountLocal = (uint32_t *) pMinCount;
   uint32_t restrict *pMaxCountLocal = (uint32_t *) pMaxCount;
   uint32_t restrict *pMinLocLocal   = (uint32_t *) pMinLoc;
   uint32_t restrict *pMaxLocLocal   = (uint32_t *) pMaxLoc;
   uint32_t restrict *pStartXLocal   = (uint32_t *) pStartX;
   uint32_t restrict *pStartYLocal   = (uint32_t *) pStartY;
   uint32_t restrict *minLocCapacity = (uint32_t *) pMinLocCapacity;
   uint32_t restrict *maxLocCapacity = (uint32_t *) pMaxLocCapacity;

   dType    origMinVal   = *pMinValLocal;
   dType    origMaxVal   = *pMaxValLocal;
   uint32_t origMinCount = 0;
   uint32_t origMaxCount = 0;

   // obtain dimensional parameters
   uint32_t width              = pKerPrivArgs->width;
   uint32_t height             = pKerPrivArgs->height;
   uint32_t strideInElements   = pKerPrivArgs->strideInElements;
   uint32_t oneVecBlocks       = pKerPrivArgs->oneVecBlocks;
   uint32_t remainingCols      = pKerPrivArgs->remainingCols;
   uint32_t firstBlockWidth    = pKerPrivArgs->firstBlockWidth;
   uint32_t firstBlockLoopCnt  = pKerPrivArgs->firstBlockLoopCnt;
   uint32_t secondBlockLoopCnt = pKerPrivArgs->secondBlockLoopCnt;
   bool     isOddHeight        = pKerPrivArgs->isOddHeight;

   // address of SE and SA parameters
   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   // retrieve SE and SA parameters
   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);

   /* ----------------------------------------------------------------------------------
                          FIND MIN MAX VALUE
     ---------------------------------------------------------------------------------- */

   __vpred predSelectLocal = predSelectMask;
   vec     numericMaxVec   = (vec) std::numeric_limits<dType>::max();
   vec     numericMinVec   = (vec) std::numeric_limits<dType>::min();
   vec     minVecSE0       = (vec)(*pMinValLocal);
   vec     maxVecSE0       = (vec)(*pMaxValLocal);
   vec     minVecSE1       = (vec)(*pMinValLocal);
   vec     maxVecSE1       = (vec)(*pMaxValLocal);
   vec     minVecSE0_rem   = (vec)(*pMinValLocal);
   vec     maxVecSE0_rem   = (vec)(*pMaxValLocal);
   vec     minVecSE1_rem   = (vec)(*pMinValLocal);
   vec     maxVecSE1_rem   = (vec)(*pMaxValLocal);

   if (oneVecBlocks > 0) {
      __SE0_OPEN(pInLocal, se0Params);
      if (height > 1) {
         se0Params.ICNT1 = height / 2;
         __SE1_OPEN(pInLocal + strideInElements, se0Params);
      }

      for (uint32_t i = 0; i < firstBlockLoopCnt; i++) {
         vec vSE0 = c7x::strm_eng<0, vec>::get_adv();
         vec vSE1 = c7x::strm_eng<1, vec>::get_adv();

         minVecSE0 = __min(minVecSE0, vSE0);
         maxVecSE0 = __max(maxVecSE0, vSE0);
         minVecSE1 = __min(minVecSE1, vSE1);
         maxVecSE1 = __max(maxVecSE1, vSE1);
      }

      if (isOddHeight) {
         for (uint32_t j = 0; j < oneVecBlocks; j++) {
            vec vSE0 = c7x::strm_eng<0, vec>::get_adv();

            minVecSE0 = __min(minVecSE0, vSE0);
            maxVecSE0 = __max(maxVecSE0, vSE0);
         }
      }
      __SE0_CLOSE();
      __SE1_CLOSE();
   }
   if (remainingCols > 0) {
      __SE0_OPEN(pInLocal + firstBlockWidth, se1Params);
      if (height > 1) {
         se1Params.ICNT1 = height / 2;
         __SE1_OPEN(pInLocal + strideInElements + firstBlockWidth, se1Params);
      }
      for (uint32_t i = 0; i < secondBlockLoopCnt; i++) {
         vec vSE0 = c7x::strm_eng<0, vec>::get_adv();
         vec vSE1 = c7x::strm_eng<1, vec>::get_adv();

         minVecSE0_rem = __min(minVecSE0_rem, vSE0);
         maxVecSE0_rem = __max(maxVecSE0_rem, vSE0);
         minVecSE1_rem = __min(minVecSE1_rem, vSE1);
         maxVecSE1_rem = __max(maxVecSE1_rem, vSE1);
      }

      if (isOddHeight) {
         vec vSE0 = c7x::strm_eng<0, vec>::get_adv();

         minVecSE0_rem = __min(minVecSE0_rem, vSE0);
         maxVecSE0_rem = __max(maxVecSE0_rem, vSE0);
      }
      minVecSE0_rem = __select(predSelectLocal, minVecSE0_rem, numericMaxVec);
      maxVecSE0_rem = __select(predSelectLocal, maxVecSE0_rem, numericMinVec);
      minVecSE1_rem = __select(predSelectLocal, minVecSE1_rem, numericMaxVec);
      maxVecSE1_rem = __select(predSelectLocal, maxVecSE1_rem, numericMinVec);

      __SE0_CLOSE();
      __SE1_CLOSE();
   }

   /* ---------- FIND MIN VAL ---------- */
   {
      vec minVec1     = __min(minVecSE0, minVecSE1);
      vec minVec2     = __min(minVecSE0_rem, minVecSE1_rem);
      vec minVecFinal = __min(minVec1, minVec2);
      *pMinValLocal   = VXLIB_horizontalMin<dType, vec>(minVecFinal);
   }

   /* ---------- FIND MAX VAL ---------- */
   {
      vec maxVec1     = __max(maxVecSE0, maxVecSE1);
      vec maxVec2     = __max(maxVecSE0_rem, maxVecSE1_rem);
      vec maxVecFinal = __max(maxVec1, maxVec2);
      *pMaxValLocal   = VXLIB_horizontalMax<dType, vec>(maxVecFinal);
   }

   VXLIB_DEBUGPRINTFN(0, "\norigMinVal: 0x%08X \nnew minVal: 0x%08X \norigMaxVal: 0x%08X \nnew maxVal: 0x%08X\n",
                      origMinVal, *pMinValLocal, origMaxVal, *pMaxValLocal);

   /* Only if the minCount or maxCount are available for counting we proceed*/
   if (pMinCountLocal || pMaxCountLocal) {
      __SE_TEMPLATE_v1 se0CountParams = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_COUNT_PARAM_OFFSET);
      __SA_TEMPLATE_v1 sa0CountParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_COUNT_PARAM_OFFSET);
      __SE0_OPEN(pInLocal, se0CountParams);
      __SA0_OPEN(sa0CountParams);

      uint32_t widthBlocks = VXLIB_ceilingDiv(width, eleCount);
      uint32_t numBlocks   = height * widthBlocks;

      /* To only calculate minCount */
      if (pMinCountLocal) {
         uint32_t minCount  = 0;
         vec      minValVec = (vec)(*pMinValLocal);
         vec      fillPartialVec = minValVec + (vec)1;
         for (uint32_t i = 0; i < numBlocks; i++) {
            vec     vIn    = c7x::strm_eng<0, vec>::get_adv();
            __vpred predIn = c7x::strm_agen<0, vec>::get_vpred();
            c7x::strm_agen<0, vec>::get_adv(pInLocal);

            vIn = __select(predIn, vIn, fillPartialVec);

            __vpred predMin = __cmp_eq_pred(minValVec, vIn);
            minCount += countEle<dType>(predMin);
         }

         origMinCount = *pMinCountLocal;
         /* Increment the original minCount if minVal form previous block and this block is same
            Else update minCount */
         if ((*pMinValLocal) == origMinVal) {
            *pMinCountLocal += minCount;
         }
         else {
            *pMinCountLocal = minCount;
         }
         VXLIB_DEBUGPRINTFN(0, "\norigMinCount: %u \nnew minCount: %u\n", origMinCount, *pMinCountLocal);
      }

      /* To only calculate maxCount */
      if (pMaxCountLocal) {
         uint32_t maxCount  = 0;
         vec      maxValVec = (vec)(*pMaxValLocal);
         vec      fillPartialVec = maxValVec - (vec)1;         
         for (uint32_t i = 0; i < numBlocks; i++) {
            vec     vIn    = c7x::strm_eng<0, vec>::get_adv();
            __vpred predIn = c7x::strm_agen<0, vec>::get_vpred();
            c7x::strm_agen<0, vec>::get_adv(pInLocal);

            vIn = __select(predIn, vIn, fillPartialVec);

            __vpred predMax = __cmp_eq_pred(maxValVec, vIn);
            maxCount += countEle<dType>(predMax);
         }

         origMaxCount = *pMaxCountLocal;
         /* Increment the original maxCount if maxVal form previous block and this block is same
            Else update maxCount */
         if ((*pMaxValLocal) == origMaxVal) {
            *pMaxCountLocal += maxCount;
         }
         else {
            *pMaxCountLocal = maxCount;
         }
         VXLIB_DEBUGPRINTFN(0, "\norigMaxCount: %u \nnew maxCount: %u\n", origMaxCount, *pMaxCountLocal);
      }

      __SE0_CLOSE();
      __SA0_CLOSE();

      /* If the optional parameters for min or max locations are available AND
       * if one of the min/max values or counts have changed during this block iteration ...*/
      if (((pMinLocLocal != NULL) && ((origMinVal != *pMinValLocal) || (origMinCount != *pMinCountLocal))) ||
          ((pMaxLocLocal != NULL) && ((origMaxVal != *pMaxValLocal) || (origMaxCount != *pMaxCountLocal)))) {

         __SE_TEMPLATE_v1 se0NewLocParams = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_LOC_PARAM_OFFSET);
         __SA_TEMPLATE_v1 sa0NewLocParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_LOC_PARAM_OFFSET);

         typedef typename std::conditional<std::is_signed<dType>::value, int32_t, uint32_t>::type signType;

         typedef typename c7x::make_full_vector<signType>::type vecInType;
         typedef typename c7x::make_full_vector<uint32_t>::type vecIndexType;

         uint32_t     eleCount32BIT     = c7x::element_count_of<vecIndexType>::value;
         uint32_t     halfEleCount32BIT = eleCount32BIT / 2;
         uint32_t     widthBlocks32BIT  = VXLIB_ceilingDiv(width, eleCount32BIT);
         vecIndexType oneVec            = (vecIndexType) 1;
         vecIndexType eleCountVec32BIT  = (vecIndexType) eleCount32BIT;
         vecIndexType startXVec         = (vecIndexType)(*pStartXLocal);
         vecIndexType startYVec         = (vecIndexType)(*pStartYLocal);
         vecIndexType rowIdxStart       = referenceRowIndex + startXVec;

         __SE0_OPEN(pInLocal, se0NewLocParams);
         __SA0_OPEN(sa0NewLocParams);

         if (pMinCountLocal && pMinLocLocal) {
            vecInType numericMaxVec32BIT = (vecInType) std::numeric_limits<signType>::max();
            vecInType minValVec32BIT     = (vecInType)(*pMinValLocal);
            uint32_t *pMinLocStore       = pMinLocLocal;
            /* Starting location offsets pointers are adjusted if the new min/max values
             * are matching the values given on the input (perhaps from a previous block).
             * This allows for this function to be called on multiple blocks
             * of an image iteratively without externally keeping track of the
             * locations during processing */
            uint32_t minLocCapacityTemp = *minLocCapacity;
            if (origMinVal == *pMinValLocal) {
               pMinLocStore       = pMinLocLocal + (2 * origMinCount);
               minLocCapacityTemp = minLocCapacityTemp - origMinCount;
            }
            vecIndexType rowIdxTrackVec = rowIdxStart;
            vecIndexType colIdxTrackVec = startYVec;

            for (uint32_t vertical = 0; vertical < height; vertical++) {
               for (uint32_t horizontal = 0; horizontal < widthBlocks32BIT; horizontal++) {
                  vecInType vIn      = c7x::strm_eng<0, vecInType>::get_adv();
                  __vpred   predMask = c7x::strm_agen<0, vecInType>::get_vpred();
                  c7x::strm_agen<0, vecInType>::get_adv(pInLocal);

                  locUpdateMinMax<vecInType>(vIn, predMask, numericMaxVec32BIT, minValVec32BIT, rowIdxTrackVec,
                                             colIdxTrackVec, halfEleCount32BIT, &minLocCapacityTemp, pMinLocStore);
                  rowIdxTrackVec = rowIdxTrackVec + eleCountVec32BIT;
               }
               colIdxTrackVec = colIdxTrackVec + oneVec;
               rowIdxTrackVec = rowIdxStart;
            }
            *minLocCapacity = minLocCapacityTemp;
         }

         if (pMaxCountLocal && pMaxLocLocal) {
            vecInType numericMinVec32BIT = (vecInType) std::numeric_limits<signType>::min();
            vecInType maxValVec32BIT     = (vecInType)(*pMaxValLocal);
            uint32_t *pMaxLocStore       = pMaxLocLocal;
            /* Starting location offsets pointers are adjusted if the new min/max values
             * are matching the values given on the input (perhaps from a previous block).
             * This allows for this function to be called on multiple blocks
             * of an image iteratively without externally keeping track of the
             * locations during processing */
            uint32_t maxLocCapacityTemp = *maxLocCapacity;
            if (origMaxVal == *pMaxValLocal) {
               pMaxLocStore       = pMaxLocLocal + (2 * origMaxCount);
               maxLocCapacityTemp = maxLocCapacityTemp - origMaxCount;
            }
            vecIndexType rowIdxTrackVec = rowIdxStart;
            vecIndexType colIdxTrackVec = startYVec;

            for (uint32_t vertical = 0; vertical < height; vertical++) {
               for (uint32_t horizontal = 0; horizontal < widthBlocks32BIT; horizontal++) {
                  vecInType vIn      = c7x::strm_eng<0, vecInType>::get_adv();
                  __vpred   predMask = c7x::strm_agen<0, vecInType>::get_vpred();
                  c7x::strm_agen<0, vecInType>::get_adv(pInLocal);

                  locUpdateMinMax<vecInType>(vIn, predMask, numericMinVec32BIT, maxValVec32BIT, rowIdxTrackVec,
                                             colIdxTrackVec, halfEleCount32BIT, &maxLocCapacityTemp, pMaxLocStore);
                  rowIdxTrackVec = rowIdxTrackVec + eleCountVec32BIT;
               }
               colIdxTrackVec = colIdxTrackVec + oneVec;
               rowIdxTrackVec = rowIdxStart;
            }
            *maxLocCapacity = maxLocCapacityTemp;
         }
         __SE0_CLOSE();
         __SA0_CLOSE();
      }
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_minMaxLoc_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS VXLIB_minMaxLoc_exec_ci<VXLIB_MINMAXLOC_TYPENAME_8U>(VXLIB_kernelHandle handle,
                                                                           void *restrict     pIn,
                                                                           void *restrict     pMinVal,
                                                                           void *restrict     pMaxVal,
                                                                           void *restrict     pMinCount,
                                                                           void *restrict     pMaxCount,
                                                                           void *restrict     pMinLocCapacity,
                                                                           void *restrict     pMaxLocCapacity,
                                                                           void *restrict     pMinLoc,
                                                                           void *restrict     pMaxLoc,
                                                                           void *restrict     pStartX,
                                                                           void *restrict     pStartY);

template VXLIB_STATUS VXLIB_minMaxLoc_exec_ci<VXLIB_MINMAXLOC_TYPENAME_8S>(VXLIB_kernelHandle handle,
                                                                           void *restrict     pIn,
                                                                           void *restrict     pMinVal,
                                                                           void *restrict     pMaxVal,
                                                                           void *restrict     pMinCount,
                                                                           void *restrict     pMaxCount,
                                                                           void *restrict     pMinLocCapacity,
                                                                           void *restrict     pMaxLocCapacity,
                                                                           void *restrict     pMinLoc,
                                                                           void *restrict     pMaxLoc,
                                                                           void *restrict     pStartX,
                                                                           void *restrict     pStartY);

template VXLIB_STATUS VXLIB_minMaxLoc_exec_ci<VXLIB_MINMAXLOC_TYPENAME_16U>(VXLIB_kernelHandle handle,
                                                                            void *restrict     pIn,
                                                                            void *restrict     pMinVal,
                                                                            void *restrict     pMaxVal,
                                                                            void *restrict     pMinCount,
                                                                            void *restrict     pMaxCount,
                                                                            void *restrict     pMinLocCapacity,
                                                                            void *restrict     pMaxLocCapacity,
                                                                            void *restrict     pMinLoc,
                                                                            void *restrict     pMaxLoc,
                                                                            void *restrict     pStartX,
                                                                            void *restrict     pStartY);

template VXLIB_STATUS VXLIB_minMaxLoc_exec_ci<VXLIB_MINMAXLOC_TYPENAME_16S>(VXLIB_kernelHandle handle,
                                                                            void *restrict     pIn,
                                                                            void *restrict     pMinVal,
                                                                            void *restrict     pMaxVal,
                                                                            void *restrict     pMinCount,
                                                                            void *restrict     pMaxCount,
                                                                            void *restrict     pMinLocCapacity,
                                                                            void *restrict     pMaxLocCapacity,
                                                                            void *restrict     pMinLoc,
                                                                            void *restrict     pMaxLoc,
                                                                            void *restrict     pStartX,
                                                                            void *restrict     pStartY);
/**********************************************************************************************************************/
/*                                                                                                                    */
/* Performance estimation function */
/*                                                                                                                    */
/**********************************************************************************************************************/

void VXLIB_minMaxLoc_perfEst(VXLIB_kernelHandle              handle,
                             const VXLIB_bufParams2D_t      *bufParamsIn,
                             const VXLIB_bufParams1D_t      *bufParamsMinLoc,
                             const VXLIB_bufParams1D_t      *bufParamsMaxLoc,
                             const VXLIB_minMaxLoc_InitArgs *pKerInitArgs,
                             size_t                         *archCycles,
                             size_t                         *estCycles)
{

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_minMaxLoc_PrivArgs *pKerPrivArgs = (VXLIB_minMaxLoc_PrivArgs *) handle;

   uint32_t height        = pKerPrivArgs->height;
   uint32_t width         = pKerPrivArgs->width;
   uint32_t oneVecBlocks  = pKerPrivArgs->oneVecBlocks;
   uint32_t remainingCols = pKerPrivArgs->remainingCols;
   //  uint32_t firstBlockWidth    = pKerPrivArgs->firstBlockWidth;
   uint32_t firstBlockLoopCnt  = pKerPrivArgs->firstBlockLoopCnt;
   uint32_t secondBlockLoopCnt = pKerPrivArgs->secondBlockLoopCnt;
   bool     isOddHeight        = pKerPrivArgs->isOddHeight;

   uint32_t eleCount      = 0;
   uint32_t eleCount32BIT = VXLIB_max_simd<VXLIB_UINT32>::value;
   if (bufParamsIn->data_type == VXLIB_UINT8) {
      typedef typename c7x::make_full_vector<uint8_t>::type vec;
      eleCount = c7x::element_count_of<vec>::value;
   }

   if (bufParamsIn->data_type == VXLIB_INT8) {
      typedef typename c7x::make_full_vector<int8_t>::type vec;
      eleCount = c7x::element_count_of<vec>::value;
   }

   if (bufParamsIn->data_type == VXLIB_UINT16) {
      typedef typename c7x::make_full_vector<uint16_t>::type vec;
      eleCount = c7x::element_count_of<vec>::value;
   }

   if (bufParamsIn->data_type == VXLIB_INT16) {
      typedef typename c7x::make_full_vector<int16_t>::type vec;
      eleCount = c7x::element_count_of<vec>::value;
   }

   //  size_t numBlocks   = pKerPrivArgs->numBlocks;
   size_t overheadCnt = 84; // profiled code before entering compute loop

   /* ----------------Estimate for MIN/MAX value  */
   size_t minMaxValCycles = 1 + firstBlockLoopCnt * 1;

   if (isOddHeight) {
      minMaxValCycles += 1 + oneVecBlocks * 1;
   }

   if (remainingCols > 0) {
      minMaxValCycles += 1 + secondBlockLoopCnt * 1;
   }

   size_t horizontalMinMaxCycles = 14;
   minMaxValCycles += horizontalMinMaxCycles;

   /* Estimate for MIN/MAX count */
   size_t minMaxCountLoop = height * VXLIB_ceilingDiv(width, eleCount);
   size_t minCountCycles  = 9 + minMaxCountLoop * 1;
   size_t maxCountCycles  = 9 + minMaxCountLoop * 1;

   /* Estimate for MIN/MAX location */
   size_t outerLoopExclusive = 6;
   size_t innerLoopCount = VXLIB_ceilingDiv(width, eleCount32BIT);
   size_t innerLoopCycles = 20 + innerLoopCount * 5;

   size_t minLocCycles  = height * (outerLoopExclusive + innerLoopCycles);
   size_t maxLocCycles  = height * (outerLoopExclusive + innerLoopCycles);
   
   *archCycles = minMaxValCycles + minCountCycles + maxCountCycles + minLocCycles + maxLocCycles;
   *estCycles  = overheadCnt + *archCycles;
}
/* ======================================================================== */
/*  End of file:  VXLIB_minMaxLoc_ci.cpp                                    */
/* ======================================================================== */
