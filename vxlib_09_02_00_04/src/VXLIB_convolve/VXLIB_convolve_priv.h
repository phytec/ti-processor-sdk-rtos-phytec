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

#ifndef VXLIB_CONVOLVE_IXX_IXX_OXX_PRIV_H_
#define VXLIB_CONVOLVE_IXX_IXX_OXX_PRIV_H_

#include "../common/VXLIB_utility.h"
#include "VXLIB_bufParams.h"
#include "VXLIB_convolve.h"
#include "VXLIB_types.h"
#include <cstdint>

/**
 * \file
 * \brief Header file for kernel's internal use. For the kernel's interface,
 * please see @ref VXLIB_convolve
 */

/**
 * @brief Macro to define the size of bufPblock array of
 *        @ref VXLIB_convolve_PrivArgs structure.
 *
 */

#define VXLIB_CONVOLVE_IXX_IXX_OXX_PBLOCK_SIZE (3 * VXLIB_SE_PARAM_SIZE + 2 * VXLIB_SA_PARAM_SIZE)

/**
 * @brief Macros that will be useful to check for datatype combinations
 *
 */

#define VXLIB_CONVOLVE_I8U_C16S_O8U                                                                                    \
   ((dTypeIn == VXLIB_UINT8) && (dTypeFilter == VXLIB_INT16) && (dTypeOut == VXLIB_UINT8))

#define VXLIB_CONVOLVE_I8U_C16S_O16S                                                                                   \
   ((dTypeIn == VXLIB_UINT8) && (dTypeFilter == VXLIB_INT16) && (dTypeOut == VXLIB_INT16))

/**
 * @brief Macros for templatization of execution functions
 *
 */

#define VXLIB_CONVOLVE_TYPENAME_I8U_C16S_O8U uint8_t, int16_t, uint8_t
#define VXLIB_CONVOLVE_TYPENAME_I8U_C16S_O16S uint8_t, int16_t, int16_t

/**
 * @brief Macros for templatization of initialization functions
 *
 */

#define VXLIB_CONVOLVE_DTYPE_I8U_C16S_O8U VXLIB_UINT8, VXLIB_INT16, VXLIB_UINT8
#define VXLIB_CONVOLVE_DTYPE_I8U_C16S_O16S VXLIB_UINT8, VXLIB_INT16, VXLIB_INT16

/**
 * @brief Macros for datatype combination of exec functions
 *
 */

#define VXLIB_CONVOLVE_I8U_C16S_O8U_TEMPLATE(dTypeOut)                                           \
   (std::is_same<dTypeOut, uint8_t>::value)
#define VXLIB_CONVOLVE_I8U_C16S_O16S_TEMPLATE(dTypeOut)                                          \
   (std::is_same<dTypeOut, int16_t>::value)

/**
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref VXLIB_convolve_exec_ci
 *         and @ref VXLIB_convolve_exec_cn.
 */

typedef VXLIB_STATUS (*pFxnVXLIB_convolve_exec)(VXLIB_kernelHandle handle,
                                                void *restrict pIn,
                                                void *restrict pFilter,
                                                void *restrict pOut);

/**
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref VXLIB_convolve_init.
 *
 * @details This function determines the configuration for the streaming engine
 *           resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref VXLIB_convolve_exec_ci would be
 *          called later independently by the application. When
 *          @ref VXLIB_convolve_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref VXLIB_convolve_exec_ci does not lose cycles
 *          to determine the hardware configuration.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsIn  :  Pointer to the structure containing
 *  dimensional input image
 *  @param [in]  bufParamsFilter  :  Pointer to the structure containing  *  dimensional information of input filter
 *  @param [out] bufParamsOut :  Pointer to the structure containing
 *  dimensional
 *                               information of output image
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * VXLIB_STATUS.
 *
 */

template <uint32_t dTypeIn, uint32_t dTypeFilter, uint32_t dTypeOut>
extern VXLIB_STATUS VXLIB_convolve_init_ci(VXLIB_kernelHandle             handle,
                                           const VXLIB_bufParams2D_t     *bufParamsIn,
                                           const VXLIB_bufParams2D_t     *bufParamsFilter,
                                           const VXLIB_bufParams2D_t     *bufParamsOut,
                                           const VXLIB_convolve_InitArgs *pKerInitArgs);

/**
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref VXLIB_convolve_exec.
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pFilter     : Pointer to buffer holding input filter
 *  @param [in]  pIn         : Pointer to buffer holding the input image
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

template <typename dTypeIn, typename dTypeFilter, typename dTypeOut>
VXLIB_STATUS
VXLIB_convolve_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut);

/**
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref VXLIB_convolve_exec.
 *
 * @details
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pIn         : Pointer to buffer holding the input data
 *  @param [in]  pFilter     : Pointer to buffer holding the input filter
 *  @param [out] pOut        : Pointer to buffer holding the output data
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * VXLIB_STATUS.
 *
 */
template <typename dTypeIn, typename dTypeFilter, typename dTypeOut>
extern VXLIB_STATUS
VXLIB_convolve_exec_cn(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut);

/**
 * @brief Structure that is reserved for internal use by the kernel
 */

typedef struct {
   /** @brief Initargs of the kernel **/
   VXLIB_convolve_InitArgs pKerInitArgs;

   /** @brief Function pointer to point to the right execution variant between
    *         @ref VXLIB_convolve_exec_cn and
    *         @ref VXLIB_convolve_exec_ci.                        */
   pFxnVXLIB_convolve_exec execute;

   /** @brief Width of image  **/
   size_t width;
   /** @brief Height of image  **/
   size_t height;

   /** @brief Stride of input in elements **/
   size_t strideInElements;

   /** @brief Stride of output in elements **/
   size_t strideOutElements;

   /** @brief Number of blocks to be processed after simidfication **/
   size_t numBlocks;

   /** @brief Array to hold SE/SA params */
   uint8_t bufPblock[VXLIB_CONVOLVE_IXX_IXX_OXX_PBLOCK_SIZE];
} VXLIB_convolve_PrivArgs;

#endif /* VXLIB_CONVOLVE_IXX_IXX_OXX_PRIV_H_ */
