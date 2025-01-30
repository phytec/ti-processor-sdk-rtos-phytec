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

#ifndef MMALIB_LINALG_MATRIXMATRIXMULTIPLYACCUMULATE_IXX_IXX_IXX_OXX_H_
#define MMALIB_LINALG_MATRIXMATRIXMULTIPLYACCUMULATE_IXX_IXX_IXX_OXX_H_

#include "../../common/MMALIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX
 * @brief Kernel for multiplying two matrices with an additive term
 *
 * @details
 *          - The kernel takes an (m x k) matrix and multiplies it with a (k x n) matrix to produce an (m x n) matrix
 *               while adding an (m x n) matrix.
 *          - C = AB + D
 *
 * @ingroup  MMALIB_LINALG 
*/
/**@{*/

/**
 *  @enum       MMALIB_LINALG_MATRIXMATRIXMULTIPLYACCUMULATE_IXX_IXX_IXX_OXX_STATUS_NAME
 *  @brief      Enumeration of different error codes for the MMALIB_LINALG_MATRIXMATRIXMULTIPLY kernel
 *
 */
typedef enum
{
   MMALIB_LINALG_MATRIXMATRIXMULTIPLYACCUMULATE_IXX_IXX_IXX_OXX_ERR_SMALL_K  = MMALIB_ERROR_MAX,
   MMALIB_LINALG_MATRIXMATRIXMULTIPLYACCUMULATE_IXX_IXX_IXX_OXX_ERR_MAX
} MMALIB_LINALG_MATRIXMATRIXMULTIPLYACCUMULATE_IXX_IXX_IXX_OXX_STATUS_NAME;


/**
 ********************************************************************************
 @struct MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_InitArgs
 @brief  This structure holds all the initialization parameters for matrix-
 matrix multiplication.
 *******************************************************************************
 */

typedef struct
{
   int8_t  funcStyle;  //!< Variant of the function; refer to @ref MMALIB_FUNCTION_STYLE
   int8_t  shift;      //!< Output shift value
} MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_InitArgs;


/**
 ********************************************************************************
 *  @brief        This is a query function to return the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it during init and exec function calls
 *******************************************************************************
 */

int32_t MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_getHandleSize(
   MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_InitArgs *pKerInitArgs);


/**
 ********************************************************************************
 *  @brief       This function call is required to initialize the handle. In
 *               this function most of the one time operations are performed
 *               and results are stored in the handle
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  src0_addr    :  Pointer to the structure containing dimensional
 *                               information of src0
 *  @param [in]  src1_addr    :  Pointer to the structure containing dimensional
 *                               information of src1
 *  @param [in]  src2_addr    :  Pointer to the structure containing dimensional
 *                               information of src2
 *  @param [out] dst_addr     :  Pointer to the structure containing dimensional
 *                               information of dst
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters

 *  @return      Status of success or error with error codes, refer to @ref MMALIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle
 *******************************************************************************
 */

MMALIB_STATUS MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_init(MMALIB_kernelHandle handle,
										const MMALIB_bufParams2D_t * src0_addr,
										const MMALIB_bufParams2D_t * src1_addr,
										const MMALIB_bufParams2D_t * src2_addr,
										const MMALIB_bufParams2D_t * dst_addr,
										const MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_InitArgs *pKerInitArgs);


/**
 ********************************************************************************
 *  @brief       This function checks the parameters and should be called
 *               before kernel executuon. It can be called once
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  src0_addr    :  Pointer to the structure containing dimensional
 *                               information of src0
 *  @param [in]  src1_addr    :  Pointer to the structure containing dimensional
 *                               information of src1
 *  @param [in]  src2_addr    :  Pointer to the structure containing dimensional
 *                               information of src2
 *  @param [out] dst_addr     :  Pointer to the structure containing dimensional
 *                               information of dst
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters

   *  @return      Status of success or error with error codes, refer to @ref MMALIB_STATUS.
 *
 *  @remarks     None
 *******************************************************************************
 */

MMALIB_STATUS MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_init_checkParams(MMALIB_kernelHandle handle,
											    const MMALIB_bufParams2D_t * src0_addr,
											    const MMALIB_bufParams2D_t * src1_addr,
											    const MMALIB_bufParams2D_t * src2_addr,
											    const MMALIB_bufParams2D_t * dst_addr,
											    const MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_InitArgs *pKerInitArgs);


