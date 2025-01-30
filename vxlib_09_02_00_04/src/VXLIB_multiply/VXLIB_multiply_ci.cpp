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

#include "VXLIB_multiply_priv.h"

/**********************************************************************************************************************/
/*                                                                                                                    */
/* DEFINES                                                                                                            */
/*                                                                                                                    */
/**********************************************************************************************************************/

#define VXLIB_MULTIPLY_UNROLL_FACTOR (4)
#define SE_PARAM_BASE (0x0000)
#define SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE1_PARAM_OFFSET (SE0_PARAM_OFFSET + VXLIB_SE_PARAM_SIZE)
#define SA0_PARAM_OFFSET (SE1_PARAM_OFFSET + VXLIB_SE_PARAM_SIZE)
#define DECIMAL_BITS 8

// clang-format off
#if (__C7X_VEC_SIZE_BITS__ == 256)
const c7x::uchar_vec vpermMaskSat = c7x::uchar_vec(0,1,4,5,2,3,6,7,8,9,12,13,10,11,14,15,16,17,20,21,18,19,22,23,24,25,28,29,26,27,30,31);

const c7x::uchar_vec vpermMaskWrap = c7x::uchar_vec(0,1,2,3,16,17,18,19,4,5,6,7,20,21,22,23,8,9,10,11,24,25,26,27,12,13,14,15,28,29,30,31);

#elif (__C7X_VEC_SIZE_BITS__ == 512)
const c7x::uchar_vec vpermMaskSat = c7x::uchar_vec(0,1,4,5,2,3,6,7,8,9,12,13,10,11,14,15,16,17,20,21,18,19,22,23,24,25,28,29,26,27,30,31,32,33,36,37,34,35,38,39,40,41,44,45,42,43,46,47,48,49,52,53,50,51,54,55,56,57,60,61,58,59,62,63);

const c7x::uchar_vec vpermMaskWrap = c7x::uchar_vec(0,1,2,3,32,33,34,35,4,5,6,7,36,37,38,39,8,9,10,11,40,41,42,43,12,13,14,15,44,45,46,47,
                              16,17,18,19,48,49,50,51,20,21,22,23,52,53,54,55,24,25,26,27,56,57,58,59,28,29,30,31,60,61,62,63);

#else
#error invalid target
#endif

// clang-format on

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_multiply_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method initializes the kernel-specific parameters
// mainly, the streaming engine and streaming address generators

