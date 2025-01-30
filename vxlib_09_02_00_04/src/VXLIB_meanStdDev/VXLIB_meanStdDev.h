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

#ifndef VXLIB_MEANSTDDEV_IXX_IXX_OXX_H_
#define VXLIB_MEANSTDDEV_IXX_IXX_OXX_H_

#include "../common/VXLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup VXLIB_KERNELS
 */

/**
 * @defgroup VXLIB_meanStdDev VXLIB_meanStdDev
 * @brief Kernel for computing the mean and standard deviation
 * of input vector
 *
 *
 * @details
 *  Performs mean and standard deviation computation of input vector.
 *  The following datatype combinations are supported:
 *
 *  | Case  | Input    | Output0 | Output1 |
 *  | :---: | :----:   | :----:  | :----:  |
 *  |   0   | uint8_t  | float   | float   |
 *  |   1   | uint16_t | float   | float   |
 *
 *
 * @par Method
 * @verbatim

    dst0 = (sum( src(x,y) )) / N
    dst1 = sqrt( sum( src(x,y) - dst0 )^2 / N)

   @endverbatim
 *
 * @ingroup  VXLIB
 */

/**@{*/

/**
 * @brief Structure containing the parameters to initialize the kernel
 */

typedef struct {
   /** @brief Variant of the function, refer to @ref VXLIB_FUNCTION_STYLE     */
   int8_t funcStyle;
} VXLIB_meanStdDev_InitArgs;

/**
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */

int32_t VXLIB_meanStdDev_getHandleSize(VXLIB_meanStdDev_InitArgs *pKerInitArgs);

/**
 *  @brief       This function should be called before the
 *               @ref VXLIB_meanStdDev_exec function is called. This
 *               function takes care of any one-time operations such as setting
 * up the configuration of required hardware resources such as the streaming engine.  The results of these
 *               operations are stored in the handle.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn :  Pointer to the structure containing dimensional
 *                               information of input image
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 *                               parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref VXLIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle.
 */

VXLIB_STATUS VXLIB_meanStdDev_init(VXLIB_kernelHandle               handle,
                                   VXLIB_bufParams2D_t             *bufParamsIn,
                                   const VXLIB_meanStdDev_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref VXLIB_meanStdDev_init function. This function
 *               is called with the same parameters as the
 *               @ref VXLIB_meanStdDev_init, and this function
 *               must be called before the
 *               @ref VXLIB_meanStdDev_init is called.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn :  Pointer to the structure containing dimensional
 *                               information of input image
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 *                               parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref VXLIB_STATUS.
 *
 *
 *  @remarks     None
 */

VXLIB_STATUS
VXLIB_meanStdDev_init_checkParams(VXLIB_kernelHandle               handle,
                                  const VXLIB_bufParams2D_t       *bufParamsIn,
                                  const VXLIB_meanStdDev_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref VXLIB_meanStdDev_exec function. This function
 *               is called with the same parameters as the
 *               @ref VXLIB_meanStdDev_init, and this function
 *               must be called before the
 *               @ref VXLIB_meanStdDev_init is called.
 *
 *  @param [in]  handle :  Active handle to the kernel
 *  @param [in]  pIn   :  Pointer to the structure input image
 *  @param [in,out] pPixelsProcessed  : Pointer to the structure number of pixels processed
 *  @param [in,out] pCurrentSum  : Pointer to the structure current pixel sum
 *  @param [in,out] pCurrentSqSum   : Pointer to the structure current pixel square sum
 *  @param [in]  pOut0   :  Pointer to the structure output buffer
 *  @param [out] pOut1   :  Pointer to the output buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref VXLIB_STATUS.
 *
 *  @remarks     None
 */

VXLIB_STATUS VXLIB_meanStdDev_exec_checkParams(VXLIB_kernelHandle handle,
                                               const void *restrict pIn,
                                               const void *restrict pOut0,
                                               const void *restrict pOut1,
                                               const void *restrict pPixelsProcessed,
                                               const void *restrict pCurrentSum,
                                               const void *restrict pCurrentSqSum

);

/**
 *  @brief       This function is the main kernel compute function.
 *
 *  @details     Please refer to details under
 *               @ref VXLIB_meanStdDev_exec
 *
 *  @param [in]  handle     :  Active handle to the kernel
 *  @param [in]  pIn        :  Pointer to the structure input image
 *  @param [in, out]  pPixelsProcessed  :  Pointer to the structure number of pixels processed
 *  @param [in,out]  pCurrentSum  :   Pointer to the structure current pixel sum
 *  @param [in,out]  pCurrentSqSum : Pointer to the structure current pixel squared sum
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
 *               @ref VXLIB_meanStdDev_init and
 *               @ref VXLIB_meanStdDev_exec_checkParams functions.
 *               This ensures resource configuration and error checks are done
 * only once for several invocations of this function.
 */

VXLIB_STATUS
VXLIB_meanStdDev_exec(VXLIB_kernelHandle handle,
                      void *restrict pIn,
                      void *restrict pOut0,
                      void *restrict pOut1,
                      void *restrict pPixelsProcessed,
                      void *restrict pCurrentSum,
                      void *restrict pCurrentSqSum);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* VXLIB_MEANSTDDEV_IXX_IXX_OXX_H_ */
