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

#include "DSPLIB_matTrans_priv.h"

DSPLIB_STATUS DSPLIB_matTrans_init_cn(DSPLIB_kernelHandle            handle,
                                      DSPLIB_bufParams2D_t          *bufParamsIn,
                                      DSPLIB_bufParams2D_t          *bufParamsOut,
                                      const DSPLIB_matTransInitArgs *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;
   return status;
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_matTrans_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_matTrans_PrivArgs *pKerPrivArgs = (DSPLIB_matTrans_PrivArgs *) handle;
   uint32_t                  heightIn     = pKerPrivArgs->heightIn;
   uint32_t                  widthIn      = pKerPrivArgs->widthIn;
   int32_t                   strideIn     = pKerPrivArgs->strideIn;
   int32_t                   strideOut    = pKerPrivArgs->strideOut;
   int32_t                   dataSize     = sizeof(dataType);

   /* Typecast void pointers to respective data type */
   dataType *pInLocal  = (dataType *) pIn;
   dataType *pOutLocal = (dataType *) pOut;

   DSPLIB_DEBUGPRINTFN(0, "pInLocal: %p pOutLocal: %p widthIn: %d heightIn: %d\n", pInLocal, pOutLocal, widthIn,
                       heightIn);

   uint32_t i, j;
   /* ------------------------------------------------------------------- */
   /*  Write each column of 'pInLocal' to a row of 'pOutLocal'.           */
   /* ------------------------------------------------------------------- */
   for (i = 0; i < widthIn; i++) {
      for (j = 0; j < heightIn; j++) {
         pOutLocal[(i * strideOut / dataSize) + j] = pInLocal[i + ((strideIn / dataSize) * j)];
      }
   }

   DSPLIB_DEBUGPRINTFN(0, "Exiting function with return status: %d\n", status);

   return (status);
}

// explicit instantiation for the different data type versions
template DSPLIB_STATUS
DSPLIB_matTrans_exec_cn<float>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_matTrans_exec_cn<double>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_matTrans_exec_cn<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_matTrans_exec_cn<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_matTrans_exec_cn<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_matTrans_exec_cn<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_matTrans_exec_cn<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_matTrans_exec_cn<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_matTrans_exec_cn<uint64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_matTrans_exec_cn<int64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
