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

/*******************************************************************************
 *
 * INCLUDES
 *
 ******************************************************************************/

#include "VXLIB_utility.h"
#include "VXLIB_types.h"
#include <cstdio>

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_SE0SE1SA0_init                                                                                               */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method initializes SE0, SE1, and SA0 for common data-access pattern across VXLIB
template <uint32_t dTypeIn0, uint32_t dTypeIn1, uint32_t dTypeOut>
size_t VXLIB_SE0SE1SA0_init(__SE_TEMPLATE_v1*          se0Params,
                                          __SE_TEMPLATE_v1*          se1Params,
                                          __SA_TEMPLATE_v1*          sa0Params,
                                          const VXLIB_bufParams2D_t* bufParamsIn0,
                                          const VXLIB_bufParams2D_t* bufParamsIn1,
                                          const VXLIB_bufParams2D_t* bufParamsOut)
{

   // containers to hold SE parameters based on datatype
   __SE_ELETYPE SE0_ELETYPE;
   __SE_VECLEN  SE0_VECLEN;
   __SE_ELETYPE SE1_ELETYPE;
   __SE_VECLEN  SE1_VECLEN;
   __SA_VECLEN  SA_VECLEN;

   bool se0Promote = false;
   bool se1Promote = false;

   size_t numBlocks = 0;
   size_t elemCount = VXLIB_max_simd<dTypeOut>::value;

   // assign SE/SA params based on vector type
   if (VXLIB_I8U_I8U_O8U_CASE) {
      SE0_ELETYPE = c7x::se_eletype<c7x::uchar_vec>::value;
      SE0_VECLEN  = c7x::se_veclen<c7x::uchar_vec>::value;

      SE1_ELETYPE = c7x::se_eletype<c7x::uchar_vec>::value;
      SE1_VECLEN  = c7x::se_veclen<c7x::uchar_vec>::value;

      SA_VECLEN = c7x::sa_veclen<c7x::uchar_vec>::value;
   }
   else if (VXLIB_I8S_I8S_O8S_CASE) {
      SE0_ELETYPE = c7x::se_eletype<c7x::char_vec>::value;
      SE0_VECLEN  = c7x::se_veclen<c7x::char_vec>::value;

      SE1_ELETYPE = c7x::se_eletype<c7x::char_vec>::value;
      SE1_VECLEN  = c7x::se_veclen<c7x::char_vec>::value;

      SA_VECLEN = c7x::sa_veclen<c7x::char_vec>::value;
   }
   else if (VXLIB_I8S_I8S_O8U_CASE) {
      SE0_ELETYPE = c7x::se_eletype<c7x::char_vec>::value;
      SE0_VECLEN  = c7x::se_veclen<c7x::char_vec>::value;

      SE1_ELETYPE = c7x::se_eletype<c7x::char_vec>::value;
      SE1_VECLEN  = c7x::se_veclen<c7x::char_vec>::value;

      SA_VECLEN = c7x::sa_veclen<c7x::uchar_vec>::value;
   }
   else if (VXLIB_I8U_I8U_O16S_CASE) {
      SE0_ELETYPE = c7x::se_eletype<c7x::uchar_vec>::value;
      SE0_VECLEN  = c7x::se_veclen<c7x::ushort_vec>::value;
      se0Promote  = true;

      SE1_ELETYPE = c7x::se_eletype<c7x::uchar_vec>::value;
      SE1_VECLEN  = c7x::se_veclen<c7x::ushort_vec>::value;
      se1Promote  = true;

      SA_VECLEN = c7x::sa_veclen<c7x::short_vec>::value;
   }
   else if (VXLIB_I8U_I16S_O16S_CASE) {
      SE0_ELETYPE = c7x::se_eletype<c7x::uchar_vec>::value;
      SE0_VECLEN  = c7x::se_veclen<c7x::short_vec>::value;
      se0Promote  = true;

      SE1_ELETYPE = c7x::se_eletype<c7x::short_vec>::value;
      SE1_VECLEN  = c7x::se_veclen<c7x::short_vec>::value;

      SA_VECLEN = c7x::sa_veclen<c7x::short_vec>::value;
   }
   else if (VXLIB_I16S_I16S_O16S_CASE) {
      SE0_ELETYPE = c7x::se_eletype<c7x::short_vec>::value;
      SE0_VECLEN  = c7x::se_veclen<c7x::short_vec>::value;

      SE1_ELETYPE = c7x::se_eletype<c7x::short_vec>::value;
      SE1_VECLEN  = c7x::se_veclen<c7x::short_vec>::value;

      SA_VECLEN = c7x::sa_veclen<c7x::short_vec>::value;
   }
   else if (VXLIB_I16U_I16U_O16U_CASE) {
      SE0_ELETYPE = c7x::se_eletype<c7x::ushort_vec>::value;
      SE0_VECLEN  = c7x::se_veclen<c7x::ushort_vec>::value;

      SE1_ELETYPE = c7x::se_eletype<c7x::ushort_vec>::value;
      SE1_VECLEN  = c7x::se_veclen<c7x::ushort_vec>::value;

      SA_VECLEN = c7x::sa_veclen<c7x::ushort_vec>::value;
   }
   else if (VXLIB_I16S_I16S_O16U_CASE) {
      SE0_ELETYPE = c7x::se_eletype<c7x::short_vec>::value;
      SE0_VECLEN  = c7x::se_veclen<c7x::short_vec>::value;

      SE1_ELETYPE = c7x::se_eletype<c7x::short_vec>::value;
      SE1_VECLEN  = c7x::se_veclen<c7x::short_vec>::value;

      SA_VECLEN = c7x::sa_veclen<c7x::ushort_vec>::value;
   }
   else {
      /*Do Nothing*/
   }

   size_t width  = bufParamsIn0->dim_x;
   size_t height = bufParamsIn0->dim_y;

   size_t strideIn0Elems = bufParamsIn0->stride_y / VXLIB_sizeof(bufParamsIn0->data_type);
   size_t strideIn1Elems = bufParamsIn1->stride_y / VXLIB_sizeof(bufParamsIn1->data_type);
   size_t strideOutElems = bufParamsOut->stride_y / VXLIB_sizeof(bufParamsOut->data_type);

   bool isLinear = false;

   if ((width == strideIn0Elems) && (strideIn0Elems == strideIn1Elems) && (strideIn0Elems == strideOutElems)) {
      isLinear = true;
   }

   if (!isLinear) {

      /***************************************/
      /* Create SE params for reading input0 */
      /***************************************/

      se0Params->ELETYPE = SE0_ELETYPE;
      se0Params->VECLEN  = SE0_VECLEN;
      se0Params->DIMFMT  = __SE_DIMFMT_2D;
      if (se0Promote) {
         se0Params->PROMOTE = __SE_PROMOTE_2X_ZEROEXT;
      }
      else {
         se0Params->PROMOTE = __SE_PROMOTE_OFF;
      }

      se0Params->ICNT0 = width;
      se0Params->ICNT1 = height;
      se0Params->DIM1  = strideIn0Elems;

      /***************************************/
      /* Create SE params for reading input1 */
      /***************************************/

      se1Params->ELETYPE = SE1_ELETYPE;
      se1Params->VECLEN  = SE1_VECLEN;
      se1Params->DIMFMT  = __SE_DIMFMT_2D;
      if (se1Promote) {
         se1Params->PROMOTE = __SE_PROMOTE_2X_ZEROEXT;
      }
      else {
         se1Params->PROMOTE = __SE_PROMOTE_OFF;
      }

      se1Params->ICNT0 = width;
      se1Params->ICNT1 = height;
      se1Params->DIM1  = strideIn1Elems;

      /****************************************/
      /* Create SA0 params for writing output */
      /****************************************/

      sa0Params->ICNT0  = width;
      sa0Params->ICNT1  = height;
      sa0Params->DIM1   = strideOutElems;
      sa0Params->VECLEN = SA_VECLEN;
      sa0Params->DIMFMT = __SA_DIMFMT_2D;

      /****************************************/
      /* Calculate number of blocks           */
      /****************************************/

      size_t wBlocks = (width + (elemCount - 1)) / (elemCount);
      numBlocks      = height * wBlocks;
   }
   else {

      /***************************************/
      /* Create SE params for reading input0 */
      /***************************************/

      se0Params->ELETYPE = SE0_ELETYPE;
      se0Params->VECLEN  = SE0_VECLEN;
      se0Params->DIMFMT  = __SE_DIMFMT_1D;
      if (se0Promote) {
         se0Params->PROMOTE = __SE_PROMOTE_2X_ZEROEXT;
      }
      else {
         se0Params->PROMOTE = __SE_PROMOTE_OFF;
      }

      se0Params->ICNT0 = width * height;

      /***************************************/
      /* Create SE params for reading input1 */
      /***************************************/

      se1Params->ELETYPE = SE1_ELETYPE;
      se1Params->VECLEN  = SE1_VECLEN;
      se1Params->DIMFMT  = __SE_DIMFMT_1D;
      if (se1Promote) {
         se1Params->PROMOTE = __SE_PROMOTE_2X_ZEROEXT;
      }
      else {
         se1Params->PROMOTE = __SE_PROMOTE_OFF;
      }

      se1Params->ICNT0 = width * height;

      /****************************************/
      /* Create SA0 params for writing output */
      /****************************************/

      sa0Params->VECLEN = SA_VECLEN;
      sa0Params->DIMFMT = __SA_DIMFMT_1D;

      sa0Params->ICNT0 = width * height;

      /****************************************/
      /* Calculate number of blocks           */
      /****************************************/

      numBlocks = (width * height + (elemCount - 1)) / (elemCount);
   }

   return numBlocks;
}

