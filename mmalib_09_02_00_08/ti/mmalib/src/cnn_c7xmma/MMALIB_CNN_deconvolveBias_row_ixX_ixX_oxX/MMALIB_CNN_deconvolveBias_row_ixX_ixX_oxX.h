/*******************************************************************************
 **+--------------------------------------------------------------------------+**
 **|                            **** |**
 **|                            **** |**
 **|                            ******o*** |**
 **|                      ********_///_**** |**
 **|                      ***** /_//_/ **** |**
 **|                       ** ** (__/ **** |**
 **|                           ********* |**
 **|                            **** |**
 **|                            *** |**
 **| |**
 **|        Copyright (c) 2016 Texas Instruments Incorporated |**
 **| |**
 **|              All rights reserved not granted herein. |**
 **| |**
 **|                         Limited License. |**
 **| |**
 **|  Texas Instruments Incorporated grants a world-wide, royalty-free, |**
 **|  non-exclusive license under copyrights and patents it now or |**
 **|  hereafter owns or controls to make, have made, use, import, offer to |**
 **|  sell and sell ("Utilize") this software subject to the terms herein. |**
 **|  With respect to the foregoing patent license, such license is granted |**
 **|  solely to the extent that any such patent is necessary to Utilize the |**
 **|  software alone.  The patent license shall not apply to any |**
 **|  combinations which include this software, other than combinations |**
 **|  with devices manufactured by or for TI ("TI Devices").  No hardware |**
 **|  patent is licensed hereunder. |**
 **| |**
 **|  Redistributions must preserve existing copyright notices and |**
 **|  reproduce this license (including the above copyright notice and the |**
 **|  disclaimer and (if applicable) source code license limitations below) |**
 **|  in the documentation and/or other materials provided with the |**
 **|  distribution |**
 **| |**
 **|  Redistribution and use in binary form, without modification, are |**
 **|  permitted provided that the following conditions are met: |**
 **| |**
 **|    *  No reverse engineering, decompilation, or disassembly of this |**
 **|  software is permitted with respect to any software provided in binary |**
 **|  form. |**
 **| |**
 **|    *  any redistribution and use are licensed by TI for use only with |**
 **|  TI Devices. |**
 **| |**
 **|    *  Nothing shall obligate TI to provide you with source code for |**
 **|  the software licensed and provided to you in object code. |**
 **| |**
 **|  If software source code is provided to you, modification and |**
 **|  redistribution of the source code are permitted provided that the |**
 **|  following conditions are met: |**
 **| |**
 **|    *  any redistribution and use of the source code, including any |**
 **|  resulting derivative works, are licensed by TI for use only with TI |**
 **|  Devices. |**
 **| |**
 **|    *  any redistribution and use of any object code compiled from the |**
 **|  source code and any resulting derivative works, are licensed by TI |**
 **|  for use only with TI Devices. |**
 **| |**
 **|  Neither the name of Texas Instruments Incorporated nor the names of |**
 **|  its suppliers may be used to endorse or promote products derived from |**
 **|  this software without specific prior written permission. |**
 **| |**
 **|  DISCLAIMER. |**
 **| |**
 **|  THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY |**
 **|  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE |**
 **|  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR |**
 **|  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL TI AND TI'S LICENSORS BE |**
 **|  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR |**
 **|  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF |**
 **|  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR |**
 **|  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, |**
 **|  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE |**
 **|  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, |**
 **|  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. |**
 **+--------------------------------------------------------------------------+**
 *******************************************************************************/

/******************************************************************************
 *                                                                             *
 * module name       :MMALIB                                                   *
 *                                                                             *
 * module descripton :Matrix Multiply Accelerator Library module for C7x+MMA   *
 *                                                                             *
 * Copyright (C) 2017-2018 Texas Instruments Incorporated - http://www.ti.com/ *
 * ALL RIGHTS RESERVED                                                         *
 *                                                                             *
 ******************************************************************************/
