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
/* VXLIB_median_3X3_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template <typename dType>
VXLIB_STATUS VXLIB_median_3X3_init_ci(VXLIB_kernelHandle           handle,
                                      const VXLIB_bufParams2D_t *  bufParamsIn,
                                      const VXLIB_bufParams2D_t *  bufParamsMask,
                                      const VXLIB_bufParams2D_t *  bufParamsOut,
                                      const VXLIB_bufParams2D_t *  bufParamsScratch,
                                      const VXLIB_median_InitArgs *pKerInitArgs)
{
   VXLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default
   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_median_PrivArgs *pKerPrivArgs = (VXLIB_median_PrivArgs *) handle;
   uint8_t *              pBlock       = pKerPrivArgs->bufPblock;
   //  bool   kernelType = pKerPrivArgs->pKerInitArgs.kernelType;

   typedef typename c7x::make_full_vector<dType>::type vec;

   // obtain image parameters and overflow policy
   size_t       width             = pKerPrivArgs->width;
   size_t       widthOut          = pKerPrivArgs->widthOut;
   size_t       heightOut         = pKerPrivArgs->heightOut;
   size_t       strideInElements  = pKerPrivArgs->strideInElements;
   size_t       strideOutElements = pKerPrivArgs->strideOutElements;
   uint32_t     eleCount          = c7x::element_count_of<vec>::value;
   uint32_t     numBlocks         = (widthOut + eleCount - 1) / eleCount;
   __SE_ELETYPE SE_ELETYPE        = c7x::se_eletype<vec>::value;
   __SE_VECLEN  SE_VECLEN         = c7x::se_veclen<vec>::value;
   __SA_VECLEN  SA_VECLEN         = c7x::sa_veclen<vec>::value;

   // structs to hold SE and SA parameters
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se1Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   se0Params.ICNT0         = eleCount;
   se0Params.ICNT1         = 3;
   se0Params.DIM1          = strideInElements;
   se0Params.ICNT2         = heightOut;
   se0Params.DIM2          = strideInElements;
   se0Params.ICNT3         = numBlocks;
   se0Params.DIM3          = eleCount;
   se0Params.ICNT4         = 1;
   se0Params.DIM4          = 0;
   se0Params.DIMFMT        = __SE_DIMFMT_5D;
   se0Params.ELETYPE       = SE_ELETYPE;
   se0Params.VECLEN        = SE_VECLEN;
   se0Params.DECDIM1       = __SE_DECDIM_DIM3;
   se0Params.DECDIM1_WIDTH = width;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET) = se0Params;

   se1Params.ICNT0         = eleCount;
   se1Params.ICNT1         = 2;
   se1Params.DIM1          = 1;
   se1Params.ICNT2         = 3;
   se1Params.DIM2          = strideInElements;
   se1Params.ICNT3         = heightOut;
   se1Params.DIM3          = strideInElements;
   se1Params.ICNT4         = numBlocks;
   se1Params.DIM4          = eleCount;
   se1Params.DIMFMT        = __SE_DIMFMT_5D;
   se1Params.ELETYPE       = SE_ELETYPE;
   se1Params.VECLEN        = SE_VECLEN;
   se1Params.DECDIM1       = __SE_DECDIM_DIM4;
   se1Params.DECDIM1SD     = __SE_DECDIMSD_DIM1;
   se1Params.DECDIM1_WIDTH = width - 1;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET) = se1Params;

   sa0Params.ICNT0         = eleCount;
   sa0Params.ICNT1         = heightOut;
   sa0Params.DIM1          = strideOutElements;
   sa0Params.ICNT2         = numBlocks;
   sa0Params.DIM2          = eleCount;
   sa0Params.DIMFMT        = __SA_DIMFMT_3D;
   sa0Params.VECLEN        = SA_VECLEN;
   sa0Params.DECDIM1       = __SA_DECDIM_DIM2;
   sa0Params.DECDIM1_WIDTH = widthOut;

   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET) = sa0Params;

   return status;
}

template VXLIB_STATUS VXLIB_median_3X3_init_ci<VXLIB_MEDIAN_TYPENAME_8U>(VXLIB_kernelHandle           handle,
                                                                         const VXLIB_bufParams2D_t *  bufParamsIn,
                                                                         const VXLIB_bufParams2D_t *  bufParamsMask,
                                                                         const VXLIB_bufParams2D_t *  bufParamsOut,
                                                                         const VXLIB_bufParams2D_t *  bufParamsScratch,
                                                                         const VXLIB_median_InitArgs *pKerInitArgs);

template VXLIB_STATUS VXLIB_median_3X3_init_ci<VXLIB_MEDIAN_TYPENAME_8S>(VXLIB_kernelHandle           handle,
                                                                         const VXLIB_bufParams2D_t *  bufParamsIn,
                                                                         const VXLIB_bufParams2D_t *  bufParamsMask,
                                                                         const VXLIB_bufParams2D_t *  bufParamsOut,
                                                                         const VXLIB_bufParams2D_t *  bufParamsScratch,
                                                                         const VXLIB_median_InitArgs *pKerInitArgs);
template VXLIB_STATUS VXLIB_median_3X3_init_ci<VXLIB_MEDIAN_TYPENAME_16U>(VXLIB_kernelHandle           handle,
                                                                          const VXLIB_bufParams2D_t *  bufParamsIn,
                                                                          const VXLIB_bufParams2D_t *  bufParamsMask,
                                                                          const VXLIB_bufParams2D_t *  bufParamsOut,
                                                                          const VXLIB_bufParams2D_t *  bufParamsScratch,
                                                                          const VXLIB_median_InitArgs *pKerInitArgs);
template VXLIB_STATUS VXLIB_median_3X3_init_ci<VXLIB_MEDIAN_TYPENAME_16S>(VXLIB_kernelHandle           handle,
                                                                          const VXLIB_bufParams2D_t *  bufParamsIn,
                                                                          const VXLIB_bufParams2D_t *  bufParamsMask,
                                                                          const VXLIB_bufParams2D_t *  bufParamsOut,
                                                                          const VXLIB_bufParams2D_t *  bufParamsScratch,
                                                                          const VXLIB_median_InitArgs *pKerInitArgs);
// template VXLIB_STATUS VXLIB_median_3X3_init_ci<VXLIB_MEDIAN_TYPENAME_64S>(VXLIB_kernelHandle           handle,
//                                                                           const VXLIB_bufParams2D_t   *bufParamsIn,
//                                                                           const VXLIB_bufParams2D_t   *bufParamsMask,
//                                                                           const VXLIB_bufParams2D_t   *bufParamsOut,
//                                                                           const VXLIB_bufParams2D_t
//                                                                           *bufParamsScratch, const
//                                                                           VXLIB_median_InitArgs *pKerInitArgs);

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_median_3X3_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template <typename dType>
VXLIB_STATUS VXLIB_median_3X3_exec_ci(VXLIB_kernelHandle handle,
                                      void *restrict     pIn,
                                      void *restrict     pMask,
                                      void *restrict     pOut,
                                      void *restrict     pScratch)
{
   VXLIB_DEBUGPRINTFN(0, "%s\n", "Entering Function");

   VXLIB_STATUS           status       = VXLIB_SUCCESS;
   VXLIB_median_PrivArgs *pKerPrivArgs = (VXLIB_median_PrivArgs *) handle;

   dType *pInLocal  = (dType *) pIn;
   dType *pOutLocal = (dType *) pOut;

   typedef typename c7x::make_full_vector<dType>::type vec;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   __SE0_OPEN(pInLocal, se0Params);

   __SE_TEMPLATE_v1 se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);
   __SE1_OPEN(pInLocal + 1, se1Params);

   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);
   __SA0_OPEN(sa0Params);

   int32_t  outCols   = pKerPrivArgs->widthOut;
   int32_t  outRows   = pKerPrivArgs->heightOut;
   uint32_t eleCount  = c7x::element_count_of<vec>::value;
   int32_t  numBlocks = (outCols + eleCount - 1) / eleCount;
   int32_t  i, j;

#pragma MUST_ITERATE(1, , )
   for (i = 0; i < numBlocks; i++) {
#pragma MUST_ITERATE(1, , )
      for (j = 0; j < outRows; j++) {
         /*------------------------------------------------------*/
         /* From a 3x3 input of (a to i) first find the min, med */
         /* and max cols as shown below.                         */
         /*                                                      */
         /*        row 0 - a  b  c        min  med  max          */
         /*        row 1 - d  e  f    =>  min  med  max          */
         /*        row 2 - g  h  i        min  med  max          */
         /*                                                      */
         /*------------------------------------------------------*/

         /* Find the MIN and MAX of Row 0  Col 0-1 */
         vec row0_col0 = c7x::strm_eng<0, vec>::get_adv();
         vec row0_col1 = c7x::strm_eng<1, vec>::get_adv();
         vec row0_col2 = c7x::strm_eng<1, vec>::get_adv();

         vec row0_01_min = MIN_VAL(row0_col0, row0_col1);
         vec row0_01_max = MAX_VAL(row0_col0, row0_col1);

         /* Find the MIN and MAX of Row 0  Col 0-1-2 */
         vec row0_012_min = MIN_VAL(row0_01_min, row0_col2);
         vec row0_012_max = MAX_VAL(row0_01_max, row0_col2);

         /* Find the MED of Row 0  Col 0-1-2 */
         vec row0_012_med = MAX_VAL(MIN_VAL(row0_01_max, row0_col2), row0_01_min);

         /* Find the MIN and MAX of Row 1  and Col 0-1 */
         vec row1_col0 = c7x::strm_eng<0, vec>::get_adv();
         vec row1_col1 = c7x::strm_eng<1, vec>::get_adv();
         vec row1_col2 = c7x::strm_eng<1, vec>::get_adv();

         vec row1_01_min = MIN_VAL(row1_col0, row1_col1);
         vec row1_01_max = MAX_VAL(row1_col0, row1_col1);

         /* Find the MIN and MAX of Row 1  and Col 0-1-2 */
         vec row1_012_min = MIN_VAL(row1_01_min, row1_col2);
         vec row1_012_max = MAX_VAL(row1_01_max, row1_col2);

         /* Find the MED of Row 1  Col 0-1-2 */
         vec row1_012_med = MAX_VAL(MIN_VAL(row1_01_max, row1_col2), row1_01_min);

         /* Find the MIN and MAX of Row 2  and Col 0-1 */
         vec row2_col0 = c7x::strm_eng<0, vec>::get_adv();
         vec row2_col1 = c7x::strm_eng<1, vec>::get_adv();
         vec row2_col2 = c7x::strm_eng<1, vec>::get_adv();

         vec row2_01_min = MIN_VAL(row2_col0, row2_col1);
         vec row2_01_max = MAX_VAL(row2_col0, row2_col1);

         /* Find the MIN and MAX of Row 2  and Col 0-1-2 */
         vec row2_012_min = MIN_VAL(row2_01_min, row2_col2);
         vec row2_012_max = MAX_VAL(row2_01_max, row2_col2);

         /* Find the MED of Row 2  Col 0-1-2 */
         vec row2_012_med = MAX_VAL(MIN_VAL(row2_01_max, row2_col2), row2_01_min);

         /*------------------------------------------------------*/
         /* Now find the max of min column, min of max column    */
         /* and median of median colum as below                  */
         /*                                                      */
         /*                      col 0  col 1 col 2              */
         /*              row 0 -  min    med   MAX               */
         /*              row 1 -  min    MED   max               */
         /*              row 2 -  MIN    med   max               */
         /*                                                      */
         /*------------------------------------------------------*/

         /* Find the MAX of MIN in col 0 */
         vec col0_012_max = MAX_VAL(MAX_VAL(row0_012_min, row1_012_min), row2_012_min);

         /* Find the MIN of MAX in col 2 */
         vec col2_012_min = MIN_VAL(MIN_VAL(row0_012_max, row1_012_max), row2_012_max);

         /* Find the MED of MED in col 1 */
         vec col1_012_med =
             MAX_VAL(MIN_VAL(MAX_VAL(row0_012_med, row1_012_med), row2_012_med), MIN_VAL(row0_012_med, row1_012_med));

         /* Find the final MED of the diagonal */
         vec median =
             MAX_VAL(MIN_VAL(MAX_VAL(col0_012_max, col1_012_med), col2_012_min), MIN_VAL(col0_012_max, col1_012_med));

         /* Write out the final median */
         __vpred vpStore = c7x::strm_agen<0, vec>::get_vpred();
         vec *   vStore  = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(vpStore, vStore, median);
      }
   }

   // Close streams
   __SE0_CLOSE();
   __SE1_CLOSE();

   // Close agen
   __SA0_CLOSE();

   VXLIB_DEBUGPRINTFN(0, "%s\n", "Exiting Function");
   return status;
}

