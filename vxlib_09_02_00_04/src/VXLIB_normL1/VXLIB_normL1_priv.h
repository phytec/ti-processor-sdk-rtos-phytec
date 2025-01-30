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

#ifndef VXLIB_NORML1_IXX_IXX_OXX_PRIV_H_
#define VXLIB_NORML1_IXX_IXX_OXX_PRIV_H_

#include "../common/VXLIB_utility.h"
#include "VXLIB_normL1.h"
#include "VXLIB_types.h"
#include <cstdint>

/**
 * \file
 * \brief Header file for kernel's internal use. For the kernel's interface,
 * please see @ref VXLIB_normL1
 */

/**
 * @brief Macro to define the size of bufPblock array of
 *        @ref VXLIB_normL1_PrivArgs structure.
 *
 */

#define VXLIB_NORML1_IXX_IXX_OXX_PBLOCK_SIZE (2 * VXLIB_SE_PARAM_SIZE + VXLIB_SA_PARAM_SIZE)

/**
 * @brief Macros that will be useful to check for datatype combinations
 *
 */

#define VXLIB_NOMRL1_I8S_O8U ((dTypeIn == VXLIB_INT8) && (dTypeOut == VXLIB_UINT8))
#define VXLIB_NORML1_I16S_O16U ((dTypeIn == VXLIB_INT16) && (dTypeOut == VXLIB_UINT16))
/**
 * @brief Macros for templatization of execution functions
 *
 */

#define VXLIB_NORML1_TYPENAME_I8S_O8U int8_t, uint8_t
#define VXLIB_NORML1_TYPENAME_I16S_O16U int16_t, uint16_t
/**
 * @brief Macros for templatization of initialization functions
 *
 */

#define VXLIB_NORML1_DTYPE_I8S_O8U VXLIB_INT8, VXLIB_UINT8
#define VXLIB_NORML1_DTYPE_I16S_O16U VXLIB_INT16, VXLIB_UINT16

/**
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref VXLIB_normL1_exec_ci
 *         and @ref VXLIB_normL1_exec_cn.
 */

typedef VXLIB_STATUS (*pFxnVXLIB_normL1_exec)(VXLIB_kernelHandle handle,
                                              void *restrict     pIn0,
                                              void *restrict     pIn1,
                                              void *restrict     pOut);

/**
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref VXLIB_normL1_init.
 *
 * @details This function determines the configuration for the streaming engine
 *           resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref VXLIB_normL1_exec_ci would be
 *          called later independently by the application. When
 *          @ref VXLIB_normL1_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref VXLIB_normL1_exec_ci does not lose cycles
 *          to determine the hardware configuration.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn0  :  Pointer to the structure containing dimensional
 *                               information of input image 0
 *  @param [in]  bufParamsIn1  :  Pointer to the structure containing dimensional
 *                               information of input image 1
 *  @param [out] bufParamsOut :  Pointer to the structure containing dimensional
 *                               information of output buffer
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * VXLIB_STATUS.
 *
 */

template <uint32_t dTypeIn, uint32_t dTypeOut>
extern VXLIB_STATUS VXLIB_normL1_init_ci(VXLIB_kernelHandle           handle,
                                         const VXLIB_bufParams2D_t *  bufParamsIn0,
                                         const VXLIB_bufParams2D_t *  bufParamsIn1,
                                         const VXLIB_bufParams2D_t *  bufParamsOut,
                                         const VXLIB_normL1_InitArgs *pKerInitArgs);

/**
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref VXLIB_normL1_exec.
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn0        : Pointer to buffer holding the input image 0
 *  @param [in]  pIn1        : Pointer to buffer holding the input image 1
 *  @param [out] pOut        : Pointer to buffer holding the output image
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * VXLIB_STATUS.
 *
 *  @par Performance Considerations:
 *    For best performance,
 *    - the input and output data buffers are expected to be in L2 memory
 *    - the buffer pointers are assumed to be 64-byte aligned
 *
 */

template <typename dTypeIn, typename dTypeOut>
extern VXLIB_STATUS
VXLIB_normL1_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut);

/**
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref VXLIB_normL1_exec.
 *
 * @details
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn0        : Pointer to buffer holding the input image 0
 *  @param [in]  pIn1        : Pointer to buffer holding the input image 1
 *  @param [out] pOut        : Pointer to buffer holding the output image
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * VXLIB_STATUS.
 *
 */
template <typename dTypeIn0, typename dTypeOut>
extern VXLIB_STATUS
VXLIB_normL1_exec_cn(VXLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut);

/**
 * @brief Structure that is reserved for internal use by the kernel
 */

typedef struct {
   /** @brief Function pointer to point to the right execution variant between
    *         @ref VXLIB_normL1_exec_cn and
    *         @ref VXLIB_normL1_exec_ci.                        */
   pFxnVXLIB_normL1_exec execute;

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
   uint8_t bufPblock[VXLIB_NORML1_IXX_IXX_OXX_PBLOCK_SIZE];
} VXLIB_normL1_PrivArgs;

#endif /* VXLIB_NORML1_IXX_IXX_OXX_PRIV_H_ */

/* ======================================================================== */
/*  End of file:  VXLIB_normL1_priv.h                                       */
/* ======================================================================== */
