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

#include "VXLIB_median_priv.h"


/**********************************************************************************************************************/
/*                                                                                                                    */
/* DEFINES                                                                                                           */
/*                                                                                                                    */
/**********************************************************************************************************************/

#define ELE_CNT(x) c7x::element_count_of<x>::value


/* ****************************************************************************************************************** */
/*                                                                                                                    */
/* VXLIB_median_MXN                                                                                                   */
/*                                                                                                                    */
/**********************************************************************************************************************/

template <typename dataType> inline dataType maskVal();

template <> inline uint8_t maskVal() { return 0xFF; }

template <> inline uint16_t maskVal() { return 0xFFFF; }

template <typename V> static inline void vecMinMax(V &vec1, V &vec2, void *scratch)
{
   V vecMin = __min(vec1, vec2);
   V vecMax = __max(vec1, vec2);

   __vpred predStoreMax = c7x::strm_agen<1, V>::get_vpred();
   V *     ptrStoreMax  = c7x::strm_agen<1, V>::get_adv(scratch);
   __vstore_pred(predStoreMax, ptrStoreMax, vecMax);
   vec1 = vecMin;
}
template void vecMinMax<c7x::uchar_vec>(c7x::uchar_vec &vec1, c7x::uchar_vec &vec2, void *scratch);
template void vecMinMax<c7x::char_vec>(c7x::char_vec &vec1, c7x::char_vec &vec2, void *scratch);
template void vecMinMax<c7x::ushort_vec>(c7x::ushort_vec &vec1, c7x::ushort_vec &vec2, void *scratch);
template void vecMinMax<c7x::short_vec>(c7x::short_vec &vec1, c7x::short_vec &vec2, void *scratch);
template void vecMinMax<c7x::long_vec>(c7x::long_vec &vec1, c7x::long_vec &vec2, void *scratch);

template <typename dType>
VXLIB_STATUS VXLIB_median_MXN(VXLIB_kernelHandle handle,
                              void *restrict     pIn,
                              void *restrict     pMask,
                              void *restrict     pOut,
                              void *restrict     pScratch,
                              const int32_t      trueCount)
{
#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_median_MXN_ci\n");
#endif

   VXLIB_STATUS           status       = VXLIB_SUCCESS;
   VXLIB_median_PrivArgs *pKerPrivArgs = (VXLIB_median_PrivArgs *) handle;

   typedef typename c7x::make_full_vector<dType>::type vec;

   typedef typename std::conditional<ELE_CNT(c7x::uchar_vec) == ELE_CNT(vec), uint8_t, uint16_t>::type unsignedType;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   __SE_TEMPLATE_v1 se0Params      = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1Params      = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa0Params      = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se0SortParams  = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_1_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1SortParams  = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa1SortParams  = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA1_1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa2StoreParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA2_1_PARAM_OFFSET);

   int32_t  unrollFactor          = 1;
   size_t   width                 = pKerPrivArgs->width;
   size_t   M                     = pKerPrivArgs->M;
   size_t   N                     = pKerPrivArgs->N;
   size_t   strideInElements      = pKerPrivArgs->strideInElements;
   size_t   strideScratchElements = pKerPrivArgs->strideScratchElements;
   int32_t  widthOut              = pKerPrivArgs->widthOut;
   int32_t  heightOut             = pKerPrivArgs->heightOut;
   uint32_t eleCount              = c7x::element_count_of<vec>::value;
   int32_t  colBlocks             = (width + eleCount - 1) / eleCount;
   int32_t  unrollBlocks          = (colBlocks + unrollFactor - 1) / unrollFactor;
   int32_t  colOutBlocks          = (widthOut + eleCount - 1) / eleCount;
   int32_t  outerLoopCnt          = VXLIB_ceilingDiv(trueCount, 2);
   int32_t  scratchLoopCnt        = M * N * colBlocks;
   int32_t  adjustedTrueCount     = (trueCount == 1) ? 2 : trueCount;
   int32_t  i, j, row, block;

   dType *restrict pInLocal      = (dType *) pIn;
   dType *restrict pMaskLocal    = (dType *) pMask;
   dType *restrict pOutLocal     = (dType *) pOut;
   dType *restrict scratch       = (dType *) pScratch;
   dType *         ptrScratchSE0 = (dType *) pScratch;
   dType *         ptrScratchSE1 = ptrScratchSE0 + strideScratchElements;
   dType *         scratchMedian = &scratch[((trueCount) / 2) * strideScratchElements];

   /* ******************************** MAIN LOOP ****************************************** */

   __SA2_OPEN(sa2StoreParams);
   for (row = 0; row < heightOut; row++) {
      uint32_t rowOffset = row * strideInElements;

      /**************************************************************************
                        STEP-2 :- GET KERNEL IN A SCRATCH BUFFER
       **************************************************************************/

      __SE0_OPEN(pInLocal + rowOffset, se0Params); // printf("SE1 fill scratch\n");
      __SE1_OPEN(pMaskLocal, se1Params);
      __SA0_OPEN(sa0Params);

      for (i = 0; i < scratchLoopCnt; i += 2) {
         unsignedType maskTemp1 = c7x::strm_eng<1, unsignedType>::get_adv();
         vec          v1        = c7x::strm_eng<0, vec>::get_adv();

         if (maskTemp1 == maskVal<unsignedType>()) {
            __vpred predStore1 = c7x::strm_agen<0, vec>::get_vpred();
            vec *   ptrStore1  = c7x::strm_agen<0, vec>::get_adv(scratch);
            __vstore_pred(predStore1, ptrStore1, v1);
         }

         unsignedType maskTemp2 = c7x::strm_eng<1, unsignedType>::get_adv();
         vec          v2        = c7x::strm_eng<0, vec>::get_adv();

         if (maskTemp2 == maskVal<unsignedType>()) {
            __vpred predStore2 = c7x::strm_agen<0, vec>::get_vpred();
            vec *   ptrStore2  = c7x::strm_agen<0, vec>::get_adv(scratch);
            __vstore_pred(predStore2, ptrStore2, v2);
         }
      }
      __SE0_CLOSE();
      __SE1_CLOSE();
      __SA0_CLOSE();

      /**********************************************************
                        STEP-3 :- SORT THE ARRAY
       ***********************************************************/

      uint32_t temp_count = adjustedTrueCount;
      vec      firstVec1;

      for (i = 0; i < outerLoopCnt; i++) {
         se0SortParams.ICNT1 = (temp_count >> 1u) + ((temp_count & 1u));
         se1SortParams.ICNT1 = (temp_count >> 1u);
         sa1SortParams.ICNT1 = temp_count--;

         __SE0_OPEN(ptrScratchSE0, se0SortParams); // printf("SE1 sort\n");
         __SE1_OPEN(ptrScratchSE1, se1SortParams);
         __SA1_OPEN(sa1SortParams);

         for (block = 0; block < unrollBlocks; block++) {

            firstVec1 = c7x::strm_eng<0, vec>::get_adv();

            for (j = i; j < (int32_t)((trueCount - 1) - 1); j += 2) {
               vec v1_1 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec1, v1_1, scratch);

               vec v2_1 = c7x::strm_eng<0, vec>::get_adv();
               vecMinMax<vec>(firstVec1, v2_1, scratch);
            }

            if (j < (trueCount - 1)) {
               vec v1_1 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec1, v1_1, scratch);
            }

            __vpred predStoreMin1 = c7x::strm_agen<1, vec>::get_vpred();
            vec *   ptrStoreMin1  = c7x::strm_agen<1, vec>::get_adv(scratch);
            __vstore_pred(predStoreMin1, ptrStoreMin1, firstVec1);
         }
         __SE0_CLOSE();
         __SE1_CLOSE();
         __SA1_CLOSE();
      }

      for (i = 0; i < colOutBlocks; i++) {
         vec     pStoreOut = *stov_ptr(vec, scratchMedian + (i * eleCount));
         __vpred predOut   = c7x::strm_agen<2, vec>::get_vpred();
         vec *   ptrOut    = c7x::strm_agen<2, vec>::get_adv(pOutLocal);
         __vstore_pred(predOut, ptrOut, pStoreOut);
      }
   }
   __SA2_CLOSE();

   return status;
}
template VXLIB_STATUS VXLIB_median_MXN<VXLIB_MEDIAN_TYPENAME_8U>(VXLIB_kernelHandle handle,
                                                                 void *restrict     pIn,
                                                                 void *restrict     pMask,
                                                                 void *restrict     pOut,
                                                                 void *restrict     pScratch,
                                                                 const int32_t      trueCount);