template VXLIB_STATUS VXLIB_median_3X3_exec_ci<VXLIB_MEDIAN_TYPENAME_8U>(VXLIB_kernelHandle handle,
                                                                         void *restrict     pIn,
                                                                         void *restrict     pMask,
                                                                         void *restrict     pOut,
                                                                         void *restrict     pScratch);
template VXLIB_STATUS VXLIB_median_3X3_exec_ci<VXLIB_MEDIAN_TYPENAME_8S>(VXLIB_kernelHandle handle,
                                                                         void *restrict     pIn,
                                                                         void *restrict     pMask,
                                                                         void *restrict     pOut,
                                                                         void *restrict     pScratch);
template VXLIB_STATUS VXLIB_median_3X3_exec_ci<VXLIB_MEDIAN_TYPENAME_16U>(VXLIB_kernelHandle handle,
                                                                          void *restrict     pIn,
                                                                          void *restrict     pMask,
                                                                          void *restrict     pOut,
                                                                          void *restrict     pScratch);
template VXLIB_STATUS VXLIB_median_3X3_exec_ci<VXLIB_MEDIAN_TYPENAME_16S>(VXLIB_kernelHandle handle,
                                                                          void *restrict     pIn,
                                                                          void *restrict     pMask,
                                                                          void *restrict     pOut,
                                                                          void *restrict     pScratch);
