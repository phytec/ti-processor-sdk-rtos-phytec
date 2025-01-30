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
#include "VXLIB_types.h"
#include "VXLIB_tableLookup_priv.h"
#include "VXLIB_utility.h"
#include "c7x_scalable.h"
#include <type_traits>

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_tableLookup_lut_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method initializes the kernel-specific parameters
// mainly, the streaming engine and streaming address generators
template <uint32_t dTypeInt> uint32_t setSEICNT1(uint32_t nTiles);

template <> inline uint32_t setSEICNT1<VXLIB_UINT8>(uint32_t nTiles) { return 2 * nTiles; }
template <> inline uint32_t setSEICNT1<VXLIB_INT8>(uint32_t nTiles) { return 2 * nTiles; }
template <> inline uint32_t setSEICNT1<VXLIB_UINT16>(uint32_t nTiles) { return nTiles; }
template <> inline uint32_t setSEICNT1<VXLIB_INT16>(uint32_t nTiles) { return nTiles; }

template <uint32_t dTypeInt> uint32_t getWidthBlockLut(uint32_t nTiles);

template <> inline uint32_t getWidthBlockLut<VXLIB_UINT8>(uint32_t nTiles) { return VXLIB_ceilingDiv(nTiles, 2); }
template <> inline uint32_t getWidthBlockLut<VXLIB_INT8>(uint32_t nTiles) { return VXLIB_ceilingDiv(nTiles, 2); }
template <> inline uint32_t getWidthBlockLut<VXLIB_UINT16>(uint32_t nTiles) { return VXLIB_ceilingDiv(nTiles, 4); }
template <> inline uint32_t getWidthBlockLut<VXLIB_INT16>(uint32_t nTiles) { return VXLIB_ceilingDiv(nTiles, 4); }

template <uint32_t dTypeInt> void setSEPromote(__SE_TEMPLATE_v1 &seParam);

template <> inline void setSEPromote<VXLIB_UINT8>(__SE_TEMPLATE_v1 &seParam)
{
   seParam.PROMOTE = __SE_PROMOTE_4X_ZEROEXT;
}
template <> inline void setSEPromote<VXLIB_INT8>(__SE_TEMPLATE_v1 &seParam)
{
   seParam.PROMOTE = __SE_PROMOTE_4X_SIGNEXT;
}
template <> inline void setSEPromote<VXLIB_UINT16>(__SE_TEMPLATE_v1 &seParam)
{
   seParam.PROMOTE = __SE_PROMOTE_2X_ZEROEXT;
}
template <> inline void setSEPromote<VXLIB_INT16>(__SE_TEMPLATE_v1 &seParam)
{
   seParam.PROMOTE = __SE_PROMOTE_2X_SIGNEXT;
}

template <uint32_t dTypeIn, uint32_t dTypeOut>
VXLIB_STATUS VXLIB_tableLookup_lut_init_ci(VXLIB_kernelHandle                handle,
                                           const VXLIB_bufParams2D_t        *bufParamsIn,
                                           const VXLIB_bufParams2D_t        *bufParamsOut,
                                           const VXLIB_bufParams1D_t        *bufParamsLut,
                                           const VXLIB_tableLookup_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   // structs to hold SE and SA parameters

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_tableLookup_PrivArgs *pKerPrivArgs = (VXLIB_tableLookup_PrivArgs *) handle;

   // obtain image size and compute number of blocks to process
   size_t  width           = pKerPrivArgs->width;
   size_t  height          = pKerPrivArgs->height;
   size_t  elemCount       = VXLIB_max_simd<dTypeIn>::value;
   size_t  promotElemCount = VXLIB_max_simd<VXLIB_UINT32>::value;
   int32_t strideIn        = pKerPrivArgs->strideInElements;
   int32_t strideOut       = pKerPrivArgs->strideOutElements;

   typedef typename VXLIB_vec_type<dTypeIn>::type vec;

   uint8_t *pBlock = pKerPrivArgs->bufPblock; // address to retrieve to store SE/SA params

   //  set SE0, and SA0 params
   int32_t nTiles     = (width + elemCount - 1) / elemCount;
   int32_t widthBlock = getWidthBlockLut<dTypeIn>(nTiles);

   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<c7x::int_vec>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<vec>::value;
   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec>::value;

   sa0Params.ICNT0         = elemCount;
   sa0Params.DIM1          = elemCount;
   sa0Params.ICNT1         = nTiles;
   sa0Params.ICNT2         = height;
   sa0Params.DIM2          = strideOut;
   sa0Params.DIMFMT        = __SA_DIMFMT_3D;
   sa0Params.VECLEN        = SA_VECLEN;
   sa0Params.DECDIM1       = __SA_DECDIM_DIM1;
   sa0Params.DECDIM1SD     = __SA_DECDIMSD_DIM0;
   sa0Params.DECDIM1_WIDTH = width;

   se0Params.ICNT0         = promotElemCount;
   se0Params.DIM1          = promotElemCount * 2;
   se0Params.ICNT1         = setSEICNT1<dTypeIn>(nTiles);
   se0Params.ICNT2         = height;
   se0Params.DIM2          = strideIn;
   se0Params.DIMFMT        = __SE_DIMFMT_3D;
   se0Params.VECLEN        = SE_VECLEN;
   se0Params.DECDIM1       = __SE_DECDIM_DIM1;
   se0Params.DECDIM1SD     = __SE_DECDIMSD_DIM0;
   se0Params.DECDIM1_WIDTH = width;
   setSEPromote<dTypeIn>(se0Params);
   se0Params.ELETYPE = SE_ELETYPE;

   pKerPrivArgs->numBlocksLut = widthBlock * height;

   /**************************/
   /* Store SE and SA params */
   /**************************/
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET) = sa0Params;
#if VXLIB_DEBUGPRINT
   printf("Exit VXLIB_tableLookup_lut_init_ci\n");
