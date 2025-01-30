/******************************************************************************/
/*!
 * \file DSPLIB_fir.c
 */
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

/******************************************************************************
 * Version 1.0  Date 4/13/22      Author: Asheesh Bhardwaj
 *****************************************************************************/

/*******************************************************************************
 *
 * INCLUDES
 *
 ******************************************************************************/

#include "../common/c71/DSPLIB_inlines.h"
#include "DSPLIB_fir_priv.h"

/*******************************************************************************
 *
 * DEFINES
 *
 ******************************************************************************/
#define SE_PARAM_BASE (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SE1_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_SA0_PARAM_OFFSET (SE_SE1_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_SA1_PARAM_OFFSET (SE_SA0_PARAM_OFFSET + SE_PARAM_SIZE)

void DSPLIB_fir_perfEst(DSPLIB_kernelHandle         handle,
                        const DSPLIB_bufParams2D_t *bufParamsIn,
                        const DSPLIB_bufParams1D_t *bufParamsFilter,
                        const DSPLIB_bufParams2D_t *bufParamsOut,
                        uint64_t                   *archCycles,
                        uint64_t                   *estCycles)
{
   return;
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_fir_getSizes(const DSPLIB_fir_InitArgs *pKerInitArgs,
                                  DSPLIB_bufParams2D_t      *bufParamsIn,
                                  DSPLIB_bufParams1D_t      *bufParamsFilter,
                                  DSPLIB_bufParams2D_t      *bufParamsOut)
{
   uint32_t                                               dataSize   = pKerInitArgs->dataSize;
   uint32_t                                               filterSize = pKerInitArgs->filterSize;
   uint32_t                                               computeLoopOutputSize, inputDataSizeRounded;
   typedef typename c7x::make_full_vector<dataType>::type vec;
   uint32_t                                               SE_VECLEN = c7x::se_veclen<vec>::value;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT DSPLIB_fir_getSizes\n");
#endif

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

      /* Number of output points produced in the C7x compute loop for a single
       * channel in the batch */
      computeLoopOutputSize = (dataSize - filterSize + 1);

      inputDataSizeRounded = (dataSize % SE_VECLEN == 0) ? dataSize : (dataSize / SE_VECLEN) + SE_VECLEN;

      bufParamsIn->dim_x = inputDataSizeRounded;

      bufParamsFilter->dim_x = filterSize;
      bufParamsOut->dim_x    = computeLoopOutputSize;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT DSPLIB_fir_getSizes bufParamsOut->dim_x %d\n", bufParamsOut->dim_x);
#endif
   return status;
}

template DSPLIB_STATUS DSPLIB_fir_getSizes<float>(const DSPLIB_fir_InitArgs *pKerInitArgs,
                                                  DSPLIB_bufParams2D_t      *bufParamsIn,
                                                  DSPLIB_bufParams1D_t      *bufParamsFilter,
                                                  DSPLIB_bufParams2D_t      *bufParamsOut);

template DSPLIB_STATUS DSPLIB_fir_getSizes<int16_t>(const DSPLIB_fir_InitArgs *pKerInitArgs,
                                                    DSPLIB_bufParams2D_t      *bufParamsIn,
                                                    DSPLIB_bufParams1D_t      *bufParamsFilter,
                                                    DSPLIB_bufParams2D_t      *bufParamsOut);
template DSPLIB_STATUS DSPLIB_fir_getSizes<int32_t>(const DSPLIB_fir_InitArgs *pKerInitArgs,
                                                    DSPLIB_bufParams2D_t      *bufParamsIn,
                                                    DSPLIB_bufParams1D_t      *bufParamsFilter,
                                                    DSPLIB_bufParams2D_t      *bufParamsOut);
template DSPLIB_STATUS DSPLIB_fir_getSizes<int8_t>(const DSPLIB_fir_InitArgs *pKerInitArgs,
                                                   DSPLIB_bufParams2D_t      *bufParamsIn,
                                                   DSPLIB_bufParams1D_t      *bufParamsFilter,
                                                   DSPLIB_bufParams2D_t      *bufParamsOut);

template <typename dataType>
DSPLIB_STATUS DSPLIB_fir_init_ci(DSPLIB_kernelHandle         handle,
                                 const DSPLIB_bufParams2D_t *bufParamsIn,
                                 const DSPLIB_bufParams1D_t *bufParamsFilter,
                                 const DSPLIB_bufParams2D_t *bufParamsOut,
                                 const DSPLIB_fir_InitArgs  *pKerInitArgs)
{
   DSPLIB_STATUS    status = DSPLIB_SUCCESS;
   __SE_TEMPLATE_v1 se0Params =__gen_SE_TEMPLATE_v1(); 
   __SE_TEMPLATE_v1 se1Params =__gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params =__gen_SA_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa1Params =__gen_SA_TEMPLATE_v1();

   DSPLIB_fir_PrivArgs *pKerPrivArgs       = (DSPLIB_fir_PrivArgs *) handle;
   uint8_t             *pBlock             = pKerPrivArgs->bufPblock;
   uint32_t             dataSize           = pKerPrivArgs->initArgs.dataSize;
   uint32_t             batchSize          = pKerPrivArgs->initArgs.batchSize;
   uint32_t             filterSize         = pKerPrivArgs->initArgs.filterSize;
   uint32_t             dataBufferInPitch  = pKerPrivArgs->dataBufferInPitch;
   uint32_t             dataBufferOutPitch = pKerPrivArgs->dataBufferOutPitch;
   int32_t              outputSize         = pKerPrivArgs->outputSize;

   int32_t filterOffset, dataInOffset;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<vec>::value;
   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_ELEDUP  SE_ELEDUP  = __SE_ELEDUP_OFF;
   int      eleCount = c7x::element_count_of<vec>::value;
   uint32_t flag;
   if (dataSize >= eleCount * 16 && filterSize <= (dataSize - 16 * eleCount + 1)) {
      flag = 1;
   }
   else {
      flag = 0;
   }
   pKerPrivArgs->flag = flag;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT SE_VECLEN: %d, SA_VECLEN: %d, SE_ELETYPE: %d "
          "batchSize %d outputSize %d\n",
          SE_VECLEN, SA_VECLEN, SE_ELETYPE, batchSize, outputSize);
#endif

   if (eleCount == 8) {
      SE_ELEDUP = __SE_ELEDUP_8X;
   }
   else if (eleCount == 16) {
      SE_ELEDUP = __SE_ELEDUP_16X;
   }
   else if (eleCount == 32) {
      SE_ELEDUP = __SE_ELEDUP_32X;
   }
   else if (eleCount == 64) {
      SE_ELEDUP = __SE_ELEDUP_64X;
   }
   else {
   }

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT SE_VECLEN: %d, SA_VECLEN: %d, SE_ELETYPE: %d "
          "SE_ELEDUP %d eleCount %d\n",
          SE_VECLEN, SA_VECLEN, SE_ELETYPE, SE_ELEDUP, eleCount);
