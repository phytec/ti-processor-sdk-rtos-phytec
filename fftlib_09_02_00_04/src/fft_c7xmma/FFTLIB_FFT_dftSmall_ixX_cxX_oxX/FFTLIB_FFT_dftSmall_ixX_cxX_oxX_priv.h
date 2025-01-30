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
  *  @file     FFTLIB_FFTLIB__dftSmall_ixX_cxX_oxX_priv.h
  *
  *  @brief    File to hold private parameter of function FFTLIB_FFT_dftSmall
  *
  *  @version  0.1 - Jan 2018 : Initial Version
  *            
 ********************************************************************************/

#ifndef FFTLIB_FFT_DFTSMALL_IXX_CXX_OXX_PRIV_H_
#define FFTLIB_FFT_DFTSMALL_IXX_CXX_OXX_PRIV_H_

#include "FFTLIB_FFT_dftSmall_ixX_cxX_oxX.h"
#include "../../common/FFTLIB_utility.h"

/*!
 * \file
 * \brief Header file for kernel's internal use. For the kernel's interface, please 
 *        see @ref FFTLIB_FFT_dftSmall_ixX_cxX_oxX
 */

/*! 
 * @brief Macro to define the size of bufPblock array of 
 *        @ref FFTLIB_FFT_dftSmall_ixX_cxX_oxX_PrivArgs structure. 
 *
 */
#define FFTLIB_FFT_DFTSMALL_IXX_CXX_OXX_PBLOCK_SIZE (3*SE_PARAM_SIZE + 1*FFTLIB_PARAM_SIZE + sizeof(int32_t))

/*!
 *  @brief This is a function pointer type that conforms to the 
 *         declaration of 
 *         @ref FFTLIB_FFT_dftSmall_ixX_cxX_oxX_exec_ci
 *         and @ref FFTLIB_FFT_dftSmall_ixX_cxX_oxX_exec_cn.
 */
typedef FFTLIB_STATUS  (*pFxnFFTLIB_FFT_dftSmall_ixX_cxX_oxX_exec)(
   FFTLIB_kernelHandle handle, 
   const void *restrict pX,
   const void *restrict pW,
   void *restrict pY);

/*!
 *  @brief This function is the initialization function for the C7x 
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of 
 *         @ref FFTLIB_FFT_dftSmall_ixX_cxX_oxX_init.
 *
 * @details This function determines the configuration for the streaming engine
 *          and MMA hardware resources based on the function call parameters, 
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, 
 *          @ref FFTLIB_FFT_dftSmall_ixX_cxX_oxX_exec_ci would be
 *          called later independently by the application. When 
 *          @ref FFTLIB_FFT_dftSmall_ixX_cxX_oxX_exec_ci runs, it 
 *          merely retrieves the configuration from the bufPBlock and uses it to 
 *          set up the hardware resources. This arrangement is so that 
 *          @ref FFTLIB_FFT_dftSmall_ixX_cxX_oxX_exec_ci does not 
 *          lose cycles to determine the hardware configuration.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsX   :  Pointer to the structure containing dimensional
 *                               information of input buffer
 *  @param [in]  bufParamsW   :  Pointer to the structure containing dimensional
 *                               information of DFT computational matrix buffer
 *  @param [out] bufParamsY   :  Pointer to the structure containing dimensional
 *                               information of ouput buffer 
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref FFTLIB_STATUS. 
 */