template size_t VXLIB_SE0SE1SA0_init<VXLIB_I8U_I8U_O8U_DTYPE>(__SE_TEMPLATE_v1*          se0params,
                                                              __SE_TEMPLATE_v1*          se1params,
                                                              __SA_TEMPLATE_v1*          sa0params,
                                                              const VXLIB_bufParams2D_t* bufParamsIn0,
                                                              const VXLIB_bufParams2D_t* bufParamsIn1,
                                                              const VXLIB_bufParams2D_t* bufParamsOut);
template size_t VXLIB_SE0SE1SA0_init<VXLIB_I8S_I8S_O8S_DTYPE>(__SE_TEMPLATE_v1*          se0params,
                                                              __SE_TEMPLATE_v1*          se1params,
                                                              __SA_TEMPLATE_v1*          sa0params,
                                                              const VXLIB_bufParams2D_t* bufParamsIn0,
                                                              const VXLIB_bufParams2D_t* bufParamsIn1,
                                                              const VXLIB_bufParams2D_t* bufParamsOut);
template size_t VXLIB_SE0SE1SA0_init<VXLIB_I8S_I8S_O8U_DTYPE>(__SE_TEMPLATE_v1*          se0params,
                                                              __SE_TEMPLATE_v1*          se1params,
                                                              __SA_TEMPLATE_v1*          sa0params,
                                                              const VXLIB_bufParams2D_t* bufParamsIn0,
                                                              const VXLIB_bufParams2D_t* bufParamsIn1,
                                                              const VXLIB_bufParams2D_t* bufParamsOut);
