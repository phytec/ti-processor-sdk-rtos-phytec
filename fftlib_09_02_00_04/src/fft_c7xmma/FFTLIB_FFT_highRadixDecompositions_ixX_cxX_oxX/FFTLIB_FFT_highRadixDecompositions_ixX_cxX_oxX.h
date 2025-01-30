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

#ifndef FFTLIB_FFT_HIGHRADIXDECOMPOSITIONS_IXX_CXX_OXX_H_
#define FFTLIB_FFT_HIGHRADIXDECOMPOSITIONS_IXX_CXX_OXX_H_

#include "../../common/FFTLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @defgroup FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX
 * FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX
 * @brief Kernel for computing DFT by using a high-radix variant of the
 *        FFT alogrithm.
 * @details
 *          - The size of data for each channel in the batch is assumed to be
 *          greater than or equal to (MMA vector width)*2, and is assumed to
 *          be a power of 2.
 *          - The kernel requires application to provide a pre-generated
 *          computational matrices along with the input data.<br>
 *          - The algorithm works by decomposing DFT computation to smaller
 *          size DFT's stage after stage. In all but the first stage, the
 *          decomposition factor is (MMA vector width)/2. This decomposition
 *          factor is chosen so that each row of the decomposition matrix would
 *          fit one full vector length. The first stage would also use a
 *          decomposition factor of (MMA vector width)/2 if the data size
 * (fftSize) is a power of (MMA vector width)/2. In case the data size is not a
 *          power of (MMA vector width)/2, the decomposition factor for the
 * first stage is less than (MMA vector width)/2.
 * @ingroup  FFTLIB_FFT
 * @{
 */

/**
 * @brief Macro to define the number of bit-shifts that are to be provided
 *        for use in the internal computation
 * @details The number of bit-shifts depends on the number of stages of
 *          computation - that is number of decomposition stages. Assuming
 *          a maximum data size of 32768 in case of 32-bit data type, maximum
 *          number of computation stages would be 5. (log(32768) base 8)
 */
#define FFTLIB_FFT_HIGHRADIXDECOMPOSITIONS_IXX_CXX_OXX_NUMSHIFTS 5

/**
 * @brief Structure containing the parameters for DFT computation
 */
typedef struct {
   /** @brief Variant of the function refer to @ref FFTLIB_FUNCTION_STYLE     */
   int8_t funcStyle;
   /** @brief Size of each channel's data in terms of number of complex
    *         points.                                                         */
   uint32_t fftSize;
   /** @brief Size of the batch in terms of number of channels of DFT's       */
   uint32_t batchSize;
   /** @brief Flag to indicate if the real and imaginary parts of data are
    *         interleaved or not. A value of 1 indicates interleaved data
    *         format, while a value of 0 indicates non-interleaved data
    *         format.                                                         */
   uint32_t interleave;
   /** @brief Array containing the bit-shift values to be used for
    *         internal computation.
    *         - For higher precision results, MMA accelerator uses 4 times
    *         the number of data bits for accumulating the intermediate
    *         computation results. And, the output data is generated by
    *         right-shifting the intermediate accumulated data by a certain
    *         number of bits. This array gives the right-shift amount in
    *         terms of number of bits.                                        */
   uint32_t
       shiftVector[FFTLIB_FFT_HIGHRADIXDECOMPOSITIONS_IXX_CXX_OXX_NUMSHIFTS];
} FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs;

/**
 * @brief Array that needs to be allocated in L1D to hold part of the
 * computational matrix data.
 */
extern uint8_t FFTLIB_FFT_twData[FFTLIB_MMA_SIZE_16_BIT *
                                 FFTLIB_MMA_SIZE_16_BIT * (uint32_t) 3 *
                                 sizeof (int16_t)];

/**
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */
int32_t FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_getHandleSize (
    FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs *pKerInitArgs);

/**
 *  @brief       This is a query function to calculate the sizes of input,
 *               output and the computational matrices'(DFT and twiddle factor
 *               matrix) buffers
 *
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 *                               parameters
 *  @param [in]  bufParamsX   :  Pointer to the structure containing dimensional
 *                               information of input buffer
 *  @param [in]  bufParamsW   :  Pointer to the structure containing dimensional
 *                               information of computational matrices' buffer
 *  @param [in]  bufParamsY   :  Pointer to the structure containing dimensional
 *                               information of ouput buffer
 *
 *  @return      Status value indicating success or failure. Refer to
 *               @ref FFTLIB_STATUS.
 *
 *  @remarks     Application is expected to allocate buffers of the requested
 *               size, fill in the input data and computational matrices' data
 *               before making the
 *               @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec
 *               function call.
 */
FFTLIB_STATUS FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_getSizes (
    const FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs *pKerInitArgs,
    FFTLIB_bufParams1D_t                                          *bufParamsX,
    FFTLIB_bufParams1D_t                                          *bufParamsW,
    FFTLIB_bufParams1D_t                                          *bufParamsY);

/**
 *  @brief       This is a utility function that generates the computational
 *               matrices (DFT and twiddle factor matrices) into the provided
 * buffer. Part of the twiddle factors and DFT matrices are generated into the
 *               @ref FFTLIB_FFT_twData buffer that must be mapped to L1D memory
 * for optimal performance. Note that this buffer is different from the buffer
 * pointed to by the pW argument that is passed to this function.
 *
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 * parameters
 *  @param [in]  pW           :  Pointer to the buffer where the computational
 * matrices are generated into.
 *  @param [in]  bufParamsW   :  Pointer to the structure containing dimensional
 *                               information of computational matrices' buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * FFTLIB_STATUS.
 *
 *  @remarks     pW must point to a buffer of size obtained from the call to
 *               @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_getSizes
 * function.
 */
FFTLIB_STATUS FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_twGen (
    const FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs *pKerInitArgs,
    void                                                          *pW,
    const FFTLIB_bufParams1D_t                                    *bufParamsW);

/**
 *  @brief       This function should be called before the
 *               @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec
 *               function is called. This function takes care of any one-time
 *               operations such as setting up the configuration of required
 *               hardware resources such as the MMA accelerator and the
 *               streaming engine.  The results of these operations are stored
 *               in the handle.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsX   :  Pointer to the structure containing dimensional
 *                               information of input buffer
 *  @param [in]  bufParamsW   :  Pointer to the structure containing dimensional
 *                               information of computational matrices' buffer
 *  @param [out] bufParamsY   :  Pointer to the structure containing dimensional
 *                               information of ouput buffer
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * FFTLIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle.
 */
FFTLIB_STATUS FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_init (
    FFTLIB_kernelHandle                                      handle,
    FFTLIB_bufParams1D_t                                    *bufParamsX,
    FFTLIB_bufParams1D_t                                    *bufParamsW,
    FFTLIB_bufParams1D_t                                    *bufParamsY,
    FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_init
 * function. This function is called with the same parameters as the
 *               @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_init, and
 * this function must be called before the
 *               @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_init is
 * called.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsX   :  Pointer to the structure containing dimensional
 *                               information of input buffer
 *  @param [in]  bufParamsW   :  Pointer to the structure containing dimensional
 *                               information of computational matrices'
 *  @param [out] bufParamsY   :  Pointer to the structure containing dimensional
 *                               information of output buffer
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * FFTLIB_STATUS.
 *
 *  @remarks     None
 */
FFTLIB_STATUS FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_init_checkParams (
    FFTLIB_kernelHandle         handle,
    const FFTLIB_bufParams1D_t *bufParamsX,
    const FFTLIB_bufParams1D_t *bufParamsW,
    const FFTLIB_bufParams1D_t *bufParamsY,
    const FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs
        *pKerInitArgs);

/**
 *  @brief       This function is the main kernel compute function.
 *
 *  @details     Please refer to details under
 *               @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pX          : Pointer to buffer holding the input data
 *  @param [in]  pW          : Pointer to buffer holding the computational
 * matrices
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
 *  @remarks     Before calling this function, application is expected to call
 *               @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_init and
 *               @ref
 * FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec_checkParams functions.
 *               This ensures resource configuration and error checks are done
 * only once for several invocations of this function.
 */
FFTLIB_STATUS
FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec (FFTLIB_kernelHandle handle,
                                                     void *restrict pX,
                                                     void *restrict pW,
                                                     void *restrict pY);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec
 * function. This function is called with the same parameters as the
 *               @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec, and
 * this function must be called before the
 *               @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec is
 * called.
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pX          : Pointer to buffer holding the input data
 *  @param [in]  pW          : Pointer to buffer holding the computational
 * matrices
 *  @param [out] pY          : Pointer to buffer holding the output matrix
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * FFTLIB_STATUS.
 *
 *  @remarks     None
 */
FFTLIB_STATUS FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec_checkParams (
    FFTLIB_kernelHandle handle,
    const void *restrict pX,
    const void *restrict pW,
    const void *restrict pY);

/**
 *  @brief       This is a utility function that gives an estimate of the cycles
 *               consumed for the kernel execution.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsX   :  Pointer to the structure containing dimensional
 *                               information of pX
 *  @param [in]  bufParamsY   :  Pointer to the structure containing dimensional
 *                               information of pY
 *  @param [in]  bufParamsW   :  Pointer to the structure containing dimensional
 *                               information of pW
 *  @param [in] fftSize       :  Size of each channel's data in terms of number
 *                               of complex points
 *  @param [in] batchSize     :  Size of the batch in terms of number of
 * channels of DFT's
 *  @param [in] interleave    :  Flag to indicate if the real and imaginary
 * parts of data are interleaved or not.
 *  @param [out] archCycles   :  Cycles estimated for the compute, startup and
 *                               teardown
 *  @param [out] estCycles    :  Cycles estimated for the compute, startup,
 *                               teardown and any associated overhead
 *  @remarks     None
 */
void FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_perfEst (
    FFTLIB_kernelHandle         handle,
    const FFTLIB_bufParams1D_t *bufParamsX,
    const FFTLIB_bufParams1D_t *bufParamsY,
    const FFTLIB_bufParams1D_t *bufParamsW,
    uint32_t                    fftSize,
    uint32_t                    batchSize,
    uint32_t                    interleave,
    uint64_t                   *archCycles,
    uint64_t                   *estCycles);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* FFTLIB_FFT_HIGHRADIXDECOMPOSITIONS_IXX_CXX_OXX_H_ */

/* ======================================================================== */
/*  End of file:  FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX.h */
/* ======================================================================== */
