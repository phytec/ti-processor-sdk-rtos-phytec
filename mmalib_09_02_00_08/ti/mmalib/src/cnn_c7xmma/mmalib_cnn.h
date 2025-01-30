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

/**
 ******************************************************************************
 *  @file     MMALIB_bufParams.h
 *
 *  @brief    File to hold buffer parameter related info for MMALIB
 *
 *  @version  0.1 - Jan 2018 : Initial Version with as a template [PKS]
 *
 *******************************************************************************
 */

#ifndef _MMALIB_CNN_H_
#define _MMALIB_CNN_H_

#include "MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX/MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX.h"

#include "MMALIB_CNN_convolve_col_smallNo_highPrecision/MMALIB_CNN_convolve_col_smallNo_highPrecision.h"
#include "MMALIB_CNN_convolve_col_smallNo_highPrecision_pointwisePost/MMALIB_CNN_convolve_col_smallNo_highPrecision_pointwisePost.h"

#include "MMALIB_CNN_convolve_col_smallNo_legacy_ixX_ixX_oxX/MMALIB_CNN_convolve_col_smallNo_legacy_ixX_ixX_oxX.h"

#include "MMALIB_CNN_convolve_row_ixX_ixX_oxX/MMALIB_CNN_convolve_row_ixX_ixX_oxX.h"

#include "MMALIB_CNN_convolveBias_row_ixX_ixX_oxX/MMALIB_CNN_convolveBias_row_ixX_ixX_oxX.h"

#include "MMALIB_CNN_fullyConnected_ixX_ixX_oxX/MMALIB_CNN_fullyConnected_ixX_ixX_oxX.h"

#include "MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX/MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX.h"

#include "MMALIB_CNN_tensor_convert_ixX_oxX/MMALIB_CNN_tensor_convert_ixX_oxX.h"

#include "MMALIB_CNN_deconvolve_row_ixX_ixX_oxX/MMALIB_CNN_deconvolve_row_ixX_ixX_oxX.h"
#include "MMALIB_CNN_pixelShuffle_row_ixX_ixX_oxX/MMALIB_CNN_pixelShuffle_row_ixX_ixX_oxX.h"
#include "MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX/MMALIB_CNN_deconvolveBias_row_ixX_ixX_oxX.h"

#endif /*_MMALIB_CNN_H_*/

/*!
 * \defgroup MMALIB_CNN Convolutional Neural Networks (CNN) kernels
 * \brief This module consists of kernels to implement the core computations
 * occurring in the context of convolutional neural networks. \details
 *          - CNN Module is partitioned into
 *            - Row Convolution, CNN convolution operation is performed with
 * continuous rows of input feature map processing. This is targeted for
 * dense convolution
 *            - Column Convolution, CNN convolution operation is performed
 * with partial rows but all columns processed. This is targeted for
 * depthwise convolution
 *          - Fully connected layer
 *          - CNN Deconvolution operation post processing operation on the
 * convolution output
 *          - Input feature map data is 8, 16 bit signed and unsigned
 *          - Coefficient data is 8, 16 bit signed
 *          - Output feature map data is 8, 16 bit signed and unsigned
 *
 */
