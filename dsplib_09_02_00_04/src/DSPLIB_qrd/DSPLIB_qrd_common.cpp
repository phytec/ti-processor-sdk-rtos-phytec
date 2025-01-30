/*******************************************************************************
**+--------------------------------------------------------------------------+**
**|                            ****                                          |**
**|                            ****                                          |**
**|                            ******o***                                    |**
**|                      ********_///_****                                   |**
**|                      ***** /_//_/ ****                                   |**
**|                       ** ** (__/ ****                                    |**
**|                           *********                                      |**
**|                            ****                                          |**
**|                            ***                                           |**
**|                                                                          |**
**|         Copyright (c) 2016 Texas Instruments Incorporated                |**
**|                        ALL RIGHTS RESERVED                               |**
**|                                                                          |**
**| Permission to use, copy, modify, or distribute this software,            |**
**| whether in part or in whole, for any purpose is forbidden withInvA        |**
**| a signed licensing agreement and NDA from Texas Instruments              |**
**| Incorporated (TI).                                                       |**
**|                                                                          |**
**| TI makes no representation or warranties with respect to the             |**
**| performance of this computer program, and specifically disclaims         |**
**| any responsibility for any damages, special or consequential,            |**
**| connected with the use of this program.                                  |**
**|                                                                          |**
**+--------------------------------------------------------------------------+**
*******************************************************************************/

/*******************************************************************************
 *
 * INCLUDES
 *
 ******************************************************************************/
#include "DSPLIB_qrd_common.h"

/*******************************************************************************
 *
 * DEFINES
 *
 ******************************************************************************/
#define DSPLIB_MATMUL_UNROLL_FACTOR (16)
#define DSPLIB_MATMUL_SE_UNROLL_FACTOR (8)

#define DSPLIB_MATMUL_GEN_UNROLL_FACTOR (8)

#define DSPLIB_MATMUL_UNROLL (256)

/*******************************************************************************
 *
 * INITIALIZATION
 *
 ******************************************************************************/
template <typename dataType>
void DSPLIB_qrd_identity_matrix_generate_init_ci(int32_t nRows, int32_t stride, uint8_t *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   int32_t                                                colStride = stride / sizeof(dataType);
   typedef typename c7x::make_full_vector<dataType>::type vec;
   __SA_VECLEN                                            SA_VECLEN = c7x::sa_veclen<vec>::value;
   __SA_TEMPLATE_v1                                       sa0Params = __gen_SA_TEMPLATE_v1();

   sa0Params.ICNT0         = nRows;
   sa0Params.DIM1          = colStride;
   sa0Params.ICNT1         = nRows;
   sa0Params.DIMFMT        = __SA_DIMFMT_2D;
   sa0Params.VECLEN        = SA_VECLEN;
   sa0Params.DECDIM1       = __SA_DECDIM_DIM1;
   sa0Params.DECDIM1_WIDTH = nRows * colStride;

   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock) = sa0Params;
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_qrd_identity_matrix_generate_init_ci<float>(int32_t nRows, int32_t stride, uint8_t *pBlock);
template void DSPLIB_qrd_identity_matrix_generate_init_ci<double>(int32_t nRows, int32_t strideQ, uint8_t *pBloc);

template <typename dataType>
void DSPLIB_qrd_blk_move_init_ci(int32_t nRows, int32_t nCols, int32_t strideR, uint8_t *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   typedef typename c7x::make_full_vector<dataType>::type vec;

   int32_t colStrideOrder = strideR / sizeof(dataType);

   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<vec>::value;

   __SE_TEMPLATE_v1 seMatrixParams = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 saMatrixParams = __gen_SA_TEMPLATE_v1();

   seMatrixParams.ICNT0   = nCols;
   seMatrixParams.ICNT1   = nRows;
   seMatrixParams.DIM1    = colStrideOrder;
   seMatrixParams.DIMFMT  = __SE_DIMFMT_2D;
   seMatrixParams.ELETYPE = SE_ELETYPE;
   seMatrixParams.VECLEN  = SE_VECLEN;

   saMatrixParams.ICNT0  = nCols;
   saMatrixParams.ICNT1  = nRows;
   saMatrixParams.DIM1   = colStrideOrder;
   saMatrixParams.DIMFMT = __SA_DIMFMT_2D;
   saMatrixParams.VECLEN = SA_VECLEN;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock)                   = seMatrixParams;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (SE_PARAM_SIZE)) = saMatrixParams;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_qrd_blk_move_init_ci<float>(int32_t nRows, int32_t nCols, int32_t strideR, uint8_t *pBlock);
template void DSPLIB_qrd_blk_move_init_ci<double>(int32_t nRows, int32_t nCols, int32_t strideR, uint8_t *pBlock);

