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

#ifndef MMALIB_CNN_FULLYCONNECTED_IXX_IXX_OXX_H_
#define MMALIB_CNN_FULLYCONNECTED_IXX_IXX_OXX_H_

#include "../../common/MMALIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @defgroup MMALIB_CNN_fullyConnected_ixX_ixX_oxX MMALIB_CNN_fullyConnected_ixX_ixX_oxX
 * @brief Kernel provides compute functionality of Fully Connected Layer: \f$ Y^T = X^T
 * \times H^T \f$
 * @details
 *	  - Kernel requires feature map (\f$ X^T \f$) and coefficients (parameter matrix,
 *          \f$ H^T \f$) to be available in memory
 *        - 8- and 16-bit datatypes supported
 *        - Supported datatypes for feature map are 8- and 16-bit signed or unsigned
 *        - Supported datatypes for coefficients are 8- and 16-bit signed
 *        - Supported datatypes for output are 8- and 16-bit signed or unsigned
 *	     - Original algorithm for batch size greater than one:
 *          \f$ Y = H \times X + B \f$
 *	     - Here, \f$ Y \f$ is output matrix, \f$ H \f$ is kernel matrix, \f$ X \f$ is
 *          feature matrix (column-major order), and \f$ B \f$ is bias matrix
 *          - Note: \f$ Y \f$ and \f$ X \f$ will be column matrices for batch size of
 *            one
 *        - After applying affine transformation, we obtain matrix structures as shown
 *          below
 *          - \f$ Y \f$ = \f$ \begin{bmatrix} H & B_1 \end{bmatrix} \times
 *             \begin{bmatrix} X \\ B_2 \end{bmatrix} \f$
 *          - Note that \f$ B \f$ matrix is mathematically split into \f$ B_1 \f$ and
 *            \f$ B_2 \f$, and \f$ B_2 \f$ is constant matrix whose value is supplied
 *            via @ref MMALIB_CNN_fullyConnected_ixX_ixX_oxX_InitArgs.bias
 *          - \f$ B_2 \f$ is appended on-the-fly via predicate register; therefore,
 *            \f$ X\f$ matrix shall not be appended with \f$ B_2 \f$ when stored in
 *            memory
 *        - Since \f$ X \f$  is stored in row-major order in memory, we apply transpose
 *          to affine-transformed matrix structure mentioned above
 *          - \f$ Y^T \f$ = \f$ \begin{bmatrix} X^T & B_2^T \end{bmatrix} \times
 *             \begin{bmatrix} H^T \\ B_1^T \end{bmatrix} \f$
 *        - This kernel requires specific data arrangement for the kernel matrix to
 *          alleviate bank conflicts in L2 when performing DMA transfers in conjunction
 *          to execution of this kernel
 *          - Desired physical bank-access pattern for streaming engines and DMA in L2 is
 *            {0,0,1,1,2,2,3,3,0,0,1,1,2,2,3,3, ...}
 *          - Re-ordering functionality of
 *            @ref MMALIB_CNN_fullyConnected_ixX_ixX_oxX_reorderWeights maybe used
 *          - Below figure shows two examples where @ref
 *            MMALIB_CNN_fullyConnected_ixX_ixX_oxX_InitArgs.kDimRows (517) = @ref
 *            MMALIB_CNN_fullyConnected_ixX_ixX_oxX_InitArgs.Ni (512) + number of bias
 *            rows (5); Number of output features = 128 or
 *            104
 * \image html fcl_h_buffer.svg "Filter Coefficient Buffer"
 *        - The kernel also supports cases when parameter or feature map  matrix does not
 *          fit in L2 memory
 *          - The multiPassH flag in structures
 *            @ref MMALIB_CNN_fullyConnected_ixX_ixX_oxX_InitArgs.multiPassH and
 *            @ref MMALIB_CNN_fullyConnected_ixX_ixX_oxX_ExecArgs.multiPassH
 facilitate
 *            the feature when parameter matrix does not fit in L2 memory
 *        - Three buffers of type @ref MMALIB_bufParams2D_t : src0 \f$ \rightarrow X^T
 *            \f$; src1 \f$ \rightarrow H^T \f$; and dst \f$ \rightarrow Y^T \f$
 * \image html fcl_buffers.svg "Buffer Arrangement"
 *        - The multiPassX flag in structures
 *            @ref MMALIB_CNN_fullyConnected_ixX_ixX_oxX_InitArgs.multiPassX and
 *            @ref MMALIB_CNN_fullyConnected_ixX_ixX_oxX_ExecArgs.multiPassX
 facilitate
 *            the feature when feature map matrix does not fit in L2 memory
 *            - Table below shows the flag combinations supported:
 *          <TABLE frame="box" rules="all" cellspacing="0" width="25%" border="1"
 *           cellpadding="1">
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

/**
 *  @enum       MMALIB_CNN_FULLYCONNECTED_IXX_IXX_OXX_STATUS_NAME
 *  @brief      Enumeration of different error codes for the MMALIB_CNN_FULLYCONNECTED
 * kernel
 *
 */

