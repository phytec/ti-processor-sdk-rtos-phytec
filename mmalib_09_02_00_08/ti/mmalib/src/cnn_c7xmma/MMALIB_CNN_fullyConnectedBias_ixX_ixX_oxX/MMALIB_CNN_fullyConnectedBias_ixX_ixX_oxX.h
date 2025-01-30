/******************************************************************************/
/*!
 * \file MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX.h
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

#ifndef MMALIB_CNN_FULLYCONNECTEDBIAS_IXX_IXX_OXX_H_
#define MMALIB_CNN_FULLYCONNECTEDBIAS_IXX_IXX_OXX_H_

#include "../../common/MMALIB_types.h"
#include "MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_processWeights.h"

#ifdef __cplusplus
extern "C" {
#endif

// clang-format off
/*!
 * @defgroup MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX
 * @brief Kernel provides compute functionality of Fully Connected Layer:
 * \f$ Y^T = X^T \times H^T  + B^T\f$
 * @details
 *  - Kernel requires feature map (\f$ X^T \f$), coefficients (\f$ H^T \f$), and bias (\f$ B^T \f$)
 *    to be available in memory
 *        - 8- and 16-bit datatypes supported
 *        - Supported datatypes for feature map are 8- and 16-bit signed or
 *          unsigned
 *        - Supported datatypes for coefficients are 8- and 16-bit signed
 *        - Supported datatypes for bias are 32- and 64-bit signed
 *        - Supported datatypes for output are 8- and 16-bit signed or unsigned
 *        - This kernel requires specific data arrangement for the kernel matrix
 *          to alleviate bank conflicts in L2 when performing DMA transfers in
 *          conjunction to execution of this kernel
 *          - Desired physical bank-access pattern for SEs and DMA in L2 is
 *            {0,0,1,1,2,2,3,3,0,0,1,1,2,2,3,3, ...}
 *          - Re-ordering functionality of
 *            @ref MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_reorderWeights may
 *            be used
 *          - Below figure shows two examples where @ref
 *            MMALIB_CNN_fullyConnected_ixX_ixX_oxX_InitArgs.Ni (512);
 *            Number of output features = 128 or  104
 * \image html fclBias_h_buffer.svg "Filter Coefficient Buffer" width=1000px
    - Three buffers of type @ref MMALIB_bufParams2D_t : src0 \f$
 \rightarrow X^T
 *  \f$; src1 \f$ \rightarrow H^T \f$; src2 \f$ \rightarrow B^T \f$,
 and dst \f$ \rightarrow Y^T \f$
 * \image html fclBias_buffers.svg "Buffer Arrangement" width=1000px
 *  - The kernel also supports cases when parameter or feature map  matrix
 *          does not fit in L2 memory
 *          - The multiPassH flag in structures
 *            @ref MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_InitArgs.multiPassH
 *            and
 *            @ref
 MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_ExecInArgs.multiPassH
 *            facilitate the feature when parameter matrix does not fit in L2
 *          - The multiPassX flag in structures
 *            @ref MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_InitArgs.multiPassX
 *            and
 *            @ref
 MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_ExecInArgs.multiPassX
 *            facilitate the feature when feature map matrix does not fit in L2
 *          - Table below shows the flag combinations supported:
 *            - Table below shows the flag combinations supported:
 *          <TABLE frame="box" rules="all" cellspacing="0" width="25%"
 *           border="1"  cellpadding="1">
 *              <TR bgcolor="lightgrey">
 *                 <TH><center>  multiPassX </center></TH>
 *                 <TH><center> multiPassH  </center></TH>
 *                 <TH><center> Status      </center></TH>
 *              </TR>
 *              <TR>
 *                 <TD><center> False </center></TD>
 *                 <TD><center> False </center></TD>
 *                 <TD><center> Supported </center></TD>
 *              </TR>
 *              <TR>
 *                 <TD><center> False </center></TD>
 *                 <TD><center> True  </center></TD>
 *                 <TD><center> Supported </center></TD>
 *              </TR>
 *              <TR>
 *                 <TD><center> True  </center></TD>
 *                 <TD><center> False </center></TD>
 *                 <TD><center> Not supported </center></TD>
 *              </TR>
 *              <TR>
 *                 <TD><center> True </center></TD>
 *                 <TD><center> True </center></TD>
 *                 <TD><center> Supported </center></TD>
 *              </TR>
 *          </TABLE>

 * @ingroup  MMALIB_CNN */
/**@{*/

// clang-format on
/**
 *  @enum       MMALIB_CNN_FULLYCONNECTEDBIAS_IXX_IXX_OXX_STATUS_NAME
 *  @brief      Enumeration of different error codes for the
 * MMALIB_CNN_FULLYCONNECTED kernel
 *
 */

