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
*  @file     MMALIB_CNN_convolveBias_row_ixX_ixX_oxX.h
*
*  @brief    Public header file for MMALIB_CNN_convolveBias_row_ixX_ixX_oxX
*            function
*
*  //version  0.1 - Jan 2018 : Initial Version - [Asheesh Bhardwaj]
*  //version  0.2 - Oct 2020 : updating scale and shift - [Asheesh Bhardwaj]  *
*******************************************************************************
*/

#ifndef MMALIB_CNN_CONVOLVEBIAS_ROW_IXX_IXX_OXX_H_
#define MMALIB_CNN_CONVOLVEBIAS_ROW_IXX_IXX_OXX_H_

#include "../../common/MMALIB_types.h"
#include "MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_processWeights.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
  * @defgroup MMALIB_CNN_convolveBias_row_ixX_ixX_oxX MMALIB_CNN_convolveBias_row_ixX_ixX_oxX
  * @brief Kernel for computing dense CNN convolution with row based processing
  * @details
  *          - Strided and non-strided convolution are supported
  *          - Kernel requires data to be available in memory for coefficients and
  *            feature map
  *          - Kernel writes output feature map into memory
  *          - Kernel performs on the fly padding and feature maps are not required to be padded in memory
  *          - Strided convolution is optimized for 1x1 stride 2, 3x3 stride 2, 3x3 stride 3, 5x5 stride 2,
  *            7x7 stride 2, 11x11 stride 4
  *          - The input feature maps are 8 bit, 16 bit signed or unsigned
  *          - The weights are 8 bit, 16 bit signed
  *          - The output feature maps are 8 bit, 16 bit signed or unsigned
  *          - The bias is loaded in a array for each output feature map as 32bit precision for 8 bit and 64bit precision for 6bit
  *          - The output of each feature map can be scaled(8 bit) and shift(8 bit) after accumulation
  *          - Programmable saturation is supported on the output values
  *
  *          The input feature maps are passed to a kernel with all the rows are next to each other. All the input feature maps of same number of pixels fed into the kernel buffer.
  *          When starting processing the feature maps can start at a given column -> col parameter of a row and kernel will start processing from the intermediate location using the parameter.
  *
  *          \image html ConvolveBias.svg "Input buffer for strided and non strided convolution"
  *
  *          \image html rowFlow.svg "starting location with col parameter"
  *
  *          The filter coeffieint buffer for each output feature map is layer out in a linear manner with Ni*Fr*Fc values. The dilated kernel coefficients are used without the dilated zero values.
  *
  *          \image html ConvolveBias_filter.svg "Filter coefficients buffer"
  *
  *          The kernel requires multiple handles for a given feature maps which will be prestored in the L1D memory. For a givn CNN layer these handles will be fixed.
  *          There are three categories of these handles
  *          - Top row handles which require top pad, right pad and left pad
  *          - Middle row handles which require right pad and left pad
  *          - Bottom row handle which require right pad, left pad and bottom pad
  *          - The top row handles will require the starting pointer srcPtr to be at the beginning of the first pixel of the feature map with "col" location provided
  *          - The middle row handles can have the pointer starting at any location with "col" location provided
  *          - The bottom row handles can have the pointer starting at any location with "col" location provided and also validColsOutBottom as input to the kernel
  *
  *          \image html 3x3stride1_row.svg "Examples of parameters for a different handles for 3x3 stride 1 convolution"
  *
  *          Strided convolution
  *          - The strided convolution will have all the handles to starting pointer srcPtr at the first pixel of the row.
  *          - Strided convolution generates complete row for a kernel call
  *          - The handles for top row, middle row and bottom row will be different
  *
  *          \image html 3x3stride2_row.svg "Examples of parameters for a different handles for 3x3 stride 2 convolution"
  *
  * @ingroup  MMALIB_CNN
 */

/**@{*/

/*!
 * @brief Enum to define the error codes
 */
typedef enum
{
   MMALIB_CNN_CONVOLVEBIAS_ROW_IXX_IXX_OXX_ERR_SMALL_K  = MMALIB_ERROR_MAX, /** Error case because k < Ni*Fr*Fc */
   MMALIB_CNN_CONVOLVEBIAS_ROW_IXX_IXX_OXX_ERR_MAX
} MMALIB_CNN_CONVOLVEBIAS_ROW_IXX_IXX_OXX_STATUS_NAME ;


