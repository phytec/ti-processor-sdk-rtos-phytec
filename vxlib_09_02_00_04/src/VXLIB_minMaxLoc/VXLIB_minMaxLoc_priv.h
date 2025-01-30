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

#ifndef VXLIB_MINMAXLOC_IXX_IXX_OXX_PRIV_H_
#define VXLIB_MINMAXLOC_IXX_IXX_OXX_PRIV_H_

#include "../common/VXLIB_utility.h"
#include "../common/VXLIB_inlines.h"
#include "VXLIB_minMaxLoc.h"
#include "VXLIB_types.h"

/**
 * \file
 * \brief Header file for kernel's internal use. For the kernel's interface,
 * please see @ref VXLIB_minMaxLoc
 */

/**
 * @brief Macro to define the size of bufPblock array of
 *        @ref VXLIB_minMaxLoc_PrivArgs structure.
 *
 */

#define VXLIB_MINMAXLOC_IXX_IXX_OXX_PBLOCK_SIZE ((4 * VXLIB_SE_PARAM_SIZE) + (3 * VXLIB_SA_PARAM_SIZE))

/**
 * @brief Macros that will be useful to check for datatype combinations
 *
 */

#define VXLIB_MINMAXLOC_8U (dType == VXLIB_UINT8)
#define VXLIB_MINMAXLOC_8S (dType == VXLIB_INT8)
#define VXLIB_MINMAXLOC_16U (dType == VXLIB_UINT16)
#define VXLIB_MINMAXLOC_16S (dType == VXLIB_INT16)

/**
 * @brief Macros for templatization of execution functions
 *
 */

#define VXLIB_MINMAXLOC_TYPENAME_8U uint8_t
#define VXLIB_MINMAXLOC_TYPENAME_8S int8_t
#define VXLIB_MINMAXLOC_TYPENAME_16U uint16_t
#define VXLIB_MINMAXLOC_TYPENAME_16S int16_t

/**
 * @brief Macros for templatization of initialization functions
 *
 */

#define VXLIB_MINMAXLOC_DTYPE_8U VXLIB_UINT8
#define VXLIB_MINMAXLOC_DTYPE_8S VXLIB_INT8
#define VXLIB_MINMAXLOC_DTYPE_16U VXLIB_UINT16
#define VXLIB_MINMAXLOC_DTYPE_16S VXLIB_INT16

/**
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref VXLIB_minMaxLoc_exec_ci
 *         and @ref VXLIB_minMaxLoc_exec_cn.
 */

typedef VXLIB_STATUS (*pFxnVXLIB_minMaxLoc_exec)(VXLIB_kernelHandle handle,
                                                 void *restrict     pIn,
                                                 void *restrict     pMinVal,
                                                 void *restrict     pMaxVal,
                                                 void *restrict     pMinCount,
                                                 void *restrict     pMaxCount,
                                                 void *restrict     pMinLocCapacity,
                                                 void *restrict     pMaxLocCapacity,
                                                 void *restrict     pMinLoc,
                                                 void *restrict     pMaxLoc,
                                                 void *restrict     pStartX,
                                                 void *restrict     pStartY);

/**
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref VXLIB_minMaxLoc_init.
 *
 * @details This function determines the configuration for the streaming engine
 *           resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref VXLIB_minMaxLoc_exec_ci would be
 *          called later independently by the application. When
 *          @ref VXLIB_minMaxLoc_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref VXLIB_minMaxLoc_exec_ci does not lose cycles
 *          to determine the hardware configuration.
 *
 *  @param [in]  handle            :  Active handle to the kernel
 *  @param [in]  bufParamsIn       :  Pointer to the structure containing dimensional
 *                                    information of input image
 *  @param [out] bufParamsMinLoc   :  Pointer to the structure containing dimensional
 *                                    information of minLoc
 *  @param [out] bufParamsMaxLoc   :  Pointer to the structure containing dimensional
 *                                    information of maxLoc
 *  @param [in]  pKerInitArgs      :  Pointer to the structure holding init
 *                                    parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * VXLIB_STATUS.
 *
 */

template <uint32_t dType>
extern VXLIB_STATUS VXLIB_minMaxLoc_init_ci(VXLIB_kernelHandle              handle,
                                            VXLIB_bufParams2D_t *           bufParamsIn,
                                            VXLIB_bufParams1D_t *           bufParamsMinLoc,
                                            VXLIB_bufParams1D_t *           bufParamsMaxLoc,
                                            const VXLIB_minMaxLoc_InitArgs *pKerInitArgs);