template <uint32_t dTypeIn0, uint32_t dTypeIn1, uint32_t dTypeOut>
VXLIB_STATUS VXLIB_multiply_init_ci(VXLIB_kernelHandle             handle,
                                    const VXLIB_bufParams2D_t *    bufParamsIn0,
                                    const VXLIB_bufParams2D_t *    bufParamsIn1,
                                    const VXLIB_bufParams2D_t *    bufParamsOut,
                                    const VXLIB_multiply_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_multiply_PrivArgs *pKerPrivArgs = (VXLIB_multiply_PrivArgs *) handle;

   uint8_t *pBlock = pKerPrivArgs->bufPblock; // address to retrieve to store SE/SA params

   // structs to hold SE and SA parameters
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se1Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   // set SE0, SE1, and SA0 params
   size_t numBlocks = VXLIB_SE0SE1SA0_init<dTypeIn0, dTypeIn1, dTypeOut>(&se0Params, &se1Params, &sa0Params,
                                                                         bufParamsIn0, bufParamsIn1, bufParamsOut);

   if (dTypeOut == VXLIB_UINT8) {
      pKerPrivArgs->numBlocks = VXLIB_ceilingDiv(numBlocks, VXLIB_MULTIPLY_UNROLL_FACTOR);
   }
   else {
      pKerPrivArgs->numBlocks = numBlocks;
   }
   /**************************/
   /* Store SE and SA params */
   /**************************/

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET) = se0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET) = se1Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET) = sa0Params;

   /* -------------------------------------------------------------------------------
           CHECK for multiply mode - (1/2^n) OR (Integer) OR (Generic)
    --------------------------------------------------------------------------------- */
   float scale = pKerPrivArgs->pKerInitArgs.scale;

   /* Calculate if "scale" is 1/2^n */
   float    recipScaleF   = (1 / scale) * (float) (1U << DECIMAL_BITS);
   uint32_t recipScaleU   = (uint32_t)(recipScaleF);
   uint32_t msb           = (recipScaleU == 0) ? 0 : (31U - __leftmost_bit_detect_one(recipScaleU));
   pKerPrivArgs->shiftVal = msb - DECIMAL_BITS;

   /* Calculate if "scale" is integer */
   float    convertScaleF = scale * (float) (1U << DECIMAL_BITS);
   uint32_t convertScaleU = (uint32_t) convertScaleF;
   uint32_t integerMask   = (1U << DECIMAL_BITS) - 1U;

   if ((msb > DECIMAL_BITS) && (recipScaleU == (1u << msb))) {
      pKerPrivArgs->multiplyMode = VXLIB_MULTIPLY_DIV_2_N;
   }
   else if (((convertScaleU & integerMask) == 0) && (convertScaleU > 0)) {
      pKerPrivArgs->multiplyMode = VXLIB_MULTIPLY_INTEGER;
   }
   else {
      pKerPrivArgs->multiplyMode = VXLIB_MULTIPLY_GENERIC;
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_multiply_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS
VXLIB_multiply_init_ci<VXLIB_MULTIPLY_DTYPE_I8U_I8U_O8U>(VXLIB_kernelHandle             handle,
                                                         const VXLIB_bufParams2D_t *    bufParamsIn0,
                                                         const VXLIB_bufParams2D_t *    bufParamsIn1,
                                                         const VXLIB_bufParams2D_t *    bufParamsOut,
                                                         const VXLIB_multiply_InitArgs *pKerInitArgs);

template VXLIB_STATUS
VXLIB_multiply_init_ci<VXLIB_MULTIPLY_DTYPE_I8U_I8U_O16S>(VXLIB_kernelHandle             handle,
                                                          const VXLIB_bufParams2D_t *    bufParamsIn0,
                                                          const VXLIB_bufParams2D_t *    bufParamsIn1,
                                                          const VXLIB_bufParams2D_t *    bufParamsOut,
                                                          const VXLIB_multiply_InitArgs *pKerInitArgs);

template VXLIB_STATUS
VXLIB_multiply_init_ci<VXLIB_MULTIPLY_DTYPE_I8U_I16S_O16S>(VXLIB_kernelHandle             handle,
                                                           const VXLIB_bufParams2D_t *    bufParamsIn0,
                                                           const VXLIB_bufParams2D_t *    bufParamsIn1,
                                                           const VXLIB_bufParams2D_t *    bufParamsOut,
                                                           const VXLIB_multiply_InitArgs *pKerInitArgs);

template VXLIB_STATUS
VXLIB_multiply_init_ci<VXLIB_MULTIPLY_DTYPE_I16S_I16S_O16S>(VXLIB_kernelHandle             handle,
                                                            const VXLIB_bufParams2D_t *    bufParamsIn0,
                                                            const VXLIB_bufParams2D_t *    bufParamsIn1,
                                                            const VXLIB_bufParams2D_t *    bufParamsOut,
                                                            const VXLIB_multiply_InitArgs *pKerInitArgs);

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_multiply_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

static inline void MultiplySat(uint8_t *pOutLocal, c7x::float_vec vscale)
{
   typedef typename c7x::make_full_vector<uint8_t>::type vec;

   c7x::ushort_vec vEven, vOdd;
   c7x::double_vec scaledD1, scaledD2, scaledD3, scaledD4, scaledD5, scaledD6, scaledD7, scaledD8;

   vec a = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0
   vec b = c7x::strm_eng<1, vec>::get_adv(); // fetch input 1 via SE1

   __vmpyubh_vvw((a), (b), vOdd, vEven);
   // short to float conversion
   c7x::float_vec unscaledF1 = __low_half_int_to_float(vOdd);
   c7x::float_vec unscaledF2 = __high_half_int_to_float(vOdd);
   c7x::float_vec unscaledF3 = __low_half_int_to_float(vEven);
   c7x::float_vec unscaledF4 = __high_half_int_to_float(vEven);

   __vmpysp2dp_vvw(vscale, unscaledF1, scaledD1, scaledD2);
   __vmpysp2dp_vvw(vscale, unscaledF2, scaledD3, scaledD4);
   __vmpysp2dp_vvw(vscale, unscaledF3, scaledD5, scaledD6);
   __vmpysp2dp_vvw(vscale, unscaledF4, scaledD7, scaledD8);

   c7x::int_vec scaledI1E = __double_to_int_rtz(scaledD1);
   c7x::int_vec scaledI1O = __double_to_int_rtz(scaledD2);
   c7x::int_vec scaledI2E = __double_to_int_rtz(scaledD3);
   c7x::int_vec scaledI2O = __double_to_int_rtz(scaledD4);
   c7x::int_vec scaledI3E = __double_to_int_rtz(scaledD5);
   c7x::int_vec scaledI3O = __double_to_int_rtz(scaledD6);
   c7x::int_vec scaledI4E = __double_to_int_rtz(scaledD7);
   c7x::int_vec scaledI4O = __double_to_int_rtz(scaledD8);

   c7x::int_vec scaledI1 = __vpackw_vvv(scaledI1O, scaledI1E);
   c7x::int_vec scaledI2 = __vpackw_vvv(scaledI2O, scaledI2E);
   c7x::int_vec scaledI3 = __vpackw_vvv(scaledI3O, scaledI3E);
   c7x::int_vec scaledI4 = __vpackw_vvv(scaledI4O, scaledI4E);

   c7x::short_vec scaledS1 = __pack_sat(scaledI2, scaledI1);
   c7x::short_vec scaledS2 = __pack_sat(scaledI4, scaledI3);
   scaledS1                = c7x::as_short_vec(__permute(vpermMaskSat, c7x::as_uchar_vec(scaledS1)));
   scaledS2                = c7x::as_short_vec(__permute(vpermMaskSat, c7x::as_uchar_vec(scaledS2)));

   c7x::uchar_vec saturatedOut = __vspackuhb_vvv(c7x::as_short_vec(scaledS2), c7x::as_short_vec(scaledS1));

   // store output via SA0
   __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
   vec *   addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
   __vstore_pred(tmp, addr, saturatedOut);                    // store value
}

static inline void MultiplyWrap(uint8_t *pOutLocal, c7x::float_vec vscale)
{
   typedef typename c7x::make_full_vector<uint8_t>::type vec;

   c7x::ushort_vec vEven, vOdd;
   c7x::double_vec scaledD1, scaledD2, scaledD3, scaledD4, scaledD5, scaledD6, scaledD7, scaledD8;

   vec a = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0
   vec b = c7x::strm_eng<1, vec>::get_adv(); // fetch input 1 via SE1

   __vmpyubh_vvw((a), (b), vEven, vOdd);

   // short to float conversion
   c7x::float_vec unscaledF1 = __low_half_int_to_float(vEven);
   c7x::float_vec unscaledF2 = __high_half_int_to_float(vEven);
   c7x::float_vec unscaledF3 = __low_half_int_to_float(vOdd);
   c7x::float_vec unscaledF4 = __high_half_int_to_float(vOdd);

   __vmpysp2dp_vvw(vscale, unscaledF1, scaledD1, scaledD2);
   __vmpysp2dp_vvw(vscale, unscaledF2, scaledD3, scaledD4);
   __vmpysp2dp_vvw(vscale, unscaledF3, scaledD5, scaledD6);
   __vmpysp2dp_vvw(vscale, unscaledF4, scaledD7, scaledD8);

   c7x::int_vec scaledI1E = __double_to_int_rtz(scaledD1);
   c7x::int_vec scaledI1O = __double_to_int_rtz(scaledD2);
   c7x::int_vec scaledI2E = __double_to_int_rtz(scaledD3);
   c7x::int_vec scaledI2O = __double_to_int_rtz(scaledD4);
   c7x::int_vec scaledI3E = __double_to_int_rtz(scaledD5);
   c7x::int_vec scaledI3O = __double_to_int_rtz(scaledD6);
   c7x::int_vec scaledI4E = __double_to_int_rtz(scaledD7);
   c7x::int_vec scaledI4O = __double_to_int_rtz(scaledD8);

   c7x::int_vec scaledI1 = __vpackw_vvv(scaledI1O, scaledI1E);
   c7x::int_vec scaledI2 = __vpackw_vvv(scaledI2O, scaledI2E);
   c7x::int_vec scaledI3 = __vpackw_vvv(scaledI3O, scaledI3E);
   c7x::int_vec scaledI4 = __vpackw_vvv(scaledI4O, scaledI4E);

   c7x::ushort_vec scaledS1 = c7x::as_ushort_vec(
       __permute_even_even_short(vpermMaskWrap, c7x::as_uchar_vec(scaledI2), c7x::as_uchar_vec(scaledI1)));
   c7x::ushort_vec scaledS2 = c7x::as_ushort_vec(
       __permute_even_even_short(vpermMaskWrap, c7x::as_uchar_vec(scaledI4), c7x::as_uchar_vec(scaledI3)));

   __vpred tmp  = c7x::strm_agen<0, c7x::char_vec>::get_vpred();                                  // generate predicate
   vec *   addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal);                                     // generate address
   __vstore_pred_interleave(tmp, addr, c7x::as_uchar_vec(scaledS1), c7x::as_uchar_vec(scaledS2)); // store value
}