template <typename dataType> void DSPLIB_qrd_matTrans_init_ci(uint8_t *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   typedef typename c7x::make_full_vector<dataType>::type vec;

   int32_t      eleCount   = c7x::element_count_of<vec>::value;
   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<vec>::value;

   se0Params.DIMFMT  = __SE_DIMFMT_3D;
   se0Params.ICNT1   = eleCount;
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;
   if (sizeof(dataType) == 4) {
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

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock)                   = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (SE_PARAM_SIZE)) = sa0Params;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void DSPLIB_qrd_matTrans_init_ci<float>(uint8_t *pBlock);
template void DSPLIB_qrd_matTrans_init_ci<double>(uint8_t *pBlock);
template <typename dataType>
DSPLIB_STATUS DSPLIB_matMul_N_unroll_init_ci(int32_t  nRowsIn0,
                                             int32_t  nColsIn0,
                                             int32_t  nColsIn1,
                                             int32_t  strideIn0,
                                             int32_t  strideIn1,
                                             int32_t  strideOut,
                                             uint8_t *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   DSPLIB_STATUS    status = DSPLIB_SUCCESS;
   __SE_TEMPLATE_v1 se0Params;
   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;

   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;
   __SA_VECLEN  SA_VECLEN;



   int32_t M         = nRowsIn0;
   int32_t K         = nColsIn0;
   int32_t N         = nColsIn1;
   int32_t colStrideIn0 = (strideIn0) / sizeof(dataType);
   int32_t colStrideIn1 = (strideIn1) / sizeof(dataType);
   int32_t colStrideOut = (strideOut) / sizeof(dataType);

   typedef typename c7x::make_full_vector<dataType>::type vec;

   int32_t elementCount = c7x::element_count_of<vec>::value;
   SE_VECLEN            = c7x::se_veclen<vec>::value;
   SA_VECLEN            = c7x::sa_veclen<vec>::value;
   SE_ELETYPE           = c7x::se_eletype<vec>::value;

   int32_t NBlocks       = ((N + (DSPLIB_MATMUL_UNROLL_FACTOR - 1)) / ((DSPLIB_MATMUL_UNROLL_FACTOR) *elementCount));

   /**********************************************************************/
   /* Prepare SA template to fetch A matrix                              */
   /**********************************************************************/

   sa0Params        = __gen_SA_TEMPLATE_v1();
   sa0Params.VECLEN = SA_VECLEN;
   sa0Params.DIMFMT = __SA_DIMFMT_4D;

   sa0Params.ICNT0 = 1;
   sa0Params.ICNT1 = K;
   sa0Params.DIM1  = 1;
   sa0Params.ICNT2 = NBlocks;
   sa0Params.DIM2  = 0;
   sa0Params.ICNT3 = M;
   sa0Params.DIM3  = colStrideIn0;

   /**********************************************************************/
   /* Prepare streaming engine 0 to fetch B matrix                       */
   /**********************************************************************/

   se0Params         = __gen_SE_TEMPLATE_v1();
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;
   se0Params.DIMFMT  = __SE_DIMFMT_5D;

   se0Params.ICNT0 = elementCount;
   se0Params.ICNT1 = DSPLIB_MATMUL_SE_UNROLL_FACTOR;
   se0Params.DIM1  = (int32_t)((uint32_t)elementCount <<  (uint32_t)1);
   se0Params.ICNT2 = K;
   se0Params.DIM2  = colStrideIn1;
   se0Params.ICNT3 = NBlocks;
   se0Params.DIM3  = elementCount * DSPLIB_MATMUL_UNROLL_FACTOR;
   se0Params.ICNT4 = M;
   se0Params.DIM4  = 0;

   /**********************************************************************/
   /* Prepare SA template to fetch A matrix                              */
   /**********************************************************************/

   sa1Params        = __gen_SA_TEMPLATE_v1();
   sa1Params.VECLEN = SA_VECLEN;
   sa1Params.DIMFMT = __SA_DIMFMT_4D;

   sa1Params.ICNT0 = elementCount;
   sa1Params.ICNT1 = DSPLIB_MATMUL_UNROLL_FACTOR;
   sa1Params.DIM1  = elementCount;
   sa1Params.ICNT2 = NBlocks;
   sa1Params.DIM2  = elementCount * DSPLIB_MATMUL_UNROLL_FACTOR;
   sa1Params.ICNT3 = M;
   sa1Params.DIM3  = colStrideOut;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock)                       = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_PARAM_SIZE)       = sa0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE)) = sa1Params;

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);
   return status;
}
template DSPLIB_STATUS DSPLIB_matMul_N_unroll_init_ci<float>(int32_t  nRowsIn0,
                                                             int32_t  nColsIn0,
                                                             int32_t  nColsIn1,
                                                             int32_t  strideIn0,
                                                             int32_t  strideIn1,
                                                             int32_t  strideOut,
                                                             uint8_t *pBlock);

template DSPLIB_STATUS DSPLIB_matMul_N_unroll_init_ci<double>(int32_t  nRowsIn0,
                                                              int32_t  nColsIn0,
                                                              int32_t  nColsIn1,
                                                              int32_t  strideIn0,
                                                              int32_t  strideIn1,
                                                              int32_t  strideOut,
                                                              uint8_t *pBlock);

