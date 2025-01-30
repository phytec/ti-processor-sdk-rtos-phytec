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

#include "VXLIB_tableLookup_priv.h"
#include "VXLIB_types.h"
#include "VXLIB_utility.h"
#include "c7x_scalable.h"
#include <type_traits>
/**********************************************************************************************************************/
/*                                                                                                                    */
/* DEFINES                                                                                                            */
/*                                                                                                                    */
/**********************************************************************************************************************/
#define VXLIB_COUNT_LIMIT 512
/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_tableLookup_init_ci                                                                                          */
/*                                                                                                                    */
/**********************************************************************************************************************/
// this method initializes the kernel-specific parameters
// mainly, the streaming engine and streaming address generators
template <uint32_t dTypeIn, uint32_t dTypeOut>
VXLIB_STATUS VXLIB_tableLookup_checkSrcIdx_init_ci(VXLIB_kernelHandle handle)
{
#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_tableLookup_checkSrcIdx_init_ci\n");
#endif
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   // structs to hold SE and SA parameters
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_tableLookup_PrivArgs *pKerPrivArgs = (VXLIB_tableLookup_PrivArgs *) handle;

   // obtain image size and compute number of blocks to process
   size_t width  = pKerPrivArgs->width;
   size_t height = pKerPrivArgs->height;

   typedef typename VXLIB_vec_type<dTypeIn>::type vec;

   size_t   elemCount = c7x::element_count_of<vec>::value;
   uint32_t nTiles    = (width + elemCount - 1) / elemCount;

   uint32_t ICNT2    = (height + 1) / 2;
   int32_t  strideIn = pKerPrivArgs->strideInElements;

   uint8_t *pBlock = pKerPrivArgs->bufPblock; // address to retrieve to store SE/SA params

   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<c7x::uint_vec>::value;
   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec>::value;

   //  set SE0, and SA0 params
   se0Params.ICNT0         = width;
   se0Params.DIM1          = strideIn * 2;
   se0Params.ICNT1         = ICNT2;
   se0Params.DIMFMT        = __SE_DIMFMT_2D;
   se0Params.VECLEN        = SE_VECLEN;
   se0Params.DECDIM1       = __SE_DECDIM_DIM1;
   se0Params.DECDIM1SD     = __SE_DECDIMSD_DIM0;
   se0Params.DECDIM1_WIDTH = height * strideIn;
   se0Params.ELETYPE       = SE_ELETYPE;

   pKerPrivArgs->numBlocksIdxCheck = ICNT2 * nTiles;

   /**************************/
   /* Store SE and SA params */
   /**************************/
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET) = se0Params;
#if VXLIB_DEBUGPRINT
   printf("Exit VXLIB_tableLookup_checkSrcIdx_init_ci\n");
