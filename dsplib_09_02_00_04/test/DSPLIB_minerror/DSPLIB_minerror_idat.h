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

#ifndef DSPLIB_MINERROR_IXX_IXX_OXX_IDAT_H

#define DSPLIB_MINERROR_IXX_IXX_OXX_IDAT_H

#include <dsplib.h>

// include test infrastructure provided by MMALIB
#include "../common/DSPLIB_test.h"

typedef struct {
   uint8_t  testPattern; /* 0: consaddt, 1: sequential, 2: random, 3: static array, 4: file, etc */
   void    *staticIn;
   void    *staticFilter;
   void    *staticOut;
   uint32_t dataType;
   uint32_t vecInSize;
   uint32_t errCoefsSize;
   int32_t  strideIn;
   uint32_t shift;              // 0-> for float, double, required for integer types
   uint32_t outputDataLocation; // 0 -> HEAP (probably L2SRAM), 1 -> MSMC
   uint32_t numReps;
   uint32_t testID;
} minerror_testParams_t;

void minerror_getTestParams(minerror_testParams_t **params, int32_t *numTests);

#endif /* define DSPLIB_minerror_IXX_IXX_OXX_IDAT_H */

/* ======================================================================== */
/*  End of file:  DSPLIB_minerror_idat.h                                    */
/* ======================================================================== */