template <typename dTypeIn0, typename dTypeIn1, typename dTypeOut>
inline void VXLIB_multiply_util_sat(VXLIB_kernelHandle handle, void *pOut, size_t size)
{
   VXLIB_DEBUGPRINTFN(0, "%s\n", "Enter function <dTypeIn0, dTypeIn1, dTypeOut>");
   typedef typename c7x::make_full_vector<dTypeOut>::type vec;

   dTypeOut *               pOutLocal    = (dTypeOut *) pOut;
   VXLIB_multiply_PrivArgs *pKerPrivArgs = (VXLIB_multiply_PrivArgs *) handle;
   VXLIB_F32                scale        = pKerPrivArgs->pKerInitArgs.scale;
   uint32_t                 shiftVal     = pKerPrivArgs->shiftVal;
   size_t                   multiplyMode = pKerPrivArgs->multiplyMode;

   c7x::int_vec vEven, vOdd;

   /* For 1 / 2^n */
   if (multiplyMode == VXLIB_MULTIPLY_DIV_2_N) {
      c7x::int_vec rightShiftVec = (c7x::int_vec) shiftVal;

      uint32_t     neg    = (1u << shiftVal) - 1u;
      c7x::int_vec negVec = (c7x::int_vec) neg;

      for (uint32_t counter = 0; counter < size; counter++) {
         vec a = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0
         vec b = c7x::strm_eng<1, vec>::get_adv(); // fetch input 1 via SE1

         __vmpyhw_vvw(a, b, vEven, vOdd);

         c7x::int_vec vEvenMask = (__shift_right(vEven, (c7x::int_vec) 31) & negVec) + vEven;
         c7x::int_vec vOddMask  = (__shift_right(vOdd, (c7x::int_vec) 31) & negVec) + vOdd;

         c7x::int_vec vEvenScaled = __shift_right(vEvenMask, rightShiftVec);
         c7x::int_vec vOddScaled  = __shift_right(vOddMask, rightShiftVec);

         c7x::short_vec vOut = __pack_sat(vOddScaled, vEvenScaled);

         __vpred pred   = c7x::strm_agen<0, vec>::get_vpred();
         vec *   pStore = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(pred, pStore, vOut);
      }
   }
   /* For Integer */
   else if (multiplyMode == VXLIB_MULTIPLY_INTEGER) {
      int32_t       intScale = (int32_t) scale;
      c7x::int_vec  vScale   = (c7x::int_vec) intScale;
      c7x::long_vec scaled1, scaled2, scaled3, scaled4;

      uint32_t loopCount = size;
      for (uint32_t counter = 0; counter < loopCount; counter++) {
         vec a = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0
         vec b = c7x::strm_eng<1, vec>::get_adv(); // fetch input 1 via SE1

         __vmpyhw_vvw(a, b, vEven, vOdd);

         __vmpywd_vvw(vEven, vScale, scaled1, scaled2);
         __vmpywd_vvw(vOdd, vScale, scaled3, scaled4);

         c7x::int_vec scaledI1 = __pack_sat(scaled2, scaled1);
         c7x::int_vec scaledI2 = __pack_sat(scaled4, scaled3);

         c7x::short_vec vOut = __pack_sat(scaledI2, scaledI1);

         __vpred pred   = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
         vec *   pStore = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
         __vstore_pred(pred, pStore, vOut);                           // store value
      }
   }
   /* Generic */
   else {
      c7x::double_vec vScaleD = (c7x::double_vec) scale;
      for (uint32_t counter = 0; counter < size; counter++) {

         vec a = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0
         vec b = c7x::strm_eng<1, vec>::get_adv(); // fetch input 1 via SE1

         __vmpyhw_vvw(a, b, vEven, vOdd);

         c7x::double_vec unscaledD1 = __low_int_to_double(vEven);
         c7x::double_vec unscaledD2 = __high_int_to_double(vEven);
         c7x::double_vec unscaledD3 = __low_int_to_double(vOdd);
         c7x::double_vec unscaledD4 = __high_int_to_double(vOdd);

         c7x::double_vec scaledD1 = unscaledD1 * vScaleD;
         c7x::double_vec scaledD2 = unscaledD2 * vScaleD;
         c7x::double_vec scaledD3 = unscaledD3 * vScaleD;
         c7x::double_vec scaledD4 = unscaledD4 * vScaleD;

         c7x::int_vec scaledI1E = __double_to_int_rtz(scaledD1);
         c7x::int_vec scaledI1O = __double_to_int_rtz(scaledD2);
         c7x::int_vec scaledI2E = __double_to_int_rtz(scaledD3);
         c7x::int_vec scaledI2O = __double_to_int_rtz(scaledD4);

         c7x::int_vec scaledI1 = __vpackw_vvv(scaledI1O, scaledI1E);
         c7x::int_vec scaledI2 = __vpackw_vvv(scaledI2O, scaledI2E);

         vec saturatedOut = __pack_sat(scaledI2, scaledI1);

         __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
         vec *   addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
         __vstore_pred(tmp, addr, saturatedOut);                    // store value
      }
   }

   VXLIB_DEBUGPRINTFN(0, "%s\n", "Exit function");
   return;
}

