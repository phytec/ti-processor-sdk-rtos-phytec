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

#include "DSPLIB_maxIndex_priv.h"

template <typename dataType>
DSPLIB_STATUS DSPLIB_maxIndex_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_maxIndex_PrivArgs *pKerPrivArgs = (DSPLIB_maxIndex_PrivArgs *) handle;
   int32_t                   blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_maxIndex_exec_cn\n");
#endif

   dataType *pInLocal  = (dataType *) pIn;
   uint32_t *pOutLocal = (uint32_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal %p  pOut %p\n", pInLocal, pOut);
#endif

   dataType a, currMax;
   uint32_t out;

   out     = 0;
   currMax = pInLocal[0];

   for (int32_t counter = 0; counter < blockSize; counter++) {
      a = *pInLocal++;

      out     = (a > currMax) ? (uint32_t) counter : out;
      currMax = (a > currMax) ? a : currMax;

#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d y %d\n", counter, out);
#endif
   }

   *pOutLocal = out;

   // printf("\n natc: %d\n", out);
   return (status);
}

// explicit instantiation for the different data type versions
template DSPLIB_STATUS
DSPLIB_maxIndex_exec_cn<float>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_maxIndex_exec_cn<double>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_maxIndex_exec_cn<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_maxIndex_exec_cn<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_maxIndex_exec_cn<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_maxIndex_exec_cn<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_maxIndex_exec_cn<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_maxIndex_exec_cn<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
