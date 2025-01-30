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

#ifndef DSPLIB_SVD_IXX_IXX_OXX_IDAT_H

#define DSPLIB_SVD_IXX_IXX_OXX_IDAT_H

#include <dsplib.h>
// include test infrastructure provided by DSPLIB
#include "../common/DSPLIB_test.h"

#define OUTPUT_ALIGN_BYTES DSPLIB_ALIGN_SHIFT_64BYTES

typedef struct {
   uint8_t  testPattern; /* 0: constant, 1: sequential, 2: sequential_negative, 3: random etc.*/
   void    *staticInA;
   void    *staticOutU;
   void    *staticOutV;
   void    *staticOutDiag;
   uint32_t dataType;
   uint32_t widthIn;
   uint32_t heightIn;
   int32_t  strideIn;
   uint32_t enableReducedForm;
   uint32_t shift;              // 0-> for float, double, required for integer types
   uint32_t outputDataLocation; // 0 -> HEAP (probably L2SRAM), 1 -> MSMC
   uint32_t numReps;
   uint32_t testID;
} DSPLIB_svd_testParams_t;

void DSPLIB_svd_getTestParams(DSPLIB_svd_testParams_t **params, int32_t *numTests);

#endif /* define DSPLIB_SVD_IXX_IXX_OXX_IDAT_H */

/* ======================================================================== */
/*  End of file:  DSPLIB_svd_idat.h                                     */
/* ======================================================================== */
