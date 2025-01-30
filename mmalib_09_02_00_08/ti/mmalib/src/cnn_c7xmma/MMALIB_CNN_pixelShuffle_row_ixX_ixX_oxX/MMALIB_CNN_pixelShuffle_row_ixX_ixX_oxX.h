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
*  @file     MMALIB_CNN_pixelShuffle_row_ixX_ixX_oxX.h
*
*  @brief    Public header file for MMALIB_CNN_pixelShuffle_row_ixX_ixX_oxX
*            function
*
*  //version  0.1 - Jan 2018 : Initial Version - [Asheesh Bhardwaj]
*
*******************************************************************************
*/

#ifndef MMALIB_CNN_PIXELSHUFFLE_ROW_IXX_IXX_OXX_H_
#define MMALIB_CNN_PIXELSHUFFLE_ROW_IXX_IXX_OXX_H_

#include "../../common/MMALIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @defgroup MMALIB_CNN_pixelShuffle_row_ixX_ixX_oxX MMALIB_CNN_pixelShuffle_row_ixX_ixX_oxX
 * @brief Kernel for computing dense CNN convolution with row based processing
 *        and matrix multiplication followed by column interleaving, which results in a
 *        partial output of final form in pixel shuffle operator
 * @details
 *          - Only non-strided convolution is supported
 *          - Supported upscale factors are 2, 4, and 8
 *          - Kernel requires data to be available in memory for coefficients and
 *            feature map
 *          - Kernel writes output column-wise interleaved feature map into memory
 *          - Kernel fill zeros in between rows for next layer for non-strided
 *            convolution
 *          - Kernel optimizes the zero pad between two rows by combing the pad
 *            in the begining or end of the row for non-strided convolution
 *          - The input feature maps are 8-bit or 16-bit signed or unsigned
 *          - The weights are 8-bit, 16-bit signed
 *          - The output feature maps are 8-bit or 16-bit signed or unsigned
 *
 * \image html MMALIB_convolve_row_stride1_1.svg "Input and coefficient buffer for convolution"
 * \image html MMALIB_pixelShuffle_row_interleaved_output_buffer.svg "Output buffer for pixelShuffle row" width=600px
 *
 * @ingroup  MMALIB_CNN
 */
/**@{*/

/*!
 * @brief Enum to define the error codes
 */
typedef enum {
   MMALIB_CNN_PIXELSHUFFLE_ROW_IXX_IXX_OXX_ERR_SMALL_K =
       MMALIB_ERROR_MAX, /** Error case because k < Ni*Fr*Fc */
   MMALIB_CNN_PIXELSHUFFLE_ROW_IXX_IXX_OXX_ERR_MAX
} MMALIB_CNN_PIXELSHUFFLE_ROW_IXX_IXX_OXX_STATUS_NAME;

/*!
 * @brief Structure containing the parameters
 * initialization of CNN convolution computation
 */