// template VXLIB_STATUS VXLIB_median_3X3_exec_ci<VXLIB_MEDIAN_TYPENAME_64S>(VXLIB_kernelHandle handle,
//                                                                           void *restrict pIn,
//                                                                           void *restrict pMask,
//                                                                           void *restrict pOut,
//                                                                           void *restrict pScratch);

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Performance estimation function */
/*                                                                                                                    */
/**********************************************************************************************************************/

void VXLIB_median_perfEst(VXLIB_kernelHandle           handle,
                          const VXLIB_bufParams2D_t   *bufParamsIn,
                          const VXLIB_bufParams2D_t   *bufParamsMask,
                          const VXLIB_bufParams2D_t   *bufParamsOut,
                          const VXLIB_bufParams2D_t   *bufParamsScratch,
                          const VXLIB_median_InitArgs *pKerInitArgs,
                          size_t                      *archCycles,
                          size_t                      *estCycles)
{

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_median_PrivArgs *pKerPrivArgs = (VXLIB_median_PrivArgs *) handle;

   uint32_t eleCount = 0;
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

   size_t overheadCnt = 55; // profiled code before entering compute loop

   /* -------------------- ESTIMATE FOR 3X3 -------------------- */
   if (pKerInitArgs->kernelType == 0) {
      uint32_t numBlocks = (pKerPrivArgs->heightOut) * VXLIB_ceilingDiv(pKerPrivArgs->widthOut, eleCount);
      *archCycles        = 7 + numBlocks * 8;
   }
   /* -------------------- ESTIMATE FOR MXN -------------------- */
   else {
      uint32_t M            = pKerPrivArgs->M;
      uint32_t N            = pKerPrivArgs->N;
      uint32_t width        = pKerPrivArgs->width;
      uint32_t widthOut     = pKerPrivArgs->widthOut;
      uint32_t heightOut    = pKerPrivArgs->heightOut;
      uint32_t unrollFactor = 0;
      if (width <= eleCount) {
         unrollFactor = 1;
      }
      else if (width <= 3 * eleCount) {
         unrollFactor = 2;
      }
      else if (width <= 6 * eleCount) {
         unrollFactor = 4;
      }
      else {
         unrollFactor = 8;
      }

      /* Truecnt loop */
      size_t trueCntLoopCnt = M * N;
      size_t trueCntCycles  = 8 + trueCntLoopCnt * 2;

      /* Scratch buffer fill loop */
      size_t scratchLoop       = M * N * VXLIB_ceilingDiv(width, eleCount);
      size_t scratchFillCycles = 4 + scratchLoop * 2;

      /* Sort loop */
      size_t loop1Count     = VXLIB_ceilingDiv((M * N), 2);
      size_t loop1Exclusive = 0; // obtained from ASM.

      size_t widthBlocks    = VXLIB_ceilingDiv(width, eleCount);
      size_t loop2Count     = VXLIB_ceilingDiv(widthBlocks, unrollFactor);
      size_t loop2Exclusive = 0;

      size_t loop3Count = ((trueCntLoopCnt - 2 > 0) ? ((trueCntLoopCnt - 2) / 2) : 1);

      size_t loop3Cycles = 0;
      if (unrollFactor == 1) {
         loop3Cycles    = 2 + loop3Count * 2;
         loop2Exclusive = 13; // from ASM
         loop1Exclusive = 23; // from ASM
      }
      else if (unrollFactor == 2) {
         loop3Cycles    = 2 + loop3Count * 4;
         loop2Exclusive = 13; // from ASM
         loop1Exclusive = 25; // from ASM
      }
      else if (unrollFactor == 4) {
         loop3Cycles    = 3 + loop3Count * 8;
         loop2Exclusive = 14; // from ASM
         loop1Exclusive = 25; // from ASM
      }
      else {
         loop3Cycles    = 4 + loop3Count * 16;
         loop2Exclusive = 17; // from ASM
         loop1Exclusive = 25; // from ASM
      }

      size_t loop2Cycles = loop2Count * (loop2Exclusive + loop3Cycles);
      size_t sortCycles  = loop1Count * (loop1Exclusive + loop2Cycles);

      /* Store median values loop */
      size_t storeLoopCnt = VXLIB_ceilingDiv(widthOut, eleCount);
      size_t storeCycles  = 6 + storeLoopCnt * 1;

      size_t outerLoopExclusive = 25; // from ASM

      *archCycles = trueCntCycles + heightOut * (outerLoopExclusive + scratchFillCycles + sortCycles + storeCycles);
   }

   *estCycles = overheadCnt + *archCycles;
}

/* ======================================================================== */
/*  End of file:  VXLIB_median_ci.cpp                                     */
/* ======================================================================== */
