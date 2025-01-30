/******************************************************************************/
/*!
 * \file DSPLIB_cholesky_solver_ci.cpp
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

#include "../common/c71/DSPLIB_inlines.h"
#include "DSPLIB_cholesky_solver_priv.h"

/*******************************************************************************
 *
 * INITIALIZATION
 *
 ******************************************************************************/

template <typename dataType> void DSPLIB_cholesky_solver_matTrans_init_ci(DSPLIB_kernelHandle handle)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_cholesky_solver_PrivArgs *pKerPrivArgs = (DSPLIB_cholesky_solver_PrivArgs *) handle;
   uint8_t                         *pBlock       = pKerPrivArgs->bufPblock;
   int32_t                          order        = pKerPrivArgs->order;
   int32_t                          strideL      = pKerPrivArgs->stride;
   int32_t                          colLStride   = strideL / sizeof(dataType);

   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;
   int32_t                                                nVec     = DSPLIB_ceilingDiv(order, (eleCount * 2));

   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<vec>::value;

   se0Params.ICNT0   = order;
   se0Params.ICNT1   = eleCount;
   se0Params.DIM1    = colLStride;
   se0Params.ICNT2   = nVec;
   se0Params.DIM2    = colLStride * eleCount * 2;
   se0Params.DIMFMT  = __SE_DIMFMT_3D;
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;
   se0Params.TRANSPOSE = __SE_TRANSPOSE_32BIT;

//   if (sizeof(dataType) == 4) {
//      se0Params.TRANSPOSE = __SE_TRANSPOSE_32BIT;
//   }
//   else {
//      se0Params.TRANSPOSE = __SE_TRANSPOSE_64BIT;
//   }

   sa0Params.ICNT0         = eleCount * 2;
   sa0Params.ICNT1         = order;
   sa0Params.DIM1          = colLStride;
   sa0Params.ICNT2         = nVec;
   sa0Params.DIM2          = eleCount * 2;
   sa0Params.VECLEN        = SA_VECLEN;
   sa0Params.DIMFMT        = __SA_DIMFMT_3D;
   sa0Params.DECDIM1       = __SA_DECDIM_DIM2;
   sa0Params.DECDIM1_WIDTH = order;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (0 * SE_PARAM_SIZE)) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE)) = sa0Params;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_cholesky_solver_matTrans_init_ci<float>(DSPLIB_kernelHandle handle);
template void DSPLIB_cholesky_solver_matTrans_init_ci<double>(DSPLIB_kernelHandle handle);

