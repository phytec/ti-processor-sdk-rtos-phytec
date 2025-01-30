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

#include "DSPLIB_bufParams.h"
#include "DSPLIB_types.h"
#include "dsplib.h"
#include <cstdio>
#include <stdint.h>

#define M (3)
#define N (3)
#define K (3)

/******************************************************************************/
/*                                                                            */
/* main                                                                       */
/*                                                                            */
/******************************************************************************/

int main(void)
{

   // clang-format off

  // Setup input and output buffers for single- and double-precision datatypes
   float in0[] = {0.71649936, 0.13543484, 0.50923542,
		  0.54119591, 0.19242506, 0.38308575,
                  0.24567145, 0.05629663, 0.99152828};

   float in1[] = {1.0, 0.0, 0.0,
		  0.0, 1.0, 0.0,
		  0.0, 0.0, 1.0};
   
   float out[] = {0., 0., 0.,
		  0., 0., 0.,
		  0., 0., 0.};

   double in0D[] = {0.71649936, 0.13543484, 0.50923542,
		  0.54119591, 0.19242506, 0.38308575,
                  0.24567145, 0.05629663, 0.99152828};

   double in1D[] = {1.0, 0.0, 0.0,
		  0.0, 1.0, 0.0,
		  0.0, 0.0, 1.0};
   
   double outD[] = {0., 0., 0.,
		  0., 0., 0.,
		  0., 0., 0.};

   // clang-format on

   // handles and struct for call to kernel
   DSPLIB_STATUS          status;
   DSPLIB_matMul_InitArgs kerInitArgs;
   int32_t                handleSize = DSPLIB_matMul_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle    handle     = malloc(handleSize);

   DSPLIB_bufParams2D_t bufParamsIn0, bufParamsIn1, bufParamsOut;

   // fill in input and output buffer parameters
   bufParamsIn0.data_type = DSPLIB_FLOAT32;
   bufParamsIn0.dim_x     = K;
   bufParamsIn0.dim_y     = M;
   bufParamsIn0.stride_y  = K * sizeof(float);

   bufParamsIn1.data_type = DSPLIB_FLOAT32;
   bufParamsIn1.dim_x     = N;
   bufParamsIn1.dim_y     = K;
   bufParamsIn1.stride_y  = N * sizeof(float);

   bufParamsOut.data_type = DSPLIB_FLOAT32;
   bufParamsOut.dim_x     = N;
   bufParamsOut.dim_y     = M;
   bufParamsOut.stride_y  = N * sizeof(float);

   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   status = DSPLIB_SUCCESS;

   // init checkparams
   // if (status == DSPLIB_SUCCESS)
   //    status = DSPLIB_matMul_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // init
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_matMul_init(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut, &kerInitArgs);

   // exec checkparams
   // if (status == DSPLIB_SUCCESS)
   //    DSPLIB_matMul_exec_checkParams(handle, in0, in1, out);

   // exec
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_matMul_exec(handle, in0, in1, out);

   printf("\n FLOAT \n");

   // print results
   for (size_t i = 0; i < M; i++) {
      printf("\n\n");
      for (size_t j = 0; j < N; j++) {
         printf("%10g, ", out[i * N + j]);
      }
   }
   printf("\n\n");

   // fill in input and output buffer parameters
   bufParamsIn0.data_type = DSPLIB_FLOAT64;
   bufParamsIn0.dim_x     = K;
   bufParamsIn0.dim_y     = M;
   bufParamsIn0.stride_y  = K * sizeof(double);

   bufParamsIn1.data_type = DSPLIB_FLOAT64;
   bufParamsIn1.dim_x     = N;
   bufParamsIn1.dim_y     = K;
   bufParamsIn1.stride_y  = N * sizeof(double);

   bufParamsOut.data_type = DSPLIB_FLOAT64;
   bufParamsOut.dim_x     = N;
   bufParamsOut.dim_y     = M;
   bufParamsOut.stride_y  = N * sizeof(double);

   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   status = DSPLIB_SUCCESS;

   // init checkparams
   // if (status == DSPLIB_SUCCESS)
   //    status = DSPLIB_matMul_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // init
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_matMul_init(handle, &bufParamsIn0, &bufParamsIn1, &bufParamsOut, &kerInitArgs);

   // exec checkparams
   // if (status == DSPLIB_SUCCESS)
   //    DSPLIB_matMul_exec_checkParams(handle, in0, in1, out);

   // exec
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_matMul_exec(handle, in0D, in1D, outD);

   printf("\n DOUBLE \n");

   // print results
   for (size_t i = 0; i < M; i++) {
      printf("\n\n");
      for (size_t j = 0; j < N; j++) {
         printf("%10g, ", outD[i * N + j]);
      }
   }
   printf("\n\n");
   return 0;
}
