/******************************************************************************
 *                                                                             *
 * module name       :DSPLIB                                                   *
 *                                                                             *
 * module descripton :Digital Signal Processing Library module for C7x+MMA   *
 *                                                                             *
 * Copyright (C) 2017-2018 Texas Instruments Incorporated - http://www.ti.com/ *
 * ALL RIGHTS RESERVED                                                         *
 *                                                                             *
 ******************************************************************************/

/**
******************************************************************************
*  @file     DSPLIB_svd_common_ci.cpp
*
*  @brief
*
*  @version  0.1 - Aug 2023 : Initial Version
*
*******************************************************************************
*/

/* *****************************************************************************
 *
 * INCLUDES
 *
 ***************************************************************************** */

#include "DSPLIB_svd_priv.h"

/* *****************************************************************************
 *
 * INITIALIZATION
 *
 ***************************************************************************** */
/*!
 *  @brief       Init function for funtions implemented in @file DSPLIB_svd_common_ci.cpp
 */
template <typename dataType> void DSPLIB_svd_matTrans_init_ci(DSPLIB_kernelHandle handle)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_svd_PrivArgs *pKerPrivArgs = (DSPLIB_svd_PrivArgs *) handle;
   uint8_t             *pBlock       = pKerPrivArgs->bufPblock;
   int32_t              dataSize     = sizeof(dataType);

   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa1Params = __gen_SA_TEMPLATE_v1();

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount   = c7x::element_count_of<vec>::value;
   __SE_ELETYPE                                           SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN                                            SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN                                            SA_VECLEN  = c7x::sa_veclen<vec>::value;

   se0Params.DIMFMT  = __SE_DIMFMT_3D;
   se0Params.ICNT1   = eleCount;
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;
   if (dataSize == 4) {
      se0Params.TRANSPOSE = __SE_TRANSPOSE_32BIT;
   }
   else {
      se0Params.TRANSPOSE = __SE_TRANSPOSE_64BIT;
   }

   sa0Params.ICNT0   = eleCount * 2;
   sa0Params.DIM2    = eleCount * 2;
   sa0Params.VECLEN  = SA_VECLEN;
   sa0Params.DIMFMT  = __SA_DIMFMT_3D;
   sa0Params.DECDIM1 = __SA_DECDIM_DIM2;

   sa1Params.DIMFMT = __SA_DIMFMT_2D;
   sa1Params.VECLEN = SA_VECLEN;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (24 * SE_PARAM_SIZE)) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (25 * SE_PARAM_SIZE)) = sa0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (26 * SE_PARAM_SIZE)) = sa1Params;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_svd_matTrans_init_ci<float>(DSPLIB_kernelHandle handle);
template void DSPLIB_svd_matTrans_init_ci<double>(DSPLIB_kernelHandle handle);

/* *****************************************************************************
 *
 * IMPLEMENTATION
 *
 ***************************************************************************** */

/*!
 *  @brief       Take transpose of of input matrix pIn an store in pOut.
 */
template <typename dataType>
void DSPLIB_svd_matTrans_ci(dataType *pIn,
                            dataType *pOut,
                            int32_t   Nrows,
                            int32_t   Ncols,
                            int32_t   colInStride,
                            int32_t   colOutStride,
                            uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (24 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (25 * SE_PARAM_SIZE));

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   int32_t nVec    = DSPLIB_ceilingDiv(Nrows, (eleCount * 2));
   se0Params.ICNT0 = Ncols;
   se0Params.DIM1  = colInStride;
   se0Params.ICNT2 = nVec;
   se0Params.DIM2  = colInStride * eleCount * 2;

   sa0Params.ICNT1         = Ncols;
   sa0Params.DIM1          = colOutStride;
   sa0Params.ICNT2         = nVec;
   sa0Params.DECDIM1_WIDTH = Nrows;

   int32_t loopCount = nVec * Ncols;

   __SE0_OPEN(pIn, se0Params);
   __SE1_OPEN(pIn + (colInStride * eleCount), se0Params);
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
template void DSPLIB_svd_matTrans_ci<float>(float   *pIn,
                                            float   *pOut,
                                            int32_t  Nrows,
                                            int32_t  Ncols,
                                            int32_t  colInStride,
                                            int32_t  colOutStride,
                                            uint8_t *pBlock);
template void DSPLIB_svd_matTrans_ci<double>(double  *pIn,
                                             double  *pOut,
                                             int32_t  Nrows,
                                             int32_t  Ncols,
                                             int32_t  colInStride,
                                             int32_t  colOutStride,
                                             uint8_t *pBlock);

/*!
 *  @brief       Copy input matrix pIn to pOut.
 */
template <typename dataType>
void DSPLIB_svd_blk_move_ci(dataType *pOut,
                            dataType *pIn,
                            int32_t   Nrows,
                            int32_t   Ncols,
                            int32_t   colOutStride,
                            int32_t   colInStride,
                            uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (22 * SE_PARAM_SIZE));
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (26 * SE_PARAM_SIZE));

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount  = c7x::element_count_of<vec>::value;
   int32_t                                                nVec      = DSPLIB_ceilingDiv(Ncols, eleCount);
   int32_t                                                loopCount = nVec * Nrows;

   se0Params.ICNT0 = Ncols;
   se0Params.ICNT1 = Nrows;
   se0Params.DIM1  = colInStride;

   sa0Params.ICNT0 = Ncols;
   sa0Params.ICNT1 = Nrows;
   sa0Params.DIM1  = colOutStride;

   __SE0_OPEN(pIn, se0Params);
   __SA0_OPEN(sa0Params);
   for (int32_t i = 0; i < loopCount; i++) {
      vec v1 = c7x::strm_eng<0, vec>::get_adv();

      __vpred pred   = c7x::strm_agen<0, vec>::get_vpred();
      vec    *pStore = c7x::strm_agen<0, vec>::get_adv(pOut);
      __vstore_pred(pred, pStore, v1);
   }

   __SE0_CLOSE();
   __SA0_CLOSE();
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Exiting function");
}
template void DSPLIB_svd_blk_move_ci<float>(float   *pOut,
                                            float   *pIn,
                                            int32_t  Nrows,
                                            int32_t  Ncols,
                                            int32_t  colOutStride,
                                            int32_t  colInStride,
                                            uint8_t *pBlock);
template void DSPLIB_svd_blk_move_ci<double>(double  *pOut,
                                             double  *pIn,
                                             int32_t  Nrows,
                                             int32_t  Ncols,
                                             int32_t  colOutStride,
                                             int32_t  colInStride,
                                             uint8_t *pBlock);

/* ======================================================================== */
/*  End of file:  DSPLIB_svd_common_ci.cpp                                     */
/* ======================================================================== */
