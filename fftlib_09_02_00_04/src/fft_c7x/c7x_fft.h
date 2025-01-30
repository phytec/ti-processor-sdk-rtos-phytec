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

/**
 ******************************************************************************
 *  @file     c7x_fft.h
 *
 *  @brief    File to hold buffer parameter related info for FFTLIB
 *
 *  @version  0.1 - May 2021 : Initial Version with as a template [PKS]
 *
 *******************************************************************************
 */

#ifndef _C7X_FFT_H_
#define _C7X_FFT_H_
#include "FFTLIB_fft1dBatched_i16sc_c16sc_o16sc/FFTLIB_fft1dBatched_i16sc_c16sc_o16sc.h"
#include "FFTLIB_fft1dBatched_i32fc_c32fc_o32fc/FFTLIB_fft1dBatched_i32fc_c32fc_o32fc.h"
#include "FFTLIB_fft1d_i16sc_c16sc_o16sc/FFTLIB_fft1d_i16sc_c16sc_o16sc.h"
#include "FFTLIB_fft1d_i32fc_c32fc_o32fc/FFTLIB_fft1d_i32fc_c32fc_o32fc.h"
#endif /*_C7X_FFT_H_*/

/*!
 * \defgroup FFT_C7X C7x Only Fast Fourier Transform (FFT) kernels
 * \brief This module consists of kernels within the C7x only FFT scope
 * \details
 *   - Data is 16-bit integer  or 32-bit float type with all inputs and output
 * of the same type
 */