#endif
   int cntFilter = filterSize / c7x::element_count_of<vec>::value;
   cntFilter += (int32_t)(filterSize % c7x::element_count_of<vec>::value > (int32_t)0);

   int32_t dataLoop    = 0;
   int32_t blkElements = 16 * eleCount;

   if (flag == 1) {
      dataLoop = (outputSize % blkElements == 0) ? (outputSize / blkElements) : ((outputSize / blkElements) + 1);
      dataLoop = (dataLoop == 0) ? 1 : dataLoop;
   }
   else {
   }

   pKerPrivArgs->dataLoop = dataLoop;
#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT dataLoop: %d (dataSize - filterSize) %d 16*eleCount "
          "%d blkElements %d ((dataSize - filterSize) / blkElements) %d\n",
          dataLoop, (dataSize - filterSize), 16 * eleCount, blkElements, ((dataSize - filterSize) / blkElements));
#endif

      int32_t icntParam = (outputSize % eleCount == 0) ? (outputSize / eleCount) : (outputSize / eleCount) + 1;
      icntParam         = (icntParam == 0) ? 1 : icntParam;

      if (flag == 0) {
         /**********************************************************************/
         /* Prepare streaming engine 1 to fetch the filter                     */
         /**********************************************************************/
         se1Params = __gen_SE_TEMPLATE_v1();

         se1Params.ICNT0 = 1;
         se1Params.ICNT1 = filterSize;
         se1Params.DIM1  = -(int32_t) 1;
         se1Params.ICNT2 = icntParam;
         se1Params.DIM2  = 0;
         se1Params.ICNT3 = batchSize;
         se1Params.DIM3  = 0;

         se1Params.ELETYPE = SE_ELETYPE;
         se1Params.VECLEN  = SE_VECLEN;
         se1Params.ELEDUP  = SE_ELEDUP;
         se1Params.DIMFMT  = __SE_DIMFMT_4D;
         se1Params.DIR     = __SE_DIR_DEC;
         //      se1Params.DECDIM1_WIDTH = filterSize;
         //      se1Params.DECDIM1       = __SE_DECDIM_DIM1;
      }
      else {
         /*********************************************
          * Prepare SA template to read filter values *
          *********************************************/
         sa1Params       = __gen_SA_TEMPLATE_v1(); // Sa parameter vector
         sa1Params.ICNT0 = (uint32_t) 1;
         sa1Params.ICNT1 = (uint32_t) filterSize;
         sa1Params.DIM1  = -(int32_t) 1;
         sa1Params.ICNT2 = dataLoop;
         sa1Params.DIM2  = 0;
         sa1Params.ICNT3 = batchSize;
         sa1Params.DIM3  = 0;

         sa1Params.DIMFMT = __SA_DIMFMT_4D;
         sa1Params.VECLEN = __SA_VECLEN_1ELEM;
      }
      /**********************************************************************/
      /* Prepare streaming engine 0 to fetch data                           */
      /**********************************************************************/
      //      int32_t filterCnt0;
      //      int32_t filterCnt1;
      //      int32_t filterDecDim0;
      //      int32_t filterDecDim1;

      if (flag == 0) {
         se0Params.ICNT0   = eleCount > filterSize ? filterSize : eleCount;
         se0Params.ICNT1   = filterSize;
         se0Params.DIM1    = (int32_t) 1;
         se0Params.ICNT2   = icntParam;
         se0Params.DIM2    = eleCount;
         se0Params.ICNT3   = batchSize;
         se0Params.DIM3    = dataBufferInPitch;
         se0Params.ELETYPE = SE_ELETYPE;
         se0Params.VECLEN  = SE_VECLEN;
         se0Params.DIMFMT  = __SE_DIMFMT_4D;
      }
      else {
         se0Params.ICNT0   = eleCount;
         se0Params.ICNT1   = 8;
         se0Params.DIM1    = (int32_t) eleCount;
         se0Params.ICNT2   = filterSize;
         se0Params.DIM2    = (int32_t) 1;
         se0Params.ICNT3   = dataLoop;
         se0Params.DIM3    = 16 * eleCount;
         se0Params.ICNT4   = batchSize;
         se0Params.DIM4    = dataBufferInPitch;
         se0Params.ELETYPE = SE_ELETYPE;
         se0Params.VECLEN  = SE_VECLEN;
         se0Params.DIMFMT  = __SE_DIMFMT_5D;
      }

      /**********************************************************************/
      /* Prepare SA template to store output                                */
      /**********************************************************************/
      if (flag == 0) {
         sa0Params.ICNT0 = eleCount > filterSize ? filterSize : eleCount;
         sa0Params.ICNT1 = icntParam;
         sa0Params.DIM1  = (int32_t) eleCount;

         /* Overwrite the data for convenience of loop structure */
         sa0Params.ICNT2         = batchSize;
         sa0Params.DIM2          = dataBufferOutPitch;
         sa0Params.DECDIM1_WIDTH = outputSize;
         sa0Params.DECDIM1       = __SA_DECDIM_DIM1;
         sa0Params.VECLEN        = SA_VECLEN;
         sa0Params.DIMFMT        = __SA_DIMFMT_3D;
      }
      else {
         sa0Params.ICNT0 = eleCount > filterSize ? filterSize : eleCount;
         sa0Params.ICNT1 = 16;
         sa0Params.DIM1  = (int32_t) eleCount;
         sa0Params.ICNT2 = dataLoop;
         sa0Params.DIM2  = (int32_t) 16 * eleCount;

         /* Overwrite the data for convenience of loop structure */
         sa0Params.ICNT3         = batchSize;
         sa0Params.DIM3          = dataBufferOutPitch;
         sa0Params.DECDIM1_WIDTH = outputSize;
         sa0Params.DECDIM1       = __SA_DECDIM_DIM2;
         sa0Params.VECLEN        = SA_VECLEN;
         sa0Params.DIMFMT        = __SA_DIMFMT_4D;
      }

      *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
      *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET) = se1Params;
      *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
      if (flag == 1) {
         *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET) = sa1Params;
      }
      else {
   }

   return status;
}

