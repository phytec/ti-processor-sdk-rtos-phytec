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

#ifndef VXLIB_MEDIAN_IXX_IXX_OXX_PRIV_H_
#define VXLIB_MEDIAN_IXX_IXX_OXX_PRIV_H_

#include "../common/VXLIB_utility.h"
#include "VXLIB_median.h"
#include "VXLIB_types.h"

/* ******************************************************************
 *
 * DEFINES
 *
 ********************************************************************* */

#define CONFIG_PARAM_BASE (0x0000)
#define SE0_PARAM_OFFSET (CONFIG_PARAM_BASE)
#define SE1_PARAM_OFFSET (SE0_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA0_PARAM_OFFSET (SE1_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE0_1_PARAM_OFFSET (SA0_PARAM_OFFSET + SA_PARAM_SIZE)
#define SE1_1_PARAM_OFFSET (SE0_1_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA1_1_PARAM_OFFSET (SE1_1_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA2_1_PARAM_OFFSET (SA1_1_PARAM_OFFSET + SA_PARAM_SIZE)


#define MIN_VAL(a, b) __min(a, b)
#define MAX_VAL(a, b) __max(a, b)

/**
 * \file
 * \brief Header file for kernel's internal use. For the kernel's interface,
 * please see @ref VXLIB_median
 */

/**
 * @brief Macro to define the size of bufPblock array of
 *        @ref VXLIB_median_PrivArgs structure.
 *
 */
#define VXLIB_MEDIAN_IXX_IXX_OXX_PBLOCK_SIZE ((4 * VXLIB_SE_PARAM_SIZE) + (3 * VXLIB_SA_PARAM_SIZE))

/**
 * @brief Macros that will be useful to check for datatype combinations
 *
 */
#define VXLIB_MEDIAN_8U (dType == VXLIB_UINT8)
#define VXLIB_MEDIAN_8S (dType == VXLIB_INT8)
#define VXLIB_MEDIAN_16U (dType == VXLIB_UINT16)
#define VXLIB_MEDIAN_16S (dType == VXLIB_INT16)
#define VXLIB_MEDIAN_64S (dType == VXLIB_INT64)

/**
 * @brief Macros for templatization of execution functions
 *
 */
#define VXLIB_MEDIAN_TYPENAME_8U uint8_t
#define VXLIB_MEDIAN_TYPENAME_8S int8_t
#define VXLIB_MEDIAN_TYPENAME_16U uint16_t
#define VXLIB_MEDIAN_TYPENAME_16S int16_t
#define VXLIB_MEDIAN_TYPENAME_64S int64_t

/**
 * @brief Macros for templatization of initialization functions
 *
 */
#define VXLIB_MEDIAN_DTYPE_8U VXLIB_UINT8
#define VXLIB_MEDIAN_DTYPE_8S VXLIB_INT8
#define VXLIB_MEDIAN_DTYPE_16U VXLIB_UINT16
#define VXLIB_MEDIAN_DTYPE_16S VXLIB_INT16
#define VXLIB_MEDIAN_DTYPE_64S VXLIB_INT64

/**
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref VXLIB_median_3X3_exec_ci @ref VXLIB_median_MXN_exec_ci
 *         and @ref VXLIB_median_exec_cn.
 */
typedef VXLIB_STATUS (*pFxnVXLIB_median_exec)(VXLIB_kernelHandle handle,
                                              void *restrict     pIn,
                                              void *restrict     pMask,
                                              void *restrict     pOut,
                                              void *restrict     pScratch);

/**
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref VXLIB_median_init.
 *
 * @details This function determines the configuration for the streaming engine
 *           resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref VXLIB_median_3X3_exec_ci would be
 *          called later independently by the application. When
 *          @ref VXLIB_median_3X3_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref VXLIB_median_3X3_exec_ci does not lose cycles
 *          to determine the hardware configuration.
 *
 *  @param [in]  handle        :  Active handle to the kernel
 *  @param [in]  bufParamsIn   :  Pointer to the structure containing dimensional
 *                                information of input image
 *  @param [in]  bufParamsMask :  Pointer to the structure containing dimensional
 *                                information of mask
 *  @param [out] bufParamsOut  :  Pointer to the structure containing dimensional
 *                                information of output image
 *  @param [out] bufParamsScratch  :  Pointer to the structure containing dimensional
 *                                information of scratch
 *  @param [in]  pKerInitArgs  :  Pointer to the structure holding init
 *                                parameters
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * VXLIB_STATUS.
 *
 */
template <typename dType>
extern VXLIB_STATUS VXLIB_median_3X3_init_ci(VXLIB_kernelHandle           handle,
                                             const VXLIB_bufParams2D_t *  bufParamsIn,
                                             const VXLIB_bufParams2D_t *  bufParamsMask,
                                             const VXLIB_bufParams2D_t *  bufParamsOut,
                                             const VXLIB_bufParams2D_t *  bufParamsScratch,
                                             const VXLIB_median_InitArgs *pKerInitArgs);

