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
#include <float.h>

/*******************************************************************************
 *
 * DEFINES
 *
 ******************************************************************************/

#define SE_PARAM_BASE (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SE1_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_BASE)
#define SE_SA0_PARAM_OFFSET (SE_SE1_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_SA1_PARAM_OFFSET (SE_SA0_PARAM_OFFSET + SE_PARAM_SIZE)

#define DSPLIB_MATMUL_UNROLL_FACTOR (16)
#define DSPLIB_MATMUL_SE_UNROLL_FACTOR (8)

template <typename dataType>
DSPLIB_STATUS DSPLIB_matMul_N_unroll_init_ci(DSPLIB_kernelHandle           handle,
                                             const DSPLIB_bufParams2D_t   *bufParamsIn0,
                                             const DSPLIB_bufParams2D_t   *bufParamsIn1,
                                             const DSPLIB_bufParams2D_t   *bufParamsOut,
                                             const DSPLIB_matMul_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS    status = DSPLIB_SUCCESS;
   __SE_TEMPLATE_v1 se0Params;
   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;

   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;
   __SA_VECLEN  SA_VECLEN;

   DSPLIB_matMul_PrivArgs *pKerPrivArgs = (DSPLIB_matMul_PrivArgs *) handle;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   int32_t M         = pKerPrivArgs->M;
   int32_t K         = pKerPrivArgs->K;
   int32_t N         = pKerPrivArgs->N;
   int32_t strideIn0 = pKerPrivArgs->strideIn0Elements;
   int32_t strideIn1 = pKerPrivArgs->strideIn1Elements;
   int32_t strideOut = pKerPrivArgs->strideOutElements;

   typedef typename c7x::make_full_vector<dataType>::type vec;

   int32_t elementCount = c7x::element_count_of<vec>::value;
   SE_VECLEN            = c7x::se_veclen<vec>::value;
   SA_VECLEN            = c7x::sa_veclen<vec>::value;
   SE_ELETYPE           = c7x::se_eletype<vec>::value;

   int32_t NBlocks       = ((N + (DSPLIB_MATMUL_UNROLL_FACTOR - 1)) / ((DSPLIB_MATMUL_UNROLL_FACTOR) *elementCount));
   pKerPrivArgs->NBlocks = NBlocks;

   /**********************************************************************/
   /* Prepare SA template to fetch A matrix                              */
   /**********************************************************************/

   sa0Params        = __gen_SA_TEMPLATE_v1();
   sa0Params.VECLEN = SA_VECLEN;
   sa0Params.DIMFMT = __SA_DIMFMT_4D;

   sa0Params.ICNT0 = 1;
   sa0Params.ICNT1 = K;
   sa0Params.DIM1  = 1;
   sa0Params.ICNT2 = NBlocks;
   sa0Params.DIM2  = 0;
   sa0Params.ICNT3 = M;
   sa0Params.DIM3  = strideIn0;

   /**********************************************************************/
   /* Prepare streaming engine 0 to fetch B matrix                       */
   /**********************************************************************/

   se0Params         = __gen_SE_TEMPLATE_v1();
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;
   se0Params.DIMFMT  = __SE_DIMFMT_5D;

   se0Params.ICNT0 = elementCount;
   se0Params.ICNT1 = DSPLIB_MATMUL_SE_UNROLL_FACTOR;
   se0Params.DIM1  = (int32_t)((uint32_t)elementCount << (uint32_t)1);
   se0Params.ICNT2 = K;
   se0Params.DIM2  = strideIn1;
   se0Params.ICNT3 = NBlocks;
   se0Params.DIM3  = elementCount * DSPLIB_MATMUL_UNROLL_FACTOR;
   se0Params.ICNT4 = M;
   se0Params.DIM4  = 0;

   /**********************************************************************/
   /* Prepare SA template to fetch A matrix                              */
   /**********************************************************************/

   sa1Params        = __gen_SA_TEMPLATE_v1();
   sa1Params.VECLEN = SA_VECLEN;
   sa1Params.DIMFMT = __SA_DIMFMT_4D;

   sa1Params.ICNT0 = elementCount;
   sa1Params.ICNT1 = DSPLIB_MATMUL_UNROLL_FACTOR;
   sa1Params.DIM1  = elementCount;
   sa1Params.ICNT2 = NBlocks;
   sa1Params.DIM2  = elementCount * DSPLIB_MATMUL_UNROLL_FACTOR;
   sa1Params.ICNT3 = M;
   sa1Params.DIM3  = strideOut;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;

   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET) = sa1Params;

   return status;
}

template DSPLIB_STATUS DSPLIB_matMul_N_unroll_init_ci<float>(DSPLIB_kernelHandle           handle,
                                                             const DSPLIB_bufParams2D_t   *bufParamsIn0,
                                                             const DSPLIB_bufParams2D_t   *bufParamsIn1,
                                                             const DSPLIB_bufParams2D_t   *bufParamsOut,
                                                             const DSPLIB_matMul_InitArgs *pKerInitArgs);
template DSPLIB_STATUS DSPLIB_matMul_N_unroll_init_ci<double>(DSPLIB_kernelHandle           handle,
                                                              const DSPLIB_bufParams2D_t   *bufParamsIn0,
                                                              const DSPLIB_bufParams2D_t   *bufParamsIn1,
                                                              const DSPLIB_bufParams2D_t   *bufParamsOut,
                                                              const DSPLIB_matMul_InitArgs *pKerInitArgs);
