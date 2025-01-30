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

#include "DSPLIB_q15tofl_priv.h"

DSPLIB_STATUS DSPLIB_q15tofl_init_cn(DSPLIB_kernelHandle            handle,
                                     const DSPLIB_bufParams1D_t    *bufParamsIn,
                                     const DSPLIB_bufParams1D_t    *bufParamsOut,
                                     const DSPLIB_q15tofl_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;
   return status;
}

template <typename dataTypeOut>
DSPLIB_STATUS DSPLIB_q15tofl_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_q15tofl_PrivArgs *pKerPrivArgs = (DSPLIB_q15tofl_PrivArgs *) handle;
   uint32_t                 blockSize    = pKerPrivArgs->blockSize;
   int16_t                 *pInLocal     = (int16_t *) pIn;
   dataTypeOut             *pOutLocal    = (dataTypeOut *) pOut;

   DSPLIB_DEBUGPRINTFN(0, "pInLocal: %p pOutLocal: %p blockSize: %d\n", pInLocal, pOutLocal, blockSize);

   int16_t     a;
   dataTypeOut out;

   for (int32_t counter = 0; counter < (int32_t) blockSize; counter++) {
      a            = *pInLocal++;
      out          = (dataTypeOut) a / 32768.0;
      *pOutLocal++ = out;
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return (status);
}

template DSPLIB_STATUS
DSPLIB_q15tofl_exec_cn<float>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_q15tofl_exec_cn<double>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
