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

/******************************************************************************/
/*                                                                            */
/* main                                                                       */
/*                                                                            */
/******************************************************************************/

int main(void)
{

   // TODO: kxk convolve
   // TODO: mxn convolve

   // declare image height and width dimensions
   uint32_t height, width;
   uint32_t filterWidth, filterHeight;

   /*****************************************/
   /*          Non-Padded K x K Example       */
   /*****************************************/

   // input image dimension must be greater than or equal to filter dimensions
   height       = 5;
   width        = 5;
   filterWidth  = 3;
   filterHeight = 3;

   // output image dimension calculation
   uint32_t outHeight = height - filterHeight + 1;
   uint32_t outWidth  = width - filterWidth + 1;

   // clang-format off
    
  uint8_t in_u8_KxK[] = {1,  2,  3,  4,  5,
			 6,  7,  8,  9,  10,
			 11, 12, 13, 14, 15,
			 16, 17, 18, 19, 20,
			 21, 22, 23, 24, 25};

  int16_t filter_s16_KxK[] = {1, 2, 3,
			      4, 5, 6,
			      7, 8, 9};
  
   uint8_t out_u8_KxK[] = {0, 0, 0,
			   0, 0, 0,
			   0, 0, 0};

   // clang-format on

   // handles and struct for call to kernel
   VXLIB_STATUS            status;
   VXLIB_convolve_InitArgs kerInitArgs;
   int32_t                 handleSize = VXLIB_convolve_getHandleSize(&kerInitArgs);
   VXLIB_kernelHandle      handle     = malloc(handleSize);

   VXLIB_bufParams2D_t bufParamsIn, bufParamsOut, bufParamsFilter;

   bufParamsIn.data_type = VXLIB_UINT8;
   bufParamsIn.dim_x     = width;
   bufParamsIn.dim_y     = height;
   bufParamsIn.stride_y  = width * sizeof(uint8_t);

   bufParamsFilter.data_type = VXLIB_INT16;
   bufParamsFilter.dim_x     = filterWidth;
   bufParamsFilter.dim_y     = filterHeight;
   bufParamsFilter.stride_y  = filterWidth * sizeof(int16_t);

   bufParamsOut.data_type = VXLIB_UINT8;
   bufParamsOut.dim_x     = outWidth;
   bufParamsOut.dim_y     = outHeight;
   bufParamsOut.stride_y  = outWidth * sizeof(uint8_t);

   kerInitArgs.padLeft      = 0;
   kerInitArgs.padRight     = 0;
   kerInitArgs.padTop       = 0;
   kerInitArgs.padBottom    = 0;
   kerInitArgs.scale        = 4;
   kerInitArgs.filterWidth  = filterWidth;
   kerInitArgs.filterHeight = filterHeight;

   kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;

   status = VXLIB_SUCCESS;

   // init checkparams
   status = VXLIB_convolve_init_checkParams(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);

   // init
   if (status == VXLIB_SUCCESS)
      status = VXLIB_convolve_init(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
   else {
      printf("\nInit checkparams failed: %d\n", status);
      return 0;
   }

   // exec checkparams
   VXLIB_convolve_exec_checkParams(handle, in_u8_KxK, filter_s16_KxK, out_u8_KxK);

   // exec
   if (status == VXLIB_SUCCESS)
      status = VXLIB_convolve_exec(handle, in_u8_KxK, filter_s16_KxK, out_u8_KxK);
   else {
      printf("\nExec checkparams failed: %d\n", status);
      return 0;
   }

   // print results
   printf("\nNon-Padded %d x %d Convolve Filter\n", filterHeight, filterWidth);
   printf("\nInput Image\n");

   for (size_t i = 0; i < height; i++) {
      printf("\n");
      for (size_t j = 0; j < width; j++) {
         printf("%d, ", in_u8_KxK[i * width + j]);
      }
   }

   printf("\n");

   printf("\nOutput Image\n");

   for (size_t i = 0; i < outHeight; i++) {
      printf("\n");
      for (size_t j = 0; j < outWidth; j++) {
         printf("%d, ", out_u8_KxK[i * outWidth + j]);
      }
   }
   printf("\n\n");

   /*****************************************/
   /*          Non-Padded MxN Example       */
   /*****************************************/

   height       = 5;
   width        = 5;
   filterHeight = 2;
   filterWidth  = 4;

   outHeight = height - filterHeight + 1;
   outWidth  = width - filterWidth + 1;

   bufParamsIn.data_type = VXLIB_UINT8;
   bufParamsIn.dim_x     = width;
   bufParamsIn.dim_y     = height;
   bufParamsIn.stride_y  = width * sizeof(uint8_t);

   bufParamsFilter.data_type = VXLIB_INT16;
   bufParamsFilter.dim_x     = filterWidth;
   bufParamsFilter.dim_y     = filterHeight;
   bufParamsFilter.stride_y  = filterWidth * sizeof(int16_t);

   bufParamsOut.data_type = VXLIB_UINT8;
   bufParamsOut.dim_x     = outWidth;
   bufParamsOut.dim_y     = outHeight;
   bufParamsOut.stride_y  = outWidth * sizeof(uint8_t);

   kerInitArgs.padLeft      = 0;
   kerInitArgs.padRight     = 0;
   kerInitArgs.padTop       = 0;
   kerInitArgs.padBottom    = 0;
   kerInitArgs.scale        = 4;
   kerInitArgs.filterWidth  = filterWidth;
   kerInitArgs.filterHeight = filterHeight;

   kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;

   status = VXLIB_SUCCESS;

   // clang-format off

   uint8_t in_u8_MxN[] = {1,  2,  3,  4,  5,
			  6,  7,  8,  9,  10,
			  11, 12, 13, 14, 15,
			  16, 17, 18, 19, 20,
			  21, 22, 23, 24, 25};

   int16_t filter_s16_MxN[] = {1, 2, 3, 4,
			       5, 6, 7, 8};
  
   uint8_t out_u8_MxN[] = {0, 0,
			   0, 0,
			   0, 0,
                           0, 0};

   // clang-format on

   // init checkparams
   status = VXLIB_convolve_init_checkParams(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);

   // init
   if (status == VXLIB_SUCCESS)
      status = VXLIB_convolve_init(handle, &bufParamsIn, &bufParamsFilter, &bufParamsOut, &kerInitArgs);
   else {
      printf("\nInit checkparams failed: %d\n", status);
      return 0;
   }

   // exec checkparams
   VXLIB_convolve_exec_checkParams(handle, in_u8_MxN, filter_s16_MxN, out_u8_MxN);

   // exec
   if (status == VXLIB_SUCCESS)
      status = VXLIB_convolve_exec(handle, in_u8_MxN, filter_s16_MxN, out_u8_MxN);
   else {
      printf("\nExec checkparams failed: %d\n", status);
      return 0;
   }

   // print results
   printf("\nNon-Padded %d x %d Convolve Filter\n", filterHeight, filterWidth);
   printf("\nInput Image\n");

   for (size_t i = 0; i < height; i++) {
      printf("\n");
      for (size_t j = 0; j < width; j++) {
         printf("%d, ", in_u8_MxN[i * width + j]);
      }
   }

   printf("\n");

   printf("\nOutput Image\n");

   for (size_t i = 0; i < outHeight; i++) {
      printf("\n");
      for (size_t j = 0; j < outWidth; j++) {
         printf("%d, ", out_u8_MxN[i * outWidth + j]);
      }
   }
   printf("\n\n");

   free(handle);

   return 0;
}