/*!
 * @brief Structure containing the parameters
 * initialization of CNN convolution computation
 */
typedef struct
{
   /*! @brief Variant of the function refer to @ref MMALIB_FUNCTION_STYLE     */
   int8_t    funcStyle;
   /*! @brief Number of output feature maps                                   */
   int32_t   No;
   /*! @brief offset of the input feature maps in B matrix. This is power of 2
    * for circular buffer and atleast 64 byte aligned for linear buffer       */
   int32_t   inChOffset;
   /*! @brief Valid columns of input feature maps in B matrix for one call of
    * processing of the kernel for non strided convolution but including
    * 1x1 stride 2. This is in units of data type                             */
   int32_t   validColsIn;
   /*! @brief Valid columns in a row of input feature maps for one call of
    * processing of the kernel for non strided convolution and for
    * 1x1 stride 2. This will not be used in J7AM instead use inWidth         */
   int32_t   validColsPerRowIn;
   /*! @brief Valid input rows of input feature maps for one call of
    * processing of the kernel for strided convolution except for
    * 1x1 stride 2                                                            */
   int32_t   validRowsIn;
   /*! @brief Valid pitch for each input rows of input feature maps
    *  for strided convolution.  This is units of bytes                       */
   int32_t   inputPitchPerRow;
   /*! @brief Valid output pitch for each output rows of output feature maps
    *  for strided convolution. This is units of bytes                        */
   int32_t   outputPitchPerRow;
   /*! @brief Width of each row of input feature map in units of data type    */
   int32_t   inWidth;
   /*! @brief Pad of each row of input feature map, specify pad only one one
    * side. specific to J7ES                                                                   */
   int32_t   pad;
   /*! @brief Pad of each row of input feature map, specify pad only on left
    * side                                                                    */
   int32_t   padLeft;
   /*! @brief Pad of each row of input feature map, specify pad only on right
    * side                                                                    */
   int32_t   padRight;
   /*! @brief Pad of each row of input feature map, specify pad only on top
    * side                                                                    */
   int32_t   padTop;
   /*! @brief Pad of each row of input feature map, specify pad only on bottom
    * side                                                                    */
   int32_t   padBottom;
   /*! @brief Output pixels for stride 1 flow when padBotton > 0              */
   int32_t  validColsOutBottom;
   /*! @brief Height of the input feature map in units of data type           */
   int32_t   maxHeight;
   /*! @brief Number of output channels per kernel call                       */
   int32_t   subMChannels;
//   /*! @brief Scale parameter for output precision                            */
//   uint8_t   scale;
//   /*! @brief Shift parameter for output precision                            */
//   int32_t   shift;
   /*! @brief number of groups per kernel call > 1 will enable processing
    * when No <= MMA size for non strided kernels and default value is 1      */
   int32_t   numGroupsPerKernel;
   /*! @brief coefficient rows (height)                                       */
   int32_t   Fr;
   /*! @brief coefficient columns (width)                                     */
   int32_t   Fc;
   /*! @brief stride of columns                                               */
   int32_t   strideX;
   /*! @brief stride of rows                                                  */
   int32_t   strideY;
   /*! @brief dilation of coefficients of columns                             */
   int32_t   dilationX;
   /*! @brief dilation of coefficients of rows                                */
   int32_t   dilationY;
   /*! @brief bias value in B matrix same as data type of B matrix            */
   int32_t   bias;
   /*! @brief activation RELU, SAT or none for output                         */
   uint8_t   activationType;
   /*! @brief Min value for parameterized saturation                          */
   int32_t   pSatMin;
   /*! @brief Max value for parameterized saturation                          */
   int32_t   pSatMax;
   /*! @brief mode for input feature map in Circular or Linear mode in
    * B matrix                                                                */
   uint8_t   mode;
   /*! @brief column number of input feature map                              */
   int32_t   col;

} MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_InitArgs ;

/*!
 * @brief Structure containing the parameters
 * for input to the execute phase of CNN convolution computation
 * These parameters will not exist in J7AM, kept for J7ES compatibility
 */