typedef enum {
   MMALIB_CNN_FULLYCONNECTED_IXX_IXX_OXX_ERR_SMALL_K = MMALIB_ERROR_MAX,
   /*@todo Add more appropriate error codes */
   MMALIB_CNN_FULLYCONNECTED_IXX_IXX_OXX_ERR_MAX
} MMALIB_CNN_FULLYCONNECTED_IXX_IXX_OXX_STATUS_NAME;

/**
 * @struct MMALIB_CNN_fullyConnected_ixX_ixX_oxX_InitArgs
 */

typedef struct {
   /*! @brief Variant of the function refer to @ref MMALIB_FUNCTION_STYLE     */
   int8_t funcStyle;
   /*! @brief Scalar value to scale output                                    */
   int8_t shift;
   /*! @brief Bias values added to output                                     */
   int32_t bias;
   /*! @brief Output activation type: Saturation or ReLU     */
   int8_t activationType;
   /*! @brief Value of \f$ N_i\f$ used in natural C code only*/
   int32_t Ni;
   /*! @brief Value of \f$ N_i + \f$ number of bias rows */
   int32_t kDimRows;
   /*! @brief flag signifies feature matrix \f$ (X) \f$ cannot be fit in memory;
    *         therefore, compute is broken down by processing the matrix in chunks */
   bool multiPassX;
   /*! @brief flag signifies kernel matrix \f$ (H) \f$ cannot be fit in memory;
    *         therefore, compute is broken down by processing the matrix in chunks */
   bool multiPassH;
   /*! @brief Buffer pointer to hold partially accumulated values in multi-pass case */
   void *bufCAccum_cn;
} MMALIB_CNN_fullyConnected_ixX_ixX_oxX_InitArgs;

/**
 * @struct MMALIB_CNN_fullyConnected_ixX_ixX_oxX_ExecArgs
 */

typedef struct {
   /*! @brief flag signifies feature matrix \f$ (X) \f$ cannot be fit in memory;
    *         therefore, compute is broken down by processing the matrix in chunks */
   bool multiPassX;
   /*! @brief flag signifies kernel matrix \f$ (H) \f$ cannot be fit in memory;
    *         therefore, compute is broken down by processing the matrix in chunks */
   bool multiPassH;
   /*! @brief flag signifies when to write accumulated value out Cbuffer */
   bool writeCBuffer;
   /*! @brief flag signifies first call to kernel when compute is broken down */
   bool firstCall;
   /*! @brief flag signifies last call to kernel when compute is broken down */
   bool lastCall;
   /*! @brief  Offset of A matrix when kernel matrix is broken down in multi-pass case
    *          to pick appropriate block of feature matrix for inner product
    *          computation; applicable only for natural C code*/
   int32_t blockOffsetA;
} MMALIB_CNN_fullyConnected_ixX_ixX_oxX_ExecArgs;

/**
 *  @brief        This is a query function to return the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it during init and exec function calls
 */

int32_t MMALIB_CNN_fullyConnected_ixX_ixX_oxX_getHandleSize (
    MMALIB_CNN_fullyConnected_ixX_ixX_oxX_InitArgs *pKerInitArgs);

/**
 *  @brief       This function call is required to initialize the handle. In
 *               this function most of the one time operations are performed
 *               and results are stored in the handle
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  src0_addr    :  Pointer to the structure containing dimensional
 *                               information of src0, which is the feature map matrix
 *  @param [in]  src1_addr    :  Pointer to the structure containing dimensional
 *                               information of src1, which is the kernel matrix
 *  @param [out] dst_addr     :  Pointer to the structure containing dimensional
 *                               information of dst
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure.
 *               Refer to @ref MMALIB_STATUS.
 *
 *  @remarks     Application is expected to provide a valid handle
 */

MMALIB_STATUS MMALIB_CNN_fullyConnected_ixX_ixX_oxX_init (
    MMALIB_kernelHandle                                   handle,
    const MMALIB_bufParams2D_t *                          src0_addr,
    const MMALIB_bufParams2D_t *                          src1_addr,
    const MMALIB_bufParams2D_t *                          dst_addr,
    const MMALIB_CNN_fullyConnected_ixX_ixX_oxX_InitArgs *pKerInitArgs);

/**
 *  @brief       This function checks the parameters and should be called
 *               before kernel execution. It can be called once
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  src0_addr    :  Pointer to the structure containing dimensional
 *                               information of src0, which is the feature map
 *  @param [in]  src1_addr    :  Pointer to the structure containing dimensional
 *                               information of src1, which is the kernel
 *  @param [out] dst_addr     :  Pointer to the structure containing dimensional
 *                               information of dst
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters

 *  @return      Status value indicating success or failure.
 *               Refer to @ref MMALIB_STATUS.
 *
 *  @remarks     None
 */