typedef enum {
   MMALIB_CNN_FULLYCONNECTEDBIAS_IXX_IXX_OXX_ERR_SMALL_K = MMALIB_ERROR_MAX,
   /*@todo Add more appropriate error codes */
   MMALIB_CNN_FULLYCONNECTEDBIAS_IXX_IXX_OXX_ERR_MAX
} MMALIB_CNN_FULLYCONNECTEDBIAS_IXX_IXX_OXX_STATUS_NAME;

/**
 * @struct MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_InitArgs
 */

typedef struct {
   /*! @brief Variant of the function refer to @ref MMALIB_FUNCTION_STYLE     */
   int8_t funcStyle;
   /*! @brief Output activation type: Saturation or ReLU     */
   int8_t activationType;
   /*! @brief Max value for paramterized saturation */
   int32_t pSatMax;
   /*! @brief Min value for paramterized saturation */
   int32_t pSatMin;
   /*! @brief Value of \f$ N_i\f$ used in natural C code only*/
   int32_t Ni;
   /*! @brief flag signifies feature matrix \f$ (X) \f$ cannot be fit in memory;
    *         therefore, compute is broken down by processing the matrix in
    * chunks */
   bool multiPassX;
   /*! @brief flag signifies kernel matrix \f$ (H) \f$ cannot be fit in memory;
    *         therefore, compute is broken down by processing the matrix in
    * chunks */
   bool multiPassH;
   /*! @brief Buffer pointer to hold partially accumulated values in multi-pass
    * case */
   void *bufCAccum_cn;
#if defined (__SE_FEAT_SPARSITY)
   /*! @brief flag indicates weights are packetized */
   int8_t packetizeMode;
#endif
} MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_InitArgs;

/**
 * @struct MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_ExecInArgs
 * @brief This structure holds all the execution input parameters for the kernel
 */

typedef struct {
   /*! @brief Flag signifies feature matrix \f$ (X) \f$ cannot be fit in memory;
    *         therefore, we process the matrix in chunks */
   bool multiPassX;
   /*! @brief Flag signifies kernel matrix \f$ (H) \f$ cannot be fit in memory;
    *         therefore, we process the matrix in chunks */
   bool multiPassH;
   /*! @brief Flag signifies when to write accumulated value out Cbuffer */
   bool writeCBuffer;
   /*! @brief Flag signifies first call to kernel when compute is broken down */
   bool firstCall;
   /*! @brief Flag signifies last call to kernel when compute is broken down */
   bool lastCall;
   /*! @brief Offset of A matrix when kernel matrix is broken down in
    *         multi-pass case to pick appropriate block of feature matrix for
    *         inner product computation; applicable only for natural C code*/
   int32_t blockOffsetA;
   /*! @brief Flag indicating whether to enable dynamic range
    *         (min,max) calculation.*/
   MMALIB_enableDynamicRange_type enableDynamicRange;
   /*! @brief Flag indicating whether to reset dynamic range
    *         (min,max) calculation at beginning of compute.*/
   MMALIB_initDynamicRange_type initDynamicRange;
   /*! @brief Specify quantization method */
   MMALIB_quantMethod_type quantMethod;
} MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_ExecInArgs;

/**
 * @struct MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_ExecOutArgs
 * @brief This structure holds all the runtime output parameters for the kernel
 */

typedef struct {
   /*! @brief Minimum computed value, controlled with
    *   @ref
    * MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_ExecInArgs.initDynamicRange */
   int64_t dynamicRangeMin;
   /*! @brief Minimum computed value, controlled with
    *   @ref
    * MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_ExecInArgs.initDynamicRange */
   int64_t dynamicRangeMax;
} MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_ExecOutArgs;

/**
 *  @brief        This is a query function to return the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it during init and exec function calls
 */

int32_t MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_getHandleSize (
    MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_InitArgs *pKerInitArgs);

/**
 *  @brief       This function call is required to initialize the handle. In
 *               this function most of the one time operations are performed
 *               and results are stored in the handle
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  src0_addr    :  Pointer to the structure containing dimensional
 *                               information of src0, which is the feature map
 * matrix
 *  @param [in]  src1_addr    :  Pointer to the structure containing dimensional
 *                               information of src1, which is the kernel matrix
 *  @param [in]  src2_addr[]  :  Pointer to the structure containing dimensional
 *                               information of src2, which is the bias vector
 *  @param [in]  src3_addr[]  :  Pointer to the structure containing dimensional
 *                               information of src3, which is the scale vector
 *  @param [out] dst_addr     :  Pointer to the structure containing dimensional
 *                               information of dst
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 * parameters
 *
 *  @return      Status value indicating success or failure.
 *               Refer to @ref MMALIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle
 */

