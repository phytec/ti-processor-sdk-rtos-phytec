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

#ifndef VXLIB_MAGNITUDE_IXX_IXX_OXX_PRIV_H_
#define VXLIB_MAGNITUDE_IXX_IXX_OXX_PRIV_H_

#include "../common/VXLIB_utility.h"
#include "VXLIB_magnitude.h"
#include "VXLIB_types.h"

/**
 * \file
 * \brief Header file for kernel's internal use. For the kernel's interface,
 * please see @ref VXLIB_magnitude
 */

/**
 * @brief Macro to define the size of bufPblock array of
 *        @ref VXLIB_magnitude_PrivArgs structure.
 *
 */

#define VXLIB_MAGNITUDE_IXX_IXX_OXX_PBLOCK_SIZE (2 * VXLIB_SE_PARAM_SIZE + VXLIB_SA_PARAM_SIZE)

/**
 * @brief Macros that will be useful to check for datatype combinations
 *
 */
#define VXLIB_MAGNITUDE_I16S_I16S_O16S                                                                                 \
   ((dTypeIn0 == VXLIB_INT16) && (dTypeIn1 == VXLIB_INT16) && (dTypeOut == VXLIB_INT16))

/**
 * @brief Macros for templatization of execution functions
 *
 */
#define VXLIB_MAGNITUDE_TYPENAME_I16S_I16S_O16S int16_t, int16_t, int16_t

/**
 * @brief Macros for templatization of initialization functions
 *
 */
#define VXLIB_MAGNITUDE_DTYPE_I16S_I16S_O16S VXLIB_INT16, VXLIB_INT16, VXLIB_INT16

/**
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref VXLIB_magnitude_exec_ci
 *         and @ref VXLIB_magnitude_exec_cn.
 */

typedef VXLIB_STATUS (*pFxnVXLIB_magnitude_exec)(VXLIB_kernelHandle handle,
                                                 void *restrict pIn0,
                                                 void *restrict pIn1,
                                                 void *restrict pOut);

/**
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref VXLIB_magnitude_init.
 *
 * @details This function determines the configuration for the streaming engine
 *           resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref VXLIB_magnitude_exec_ci would be
 *          called later independently by the application. When
 *          @ref VXLIB_magnitude_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref VXLIB_magnitude_exec_ci does not lose cycles
 *          to determine the hardware configuration.
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [in]  bufParamsIn0 : Pointer to the structure containing dimensional
 *                              information of input image 0
 *  @param [in]  bufParamsIn1 : Pointer to the structure containing dimensional
 *                              information of input image 1
 *  @param [out] bufParamsOut : Pointer to the structure containing dimensional
 *                              information of output image
 *  @param [in]  pKerInitArgs : Pointer to the structure holding init
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * VXLIB_STATUS.
 *
 */

template <uint32_t dTypeIn0, uint32_t dTypeIn1, uint32_t dTypeOut>
extern VXLIB_STATUS VXLIB_magnitude_init_ci(VXLIB_kernelHandle              handle,
                                            const VXLIB_bufParams2D_t      *bufParamsIn0,
                                            const VXLIB_bufParams2D_t      *bufParamsIn1,
                                            const VXLIB_bufParams2D_t      *bufParamsOut,
                                            const VXLIB_magnitude_InitArgs *pKerInitArgs);

/**
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref VXLIB_magnitude_exec.
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn0        : Pointer to buffer holding the input image 0
 *  @param [in]  pIn1        : Pointer to buffer holding the input image 1
 *  @param [out] pOut        : Pointer to buffer holding the output data
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * VXLIB_STATUS.
 *
 *  @par Performance Considerations:
 *    For best performance,
 *    - the input and output data images are expected to be in L2 memory
 *    - the buffer pointers are assumed to be 64-byte aligned
 *
 */

template <typename dTypeIn0, typename dTypeIn1, typename dTypeOut>
VXLIB_STATUS
VXLIB_magnitude_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut);

/**
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref VXLIB_magnitude_exec.
 *
 * @details
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn0        : Pointer to buffer holding the input image 0
 *  @param [in]  pIn1        : Pointer to buffer holding the input image 1
 *  @param [out] pOut        : Pointer to buffer holding the output data
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * VXLIB_STATUS.
 *
 */
template <typename dTypeIn0, typename dTypeIn1, typename dTypeOut>
extern VXLIB_STATUS
VXLIB_magnitude_exec_cn(VXLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut);

/**
 * @brief Structure that is reserved for internal use by the kernel
 */

typedef struct {
   /** @brief Initargs of the kernel **/
   VXLIB_magnitude_InitArgs pKerInitArgs;

   /** @brief Function pointer to point to the right execution variant between
    *         @ref VXLIB_magnitude_exec_cn and
    *         @ref VXLIB_magnitude_exec_ci.                        */
   pFxnVXLIB_magnitude_exec execute;

   /** @brief Width of image  **/
   size_t width;
   /** @brief Height of image  **/
   size_t height;

   /** @brief Stride of input0 in elements **/
   size_t strideIn0Elements;

   /** @brief Stride of input1 in elements **/
   size_t strideIn1Elements;

   /** @brief Stride of output in elements **/
   size_t strideOutElements;

   /** @brief Number of blocks to be processed after simidfication **/
   size_t numBlocks;

   /** @brief Array to hold SE/SA params */
   uint8_t bufPblock[VXLIB_MAGNITUDE_IXX_IXX_OXX_PBLOCK_SIZE];
} VXLIB_magnitude_PrivArgs;

/**
 *  @brief This function is the initialization function for the natural C
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref VXLIB_magnitude_init.
 *
 * @details
 *
 *  @param [in]  handle                 :  Active handle to the kernel
 *  @param [in]  bufParamsIn            :  Pointer to the structure containing
 * dimensional information of input buffer
 *  @param [out] bufParamsOut           :  Pointer to the structure containing
 * dimensional information of ouput buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * VXLIB_STATUS.
 *
 */
VXLIB_STATUS
VXLIB_magnitude_init_cn(VXLIB_kernelHandle handle, VXLIB_bufParams2D_t *bufParamsIn, VXLIB_bufParams2D_t *bufParamsOut);

#endif /* VXLIB_MAGNITUDE_IXX_IXX_OXX_PRIV_H_ */
