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

#ifndef VXLIB_MEDIAN_IXX_IXX_OXX_H_
#define VXLIB_MEDIAN_IXX_IXX_OXX_H_

#include "../common/VXLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup VXLIB_KERNELS
 */

/**
 * @defgroup VXLIB_median VXLIB_median
 * @brief Kernel for computing the median pixel over a window of imput image, for each pixel.
 *
 *
 * @details
 *  Computes the median pixel over a window of imput image, for each pixel.
 *  This kernel supports two types on implementaions based on kernel size i.e. 3 X 3 and M X N.
 *   - For 3 X 3 Kernel set @ref VXLIB_median_InitArgs.kernelType = false.
 *   - For M X N Kernel set @ref VXLIB_median_InitArgs.kernelType = true.
 * 
 *   A 3 X 3 implementaion takes all 9 values present in the kernel, sorts them in ascending order
 *   and returns the middle i.e. 5th value as median value and the kernel is moved forward
 *   with the stride of 1.
 *
 *   A M X N implementaion takes values corresponding to the true values in MASK kernel of size M X N, sorts them in ascending order
 *   and returns the median and the kernel is moved forward with the stride of 1.
 *
 *   OpenVX Function Reference Page : https://www.khronos.org/registry/vx/specs/1.1/html/d3/d77/group__group__vision__function__median__image.html
 * 
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
   /** @brief  To select between 3 X 3 OR M X N kernel size
    *          0 for 3 X 3
    *          1 for M X N
    */
   bool kernelType;
} VXLIB_median_InitArgs;

/**
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */

int32_t VXLIB_median_getHandleSize(VXLIB_median_InitArgs *pKerInitArgs);

/**
 *  @brief       This function should be called before the
 *               @ref VXLIB_median_exec function is called. This
 *               function takes care of any one-time operations such as setting
 * up the configuration of required hardware resources such as the streaming engine.  The results of these
 *               operations are stored in the handle.
 *
 *  @param [in]  handle        :  Active handle to the kernel
 *  @param [in]  bufParamsIn   :  Pointer to the structure containing dimensional
 *                                information of input image
 *  @param [in]  bufParamsMask :  Pointer to the structure containing dimensional
 *                                information of mask
 *  @param [out] bufParamsOut  :  Pointer to the structure containing dimensional
 *                                information of output image
 *  @param [out] bufParamsScratch  :  Pointer to the structure containing dimensional
 *                                information of scratch buffer
 *  @param [in]  pKerInitArgs  :  Pointer to the structure holding init
 *                                parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref VXLIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle.
 */