#endif
   return status;
}
template VXLIB_STATUS VXLIB_tableLookup_checkSrcIdx_init_ci<VXLIB_TABLELOOKUP_DTYPE_I8U_O8U>(VXLIB_kernelHandle handle);
template VXLIB_STATUS VXLIB_tableLookup_checkSrcIdx_init_ci<VXLIB_TABLELOOKUP_DTYPE_I8S_O8S>(VXLIB_kernelHandle handle);
template VXLIB_STATUS
VXLIB_tableLookup_checkSrcIdx_init_ci<VXLIB_TABLELOOKUP_DTYPE_I16U_O16U>(VXLIB_kernelHandle handle);
template VXLIB_STATUS
VXLIB_tableLookup_checkSrcIdx_init_ci<VXLIB_TABLELOOKUP_DTYPE_I16S_O16S>(VXLIB_kernelHandle handle);
template <uint32_t dTypeIn, uint32_t dTypeOut>
VXLIB_STATUS VXLIB_tableLookup_init_ci(VXLIB_kernelHandle                handle,
                                       const VXLIB_bufParams2D_t        *bufParamsIn,
                                       const VXLIB_bufParams2D_t        *bufParamsOut,
                                       const VXLIB_bufParams1D_t        *bufParamsLut,
                                       const VXLIB_tableLookup_InitArgs *pKerInitArgs)
{
#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_tableLookup_init_ci\n");
#endif
   VXLIB_STATUS                status       = VXLIB_SUCCESS; // assign status to success by default

   status = VXLIB_tableLookup_checkSrcIdx_init_ci<dTypeIn, dTypeOut>(handle);

   VXLIB_tableLookup_PrivArgs *pKerPrivArgs = (VXLIB_tableLookup_PrivArgs *) handle;
   uint32_t                    count        = pKerPrivArgs->pKerInitArgs.count;
   if (count > VXLIB_COUNT_LIMIT) {
      status = VXLIB_tableLookup_generic_init_ci<dTypeIn, dTypeOut>(handle, bufParamsIn, bufParamsOut, bufParamsLut, pKerInitArgs);
   }
   else {
      status = VXLIB_tableLookup_lut_init_ci<dTypeIn, dTypeOut>(handle, bufParamsIn, bufParamsOut, bufParamsLut, pKerInitArgs);
   }

#if VXLIB_DEBUGPRINT
   printf("Exit VXLIB_tableLookup_init_ci\n");
#endif
   return status;
}
template VXLIB_STATUS
VXLIB_tableLookup_init_ci<VXLIB_TABLELOOKUP_DTYPE_I8U_O8U>(VXLIB_kernelHandle                handle,
                                                           const VXLIB_bufParams2D_t *       bufParamsIn,
                                                           const VXLIB_bufParams2D_t *       bufParamsOut,
                                                           const VXLIB_bufParams1D_t *       bufParamsLut,
                                                           const VXLIB_tableLookup_InitArgs *pKerInitArgs);
template VXLIB_STATUS
VXLIB_tableLookup_init_ci<VXLIB_TABLELOOKUP_DTYPE_I8S_O8S>(VXLIB_kernelHandle                handle,
                                                           const VXLIB_bufParams2D_t *       bufParamsIn,
                                                           const VXLIB_bufParams2D_t *       bufParamsOut,
                                                           const VXLIB_bufParams1D_t *       bufParamsLut,
                                                           const VXLIB_tableLookup_InitArgs *pKerInitArgs);
template VXLIB_STATUS
VXLIB_tableLookup_init_ci<VXLIB_TABLELOOKUP_DTYPE_I16U_O16U>(VXLIB_kernelHandle                handle,
                                                             const VXLIB_bufParams2D_t *       bufParamsIn,
                                                             const VXLIB_bufParams2D_t *       bufParamsOut,
                                                             const VXLIB_bufParams1D_t *       bufParamsLut,
                                                             const VXLIB_tableLookup_InitArgs *pKerInitArgs);
template VXLIB_STATUS
VXLIB_tableLookup_init_ci<VXLIB_TABLELOOKUP_DTYPE_I16S_O16S>(VXLIB_kernelHandle                handle,
                                                             const VXLIB_bufParams2D_t *       bufParamsIn,
                                                             const VXLIB_bufParams2D_t *       bufParamsOut,
                                                             const VXLIB_bufParams1D_t *       bufParamsLut,
                                                             const VXLIB_tableLookup_InitArgs *pKerInitArgs);

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_tableLookup_set_ci                                                                                           */
/*                                                                                                                    */
/**********************************************************************************************************************/

template <typename dataType> VXLIB_STATUS VXLIB_tableLookup_set_ci(VXLIB_kernelHandle handle, void *restrict lut)
{
#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_tableLookup_set_ci\n");
#endif
   VXLIB_STATUS                status       = VXLIB_SUCCESS; // assign status to success by default

   VXLIB_tableLookup_PrivArgs *pKerPrivArgs = (VXLIB_tableLookup_PrivArgs *) handle;
   uint32_t                    count        = pKerPrivArgs->pKerInitArgs.count;
   if (count > VXLIB_COUNT_LIMIT) {
      status = VXLIB_tableLookup_generic_set_ci<dataType>(handle, lut);
   }
   else {
      status = VXLIB_tableLookup_lut_set_ci<dataType>(handle, lut);
   }

#if VXLIB_DEBUGPRINT
   printf("Exit VXLIB_tableLookup_set_ci\n");
#endif
   return (status);
}
template VXLIB_STATUS VXLIB_tableLookup_set_ci<uint8_t>(VXLIB_kernelHandle handle, void *restrict lut);
template VXLIB_STATUS VXLIB_tableLookup_set_ci<int8_t>(VXLIB_kernelHandle handle, void *restrict lut);
template VXLIB_STATUS VXLIB_tableLookup_set_ci<uint16_t>(VXLIB_kernelHandle handle, void *restrict lut);
template VXLIB_STATUS VXLIB_tableLookup_set_ci<int16_t>(VXLIB_kernelHandle handle, void *restrict lut);

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_tableLookup_exec_ci                                                                                          */
/*                                                                                                                    */
/**********************************************************************************************************************/
template <typename dataType>
VXLIB_STATUS VXLIB_tableLookup_checkSrcIdx_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn);