template <>
inline void
VXLIB_multiply_util_sat<VXLIB_MULTIPLY_TYPENAME_I8U_I8U_O8U>(VXLIB_kernelHandle handle, void *pOut, size_t size)
{
   VXLIB_DEBUGPRINTFN(0, "%s\n", "Enter function <VXLIB_MULTIPLY_TYPENAME_I8U_I8U_O8U>");
   typedef typename c7x::make_full_vector<uint8_t>::type vec;

   uint8_t *                pOutLocal    = (uint8_t *) pOut;
   VXLIB_multiply_PrivArgs *pKerPrivArgs = (VXLIB_multiply_PrivArgs *) handle;
   VXLIB_F32                scale        = pKerPrivArgs->pKerInitArgs.scale;
   uint32_t                 shiftVal     = pKerPrivArgs->shiftVal;
   size_t                   multiplyMode = pKerPrivArgs->multiplyMode;

   /* For 1 / 2^n */
   if (multiplyMode == VXLIB_MULTIPLY_DIV_2_N) {
      c7x::short_vec  rightShiftVec = (c7x::short_vec) shiftVal;
      c7x::ushort_vec vEven, vOdd;
      uint32_t        loopCount = size * VXLIB_MULTIPLY_UNROLL_FACTOR / 2;

      for (uint32_t counter = 0; counter < loopCount; counter++) {
         vec a = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0
         vec b = c7x::strm_eng<1, vec>::get_adv(); // fetch input 1 via SE1

         __vmpyubh_vvw((a), (b), vEven, vOdd);

         c7x::ushort_vec vEvenScaled = __shift_right(vEven, c7x::as_uint_vec(rightShiftVec));
         c7x::ushort_vec vOddScaled  = __shift_right(vOdd, c7x::as_uint_vec(rightShiftVec));

         c7x::uchar_vec vOut = __pack_usat(c7x::as_short_vec(vOddScaled), c7x::as_short_vec(vEvenScaled));

         __vpred pred   = c7x::strm_agen<0, vec>::get_vpred();
         vec *   pStore = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(pred, pStore, vOut);

         a = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0
         b = c7x::strm_eng<1, vec>::get_adv(); // fetch input 1 via SE1

         __vmpyubh_vvw((a), (b), vEven, vOdd);

         vEvenScaled = __shift_right(vEven, c7x::as_uint_vec(rightShiftVec));
         vOddScaled  = __shift_right(vOdd, c7x::as_uint_vec(rightShiftVec));

         vOut = __pack_usat(c7x::as_short_vec(vOddScaled), c7x::as_short_vec(vEvenScaled));

         pred   = c7x::strm_agen<0, vec>::get_vpred();
         pStore = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(pred, pStore, vOut);
      }
   }
   /* For Integer */
   else if (multiplyMode == VXLIB_MULTIPLY_INTEGER) {
      int16_t        intScale = (int16_t) scale;
      c7x::short_vec vScale   = (c7x::short_vec) intScale;

      c7x::ushort_vec vEven, vOdd;
      c7x::int_vec    scaled1, scaled2, scaled3, scaled4;

      uint32_t loopCount = size * VXLIB_MULTIPLY_UNROLL_FACTOR;

      for (uint32_t counter = 0; counter < loopCount; counter++) {
         vec a = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0
         vec b = c7x::strm_eng<1, vec>::get_adv(); // fetch input 1 via SE1

         __vmpyubh_vvw(a, b, vEven, vOdd);
         __vmpysuhw_vvw(vScale, vEven, scaled1, scaled2);
         __vmpysuhw_vvw(vScale, vOdd, scaled3, scaled4);

         c7x::short_vec scaledS1 = __pack_sat(scaled2, scaled1);
         c7x::short_vec scaledS2 = __pack_sat(scaled4, scaled3);

         c7x::uchar_vec vOut = __pack_usat((scaledS2), (scaledS1));

         __vpred pred   = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
         vec *   pStore = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
         __vstore_pred(pred, pStore, vOut);                           // store value
      }
   }
   /* Generic */
   else {
      c7x::float_vec vScale = (c7x::float_vec) scale;
      for (uint32_t counter = 0; counter < size; counter++) {
         MultiplySat(pOutLocal, vScale);
         MultiplySat(pOutLocal, vScale);
         MultiplySat(pOutLocal, vScale);
         MultiplySat(pOutLocal, vScale);
      }
   }

   VXLIB_DEBUGPRINTFN(0, "%s\n", "Exit function");
   return;
}

