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

#ifndef MATHLIB_LUT_H_
#define MATHLIB_LUT_H_ 1

/******************************************************************************/
/*                                                                            */
/* Includes                                                                   */
/*                                                                            */
/******************************************************************************/
#include "MATHLIB_debugPrint.h"
#include "MATHLIB_types.h"
#include "c7x.h"
#include "c7x_scalable.h"

/**
 * \ingroup MATHLIB_COMMON
 */

/**
 * \defgroup MATHLIB_LUT Lookup table utilities
 * \brief This module consists of utility functions tailored towards LUT that are commonly applicable to various
 * MATHLIB kernels.
 */
/**@{*/

/******************************************************************************/
/*                                                                            */
/* Defines                                                                    */
/*                                                                            */
/******************************************************************************/

#define MATHLIB_KTABLE_OFFSET 0
#define MATHLIB_JTABLE_OFFSET 4
#define MATHLIB_LOGTABLE_OFFSET 8
#define MATHLIB_VTABLE_OFFSET 16

/******************************************************************************/
/*                                                                            */
/* MATHLIB_LUTReadUpperBits                                                   */
/*                                                                            */
/******************************************************************************/

/**
 * @brief This method reads bits 63-32 of LUT value at vecOffset.
 *
 * @tparam      vecType       : datatype of implementation
 * @param [in]  vecOffset   : input vector of LUT offsets
 *
 * @return  vector with LUT upper bit values.
 *
 * @remarks None
 */

template <typename vecType> static inline c7x::uint_vec MATHLIB_LUTReadUpperBits(vecType vecOffset)
{
   c7x::uint_vec vecOut;
   vecOut = c7x::reinterpret<c7x::uint_vec>(__lookup_read_int(__LUT_SET0, vecOffset));

   return vecOut;
}

/******************************************************************************/
/*                                                                            */
/* MATHLIB_LUTReadLowerBits                                                   */
/*                                                                            */
/******************************************************************************/

/**
 * @brief This method reads bits 31-0 of LUT value at vecOffset.
 *
 * @tparam      vecType       : datatype of implementation
 * @param [in]  vecOffset   : input vector of LUT offsets
 *
 * @return  vector with LUT lower bit values.
 *
 * @remarks None
 */

template <typename vecType> static inline c7x::uint_vec MATHLIB_LUTReadLowerBits(vecType vecOffset)
{
   c7x::uint_vec vecOut;
   vecOut = c7x::reinterpret<c7x::uint_vec>(__lookup_read_int(__LUT_SET1, vecOffset));

   return vecOut;
}

/******************************************************************************/
/*                                                                            */
/* MATHLIB_LUTInit                                                            */
/*                                                                            */
/******************************************************************************/

/**
 * @brief This method intializes the upper and lower bit lookup tables.
 *
 * @remarks None
 */
