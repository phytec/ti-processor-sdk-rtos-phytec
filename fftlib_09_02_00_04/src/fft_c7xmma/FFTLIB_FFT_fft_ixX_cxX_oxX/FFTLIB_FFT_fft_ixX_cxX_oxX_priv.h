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
  *  @file     FFTLIB_FFTLIB__fft_ixX_cxX_oxX_priv.h
  *
  *  @brief    File to hold private parameter of function FFTLIB_FFT_fft
  *
  *  @version  0.1 - Jan 2018 : Initial Version
  *
 ********************************************************************************/

#ifndef FFTLIB_FFT_FFT_IXX_CXX_OXX_PRIV_H_
#define FFTLIB_FFT_FFT_IXX_CXX_OXX_PRIV_H_


#include "FFTLIB_FFT_fft_ixX_cxX_oxX.h"
#include "../../common/FFTLIB_utility.h"
#include "../FFTLIB_FFT_dftSmall_ixX_cxX_oxX/FFTLIB_FFT_dftSmall_ixX_cxX_oxX_priv.h"
#include "../FFTLIB_FFT_dftLarge_ixX_cxX_oxX/FFTLIB_FFT_dftLarge_ixX_cxX_oxX_priv.h"
#include "../FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX/FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_priv.h"

/*!
 *  @brief   This is an internal function that is used to determine which FFT kernel
 *           variant to call for optimal DFT computation.
 *
 *  @details Different kernel variants in this module implement DFT computation
 *           optimally for different FFT sizes and different batch sizes. This 
 *           function is used to determine which kernel variant is optimal for
 *           a given FFT size and a batch size.
 *
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *  @param [in]  MMA_SIZE     :  MMA vector width
 *  @param [out] dftSmall     :  Parameter that takes values 1 or 0 depending on
 *                               whether @ref FFTLIB_FFT_dftSmall_ixX_cxX_oxX is
 *                               the optimal kernel variant to use or not.
 *  @param [out] dftLarge     :  Parameter that takes values 1 or 0 depending on
 *                               whether @ref FFTLIB_FFT_dftLarge_ixX_cxX_oxX is
 *                               the optimal kernel variant to use or not.
 *  @param [out] highRadixDecompositions: Parameter that takes values 1 or 0 
 *                               depending on whether 
 *                               @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX
 *                               is the optimal kernel variant to use or not.
 *
 *  @return      void
 *
 */
extern void FFTLIB_FFT_fft_ixX_cxX_oxX_determineKernelVariant(
   const FFTLIB_FFT_fft_ixX_cxX_oxX_InitArgs *pKerInitArgs,
   int32_t MMA_SIZE,
   int32_t *dftSmall,
   int32_t *dftLarge,
   int32_t *highRadixDecompositions);

/*!
 *  @brief   This is a utility function that generates the computational 
 *           matrices into the provided buffer.
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
extern FFTLIB_STATUS FFTLIB_FFT_fft_ixX_cxX_oxX_twGen(
   const FFTLIB_FFT_fft_ixX_cxX_oxX_InitArgs * pKerInitArgs,
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
extern FFTLIB_STATUS FFTLIB_FFT_fft_ixX_cxX_oxX_getSizes(
   const FFTLIB_FFT_fft_ixX_cxX_oxX_InitArgs * pKerInitArgs,
   FFTLIB_bufParams1D_t * bufParamsX,
   FFTLIB_bufParams1D_t * bufParamsW,
   FFTLIB_bufParams1D_t * bufParamsY);

/*!
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct
{
   /*! @brief Structure holding initialization parameters                     */
   FFTLIB_FFT_fft_ixX_cxX_oxX_InitArgs initArgs;
   /*! @brief Field to maintain the MMA vector width value                    */
   int32_t mmaSize;
   /* ---------------------------------------------------------------- */
   /*  MISRAC Rule 19.2(UNION) Deviation: Only one of the members      */
   /*  of the following union is used in a given use case.             */
   /* ---------------------------------------------------------------- */
   union {
      /*! @brief Structure to maintain the internal use parameters of
       *  @ref FFTLIB_FFT_dftSmall_ixX_cxX_oxX kernel                         */
      FFTLIB_FFT_dftSmall_ixX_cxX_oxX_PrivArgs dftSmall;
      /*! @brief Structure to maintain the internal use parameters of
       *  @ref FFTLIB_FFT_dftLarge_ixX_cxX_oxX kernel                         */
      FFTLIB_FFT_dftLarge_ixX_cxX_oxX_PrivArgs dftLarge;
      /*! @brief Structure to maintain the internal use parameters of
       *  @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX kernel          */
      FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_PrivArgs highRadixDecompositions;
   } privArgs;
} FFTLIB_FFT_fft_ixX_cxX_oxX_PrivArgs;

#endif /* FFTLIB_FFT_FFT_IXX_CXX_OXX_PRIV_H_ */

/* ======================================================================== */
/*  End of file:  FFTLIB_FFT_fft_ixX_cxX_oxX.h          */
/* ======================================================================== */
