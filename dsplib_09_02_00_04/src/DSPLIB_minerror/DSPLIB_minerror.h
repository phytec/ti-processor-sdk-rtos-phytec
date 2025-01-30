/*******************************************************************************
**+--------------------------------------------------------------------------+**
**|                            ****                                          |**
**|                            ****                                          |**
**|                            ******o***                                    |**
**|                      ********_///_****                                   |**
**|                      ***** /_//_/ ****                                   |**
**|                       ** ** (__/ ****                                    |**
**|                           *********                                      |**
**|                            ****                                          |**
**|                            ***                                           |**
**|                                                                          |**
**|        Copyright (c) 2016 Texas Instruments Incorporated                 |**
**|                                                                          |**
**|              All rights reserved not granted herein.                     |**
**|                                                                          |**
**|                         Limited License.                                 |**
**|                                                                          |**
**|  Texas Instruments Incorporated grants a world-wide, royalty-free,       |**
**|  non-exclusive license under copyrights and patents it now or            |**
**|  hereafter owns or controls to make, have made, use, import, offer to    |**
**|  sell and sell ("Utilize") this software subject to the terms herein.    |**
**|  With respect to the foregoing patent license, such license is granted   |**
**|  solely to the extent that any such patent is necessary to Utilize the   |**
**|  software alone.  The patent license shall not apply to any              |**
**|  combinations which include this software, other than combinations       |**
**|  with devices manufactured by or for TI ("TI Devices").  No hardware     |**
**|  patent is licensed hereunder.                                           |**
**|                                                                          |**
**|  Redistributions must preserve existing copyright notices and            |**
**|  reproduce this license (including the above copyright notice and the    |**
**|  disclaimer and (if applicable) source code license limitations below)   |**
**|  in the documentation and/or other materials provided with the           |**
**|  distribution                                                            |**
**|                                                                          |**
**|  Redistribution and use in binary form, without modification, are        |**
**|  permitted provided that the following conditions are met:               |**
**|                                                                          |**
**|    *  No reverse engineering, decompilation, or disassembly of this      |**
**|  software is permitted with respect to any software provided in binary   |**
**|  form.                                                                   |**
**|                                                                          |**
**|    *  any redistribution and use are licensed by TI for use only with    |**
**|  TI Devices.                                                             |**
**|                                                                          |**
**|    *  Nothing shall obligate TI to provide you with source code for      |**
**|  the software licensed and provided to you in object code.               |**
**|                                                                          |**
**|  If software source code is provided to you, modification and            |**
**|  redistribution of the source code are permitted provided that the       |**
**|  following conditions are met:                                           |**
**|                                                                          |**
**|    *  any redistribution and use of the source code, including any       |**
**|  resulting derivative works, are licensed by TI for use only with TI     |**
**|  Devices.                                                                |**
**|                                                                          |**
**|    *  any redistribution and use of any object code compiled from the    |**
**|  source code and any resulting derivative works, are licensed by TI      |**
**|  for use only with TI Devices.                                           |**
**|                                                                          |**
**|  Neither the name of Texas Instruments Incorporated nor the names of     |**
**|  its suppliers may be used to endorse or promote products derived from   |**
**|  this software without specific prior written permission.                |**
**|                                                                          |**
**|  DISCLAIMER.                                                             |**
**|                                                                          |**
**|  THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY      |**
**|  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE       |**
**|  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR      |**
**|  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL TI AND TI'S LICENSORS BE      |**
**|  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR     |**
**|  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF    |**
**|  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR         |**
**|  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,   |**
**|  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE    |**
**|  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,       |**
**|  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                      |**
**+--------------------------------------------------------------------------+**
*******************************************************************************/

#ifndef DSPLIB_MINERROR_IXX_IXX_OXX_H_
#define DSPLIB_MINERROR_IXX_IXX_OXX_H_

