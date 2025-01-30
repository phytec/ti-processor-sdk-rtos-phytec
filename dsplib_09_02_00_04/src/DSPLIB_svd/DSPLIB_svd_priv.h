/******************************************************************************
 *                                                                             *
 * module name       :DSPLIB                                                   *
 *                                                                             *
 * module descripton :Digital Signal Processing Library module for C7x+MMA   *
 *                                                                             *
 * Copyright (C) 2017-2018 Texas Instruments Incorporated - http://www.ti.com/ *
 * ALL RIGHTS RESERVED                                                         *
 *                                                                             *
 ******************************************************************************/

/********************************************************************************
 *  @file     DSPLIB_svd_priv.h
 *
 *  @brief    File to hold private parameter of function DSPLIB_svd
 *
 *  @version  0.1 - Jun 2023 : Initial Version
 *
 ********************************************************************************/

#ifndef DSPLIB_SVD_IXX_IXX_OXX_PRIV_H_
#define DSPLIB_SVD_IXX_IXX_OXX_PRIV_H_

#include "../common/DSPLIB_inlines.h"
#include "../common/DSPLIB_utility.h"
#include "DSPLIB_svd.h"

/*!
 * \file
 * \brief Header file for kernel's internal use. For the kernel's interface, please
 *        see @ref DSPLIB_svd
 */

/*!
 * @brief Macro to define the size of bufPblock array of
 *        @ref DSPLIB_svd_PrivArgs structure.
 *
 */
#define DSPLIB_SVD_IXX_IXX_OXX_PBLOCK_SIZE (28 * SE_PARAM_SIZE)

/*!
 *  @brief This is a function pointer type that conforms to the
 *         declaration of @ref DSPLIB_svd_exec_ci
 *         and @ref DSPLIB_svd_exec_cn.
 */
typedef DSPLIB_STATUS (*pFxnDSPLIB_svd_exec)(DSPLIB_kernelHandle handle,
                                             void *restrict pA,
                                             void *restrict pU,
                                             void *restrict pV,
                                             void *restrict pDiag,
                                             void *restrict pSuperDiag,
                                             void *restrict pU1,
                                             void *restrict pV1,
                                             void *restrict pScratch);

/*!
 *  @brief This function is the initialization function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_svd_init.
 *
 * @details This function determines the configuration for the streaming engine
 *          resources based on the function call parameters,
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, @ref DSPLIB_svd_exec_ci would be
 *          called later independently by the application. When
 *          @ref DSPLIB_svd_exec_ci runs, it merely retrieves
 *          the configuration from the bufPBlock and uses it to set up the
 *          hardware resources. This arrangement is so that
 *          @ref DSPLIB_svd_exec_ci does not lose cycles
 *          to determine the hardware configuration.
 *
 *  @param [in]  handle             : Active handle to the kernel
 *  @param [in]  bufParamsIn        : Pointer to the structure containing dimensional
 *                                    information of input buffer
 *  @param [out] bufParamsU         : Pointer to the structure containing dimensional
 *                                    information of output buffer U
 *  @param [out] bufParamsV         : Pointer to the structure containing dimensional
 *                                    information of output buffer V
 *  @param [out] bufParamsDiag      : Pointer to the structure containing dimensional
 *                                    information of output buffer Diag
 *  @param [out] bufParamsSuperDiag : Pointer to the structure containing dimensional
 *                                    information of output buffer SuperDiag
 *  @param [in]  pKerInitArgs       : Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */

template <typename dataType>
extern DSPLIB_STATUS DSPLIB_svd_init_ci(DSPLIB_kernelHandle         handle,
                                        const DSPLIB_bufParams2D_t *bufParamsIn,
                                        const DSPLIB_bufParams2D_t *bufParamsU,
                                        const DSPLIB_bufParams2D_t *bufParamsV,
                                        const DSPLIB_bufParams1D_t *bufParamsDiag,
                                        const DSPLIB_bufParams1D_t *bufParamsSuperDiag,
                                        const DSPLIB_svdInitArgs   *pKerInitArgs);


