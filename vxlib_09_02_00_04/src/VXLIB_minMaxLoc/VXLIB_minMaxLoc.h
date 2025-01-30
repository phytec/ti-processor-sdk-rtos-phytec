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

#ifndef VXLIB_MINMAXLOC_IXX_IXX_OXX_H_
#define VXLIB_MINMAXLOC_IXX_IXX_OXX_H_

#include "../common/VXLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup VXLIB_KERNELS
 */

/**
 * @defgroup VXLIB_minMaxLoc VXLIB_minMaxLoc
 * @brief Kernel for finding minimum and maximum values in an image and a location for each.
 *
 *
 * @details
 *       - The minimum and maximum is found in a first pass through the image.
 *       - The minCount and maxCounts are optionally found in a second pass through the image.
 *       - The minLoc and maxLoc list of pixel coordinates are optionally found in a third pass
 *         through the image.
 *
 * @par Method
 * @verbatim

    minVal = min(src(x',y')) where 0 <= x' < width, 0 <= y' < height
    maxVal = max(src(x',y')) where 0 <= x' < width, 0 <= y' < height

   @endverbatim
 *     
 *   OpenVX Function Reference Page : https://www.khronos.org/registry/vx/specs/1.1/html/d8/d05/group__group__vision__function__minmaxloc.html
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
} VXLIB_minMaxLoc_InitArgs;

/**
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */

int32_t VXLIB_minMaxLoc_getHandleSize(VXLIB_minMaxLoc_InitArgs *pKerInitArgs);

/**
 *  @brief       This function should be called before the
 *               @ref VXLIB_minMaxLoc_exec function is called. This
 *               function takes care of any one-time operations such as setting
 * up the configuration of required hardware resources such as the streaming engine.  The results of these
 *               operations are stored in the handle.
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
 *  @return      Status value indicating success or failure. Refer to @ref VXLIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle.
 */