#endif
   return status;
}
template VXLIB_STATUS
VXLIB_tableLookup_lut_init_ci<VXLIB_TABLELOOKUP_DTYPE_I8U_O8U>(VXLIB_kernelHandle                handle,
                                                               const VXLIB_bufParams2D_t        *bufParamsIn,
                                                               const VXLIB_bufParams2D_t        *bufParamsOut,
                                                               const VXLIB_bufParams1D_t        *bufParamsLut,
                                                               const VXLIB_tableLookup_InitArgs *pKerInitArgs);
template VXLIB_STATUS
VXLIB_tableLookup_lut_init_ci<VXLIB_TABLELOOKUP_DTYPE_I8S_O8S>(VXLIB_kernelHandle                handle,
                                                               const VXLIB_bufParams2D_t        *bufParamsIn,
                                                               const VXLIB_bufParams2D_t        *bufParamsOut,
                                                               const VXLIB_bufParams1D_t        *bufParamsLut,
                                                               const VXLIB_tableLookup_InitArgs *pKerInitArgs);
template VXLIB_STATUS
VXLIB_tableLookup_lut_init_ci<VXLIB_TABLELOOKUP_DTYPE_I16U_O16U>(VXLIB_kernelHandle                handle,
                                                                 const VXLIB_bufParams2D_t        *bufParamsIn,
                                                                 const VXLIB_bufParams2D_t        *bufParamsOut,
                                                                 const VXLIB_bufParams1D_t        *bufParamsLut,
                                                                 const VXLIB_tableLookup_InitArgs *pKerInitArgs);
template VXLIB_STATUS
VXLIB_tableLookup_lut_init_ci<VXLIB_TABLELOOKUP_DTYPE_I16S_O16S>(VXLIB_kernelHandle                handle,
                                                                 const VXLIB_bufParams2D_t        *bufParamsIn,
                                                                 const VXLIB_bufParams2D_t        *bufParamsOut,
                                                                 const VXLIB_bufParams1D_t        *bufParamsLut,
                                                                 const VXLIB_tableLookup_InitArgs *pKerInitArgs);

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_tableLookup_lut_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_tableLookup_lut_set_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/
template <typename dataType> void setLTCRFlag(__sLTCRFlags_t &set0flags);

template <> inline void setLTCRFlag<uint8_t>(__sLTCRFlags_t &set0flags)
{
   set0flags.SIGNUNSIGN = __LUT_UNSIGNED;     // data elements are signed
   set0flags.ELETYPE    = __LUT_ELETYPE_8BIT; // 8-bit elements
   set0flags.PROMOTION  = __LUT_PROMOTE_4X;
}
template <> inline void setLTCRFlag<int8_t>(__sLTCRFlags_t &set0flags)
{
   set0flags.SIGNUNSIGN = __LUT_SIGNED;       // data elements are signed
   set0flags.ELETYPE    = __LUT_ELETYPE_8BIT; // 8-bit elements
   set0flags.PROMOTION  = __LUT_PROMOTE_4X;
}
template <> inline void setLTCRFlag<uint16_t>(__sLTCRFlags_t &set0flags)
{
   set0flags.SIGNUNSIGN = __LUT_UNSIGNED;      // data elements are signed
   set0flags.ELETYPE    = __LUT_ELETYPE_16BIT; // 16-bit elements
   set0flags.PROMOTION  = __LUT_PROMOTE_2X;
}
template <> inline void setLTCRFlag<int16_t>(__sLTCRFlags_t &set0flags)
{
   set0flags.SIGNUNSIGN = __LUT_SIGNED;        // data elements are signed
   set0flags.ELETYPE    = __LUT_ELETYPE_16BIT; // 16-bit elements
   set0flags.PROMOTION  = __LUT_PROMOTE_2X;
}

template <typename dataType> void lookUpTable_init(void *lutLocal, uint32_t tableSize);