template <typename dataType> void DSPLIB_cholesky_solver_backSubstitution_init_ci(DSPLIB_kernelHandle handle)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_cholesky_solver_PrivArgs *pKerPrivArgs = (DSPLIB_cholesky_solver_PrivArgs *) handle;
   uint8_t                         *pBlock       = pKerPrivArgs->bufPblock;
   int32_t                          order        = pKerPrivArgs->order;
   int32_t                          strideL      = pKerPrivArgs->stride;
   int32_t                          colLstride   = strideL / sizeof(dataType);

   typedef typename c7x::make_full_vector<dataType>::type vec;
   uint32_t                                               eleCount = c7x::element_count_of<vec>::value;

   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<vec>::value;
   __SE_ELEDUP  SE_ELEDUP  = c7x::se_eledup<dataType>::value;

   __SE_TEMPLATE_v1 seDiagReadParams                                = __gen_SE_TEMPLATE_v1();
   seDiagReadParams.ICNT0                                           = 1;
   seDiagReadParams.ICNT1                                           = order;
   seDiagReadParams.DIM1                                            = colLstride + 1;
   seDiagReadParams.DIMFMT                                          = __SE_DIMFMT_2D;
   seDiagReadParams.ELETYPE                                         = SE_ELETYPE;
   seDiagReadParams.VECLEN                                          = __SE_VECLEN_1ELEM;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE)) = seDiagReadParams;

   __SA_TEMPLATE_v1 saWriteXParams                                  = __gen_SA_TEMPLATE_v1();
   saWriteXParams.ICNT0                                             = 1;
   saWriteXParams.ICNT1                                             = order;
   saWriteXParams.DIM1                                              = -1;
   saWriteXParams.DIMFMT                                            = __SA_DIMFMT_2D;
   saWriteXParams.VECLEN                                            = SA_VECLEN;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE)) = saWriteXParams;

   __SE_TEMPLATE_v1 seDivReadParams                                 = __gen_SE_TEMPLATE_v1();
   seDivReadParams.ICNT0                                            = order;
   seDivReadParams.DIMFMT                                           = __SE_DIMFMT_1D;
   seDivReadParams.ELETYPE                                          = SE_ELETYPE;
   seDivReadParams.VECLEN                                           = SE_VECLEN;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE)) = seDivReadParams;

   __SA_TEMPLATE_v1 saDivStoreParams                                = __gen_SA_TEMPLATE_v1();
   saDivStoreParams.ICNT0                                           = order;
   saDivStoreParams.DIMFMT                                          = __SA_DIMFMT_1D;
   saDivStoreParams.VECLEN                                          = SA_VECLEN;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE)) = saDivStoreParams;

   __SE_TEMPLATE_v1 seReadXParams                                   = __gen_SE_TEMPLATE_v1();
   seReadXParams.ICNT0                                              = 1;
   seReadXParams.DIM1                                               = -1;
   seReadXParams.DIMFMT                                             = __SE_DIMFMT_2D;
   seReadXParams.VECLEN                                             = SE_VECLEN;
   seReadXParams.ELETYPE                                            = SE_ELETYPE;
   seReadXParams.ELEDUP                                             = SE_ELEDUP;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (6 * SE_PARAM_SIZE)) = seReadXParams;

   __SE_TEMPLATE_v1 seBlockParams                                   = __gen_SE_TEMPLATE_v1();
   seBlockParams.ICNT0                                              = eleCount;
   seBlockParams.DIM1                                               = -colLstride;
   seBlockParams.DIMFMT                                             = __SE_DIMFMT_2D;
   seBlockParams.ELETYPE                                            = SE_ELETYPE;
   seBlockParams.VECLEN                                             = SE_VECLEN;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (7 * SE_PARAM_SIZE)) = seBlockParams;

   __SA_TEMPLATE_v1 saReverseParams                                 = __gen_SA_TEMPLATE_v1();
   saReverseParams.ICNT0                                            = eleCount;
   saReverseParams.DIM1                                             = -((int32_t)eleCount);
   saReverseParams.DIMFMT                                           = __SA_DIMFMT_2D;
   saReverseParams.VECLEN                                           = SA_VECLEN;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (8 * SE_PARAM_SIZE)) = saReverseParams;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_cholesky_solver_backSubstitution_init_ci<float>(DSPLIB_kernelHandle handle);
template void DSPLIB_cholesky_solver_backSubstitution_init_ci<double>(DSPLIB_kernelHandle handle);

template <typename dataType>
DSPLIB_STATUS DSPLIB_cholesky_solver_init_ci(DSPLIB_kernelHandle                    handle,
                                             DSPLIB_bufParams2D_t                  *bufParamsU,
                                             DSPLIB_bufParams2D_t                  *bufParamsScratch,
                                             DSPLIB_bufParams1D_t                  *bufParamsY,
                                             DSPLIB_bufParams1D_t                  *bufParamsB,
                                             DSPLIB_bufParams1D_t                  *bufParamsX,
                                             DSPLIB_bufParams1D_t                  *bufParamsDiv,
                                             const DSPLIB_cholesky_solver_InitArgs *pKerInitArgs)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_STATUS status = DSPLIB_SUCCESS;
   DSPLIB_cholesky_solver_matTrans_init_ci<dataType>(handle);
   DSPLIB_cholesky_solver_backSubstitution_init_ci<dataType>(handle);
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}