template <typename dataType>
DSPLIB_STATUS DSPLIB_matMul_generic_init_ci(int32_t  nRowsIn0,
                                            int32_t  nColsIn0,
                                            int32_t  nColsIn1,
                                            int32_t  strideIn0,
                                            int32_t  strideIn1,
                                            int32_t  strideOut,
                                            uint8_t *pBlock)
{
   DSPLIB_STATUS    status = DSPLIB_SUCCESS;
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;
   __SA_TEMPLATE_v1 sa2Params;

   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;
   __SA_VECLEN  SA_VECLEN;

   int32_t M            = nRowsIn0;
   int32_t K            = nColsIn0;
   int32_t N            = nColsIn1;
   int32_t colStrideIn0 = (strideIn0) / sizeof(dataType);
   int32_t colStrideIn1 = (strideIn1) / sizeof(dataType);
   int32_t colStrideOut = (strideOut) / sizeof(dataType);

   typedef typename c7x::make_full_vector<dataType>::type vec;

   int32_t elementCount = c7x::element_count_of<vec>::value;
   SE_VECLEN            = c7x::se_veclen<vec>::value;
   SA_VECLEN            = c7x::sa_veclen<vec>::value;
   SE_ELETYPE           = c7x::se_eletype<vec>::value;

   int32_t KBlocks = ((K + elementCount - 1)) / (elementCount);
   int32_t NBlocks = ((N + elementCount - 1)) / (elementCount);

   /**********************************************************************/
   /* Prepare SA0 template to fetch A matrix                             */
   /**********************************************************************/

   sa0Params           = __gen_SA_TEMPLATE_v1();
   sa0Params.VECLEN    = SA_VECLEN;
   sa0Params.DIMFMT    = __SA_DIMFMT_5D;
   sa0Params.DECDIM1   = __SA_DECDIM_DIM2;
   sa0Params.DECDIM1SD = __SA_DECDIMSD_DIM1;

   sa0Params.ICNT0         = 1;
   sa0Params.ICNT1         = DSPLIB_MATMUL_GEN_UNROLL_FACTOR;
   sa0Params.DIM1          = 2;
   sa0Params.ICNT2         = KBlocks;
   sa0Params.DIM2          = (int32_t)((uint32_t)(DSPLIB_MATMUL_GEN_UNROLL_FACTOR) << (uint32_t) 1);
   sa0Params.DECDIM1_WIDTH = K;
   sa0Params.ICNT3         = NBlocks;
   sa0Params.DIM3          = 0;
   sa0Params.ICNT4         = M;
   sa0Params.DIM4          = colStrideIn0;

   /**********************************************************************/
   /* Prepare SA2 template to fetch A matrix                             */
   /**********************************************************************/

   sa2Params           = __gen_SA_TEMPLATE_v1();
   sa2Params.VECLEN    = SA_VECLEN;
   sa2Params.DIMFMT    = __SA_DIMFMT_5D;
   sa2Params.DECDIM1   = __SA_DECDIM_DIM2;
   sa2Params.DECDIM1SD = __SA_DECDIMSD_DIM1;

   sa2Params.ICNT0         = 1;
   sa2Params.ICNT1         = DSPLIB_MATMUL_GEN_UNROLL_FACTOR;
   sa2Params.DIM1          = 2;
   sa2Params.ICNT2         = KBlocks;
   sa2Params.DIM2          = (int32_t) ((uint32_t) (DSPLIB_MATMUL_GEN_UNROLL_FACTOR) << (uint32_t) 1);
   sa2Params.DECDIM1_WIDTH = (uint32_t) (K % 2 == 0 ? K : K - 1);
   sa2Params.ICNT3         = NBlocks;
   sa2Params.DIM3          = 0;
   sa2Params.ICNT4         = M;
   sa2Params.DIM4          = colStrideIn0;

   /**********************************************************************/
   /* Prepare streaming engine 0 to fetch B matrix                       */
   /**********************************************************************/

   se0Params           = __gen_SE_TEMPLATE_v1();
   se0Params.ELETYPE   = SE_ELETYPE;
   se0Params.VECLEN    = SE_VECLEN;
   se0Params.DIMFMT    = __SE_DIMFMT_5D;
   se0Params.DECDIM1   = __SE_DECDIM_DIM2;
   se0Params.DECDIM2   = __SE_DECDIM_DIM3;
   se0Params.DECDIM1SD = __SE_DECDIMSD_DIM1;
   se0Params.DECDIM2SD = __SE_DECDIMSD_DIM0;

   se0Params.ICNT0         = elementCount;
   se0Params.ICNT1         = DSPLIB_MATMUL_GEN_UNROLL_FACTOR;
   se0Params.DIM1          = (int32_t) ((uint32_t) colStrideIn1 << (uint32_t) 1);
   se0Params.ICNT2         = KBlocks;
   se0Params.DIM2          = DSPLIB_MATMUL_GEN_UNROLL_FACTOR * (int32_t) ((uint32_t) (colStrideIn1) << (uint32_t) 1);
   se0Params.DECDIM1_WIDTH = (uint32_t) K * colStrideIn1;
   se0Params.ICNT3         = NBlocks;
   se0Params.DIM3          = elementCount;
   se0Params.DECDIM2_WIDTH = (uint32_t) N;
   se0Params.ICNT4         = M;
   se0Params.DIM4          = 0;

   /**********************************************************************/
   /* Prepare streaming engine 1 to fetch B matrix                       */
   /**********************************************************************/

   se1Params           = __gen_SE_TEMPLATE_v1();
   se1Params.ELETYPE   = SE_ELETYPE;
   se1Params.VECLEN    = SE_VECLEN;
   se1Params.DIMFMT    = __SE_DIMFMT_5D;
   se1Params.DECDIM1   = __SE_DECDIM_DIM2;
   se1Params.DECDIM2   = __SE_DECDIM_DIM3;
   se1Params.DECDIM1SD = __SE_DECDIMSD_DIM1;
   se1Params.DECDIM2SD = __SE_DECDIMSD_DIM0;

   se1Params.ICNT0         = elementCount;
   se1Params.ICNT1         = DSPLIB_MATMUL_GEN_UNROLL_FACTOR;
   se1Params.DIM1          = (int32_t)((uint32_t)colStrideIn1 << (uint32_t) 1);
   se1Params.ICNT2         = KBlocks;
   se1Params.DIM2          = DSPLIB_MATMUL_GEN_UNROLL_FACTOR * (int32_t)((uint32_t)(colStrideIn1) << (uint32_t) 1);
   se1Params.DECDIM1_WIDTH = (uint32_t) (K % 2 == 0 ? K : K - 1) * colStrideIn1;
   se1Params.ICNT3         = NBlocks;
   se1Params.DIM3          = elementCount;
   se1Params.DECDIM2_WIDTH = (uint32_t) N;
   se1Params.ICNT4         = M;
   se1Params.DIM4          = 0;

   /**********************************************************************/
   /* Prepare SA template to write C matrix                              */
   /**********************************************************************/

   sa1Params           = __gen_SA_TEMPLATE_v1();
   sa1Params.VECLEN    = SA_VECLEN;
   sa1Params.DIMFMT    = __SA_DIMFMT_3D;
   sa1Params.DECDIM1   = __SA_DECDIM_DIM1;
   sa1Params.DECDIM1SD = __SA_DECDIMSD_DIM0;

   sa1Params.ICNT0         = elementCount;
   sa1Params.ICNT1         = NBlocks;
   sa1Params.DIM1          = elementCount;
   sa1Params.DECDIM1_WIDTH = N;
   sa1Params.ICNT2         = M;
   sa1Params.DIM2          = colStrideOut;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock)                       = se0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (SE_PARAM_SIZE))     = se1Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE)) = sa0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE)) = sa1Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE)) = sa2Params;

   return status;
}
template DSPLIB_STATUS DSPLIB_matMul_generic_init_ci<float>(int32_t  nRowsIn0,
                                                            int32_t  nColsIn0,
                                                            int32_t  nColsIn1,
                                                            int32_t  strideIn0,
                                                            int32_t  strideIn1,
                                                            int32_t  strideOut,
                                                            uint8_t *pBlock);