typedef struct {
   /*! @brief Variant of the function refer to @ref MMALIB_FUNCTION_STYLE     */
   int8_t funcStyle;
   /*! @brief Number of output feature maps                                   */
   int32_t No;
   /*! @brief offset of the input feature maps in B matrix. This is power of 2
    * for circular buffer and atleast 64 byte aligned for linear buffer       */
   int32_t inChOffset;
   /*! @brief Valid columns of input feature maps in B matrix for one call of
    *         processing of the kernel for non strided convolution.
    *         - This is in units of data type.
    *         - ValidColsIn is initialized which remains the same for all calls
    *           to kernel for a given layer except for the last call
    *         - The input feature maps are flattened such that rows and
    *           columns are represented in a single dimension.
    *         - For Pad, the rows are padded minimum of (Fc - 1)/2 where Fc is
    *           filter coefficient width.
    *         - The data is optimized such that only one side of the
    *           pad is included between two rows. This reduces the computation
    *           Lc = input width, Lr = input height, Filter width Fc, Filter
    *           Height Fr
    *         - Pi = Input Pad = (Fc - 1)*DilationX
    *         - The minimum number of validColsIn (Lc + (Fc -1)/2)*(Fr - 1) + Fc
    *           validColsIn for entire feature map will be
    *         - Ip = (Lc + Pi/2)*(Lr + Pi) + Pi/2
    *           In case the next layer has pad Po > Pi
    *           validColsIn for entire feature map will be
    *           Ip' = (Lc + Po/2)*(Lr + Po) + Po/2                             */
   int32_t validColsIn;
   /*! @brief Valid columns in a row of input feature maps for one call of
    *         processing of the kernel for strided convolution.
    * The is validColsPerRowIn = inWidth + pad. The pad is on the left        */
   int32_t validColsPerRowIn;
   /*! @brief Valid input rows of input feature maps for one call of
    *         processing of the kernel for strided convolution.
    *         Ip = Lc + Pi/2                                                  */
   int32_t validRowsIn;
   /*! @brief Valid pitch for each input rows of input feature maps
    *         for strided convolution.  This is in units of bytes
    *         Lr + Pi for entire feature map but can be Fr rows to be minimum  */
   int32_t inputPitchPerRow;
   /*! @brief Valid output pitch for each output rows of output feature maps
    *         for strided convolution. This is units of bytes                 */
   int32_t outputPitchPerRow;
   /*! @brief Width of each row of input feature map in units of data type    */
   int32_t inWidth;
   /*! @brief Pad of each row of input feature map, specify pad only on one
    *         side and for strided flow the Pad is on the left                */
   int32_t pad;
   /*! @brief Height of the input feature map in units of data type           */
   int32_t maxHeight;
   /*! @brief Number of output channels per kernel call after convolution and before pixel
    * shuffle operation
    *        1. When filter coefficients does not fit in L2 memory but feature
    *           map does than the kernel is called multiple times with same
    *           subMChannels in each kernel call
    *           a. subMChhales can be more than MMA SIZE for non strided
    *              convolution
    *           b. subMChannels should be less than or equal to MMA Size for
    *              strided convolution
    *        2. When filter coefficients fit in L2 memory but feature does not
    *           than kernel is called multiple times for the entire feature
    *           map and subMChannels is same as number of rows in the feature
    *           map buffer structure.
    *        MMA Size (64 for 8 bit and 32 for 16 bit)  per kernel call       */
   int32_t subMChannels;
   /*! @brief Shift parameter for output precision                            */
   int32_t shift;
   /*! @brief coefficient rows (height)                                       */
   int32_t Fr;
   /*! @brief coefficient columns (width)                                     */
   int32_t Fc;
   /*! @brief stride of columns                                               */
   int32_t strideX;
   /*! @brief stride of rows                                                  */
   int32_t strideY;
   /*! @brief dilation of coefficients of columns                             */
   int32_t dilationX;
   /*! @brief dilation of coefficients of rows                                */
   int32_t dilationY;
   /*! @brief upscale factor for pixel shuffle */
   int32_t upscaleFactor;
   /*! @brief bias value in B matrix same as data type of B matrix            */
   int32_t bias;
   /*! @brief activation RELU, SAT or none for output                         */
   uint8_t activationType;
   /*! @brief mode for input feature map in Circular or Linear mode in
    * B matrix                                                                */
   uint8_t mode;
} MMALIB_CNN_pixelShuffle_row_ixX_ixX_oxX_InitArgs;

/*!
 * @brief Structure containing the parameters
 * for input to the execute phase of CNN convolution computation
 */