/**
******************************************************************************
*  @file     MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX.h
*
*  @brief    Public header file for MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX
*            function
*
*  @version  0.2 - March 2018 : Intial version for 8- and 16-bit datatypes
*
*******************************************************************************
*/

#ifndef MMALIB_CNN_DECONVOLVEBIAS_ROW_IXX_IXX_OXX_H_
#define MMALIB_CNN_DECONVOLVEBIAS_ROW_IXX_IXX_OXX_H_

#include "../../common/MMALIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @defgroup MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX
 * @brief Kernel for computing dense CNN deconvolution with row-based processing
 *        and matrix-matrix multiplication
 * @details
 *	  - Kernel requires coefficients, \f$ H \f$ and feature map, \f$ X \f$
 *          to be available in memory
 *        - 8- and 16-bit datatypes supported
 *        - Supported datatypes for feature map are 8- and
 *          16-bit signed and unsigned
 *        - Supported datatypes for coefficients are 8- and 16-bit signed
 *        - Kernel leverages code from convolve rows kernel
 *          - \f$ Y = H \times X \f$, which translates to
 *            \f$ C = A \times B \f$ in relation to MMA's hardware architecture
 *        - Feature map is expected to be padded appropriately with zeroes
 *        - Feature map will be padded with a constant matrix by the kernel
 *on-the-fly, whose scalar value is represented by
 *          @ref MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_InitArgs.bias
 *        - It is assumed that filter coefficient tensor is padded with bias
 *appropriately
 *        - Kernel supports only <b>\f$ 4 \times 4 \f$ stride by \f$ 2 \f$</b>,
 *          <b>\f$ 2 \times 2 \f$ stride by \f$ 2 \f$</b>, and <b>\f$ 8 \times 8
 *\f$ stride by \f$ 2 \f$</b>  deconvolutions
 *        - Filter coefficient tensor is assumed to be pre-processed
 *        - Pre-processing functions
 *          @ref MMALIB_CNN_deconvolveBias_row_4x4Stride2PreProcessParameters
 *          @ref MMALIB_CNN_deconvolveBias_row_2x2Stride2PreProcessParameters,
 *and
 *          @ref MMALIB_CNN_deconvolveBias_row_8x8Stride2PreProcessParameters
 *can be used to pre-process the filter coefficients for \f$ 4 \times 4 \f$
 *stride by \f$ 2 \f$, \f$ 2 \times 2 \f$ stride by \f$ 2 \f$, and \f$ 8 \times
 *8 \f$ stride by \f$ 2 \f$ deconvolutions respectively
 *        - Kernel performs \f$ 4 \times 4 \f$ stride by \f$ 2 \f$ and
 *          \f$ 2 \times 2 \f$ stride by \f$ 2 \f$ deconvolutions via four, \f$
 *2 \times 2 \f$ stride by \f$ 1 \f$ and \f$ 1 \times 1 \f$ stride by \f$ 1 \f$
 *          convolutions respectively
 *          - Process results in interleaved rows and columns of actual output
 *        - Kernel outputs partial deconvolution output
 *          - Columns are interleaved by MMALIB, while interleaving of rows
 *maybe performed in background by DRU
 *          - Zero padding of current output as input to subsequent layer
 *            may also be performed in background of compute via DRU
 * \image html deconvovle_h_buffer.svg  "Pre-processed coefficient buffer"
 *width=1200px \image html deconvolveBias_x_buffer.svg "Input feature map
 *buffer" width=1200px \image html deconvolveBias_y_buffer.svg "Output feature
 *map buffer" width=1800px
 * @ingroup  MMALIB_CNN */
/**@{*/

/**
 *  @enum       MMALIB_CNN_DECONVOLVEBIAS_ROW_IXX_IXX_OXX_STATUS_NAME
 *  @brief      Enumeration for different Error codes for
 * MMALIB_CNN_DECONVOLVEBIAS_ROW Kernel
 *
 */