template DSPLIB_STATUS DSPLIB_matMul_generic_init_ci<double>(int32_t  nRowsIn0,
                                                             int32_t  nColsIn0,
                                                             int32_t  nColsIn1,
                                                             int32_t  strideIn0,
                                                             int32_t  strideIn1,
                                                             int32_t  strideOut,
                                                             uint8_t *pBlock);
template <typename dataType>
DSPLIB_STATUS DSPLIB_matMul_init_ci(int32_t  nRowsIn0,
                                    int32_t  nColsIn0,
                                    int32_t  nColsIn1,
                                    int32_t  strideIn0,
                                    int32_t  strideIn1,
                                    int32_t  strideOut,
                                    uint8_t *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   if (nColsIn1 % DSPLIB_MATMUL_UNROLL == 0) {
      DSPLIB_matMul_N_unroll_init_ci<dataType>(nRowsIn0, nColsIn0, nColsIn1, strideIn0, strideOut, strideIn1, pBlock);
   }
   else {
      DSPLIB_matMul_generic_init_ci<dataType>(nRowsIn0, nColsIn0, nColsIn1, strideIn0, strideOut, strideIn1, pBlock);
   }
  DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
  return DSPLIB_SUCCESS;

}                                                     
template DSPLIB_STATUS DSPLIB_matMul_init_ci<float>(int32_t  nRowsIn0,
                                                    int32_t  nColsIn0,
                                                    int32_t  nColsIn1,
                                                    int32_t  strideIn0,
                                                    int32_t  strideIn1,
                                                    int32_t  strideOut,
                                                    uint8_t *pBlock);
template DSPLIB_STATUS DSPLIB_matMul_init_ci<double>(int32_t  nRowsIn0,
                                                     int32_t  nColsIn0,
                                                     int32_t  nColsIn1,
                                                     int32_t  strideIn0,
                                                     int32_t  strideIn1,
                                                     int32_t  strideOut,
                                                     uint8_t *pBlock);
/*******************************************************************************
 *
 * IMPLEMENTATION
 *
 ******************************************************************************/
