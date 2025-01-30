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

#include "DSPLIB_bexp_priv.h"

/**
 * C Natural implementation of the bexp kernels.
 * Handles unsigned and signed data types internally
 */
template <typename dataType>
DSPLIB_STATUS DSPLIB_bexp_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_bexp_PrivArgs *pKerPrivArgs = (DSPLIB_bexp_PrivArgs *) handle;
   uint32_t              blockSize    = pKerPrivArgs->blockSize;

   DSPLIB_DEBUGPRINTFN(0, "%s", "Entering function\n");

   // casting pOutLocal as int32_t since the data type
   // of the output remains same regardless of the input data type
   dataType *pInLocal  = (dataType *) pIn;
   int32_t  *pOutLocal = (int32_t *) pOut;

   DSPLIB_DEBUGPRINTFN(0, "Enter pInLocal %p  pOut %p\n", pInLocal, pOut);

   dataType x;

   using udataType    = std::make_unsigned_t<dataType>;
   udataType mask     = 0;
   int       num_bits = sizeof(dataType) * 8;
   int       shift;

   // Handling signed and unsigned data types differently
   if (std::numeric_limits<dataType>::is_signed) {
      for (int32_t counter = 0; counter < (int32_t) blockSize; counter++) {
         x = *pInLocal++;
         if (x < 0) {
            udataType x_abs;
            // Handling min value separately. For example -128 in case of int8
            if (x == (dataType) (1u << (uint32_t)(num_bits - 1))) {
               x += 1;
            }
            // Taking the absolute value and doing an OR operation
            x_abs = (udataType) (-(long long) x);
            mask |= x_abs;
         }
         // For positive values, OR operation can be done with the original values itself
         else {
            mask |= x;
         }
      }
      // Checking the least power of two that is larger than the overall mask value
      for (shift = num_bits - 1; shift >= 0; shift--) {
         if (((udataType) 1 << (udataType)shift) & mask){
            break;
         }
      }

      // For signed values, we need an extra - 1 since the MSBit is ignored
      *pOutLocal = (int32_t) (num_bits - 1 - shift - 1);
   }
   else {
      for (int32_t counter = 0; counter < (int32_t) blockSize; counter++) {
         x = *pInLocal++;
         mask |= x;
      }
      for (shift = num_bits - 1; shift >= 0; shift--) {
         if (((udataType) 1 << (udataType)shift) & mask){
            break;
         }
      }

      *pOutLocal = (int32_t) (num_bits - 1 - shift);
   }

   DSPLIB_DEBUGPRINTFN(0, "Exit with status: %d\n", status);
   return (status);
}

template DSPLIB_STATUS DSPLIB_bexp_exec_cn<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_bexp_exec_cn<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_bexp_exec_cn<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_bexp_exec_cn<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_bexp_exec_cn<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_bexp_exec_cn<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_bexp_exec_cn<int64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_bexp_exec_cn<uint64_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);