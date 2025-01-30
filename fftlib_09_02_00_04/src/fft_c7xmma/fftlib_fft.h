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
 *  @file     fftlib_fft.h
 *
 *  @brief    File to hold buffer parameter related info for FFTLIB
 *
 *  @version  0.1 - Jan 2018 : Initial Version with as a template [PKS]
 *
 *******************************************************************************
 */
#ifndef _FFTLIB_FFT_H_
#define _FFTLIB_FFT_H_

#include "FFTLIB_FFT_dftLarge_ixX_cxX_oxX/FFTLIB_FFT_dftLarge_ixX_cxX_oxX.h"
#include "FFTLIB_FFT_dftSmall_ixX_cxX_oxX/FFTLIB_FFT_dftSmall_ixX_cxX_oxX.h"
#include "FFTLIB_FFT_fft_ixX_cxX_oxX/FFTLIB_FFT_fft_ixX_cxX_oxX.h"
#include "FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX/FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX.h"
#endif /*_FFTLIB_FFT_H_*/

/*!
 * \defgroup FFTLIB_FFT Fast Fourier Transform (FFT) kernels
 * \brief This module consists of kernels to compute Discrete Fourier
 *        Transform(DFT) of data using C7x + MMA
 * \details
 *          - The data is assumed to be of complex type.
 *          - The kernels support computation of a batch of several DFT's in one
 *          kernel call.
 *          - In a batch of several DFT's, each DFT is referred to as a channel.
 * The data size for all channels in a batch is assumed to be same.
 *          - Unless specified otherwise, the number of complex points for each
 *          channel is assumed to be a power of 2.
 *          - The input and output data is arranged in the batch such that
 *          channel 1's data follows channel 0's data, channel 2's data follows
 *          channel 1's data and so on.
 *          - For each channel, the real and imaginary parts of the data may be
 *          interleaved or non-interleaved.
 *          - In case the data is interleaved, the format is as
 * |R0|I0|R1|I1|..., while in case the data is non-interleaved, the format is as
 *          |R0|R1|...|I0|I1|... where Rn and In are the real and imaginary
 * parts respectively of the nth complex data point.
 */