typedef enum {
   MMALIB_CNN_DECONVOLVEBIAS_ROW_IXX_IXX_OXX_ERR_SMALL_K =
       MMALIB_ERROR_MAX, /** Error case because k < Ni*Fr*Fc */
   MMALIB_CNN_DECONVOLVEBIAS_ROW_IXX_IXX_OXX_ERR_MAX
} MMALIB_CNN_DECONVOLVEBIAS_ROW_IXX_IXX_OXX_STATUS_NAME;

/**
 * @struct MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_InitArgs
 * @brief Structure containing the parameters
 *        initialization of CNN deconvolution computation
 */

typedef struct {
   /*! @brief Variant of the function refer to @ref MMALIB_FUNCTION_STYLE     */
   int8_t funcStyle;
   /*! @brief Total number of output channels to be processed */
   uint32_t No;
   /*! @brief Total number output groups; supported for only Ni=No=1 */
   uint32_t numGroups;
   /*! @brief Input Channel offset */
   uint32_t inChOffset;
   /*! @brief Width of buffer C */
   int32_t validColsIn;
   /*! @brief Strided convolution - input columns per row */
   int32_t validColsPerRowIn;
   /*! @brief Strided convolution - input rows */
   int32_t validRowsIn;
   /*! @brief Strided convolution - input pitch per row */
   int32_t inputPitchPerRow;
   /*! @brief Strided convolution - output pitch per row */
   int32_t outputPitchPerRow;
   /*! @brief Line offset in unit of dataType */
   uint32_t inWidth;
   /*! @brief Pad between rows */
   uint32_t pad;
   /*! @brief Max Height of input feature maps */
   int32_t maxHeight;
   /*! @brief Input valid rows for A matrix */
   uint32_t subMChannels;
   /*! @brief Output shift value */
   uint32_t shift;
   /*! @brief Kernel Height */
   int32_t Fr;
   /*! @brief Kernel Width */
   int32_t Fc;
   /*! @brief stride in horizontal */
   int32_t strideX;
   /*! @brief stride in vertical */
   int32_t strideY;
   /*! @brief dilation of kernel widht */
   int32_t dilationX;
   /*! @brief dilation of kernel heigth */
   int32_t dilationY;
   /*! @brief bias value */
   uint32_t bias;
   /*! @brief RELU/SAT/None */
   uint8_t activationType;
   /*! @brief Circular or Linear addressing for B matrix */
   uint8_t mode;
} MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_InitArgs;

/**
 * @struct MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_ExecInArgs
 * @brief Structure containing the parameters
 *        for input to the execute phase of CNN deconvolution computation
 */

typedef struct {
   /*! @brief Width of buffer C */
   int32_t validColsIn;
   /*! @brief Input valid rows for A matrix */
   uint32_t subMChannels;
   /*! @brief quantMethod 0 - per call 1 - per channel */
   MMALIB_quantMethod_type quantMethod;
} MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_ExecInArgs;

/**
 * @struct MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_ExecInArgs
 * @brief  This structure holds all the runtime output parameter for CNN
 * row-based deconvolution kernel
 */

typedef struct {
   /*! @brief Width of buffer C */
   uint32_t validColsOut;
} MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_ExecOutArgs;

/**
 *  @brief        This is a query function to return the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it during init and exec function calls
 */

int32_t MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_getHandleSize (
    MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_InitArgs *pKerInitArgs);

/**
 *  @brief     This function call is required to initialize the handle. In
 *	       this function, most of the one-time operation are performed
 *	       and results are stored in handle
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  src0_addr    :  Pointer to structure containing dimensional
 *				   information of src0
 *  @param [in]  src1_addr    :  Pointer to structure containing dimensional
 *				   information of src1
 *  @param [out] dst_addr     :  Pointer to structure containing dimensional
 *				   information of dst
 *  @param [in]  pKerInitArgs : Pointer to structure holding init parameters
 *  @return      Status of success or Error with Error Codes.
 *		    Refer to  @ref MMALIB_STATUS
 *
 *  @remarks     Application is expected to do provide valid handle
 */

