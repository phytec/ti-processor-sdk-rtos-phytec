/******************************************************************************/
/*!
 * \file DSPLIB_cholesky_ci.cpp
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

/******************************************************************************
 * Version 1.0  Date 10/2/22      Author: Asheesh Bhardwaj
 *****************************************************************************/

/*******************************************************************************
 *
 * INCLUDES
 *
 ******************************************************************************/

#include "DSPLIB_cholesky_priv.h"

/*******************************************************************************
 *
 * DEFINES
 *
 ******************************************************************************/

#define UNROLL_COUNT 4
#define MIN_HORIZONTAL_COLUMNS_FOR_UNROLL 2
#define NUM_VECS_IN_TILE 6

/*******************************************************************************
 *
 * INITIALIZATION
 *
 ******************************************************************************/

template <typename dataType> DSPLIB_STATUS DSPLIB_cholesky_c7x_PingPong_init(DSPLIB_kernelHandle handle)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   __SE_TEMPLATE_v1 seParamFetchL;   // Left fetch
   __SE_TEMPLATE_v1 seParamFetchR;   // Right fetch
   __SA_TEMPLATE_v1 saParamMulStore; // Stores the multiplier values
   __SA_TEMPLATE_v1 saParamLStore;   // Stores the L values

   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;
   __SA_VECLEN  SA_VECLEN;

   DSPLIB_cholesky_PrivArgs *pKerPrivArgs = (DSPLIB_cholesky_PrivArgs *) handle;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   SE_VECLEN  = c7x::se_veclen<vec>::value;
   SE_ELETYPE = c7x::se_eletype<vec>::value;
   SA_VECLEN  = c7x::sa_veclen<vec>::value;

   uint32_t eleCount = c7x::element_count_of<vec>::value;

   pKerPrivArgs->shiftForVecLenDiv = -1;
   uint32_t vecLenValue             = eleCount;
   while (vecLenValue != 0) {
      vecLenValue >>= 1;
      pKerPrivArgs->shiftForVecLenDiv++;
   }

   int32_t yStride = pKerPrivArgs->stride / sizeof(dataType);
   /**********************************************************************/
   /* Prepare streaming engine for fetching L values(Left) Merge approach*/
   /**********************************************************************/
   seParamFetchL = __gen_SE_TEMPLATE_v1();

   seParamFetchL.ICNT0 = eleCount;
   seParamFetchL.ICNT1 = 0;       // No of rows to process
   seParamFetchL.DIM1  = yStride; // order
   seParamFetchL.ICNT2 = 0;       // No of left fetches
   seParamFetchL.DIM2  = eleCount << 1;

   seParamFetchL.ELETYPE = SE_ELETYPE;
   seParamFetchL.VECLEN  = SE_VECLEN;
   seParamFetchL.DIMFMT  = __SE_DIMFMT_3D;

   /**********************************************************************/
   /* Prepare streaming engine for fetching L values(Right) Merge approach*/
   /**********************************************************************/
   seParamFetchR = __gen_SE_TEMPLATE_v1();

   seParamFetchR.ICNT0 = eleCount;
   seParamFetchR.ICNT1 = 0; // No of rows to process
   seParamFetchR.DIM1  = yStride;
   seParamFetchR.ICNT2 = 0; // No of right fetches
   seParamFetchR.DIM2  = eleCount << 1;

   seParamFetchR.ELETYPE = SE_ELETYPE;
   seParamFetchR.VECLEN  = SE_VECLEN;
   seParamFetchR.DIMFMT  = __SE_DIMFMT_3D;

   /**********************************************************************/
   /* Prepare Address generator to store the multipliers                 */
   /**********************************************************************/
   saParamMulStore = __gen_SA_TEMPLATE_v1();

   saParamMulStore.ICNT0  = pKerPrivArgs->order;
   saParamMulStore.VECLEN = __SA_VECLEN_1ELEM;
   saParamMulStore.DIMFMT = __SA_DIMFMT_1D;

   /**********************************************************************/
   /* Prepare Address generator to store the L values                    */
   /**********************************************************************/
   saParamLStore = __gen_SA_TEMPLATE_v1();

   saParamLStore.ICNT0  = pKerPrivArgs->order;
   saParamLStore.VECLEN = SA_VECLEN;
   saParamLStore.DIMFMT = __SA_DIMFMT_1D;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pKerPrivArgs->bufPblock + SE_SE2_PARAM_OFFSET) = seParamFetchL;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pKerPrivArgs->bufPblock + SE_SE3_PARAM_OFFSET) = seParamFetchR;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pKerPrivArgs->bufPblock + SA_SA0_PARAM_OFFSET) = saParamMulStore;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pKerPrivArgs->bufPblock + SA_SA1_PARAM_OFFSET) = saParamLStore;

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
   return status;
}

