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

#ifndef VXLIB_TABLELOOKUP_IXX_IXX_OXX_H_
#define VXLIB_TABLELOOKUP_IXX_IXX_OXX_H_

#include "../common/VXLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup VXLIB_KERNELS
 */

/**
 * @defgroup VXLIB_tableLookup VXLIB_tableLookup
 * @brief Kernel for mapping an input pixel value to another value via lookup table
 *
 * @details
 * Typically used for tone mapping input pixels.
 *
 *  | Case    | Input   | Output  |
 *  |  ----:  | :----:  | :----:  |
 *  |    0    | uint8_t | uint8_t |
 *  |    1    | int16_t | int16_t |
 *
 * @par Method
 *  The Table Lookup Image Kernel is computed by using the following equation:
 * @verbatim

   dst(x,y) = lut[src(x,y)]

   @endverbatim

 * @ingroup  VXLIB
 */

/**@{*/

/**
 * @brief Structure containing the parameters to initialize the kernel
 */

typedef struct {
   /** @brief Variant of the function, refer to @ref VXLIB_FUNCTION_STYLE        */
   int8_t funcStyle;
   /** @brief Parameter indicating size of lookup table                          */
   uint32_t count;
   /** @brief Parameter indicating index of input value = 0 in the lookup table  */
   uint16_t offset;

} VXLIB_tableLookup_InitArgs;

/**
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */

int32_t VXLIB_tableLookup_getHandleSize(VXLIB_tableLookup_InitArgs *pKerInitArgs);

/**
 *  @brief       This function should be called before the
 *               @ref VXLIB_tableLookup_exec function is called.
 *               This function takes care of any one-time operations
 *               such as setting up the configuration of required
 *               hardware resources such as the streaming engine.
 *               The results of these operations are stored in the handle.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn  :  Pointer to the structure containing dimensional
 *                               information of input image
 *  @param [out] bufParamsOut :  Pointer to the structure containing dimensional
 *                               information of output image
 *  @param [in]  bufParamsLut :  Pointer to the structure containing dimensional
 *                               information of lookup table
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 *                               parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref VXLIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle.
 */

VXLIB_STATUS VXLIB_tableLookup_init(VXLIB_kernelHandle                handle,
                                    VXLIB_bufParams2D_t              *bufParamsIn,
                                    VXLIB_bufParams2D_t              *bufParamsOut,
                                    VXLIB_bufParams1D_t              *bufParamsLut,
                                    const VXLIB_tableLookup_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref VXLIB_tableLookup_init function. This function
 *               is called with the same parameters as the
 *               @ref VXLIB_tableLookup_init, and this function
 *               must be called before the
 *               @ref VXLIB_tableLookup_init is called.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn  :  Pointer to the structure containing dimensional
 *                               information of input image
 *  @param [out] bufParamsOut :  Pointer to the structure containing dimensional
 *                               information of output image
 *  @param [in]  bufParamsLut :  Pointer to the structure containing dimensional
 *                               information of lookup table
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 *                               parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref VXLIB_STATUS.
 *
 *
 *  @remarks     None
 */

VXLIB_STATUS
VXLIB_tableLookup_init_checkParams(VXLIB_kernelHandle                handle,
                                   const VXLIB_bufParams2D_t        *bufParamsIn,
                                   const VXLIB_bufParams2D_t        *bufParamsOut,
                                   const VXLIB_bufParams1D_t        *bufParamsLut,
                                   const VXLIB_tableLookup_InitArgs *pKerInitArgs);

/**
 *  @brief       This function place the lookup table in suitable memory location

 *  @param [in]  handle :  Active handle to the kernel
 *  @param [in]  pLut   :  Pointer to the lookup table buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref VXLIB_STATUS.
 *
 *  @remarks     None
 */
VXLIB_STATUS
VXLIB_tableLookup_set(VXLIB_kernelHandle handle, void *restrict pLut);
/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref VXLIB_tableLookup_exec function. This function
 *               is called with the same parameters as the
 *               @ref VXLIB_tableLookup_exec, and this function
 *               must be called before the
 *               @ref VXLIB_tableLookup_exec is called.
 *
 *  @param [in]  handle :  Active handle to the kernel
 *  @param [in]  pIn    :  Pointer to the structure input image
 *  @param [out] pOut   :  Pointer to the output image
 *  @param [in]  pLut   :  Pointer to the lookup table buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref VXLIB_STATUS.
 *
 *  @remarks     None
 */

VXLIB_STATUS VXLIB_tableLookup_exec_checkParams(VXLIB_kernelHandle handle,
                                                const void *restrict pIn,
                                                const void *restrict pOut,
                                                const void *restrict pLut);

/**
 *  @brief       This function is the main kernel compute function.
 *
 *  @details     Please refer to details under
 *               @ref VXLIB_tableLookup_exec
 *
 *  @param [in]  handle :  Active handle to the kernel
 *  @param [in]  pIn    :  Pointer to the structure input image
 *  @param [out] pOut   :  Pointer to the output image
 *  @param [in]  pLut   :  Pointer to the lookup table buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref VXLIB_STATUS.
 *
 *  @par Assumptions:
 *    - I/O buffer pointers are assumed to be not aliased.
 *    - @ref VXLIB_tableLookup_InitArgs.count should not greater than the
 *      max limit of the data type used and should not less the
 *      each value in pIn buffer.
 *    - @ref VXLIB_tableLookup_InitArgs.offset should less than
 *      @ref VXLIB_tableLookup_InitArgs.count
 *    - The strides of each image shall be equal to or greater than the x dimension
 *
 *  @par Performance Considerations:
 *    For best performance,
 *    - the input and output data image are expected to be in L2 memory
 *    - the buffer pointers are assumed to be 64-byte aligned
 *
 *  @remarks     Before calling this function, application is expected to call
 *               @ref VXLIB_tableLookup_init and
 *               @ref VXLIB_tableLookup_exec_checkParams functions.
 *               This ensures resource configuration and error checks are done only
 *               once for several invocations of this function.
 */

VXLIB_STATUS
VXLIB_tableLookup_exec(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut, void *restrict pLut);

/**
 *  @brief       This function estimates the archCycles and estCycles.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn  :  Pointer to the structure containing dimensional
 *                               information of input image
 *  @param [out] bufParamsOut :  Pointer to the structure containing dimensional
 *                               information of output image
 *  @param [in]  bufParamsLut :  Pointer to the structure containing dimensional
 *                               information of lookup table
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 *                               parameters
 *  @param [out] archCycles   :  Arch compute cycles obtained from asm.
 *
 *  @param [out] estCycles    :  Estimated cycles including overhead.
 *
 *  @return      None.
 *
 *  @par Assumptions:
 *    - The function @ref VXLIB_tableLookup_init is called prior to this function call
 * *
 *  @remarks     This function is expected to be called when the estimation of cycles is needed.
 */

void VXLIB_tableLookup_perfEst(VXLIB_kernelHandle                handle,
                               const VXLIB_bufParams2D_t        *bufParamsIn,
                               const VXLIB_bufParams2D_t        *bufParamsOut,
                               const VXLIB_bufParams1D_t        *bufParamsLut,
                               const VXLIB_tableLookup_InitArgs *pKerInitArgs,
                               size_t                           *archCycles,
                               size_t                           *estCycles);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* VXLIB_TABLELOOKUP_IXX_IXX_OXX_H_ */
/* ======================================================================== */
/*  End of file:  VXLIB_tableLookup.h                                       */
/* ======================================================================== */
