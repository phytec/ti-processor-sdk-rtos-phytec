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

#ifndef FFTLIB_FFT1DBATCHED_I16SC_C16SC_O16SC_CN_H_
#define FFTLIB_FFT1DBATCHED_I16SC_C16SC_O16SC_CN_H_ 1

#include "../../common/FFTLIB_types.h"

FFTLIB_STATUS
FFTLIB_fft1dBatched_i16sc_c16sc_o16sc_cn (int16_t *             pX,
                                          FFTLIB_bufParams1D_t *bufParamsX,
                                          int16_t *             pW,
                                          FFTLIB_bufParams1D_t *bufParamsW,
                                          int16_t *             pY,
                                          FFTLIB_bufParams1D_t *bufParamsY,
                                          uint32_t *            pShift,
                                          FFTLIB_bufParams1D_t *bufParamsShift,
                                          uint32_t              numPoints,
                                          uint32_t              numChannels);

FFTLIB_STATUS FFTLIB_fft1dBatched_i16sc_c16sc_o16sc_checkParams_cn (
    int16_t *             pX,
    FFTLIB_bufParams1D_t *bufParamsX,
    int16_t *             pW,
    FFTLIB_bufParams1D_t *bufParamsW,
    int16_t *             pY,
    FFTLIB_bufParams1D_t *bufParamsY,
    uint32_t *            pShift,
    FFTLIB_bufParams1D_t *bufParamsShift,
    uint32_t              numPoints,
    uint32_t              numChannels);

#endif /* FFTLIB_FFT1DBATCHED_I16SC_C16SC_O16SC_CN_H_ */