VXLIB_STATUS VXLIB_minMaxLoc_init(VXLIB_kernelHandle              handle,
                                  VXLIB_bufParams2D_t *           bufParamsIn,
                                  VXLIB_bufParams1D_t *           bufParamsMinLoc,
                                  VXLIB_bufParams1D_t *           bufParamsMaxLoc,
                                  const VXLIB_minMaxLoc_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref VXLIB_minMaxLoc_init function. This function
 *               is called with the same parameters as the
 *               @ref VXLIB_minMaxLoc_init, and this function
 *               must be called before the
 *               @ref VXLIB_minMaxLoc_init is called.
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
 *  @return      Status value indicating success or failure. Refer to @ref VXLIB_STATUS.
 *
 *
 *  @remarks     None
 */

VXLIB_STATUS
VXLIB_minMaxLoc_init_checkParams(VXLIB_kernelHandle              handle,
                                 const VXLIB_bufParams2D_t *     bufParamsIn,
                                 const VXLIB_bufParams1D_t *     bufParamsMinLoc,
                                 const VXLIB_bufParams1D_t *     bufParamsMaxLoc,
                                 const VXLIB_minMaxLoc_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref VXLIB_minMaxLoc_exec function. This function
 *               is called with the same parameters as the
 *               @ref VXLIB_minMaxLoc_exec, and this function
 *               must be called before the
 *               @ref VXLIB_minMaxLoc_exec is called.
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
 *  @return      Status value indicating success or failure. Refer to @ref VXLIB_STATUS.
 *
 *  @remarks     None
 */

VXLIB_STATUS VXLIB_minMaxLoc_exec_checkParams(VXLIB_kernelHandle   handle,
                                              const void *restrict pIn,
                                              const void *restrict pMinVal,
                                              const void *restrict pMaxVal,
                                              const void *restrict pMinCount,
                                              const void *restrict pMaxCount,
                                              const void *restrict pMinLocCapacity,
                                              const void *restrict pMaxLocCapacity,
                                              const void *restrict pMinLoc,
                                              const void *restrict pMaxLoc,
                                              const void *restrict pStartX,
                                              const void *restrict pStartY);

/**
 *  @brief       This function is the main kernel compute function.
 *
 *  @details     Please refer to details under
 *               @ref VXLIB_minMaxLoc_exec
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
 *  @return      Status value indicating success or failure. Refer to @ref VXLIB_STATUS.
 *
 *  @par Assumptions:
 *     - I/O buffer pointers are assumed to be not aliased.
 *     - PARAMETER INITIALIZATION:
 *         - pMinVal and pMaxVal should be externally initialized to numeric MAX and MIN values, respectively,
 *           before calling the function for the first time for each image.
 *         - If the optional parameters pMinCount or pMaxCount are to be passed (not set to NULL), then
 *           they each need to be externally initialized to 0 before calling the function for the first
 *           time for each image.
 *         - The reason the above parameters are not internally initialized is in case a user wants to
 *           divide processing of the image into smaller pieces, and call this function for each piece.
 *           In this case, the external intitialization only needs to happen on the first block of the image, and
 *           the same parameters can be updated internally by subsequent blocks.
 *         - startX and startY may be non-zero when the user sets pMinLoc or pMaxLoc pointers to non-NULL values, AND
 *           divides the processing of the image into smaller pieces.
 *     - PARAMETER DEPENDENCIES:
 *         - If pMinLoc is non-NULL, then pMinCount should be non-NULL so that the user knows how much of the array
 *           was filled.  If the pMinCount returned is > minLocCapacity, then there are only minLocCapacity items
 *           filled in the array.
 *         - If pMaxLoc is non-NULL, then pMaxCount should be non-NULL so that the user knows how much of the array
 *           was filled.  If the pMaxCount returned is > maxLocCapacity, then there are only maxLocCapacity items
 *           filled in the array.
 *         - If minLoc is NULL, then minLocCapacity is ignored by the function
 *         - If maxLoc is NULL, then maxLocCapacity is ignored by the function
 *         - If minLoc and maxLoc are both NULL then startX and startY parameters are also ignored by the function
 *
 *  @par Performance Considerations:
 *     - For best performance, the following parameter settings are recommended:
 *       - The input and output data are expected to be in L2/MSMC memory
 *       - The buffer pointers are assumed to be 64-byte aligned
 *       - The highest performing mode is when all the "optional" parameters are set to NULL, such that only min
 *         and max values are found.
 *       - The second highest performing mode is when the minLoc and maxLoc pointers are set to NULL, such that
 *         only min and max values and counts are found, not the locations.
 *
 *  @remarks     Before calling this function, application is expected to call
 *               @ref VXLIB_minMaxLoc_init and
 *               @ref VXLIB_minMaxLoc_exec_checkParams functions.
 *               This ensures resource configuration and error checks are done
 * only once for several invocations of this function.
 */

VXLIB_STATUS
VXLIB_minMaxLoc_exec(VXLIB_kernelHandle handle,
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
*  @brief       This function estimates the archCycles and estCycles.
*
*  @param [in]  handle         :  Active handle to the kernel
*  @param [in]  bufParamsIn    :  Pointer to the structure containing dimensional
*                                 information of input image
*  @param [in] bufParamsMinLoc :  Pointer to the structure containing dimensional
*                                 information of minLoc
*  @param [in] bufParamsMaxLoc :  Pointer to the structure containing dimensional
*                                 information of maxLoc
*  @param [in]  pKerInitArgs   :  Pointer to the structure holding init
*                                 parameters
*  @param [out] archCycles     :  Arch compute cycles obtained from asm.
*  @param [out] estCycles      :  Estimated cycles including overhead.
*
*  @return      None.
*
*  @par Assumptions:
*    - The function @ref VXLIB_minMaxLoc_init is called prior to this function call 
*    - This function takes into account the estimation for all three steps i.e min/max value, min/max count and min/max location.
* *
*  @remarks     This function is expected to be called when the estimation of cycles is needed.
*/
void VXLIB_minMaxLoc_perfEst(VXLIB_kernelHandle              handle,
                            const VXLIB_bufParams2D_t *     bufParamsIn,
                            const VXLIB_bufParams1D_t *     bufParamsMinLoc,
                            const VXLIB_bufParams1D_t *     bufParamsMaxLoc,
                            const VXLIB_minMaxLoc_InitArgs * pKerInitArgs,
                            size_t *archCycles,
                            size_t *estCycles);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* VXLIB_MINMAXLOC_IXX_IXX_OXX_H_ */

/* ======================================================================== */
/*  End of file:  VXLIB_minMaxLoc.h                                         */
/* ======================================================================== */
