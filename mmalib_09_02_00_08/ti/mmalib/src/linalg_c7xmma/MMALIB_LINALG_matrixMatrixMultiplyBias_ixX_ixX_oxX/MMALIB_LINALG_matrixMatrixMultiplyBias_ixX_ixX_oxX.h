/******************************************************************************/
/*!
 * \file MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX.h
 */
/* Copyright (C) 2017 Texas Instruments Incorporated - http://www.ti.com/
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

#ifndef MMALIB_LINALG_MATRIXMATRIXMULTIPLYBIAS_IXX_IXX_OXX_H_
#define MMALIB_LINALG_MATRIXMATRIXMULTIPLYBIAS_IXX_IXX_OXX_H_

#include "../../common/MMALIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// clang-format off
/*!
 * @defgroup MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX
 * @brief Kernel for multiplying two matrices with bias, scale and shift
 * 
 * @details
 *          - The kernel takes an (m x k) matrix and multiplies it with a (k x n) matrix to produce an (m x n) matrix.
 *          - C = AB
 * 
 * @ingroup  MMALIB_LINALG */
/**@{**/

// clang-format on
/**
 *  @enum       MMALIB_LINALG_MATRIXMATRIXMULTIPLYBIAS_IXX_IXX_OXX_STATUS_NAME
 *  @brief      Enumeration of different error codes for the
 * MMALIB_LINALG_matrixMatrixMultiply kernel
 *
 */

typedef enum {
   MMALIB_LINALG_MATRIXMATRIXMULTIPLYBIAS_IXX_IXX_OXX_ERR_SMALL_K = MMALIB_ERROR_MAX,
   /*@todo Add more appropriate error codes */
   MMALIB_LINALG_MATRIXMATRIXMULTIPLYBIAS_IXX_IXX_OXX_ERR_MAX
} MMALIB_LINALG_MATRIXMATRIXMULTIPLYBIAS_IXX_IXX_OXX_STATUS_NAME;

typedef enum
{
    MMALIB_LINALG_MATRIXMATRIXMULTIPLYBIAS_IXX_IXX_OXX_B_NON_TRANSPOSED  = 0,
    MMALIB_LINALG_MATRIXMATRIXMULTIPLYBIAS_IXX_IXX_OXX_B_TRANSPOSED = 1
} MMALIB_LINALG_MATRIXMATRIXMULTIPLYBIAS_IXX_IXX_OXX_B_TRANSPOSE_STATUS;

typedef enum
{
    MMALIB_LINALG_MATRIXMATRIXMULTIPLYBIAS_IXX_IXX_OXX_BIAS_ORDER_ROW  = 0,
    MMALIB_LINALG_MATRIXMATRIXMULTIPLYBIAS_IXX_IXX_OXX_BIAS_ORDER_COL = 1
} MMALIB_LINALG_MATRIXMATRIXMULTIPLYBIAS_IXX_IXX_OXX_BIAS_ORDER;

typedef enum
{
    MMALIB_LINALG_MATRIXMATRIXMULTIPLYBIAS_IXX_IXX_OXX_SCALE_SHIFT_ORDER_ROW  = 0,
    MMALIB_LINALG_MATRIXMATRIXMULTIPLYBIAS_IXX_IXX_OXX_SCALE_SHIFT_ORDER_COL = 1
} MMALIB_LINALG_MATRIXMATRIXMULTIPLYBIAS_IXX_IXX_OXX_SCALE_SHIFT_ORDER;

typedef enum
{
    MMALIB_LINALG_MATRIXMATRIXMULTIPLYBIAS_IXX_IXX_OXX_SCALE_SHIFT_SCALAR = 0,
    MMALIB_LINALG_MATRIXMATRIXMULTIPLYBIAS_IXX_IXX_OXX_SCALE_SHIFT_VECTOR = 1
} MMALIB_LINALG_MATRIXMATRIXMULTIPLYBIAS_IXX_IXX_OXX_SCALE_SHIFT_ENABLE_STATUS;

