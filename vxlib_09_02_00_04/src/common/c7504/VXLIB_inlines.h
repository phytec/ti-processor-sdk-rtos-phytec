/******************************************************************************/
/*!
 * \file
 */
/* Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
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
#ifndef C7504_INLINES_H
#define C7504_INLINES_H

/*******************************************************************************
 *
 * Inlined functions
 *
 ******************************************************************************/

/* Horizontal Minimum */

template <typename U, typename V> static inline U VXLIB_horizontalMin(V vecIn);

template <> inline uint8_t VXLIB_horizontalMin(c7x::uchar_vec vecIn)
{
   c7x::uchar_hvec vecMin1 = __min(vecIn.hi(), vecIn.lo());
   c7x::ushort_vec vecMin2 = c7x::convert<c7x::ushort_vec, c7x::uchar_hvec>(vecMin1);
   c7x::ushort_vec vecSort = __sort_asc(vecMin2);
   uint16_t        minVal  = vecSort.s[0];
   uint8_t         minOut  = (uint8_t) (minVal);
   return minOut;
}

template <> inline int8_t VXLIB_horizontalMin(c7x::char_vec vecIn)
{
   c7x::char_hvec vecMin1 = __min(vecIn.hi(), vecIn.lo());
   c7x::short_vec vecMin2 = c7x::convert<c7x::short_vec, c7x::char_hvec>(vecMin1);
   c7x::short_vec vecSort = __sort_asc(vecMin2);
   int16_t        minVal  = vecSort.s[0];
   //  int8_t         minOut  = (int8_t) ((uint16_t) minVal & 0x00FFu);
   int8_t minOut = (int8_t) (minVal);
   return minOut;
}

template <> inline uint16_t VXLIB_horizontalMin(c7x::ushort_vec vecIn)
{
   c7x::ushort_vec vecSort = __sort_asc(vecIn);
   uint16_t        minOut  = vecSort.s[0];
   return minOut;
}

template <> inline int16_t VXLIB_horizontalMin(c7x::short_vec vecIn)
{
   c7x::short_vec vecSort = __sort_asc(vecIn);
   int16_t        minOut  = vecSort.s[0];
   return minOut;
}



/* Horizontal Maximum */
template <typename U, typename V> static inline U VXLIB_horizontalMax(V vecIn);

template <> inline uint8_t VXLIB_horizontalMax(c7x::uchar_vec vecIn)
{
   c7x::uchar_hvec vecMax1 = __max(vecIn.hi(), vecIn.lo());
   c7x::ushort_vec vecMax2 = c7x::convert<c7x::ushort_vec, c7x::uchar_hvec>(vecMax1);
   c7x::ushort_vec vecSort = __sort_desc(vecMax2);
   uint16_t        maxVal  = vecSort.s[0];
   uint8_t maxOut = (uint8_t)(maxVal);
   return maxOut;
}

template <> inline int8_t VXLIB_horizontalMax(c7x::char_vec vecIn)
{
   c7x::char_hvec vecMax1 = __max(vecIn.hi(), vecIn.lo());
   c7x::short_vec vecMax2 = c7x::convert<c7x::short_vec, c7x::char_hvec>(vecMax1);
   c7x::short_vec vecSort = __sort_desc(vecMax2);
   int16_t        maxVal  = vecSort.s[0];
   int8_t maxOut = (int8_t)(maxVal);
   return maxOut;
}

template <> inline uint16_t VXLIB_horizontalMax(c7x::ushort_vec vecIn)
{
   c7x::ushort_vec vecSort = __sort_desc(vecIn);
   uint16_t        maxOut  = vecSort.s[0];
   return maxOut;
}

template <> inline int16_t VXLIB_horizontalMax(c7x::short_vec vecIn)
{
   c7x::short_vec vecSort = __sort_desc(vecIn);
   int16_t        maxOut  = vecSort.s[0];
   return maxOut;
}


/******************************************************************************/

#endif
