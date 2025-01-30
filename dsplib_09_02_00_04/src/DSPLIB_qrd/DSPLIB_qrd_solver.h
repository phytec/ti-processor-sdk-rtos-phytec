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

#ifndef DSPLIB_QRD_SOLVER_IXX_IXX_OXX_H_
#define DSPLIB_QRD_SOLVER_IXX_IXX_OXX_H_

#include "../common/DSPLIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @defgroup DSPLIB_qrd_solver DSPLIB_qrd_solver
 * @brief Kernel for find the solution of linear equation using Q and R matrices.
 *
 *  @details     This function solves the system of linear equations \f$A \times x=b\f$ for 
 *               \f$x\f$ using the input matrices \f$Q\f$ and \f$R\f$ produced via 
 *               QR Decomposition.
 *               The following procedure is performed:
 *                1. Use matrix multiplication to generate \f$y\f$: \f$y = Q^{T} \times b\f$<BR>
 *                2. Use backward substitution to solve for \f$x\f$: \f$x = R^{-1} \times y \f$<BR>
 *
 * @ingroup  DSPLIB
 */

/**@{*/

/*!
 * @brief Structure containing the parameters to initialize the kernel
 */
typedef struct {
   /*! @brief Variant of the function refer to @ref DSPLIB_FUNCTION_STYLE     */
   int8_t funcStyle;
   /*! @brief Size of input data
              - dimX => Number of columns */
   uint32_t dimX;
   /*! @brief Size of input data
               - dimY => number of rows */
   uint32_t dimY;
} DSPLIB_qrdSolverInitArgs;

/*!
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */
int32_t DSPLIB_qrd_solver_getHandleSize(DSPLIB_qrdSolverInitArgs *pKerInitArgs);

/*!
 *  @brief       This function should be called before the
 *               @ref DSPLIB_qrd_solver_exec function is called. This
 *               function takes care of any one-time operations such as setting up
 *               the configuration of required hardware resources such as the
 *               streaming engine and streaming address generator. The results of these
 *               operations are stored in the handle.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsQ   :  Pointer to the structure containing dimensional
 *                               information of Q matrix
 *  @param [in]  bufParamsR   :  Pointer to the structure containing dimensional
 *                               information of R matrix
 *  @param [in]  bufParamsB   :  Pointer to the structure containing dimensional
 *                               information of input vector B
 *  @param [in]  bufParamsY   :  Pointer to the structure containing dimensional
 *                               information of temporary vector Y
 *  @param [out] bufParamsX   :  Pointer to the structure containing dimensional
 *                               information of ouput vector X
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle.
 */
DSPLIB_STATUS DSPLIB_qrd_solver_init(DSPLIB_kernelHandle             handle,
                                     DSPLIB_bufParams2D_t           *bufParamsQ,
                                     DSPLIB_bufParams2D_t           *bufParamsR,
                                     DSPLIB_bufParams1D_t           *bufParamsB,
                                     DSPLIB_bufParams1D_t           *bufParamsY,
                                     DSPLIB_bufParams1D_t           *bufParamsX,
                                     const DSPLIB_qrdSolverInitArgs *pKerInitArgs);