template <>
inline void
VXLIB_multiply_util_sat<VXLIB_MULTIPLY_TYPENAME_I8U_I8U_O16S>(VXLIB_kernelHandle handle, void *pOut, size_t size)
{
   VXLIB_DEBUGPRINTFN(0, "%s\n", "Enter function <VXLIB_MULTIPLY_TYPENAME_I8U_I8U_O16S>");
   typedef typename c7x::make_full_vector<int16_t>::type vec;

   int16_t *                pOutLocal    = (int16_t *) pOut;
   VXLIB_multiply_PrivArgs *pKerPrivArgs = (VXLIB_multiply_PrivArgs *) handle;
   VXLIB_F32                scale        = pKerPrivArgs->pKerInitArgs.scale;
   uint32_t                 shiftVal     = pKerPrivArgs->shiftVal;
   size_t                   multiplyMode = pKerPrivArgs->multiplyMode;

   c7x::int_vec vEven, vOdd;

   /* For 1 / 2^n */
   if (multiplyMode == VXLIB_MULTIPLY_DIV_2_N) {
      c7x::int_vec rightShiftVec = (c7x::int_vec) shiftVal;
      uint32_t     loopCount     = VXLIB_ceilingDiv(size, 2);

      for (uint32_t counter = 0; counter < loopCount; counter++) {
         vec a = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0
         vec b = c7x::strm_eng<1, vec>::get_adv(); // fetch input 1 via SE1

         __vmpyhw_vvw(a, b, vEven, vOdd);

         c7x::int_vec vEvenScaled = __shift_right(vEven, rightShiftVec);
         c7x::int_vec vOddScaled  = __shift_right(vOdd, rightShiftVec);

         c7x::short_vec vOut = __pack_sat(vOddScaled, vEvenScaled);

         __vpred pred   = c7x::strm_agen<0, vec>::get_vpred();
         vec *   pStore = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(pred, pStore, vOut);

         a = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0
         b = c7x::strm_eng<1, vec>::get_adv(); // fetch input 1 via SE1

         __vmpyhw_vvw(a, b, vEven, vOdd);

         vEvenScaled = __shift_right(vEven, rightShiftVec);
         vOddScaled  = __shift_right(vOdd, rightShiftVec);

         vOut = __pack_sat(vOddScaled, vEvenScaled);

         pred   = c7x::strm_agen<0, vec>::get_vpred();
         pStore = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(pred, pStore, vOut);
      }
   }
   /* For Integer */
   else if (multiplyMode == VXLIB_MULTIPLY_INTEGER) {
      int32_t      intScale = (int32_t) scale;
      c7x::int_vec vScale   = (c7x::int_vec) intScale;

      uint32_t loopCount = size;
      for (uint32_t counter = 0; counter < loopCount; counter++) {
         vec a = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0
         vec b = c7x::strm_eng<1, vec>::get_adv(); // fetch input 1 via SE1

         __vmpyhw_vvw(a, b, vEven, vOdd);

         c7x::int_vec scaled1 = vEven * vScale;
         c7x::int_vec scaled2 = vOdd * vScale;

         c7x::short_vec vOut = __pack_sat(scaled2, scaled1);

         __vpred pred   = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
         vec *   pStore = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
         __vstore_pred(pred, pStore, vOut);                           // store value
      }
   }
   /* Generic */
   else {
      c7x::double_vec scaledD1, scaledD2, scaledD3, scaledD4;

      c7x::float_vec vscale = (c7x::float_vec) scale;
      for (uint32_t counter = 0; counter < size; counter++) {

         vec a = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0
         vec b = c7x::strm_eng<1, vec>::get_adv(); // fetch input 1 via SE1

         __vmpyhw_vvw(a, b, vOdd, vEven);

         c7x::float_vec unscaledF1 = __int_to_float(vOdd);
         c7x::float_vec unscaledF2 = __int_to_float(vEven);

         __vmpysp2dp_vvw(vscale, unscaledF1, scaledD1, scaledD2);
         __vmpysp2dp_vvw(vscale, unscaledF2, scaledD3, scaledD4);

         c7x::int_vec scaledI1E = __double_to_int_rtz(scaledD1);
         c7x::int_vec scaledI1O = __double_to_int_rtz(scaledD2);
         c7x::int_vec scaledI2E = __double_to_int_rtz(scaledD3);
         c7x::int_vec scaledI2O = __double_to_int_rtz(scaledD4);

         c7x::int_vec scaledI1 = __vpackw_vvv(scaledI1O, scaledI1E);
         c7x::int_vec scaledI2 = __vpackw_vvv(scaledI2O, scaledI2E);

         vec     saturatedOut = __pack_sat(scaledI2, scaledI1);
         __vpred tmp          = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
         vec *   addr         = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
         __vstore_pred(tmp, addr, saturatedOut);                            // store value
      }
   }

   VXLIB_DEBUGPRINTFN(0, "%s\n", "Exit function");
   return;
}

template <typename dTypeIn0, typename dTypeIn1, typename dTypeOut>
inline void VXLIB_multiply_util_wrap(VXLIB_kernelHandle handle, void *pOut, size_t size)
{
   VXLIB_DEBUGPRINTFN(0, "%s\n", "Enter function <dTypeIn0, dTypeIn1, dTypeOut>");
   typedef typename c7x::make_full_vector<dTypeOut>::type vec;

   dTypeOut *               pOutLocal    = (dTypeOut *) pOut;
   VXLIB_multiply_PrivArgs *pKerPrivArgs = (VXLIB_multiply_PrivArgs *) handle;
   VXLIB_F32                scale        = pKerPrivArgs->pKerInitArgs.scale;
   uint32_t                 shiftVal     = pKerPrivArgs->shiftVal;
   size_t                   multiplyMode = pKerPrivArgs->multiplyMode;

   c7x::int_vec vEven, vOdd;

   /* For 1 / 2^n */
   if (multiplyMode == VXLIB_MULTIPLY_DIV_2_N) {
      c7x::int_vec rightShiftVec = (c7x::int_vec) shiftVal;
      uint32_t     neg           = (1u << shiftVal) - 1u;
      c7x::int_vec negVec        = (c7x::int_vec) neg;

      for (uint32_t counter = 0; counter < VXLIB_ceilingDiv(size, 2); counter++) {
         vec a = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0
         vec b = c7x::strm_eng<1, vec>::get_adv(); // fetch input 1 via SE1

         __vmpyhw_vvw(a, b, vEven, vOdd);

         c7x::int_vec vEvenMask = (__shift_right(vEven, (c7x::int_vec) 31) & negVec) + vEven;
         c7x::int_vec vOddMask  = (__shift_right(vOdd, (c7x::int_vec) 31) & negVec) + vOdd;

         c7x::int_vec vEvenScaled = __shift_right(vEvenMask, rightShiftVec);
         c7x::int_vec vOddScaled  = __shift_right(vOddMask, rightShiftVec);

         __vpred pred   = c7x::strm_agen<0, vec>::get_vpred();
         vec *   pStore = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred_interleave(pred, pStore, c7x::as_short_vec(vEvenScaled), c7x::as_short_vec(vOddScaled));

         a = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0
         b = c7x::strm_eng<1, vec>::get_adv(); // fetch input 1 via SE1

         __vmpyhw_vvw(a, b, vEven, vOdd);

         vEvenMask = (__shift_right(vEven, (c7x::int_vec) 31) & negVec) + vEven;
         vOddMask  = (__shift_right(vOdd, (c7x::int_vec) 31) & negVec) + vOdd;

         vEvenScaled = __shift_right(vEvenMask, rightShiftVec);
         vOddScaled  = __shift_right(vOddMask, rightShiftVec);

         pred   = c7x::strm_agen<0, vec>::get_vpred();
         pStore = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred_interleave(pred, pStore, c7x::as_short_vec(vEvenScaled), c7x::as_short_vec(vOddScaled));
      }
   }
   /* For Integer */
   else if (multiplyMode == VXLIB_MULTIPLY_INTEGER) {
      int32_t       intScale = (int32_t) scale;
      c7x::int_vec  vScale   = (c7x::int_vec) intScale;
      c7x::long_vec scaled1, scaled2, scaled3, scaled4;

      uint32_t loopCount = size;
      for (uint32_t counter = 0; counter < loopCount; counter++) {
         vec a = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0
         vec b = c7x::strm_eng<1, vec>::get_adv(); // fetch input 1 via SE1

         __vmpyhw_vvw(a, b, vEven, vOdd);

         __vmpywd_vvw(vEven, vScale, scaled1, scaled2);
         __vmpywd_vvw(vOdd, vScale, scaled3, scaled4);

         c7x::int_vec scaledI1 = __vpackw_vvv(c7x::as_int_vec(scaled2), c7x::as_int_vec(scaled1));
         c7x::int_vec scaledI2 = __vpackw_vvv(c7x::as_int_vec(scaled4), c7x::as_int_vec(scaled3));

         __vpred pred   = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
         vec *   pStore = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
         __vstore_pred_interleave(pred, pStore, c7x::as_short_vec(scaledI1), c7x::as_short_vec(scaledI2));
      }
   }
   /* Generic */
   else {

      c7x::double_vec vScaleD = (c7x::double_vec) scale;
      for (uint32_t counter = 0; counter < size; counter++) {

         vec a = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0
         vec b = c7x::strm_eng<1, vec>::get_adv(); // fetch input 1 via SE1

         __vmpyhw_vvw(a, b, vEven, vOdd);

         c7x::double_vec unscaledD1 = __low_int_to_double(vEven);
         c7x::double_vec unscaledD2 = __high_int_to_double(vEven);
         c7x::double_vec unscaledD3 = __low_int_to_double(vOdd);
         c7x::double_vec unscaledD4 = __high_int_to_double(vOdd);

         c7x::double_vec scaledD1 = unscaledD1 * vScaleD;
         c7x::double_vec scaledD2 = unscaledD2 * vScaleD;
         c7x::double_vec scaledD3 = unscaledD3 * vScaleD;
         c7x::double_vec scaledD4 = unscaledD4 * vScaleD;

         c7x::int_vec scaledI1E = __double_to_int_rtz(scaledD1);
         c7x::int_vec scaledI1O = __double_to_int_rtz(scaledD2);
         c7x::int_vec scaledI2E = __double_to_int_rtz(scaledD3);
         c7x::int_vec scaledI2O = __double_to_int_rtz(scaledD4);

         c7x::int_vec scaledI1 = __vpackw_vvv(scaledI1O, scaledI1E);
         c7x::int_vec scaledI2 = __vpackw_vvv(scaledI2O, scaledI2E);

         __vpred tmp  = c7x::strm_agen<0, c7x::short_vec>::get_vpred(); // generate predicate
         vec *   addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal);     // generate address
         __vstore_pred_interleave(tmp, addr, c7x::as_short_vec(scaledI1), c7x::as_short_vec(scaledI2));
      }
   }

   VXLIB_DEBUGPRINTFN(0, "%s\n", "Exit function");
   return;
}