template <> inline void lookUpTable_init<uint8_t>(void *lutLocal, uint32_t tableSize)
{
   uint64_t      *pData = (uint64_t *) lutLocal;
   c7x::ulong_vec vData;
   uint32_t       rowOffset    = 0;
   uint32_t       rowIncrement = 8;
   uint32_t       loopCount    = ((tableSize + rowIncrement - 1) / rowIncrement);
   for (uint32_t j = 0; j < loopCount; j++) {
      // Read the quantum into a vector of ulong8
      vData = (c7x::ulong_vec)(pData[j]);

      // Update the quanta in all N ways
      __lookup_init(__LUT_SET0, __as_uchar64(vData), rowOffset);

      // Increment the row_offset by 8 as we are initializing 8 8-bit elements at a time
      rowOffset += rowIncrement;
   }
}
template <> inline void lookUpTable_init<int8_t>(void *lutLocal, uint32_t tableSize)
{
   uint64_t      *pData = (uint64_t *) lutLocal;
   c7x::ulong_vec vData;
   uint32_t       rowOffset    = 0;
   uint32_t       rowIncrement = 8;
   uint32_t       loopCount    = ((tableSize + rowIncrement - 1) / rowIncrement);
   for (uint32_t j = 0; j < loopCount; j++) {
      // Read the quantum into a vector of ulong8
      vData = (c7x::ulong_vec)(pData[j]);

      // Update the quanta in all N ways
      __lookup_init(__LUT_SET0, __as_uchar64(vData), rowOffset);

      // Increment the row_offset by 8 as we are initializing 8 8-bit elements at a time
      rowOffset += rowIncrement;
   }
}
template <> inline void lookUpTable_init<uint16_t>(void *lutLocal, uint32_t tableSize)
{
   uint64_t      *pData = (uint64_t *) lutLocal;
   c7x::ulong_vec vData;
   uint32_t       rowOffset    = 0;
   uint32_t       rowIncrement = 4;
   uint32_t       loopCount    = ((tableSize + rowIncrement - 1) / rowIncrement);
   for (uint32_t j = 0; j < loopCount; j++) {
      // Read the quantum into a vector of ulong8
      vData = (c7x::ulong_vec)(pData[j]);

      // Update the quanta in all N ways
      __lookup_init(__LUT_SET0, __as_ushort32(vData), rowOffset);

      // Increment the row_offset by 8 as we are initializing 8 8-bit elements at a time
      rowOffset += rowIncrement;
   }
}
template <> inline void lookUpTable_init<int16_t>(void *lutLocal, uint32_t tableSize)
{
   uint64_t      *pData = (uint64_t *) lutLocal;
   c7x::ulong_vec vData;
   uint32_t       rowOffset    = 0;
   uint32_t       rowIncrement = 4;
   uint32_t       loopCount    = ((tableSize + rowIncrement - 1) / rowIncrement);
   for (uint32_t j = 0; j < loopCount; j++) {
      // Read the quantum into a vector of ulong8
      vData = (c7x::ulong_vec)(pData[j]);

      // Update the quanta in all N ways
      __lookup_init(__LUT_SET0, __as_ushort32(vData), rowOffset);

      // Increment the row_offset by 8 as we are initializing 8 8-bit elements at a time
      rowOffset += rowIncrement;
   }
}
template <typename dataType> VXLIB_STATUS VXLIB_tableLookup_lut_set_ci(VXLIB_kernelHandle handle, void *restrict pLut)
{
#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_tableLookup_lut_set_ci\n");
#endif
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default
   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_tableLookup_PrivArgs *pKerPrivArgs = (VXLIB_tableLookup_PrivArgs *) handle;
   uint32_t                    count        = pKerPrivArgs->pKerInitArgs.count;
   dataType *restrict lutLocal              = (dataType *) pLut;
   const uint32_t tableSize                 = count;
   // Upper Bits LUT - Contains bits 63-32 for all lookup table values
   __sLTCRFlags_t set0flags;

   set0flags.INTERPOLATION = __LUT_INTERP_OFF;
   set0flags.SATURATION    = __LUT_SAT_OFF;
   set0flags.NUMTBLS       = __LUT_NUM_TBL_16;
   set0flags.TBLSIZE       = __LUT_TBL_SIZE_16KBYTES; // Total table size
   set0flags.WEIGHTSIZE    = __LUT_WSIZE_8BIT;

   setLTCRFlag<dataType>(set0flags);

   __LUT_SET_LTCR(__LUT_SET0, __lut_set_param(&set0flags));
   // Start the table at offset 0 from the beginning of L1D-SRAM
   __LUT_SET_LTBR(__LUT_SET0, 0x0000);
   // Enable set 0
   __LUT_SET_LTER(__LUT_ENABLE_0);

   lookUpTable_init<dataType>(lutLocal, tableSize);
#if VXLIB_DEBUGPRINT
   printf("Exit VXLIB_tableLookup_lut_set_ci\n");
#endif
   return (status);
}
template VXLIB_STATUS VXLIB_tableLookup_lut_set_ci<uint8_t>(VXLIB_kernelHandle handle, void *restrict pLut);
template VXLIB_STATUS VXLIB_tableLookup_lut_set_ci<int8_t>(VXLIB_kernelHandle handle, void *restrict pLut);
template VXLIB_STATUS VXLIB_tableLookup_lut_set_ci<uint16_t>(VXLIB_kernelHandle handle, void *restrict pLut);
template VXLIB_STATUS VXLIB_tableLookup_lut_set_ci<int16_t>(VXLIB_kernelHandle handle, void *restrict pLut);
/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_tableLookup_lut_set_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_tableLookup_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/
template <typename dataType>
VXLIB_STATUS VXLIB_tableLookup_lut_unrolled_exec_ci(VXLIB_kernelHandle handle,
                                                    int32_t            width,
                                                    int32_t            height,
                                                    uint16_t           offset,
                                                    int32_t            numBlocksLut,
                                                    dataType          *pOutLocal);