/*!
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_qrd_solver_init function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_qrd_solver_init, and this function
 *               must be called before the
 *               @ref DSPLIB_qrd_solver_init is called.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsQ   :  Pointer to the structure containing dimensional
 *                               information of Q matrix
 *  @param [in]  bufParamsR   :  Pointer to the structure containing dimensional
 *                               information of R matrix
 *  @param [in]  bufParamsB   :  Pointer to the structure containing dimensional
 *                               information of input vector B
 *  @param [in]  bufParamsY   :  Pointer to the structure containing dimensional
 *                               information of temporary vector Y
 *  @param [out] bufParamsX   :  Pointer to the structure containing dimensional
 *                               information of ouput vector X
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS DSPLIB_qrd_solver_init_checkParams(DSPLIB_kernelHandle             handle,
                                                 DSPLIB_bufParams2D_t           *bufParamsQ,
                                                 DSPLIB_bufParams2D_t           *bufParamsR,
                                                 DSPLIB_bufParams1D_t           *bufParamsB,
                                                 DSPLIB_bufParams1D_t           *bufParamsY,
                                                 DSPLIB_bufParams1D_t           *bufParamsX,
                                                 const DSPLIB_qrdSolverInitArgs *pKerInitArgs);

/*!
 *  @brief       This function checks the validity of the parameters passed to
 *               @ref DSPLIB_qrd_solver_exec function. This function
 *               is called with the same parameters as the
 *               @ref DSPLIB_qrd_solver_exec, and this function
 *               must be called before the
 *               @ref DSPLIB_qrd_solver_exec is called.
 *
 *  @param [in]  handle :  Active handle to the kernel.
 *  @param [in]  pQ     :  Pointer to the input matrix Q.
 *  @param [in]  pR     :  Pointer to the input matrix R.
 *  @param [in]  pB     :  Pointer to the input vector B.
 *  @param [in]  pY     :  Pointer to the temporary vector Y.
 *  @param [out] pX     :  Pointer to the output vector X.
 *  @param [in]  pD     :  Pointer to the Scratch buffer D.
 *  @param [in]  pR1    :  Pointer to the Scratch buffer for transposed matrix of R.
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @remarks     None
 */
DSPLIB_STATUS DSPLIB_qrd_solver_exec_checkParams(DSPLIB_kernelHandle handle,
                                                 void *restrict pQ,
                                                 void *restrict pR,
                                                 void *restrict pB,
                                                 void *restrict pY,
                                                 void *restrict pX,
                                                 void *restrict pD,
                                                 void *restrict pR1);

/*!
 *  @brief       This function is the main kernel compute function.
 *
 *  @details     Please refer to details under
 *               @ref DSPLIB_qrd_solver_exec
 *
 *  @param [in]  handle :  Active handle to the kernel.
 *  @param [in]  pQ     :  Pointer to the input matrix Q.
 *  @param [in]  pR     :  Pointer to the input matrix R.
 *  @param [in]  pB     :  Pointer to the input vector B.
 *  @param [in]  pY     :  Pointer to the temporary vector Y.
 *  @param [out] pX     :  Pointer to the output vector X.
 *  @param [in]  pD     :  Pointer to the Scratch buffe D.
 *  @param [in]  pR1    :  Pointer to the Scratch buffer for transposed matrix of R.
 * 
 *  @par Memory Requirements 
 * | Buffer | dimY | dimX | Comments |
 * | :--    | :--: | :--: | :-----|
 * | pQ     | M    | M    | Input Square Matrix |
 * | pR     | M    | M    | Input Square Matrix |
 * | pB     | 1    | M    | Input Vector |
 * | pY     | 1    | M    | Intermediate solution vector |
 * | pX     | 1    | M    | Output Vector |
 * | pD     | 1    | M    | Scratch buffer to store reciprocals of diagonal elements |
 * | pR1    | M    | M    | Scratch buffer to store transpose of R |
 * 
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @par Assumptions:
 *    - None
 *
 *  @par Performance Considerations:
 *    For best performance,
 *    - The input and output data buffers are expected to be in L2 memory
 *    - The buffer pointers are assumed to be 64-byte aligned
 *    - Ensure that the starting point of adjacent rows in a 2-D matrix are allocated from
 *      different banks in L2 memory with the help of memory padding.  
 * 
 *  @remarks     Before calling this function, application is expected to call
 *               @ref DSPLIB_qrd_solver_init and
 *               @ref DSPLIB_qrd_solver_exec_checkParams functions.
 *               This ensures resource configuration and error checks are done only
 *               once for several invocations of this function.
 */
DSPLIB_STATUS DSPLIB_qrd_solver_exec(DSPLIB_kernelHandle handle,
                                     void *restrict pQ,
                                     void *restrict pR,
                                     void *restrict pB,
                                     void *restrict pY,
                                     void *restrict pX,
                                     void *restrict pD,
                                     void *restrict pR1);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* DSPLIB_QRD_SOLVER_IXX_IXX_OXX_H_ */

/* ======================================================================== */
/*  End of file:  DSPLIB_qrd_solver.h                   */
/* ======================================================================== */
