/*******************************************************************************
**+--------------------------------------------------------------------------+**
**|                            ****                                          |**
**|                            ****                                          |**
**|                            ******o***                                    |**
**|                      ********_///_****                                   |**
**|                      ***** /_//_/ ****                                   |**
**|                       ** ** (__/ ****                                    |**
**|                           *********                                      |**
**|                            ****                                          |**
**|                            ***                                           |**
**|                                                                          |**
**|         Copyright (c) 2017 Texas Instruments Incorporated                |**
**|                        ALL RIGHTS RESERVED                               |**
**|                                                                          |**
**| Permission to use, copy, modify, or distribute this software,            |**
**| whether in part or in whole, for any purpose is forbidden without        |**
**| a signed licensing agreement and NDA from Texas Instruments              |**
**| Incorporated (TI).                                                       |**
**|                                                                          |**
**| TI makes no representation or warranties with respect to the             |**
**| performance of this computer program, and specifically disclaims         |**
**| any responsibility for any damages, special or consequential,            |**
**| connected with the use of this program.                                  |**
**|                                                                          |**
**+--------------------------------------------------------------------------+**
*******************************************************************************/

#ifndef FFTLIB_FFT1D_I32F_C32FC_O32FC_CN_H_
#define FFTLIB_FFT1D_I32F_C32FC_O32FC_CN_H_ 1

#include "../../common/FFTLIB_types.h"
#include "../FFTLIB_ifft1d_i32fc_c32fc_o32fc/FFTLIB_ifft1d_i32fc_c32fc_o32fc_cn.h"

FFTLIB_STATUS
FFTLIB_ifft1d_i32fc_c32fc_o32f_cn (FFTLIB_F32 *          pX,
                                   FFTLIB_bufParams1D_t *bufParamsX,
                                   FFTLIB_F32 *          pW,
                                   FFTLIB_bufParams1D_t *bufParamsW,
                                   FFTLIB_F32 *          pXFFT,
                                   FFTLIB_bufParams1D_t *bufParamsXFFT,
                                   FFTLIB_F32 *          pSplitFactor,
                                   FFTLIB_bufParams1D_t *bufParamsSplitFactor,
                                   FFTLIB_F32 *          pY,
                                   FFTLIB_bufParams1D_t *bufParamsY);

FFTLIB_STATUS
FFTLIB_ifft1d_i32fc_c32fc_o32f_checkParams_cn (
    FFTLIB_F32 *          pX,
    FFTLIB_bufParams1D_t *bufParamsX,
    FFTLIB_F32 *          pW,
    FFTLIB_bufParams1D_t *bufParamsW,
    FFTLIB_F32 *          pY,
    FFTLIB_bufParams1D_t *bufParamsY);

#endif