template DSPLIB_STATUS DSPLIB_cholesky_c7x_PingPong_init<float>(DSPLIB_kernelHandle handle);
// template DSPLIB_STATUS DSPLIB_cholesky_c7x_PingPong_init<double>(
//     DSPLIB_kernelHandle handle);

// Initialize SE params
template <typename dataType>
DSPLIB_STATUS DSPLIB_cholesky_init_ci(DSPLIB_kernelHandle             handle,
                                      const DSPLIB_bufParams2D_t     *bufParamsIn,
                                      const DSPLIB_bufParams2D_t     *bufParamsOut,
                                      const DSPLIB_cholesky_InitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS             status       = DSPLIB_SUCCESS;
   DSPLIB_cholesky_PrivArgs *pKerPrivArgs = (DSPLIB_cholesky_PrivArgs *) handle;
   uint8_t                  *pBlock       = pKerPrivArgs->bufPblock;
   int32_t                   order        = pKerPrivArgs->order;
   int32_t                   strideA      = pKerPrivArgs->stride;
   int32_t                   colAStride   = strideA / sizeof(dataType);

   DSPLIB_cholesky_c7x_PingPong_init<dataType>(handle);
   DSPLIB_cholesky_inplace_isPosDefinite_init<dataType>(order, colAStride, pBlock);

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");

   return status;
}