MMALIB_STATUS MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_init (
    MMALIB_kernelHandle                                       handle,
    const MMALIB_bufParams2D_t                               *src0_addr,
    const MMALIB_bufParams2D_t                               *src1_addr,
    const MMALIB_bufParams1D_t                               *src2_addr,
    const MMALIB_bufParams1D_t                               *src3_addr,
    const MMALIB_bufParams2D_t                               *dst_addr,
    const MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_InitArgs *pKerInitArgs);

/**
*  @brief       This function call is required to initialize the handle. In
*               this function, most of the one-time operation are performed
*               and results are stored in handle
*
*  @param [in]  handle       :  Active handle to the kernel
*  @param [in]  src0_addr    :  Pointer to structure containing dimensional
*                               information of src0 weights/coefficients
*  @param [in]  src1_addr    :  Pointer to structure containing dimensional
*                               information of src1 input feature maps
*  @param [out] dst_addr     :  Pointer to structure containing dimensional
*                               information of dst output feature maps
*  @param [in]  pKerInitArgs : Pointer to structure holding init parameters

*  @return      Status of success or Error with Error Codes
*
*  @remarks     Application is expected to do provide valid handle
*/

MMALIB_STATUS MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_init_checkParams (
    MMALIB_kernelHandle                                       handle,
    const MMALIB_bufParams2D_t                               *src0_addr,
    const MMALIB_bufParams2D_t                               *src1_addr,
    const MMALIB_bufParams1D_t                               *src2_addr,
    const MMALIB_bufParams2D_t                               *dst_addr,
    const MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_InitArgs *pKerInitArgs);

/**
 *  @brief       This function is the main compute function and performs
 *               the deconvolution primitive (conv + ReLU) for CNN on the
 *               row-based data arrangement. It is typically called multiple
 times
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  src0[]      : Pointer to buffer holding convolution weights
 *                             [ A matrix]
 *  @param [in]  src1[]      : Pointer to buffer holding input feature map
 *                             [ B matrix]
 *  @param [out] dst[]       : Pointer to buffer holding paritial output feature
 map *				  [ C matrix]
 *  @param [in]  pKerInArgs  : Pointer to structure holding input Arguments
 *  @param [out] pKerOutArgs : Pointer to structure holding output Arguments

 *  @return      Status of success or Error with Error Codes.
 *		    Refer to @ref MMALIB_STATUS
 *
 *  @par Assumptions:
 *    - I/O buffer pointers are assumed to be not aliased.
 *
 *  @par Performance Considerations:
 *    - For best performance, the following parameter settings are recommended:
 *      - Set widths equal to strides
 *      - Align all pointers to 8-byte boundaries
 *      - Set all stride values to a multiple of 8
 *      - Set all width values to a multiple of 16
 *  @remarks     Application is expected to do call of checkParams function
 prior
 *               to this function as it avoids check of paramaters for each
 *               invocation for optimization
 */

MMALIB_STATUS MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_exec (
    MMALIB_kernelHandle                                         handle,
    const void                                                 *src0,
    const void                                                 *src1,
    const void                                                 *src2,
    const void                                                 *src3,
    const void                                                 *src4,
    void                                                       *dst,
    const MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_ExecInArgs *pKerInArgs,
    MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_ExecOutArgs      *pKerOutArgs);

/**
 *  @brief       This function checks the parameters and should be called
 *               before kernel execution. It can be called once
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  src0[]      : Pointer to buffer holding convolution weights
 *                             [ A matrix]
 *  @param [in]  src1[]      : Pointer to buffer holding input feature map data
 *                             [ B matrix]
 *  @param [out] dst[]       : Pointer to buffer holding output feature map data
 *                             [ C matrix]
 *  @param [in]  pKerInArgs  : Pointer to structure holding input Arguments
 *
 *  @return      Status of success or Error with Error Codes.
 *		    Refer to @ref MMALIB_STATUS
 *
 *  @remarks     None
 */