MMALIB_STATUS MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_init (
    MMALIB_kernelHandle                                       handle,
    const MMALIB_bufParams2D_t                               *src0_addr,
    const MMALIB_bufParams2D_t                               *src1_addr,
    const MMALIB_bufParams1D_t                               *src2_addr,
    const MMALIB_bufParams1D_t                               *src3_addr,
    const MMALIB_bufParams2D_t                               *dst_addr,
    const MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the parameters and should be called
 *               before kernel execution. It can be called once
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  src0_addr    :  Pointer to the structure containing dimensional
 *                               information of src0, which is the feature map
 *  @param [in]  src1_addr    :  Pointer to the structure containing dimensional
 *                               information of src1, which is the kernel
 *  @param [in]  src2_addr[]  :  Pointer to the structure containing dimensional
 *                               information of src2, which is the bias vector
 *  @param [out] dst_addr     :  Pointer to the structure containing dimensional
 *                               information of dst
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init
 parameters

 *  @return      Status value indicating success or failure.
 *               Refer to @ref MMALIB_STATUS.
 *
 *  @remarks     None
 */

MMALIB_STATUS MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_init_checkParams (
    MMALIB_kernelHandle                                       handle,
    const MMALIB_bufParams2D_t                               *src0_addr,
    const MMALIB_bufParams2D_t                               *src1_addr,
    const MMALIB_bufParams1D_t                               *src2_addr,
    const MMALIB_bufParams2D_t                               *dst_addr,
    const MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_InitArgs *pKerInitArgs);

/**
 *  @brief       This function is the main compute function, and performs
 *               matrix-matrix multiplication.
 *
 *  @details     The flow and the expectations of this function are as follows
 *
 *  @param [in]  handle           : Active handle to the kernel
 *  @param [in]  src0[]           : Pointer to buffer holding the first matrix
 *                                  input [ A matrix], which is the feature map
 *  @param [in]  src1[]           : Pointer to buffer holding the second matrix
 *                                  input [ B matrix], which is the
 * pre-processed kernel matrix with bias values
 *  @param [in]  src2[]           : Pointer to buffer holding the bias vector
 *  @param [in]  src3[]           : Pointer to buffer holding the scale vector
 *  @param [in]  src4[]           : Pointer to buffer holding the shift vector
 *  @param [out] dst[]            : Pointer to buffer holding the output matrix
 *                                  [ C matrix]
 *  @param [in]  pKerExecInArgs   : Pointer to the structure holding input exec
 *                                  parameters
 *  @param [out]  pKerExecOutArgs : Pointer to the structure holding exec output
 *                                 parameters
 *  @return      Status value indicating success or failure.
 *               Refer to @ref MMALIB_STATUS.
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
 *  @remarks     Application is expected to call the checkParams function prior
 *               to this function as it avoids check of paramaters for each
 *               invocation.
 */

MMALIB_STATUS MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_exec (
    MMALIB_kernelHandle                                         handle,
    const void                                                 *src0,
    const void                                                 *src1,
    const void                                                 *src2,
    const void                                                 *src3,
    const void                                                 *src4,
    void                                                       *dst,
    const MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_ExecInArgs *pKerExecInArgs,
    MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_ExecOutArgs *pKerExecOutArgs);

/**
 *  @brief       This function checks the parameters and should be called
 *               before kernel executuon. It can be called once
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  src0[]      : Pointer to buffer holding the first matrix input
 *                             [ A matrix], which is the feature map
 *  @param [in]  src1[]      : Pointer to buffer holding the second matrix input
 *                             [ B matrix], which is the pre-processed
 *                             parameter matrix
 *  @param [in]  src2[]      : Pointer to buffer holding the bias vector
 *  @param [out] dst[]       : Pointer to buffer holding the output matrix
 *                              [ C matrix]

 *  @return      Status value indicating success or failure.
 *               Refer to @ref MMALIB_STATUS.
 *
 *  @remarks     None
 */

MMALIB_STATUS
MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_exec_checkParams (
    MMALIB_kernelHandle handle,
    const void         *src0,
    const void         *src1,
    const void         *src2,
    const void         *dst);

#if defined(__SE_FEAT_SPARSITY)
MMALIB_STATUS
MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_packetizeWeights (
      MMALIB_bufParams2D_t* restrict h_addr,
const void *restrict                 pWeights,
      void *restrict                 pReorderWeights
    );
#endif

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

void MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_perfEst (
    MMALIB_kernelHandle         handle,
    const MMALIB_bufParams2D_t *src0_addr,
    const MMALIB_bufParams2D_t *src1_addr,
    const MMALIB_bufParams2D_t *dst_addr,
    uint64_t                   *idealCycles,
    uint64_t                   *archCycles,
    uint64_t                   *estCycles,
    int32_t                    *caseNumber);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* MMALIB_CNN_FULLYCONNECTEDBIAS_IXX_IXX_OXX_H_ */

/* ======================================================================== */
/*  End of file:  MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX.h          */
/* ======================================================================== */