template <> VXLIB_STATUS VXLIB_tableLookup_checkSrcIdx_exec_ci<uint8_t>(VXLIB_kernelHandle handle, void *restrict pIn)
{
#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_tableLookup_checkSrcIdx_exec_ci\n");
#endif
   VXLIB_STATUS                status       = VXLIB_SUCCESS; // assign status to success by default
   VXLIB_tableLookup_PrivArgs *pKerPrivArgs = (VXLIB_tableLookup_PrivArgs *) handle;
   uint32_t                    count        = pKerPrivArgs->pKerInitArgs.count;
   uint32_t                    offset       = pKerPrivArgs->pKerInitArgs.offset;
   size_t                      height       = pKerPrivArgs->height;
   int32_t                     strideIn     = pKerPrivArgs->strideInElements;

   uint8_t *pBlock            = pKerPrivArgs->bufPblock; // address to retrieve to store SE/SA params
   int32_t  numBlocksIdxCheck = (int32_t) pKerPrivArgs->numBlocksIdxCheck;

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   se1Params.DECDIM1_WIDTH    = (height % 2 == 0) ? (height * strideIn) : ((height - 1) * strideIn);

   typedef typename c7x::make_full_vector<uint8_t>::type vec;

   vec offsetVec = (vec) offset;
   vec countVec  = (vec) (count - 1);
   countVec -= offsetVec;

   uint8_t *pInLocal = (uint8_t *) pIn;

   vec flagVec0 = (vec) 0;
   vec flagVec1 = (vec) 0;
   vec oneVec   = (vec) 1;

   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pInLocal + strideIn, se1Params);

   for (int32_t i = 0; i < numBlocksIdxCheck; i++) {
      vec srcVec0 = c7x::strm_eng<0, vec>::get_adv();
      vec srcVec1 = c7x::strm_eng<1, vec>::get_adv();

      __vpred vpred0 = __cmp_gt_pred(srcVec0, countVec);
      __vpred vpred1 = __cmp_gt_pred(srcVec1, countVec);

      flagVec0 = __select(vpred0, oneVec, flagVec0);
      flagVec1 = __select(vpred1, oneVec, flagVec1);
   }
   __SE0_CLOSE();
   __SE1_CLOSE();
   flagVec0 += flagVec1;
   uint32_t flag = __horizontal_add(flagVec0);
   status        = (flag == 0) ? VXLIB_SUCCESS : VXLIB_ERR_INVALID_DIMENSION;
#if VXLIB_DEBUGPRINT
   printf("Exit VXLIB_tableLookup_checkSrcIdx_exec_ci \n");
#endif

   return (status);
}