template DSPLIB_STATUS DSPLIB_cholesky_solver_init_ci<float>(DSPLIB_kernelHandle                    handle,
                                                             DSPLIB_bufParams2D_t                  *bufParamsU,
                                                             DSPLIB_bufParams2D_t                  *bufParamsScratch,
                                                             DSPLIB_bufParams1D_t                  *bufParamsY,
                                                             DSPLIB_bufParams1D_t                  *bufParamsB,
                                                             DSPLIB_bufParams1D_t                  *bufParamsX,
                                                             DSPLIB_bufParams1D_t                  *bufParamsDiv,
                                                             const DSPLIB_cholesky_solver_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_cholesky_solver_init_ci<double>(DSPLIB_kernelHandle                    handle,
                                                              DSPLIB_bufParams2D_t                  *bufParamsU,
                                                              DSPLIB_bufParams2D_t                  *bufParamsScratch,
                                                              DSPLIB_bufParams1D_t                  *bufParamsY,
                                                              DSPLIB_bufParams1D_t                  *bufParamsB,
                                                              DSPLIB_bufParams1D_t                  *bufParamsX,
                                                              DSPLIB_bufParams1D_t                  *bufParamsDiv,
                                                              const DSPLIB_cholesky_solver_InitArgs *pKerInitArgs);

/*******************************************************************************
 *
 * IMPLEMENTATION
 *
 ******************************************************************************/

template <typename dataType>
static inline void
DSPLIB_cholesky_solver_matTrans_ci(dataType *pIn, dataType *pOut, int32_t order, int32_t colLStride, uint8_t *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");


   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (0 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (1 * SE_PARAM_SIZE));

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;
   int32_t                                                nVec     = DSPLIB_ceilingDiv(order, (eleCount * 2));

   int32_t loopCount = nVec * order;
   __SE0_OPEN(pIn, se0Params);
   __SE1_OPEN(pIn + (colLStride * eleCount), se0Params);
   __SA0_OPEN(sa0Params);

   for (int32_t i = 0; i < loopCount; i++) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();
      vec v2 = c7x::strm_eng<1, vec>::get_adv();

      __vpred pred1   = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStore1 = c7x::strm_agen<0, vec>::get_adv(pOut);
      __vstore_pred(pred1, pStore1, v1);

      __vpred pred2   = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStore2 = c7x::strm_agen<0, vec>::get_adv(pOut);
      __vstore_pred(pred2, pStore2, v2);
   }

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
}
template void
DSPLIB_cholesky_solver_matTrans_ci<float>(float *pIn, float *pOut, int32_t order, int32_t colLStride, uint8_t *pBlock);
template void DSPLIB_cholesky_solver_matTrans_ci<double>(double  *pIn,
                                                         double  *pOut,
                                                         int32_t  order,
                                                         int32_t  colLStride,
                                                         uint8_t *pBlock);

template <typename dataType, typename V = typename c7x::make_full_vector<dataType>::type>
inline void                       getElement(V inVec, uint32_t index, dataType *element);
template <typename V> inline void getElement(V inVec, uint32_t index, float *element)
{
   *element = __as_float(__vgetw_vrd(c7x::as_int_vec(inVec), index));
}

// template <typename V> inline void getElement(V inVec, uint32_t index, double *element)
// {
//    *element = __as_double(__vgetd_vrd(c7x::as_long_vec(inVec), index));
// }