template DSPLIB_STATUS DSPLIB_cholesky_init_ci<float>(DSPLIB_kernelHandle             handle,
                                                      const DSPLIB_bufParams2D_t     *bufParamsIn,
                                                      const DSPLIB_bufParams2D_t     *bufParamsOut,
                                                      const DSPLIB_cholesky_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_cholesky_init_ci<double>(DSPLIB_kernelHandle             handle,
                                                       const DSPLIB_bufParams2D_t     *bufParamsIn,
                                                       const DSPLIB_bufParams2D_t     *bufParamsOut,
                                                       const DSPLIB_cholesky_InitArgs *pKerInitArgs);

/*******************************************************************************
 *
 * IMPLEMENTATION
 *
 ******************************************************************************/

template <typename dataType> inline dataType getRecipSqrt(dataType a)
{

   const dataType Half  = 0.5f;
   const dataType OneP5 = 1.5f;
   dataType       x;

   x = __recip_sqrt(a); // compute square root reciprocal

   x = x * (OneP5 - (a * x * x * Half));
   x = x * (OneP5 - (a * x * x * Half));
   // PRAGMA: do not unroll this loop
   // int i;
   // #pragma UNROLL(1)
   //     for (i = 0; i < 2; i++)
   //     {
   //         x = x * (OneP5 - (a * x * x * Half));
   //     }

   return x;
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_cholesky_c7x_PingPong(int                       enable_test,
                                           DSPLIB_cholesky_PrivArgs *pKerPrivArgs,
                                           dataType *restrict pInALocal,
                                           dataType *restrict pOutULocal,
                                           dataType *restrict pMulBuffer)
{

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   __SE_TEMPLATE_v1 seParamFetchL = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pKerPrivArgs->bufPblock + SE_SE2_PARAM_OFFSET);
   __SE_TEMPLATE_v1 seParamFetchR = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pKerPrivArgs->bufPblock + SE_SE3_PARAM_OFFSET);

   __SA_TEMPLATE_v1 saParamLStore = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pKerPrivArgs->bufPblock + SA_SA1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 saParamALoad  = saParamLStore;

   int32_t order  = pKerPrivArgs->order;
   int32_t vecLen = eleCount;

   int32_t row, fetch, lRow;
   int32_t shiftForVecLenDiv = pKerPrivArgs->shiftForVecLenDiv;
   int32_t stride            = pKerPrivArgs->stride;
   int32_t yStride           = stride / sizeof(dataType);

   dataType *pLFirstRow = pOutULocal;
   dataType  recipDiagValue;

   uchar64 vMask;
   uchar64 vMaskInit;
   vMaskInit.s[0] = 0;
   vMaskInit.s[1] = 1;
   vMaskInit.s[2] = 2;
   vMaskInit.s[3] = 3;
   uchar64 vMaskIncrement = (uchar64) 4; 
   int32_t blockMax  = int32_t((uint32_t)(order + vecLen - 1) >> (uint32_t)shiftForVecLenDiv);
   int32_t extraRows = vecLen - (int32_t)((uint32_t)order & (uint32_t)(vecLen - 1)); // gives extra rows needed
                                                        // to make the height of matrix
                                                        // integral multiple of vecLen
   if (extraRows == vecLen){
     extraRows = 0;
   }
      
   int32_t elemsPerRow = order;
   int32_t offset      = 0;
   int32_t rowNumber   = 0;
   int32_t block;
   int32_t elemsPerRowCeil = elemsPerRow + vecLen - 1;

   int32_t   lezrCount[UNROLL_COUNT];
   __SE_LEZR lezrDim[UNROLL_COUNT];

   int32_t   *lezrCountPtr = lezrCount;
   __SE_LEZR *lezrDimPtr   = lezrDim;

   *lezrCountPtr = 0;
   *lezrDimPtr   = __SE_LEZR_OFF;
   lezrCountPtr++;
   lezrDimPtr++;

   for (int32_t i = UNROLL_COUNT - 1; i > 0; i--) {
      *lezrCountPtr = i;
      *lezrDimPtr   = __SE_LEZR_ICNT1;
      lezrCountPtr++;
      lezrDimPtr++;
   }

   for (block = 0; block < blockMax - 2; block++) {
      // configuration for sa
      saParamLStore.ICNT0 = elemsPerRow;
      saParamALoad.ICNT0  = elemsPerRow;

      vMask = vMaskInit;
      for (row = 0; row < vecLen; row++) {
         int32_t fetchesPerRow      = (int32_t)((uint32_t)elemsPerRowCeil >> (uint32_t)shiftForVecLenDiv); // number of vector fetches per row
         int32_t leftFetchesPerRow  = (int32_t)((uint32_t)(fetchesPerRow + 1) >> 1u);               // number of fetches by SE0
         int32_t rightFetchesPerRow = fetchesPerRow - leftFetchesPerRow;      // number of fetches by SE1
         int32_t lezrIndex = (int32_t)((uint32_t)rowNumber & (uint32_t)(UNROLL_COUNT - 1)); // rowNumber%4=>4 is unroll count

         // configuration for SE
         seParamFetchL.ICNT1    = rowNumber;
         seParamFetchL.ICNT2    = leftFetchesPerRow;
         seParamFetchL.LEZR     = lezrDim[lezrIndex];
         seParamFetchL.LEZR_CNT = lezrCount[lezrIndex];

         seParamFetchR.ICNT1    = rowNumber;
         seParamFetchR.ICNT2    = rightFetchesPerRow;
         seParamFetchR.LEZR     = lezrDim[lezrIndex];
         seParamFetchR.LEZR_CNT = lezrCount[lezrIndex];

         if (rowNumber > 0) {
            __SE0_OPEN(pLFirstRow, seParamFetchL);
            __SE1_OPEN(pLFirstRow + vecLen, seParamFetchR);
         }

         __SA1_OPEN(saParamLStore);
         __SA2_OPEN(saParamALoad);

         dataType *ptrL = pOutULocal + offset;
         dataType *ptrA = pInALocal + offset;

         vec vLSum0 = vec(0); // Holds sum of all LxLy
         vec vLSum1 = vec(0);
         vec vLSum2 = vec(0);
         vec vLSum3 = vec(0);

         vec vLA = *(c7x::strm_agen<2, vec>::get_adv(ptrA));

         vec vRSum0 = vec(0); // Holds sum of all LxLy
         vec vRSum1 = vec(0);
         vec vRSum2 = vec(0);
         vec vRSum3 = vec(0);

         vec vRA = *(c7x::strm_agen<2, vec>::get_adv(ptrA));

         dataType *pMulStore = pMulBuffer;

         for (lRow = 0; lRow < rowNumber; lRow += UNROLL_COUNT) {
            vec vLL0     = c7x::strm_eng<0, vec>::get_adv(); // L value vector from each row
            vec vLL0Temp = __as_float16(__permute(vMask, __as_uchar64(vLL0)));
            vLSum0 += vLL0 * vLL0Temp.s[0];
            vec vLR0 = c7x::strm_eng<1, vec>::get_adv(); // L value vector from each row
            vRSum0 += vLR0 * vLL0Temp.s[0];
            *pMulStore = vLL0Temp.s[0];
            pMulStore++;

            vec vLL1     = c7x::strm_eng<0, vec>::get_adv();
            vec vLL1Temp = __as_float16(__permute(vMask, __as_uchar64(vLL1)));
            vLSum1 += vLL1 * vLL1Temp.s[0];
            vec vLR1 = c7x::strm_eng<1, vec>::get_adv(); // L value vector from each row
            vRSum1 += vLR1 * vLL1Temp.s[0];
            *pMulStore = vLL1Temp.s[0];
            pMulStore++;

            vec vLL2     = c7x::strm_eng<0, vec>::get_adv();
            vec vLL2Temp = __as_float16(__permute(vMask, __as_uchar64(vLL2)));
            vLSum2 += vLL2 * vLL2Temp.s[0];
            vec vLR2 = c7x::strm_eng<1, vec>::get_adv(); // L value vector from each row
            vRSum2 += vLR2 * vLL2Temp.s[0];
            *pMulStore = vLL2Temp.s[0];
            pMulStore++;

            vec vLL3     = c7x::strm_eng<0, vec>::get_adv();
            vec vLL3Temp = __as_float16(__permute(vMask, __as_uchar64(vLL3)));
            vLSum3 += vLL3 * vLL3Temp.s[0];
            vec vLR3 = c7x::strm_eng<1, vec>::get_adv(); // L value vector from each row
            vRSum3 += vLR3 * vLL3Temp.s[0];
            *pMulStore = vLL3Temp.s[0];
            pMulStore++;
         }

         vLSum0 += vLSum1;
         vLSum2 += vLSum3;

         vRSum0 += vRSum1;
         vRSum2 += vRSum3;

         vLA -= vLSum2;
         vec vLDiff = vLA - vLSum0;

         vRA -= vRSum2;
         vec vRDiff = vRA - vRSum0;

         vec vLDiffTemp = __as_float16(__permute(vMask, __as_uchar64(vLDiff)));

         recipDiagValue = getRecipSqrt(vLDiffTemp.s[0]);

         __vpred vpStoreL   = c7x::strm_agen<1, vec>::get_vpred();
         vec    *outVecPtrL = c7x::strm_agen<1, vec>::get_adv(ptrL);
         __vstore_pred(vpStoreL, outVecPtrL, vLDiff * recipDiagValue);

         __vpred vpStoreR   = c7x::strm_agen<1, vec>::get_vpred();
         vec    *outVecPtrR = c7x::strm_agen<1, vec>::get_adv(ptrL);
         __vstore_pred(vpStoreR, outVecPtrR, vRDiff * recipDiagValue);

         /* Handling all the pong fetches */

         for (fetch = 0; fetch < leftFetchesPerRow - 1; fetch++) {
            vLSum0 = vec(0);
            vLSum1 = vec(0);
            vLSum2 = vec(0);
            vLSum3 = vec(0);

            vLA = *(c7x::strm_agen<2, vec>::get_adv(ptrA));

            vRSum0 = vec(0);
            vRSum1 = vec(0);
            vRSum2 = vec(0);
            vRSum3 = vec(0);

            vRA = *(c7x::strm_agen<2, vec>::get_adv(ptrA));

            pMulStore = pMulBuffer;

            for (lRow = 0; lRow < rowNumber; lRow += UNROLL_COUNT) {
               vec multiplier0 = __vload_dup(pMulStore);
               pMulStore++;
               vec vLL0 = c7x::strm_eng<0, vec>::get_adv();
               vec vLR0 = c7x::strm_eng<1, vec>::get_adv();
               vLSum0 += vLL0 * multiplier0;
               vRSum0 += vLR0 * multiplier0;

               vec multiplier1 = __vload_dup(pMulStore);
               pMulStore++;
               vec vLL1 = c7x::strm_eng<0, vec>::get_adv();
               vec vLR1 = c7x::strm_eng<1, vec>::get_adv();
               vLSum1 += vLL1 * multiplier1;
               vRSum1 += vLR1 * multiplier1;

               vec multiplier2 = __vload_dup(pMulStore);
               pMulStore++;
               vec vLL2 = c7x::strm_eng<0, vec>::get_adv();
               vec vLR2 = c7x::strm_eng<1, vec>::get_adv();
               vLSum2 += vLL2 * multiplier2;
               vRSum2 += vLR2 * multiplier2;

               vec multiplier3 = __vload_dup(pMulStore);
               pMulStore++;
               vec vLL3 = c7x::strm_eng<0, vec>::get_adv();
               vec vLR3 = c7x::strm_eng<1, vec>::get_adv();
               vLSum3 += vLL3 * multiplier3;
               vRSum3 += vLR3 * multiplier3;
            }

            vLSum0 += vLSum1;
            vLSum2 += vLSum3;

            vRSum0 += vRSum1;
            vRSum2 += vRSum3;

            vLA -= vLSum2;
            vec vLDiff1 = vLA - vLSum0;

            vRA -= vRSum2;
            vec vRDiff1 = vRA - vRSum0;

            __vpred vpStoreL1   = c7x::strm_agen<1, vec>::get_vpred();
            vec    *outVecPtrL1 = c7x::strm_agen<1, vec>::get_adv(ptrL);
            __vstore_pred(vpStoreL1, outVecPtrL1, vLDiff1 * recipDiagValue);

            __vpred vpStoreR1   = c7x::strm_agen<1, vec>::get_vpred();
            vec    *outVecPtrR1 = c7x::strm_agen<1, vec>::get_adv(ptrL);
            __vstore_pred(vpStoreR1, outVecPtrR1, vRDiff1 * recipDiagValue);
         }

         offset += yStride;
         rowNumber++;
         vMask += vMaskIncrement; //uchar(4);
      }

      pLFirstRow += vecLen;
      offset += vecLen;
      elemsPerRow -= vecLen;
      elemsPerRowCeil -= vecLen;
   }

   for (; block < blockMax - 1; block++) {
      // configuration for sa
      saParamLStore.ICNT0 = elemsPerRow;
      saParamALoad.ICNT0  = elemsPerRow;

      vMask = vMaskInit;
      for (row = 0; row < vecLen; row++) {
         int32_t lezrIndex = (int32_t)((uint32_t)rowNumber & (uint32_t)(UNROLL_COUNT - 1)); // rowNumber%4=>4 is unroll count

         // configuration for SE
         seParamFetchL.ICNT1    = rowNumber;
         seParamFetchL.ICNT2    = 1;
         seParamFetchL.LEZR     = lezrDim[lezrIndex];
         seParamFetchL.LEZR_CNT = lezrCount[lezrIndex];

         seParamFetchR.ICNT1    = rowNumber;
         seParamFetchR.ICNT2    = 1;
         seParamFetchR.LEZR     = lezrDim[lezrIndex];
         seParamFetchR.LEZR_CNT = lezrCount[lezrIndex];

         if (rowNumber > 0) {
            __SE0_OPEN(pLFirstRow, seParamFetchL);
            __SE1_OPEN(pLFirstRow + vecLen, seParamFetchR);
         }

         __SA1_OPEN(saParamLStore);
         __SA2_OPEN(saParamALoad);

         dataType *ptrL = pOutULocal + offset;
         dataType *ptrA = pInALocal + offset;

         vec vLSum0 = vec(0); // Holds sum of all LxLy
         vec vLSum1 = vec(0);
         vec vLSum2 = vec(0);
         vec vLSum3 = vec(0);

         vec vLA = *(c7x::strm_agen<2, vec>::get_adv(ptrA));

         vec vRSum0 = vec(0); // Holds sum of all LxLy
         vec vRSum1 = vec(0);
         vec vRSum2 = vec(0);
         vec vRSum3 = vec(0);

         vec vRA = *(c7x::strm_agen<2, vec>::get_adv(ptrA));

         for (lRow = 0; lRow < rowNumber; lRow += UNROLL_COUNT) {
            vec vLL0     = c7x::strm_eng<0, vec>::get_adv(); // L value vector from each row
            vec vLL0Temp = __as_float16(__permute(vMask, __as_uchar64(vLL0)));
            vLSum0 += vLL0 * vLL0Temp.s[0];
            vec vLR0 = c7x::strm_eng<1, vec>::get_adv(); // L value vector from each row
            vRSum0 += vLR0 * vLL0Temp.s[0];

            vec vLL1     = c7x::strm_eng<0, vec>::get_adv();
            vec vLL1Temp = __as_float16(__permute(vMask, __as_uchar64(vLL1)));
            vLSum1 += vLL1 * vLL1Temp.s[0];
            vec vLR1 = c7x::strm_eng<1, vec>::get_adv(); // L value vector from each row
            vRSum1 += vLR1 * vLL1Temp.s[0];

            vec vLL2     = c7x::strm_eng<0, vec>::get_adv();
            vec vLL2Temp = __as_float16(__permute(vMask, __as_uchar64(vLL2)));
            vLSum2 += vLL2 * vLL2Temp.s[0];
            vec vLR2 = c7x::strm_eng<1, vec>::get_adv(); // L value vector from each row
            vRSum2 += vLR2 * vLL2Temp.s[0];

            vec vLL3     = c7x::strm_eng<0, vec>::get_adv();
            vec vLL3Temp = __as_float16(__permute(vMask, __as_uchar64(vLL3)));
            vLSum3 += vLL3 * vLL3Temp.s[0];
            vec vLR3 = c7x::strm_eng<1, vec>::get_adv(); // L value vector from each row
            vRSum3 += vLR3 * vLL3Temp.s[0];
         }

         vLSum0 += vLSum1;
         vLSum2 += vLSum3;

         vRSum0 += vRSum1;
         vRSum2 += vRSum3;

         vLA -= vLSum2;
         vec vLDiff = vLA - vLSum0;

         vRA -= vRSum2;
         vec vRDiff = vRA - vRSum0;

         vec vLDiffTemp = __as_float16(__permute(vMask, __as_uchar64(vLDiff)));

         recipDiagValue = getRecipSqrt(vLDiffTemp.s[0]);

         __vpred vpStoreL   = c7x::strm_agen<1, vec>::get_vpred();
         vec    *outVecPtrL = c7x::strm_agen<1, vec>::get_adv(ptrL);
         __vstore_pred(vpStoreL, outVecPtrL, vLDiff * recipDiagValue);

         __vpred vpStoreR   = c7x::strm_agen<1, vec>::get_vpred();
         vec    *outVecPtrR = c7x::strm_agen<1, vec>::get_adv(ptrL);
         __vstore_pred(vpStoreR, outVecPtrR, vRDiff * recipDiagValue);

         offset += yStride;
         rowNumber++;
         vMask += vMaskIncrement; //uchar(4);
      }

      pLFirstRow += vecLen;
      offset += vecLen;
      elemsPerRow -= vecLen;
   }

   for (; block < blockMax; block++) {
      // configuration for sa
      saParamLStore.ICNT0 = elemsPerRow;
      saParamALoad.ICNT0  = elemsPerRow;

      /*seParamFetchL => Up fetch  seParamFetchR => Down fetch*/
      seParamFetchL.ICNT0 = vecLen;
      seParamFetchL.DIM1  = (int32_t)((uint32_t)yStride << 1u); // order
      seParamFetchR.ICNT0 = vecLen;
      seParamFetchR.DIM1  = (int32_t)((uint32_t)yStride << 1u); // order

      seParamFetchL.DIMFMT = __SE_DIMFMT_2D;
      seParamFetchR.DIMFMT = __SE_DIMFMT_2D;

      vMask = vMaskInit;

      for (row = 0; row < vecLen - extraRows; row++) {

         if (rowNumber < 2) {

            // configuration for SE
            seParamFetchL.ICNT1 = rowNumber;

            if (rowNumber > 0) {

               __SE0_OPEN(pLFirstRow, seParamFetchL);

               __SA1_OPEN(saParamLStore);
               __SA2_OPEN(saParamALoad);

               dataType *ptrA = pInALocal + offset;

               vec vLA = *(c7x::strm_agen<2, vec>::get_adv(ptrA));

               vec       vLSum0 = vec(0); // Holds sum of all LxLy
               dataType *ptrL   = pOutULocal + offset;

               vec vLL0     = c7x::strm_eng<0, vec>::get(); // L value vector from each row
               vec vLL0Temp = __as_float16(__permute(vMask, __as_uchar64(vLL0)));
               vLSum0 += vLL0 * vLL0Temp.s[0];

               vec vLDiff = vLA - vLSum0;

               vec vLDiffTemp = __as_float16(__permute(vMask, __as_uchar64(vLDiff)));

               recipDiagValue = getRecipSqrt(vLDiffTemp.s[0]);

               __vpred vpStoreL   = c7x::strm_agen<1, vec>::get_vpred();
               vec    *outVecPtrL = c7x::strm_agen<1, vec>::get_adv(ptrL);
               __vstore_pred(vpStoreL, outVecPtrL, vLDiff * recipDiagValue);

               offset += yStride;
               rowNumber++;
            }
            else {
               __SA1_OPEN(saParamLStore);
               __SA2_OPEN(saParamALoad);

               dataType *ptrA = pInALocal + offset;
               vec       vLA  = *(c7x::strm_agen<2, vec>::get_adv(ptrA));
               dataType *ptrL = pOutULocal + offset;

               vec vLDiff     = vLA; // - vLSum0;
               vec vLDiffTemp = __as_float16(__permute(vMask, __as_uchar64(vLDiff)));
               recipDiagValue = getRecipSqrt(vLDiffTemp.s[0]);

               __vpred vpStoreL   = c7x::strm_agen<1, vec>::get_vpred();
               vec    *outVecPtrL = c7x::strm_agen<1, vec>::get_adv(ptrL);
               __vstore_pred(vpStoreL, outVecPtrL, vLDiff * recipDiagValue);

               offset += yStride;
               rowNumber++;
            }
         }
         else {
            int32_t upFetchesPerRow   = (int32_t)((uint32_t)(rowNumber + 1) >> 1u);
            int32_t downFetchesPerRow = (int32_t)((uint32_t)rowNumber >> 1u);

            // configuration for SE
            seParamFetchL.ICNT1 = upFetchesPerRow;
            seParamFetchR.ICNT1 = downFetchesPerRow;

            __SE0_OPEN(pLFirstRow, seParamFetchL);
            __SE1_OPEN(pLFirstRow + yStride, seParamFetchR);

            __SA1_OPEN(saParamLStore);
            __SA2_OPEN(saParamALoad);

            dataType *ptrL = pOutULocal + offset;
            dataType *ptrA = pInALocal + offset;

            vec vLSum0 = vec(0); // Holds sum of all LxLy
            vec vLSum1 = vec(0);
            vec vLSum2 = vec(0);
            vec vLSum3 = vec(0);

            vec vRSum0 = vec(0); // Holds sum of all LxLy
            vec vRSum1 = vec(0);
            vec vRSum2 = vec(0);
            vec vRSum3 = vec(0);

            vec vLA = *(c7x::strm_agen<2, vec>::get_adv(ptrA));

            for (lRow = 0; lRow < upFetchesPerRow; lRow += UNROLL_COUNT) {
               vec vLL0     = c7x::strm_eng<0, vec>::get_adv(); // L value vector from each row
               vec vLL0Temp = __as_float16(__permute(vMask, __as_uchar64(vLL0)));
               vLSum0 += vLL0 * vLL0Temp.s[0];

               vec vLR0     = c7x::strm_eng<1, vec>::get_adv(); // L value vector from each row
               vec vLR0Temp = __as_float16(__permute(vMask, __as_uchar64(vLR0)));
               vRSum0 += vLR0 * vLR0Temp.s[0];

               vec vLL1     = c7x::strm_eng<0, vec>::get_adv();
               vec vLL1Temp = __as_float16(__permute(vMask, __as_uchar64(vLL1)));
               vLSum1 += vLL1 * vLL1Temp.s[0];
               vec vLR1     = c7x::strm_eng<1, vec>::get_adv(); // L value vector from each row
               vec vLR1Temp = __as_float16(__permute(vMask, __as_uchar64(vLR1)));
               vRSum1 += vLR1 * vLR1Temp.s[0];

               vec vLL2     = c7x::strm_eng<0, vec>::get_adv();
               vec vLL2Temp = __as_float16(__permute(vMask, __as_uchar64(vLL2)));
               vLSum2 += vLL2 * vLL2Temp.s[0];
               vec vLR2     = c7x::strm_eng<1, vec>::get_adv(); // L value vector from each row
               vec vLR2Temp = __as_float16(__permute(vMask, __as_uchar64(vLR2)));
               vRSum2 += vLR2 * vLR2Temp.s[0];

               vec vLL3     = c7x::strm_eng<0, vec>::get_adv();
               vec vLL3Temp = __as_float16(__permute(vMask, __as_uchar64(vLL3)));
               vLSum3 += vLL3 * vLL3Temp.s[0];
               vec vLR3     = c7x::strm_eng<1, vec>::get_adv(); // L value vector from each row
               vec vLR3Temp = __as_float16(__permute(vMask, __as_uchar64(vLR3)));
               vRSum3 += vLR3 * vLR3Temp.s[0];
            }

            vLSum0 += vLSum1;
            vLSum2 += vLSum3;

            vRSum0 += vRSum1;
            vRSum2 += vRSum3;

            vLSum2 += vLSum0;
            vRSum2 += vRSum0;

            vec vLDiff = vLA - vLSum2 - vRSum2;

            vec vLDiffTemp = __as_float16(__permute(vMask, __as_uchar64(vLDiff)));

            recipDiagValue = getRecipSqrt(vLDiffTemp.s[0]);

            __vpred vpStoreL   = c7x::strm_agen<1, vec>::get_vpred();
            vec    *outVecPtrL = c7x::strm_agen<1, vec>::get_adv(ptrL);
            __vstore_pred(vpStoreL, outVecPtrL, vLDiff * recipDiagValue);

            offset += yStride;
            rowNumber++;
         }
         vMask += vMaskIncrement; //uchar(4);
      }

      pLFirstRow += vecLen;
      offset += vecLen;
      elemsPerRow -= vecLen;
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA1_CLOSE();
   __SA2_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
   return DSPLIB_SUCCESS;
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_cholesky_exec_ci(DSPLIB_kernelHandle handle,
                                      void *restrict pInA,
                                      void *restrict pOutU,
                                      void *restrict pMulBuffer)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_cholesky_PrivArgs *pKerPrivArgs = (DSPLIB_cholesky_PrivArgs *) handle;
   dataType                 *pLocalA      = (dataType *) pInA;
   dataType                 *pOutULocal   = (dataType *) pOutU;

   dataType                                              *pLocalMul   = (dataType *) pMulBuffer;
   uint8_t                                               *pBlock      = pKerPrivArgs->bufPblock;
   int32_t                                                order       = pKerPrivArgs->order;
   int32_t                                                enable_test = pKerPrivArgs->enableTest;
   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   DSPLIB_DEBUGPRINTFN(0, "pLocalA: %p pOutUocal: %p\n", pLocalA, pOutULocal);
   if (enable_test) {

      dataType sum = DSPLIB_cholesky_inplace_isPosDefinite(pLocalA, order, eleCount, pBlock);
      if (sum <= 0) {
         status = DSPLIB_ERR_FAILURE;
      }
      else{
         status = DSPLIB_cholesky_c7x_PingPong(enable_test, pKerPrivArgs, pLocalA, pOutULocal, pLocalMul);
      }
   }
   else{
      status = DSPLIB_cholesky_c7x_PingPong(enable_test, pKerPrivArgs, pLocalA, pOutULocal, pLocalMul);
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}

template DSPLIB_STATUS DSPLIB_cholesky_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                      void *restrict pInA,
                                                      void *restrict pOutU,
                                                      void *restrict pMulBuffer);

// template DSPLIB_STATUS DSPLIB_cholesky_exec_ci<double>(DSPLIB_kernelHandle handle,
//                                                        void *restrict pInA,
//                                                        void *restrict pOutU,
//                                                        void *restrict pMulBuffer);

/* ======================================================================== */
/*  End of file:  DSPLIB_cholesky_ci.cpp                   */
/* ======================================================================== */