VXLIB_STATUS VXLIB_median_init(VXLIB_kernelHandle           handle,
                               VXLIB_bufParams2D_t         *bufParamsIn,
                               VXLIB_bufParams2D_t         *bufParamsMask,
                               VXLIB_bufParams2D_t         *bufParamsOut,
                               VXLIB_bufParams2D_t         *bufParamsScratch,
                               const VXLIB_median_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref VXLIB_median_init function. This function
 *               is called with the same parameters as the
 *               @ref VXLIB_median_init, and this function
 *               must be called before the
 *               @ref VXLIB_median_init is called.
 *
 *  @param [in]  handle        :  Active handle to the kernel
 *  @param [in]  bufParamsIn   :  Pointer to the structure containing dimensional
 *                                information of input image
 *  @param [in]  bufParamsMask :  Pointer to the structure containing dimensional
 *                                information of mask
 *  @param [out] bufParamsOut  :  Pointer to the structure containing dimensional
 *                                information of output image
 *  @param [out] bufParamsScratch  :  Pointer to the structure containing dimensional
 *                                information of scratch buffer
 *  @param [in]  pKerInitArgs  :  Pointer to the structure holding init
 *                                parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref VXLIB_STATUS.
 *
 *
 *  @remarks     None
 */

VXLIB_STATUS
VXLIB_median_init_checkParams(VXLIB_kernelHandle           handle,
                              const VXLIB_bufParams2D_t   *bufParamsIn,
                              const VXLIB_bufParams2D_t   *bufParamsMask,
                              const VXLIB_bufParams2D_t   *bufParamsOut,
                              const VXLIB_bufParams2D_t   *bufParamsScratch,
                              const VXLIB_median_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref VXLIB_median_exec function. This function
 *               is called with the same parameters as the
 *               @ref VXLIB_median_exec, and this function
 *               must be called before the
 *               @ref VXLIB_median_exec is called.
 *
 *  @param [in]  handle :  Active handle to the kernel
 *  @param [in]  pIn    :  Pointer to the structure input image
 *  @param [in]  pMask  :  Pointer to the structure mask
 *  @param [out] pOut   :  Pointer to the output buffer
 *  @param [out] pScratch   :  Pointer to the scratch buffer
 *
 *
 *  @return      Status value indicating success or failure. Refer to @ref VXLIB_STATUS.
 *
 *  @remarks     None
 */

VXLIB_STATUS VXLIB_median_exec_checkParams(VXLIB_kernelHandle handle,
                                           const void *restrict pIn,
                                           const void *restrict pMask,
                                           const void *restrict pOut,
                                           const void *restrict pScratch);

/**
 *  @brief       This function is the main kernel compute function.
 *
 *  @details     Please refer to details under
 *               @ref VXLIB_median_exec
 *
 *  @param [in]  handle :  Active handle to the kernel
 *  @param [in]  pIn    :  Pointer to the structure input image
 *  @param [in]  pMask  :  Pointer to the structure mask
 *  @param [out] pOut   :  Pointer to the output buffer
 *  @param [out] pScratch   :  Pointer to the scratch buffer
 *
 *  @par Diamensions of Input/Output buffers
 *  | Buffer   | Row          | Column      |
 *  | :----:   | :----:       | :----:      |
 *  | pIn      | R            | C           |
 *  | pMask    | M            | N           |
 *  | pOut     | R - (M - 1)  | C - (N - 1) |
 *  | pScratch | M * N        | R           |
 *
 *  @return      Status value indicating success or failure. Refer to @ref VXLIB_STATUS.
 *
 *  @par Assumptions:
 *    - None
 *
 *  @par Performance Considerations:
 *    For best performance,
 *    - The input and output data buffers are expected to be in L2 memory
 *    - The buffer pointers are assumed to be 64-byte aligned
 *
 *  @remarks     Before calling this function, application is expected to call
 *               @ref VXLIB_median_init and
 *               @ref VXLIB_median_exec_checkParams functions.
 *               This ensures resource configuration and error checks are done
 * only once for several invocations of this function.
 */

VXLIB_STATUS
VXLIB_median_exec(VXLIB_kernelHandle handle,
                  void *restrict pIn,
                  void *restrict pMask,
                  void *restrict pOut,
                  void *restrict pScratch);



/**
*  @brief       This function estimates the archCycles and estCycles.
*
*  @param [in]  handle          :  Active handle to the kernel
*  @param [in]  bufParamsIn     :  Pointer to the structure containing dimensional
*                                  information of input image
*  @param [in]  bufParamsMask   :  Pointer to the structure containing dimensional
*                                  information of mask
*  @param [in] bufParamsOut     :  Pointer to the structure containing dimensional
*                                  information of output image
*  @param [in] bufParamsScratch :  Pointer to the structure containing dimensional
*                                  information of scratch buffer
*  @param [in]  pKerInitArgs    :  Pointer to the structure holding init
*                                  parameters
*  @param [out] archCycles      :  Arch compute cycles obtained from asm.
*  @param [out] estCycles       :  Estimated cycles including overhead.
*
*  @return      None.
*
*  @par Assumptions:
*    - The function @ref VXLIB_median_init is called prior to this function call 
*
*  @remarks     This function is expected to be called when the estimation of cycles is needed.
*/
void VXLIB_median_perfEst(VXLIB_kernelHandle           handle,
                          const VXLIB_bufParams2D_t   *bufParamsIn,
                          const VXLIB_bufParams2D_t   *bufParamsMask,
                          const VXLIB_bufParams2D_t   *bufParamsOut,
                          const VXLIB_bufParams2D_t   *bufParamsScratch,
                          const VXLIB_median_InitArgs *pKerInitArgs,
                          size_t *archCycles,
                          size_t *estCycles);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* VXLIB_MEDIAN_IXX_IXX_OXX_H_ */

/* ======================================================================== */
/*  End of file:  VXLIB_median.h                                          */
/* ======================================================================== */
