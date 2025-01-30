/******************************************************************************/
/* Copyright (C) 2017 Texas Instruments Incorporated - http://www.ti.com/
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

/*******************************************************************************
 *
 * INCLUDES
 *
 ******************************************************************************/

#include "../common/c71/DSPLIB_inlines.h"
#include "DSPLIB_matMul_priv.h"
#include "DSPLIB_types.h"
#include "TI_platforms.h"
#include <cstdint>
#include <float.h>

#define DSPLIB_MATMUL_UNROLL (1024)

template <typename dataType>
DSPLIB_STATUS DSPLIB_matMul_init_ci(DSPLIB_kernelHandle           handle,
                                    const DSPLIB_bufParams2D_t   *bufParamsIn0,
                                    const DSPLIB_bufParams2D_t   *bufParamsIn1,
                                    const DSPLIB_bufParams2D_t   *bufParamsOut,
                                    const DSPLIB_matMul_InitArgs *pKerInitArgs)
{

   DSPLIB_STATUS           status       = DSPLIB_SUCCESS;
   DSPLIB_matMul_PrivArgs *pKerPrivArgs = (DSPLIB_matMul_PrivArgs *) handle;

   int32_t N = pKerPrivArgs->N;

   if ((N * sizeof(dataType)) % DSPLIB_MATMUL_UNROLL == 0) {
      status = DSPLIB_matMul_N_unroll_init_ci<dataType>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut, pKerInitArgs);
   }
   else {
      status = DSPLIB_matMul_generic_init_ci<dataType>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut, pKerInitArgs);
   }

   return status;
}

template DSPLIB_STATUS DSPLIB_matMul_init_ci<float>(DSPLIB_kernelHandle           handle,
                                                    const DSPLIB_bufParams2D_t   *bufParamsIn0,
                                                    const DSPLIB_bufParams2D_t   *bufParamsIn1,
                                                    const DSPLIB_bufParams2D_t   *bufParamsOut,
                                                    const DSPLIB_matMul_InitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_matMul_init_ci<double>(DSPLIB_kernelHandle           handle,
                                                     const DSPLIB_bufParams2D_t   *bufParamsIn0,
                                                     const DSPLIB_bufParams2D_t   *bufParamsIn1,
                                                     const DSPLIB_bufParams2D_t   *bufParamsOut,
                                                     const DSPLIB_matMul_InitArgs *pKerInitArgs);
template <typename dataType>
DSPLIB_STATUS
DSPLIB_matMul_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut)
{

   DSPLIB_STATUS           status       = DSPLIB_SUCCESS;
   DSPLIB_matMul_PrivArgs *pKerPrivArgs = (DSPLIB_matMul_PrivArgs *) handle;

   int32_t N = pKerPrivArgs->N;

   if ((N * sizeof(dataType)) % DSPLIB_MATMUL_UNROLL == 0) {

      status = DSPLIB_matMul_N_unroll_exec_ci<dataType>(handle, pIn0, pIn1, pOut);
   }
   else {

      status = DSPLIB_matMul_generic_exec_ci<dataType>(handle, pIn0, pIn1, pOut);
   }

   return status;
}

template DSPLIB_STATUS
DSPLIB_matMul_exec_ci<float>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_matMul_exec_ci<double>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut);