template <>
inline VXLIB_STATUS VXLIB_tableLookup_lut_unrolled_exec_ci<uint8_t>(VXLIB_kernelHandle handle,
                                                                    int32_t            width,
                                                                    int32_t            height,
                                                                    uint16_t           offset,
                                                                    int32_t            numBlocksLut,
                                                                    uint8_t           *pOutLocal)
{
#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_tableLookup_lut_unrolled_exec_ci\n");
#endif
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   typedef typename c7x::make_full_vector<uint8_t>::type vec;

   c7x::uint_vec offsetVec = (c7x::uint_vec) offset;

   /******************************* LUT OPERATION ******************************/
#if VXLIB_DEBUGPRINT
   printf("In VXLIB_tableLookup_lut_unrolled_exec_ci, width: %d, height: %d\n", width, height);
#endif

   for (int32_t j = 0; j < numBlocksLut; j++) {

      c7x::uint_vec srcVec = c7x::strm_eng<0, c7x::uint_vec>::get_adv();
      srcVec += offsetVec;
      c7x::uint_vec vecOut1 = (__lookup_read_uint(__LUT_SET0, srcVec));

      srcVec = c7x::strm_eng<1, c7x::uint_vec>::get_adv();
      srcVec += offsetVec;
      c7x::uint_vec vecOut2 = (__lookup_read_uint(__LUT_SET0, srcVec));

      c7x::ushort_vec out01 = __vhpackl_vvv(c7x::as_ushort_vec(vecOut2), c7x::as_ushort_vec(vecOut1));

      srcVec = c7x::strm_eng<0, c7x::uint_vec>::get_adv();
      srcVec += offsetVec;
      c7x::uint_vec vecOut3 = (__lookup_read_uint(__LUT_SET0, srcVec));

      srcVec = c7x::strm_eng<1, c7x::uint_vec>::get_adv();
      srcVec += offsetVec;
      c7x::uint_vec vecOut4 = (__lookup_read_uint(__LUT_SET0, srcVec));

      c7x::ushort_vec out02 = __vhpackl_vvv(c7x::as_ushort_vec(vecOut4), c7x::as_ushort_vec(vecOut3));

      srcVec = c7x::strm_eng<0, c7x::uint_vec>::get_adv();
      srcVec += offsetVec;
      vecOut1 = (__lookup_read_uint(__LUT_SET0, srcVec));

      srcVec = c7x::strm_eng<1, c7x::uint_vec>::get_adv();
      srcVec += offsetVec;
      vecOut2 = (__lookup_read_uint(__LUT_SET0, srcVec));

      c7x::ushort_vec out11 = __vhpackl_vvv(c7x::as_ushort_vec(vecOut2), c7x::as_ushort_vec(vecOut1));

      srcVec = c7x::strm_eng<0, c7x::uint_vec>::get_adv();
      srcVec += offsetVec;
      vecOut3 = (__lookup_read_uint(__LUT_SET0, srcVec));

      srcVec = c7x::strm_eng<1, c7x::uint_vec>::get_adv();
      srcVec += offsetVec;
      vecOut4 = (__lookup_read_uint(__LUT_SET0, srcVec));

      c7x::ushort_vec out12 = __vhpackl_vvv(c7x::as_ushort_vec(vecOut4), c7x::as_ushort_vec(vecOut3));

      __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
      vec    *addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
      __vstore_pred_packl_2src(tmp, addr, out01, out02);

      tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
      addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
      __vstore_pred_packl_2src(tmp, addr, out11, out12);
   }
#if VXLIB_DEBUGPRINT
   printf("Exit VXLIB_tableLookup_lut_unrolled_exec_ci\n");
#endif
   return (status);
}

template <>
inline VXLIB_STATUS VXLIB_tableLookup_lut_unrolled_exec_ci<int8_t>(VXLIB_kernelHandle handle,
                                                                   int32_t            width,
                                                                   int32_t            height,
                                                                   uint16_t           offset,
                                                                   int32_t            numBlocksLut,
                                                                   int8_t            *pOutLocal)
{
#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_tableLookup_lut_unrolled_exec_ci\n");
#endif
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   typedef typename c7x::make_full_vector<int8_t>::type vec;

   c7x::int_vec offsetVec = (c7x::int_vec) offset;

   /******************************* LUT OPERATION ******************************/
#if VXLIB_DEBUGPRINT
   printf("In VXLIB_tableLookup_lut_unrolled_exec_ci, width: %d, height: %d\n", width, height);
#endif

   for (int32_t j = 0; j < numBlocksLut; j++) {

      c7x::int_vec srcVec = c7x::strm_eng<0, c7x::int_vec>::get_adv();
      srcVec += offsetVec;
      c7x::int_vec vecOut1 = (__lookup_read_int(__LUT_SET0, c7x::as_uint_vec(srcVec)));

      srcVec = c7x::strm_eng<1, c7x::int_vec>::get_adv();
      srcVec += offsetVec;
      c7x::int_vec vecOut2 = (__lookup_read_int(__LUT_SET0, c7x::as_uint_vec(srcVec)));

      c7x::short_vec out01 = __vhpackl_vvv(c7x::as_short_vec(vecOut2), c7x::as_short_vec(vecOut1));

      srcVec = c7x::strm_eng<0, c7x::int_vec>::get_adv();
      srcVec += offsetVec;
      c7x::int_vec vecOut3 = (__lookup_read_int(__LUT_SET0, c7x::as_uint_vec(srcVec)));

      srcVec = c7x::strm_eng<1, c7x::int_vec>::get_adv();
      srcVec += offsetVec;
      c7x::int_vec vecOut4 = (__lookup_read_int(__LUT_SET0, c7x::as_uint_vec(srcVec)));

      c7x::short_vec out02 = __vhpackl_vvv(c7x::as_short_vec(vecOut4), c7x::as_short_vec(vecOut3));

      srcVec = c7x::strm_eng<0, c7x::int_vec>::get_adv();
      srcVec += offsetVec;
      vecOut1 = (__lookup_read_int(__LUT_SET0, c7x::as_uint_vec(srcVec)));

      srcVec = c7x::strm_eng<1, c7x::int_vec>::get_adv();
      srcVec += offsetVec;
      vecOut2 = (__lookup_read_int(__LUT_SET0, c7x::as_uint_vec(srcVec)));

      c7x::short_vec out11 = __vhpackl_vvv(c7x::as_short_vec(vecOut2), c7x::as_short_vec(vecOut1));

      srcVec = c7x::strm_eng<0, c7x::int_vec>::get_adv();
      srcVec += offsetVec;
      vecOut3 = (__lookup_read_int(__LUT_SET0, c7x::as_uint_vec(srcVec)));

      srcVec = c7x::strm_eng<1, c7x::int_vec>::get_adv();
      srcVec += offsetVec;
      vecOut4 = (__lookup_read_int(__LUT_SET0, c7x::as_uint_vec(srcVec)));

      c7x::short_vec out12 = __vhpackl_vvv(c7x::as_short_vec(vecOut4), c7x::as_short_vec(vecOut3));

      __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
      vec    *addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
      __vstore_pred_packl_2src(tmp, addr, out01, out02);

      tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
      addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
      __vstore_pred_packl_2src(tmp, addr, out11, out12);
   }
#if VXLIB_DEBUGPRINT
   printf("Exit VXLIB_tableLookup_lut_unrolled_exec_ci\n");
#endif
   return (status);
}