/**
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref VXLIB_minMaxLoc_exec.
 *
 *  @param [in]      handle         : Active handle to the kernel
 *  @param [in]      pIn[]          : Pointer to array containing input image
 *  @param [in,out]  pMinVal        : Pointer to Minimum value in the image
 *  @param [in,out]  pMaxVal        : Pointer to Maximum value in the image
 *  @param [in,out]  pMinCount      : Ponter to total number of detected minimum values in the image (optional)
 *  @param [in,out]  pMaxCount      : Ponter to total number of detected maximum values in the image (optional)
 *  @param [in]      pMinLocCapacity : Allocated array size of minLoc array.
 *  @param [in]      pMaxLocCapacity :  Allocated array size of maxLoc array.
 *  @param [in,out]  pMinLoc[]      : Pointer to coordinates of minimum values in the image (x,y) (optional)
 *  @param [in,out]  pMaxLoc[]      : Pointer to coordinates of maximum values in the image (x,y) (optional)
 *  @param [in]      pStartX        : Starting x coordinate in the image corresponding to the src pointer
 *  @param [in]      pStartY        : Starting y coordinate in the image corresponding to the src pointer
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
VXLIB_STATUS VXLIB_minMaxLoc_exec_ci(VXLIB_kernelHandle handle,
                                     void *restrict     pIn,
                                     void *restrict     pMinVal,
                                     void *restrict     pMaxVal,
                                     void *restrict     pMinCount,
                                     void *restrict     pMaxCount,
                                     void *restrict     pMinLocCapacity,
                                     void *restrict     pMaxLocCapacity,
                                     void *restrict     pMinLoc,
                                     void *restrict     pMaxLoc,
                                     void *restrict     pStartX,
                                     void *restrict     pStartY);

/**
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref VXLIB_minMaxLoc_exec.
 *
 * @details
 *
 *  @param [in]      handle         : Active handle to the kernel
 *  @param [in]      pIn[]          : Pointer to array containing input image
 *  @param [in,out]  pMinVal        : Pointer to Minimum value in the image
 *  @param [in,out]  pMaxVal        : Pointer to Maximum value in the image
 *  @param [in,out]  pMinCount      : Ponter to total number of detected minimum values in the image (optional)
 *  @param [in,out]  pMaxCount      : Ponter to total number of detected maximum values in the image (optional)
 *  @param [in]      pMinLocCapacity : Allocated array size of minLoc array.
 *  @param [in]      pMaxLocCapacity :  Allocated array size of maxLoc array.
 *  @param [in,out]  pMinLoc[]      : Pointer to coordinates of minimum values in the image (x,y) (optional)
 *  @param [in,out]  pMaxLoc[]      : Pointer to coordinates of maximum values in the image (x,y) (optional)
 *  @param [in]      pStartX        : Starting x coordinate in the image corresponding to the src pointer
 *  @param [in]      pStartY        : Starting y coordinate in the image corresponding to the src pointer
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * VXLIB_STATUS.
 *
 */
template <typename dType>
extern VXLIB_STATUS VXLIB_minMaxLoc_exec_cn(VXLIB_kernelHandle handle,
                                            void *restrict     pIn,
                                            void *restrict     pMinVal,
                                            void *restrict     pMaxVal,
                                            void *restrict     pMinCount,
                                            void *restrict     pMaxCount,
                                            void *restrict     pMinLocCapacity,
                                            void *restrict     pMaxLocCapacity,
                                            void *restrict     pMinLoc,
                                            void *restrict     pMaxLoc,
                                            void *restrict     pStartX,
                                            void *restrict     pStartY);

/**
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /** @brief Initargs of the kernel **/
   VXLIB_minMaxLoc_InitArgs pKerInitArgs;

   /** @brief Function pointer to point to the right execution variant between
    *         @ref VXLIB_minMaxLoc_exec_cn and
    *         @ref VXLIB_minMaxLoc_exec_ci.                        */
   pFxnVXLIB_minMaxLoc_exec execute;

   /** @brief Width of image  **/
   uint32_t width;
   
   /** @brief Height of image  **/
   uint32_t height;

   /** @brief Stride of input0 in elements **/
   uint32_t strideInElements;

   /** @brief Stride of output in elements **/
   uint32_t strideMinLocElements;
   
   /** @brief Stride of output in elements **/
   uint32_t strideMaxLocElements;

   uint32_t oneVecBlocks;
   uint32_t firstBlockWidth;
   uint32_t remainingCols;

   /** @brief Number of blocks with exact multiple of 2 * eleCount **/
   uint32_t firstBlockLoopCnt;

   /** @brief number of remaining blocks **/
   uint32_t secondBlockLoopCnt;

   bool isOddHeight;

   /** @brief Array to hold SE/SA params */
   uint8_t bufPblock[VXLIB_MINMAXLOC_IXX_IXX_OXX_PBLOCK_SIZE];
} VXLIB_minMaxLoc_PrivArgs;

/**
 *  @brief This function is the initialization function for the natural C
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref VXLIB_minMaxLoc_init.
 *
 * @details
 *
 *  @param [in]  handle            :  Active handle to the kernel
 *  @param [in]  bufParamsIn       :  Pointer to the structure containing dimensional
 *                                    information of input image
 *  @param [out] bufParamsMinLoc   :  Pointer to the structure containing dimensional
 *                                    information of minLoc
 *  @param [out] bufParamsMaxLoc   :  Pointer to the structure containing dimensional
 *                                    information of maxLoc
 *  @param [in]  pKerInitArgs      :  Pointer to the structure holding init
 *                                    parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref
 * VXLIB_STATUS.
 *
 */
VXLIB_STATUS
VXLIB_minMaxLoc_init_cn(VXLIB_kernelHandle              handle,
                        VXLIB_bufParams2D_t *           bufParamsIn,
                        VXLIB_bufParams1D_t *           bufParamsMinLoc,
                        VXLIB_bufParams1D_t *           bufParamsMaxLoc,
                        const VXLIB_minMaxLoc_InitArgs *pKerInitArgs);

#endif /* VXLIB_MINMAXLOC_IXX_IXX_OXX_PRIV_H_ */
/* ======================================================================== */
/*  End of file:  VXLIB_minMaxLoc_priv.h                                         */
/* ======================================================================== */