MMALIB_STATUS MMALIB_CNN_fullyConnected_ixX_ixX_oxX_init_checkParams (
    MMALIB_kernelHandle                                   handle,
    const MMALIB_bufParams2D_t *                          src0_addr,
    const MMALIB_bufParams2D_t *                          src1_addr,
    const MMALIB_bufParams2D_t *                          dst_addr,
    const MMALIB_CNN_fullyConnected_ixX_ixX_oxX_InitArgs *pKerInitArgs);

/**
 *  @brief       This function is the main compute function, and performs
 *               matrix-matrix multiplication.
 *
 *  @details     The flow and the expectations of this function are as follows
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [in]  src0[]       : Pointer to buffer holding the first matrix input
 *                              [ A matrix], which is the feature map
 *  @param [in]  src1[]       : Pointer to buffer holding the second matrix input
 *                              [ B matrix], which is the pre-processed kernel matrix
 *                              with bias values
 *  @param [out] dst[]        : Pointer to buffer holding the output matrix
 *                              [ C matrix]
 *  @param [in]  pKerExecArgs : Pointer to the structure holding exec parameters
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

MMALIB_STATUS MMALIB_CNN_fullyConnected_ixX_ixX_oxX_exec (
    MMALIB_kernelHandle                                   handle,
    const void *                                          src0,
    const void *                                          src1,
    void *                                                dst,
    const MMALIB_CNN_fullyConnected_ixX_ixX_oxX_ExecArgs *pKerExecArgs);

/**
 *  @brief       This function checks the parameters and should be called
 *               before kernel executuon. It can be called once
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  src0[]      : Pointer to buffer holding the first matrix input
 *                              [ A matrix], which is the feature map
 *  @param [in]  src1[]      : Pointer to buffer holding the second matrix input
 *                              [ B matrix], which is the pre-processed kernel matrix
 *  @param [out] dst[]       : Pointer to buffer holding the output matrix
 *                              [ C matrix]

 *  @return      Status value indicating success or failure.
 *               Refer to @ref MMALIB_STATUS.
 *
 *  @remarks     None
 */

MMALIB_STATUS
MMALIB_CNN_fullyConnected_ixX_ixX_oxX_exec_checkParams (MMALIB_kernelHandle handle,
                                                        const void *        src0,
                                                        const void *        src1,
                                                        const void *        dst);

/**
 *  @brief       This function generates the predicate buffer for feature matrix
 *
 *  @param [in]  handle       : Active handle to the kernel
 *  @param [in]  inputWidth   : Width of Feature map
 *  @param [in]  processWidth : Total width of feature map augmented with bias
 *  @param [in]  mmaWidth     : MMA width

 *  @return      totalBytes   : total bytes of data in predicate buffer
 *
 *  @remarks     None
 */

int32_t MMALIB_CNN_generateFillBiasPredicateRegisters (MMALIB_kernelHandle handle,
                                                       int32_t             inputWidth,
                                                       int32_t             processWidth,
                                                       uint32_t            mmaWidth);

/**
 * @brief       This function re-orders the input weights (kernel matrix).
 *              Reordering of data is required to  alleviate potential bank conflicts
 *              arising when the kernel matrix is  accessed in L2 via SE1 in
 *              conjunction with data transfer from MSMC to L2 via DMA. The current
 *              re-ordering scheme results in a bank-acess pattern of
 *              {0,0,1,1,2,2,3,3,0,0,1,1,2,2,3,3, ...} for SE1
 *
 * @param [in]  mmaSize         : Size of MMA for given test case
 *                                - 64 for 8-bit datatype
 *                                - 32 for 16-bit datatype
 * @param [in]  NiBias          : Ni (number of input channels) + Bias columns
 * @param [in]  No              : Number of output features
 * @param [in]  strideOut       : Stride of matrix after re-ordering
 * @param [in]  pWeights        : Pointer for input weights
 * @param [in]  pReorderWeights : Pointer for ouput weights after re-ordering
 *
 *  @return      Status value indicating success or failure.
 *               Refer to @ref MMALIB_STATUS.
 */

MMALIB_STATUS
MMALIB_CNN_fullyConnected_ixX_ixX_oxX_reorderWeights (int32_t     mmaSize,
                                                      int32_t     NiBias,
                                                      int32_t     No,
                                                      uint32_t    strideOut,
                                                      const void *restrict pWeights,
                                                      void *restrict pReorderWeights);

/*!
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

void MMALIB_CNN_fullyConnected_ixX_ixX_oxX_perfEst (MMALIB_kernelHandle         handle,
                                                    const MMALIB_bufParams2D_t *src0_addr,
                                                    const MMALIB_bufParams2D_t *src1_addr,
                                                    const MMALIB_bufParams2D_t *dst_addr,
                                                    uint64_t *archCycles,
                                                    uint64_t *estCycles,
                                                    int32_t * caseNumber);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* MMALIB_CNN_FULLYCONNECTED_IXX_IXX_OXX_H_ */

/* ======================================================================== */
/*  End of file:  MMALIB_CNN_fullyConnected_ixX_ixX_oxX.h          */
/* ======================================================================== */
