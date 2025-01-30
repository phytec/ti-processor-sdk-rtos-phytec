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
  *  @file     fftlib_linalg.h
  *
  *  @brief
  *
  *  @version  0.1 - Jan 2018 : Initial Version
  *            
 *******************************************************************************
*/
#ifndef _FFTLIB_LINALG_H_
#define _FFTLIB_LINALG_H_

#include "FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX/FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX.h"
#include "FFTLIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX/FFTLIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX.h"
#include "FFTLIB_LINALG_matrixTranspose_ixX_oxX/FFTLIB_LINALG_matrixTranspose_ixX_oxX.h"
#include "FFTLIB_LINALG_pointwiseMatrixMatrixMultiply_ixX_ixX_oxX/FFTLIB_LINALG_pointwiseMatrixMatrixMultiply_ixX_ixX_oxX.h"

#endif /*_FFTLIB_LINALG_H_*/

/*!
 * \defgroup FFTLIB_LINALG Linear Algebra (LINALG) kernels 
 * \brief This module consists of kernels within the linear algebra scope.
 * \details
 *   - Data is 8, 16 or 32 bit signed integer type with all inputs and output of the same type)
 *   - Overflow is handled via saturation
 */