/*!
 *  @brief This function is the main execution function for the C7x
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_svd_exec.
 *
 * @details
 * 
 *  @param [in]  handle     : Active handle to the kernel
 *  @param [in]  pA         : Pointer to the input buffer pA
 *  @param [out] pU         : Pointer to the output buffer pU
 *  @param [out] pV         : Pointer to the output buffer pV
 *  @param [out] pDiag      : Pointer to the output buffer pDiag
 *  @param [out] pSuperDiag : Pointer to the output buffer pSuperDiag
 *  @param [in]  pU1        : Pointer to the buffer pU1
 *  @param [in]  pV1        : Pointer to the buffer pV1
 *  @param [out] pScratch   : Pointer to the buffer pScratch
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 *  @par Performance Considerations:
 *       For best performance,
 *        - the input and output data buffers are expected to be in L2 memory.
 *        - the buffer pointers are assumed to be 64-byte aligned
 *
 * @par Assumptions:
 *       The arrays A, U, V, Diag, SuperDiag, U1, V1 and Scratch are stored in distinct arrays
 *       In-place processing of A is not allowed.
 */
template <typename dataType>
extern DSPLIB_STATUS DSPLIB_svd_exec_ci(DSPLIB_kernelHandle handle,
                                        void *restrict pA,
                                        void *restrict pU,
                                        void *restrict pV,
                                        void *restrict pDiag,
                                        void *restrict pSuperDiag,
                                        void *restrict pU1,
                                        void *restrict pV1,
                                        void *restrict pScratch);

/*!
 *  @brief This function is the initialization function for the natural C
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_svd_init.
 *
 * @details
 *
 *  @param [in]  handle             : Active handle to the kernel
 *  @param [in]  bufParamsIn        : Pointer to the structure containing dimensional
 *                                    information of input buffer
 *  @param [out] bufParamsU         : Pointer to the structure containing dimensional
 *                                    information of output buffer U
 *  @param [out] bufParamsV         : Pointer to the structure containing dimensional
 *                                    information of output buffer V
 *  @param [out] bufParamsDiag      : Pointer to the structure containing dimensional
 *                                    information of output buffer Diag
 *  @param [out] bufParamsSuperDiag : Pointer to the structure containing dimensional
 *                                    information of output buffer SuperDiag
 *  @param [in]  pKerInitArgs       : Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */
DSPLIB_STATUS DSPLIB_svd_init_cn(DSPLIB_kernelHandle         handle,
                                 const DSPLIB_bufParams2D_t *bufParamsIn,
                                 const DSPLIB_bufParams2D_t *bufParamsU,
                                 const DSPLIB_bufParams2D_t *bufParamsV,
                                 const DSPLIB_bufParams1D_t *bufParamsDiag,
                                 const DSPLIB_bufParams1D_t *bufParamsSuperDiag,
                                 const DSPLIB_svdInitArgs   *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of @ref DSPLIB_svd_exec.
 *
 * @details
 *
 *  @param [in]  handle     : Active handle to the kernel
 *  @param [in]  pA         : Pointer to the input buffer pA
 *  @param [out] pU         : Pointer to the output buffer pU
 *  @param [out] pV         : Pointer to the output buffer pV
 *  @param [out] pDiag      : Pointer to the output buffer pDiag
 *  @param [out] pSuperDiag : Pointer to the output buffer pSuperDiag
 *  @param [in]  pU1        : Pointer to the buffer pU1
 *  @param [in]  pV1        : Pointer to the buffer pV1
 *  @param [out] pScratch   : Pointer to the buffer pScratch
 *
 *  @return      Status value indicating success or failure. Refer to @ref DSPLIB_STATUS.
 *
 */
template <typename dataType>
extern DSPLIB_STATUS DSPLIB_svd_exec_cn(DSPLIB_kernelHandle handle,
                                        void *restrict pA,
                                        void *restrict pU,
                                        void *restrict pV,
                                        void *restrict pDiag,
                                        void *restrict pSuperDiag,
                                        void *restrict pU1,
                                        void *restrict pV1,
                                        void *restrict pScratch);

/*!
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct {
   /*! @brief Function pointer to point to the right execution variant between
    *         @ref DSPLIB_svd_exec_cn and
    *         @ref DSPLIB_svd_exec_ci.                        */
   pFxnDSPLIB_svd_exec execute;
   /*! @brief Size of input buffer for different batches
    *         @ref DSPLIB_svd_init that will be retrieved
    *         and used by @ref DSPLIB_svd_exec                */
   /*! @brief Width of input data matrix                           */
   uint32_t widthIn;
   /*! @brief Height of input data matrix                          */
   uint32_t heightIn;
   /*! @brief Stride between rows of input data matrix             */
   int32_t strideIn;
   /*! @brief Stride between rows of U matrix            */
   uint32_t strideU;
   /*! @brief Stride between rows of V matrix            */
   uint32_t strideV;
   uint32_t strideURows;
   uint32_t strideVRows;
   /*! @brief Flag for enabling the calculation of reduced form
              enableReducedForm = 1 for reduced form SVD calc
              enableReducedForm = 0 for normal SVD calc            */
   uint32_t enableReducedForm;
   /*! @brief Buffer to save SE & SA configuration parameters      */
   uint8_t bufPblock[DSPLIB_SVD_IXX_IXX_OXX_PBLOCK_SIZE];
} DSPLIB_svd_PrivArgs;