template VXLIB_STATUS VXLIB_median_MXN<VXLIB_MEDIAN_TYPENAME_8S>(VXLIB_kernelHandle handle,
                                                                 void *restrict     pIn,
                                                                 void *restrict     pMask,
                                                                 void *restrict     pOut,
                                                                 void *restrict     pScratch,
                                                                 const int32_t      trueCount);
template VXLIB_STATUS VXLIB_median_MXN<VXLIB_MEDIAN_TYPENAME_16U>(VXLIB_kernelHandle handle,
                                                                  void *restrict     pIn,
                                                                  void *restrict     pMask,
                                                                  void *restrict     pOut,
                                                                  void *restrict     pScratch,
                                                                  const int32_t      trueCount);
template VXLIB_STATUS VXLIB_median_MXN<VXLIB_MEDIAN_TYPENAME_16S>(VXLIB_kernelHandle handle,
                                                                  void *restrict     pIn,
                                                                  void *restrict     pMask,
                                                                  void *restrict     pOut,
                                                                  void *restrict     pScratch,
                                                                  const int32_t      trueCount);
// template VXLIB_STATUS VXLIB_median_MXN<VXLIB_MEDIAN_TYPENAME_64S>(VXLIB_kernelHandle handle,
//                                                                   void *restrict pIn,
//                                                                   void *restrict pMask,
//                                                                   void *restrict pOut,
//                                                                   void *restrict pScratch,
//                                                                   const int32_t trueCount);

