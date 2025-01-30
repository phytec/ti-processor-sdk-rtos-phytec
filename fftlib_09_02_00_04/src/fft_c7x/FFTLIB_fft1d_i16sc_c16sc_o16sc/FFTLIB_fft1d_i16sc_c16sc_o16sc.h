/*******************************************************************************
**+--------------------------------------------------------------------------+**
**|                            ****                                          |**
**|                            ****                                          |**
**|                            ******o***                                    |**
**|                      ********_///_****                                   |**
**|                      ***** /_//_/ ****                                   |**
**|                       ** ** (__/ ****                                    |**
**|                           *********                                      |**
**|                            ****                                          |**
**|                            ***                                           |**
**|                                                                          |**
**|        Copyright (c) 2017 Texas Instruments Incorporated                 |**
**|                                                                          |**
**|              All rights reserved not granted herein.                     |**
**|                                                                          |**
**|                         Limited License.                                 |**
**|                                                                          |**
**|  Texas Instruments Incorporated grants a world-wide, royalty-free,       |**
**|  non-exclusive license under copyrights and patents it now or            |**
**|  hereafter owns or controls to make, have made, use, import, offer to    |**
**|  sell and sell ("Utilize") this software subject to the terms herein.    |**
**|  With respect to the foregoing patent license, such license is granted   |**
**|  solely to the extent that any such patent is necessary to Utilize the   |**
**|  software alone.  The patent license shall not apply to any              |**
**|  combinations which include this software, other than combinations       |**
**|  with devices manufactured by or for TI ("TI Devices").  No hardware     |**
**|  patent is licensed hereunder.                                           |**
**|                                                                          |**
**|  Redistributions must preserve existing copyright notices and            |**
**|  reproduce this license (including the above copyright notice and the    |**
**|  disclaimer and (if applicable) source code license limitations below)   |**
**|  in the documentation and/or other materials provided with the           |**
**|  distribution                                                            |**
**|                                                                          |**
**|  Redistribution and use in binary form, without modification, are        |**
**|  permitted provided that the following conditions are met:               |**
**|                                                                          |**
**|    *  No reverse engineering, decompilation, or disassembly of this      |**
**|  software is permitted with respect to any software provided in binary   |**
**|  form.                                                                   |**
**|                                                                          |**
**|    *  any redistribution and use are licensed by TI for use only with    |**
**|  TI Devices.                                                             |**
**|                                                                          |**
**|    *  Nothing shall obligate TI to provide you with source code for      |**
**|  the software licensed and provided to you in object code.               |**
**|                                                                          |**
**|  If software source code is provided to you, modification and            |**
**|  redistribution of the source code are permitted provided that the       |**
**|  following conditions are met:                                           |**
**|                                                                          |**
**|    *  any redistribution and use of the source code, including any       |**
**|  resulting derivative works, are licensed by TI for use only with TI     |**
**|  Devices.                                                                |**
**|                                                                          |**
**|    *  any redistribution and use of any object code compiled from the    |**
**|  source code and any resulting derivative works, are licensed by TI      |**
**|  for use only with TI Devices.                                           |**
**|                                                                          |**
**|  Neither the name of Texas Instruments Incorporated nor the names of     |**
**|  its suppliers may be used to endorse or promote products derived from   |**
**|  this software without specific prior written permission.                |**
**|                                                                          |**
**|  DISCLAIMER.                                                             |**
**|                                                                          |**
**|  THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY      |**
**|  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE       |**
**|  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR      |**
**|  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL TI AND TI'S LICENSORS BE      |**
**|  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR     |**
**|  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF    |**
**|  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR         |**
**|  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,   |**
**|  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE    |**
**|  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,       |**
**|  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                      |**
**+--------------------------------------------------------------------------+**
*******************************************************************************/

#ifndef FFTLIB_FFT1D_I16SC_C16SC_O16SC_H_
#define FFTLIB_FFT1D_I16SC_C16SC_O16SC_H_ 1

#include "../../common/FFTLIB_types.h"
/*!
 * @defgroup FFTLIB_fft1d_i16sc_c16sc_o16sc FFTLIB_fft1d_i16sc_c16sc_o16sc
 * @brief Kernel for computing 16-bit integer FFT
 *
 * @details
 *      - The size of data is required to be a power of 2.
 *      - The kernel requires application to provide a pre-generated
 *        twiddle factors along with the input data.
 * @ingroup  FFT_C7X
 * @{
 */

/**
 * @brief Macro to define the array size for storing SE/SA parameters
 */