template <uint32_t dataType>
extern FFTLIB_STATUS FFTLIB_FFT_dftSmall_ixX_cxX_oxX_init_ci(
   FFTLIB_kernelHandle handle,
   const FFTLIB_bufParams1D_t *bufParamsX,
   const FFTLIB_bufParams1D_t *bufParamsW,
   const FFTLIB_bufParams1D_t *bufParamsY,
   const FFTLIB_FFT_dftSmall_ixX_cxX_oxX_InitArgs *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the C7x 
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of 
 *         @ref FFTLIB_FFT_dftSmall_ixX_cxX_oxX_exec.
 *
 * @details The kernel is the optimized implementation for the Discrete Fourier
 *          Transform (DFT) computation using the C7x and MMA resources. The
 *          FFT size (number of FFT coefficients) is assumed to be less than or
 *          equal to (MMA vector width)/2. 
 *          
 *          The kernel computes the DFT by doing the direct multiplication
 *          of data vectors with the DFT matrix using MMA. The DFT matrix is 
 *          computed a priori by the 
 *          @ref FFTLIB_FFT_dftSmall_ixX_cxX_oxX_twGen and is provided as
 *          input to the current function.
 *
 *          Data reading and writing are accomplished using C7x, streaming
 *          engine and stream address generator.
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pX          : Pointer to buffer holding the input data
 *  @param [in]  pW          : Pointer to buffer holding the DFT computational
 *                             matrix
 *  @param [out] pY          : Pointer to buffer holding the output data
 *
 *  @return      Status value indicating success or failure. Refer to @ref FFTLIB_STATUS. 
 *
 *  @par Performance Considerations:
 *    For best performance,
 *    - the input and output data buffers are expected to be in L2 memory
 *    - the buffer pointers are assumed to be 64-byte aligned
 *
 */
template <typename dataType, uint32_t MMA_SIZE>
extern FFTLIB_STATUS FFTLIB_FFT_dftSmall_ixX_cxX_oxX_exec_ci(
   FFTLIB_kernelHandle handle,
   const void *restrict pX,
   const void *restrict pW,
   void *restrict pY);

/*!
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of 
 *         @ref FFTLIB_FFT_dftSmall_ixX_cxX_oxX_exec.
 *
 * @details 
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pX          : Pointer to buffer holding the input data
 *  @param [in]  pW          : Pointer to buffer holding the DFT computational
 *                             matrix
 *  @param [out] pY          : Pointer to buffer holding the output data
 *
 *  @return      Status value indicating success or failure. Refer to @ref FFTLIB_STATUS. 
 *
 */
template <typename dataType, typename accumulateType, uint32_t MMA_SIZE>
extern FFTLIB_STATUS FFTLIB_FFT_dftSmall_ixX_cxX_oxX_exec_cn(
   FFTLIB_kernelHandle handle,
   const void *restrict pX,
   const void *restrict pW,
   void *restrict pY);

/*!
 *  @brief   This is a utility function that generates the DFT computational 
 *           matrix into the provided buffer.
 *
 *  @details None
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
extern FFTLIB_STATUS FFTLIB_FFT_dftSmall_ixX_cxX_oxX_twGen(
   const FFTLIB_FFT_dftSmall_ixX_cxX_oxX_InitArgs * pKerInitArgs,
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
extern FFTLIB_STATUS FFTLIB_FFT_dftSmall_ixX_cxX_oxX_getSizes(
   const FFTLIB_FFT_dftSmall_ixX_cxX_oxX_InitArgs * pKerInitArgs,
   FFTLIB_bufParams1D_t * bufParamsX, 
   FFTLIB_bufParams1D_t * bufParamsW, 
   FFTLIB_bufParams1D_t * bufParamsY);

/*!
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct
{
   /*! @brief Structure holding initialization parameters                     */
   FFTLIB_FFT_dftSmall_ixX_cxX_oxX_InitArgs initArgs;
   /*! @brief Function pointer to point to the right execution variant between
    *         @ref FFTLIB_FFT_dftSmall_ixX_cxX_oxX_exec_cn and 
    *         @ref FFTLIB_FFT_dftSmall_ixX_cxX_oxX_exec_ci.    */
   pFxnFFTLIB_FFT_dftSmall_ixX_cxX_oxX_exec execute;
   /*! @brief Array to store the configuration prepared by 
    *         @ref FFTLIB_FFT_dftSmall_ixX_cxX_oxX_init_ci that 
    *         will be retrieved and used by 
    *         @ref FFTLIB_FFT_dftSmall_ixX_cxX_oxX_exec_ci     */
   uint8_t bufPblock[FFTLIB_FFT_DFTSMALL_IXX_CXX_OXX_PBLOCK_SIZE];
} FFTLIB_FFT_dftSmall_ixX_cxX_oxX_PrivArgs;

/*!
 *  @brief This function is the initialization function for the natural C
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of 
 *         @ref FFTLIB_FFT_dftSmall_ixX_cxX_oxX_init.
 *
 *  @details None
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsX   :  Pointer to the structure containing dimensional
 *                               information of input buffer
 *  @param [in]  bufParamsW   :  Pointer to the structure containing dimensional
 *                               information of DFT computational matrix buffer
 *  @param [out] bufParamsY   :  Pointer to the structure containing dimensional
 *                               information of ouput buffer 
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref FFTLIB_STATUS. 
 */
FFTLIB_STATUS FFTLIB_FFT_dftSmall_ixX_cxX_oxX_init_cn(
   FFTLIB_kernelHandle handle,
   FFTLIB_bufParams1D_t *bufParamsX,
   FFTLIB_bufParams1D_t *bufParamsW,
   FFTLIB_bufParams1D_t *bufParamsY,
   FFTLIB_FFT_dftSmall_ixX_cxX_oxX_InitArgs *pKerInitArgs);

#endif /* FFTLIB_FFT_DFTSMALL_IXX_CXX_OXX_PRIV_H_ */

/* ======================================================================== */
/*  End of file:  FFTLIB_FFT_dftSmall_ixX_cxX_oxX.h          */
/* ======================================================================== */