template <typename dataType>
static inline void DSPLIB_cholesky_solver_forwardSubstitution_ci(dataType *pL,
                                                                 dataType *pY,
                                                                 dataType *pB,
                                                                 dataType *pDiv,
                                                                 int32_t   order,
                                                                 int32_t   colLstride,
                                                                 uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   __SE_TEMPLATE_v1 seDivReadParams;
   __SA_TEMPLATE_v1 saDivStoreParams;
   __SE_TEMPLATE_v1 seDiagReadParams;
   __SA_TEMPLATE_v1 saDiagWriteParams;
   seDiagReadParams  = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));
   saDiagWriteParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE));
   seDivReadParams   = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE));
   saDivStoreParams  = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (5 * SE_PARAM_SIZE));

   saDiagWriteParams.DIM1 = 1;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;
   int32_t                                                nVec     = DSPLIB_ceilingDiv(order, eleCount);

   /* Calculate reciprocals of Diagonal Elements */
   __SE1_OPEN(pL, seDiagReadParams);
   __SA1_OPEN(saDiagWriteParams);
   int32_t row = 0;

   for (row = 0; row < order; row++) {
      vec vecDiag = c7x::strm_eng<1, vec>::get_adv();

      __vpred predDiag   = c7x::strm_agen<1, vec>::get_vpred();
      vec    *pStoreDiag = c7x::strm_agen<1, vec>::get_adv(pDiv);
      __vstore_pred(predDiag, pStoreDiag, vecDiag);
   }
   __SE1_CLOSE();
   __SA1_CLOSE();

   __SE0_OPEN(pDiv, seDivReadParams);
   __SA0_OPEN(saDivStoreParams);
   dataType TwoP0 = 2.0;
   int32_t  ii    = 0;

   for (ii = 0; ii < nVec - 3; ii += 4) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();
      vec v2 = c7x::strm_eng<0, vec>::get_adv();
      vec v3 = c7x::strm_eng<0, vec>::get_adv();
      vec v4 = c7x::strm_eng<0, vec>::get_adv();

      vec yy1 = __recip(v1);
      yy1     = yy1 * (TwoP0 - v1 * yy1);
      yy1     = yy1 * (TwoP0 - v1 * yy1);

      vec yy2 = __recip(v2);
      yy2     = yy2 * (TwoP0 - v2 * yy2);
      yy2     = yy2 * (TwoP0 - v2 * yy2);

      vec yy3 = __recip(v3);
      yy3     = yy3 * (TwoP0 - v3 * yy3);
      yy3     = yy3 * (TwoP0 - v3 * yy3);

      vec yy4 = __recip(v4);
      yy4     = yy4 * (TwoP0 - v4 * yy4);
      yy4     = yy4 * (TwoP0 - v4 * yy4);

      __vpred predDiv1   = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreDiv1 = c7x::strm_agen<0, vec>::get_adv(pDiv);
      __vstore_pred(predDiv1, pStoreDiv1, yy1);

      __vpred predDiv2   = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreDiv2 = c7x::strm_agen<0, vec>::get_adv(pDiv);
      __vstore_pred(predDiv2, pStoreDiv2, yy2);

      __vpred predDiv3   = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreDiv3 = c7x::strm_agen<0, vec>::get_adv(pDiv);
      __vstore_pred(predDiv3, pStoreDiv3, yy3);

      __vpred predDiv4   = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreDiv4 = c7x::strm_agen<0, vec>::get_adv(pDiv);
      __vstore_pred(predDiv4, pStoreDiv4, yy4);
   }

   for (; ii < nVec; ii++) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();

      vec yy1 = __recip(v1);
      yy1     = yy1 * (TwoP0 - v1 * yy1);
      yy1     = yy1 * (TwoP0 - v1 * yy1);

      __vpred predDiv1   = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStoreDiv1 = c7x::strm_agen<0, vec>::get_adv(pDiv);
      __vstore_pred(predDiv1, pStoreDiv1, yy1);
   }

   __SE0_CLOSE();
   __SA0_CLOSE();

   /*----------------------------------------------------------------------
    Forward Substitution
    -----------------------------------------------------------------------*/

   __SE_TEMPLATE_v1 seBlockParams;
   __SE_TEMPLATE_v1 seReadYParams;
   __SA_TEMPLATE_v1 saWriteYParams;
   __SA_TEMPLATE_v1 sa1DReadParams;

   saWriteYParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE));
   seReadYParams  = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (6 * SE_PARAM_SIZE));
   seBlockParams  = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (7 * SE_PARAM_SIZE));
   sa1DReadParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE));

   saWriteYParams.DIM1 = 1;
   seReadYParams.DIM1  = 1;
   seBlockParams.DIM1  = colLstride;

   dataType *pSE0 = pL;
   dataType *pSA1 = pY;
   dataType *pSA2 = pB;
   dataType *pSA3 = pDiv;

   __SA1_OPEN(saWriteYParams);
   __SA2_OPEN(sa1DReadParams);
   __SA3_OPEN(sa1DReadParams);

   for (int32_t block = 0; block < nVec; block++) {
      __vpred predB  = c7x::strm_agen<2, vec>::get_vpred();
      vec    *pLoadB = c7x::strm_agen<2, vec>::get_adv(pSA2);
      vec     vecB   = __vload_pred(predB, pLoadB);

      __vpred predDiv  = c7x::strm_agen<3, vec>::get_vpred();
      vec    *pLoadDiv = c7x::strm_agen<3, vec>::get_adv(pSA3);
      vec     vecDiv   = __vload_pred(predDiv, pLoadDiv);

      int32_t sumRows   = block * eleCount;
      int32_t totalRows = sumRows + eleCount;

      seBlockParams.ICNT1 = totalRows;
      seReadYParams.ICNT1 = sumRows;

      __SE0_OPEN(pSE0, seBlockParams);
      if (sumRows > 0) {
         __SE1_OPEN(pY, seReadYParams);
      }

      /* Calculate sum */
      vec     vecSum   = (vec) 0;
      vec     vecSum1  = (vec) 0;
      vec     vecSum2  = (vec) 0;
      vec     vecSum3  = (vec) 0;
      vec     vecSum4  = (vec) 0;
      int32_t vertical = 0;

      for (vertical = 0; vertical < sumRows - 3; vertical += 4) {
         vec v1 = c7x::strm_eng<0, vec>::get_adv();
         vec y1 = c7x::strm_eng<1, vec>::get_adv();
         vecSum1 += v1 * y1;

         vec v2 = c7x::strm_eng<0, vec>::get_adv();
         vec y2 = c7x::strm_eng<1, vec>::get_adv();
         vecSum2 += v2 * y2;

         vec v3 = c7x::strm_eng<0, vec>::get_adv();
         vec y3 = c7x::strm_eng<1, vec>::get_adv();
         vecSum3 += v3 * y3;

         vec v4 = c7x::strm_eng<0, vec>::get_adv();
         vec y4 = c7x::strm_eng<1, vec>::get_adv();
         vecSum4 += v4 * y4;
      }

      vecSum = vecSum1 + vecSum2 + vecSum3 + vecSum4;

      /* Calculate values */
      dataType resultEle1;
      for (vertical = 0; vertical < eleCount; vertical++) {
         vec v1      = c7x::strm_eng<0, vec>::get_adv();
         vec result1 = (vecB - vecSum) * vecDiv;
         getElement(result1, vertical, &resultEle1);
         vecSum += v1 * (resultEle1);
         __vpred predYCalc = c7x::strm_agen<1, vec>::get_vpred();
         vec    *pStoreY   = c7x::strm_agen<1, vec>::get_adv(pSA1);
         __vstore_pred(predYCalc, pStoreY, (vec) resultEle1);
      }

      pSE0 += eleCount;
      __SE0_CLOSE();
      __SE1_CLOSE();
   }
   __SA2_CLOSE();
   __SA3_CLOSE();
   __SA1_CLOSE();
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
}
template void DSPLIB_cholesky_solver_forwardSubstitution_ci<float>(float   *pL,
                                                                   float   *pX,
                                                                   float   *pY,
                                                                   float   *pDiv,
                                                                   int32_t  order,
                                                                   int32_t  colLstride,
                                                                   uint8_t *pBlock);