template <>
inline VXLIB_STATUS VXLIB_tableLookup_lut_unrolled_exec_ci<uint16_t>(VXLIB_kernelHandle handle,
                                                                     int32_t            width,
                                                                     int32_t            height,
                                                                     uint16_t           offset,
                                                                     int32_t            numBlocksLut,
                                                                     uint16_t          *pOutLocal)
{

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_tableLookup_lut_unrolled_exec_ci\n");
#endif
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   typedef typename c7x::make_full_vector<uint16_t>::type vec;

   c7x::uint_vec offsetVec = (c7x::uint_vec) offset;

   /******************************* LUT OPERATION ******************************/
#if VXLIB_DEBUGPRINT
   printf("In VXLIB_tableLookup_lut_unrolled_exec_ci, width: %d, height: %d\n", width, height);
#endif
   for (int32_t j = 0; j < numBlocksLut; j++) {

      c7x::uint_vec srcVec = c7x::strm_eng<0, c7x::uint_vec>::get_adv();
      srcVec += offsetVec;

      c7x::uint_vec vecOut1 = (__lookup_read_uint(__LUT_SET0, srcVec));
      srcVec                = c7x::strm_eng<1, c7x::uint_vec>::get_adv();
      srcVec += offsetVec;
      c7x::uint_vec vecOut2 = (__lookup_read_uint(__LUT_SET0, srcVec));

      __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
      vec    *addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
      __vstore_pred_packl_2src(tmp, addr, vecOut1, vecOut2);

      srcVec = c7x::strm_eng<0, c7x::uint_vec>::get_adv();
      srcVec += offsetVec;
      c7x::uint_vec vecOut3 = (__lookup_read_uint(__LUT_SET0, srcVec));

      srcVec = c7x::strm_eng<1, c7x::uint_vec>::get_adv();
      srcVec += offsetVec;
      c7x::uint_vec vecOut4 = (__lookup_read_uint(__LUT_SET0, srcVec));

      tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
      addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
      __vstore_pred_packl_2src(tmp, addr, vecOut3, vecOut4);

      srcVec = c7x::strm_eng<0, c7x::uint_vec>::get_adv();
      srcVec += offsetVec;
      vecOut1 = (__lookup_read_uint(__LUT_SET0, srcVec));

      srcVec = c7x::strm_eng<1, c7x::uint_vec>::get_adv();
      srcVec += offsetVec;
      vecOut2 = (__lookup_read_uint(__LUT_SET0, srcVec));

      tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
      addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
      __vstore_pred_packl_2src(tmp, addr, vecOut1, vecOut2);

      srcVec = c7x::strm_eng<0, c7x::uint_vec>::get_adv();
      srcVec += offsetVec;
      vecOut3 = (__lookup_read_uint(__LUT_SET0, srcVec));

      srcVec = c7x::strm_eng<1, c7x::uint_vec>::get_adv();
      srcVec += offsetVec;
      vecOut4 = (__lookup_read_uint(__LUT_SET0, srcVec));

      tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
      addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
      __vstore_pred_packl_2src(tmp, addr, vecOut3, vecOut4);
   }

#if VXLIB_DEBUGPRINT
   printf("Exit VXLIB_tableLookup_lut_unrolled_exec_ci\n");
#endif
   return (status);
}