/* *****************************************************************************
 *
 * INTERNAL IMPLEMENTATION FUNCTIONS DECLARATION
 *
 ***************************************************************************** */

template <typename dataType>
void uColProcess(dataType *U,
                 int32_t   Nrows,
                 int32_t   Ncols,
                 int32_t   colUStride,
                 dataType  half_norm_squared,
                 dataType *U1,
                 dataType  scale,
                 uint8_t  *pBlock);

template <typename dataType>
dataType DSPLIB_bidiag_uCol_halfnorm_ci(dataType *U,
                                        int32_t   Nrows,
                                        int32_t   Ncols,
                                        int32_t   colUStride,
                                        dataType *half_norm_squared,
                                        dataType *U1,
                                        dataType *s,
                                        uint8_t  *pBlock);

template <typename dataType>
dataType DSPLIB_bidiag_uRow_halfnorm_ci(dataType *U,
                                        int32_t   Nrows,
                                        int32_t   Ncols,
                                        int32_t   colUStride,
                                        dataType *half_norm_squared,
                                        dataType *U1,
                                        dataType *s,
                                        dataType *superdiag,
                                        uint8_t  *pBlock);

template <typename dataType>
void uRowProcess(dataType *U,
                 int32_t   Nrows,
                 int32_t   Ncols,
                 int32_t   colUStride,
                 dataType *superdiag,
                 dataType *U1,
                 dataType  scale,
                 uint8_t  *pBlock);

template <typename dataType>
void vColProcess(dataType *V,
                 int32_t   Nrows,
                 int32_t   Ncols,
                 int32_t   colVStride,
                 dataType  s,
                 dataType *U1,
                 dataType *U,
                 int32_t   colUStride,
                 uint8_t  *PBlock);

template <typename dataType>
void DSPLIB_bidiag_uFinal_expand_ci(dataType *U, int32_t Nrows, int32_t Ncols, int32_t colUStride, uint8_t *pBlock);

template <typename dataType>
void uSiUpdate(dataType *U,
               int32_t   Nrows,
               int32_t   Ncols,
               int32_t   colUStride,
               dataType  s,
               dataType *U1,
               uint8_t  *pBlock);

template <typename dataType>
void DSPLIB_bidiag_uFinal_normalize_ci(dataType *U, int32_t Nrows, dataType s, int32_t colUStride, uint8_t *pBlock);

template <typename dataType>
void DSPLIB_bidiag_uFinal_initalize_ci(dataType *U,
                                       int32_t   Nrows,
                                       int32_t   Ncols,
                                       int32_t   colUStride,
                                       dataType  s,
                                       dataType *U1,
                                       uint8_t  *pBlock);

template <typename dataType>
void DSPLIB_diag_proc_ci(dataType *V,
                         int32_t   startRow,
                         int32_t   Nrows,
                         int32_t   Ncols,
                         int32_t   rowVStride,
                         dataType *cV,
                         dataType *sV,
                         uint8_t  *pBlock);

template <typename dataType>
void DSPLIB_singularSort_swap_ci(dataType *V,
                                 int32_t   Nrows,
                                 int32_t   Ncols,
                                 int32_t   rowVStride,
                                 int32_t  *sortIndex,
                                 dataType *vBuff,
                                 uint8_t  *pBlock);

