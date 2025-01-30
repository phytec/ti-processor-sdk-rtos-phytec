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
*  @file     MMALIB_CNN_convolve_col_smallNo_highPrecision_reorderWeights.h
*
*  @brief    Public header file for MMALIB_CNN_convolveBias_row_ixX_ixX_oxX_compressWeights
*            function
*
*  @version  0.1 - Dec 2018 : Initial Version
*            
********************************************************************************/


#ifndef MMALIB_CNN_CONVOLVEBIAS_ROW_PROCESSWEIGHTS_H_
#define  MMALIB_CNN_CONVOLVEBIAS_ROW_PROCESSWEIGHTS_H_

#include "../../common/MMALIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/*!
 *******************************************************************************
 @struct MMALIB_CNN_convolveBias_row_processWeights_Args
 @brief  This structure holds all the input parameters for reordering CNN filter
 weights
 ********************************************************************************
 */

typedef struct
{
   int8_t  funcStyle;          //!< Variant of the function refer to @ref MMALIB_FUNCTION_STYLE
   int32_t data_type;          //!< Data type must be a member of @ref MMALIB_data_type_e
   int32_t Fr;                 //!< Kernel height
   int32_t Fc;                 //!< Kernel width
   int32_t pitchA;             //!< src0_addr pitchA
   int32_t numInChPerGroup;    //!< Number of input channels in one group
   int32_t subMChannels;       //!< Number of output channels per kernel call
   int32_t No;                 //!< Number of output channels in total
   int32_t numGroupsPerKernel; //!< Number of groups to be processed in kernel call
   int32_t maxSize;            //!< Maximum size of reordered subMChannels across all kernel calls. Populated by getMemorySize
} MMALIB_CNN_convolveBias_row_processWeights_Args ;

/*!
 ********************************************************************************
 *  @brief       This function returns the amount of memory that needs to be
 *               allocated for reordered kernel coefficients needed to support
 *               @ref MMALIB_CNN_convolveBias_row_ixX_ixX_oxX
 *
 *  @param [in]  pArgs  :  Pointer to the structure containing the required dimensional information
 *
 *  @param [in]  pWeights : Pointer to weights array in natural order 
 *  @return      Number of bytes required to store the reordered kernel coefficients
 *
 *  @remarks     Application is expected allocate this amount of memory for kernel coefficients
 *******************************************************************************/
int32_t MMALIB_CNN_convolveBias_row_processWeights_getMemorySize(
   MMALIB_CNN_convolveBias_row_processWeights_Args *pArgs,
   const void *restrict pWeights);

/*!
 ********************************************************************************
 *  @brief       This function takes a set of weights and reorders them for use in
 *               computing convolve row flow convolution.
 *
 *  @details     The function can receive the kernel weights a priori.
 *
 *  @param [in]  pArgs         : Pointer to argument structure containing necessary parameters for reordering weights
 *  @param [in]  pWeights[]    : Pointer to buffer holding naturally ordered convolution weights
 *  @param [out] pReorderedWeights[]
 *                             : Pointer to buffer holding the reordered weights output
 *
 *  @return      Status of success or error with error codes, refer to @ref MMALIB_STATUS.
 *
 *
 *  @par Performance Considerations:
 *    - This function may either be called during the processing flow, or offline whenever the weights are known.
 *
 *******************************************************************************
 */
MMALIB_STATUS MMALIB_CNN_convolveBias_row_processWeights_reorder(const MMALIB_CNN_convolveBias_row_processWeights_Args *pArgs,
                                                          const void *restrict pWeights, // natural order for kernel weights
                                                          void *restrict pReorderedWeights); // weights reordered for this kernel

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // #ifndef MMALIB_CNN_CONVOLVEBIAS_ROW_PROCESSWEIGHTS_H_