/* PBLOCK has to accommodate certain number of SE/SA configurations */
#define FFTLIB_FFT1D_I16SC_C16SC_O16SC_PBLOCK_SIZE (9 * SE_PARAM_SIZE)

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief       This function should be called before the
 *               @ref FFTLIB_fft1d_i16sc_c16sc_o16sc_kernel function is called.
 *               This function takes care of any one-time operations such as
 *               setting up the configuration for the streaming engine.
 *
 *  @param [in]  pX               :  Pointer to buffer with input data
 *  @param [in]  bufParamsX       :  Pointer to the structure containing
 *                                   dimensional information of input buffer
 *  @param [in]  pW               :  Pointer to buffer with twiddle factors
 *  @param [in]  bufParamsW       :  Pointer to the structure containing
 *                                   dimensional information of twiddle factor
 *                                   buffer
 *  @param [in]  pY               :  Pointer to buffer with output data
 *  @param [in]  bufParamsY       :  Pointer to the structure containing
 *                                   dimensional information of output buffer
 *  @param [in]  pShift           :  Pointer to buffer with shift value at every
 *                                   stage of FFT
 *  @param [in]  bufParamsShift   :  Pointer to the structure containing
 *                                   dimensional information of shift buffer
 *  @param [out] pBlock           :  Pointer to the buffer that will hold the
 *                                   streaming engine parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 *               FFTLIB_STATUS.
 *
 */

FFTLIB_STATUS
FFTLIB_fft1d_i16sc_c16sc_o16sc_init (int16_t *             pX,
                                     FFTLIB_bufParams1D_t *bufParamsX,
                                     int16_t *             pW,
                                     FFTLIB_bufParams1D_t *bufParamsW,
                                     int16_t *             pY,
                                     FFTLIB_bufParams1D_t *bufParamsY,
                                     uint32_t *            pShift,
                                     FFTLIB_bufParams1D_t *bufParamsShift,
                                     void *                pBlock);

/**
 *  @brief       This function is the main kernel compute function.
 *
 *  @param [in]  pX               :  Pointer to buffer with input data
 *  @param [in]  bufParamsX       :  Pointer to the structure containing
 *                                   dimensional information of input buffer
 *  @param [in]  pW               :  Pointer to buffer with twiddle factors
 *  @param [in]  bufParamsW       :  Pointer to the structure containing
 *                                   dimensional information of twiddle factor
 *                                   buffer
 *  @param [out] pY               :  Pointer to buffer with output data
 *  @param [in]  bufParamsY       :  Pointer to the structure containing
 *                                   dimensional information of output buffer
 *  @param [in]  pShift           :  Pointer to buffer with shift value at every
 *                                   stage of FFT
 *  @param [in]  bufParamsShift   :  Pointer to the structure containing
 *                                   dimensional information of shift buffer
 *  @param [in]  pBlock           :  Pointer to the buffer that will hold the
 *                                   streaming engine parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 *               FFTLIB_STATUS.
 *
 */

FFTLIB_STATUS
FFTLIB_fft1d_i16sc_c16sc_o16sc_kernel (int16_t *             pX,
                                       FFTLIB_bufParams1D_t *bufParamsX,
                                       int16_t *             pW,
                                       FFTLIB_bufParams1D_t *bufParamsW,
                                       int16_t *             pY,
                                       FFTLIB_bufParams1D_t *bufParamsY,
                                       uint32_t *            pShift,
                                       FFTLIB_bufParams1D_t *bufParamsShift,
                                       void *                pBlock);
/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref FFTLIB_fft1d_i16sc_c16sc_o16sc_init and
 *               @ref FFTLIB_fft1d_i16sc_c16sc_o16sc_kernel functions.
 *
 *  @param [in]  pX               :  Pointer to buffer with input data
 *  @param [in]  bufParamsX       :  Pointer to the structure containing
 *                                   dimensional information of input buffer
 *  @param [in]  pW               :  Pointer to buffer with twiddle factors
 *  @param [in]  bufParamsW       :  Pointer to the structure containing
 *                                   dimensional information of twiddle factor
 *                                   buffer
 *  @param [in]  pY               :  Pointer to buffer with output data
 *  @param [in]  bufParamsY       :  Pointer to the structure containing
 *                                   dimensional information of output buffer
 *  @param [in]  pShift           :  Pointer to buffer with shift value at every
 *                                   stage of FFT
 *  @param [in]  bufParamsShift   :  Pointer to the structure containing
 *                                   dimensional information of shift buffer
 *  @param [in]  pBlock           :  Pointer to the buffer that will hold the
 *                                   streaming engine parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 *               FFTLIB_STATUS.
 */

FFTLIB_STATUS FFTLIB_fft1d_i16sc_c16sc_o16sc_checkParams (
    int16_t *             pX,
    FFTLIB_bufParams1D_t *bufParamsX,
    int16_t *             pW,
    FFTLIB_bufParams1D_t *bufParamsW,
    int16_t *             pY,
    FFTLIB_bufParams1D_t *bufParamsY,
    uint32_t *            pShift,
    FFTLIB_bufParams1D_t *bufParamsShift,
    void *                pBlock);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