template <typename dataType>
void DSPLIB_bidiag_uCol_ci(dataType *U,
                           int32_t   Nrows,
                           int32_t   Ncols,
                           int32_t   colUStride,
                           dataType  half_norm_squared,
                           dataType *U1,
                           dataType  scale,
                           uint8_t  *pBlock);

template <typename dataType>
void DSPLIB_bidiag_uRow_ci(dataType *U,
                           int32_t   Nrows,
                           int32_t   Ncols,
                           int32_t   colUStride,
                           dataType *superdiag,
                           dataType *U1,
                           dataType  scale,
                           uint8_t  *pBlock);

template <typename dataType>
void DSPLIB_bidiag_uFinal_ci(dataType *U,
                             int32_t   Nrows,
                             int32_t   Ncols,
                             int32_t   colUStride,
                             dataType  s,
                             dataType *U1,
                             uint8_t  *pBlock);

template <typename dataType>
void DSPLIB_bidiag_v_ci(dataType *V,
                        int32_t   Nrows,
                        int32_t   Ncols,
                        int32_t   colVStride,
                        dataType  s,
                        dataType *U1,
                        dataType *U,
                        int32_t   colUStride,
                        uint8_t  *pBlock);

template <typename dataType>
void DSPLIB_diag_negate_v_ci(dataType *V, int32_t Ncols, int32_t colVStride, uint8_t *pBlock);

template <typename dataType>
void DSPLIB_diag_epsilon_ci(dataType *diag, dataType *superdiag, dataType *epsilon, int32_t Ncols, uint8_t *pBlock);

template <typename dataType>
void DSPLIB_diag_rotation_check_ci(dataType *diag,
                                   dataType *superdiag,
                                   dataType  epsilon,
                                   int32_t  *m,
                                   int32_t  *rotation_test,
                                   int32_t   Ncols,
                                   uint8_t  *pBlock);

template <typename dataType>
void DSPLIB_diag_rotation_proc_ci(dataType *U,
                                  int32_t   startRow,
                                  int32_t   Nrows,
                                  int32_t   Ncols,
                                  int32_t   rowUStride,
                                  dataType *cU,
                                  dataType *sU,
                                  uint8_t  *pBlock);

template <typename dataType>
void DSPLIB_singularSort_index_ci(dataType *singular_values,
                                  dataType *singularBuffer,
                                  int32_t  *maxIndArr,
                                  int32_t   Ncols,
                                  uint8_t  *pBlock);

template <typename dataType>
void DSPLIB_svd_matTrans_ci(dataType *pIn,
                            dataType *pOut,
                            int32_t   Nrows,
                            int32_t   Ncols,
                            int32_t   colInStride,
                            int32_t   colOutStride,
                            uint8_t  *pBlock);

template <typename dataType>
void DSPLIB_diag_sqrt_ci(dataType *superdiag, dataType *diag, int32_t length, uint8_t *pBlock);

template <typename dataType>
void DSPLIB_svd_blk_move_ci(dataType *pOut,
                            dataType *pIn,
                            int32_t   Nrows,
                            int32_t   Ncols,
                            int32_t   colOutStride,
                            int32_t   colInStride,
                            uint8_t  *pBlock);

/* *****************************************************************************
 *
 * INTERNAL INIT FUNCTIONS DECLARATION
 *
 ***************************************************************************** */

template <typename dataType> void DSPLIB_bidiag_uFinal_init_ci(DSPLIB_kernelHandle handle);

template <typename dataType> void DSPLIB_diag_proc_init_ci(DSPLIB_kernelHandle handle);

template <typename dataType> void DSPLIB_singularSort_swap_init_ci(DSPLIB_kernelHandle handle);

template <typename dataType> void DSPLIB_bidiag_u_init_ci(DSPLIB_kernelHandle handle);

template <typename dataType> void DSPLIB_bidiag_v_init_ci(DSPLIB_kernelHandle handle);

template <typename dataType> void DSPLIB_svd_matTrans_init_ci(DSPLIB_kernelHandle handle);

#endif /* DSPLIB_SVD_IXX_IXX_OXX_PRIV_H_ */

/* ======================================================================== */
/*  End of file:  DSPLIB_svd_priv.h          */
/* ======================================================================== */