#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @defgroup DSPLIB_minerror DSPLIB_minerror
 * @brief Performs a dot product on 256 pairs of 9 element vectors and
 * searches for the pair of vectors which produces the maximum dot
 * product result and returns the value of the highest dot product.
 * This is a large part of the Vector sum excited linear prediction (VSELP) vocoder codebook search.
 *
 * @details
 *             - The kernel support signed and unsigned integer data types 8bits
 *                through 64 bits and floating point data types of 32 and 64 bits
 *
 * @ingroup  DSPLIB
 */
/**@{*/

/*!
 * @brief Structure containing the parameters to initialize the kernel
 */
typedef struct {
   /*! @brief Variant of the function refer to @ref DSPLIB_FUNCTION_STYLE      */
   int8_t funcStyle;
   /*! @brief Size of input data                                               */
   uint32_t vecInSize;
   /*! @brief Size of error coefficients vector                                */
   uint32_t errCoefsSize;
   uint32_t strideIn;
} DSPLIB_minerror_InitArgs;

/*!
 *  @brief          This is a query function to calculate the size of internal
 *                  handle
 *  @param [in]    pKerInitArgs  : Pointer to structure holding init parameters
 *  @return         Size of the buffer in bytes
 *  @remarks        Application is expected to allocate buffer of the requested
 *                     size and provide it as input to other functions requiring it.
 */
int32_t DSPLIB_minerror_getHandleSize(DSPLIB_minerror_InitArgs *pKerInitArgs);

/*!
 *  @brief         This function should be called before the
 *                    @ref DSPLIB_minerror_exec function is called. This
 *                    function takes care of any one-time operations such as setting up
 *                    the configuration of required hardware resources such as the
 *                    streaming engine.  The results of these
 *                    operations are stored in the handle.
 *
 *  @param [in]  handle                 :  Active handle to the kernel
 *  @param [in]  bufParamsIn            :  Pointer to the structure containing dimensional
 *                                                 information of input buffer
 *  @param [in]  bufParamsErrCoefs      :  Pointer to the structure containing error coefficients
 *                                                 information of input buffer
 *  @param [out] bufParamsOutIndex      :  Pointer to the structure containing
 *                                                 information of ouput buffer
 *  @param [out] bufParamsOutVal        :  Pointer to the structure containing
 *                                                 information of ouput buffer
 *  @param [in]  pKerInitArgs           :  Pointer to the structure holding init parameters
 *
 *  @return        Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks      Application is expected to provide a valid handle.
 */
DSPLIB_STATUS DSPLIB_minerror_init(DSPLIB_kernelHandle       handle,
                                   DSPLIB_bufParams2D_t     *bufParamsIn,
                                   DSPLIB_bufParams1D_t     *bufParamsErrCoefs,
                                   DSPLIB_bufParams1D_t     *bufParamsOutIndex,
                                   DSPLIB_bufParams1D_t     *bufParamsOutVal,
                                   DSPLIB_minerror_InitArgs *pKerInitArgs);

/*!
 *  @brief         This function checks the validity of the parameters passed to
 *                    @ref DSPLIB_minerror_init function. This function
 *                    is called with the same parameters as the
 *                    @ref DSPLIB_minerror_init, and this function
 *                    must be called before the
 *                    @ref DSPLIB_minerror_init is called.
 *
 *  @param [in]  handle                 :  Active handle to the kernel
 *  @param [in]  bufParamsIn            :  Pointer to the structure containing dimensional
 *                                                 information of input buffer
 *  @param [in]  bufParamsErrCoefs      :  Pointer to the structure containing error coefficients
 *                                                 information of input buffer
 *  @param [out] bufParamsOutIndex      :  Pointer to the structure containing
 *                                                 information of ouput buffer
 *  @param [out] bufParamsOutVal        :  Pointer to the structure containing
 *                                                 information of ouput buffer
 *  @param [in]  pKerInitArgs           :  Pointer to the structure holding init parameters
 *
 *  @return        Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks      None
 */
