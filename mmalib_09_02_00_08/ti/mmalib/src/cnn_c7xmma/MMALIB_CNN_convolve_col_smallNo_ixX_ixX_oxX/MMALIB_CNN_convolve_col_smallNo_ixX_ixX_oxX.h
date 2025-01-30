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

/********************************************************************************
 *  @file     MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX.h
 *
 *  @brief    Public header file for MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX
 *            function
 *
 *  @version  0.1 - Feb 2018 : Initial Version
 *
 ********************************************************************************/

#ifndef MMALIB_CNN_CONVOLVE_COL_SMALLNO_IXX_IXX_OXX_H_
#define MMALIB_CNN_CONVOLVE_COL_SMALLNO_IXX_IXX_OXX_H_

#include "../../common/MMALIB_types.h"
#include "MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_reorderWeights.h"

#ifdef __cplusplus
extern "C" {
#endif

   /*!
    *******************************************************************************
    * @defgroup MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX
    * @brief Kernel for computing CNN-style 2D convolution using column major
    *        data ordering on the input and output feature maps.  This approach computes more
    *        quickly if filter grouping is chosen such that Ni=No=1, or if filter grouping is
    *        chosen such that NiFrFc < MMA_SIZE, otherwise use regular convolution
    *        method @ref MMALIB_CNN_convolve_row_ixX_ixX_oxX.
    *        This kernel is also referred to as depth-wise convolution.
    * @details
    *        The kernel is designed to process a pair of MMA-wide columns at a time, although processing a single column
    *        is supported.  These two columns are typically adjacent columns in the same input feature map, although
    *        provisions are made in the interface for the two columns to come from separate input feature maps.
    *
    *        On C7100 devices, this kernel requires that the input feature maps are padded before they are passed to the kernel 
    *        whereas other devices support padding inside this kernel.
    *
    *        This kernel differs from @ref MMALIB_CNN_convolve_col_smallNo_highPrecision in that it requires all scale, shift and bias values to be 
    *        the constant across all groups called at the same time.  This restriction enables the same amount of input data to be processed
    *        in fewer cycles.
    *
    *          - Supports Ni == No for small values of No
    *          - 8 and 16-bit integer data type support
    *          - 3x3, 5x5 and 7x7(8-bit only) kernel sizes supported
    *          - Supports stride values of 1 or 2
    *             - When stride == 2, the number of rows in an input feature map should be even, including the top and bottom pad
    *             -- i.e. (pKerInitArgs->blockFeatureHeight + pKerInitArgs->topPad + pKerInitArgs->bottomPad) % 2 == 0
    *             - When stride == 2, the kernel size must be 3x3 or 5x5
    *             - When stride == 2 and the number of groups > 1, then there must be space allocated for an even number of input feature map groups. 
    *             -- If the number of groups is odd (i.e. 5), then space in L2 memory must be allocated for the next largest even number of groups (i.e. 6)
    *             -- This extra memory does not need to be initialized, but an exception will be generated if the addresses where the extra group would go
    *             -- are outside the L2 memory region. 
    *          - May compute multiple groups in a single call
    *
    *        The input data for this kernel consists of the filter coefficients and the input feature maps, while the
    *        output is the output feature maps.  The kernel requires the filter coefficients to be preprocessed and
    *        stored in a custom arrangement prior to calling the kernel execute function.  This reordering can be done
    *        offline (more efficient) or at runtime.  MMALIB provides the utilities in
    *        @ref MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_reorderWeights to generate this reordering (src0) and the
    *        associated @ref MMALIB_bufParams2D_t struct (src0_addr).
    *
    *        The input feature map memory arrangement is flexible and is described by the parameters illustrated in the
    *        the figure below.  Note that the input feature maps for each groups must be vertically stacked on top of
    *        each other as there is no parameter to control the offset between groups.
    *          \image html convolve_col_inputFeatureMapInterface.svg
    *          \image latex convolve_col_inputFeatureMapInterface.eps
    *
    *        The output feature map is described by the parameters illustrated in the
    *        the figure below.
    *          \image html convolve_col_outputFeatureMapInterface.svg
    *          \image latex convolve_col_outputFeatureMapInterface.eps
    *
    * @ingroup  MMALIB_CNN
    *******************************************************************************
    */
   /**@{**/

   /*!
    *******************************************************************************
    *  @enum       MMALIB_CNN_CONVOLVE_COL_SMALLNO_IXX_IXX_OXX_STATUS_NAME
    *  @brief      Enumeration for different error codes for MMALIB_CNN_CONVOLVE_COL_SMALLNO kernel
    *******************************************************************************
    */

   typedef enum
   {
      MMALIB_CNN_CONVOLVE_COL_SMALLNO_IXX_IXX_OXX_ERR_SMALL_K  = MMALIB_ERROR_MAX, /** Error case because k < Ni*Fr*Fc */
      /* @todo Add more appropriate error codes */
      MMALIB_CNN_CONVOLVE_COL_SMALLNO_IXX_IXX_OXX_ERR_MAX
   } MMALIB_CNN_CONVOLVE_COL_SMALLNO_IXX_IXX_OXX_STATUS_NAME ;
   
#define MMALIB_CONVOLVE_COL_SHIFT_SINGLE 0      //!< Use the same shift value for all groups in calls to @ref MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_exec
#define MMALIB_CONVOLVE_COL_SHIFT_PER_GROUP 1   //!< [Future feature, no yet supported] Use the unique shift value for each group in calls to @ref MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_exec

   /*!
    *******************************************************************************
    * @struct MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_InitArgs
    * @brief This structure holds all the initialization parameters for CNN column based
    convolution kernel
    *******************************************************************************
    */
   typedef struct
   {
      int8_t    funcStyle;      //!< Variant of the function; refer to @ref MMALIB_FUNCTION_STYLE
      int32_t   Ni;             //!< Number of input channels to be processed
      int32_t   No;             //!< Number of output channels to be processed
      int32_t   Fr;             //!< Number of kernel rows (height)
      int32_t   Fc;             //!< Number of kernel columns (width)
      uint8_t   shift;          //!< Number of bits of right shift parameter for output precision
      int8_t    shiftMethod;    //!< Control behavior of MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_InitArgs.shift.  @ref MMALIB_CONVOLVE_COL_SHIFT_SINGLE is currently the only supported value.
      int32_t   strideX;        //!< Stride size across columns (horizontal dimension)
      int32_t   strideY;        //!< Stride size across rows (vertical dimension)
      int32_t   dilationX;      //!< Dilation of kernel width
      int32_t   dilationY;      //!< Dilation of kernel height
      int32_t   bias;           //!< Bias value in B matrix
      int32_t   biasDataType;   //!< Data type of the bias
      int32_t   numBiasVals;    //!< Number of elements used for the bias (cols in weights, rows in feature maps)
      uint8_t   activationType; //!< Activation of RELU, SAT or none for output
      int32_t   featureWidth;   //!< Total feature map width, without pad
      int32_t   blockFeatureHeight; //!< Number of rows from the input feature map to be processed, without pad
      int32_t   blockFeaturePitch; //!< Pitch between rows of input feature map, in bytes, without pad
      int32_t   columnOffset;   //!< Distance, in elements, between starting points of even-numbered input feature map block columns (0,2,4,...)
      int32_t   inPairOffset;   //!< Distance, in elements, between the even- and odd-numbered input feature map block columns (i.e. distance from 0 to 1)
      int32_t   topPad;         //!< pixels of pad to apply to the top side of the input data
      int32_t   bottomPad;      //!< pixels of pad to apply to the bottom side of the input data
      int32_t   leftPad;        //!< pixels of pad to apply to the left side of the input data
      int32_t   rightPad;       //!< pixels of pad to apply to the right side of the input data
      int32_t   groupOffset;    //!< Distance, in elements, between groups
      int32_t   pSatMin;        //!< Minimum value for parameterized saturation 
      int32_t   pSatMax;        //!< Maximum value for parameterized saturation 
      int32_t   inChOffset;     //!< Distance, in elements, between the input channels
      int32_t   outPairOffset;  //!< Distance, in elements, between the output of the even- and odd-numbered output feature map block columns
      int32_t   numGroupsPerKernel; //!< Number of groups to be processed in a single kernel call
   } MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_InitArgs ;


   /*!
    *******************************************************************************
    * @struct MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_ExecInArgs
    * @brief This structure holds all the execution input parameters for the CNN column based convolution kernel
    *******************************************************************************
    */

   typedef struct
   {
      int32_t   blockFeatureWidth;                       //!< Width of the input feature map to be processed
      int32_t   padFillValue;                            //!< Pad fill value (not supported for C7100 devices)
      MMALIB_enableDynamicRange_type enableDynamicRange; //!< Flag indicating whether to enable dynamic range range (min,max) calculation.
      MMALIB_initDynamicRange_type  initDynamicRange;    //!< Flag indicating whether to reset dynamic range (min,max) calculation at beginning of compute.
   } MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_ExecInArgs ;


   /*!
    *******************************************************************************
    * @struct MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_ExecOutArgs
    * @brief This structure holds all the runtime output parameters for CNN column based convolution kernel
    *******************************************************************************
    */

   typedef struct
   {
      int32_t validColsOut;    //!< Width of buffer C
      int64_t dynamicRangeMin;      //!< Minimum computed value, controlled with MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_ExecInArgs.initDynamicRange
      int64_t dynamicRangeMax;      //!< Maximum computed value, controlled with MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_ExecInArgs.initDynamicRange
   } MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_ExecOutArgs ;


   /*!
    *******************************************************************************
    *  @brief        This is a query function to kernel to get the size of internal
    *                handle
    *  @param [in]   pKerInitArgs  : Pointer to structure holding init parameters
    *  @return       Size of the buffer in bytes
    *  @remarks      Application is expected to allocate buffer of the requested
    *                size and provide it during init and exec function calls
    *******************************************************************************
    */

   int32_t MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_getHandleSize(MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_InitArgs *pKerInitArgs);


   /*!
    *******************************************************************************
    *  @brief       This function call is required to initialize the handle. In
    *               this function most of the one time operations are performed
    *               and results are stored in the handle
    *  @param [in]  handle       :  Active handle to the kernel
    *  @param [in]  src0_addr    :  Pointer to structure containing dimensional
    *                               information of src0, content at this pointer address must be generated by
    *                               @ref MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_reorderWeights_fillBufParams
    *  @param [in]  src1_addr    :  Pointer to structure containing dimensional
    *                               information of src1
    *  @param [out] dst_addr     :  Pointer to structure containing dimensional
    *                               information of dst
    *  @param [in]  pKerInitArgs : Pointer to structure holding init parameters
    *  @return      Status of success or error with error codes, refer to @ref MMALIB_STATUS.
    *
    *  @remarks     Application is expected to provide a valid handle
    *******************************************************************************
    */

   MMALIB_STATUS MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_init(MMALIB_kernelHandle handle,
								  const MMALIB_bufParams2D_t * src0_addr,
								  const MMALIB_bufParams2D_t * src1_addr,
								  const MMALIB_bufParams3D_t * dst_addr ,
								  const MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_InitArgs *pKerInitArgs);


   /*!
    *******************************************************************************
    *  @brief       This function checks the parameters and should be called
    *               before kernel executuon. It can be called once.
    *  @param [in]  handle      : Active handle to the kernel
    *  @param [in]  src0_addr    :  Pointer to structure containing dimensional
    *                               information of src0
    *  @param [in]  src1_addr    :  Pointer to structure containing dimensional
    *                               information of src1
    *  @param [out] dst_addr     :  Pointer to structure containing dimensional
    *                               information of dst
    *  @param [in]  pKerInitArgs : Pointer to structure holding init parameters
    *  @return      Status of success or error with error codes,
    *               refer to @ref MMALIB_STATUS.
    *
    *  @remarks     None
    ********************************************************************************
    */

   MMALIB_STATUS MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_init_checkParams(MMALIB_kernelHandle handle,
									      const MMALIB_bufParams2D_t * src0_addr,
									      const MMALIB_bufParams2D_t * src1_addr,
									      const MMALIB_bufParams3D_t * dst_addr,
									      const MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_InitArgs *pKerInitArgs);


   /*!
    *******************************************************************************
    *  @brief       This function is the main compute function, and performs
    *               the convolution primitive (conv + ReLU) for CNN on the column
    *               based data arrangement. It is called multiple times.
    *
    *
    *  @param [in]  handle      : Active handle to the kernel
    *  @param [in]  src0[]      : Pointer to buffer holding convolution weights. Content at this pointer address must be generated by
    *                             @ref MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_reorderWeights_exec.
    *                             [ A matrix]
    *  @param [in]  src1[]      : Pointer to buffer holding input feature map data.
    *                             [ B matrix]
    *  @param [in]  shiftValues[] : Pointer to buffer of shift values to apply results prior to output.
    *                              Set to NULL if using @ref MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_InitArgs.shiftMethod = @ref MMALIB_CONVOLVE_COL_SHIFT_SINGLE, which is currently the only supported option.
    *                              [Future development] Set to numGroupsPerKernel if using @ref MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_InitArgs.shiftMethod = @ref MMALIB_CONVOLVE_COL_SHIFT_PER_GROUP.
    *  @param [in]  biasBValues[] : Pointer to buffer of bias values to load into the MMA B matrix.
    *                              Set to NULL if using @ref MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_InitArgs.shiftMethod = @ref MMALIB_CONVOLVE_COL_SHIFT_SINGLE, which is currently the only supported option.
    *                              [Future development] Set to numGroupsPerKernel if using @ref MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_InitArgs.shiftMethod = @ref MMALIB_CONVOLVE_COL_SHIFT_PER_GROUP.
    *                              @ref MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_InitArgs.numBiasVals must == 1 for @ref MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_InitArgs.shiftMethod = @ref MMALIB_CONVOLVE_COL_SHIFT_PER_GROUP
    *  @param [out] dst[]       : Pointer to buffer holding output feature map data.
    *                              [ C matrix]
    *  @param [in]  pKerInArgs  : Pointer to structure holding input arguments
    *  @param [out] pKerOutArgs : Pointer to structure holding output arguments

    *  @return      Status of success or error with error codes, refer @ref MMALIB_STATUS.
    *
    *  @par Assumptions:
    *    - I/O buffer pointers are assumed to be not aliased.
    *    - Fr = Fc = 3, 5 or 7 (defaults to natural-c if not the case, 7 valid for 8-bit data type only)
    *    - stride-by-2 calls have an even number of rows in the feature map (defaults to natural-c if not the case)
    *    - Ni == No (defaults to natural-c if not the case)
    *    - Ni*Fr*Fc < MMA_SIZE (defaults to natural-c if not the case)
    *
    *  @par Performance Considerations:
    *    - For best performance, the following parameter settings are recommended:
    *      - Set widths equal to strides
    *      - Align all pointers to 8 byte boundaries
    *      - Set all stride values to a multiple of 8
    *      - Set all width values to a multiple of 16
    *
    *
    *  @remarks     Application is expected to call the checkParams function prior
    *               to this function as it avoids check of paramaters for each
    *               invocation for optimization
    *******************************************************************************
    */

   MMALIB_STATUS MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_exec(MMALIB_kernelHandle handle,
                                                                  const void *src0,
                                                                  const void *src1,
                                                                  const uint8_t *shiftValues,
                                                                  const int32_t *biasBValues,
                                                                  void *restrict dst,
                                                                  const MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_ExecInArgs *pKerInArgs,
                                                                  MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_ExecOutArgs *pKerOutArgs);


   /*!
    *******************************************************************************
    *  @brief       This function checks the parameters and should be called
    *               before kernel executuon. It can be called once
    *  @param [in]  handle      : Active handle to the kernel
    *  @param [in]  src0[]      : Pointer to buffer holding convolution weights
    *                             [ A matrix]
    *  @param [in]  src1[]      : Pointer to buffer holding input feature map data
    *                             [ B matrix]
    *  @param [in]  shiftValues[] : Pointer to buffer of shift values to apply results prior to output
    *  @param [in]  biasBValues[] : Pointer to buffer of bias values to load into the MMA B-matrix
    *  @param [out] dst[]       : Pointer to buffer holding output feature map data
    *                              [ C matrix]
    *  @param [in]  pKerInArgs  : Pointer to structure holding input arguments

    *  @return      Status of success or error with error codes
    *               refer @ref MMALIB_STATUS.
    *
    *  @remarks     None
    *******************************************************************************
    */

   MMALIB_STATUS MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_exec_checkParams(MMALIB_kernelHandle handle,
                                                                              const void *src0,
                                                                              const void *src1,
                                                                              const uint8_t *shiftValues,
                                                                              const int32_t *biasBValues,
                                                                              const void *dst,
                                                                              const MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_ExecInArgs *pKerInArgs);

   
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
    *  @param [in]  pKerInitArgs :  Pointer to structure holding init parameters
    *  @param [in]  pExecInArgs  :  Pointer to structure holding execute input arguments
    *  @param [in]  pExecOutArgs :  Pointer to structure holding execute output arguments
    *  @param [out] archCycles   :  Cycles estimated for the compute, startup and
    *                               teardown
    *  @param [out] estCycles    :  Cycles estimated for the compute, startup,
    *                               teardown and any associated overhead
    *
    *  @remarks     None
    *******************************************************************************
    */
   
   void MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_perfEst( MMALIB_kernelHandle handle,
                                                            const MMALIB_bufParams2D_t * src0_addr,
                                                            const MMALIB_bufParams2D_t * src1_addr,
                                                            const MMALIB_bufParams3D_t * dst_addr,
                                                            const MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_InitArgs *pKerInitArgs,
                                                            const MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_ExecInArgs *pExecInArgs,
                                                            MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_ExecOutArgs *pExecOutArgs,
                                                            uint64_t *archCycles,  uint64_t *estCycles);
   
   /**@}**/
#ifdef __cplusplus
}
#endif
#endif /* MMALIB_CNN_CONVOLVE_COL_SMALLNO_IXX_IXX_OXX_H_ */

/* ======================================================================== */
/*  End of file:  MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX.h                     */
/* ======================================================================== */