typedef struct
{ 
   /*! @brief Valid columns in a row of input feature maps for one call of
   * processing of the kernel for non strided convolution and for
   * 1x1 stride 2 for execute phase                                           */
   int32_t   validColsIn;      // Width of buffer C
   /*! @brief Valid columns in a row of input feature maps for one call of
    * processing of the kernel for strided convolution except for
    * 1x1 stride 2  for execute phase                                         */
   int32_t   validColsPerRowIn;
   /*! @brief Valid input rows of input feature maps for one call of
    * processing of the kernel for strided convolution except for
    * 1x1 stride 2 for execute phase                                          */
   int32_t   validRowsIn;
   /*! @brief column offset of each row of input feature map enables with
    *  a flag in execute phase                                                */
   int32_t   col;
   /*! @brief Number of output channels per kernel call in execute phase      */
   int32_t   subMChannels;
   /*! @brief quantMethod 0 - per call 1 - per channel                        */
   int32_t   quantMethod;
   /*! @brief pad fill value                                                  */
   int32_t   padFillValue;
   /*! @brief Flag indicating whether to enable dynamic range range (min,max) calculation */
   MMALIB_enableDynamicRange_type enableDynamicRange;
   /*! @brief Flag indicating whether to reset dynamic range (min,max) calculation at beginning of compute */
   MMALIB_initDynamicRange_type   initDynamicRange;

} MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_ExecInArgs ;


/*!
 * @brief Structure containing the parameters
 * for output from the execute phase of CNN convolution computation
 */
typedef struct
{ 
   /*! @brief Valid columns out in a row of output feature maps for one call of
   * processing of the kernel for non strided convolution and for
   * 1x1 stride 2  for execute phase                                         */
   int32_t validColsOut;
   /*! @brief Valid columns in a row of output feature maps for one call of
   * processing of the kernel for strided convolution except for
   * 1x1 stride 2  for execute phase                                         */
   int32_t validColsPerRowOut;
   /*! @brief Valid output rows of output feature maps for one call of
   * processing of the kernel for strided convolution except for
   * 1x1 stride 2 for execute phase                                          */
   int32_t validRowsOut;
   /*! @brief Minimum computed value, controlled with MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_ExecInArgs.initDynamicRange */
   int64_t dynamicRangeMin;
   /*! @brief Maximum computed value, controlled with MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_ExecInArgs.initDynamicRange */
   int64_t dynamicRangeMax;

} MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_ExecOutArgs ;

/*!
 *  @brief        This is a query function to calculate the size of internal
 *                handle
 *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
 *  @return       Size of the buffer in bytes
 *  @remarks      Application is expected to allocate buffer of the requested
 *                size and provide it as input to other functions requiring it.
 */
int32_t MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_getHandleSize(
   MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_InitArgs *pKerInitArgs);


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
*  @param [in]  src2_addr    :  Pointer to structure containing dimensional
*                               information of src2 bias
*  @param [in]  src3_addr    :  Pointer to structure containing dimensional
*                               information of src3 scale values
*  @param [out] dst_addr     :  Pointer to structure containing dimensional 
*                               information of dst feature maps
*  @param [in]  pKerInitArgs : Pointer to structure holding init parameters

*  @return      Status of success or Error with Error Codes 
*
*  @remarks     Application is expected to do provide valid handle
*/

MMALIB_STATUS MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_init(
   MMALIB_kernelHandle handle,
   const MMALIB_bufParams2D_t * src0_addr,
   const MMALIB_bufParams2D_t * src1_addr,
   const MMALIB_bufParams2D_t * src2_addr,
   const MMALIB_bufParams1D_t * src3_addr,
//   const MMALIB_bufParams2D_t * src4_addr,
   const MMALIB_bufParams3D_t * dst_addr ,
   const MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_InitArgs *pKerInitArgs);

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
*  @param [in]  src2_addr    :  Pointer to structure containing dimensional
*                               information of src2 bias
*  @param [in]  src3_addr    :  Pointer to structure containing dimensional
*                               information of src3 scale values
*  @param [out] dst_addr     :  Pointer to structure containing dimensional
*                               information of dst output feature maps
*  @param [in]  pKerInitArgs : Pointer to structure holding init parameters

*  @return      Status of success or Error with Error Codes
*
*  @remarks     Application is expected to do provide valid handle
*/