template <typename dType>
VXLIB_STATUS VXLIB_median_MXN_UNROLL2(VXLIB_kernelHandle handle,
                                      void *restrict     pIn,
                                      void *restrict     pMask,
                                      void *restrict     pOut,
                                      void *restrict     pScratch,
                                      const int32_t      trueCount)
{
#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_median_MXN_UNROLL2_ci\n");
#endif

   VXLIB_STATUS           status       = VXLIB_SUCCESS;
   VXLIB_median_PrivArgs *pKerPrivArgs = (VXLIB_median_PrivArgs *) handle;

   // create local pointers
   typedef typename c7x::make_full_vector<dType>::type vec;

   typedef typename std::conditional<ELE_CNT(c7x::uchar_vec) == ELE_CNT(vec), uint8_t, uint16_t>::type unsignedType;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   __SE_TEMPLATE_v1 se0Params      = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1Params      = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa0Params      = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se0SortParams  = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_1_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1SortParams  = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa1SortParams  = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA1_1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa2StoreParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA2_1_PARAM_OFFSET);

   int32_t  unrollFactor          = 2;
   size_t   width                 = pKerPrivArgs->width;
   size_t   M                     = pKerPrivArgs->M;
   size_t   N                     = pKerPrivArgs->N;
   size_t   strideInElements      = pKerPrivArgs->strideInElements;
   size_t   strideScratchElements = pKerPrivArgs->strideScratchElements;
   int32_t  widthOut              = pKerPrivArgs->widthOut;
   int32_t  heightOut             = pKerPrivArgs->heightOut;
   uint32_t eleCount              = c7x::element_count_of<vec>::value;
   int32_t  colBlocks             = (width + eleCount - 1) / eleCount;
   int32_t  unrollBlocks          = (colBlocks + unrollFactor - 1) / unrollFactor;
   int32_t  colOutBlocks          = (widthOut + eleCount - 1) / eleCount;
   int32_t  outerLoopCnt          = VXLIB_ceilingDiv(trueCount, 2);
   int32_t  scratchLoopCnt        = M * N * colBlocks;
   int32_t  adjustedTrueCount     = (trueCount == 1) ? 2 : trueCount;
   int32_t  i, j, row, block;

   dType *restrict pInLocal      = (dType *) pIn;
   dType *restrict pMaskLocal    = (dType *) pMask;
   dType *restrict pOutLocal     = (dType *) pOut;
   dType *restrict scratch       = (dType *) pScratch;
   dType *         ptrScratchSE0 = (dType *) pScratch;
   dType *         ptrScratchSE1 = ptrScratchSE0 + strideScratchElements;
   dType *         scratchMedian = &scratch[((trueCount) / 2) * strideScratchElements];

   /* ******************************** MAIN LOOP ****************************************** */
   __SA2_OPEN(sa2StoreParams);
   for (row = 0; row < heightOut; row++) {
      uint32_t rowOffset = row * strideInElements;

      /**************************************************************************
                        STEP-2 :- GET KERNEL IN A SCRATCH BUFFER
       **************************************************************************/

      __SE0_OPEN(pInLocal + rowOffset, se0Params);
      __SE1_OPEN(pMaskLocal, se1Params);
      __SA0_OPEN(sa0Params);

      for (i = 0; i < scratchLoopCnt; i += 2) {
         unsignedType maskTemp1 = c7x::strm_eng<1, unsignedType>::get_adv();
         vec          v1        = c7x::strm_eng<0, vec>::get_adv();

         if (maskTemp1 == maskVal<unsignedType>()) {
            __vpred predStore1 = c7x::strm_agen<0, vec>::get_vpred();
            vec *   ptrStore1  = c7x::strm_agen<0, vec>::get_adv(scratch);
            __vstore_pred(predStore1, ptrStore1, v1);
         }

         unsignedType maskTemp2 = c7x::strm_eng<1, unsignedType>::get_adv();
         vec          v2        = c7x::strm_eng<0, vec>::get_adv();

         if (maskTemp2 == maskVal<unsignedType>()) {
            __vpred predStore2 = c7x::strm_agen<0, vec>::get_vpred();
            vec *   ptrStore2  = c7x::strm_agen<0, vec>::get_adv(scratch);
            __vstore_pred(predStore2, ptrStore2, v2);
         }
      }
      __SE0_CLOSE();
      __SE1_CLOSE();
      __SA0_CLOSE();

      /**********************************************************
                        STEP-3 :- SORT THE ARRAY
       ***********************************************************/

      uint32_t temp_count = adjustedTrueCount;
      vec      firstVec1, firstVec2;

      for (i = 0; i < outerLoopCnt; i++) {

         se0SortParams.ICNT1 = (temp_count >> 1) + ((temp_count & 1u)); // ceil div by 2
         se1SortParams.ICNT1 = (temp_count >> 1);
         sa1SortParams.ICNT1 = temp_count--;

         __SE0_OPEN(ptrScratchSE0, se0SortParams);
         __SE1_OPEN(ptrScratchSE1, se1SortParams);
         __SA1_OPEN(sa1SortParams);

         for (block = 0; block < unrollBlocks; block++) {

            firstVec1 = c7x::strm_eng<0, vec>::get_adv();
            firstVec2 = c7x::strm_eng<0, vec>::get_adv();

            for (j = i; j < (int32_t)((trueCount - 1) - 1); j += 2) {
               vec v1_1 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec1, v1_1, scratch);
               vec v1_2 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec2, v1_2, scratch);

               vec v2_1 = c7x::strm_eng<0, vec>::get_adv();
               vecMinMax<vec>(firstVec1, v2_1, scratch);
               vec v2_2 = c7x::strm_eng<0, vec>::get_adv();
               vecMinMax<vec>(firstVec2, v2_2, scratch);
            }

            if (j < (trueCount - 1)) {
               vec v1_1 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec1, v1_1, scratch);
               vec v1_2 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec2, v1_2, scratch);
            }

            __vpred predStoreMin1 = c7x::strm_agen<1, vec>::get_vpred();
            vec *   ptrStoreMin1  = c7x::strm_agen<1, vec>::get_adv(scratch);
            __vstore_pred(predStoreMin1, ptrStoreMin1, firstVec1);

            __vpred predStoreMin2 = c7x::strm_agen<1, vec>::get_vpred();
            vec *   ptrStoreMin2  = c7x::strm_agen<1, vec>::get_adv(scratch);
            __vstore_pred(predStoreMin2, ptrStoreMin2, firstVec2);
         }
         __SE0_CLOSE();
         __SE1_CLOSE();
         __SA1_CLOSE();
      }

      for (i = 0; i < colOutBlocks; i++) {
         vec     pStoreOut = *stov_ptr(vec, scratchMedian + (i * eleCount));
         __vpred predOut   = c7x::strm_agen<2, vec>::get_vpred();
         vec *   ptrOut    = c7x::strm_agen<2, vec>::get_adv(pOutLocal);
         __vstore_pred(predOut, ptrOut, pStoreOut);
      }
   }
   __SA2_CLOSE();

   return status;
}
template VXLIB_STATUS VXLIB_median_MXN_UNROLL2<VXLIB_MEDIAN_TYPENAME_8U>(VXLIB_kernelHandle handle,
                                                                         void *restrict     pIn,
                                                                         void *restrict     pMask,
                                                                         void *restrict     pOut,
                                                                         void *restrict     pScratch,
                                                                         const int32_t      trueCount);
