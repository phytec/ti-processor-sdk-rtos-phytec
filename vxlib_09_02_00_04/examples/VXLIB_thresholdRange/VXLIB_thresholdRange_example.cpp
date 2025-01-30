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
    
    uint8_t in_u8[] = {222, 2, 33,
			                  44, 80, 6,
			                  100, 8, 200};

    int8_t in_i8[] = {-11, 121, -113,
			                  41, -51, 101,
			                  72, 82, -92};

    uint16_t in_u16[] = {5000, 3200, 345,
			                  3411, 32627, 1234,
			                  1000, 800, 11120};

    int16_t in_i16[] = {1111, -112, 2313,
			                  41, 51, -661,
			                  2732, 1282, -32622};

    uint8_t out_u8[] = {0, 0, 0,
			                   0, 0, 0,
			                   0, 0, 0};

    int8_t out_i8[] = {0, 0, 0,
			                   0, 0, 0,
			                   0, 0, 0};


    uint16_t out_u16[] = {0, 0, 0,
			                   0, 0, 0,
			                   0, 0, 0};

    int16_t out_i16[] = {0, 0, 0,
			                   0, 0, 0,
			                   0, 0, 0};

   // clang-format on

   // handles and struct for call to kernel

   VXLIB_STATUS                  status;
   VXLIB_thresholdRange_InitArgs kerInitArgs;
   int32_t                       handleSize = VXLIB_thresholdRange_getHandleSize(&kerInitArgs);
   VXLIB_kernelHandle            handle     = malloc(handleSize);

   kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;

   VXLIB_bufParams2D_t bufParamsIn, bufParamsOut;

   /* *********************************************************************************
                                   VXLIB_thresholdRange_I8U_O8U
    *********************************************************************************** */
   bufParamsIn.data_type = VXLIB_UINT8;
   bufParamsIn.dim_x     = width;
   bufParamsIn.dim_y     = height;
   bufParamsIn.stride_y  = width * sizeof(uint8_t);

   bufParamsOut.data_type = VXLIB_UINT8;
   bufParamsOut.dim_x     = width;
   bufParamsOut.dim_y     = height;
   bufParamsOut.stride_y  = width * sizeof(uint8_t);

   uint8_t upperVal1 = 200;
   uint8_t lowerVal1 = 40;
   uint8_t trueVal1  = 255;
   uint8_t falseVal1 = 0;

   status = VXLIB_SUCCESS;

   // init checkparams
   status = VXLIB_thresholdRange_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // init
   if (status == VXLIB_SUCCESS)
      status = VXLIB_thresholdRange_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
   else {
      printf("\nInit checkparams failed: %d\n", status);
      return 0;
   }

   // exec checkparams
   VXLIB_thresholdRange_exec_checkParams(handle, in_u8, out_u8, &upperVal1, &lowerVal1, &trueVal1, &falseVal1);

   // exec
   if (status == VXLIB_SUCCESS)
      status = VXLIB_thresholdRange_exec(handle, in_u8, out_u8, &upperVal1, &lowerVal1, &trueVal1, &falseVal1);
   else {
      printf("\nExec checkparams failed: %d\n", status);
      return 0;
   }

   // print results
   printf("thresholdRange in_u8 : out_u8\n");
   printf("\n upperVal: %u\n", upperVal1);
   printf("\n lowerVal: %u\n", lowerVal1);
   printf("\n trueVal: %u\n", trueVal1);
   printf("\n falseVal: %u\n", falseVal1);

   printf("in_u8:\n");
   for (size_t i = 0; i < height; i++) {
      for (size_t j = 0; j < width; j++) {
         printf("%5u, ", in_u8[i * width + j]);
      }
      printf("\n");
   }
   printf("\n");

   printf("out_u8:\n");
   for (size_t i = 0; i < height; i++) {
      for (size_t j = 0; j < width; j++) {
         printf("%5u, ", out_u8[i * width + j]);
      }
      printf("\n");
   }
   printf("\n");

   /* *********************************************************************************
                                   VXLIB_thresholdRange_I8S_O8S
    *********************************************************************************** */
   bufParamsIn.data_type = VXLIB_INT8;
   bufParamsIn.dim_x     = width;
   bufParamsIn.dim_y     = height;
   bufParamsIn.stride_y  = width * sizeof(int8_t);

   bufParamsOut.data_type = VXLIB_INT8;
   bufParamsOut.dim_x     = width;
   bufParamsOut.dim_y     = height;
   bufParamsOut.stride_y  = width * sizeof(int8_t);

   int8_t upperVal2 = 100;
   int8_t lowerVal2 = 10;
   int8_t trueVal2  = 127;
   int8_t falseVal2 = 0;

   status = VXLIB_SUCCESS;

   // init checkparams
   status = VXLIB_thresholdRange_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // init
   if (status == VXLIB_SUCCESS)
      status = VXLIB_thresholdRange_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
   else {
      printf("\nInit checkparams failed: %d\n", status);
      return 0;
   }

   // exec checkparams
   VXLIB_thresholdRange_exec_checkParams(handle, in_i8, out_i8, &upperVal2, &lowerVal2, &trueVal2, &falseVal2);

   // exec
   if (status == VXLIB_SUCCESS)
      status = VXLIB_thresholdRange_exec(handle, in_i8, out_i8, &upperVal2, &lowerVal2, &trueVal2, &falseVal2);
   else {
      printf("\nExec checkparams failed: %d\n", status);
      return 0;
   }

   // print results
   printf("thresholdRange in_i8 : out_i8\n");
   printf("\n upperVal: %d\n", upperVal2);
   printf("\n lowerVal: %d\n", lowerVal2);
   printf("\n trueVal: %d\n", trueVal2);
   printf("\n falseVal: %d\n", falseVal2);

   printf("in_i8:\n");
   for (size_t i = 0; i < height; i++) {
      for (size_t j = 0; j < width; j++) {
         printf("%5d, ", in_i8[i * width + j]);
      }
      printf("\n");
   }
   printf("\n");

   printf("out_i8:\n");
   for (size_t i = 0; i < height; i++) {
      for (size_t j = 0; j < width; j++) {
         printf("%5d, ", out_i8[i * width + j]);
      }
      printf("\n");
   }
   printf("\n");

   /* *********************************************************************************
                                   VXLIB_thresholdRange_I16U_O16U
    *********************************************************************************** */
   bufParamsIn.data_type = VXLIB_UINT16;
   bufParamsIn.dim_x     = width;
   bufParamsIn.dim_y     = height;
   bufParamsIn.stride_y  = width * sizeof(uint16_t);

   bufParamsOut.data_type = VXLIB_UINT16;
   bufParamsOut.dim_x     = width;
   bufParamsOut.dim_y     = height;
   bufParamsOut.stride_y  = width * sizeof(uint16_t);

   uint16_t upperVal3 = 35000;
   uint16_t lowerVal3 = 1000;
   uint16_t trueVal3  = 3600;
   uint16_t falseVal3 = 0;

   status = VXLIB_SUCCESS;

   // init checkparams
   status = VXLIB_thresholdRange_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // init
   if (status == VXLIB_SUCCESS)
      status = VXLIB_thresholdRange_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
   else {
      printf("\nInit checkparams failed: %d\n", status);
      return 0;
   }

   // exec checkparams
   VXLIB_thresholdRange_exec_checkParams(handle, in_u16, out_u16, &upperVal3, &lowerVal3, &trueVal3, &falseVal3);

   // exec
   if (status == VXLIB_SUCCESS)
      status = VXLIB_thresholdRange_exec(handle, in_u16, out_u16, &upperVal3, &lowerVal3, &trueVal3, &falseVal3);
   else {
      printf("\nExec checkparams failed: %d\n", status);
      return 0;
   }

   // print results
   printf("thresholdRange in_u16 : out_u16\n");
   printf("\n upperVal: %u\n", upperVal3);
   printf("\n lowerVal: %u\n", lowerVal3);
   printf("\n trueVal: %u\n", trueVal3);
   printf("\n falseVal: %u\n", falseVal3);

   printf("in_u16:\n");
   for (size_t i = 0; i < height; i++) {
      for (size_t j = 0; j < width; j++) {
         printf("%5u, ", in_u16[i * width + j]);
      }
      printf("\n");
   }
   printf("\n");

   printf("out_u16:\n");
   for (size_t i = 0; i < height; i++) {
      for (size_t j = 0; j < width; j++) {
         printf("%5u, ", out_u16[i * width + j]);
      }
      printf("\n");
   }
   printf("\n");

   /* *********************************************************************************
                                   VXLIB_thresholdRange_I16S_I16S_O16S
    *********************************************************************************** */
   bufParamsIn.data_type = VXLIB_INT16;
   bufParamsIn.dim_x     = width;
   bufParamsIn.dim_y     = height;
   bufParamsIn.stride_y  = width * sizeof(int16_t);

   bufParamsOut.data_type = VXLIB_INT16;
   bufParamsOut.dim_x     = width;
   bufParamsOut.dim_y     = height;
   bufParamsOut.stride_y  = width * sizeof(int16_t);

   int16_t upperVal4 = 10000;
   int16_t lowerVal4 = -10000;
   int16_t trueVal4  = 3600;
   int16_t falseVal4 = 0;

   status = VXLIB_SUCCESS;

   // init checkparams
   status = VXLIB_thresholdRange_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // init
   if (status == VXLIB_SUCCESS)
      status = VXLIB_thresholdRange_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
   else {
      printf("\nInit checkparams failed: %d\n", status);
      return 0;
   }

   // exec checkparams
   VXLIB_thresholdRange_exec_checkParams(handle, in_i16, out_i16, &upperVal4, &lowerVal4, &trueVal4, &falseVal4);

   // exec
   if (status == VXLIB_SUCCESS)
      status = VXLIB_thresholdRange_exec(handle, in_i16, out_i16, &upperVal4, &lowerVal4, &trueVal4, &falseVal4);
   else {
      printf("\nExec checkparams failed: %d\n", status);
      return 0;
   }

   // print results
   printf("thresholdRange in_i16 : out_i16\n");
   printf("\n upperVal: %d\n", upperVal4);
   printf("\n lowerVal: %d\n", lowerVal4);
   printf("\n trueVal: %d\n", trueVal4);
   printf("\n falseVal: %d\n", falseVal4);

   printf("in_i16:\n");
   for (size_t i = 0; i < height; i++) {
      for (size_t j = 0; j < width; j++) {
         printf("%5d, ", in_i16[i * width + j]);
      }
      printf("\n");
   }
   printf("\n");

   printf("out_i16:\n");
   for (size_t i = 0; i < height; i++) {
      for (size_t j = 0; j < width; j++) {
         printf("%5d, ", out_i16[i * width + j]);
      }
      printf("\n");
   }
   printf("\n");

   printf("----- End of thresholdRange example -----\n");
   free(handle);

   return 0;
}