template <typename dataType>
void DSPLIB_qrd_identity_matrix_generate_exec_ci(dataType *pMat, int32_t nRows, int32_t colStride, uint8_t *pBlock)
{

   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   typedef typename c7x::make_full_vector<dataType>::type vec;

   uint32_t eleCount = c7x::element_count_of<vec>::value;

   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock);
   __SA_TEMPLATE_v1 sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock);

   int32_t iter1   = DSPLIB_ceilingDiv(nRows, eleCount);
   vec     zeroVec = (vec) 0;

   __SA0_OPEN(sa0Params);
   for (int32_t block = 0; block < nRows; block++) {

      for (int32_t horizontal = 0; horizontal < iter1; horizontal++) {
         __vpred pred      = c7x::strm_agen<0, vec>::get_vpred();
         vec    *pStoreVec = c7x::strm_agen<0, vec>::get_adv(pMat);
         __vstore_pred(pred, pStoreVec, zeroVec);
      }
   }
   __SA0_CLOSE();

   sa1Params.ICNT0 = 1;
   sa1Params.ICNT1 = nRows / 2;
   sa1Params.DIM1  = 2 * (colStride + 1);

   sa0Params.ICNT0 = 1;
   sa0Params.ICNT1 = nRows - sa1Params.ICNT1;
   sa0Params.DIM1  = 2 * (colStride + 1);

   __SA0_OPEN(sa0Params);
   if (sa1Params.ICNT1 != 0) {
      __SA1_OPEN(sa1Params);
   }

   for (int32_t diag = 0; diag < nRows; diag += 2) {
      dataType *pDiag  = c7x::strm_agen<0, dataType>::get_adv(pMat);
      *pDiag           = 1.0;
      dataType *pDiag1 = c7x::strm_agen<1, dataType>::get_adv(pMat + (colStride + 1));
      *pDiag1          = 1.0;
   }
   __SA0_CLOSE();
   __SA1_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
}
template void
DSPLIB_qrd_identity_matrix_generate_exec_ci<float>(float *pMat, int32_t nRows, int32_t colStride, uint8_t *pBlock);
template void
DSPLIB_qrd_identity_matrix_generate_exec_ci<double>(double *pMat, int32_t nRows, int32_t colStride, uint8_t *pBlock);

template <typename dataType>
void DSPLIB_qrd_blk_move_exec_ci(dataType *pOut,
                                 dataType *pIn,
                                 int32_t   Nrows,
                                 int32_t   Ncols,
                                 int32_t   colOutStride,
                                 int32_t   colInStride,
                                 uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock);
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (SE_PARAM_SIZE));

   typedef typename c7x::make_full_vector<dataType>::type vec;

   int32_t eleCount  = c7x::element_count_of<vec>::value;
   int32_t nVec      = DSPLIB_ceilingDiv(Ncols, eleCount);
   int32_t loopCount = nVec * Nrows;

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
template void DSPLIB_qrd_blk_move_exec_ci<float>(float   *pOut,
                                                 float   *pIn,
                                                 int32_t  Nrows,
                                                 int32_t  Ncols,
                                                 int32_t  colOutStride,
                                                 int32_t  colInStride,
                                                 uint8_t *pBlock);
template void DSPLIB_qrd_blk_move_exec_ci<double>(double  *pOut,
                                                  double  *pIn,
                                                  int32_t  Nrows,
                                                  int32_t  Ncols,
                                                  int32_t  colOutStride,
                                                  int32_t  colInStride,
                                                  uint8_t *pBlock);

template <typename dataType>
void DSPLIB_qrd_matTrans_exec_ci(dataType *pIn,
                                 dataType *pOut,
                                 int32_t   nRows,
                                 int32_t   nCols,
                                 int32_t   colInStride,
                                 int32_t   colOutStride,
                                 uint8_t  *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock);
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (SA_PARAM_SIZE));

   typedef typename c7x::make_full_vector<dataType>::type vec;

   int32_t eleCount = c7x::element_count_of<vec>::value;
   int32_t nVec     = DSPLIB_ceilingDiv(nRows, (eleCount * 2));
   se0Params.ICNT0  = nCols;
   se0Params.DIM1   = colInStride;
   se0Params.ICNT2  = nVec;
   se0Params.DIM2   = colInStride * eleCount * 2;

   sa0Params.ICNT1         = nCols;
   sa0Params.DIM1          = colOutStride;
   sa0Params.ICNT2         = nVec;
   sa0Params.DECDIM1_WIDTH = nRows;

   int32_t loopCount = nVec * nCols;

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
template void DSPLIB_qrd_matTrans_exec_ci<float>(float   *pIn,
                                                 float   *pOut,
                                                 int32_t  nRows,
                                                 int32_t  nCols,
                                                 int32_t  colInStride,
                                                 int32_t  colOutStride,
                                                 uint8_t *pBlock);
template void DSPLIB_qrd_matTrans_exec_ci<double>(double  *pIn,
                                                  double  *pOut,
                                                  int32_t  nRows,
                                                  int32_t  nCols,
                                                  int32_t  colInStride,
                                                  int32_t  colOutStride,
                                                  uint8_t *pBlock);

template <typename T, typename vec, uint32_t id> static inline vec loadAMatSA(__vpred tmp, void *pIn)
{

   vec out;

   tmp = c7x::strm_agen<id, T>::get_vpred();
   out = __vload_pred_dup(tmp, (c7x::strm_agen<id, T>::get_adv(pIn)));

   return out;
}