MMALIB_STATUS MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_exec_checkParams (
    MMALIB_kernelHandle                                         handle,
    const void                                                 *src0,
    const void                                                 *src1,
    const void                                                 *src2,
    const void                                                 *src3,
    const void                                                 *src4,
    const void                                                 *dst,
    const MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_ExecInArgs *pKerInArgs);

/**
 * @brief        This is a pre-processing function that reshapes the parameter
 *buffer from \f$ N_o \times N_i \times F_r \times F_c \f$ to \f$ 4 \times N_o
 *\times N_i \times \frac{F_r}{2} \times \frac{F_c}{2} \f$. The kernel expects
 *the parameter tensor in the aforementioned shape to perform \f$ 4 \times 4 \f$
 *stride 2 deconvolution via four, \f$ 2 \times 2 \f$ stride 1 convolutions
 *
 * @param  [in]  kDim           : Length of parameter buffer
 * @param  [in]  numInChannels  : Number of input channels in parameter tensor
 * @param  [in]  pitchA         : Pitch of parameter buffer
 * @param  [in]  numOutChannels : Number of output channels in parameter tensor
 * @param  [in]  numGroups      : Number of groups in parameter tensor
 * @param  [in]  mmaSize        : MMA width
 * @param  [in]  src            : Pointer to buffer with parameter tensor
 * @param  [out] dst            : Pointer to buffer with reshaped parameter
 *tensor
 *
 * @return       Status of success or Error with Error Codes.
 *		    Refer to @ref MMALIB_STATUS
 */

MMALIB_STATUS
MMALIB_CNN_deconvolveBias_row_4x4Stride2PreProcessParameters (
    uint32_t       numInChannels,
    uint32_t       pitchA,
    uint32_t       numOutChannels,
    uint32_t       numGroups,
    const uint32_t mmaSize,
    const void *restrict src,
    void *restrict dst);

/**
 * @brief        This is a pre-processing function that reshapes the parameter
 *buffer from \f$ N_o \times N_i \times F_r \times F_c \f$ to \f$ 4 \times N_o
 *\times N_i \times \frac{F_r}{2} \times \frac{F_c}{2} \f$. The kernel expects
 *the parameter tensor in the aforementioned shape to perform \f$ 2 \times 2 \f$
 *stride 2 deconvolution via four, \f$ 1 \times 1 \f$ stride 1 convolutions
 *
 * @param  [in]  kDim           : Length of parameter buffer
 * @param  [in]  numInChannels  : Number of input channels in parameter tensor
 * @param  [in]  pitchA         : Pitch of parameter buffer
 * @param  [in]  numOutChannels : Number of output channels in parameter tensor
 * @param  [in]  numGroups      : Number of groups in parameter tensor
 * @param  [in]  mmaSize        : MMA width
 * @param  [in]  src            : Pointer to buffer with parameter tensor
 * @param  [out] dst            : Pointer to buffer with reshaped parameter
 *tensor
 *
 * @return       Status of success or Error with Error Codes.
 *		    Refer to @ref MMALIB_STATUS
 */

MMALIB_STATUS
MMALIB_CNN_deconvolveBias_row_2x2Stride2PreProcessParameters (
    uint32_t       kDim,
    uint32_t       numInChannels,
    uint32_t       pitchA,
    uint32_t       numOutChannels,
    uint32_t       numGroups,
    const uint32_t mmaSize,
    const void *restrict src,
    void *restrict dst);

