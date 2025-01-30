/******************************************************************************
 * Copyright (C) 2023 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Includes                                                                   */
/*                                                                            */
/******************************************************************************/

#include "VXLIB_bufParams.h"
#include "VXLIB_types.h"
#include "vxlib.h"
#include <cstdio>
#include <stdint.h>

// #define height 8
// #define width 2
// #define offset 2
// #define numBins 4
// #define range 10
// #define lastBlock 1

/******************************************************************************/
/*                                                                            */
/* main                                                                       */
/*                                                                            */
/******************************************************************************/

int main(void)
{
   uint8_t height    = 2;
   uint8_t width     = 8;
   uint8_t offset    = 2;
   uint8_t numBins   = 4;
   uint8_t range     = 10;
   uint8_t lastBlock = 1;

   // clang-format off
    
  uint8_t in_u8[] = {1,  2,  3,  4,  5, 6,  7,  8,  
  9,  10, 11, 12, 13, 14, 15, 16};


   uint32_t out_u32[] = {0, 0, 0, 0};

   // clang-format on

   // handles and struct for call to kernel
   VXLIB_STATUS             status;
   VXLIB_histogram_InitArgs kerInitArgs;
   int32_t                  handleSize = VXLIB_histogram_getHandleSize(&kerInitArgs);
   VXLIB_kernelHandle       handle     = malloc(handleSize);

   VXLIB_bufParams2D_t bufParamsIn;
   VXLIB_bufParams1D_t bufParamsOut;

   bufParamsIn.data_type = VXLIB_UINT8;
   bufParamsIn.dim_x     = width;
   bufParamsIn.dim_y     = height;
   bufParamsIn.stride_y  = width * sizeof(uint8_t);

   bufParamsOut.data_type = VXLIB_UINT32;
   bufParamsOut.dim_x     = numBins;

   kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;
   kerInitArgs.offset    = offset;
   kerInitArgs.numBins   = numBins;
   kerInitArgs.range     = range;
   kerInitArgs.lastBlock = lastBlock;

   status = VXLIB_SUCCESS;

   // init checkparams
   status = VXLIB_histogram_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // init
   if (status == VXLIB_SUCCESS)
      status = VXLIB_histogram_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
   else {
      printf("\nInit checkparams failed: %d\n", status);
      return 0;
   }

   // exec checkparams
   VXLIB_histogram_exec_checkParams(handle, in_u8, out_u32);

   // exec
   if (status == VXLIB_SUCCESS)
      status = VXLIB_histogram_exec(handle, in_u8, out_u32);
   else {
      printf("\nExec checkparams failed: %d\n", status);
      return 0;
   }

   // print results
   printf("\nHistogram\n");
   printf("\nInput Image\n");

   for (size_t i = 0; i < height; i++) {
      printf("\n");
      for (size_t j = 0; j < width; j++) {
         printf("%d, ", in_u8[i * width + j]);
      }
   }

   printf("\n");

   printf("\nOutput Histogram\n");

   for (size_t i = 0; i < numBins; i++) {
      printf("%d, ", out_u32[i]);
   }
   printf("\n\n");

   free(handle);

   return 0;
}