template <> VXLIB_STATUS VXLIB_tableLookup_checkSrcIdx_exec_ci<uint16_t>(VXLIB_kernelHandle handle, void *restrict pIn)
{
#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_tableLookup_checkSrcIdx_exec_ci\n");
#endif
   VXLIB_STATUS                status       = VXLIB_SUCCESS; // assign status to success by default
   VXLIB_tableLookup_PrivArgs *pKerPrivArgs = (VXLIB_tableLookup_PrivArgs *) handle;
   uint32_t                    count        = pKerPrivArgs->pKerInitArgs.count;
   uint32_t                    offset       = pKerPrivArgs->pKerInitArgs.offset;
   size_t                      height       = pKerPrivArgs->height;
   int32_t                     strideIn     = pKerPrivArgs->strideInElements;

   uint8_t *pBlock            = pKerPrivArgs->bufPblock; // address to retrieve to store SE/SA params
   int32_t  numBlocksIdxCheck = (int32_t) pKerPrivArgs->numBlocksIdxCheck;

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   se1Params.DECDIM1_WIDTH    = (height % 2 == 0) ? (height * strideIn) : ((height - 1) * strideIn);

   typedef typename c7x::make_full_vector<uint16_t>::type vec;

   vec offsetVec = (vec) offset;
   vec countVec  = (vec) (count - 1);
   countVec -= offsetVec;

   uint16_t *pInLocal = (uint16_t *) pIn;

   vec flagVec0 = (vec) 0;
   vec flagVec1 = (vec) 0;
   vec oneVec   = (vec) 1;

   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pInLocal + strideIn, se1Params);

   for (int32_t i = 0; i < numBlocksIdxCheck; i++) {
      vec srcVec0 = c7x::strm_eng<0, vec>::get_adv();
      vec srcVec1 = c7x::strm_eng<1, vec>::get_adv();

      __vpred vpred0 = __cmp_gt_pred(srcVec0, countVec);
      __vpred vpred1 = __cmp_gt_pred(srcVec1, countVec);

      flagVec0 = __select(vpred0, oneVec, flagVec0);
      flagVec1 = __select(vpred1, oneVec, flagVec1);
   }
   __SE0_CLOSE();
   __SE1_CLOSE();
   flagVec0 += flagVec1;
   uint32_t flag = __horizontal_add(flagVec0);
   status        = (flag == 0) ? VXLIB_SUCCESS : VXLIB_ERR_INVALID_DIMENSION;
#if VXLIB_DEBUGPRINT
   printf("Exit VXLIB_tableLookup_checkSrcIdx_exec_ci \n");
#endif

   return (status);
}
template <> VXLIB_STATUS VXLIB_tableLookup_checkSrcIdx_exec_ci<int8_t>(VXLIB_kernelHandle handle, void *restrict pIn)
{
#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_tableLookup_checkSrcIdx_exec_ci\n");
#endif
   VXLIB_STATUS                status       = VXLIB_SUCCESS; // assign status to success by default
   VXLIB_tableLookup_PrivArgs *pKerPrivArgs = (VXLIB_tableLookup_PrivArgs *) handle;
   uint32_t                    count        = pKerPrivArgs->pKerInitArgs.count;
   uint32_t                    offset       = pKerPrivArgs->pKerInitArgs.offset;
   size_t                      height       = pKerPrivArgs->height;
   int32_t                     strideIn     = pKerPrivArgs->strideInElements;

   uint8_t *pBlock            = pKerPrivArgs->bufPblock; // address to retrieve to store SE/SA params
   int32_t  numBlocksIdxCheck = (int32_t) pKerPrivArgs->numBlocksIdxCheck;

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   se1Params.DECDIM1_WIDTH    = (height % 2 == 0) ? (height * strideIn) : ((height - 1) * strideIn);

   typedef typename c7x::make_full_vector<int8_t>::type  vec;
   typedef typename c7x::make_full_vector<uint8_t>::type uvec;

   uvec offsetVec = (uvec) offset;
   uvec countVec  = (uvec) (count - 1);

   vec zeroVec = (vec) (-(int8_t) offset);

   int8_t *pInLocal = (int8_t *) pIn;

   vec flagVec0 = (vec) 0;
   vec flagVec1 = (vec) 0;
   vec oneVec   = (vec) 1;

   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pInLocal + strideIn, se1Params);

   for (int32_t i = 0; i < numBlocksIdxCheck; i++) {
      vec srcVec0 = c7x::strm_eng<0, vec>::get_adv();
      vec srcVec1 = c7x::strm_eng<1, vec>::get_adv();

      __vpred vpred0 = __cmp_gt_pred(c7x::as_uchar_vec(srcVec0 + c7x::as_char_vec(offsetVec)), countVec);
      __vpred vpred1 = __cmp_gt_pred(c7x::as_uchar_vec(srcVec1 + c7x::as_char_vec(offsetVec)), countVec);

      __vpred vpred2 = __cmp_gt_pred(zeroVec, srcVec0);
      __vpred vpred3 = __cmp_gt_pred(zeroVec, srcVec1);

      vpred0 = __or(vpred0, vpred2);
      vpred1 = __or(vpred1, vpred3);

      flagVec0 = __select(vpred0, oneVec, flagVec0);
      flagVec1 = __select(vpred1, oneVec, flagVec1);
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   flagVec0 += flagVec1;
   uint32_t flag = __horizontal_add(flagVec0);
   status        = (flag == 0) ? VXLIB_SUCCESS : VXLIB_ERR_INVALID_DIMENSION;
#if VXLIB_DEBUGPRINT
   printf("Exit VXLIB_tableLookup_checkSrcIdx_exec_ci \n");
#endif

   return (status);
}

