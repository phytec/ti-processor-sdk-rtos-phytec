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

#ifndef MMALIB_LINALG_MATRIXMATRIXMULTIPLYACCUMULATE_IXX_IXX_IXX_OXX_IDAT_H
#define MMALIB_LINALG_MATRIXMATRIXMULTIPLYACCUMULATE_IXX_IXX_IXX_OXX_IDAT_H

#include <mmalib.h>

// include test infrastructure provided by MMALIB
#include "../../test/MMALIB_test.h"

typedef struct {
   uint8_t  testPattern;   /* 0: constant, 1: sequential, 2: random, 3: static array, 4: file, etc */
   void *staticIn0;
   uint32_t dataTypeIn0;
   void *staticIn1;
   uint32_t dataTypeIn1;
   void *staticIn2;
   uint32_t dataTypeIn2;
   void *staticOut;
   uint32_t dataTypeOut;
   uint16_t heightIn0;
   uint16_t widthIn0;
   uint16_t strideIn0;
   uint16_t heightIn1;
   uint16_t widthIn1;
   uint16_t strideIn1;
   uint16_t heightIn2;
   uint16_t widthIn2;
   uint16_t strideIn2;
   uint16_t heightOut;
   uint16_t widthOut;
   uint16_t strideOut;
   uint16_t shiftOut;
   uint32_t outputDataLocation; // 0 -> HEAP (probably L2SRAM), 1 -> MSMC
   uint32_t numReps;
   uint32_t testID;
} matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_testParams_t;

void matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_getTestParams(matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_testParams_t * *params, int32_t *numTests);

#endif /* define MMALIB_LINALG_MATRIXMATRIXMULTIPLY_IXX_IXX_OXX_IDAT_H */

/* ======================================================================== */
/*  End of file:  MMALIB_LINALG_matrixMatrixMultiplyAccumulate_ixX_ixX_ixX_oxX_idat.h                                     */
/* ======================================================================== */