/**
 * @struct MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_InitArgs
 */

typedef struct {
   /*! @brief Variant of the function refer to @ref MMALIB_FUNCTION_STYLE     */
   int8_t funcStyle;
   /*! @brief Output activation type: Saturation or ReLU 
              @ref MMALIB_activation_type_e */
   int8_t activationType;
   /*! @brief Max value for paramterized saturation */
   int32_t pSatMax;
   /*! @brief Min value for paramterized saturation */
   int32_t pSatMin;
   /*! @brief flag signifies whether the B matrix is transposed or not
              @ref MMALIB_LINALG_MATRIXMATRIXMULTIPLYBIAS_IXX_IXX_OXX_B_TRANSPOSE_STATUS */
   int8_t bTranspose;
   /*! @brief flag to indicate bias order (row vector or column vector) 
              @ref MMALIB_LINALG_MATRIXMATRIXMULTIPLYBIAS_IXX_IXX_OXX_BIAS_ORDER*/
   int8_t biasOrder;
   /*! @brief flag to indicate whether the scale and shift are vectors or scalars
              @ref MMALIB_LINALG_MATRIXMATRIXMULTIPLYBIAS_IXX_IXX_OXX_SCALE_SHIFT_ENABLE_STATUS*/
   bool scaleAndShiftFlag;
   /*! @brief flag to indicate scale and shift order (row vector or column vector) 
              @ref MMALIB_LINALG_MATRIXMATRIXMULTIPLYBIAS_IXX_IXX_OXX_SCALE_SHIFT_ORDER*/
   int8_t scaleShiftOrder;
} MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_InitArgs;

/**
 *  @brief        This is a query function to return the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it during init and exec function calls
 */

int32_t MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_getHandleSize (
    MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_InitArgs *pKerInitArgs);

/**
 *  @brief       This function call is required to initialize the handle. In
 *               this function most of the one time operations are performed
 *               and results are stored in the handle
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  src0_addr    :  Pointer to the structure (@ref MMALIB_bufParams3D_t) containing dimensional
 *                               information of src0 (i.e. A)
 *  @param [in]  src1_addr    :  Pointer to the structure (@ref MMALIB_bufParams3D_t) containing dimensional
 *                               information of src1 (i.e. B)
 *  @param [in]  src2_addr    :  Pointer to the structure containing dimensional
 *                               information of src2, which is the bias vector
 *  @param [in]  src3_addr    :  Pointer to the structure containing dimensional
 *                               information of src3, which is the scale vector
 *  @param [out] dst_addr     :  Pointer to the structure containing dimensional
 *                               information of dst (i.e. C)
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure.
 *               Refer to @ref MMALIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle
 */