template <> VXLIB_STATUS VXLIB_tableLookup_checkSrcIdx_exec_ci<int16_t>(VXLIB_kernelHandle handle, void *restrict pIn)
{
#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_tableLookup_checkSrcIdx_exec_ci\n");
#endif
   VXLIB_STATUS                status       = VXLIB_SUCCESS; // assign status to success by default
   VXLIB_tableLookup_PrivArgs *pKerPrivArgs = (VXLIB_tableLookup_PrivArgs *) handle;
   uint32_t                    count        = pKerPrivArgs->pKerInitArgs.count;
   uint32_t                    offset       = pKerPrivArgs->pKerInitArgs.offset;
   size_t                      height       = pKerPrivArgs->height;
   int32_t                     strideIn     = pKerPrivArgs->strideInElements;

   uint8_t *pBlock            = pKerPrivArgs->bufPblock; // address to retrieve to store SE/SA params
   int32_t  numBlocksIdxCheck = (int32_t) pKerPrivArgs->numBlocksIdxCheck;

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   se1Params.DECDIM1_WIDTH    = (height % 2 == 0) ? (height * strideIn) : ((height - 1) * strideIn);

   typedef typename c7x::make_full_vector<int16_t>::type  vec;
   typedef typename c7x::make_full_vector<uint16_t>::type uvec;

   uvec offsetVec = (uvec) offset;
   uvec countVec  = (uvec) (count - 1);

   vec zeroVec = (vec) (-(int16_t) offset);

   int16_t *pInLocal = (int16_t *) pIn;

   vec flagVec0 = (vec) 0;
   vec flagVec1 = (vec) 0;
   vec oneVec   = (vec) 1;

   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pInLocal + strideIn, se1Params);

   for (int32_t i = 0; i < numBlocksIdxCheck; i++) {
      vec srcVec0 = c7x::strm_eng<0, vec>::get_adv();
      vec srcVec1 = c7x::strm_eng<1, vec>::get_adv();

      __vpred vpred0 = __cmp_gt_pred(c7x::as_ushort_vec(srcVec0 + c7x::as_short_vec(offsetVec)), countVec);
      __vpred vpred1 = __cmp_gt_pred(c7x::as_ushort_vec(srcVec1 + c7x::as_short_vec(offsetVec)), countVec);

      __vpred vpred2 = __cmp_gt_pred(zeroVec, srcVec0);
      __vpred vpred3 = __cmp_gt_pred(zeroVec, srcVec1);

      vpred0 = __or(vpred0, vpred2);
      vpred1 = __or(vpred1, vpred3);

      flagVec0 = __select(vpred0, oneVec, flagVec0);
      flagVec1 = __select(vpred1, oneVec, flagVec1);
   }
   __SE0_CLOSE();
   __SE1_CLOSE();
   flagVec0 += flagVec1;
   uint32_t flag = __horizontal_add(flagVec0);
   status        = (flag == 0) ? VXLIB_SUCCESS : VXLIB_ERR_INVALID_DIMENSION;
#if VXLIB_DEBUGPRINT
   printf("Exit VXLIB_tableLookup_checkSrcIdx_exec_ci \n");
#endif

   return (status);
}