template <>
inline VXLIB_STATUS VXLIB_tableLookup_lut_unrolled_exec_ci<int16_t>(VXLIB_kernelHandle handle,
                                                                    int32_t            width,
                                                                    int32_t            height,
                                                                    uint16_t           offset,
                                                                    int32_t            numBlocksLut,
                                                                    int16_t           *pOutLocal)
{

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_tableLookup_lut_unrolled_exec_ci\n");
#endif
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   typedef typename c7x::make_full_vector<int16_t>::type vec;

   c7x::int_vec offsetVec = (c7x::int_vec) offset;

   /******************************* LUT OPERATION ******************************/
#if VXLIB_DEBUGPRINT
   printf("In VXLIB_tableLookup_lut_unrolled_exec_ci, width: %d, height: %d\n", width, height);
#endif
   for (int32_t j = 0; j < numBlocksLut; j++) {
      c7x::int_vec srcVec = c7x::strm_eng<0, c7x::int_vec>::get_adv();
      srcVec += offsetVec;
      c7x::int_vec vecOut1 = (__lookup_read_int(__LUT_SET0, c7x::as_uint_vec(srcVec)));
      srcVec               = c7x::strm_eng<1, c7x::int_vec>::get_adv();
      srcVec += offsetVec;
      c7x::int_vec vecOut2 = (__lookup_read_int(__LUT_SET0, c7x::as_uint_vec(srcVec)));

      __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
      vec    *addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
      __vstore_pred_packl_2src(tmp, addr, vecOut1, vecOut2);

      srcVec = c7x::strm_eng<0, c7x::int_vec>::get_adv();
      srcVec += offsetVec;
      c7x::int_vec vecOut3 = (__lookup_read_int(__LUT_SET0, c7x::as_uint_vec(srcVec)));

      srcVec = c7x::strm_eng<1, c7x::int_vec>::get_adv();
      srcVec += offsetVec;
      c7x::int_vec vecOut4 = (__lookup_read_int(__LUT_SET0, c7x::as_uint_vec(srcVec)));

      tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
      addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
      __vstore_pred_packl_2src(tmp, addr, vecOut3, vecOut4);

      srcVec = c7x::strm_eng<0, c7x::int_vec>::get_adv();
      srcVec += offsetVec;
      vecOut1 = (__lookup_read_int(__LUT_SET0, c7x::as_uint_vec(srcVec)));

      srcVec = c7x::strm_eng<1, c7x::int_vec>::get_adv();
      srcVec += offsetVec;
      vecOut2 = (__lookup_read_int(__LUT_SET0, c7x::as_uint_vec(srcVec)));

      tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
      addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
      __vstore_pred_packl_2src(tmp, addr, vecOut1, vecOut2);

      srcVec = c7x::strm_eng<0, c7x::int_vec>::get_adv();
      srcVec += offsetVec;
      vecOut3 = (__lookup_read_int(__LUT_SET0, c7x::as_uint_vec(srcVec)));

      srcVec = c7x::strm_eng<1, c7x::int_vec>::get_adv();
      srcVec += offsetVec;
      vecOut4 = (__lookup_read_int(__LUT_SET0, c7x::as_uint_vec(srcVec)));

      tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
      addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
      __vstore_pred_packl_2src(tmp, addr, vecOut3, vecOut4);
   }

#if VXLIB_DEBUGPRINT
   printf("Exit VXLIB_tableLookup_lut_unrolled_exec_ci\n");
#endif
   return (status);
}

template <typename dataType>
VXLIB_STATUS VXLIB_tableLookup_lut_generic_exec_ci(VXLIB_kernelHandle handle,
                                                   int32_t            width,
                                                   int32_t            height,
                                                   uint16_t           offset,
                                                   int32_t            numBlocksLut,
                                                   dataType          *pOutLocal);
template <>
inline VXLIB_STATUS VXLIB_tableLookup_lut_generic_exec_ci<uint8_t>(VXLIB_kernelHandle handle,
                                                                   int32_t            width,
                                                                   int32_t            height,
                                                                   uint16_t           offset,
                                                                   int32_t            numBlocksLut,
                                                                   uint8_t           *pOutLocal)
{
#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_tableLookup_lut_generic_exec_ci\n");
#endif
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   typedef typename c7x::make_full_vector<uint8_t>::type vec;

   c7x::uint_vec offsetVec = (c7x::uint_vec) offset;

   /******************************* LUT OPERATION ******************************/
#if VXLIB_DEBUGPRINT
   printf("In VXLIB_tableLookup_lut_generic_exec_ci, width: %d, height: %d\n", width, height);
#endif

   for (int32_t j = 0; j < numBlocksLut; j++) {

      c7x::uint_vec srcVec = c7x::strm_eng<0, c7x::uint_vec>::get_adv();
      srcVec += offsetVec;
      c7x::uint_vec vecOut1 = (__lookup_read_uint(__LUT_SET0, srcVec));
      c7x::uint_vec vecOut2 = (c7x::uint_vec) 0;

      c7x::ushort_vec out01 = __vhpackl_vvv(c7x::as_ushort_vec(vecOut2), c7x::as_ushort_vec(vecOut1));

      srcVec = c7x::strm_eng<0, c7x::uint_vec>::get_adv();
      srcVec += offsetVec;
      c7x::uint_vec vecOut3 = (__lookup_read_uint(__LUT_SET0, srcVec));
      c7x::uint_vec vecOut4 = (c7x::uint_vec) 0;

      c7x::ushort_vec out02 = __vhpackl_vvv(c7x::as_ushort_vec(vecOut4), c7x::as_ushort_vec(vecOut3));

      srcVec = c7x::strm_eng<0, c7x::uint_vec>::get_adv();
      srcVec += offsetVec;
      vecOut1 = (__lookup_read_uint(__LUT_SET0, srcVec));
      vecOut2 = (c7x::uint_vec) 0;

      c7x::ushort_vec out11 = __vhpackl_vvv(c7x::as_ushort_vec(vecOut2), c7x::as_ushort_vec(vecOut1));

      srcVec = c7x::strm_eng<0, c7x::uint_vec>::get_adv();
      srcVec += offsetVec;
      vecOut3 = (__lookup_read_uint(__LUT_SET0, srcVec));
      vecOut4 = (c7x::uint_vec) 0;

      c7x::ushort_vec out12 = __vhpackl_vvv(c7x::as_ushort_vec(vecOut4), c7x::as_ushort_vec(vecOut3));

      __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
      vec    *addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
      __vstore_pred_packl_2src(tmp, addr, out01, out02);

      tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
      addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
      __vstore_pred_packl_2src(tmp, addr, out11, out12);
   }
#if VXLIB_DEBUGPRINT
   printf("Exit VXLIB_tableLookup_lut_generic_exec_ci\n");
#endif
   return (status);
}