/**
 * @brief        This is a pre-processing function that reshapes the parameter
 *buffer from \f$ N_o \times N_i \times F_r \times F_c \f$ to \f$ 4 \times N_o
 *\times N_i \times \frac{F_r}{2} \times \frac{F_c}{2} \f$. The kernel expects
 *the parameter tensor in the aforementioned shape to perform \f$ 8 \times 8 \f$
 *stride 2 deconvolution via four, \f$ 4 \times 4 \f$ stride 1 convolutions
 *
 * @param  [in]  kDim           : Length of parameter buffer
 * @param  [in]  numInChannels  : Number of input channels in parameter tensor
 * @param  [in]  pitchA         : Pitch of parameter buffer
 * @param  [in]  numOutChannels : Number of output channels in parameter tensor
 * @param  [in]  numGroups      : Number of groups in parameter tensor
 * @param  [in]  mmaSize        : MMA width
 * @param  [in]  src            : Pointer to buffer with parameter tensor
 * @param  [out] dst            : Pointer to buffer with reshaped parameter
 *tensor
 *
 * @return       Status of success or Error with Error Codes.
 *		    Refer to @ref MMALIB_STATUS
 */

MMALIB_STATUS
MMALIB_CNN_deconvolveBias_row_8x8Stride2PreProcessParameters (
    uint32_t       kDim,
    uint32_t       numInChannels,
    uint32_t       pitchA,
    uint32_t       numOutChannels,
    uint32_t       numGroups,
    const uint32_t mmaSize,
    const void *restrict src,
    void *restrict dst);

MMALIB_STATUS
MMALIB_CNN_deconvolveBias_row_4x4Stride2PreProcessBias (void    *src,
                                                        void    *dst,
                                                        uint32_t No,
                                                        uint32_t numGroups,
                                                        uint32_t MMA_SIZE);

MMALIB_STATUS
MMALIB_CNN_deconvolveBias_row_4x4Stride2PreProcessScaleAndShift (
    void    *scaleVecIn,
    void    *shiftVecIn,
    void    *scaleVecOut,
    void    *shiftVecOut,
    uint32_t No,
    uint32_t numGroups);

/*!
 ********************************************************************************
 *  @brief       This function estimates the cycles consumed for the kernel
 *               execution.
 *
 *  @param [in]  src0_addr    :  Pointer to the structure containing
 *dimensional information of src0
 *  @param [in]  src1_addr    :  Pointer to the structure containing
 *dimensional information of src1
 *  @param [out] dst_addr     :  Pointer to the structure containing
 *dimensional information of dst
 *  @param [in]  kerInitArgs  :  Pointer to structure holding init
 *parameters
 *  @param [in]  pKerInArgs   :  Pointer to structure holding input
 *arguments
 *  @param [in]  pKerOutArgs  :  Pointer to structure holding output
 *arguments
 *  @param [in]  iterN        :  Number of subMBlocks iterations
 *  @param [out] archCycles   :  Cycles estimated for the compute, startup
 *and teardown
 *  @param [out] estCycles    :  Cycles estimated for the compute, startup,
 *                               teardown and any associated overhead
 *
 *  @return      None
 *
 *  @remarks     None
 *******************************************************************************
 */

void MMALIB_CNN_deconvolveBiasBiasReLUCompute_ixX_ixX_oxX_perfEst (
    const MMALIB_bufParams2D_t                                 *src0_addr,
    const MMALIB_bufParams2D_t                                 *src1_addr,
    const MMALIB_bufParams2D_t                                 *dst_addr,
    const MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_InitArgs   *kerInitArgs,
    const MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_ExecInArgs *pKerInArgs,
    MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX_ExecOutArgs      *pKerOutArgs,
    int32_t                                                     iterN,
    uint64_t                                                   *archCycles,
    uint64_t                                                   *estCycles);

/**@}*/

#ifdef __cplusplus
}
#endif
#endif /* MMALIB_CNN_DECONVOLVEBIAS_ROW_IXX_IXX_OXX_H_ */

/* ======================================================================== */
/*  End of file:  MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX.h */
/* ======================================================================== */