template <typename dataType>
VXLIB_STATUS
VXLIB_tableLookup_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut, void *restrict lut)
{
#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_tableLookup_exec_ci\n");
#endif
   VXLIB_STATUS                status       = VXLIB_SUCCESS; // assign status to success by default

   status = VXLIB_tableLookup_checkSrcIdx_exec_ci<dataType>(handle, pIn);

   if (status == VXLIB_SUCCESS) {
      VXLIB_tableLookup_PrivArgs *pKerPrivArgs = (VXLIB_tableLookup_PrivArgs *) handle;
      uint32_t count = pKerPrivArgs->pKerInitArgs.count;
      if (count > VXLIB_COUNT_LIMIT) {
         status = VXLIB_tableLookup_generic_exec_ci<dataType>(handle, pIn, pOut, lut);
      }
      else {
         status = VXLIB_tableLookup_lut_exec_ci<dataType>(handle, pIn, pOut, lut);
      }
   }
   else {
      /* Nothing do here */
   }
#if VXLIB_DEBUGPRINT
   printf("Exit VXLIB_tableLookup_exec_ci \n");
#endif
   return (status);
}
template VXLIB_STATUS VXLIB_tableLookup_exec_ci<uint8_t>(VXLIB_kernelHandle handle,
                                                         void *restrict     pIn,
                                                         void *restrict     pOut,
                                                         void *restrict     lut);
template VXLIB_STATUS VXLIB_tableLookup_exec_ci<int8_t>(VXLIB_kernelHandle handle,
                                                        void *restrict     pIn,
                                                        void *restrict     pOut,
                                                        void *restrict     lut);
template VXLIB_STATUS VXLIB_tableLookup_exec_ci<uint16_t>(VXLIB_kernelHandle handle,
                                                          void *restrict     pIn,
                                                          void *restrict     pOut,
                                                          void *restrict     lut);
template VXLIB_STATUS VXLIB_tableLookup_exec_ci<int16_t>(VXLIB_kernelHandle handle,
                                                         void *restrict     pIn,
                                                         void *restrict     pOut,
                                                         void *restrict     lut);

template <uint32_t dTypeIn, uint32_t dTypeLut>
void VXLIB_idxCheck_perfEst(VXLIB_kernelHandle handle, size_t *archCycles);

template <> void VXLIB_idxCheck_perfEst<VXLIB_UINT8, VXLIB_UINT8>(VXLIB_kernelHandle handle, size_t *archCycles)
{
   VXLIB_tableLookup_PrivArgs *pKerPrivArgs = (VXLIB_tableLookup_PrivArgs *) handle;

   // obtain loop count for compute loop
   size_t numBlocksIdxCheck = pKerPrivArgs->numBlocksIdxCheck;
   *archCycles              = 1 + numBlocksIdxCheck * 1; // obtained from asm
}
template <> void VXLIB_idxCheck_perfEst<VXLIB_INT8, VXLIB_INT8>(VXLIB_kernelHandle handle, size_t *archCycles)
{
   VXLIB_tableLookup_PrivArgs *pKerPrivArgs = (VXLIB_tableLookup_PrivArgs *) handle;

   // obtain loop count for compute loop
   size_t numBlocksIdxCheck = pKerPrivArgs->numBlocksIdxCheck;
   *archCycles              = 3 + numBlocksIdxCheck * 2; // obtained from asm
}
template <> void VXLIB_idxCheck_perfEst<VXLIB_UINT16, VXLIB_UINT16>(VXLIB_kernelHandle handle, size_t *archCycles)
{
   VXLIB_tableLookup_PrivArgs *pKerPrivArgs = (VXLIB_tableLookup_PrivArgs *) handle;

   // obtain loop count for compute loop
   size_t numBlocksIdxCheck = pKerPrivArgs->numBlocksIdxCheck;
   *archCycles              = 1 + numBlocksIdxCheck * 1; // obtained from asm
}
template <> void VXLIB_idxCheck_perfEst<VXLIB_INT16, VXLIB_INT16>(VXLIB_kernelHandle handle, size_t *archCycles)
{
   VXLIB_tableLookup_PrivArgs *pKerPrivArgs = (VXLIB_tableLookup_PrivArgs *) handle;

   // obtain loop count for compute loop
   size_t numBlocksIdxCheck = pKerPrivArgs->numBlocksIdxCheck;
   *archCycles              = 3 + numBlocksIdxCheck * 2; // obtained from asm
}

template <uint32_t dTypeIn, uint32_t dTypeLut>
void VXLIB_lut_perfEst(VXLIB_kernelHandle handle, size_t *archCycles, size_t width);