template DSPLIB_STATUS DSPLIB_fir_init_ci<int16_t>(DSPLIB_kernelHandle         handle,
                                                   const DSPLIB_bufParams2D_t *bufParamsIn,
                                                   const DSPLIB_bufParams1D_t *bufParamsFilter,
                                                   const DSPLIB_bufParams2D_t *bufParamsOut,
                                                   const DSPLIB_fir_InitArgs  *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_fir_init_ci<int32_t>(DSPLIB_kernelHandle         handle,
                                                   const DSPLIB_bufParams2D_t *bufParamsIn,
                                                   const DSPLIB_bufParams1D_t *bufParamsFilter,
                                                   const DSPLIB_bufParams2D_t *bufParamsOut,
                                                   const DSPLIB_fir_InitArgs  *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_fir_init_ci<int8_t>(DSPLIB_kernelHandle         handle,
                                                  const DSPLIB_bufParams2D_t *bufParamsIn,
                                                  const DSPLIB_bufParams1D_t *bufParamsFilter,
                                                  const DSPLIB_bufParams2D_t *bufParamsOut,
                                                  const DSPLIB_fir_InitArgs  *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_fir_init_ci<float>(DSPLIB_kernelHandle         handle,
                                                 const DSPLIB_bufParams2D_t *bufParamsIn,
                                                 const DSPLIB_bufParams1D_t *bufParamsFilter,
                                                 const DSPLIB_bufParams2D_t *bufParamsOut,
                                                 const DSPLIB_fir_InitArgs  *pKerInitArgs);
/*******************************************************************************
 *
 * EXECUTION FUNCTIONS
 *
 ******************************************************************************/

template <typename dataType>
DSPLIB_STATUS
DSPLIB_fir_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut)
{
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;
   __SE_TEMPLATE_v1 se2Params;

   DSPLIB_fir_PrivArgs *pKerPrivArgs = (DSPLIB_fir_PrivArgs *) handle;
   uint8_t             *pBlock       = pKerPrivArgs->bufPblock;
   uint32_t             batchSize    = pKerPrivArgs->initArgs.batchSize;
   uint32_t             filterSize   = pKerPrivArgs->initArgs.filterSize;
   uint32_t             flag         = pKerPrivArgs->flag;
   int32_t              outputSize   = pKerPrivArgs->outputSize;
   int32_t              loopCount;
   int32_t              filterOffset, dataInOffset;

   dataType *pInLocal     = (dataType *) pIn;
   dataType *pFilterLocal = (dataType *) pFilter;
   dataType *pOutLocal    = (dataType *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_fir_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<dataType>::type vec;
   // typedef vec* vec_ptr;
   vec *vec_ptr;
   int  eleCount = c7x::element_count_of<vec>::value;

   vec input;
   vec filter;
   vec accumulator_0, accumulator_1, accumulator_2, accumulator_3;
   vec accumulator_x, accumulator_y;

   vec accumulator_0_0;
   vec accumulator_0_1;
   vec accumulator_0_2;
   vec accumulator_0_3;
   vec accumulator_0_4;
   vec accumulator_0_5;
   vec accumulator_0_6;
   vec accumulator_0_7;
   vec accumulator_1_0;
   vec accumulator_1_1;
   vec accumulator_1_2;
   vec accumulator_1_3;
   vec accumulator_1_4;
   vec accumulator_1_5;
   vec accumulator_1_6;
   vec accumulator_1_7;

#if DSPLIB_DEBUGPRINT
   printf("Enter pFilter %p\n", pFilter);
#endif

   if (flag == 1) {
      pFilterLocal = pFilterLocal + filterSize - 1;
   }
   else {
      pFilterLocal = pFilterLocal + filterSize;
   }

#if DSPLIB_DEBUGPRINT
   printf("Enter pFilter %p\n", pFilterLocal);
#endif

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SE0_OPEN(pInLocal, se0Params);

   if (flag == 0) {
      se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
      __SE1_OPEN(pFilterLocal, se1Params);
   }
   else {

      pInLocal  = pInLocal + +8 * eleCount;
      se2Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
      __SE1_OPEN(pInLocal, se2Params);

      // Filter coefficient
      sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);
      __SA1_OPEN(sa1Params);
   }

   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA0_OPEN(sa0Params);

   int cntFilter = filterSize / c7x::element_count_of<vec>::value;
   cntFilter += (int32_t)(filterSize % c7x::element_count_of<vec>::value > (int32_t)0);

   int cntData = (outputSize) / c7x::element_count_of<vec>::value;
   cntData += (int32_t)((outputSize) % c7x::element_count_of<vec>::value > (int32_t)0);

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT dataSize %d filterSize %d cntData %d\n", pKerPrivArgs->initArgs.dataSize, filterSize, cntData);
#endif

   vec *output_ptr;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT cntFilter %d  cntData %d\n", cntFilter, cntData);
#endif

   filterSize = (filterSize % 4 == 0) ? filterSize : ((filterSize / 4) * 4 + 4);

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT filterSize %d\n", filterSize);
#endif

   if (flag == 0) {
      for (int batchSizeCount = 0; batchSizeCount < batchSize; batchSizeCount++) {
         for (int dataLoopCount = 0; dataLoopCount < cntData; dataLoopCount++) {
            accumulator_0 = (vec) 0;
            accumulator_1 = (vec) 0;
            accumulator_2 = (vec) 0;
            accumulator_3 = (vec) 0;

            for (int filterLoopCount = 0; filterLoopCount < (filterSize >> 2); filterLoopCount++) {
               vec tmp0 = c7x::strm_eng<0, vec>::get_adv();
               vec tmp1 = c7x::strm_eng<1, vec>::get_adv();
#if DSPLIB_DEBUGPRINT
//               DSPLIB_debugPrintVector(tmp0);
//               DSPLIB_debugPrintVector(tmp1);
#endif
               accumulator_0 += tmp0 * tmp1;

               vec tmp2 = c7x::strm_eng<0, vec>::get_adv();
               vec tmp3 = c7x::strm_eng<1, vec>::get_adv();
               accumulator_1 += tmp2 * tmp3;

               vec tmp4 = c7x::strm_eng<0, vec>::get_adv();
               vec tmp5 = c7x::strm_eng<1, vec>::get_adv();
               accumulator_2 += tmp4 * tmp5;

               vec tmp6 = c7x::strm_eng<0, vec>::get_adv();
               vec tmp7 = c7x::strm_eng<1, vec>::get_adv();
               accumulator_3 += tmp6 * tmp7;
            }
            accumulator_x = accumulator_0 + accumulator_1;
            accumulator_y = accumulator_2 + accumulator_3;
            accumulator_x = accumulator_x + accumulator_y;
            __vpred tmp   = c7x::strm_agen<0, vec>::get_vpred();
            vec    *VB1   = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
            __vstore_pred(tmp, VB1, accumulator_x);

#if DSPLIB_DEBUGPRINT
            DSPLIB_debugPrintVector(accumulator_x);
#endif
         }
      }
   }
   else {
      cntData = pKerPrivArgs->dataLoop;
      for (int batchSizeCount = 0; batchSizeCount < batchSize; batchSizeCount++) {
         for (int dataLoopCount = 0; dataLoopCount < cntData; dataLoopCount++) {
            accumulator_0_0 = (vec) 0;
            accumulator_0_1 = (vec) 0;
            accumulator_0_2 = (vec) 0;
            accumulator_0_3 = (vec) 0;
            accumulator_0_4 = (vec) 0;
            accumulator_0_5 = (vec) 0;
            accumulator_0_6 = (vec) 0;
            accumulator_0_7 = (vec) 0;
            accumulator_1_0 = (vec) 0;
            accumulator_1_1 = (vec) 0;
            accumulator_1_2 = (vec) 0;
            accumulator_1_3 = (vec) 0;
            accumulator_1_4 = (vec) 0;
            accumulator_1_5 = (vec) 0;
            accumulator_1_6 = (vec) 0;
            accumulator_1_7 = (vec) 0;

            for (int filterLoopCount = 0; filterLoopCount < (filterSize); filterLoopCount++) {
               dataType *VB1Dup = c7x::strm_agen<1, dataType>::get_adv(pFilterLocal);
               vec       flt = __vload_dup(VB1Dup);

               vec tmp0_0 = c7x::strm_eng<0, vec>::get_adv();
               accumulator_0_0 += tmp0_0 * flt;

               vec tmp0_1 = c7x::strm_eng<0, vec>::get_adv();
               accumulator_0_1 += tmp0_1 * flt;

               vec tmp0_2 = c7x::strm_eng<0, vec>::get_adv();
               accumulator_0_2 += tmp0_2 * flt;

               vec tmp0_3 = c7x::strm_eng<0, vec>::get_adv();
               accumulator_0_3 += tmp0_3 * flt;

               vec tmp0_4 = c7x::strm_eng<0, vec>::get_adv();
               accumulator_0_4 += tmp0_4 * flt;

               vec tmp0_5 = c7x::strm_eng<0, vec>::get_adv();
               accumulator_0_5 += tmp0_5 * flt;

               vec tmp0_6 = c7x::strm_eng<0, vec>::get_adv();
               accumulator_0_6 += tmp0_6 * flt;

               vec tmp0_7 = c7x::strm_eng<0, vec>::get_adv();
               accumulator_0_7 += tmp0_7 * flt;

               vec tmp1_0 = c7x::strm_eng<1, vec>::get_adv();
               accumulator_1_0 += tmp1_0 * flt;

               vec tmp1_1 = c7x::strm_eng<1, vec>::get_adv();
               accumulator_1_1 += tmp1_1 * flt;

               vec tmp1_2 = c7x::strm_eng<1, vec>::get_adv();
               accumulator_1_2 += tmp1_2 * flt;

               vec tmp1_3 = c7x::strm_eng<1, vec>::get_adv();
               accumulator_1_3 += tmp1_3 * flt;

               vec tmp1_4 = c7x::strm_eng<1, vec>::get_adv();
               accumulator_1_4 += tmp1_4 * flt;

               vec tmp1_5 = c7x::strm_eng<1, vec>::get_adv();
               accumulator_1_5 += tmp1_5 * flt;

               vec tmp1_6 = c7x::strm_eng<1, vec>::get_adv();
               accumulator_1_6 += tmp1_6 * flt;

               vec tmp1_7 = c7x::strm_eng<1, vec>::get_adv();
               accumulator_1_7 += tmp1_7 * flt;
#if DSPLIB_DEBUGPRINT
//                DSPLIB_debugPrintVector(flt);
//                DSPLIB_debugPrintVector(tmp0_0);
//                DSPLIB_debugPrintVector(tmp0_1);
//                DSPLIB_debugPrintVector(tmp0_2);
//                DSPLIB_debugPrintVector(tmp0_3);
//                DSPLIB_debugPrintVector(tmp0_4);
//                DSPLIB_debugPrintVector(tmp0_5);
//                DSPLIB_debugPrintVector(tmp0_6);
//                DSPLIB_debugPrintVector(tmp0_7);
//                DSPLIB_debugPrintVector(tmp1_0);
//                DSPLIB_debugPrintVector(tmp0_1);
//                DSPLIB_debugPrintVector(tmp1_2);
//                DSPLIB_debugPrintVector(tmp1_3);
//                DSPLIB_debugPrintVector(tmp1_4);
//                DSPLIB_debugPrintVector(tmp1_5);
//                DSPLIB_debugPrintVector(tmp1_6);
//                DSPLIB_debugPrintVector(tmp1_7);
#endif
            }
            __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
            vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
            __vstore_pred(tmp, VB1, accumulator_0_0);

            tmp = c7x::strm_agen<0, vec>::get_vpred();
            VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
            __vstore_pred(tmp, VB1, accumulator_0_1);

            tmp = c7x::strm_agen<0, vec>::get_vpred();
            VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
            __vstore_pred(tmp, VB1, accumulator_0_2);

            tmp = c7x::strm_agen<0, vec>::get_vpred();
            VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
            __vstore_pred(tmp, VB1, accumulator_0_3);

            tmp = c7x::strm_agen<0, vec>::get_vpred();
            VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
            __vstore_pred(tmp, VB1, accumulator_0_4);

            tmp = c7x::strm_agen<0, vec>::get_vpred();
            VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
            __vstore_pred(tmp, VB1, accumulator_0_5);

            tmp = c7x::strm_agen<0, vec>::get_vpred();
            VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
            __vstore_pred(tmp, VB1, accumulator_0_6);

            tmp = c7x::strm_agen<0, vec>::get_vpred();
            VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
            __vstore_pred(tmp, VB1, accumulator_0_7);

            tmp = c7x::strm_agen<0, vec>::get_vpred();
            VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
            __vstore_pred(tmp, VB1, accumulator_1_0);

            tmp = c7x::strm_agen<0, vec>::get_vpred();
            VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
            __vstore_pred(tmp, VB1, accumulator_1_1);

            tmp = c7x::strm_agen<0, vec>::get_vpred();
            VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
            __vstore_pred(tmp, VB1, accumulator_1_2);

            tmp = c7x::strm_agen<0, vec>::get_vpred();
            VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
            __vstore_pred(tmp, VB1, accumulator_1_3);

            tmp = c7x::strm_agen<0, vec>::get_vpred();
            VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
            __vstore_pred(tmp, VB1, accumulator_1_4);

            tmp = c7x::strm_agen<0, vec>::get_vpred();
            VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
            __vstore_pred(tmp, VB1, accumulator_1_5);

            tmp = c7x::strm_agen<0, vec>::get_vpred();
            VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
            __vstore_pred(tmp, VB1, accumulator_1_6);

            tmp = c7x::strm_agen<0, vec>::get_vpred();
            VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
            __vstore_pred(tmp, VB1, accumulator_1_7);

#if DSPLIB_DEBUGPRINT
            //             DSPLIB_debugPrintVector(accumulator_0_0);
            //             DSPLIB_debugPrintVector(accumulator_0_1);
            //             DSPLIB_debugPrintVector(accumulator_0_2);
            //             DSPLIB_debugPrintVector(accumulator_0_3);
            //             DSPLIB_debugPrintVector(accumulator_0_4);
            //             DSPLIB_debugPrintVector(accumulator_0_5);
            //             DSPLIB_debugPrintVector(accumulator_0_6);
            //             DSPLIB_debugPrintVector(accumulator_0_7);
            //
            //             DSPLIB_debugPrintVector(accumulator_1_0);
            //             DSPLIB_debugPrintVector(accumulator_1_1);
            //             DSPLIB_debugPrintVector(accumulator_1_2);
            //             DSPLIB_debugPrintVector(accumulator_1_3);
            //             DSPLIB_debugPrintVector(accumulator_1_4);
            //             DSPLIB_debugPrintVector(accumulator_1_5);
            //             DSPLIB_debugPrintVector(accumulator_1_6);
            //             DSPLIB_debugPrintVector(accumulator_1_7);
#endif
         }
      }
   }
   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_fir_exec_ci<int16_t>(DSPLIB_kernelHandle handle,
                                                   void *restrict pIn,
                                                   void *restrict pFilter,
                                                   void *restrict pOut);

template DSPLIB_STATUS DSPLIB_fir_exec_ci<int32_t>(DSPLIB_kernelHandle handle,
                                                   void *restrict pIn,
                                                   void *restrict pFilter,
                                                   void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_fir_exec_ci<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_fir_exec_ci<float>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut);
