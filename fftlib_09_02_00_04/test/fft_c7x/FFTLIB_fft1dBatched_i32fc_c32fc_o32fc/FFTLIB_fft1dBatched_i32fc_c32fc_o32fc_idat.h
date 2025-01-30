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

#ifndef FFTLIB_FFT1DBATCHED_I32FC_C32FC_O32FC_IDAT_H

#define FFTLIB_FFT1DBATCHED_I32FC_C32FC_O32FC_IDAT_H

#include <fftlib.h>

// include test infrastructure provided by FFTLIB
#include "FFTLIB_test.h"


typedef struct {
   uint8_t testPattern; /* 0: constant, 1: sequential, 2: random,
                           3: static array, 4: file, etc           */
   FFTLIB_F32 *staticIn0;
   FFTLIB_F32 *staticOut;
   uint32_t    numChannels;
   uint32_t    numPoints;
  uint32_t testID;
} fft1d_testParams_t;

void fft1d_getTestParams (fft1d_testParams_t **params, int32_t *numTests);

#endif /* define FFTLIB_FFT1DBATCHED_I32FC_C32FC_O32FC_IDAT_H */

/* ======================================================================== */
/*  End of file:  FFTLIB_fft1dBatched_i32fc_c32fc_o32fc_idat.h */
/* ======================================================================== */
