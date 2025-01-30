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
*  @file     MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_reorderWeights.h
*
*  @brief    Public header file for MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_reorderWeights
*            function
*
*  @version  0.1 - Jan 2023 : Initial Version
*            
********************************************************************************/


#ifndef MMALIB_CNN_FULLYCONNECTEDBIAS_REORDER_WEIGHTS_H_
#define MMALIB_CNN_FULLYCONNECTEDBIAS_REORDER_WEIGHTS_H_

#include "../../common/MMALIB_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__SE_FEAT_SPARSITY)
#define INDICATOR_BITS 64
#define PACKET_ROWS INDICATOR_BITS/__MMA_A_COLS(1)

#if PACKET_ROWS == 1
#define PACKET_ROWS_SHIFT 0
#define PACKET_ROWS_MASK  0x0ULL
#elif PACKET_ROWS == 2
#define PACKET_ROWS_SHIFT 1
#define PACKET_ROWS_MASK  0x1ULL
#elif PACKET_ROWS == 4
#define PACKET_ROWS_SHIFT 2
#define PACKET_ROWS_MASK  0x3ULL
#endif
#endif


/*!
 *******************************************************************************
 @struct MMALIB_CNN_fullyConnectedBias_processWeights_Args
 @brief  This structure holds all the input parameters for reordering CNN filter
 weights
 ********************************************************************************
 */

typedef struct
{
   uint32_t data_type;          //!< Data type must be a member of @ref MMALIB_data_type_e
   uint32_t dim_x;              /*!< \brief Width of h buffer in X dimension in elements. */
   uint32_t dim_y;              /*!< \brief Height of h buffer in Y dimension in elements. */
   int32_t  stride_y;           /*!< \brief Stride of h buffer in Y dimension in bytes. */
   int8_t   funcStyle;          //!< Variant of the function refer to @ref MMALIB_FUNCTION_STYLE
#if defined (__SE_FEAT_SPARSITY)
   int8_t   packetizeMode;      //!< Flag to indicate packetization required
#endif
} MMALIB_CNN_fullyConnectedBias_processWeights_Args ;


/**
 * @brief       This function re-orders the input weights (kernel matrix).
 *              Reordering of data is required to alleviate potential bank
 * conflicts arising when the kernel matrix is  accessed in L2 via SE1 in
 *              conjunction with data transfer from MSMC to L2 via DMA. The
 * current re-ordering scheme results in a bank-acess pattern of
 *              {0,0,1,1,2,2,3,3,0,0,1,1,2,2,3,3, ...} for SE1
 *
 * @param [in]  mmaSize         : Size of MMA for given test case
 *                                - 64 for 8-bit datatype
 *                                - 32 for 16-bit datatype
 * @param [in]  NiBias          : Ni (number of input channels) + Bias columns
 * @param [in]  No              : Number of output channels
 * @param [in]  strideOut       : Stride of matrix after re-ordering
 * @param [in]  pWeights        : Pointer for input weights
 * @param [in]  pReorderWeights : Pointer for ouput weights after re-ordering
 *
 *  @return      Status value indicating success or failure.
 *               Refer to @ref MMALIB_STATUS.
 */

MMALIB_STATUS
MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_reorderWeights (
    int32_t  mmaSize,
    int32_t  NiBias,
    int32_t  No,
    uint32_t strideOut,
    const void *restrict pWeights,
    void *restrict pReorderWeights);

/*!
 ********************************************************************************
 *  @brief       This function returns the amount of memory that needs to be
 *               allocated for reordered kernel coefficients needed to support
 *               @ref MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX
 *
 *  @param [in]  pArgs  :  Pointer to the structure containing the required dimensional information
 *
 *  @param [in]  pWeights : Pointer to weights array in natural order 
 *
 *  @return      Number of bytes required to store the reordered kernel coefficients
 *
 *  @remarks     Application is expected allocate this amount of memory for kernel coefficients
 *******************************************************************************/
int32_t MMALIB_CNN_fullyConnectedBias_processWeights_getMemorySize(
   const MMALIB_CNN_fullyConnectedBias_processWeights_Args *pArgs,
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
 *  @param [out] pReordered_Weights[]
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
MMALIB_STATUS MMALIB_CNN_fullyConnectedBias_processWeights_reorder(const MMALIB_CNN_fullyConnectedBias_processWeights_Args *pArgs,
                                                            const void *restrict pWeights, // natural order for kernel weights
                                                            void *restrict pReordered_Weights); // weights reordered for this kernel


#ifdef __cplusplus
}
#endif
#endif