template VXLIB_STATUS VXLIB_median_MXN_UNROLL2<VXLIB_MEDIAN_TYPENAME_8S>(VXLIB_kernelHandle handle,
                                                                         void *restrict     pIn,
                                                                         void *restrict     pMask,
                                                                         void *restrict     pOut,
                                                                         void *restrict     pScratch,
                                                                         const int32_t      trueCount);
template VXLIB_STATUS VXLIB_median_MXN_UNROLL2<VXLIB_MEDIAN_TYPENAME_16U>(VXLIB_kernelHandle handle,
                                                                          void *restrict     pIn,
                                                                          void *restrict     pMask,
                                                                          void *restrict     pOut,
                                                                          void *restrict     pScratch,
                                                                          const int32_t      trueCount);
template VXLIB_STATUS VXLIB_median_MXN_UNROLL2<VXLIB_MEDIAN_TYPENAME_16S>(VXLIB_kernelHandle handle,
                                                                          void *restrict     pIn,
                                                                          void *restrict     pMask,
                                                                          void *restrict     pOut,
                                                                          void *restrict     pScratch,
                                                                          const int32_t      trueCount);
// template VXLIB_STATUS VXLIB_median_MXN_UNROLL2<VXLIB_MEDIAN_TYPENAME_64S>(VXLIB_kernelHandle handle,
//                                                                           void *restrict pIn,
//                                                                           void *restrict pMask,
//                                                                           void *restrict pOut,
//                                                                           void *restrict pScratch,
//                                                                           const int32_t trueCount);