template <typename T, typename vec> static inline void writeOutSA1(__vpred tmp, vec *addr, T pOut, vec out)
{
   /* printf("\nOut vector below:\n"); */
   DSPLIB_debugPrintVector(out);
   tmp  = c7x::strm_agen<1, vec>::get_vpred();
   addr = c7x::strm_agen<1, vec>::get_adv(pOut);
   __vstore_pred(tmp, addr, out);
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_matMul_N_unroll_exec_ci(DSPLIB_kernelHandle handle,
                                             void *restrict pIn0,
                                             void *restrict pIn1,
                                             void *restrict pOut)
{
   DSPLIB_matMul_PrivArgs *pKerPrivArgs = (DSPLIB_matMul_PrivArgs *) handle;

   int32_t M       = pKerPrivArgs->M;
   int32_t K       = pKerPrivArgs->K;
   int32_t NBlocks = pKerPrivArgs->NBlocks;

   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_matMul_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<dataType>::type vec;

   int32_t elementCount = c7x::element_count_of<vec>::value;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);

   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);

   // Input samples
   __SE0_OPEN(pIn1, se0Params);
   __SE1_OPEN(((dataType *) pIn1 + elementCount), se1Params);

   // Output samples
   __SA0_OPEN(sa0Params);
   __SA1_OPEN(sa1Params);

   /* vec     c; */
   vec a;
   vec b;
   vec r00, r01, r03, r02, r04, r05, r06, r07;
   vec r08, r09, r0a, r0b, r0c, r0d, r0e, r0f;

   __vpred tmp;
   vec    *addr;
 
      /* #pragma MUST_ITERATE(2, , 2) */
      for (int32_t n = 0; n < M * NBlocks; n++) {
         /* for (int32_t n = 0; n < 1; n++) { */

         r00 = (vec) 0;
         r01 = (vec) 0;
         r02 = (vec) 0;
         r03 = (vec) 0;
         r04 = (vec) 0;
         r05 = (vec) 0;
         r06 = (vec) 0;
         r07 = (vec) 0;
         r08 = (vec) 0;
         r09 = (vec) 0;
         r0a = (vec) 0;
         r0b = (vec) 0;
         r0c = (vec) 0;
         r0d = (vec) 0;
         r0e = (vec) 0;
         r0f = (vec) 0;

         for (int32_t k = 0; k < K; k++) {
            /* printf("Iteration count m = %d, nBlocks= %d,  = %d\n", m, n, k); */
            dataType *addrA = (c7x::strm_agen<0, dataType>::get_adv(pIn0));
            a               = __vload_dup(addrA);

            /* printf("Vector a below:\n"); */
            DSPLIB_debugPrintVector(a);

            b = c7x::strm_eng<0, vec>::get_adv();
            r00 += a * b;

            b = c7x::strm_eng<1, vec>::get_adv();
            r01 += a * b;

            b = c7x::strm_eng<0, vec>::get_adv();
            /* printf("Vector b below:\n"); */
            DSPLIB_debugPrintVector(b);
            r02 += a * b;
            /* printf("Vector r02 below:\n"); */
            DSPLIB_debugPrintVector(r02);

            b = c7x::strm_eng<1, vec>::get_adv();
            r03 += a * b;

            b = c7x::strm_eng<0, vec>::get_adv();
            r04 += a * b;

            b = c7x::strm_eng<1, vec>::get_adv();
            r05 += a * b;

            b = c7x::strm_eng<0, vec>::get_adv();
            r06 += a * b;

            b = c7x::strm_eng<1, vec>::get_adv();
            r07 += a * b;

            b = c7x::strm_eng<0, vec>::get_adv();
            r08 += a * b;

            b = c7x::strm_eng<1, vec>::get_adv();
            r09 += a * b;

            b = c7x::strm_eng<0, vec>::get_adv();
            r0a += a * b;

            b = c7x::strm_eng<1, vec>::get_adv();
            r0b += a * b;

            b = c7x::strm_eng<0, vec>::get_adv();
            r0c += a * b;

            b = c7x::strm_eng<1, vec>::get_adv();
            r0d += a * b;

            b = c7x::strm_eng<0, vec>::get_adv();
            r0e += a * b;

            b = c7x::strm_eng<1, vec>::get_adv();
            r0f += a * b;
         }

         writeOutSA1(tmp, addr, pOut, r00);
         writeOutSA1(tmp, addr, pOut, r01);
         writeOutSA1(tmp, addr, pOut, r02);
         writeOutSA1(tmp, addr, pOut, r03);
         writeOutSA1(tmp, addr, pOut, r04);
         writeOutSA1(tmp, addr, pOut, r05);
         writeOutSA1(tmp, addr, pOut, r06);
         writeOutSA1(tmp, addr, pOut, r07);
         writeOutSA1(tmp, addr, pOut, r08);
         writeOutSA1(tmp, addr, pOut, r09);
         writeOutSA1(tmp, addr, pOut, r0a);
         writeOutSA1(tmp, addr, pOut, r0b);
         writeOutSA1(tmp, addr, pOut, r0c);
         writeOutSA1(tmp, addr, pOut, r0d);
         writeOutSA1(tmp, addr, pOut, r0e);
         writeOutSA1(tmp, addr, pOut, r0f);
      
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_matMul_N_unroll_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                             void *restrict pIn0,
                                                             void *restrict pIn1,
                                                             void *restrict pOut);
template DSPLIB_STATUS DSPLIB_matMul_N_unroll_exec_ci<double>(DSPLIB_kernelHandle handle,
                                                              void *restrict pIn0,
                                                              void *restrict pIn1,
                                                              void *restrict pOut);