template <>
inline VXLIB_STATUS VXLIB_tableLookup_lut_generic_exec_ci<int8_t>(VXLIB_kernelHandle handle,
                                                                  int32_t            width,
                                                                  int32_t            height,
                                                                  uint16_t           offset,
                                                                  int32_t            numBlocksLut,
                                                                  int8_t            *pOutLocal)
{
#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_tableLookup_lut_generic_exec_ci\n");
#endif
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   typedef typename c7x::make_full_vector<int8_t>::type vec;

   c7x::int_vec offsetVec = (c7x::int_vec) offset;

   /******************************* LUT OPERATION ******************************/
#if VXLIB_DEBUGPRINT
   printf("In VXLIB_tableLookup_lut_generic_exec_ci, width: %d, height: %d\n", width, height);
#endif

   for (int32_t j = 0; j < numBlocksLut; j++) {

      c7x::int_vec srcVec = c7x::strm_eng<0, c7x::int_vec>::get_adv();
      srcVec += offsetVec;
      c7x::int_vec vecOut1 = (__lookup_read_int(__LUT_SET0, c7x::as_uint_vec(srcVec)));
      c7x::int_vec vecOut2 = (c7x::int_vec) 0;

      c7x::short_vec out01 = __vhpackl_vvv(c7x::as_short_vec(vecOut2), c7x::as_short_vec(vecOut1));

      srcVec = c7x::strm_eng<0, c7x::int_vec>::get_adv();
      srcVec += offsetVec;
      c7x::int_vec vecOut3 = (__lookup_read_int(__LUT_SET0, c7x::as_uint_vec(srcVec)));
      c7x::int_vec vecOut4 = (c7x::int_vec) 0;

      c7x::short_vec out02 = __vhpackl_vvv(c7x::as_short_vec(vecOut4), c7x::as_short_vec(vecOut3));

      srcVec = c7x::strm_eng<0, c7x::int_vec>::get_adv();
      srcVec += offsetVec;
      vecOut1 = (__lookup_read_int(__LUT_SET0, c7x::as_uint_vec(srcVec)));
      vecOut2 = (c7x::int_vec) 0;

      c7x::short_vec out11 = __vhpackl_vvv(c7x::as_short_vec(vecOut2), c7x::as_short_vec(vecOut1));

      srcVec = c7x::strm_eng<0, c7x::int_vec>::get_adv();
      srcVec += offsetVec;
      vecOut3 = (__lookup_read_int(__LUT_SET0, c7x::as_uint_vec(srcVec)));
      vecOut4 = (c7x::int_vec) 0;

      c7x::short_vec out12 = __vhpackl_vvv(c7x::as_short_vec(vecOut4), c7x::as_short_vec(vecOut3));

      __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
      vec    *addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
      __vstore_pred_packl_2src(tmp, addr, out01, out02);

      tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
      addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
      __vstore_pred_packl_2src(tmp, addr, out11, out12);
   }
#if VXLIB_DEBUGPRINT
   printf("Exit VXLIB_tableLookup_lut_generic_exec_ci\n");
#endif
   return (status);
}

template <>
inline VXLIB_STATUS VXLIB_tableLookup_lut_generic_exec_ci<uint16_t>(VXLIB_kernelHandle handle,
                                                                    int32_t            width,
                                                                    int32_t            height,
                                                                    uint16_t           offset,
                                                                    int32_t            numBlocksLut,
                                                                    uint16_t          *pOutLocal)
{

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_tableLookup_lut_generic_exec_ci\n");
#endif
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   typedef typename c7x::make_full_vector<uint16_t>::type vec;

   c7x::uint_vec offsetVec = (c7x::uint_vec) offset;

   /******************************* LUT OPERATION ******************************/
#if VXLIB_DEBUGPRINT
   printf("In VXLIB_tableLookup_lut_generic_exec_ci, width: %d, height: %d\n", width, height);
#endif
   for (int32_t j = 0; j < numBlocksLut; j++) {

      c7x::uint_vec srcVec = c7x::strm_eng<0, c7x::uint_vec>::get_adv();
      srcVec += offsetVec;

      c7x::uint_vec vecOut1 = (__lookup_read_uint(__LUT_SET0, srcVec));
      c7x::uint_vec vecOut2 = (c7x::uint_vec) 0;

      __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
      vec    *addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
      __vstore_pred_packl_2src(tmp, addr, vecOut1, vecOut2);

      srcVec = c7x::strm_eng<0, c7x::uint_vec>::get_adv();
      srcVec += offsetVec;
      c7x::uint_vec vecOut3 = (__lookup_read_uint(__LUT_SET0, srcVec));

      c7x::uint_vec vecOut4 = (c7x::uint_vec) 0;

      tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
      addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
      __vstore_pred_packl_2src(tmp, addr, vecOut3, vecOut4);

      srcVec = c7x::strm_eng<0, c7x::uint_vec>::get_adv();
      srcVec += offsetVec;
      vecOut1 = (__lookup_read_uint(__LUT_SET0, srcVec));

      vecOut2 = (c7x::uint_vec) 0;

      tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
      addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
      __vstore_pred_packl_2src(tmp, addr, vecOut1, vecOut2);

      srcVec = c7x::strm_eng<0, c7x::uint_vec>::get_adv();
      srcVec += offsetVec;
      vecOut3 = (__lookup_read_uint(__LUT_SET0, srcVec));

      vecOut4 = (c7x::uint_vec) 0;

      tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
      addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
      __vstore_pred_packl_2src(tmp, addr, vecOut3, vecOut4);
   }

#if VXLIB_DEBUGPRINT
   printf("Exit VXLIB_tableLookup_lut_generic_exec_ci\n");
#endif
   return (status);
}