MMALIB_STATUS MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_init_checkParams(
   MMALIB_kernelHandle handle,
   const MMALIB_bufParams2D_t * src0_addr,
   const MMALIB_bufParams2D_t * src1_addr,
   const MMALIB_bufParams2D_t * src2_addr,
   const MMALIB_bufParams1D_t * src3_addr,
//   const MMALIB_bufParams2D_t * src4_addr,
   const MMALIB_bufParams3D_t * dst_addr,
   const MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_InitArgs *pKerInitArgs);

/*!
*  @brief       This function is the main compute function, and performs
*               the convolution primitive (conv + ReLU) for CNN on the row 
*               based data arrangement. It is called multiple times
*
*  @details     The flow and the expectations of this function are as follows
*               - Performs both strided and non-strided CNN convolution
*               - Function generates partial or full output feature maps with
*                 multiple calls by the application
*               - Function creates atleast three output blocks when KBlocks is
*                 less than 3
*               - Function creates at least one output block when KBlocks is greater
*                 than equal to 3 except for 1x1 stride 2 convolution has greater
*                 than 3
*               - Functions expect all the data for input and weights available for
*                 one block of output
*               - One output block has 64 output feature maps and 64 columns for 8 bit
*               - One output block has 64 output feature maps and 64 columns for 16 bit
*               - Function computes non multiple of 64 for 8 bit and 32 for 16 bit
*                 of output feature maps without requirement of extra memory
*               - Function takes Bias as compute with a constant value in B matrix and
*                 variable values for A matrix with both 8 bit or 16 bit based on precision.
*                 example Bias = (A0 + A1 + A2 + ....)*B.
*
*
*  @param [in]  handle      : Active handle to the kernel
*  @param [in]  src0[]      : Pointer to buffer holding convolution weights/coefficents*
*  @param [in]  src1[]      : Pointer to buffer holding input feature map
*  @param [in]  src2[]      : Pointer to buffer holding the bias
*  @param [in]  src3[]      : Pointer to buffer holding the scale values
*  @param [in]  src4[]      : Pointer to buffer holding the shift values
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


MMALIB_STATUS MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_exec(
   MMALIB_kernelHandle handle,
   const void *src0,
   const void *src1,
   const void *src2,
   const void *src3,
   const uint8_t *src4,
   void *dst,
   const MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_ExecInArgs *pKerInArgs,
   MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_ExecOutArgs *pKerOutArgs);


/*!
*  @brief       This function checks the parameters and should be called
*               before kernel executuon. It can be called once 
*
*  @param [in]  handle      : Active handle to the kernel
*  @param [in]  src0[]      : Pointer to buffer holding convolution weights/coefficents*
*  @param [in]  src1[]      : Pointer to buffer holding input feature map
*  @param [in]  src2[]      : Pointer to buffer holding bias
*  @param [in]  src3[]      : Pointer to buffer holding scale
*  @param [in]  src4[]      : Pointer to buffer holding shift
*  @param [out] dst[]       : Pointer to buffer holding output feature map
*  @param [in]  pKerInArgs  : Pointer to structure holding input Arguments

*  @return      Status of success or Error with Error Codes 
*
*  @remarks     None
*/

MMALIB_STATUS MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_exec_checkParams(
   MMALIB_kernelHandle handle,
   const void *src0,
   const void *src1,
   const void *src2,
   const void *src3,
   const uint8_t *src4,
   void *dst,
   const MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_ExecInArgs *pKerInArgs);

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
void MMA_CNNLIB_convolveBias_ixX_ixX_oxX_perfEst (
    const MMALIB_bufParams2D_t *src0_addr,
    const MMALIB_bufParams2D_t *src1_addr,
    const MMALIB_bufParams3D_t *dst_addr,
    MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_InitArgs *pKerInitArgs,
    const MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_ExecInArgs *pKerInArgs,
    MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_ExecOutArgs *pKerOutArgs,
    int32_t iterN,
    uint64_t *archCycles,
    uint64_t *estCycles);

/**@}*/
#ifdef __cplusplus
}
#endif
#endif /* _MMALIB_CNN_CONVOLVEBIAS_ROW_IXX_IXX_OXX_H_ */

/* ======================================================================== */
/*  End of file:  MMALIB_CNN_convolveBias_row_ixX_ixX_oxX.h                     */
/* ======================================================================== */

