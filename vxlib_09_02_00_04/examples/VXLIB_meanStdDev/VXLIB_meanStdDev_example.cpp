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

   // unpartitioned input image
   uint8_t in_u8[] = {1, 2, 3, 4, 5, 6,
		      7, 8, 9, 10, 11, 12,
                      13, 14, 15, 16, 17, 18};
    
    // partitioned into 2 input blocks
    uint8_t in0_u8[] = {1, 2, 3,
                        7, 8, 9,
                        13, 14, 15};

    uint8_t in1_u8[] = {4, 5, 6,
                        10, 11, 12,
                        16, 17, 18};
    
    // block based computation parameters
    uint32_t pixelsProcessed = 0;
    uint32_t currentSum = 0;
    uint32_t currentSqSum = 0;

    // initialize output
    float out0_f32 = 0;  // mean
    float out1_f32 = 0; // standard deviation

   // clang-format on

   // handles and struct for call to kernel
   VXLIB_STATUS              status;
   VXLIB_meanStdDev_InitArgs kerInitArgs;
   int32_t                   handleSize = VXLIB_meanStdDev_getHandleSize(&kerInitArgs);
   VXLIB_kernelHandle        handle     = malloc(handleSize);

   VXLIB_bufParams2D_t bufParamsIn;

   bufParamsIn.data_type = VXLIB_UINT8;
   bufParamsIn.dim_x     = width;
   bufParamsIn.dim_y     = height;
   bufParamsIn.stride_y  = width * sizeof(uint8_t);

   kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;

   status = VXLIB_SUCCESS;

   /* Compute Intermediate Block Outputs */

   // init checkparams
   status = VXLIB_meanStdDev_init_checkParams(handle, &bufParamsIn, &kerInitArgs);

   // init
   if (status == VXLIB_SUCCESS)
      status = VXLIB_meanStdDev_init(handle, &bufParamsIn, &kerInitArgs);
   else {
      printf("\nInit checkparams failed: %d\n", status);
      return 0;
   }

   // exec checkparams
   VXLIB_meanStdDev_exec_checkParams(handle, in0_u8, &out0_f32, &out1_f32, &pixelsProcessed, &currentSum,
                                     &currentSqSum);

   // exec
   if (status == VXLIB_SUCCESS)
      status =
          VXLIB_meanStdDev_exec(handle, in0_u8, &out0_f32, &out1_f32, &pixelsProcessed, &currentSum, &currentSqSum);
   else {
      printf("\nExec checkparams failed: %d\n", status);
      return 0;
   }

   // print results
   printf("\nIntermediate Block Outputs:\n");
   printf("Mean: %f\n", out0_f32);
   printf("Standard Deviation: %f\n", out1_f32);
   printf("Pixels Processed: %u\n", pixelsProcessed);
   printf("Current Sum: %u\n", currentSum);
   printf("Current Squared Sum: %u\n", currentSqSum);

   /* Process next block and get output for entire image */

   // re-run init and init checkparams again if bufparams need to be modified

   // exec checkparams
   VXLIB_meanStdDev_exec_checkParams(handle, in1_u8, &out0_f32, &out1_f32, &pixelsProcessed, &currentSum,
                                     &currentSqSum);

   // exec
   if (status == VXLIB_SUCCESS)
      status =
          VXLIB_meanStdDev_exec(handle, in1_u8, &out0_f32, &out1_f32, &pixelsProcessed, &currentSum, &currentSqSum);
   else {
      printf("\nExec checkparams failed: %d\n", status);
      return 0;
   }

   printf("\nEntire Image Outputs:\n");
   printf("Mean: %f\n", out0_f32);
   printf("Standard Deviation: %f\n", out1_f32);
   printf("Pixels Processed: %u\n", pixelsProcessed);
   printf("Current Sum: %u\n", currentSum);
   printf("Current Squared Sum: %u\n", currentSqSum);

   free(handle);

   return 0;
}