static inline void MATHLIB_LUTInit()
{
   const uint32_t tableSize = 20;

   // Upper Bits LUT - Contains bits 63-32 for all lookup table values

   uint32_t MATHLIB_LUTHigh[tableSize] = {
       0x3FF00000u, // 2^(0/4) 1.000000000 ti_math_kTable
       0x3FF306FEu, // 2^(1/4) 1.189207115
       0x3FF6A09Eu, // 2^(2/4) 1.414213562
       0x3FFAE89Fu, // 2^(3/4) 1.681792831
       0x3FF00000u, // 2^(0/16) 1.000000000 ti_math_jTable
       0x3FF0B558u, // 2^(1/16) 1.044273782
       0x3FF172B8u, // 2^(2/16) 1.090507733
       0x3FF2387Au, // 2^(3/16) 1.138788635
       0x00000000u, // 0.0000000000 ti_math_logtable
       0xBFBE2707u, // -0.1177830356
       0xBFCC8FF7u, // -0.2231435513
       0xBFD4618Bu, // -0.3184537311
       0xBFD9F323u, // -0.4054651081
       0xBFDF128Fu, // -0.4855078157
       0xBFE1E85Fu, // -0.5596157879
       0xBFE41D8Fu, // -0.6286086594
       0x00000000u, // 0.00000000000000000000 ti_math_vTable
       0x3FE0C152u, //  0.52359877559829887308
       0x3FF921FBu, // 1.57079632679489661923
       0x3FF0C152u  // 1.04719755119659774615
   };

   // Lower Bits LUT - Contains bits 31-0 for all lookup table values

   uint32_t MATHLIB_LUTLow[tableSize] = {
       0x00000000u, // 2^(0/4) 1.000000000 ti_math_kTable
       0x0A318737u, // 2^(1/4) 1.189207115
       0x6665983Eu, // 2^(2/4) 1.414213562
       0x997CAD14u, // 2^(3/4) 1.681792831
       0x00000000u, // 2^(0/16) 1.000000000 ti_math_jTable
       0x6CDC29EBu, // 2^(1/16) 1.044273782
       0x3C945254u, // 2^(2/16) 1.090507733
       0x6E861A8Cu, // 2^(3/16) 1.138788635
       0x00000000u, // 0.0000000000 ti_math_logtable
       0x6DECF45Eu, // -0.1177830356
       0xC792CA49u, // -0.2231435513
       0xC2174681u, // -0.3184537311
       0xECBD59C8u, // -0.4054651081
       0x5F9891BEu, // -0.4855078157
       0x5E6B627Du, // -0.5596157879
       0xE8435F76u, // -0.6286086594
       0x00000000u, // 0.00000000000000000000 ti_math_vTable
       0x382D7366u, //  0.52359877559829887308
       0x54442D18u, // 1.57079632679489661923
       0x382D7366u  // 1.04719755119659774615
   };

   // Intialize Upper Bits LUT
   __sLTCRFlags_t set0flags;

   // 16 way lut for 32-bit data with 8 entries
   set0flags.INTERPOLATION = __LUT_INTERP_OFF;
   set0flags.SATURATION    = __LUT_SAT_OFF;
   set0flags.SIGNUNSIGN    = __LUT_SIGNED;           // data elements are signed
   set0flags.ELETYPE       = __LUT_ELETYPE_32BIT;    // 32-bit elements
   set0flags.NUMTBLS       = __LUT_NUM_TBL_16;       // 16 parallel lookup every cycle
   set0flags.TBLSIZE       = __LUT_TBL_SIZE_2KBYTES; // Total table size
   set0flags.WEIGHTSIZE    = __LUT_WSIZE_8BIT;
   set0flags.PROMOTION     = __LUT_PROMOTE_OFF;

   // Set configuration register for SET0
   __LUT_SET_LTCR(__LUT_SET0, __lut_set_param(&set0flags));

   // Start the table at offset 0 from the beginning of L1D-SRAM
   __LUT_SET_LTBR(__LUT_SET0, 0x0000);

   // Enable set 0
   __LUT_SET_LTER(__LUT_ENABLE_0);

   // Read the elements as a quantum of unsigned 64-bits
   uint64_t      *pData = (uint64_t *) &MATHLIB_LUTHigh[0];
   c7x::ulong_vec vData;
   uint32_t       rowOffset    = 0;
   uint32_t       rowIncrement = 2;

   for (uint32_t j = 0; j < (tableSize / rowIncrement); j++) {
      // Read the quantum into a vector of ulong8
      vData = (c7x::ulong_vec)(pData[j]);

      // Update the quanta in all N ways
      __lookup_init(__LUT_SET0, __as_uint16(vData), rowOffset);

      // Increment the row_offset by 2 as we are initializing 2 32-bit elements at a time
      rowOffset += rowIncrement;
   }

   // Intialize Lower Bits LUT
   __sLTCRFlags_t set1flags;

   // 16 way lut for 32-bit data with 8 entries
   set1flags.INTERPOLATION = __LUT_INTERP_OFF;
   set1flags.SATURATION    = __LUT_SAT_OFF;
   set1flags.SIGNUNSIGN    = __LUT_SIGNED;           // data elements are signed
   set1flags.ELETYPE       = __LUT_ELETYPE_32BIT;    // working with 32-bit elements
   set1flags.NUMTBLS       = __LUT_NUM_TBL_16;       // 16 parallel lookup every cycle
   set1flags.TBLSIZE       = __LUT_TBL_SIZE_2KBYTES; // Total table size
   set1flags.WEIGHTSIZE    = __LUT_WSIZE_8BIT;
   set1flags.PROMOTION     = __LUT_PROMOTE_OFF;

   // Set configuration register for SET1
   __LUT_SET_LTCR(__LUT_SET1, __lut_set_param(&set1flags));

   // Start the table at offset 0x2000 from the beginning of L1D-SRAM
   __LUT_SET_LTBR(__LUT_SET1, 0x2000);

   // Enable set 1
   __LUT_SET_LTER(__LUT_ENABLE_1);

   // Read the elements as a quantum of unsigned 64-bits
   pData     = (uint64_t *) &MATHLIB_LUTLow[0];
   rowOffset = 0;

   for (uint32_t j = 0; j < (tableSize / rowIncrement); j++) {
      // Read the quantum into a vector of ulong8
      vData = (c7x::ulong_vec)(pData[j]);

      // Update the quanta in all N ways
      __lookup_init(__LUT_SET1, __as_uint16(vData), rowOffset);

      pData     = (uint64_t *) &MATHLIB_LUTLow[0];
      rowOffset = 0;

      for (uint32_t i = 0; i < (tableSize / rowIncrement); i++) {
         // Read the quantum into a vector of ulong8
         vData = (c7x::ulong_vec)(pData[i]);

         // Update the quanta in all N ways
         __lookup_init(__LUT_SET1, __as_uint16(vData), rowOffset);

         // Increment the row_offset by 2 as we are initializing 2 32-bit elements at a time
         rowOffset += rowIncrement;
      }

#if MATHLIB_DEBUGPRINT
      // Print all LUT Values
      c7x::uint_vec indices1 = c7x::uint_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
      c7x::uint_vec indices2 = c7x::uint_vec(16, 17, 18, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
      printf("\nUpper Bits\n");
      MATHLIB_printVec(MATHLIB_LUTReadUpperBits(indices1));
      MATHLIB_printVec(MATHLIB_LUTReadUpperBits(indices2));
      printf("\nLower Bits\n");
      MATHLIB_printVec(MATHLIB_LUTReadLowerBits(indices1));
      MATHLIB_printVec(MATHLIB_LUTReadLowerBits(indices2));
#endif
   }
}

/** @}*/

#endif