// template void DSPLIB_cholesky_solver_forwardSubstitution_ci<double>(double  *pL,
//                                                                     double  *pX,
//                                                                     double  *pY,
//                                                                     double  *pDiv,
//                                                                     int32_t  order,
//                                                                     int32_t  colLstride,
//                                                                     uint8_t *pBlock);

template <typename dataType>
static inline void DSPLIB_cholesky_solver_backSubstitution_ci(dataType *pL,
                                                              dataType *pX,
                                                              dataType *pY,
                                                              dataType *pDiv,
                                                              int32_t   order,
                                                              int32_t   colLstride,
                                                              uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   /* Calculate reciprocals of Diagonal Elements */

   int32_t totalBlocks  = order / eleCount;
   int32_t remainingEle = order - (totalBlocks * eleCount);

   __SE_TEMPLATE_v1 seBlockParams;
   __SE_TEMPLATE_v1 seReadXParams;
   __SA_TEMPLATE_v1 saWriteXParams;
   __SA_TEMPLATE_v1 saReverseParams;

   saWriteXParams  = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE));
   seReadXParams   = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (6 * SE_PARAM_SIZE));
   seBlockParams   = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (7 * SE_PARAM_SIZE));
   saReverseParams = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (8 * SE_PARAM_SIZE));

   seReadXParams.ICNT0   = 1;
   seReadXParams.DIM1    = -1;
   saReverseParams.ICNT1 = totalBlocks;

   dataType *pLLastElem = &pL[(order - 1) + ((order - 1) * colLstride)];
   dataType *pXLastElem = &pX[order - 1];
   dataType *pSE0       = pLLastElem - (eleCount - 1);
   dataType *pSA1       = pX + order - 1;
   dataType *pSA2       = pY + order - eleCount;
   dataType *pSA3       = pDiv + order - eleCount;

   __SA1_OPEN(saWriteXParams);

   if (totalBlocks > 0) {
      __SA2_OPEN(saReverseParams);
      __SA3_OPEN(saReverseParams);

      for (int32_t block = 0; block < totalBlocks; block++) {
         __vpred predY  = c7x::strm_agen<2, vec>::get_vpred();
         vec    *pLoadY = c7x::strm_agen<2, vec>::get_adv(pSA2);
         vec     vecY   = __vload_pred(predY, pLoadY);

         __vpred predDiv  = c7x::strm_agen<3, vec>::get_vpred();
         vec    *pLoadDiv = c7x::strm_agen<3, vec>::get_adv(pSA3);
         vec     vecDiv   = __vload_pred(predDiv, pLoadDiv);

         int32_t sumRows   = block * eleCount;
         int32_t totalRows = sumRows + eleCount;

         seBlockParams.ICNT1 = totalRows;
         seReadXParams.ICNT1 = sumRows;

         __SE0_OPEN(pSE0, seBlockParams);
         if (sumRows > 0) {
            __SE1_OPEN(pXLastElem, seReadXParams);
         }

         /* Calculate sum */
         vec     vecSum   = (vec) 0;
         vec     vecSum1  = (vec) 0;
         vec     vecSum2  = (vec) 0;
         vec     vecSum3  = (vec) 0;
         vec     vecSum4  = (vec) 0;
         int32_t vertical = 0;

         for (vertical = 0; vertical < sumRows - 3; vertical += 4) {
            vec v1 = c7x::strm_eng<0, vec>::get_adv();
            vec x1 = c7x::strm_eng<1, vec>::get_adv();
            vecSum1 += v1 * x1;

            vec v2 = c7x::strm_eng<0, vec>::get_adv();
            vec x2 = c7x::strm_eng<1, vec>::get_adv();
            vecSum2 += v2 * x2;

            vec v3 = c7x::strm_eng<0, vec>::get_adv();
            vec x3 = c7x::strm_eng<1, vec>::get_adv();
            vecSum3 += v3 * x3;

            vec v4 = c7x::strm_eng<0, vec>::get_adv();
            vec x4 = c7x::strm_eng<1, vec>::get_adv();
            vecSum4 += v4 * x4;
         }

         vecSum = vecSum1 + vecSum2 + vecSum3 + vecSum4;

         /* Calculate values */
         uint32_t vecIndex = eleCount - 1;
         dataType resultEle1;

         for (vertical = 0; vertical < eleCount; vertical++) {
            vec v1      = c7x::strm_eng<0, vec>::get_adv();
            vec result1 = (vecY - vecSum) * vecDiv;
            getElement(result1, vecIndex--, &resultEle1);
            vecSum += v1 * (resultEle1);
            dataType *pStoreX1 = c7x::strm_agen<1, dataType>::get_adv(pSA1);
            *pStoreX1          = resultEle1;
         }

         pSE0 -= eleCount;
         __SE0_CLOSE();
         __SE1_CLOSE();
      }
      __SA2_CLOSE();
      __SA3_CLOSE();
   }
   /* Processing Remaining Elements */
   if (remainingEle > 0) {
      seBlockParams.ICNT0 = remainingEle;
      seBlockParams.ICNT1 = order;

      seReadXParams.ICNT1 = order;

      saReverseParams.ICNT0 = remainingEle;
      saReverseParams.ICNT1 = 1;
      saReverseParams.DIM1  = 0;

      pSE0 = &pL[(order - 1) * colLstride];
      __SE0_OPEN(pSE0, seBlockParams);
      __SE1_OPEN(pXLastElem, seReadXParams);
      __SA2_OPEN(saReverseParams);
      __SA3_OPEN(saReverseParams);

      int32_t sumRows = totalBlocks * eleCount;

      __vpred predY  = c7x::strm_agen<2, vec>::get_vpred();
      vec    *pLoadY = c7x::strm_agen<2, vec>::get_adv(pY);
      vec     vecY   = __vload_pred(predY, pLoadY);

      __vpred predDiv  = c7x::strm_agen<3, vec>::get_vpred();
      vec    *pLoadDiv = c7x::strm_agen<3, vec>::get_adv(pDiv);
      vec     vecDiv   = __vload_pred(predDiv, pLoadDiv);

      vec     vecSum   = (vec) 0;
      vec     vecSum1  = (vec) 0;
      vec     vecSum2  = (vec) 0;
      vec     vecSum3  = (vec) 0;
      vec     vecSum4  = (vec) 0;
      int32_t vertical = 0;

      for (vertical = 0; vertical < sumRows - 3; vertical += 4) {
         vec v1 = c7x::strm_eng<0, vec>::get_adv();
         vec x1 = c7x::strm_eng<1, vec>::get_adv();
         vecSum1 += v1 * x1;

         vec v2 = c7x::strm_eng<0, vec>::get_adv();
         vec x2 = c7x::strm_eng<1, vec>::get_adv();
         vecSum2 += v2 * x2;

         vec v3 = c7x::strm_eng<0, vec>::get_adv();
         vec x3 = c7x::strm_eng<1, vec>::get_adv();
         vecSum3 += v3 * x3;

         vec v4 = c7x::strm_eng<0, vec>::get_adv();
         vec x4 = c7x::strm_eng<1, vec>::get_adv();
         vecSum4 += v4 * x4;
      }

      vecSum = vecSum1 + vecSum2 + vecSum3 + vecSum4;

      int32_t  vecIndex = remainingEle - 1;
      dataType resultEle;

      for (vertical = 0; vertical < remainingEle; vertical++) {
         vec v1     = c7x::strm_eng<0, vec>::get_adv();
         vec result = (vecY - vecSum) * vecDiv;

         getElement(result, vecIndex--, &resultEle);
         vecSum += v1 * (resultEle);

         dataType *pStoreX = c7x::strm_agen<1, dataType>::get_adv(pSA1);
         *pStoreX          = resultEle;
      }

      __SE0_CLOSE();
      __SE1_CLOSE();
      __SA2_CLOSE();
      __SA3_CLOSE();
   }
   __SA1_CLOSE();
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
}
template void DSPLIB_cholesky_solver_backSubstitution_ci<float>(float   *pL,
                                                                float   *pX,
                                                                float   *pY,
                                                                float   *pDiv,
                                                                int32_t  order,
                                                                int32_t  colLstride,
                                                                uint8_t *pBlock);