template <typename dType>
VXLIB_STATUS VXLIB_median_MXN_UNROLL4(VXLIB_kernelHandle handle,
                                      void *restrict     pIn,
                                      void *restrict     pMask,
                                      void *restrict     pOut,
                                      void *restrict     pScratch,
                                      const int32_t      trueCount)
{
#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_median_MXN_UNROLL4_ci\n");
#endif

   VXLIB_STATUS           status       = VXLIB_SUCCESS;
   VXLIB_median_PrivArgs *pKerPrivArgs = (VXLIB_median_PrivArgs *) handle;

   typedef typename c7x::make_full_vector<dType>::type vec;

   typedef typename std::conditional<ELE_CNT(c7x::uchar_vec) == ELE_CNT(vec), uint8_t, uint16_t>::type unsignedType;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   __SE_TEMPLATE_v1 se0Params      = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1Params      = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa0Params      = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se0SortParams  = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_1_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1SortParams  = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa1SortParams  = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA1_1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa2StoreParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA2_1_PARAM_OFFSET);

   int32_t  unrollFactor          = 4;
   size_t   width                 = pKerPrivArgs->width;
   size_t   M                     = pKerPrivArgs->M;
   size_t   N                     = pKerPrivArgs->N;
   size_t   strideInElements      = pKerPrivArgs->strideInElements;
   size_t   strideScratchElements = pKerPrivArgs->strideScratchElements;
   int32_t  widthOut              = pKerPrivArgs->widthOut;
   int32_t  heightOut             = pKerPrivArgs->heightOut;
   uint32_t eleCount              = c7x::element_count_of<vec>::value;
   int32_t  colBlocks             = (width + eleCount - 1) / eleCount;
   int32_t  unrollBlocks          = (colBlocks + unrollFactor - 1) / unrollFactor;
   int32_t  colOutBlocks          = (widthOut + eleCount - 1) / eleCount;
   int32_t  outerLoopCnt          = VXLIB_ceilingDiv(trueCount, 2);
   int32_t  scratchLoopCnt        = M * N * colBlocks;
   int32_t  adjustedTrueCount     = (trueCount == 1) ? 2 : trueCount;
   int32_t  i, j, row, block;

   // create local pointers
   dType *restrict pInLocal      = (dType *) pIn;
   dType *restrict pMaskLocal    = (dType *) pMask;
   dType *restrict pOutLocal     = (dType *) pOut;
   dType *restrict scratch       = (dType *) pScratch;
   dType *         ptrScratchSE0 = (dType *) pScratch;
   dType *         ptrScratchSE1 = ptrScratchSE0 + strideScratchElements;
   dType *         scratchMedian = &scratch[((trueCount) / 2) * strideScratchElements];

   /* ******************************** MAIN LOOP ****************************************** */
   __SA2_OPEN(sa2StoreParams);
   for (row = 0; row < heightOut; row++) {
      uint32_t rowOffset = row * strideInElements;

      /**************************************************************************
                        STEP-2 :- GET KERNEL IN A SCRATCH BUFFER
       **************************************************************************/

      __SE0_OPEN(pInLocal + rowOffset, se0Params);
      __SE1_OPEN(pMaskLocal, se1Params);
      __SA0_OPEN(sa0Params);

      for (i = 0; i < scratchLoopCnt; i += 2) {
         unsignedType maskTemp1 = c7x::strm_eng<1, unsignedType>::get_adv();
         vec          v1        = c7x::strm_eng<0, vec>::get_adv();

         if (maskTemp1 == maskVal<unsignedType>()) {
            __vpred predStore1 = c7x::strm_agen<0, vec>::get_vpred();
            vec *   ptrStore1  = c7x::strm_agen<0, vec>::get_adv(scratch);
            __vstore_pred(predStore1, ptrStore1, v1);
         }

         unsignedType maskTemp2 = c7x::strm_eng<1, unsignedType>::get_adv();
         vec          v2        = c7x::strm_eng<0, vec>::get_adv();

         if (maskTemp2 == maskVal<unsignedType>()) {
            __vpred predStore2 = c7x::strm_agen<0, vec>::get_vpred();
            vec *   ptrStore2  = c7x::strm_agen<0, vec>::get_adv(scratch);
            __vstore_pred(predStore2, ptrStore2, v2);
         }
      }
      __SE0_CLOSE();
      __SE1_CLOSE();
      __SA0_CLOSE();

      /**********************************************************
                        STEP-3 :- SORT THE ARRAY
       ***********************************************************/

      uint32_t temp_count = adjustedTrueCount;
      vec      firstVec1, firstVec2, firstVec3, firstVec4;

      for (i = 0; i < outerLoopCnt; i++) {
         se0SortParams.ICNT1 = (temp_count >> 1) + ((temp_count & 1u));
         se1SortParams.ICNT1 = (temp_count >> 1);
         sa1SortParams.ICNT1 = temp_count--;

         __SE0_OPEN(ptrScratchSE0, se0SortParams);
         __SE1_OPEN(ptrScratchSE1, se1SortParams);
         __SA1_OPEN(sa1SortParams);

         for (block = 0; block < unrollBlocks; block++) {

            firstVec1 = c7x::strm_eng<0, vec>::get_adv();
            firstVec2 = c7x::strm_eng<0, vec>::get_adv();
            firstVec3 = c7x::strm_eng<0, vec>::get_adv();
            firstVec4 = c7x::strm_eng<0, vec>::get_adv();

            for (j = i; j < (int32_t)((trueCount - 1) - 1); j += 2) {
               vec v1_1 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec1, v1_1, scratch);
               vec v1_2 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec2, v1_2, scratch);
               vec v1_3 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec3, v1_3, scratch);
               vec v1_4 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec4, v1_4, scratch);

               vec v2_1 = c7x::strm_eng<0, vec>::get_adv();
               vecMinMax<vec>(firstVec1, v2_1, scratch);
               vec v2_2 = c7x::strm_eng<0, vec>::get_adv();
               vecMinMax<vec>(firstVec2, v2_2, scratch);
               vec v2_3 = c7x::strm_eng<0, vec>::get_adv();
               vecMinMax<vec>(firstVec3, v2_3, scratch);
               vec v2_4 = c7x::strm_eng<0, vec>::get_adv();
               vecMinMax<vec>(firstVec4, v2_4, scratch);
            }

            if (j < (trueCount - 1)) {
               vec v1_1 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec1, v1_1, scratch);
               vec v1_2 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec2, v1_2, scratch);
               vec v1_3 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec3, v1_3, scratch);
               vec v1_4 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec4, v1_4, scratch);
            }

            __vpred predStoreMin1 = c7x::strm_agen<1, vec>::get_vpred();
            vec *   ptrStoreMin1  = c7x::strm_agen<1, vec>::get_adv(scratch);
            __vstore_pred(predStoreMin1, ptrStoreMin1, firstVec1);

            __vpred predStoreMin2 = c7x::strm_agen<1, vec>::get_vpred();
            vec *   ptrStoreMin2  = c7x::strm_agen<1, vec>::get_adv(scratch);
            __vstore_pred(predStoreMin2, ptrStoreMin2, firstVec2);

            __vpred predStoreMin3 = c7x::strm_agen<1, vec>::get_vpred();
            vec *   ptrStoreMin3  = c7x::strm_agen<1, vec>::get_adv(scratch);
            __vstore_pred(predStoreMin3, ptrStoreMin3, firstVec3);

            __vpred predStoreMin4 = c7x::strm_agen<1, vec>::get_vpred();
            vec *   ptrStoreMin4  = c7x::strm_agen<1, vec>::get_adv(scratch);
            __vstore_pred(predStoreMin4, ptrStoreMin4, firstVec4);
         }
         __SE0_CLOSE();
         __SE1_CLOSE();
         __SA1_CLOSE();
      }

      for (i = 0; i < colOutBlocks; i++) {
         vec     pStoreOut = *stov_ptr(vec, scratchMedian + (i * eleCount));
         __vpred predOut   = c7x::strm_agen<2, vec>::get_vpred();
         vec *   ptrOut    = c7x::strm_agen<2, vec>::get_adv(pOutLocal);
         __vstore_pred(predOut, ptrOut, pStoreOut);
      }
   }
   __SA2_CLOSE();

   return status;
}
template VXLIB_STATUS VXLIB_median_MXN_UNROLL4<VXLIB_MEDIAN_TYPENAME_8U>(VXLIB_kernelHandle handle,
                                                                         void *restrict     pIn,
                                                                         void *restrict     pMask,
                                                                         void *restrict     pOut,
                                                                         void *restrict     pScratch,
                                                                         const int32_t      trueCount);
