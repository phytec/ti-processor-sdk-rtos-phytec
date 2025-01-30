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
*  @file     MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_reorderWeights.h
*
*  @brief    Public header file for MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_reorderWeights
*            function
*
*  @version  0.1 - Dec 2018 : Initial Version
*            
********************************************************************************/

#ifndef MMALIB_CNN_CONVOLVE_COL_SMALLNO_IXX_IXX_OXX_REORDERWEIGHTS_H_
#define MMALIB_CNN_CONVOLVE_COL_SMALLNO_IXX_IXX_OXX_REORDERWEIGHTS_H_

#include "../../common/MMALIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif
  

/*!
 * @defgroup MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_reorderWeights MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_reorderWeights
 *
 * @brief
 *        @ref MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX requires that the weights be preprocessed into
 *        a specific arrangement.  The functions in this module perform that preprocessing and other associated tasks.
 *
 * @details
 *        This preprocessing can be done offline (more efficient) or at runtime.  The input weights are assumed to be
 *        stored as illustrated in this Python pseudocode:
\code{.py}
 
   print('{')
   for g in range(0, numGroupsPerKernel):
      for no in range(0, No):
         for ni in range(0, Ni):
            for row in range(0, Fr):
               for col in range(0, Fc):
                  print("{:>3}, ".format(pWeights[no + g*No, ni, row, col]), end='')
 
         for b in range(0,numBiasVals):
            print('{:>5}'.format(pBias[no + g*No, b])), end=',')
         print('')
      print('')
   print('};')
\endcode
 *
 * @ingroup  MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX
 */
