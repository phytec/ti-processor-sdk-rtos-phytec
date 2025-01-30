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

#ifndef C7504_FFTLIB_TEST_H_
#define C7504_FFTLIB_TEST_H_ 1

/*******************************************************************************
 *
 * INCLUDES
 *
 ******************************************************************************/

// FFTLIB_TEST_compareDynamicRange uses some natural-c utilities from FFTLIB
#include "../src/common/FFTLIB_utility.h"

/*******************************************************************************
 *
 * External variables
 *
 ******************************************************************************/

/*******************************************************************************
 *
 * Inline functions
 *
 ******************************************************************************/

/*******************************************************************************
 *
 * C++ function prototypes
 *
 ******************************************************************************/
#ifdef __cplusplus
// functions only implemented in C++.  Could potentially develop a wrapper for C
// linkage if necessary

#endif //__cplusplus

/*******************************************************************************
 *
 * C function prototypes
 *
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

uint8_t FFTLIB_TEST_compareDynamicRange(void    *dynamicRangeRef,
                                        int64_t  dynamicRangeComputed,
                                        uint32_t outputType,
                                        uint8_t  shift,
                                        uint8_t  activationType);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* C7504_FFTLIB_TEST_H_ */