template VXLIB_STATUS VXLIB_median_MXN_UNROLL4<VXLIB_MEDIAN_TYPENAME_8S>(VXLIB_kernelHandle handle,
                                                                         void *restrict     pIn,
                                                                         void *restrict     pMask,
                                                                         void *restrict     pOut,
                                                                         void *restrict     pScratch,
                                                                         const int32_t      trueCount);
template VXLIB_STATUS VXLIB_median_MXN_UNROLL4<VXLIB_MEDIAN_TYPENAME_16U>(VXLIB_kernelHandle handle,
                                                                          void *restrict     pIn,
                                                                          void *restrict     pMask,
                                                                          void *restrict     pOut,
                                                                          void *restrict     pScratch,
                                                                          const int32_t      trueCount);
template VXLIB_STATUS VXLIB_median_MXN_UNROLL4<VXLIB_MEDIAN_TYPENAME_16S>(VXLIB_kernelHandle handle,
                                                                          void *restrict     pIn,
                                                                          void *restrict     pMask,
                                                                          void *restrict     pOut,
                                                                          void *restrict     pScratch,
                                                                          const int32_t      trueCount);
// template VXLIB_STATUS VXLIB_median_MXN_UNROLL4<VXLIB_MEDIAN_TYPENAME_64S>(VXLIB_kernelHandle handle,
//                                                                           void *restrict pIn,
//                                                                           void *restrict pMask,
//                                                                           void *restrict pOut,
//                                                                           void *restrict pScratch,
//                                                                           const int32_t trueCount);

