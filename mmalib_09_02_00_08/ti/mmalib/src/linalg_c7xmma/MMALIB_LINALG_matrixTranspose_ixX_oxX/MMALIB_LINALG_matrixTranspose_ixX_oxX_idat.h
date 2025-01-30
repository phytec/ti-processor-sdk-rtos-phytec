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

#ifndef MMALIB_LINALG_MATRIXTRANSPOSE_IXX_OXX_IDAT_H
#define MMALIB_LINALG_MATRIXTRANSPOSE_IXX_OXX_IDAT_H

#include <mmalib.h>

// include test infrastructure provided by MMALIB
#include "../../test/MMALIB_test.h"

typedef struct {
   uint8_t  testPattern;   /* 0: constant, 1: sequential, 2: random, 3: static array, 4: file, etc */
   void *staticIn;
   void *staticOut;
   uint32_t dataType;
   uint32_t heightIn;
   uint32_t widthIn;
   uint32_t strideIn;
   uint32_t heightOut;
   uint32_t widthOut;
   uint32_t strideOut;
   uint32_t outputDataLocation; // 0 -> HEAP (probably L2SRAM), 1 -> MSMC
   uint32_t numReps;
   uint32_t testID;
} matrixTranspose_ixX_oxX_testParams_t;

void matrixTranspose_ixX_oxX_getTestParams(matrixTranspose_ixX_oxX_testParams_t * *params, int32_t *numTests);

#endif /* define MMALIB_LINALG_MATRIXTRANSPOSE_IXX_OXX_IDAT_H */

/* ======================================================================== */
/*  End of file:  MMALIB_LINALG_matrixTranspose_ixX_oxX_idat.h                                     */
/* ======================================================================== */