DSPLIB_STATUS DSPLIB_minerror_init_checkParams(DSPLIB_kernelHandle             handle,
                                               const DSPLIB_bufParams2D_t     *bufParamsIn,
                                               const DSPLIB_bufParams1D_t     *bufParamsErrCoefs,
                                               const DSPLIB_bufParams1D_t     *bufParamsOutIndex,
                                               const DSPLIB_bufParams1D_t     *bufParamsOutVal,
                                               const DSPLIB_minerror_InitArgs *pKerInitArgs);

/*!
 *  @brief         This function checks the validity of the parameters passed to
 *                    @ref DSPLIB_minerror_exec function. This function
 *                    is called with the same parameters as the
 *                    @ref DSPLIB_minerror_init, and this function
 *                    must be called before the
 *                    @ref DSPLIB_minerror_init is called.
 *
 *  @param [in]  handle         :  Active handle to the kernel
 *  @param [in]  pIn            : Pointer to buffer holding the input data
 *  @param [in]  pErrCoefs      : Pointer to buffer holding the error coefficients
 *  @param [out] pMaxIndex      : Pointer to buffer holding the index to the element vector that resulted in the maximum
 * dot product
 *  @param [out] pMaxVal        : Pointer to buffer holding the value of the maximum dot product
 *
 *  @return        Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks      None
 */
DSPLIB_STATUS DSPLIB_minerror_exec_checkParams(DSPLIB_kernelHandle handle,
                                               const void *restrict pIn,
                                               const void *restrict pErrCoefs,
                                               const int *restrict pMaxIndex,
                                               const void *restrict pMaxVal);

/*!
 *  @brief         This function is the main kernel compute function.
 *
 *  @details      Please refer to details under
 *                    @ref DSPLIB_minerror_exec
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [in]  pIn          : Pointer to buffer holding the input data
 *  @param [in]  pErrCoefs    : Pointer to buffer holding the error coefficients
 *  @param [out] pMaxIndex    : Pointer to buffer holding the index to the element vector that resulted in the maximum
 * dot product
 *  @param [out] pMaxVal      : Pointer to buffer holding the value of the maximum dot product
 *  @return        Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @par Assumptions:
 *     - None
 *
 *  @par Performance Considerations:
 *     For best performance,
 *     - the input and output data buffers are expected to be in L2 memory
 *     - the buffer pointers are assumed to be 64-byte aligned
 *
 *  @remarks      Before calling this function, application is expected to call
 *                @ref DSPLIB_minerror_init and
 *                @ref DSPLIB_minerror_exec_checkParams functions.
 *                This ensures resource configuration and error checks are done only
 *                once for several invocations of this function.
 */

DSPLIB_STATUS DSPLIB_minerror_exec(DSPLIB_kernelHandle handle,
                                   void *restrict pIn,
                                   void *restrict pErrCoefs,
                                   int *restrict pMaxIndex,
                                   void *restrict pMaxVal);

/*!
 *  @brief         This function checks the validity of the parameters passed to
 *                    @ref DSPLIB_minerror_exec function. This function
 *                    is called with the same parameters as the
 *                    @ref DSPLIB_minerror_exec, and this function
 *                    must be called before the
 *                    @ref DSPLIB_minerror_exec is called.
 *
 *  @param [in]  handle         : Active handle to the kernel
 *  @param [in]  pIn            : Pointer to buffer holding the input data
 *  @param [in]  pErrCoefs      : Pointer to buffer holding the error coefficients
 *  @param [out] pMaxIndex      : Pointer to buffer holding the index to the element vector that resulted in the maximum
 * dot product
 *  @param [out] pMaxVal        : Pointer to buffer holding the value of the maximum dot product
 *  @return        Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks      None
 */

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_MINERROR_IXX_IXX_OXX_H_ */

/* ======================================================================== */
/*  End of file:  DSPLIB_minerror.h                                         */
/* ======================================================================== */