template <typename dType>
VXLIB_STATUS VXLIB_median_MXN_UNROLL8(VXLIB_kernelHandle handle,
                                      void *restrict     pIn,
                                      void *restrict     pMask,
                                      void *restrict     pOut,
                                      void *restrict     pScratch,
                                      const int32_t      trueCount)
{
#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_median_MXN_UNROLL8_ci\n");
#endif

   VXLIB_STATUS           status       = VXLIB_SUCCESS;
   VXLIB_median_PrivArgs *pKerPrivArgs = (VXLIB_median_PrivArgs *) handle;

   typedef typename c7x::make_full_vector<dType>::type vec;

   typedef typename std::conditional<ELE_CNT(c7x::uchar_vec) == ELE_CNT(vec), uint8_t, uint16_t>::type unsignedType;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   __SE_TEMPLATE_v1 se0Params      = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1Params      = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa0Params      = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se0SortParams  = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_1_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1SortParams  = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa1SortParams  = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA1_1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa2StoreParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA2_1_PARAM_OFFSET);

   int32_t  unrollFactor          = 8;
   size_t   width                 = pKerPrivArgs->width;
   size_t   M                     = pKerPrivArgs->M;
   size_t   N                     = pKerPrivArgs->N;
   size_t   strideInElements      = pKerPrivArgs->strideInElements;
   size_t   strideScratchElements = pKerPrivArgs->strideScratchElements;
   int32_t  widthOut              = pKerPrivArgs->widthOut;
   int32_t  heightOut             = pKerPrivArgs->heightOut;
   uint32_t eleCount              = c7x::element_count_of<vec>::value;
   int32_t  colBlocks             = (width + eleCount - 1) / eleCount;
   int32_t  unrollBlocks          = (colBlocks + unrollFactor - 1) / unrollFactor;
   int32_t  colOutBlocks          = (widthOut + eleCount - 1) / eleCount;
   int32_t  outerLoopCnt          = VXLIB_ceilingDiv(trueCount, 2);
   int32_t  scratchLoopCnt        = M * N * colBlocks;
   int32_t  adjustedTrueCount     = (trueCount == 1) ? 2 : trueCount;
   int32_t  i, j, row, block;

   // create local pointers
   dType *restrict pInLocal      = (dType *) pIn;
   dType *restrict pMaskLocal    = (dType *) pMask;
   dType *restrict pOutLocal     = (dType *) pOut;
   dType *restrict scratch       = (dType *) pScratch;
   dType *         ptrScratchSE0 = (dType *) pScratch;
   dType *         ptrScratchSE1 = ptrScratchSE0 + strideScratchElements;
   dType *         scratchMedian = &scratch[((trueCount) / 2) * strideScratchElements];

   /* ******************************** MAIN LOOP ****************************************** */

   __SA2_OPEN(sa2StoreParams);
   for (row = 0; row < heightOut; row++) {
      uint32_t rowOffset = row * strideInElements;

      /**************************************************************************
                        STEP-2 :- GET KERNEL IN A SCRATCH BUFFER
       **************************************************************************/

      __SE0_OPEN(pInLocal + rowOffset, se0Params);
      __SE1_OPEN(pMaskLocal, se1Params);
      __SA0_OPEN(sa0Params);

      for (i = 0; i < scratchLoopCnt; i += 2) {
         unsignedType maskTemp1 = c7x::strm_eng<1, unsignedType>::get_adv();
         vec          v1        = c7x::strm_eng<0, vec>::get_adv();

         if (maskTemp1 == maskVal<unsignedType>()) {
            __vpred predStore1 = c7x::strm_agen<0, vec>::get_vpred();
            vec *   ptrStore1  = c7x::strm_agen<0, vec>::get_adv(scratch);
            __vstore_pred(predStore1, ptrStore1, v1);
         }

         unsignedType maskTemp2 = c7x::strm_eng<1, unsignedType>::get_adv();
         vec          v2        = c7x::strm_eng<0, vec>::get_adv();

         if (maskTemp2 == maskVal<unsignedType>()) {
            __vpred predStore2 = c7x::strm_agen<0, vec>::get_vpred();
            vec *   ptrStore2  = c7x::strm_agen<0, vec>::get_adv(scratch);
            __vstore_pred(predStore2, ptrStore2, v2);
         }
      }
      __SE0_CLOSE();
      __SE1_CLOSE();
      __SA0_CLOSE();

      /**********************************************************
                        STEP-3 :- SORT THE ARRAY
       ***********************************************************/

      uint32_t temp_count = adjustedTrueCount;
      vec      firstVec1, firstVec2, firstVec3, firstVec4;
      vec      firstVec5, firstVec6, firstVec7, firstVec8;

      for (i = 0; i < outerLoopCnt; i++) {
         se0SortParams.ICNT1 = (temp_count >> 1) + ((temp_count & 1u));
         se1SortParams.ICNT1 = (temp_count >> 1);
         sa1SortParams.ICNT1 = temp_count--;

         __SE0_OPEN(ptrScratchSE0, se0SortParams);
         __SE1_OPEN(ptrScratchSE1, se1SortParams);
         __SA1_OPEN(sa1SortParams);

         for (block = 0; block < unrollBlocks; block++) {

            firstVec1 = c7x::strm_eng<0, vec>::get_adv();
            firstVec2 = c7x::strm_eng<0, vec>::get_adv();
            firstVec3 = c7x::strm_eng<0, vec>::get_adv();
            firstVec4 = c7x::strm_eng<0, vec>::get_adv();
            firstVec5 = c7x::strm_eng<0, vec>::get_adv();
            firstVec6 = c7x::strm_eng<0, vec>::get_adv();
            firstVec7 = c7x::strm_eng<0, vec>::get_adv();
            firstVec8 = c7x::strm_eng<0, vec>::get_adv();

            for (j = i; j < (int32_t)((trueCount - 1) - 1); j += 2) {
               vec v1_1 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec1, v1_1, scratch);
               vec v1_2 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec2, v1_2, scratch);
               vec v1_3 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec3, v1_3, scratch);
               vec v1_4 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec4, v1_4, scratch);
               vec v1_5 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec5, v1_5, scratch);
               vec v1_6 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec6, v1_6, scratch);
               vec v1_7 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec7, v1_7, scratch);
               vec v1_8 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec8, v1_8, scratch);

               vec v2_1 = c7x::strm_eng<0, vec>::get_adv();
               vecMinMax<vec>(firstVec1, v2_1, scratch);
               vec v2_2 = c7x::strm_eng<0, vec>::get_adv();
               vecMinMax<vec>(firstVec2, v2_2, scratch);
               vec v2_3 = c7x::strm_eng<0, vec>::get_adv();
               vecMinMax<vec>(firstVec3, v2_3, scratch);
               vec v2_4 = c7x::strm_eng<0, vec>::get_adv();
               vecMinMax<vec>(firstVec4, v2_4, scratch);
               vec v2_5 = c7x::strm_eng<0, vec>::get_adv();
               vecMinMax<vec>(firstVec5, v2_5, scratch);
               vec v2_6 = c7x::strm_eng<0, vec>::get_adv();
               vecMinMax<vec>(firstVec6, v2_6, scratch);
               vec v2_7 = c7x::strm_eng<0, vec>::get_adv();
               vecMinMax<vec>(firstVec7, v2_7, scratch);
               vec v2_8 = c7x::strm_eng<0, vec>::get_adv();
               vecMinMax<vec>(firstVec8, v2_8, scratch);
            }

            if (j < (trueCount - 1)) {
               vec v1_1 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec1, v1_1, scratch);
               vec v1_2 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec2, v1_2, scratch);
               vec v1_3 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec3, v1_3, scratch);
               vec v1_4 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec4, v1_4, scratch);
               vec v1_5 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec5, v1_5, scratch);
               vec v1_6 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec6, v1_6, scratch);
               vec v1_7 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec7, v1_7, scratch);
               vec v1_8 = c7x::strm_eng<1, vec>::get_adv();
               vecMinMax<vec>(firstVec8, v1_8, scratch);
            }

            __vpred predStoreMin1 = c7x::strm_agen<1, vec>::get_vpred();
            vec *   ptrStoreMin1  = c7x::strm_agen<1, vec>::get_adv(scratch);
            __vstore_pred(predStoreMin1, ptrStoreMin1, firstVec1);

            __vpred predStoreMin2 = c7x::strm_agen<1, vec>::get_vpred();
            vec *   ptrStoreMin2  = c7x::strm_agen<1, vec>::get_adv(scratch);
            __vstore_pred(predStoreMin2, ptrStoreMin2, firstVec2);

            __vpred predStoreMin3 = c7x::strm_agen<1, vec>::get_vpred();
            vec *   ptrStoreMin3  = c7x::strm_agen<1, vec>::get_adv(scratch);
            __vstore_pred(predStoreMin3, ptrStoreMin3, firstVec3);

            __vpred predStoreMin4 = c7x::strm_agen<1, vec>::get_vpred();
            vec *   ptrStoreMin4  = c7x::strm_agen<1, vec>::get_adv(scratch);
            __vstore_pred(predStoreMin4, ptrStoreMin4, firstVec4);

            __vpred predStoreMin5 = c7x::strm_agen<1, vec>::get_vpred();
            vec *   ptrStoreMin5  = c7x::strm_agen<1, vec>::get_adv(scratch);
            __vstore_pred(predStoreMin5, ptrStoreMin5, firstVec5);

            __vpred predStoreMin6 = c7x::strm_agen<1, vec>::get_vpred();
            vec *   ptrStoreMin6  = c7x::strm_agen<1, vec>::get_adv(scratch);
            __vstore_pred(predStoreMin6, ptrStoreMin6, firstVec6);

            __vpred predStoreMin7 = c7x::strm_agen<1, vec>::get_vpred();
            vec *   ptrStoreMin7  = c7x::strm_agen<1, vec>::get_adv(scratch);
            __vstore_pred(predStoreMin7, ptrStoreMin7, firstVec7);

            __vpred predStoreMin8 = c7x::strm_agen<1, vec>::get_vpred();
            vec *   ptrStoreMin8  = c7x::strm_agen<1, vec>::get_adv(scratch);
            __vstore_pred(predStoreMin8, ptrStoreMin8, firstVec8);
         }
         __SE0_CLOSE();
         __SE1_CLOSE();
         __SA1_CLOSE();
      }

      for (i = 0; i < colOutBlocks; i++) {
         vec     pStoreOut = *stov_ptr(vec, scratchMedian + (i * eleCount));
         __vpred predOut   = c7x::strm_agen<2, vec>::get_vpred();
         vec *   ptrOut    = c7x::strm_agen<2, vec>::get_adv(pOutLocal);
         __vstore_pred(predOut, ptrOut, pStoreOut);
      }
   }

   __SA2_CLOSE();

   return status;
}
template VXLIB_STATUS VXLIB_median_MXN_UNROLL8<VXLIB_MEDIAN_TYPENAME_8U>(VXLIB_kernelHandle handle,
                                                                         void *restrict     pIn,
                                                                         void *restrict     pMask,
                                                                         void *restrict     pOut,
                                                                         void *restrict     pScratch,
                                                                         const int32_t      trueCount);