/**@{**/

   
   /**
    *  @enum       MMALIB_reorder_weights_operation_e
    *  @brief      Enumeration for different operations for the reorderWeights function
    *
    */
   typedef enum _MMALIB_reorder_weights_operation_e
   {
      REORDER_WEIGHTS_AND_BIAS,
      REORDER_WEIGHTS,
      REORDER_BIAS,
   } MMALIB_reorder_weights_operation_e;

  
   /*!
    *******************************************************************************
    @struct MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_reorderWeights_Args
    @brief  This structure holds all the input parameters for reordering CNN filter
    weights for column-based convolution kernel
    ********************************************************************************
    */
   
   typedef struct
   {
      int32_t dataType;       //!< Data type must be a member of @ref MMALIB_data_type_e
      int32_t Ni;             //!< Number of input channels to be processed
      int32_t No;             //!< Number of output channels to be processed
      int32_t Fr;             //!< Kernel height
      int32_t Fc;             //!< Kernel width
      int32_t strideX;        //!< Stride in the horizontal dimension
      int32_t strideY;        //!< Stride in the vertical dimension
      int32_t dilationX;      //!< Dilation in the horizontal dimension
      int32_t featureWidth;   //!< Total feature width
      int32_t blockFeatureHeight; //!< Number of rows from the input feature map to be processed, including top and bottom pad that is to be applied inside the kernel
      int32_t topPad;             //!< Pixels of pad to apply to the top side of the input data
      int32_t bottomPad;          //!< Pixels of pad to apply to the bottom side of the input data
      int32_t leftPad;            //!< Pixels of pad to apply to the left side of the input data
      int32_t rightPad;           //!< Pixels of pad to apply to the rights side of the input data
      int32_t numBiasVals;        //!< Number of elements used for the bias (cols in weights, rows in feature maps)
      int32_t numGroupsPerKernel; //!< Number of groups to be processed in kernel call
   } MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_reorderWeights_Args ;
      
   
   /*!
    ********************************************************************************
    *  @brief       This function returns the amount of memory that needs to be
    *               allocated for reordered kernel coefficients needed to support
    *               @ref MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX
    *
    *  @param [in]  pArgs  :  Pointer to the structure containing the required dimensional information
    *
    *  @return      Number of bytes required to store the reordered kernel coefficients
    *
    *  @remarks     Application is expected allocate this amount of memory for reordered coefficients
    *******************************************************************************
    */
   
   int32_t MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_reorderWeights_getMemorySize(const MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_reorderWeights_Args *pArgs);
   
   
   /*!
    ********************************************************************************
    *  @brief       This function populates a structure of dimensional information
    *               about the reordered coefficients buffer from a
    *               @ref MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_reorderWeights_Args
    *               structure.
    *
    *  @param [in]  pArgs                    :  Pointer to the structure containing the required dimensional information
    *  @param [out] pReorderedWeights_addr   :  Pointer to structure containing dimensional
    *                                             information of the reordered weights buffer.
    *
    *  @return      Status of success or error with error codes, refer to @ref MMALIB_STATUS.
    *
    *  @remarks     Application is expected to provide a valid handle
    *******************************************************************************
    */
   
   MMALIB_STATUS MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_reorderWeights_fillBufParams(const MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_reorderWeights_Args *pArgs,
                                                                                          MMALIB_bufParams2D_t *pReorderedWeights_addr);
   

   /*!
    ********************************************************************************
    *  @brief       This function takes a set of weights and biases (optional) and reorders the coefficients for
    *               improved efficiency when computing column flow convolution.
    *
    *  @details     The function can receive the bias values either embedded in the
    *               kernel weights a priori, or as a separate vector.  If the weights
    *               are already embedded in pWeights, then the function must be called
    *               with pBias == NULL.
    *
    *  @param [in]  operation     : Desired reordering configuration, arguments are of type @ref MMALIB_reorder_weights_operation_e
    *                                 REORDER_WEIGHTS_AND_BIAS = reorder weights and bias values simultaneously
    *                                 REORDER_WEIGHTS = reorder weights, leaving space for bias values to be added later
    *                                 REORDER_BIAS = assume weights are already present, add-in bias values
    *  @param [in]  pArgs         : Pointer to argument structure containing necessary parameters for reordering weights
    *  @param [in]  pWeights_addr : Pointer to structure containing dimensional
    *                                information of the naturally ordered weights buffer.
    *  @param [in]  pWeights[]    : Pointer to buffer holding naturally ordered convolution weights
    *  @param [in]  pBias_addr    : Pointer to structure containing dimensional
    *                                 information of the bias values buffer.
    *  @param [in]  pBias[]       : Pointer to buffer holding bias values
    *  @param [in]  pReorderedWeights_addr   :  Pointer to structure containing dimensional
    *                                            information of the reordered weights buffer.
    *  @param [out] pReorderedWeights[] : Pointer to buffer holding the reordered weights
    *
    *  @return      Status of success or error with error codes, refer to @ref MMALIB_STATUS.
    *
    *  @par Assumptions:
    *    - pBias == NULL if the bias values are already stored with weights in pWeights in REORDER_WEIGHTS_AND_BIAS mode
    *
    *  @par Performance Considerations:
    *    - This function may either be called during the processing flow, or offline whenever the weights are known.
    *
    *******************************************************************************
    */
   
   MMALIB_STATUS MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_reorderWeights_exec(uint32_t operation,
                                                                                 const MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_reorderWeights_Args *pArgs,
                                                                                 const MMALIB_bufParams2D_t *pWeights_addr,
                                                                                 const void *restrict pWeights, // natural order for kernel weights
                                                                                 const MMALIB_bufParams2D_t *pBias_addr,
                                                                                 const void *restrict pBias,    // bias values
                                                                                 const MMALIB_bufParams2D_t *pReorderedWeights_addr,
                                                                                 void *restrict pReorderedWeights); // weights reordered for this kernel
   
   
/**@}**/
   
#ifdef __cplusplus
}
#endif
#endif /* MMALIB_CNN_CONVOLVE_COL_SMALLNO_IXX_IXX_OXX_REORDERWEIGHTS_H_ */

/* ============================================================================ */
/*  End of file:  MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX_reorderWeights.h  */
/* ============================================================================ */

