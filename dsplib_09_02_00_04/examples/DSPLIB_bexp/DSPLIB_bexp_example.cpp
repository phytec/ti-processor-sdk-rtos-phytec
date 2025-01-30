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

int main(void)
{

   // Setup input and output buffers for different datatypes
   uint8_t inInt8[] = {1, 9, 15, 3, 12, 14, 15, 13, 2, 8, 6, 7, 9, 2};

   uint32_t inInt32[] = {2181465, 2462575, 3127500, 1575700, 4112055, 2767555, 2957695,
                         1800006, 1058335, 3940625, 5669641, 1475868, 5595901, 4069899};

   int64_t inInt64[] = {-823316427289107, -40974726053892, 621756103702919, 410477363340179, 396819477993780,
                        14648199608068,   -52453922443103, 564813653944084, 495780439362992, -355413015034085,
                        166950099247693,  489582244777414, 187861904257253, 190620153460260};

   int32_t out = 0;

   size_t size = 14;

   // Status variables
   DSPLIB_STATUS status_init = DSPLIB_SUCCESS;
   DSPLIB_STATUS status_exec = DSPLIB_SUCCESS;

   // Initialize the kernel handle
   DSPLIB_bexp_InitArgs kerInitArgs;
   int32_t              handleSize = DSPLIB_bexp_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle  handle     = malloc(handleSize);
   DSPLIB_bufParams1D_t bufParamsIn, bufParamsOut;

   // Initialize the kerInitArgs
   kerInitArgs.dataSize  = size;
   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   /**
    * Example 1: uint8 vector
    */
   // Update initArgs and bufparams
   bufParamsIn.data_type = DSPLIB_UINT8;
   bufParamsIn.dim_x     = size;

   bufParamsOut.data_type = DSPLIB_INT32;
   bufParamsOut.dim_x     = 1;

   // Step 1: Init check params
   status_init = DSPLIB_bexp_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
   // Step 2: Init the kernel
   status_init = DSPLIB_bexp_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
   // Step 3: Exec check params
   status_exec = DSPLIB_bexp_exec_checkParams(handle, &inInt8, &out);
   // Step 4: Exec the kernel
   status_exec = DSPLIB_bexp_exec(handle, &inInt8, &out);
   // Print the output
   printf("UINT8 Example bexp kernel output = %d\n", out);

   /**
    * Example 2: uint32 vector
    */
   // Update initArgs and bufparams
   bufParamsIn.data_type = DSPLIB_UINT32;
   bufParamsIn.dim_x     = size;

   bufParamsOut.data_type = DSPLIB_INT32;
   bufParamsOut.dim_x     = 1;

   // Step 1: Init check params
   status_init = DSPLIB_bexp_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
   // Step 2: Init the kernel
   status_init = DSPLIB_bexp_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
   // Step 3: Exec check params
   status_exec = DSPLIB_bexp_exec_checkParams(handle, &inInt32, &out);
   // Step 4: Exec the kernel
   status_exec = DSPLIB_bexp_exec(handle, &inInt32, &out);
   // Print the output
   printf("UINT32 Example bexp kernel output = %d\n", out);

   /**
    * Example 1: uint8 vector
    */
   // Update initArgs and bufparams
   bufParamsIn.data_type = DSPLIB_INT64;
   bufParamsIn.dim_x     = size;

   bufParamsOut.data_type = DSPLIB_INT32;
   bufParamsOut.dim_x     = 1;

   // Step 1: Init check params
   status_init = DSPLIB_bexp_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
   // Step 2: Init the kernel
   status_init = DSPLIB_bexp_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);
   // Step 3: Exec check params
   status_exec = DSPLIB_bexp_exec_checkParams(handle, &inInt64, &out);
   // Step 4: Exec the kernel
   status_exec = DSPLIB_bexp_exec(handle, &inInt64, &out);
   // Print the output
   printf("INT64 Example bexp kernel output = %d\n", out);

   return 0;
}