template VXLIB_STATUS VXLIB_median_MXN_UNROLL8<VXLIB_MEDIAN_TYPENAME_8S>(VXLIB_kernelHandle handle,
                                                                         void *restrict     pIn,
                                                                         void *restrict     pMask,
                                                                         void *restrict     pOut,
                                                                         void *restrict     pScratch,
                                                                         const int32_t      trueCount);
template VXLIB_STATUS VXLIB_median_MXN_UNROLL8<VXLIB_MEDIAN_TYPENAME_16U>(VXLIB_kernelHandle handle,
                                                                          void *restrict     pIn,
                                                                          void *restrict     pMask,
                                                                          void *restrict     pOut,
                                                                          void *restrict     pScratch,
                                                                          const int32_t      trueCount);
template VXLIB_STATUS VXLIB_median_MXN_UNROLL8<VXLIB_MEDIAN_TYPENAME_16S>(VXLIB_kernelHandle handle,
                                                                          void *restrict     pIn,
                                                                          void *restrict     pMask,
                                                                          void *restrict     pOut,
                                                                          void *restrict     pScratch,
                                                                          const int32_t      trueCount);
// template VXLIB_STATUS VXLIB_median_MXN_UNROLL8<VXLIB_MEDIAN_TYPENAME_64S>(VXLIB_kernelHandle handle,
//                                                                           void *restrict pIn,
//                                                                           void *restrict pMask,
//                                                                           void *restrict pOut,
//                                                                           void *restrict pScratch,
//                                                                           const int32_t trueCount);

/* ======================================================================== */
/*  End of file:  VXLIB_median_MXN_exec.cpp                                     */
/* ======================================================================== */
