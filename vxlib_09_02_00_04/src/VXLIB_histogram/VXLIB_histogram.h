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

#ifndef VXLIB_HISTOGRAM_IXX_IXX_OXX_H_
#define VXLIB_HISTOGRAM_IXX_IXX_OXX_H_

#include "../common/VXLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup VXLIB_KERNELS
 */

/**
 * @defgroup VXLIB_histogram VXLIB_histogram
 * @brief Kernel for creating a histogram distribution from an image based on a given given offset, range, and number of
 * bins.
 *
 *
 * @details
 *  Generates a histogram distribution from an image. This kernel counts the number of occurrences of each pixel value
 *  within the window size of a pre-calculated number of bins.
 *  The following datatype combinations are supported:
 *
 *  | Case    | Input   | Output   |
 *  |  ----:  | :----:  | :----    |
 *  |    0    | uint8_t | uint32_t |
 *
 *
 * @par Method
 * @verbatim

     if ((offset <= pixel) && (pixel < (offset + range))) {
            index = ((pixel - offset) * numBins) / range;
            histogramBin[index]++;
      }

   @endverbatim
 *
 * @par Assumptions
 *    - Width of image must be less than 1024 bytes, due to default size of bufPredicateStore buffer
 *    - numBins should be <= 256
 *    - (offset + range) should be <= 256
 *    - L1D memory is initialized to zero, and execution of histogram function will continue to populate L1D memory
 *      until lastBlock = 1, when the data will be stored into output buffer
 * @ingroup  VXLIB
 */

/**@{*/

/**
 * @brief Structure containing the parameters to initialize the kernel
 */

typedef struct {
   /** @brief Variant of the function, refer to @ref VXLIB_FUNCTION_STYLE     */
   int8_t funcStyle;
   /** @brief Parameter indicating distribution offset */
   uint8_t offset;
   /** @brief Parameter indicating distribution range (<= 256) */
   uint16_t range;
   /** @brief 	Parameter indicating distribution number of bins (<= 256) */
   uint16_t numBins;
   /** @brief Flag that indicates if the function call is the final call for the image (0: intermediate call, 1: Final
    * call) */
   uint8_t lastBlock;
} VXLIB_histogram_InitArgs;

/**
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */

int32_t VXLIB_histogram_getHandleSize(VXLIB_histogram_InitArgs *pKerInitArgs);

/**
 *  @brief        This is a helper function to help clear the values stored in the LUT histogram
 *  @param [in]   numBins  : Number of bins in distribution
 *  @remarks      Data will accumulate in histogram between multiple calls of exec() function if init is not called.
 *                This function can be used to reset the histogram values to 0 without reinitalization of SE and LUT
 *                histogram parameters.
 */

void VXLIB_histogram_val_init(uint16_t numBins);

/**
 *  @brief       This function should be called before the
 *               @ref VXLIB_histogram_exec function is called. This
 *               function takes care of any one-time operations such as setting
 * up the configuration of required hardware resources such as the streaming engine and LUT histogram.  The results of
 * these operations are stored in the handle.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn :  Pointer to the structure containing dimensional
 *                               information of input image
 *  @param [out] bufParamsOut :  Pointer to the structure containing dimensional
 *                               information of output image
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 *                               parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref VXLIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle.
 */

VXLIB_STATUS VXLIB_histogram_init(VXLIB_kernelHandle              handle,
                                  VXLIB_bufParams2D_t            *bufParamsIn,
                                  VXLIB_bufParams1D_t            *bufParamsOut,
                                  const VXLIB_histogram_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref VXLIB_histogram_init function. This function
 *               is called with the same parameters as the
 *               @ref VXLIB_histogram_init, and this function
 *               must be called before the
 *               @ref VXLIB_histogram_init is called.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn :  Pointer to the structure containing dimensional
 *                               information of input image 0
 *  @param [out] bufParamsOut :  Pointer to the structure containing dimensional
 *                               information of output buffer
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 *                               parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref VXLIB_STATUS.
 *
 *
 *  @remarks     None
 */

VXLIB_STATUS
VXLIB_histogram_init_checkParams(VXLIB_kernelHandle              handle,
                                 const VXLIB_bufParams2D_t      *bufParamsIn,
                                 const VXLIB_bufParams1D_t      *bufParamsOut,
                                 const VXLIB_histogram_InitArgs *pKerInitArgs);
/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref VXLIB_histogram_exec function. This function
 *               is called with the same parameters as the
 *               @ref VXLIB_histogram_init, and this function
 *               must be called before the
 *               @ref VXLIB_histogram_init is called.
 *
 *  @param [in]  handle :  Active handle to the kernel
 *  @param [in]  pIn   :  Pointer to the structure input image
 *  @param [out] pOut   :  Pointer to the output buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref VXLIB_STATUS.
 *
 *  @remarks     None
 */

VXLIB_STATUS
VXLIB_histogram_exec_checkParams(VXLIB_kernelHandle handle, const void *restrict pIn, const void *restrict pOut);

/**
 *  @brief       This function is the main kernel compute function.
 *
 *  @details     Please refer to details under
 *               @ref VXLIB_histogram_exec
 *
 *  @param [in]  handle :  Active handle to the kernel
 *  @param [in]  pIn   :  Pointer to the structure input image
 *  @param [out] pOut   :  Pointer to the output image
 *
 *  @return      Status value indicating success or failure. Refer to @ref VXLIB_STATUS.
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
 *               @ref VXLIB_histogram_init and
 *               @ref VXLIB_histogram_exec_checkParams functions.
 *               This ensures resource configuration and error checks are done
 * only once for several invocations of this function.
 */

VXLIB_STATUS
VXLIB_histogram_exec(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

void VXLIB_histogram_perfEst(VXLIB_kernelHandle handle, size_t *archCycles, size_t *estCycles);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* VXLIB_HISTOGRAM_IXX_IXX_OXX_H_ */
