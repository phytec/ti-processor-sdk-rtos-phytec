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
    
    uint8_t in0_u8[] = {45, 6, 34,
			                  4, 88, 61,
			                  50, 8, 100};

    uint8_t in1_u8[] = {1, 12, 33,
			                  21, 31, 41,
			                  34, 42, 52};

    int16_t in0_i16[] = {32767, -7500, 50,
                         -5500, 1150, 60,
                          600, 1000, -110};
    

    int16_t in1_i16[] = {-3278, 750, 50,
                         560, 700, 900,
                          1000, 20, -100};


    uint8_t out_u8[] = {0, 0, 0,
			                   0, 0, 0,
			                   0, 0, 0};

    int16_t out_i16[] = {0, 0, 0,
			                   0, 0, 0,
			                   0, 0, 0};

   // clang-format on

   // handles and struct for call to kernel

   VXLIB_STATUS            status;
   VXLIB_multiply_InitArgs kerInitArgs;
   int32_t                 handleSize = VXLIB_multiply_getHandleSize(&kerInitArgs);
   VXLIB_kernelHandle      handle     = malloc(handleSize);

   kerInitArgs.funcStyle      = VXLIB_FUNCTION_OPTIMIZED;
   kerInitArgs.overFlowPolicy = VXLIB_SATURATE;
   kerInitArgs.scale          = 0.5;
   VXLIB_bufParams2D_t bufParamsIn0, bufParamsIn1, bufParamsOut;

   /* *********************************************************************************
                                   VXLIB_multiply_I8U_I8U_O8U
    *********************************************************************************** */
   bufParamsIn0.data_type = VXLIB_UINT8;
   bufParamsIn0.dim_x     = width;
   bufParamsIn0.dim_y     = height;
   bufParamsIn0.stride_y  = width * sizeof(uint8_t);

   bufParamsIn1.data_type = VXLIB_UINT8;
   bufParamsIn1.dim_x     = width;
   bufParamsIn1.dim_y     = height;
   bufParamsIn1.stride_y  = width * sizeof(uint8_t);

   bufParamsOut.data_type = VXLIB_UINT8;
   bufParamsOut.dim_x     = width;
   bufParamsOut.dim_y     = height;
   bufParamsOut.stride_y  = width * sizeof(uint8_t);

   status = VXLIB_SUCCESS;

   // init checkparams
   status = VXLIB_multiply_init_checkParams(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut, &kerInitArgs);

   // init
   if (status == VXLIB_SUCCESS)
      status = VXLIB_multiply_init(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut, &kerInitArgs);
   else {
      printf("\nInit checkparams failed: %d\n", status);
      return 0;
   }

   // exec checkparams
   VXLIB_multiply_exec_checkParams(handle, in0_u8, in1_u8, out_u8);

   // exec
   if (status == VXLIB_SUCCESS)
      status = VXLIB_multiply_exec(handle, in0_u8, in1_u8, out_u8);
   else {
      printf("\nExec checkparams failed: %d\n", status);
      return 0;
   }

   // print results
   printf("\n\nMULTIPLY SATURATE in0_u8 * in1_u8 = out_u8\n");
   printf("\nScale value: %f\n", kerInitArgs.scale);

   printf("\nIN0:\n");
   for (size_t i = 0; i < height; i++) {
      for (size_t j = 0; j < width; j++) {
         printf("%5u   ", in0_u8[i * width + j]);
      }
      printf("\n");
   }
   printf("\n");

   printf("IN1:\n");
   for (size_t i = 0; i < height; i++) {
      for (size_t j = 0; j < width; j++) {
         printf("%5u   ", in1_u8[i * width + j]);
      }
      printf("\n");
   }
   printf("\n");

   printf("OUT:\n");
   for (size_t i = 0; i < height; i++) {
      for (size_t j = 0; j < width; j++) {
         printf("%5u   ", out_u8[i * width + j]);
      }
      printf("\n");
   }
   printf("\n");

   /* *********************************************************************************
                                   VXLIB_multiply_I8U_I8U_O16S
    *********************************************************************************** */
   bufParamsIn0.data_type = VXLIB_UINT8;
   bufParamsIn0.dim_x     = width;
   bufParamsIn0.dim_y     = height;
   bufParamsIn0.stride_y  = width * sizeof(uint8_t);

   bufParamsIn1.data_type = VXLIB_UINT8;
   bufParamsIn1.dim_x     = width;
   bufParamsIn1.dim_y     = height;
   bufParamsIn1.stride_y  = width * sizeof(uint8_t);

   bufParamsOut.data_type = VXLIB_INT16;
   bufParamsOut.dim_x     = width;
   bufParamsOut.dim_y     = height;
   bufParamsOut.stride_y  = width * sizeof(int16_t);

   kerInitArgs.overFlowPolicy = VXLIB_WRAP;

   status = VXLIB_SUCCESS;

   // init checkparams
   status = VXLIB_multiply_init_checkParams(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut, &kerInitArgs);

   // init
   if (status == VXLIB_SUCCESS)
      status = VXLIB_multiply_init(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut, &kerInitArgs);
   else {
      printf("\nInit checkparams failed: %d\n", status);
      return 0;
   }

   // exec checkparams
   VXLIB_multiply_exec_checkParams(handle, in0_u8, in1_u8, out_i16);

   // exec
   if (status == VXLIB_SUCCESS)
      status = VXLIB_multiply_exec(handle, in0_u8, in1_u8, out_i16);
   else {
      printf("\nExec checkparams failed: %d\n", status);
      return 0;
   }

   // print results
   printf("\n\nMULTIPLY WRAP in0_u8 * in1_u8 = out_i16\n");
   printf("\nScale value: %f\n", kerInitArgs.scale);
   printf("\nIN0:\n");
   for (size_t i = 0; i < height; i++) {
      for (size_t j = 0; j < width; j++) {
         printf("%5u   ", in0_u8[i * width + j]);
      }
      printf("\n");
   }
   printf("\n");

   printf("IN1:\n");
   for (size_t i = 0; i < height; i++) {
      for (size_t j = 0; j < width; j++) {
         printf("%5u   ", in1_u8[i * width + j]);
      }
      printf("\n");
   }
   printf("\n");

   printf("OUT:\n");
   for (size_t i = 0; i < height; i++) {
      for (size_t j = 0; j < width; j++) {
         printf("%5d   ", out_i16[i * width + j]);
      }
      printf("\n");
   }
   printf("\n");

   /* *********************************************************************************
                                   VXLIB_multiply_I8U_I16S_O16S
    *********************************************************************************** */

   bufParamsIn0.data_type = VXLIB_UINT8;
   bufParamsIn0.dim_x     = width;
   bufParamsIn0.dim_y     = height;
   bufParamsIn0.stride_y  = width * sizeof(uint8_t);

   bufParamsIn1.data_type = VXLIB_INT16;
   bufParamsIn1.dim_x     = width;
   bufParamsIn1.dim_y     = height;
   bufParamsIn1.stride_y  = width * sizeof(int16_t);

   bufParamsOut.data_type = VXLIB_INT16;
   bufParamsOut.dim_x     = width;
   bufParamsOut.dim_y     = height;
   bufParamsOut.stride_y  = width * sizeof(int16_t);

   kerInitArgs.overFlowPolicy = VXLIB_SATURATE;

   status = VXLIB_SUCCESS;

   // init checkparams
   status = VXLIB_multiply_init_checkParams(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut, &kerInitArgs);

   // init
   if (status == VXLIB_SUCCESS)
      status = VXLIB_multiply_init(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut, &kerInitArgs);
   else {
      printf("\nInit checkparams failed: %d\n", status);
      return 0;
   }

   // exec checkparams
   VXLIB_multiply_exec_checkParams(handle, in0_u8, in1_i16, out_i16);

   // exec
   if (status == VXLIB_SUCCESS)
      status = VXLIB_multiply_exec(handle, in0_u8, in1_i16, out_i16);
   else {
      printf("\nExec checkparams failed: %d\n", status);
      return 0;
   }

   // print results
   printf("\n\nMULTIPLY SATURATE in0_u8 * in1_u16 = out_i16\n");
   printf("\nScale value: %f\n", kerInitArgs.scale);
   printf("\nIN0:\n");
   for (size_t i = 0; i < height; i++) {
      for (size_t j = 0; j < width; j++) {
         printf("%7u ", in0_u8[i * width + j]);
      }
      printf("\n");
   }
   printf("\n");

   printf("IN1:\n");
   for (size_t i = 0; i < height; i++) {
      for (size_t j = 0; j < width; j++) {
         printf("%7d ", in1_i16[i * width + j]);
      }
      printf("\n");
   }
   printf("\n");

   printf("OUT:\n");
   for (size_t i = 0; i < height; i++) {
      for (size_t j = 0; j < width; j++) {
         printf("%7d ", out_i16[i * width + j]);
      }
      printf("\n");
   }
   printf("\n");

   /* *********************************************************************************
                                   VXLIB_multiply_I16S_I16S_O16S
    *********************************************************************************** */

   bufParamsIn0.data_type = VXLIB_INT16;
   bufParamsIn0.dim_x     = width;
   bufParamsIn0.dim_y     = height;
   bufParamsIn0.stride_y  = width * sizeof(int16_t);

   bufParamsIn1.data_type = VXLIB_INT16;
   bufParamsIn1.dim_x     = width;
   bufParamsIn1.dim_y     = height;
   bufParamsIn1.stride_y  = width * sizeof(int16_t);

   bufParamsOut.data_type = VXLIB_INT16;
   bufParamsOut.dim_x     = width;
   bufParamsOut.dim_y     = height;
   bufParamsOut.stride_y  = width * sizeof(int16_t);

   kerInitArgs.overFlowPolicy = VXLIB_WRAP;

   status = VXLIB_SUCCESS;

   // init checkparams
   status = VXLIB_multiply_init_checkParams(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut, &kerInitArgs);

   // init
   if (status == VXLIB_SUCCESS)
      status = VXLIB_multiply_init(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut, &kerInitArgs);
   else {
      printf("\nInit checkparams failed: %d\n", status);
      return 0;
   }

   // exec checkparams
   VXLIB_multiply_exec_checkParams(handle, in0_i16, in1_i16, out_i16);

   // exec
   if (status == VXLIB_SUCCESS)
      status = VXLIB_multiply_exec(handle, in0_i16, in1_i16, out_i16);
   else {
      printf("\nExec checkparams failed: %d\n", status);
      return 0;
   }

   // print results
   printf("\n\nMULTIPLY WRAP in0_u16 * in1_i16 = out_i16\n");
   printf("\nScale value: %f\n", kerInitArgs.scale);
   printf("\nIN0:\n");
   for (size_t i = 0; i < height; i++) {
      for (size_t j = 0; j < width; j++) {
         printf("%7d", in0_i16[i * width + j]);
      }
      printf("\n");
   }
   printf("\n");

   printf("IN1:\n");
   for (size_t i = 0; i < height; i++) {
      for (size_t j = 0; j < width; j++) {
         printf("%7d", in1_i16[i * width + j]);
      }
      printf("\n");
   }
   printf("\n");

   printf("OUT:\n");
   for (size_t i = 0; i < height; i++) {
      for (size_t j = 0; j < width; j++) {
         printf("%7d", out_i16[i * width + j]);
      }
      printf("\n");
   }
   printf("\n");

   printf("----- End of multiply example -----\n");
   free(handle);

   return 0;
}