MMALIB_STATUS MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_init (
    MMALIB_kernelHandle                                       handle,
    const MMALIB_bufParams3D_t                               *src0_addr,
    const MMALIB_bufParams3D_t                               *src1_addr,
    const MMALIB_bufParams2D_t                               *src2_addr,
    const MMALIB_bufParams2D_t                               *src3_addr,
    const MMALIB_bufParams3D_t                               *dst_addr,
    const MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the parameters and should be called
 *               before kernel execution. It can be called once
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  src0_addr    :  Pointer to the structure (@ref MMALIB_bufParams3D_t) containing dimensional
 *                               information of src0 (i.e. A)
 *  @param [in]  src1_addr    :  Pointer to the structure (@ref MMALIB_bufParams3D_t) containing dimensional
 *                               information of src1 (i.e. B)
 *  @param [in]  src2_addr    :  Pointer to the structure containing dimensional
 *                               information of src2, which is the bias vector
 *  @param [out] dst_addr     :  Pointer to the structure containing dimensional
 *                               information of dst (i.e. C)
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 * 
 *  @return      Status value indicating success or failure.
 *               Refer to @ref MMALIB_STATUS.
 *
 *  @remarks     None
 */

MMALIB_STATUS MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_init_checkParams (
    MMALIB_kernelHandle                                       handle,
    const MMALIB_bufParams3D_t                               *src0_addr,
    const MMALIB_bufParams3D_t                               *src1_addr,
    const MMALIB_bufParams2D_t                               *src2_addr,
    const MMALIB_bufParams3D_t                               *dst_addr,
    const MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_InitArgs *pKerInitArgs);

/**
 *  @brief       This function is the main compute function, and performs
 *               matrix-matrix multiplication.
 *
 *  @details     The flow and the expectations of this function are as follows
 *
 *  @param [in]  handle           : Active handle to the kernel
 *  @param [in]  src0[]           : Pointer to the structure (@ref MMALIB_bufParams3D_t) containing dimensional
 *                                  information of src0 (i.e. A)
 *  @param [in]  src1[]           : Pointer to the structure (@ref MMALIB_bufParams3D_t) containing dimensional
 *                                  information of src1 (i.e. B)
 *  @param [in]  src2[]           : Pointer to buffer holding the bias vector
 *  @param [in]  src3[]           : Pointer to buffer holding the scale vector
 *  @param [in]  src4[]           : Pointer to buffer holding the shift vector
 *  @param [out] dst[]            : Pointer to buffer holding the output matrix
 *                                  [ C matrix]
 * 
 *  @return      Status value indicating success or failure.
 *               Refer to @ref MMALIB_STATUS.
 *
 *  @remarks     Application is expected to call the checkParams function prior
 *               to this function as it avoids check of paramaters for each
 *               invocation.
 */

MMALIB_STATUS MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_exec (
    MMALIB_kernelHandle                                         handle,
    const void                                                 *src0,
    const void                                                 *src1,
    const void                                                 *src2,
    const void                                                 *src3,
    const void                                                 *src4,
    void                                                       *dst);

/**
 *  @brief       This function checks the parameters and should be called
 *               before kernel execution. It can be called once
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  src0[]      : Pointer to buffer holding the first matrix input
 *                             [ A matrix]
 *  @param [in]  src1[]      : Pointer to buffer holding the second matrix input
 *                             [ B matrix]
 *  @param [in]  src2[]      : Pointer to buffer holding the bias vector
 *  @param [out] dst[]       : Pointer to buffer holding the output matrix
 *                             [ C matrix]

 *  @return      Status value indicating success or failure.
 *               Refer to @ref MMALIB_STATUS.
 *
 *  @remarks     None
 */

MMALIB_STATUS
MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_exec_checkParams (
    MMALIB_kernelHandle handle,
    const void         *src0,
    const void         *src1,
    const void         *src2,
    const void         *dst);

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
 *  @param [out] dst_addr     :  Pointer to the structure containing dimensional
 *                               information of dst
 *  @param [out] idealCycles  :  Cycles estimated for the compute, ideally
 *  @param [out] archCycles   :  Cycles estimated for the compute, startup and
 *                               teardown
 *  @param [out] estCycles    :  Cycles estimated for the compute, startup,
 *                               teardown and any associated overhead
 *  @param [out] caseNumber   :  The case (execution path) taken insided the
 *                               execution of the kernel
 *  @remarks     None
 *******************************************************************************
 */

void MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_perfEst (
    MMALIB_kernelHandle         handle,
    const MMALIB_bufParams3D_t *src0_addr,
    const MMALIB_bufParams3D_t *src1_addr,
    const MMALIB_bufParams3D_t *dst_addr,
    uint64_t                   *idealCycles,
    uint64_t                   *archCycles,
    uint64_t                   *estCycles,
    int32_t                    *caseNumber);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* MMALIB_LINALG_MATRIXMATRIXMULTIPLYBIAS_IXX_IXX_OXX_H_ */

/* ======================================================================== */
/*  End of file:  MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX.h          */
/* ======================================================================== */