/**
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref VXLIB_median_init.
 *
 * @details This function determines the configuration for the streaming engine
 *           resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref VXLIB_median_MXN_exec_ci would be
 *          called later independently by the application. When
 *          @ref VXLIB_median_MXN_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref VXLIB_median_MXN_exec_ci does not lose cycles
 *          to determine the hardware configuration.
 *
 *  @param [in]  handle        :  Active handle to the kernel
 *  @param [in]  bufParamsIn   :  Pointer to the structure containing dimensional
 *                                information of input image
 *  @param [in]  bufParamsMask :  Pointer to the structure containing dimensional
 *                                information of mask
 *  @param [out] bufParamsOut  :  Pointer to the structure containing dimensional
 *                                information of output image
 *  @param [out] bufParamsScratch  :  Pointer to the structure containing dimensional
 *                                information of scratch
 *  @param [in]  pKerInitArgs  :  Pointer to the structure holding init
 *                                parameters
 * parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * VXLIB_STATUS.
 *
 */
template <typename dType>
extern VXLIB_STATUS VXLIB_median_MXN_init_ci(VXLIB_kernelHandle           handle,
                                             const VXLIB_bufParams2D_t *  bufParamsIn,
                                             const VXLIB_bufParams2D_t *  bufParamsMask,
                                             const VXLIB_bufParams2D_t *  bufParamsOut,
                                             const VXLIB_bufParams2D_t *  bufParamsScratch,
                                             const VXLIB_median_InitArgs *pKerInitArgs);

/**
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref VXLIB_median_exec.
 *
 *  @param [in]  handle :  Active handle to the kernel
 *  @param [in]  pIn    :  Pointer to the structure input image
 *  @param [in]  pMask  :  Pointer to the structure mask
 *  @param [out] pOut   :  Pointer to the output buffer
 *  @param [out] pScratch   :  Pointer to the scratch buffer
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
template <typename dType>
VXLIB_STATUS VXLIB_median_3X3_exec_ci(VXLIB_kernelHandle handle,
                                      void *restrict     pIn,
                                      void *restrict     pMask,
                                      void *restrict     pOut,
                                      void *restrict     pScratch);

/**
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref VXLIB_median_exec.
 *
 *  @param [in]  handle :  Active handle to the kernel
 *  @param [in]  pIn    :  Pointer to the structure input image
 *  @param [in]  pMask  :  Pointer to the structure mask
 *  @param [out] pOut   :  Pointer to the output buffer
 *  @param [out] pScratch   :  Pointer to the scratch buffer
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
template <typename dType>
VXLIB_STATUS VXLIB_median_MXN_exec_ci(VXLIB_kernelHandle handle,
                                      void *restrict     pIn,
                                      void *restrict     pMask,
                                      void *restrict     pOut,
                                      void *restrict     pScratch);

/**
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref VXLIB_median_exec.
 *
 * @details
 *
 *  @param [in]  handle :  Active handle to the kernel
 *  @param [in]  pIn    :  Pointer to the structure input image
 *  @param [in]  pMask  :  Pointer to the structure mask
 *  @param [out] pOut   :  Pointer to the output buffer
 *  @param [out] pScratch   :  Pointer to the scratch buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * VXLIB_STATUS.
 *
 */
template <typename dType>
extern VXLIB_STATUS VXLIB_median_exec_cn(VXLIB_kernelHandle handle,
                                         void *restrict     pIn,
                                         void *restrict     pMask,
                                         void *restrict     pOut,
                                         void *restrict     pScratch);

/**
 * @brief Structure that is reserved for internal use by the kernel
 */

typedef struct {
   /** @brief Initargs of the kernel **/
   VXLIB_median_InitArgs pKerInitArgs;

   /** @brief Function pointer to point to the right execution variant between
    *         @ref VXLIB_median_exec_cn and
    *         @ref VXLIB_median_3X3_exec_ci
    *         @ref VXLIB_median_MXN_exec_ci                        */
   pFxnVXLIB_median_exec execute;

   /** @brief Width of image  **/
   size_t width;
   /** @brief Height of image  **/
   size_t height;

   /** @brief Width of image  **/
   size_t N;
   /** @brief Height of image  **/
   size_t M;

   /** @brief Width of image  **/
   size_t widthOut;
   /** @brief Height of image  **/
   size_t heightOut;

   /** @brief Width of scratch  **/
   size_t widthScratch;
   /** @brief Height of image  **/
   size_t heightScratch;

   /** @brief Stride of INPUT in elements **/
   size_t strideInElements;

   /** @brief Stride of MASK in elements **/
   size_t strideMaskElements;

   /** @brief Stride of output in elements **/
   size_t strideOutElements;

   /** @brief Stride of scratch in elements **/
   size_t strideScratchElements;

   /** @brief Number of blocks to be processed after simidfication **/
   size_t numBlocks;

   /** @brief Array to hold SE/SA params */
   uint8_t bufPblock[VXLIB_MEDIAN_IXX_IXX_OXX_PBLOCK_SIZE];
} VXLIB_median_PrivArgs;

#endif /* VXLIB_MEDIAN_IXX_IXX_OXX_PRIV_H_ */

/* ======================================================================== */
/*  End of file:  VXLIB_median_priv.h                                     */
/* ======================================================================== */
