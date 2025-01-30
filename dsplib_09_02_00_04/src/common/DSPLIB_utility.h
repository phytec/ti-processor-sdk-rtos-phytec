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

#ifndef COMMON_DSPLIB_UTILITY_H_
#define COMMON_DSPLIB_UTILITY_H_ 1

/*******************************************************************************
 *
 * INCLUDES
 *
 ******************************************************************************/

#include <float.h>  // for max float, double values
#include <limits.h> // for min, max integer values
#include <math.h>

#include "DSPLIB_bufParams.h"
#include "DSPLIB_types.h"

/*******************************************************************************
 *
 * EXTERNAL VARIABLES
 *
 ******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif                     /* __cplusplus */
extern uint64_t beg_count; /* Begin cycle count for profiling */
extern uint64_t end_count; /* End cycle count for profiling */
extern uint64_t overhead;  /* Cycle profiling overhead */
#ifdef __cplusplus
}
#endif /* __cplusplus */

/*******************************************************************************
 *
 * Definition and arithmetic for DSPLIB_int128_t class
 *
 ******************************************************************************/

// Define a 128-bit integer class to allow natural-c implementations of DSPLIB
// 32-bit input/output functions to be templated.  The class is implemented in
// a header file for easy sharing.  All member functions, including constructors
// are declared inline for two reasons: (1) performance and (2) necessary for
// implementing the class in a multiple-inclusion header file.

class DSPLIB_int128_t {
 public:
   int64_t hi;
   int64_t lo;
   DSPLIB_int128_t(int64_t h,
                   int64_t l); // constructor for both high and low specified
   DSPLIB_int128_t(int64_t l); // constructor for just low specified (sign extends to high)
   DSPLIB_int128_t();          // constructor for neither field specified
   DSPLIB_int128_t operator+(const DSPLIB_int128_t &) const; // operator +
   DSPLIB_int128_t operator>>(const int8_t &) const;         // operator >>
};

// define constructor
inline DSPLIB_int128_t::DSPLIB_int128_t(int64_t h, int64_t l)
{
   hi = h;
   lo = l;
}

// define constructor
inline DSPLIB_int128_t::DSPLIB_int128_t(int64_t l)
{
   // sign extend l
   hi = (((uint64_t) l & 0x8000000000000000ULL) != 0LL) ? (int64_t) 0xffffffffffffffffULL
                                                        : (int64_t) 0x0000000000000000ULL;
   lo = l;
}

// define constructor
inline DSPLIB_int128_t::DSPLIB_int128_t()
{
   hi = 0x0000000000000000LL;
   lo = 0x0000000000000000LL;
}

#endif