typedef struct {
   /*! @brief Valid columns in a row of input feature maps for one call of
    *          processing of the kernel for non strided convolution in execute
    *          phase. This is fixed for every kernel call for a given layer
    *          similar to init except for the last kernel call. The last call
    *          can have a lower or same as previous calls                     */
   int32_t validColsIn;
   /*! @brief Valid columns in a row of input feature maps for one call of
    *         processing of the kernel for strided convolution  for execute
    *         phase. This remains same for all the kernel calls for a given
    *         layer                                                          */
   int32_t validColsPerRowIn;
   /*! @brief Valid input rows of input feature maps for one call of
    *         processing of the kernel for strided convolution. This remains
    *         same for all kernel calls for a given layer except for last
    *         call                                                           */
   int32_t validRowsIn;
   /*! @brief Pad of each row of input feature map, specify pad only on one
    *         side in execute phase.
    *         This parameter is not used, it is same as initArgs              */
   int32_t pad;
   /*! @brief Number of output channels per kernel call after convolution and before pixel
    * shuffle; in execute phase, this parameter is not used, it is same as initArgs */
   int32_t subMChannels;
} MMALIB_CNN_pixelShuffle_row_ixX_ixX_oxX_ExecInArgs;

/*!
 * @brief Structure containing the parameters
 * for output from the execute phase of CNN convolution computation
 */
typedef struct {
   /*! @brief Valid columns out in a  row of output feature maps for one
    *         call of processing of the kernel for non strided convolution
    *         - validColsOut output pixels generated will be
    *           (validColsIn - (Lc +Pi/2)*Pi - Pi)
    *          - In case the following layer has pad Po > Pi
    *            The validColsOut generated will be
    *            (Ip' - (Lc +Po/2)*Po- Po)                                     */
   int32_t validColsOut;
   /*! @brief Valid columns in a row of output feature maps for one call of
    *          processing of the kernel for strided convolution
    *          - (Ip - Pi/2 + StrideX - 1)/StrideX                             */
   int32_t validColsPerRowOut;
   /*! @brief Valid output rows of output feature maps after convolution for one call of
    *          processing of the kernel for strided convolution for execute phase
    *          ((validRowsIn - Fr - (Fr - 1U)*(DilationY - 1U))/StrideX) + 1U  */
   int32_t validRowsOut;
} MMALIB_CNN_pixelShuffle_row_ixX_ixX_oxX_ExecOutArgs;

/*!
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */
int32_t MMALIB_CNN_pixelShuffle_row_ixX_ixX_oxX_getHandleSize (
    MMALIB_CNN_pixelShuffle_row_ixX_ixX_oxX_InitArgs *pKerInitArgs);

/*!
*  @brief       This function call is required to initialize the handle. In
*               this function most of the one time operation are performed
*               and results are stored in handle
*
*  @param [in]  handle       :  Active handle to the kernel
*  @param [in]  src0_addr    :  Pointer to structure containing dimensional
*                               information of src0 weights/coeffcients
*  @param [in]  src1_addr    :  Pointer to structure containing dimensional
*                               information of src1 feature maps
*  @param [out] dst_addr     :  Pointer to structure containing dimensional
*                               information of dst feature maps
*  @param [in]  pKerInitArgs : Pointer to structure holding init parameters

*  @return      Status of success or Error with Error Codes
*
*  @remarks     Application is expected to do provide valid handle
*/

MMALIB_STATUS MMALIB_CNN_pixelShuffle_row_ixX_ixX_oxX_init (
    MMALIB_kernelHandle                                     handle,
    const MMALIB_bufParams2D_t *                            src0_addr,
    const MMALIB_bufParams2D_t *                            src1_addr,
    const MMALIB_bufParams2D_t *                            dst_addr,
    const MMALIB_CNN_pixelShuffle_row_ixX_ixX_oxX_InitArgs *pKerInitArgs);