template <>
inline void
VXLIB_multiply_util_wrap<VXLIB_MULTIPLY_TYPENAME_I8U_I8U_O8U>(VXLIB_kernelHandle handle, void *pOut, size_t size)
{
   VXLIB_DEBUGPRINTFN(0, "%s\n", "Enter function <VXLIB_MULTIPLY_TYPENAME_I8U_I8U_O8U>");
   typedef typename c7x::make_full_vector<uint8_t>::type vec;

   uint8_t *                pOutLocal    = (uint8_t *) pOut;
   VXLIB_multiply_PrivArgs *pKerPrivArgs = (VXLIB_multiply_PrivArgs *) handle;
   VXLIB_F32                scale        = pKerPrivArgs->pKerInitArgs.scale;
   uint32_t                 shiftVal     = pKerPrivArgs->shiftVal;
   size_t                   multiplyMode = pKerPrivArgs->multiplyMode;

   /* For 1 / 2^n */
   if (multiplyMode == VXLIB_MULTIPLY_DIV_2_N) {
      c7x::short_vec  rightShiftVec = (c7x::short_vec) shiftVal;
      c7x::ushort_vec vEven, vOdd;
      uint32_t        loopCount = size * VXLIB_MULTIPLY_UNROLL_FACTOR;
      for (uint32_t counter = 0; counter < loopCount; counter++) {
         vec a = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0
         vec b = c7x::strm_eng<1, vec>::get_adv(); // fetch input 1 via SE1

         __vmpyubh_vvw((a), (b), vEven, vOdd);

         c7x::ushort_vec vEvenScaled = __shift_right(vEven, c7x::as_uint_vec(rightShiftVec));
         c7x::ushort_vec vOddScaled  = __shift_right(vOdd, c7x::as_uint_vec(rightShiftVec));

         __vpred         pred   = c7x::strm_agen<0, vec>::get_vpred();
         c7x::uchar_vec *pStore = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred_interleave(pred, pStore, c7x::as_uchar_vec(vEvenScaled), c7x::as_uchar_vec(vOddScaled));
      }
   }
   /* For Integer */
   else if (multiplyMode == VXLIB_MULTIPLY_INTEGER) {
      int16_t        intScale = (int16_t) scale;
      c7x::short_vec vScale   = (c7x::short_vec) intScale;

      c7x::ushort_vec vEven, vOdd;
      c7x::int_vec    scaled1, scaled2, scaled3, scaled4;

      uint32_t loopCount = size * VXLIB_MULTIPLY_UNROLL_FACTOR;
      for (uint32_t counter = 0; counter < loopCount; counter++) {
         vec a = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0
         vec b = c7x::strm_eng<1, vec>::get_adv(); // fetch input 1 via SE1

         __vmpyubh_vvw(a, b, vEven, vOdd);

         __vmpyubh_vvw(a, b, vEven, vOdd);
         __vmpysuhw_vvw(vScale, vEven, scaled1, scaled2);
         __vmpysuhw_vvw(vScale, vOdd, scaled3, scaled4);

         c7x::short_vec scaledS1 = __vpackl2_vvv(c7x::as_short_vec(scaled2), c7x::as_short_vec(scaled1));
         c7x::short_vec scaledS2 = __vpackl2_vvv(c7x::as_short_vec(scaled4), c7x::as_short_vec(scaled3));

         __vpred pred   = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
         vec *   pStore = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
         __vstore_pred_interleave(pred, pStore, c7x::as_uchar_vec(scaledS1), c7x::as_uchar_vec(scaledS2));
      }
   }

   /* Generic */
   else {
      c7x::float_vec vScale = (c7x::float_vec) scale;

      for (uint32_t counter = 0; counter < size; counter++) {

         MultiplyWrap(pOutLocal, vScale);
         MultiplyWrap(pOutLocal, vScale);
         MultiplyWrap(pOutLocal, vScale);
         MultiplyWrap(pOutLocal, vScale);
      }
   }
   VXLIB_DEBUGPRINTFN(0, "%s\n", "Exit function");
   return;
}

