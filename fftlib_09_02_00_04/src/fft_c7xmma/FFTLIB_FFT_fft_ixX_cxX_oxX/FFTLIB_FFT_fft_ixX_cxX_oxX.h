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
**|        Copyright (c) 2016 Texas Instruments Incorporated                 |**
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

#ifndef FFTLIB_FFT_FFT_IXX_CXX_OXX_H_
#define FFTLIB_FFT_FFT_IXX_CXX_OXX_H_

#include "../../common/FFTLIB_types.h"
#include "../../linalg_c7xmma/FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX/FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @defgroup FFTLIB_FFT_fft_ixX_cxX_oxX FFTLIB_FFT_fft_ixX_cxX_oxX
 * @brief Kernel for coordinating the DFT computation by calling one of
 *        @ref FFTLIB_FFT_dftSmall_ixX_cxX_oxX, @ref
 * FFTLIB_FFT_dftLarge_ixX_cxX_oxX or @ref
 * FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX kernels, depending on fftSize
 * and batchSize parameters
 * @details
 *        - If size of data is greater than MMA vector width, then the size is
 *          assumed to be a power of 2.
 *        - The kernel requires application to provide a pre-generated
 *          computational matrix along with the input data.
 * @ingroup  FFTLIB_FFT
 * @{
 */

/**
 * @brief Macro to define the number of bit-shifts that are to be provided
 *        for use in the internal DFT computation
 * @details The number of bit-shifts depends on the number of stages of
 *          computation. There would be 5 stages of computation when
 *          @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX kernel is
 *          called internally by this kernel for the DFT computation.
 */
#define FFTLIB_FFT_FFT_IXX_CXX_OXX_NUMSHIFTS 5

/**
 * @brief Structure containing the parameters for DFT computation
 */
typedef struct {
   /** @brief Variant of the function refer to @ref FFTLIB_FUNCTION_STYLE */
   int8_t funcStyle;
   /** @brief Size of each channel's data in terms of number of complex
    *         points. */
   uint32_t fftSize;
   /** @brief Size of the batch in terms of number of channels of DFT's */
   uint32_t batchSize;
   /** @brief Flag to indicate if the real and imaginary parts of data are
    *         interleaved or not. A value of 1 indicates interleaved data
    *         format, while a value of 0 indicates non-interleaved data
    *         format. */
   uint32_t interleave;
   /** @brief Array containing the bit-shift values to be used for
    *         internal computation.
    *         - For higher precision results, MMA accelerator uses 4 times
    *         the number of data bits for accumulating the intermediate
    *         computation results. And, the output data is generated by
    *         right-shifting the intermediate accumulated data by a certain
    *         number of bits. This array gives the right-shift amount in
    *         terms of number of bits. */
   uint32_t shiftVector[FFTLIB_FFT_FFT_IXX_CXX_OXX_NUMSHIFTS];
} FFTLIB_FFT_fft_ixX_cxX_oxX_InitArgs;

/**
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init
 * parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring
 * it.
 */
int32_t FFTLIB_FFT_fft_ixX_cxX_oxX_getHandleSize (
    FFTLIB_FFT_fft_ixX_cxX_oxX_InitArgs *pKerInitArgs);

/**
 *  @brief       This is a query function to calculate the sizes of input,
 * output and the computational matrix buffers
 *
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 * parameters
 *  @param [in]  bufParamsX   :  Pointer to the structure containing
 * dimensional information of input buffer
 *  @param [in]  bufParamsW   :  Pointer to the structure containing
 * dimensional information of computational matrix buffer
 *  @param [in]  bufParamsY   :  Pointer to the structure containing
 * dimensional information of ouput buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * FFTLIB_STATUS.
 *
 *  @remarks     Application is expected to allocate buffers of the requested
 *               size, fill in the input data and computational matrix data
 * before making the  @ref FFTLIB_FFT_fft_ixX_cxX_oxX_exec function call.
 */
FFTLIB_STATUS FFTLIB_FFT_fft_ixX_cxX_oxX_getSizes (
    const FFTLIB_FFT_fft_ixX_cxX_oxX_InitArgs *pKerInitArgs,
    FFTLIB_bufParams1D_t                      *bufParamsX,
    FFTLIB_bufParams1D_t                      *bufParamsW,
    FFTLIB_bufParams1D_t                      *bufParamsY);

/**
 *  @brief       This is a utility function that generates the computational
 * matrix (DFT matrix and twiddle factors) into the provided buffer.
 *
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 * parameters
 *  @param [in]  pW           :  Pointer to the buffer where the
 * computational matrix is generated into.
 *  @param [in]  bufParamsW   :  Pointer to the structure containing
 * dimensional information of computational matrix buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * FFTLIB_STATUS.
 *
 *  @remarks     pW must point to a buffer of size obtained from the call to
 *               @ref FFTLIB_FFT_fft_ixX_cxX_oxX_getSizes function.
 */
FFTLIB_STATUS FFTLIB_FFT_fft_ixX_cxX_oxX_twGen (
    const FFTLIB_FFT_fft_ixX_cxX_oxX_InitArgs *pKerInitArgs,
    void                                      *pW,
    const FFTLIB_bufParams1D_t                *bufParamsW);

/**
 *  @brief       This function should be called before the
 *               @ref FFTLIB_FFT_fft_ixX_cxX_oxX_exec function is called.
 * This function takes care of any one-time operations such as setting up the
 * configuration of required hardware resources such as the MMA accelerator
 * and the streaming engine.  The results of these operations are stored in
 * the handle.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsX   :  Pointer to the structure containing
 * dimensional information of input buffer
 *  @param [in]  bufParamsW   :  Pointer to the structure containing
 * dimensional information of compuatational matrix buffer
 *  @param [out] bufParamsY   :  Pointer to the structure containing
 * dimensional information of ouput buffer
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * FFTLIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle.
 */
FFTLIB_STATUS FFTLIB_FFT_fft_ixX_cxX_oxX_init (
    FFTLIB_kernelHandle                        handle,
    FFTLIB_bufParams1D_t                      *bufParamsX,
    FFTLIB_bufParams1D_t                      *bufParamsW,
    FFTLIB_bufParams1D_t                      *bufParamsY,
    const FFTLIB_FFT_fft_ixX_cxX_oxX_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed
 * to
 *               @ref FFTLIB_FFT_fft_ixX_cxX_oxX_init function. This function
 *               is called with the same parameters as the
 *               @ref FFTLIB_FFT_fft_ixX_cxX_oxX_init, and this function
 *               must be called before the
 *               @ref FFTLIB_FFT_fft_ixX_cxX_oxX_init is called.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsX   :  Pointer to the structure containing
 * dimensional information of input buffer
 *  @param [in]  bufParamsW   :  Pointer to the structure containing
 * dimensional information of computational matrix
 *  @param [out] bufParamsY   :  Pointer to the structure containing
 * dimensional information of output buffer
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * FFTLIB_STATUS.
 *
 *  @remarks     None
 */
FFTLIB_STATUS FFTLIB_FFT_fft_ixX_cxX_oxX_init_checkParams (
    FFTLIB_kernelHandle                        handle,
    const FFTLIB_bufParams1D_t                *bufParamsX,
    const FFTLIB_bufParams1D_t                *bufParamsW,
    const FFTLIB_bufParams1D_t                *bufParamsY,
    const FFTLIB_FFT_fft_ixX_cxX_oxX_InitArgs *pKerInitArgs);

/**
 *  @brief       This function is the main kernel compute function.
 *
 *  @details     Please refer to details under
 *               @ref FFTLIB_FFT_fft_ixX_cxX_oxX
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pX          : Pointer to buffer holding the input data
 *  @param [in]  pW          : Pointer to buffer holding the computational
 * matrix
 *  @param [out] pY          : Pointer to buffer holding the output data
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * FFTLIB_STATUS.
 *
 *  @par Assumptions:
 *    - None
 *
 *  @par Performance Considerations:
 *    For best performance,
 *    - the input and output data buffers are expected to be in L2 memory
 *    - the buffer pointers are assumed to be 64-byte aligned
 *
 *  @remarks     Before calling this function, application is expected to
 * call
 *               @ref FFTLIB_FFT_fft_ixX_cxX_oxX_init and
 *               @ref FFTLIB_FFT_fft_ixX_cxX_oxX_exec_checkParams functions.
 *               This ensures resource configuration and error checks are
 * done only once for several invocations of this function.
 */
FFTLIB_STATUS FFTLIB_FFT_fft_ixX_cxX_oxX_exec (FFTLIB_kernelHandle handle,
                                               void *restrict pX,
                                               void *restrict pW,
                                               void *restrict pY);

/**
 *  @brief       This function checks the validity of the parameters passed
 * to
 *               @ref FFTLIB_FFT_fft_ixX_cxX_oxX_exec function. This function
 *               is called with the same parameters as the
 *               @ref FFTLIB_FFT_fft_ixX_cxX_oxX_exec, and this function
 *               must be called before the
 *               @ref FFTLIB_FFT_fft_ixX_cxX_oxX_exec is called.
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pX          : Pointer to buffer holding the input data
 *  @param [in]  pW          : Pointer to buffer holding the computational
 * matrix
 *  @param [out] pY          : Pointer to buffer holding the output matrix
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * FFTLIB_STATUS.
 *
 *  @remarks     None
 */
FFTLIB_STATUS
FFTLIB_FFT_fft_ixX_cxX_oxX_exec_checkParams (FFTLIB_kernelHandle handle,
                                             const void *restrict pX,
                                             const void *restrict pW,
                                             const void *restrict pY);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _FFTLIB_FFT_FFT_IXX_IXX_OXX_H_ */

/* ======================================================================== */
/*  End of file:  FFTLIB_FFT_FFT_ixX_cxX_oxX.h                   */
/* ======================================================================== */