template <>
void VXLIB_lut_perfEst<VXLIB_UINT8, VXLIB_UINT8>(VXLIB_kernelHandle handle, size_t *archCycles, size_t width)
{
   VXLIB_tableLookup_PrivArgs *pKerPrivArgs = (VXLIB_tableLookup_PrivArgs *) handle;
   size_t                      iterConst;
   size_t                      ii;
   size_t                      numBlocksLut = pKerPrivArgs->numBlocksLut;

   if (width < WIDTH_UNROLL_FACTOR) {
      ii        = 6;
      iterConst = 12;
   }
   else {
      ii        = 10;
      iterConst = 11;
   }

   // obtain loop count for compute loop
   *archCycles = iterConst + numBlocksLut * ii; // obtained from asm
}
template <> void VXLIB_lut_perfEst<VXLIB_INT8, VXLIB_INT8>(VXLIB_kernelHandle handle, size_t *archCycles, size_t width)
{
   VXLIB_tableLookup_PrivArgs *pKerPrivArgs = (VXLIB_tableLookup_PrivArgs *) handle;
   size_t                      iterConst;
   size_t                      ii;
   size_t                      numBlocksLut = pKerPrivArgs->numBlocksLut;

   if (width < WIDTH_UNROLL_FACTOR) {
      ii        = 6;
      iterConst = 12;
   }
   else {
      ii        = 10;
      iterConst = 11;
   }

   // obtain loop count for compute loop
   *archCycles = iterConst + numBlocksLut * ii; // obtained from asm
}

template <>
void VXLIB_lut_perfEst<VXLIB_UINT16, VXLIB_UINT16>(VXLIB_kernelHandle handle, size_t *archCycles, size_t width)
{
   VXLIB_tableLookup_PrivArgs *pKerPrivArgs = (VXLIB_tableLookup_PrivArgs *) handle;
   size_t                      iterConst;
   size_t                      ii;
   size_t                      numBlocksLut = pKerPrivArgs->numBlocksLut;

   if (width < WIDTH_UNROLL_FACTOR) {
      ii        = 8;
      iterConst = 7;
   }
   else {
      ii        = 12;
      iterConst = 8;
   }

   // obtain loop count for compute loop
   *archCycles = iterConst + numBlocksLut * ii; // obtained from asm
}
template <>
void VXLIB_lut_perfEst<VXLIB_INT16, VXLIB_INT16>(VXLIB_kernelHandle handle, size_t *archCycles, size_t width)
{
   VXLIB_tableLookup_PrivArgs *pKerPrivArgs = (VXLIB_tableLookup_PrivArgs *) handle;
   size_t                      iterConst;
   size_t                      ii;
   size_t                      numBlocksLut = pKerPrivArgs->numBlocksLut;

   if (width < WIDTH_UNROLL_FACTOR) {
      ii        = 8;
      iterConst = 12;
   }
   else {
      ii        = 12;
      iterConst = 8;
   }
   // obtain loop count for compute loop
   *archCycles = iterConst + numBlocksLut * ii; // obtained from asm
}
template <uint32_t dTypeIn, uint32_t dTypeLut> void VXLIB_generic_perfEst(VXLIB_kernelHandle handle, size_t *archCycles)
{
   VXLIB_tableLookup_PrivArgs *pKerPrivArgs = (VXLIB_tableLookup_PrivArgs *) handle;

   // obtain loop count for compute loop
   size_t numBlocksLut = pKerPrivArgs->numBlocksLut;
   *archCycles         = 10 + numBlocksLut * 8; // obtained from asm
}

template void VXLIB_generic_perfEst<VXLIB_UINT8, VXLIB_UINT8>(VXLIB_kernelHandle handle, size_t *archCycles);
template void VXLIB_generic_perfEst<VXLIB_INT8, VXLIB_INT8>(VXLIB_kernelHandle handle, size_t *archCycles);
template void VXLIB_generic_perfEst<VXLIB_UINT16, VXLIB_UINT16>(VXLIB_kernelHandle handle, size_t *archCycles);
template void VXLIB_generic_perfEst<VXLIB_INT16, VXLIB_INT16>(VXLIB_kernelHandle handle, size_t *archCycles);