template size_t VXLIB_SE0SE1SA0_init<VXLIB_I8U_I8U_O16S_DTYPE>(__SE_TEMPLATE_v1*          se0params,
                                                               __SE_TEMPLATE_v1*          se1params,
                                                               __SA_TEMPLATE_v1*          sa0params,
                                                               const VXLIB_bufParams2D_t* bufParamsIn0,
                                                               const VXLIB_bufParams2D_t* bufParamsIn1,
                                                               const VXLIB_bufParams2D_t* bufParamsOut);

template size_t VXLIB_SE0SE1SA0_init<VXLIB_I8U_I16S_O16S_DTYPE>(__SE_TEMPLATE_v1*          se0params,
                                                                __SE_TEMPLATE_v1*          se1params,
                                                                __SA_TEMPLATE_v1*          sa0params,
                                                                const VXLIB_bufParams2D_t* bufParamsIn0,
                                                                const VXLIB_bufParams2D_t* bufParamsIn1,
                                                                const VXLIB_bufParams2D_t* bufParamsOut);

template size_t VXLIB_SE0SE1SA0_init<VXLIB_I16S_I16S_O16S_DTYPE>(__SE_TEMPLATE_v1*          se0params,
                                                                 __SE_TEMPLATE_v1*          se1params,
                                                                 __SA_TEMPLATE_v1*          sa0params,
                                                                 const VXLIB_bufParams2D_t* bufParamsIn0,
                                                                 const VXLIB_bufParams2D_t* bufParamsIn1,
                                                                 const VXLIB_bufParams2D_t* bufParamsOut);