/**
 ********************************************************************************
 *  @brief       This function is the main compute function, and performs
 *               matrix-matrix multiplication with accumulate.
 *
 *  @details     Performs C = AB + D
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  src0[]      : Pointer to buffer holding the first matrix input
 *                              [ A matrix]
 *  @param [in]  src1[]      : Pointer to buffer holding the second matrix input
 *                              [ B matrix]
 *  @param [in]  src2[]      : Pointer to buffer holding the third matrix input
 *                              [ D matrix]
 *  @param [out] dst[]       : Pointer to buffer holding the output matrix
 *                              [ C matrix]

   *  @return      Status of success or error with error codes, refer to @ref MMALIB_STATUS.
 *
 *  @par Assumptions:
 *    - I/O buffer pointers are assumed to be not aliased.
 *
 *  @par Performance Considerations:
 *    - For best performance, the following parameter settings are recommended:
 *      - Align all pointers to 8 byte boundaries
 *      - Set all matrix dimensions to a multiple of
 *          - 64 for 8-bit data
 *          - 32 for 16-bit data
 *          - 16 for 32-bit data
 *  @remarks     Application is expected to call the checkParams function prior
 *               to this function as it avoids check of paramaters for each
 *               invocation.
 *******************************************************************************
 */

MMALIB_STATUS MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_exec(MMALIB_kernelHandle handle,
										const void *src0,
										const void *src1,
										const void *src2,
										void *dst);


/**
 ********************************************************************************
 *  @brief       This function checks the parameters and should be called
 *               before kernel executuon. It can be called once
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  src0[]      : Pointer to buffer holding the first matrix input
 *                              [ A matrix]
 *  @param [in]  src1[]      : Pointer to buffer holding the second matrix input
 *                              [ B matrix]
 *  @param [in]  src2[]      : Pointer to buffer holding the second matrix input
 *                              [ D matrix]
 *  @param [out] dst[]       : Pointer to buffer holding the output matrix
 *                              [ C matrix]

  *  @return      Status of success or error with error codes, refer to @ref MMALIB_STATUS.
 *
 *  @remarks     None
 *******************************************************************************
 */

MMALIB_STATUS MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_exec_checkParams(MMALIB_kernelHandle handle,
											    const void *src0,
											    const void *src1,
											    const void *src2,
											    const void *dst);

   /**
    ********************************************************************************
    *  @brief       This function estimates the cycles consumed for the kernel
    *               execution.
    *
    *  @param [in]  handle       :  Active handle to the kernel
    *  @param [in]  src0_addr    :  Pointer to the structure containing dimensional
    *                               information of src0
    *  @param [in]  src1_addr    :  Pointer to the structure containing dimensional
    *                               information of src1
    *  @param [in]  src2_addr    :  Pointer to the structure containing dimensional
    *                               information of src2
    *  @param [out] dst_addr     :  Pointer to the structure containing dimensional
    *                               information of dst
    *  @param [out] archCycles   :  Cycles estimated for the compute, startup and
    *                               teardown
    *  @param [out] estCycles    :  Cycles estimated for the compute, startup,
    *                               teardown and any associated overhead
    *  @param [out] caseNumber   :  The case (execution path) taken insided the
    *                               execution of the kernel
    *
    *  @remarks     None
    *******************************************************************************
    */
   
   void MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_perfEst(MMALIB_kernelHandle handle,
                                                                             const MMALIB_bufParams2D_t *src0_addr,
                                                                             const MMALIB_bufParams2D_t *src1_addr,
                                                                             const MMALIB_bufParams2D_t *src2_addr,
                                                                             const MMALIB_bufParams2D_t *dst_addr,
                                                                             uint64_t *archCycles,
                                                                             uint64_t *estCycles,
                                                                             int32_t *caseNumber);
   
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* MMALIB_LINALG_MATRIXMATRIXMULTIPLYACCUMULATE_IXX_IXX_IXX_OXX_H_ */

/* ======================================================================== */
/*  End of file:  MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX.h          */
/* ======================================================================== */
