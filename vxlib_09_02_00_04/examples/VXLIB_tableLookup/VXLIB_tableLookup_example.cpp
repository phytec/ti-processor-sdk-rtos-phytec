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
    int32_t count = 9;
    int32_t offset = 0;
    uint8_t src_u8[] = {8, 6, 3,
			               4, 5, 6,
			               7, 1, 0};

    
    uint8_t dst_u8[] = {0, 0, 0,
			               0, 0, 0,
			               0, 0, 0};

    int8_t src_s8[] = {8, 6, 3,
			               4, 5, 6,
			               7, 1, 0};

    
    int8_t dst_s8[] = {0, 0, 0,
			               0, 0, 0,
			               0, 0, 0};

    uint16_t src_s16[] = {8, 8, 7,
                        6, 0, 4,
                        3, 2, 1};
    
    uint16_t dst_s16[] = {0, 0, 0,
			               0, 0, 0,
			               0, 0, 0};

    int16_t src_u16[] = {8, 8, 7,
                        6, 0, 4,
                        3, 2, 1};
    
    int16_t dst_u16[] = {0, 0, 0,
			               0, 0, 0,
			               0, 0, 0};

    uint8_t lut_u8[] = {13,25, 10,
			               13, 11, 4,
			               19, 41, 2};

    int8_t lut_s8[] = {13,25, 10,
			               13, 11, 4,
			               19, 41, 2};

    uint16_t lut_u16[] = {9, 8, 7,
                        6, 5, 4,
                        3, 2, 1};
    int16_t lut_s16[] = {9, 8, 7,
                        6, 5, 4,
                        3, 2, 1};

   // clang-format on

   // handles and struct for call to kernel
   VXLIB_STATUS               status;
   VXLIB_tableLookup_InitArgs kerInitArgs;
   int32_t                    handleSize = VXLIB_tableLookup_getHandleSize(&kerInitArgs);
   VXLIB_kernelHandle         handle     = malloc(handleSize);

   VXLIB_bufParams2D_t bufParamsIn0, bufParamsOut;
   VXLIB_bufParams1D_t bufParamsLut;

   bufParamsIn0.data_type = VXLIB_UINT8;
   bufParamsIn0.dim_x     = width;
   bufParamsIn0.dim_y     = height;
   bufParamsIn0.stride_y  = width * sizeof(uint8_t);

   bufParamsOut.data_type = VXLIB_UINT8;
   bufParamsOut.dim_x     = width;
   bufParamsOut.dim_y     = height;
   bufParamsOut.stride_y  = width * sizeof(uint8_t);

   kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;
   kerInitArgs.count     = count;
   kerInitArgs.offset    = offset;

   bufParamsLut.data_type = VXLIB_UINT8;

   status = VXLIB_SUCCESS;

   // init checkparams
   status = VXLIB_tableLookup_init_checkParams(handle, &bufParamsIn0, &bufParamsOut, &bufParamsLut, &kerInitArgs);

   // init
   if (status == VXLIB_SUCCESS)
      status = VXLIB_tableLookup_init(handle, &bufParamsIn0, &bufParamsOut, &bufParamsLut, &kerInitArgs);
   else {
      printf("\nInit checkparams failed: %d\n", status);
      return 0;
   }
   status = VXLIB_tableLookup_set(handle, lut_u8);
   // exec checkparams
   status = VXLIB_tableLookup_exec_checkParams(handle, src_u8, dst_u8, lut_u8);

   // exec
   if (status == VXLIB_SUCCESS)
      status = VXLIB_tableLookup_exec(handle, src_u8, dst_u8, lut_u8);
   else {
      printf("\nExec checkparams failed: %d\n", status);
      return 0;
   }

   printf("\n UNSIGNED 8 BIT tableLookup \n");
   printf("\n src Buffer \n");
   for (size_t i = 0; i < height; i++) {
      printf("\n\n");
      for (size_t j = 0; j < width; j++) {
         printf("%8d, ", src_u8[i * width + j]);
      }
   }
   printf("\n LUT values \n");
   for (size_t i = 0; i < count; i++) {
      printf("%8d, ", lut_u8[i]);
   }
   printf("\n\n");
   printf("\n\n");
   printf("\n dst Buffer \n");

   for (size_t i = 0; i < height; i++) {
      printf("\n\n");
      for (size_t j = 0; j < width; j++) {
         printf("%8d, ", dst_u8[i * width + j]);
      }
   }
   printf("\n\n");

   bufParamsIn0.data_type = VXLIB_INT8;
   bufParamsIn0.dim_x     = width;
   bufParamsIn0.dim_y     = height;
   bufParamsIn0.stride_y  = width * sizeof(int8_t);

   bufParamsOut.data_type = VXLIB_INT8;
   bufParamsOut.dim_x     = width;
   bufParamsOut.dim_y     = height;
   bufParamsOut.stride_y  = width * sizeof(int8_t);

   kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;
   kerInitArgs.count     = count;
   kerInitArgs.offset    = offset;

   bufParamsLut.data_type = VXLIB_INT8;

   status = VXLIB_SUCCESS;

   // init checkparams
   status = VXLIB_tableLookup_init_checkParams(handle, &bufParamsIn0, &bufParamsOut, &bufParamsLut, &kerInitArgs);

   // init
   if (status == VXLIB_SUCCESS)
      status = VXLIB_tableLookup_init(handle, &bufParamsIn0, &bufParamsOut, &bufParamsLut, &kerInitArgs);
   else {
      printf("\nInit checkparams failed: %d\n", status);
      return 0;
   }
   status = VXLIB_tableLookup_set(handle, lut_s8);
   // exec checkparams
   status = VXLIB_tableLookup_exec_checkParams(handle, src_s8, dst_s8, lut_s8);

   // exec
   if (status == VXLIB_SUCCESS)
      status = VXLIB_tableLookup_exec(handle, src_s8, dst_s8, lut_s8);
   else {
      printf("\nExec checkparams failed: %d\n", status);
      return 0;
   }

   printf("\n SIGNED 8 BIT tableLookup \n");
   printf("\n src Buffer \n");
   for (size_t i = 0; i < height; i++) {
      printf("\n\n");
      for (size_t j = 0; j < width; j++) {
         printf("%8d, ", src_s8[i * width + j]);
      }
   }
   printf("\n LUT values \n");
   for (size_t i = 0; i < count; i++) {
      printf("%8d, ", lut_s8[i]);
   }
   printf("\n\n");
   printf("\n\n");
   printf("\n dst Buffer \n");

   for (size_t i = 0; i < height; i++) {
      printf("\n\n");
      for (size_t j = 0; j < width; j++) {
         printf("%8d, ", dst_s8[i * width + j]);
      }
   }
   printf("\n\n");

   bufParamsIn0.data_type = VXLIB_UINT16;
   bufParamsIn0.dim_x     = width;
   bufParamsIn0.dim_y     = height;
   bufParamsIn0.stride_y  = width * sizeof(uint16_t);

   bufParamsOut.data_type = VXLIB_UINT16;
   bufParamsOut.dim_x     = width;
   bufParamsOut.dim_y     = height;
   bufParamsOut.stride_y  = width * sizeof(uint16_t);

   kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;
   kerInitArgs.count     = count;

   bufParamsLut.data_type = VXLIB_UINT16;

   status = VXLIB_SUCCESS;

   // init checkparams
   status = VXLIB_tableLookup_init_checkParams(handle, &bufParamsIn0, &bufParamsOut, &bufParamsLut, &kerInitArgs);

   // init
   if (status == VXLIB_SUCCESS)
      status = VXLIB_tableLookup_init(handle, &bufParamsIn0, &bufParamsOut, &bufParamsLut, &kerInitArgs);
   else {
      printf("\nInit checkparams failed: %d\n", status);
      return 0;
   }
   status = VXLIB_tableLookup_set(handle, lut_u16);
   // exec checkparams
   status = VXLIB_tableLookup_exec_checkParams(handle, src_u16, dst_u16, lut_u16);

   // exec
   if (status == VXLIB_SUCCESS)
      status = VXLIB_tableLookup_exec(handle, src_u16, dst_u16, lut_u16);
   else {
      printf("\nExec checkparams failed: %d\n", status);
      return 0;
   }

   printf("\n UNSIGEND 16 BIT tableLookup \n");
   printf("\n src Buffer \n");
   for (size_t i = 0; i < height; i++) {
      printf("\n\n");
      for (size_t j = 0; j < width; j++) {
         printf("%8d, ", src_u16[i * width + j]);
      }
   }
   printf("\n LUT values \n");
   for (size_t i = 0; i < count; i++) {
      printf("%8d, ", lut_u16[i]);
   }
   printf("\n\n");
   printf("\n\n");
   printf("\n dst Buffer \n");

   for (size_t i = 0; i < height; i++) {
      printf("\n\n");
      for (size_t j = 0; j < width; j++) {
         printf("%8d, ", dst_u16[i * width + j]);
      }
   }
   printf("\n\n");

   bufParamsIn0.data_type = VXLIB_INT16;
   bufParamsIn0.dim_x     = width;
   bufParamsIn0.dim_y     = height;
   bufParamsIn0.stride_y  = width * sizeof(int16_t);

   bufParamsOut.data_type = VXLIB_INT16;
   bufParamsOut.dim_x     = width;
   bufParamsOut.dim_y     = height;
   bufParamsOut.stride_y  = width * sizeof(int16_t);

   kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;
   kerInitArgs.count     = count;

   bufParamsLut.data_type = VXLIB_INT16;

   status = VXLIB_SUCCESS;

   // init checkparams
   status = VXLIB_tableLookup_init_checkParams(handle, &bufParamsIn0, &bufParamsOut, &bufParamsLut, &kerInitArgs);


   // init
   if (status == VXLIB_SUCCESS)
      status = VXLIB_tableLookup_init(handle, &bufParamsIn0, &bufParamsOut, &bufParamsLut, &kerInitArgs);
   else {
      printf("\nInit checkparams failed: %d\n", status);
      return 0;
   }
   status = VXLIB_tableLookup_set(handle, lut_s16);
   // exec checkparams
   status = VXLIB_tableLookup_exec_checkParams(handle, src_s16, dst_s16, lut_s16);

   // exec
   if (status == VXLIB_SUCCESS)
      status = VXLIB_tableLookup_exec(handle, src_s16, dst_s16, lut_s16);
   else {
      printf("\nExec checkparams failed: %d\n", status);
      return 0;
   }

   printf("\n SIGEND 16 BIT tableLookup \n");
   printf("\n src Buffer \n");
   for (size_t i = 0; i < height; i++) {
      printf("\n\n");
      for (size_t j = 0; j < width; j++) {
         printf("%8d, ", src_s16[i * width + j]);
      }
   }
   printf("\n LUT values \n");
   for (size_t i = 0; i < count; i++) {
      printf("%8d, ", lut_s16[i]);
   }
   printf("\n\n");
   printf("\n\n");
   printf("\n dst Buffer \n");

   for (size_t i = 0; i < height; i++) {
      printf("\n\n");
      for (size_t j = 0; j < width; j++) {
         printf("%8d, ", dst_s16[i * width + j]);
      }
   }
   printf("\n\n");

   free(handle);

   return 0;
}
