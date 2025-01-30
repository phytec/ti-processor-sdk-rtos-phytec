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

// clang-format off

#define height    (10)
#define width     (10)
#define M         (4)
#define N         (4)
#define heightOut (height - (M - 1))
#define widthOut  (width - (N - 1))

// clang-format on

/******************************************************************************/
/*                                                                            */
/* main                                                                       */
/*                                                                            */
/******************************************************************************/

int main(void)
{

   // clang-format off
uint8_t in_u8[height][width] = {
                                {1,   2,  3,  4,  5,  6,  7,  8,  9, 10},
                                {11, 12, 13, 14, 15, 16, 17, 18, 19, 20},
                                {21, 22, 23, 24, 25, 26, 27, 28, 29, 30},
                                {31, 32, 33, 34, 35, 36, 37, 38, 39, 40},
                                {41, 42, 43, 44, 45, 46, 47, 48, 49, 50},
                                {51, 52, 53, 54, 55, 56, 57, 58, 59, 60},
                                {61, 62, 63, 64, 65, 66, 67, 68, 69, 70},
                                {71, 72, 73, 74, 75, 76, 77, 78, 79, 80},
                                {81, 82, 83, 84, 85, 86, 87, 88, 89, 90},
                                {91, 92, 93, 94, 95, 96, 97, 98, 99, 100}
                                };

uint8_t mask_u8[M][N] = { {  0,   0, 255, 255},
                          {  0, 255,   0, 255},
                          {  0, 255,   0, 255},
                          {255,   0,   0, 255} };

uint8_t scratch_u8[M * N * width];

   // clang-format on

   // handles and struct for call to kernel
   VXLIB_STATUS          status;
   VXLIB_median_InitArgs kerInitArgs;
   int32_t               handleSize = VXLIB_median_getHandleSize(&kerInitArgs);
   VXLIB_kernelHandle    handle     = malloc(handleSize);

   VXLIB_bufParams2D_t bufParamsIn, bufParamsMask, bufParamsScratch, bufParamsOut;

   /* ************************************************************************************************
                                     EXAMPLE FOR 3X3 KERNEL
   ************************************************************************************************ */

   bufParamsIn.data_type = VXLIB_UINT8;
   bufParamsIn.dim_x     = width;
   bufParamsIn.dim_y     = height;
   bufParamsIn.stride_y  = bufParamsIn.dim_x * sizeof(uint8_t);

   bufParamsMask.data_type = VXLIB_UINT8;
   bufParamsMask.dim_x     = 3;
   bufParamsMask.dim_y     = 3;
   bufParamsMask.stride_y  = bufParamsMask.dim_x * sizeof(uint8_t);

   bufParamsScratch.data_type = VXLIB_UINT8;
   bufParamsScratch.dim_x     = width;
   bufParamsScratch.dim_y     = bufParamsMask.dim_x * bufParamsMask.dim_y;
   bufParamsScratch.stride_y  = bufParamsScratch.dim_x * sizeof(uint8_t);

   bufParamsOut.data_type = VXLIB_UINT8;
   bufParamsOut.dim_x     = width - 2;
   bufParamsOut.dim_y     = height - 2;
   bufParamsOut.stride_y  = bufParamsOut.dim_x * sizeof(uint8_t);

   uint8_t out_u8_3X3[bufParamsOut.dim_y][bufParamsOut.dim_x] = {0};

   kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;

   // Select 3X3 kernel processing
   kerInitArgs.kernelType = 0;

   status = VXLIB_SUCCESS;

   // init checkparams
   if (status == VXLIB_SUCCESS)
      status = VXLIB_median_init_checkParams(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut, &bufParamsScratch,
                                             &kerInitArgs);

   // init
   if (status == VXLIB_SUCCESS)
      status = VXLIB_median_init(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut, &bufParamsScratch, &kerInitArgs);

   // exec checkparams
   if (status == VXLIB_SUCCESS)
      VXLIB_median_exec_checkParams(handle, in_u8, mask_u8, out_u8_3X3, scratch_u8);

   // exec
   if (status == VXLIB_SUCCESS)
      status = VXLIB_median_exec(handle, in_u8, mask_u8, out_u8_3X3, scratch_u8);

   printf("INPUT MATRIX");
   // print results
   for (size_t i = 0; i < bufParamsIn.dim_y; i++) {
      printf("\n");
      for (size_t j = 0; j < bufParamsIn.dim_x; j++) {
         printf("%5u ", uint8_t(in_u8[i][j]));
      }
   }
   printf("\n\n");

   printf("OUTPUT MATRIX with kernelType = 0 i.e. 3X3 kernel");
   // print results
   for (size_t i = 0; i < bufParamsOut.dim_y; i++) {
      printf("\n");
      for (size_t j = 0; j < bufParamsOut.dim_x; j++) {
         printf("%5u ", uint8_t(out_u8_3X3[i][j]));
      }
   }
   printf("\n\n");

   /* ************************************************************************************************
                                     EXAMPLE FOR MXN KERNEL
   ************************************************************************************************ */

   // Select MXN kernel processing
   kerInitArgs.kernelType = 1;

   bufParamsMask.data_type = VXLIB_UINT8;
   bufParamsMask.dim_x     = N;
   bufParamsMask.dim_y     = M;
   bufParamsMask.stride_y  = bufParamsMask.dim_x * sizeof(uint8_t);

   bufParamsOut.data_type = VXLIB_UINT8;
   bufParamsOut.dim_x     = width - (N - 1);
   bufParamsOut.dim_y     = height - (M - 1);
   bufParamsOut.stride_y  = bufParamsOut.dim_x * sizeof(uint8_t);

   uint8_t out_u8_MXN[bufParamsOut.dim_y][bufParamsOut.dim_x] = {0};

   status = VXLIB_SUCCESS;

   // init checkparams
   if (status == VXLIB_SUCCESS)
      status = VXLIB_median_init_checkParams(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut, &bufParamsScratch,
                                             &kerInitArgs);

   // init
   if (status == VXLIB_SUCCESS)
      status = VXLIB_median_init(handle, &bufParamsIn, &bufParamsMask, &bufParamsOut, &bufParamsScratch, &kerInitArgs);

   // exec checkparams
   if (status == VXLIB_SUCCESS)
      VXLIB_median_exec_checkParams(handle, in_u8, mask_u8, out_u8_MXN, scratch_u8);

   // exec
   if (status == VXLIB_SUCCESS)
      status = VXLIB_median_exec(handle, in_u8, mask_u8, out_u8_MXN, scratch_u8);

   printf("INPUT MATRIX");
   // print results
   for (size_t i = 0; i < bufParamsIn.dim_y; i++) {
      printf("\n");
      for (size_t j = 0; j < bufParamsIn.dim_x; j++) {
         printf("%5u ", uint8_t(in_u8[i][j]));
      }
   }
   printf("\n\n");

   printf("MASK");
   // print results
   for (size_t i = 0; i < bufParamsMask.dim_y; i++) {
      printf("\n");
      for (size_t j = 0; j < bufParamsMask.dim_x; j++) {
         printf("%5u ", uint8_t(mask_u8[i][j]));
      }
   }
   printf("\n\n");
   printf("OUTPUT MATRIX with kernelType = 1 i.e. 4X4 kernel with masked values");
   // print results
   for (size_t i = 0; i < bufParamsOut.dim_y; i++) {
      printf("\n");
      for (size_t j = 0; j < bufParamsOut.dim_x; j++) {
         printf("%5u ", uint8_t(out_u8_MXN[i][j]));
      }
   }
   printf("\n\n");

   free(handle);

   printf("***** END of median example ***** \n");
   return 0;
}