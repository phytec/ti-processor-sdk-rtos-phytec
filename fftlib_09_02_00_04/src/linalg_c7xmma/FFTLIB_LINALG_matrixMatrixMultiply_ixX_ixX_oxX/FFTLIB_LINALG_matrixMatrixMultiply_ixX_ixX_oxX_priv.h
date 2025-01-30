/******************************************************************************
*                                                                             *
* module name       :FFTLIB                                                   *
*                                                                             *
* module descripton :Matrix Multiply Accelerator Library module for C7x+MMA   *
*                                                                             *
* Copyright (C) 2017-2018 Texas Instruments Incorporated - http://www.ti.com/ *
* ALL RIGHTS RESERVED                                                         *
*                                                                             *
******************************************************************************/

/********************************************************************************
  *  @file     FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_priv.h
  *
  *  @brief    File to hold private parameters of function FFTLIB_LINALG_matrixMatrixMultiply
  *
  *  @version  0.1 - Jan 2018 : Initial Version
  *            
 ********************************************************************************/

#ifndef FFTLIB_LINALG_MATRIXMATRIXMULTIPLY_IXX_IXX_OXX_PRIV_H_
#define FFTLIB_LINALG_MATRIXMATRIXMULTIPLY_IXX_IXX_OXX_PRIV_H_

#include "FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX.h"
#include "../../common/FFTLIB_utility.h"

/*!
 * \file
 * \brief Header file for kernel's internal use. For the kernel's interface, please
 *        see @ref FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX
 */

/*!
 * @brief Macro to define the size of bufPblock array of
 *        @ref FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_PrivArgs structure.
 *
 */
#define FFTLIB_LINALG_MATRIXMATRIXMULTIPLY_IXX_IXX_OXX_PBLOCK_SIZE (3*SE_PARAM_SIZE + 1*FFTLIB_PARAM_SIZE)


/*!
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec_ci
 *         and @ref FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec_cn.
 */
typedef FFTLIB_STATUS  (*pFxnFFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec)(
                                                                                  FFTLIB_kernelHandle handle,
                                                                                  const void * src0,
                                                                                  const void * src1,
                                                                                  void *dst);

/*!
 @brief  This structure holds all private arguments
 */
typedef struct
{
   FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_InitArgs initArgs;  //!< Structure holding initialization parameters
   pFxnFFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec execute;   //!< Function pointer to the execution variant
   int32_t M;                //!< Number of blocks in the vertical dimension of the first input matrix.  In the case of natural C code, the block size == 1.
   int32_t K;                //!< Number of blocks in the horizontal dimension of the first input matrix.  In the case of natural C code, the block size == 1.
   int32_t N;                //!< Number of blocks in the horizontal dimension of the second input matrix.  In the case of natural C code, the block size == 1.
   
   int32_t strideAElements;  //!< Row stride for the A matrix
   int32_t strideBElements;  //!< Row stride for the B matrix
   int32_t strideCElements;  //!< Row stride for the C matrix
   
   uint8_t bufPblock[FFTLIB_LINALG_MATRIXMATRIXMULTIPLY_IXX_IXX_OXX_PBLOCK_SIZE]; //!< Array to store the configuration prepared by @ref FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_init_ci that will be retrieved and used by @ref FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec_ci
} FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_PrivArgs;


/*!
 *  @brief This function is the initialization function for the optimized C
 *         implementation of the kernel.
 *
 * @details This function determines the configuration for the streaming engine
 *          and MMA hardware resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_init_ci would be
 *          called later independently by the application. When
 *          @ref FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec_ci does not lose cycles
 *          to determine the hardware configuration. The function is templated for
 *          input and output data type. The data types supported are signed a
 *          unsigned 8, 16 and 32 bit fixed point.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  src0_addr    :  Pointer to structure containing dimensional
 *                               information of src0
 *  @param [in]  src1_addr    :  Pointer to structure containing dimensional
 *                               information of src1
 *  @param [out] dst_addr     :  Pointer to structure containing dimensional
 *                               information of dst
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref FFTLIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle.
 */
template <uint32_t dataType>
FFTLIB_STATUS FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_init_ci(FFTLIB_kernelHandle handle,
                                                                     const FFTLIB_bufParams2D_t *src0_addr,
                                                                     const FFTLIB_bufParams2D_t *src1_addr,
                                                                     const FFTLIB_bufParams2D_t *dst_addr,
                                                                     const FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_InitArgs *pKerInitArgs);


/*!
 *  @brief This function is the main execution function for the optimized C7x-MMA
 *         implementation of the matrix-matrix multiply kernel, C = A x B. The function declaration conforms
 *         to the declaration of @ref pFxnFFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec.
 *         The function is templated for
 *             MMA_SIZE corresponding to the data type of matrices A, B and C
 *
 * @details
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  src0         :  Pointer to input matrix A
 *  @param [in]  src1         :  Pointer to input matrix B
 *  @param [out] dst          :  Pointer to output matrix C
 *
 *  @return      Status value indicating success or failure. Refer to @ref FFTLIB_STATUS.
 *
 */
template <int32_t MMA_SIZE>
extern FFTLIB_STATUS FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec_ci(FFTLIB_kernelHandle handle,
                                                                            const void *src0,
                                                                            const void *src1,
                                                                            void *dst);



/*!
 *  @brief This function is the initialization function for the natural C
 *         implementation of the kernel.
 *
 * @details This function populates some of the private fields of @ref FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_PrivArgs
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  src0_addr    :  Pointer to structure containing dimensional
 *                               information of src0
 *  @param [in]  src1_addr    :  Pointer to structure containing dimensional
 *                               information of src1
 *  @param [out] dst_addr     :  Pointer to structure containing dimensional
 *                               information of dst
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref FFTLIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle.
 */
FFTLIB_STATUS FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_init_cn(FFTLIB_kernelHandle handle,
                                                                     const FFTLIB_bufParams2D_t *src0_addr,
                                                                     const FFTLIB_bufParams2D_t *src1_addr,
                                                                     const FFTLIB_bufParams2D_t *dst_addr,
                                                                     const FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_InitArgs *pKerInitArgs);


/*!
 *  @brief This function is the main execution function for the natural
 *         C implementation of the matrix-matrix multiply kernel, C = A x B. The function declaration conforms
 *         to the declaration of @ref pFxnFFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec.
 *         The function is templated for
 *             dataType of matrices A, B and C
 *             accumulateType output accumulation data type
 *
 * @details
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  src0         :  Pointer to input matrix A
 *  @param [in]  src1         :  Pointer to input matrix B
 *  @param [out] dst          :  Pointer to output matrix C
 *
 *  @return      Status value indicating success or failure. Refer to @ref FFTLIB_STATUS.
 *
 */
template <typename dataType, typename accumulateType>
extern FFTLIB_STATUS FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec_cn(FFTLIB_kernelHandle handle,
                                                                            const void *src0,
                                                                            const void *src1,
                                                                            void *dst);

 
#endif /* FFTLIB_LINALG_MATRIXMATRIXMULTIPLY_IXX_IXX_OXX_PRIV_H_ */

/* ======================================================================== */
/*  End of file:  FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX.h                     */
/* ======================================================================== */

