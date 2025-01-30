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

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_tableLookup_generic_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method initializes the kernel-specific parameters
// mainly, the streaming engine and streaming address generators
template <uint32_t dTypeIn, uint32_t dTypeOut>
VXLIB_STATUS VXLIB_tableLookup_generic_init_ci(VXLIB_kernelHandle                handle,
                                               const VXLIB_bufParams2D_t        *bufParamsIn,
                                               const VXLIB_bufParams2D_t        *bufParamsOut,
                                               const VXLIB_bufParams1D_t        *bufParamsLut,
                                               const VXLIB_tableLookup_InitArgs *pKerInitArgs)
{
#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_tableLookup_generic_init_ci\n");
#endif
   VXLIB_STATUS                status       = VXLIB_SUCCESS; // assign status to success by default
   VXLIB_tableLookup_PrivArgs *pKerPrivArgs = (VXLIB_tableLookup_PrivArgs *) handle;

   // structs to hold SE and SA parameters
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();
   size_t           width     = pKerPrivArgs->width;
   size_t           height    = pKerPrivArgs->height;
   // typecast handle (void) to struct pointer type associated to kernel

   uint8_t *pBlock = pKerPrivArgs->bufPblock; // address to retrieve to store SE/SA params

   //  set SE0, and SA0 params
   VXLIB_SE0SA0_init<dTypeIn, dTypeIn>(&se0Params, &sa0Params, bufParamsIn, bufParamsOut);
   sa0Params.VECLEN = __SA_VECLEN_1ELEM;
   se0Params.VECLEN = __SE_VECLEN_1ELEM;

   pKerPrivArgs->numBlocksLut = (height * width) / 8;
   /**************************/
   /* Store SE and SA params */
   /**************************/
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET) = sa0Params;
#if VXLIB_DEBUGPRINT
   printf("Exit VXLIB_tableLookup_generic_init_ci\n");
#endif
   return status;
}
template VXLIB_STATUS
VXLIB_tableLookup_generic_init_ci<VXLIB_TABLELOOKUP_DTYPE_I8U_O8U>(VXLIB_kernelHandle                handle,
                                                                   const VXLIB_bufParams2D_t *       bufParamsIn,
                                                                   const VXLIB_bufParams2D_t *       bufParamsOut,
                                                                   const VXLIB_bufParams1D_t *       bufParamsLut,
                                                                   const VXLIB_tableLookup_InitArgs *pKerInitArgs);
template VXLIB_STATUS
VXLIB_tableLookup_generic_init_ci<VXLIB_TABLELOOKUP_DTYPE_I8S_O8S>(VXLIB_kernelHandle                handle,
                                                                   const VXLIB_bufParams2D_t *       bufParamsIn,
                                                                   const VXLIB_bufParams2D_t *       bufParamsOut,
                                                                   const VXLIB_bufParams1D_t *       bufParamsLut,
                                                                   const VXLIB_tableLookup_InitArgs *pKerInitArgs);
template VXLIB_STATUS
VXLIB_tableLookup_generic_init_ci<VXLIB_TABLELOOKUP_DTYPE_I16U_O16U>(VXLIB_kernelHandle                handle,
                                                                     const VXLIB_bufParams2D_t *       bufParamsIn,
                                                                     const VXLIB_bufParams2D_t *       bufParamsOut,
                                                                     const VXLIB_bufParams1D_t *       bufParamsLut,
                                                                     const VXLIB_tableLookup_InitArgs *pKerInitArgs);

template VXLIB_STATUS
VXLIB_tableLookup_generic_init_ci<VXLIB_TABLELOOKUP_DTYPE_I16S_O16S>(VXLIB_kernelHandle                handle,
                                                                     const VXLIB_bufParams2D_t *       bufParamsIn,
                                                                     const VXLIB_bufParams2D_t *       bufParamsOut,
                                                                     const VXLIB_bufParams1D_t *       bufParamsLut,
                                                                     const VXLIB_tableLookup_InitArgs *pKerInitArgs);

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_tableLookup_generic_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_tableLookup_generic_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/
template <typename dataType>
VXLIB_STATUS VXLIB_tableLookup_generic_set_ci(VXLIB_kernelHandle handle, void *restrict pLut)
{
#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_tableLookup_generic_set_ci\n");
#endif
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

#if VXLIB_DEBUGPRINT
   printf("Exit VXLIB_tableLookup_generic_set_ci\n");
#endif
   return (status);
}
template VXLIB_STATUS VXLIB_tableLookup_generic_set_ci<uint8_t>(VXLIB_kernelHandle handle, void *restrict pLut);
template VXLIB_STATUS VXLIB_tableLookup_generic_set_ci<int8_t>(VXLIB_kernelHandle handle, void *restrict pLut);
template VXLIB_STATUS VXLIB_tableLookup_generic_set_ci<uint16_t>(VXLIB_kernelHandle handle, void *restrict pLut);
template VXLIB_STATUS VXLIB_tableLookup_generic_set_ci<int16_t>(VXLIB_kernelHandle handle, void *restrict pLut);

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_tableLookup_generic_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/
template <typename vecType, typename dataType> static inline dataType getVal(vecType inpVec, dataType idx)
{
   return inpVec.s[0];
}