template <typename T, typename vec> static inline void writeOutSA1(__vpred tmp, vec *addr, T pOut, vec out)
{

   tmp  = c7x::strm_agen<1, vec>::get_vpred();
   addr = c7x::strm_agen<1, vec>::get_adv(pOut);
   __vstore_pred(tmp, addr, out);
   return;
}
template <typename dataType>
DSPLIB_STATUS DSPLIB_matMul_N_unroll_exec_ci(dataType *restrict pIn0,
                                             dataType *restrict pIn1,
                                             dataType *restrict pOut,
                                             int32_t  nRowsIn0,
                                             int32_t  nColsIn0,
                                             int32_t  nColsIn1,
                                             int32_t  strideIn1,
                                             uint8_t *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   int32_t M   = nRowsIn0;
   int32_t K   = nColsIn0;
   int32_t N   = nColsIn1;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   int32_t elementCount = c7x::element_count_of<vec>::value;
   int32_t NBlocks      = ((N + (DSPLIB_MATMUL_UNROLL_FACTOR - 1)) / ((DSPLIB_MATMUL_UNROLL_FACTOR) *elementCount));

   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_matMul_exec_ci\n");
#endif

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock);
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock);

   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_PARAM_SIZE);
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));

   // Input samples
   __SE0_OPEN(pIn1, se0Params);
   __SE1_OPEN(((dataType *) pIn1 + elementCount), se1Params);

   // Output samples
   __SA0_OPEN(sa0Params);
   __SA1_OPEN(sa1Params);

   /* vec     c; */
   vec a;
   vec b;
   vec r00, r01, r03, r02, r04, r05, r06, r07;
   vec r08, r09, r0a, r0b, r0c, r0d, r0e, r0f;

   __vpred tmp;
   vec    *addr;

   /* printf("\nIn _ci.cpp M = %d, NBlocks = %d, K = %d: \n", M, NBlocks, K); */
   for (int32_t m = 0; m < M; m++) {
      /* #pragma MUST_ITERATE(2, , 2) */
      for (int32_t n = 0; n < NBlocks; n++) {
         /* for (int32_t n = 0; n < 1; n++) { */

         r00 = (vec) 0;
         r01 = (vec) 0;
         r02 = (vec) 0;
         r03 = (vec) 0;
         r04 = (vec) 0;
         r05 = (vec) 0;
         r06 = (vec) 0;
         r07 = (vec) 0;
         r08 = (vec) 0;
         r09 = (vec) 0;
         r0a = (vec) 0;
         r0b = (vec) 0;
         r0c = (vec) 0;
         r0d = (vec) 0;
         r0e = (vec) 0;
         r0f = (vec) 0;

         for (int32_t k = 0; k < K; k++) {
            /* printf("Iteration count m = %d, nBlocks= %d,  = %d\n", m, n, k); */
            dataType *addrA = (c7x::strm_agen<0, dataType>::get_adv(pIn0));
            a               = __vload_dup(addrA);

            /* printf("Vector a below:\n"); */

            b = c7x::strm_eng<0, vec>::get_adv();
            r00 += a * b;

            b = c7x::strm_eng<1, vec>::get_adv();
            r01 += a * b;

            b = c7x::strm_eng<0, vec>::get_adv();
            /* printf("Vector b below:\n"); */
            r02 += a * b;
            /* printf("Vector r02 below:\n"); */

            b = c7x::strm_eng<1, vec>::get_adv();
            r03 += a * b;

            b = c7x::strm_eng<0, vec>::get_adv();
            r04 += a * b;

            b = c7x::strm_eng<1, vec>::get_adv();
            r05 += a * b;

            b = c7x::strm_eng<0, vec>::get_adv();
            r06 += a * b;

            b = c7x::strm_eng<1, vec>::get_adv();
            r07 += a * b;

            b = c7x::strm_eng<0, vec>::get_adv();
            r08 += a * b;

            b = c7x::strm_eng<1, vec>::get_adv();
            r09 += a * b;

            b = c7x::strm_eng<0, vec>::get_adv();
            r0a += a * b;

            b = c7x::strm_eng<1, vec>::get_adv();
            r0b += a * b;

            b = c7x::strm_eng<0, vec>::get_adv();
            r0c += a * b;

            b = c7x::strm_eng<1, vec>::get_adv();
            r0d += a * b;

            b = c7x::strm_eng<0, vec>::get_adv();
            r0e += a * b;

            b = c7x::strm_eng<1, vec>::get_adv();
            r0f += a * b;
         }

         writeOutSA1(tmp, addr, pOut, r00);
         writeOutSA1(tmp, addr, pOut, r01);
         writeOutSA1(tmp, addr, pOut, r02);
         writeOutSA1(tmp, addr, pOut, r03);
         writeOutSA1(tmp, addr, pOut, r04);
         writeOutSA1(tmp, addr, pOut, r05);
         writeOutSA1(tmp, addr, pOut, r06);
         writeOutSA1(tmp, addr, pOut, r07);
         writeOutSA1(tmp, addr, pOut, r08);
         writeOutSA1(tmp, addr, pOut, r09);
         writeOutSA1(tmp, addr, pOut, r0a);
         writeOutSA1(tmp, addr, pOut, r0b);
         writeOutSA1(tmp, addr, pOut, r0c);
         writeOutSA1(tmp, addr, pOut, r0d);
         writeOutSA1(tmp, addr, pOut, r0e);
         writeOutSA1(tmp, addr, pOut, r0f);
      }
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_matMul_N_unroll_exec_ci<float>(float *restrict pIn0,
                                                             float *restrict pIn1,
                                                             float *restrict pOut,
                                                             int32_t  nRowsIn0,
                                                             int32_t  nColsIn0,
                                                             int32_t  nColsIn1,
                                                             int32_t  strideIn1,
                                                             uint8_t *pBlock);

template DSPLIB_STATUS DSPLIB_matMul_N_unroll_exec_ci<double>(double *restrict pIn0,
                                                              double *restrict pIn1,
                                                              double *restrict pOut,
                                                              int32_t  nRowsIn0,
                                                              int32_t  nColsIn0,
                                                              int32_t  nColsIn1,
                                                              int32_t  strideIn1,
                                                              uint8_t *pBlock);


template <typename dataType>
DSPLIB_STATUS DSPLIB_matMul_generic_exec_ci(dataType *restrict pIn0,
                                    dataType *restrict pIn1,
                                    dataType *restrict pOut,
                                    int32_t  nRowsIn0,
                                    int32_t  nColsIn0,
                                    int32_t  nColsIn1,
                                    int32_t  strideIn1,
                                    uint8_t *pBlock)
{

   int32_t M = nRowsIn0;
   int32_t K = nColsIn0;
   int32_t N = nColsIn1;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   int32_t elementCount = c7x::element_count_of<vec>::value;
   int32_t KBlocks      = ((K + elementCount - 1)) / (elementCount);
   int32_t NBlocks      = ((N + elementCount - 1)) / (elementCount);

   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;
   __SA_TEMPLATE_v1 sa2Params;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_matMul_generic_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<dataType>::type vec;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock);
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + (SE_PARAM_SIZE));

   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (2 * SE_PARAM_SIZE));
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (3 * SE_PARAM_SIZE));
   sa2Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + (4 * SE_PARAM_SIZE));

   // Input samples
   __SE0_OPEN(pIn1, se0Params);
   __SE1_OPEN(((dataType *) pIn1 + strideIn1), se1Params);

   // Output samples
   __SA0_OPEN(sa0Params);
   __SA2_OPEN(sa2Params);
   __SA1_OPEN(sa1Params);

   vec a;
   vec b;

   vec r00, r01, r03, r02, r04, r05, r06, r07;
   vec r08, r09, r0a, r0b, r0c, r0d, r0e, r0f;

   vec c0 = vec(0);
   vec c1 = vec(0);
   vec c2 = vec(0);
   vec c3 = vec(0);
   vec c4, c5, c6, c7;

   __vpred tmp;
   vec    *addr;

   DSPLIB_DEBUGPRINTFN(1, "\nIn _ci.cpp M = %d, NBlocks = %d, KBlocks = %d: \n", M, NBlocks, KBlocks);

   vec a00, a01, a02, a03;
   vec a04, a05, a06, a07;

   dataType *pIn0Local = ((dataType *) pIn0 + 1);

   __vpred predA;
   a00 = loadAMatSA<dataType, vec, 2>(predA, pIn0Local);
   a01 = loadAMatSA<dataType, vec, 2>(predA, pIn0Local);
   a02 = loadAMatSA<dataType, vec, 2>(predA, pIn0Local);
   a03 = loadAMatSA<dataType, vec, 2>(predA, pIn0Local);
   a04 = loadAMatSA<dataType, vec, 2>(predA, pIn0Local);
   a05 = loadAMatSA<dataType, vec, 2>(predA, pIn0Local);
   a06 = loadAMatSA<dataType, vec, 2>(predA, pIn0Local);
   a07 = loadAMatSA<dataType, vec, 2>(predA, pIn0Local);

   for (int32_t mn = 0; mn < M * NBlocks; mn++) {

      r00 = (vec) 0;
      r01 = (vec) 0;
      r02 = (vec) 0;
      r03 = (vec) 0;
      r04 = (vec) 0;
      r05 = (vec) 0;
      r06 = (vec) 0;
      r07 = (vec) 0;
      r08 = (vec) 0;
      r09 = (vec) 0;
      r0a = (vec) 0;
      r0b = (vec) 0;
      r0c = (vec) 0;
      r0d = (vec) 0;
      r0e = (vec) 0;
      r0f = (vec) 0;

      for (int32_t k = 0; k < KBlocks; k++) {

         a = loadAMatSA<dataType, vec, 0>(predA, pIn0);
         b = c7x::strm_eng<0, vec>::get_adv();
         r00 += a * b;

         b = c7x::strm_eng<1, vec>::get_adv();
         r01 += a00 * b;

         a = loadAMatSA<dataType, vec, 0>(predA, pIn0);
         b = c7x::strm_eng<0, vec>::get_adv();
         r02 += a * b;

         b = c7x::strm_eng<1, vec>::get_adv();
         r03 += a01 * b;

         a = loadAMatSA<dataType, vec, 0>(predA, pIn0);
         b = c7x::strm_eng<0, vec>::get_adv();
         r04 += a * b;

         b = c7x::strm_eng<1, vec>::get_adv();
         r05 += a02 * b;

         a = loadAMatSA<dataType, vec, 0>(predA, pIn0);
         b = c7x::strm_eng<0, vec>::get_adv();
         r06 += a * b;

         b = c7x::strm_eng<1, vec>::get_adv();
         r07 += a03 * b;

         a = loadAMatSA<dataType, vec, 0>(predA, pIn0);
         b = c7x::strm_eng<0, vec>::get_adv();
         r08 += a * b;

         b = c7x::strm_eng<1, vec>::get_adv();
         r09 += a04 * b;

         a = loadAMatSA<dataType, vec, 0>(predA, pIn0);
         b = c7x::strm_eng<0, vec>::get_adv();
         r0a += a * b;

         b = c7x::strm_eng<1, vec>::get_adv();
         r0b += a05 * b;

         a = loadAMatSA<dataType, vec, 0>(predA, pIn0);
         b = c7x::strm_eng<0, vec>::get_adv();
         r0c += a * b;

         b = c7x::strm_eng<1, vec>::get_adv();
         r0d += a06 * b;

         a = loadAMatSA<dataType, vec, 0>(predA, pIn0);
         b = c7x::strm_eng<0, vec>::get_adv();
         r0e += a * b;

         b = c7x::strm_eng<1, vec>::get_adv();
         r0f += a07 * b;

         a00 = loadAMatSA<dataType, vec, 2>(predA, pIn0Local);
         a01 = loadAMatSA<dataType, vec, 2>(predA, pIn0Local);
         a02 = loadAMatSA<dataType, vec, 2>(predA, pIn0Local);
         a03 = loadAMatSA<dataType, vec, 2>(predA, pIn0Local);

         a04 = loadAMatSA<dataType, vec, 2>(predA, pIn0Local);
         a05 = loadAMatSA<dataType, vec, 2>(predA, pIn0Local);
         a06 = loadAMatSA<dataType, vec, 2>(predA, pIn0Local);
         a07 = loadAMatSA<dataType, vec, 2>(predA, pIn0Local);
      }

      c0 = r00 + r01;
      c1 = r02 + r03;
      c2 = r04 + r05;
      c3 = r06 + r07;
      c4 = r08 + r09;
      c5 = r0a + r0b;
      c6 = r0c + r0d;
      c7 = r0e + r0f;

      c0 += c1;
      c2 += c3;
      c4 += c5;
      c6 += c7;

      c0 += c2;
      c4 += c6;
      c0 += c4;

      writeOutSA1(tmp, addr, pOut, c0);
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();

   return DSPLIB_SUCCESS;
}
template DSPLIB_STATUS DSPLIB_matMul_generic_exec_ci<float>(float *restrict pIn0,
                                                            float *restrict pIn1,
                                                            float *restrict pOut,
                                                            int32_t  nRowsIn0,
                                                            int32_t  nColsIn0,
                                                            int32_t  nColsIn1,
                                                            int32_t  strideIn1,
                                                            uint8_t *pBlock);