template <>
inline void
VXLIB_multiply_util_wrap<VXLIB_MULTIPLY_TYPENAME_I8U_I8U_O16S>(VXLIB_kernelHandle handle, void *pOut, size_t size)
{
   VXLIB_DEBUGPRINTFN(0, "%s\n", "Enter function <VXLIB_MULTIPLY_TYPENAME_I8U_I8U_O16S>");
   typedef typename c7x::make_full_vector<int16_t>::type vec;

   int16_t *                pOutLocal    = (int16_t *) pOut;
   VXLIB_multiply_PrivArgs *pKerPrivArgs = (VXLIB_multiply_PrivArgs *) handle;
   VXLIB_F32                scale        = pKerPrivArgs->pKerInitArgs.scale;
   uint32_t                 shiftVal     = pKerPrivArgs->shiftVal;
   size_t                   multiplyMode = pKerPrivArgs->multiplyMode;

   c7x::int_vec vEven, vOdd;
   /* For 1 / 2^n */
   if (multiplyMode == VXLIB_MULTIPLY_DIV_2_N) {
      c7x::int_vec rightShiftVec = (c7x::int_vec) shiftVal;
      uint32_t     loopCount     = size;

      for (uint32_t counter = 0; counter < loopCount; counter++) {
         vec a = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0
         vec b = c7x::strm_eng<1, vec>::get_adv(); // fetch input 1 via SE1

         __vmpyhw_vvw(a, b, vEven, vOdd);

         c7x::int_vec vEvenScaled = __shift_right(vEven, rightShiftVec);
         c7x::int_vec vOddScaled  = __shift_right(vOdd, rightShiftVec);

         __vpred pred   = c7x::strm_agen<0, vec>::get_vpred();
         vec *   pStore = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred_interleave(pred, pStore, c7x::as_short_vec(vEvenScaled), c7x::as_short_vec(vOddScaled));
      }
   }
   /* For Integer */
   else if (multiplyMode == VXLIB_MULTIPLY_INTEGER) {
      int32_t      intScale = (int32_t) scale;
      c7x::int_vec vScale   = (c7x::int_vec) intScale;

      uint32_t loopCount = size;
      for (uint32_t counter = 0; counter < loopCount; counter++) {
         vec a = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0
         vec b = c7x::strm_eng<1, vec>::get_adv(); // fetch input 1 via SE1

         __vmpyhw_vvw(a, b, vEven, vOdd);

         c7x::int_vec scaled1 = vEven * vScale;
         c7x::int_vec scaled2 = vOdd * vScale;

         __vpred pred   = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
         vec *   pStore = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
         __vstore_pred_interleave(pred, pStore, c7x::as_short_vec(scaled1), c7x::as_short_vec(scaled2));
      }
   }
   /* Generic */
   else {
      c7x::double_vec scaledD1, scaledD2, scaledD3, scaledD4;

      c7x::float_vec vscale = (c7x::float_vec) scale;
      for (uint32_t counter = 0; counter < size; counter++) {

         vec a = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0
         vec b = c7x::strm_eng<1, vec>::get_adv(); // fetch input 1 via SE1

         __vmpyhw_vvw(a, b, vEven, vOdd);

         c7x::float_vec unscaledF1 = __int_to_float(vEven);
         c7x::float_vec unscaledF2 = __int_to_float(vOdd);

         __vmpysp2dp_vvw(vscale, unscaledF1, scaledD1, scaledD2);
         __vmpysp2dp_vvw(vscale, unscaledF2, scaledD3, scaledD4);

         c7x::int_vec scaledI1E = __double_to_int_rtz(scaledD1);
         c7x::int_vec scaledI1O = __double_to_int_rtz(scaledD2);
         c7x::int_vec scaledI2E = __double_to_int_rtz(scaledD3);
         c7x::int_vec scaledI2O = __double_to_int_rtz(scaledD4);

         c7x::int_vec scaledI1 = __vpackw_vvv(scaledI1O, scaledI1E);
         c7x::int_vec scaledI2 = __vpackw_vvv(scaledI2O, scaledI2E);

         __vpred tmp  = c7x::strm_agen<0, c7x::short_vec>::get_vpred(); // generate predicate
         vec *   addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal);     // generate address
         __vstore_pred_interleave(tmp, addr, c7x::as_short_vec(scaledI1), c7x::as_short_vec(scaledI2));
      }
   }
   VXLIB_DEBUGPRINTFN(0, "%s\n", "Exit function");
   return;
}

// this method performs pixel-wise addition of two input images
template <typename dTypeIn0, typename dTypeIn1, typename dTypeOut>
VXLIB_STATUS
VXLIB_multiply_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut)
{
   VXLIB_DEBUGPRINTFN(0, "%s\n", "Enter Function");

   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_multiply_PrivArgs *pKerPrivArgs = (VXLIB_multiply_PrivArgs *) handle;

   // obtain image parameters and overflow policy
   size_t                numBlocks      = pKerPrivArgs->numBlocks;
   VXLIB_OVERFLOW_POLICY overFlowPolicy = pKerPrivArgs->pKerInitArgs.overFlowPolicy;

   // create local pointers
   dTypeIn0 *restrict pInLocal0 = (dTypeIn0 *) pIn0;
   dTypeIn1 *restrict pInLocal1 = (dTypeIn1 *) pIn1;

   // address of SE and SA parameters
   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   // retrieve SE and SA parameters
   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);

   __SE0_OPEN(pInLocal0, se0Params);
   __SE1_OPEN(pInLocal1, se1Params);

   // open SA0 to write output samples
   __SA0_OPEN(sa0Params);

   if (overFlowPolicy == VXLIB_SATURATE) {
      VXLIB_multiply_util_sat<dTypeIn0, dTypeIn1, dTypeOut>(handle, pOut, numBlocks);
   }
   else {
      VXLIB_multiply_util_wrap<dTypeIn0, dTypeIn1, dTypeOut>(handle, pOut, numBlocks);
   }

   VXLIB_DEBUGPRINTFN(0, "%s\n", "Exit Function");
   return (status);
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_multiply_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS VXLIB_multiply_exec_ci<VXLIB_MULTIPLY_TYPENAME_I8U_I8U_O8U>(VXLIB_kernelHandle handle,
                                                                                  void *restrict     pIn0,
                                                                                  void *restrict     pIn1,
                                                                                  void *restrict     pOut);

