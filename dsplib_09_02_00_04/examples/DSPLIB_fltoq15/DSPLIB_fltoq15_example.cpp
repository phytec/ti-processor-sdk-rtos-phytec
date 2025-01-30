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

#include "dsplib.h"
#include <stdint.h>

/******************************************************************************/
/*                                                                            */
/* main                                                                       */
/*                                                                            */
/******************************************************************************/

float inSp[] = {0.8272705078,  0.3588867188,  0.2372436523,  0.4108581543,  0.3357849121,  0.4237976074,  -0.4877624512,
                -0.2095642090, -0.8106994629, -0.0259704590, -0.9184570312, -0.7265319824, 0.9953613281,  -0.4885864258,
                0.1446838379,  -0.2138366699, -0.8549194336, 0.6249389648,  -0.7652587891, -0.6113586426, -0.6343078613,
                -0.4600830078, -0.7890319824, -0.4629516602, -0.4877624512, 0.9420776367,  0.2792358398,  -0.1009826660,
                -0.8718261719, -0.7282714844, 0.0175170898,  0.2448425293};

double  inDp[] = {0.8272705078,  0.3588867188,  0.2372436523,  0.4108581543,  0.3357849121,  0.4237976074,
                  -0.4877624512, -0.2095642090, -0.8106994629, -0.0259704590, -0.9184570312, -0.7265319824,
                  0.9953613281,  -0.4885864258, 0.1446838379,  -0.2138366699, -0.8549194336, 0.6249389648,
                  -0.7652587891, -0.6113586426, -0.6343078613, -0.4600830078, -0.7890319824, -0.4629516602,
                  -0.4877624512, 0.9420776367,  0.2792358398,  -0.1009826660, -0.8718261719, -0.7282714844,
                  0.0175170898,  0.2448425293};
int16_t out[]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/*
int16_t refOut[] = {27108, 11760, 7774, 13463, 11003, 13887, -15983, -6867,
                -26565, -851, -30096, -23807, 32616, -16010, 4741, -7007,
                -28014, 20478, -25076, -20033, -20785, -15076, -25855, -15170,
                -15983, 30870, 9150, -3309, -28568, -23864, 574, 8023};
 */

int32_t size = 32;

int main(void)
{
   DSPLIB_fltoq15_InitArgs kerInitArgs;
   kerInitArgs.dataSize  = size;
   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   int32_t             handleSize = DSPLIB_fltoq15_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle handle     = malloc(handleSize);

   DSPLIB_bufParams1D_t bufParamsIn, bufParamsOut;
   bufParamsOut.data_type = DSPLIB_INT16;
   bufParamsOut.dim_x     = size;

   /* Example for float to q15 conversion */
   bufParamsIn.data_type = DSPLIB_FLOAT32;
   bufParamsIn.dim_x     = size;

   int32_t status = DSPLIB_fltoq15_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_fltoq15_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_fltoq15_exec_checkParams(handle, inSp, out);

         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_fltoq15_exec(handle, inSp, out);
         }
      }
   }
   printf("Single precision floating point to q15 conversion\n");
   printf("%12s -> %s\n", "Input", "Output");
   for (int32_t i = 0; i < size; i++) {
      printf("%.10f -> %10d\n", inSp[i], out[i]);
   }
   printf("\n");

   /* Example for double to q15 conversion */
   bufParamsIn.data_type = DSPLIB_FLOAT64;
   bufParamsIn.dim_x     = size;

   status = DSPLIB_fltoq15_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   if (status == DSPLIB_SUCCESS) {
      status = DSPLIB_fltoq15_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

      if (status == DSPLIB_SUCCESS) {
         status = DSPLIB_fltoq15_exec_checkParams(handle, inDp, out);

         if (status == DSPLIB_SUCCESS) {
            status = DSPLIB_fltoq15_exec(handle, inDp, out);
         }
      }
   }
   printf("Double precision floating point to q15 conversion\n");
   printf("%12s -> %s\n", "Input", "Output");
   for (int32_t i = 0; i < size; i++) {
      printf("%.10lf -> %10d\n", inDp[i], out[i]);
   }
   printf("\n");

   return 0;
}