template DSPLIB_STATUS DSPLIB_matMul_generic_exec_ci<double>(double *restrict pIn0,
                                                             double *restrict pIn1,
                                                             double *restrict pOut,
                                                             int32_t  nRowsIn0,
                                                             int32_t  nColsIn0,
                                                             int32_t  nColsIn1,
                                                             int32_t  strideIn1,
                                                             uint8_t *pBlock);
template <typename dataType>
DSPLIB_STATUS DSPLIB_matMul_exec_ci(dataType *restrict pIn0,
                                    dataType *restrict pIn1,
                                    dataType *restrict pOut,
                                    int32_t  nRowsIn0,
                                    int32_t  nColsIn0,
                                    int32_t  nColsIn1,
                                    int32_t  strideIn1,
                                    uint8_t *pBlock)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   if (nColsIn1 % DSPLIB_MATMUL_UNROLL == 0) {
      DSPLIB_matMul_N_unroll_exec_ci<dataType>(pIn0, pIn1, pOut, nRowsIn0, nColsIn0, nColsIn1, strideIn1, pBlock);
   }
   else {
      DSPLIB_matMul_generic_exec_ci<dataType>(pIn0, pIn1, pOut, nRowsIn0, nColsIn0, nColsIn1, strideIn1, pBlock);
   }
   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", DSPLIB_SUCCESS);
  return DSPLIB_SUCCESS;
}


template DSPLIB_STATUS DSPLIB_matMul_exec_ci<float>(float *restrict pIn0,
                                                    float *restrict pIn1,
                                                    float *restrict pOut,
                                                    int32_t  nRowsIn0,
                                                    int32_t  nColsIn0,
                                                    int32_t  nColsIn1,
                                                    int32_t  strideIn1,
                                                    uint8_t *pBlock);
template DSPLIB_STATUS DSPLIB_matMul_exec_ci<double>(double *restrict pIn0,
                                                     double *restrict pIn1,
                                                     double *restrict pOut,
                                                     int32_t  nRowsIn0,
                                                     int32_t  nColsIn0,
                                                     int32_t  nColsIn1,
                                                     int32_t  strideIn1,
                                                     uint8_t *pBlock);
/* ======================================================================== */
/*  End of file:  DSPLIB_qrd_common.cpp                                     */
/* ======================================================================== */
