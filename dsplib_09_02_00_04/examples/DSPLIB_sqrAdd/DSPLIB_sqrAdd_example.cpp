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

   // Setup input and output buffers for single-precision datatype

   float in[] = {
       -93402767.98696518,  -635811590.2500186,  -2048438858.1264997, -943043884.078691,
       329773452.93652534,  -2010646387.0167627, -394264694.3655772,  1538797820.7693605,
       -253024826.9190998,  1761580513.2072868,  -1567489803.6343036, -836398874.9244952,
       -1052503241.4965706, 1457407716.1291046,  1070422402.7200341,  -2114310238.265214,
   };

   // {0.,         0.78539816, 1.57079633, 2.35619449, 3.14159265, 3.92699082, 4.71238898,
   // 5.49778714, 0.,         0.78539816, 1.57079633, 2.35619449, 3.14159265, 3.92699082};

   float out[]          = {0.};
   float expected_out[] = {2.73448938427507e+19};

   uint32_t size_in  = 16;
   uint32_t size_out = 1;

   // handles and struct for call to kernel
   DSPLIB_STATUS          status;
   DSPLIB_sqrAdd_InitArgs kerInitArgs;
   int32_t                handleSize = DSPLIB_sqrAdd_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle    handle     = malloc(handleSize);

   DSPLIB_bufParams1D_t bufParamsIn, bufParamsOut;

   // fill in input and output buffer parameters
   bufParamsIn.data_type = DSPLIB_FLOAT32;
   bufParamsIn.dim_x     = size_in;

   bufParamsOut.data_type = DSPLIB_FLOAT32;
   bufParamsOut.dim_x     = size_out;

   kerInitArgs.dataSize  = size_in;
   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   status = DSPLIB_SUCCESS;

   // init checkparams
   // if (status == DSPLIB_SUCCESS)
   //    status = DSPLIB_sqrAdd_init_checkParams(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // init
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_sqrAdd_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   // exec checkparams
   // if (status == DSPLIB_SUCCESS)
   //    DSPLIB_sqrAdd_exec_checkParams(handle, in0, in1, out);

   // exec
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_sqrAdd_exec(handle, in, out);

   // print results
   printf("---------------------------------\n");
   printf("|         Input Vector          |\n");
   printf("---------------------------------\n");
   for (size_t c = 0; c < size_in; c++) {
      printf("%10g \n", in[c]);
   }

   printf("---------------------------------\n");
   printf("|           Results              |\n");
   printf("---------------------------------\n");
   printf("Square Add of input vecotr: Expected = %10g\n", expected_out[0]);
   printf("Square Add of input vecotr: Actual   = %10g\n", out[0]);

   return 0;
}