void VXLIB_tableLookup_perfEst(VXLIB_kernelHandle                handle,
                               const VXLIB_bufParams2D_t        *bufParamsIn,
                               const VXLIB_bufParams2D_t        *bufParamsOut,
                               const VXLIB_bufParams1D_t        *bufParamsLut,
                               const VXLIB_tableLookup_InitArgs *pKerInitArgs,
                               size_t                           *archCycles,
                               size_t                           *estCycles)
{

   // typecast handle (void) to struct pointer type associated to kernel

   const uint32_t dTypeIn  = bufParamsIn->data_type;
   const uint32_t dTypeOut = bufParamsOut->data_type;
   const uint32_t dTypeLut = bufParamsLut->data_type;

   // obtain loop count for compute loop
   size_t overheadCnt = 110; // profiled code before entering compute loop

   size_t archSrcIdx;
   size_t archLutLogic;

   /* srcIdx Check cycle estimation */
   if (VXLIB_TABLELOOKUP_I8U_I8U_O8U) {
      VXLIB_idxCheck_perfEst<VXLIB_TABLELOOKUP_DTYPE_I8U_O8U>(handle, &archSrcIdx);
   }
   else if (VXLIB_TABLELOOKUP_I8S_I8S_O8S) {
      VXLIB_idxCheck_perfEst<VXLIB_TABLELOOKUP_DTYPE_I8S_O8S>(handle, &archSrcIdx);
   }
   else if (VXLIB_TABLELOOKUP_I16U_I16U_O16U) {
      VXLIB_idxCheck_perfEst<VXLIB_TABLELOOKUP_DTYPE_I16U_O16U>(handle, &archSrcIdx);
   }
   else {
      VXLIB_idxCheck_perfEst<VXLIB_TABLELOOKUP_DTYPE_I16S_O16S>(handle, &archSrcIdx);
   }

   /* LUT operation cycle estimation */
   VXLIB_tableLookup_PrivArgs *pKerPrivArgs = (VXLIB_tableLookup_PrivArgs *) handle;
   size_t         width    = pKerPrivArgs->width;
   uint32_t count = pKerPrivArgs->pKerInitArgs.count;
   if (count < VXLIB_COUNT_LIMIT) {

      if (VXLIB_TABLELOOKUP_I8U_I8U_O8U) {
         VXLIB_lut_perfEst<VXLIB_TABLELOOKUP_DTYPE_I8U_O8U>(handle, &archLutLogic, width);
      }
      else if (VXLIB_TABLELOOKUP_I8S_I8S_O8S) {
         VXLIB_lut_perfEst<VXLIB_TABLELOOKUP_DTYPE_I8S_O8S>(handle, &archLutLogic, width);
      }
      else if (VXLIB_TABLELOOKUP_I16U_I16U_O16U) {
         VXLIB_lut_perfEst<VXLIB_TABLELOOKUP_DTYPE_I16U_O16U>(handle, &archLutLogic, width);
      }
      else {
         VXLIB_lut_perfEst<VXLIB_TABLELOOKUP_DTYPE_I16S_O16S>(handle, &archLutLogic, width);
      }
   }
   else {
      if (VXLIB_TABLELOOKUP_I8U_I8U_O8U) {
         VXLIB_generic_perfEst<VXLIB_TABLELOOKUP_DTYPE_I8U_O8U>(handle, &archLutLogic);
      }
      else if (VXLIB_TABLELOOKUP_I8S_I8S_O8S) {
         VXLIB_generic_perfEst<VXLIB_TABLELOOKUP_DTYPE_I8S_O8S>(handle, &archLutLogic);
      }
      else if (VXLIB_TABLELOOKUP_I16U_I16U_O16U) {
         VXLIB_generic_perfEst<VXLIB_TABLELOOKUP_DTYPE_I16U_O16U>(handle, &archLutLogic);
      }
      else {
         VXLIB_generic_perfEst<VXLIB_TABLELOOKUP_DTYPE_I16S_O16S>(handle, &archLutLogic);
      }
   }
   *archCycles = archSrcIdx + archLutLogic; // obtained from asm
   *estCycles  = *archCycles + overheadCnt;
}
/* ======================================================================== */
/*  End of file:  VXLIB_tableLookup_ci.cpp                                  */
/* ======================================================================== */
