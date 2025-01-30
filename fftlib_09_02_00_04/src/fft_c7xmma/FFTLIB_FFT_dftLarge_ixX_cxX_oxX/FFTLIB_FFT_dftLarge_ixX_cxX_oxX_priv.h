/******************************************************************************
*                                                                             *
* module name       :FFTLIB                                                   *
*                                                                             *
* module descripton :Matrix Multiply Accelerator Library module for C7x+MMA   *
*                                                                             *
* Copyright (C) 2017-2018 Texas Instruments Incorporated - http://www.ti.com/ *
* ALL RIGHTS RESERVED                                                         *
*                                                                             *
******************************************************************************/

/********************************************************************************
  *  @file     FFTLIB_FFTLIB__dftLarge_ixX_cxX_oxX_priv.h
  *
  *  @brief    File to hold private parameter of function FFTLIB_FFT_dftLarge
  *
  *  @version  0.1 - Jan 2018 : Initial Version
  *            
 ********************************************************************************/

#ifndef FFTLIB_FFT_DFTLARGE_IXX_CXX_OXX_PRIV_H_
#define FFTLIB_FFT_DFTLARGE_IXX_CXX_OXX_PRIV_H_


#include "FFTLIB_FFT_dftLarge_ixX_cxX_oxX.h"
#include "../../common/FFTLIB_utility.h"
#include "../../linalg_c7xmma/FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX/FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_priv.h"

/*!
 * \file
 * \brief Header file for kernel's internal use. For the kernel's interface, please 
 *        see @ref FFTLIB_FFT_dftLarge_ixX_cxX_oxX
 */

/*!
 *  @brief   This is a utility function that generates the DFT computational 
 *           matrix into the provided buffer.
 *
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *  @param [in]  pW           :  Pointer to the buffer where the DFT computational 
 *                               matrix is generated into. 
 *  @param [in]  bufParamsW   :  Pointer to the structure containing dimensional
 *                               information of DFT computational matrix buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref FFTLIB_STATUS. 
 *
 */
template <typename dataType>
extern FFTLIB_STATUS FFTLIB_FFT_dftLarge_ixX_cxX_oxX_twGen(
   const FFTLIB_FFT_dftLarge_ixX_cxX_oxX_InitArgs * pKerInitArgs,
   void *restrict pW,
   const FFTLIB_bufParams1D_t * bufParamsW);

/*!
 *  @brief This function is a utility function that the application calls to
 *         obtain the sizes of data buffers. 
 *
 *  @details This function calculates the sizes of the buffers based on FFT
 *          size and batch size.
 *
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *  @param [in]  bufParamsX   :  Pointer to the structure containing dimensional
 *                               information of input buffer
 *  @param [in]  bufParamsW   :  Pointer to the structure containing dimensional
 *                               information of DFT computational matrix buffer
 *  @param [in]  bufParamsY   :  Pointer to the structure containing dimensional
 *                               information of ouput buffer 
 *
 *  @return      Status value indicating success or failure. Refer to @ref FFTLIB_STATUS. 
 *
 */ 
template <uint32_t dataType>
extern FFTLIB_STATUS FFTLIB_FFT_dftLarge_ixX_cxX_oxX_getSizes(
   const FFTLIB_FFT_dftLarge_ixX_cxX_oxX_InitArgs * pKerInitArgs,
   FFTLIB_bufParams1D_t * bufParamsX, 
   FFTLIB_bufParams1D_t * bufParamsW, 
   FFTLIB_bufParams1D_t * bufParamsY);

/*!
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct
{
   /*! @brief Structure holding initialization parameters                     */
   FFTLIB_FFT_dftLarge_ixX_cxX_oxX_InitArgs initArgs;
   /*! @brief Structure to hold the initialization parameters of
    *         @ref FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX kernel that
    *         is called by the current kernel.                                */
   FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_InitArgs matrixMatrixMultiplyInitArgs;
   /*! @brief Structure to maintain the internal use parameters of 
    *         @ref FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX kernel that
    *         is called by the current kernel.                                */
   FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_PrivArgs matrixMatrixMultiplyPrivArgs;
} FFTLIB_FFT_dftLarge_ixX_cxX_oxX_PrivArgs;
 
#endif /* FFTLIB_FFT_DFTLARGE_IXX_CXX_OXX_PRIV_H_ */

/* ======================================================================== */
/*  End of file:  FFTLIB_FFT_dftLarge_ixX_cxX_oxX.h          */
/* ======================================================================== */