/*!
*  @brief       This function call is required to initialize the handle. In
*               this function most of the one time operation are performed
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

MMALIB_STATUS MMALIB_CNN_pixelShuffle_row_ixX_ixX_oxX_init_checkParams (
    MMALIB_kernelHandle                                     handle,
    const MMALIB_bufParams2D_t *                            src0_addr,
    const MMALIB_bufParams2D_t *                            src1_addr,
    const MMALIB_bufParams2D_t *                            dst_addr,
    const MMALIB_CNN_pixelShuffle_row_ixX_ixX_oxX_InitArgs *pKerInitArgs);

/*!
*  @brief       This function is the main compute function, and performs
*               the convolution primitive (conv + ReLU) for CNN on the row
*               based data arrangement. It is called multiple times
*
*  @details     The flow and the expectations of this function are as follows
*               - Performs both strided and non-strided CNN convolution
*               - Function generates partial or full output feature maps with
*                 multiple calls by the application
*               - Functions expect all the data for input and weights available for
*                 one block of output
*               - One output block has 64 output feature maps and 64 columns for 8 bit
*               - One output block has 64 output feature maps and 64 columns for 16 bit
*               - Function computes non multiple of 64 for 8 bit and 32 for 16 bit
*                 of output feature maps without requirement of extra memory
*               - Function takes Bias as compute with a constant value in B matrix and
*                 variable values for A matrix with both 8 bit or 16 bit based on
precision.
*                 example Bias = (A0 + A1 + A2 + ....)*B.
*
*
*  @param [in]  handle      : Active handle to the kernel
*  @param [in]  src0[]      : Pointer to buffer holding convolution weights/coefficents*
*  @param [in]  src1[]      : Pointer to buffer holding input feature map
*  @param [out] dst[]       : Pointer to buffer holding output feature map
*  @param [in]  pKerInArgs  : Pointer to structure holding input Arguments
*  @param [out] pKerOutArgs : Pointer to structure holding output Arguments

*  @return      Status of success or Error with Error Codes
*
*  @par Assumptions:
*    - I/O buffer pointers are assumed to be not aliased.
*
*  @par Performance Considerations:
*    - For best performance, the following parameter settings are recommended:
*      - Set widths equal to strides
*      - Align all pointers to 64 byte boundaries
*      - Set all stride values to a multiple of 64 for 8 bit and 32 for 16 bit
*      - Set all width values to a multiple of 64 for 8 bit and 32 for 16 bit
*      - Set output feature maps to be 64 for 8 bit and 32 for 16 bit
*      - Bias value trained to fit in the B matrix rows upto making the B
*        matrix as multiple of SIMD width
*  @remarks     Application is expected to do call of checkParams function prior
*               to this function as it avoids check of paramaters for each
*               invocation for optimization
*/

MMALIB_STATUS MMALIB_CNN_pixelShuffle_row_ixX_ixX_oxX_exec (
    MMALIB_kernelHandle                                       handle,
    const void *                                              src0,
    const void *                                              src1,
    void *                                                    dst,
    const MMALIB_CNN_pixelShuffle_row_ixX_ixX_oxX_ExecInArgs *pKerInArgs,
    MMALIB_CNN_pixelShuffle_row_ixX_ixX_oxX_ExecOutArgs *     pKerOutArgs);

/*!
*  @brief       This function checks the parameters and should be called
*               before kernel executuon. It can be called once
*
*  @param [in]  handle      : Active handle to the kernel
*  @param [in]  src0[]      : Pointer to buffer holding convolution weights/coefficents*
*  @param [in]  src1[]      : Pointer to buffer holding input feature map
*  @param [out] dst[]       : Pointer to buffer holding output feature map
*  @param [in]  pKerInArgs  : Pointer to structure holding input Arguments

*  @return      Status of success or Error with Error Codes
*
*  @remarks     None
*/

MMALIB_STATUS MMALIB_CNN_pixelShuffle_row_ixX_ixX_oxX_exec_checkParams (
    MMALIB_kernelHandle                                       handle,
    const void *                                              src0,
    const void *                                              src1,
    void *                                                    dst,
    const MMALIB_CNN_pixelShuffle_row_ixX_ixX_oxX_ExecInArgs *pKerInArgs);

/*!
 *  @brief       This function generates the predicate registers once per layer
 *               Predicate buffers are create to identify where to insert pad in
 *               the output generated between consecutive rows. The pad inserted
 *               is either same as the current layer or used for the next layer
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  inputWidth  : Width of Feature map
 *  @param [in]  pad         : Pad between rows
 *  @param [out] inputHeight : Maximum height of feature map
 *  @param [in]  mmaWidth    : MMA width
 *  @param [in]  MChannels   : Number of output channels
 *  @param [in]  subMChannels: Number of output channels per kernel call after convolution
 *                             and before pixel shuffle operation (column-wise
 *                             interleaving only)
 *  @return      number of bytes allocated for the predicate buffer
 *
 *  @remarks     None
 */

