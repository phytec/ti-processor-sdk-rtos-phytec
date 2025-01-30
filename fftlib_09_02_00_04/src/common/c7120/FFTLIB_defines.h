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

#ifndef C7120_FFTLIB_DEFINES_H_
#define C7120_FFTLIB_DEFINES_H_ 1


/*******************************************************************************
*
* SOC HARDWARE DEFINES
*
******************************************************************************/

#define FFTLIB_SOC_MSMC 1                             //!< SOC supports MSMC memory


/*******************************************************************************
*
* MMA HARDWARE DEFINES
*
******************************************************************************/

#define FFTLIB_MMA_CONFIG_REG __HWA_CONFIG_REG_v1
#define FFTLIB_MMA_ARF 1                           //<! Does the MMA implement an A register file (ARF)
#define FFTLIB_MMA_NUM_ARF_ROWS 64

#define FFTLIB_NONZER0_PADFILL 1

#define MMA_INDEPENDENT_B_ROW_COUNTERS 1
#define MMA_PSAT_ACTIVATION_SUPPORTED 1


/*******************************************************************************
*
* FFTLIB SOFTWARE DEFINES
*
******************************************************************************/

// this should be deprecated
#define ENABLE_DYN_RANGE_CHECKING 1
// in favor of this
#define FFTLIB_ENABLE_DYNAMIC_RANGE_TRACKING 1

#define FFTLIB_CONVOLVE_COL_IN_KERNEL_PADDING 1


/*******************************************************************************
*
* Function prototypes
*
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */






#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* C7120_FFTLIB_DEFINES_H_ */

