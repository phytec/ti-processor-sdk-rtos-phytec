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
#ifndef C71_MMA_INLINES_H
#define C71_MMA_INLINES_H

#include "../FFTLIB_types.h"
#if defined(FFTLIB_DEBUGPRINT)
#include "FFTLIB_debug.h"
#endif
#include <float.h>  // for max float, double values
#include <limits.h> // for min, max integer values

#include "FFTLIB_UTIL_SE0Bload.h"

/*******************************************************************************
 *
 * Inlined functions
 *
 ******************************************************************************/

/******************************************************************************/

#include "FFTLIB_UTIL_SA0Cstore.h"
#include "FFTLIB_UTIL_SA2CloadSA0Cstore.h"
#include "FFTLIB_UTIL_SA2CloadSE1Bload.h"
#include "FFTLIB_UTIL_SE0AloadComputeC.h"
#include "FFTLIB_UTIL_SE0AloadComputeCSA0Cstore.h"
#include "FFTLIB_UTIL_SE0AloadSE1BloadComputeC.h"
#include "FFTLIB_UTIL_SE0AloadSE1BloadComputeCSA0Cstore.h"
#include "FFTLIB_UTIL_SE0Bload.h"
#include "FFTLIB_UTIL_SE1Bload.h"
#include "FFTLIB_UTIL_zeroBload.h"
#endif