// template void DSPLIB_cholesky_solver_backSubstitution_ci<double>(double  *pL,
//                                                                  double  *pX,
//                                                                  double  *pY,
//                                                                  double  *pDiv,
//                                                                  int32_t  order,
//                                                                  int32_t  colLstride,
//                                                                  uint8_t *pBlock);

template <typename dataType>
DSPLIB_STATUS DSPLIB_cholesky_solver_exec_ci(DSPLIB_kernelHandle handle,
                                             void *restrict pU,
                                             void *restrict pScratch,
                                             void *restrict pY,
                                             void *restrict pB,
                                             void *restrict pX,
                                             void *restrict pDiv)
{

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_cholesky_solver_PrivArgs *pKerPrivArgs = (DSPLIB_cholesky_solver_PrivArgs *) handle;
   uint8_t                         *pBlock       = pKerPrivArgs->bufPblock;
   int32_t                          order        = pKerPrivArgs->order;
   int32_t                          strideL      = pKerPrivArgs->stride;
   int32_t                          colLStride   = strideL / sizeof(dataType);

   dataType *pLocalU      = (dataType *) pU;
   dataType *pLocaltransU = (dataType *) pScratch;
   dataType *pLocalY      = (dataType *) pY;
   dataType *pLocalB      = (dataType *) pB;
   dataType *pLocalX      = (dataType *) pX;
   dataType *pLocalDiv    = (dataType *) pDiv;
  
   DSPLIB_DEBUGPRINTFN(0, "pLocalU: %p pLocalX: %p\n", pLocalU, pLocalX);

   /*-----------------------------------------------------------------------
   solve L*y=b for y using forward substitution
   -------------------------------------------------------------------------*/
   DSPLIB_cholesky_solver_forwardSubstitution_ci<dataType>(pLocalU, pLocalY, pLocalB, pLocalDiv, order, colLStride,
                                                           pBlock);

   /*-----------------------------------------------------------------------
    solve U*x=y for x using backward substitution
    ------------------------------------------------------------------------ */
   DSPLIB_cholesky_solver_matTrans_ci<dataType>(pLocalU, pLocaltransU, order, colLStride, pBlock);

   DSPLIB_cholesky_solver_backSubstitution_ci<dataType>(pLocaltransU, pLocalX, pLocalY, pLocalDiv, order, colLStride,
                                                        pBlock);

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_cholesky_solver_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                             void *restrict pU,
                                                             void *restrict pScratch,
                                                             void *restrict pY,
                                                             void *restrict pB,
                                                             void *restrict pX,
                                                             void *restrict pDiv);

// template DSPLIB_STATUS DSPLIB_cholesky_solver_exec_ci<double>(DSPLIB_kernelHandle handle,
//                                                               void *restrict pU,
//                                                               void *restrict pScratch,
//                                                               void *restrict pY,
//                                                               void *restrict pB,
//                                                               void *restrict pX,
//                                                               void *restrict pDiv);
/* ======================================================================== */
/*  End of file:  DSPLIB_cholesky_solver_ci.cpp                   */
/* ======================================================================== */