template <typename dataType>
VXLIB_STATUS VXLIB_tableLookup_generic_exec_ci(VXLIB_kernelHandle handle,
                                               void *restrict     pIn,
                                               void *restrict     pOut,
                                               void *restrict     pLut)
{
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_tableLookup_generic_exec_ci\n");
#endif

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_tableLookup_PrivArgs *pKerPrivArgs = (VXLIB_tableLookup_PrivArgs *) handle;

   // obtain image parameters
   size_t   width  = pKerPrivArgs->width;
   size_t   height = pKerPrivArgs->height;
   dataType offset = pKerPrivArgs->pKerInitArgs.offset;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   /******************************* INDEX FINDING ******************************/
   uint8_t         *pBlock       = pKerPrivArgs->bufPblock;
   int32_t          numBlocksLut = pKerPrivArgs->numBlocksLut;
   __SE_TEMPLATE_v1 se0Params    = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa0Params    = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);

   dataType *restrict pInLocal  = (dataType *) pIn;
   dataType *restrict pOutLocal = (dataType *) pOut;
   dataType *restrict lutLocal  = (dataType *) pLut;
#if VXLIB_DEBUGPRINT
   printf("In VXLIB_tableLookup_generic_exec_ci, width: %d, height: %d\n", width, height);
#endif

   __SE0_OPEN(pInLocal, se0Params);
   __SA0_OPEN(sa0Params);

   int32_t remElem = (height * width) - (numBlocksLut * 8);
   for (int32_t j = 0; j < numBlocksLut; j++) {
      vec       srcVec   = c7x::strm_eng<0, vec>::get_adv();
      uint64_t  srcIndex = getVal(srcVec, 0) + offset;
      dataType  lutValue = lutLocal[srcIndex];
      dataType *addr     = c7x::strm_agen<0, dataType>::get_adv(pOutLocal); // generate address
      *addr              = lutValue;

      srcVec   = c7x::strm_eng<0, vec>::get_adv();
      srcIndex = getVal(srcVec, 0) + offset;
      lutValue = lutLocal[srcIndex];
      addr     = c7x::strm_agen<0, dataType>::get_adv(pOutLocal); // generate address
      *addr    = lutValue;

      srcVec   = c7x::strm_eng<0, vec>::get_adv();
      srcIndex = getVal(srcVec, 0) + offset;
      lutValue = lutLocal[srcIndex];
      addr     = c7x::strm_agen<0, dataType>::get_adv(pOutLocal); // generate address
      *addr    = lutValue;

      srcVec   = c7x::strm_eng<0, vec>::get_adv();
      srcIndex = getVal(srcVec, 0) + offset;
      lutValue = lutLocal[srcIndex];
      addr     = c7x::strm_agen<0, dataType>::get_adv(pOutLocal); // generate address
      *addr    = lutValue;

      srcVec   = c7x::strm_eng<0, vec>::get_adv();
      srcIndex = getVal(srcVec, 0) + offset;
      lutValue = lutLocal[srcIndex];
      addr     = c7x::strm_agen<0, dataType>::get_adv(pOutLocal); // generate address
      *addr    = lutValue;

      srcVec   = c7x::strm_eng<0, vec>::get_adv();
      srcIndex = getVal(srcVec, 0) + offset;
      lutValue = lutLocal[srcIndex];
      addr     = c7x::strm_agen<0, dataType>::get_adv(pOutLocal); // generate address
      *addr    = lutValue;

      srcVec   = c7x::strm_eng<0, vec>::get_adv();
      srcIndex = getVal(srcVec, 0) + offset;
      lutValue = lutLocal[srcIndex];
      addr     = c7x::strm_agen<0, dataType>::get_adv(pOutLocal); // generate address
      *addr    = lutValue;

      srcVec   = c7x::strm_eng<0, vec>::get_adv();
      srcIndex = getVal(srcVec, 0) + offset;
      lutValue = lutLocal[srcIndex];
      addr     = c7x::strm_agen<0, dataType>::get_adv(pOutLocal); // generate address
      *addr    = lutValue;
   }
   for (int32_t j = 0; j < remElem; j++) {

      vec       srcVec   = c7x::strm_eng<0, vec>::get_adv();
      uint64_t  srcIndex = getVal(srcVec, 0) + offset;
      dataType  lutValue = lutLocal[srcIndex];
      dataType *addr     = c7x::strm_agen<0, dataType>::get_adv(pOutLocal); // generate address
      *addr              = lutValue;
   }

   __SE0_CLOSE();
   __SA0_CLOSE();
#if VXLIB_DEBUGPRINT
   printf("Exit VXLIB_tableLookup_generic_exec_ci \n");
#endif
   return (status);
}
template VXLIB_STATUS VXLIB_tableLookup_generic_exec_ci<uint8_t>(VXLIB_kernelHandle handle,
                                                                 void *restrict     pIn,
                                                                 void *restrict     pOut,
                                                                 void *restrict     pLut);
template VXLIB_STATUS VXLIB_tableLookup_generic_exec_ci<int8_t>(VXLIB_kernelHandle handle,
                                                                void *restrict     pIn,
                                                                void *restrict     pOut,
                                                                void *restrict     pLut);
template VXLIB_STATUS VXLIB_tableLookup_generic_exec_ci<uint16_t>(VXLIB_kernelHandle handle,
                                                                  void *restrict     pIn,
                                                                  void *restrict     pOut,
                                                                  void *restrict     pLut);
template VXLIB_STATUS VXLIB_tableLookup_generic_exec_ci<int16_t>(VXLIB_kernelHandle handle,
                                                                 void *restrict     pIn,
                                                                 void *restrict     pOut,
                                                                 void *restrict     pLut);

/* ======================================================================== */
/*  End of file:  VXLIB_tableLookup_generic_ci.cpp                          */
/* ======================================================================== */