template size_t VXLIB_SE0SE1SA0_init<VXLIB_I16U_I16U_O16U_DTYPE>(__SE_TEMPLATE_v1*          se0params,
                                                                 __SE_TEMPLATE_v1*          se1params,
                                                                 __SA_TEMPLATE_v1*          sa0params,
                                                                 const VXLIB_bufParams2D_t* bufParamsIn0,
                                                                 const VXLIB_bufParams2D_t* bufParamsIn1,
                                                                 const VXLIB_bufParams2D_t* bufParamsOut);
template size_t VXLIB_SE0SE1SA0_init<VXLIB_I16S_I16S_O16U_DTYPE>(__SE_TEMPLATE_v1*          se0params,
                                                                 __SE_TEMPLATE_v1*          se1params,
                                                                 __SA_TEMPLATE_v1*          sa0params,
                                                                 const VXLIB_bufParams2D_t* bufParamsIn0,
                                                                 const VXLIB_bufParams2D_t* bufParamsIn1,
                                                                 const VXLIB_bufParams2D_t* bufParamsOut);

template <uint32_t dTypeIn0, uint32_t dTypeOut>
size_t VXLIB_SE0SA0_init(__SE_TEMPLATE_v1*          se0Params,
                                       __SA_TEMPLATE_v1*          sa0Params,
                                       const VXLIB_bufParams2D_t* bufParamsIn0,
                                       const VXLIB_bufParams2D_t* bufParamsOut)
{

   size_t numBlocks = 0;
   size_t elemCount = VXLIB_max_simd<dTypeOut>::value;
   // containers to hold SE parameters based on datatype
   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;
   __SA_VECLEN  SA_VECLEN;

   bool se0Promote = false;

   // assign SE/SA params based on vector type
   if (VXLIB_I8U_O8U_CASE) {
      SE_ELETYPE = c7x::se_eletype<c7x::uchar_vec>::value;
      SE_VECLEN  = c7x::se_veclen<c7x::uchar_vec>::value;

      SA_VECLEN = c7x::sa_veclen<c7x::uchar_vec>::value;
   }
   if (VXLIB_I8S_O8S_CASE) {
      SE_ELETYPE = c7x::se_eletype<c7x::char_vec>::value;
      SE_VECLEN  = c7x::se_veclen<c7x::char_vec>::value;

      SA_VECLEN = c7x::sa_veclen<c7x::char_vec>::value;
   }
   else if (VXLIB_I8U_O16S_CASE) {
      SE_ELETYPE = c7x::se_eletype<c7x::uchar_vec>::value;
      SE_VECLEN  = c7x::se_veclen<c7x::ushort_vec>::value;
      se0Promote = true;

      SA_VECLEN = c7x::sa_veclen<c7x::short_vec>::value;
   }
   else if (VXLIB_I8U_O16S_CASE) {
      SE_ELETYPE = c7x::se_eletype<c7x::uchar_vec>::value;
      SE_VECLEN  = c7x::se_veclen<c7x::short_vec>::value;
      se0Promote = true;

      SA_VECLEN = c7x::sa_veclen<c7x::short_vec>::value;
   }
   else if (VXLIB_I16S_O16S_CASE) {
      SE_ELETYPE = c7x::se_eletype<c7x::short_vec>::value;
      SE_VECLEN  = c7x::se_veclen<c7x::short_vec>::value;

      SA_VECLEN = c7x::sa_veclen<c7x::short_vec>::value;
   }
   else if (VXLIB_I16U_O16U_CASE) {
      SE_ELETYPE = c7x::se_eletype<c7x::ushort_vec>::value;
      SE_VECLEN  = c7x::se_veclen<c7x::ushort_vec>::value;

      SA_VECLEN = c7x::sa_veclen<c7x::ushort_vec>::value;
   }
   else {
      /*Do Nothing*/
   }

   size_t width  = bufParamsIn0->dim_x;
   size_t height = bufParamsIn0->dim_y;

   size_t strideInElems  = bufParamsIn0->stride_y / VXLIB_sizeof(bufParamsIn0->data_type);
   size_t strideOutElems = bufParamsOut->stride_y / VXLIB_sizeof(bufParamsOut->data_type);

   bool isLinear = false;

   if ((width == strideInElems) && (strideInElems == strideOutElems)) {
      isLinear = true;
   }

   if (!isLinear) {

      /***************************************/
      /* Create SE params for reading input0 */
      /***************************************/

      se0Params->ELETYPE = SE_ELETYPE;
      se0Params->VECLEN  = SE_VECLEN;
      se0Params->DIMFMT  = __SE_DIMFMT_2D;
      if (se0Promote) {
         se0Params->PROMOTE = __SE_PROMOTE_2X_ZEROEXT;
      }
      else {
         se0Params->PROMOTE = __SE_PROMOTE_OFF;
      }

      se0Params->ICNT0 = width;
      se0Params->ICNT1 = height;
      se0Params->DIM1  = strideInElems;

      /****************************************/
      /* Create SA0 params for writing output */
      /****************************************/

      sa0Params->ICNT0  = width;
      sa0Params->ICNT1  = height;
      sa0Params->DIM1   = strideOutElems;
      sa0Params->VECLEN = SA_VECLEN;
      sa0Params->DIMFMT = __SA_DIMFMT_2D;

      /****************************************/
      /* Calculate number of blocks           */
      /****************************************/

      size_t wBlocks = (width + (elemCount - 1)) / (elemCount);
      numBlocks      = height * wBlocks;
   }
   else {

      /***************************************/
      /* Create SE params for reading input0 */
      /***************************************/

      se0Params->ELETYPE = SE_ELETYPE;
      se0Params->VECLEN  = SE_VECLEN;
      se0Params->DIMFMT  = __SE_DIMFMT_1D;
      if (se0Promote) {
         se0Params->PROMOTE = __SE_PROMOTE_2X_ZEROEXT;
      }
      else {
         se0Params->PROMOTE = __SE_PROMOTE_OFF;
      }

      se0Params->ICNT0 = width * height;

      /****************************************/
      /* Create SA0 params for writing output */
      /****************************************/

      sa0Params->VECLEN = SA_VECLEN;
      sa0Params->DIMFMT = __SA_DIMFMT_1D;

      sa0Params->ICNT0 = width * height;

      /****************************************/
      /* Calculate number of blocks           */
      /****************************************/

      numBlocks = (width * height + (elemCount - 1)) / (elemCount);
   }

   return numBlocks;
}

