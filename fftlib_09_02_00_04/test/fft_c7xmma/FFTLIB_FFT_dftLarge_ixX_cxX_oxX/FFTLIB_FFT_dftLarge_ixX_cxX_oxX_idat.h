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
**|         Copyright (c) 2016 Texas Instruments Incorporated                |**
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

#ifndef FFTLIB_FFT_DFTLARGE_IXX_CXX_OXX_IDAT_H

#define FFTLIB_FFT_DFTLARGE_IXX_CXX_OXX_IDAT_H

#include <fftlib.h>

// include test infrastructure provided by FFTLIB
#include "FFTLIB_test.h"

typedef struct {
   uint8_t  testPattern;   /* 0: constant, 1: sequential, 2: random, 3: static array, 4: file, etc */
   void *staticIn;
   void *staticOut;
   uint32_t *shiftVector;
   uint32_t dataType;
   uint32_t batchSize;
   uint32_t fftSize;
   uint32_t interleave;
   uint32_t numReps;
   uint32_t testID;
} dftLarge_ixX_cxX_oxX_testParams_t;

void dftLarge_ixX_cxX_oxX_getTestParams(
   dftLarge_ixX_cxX_oxX_testParams_t * *params, 
   int32_t *numTests);

#endif /* define FFTLIB_FFT_DFTLARGE_IXX_CXX_OXX_IDAT_H */

/* ======================================================================== */
/*  End of file:  FFTLIB_FFT_dftLarge_ixX_cxX_oxX_idat.h                                     */
/* ======================================================================== */

