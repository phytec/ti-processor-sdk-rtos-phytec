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

#define height (3)
#define width (3)

/******************************************************************************/
/*                                                                            */
/* main                                                                       */
/*                                                                            */
/******************************************************************************/

int main(void)
{

   // clang-format off
    
    int16_t in0_i16[] = {-32767, -7500, 500,
                         -5500, 150, 600,
                          400, 100, -10};
    
    uint8_t out_u8[] = {0, 0, 0,
		                0, 0, 0,
		                0, 0, 0};
    uint8_t shift = 3;

   // clang-format on

   // handles and struct for call to kernel
   VXLIB_STATUS                status;
   VXLIB_convertDepth_InitArgs kerInitArgs;
   int32_t                     handleSize = VXLIB_convertDepth_getHandleSize(&kerInitArgs);
   VXLIB_kernelHandle          handle     = malloc(handleSize);

   VXLIB_bufParams2D_t bufParamsIn0, bufParamsOut;

   bufParamsIn0.data_type = VXLIB_INT16;
   bufParamsIn0.dim_x     = width;
   bufParamsIn0.dim_y     = height;
   bufParamsIn0.stride_y  = width * sizeof(int16_t);

   bufParamsOut.data_type = VXLIB_UINT8;
   bufParamsOut.dim_x     = width;
   bufParamsOut.dim_y     = height;
   bufParamsOut.stride_y  = width * sizeof(uint8_t);

   kerInitArgs.funcStyle      = VXLIB_FUNCTION_OPTIMIZED;
   kerInitArgs.overFlowPolicy = VXLIB_WRAP;
   kerInitArgs.shift          = shift;

   status = VXLIB_SUCCESS;

   // init checkparams
   if (status == VXLIB_SUCCESS)
      status = VXLIB_convertDepth_init_checkParams(handle, &bufParamsIn0, &bufParamsOut, &kerInitArgs);

   // init
   if (status == VXLIB_SUCCESS)
      status = VXLIB_convertDepth_init(handle, &bufParamsIn0, &bufParamsOut, &kerInitArgs);

   // exec checkparams
   if (status == VXLIB_SUCCESS)
      VXLIB_convertDepth_exec_checkParams(handle, in0_i16, out_u8);

   // exec
   if (status == VXLIB_SUCCESS)
      status = VXLIB_convertDepth_exec(handle, in0_i16, out_u8);

   printf("16 BIT TO 8 BIT IMAGE - WRAP POLICY");
   // print results
   for (size_t i = 0; i < height; i++) {
      printf("\n\n");
      for (size_t j = 0; j < width; j++) {
         printf("%d, ", out_u8[i * width + j]);
      }
   }
   printf("\n\n");

   kerInitArgs.overFlowPolicy = VXLIB_SATURATE;

   status = VXLIB_SUCCESS;

   // init checkparams
   if (status == VXLIB_SUCCESS)
      status = VXLIB_convertDepth_init_checkParams(handle, &bufParamsIn0, &bufParamsOut, &kerInitArgs);

   // init
   if (status == VXLIB_SUCCESS)
      status = VXLIB_convertDepth_init(handle, &bufParamsIn0, &bufParamsOut, &kerInitArgs);

   // exec checkparams
   if (status == VXLIB_SUCCESS)
      VXLIB_convertDepth_exec_checkParams(handle, in0_i16, out_u8);

   // exec
   if (status == VXLIB_SUCCESS)
      status = VXLIB_convertDepth_exec(handle, in0_i16, out_u8);

   printf("16 BIT TO 8 BIT IMAGE - SATURATE POLICY");
   // print results
   for (size_t i = 0; i < height; i++) {
      printf("\n\n");
      for (size_t j = 0; j < width; j++) {
         printf("%d, ", out_u8[i * width + j]);
      }
   }
   printf("\n\n");

   // clang-format off
    
    uint8_t in0_u8[] = {67, 75, 50,
                        55, 150, 60,
                        40, 100, 10};
    
    int16_t out_i16[] = {0, 0, 0,
		                0, 0, 0,
		                0, 0, 0};

   // clang-format on

   bufParamsIn0.data_type = VXLIB_UINT8;
   bufParamsIn0.dim_x     = width;
   bufParamsIn0.dim_y     = height;
   bufParamsIn0.stride_y  = width * sizeof(uint8_t);

   bufParamsOut.data_type = VXLIB_INT16;
   bufParamsOut.dim_x     = width;
   bufParamsOut.dim_y     = height;
   bufParamsOut.stride_y  = width * sizeof(int16_t);

   kerInitArgs.funcStyle      = VXLIB_FUNCTION_OPTIMIZED;
   kerInitArgs.overFlowPolicy = VXLIB_WRAP;
   kerInitArgs.shift          = shift;

   status = VXLIB_SUCCESS;

   // init checkparams
   if (status == VXLIB_SUCCESS)
      status = VXLIB_convertDepth_init_checkParams(handle, &bufParamsIn0, &bufParamsOut, &kerInitArgs);

   // init
   if (status == VXLIB_SUCCESS)
      status = VXLIB_convertDepth_init(handle, &bufParamsIn0, &bufParamsOut, &kerInitArgs);

   // exec checkparams
   if (status == VXLIB_SUCCESS)
      VXLIB_convertDepth_exec_checkParams(handle, in0_u8, out_i16);

   // exec
   if (status == VXLIB_SUCCESS)
      status = VXLIB_convertDepth_exec(handle, in0_u8, out_i16);

   printf("8 BIT TO 16 BIT IMAGE");
   // print results
   for (size_t i = 0; i < height; i++) {
      printf("\n\n");
      for (size_t j = 0; j < width; j++) {
         printf("%d, ", out_i16[i * width + j]);
      }
   }
   printf("\n\n");

   free(handle);

   return 0;
}