template size_t VXLIB_SE0SA0_init<VXLIB_I8U_O8U_DTYPE>(__SE_TEMPLATE_v1*          se0params,
                                                       __SA_TEMPLATE_v1*          sa0params,
                                                       const VXLIB_bufParams2D_t* bufParamsIn0,
                                                       const VXLIB_bufParams2D_t* bufParamsOut);
template size_t VXLIB_SE0SA0_init<VXLIB_I8S_O8S_DTYPE>(__SE_TEMPLATE_v1*          se0params,
                                                       __SA_TEMPLATE_v1*          sa0params,
                                                       const VXLIB_bufParams2D_t* bufParamsIn0,
                                                       const VXLIB_bufParams2D_t* bufParamsOut);
template size_t VXLIB_SE0SA0_init<VXLIB_I8U_O16S_DTYPE>(__SE_TEMPLATE_v1*          se0params,
                                                        __SA_TEMPLATE_v1*          sa0params,
                                                        const VXLIB_bufParams2D_t* bufParamsIn0,
                                                        const VXLIB_bufParams2D_t* bufParamsOut);

template size_t VXLIB_SE0SA0_init<VXLIB_I16S_O16S_DTYPE>(__SE_TEMPLATE_v1*          se0params,
                                                         __SA_TEMPLATE_v1*          sa0params,
                                                         const VXLIB_bufParams2D_t* bufParamsIn0,
                                                         const VXLIB_bufParams2D_t* bufParamsOut);
template size_t VXLIB_SE0SA0_init<VXLIB_I16U_O16U_DTYPE>(__SE_TEMPLATE_v1*          se0params,
                                                         __SA_TEMPLATE_v1*          sa0params,
                                                         const VXLIB_bufParams2D_t* bufParamsIn0,
                                                         const VXLIB_bufParams2D_t* bufParamsOut);