int32_t
MMALIB_CNN_generateFillSeamPredicateRegistersPixelShuffle (MMALIB_kernelHandle handle,
                                                           int32_t             inputWidth,
                                                           int32_t             pad,
                                                           int32_t inputHeight,
                                                           int32_t mmaWidth,
                                                           int32_t MChannels,
                                                           int32_t subMChannels);
/*!
 *  @brief       This function provides total bytes needed for seam insertion buffer
 *
 *  @param [in]  inputWidth  : Width of Feature map
 *  @param [in]  pad         : Pad between rows
 *  @param [out] inputHeight : Maximum height of feature map
 *  @param [in]  mmaWidth    : MMA width
 *  @param [in]  MChannels   : Number of output channels
 *  @param [in]  subMChannels: Number of output channels per kernel call after convolution
 *                             and before pixel shuffle operation (column-wise
 *                             interleaving only)
 *  @return      number of bytes allocated for the predicate buffer
 *
 *  @remarks     None
 */

int32_t MMALIB_CNN_seamPredicateRegistersSizePixelShuffle (int32_t inputWidth,
                                                           int32_t pad,
                                                           int32_t inputHeight,
                                                           int32_t mmaWidth,
                                                           int32_t MChannels,
                                                           int32_t subMChannels);

/*!
 *  @brief       This function generates the predicate registers once per layer
 *
 *  @return      number of bytes allocated for the predicate buffer
 *
 *  @remarks     None
 */
int32_t MMALIB_CNN_seamPredicateRegistersSizeDefaultPixelShuffle ();

/*!
 *  @brief       This function generates the performance of MMALIB kernels
 *
 *  @param [in]  src0_addr    :  Pointer to structure containing dimensional
 *                               information of src0 weights/coefficients
 *  @param [in]  src1_addr    :  Pointer to structure containing dimensional
 *                               information of src1 input feature maps
 *  @param [out] dst_addr     :  Pointer to structure containing dimensional
 *                               information of dst output feature maps
 *  @param [in]  pKerInitArgs : Pointer to structure holding init parameters
 *  @param [in]  pKerInArgs   : Pointer to structure holding input Arguments
 *  @param [in]  pKerOutArgs  : Pointer to structure holding output Arguments
 *  @param [in]  iterN        : number of subMBlocks iterations
 *  @param [out]  archCycles  : pointer to store architecture cycles
 *  @param [out]  estCycles   : pointer to store estimated kernel cycles
 *
 *  @remarks     None
 */
void MMA_CNNLIB_pixelShuffle_rowBiasReLUCompute_ixX_ixX_oxX_perfEst (
    const MMALIB_bufParams2D_t *                              src0_addr,
    const MMALIB_bufParams2D_t *                              src1_addr,
    const MMALIB_bufParams2D_t *                              dst_addr,
    MMALIB_CNN_pixelShuffle_row_ixX_ixX_oxX_InitArgs *        pKerInitArgs,
    const MMALIB_CNN_pixelShuffle_row_ixX_ixX_oxX_ExecInArgs *pKerInArgs,
    MMALIB_CNN_pixelShuffle_row_ixX_ixX_oxX_ExecOutArgs *     pKerOutArgs,
    int32_t                                                   iterN,
    uint64_t *                                                archCycles,
    uint64_t *                                                estCycles);

/**@}*/
#ifdef __cplusplus
}
#endif
#endif /* _MMALIB_CNN_PIXELSHUFFLE_ROW_IXX_IXX_OXX_H_ */

/* ======================================================================== */
/*  End of file:  MMALIB_CNN_pixelShuffle_row_ixX_ixX_oxX.h                     */
/* ======================================================================== */