template VXLIB_STATUS VXLIB_multiply_exec_ci<VXLIB_MULTIPLY_TYPENAME_I8U_I8U_O16S>(VXLIB_kernelHandle handle,
                                                                                   void *restrict     pIn0,
                                                                                   void *restrict     pIn1,
                                                                                   void *restrict     pOut);

template VXLIB_STATUS VXLIB_multiply_exec_ci<VXLIB_MULTIPLY_TYPENAME_I8U_I16S_O16S>(VXLIB_kernelHandle handle,
                                                                                    void *restrict     pIn0,
                                                                                    void *restrict     pIn1,
                                                                                    void *restrict     pOut);

template VXLIB_STATUS VXLIB_multiply_exec_ci<VXLIB_MULTIPLY_TYPENAME_I16S_I16S_O16S>(VXLIB_kernelHandle handle,
                                                                                     void *restrict     pIn0,
                                                                                     void *restrict     pIn1,
                                                                                     void *restrict     pOut);

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Performance estimation function */
/*                                                                                                                    */
/**********************************************************************************************************************/

void VXLIB_multiply_perfEst(VXLIB_kernelHandle             handle,
                            const VXLIB_bufParams2D_t     *bufParamsIn0,
                            const VXLIB_bufParams2D_t     *bufParamsIn1,
                            const VXLIB_bufParams2D_t     *bufParamsOut,
                            const VXLIB_multiply_InitArgs *pKerInitArgs,
                            size_t                        *archCycles,
                            size_t                        *estCycles)
{

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_multiply_PrivArgs *pKerPrivArgs = (VXLIB_multiply_PrivArgs *) handle;
   size_t                   multiplyMode = pKerPrivArgs->multiplyMode;

   uint32_t dTypeIn0 = bufParamsIn0->data_type;
   uint32_t dTypeIn1 = bufParamsIn1->data_type;
   uint32_t dTypeOut = bufParamsOut->data_type;

   // obtain loop count for compute loop
   size_t numBlocks   = pKerPrivArgs->numBlocks;
   size_t overheadCnt = 58; // profiled code before entering compute loop

   VXLIB_OVERFLOW_POLICY overFlowPolicy = pKerPrivArgs->pKerInitArgs.overFlowPolicy;

   /* Estimation for SATURATE cases */
   if (overFlowPolicy == VXLIB_SATURATE) {
      if (multiplyMode == VXLIB_MULTIPLY_DIV_2_N) {
         if (VXLIB_MULTIPLY_I8U_I8U_O8U) {
            numBlocks   = (numBlocks * VXLIB_MULTIPLY_UNROLL_FACTOR) / 2;
            *archCycles = 6 + numBlocks * 3;
         }
         else if (VXLIB_MULTIPLY_I8U_I8U_O16S) {
            numBlocks   = (numBlocks) / 2;
            *archCycles = 6 + numBlocks * 3;
         }
         else {
            *archCycles = 8 + numBlocks * 3;
         }
      }
      else if (multiplyMode == VXLIB_MULTIPLY_INTEGER) {
         if (VXLIB_MULTIPLY_I8U_I8U_O8U) {
            numBlocks   = (numBlocks * VXLIB_MULTIPLY_UNROLL_FACTOR);
            *archCycles = 9 + numBlocks * 3;
         }
         else if (VXLIB_MULTIPLY_I8U_I8U_O16S) {
            *archCycles = 9 + numBlocks * 2;
         }
         else {
            *archCycles = 9 + numBlocks * 3;
         }
      }
      else {
         if (VXLIB_MULTIPLY_I8U_I8U_O8U) {
            *archCycles = 10 + numBlocks * 44;
         }
         else if (VXLIB_MULTIPLY_I8U_I8U_O16S) {
            *archCycles = 18 + numBlocks * 4;
         }
         else {
            *archCycles = 19 + numBlocks * 5;
         }
      }
   }
   /* Estimation for WRAP cases */
   else {
      if (multiplyMode == VXLIB_MULTIPLY_DIV_2_N) {
         if (VXLIB_MULTIPLY_I8U_I8U_O8U) {
            numBlocks   = (numBlocks * VXLIB_MULTIPLY_UNROLL_FACTOR);
            *archCycles = 5 + numBlocks * 1;
         }
         else if (VXLIB_MULTIPLY_I8U_I8U_O16S) {
            *archCycles = 5 + numBlocks * 1;
         }
         else {
            numBlocks   = (numBlocks) / 2;
            *archCycles = 7 + numBlocks * 5;
         }
      }
      else if (multiplyMode == VXLIB_MULTIPLY_INTEGER) {
         if (VXLIB_MULTIPLY_I8U_I8U_O8U) {
            numBlocks   = (numBlocks * VXLIB_MULTIPLY_UNROLL_FACTOR);
            *archCycles = 8 + numBlocks * 3;
         }
         else if (VXLIB_MULTIPLY_I8U_I8U_O16S) {
            *archCycles = 8 + numBlocks * 2;
         }
         else {
            *archCycles = 8 + numBlocks * 3;
         }
      }
      else {
         if (VXLIB_MULTIPLY_I8U_I8U_O8U) {
            *archCycles = 33 + numBlocks * 33;
         }
         else if (VXLIB_MULTIPLY_I8U_I8U_O16S) {
            *archCycles = 16 + numBlocks * 4;
         }
         else {
            *archCycles = 18 + numBlocks * 5;
         }
      }
   }
   *estCycles = overheadCnt + *archCycles;
}

/* ======================================================================== */
/*  End of file:  VXLIB_multiply_ci.cpp                                     */
/* ======================================================================== */