template <>
inline VXLIB_STATUS VXLIB_tableLookup_lut_generic_exec_ci<int16_t>(VXLIB_kernelHandle handle,
                                                                   int32_t            width,
                                                                   int32_t            height,
                                                                   uint16_t           offset,
                                                                   int32_t            numBlocksLut,
                                                                   int16_t           *pOutLocal)
{

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_tableLookup_lut_generic_exec_ci\n");
#endif
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   typedef typename c7x::make_full_vector<int16_t>::type vec;

   c7x::int_vec offsetVec = (c7x::int_vec) offset;

   /******************************* LUT OPERATION ******************************/
#if VXLIB_DEBUGPRINT
   printf("In VXLIB_tableLookup_lut_generic_exec_ci, width: %d, height: %d\n", width, height);
#endif
   for (int32_t j = 0; j < numBlocksLut; j++) {
      c7x::int_vec srcVec = c7x::strm_eng<0, c7x::int_vec>::get_adv();
      srcVec += offsetVec;
      c7x::int_vec vecOut1 = (__lookup_read_int(__LUT_SET0, c7x::as_uint_vec(srcVec)));
      c7x::int_vec vecOut2 = (c7x::int_vec) 0;

      __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
      vec    *addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
      __vstore_pred_packl_2src(tmp, addr, vecOut1, vecOut2);

      srcVec = c7x::strm_eng<0, c7x::int_vec>::get_adv();
      srcVec += offsetVec;
      c7x::int_vec vecOut3 = (__lookup_read_int(__LUT_SET0, c7x::as_uint_vec(srcVec)));
      c7x::int_vec vecOut4 = (c7x::int_vec) 0;

      tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
      addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
      __vstore_pred_packl_2src(tmp, addr, vecOut3, vecOut4);

      srcVec = c7x::strm_eng<0, c7x::int_vec>::get_adv();
      srcVec += offsetVec;
      vecOut1 = (__lookup_read_int(__LUT_SET0, c7x::as_uint_vec(srcVec)));
      vecOut2 = (c7x::int_vec) 0;

      tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
      addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
      __vstore_pred_packl_2src(tmp, addr, vecOut1, vecOut2);

      srcVec = c7x::strm_eng<0, c7x::int_vec>::get_adv();
      srcVec += offsetVec;
      vecOut3 = (__lookup_read_int(__LUT_SET0, c7x::as_uint_vec(srcVec)));
      vecOut4 = (c7x::int_vec) 0;

      tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
      addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
      __vstore_pred_packl_2src(tmp, addr, vecOut3, vecOut4);
   }

#if VXLIB_DEBUGPRINT
   printf("Exit VXLIB_tableLookup_lut_generic_exec_ci\n");
#endif
   return (status);
}

template <typename dataType>
VXLIB_STATUS
VXLIB_tableLookup_lut_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut, void *restrict pLut)
{
#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_tableLookup_lut_exec_ci\n");
#endif
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default
   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_tableLookup_PrivArgs *pKerPrivArgs = (VXLIB_tableLookup_PrivArgs *) handle;

   // obtain image parameters
   size_t   width        = pKerPrivArgs->width;
   size_t   height       = pKerPrivArgs->height;
   uint16_t offset       = pKerPrivArgs->pKerInitArgs.offset;
   int32_t  numBlocksLut = pKerPrivArgs->numBlocksLut;
   uint8_t *pBlock       = pKerPrivArgs->bufPblock;

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);

   size_t intElemCount = c7x::element_count_of<c7x::int_vec>::value;

   dataType *restrict pInLocal  = (dataType *) pIn;
   dataType *restrict pOutLocal = (dataType *) pOut;

#if VXLIB_DEBUGPRINT
   printf("In VXLIB_tableLookup_lut_exec_ci, width: %d, height: %d\n", width, height);
#endif
   if (width > WIDTH_UNROLL_FACTOR) {

      se1Params.DECDIM1_WIDTH = width - intElemCount;
      __SE0_OPEN(pInLocal, se0Params);
      __SE1_OPEN(pInLocal + intElemCount, se1Params);
      __SA0_OPEN(sa0Params);

      status = VXLIB_tableLookup_lut_unrolled_exec_ci<dataType>(handle, width, height, offset, numBlocksLut, pOutLocal);

      __SE0_CLOSE();
      __SE1_CLOSE();
      __SA0_CLOSE();
   }
   else {
      __SE0_OPEN(pInLocal, se0Params);
      __SA0_OPEN(sa0Params);

      status = VXLIB_tableLookup_lut_generic_exec_ci<dataType>(handle, width, height, offset, numBlocksLut, pOutLocal);

      __SE0_CLOSE();
      __SA0_CLOSE();
   }
#if VXLIB_DEBUGPRINT
   printf("Exit VXLIB_tableLookup_lut_exec_ci\n");
#endif
   return (status);
}
template VXLIB_STATUS VXLIB_tableLookup_lut_exec_ci<uint8_t>(VXLIB_kernelHandle handle,
                                                             void *restrict pIn,
                                                             void *restrict pOut,
                                                             void *restrict pLut);
template VXLIB_STATUS VXLIB_tableLookup_lut_exec_ci<int8_t>(VXLIB_kernelHandle handle,
                                                            void *restrict pIn,
                                                            void *restrict pOut,
                                                            void *restrict pLut);
template VXLIB_STATUS VXLIB_tableLookup_lut_exec_ci<uint16_t>(VXLIB_kernelHandle handle,
                                                              void *restrict pIn,
                                                              void *restrict pOut,
                                                              void *restrict pLut);
template VXLIB_STATUS VXLIB_tableLookup_lut_exec_ci<int16_t>(VXLIB_kernelHandle handle,
                                                             void *restrict pIn,
                                                             void *restrict pOut,
                                                             void *restrict pLut);

/* ======================================================================== */
/*  End of file:  VXLIB_tableLookup_lut_ci.cpp                              */
/* ======================================================================== */
