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

#ifndef FFTLIB_FFT1D_I16SC_C16SC_O16SC_IDAT_H

#define FFTLIB_FFT1D_I16SC_C16SC_O16SC_IDAT_H

#include "FFTLIB_test.h"

typedef struct {
   uint8_t testPattern; /* 0: constant, 1: sequential, 2: random, 3: static
                           array, 4: file, etc */
   int16_t * staticIn0;
   uint32_t *shiftVector;
   int16_t * staticOut;
   uint32_t  numShifts;
   uint32_t  numPoints;
  uint32_t testID;
} fft1d_i16sc_c16sc_o16sc_testParams_t;

void fft1d_i16sc_c16sc_o16sc_getTestParams (
    fft1d_i16sc_c16sc_o16sc_testParams_t **params,
    int32_t *                              numTests);

#endif /* define FFTLIB_FFT1D_I16SC_C16SC_O16SC_IDAT_H */

/* ======================================================================== */
/*  End of file:  fftlib_fft1d_i16sc_c16sc_o16sc_idat_h.h                   */
/* ======================================================================== */
