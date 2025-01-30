/******************************************************************************/
/*!
 * \file DSPLIB_cascadeBiquad.c
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
#include "DSPLIB_cascadeBiquad_priv.h"

template <typename dataType>
DSPLIB_STATUS DSPLIB_cascadeBiquad7Stage_init_ci(DSPLIB_kernelHandle                  handle,
                                                 const DSPLIB_bufParams2D_t          *bufParamsIn,
                                                 const DSPLIB_bufParams1D_t          *bufParamsFilterCoeff,
                                                 const DSPLIB_bufParams2D_t          *bufParamsFilterVar,
                                                 const DSPLIB_bufParams2D_t          *bufParamsOut,
                                                 const DSPLIB_cascadeBiquad_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS    status = DSPLIB_SUCCESS;
   __SE_TEMPLATE_v1 se1Params;

   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;
   __SA_TEMPLATE_v1 sa2Params;
   __SA_TEMPLATE_v1 sa3Params;

   DSPLIB_cascadeBiquad_PrivArgs *pKerPrivArgs       = (DSPLIB_cascadeBiquad_PrivArgs *) handle;
   uint8_t                       *pBlock             = pKerPrivArgs->bufPblock;
   uint32_t                       dataSize           = pKerPrivArgs->initArgs.dataSize;
   uint32_t                       numChannels        = pKerPrivArgs->initArgs.numChannels;
   uint32_t                       numStages          = pKerPrivArgs->initArgs.numStages;
   uint32_t                       dataBufferInPitch  = pKerPrivArgs->dataBufferInPitch;
   uint32_t                       dataBufferOutPitch = pKerPrivArgs->dataBufferOutPitch;
   uint32_t                       filterVarPitch     = pKerPrivArgs->filterVarPitch;

   __SE_ELEDUP  SE_ELEDUP =  __SE_ELEDUP_OFF;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<vec>::value;
   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec>::value;

   uint32_t eleCount = c7x::element_count_of<vec>::value;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT Enter DSPLIB_cascadeBiquad7Stage_init_ci\n");
   printf("DSPLIB_DEBUGPRINT SE_VECLEN: %d, SA_VECLEN: %d, SE_ELETYPE: %d numChannels %d numStages %d\n", SE_VECLEN,
          SA_VECLEN, SE_ELETYPE, numChannels, numStages);
#endif

  //  if (eleCount == 16) {
      SE_ELEDUP = __SE_ELEDUP_16X;
  //  }
  //  else {
  //   /* Do nothing here */
  //  }

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT SE_VECLEN: %d, SA_VECLEN: %d, SE_ELETYPE: %d SE_ELEDUP %d eleCount %d __SE_ELEDUP_8X %d\n",
          SE_VECLEN, SA_VECLEN, SE_ELETYPE, SE_ELEDUP, eleCount, __SE_ELEDUP_8X);
#endif

      uint32_t icnt1Param = (numChannels > eleCount) ? eleCount : numChannels;
      icnt1Param          = (icnt1Param == 0) ? 1 : icnt1Param;
      uint32_t icnt2Param =
          (numChannels % eleCount == 0) ? (numChannels / eleCount) : (numChannels + eleCount) / eleCount;

      /**********************************************************************/
      /* Prepare streaming engine 1 to fetch the coefficients               */
      /**********************************************************************/
      se1Params = __gen_SE_TEMPLATE_v1();

      se1Params.ICNT0         = 1;
      se1Params.ICNT1         = numStages * 2;
      se1Params.DIM1          = (int32_t) 1;
      se1Params.ICNT2         = dataSize;
      se1Params.DIM2          = 0;
      se1Params.ICNT3         = icnt2Param;
      se1Params.DIM3          = 0;
      se1Params.ELETYPE       = SE_ELETYPE;
      se1Params.VECLEN        = SE_VECLEN;
      se1Params.DECDIM1_WIDTH = numStages * 2;
      se1Params.DECDIM1       = __SE_DECDIM_DIM1;
      se1Params.ELEDUP        = SE_ELEDUP;
      se1Params.DIMFMT        = __SE_DIMFMT_4D;

      /**********************************************************************/
      /* Prepare streaming address generator 2 to fetch the input                      */
      /**********************************************************************/
      sa2Params = __gen_SA_TEMPLATE_v1();

      sa2Params.ICNT0         = icnt1Param;
      sa2Params.ICNT1         = dataSize;
      sa2Params.DIM1          = (int32_t) dataBufferInPitch;
      sa2Params.ICNT2         = icnt2Param;
      sa2Params.DIM2          = eleCount;
      sa2Params.VECLEN        = SA_VECLEN;
      sa2Params.DECDIM1_WIDTH = numChannels;
      sa2Params.DECDIM1       = __SA_DECDIM_DIM2;
      sa2Params.DIMFMT        = __SA_DIMFMT_3D;

      /**********************************************************************/
      /* Prepare streaming address 3 to fetch filter variables               */
      /**********************************************************************/
      sa3Params = __gen_SA_TEMPLATE_v1();

      sa3Params.ICNT0         = icnt1Param;
      sa3Params.ICNT1         = numStages * 2;
      sa3Params.DIM1          = (int32_t) filterVarPitch;
      sa3Params.ICNT2         = icnt2Param;
      sa3Params.DIM2          = eleCount;
      sa3Params.VECLEN        = SA_VECLEN;
      sa3Params.DECDIM1_WIDTH = numChannels;
      sa3Params.DECDIM1       = __SA_DECDIM_DIM2;
      sa3Params.DIMFMT        = __SA_DIMFMT_3D;

      /**********************************************************************/
      /* Prepare SA template to store output                                */
      /**********************************************************************/
      sa0Params = __gen_SA_TEMPLATE_v1();

      sa0Params.ICNT0         = icnt1Param;
      sa0Params.ICNT1         = dataSize;
      sa0Params.DIM1          = (int32_t) dataBufferOutPitch;
      sa0Params.ICNT2         = icnt2Param;
      sa0Params.DIM2          = eleCount;
      sa0Params.DECDIM1_WIDTH = numChannels;
      sa0Params.DECDIM1       = __SA_DECDIM_DIM2;
      sa0Params.DECDIM2_WIDTH = dataBufferOutPitch * dataSize;
      sa0Params.DECDIM2       = __SA_DECDIM_DIM2;
      sa0Params.VECLEN        = SA_VECLEN;
      sa0Params.DIMFMT        = __SA_DIMFMT_3D;

      /**********************************************************************/
      /* Prepare SA template to store filter variables                      */
      /**********************************************************************/
      sa1Params = __gen_SA_TEMPLATE_v1();

      sa1Params.ICNT0         = icnt1Param;
      sa1Params.ICNT1         = numStages * 2;
      sa1Params.DIM1          = (int32_t) filterVarPitch;
      sa1Params.ICNT2         = icnt2Param;
      sa1Params.DIM2          = eleCount;
      sa1Params.DECDIM1_WIDTH = numChannels;
      sa1Params.DECDIM1       = __SA_DECDIM_DIM2;
      sa1Params.VECLEN        = SA_VECLEN;
      sa1Params.DIMFMT        = __SA_DIMFMT_3D;

      *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET) = se1Params;
      *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
      *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET) = sa1Params;
      *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA2_PARAM_OFFSET) = sa2Params;
      *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA3_PARAM_OFFSET) = sa3Params;

   return status;
}


template DSPLIB_STATUS DSPLIB_cascadeBiquad7Stage_init_ci<float>(DSPLIB_kernelHandle         handle,
                                                                 const DSPLIB_bufParams2D_t *bufParamsIn,
                                                                 const DSPLIB_bufParams1D_t *bufParamsFilterCoeff,
                                                                 const DSPLIB_bufParams2D_t *bufParamsFilterVar,
                                                                 const DSPLIB_bufParams2D_t *bufParamsOut,
                                                                 const DSPLIB_cascadeBiquad_InitArgs *pKerInitArgs);
/*******************************************************************************
 *
 * EXECUTION FUNCTIONS
 *
 ******************************************************************************/

template <typename dataType>
DSPLIB_STATUS DSPLIB_cascadeBiquad7Stage_exec_ci(DSPLIB_kernelHandle handle,
                                                 void *restrict pIn,
                                                 void *restrict pFilterCoeff,
                                                 void *restrict pFilterVar,
                                                 void *restrict pOut)
{
   __SE_TEMPLATE_v1 se0Params;
   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;
   __SA_TEMPLATE_v1 sa2Params;
   __SA_TEMPLATE_v1 sa3Params;

   DSPLIB_cascadeBiquad_PrivArgs *pKerPrivArgs = (DSPLIB_cascadeBiquad_PrivArgs *) handle;
   uint8_t                       *pBlock       = pKerPrivArgs->bufPblock;
   uint32_t                       dataSize     = pKerPrivArgs->initArgs.dataSize;
   //   uint32_t batchSize    = pKerPrivArgs->initArgs.batchSize;
   uint32_t numStages   = pKerPrivArgs->initArgs.numStages;
   uint32_t numChannels = pKerPrivArgs->initArgs.numChannels;

   dataType *pInLocal          = (dataType *) pIn;
   dataType *pFilterCoeffLocal = (dataType *) pFilterCoeff;
   dataType *pFilterVarLocal   = (dataType *) pFilterVar;
   dataType *pOutLocal         = (dataType *) pOut;
//   dataType outGain            = pKerPrivArgs->outGain;
#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_cascadeBiquad7Stage_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<dataType>::type vec;
   //   typedef vec* vec_ptr;
   int eleCount = c7x::element_count_of<vec>::value;

   vec input;
   vec filter;
   vec outGain = (vec) (pKerPrivArgs->outGain);

#if DSPLIB_DEBUGPRINT
   printf("Enter pFilterCoeffLocal %p pFilterVarLocal %p\n", pFilterCoeffLocal, pFilterVarLocal);
#endif

   // Filter coefficients
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   __SE1_OPEN(pFilterCoeffLocal, se0Params);
   int32_t offset = ((numStages * 5 + eleCount) / eleCount) * eleCount;
   __SE0_OPEN(pFilterCoeffLocal + offset, se0Params);

   // Output samples
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA0_OPEN(sa0Params);

   // Filter variables write
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);
   __SA1_OPEN(sa1Params);

   // Input samples
   sa2Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA2_PARAM_OFFSET);
   __SA2_OPEN(sa2Params);

   // Filter variables read
   sa3Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA3_PARAM_OFFSET);
   __SA3_OPEN(sa3Params);

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT dataSize %d numChannels %d numStages %d pFilterVarLocal %p\n", dataSize, numChannels,
          numStages, pFilterVarLocal);
#endif

   vec output1, output2, output3;
   vec output4, output5, output6, output7, output8, output9;
   vec temp1_mul_1, temp0_mul_1, d0_temp_1, d1_mul_1;
   vec temp1_mul_2, temp0_mul_2, d0_temp_2, d1_mul_2;
   vec temp1_mul_3, temp0_mul_3, d0_temp_3, d1_mul_3;
   vec temp1_mul_4, temp0_mul_4, d0_temp_4, d1_mul_4;
   vec temp1_mul_5, temp0_mul_5, d0_temp_5, d1_mul_5;
   vec temp1_mul_6, temp0_mul_6, d0_temp_6, d1_mul_6;
   vec temp1_mul_7, temp0_mul_7, d0_temp_7, d1_mul_7;
   vec temp1_mul_8, temp0_mul_8, d0_temp_8, d1_mul_8;
   vec temp1_mul_9, temp0_mul_9, d0_temp_9, d1_mul_9;
   vec temp1_add_1, temp1_add_2, temp1_add_3;
   vec temp1_add_4, temp1_add_5, temp1_add_6, temp1_add_7, temp1_add_8, temp1_add_9;

   vec b11_a11, b21_a21, b31_a31, b41_a41, b51_a51, b61_a61, b71_a71, b81_a81, b91_a91;
   vec b12_a12, b22_a22, b32_a32, b42_a42, b52_a52, b62_a62, b72_a72, b82_a82, b92_a92;
   vec a11, a12, a22, a21, a31, a32, a41, a42, a51, a52, a61, a62, a71, a72, a81, a82, a91, a92;

#if DSPLIB_DEBUGPRINT
//      DSPLIB_debugPrintVector(b10);
//      DSPLIB_debugPrintVector(b11);
//      DSPLIB_debugPrintVector(b12);
//      DSPLIB_debugPrintVector(b20);
//      DSPLIB_debugPrintVector(b21);
//      DSPLIB_debugPrintVector(b22);
//      DSPLIB_debugPrintVector(b30);
//      DSPLIB_debugPrintVector(b31);
//      DSPLIB_debugPrintVector(b32);
#endif

   for (uint32_t chCount = 0; chCount < numChannels; chCount += eleCount) {
      // Read filter variables
      __vpred filtVars      = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      vec    *VB4           = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec     filtVars_d0_1 = __vload_pred(filtVars, VB4);
      filtVars              = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4                   = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_1     = __vload_pred(filtVars, VB4);

      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d0_2 = __vload_pred(filtVars, VB4);
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_2 = __vload_pred(filtVars, VB4);

      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d0_3 = __vload_pred(filtVars, VB4);
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_3 = __vload_pred(filtVars, VB4);

      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d0_4 = __vload_pred(filtVars, VB4);
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_4 = __vload_pred(filtVars, VB4);

      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d0_5 = __vload_pred(filtVars, VB4);
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_5 = __vload_pred(filtVars, VB4);

      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d0_6 = __vload_pred(filtVars, VB4);
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_6 = __vload_pred(filtVars, VB4);

      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d0_7 = __vload_pred(filtVars, VB4);
      filtVars          = c7x::strm_agen<3, c7x::float_vec>::get_vpred();
      VB4               = c7x::strm_agen<3, c7x::float_vec>::get_adv(pFilterVarLocal);
      vec filtVars_d1_7 = __vload_pred(filtVars, VB4);

#if DSPLIB_DEBUGPRINT
      DSPLIB_debugPrintVector(filtVars_d0_1);
      DSPLIB_debugPrintVector(filtVars_d1_1);
      DSPLIB_debugPrintVector(filtVars_d0_2);
      DSPLIB_debugPrintVector(filtVars_d1_2);
      DSPLIB_debugPrintVector(filtVars_d0_3);
      DSPLIB_debugPrintVector(filtVars_d1_3);
      DSPLIB_debugPrintVector(filtVars_d0_4);
      DSPLIB_debugPrintVector(filtVars_d1_4);
      DSPLIB_debugPrintVector(filtVars_d0_5);
      DSPLIB_debugPrintVector(filtVars_d1_5);
      DSPLIB_debugPrintVector(filtVars_d0_6);
      DSPLIB_debugPrintVector(filtVars_d1_6);
      DSPLIB_debugPrintVector(filtVars_d0_7);
      DSPLIB_debugPrintVector(filtVars_d1_7);
#endif
      for (uint32_t samp = 0; samp < dataSize; samp++) {
         // vec tmp1 = c7x::strm_eng<1, vec>::get();
         __vpred         tmpld = c7x::strm_agen<2, c7x::float_vec>::get_vpred();
         c7x::float_vec *VB3   = c7x::strm_agen<2, c7x::float_vec>::get_adv(pInLocal);
         c7x::float_vec  tmp2  = __vload_pred(tmpld, VB3);

         output1 = tmp2 + filtVars_d0_1; //   output01 = input1 + vd00;
         b11_a11 = c7x::strm_eng<1, vec>::get_adv();

         temp1_mul_1   = tmp2 * b11_a11; //   t1      = (((b11 + a11) * input1) + vd01) ;
         temp1_add_1   = temp1_mul_1 + filtVars_d1_1;
         a12           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_1   = a12 * filtVars_d0_1; //   t0      = (a12 * vd00);
         a11           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_1     = a11 * filtVars_d0_1; //   vd00     =  ((a11 * vd00) + t1) ;
         filtVars_d0_1 = d0_temp_1 + temp1_add_1;
         b12_a12       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_1      = b12_a12 * tmp2; //   vd01     = (((b12 + a12) * input1) + t0) ;
         filtVars_d1_1 = d1_mul_1 + temp0_mul_1;

#if DSPLIB_DEBUGPRINT
         if (samp < 5) {
            DSPLIB_debugPrintVector(tmp2);
            DSPLIB_debugPrintVector(b11_a11);
            DSPLIB_debugPrintVector(b12_a12);
            DSPLIB_debugPrintVector(a12);
            DSPLIB_debugPrintVector(a11);
            DSPLIB_debugPrintVector(temp1_add_1);
            DSPLIB_debugPrintVector(temp0_mul_1);
            DSPLIB_debugPrintVector(filtVars_d0_1);
            DSPLIB_debugPrintVector(filtVars_d1_1);
         }
#endif
         // second stage
         output2       = output1 + filtVars_d0_2; //   output02 = (output01 + vd02);
         b21_a21       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_2   = b21_a21 * output1; //   t3      = (((b21 + a21) * output01) + vd03);
         temp1_add_2   = temp1_mul_2 + filtVars_d1_2;
         a22           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_2   = a22 * filtVars_d0_2; //   t2      = (a22 * vd02);
         a21           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_2     = a21 * filtVars_d0_2; //   vd02     = ((a21 * vd02) + t3) ;
         filtVars_d0_2 = d0_temp_2 + temp1_add_2;
         b22_a22       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_2      = b22_a22 * output1; //   vd03     = (((b22 + a22) * output01) + t2) ;
         filtVars_d1_2 = d1_mul_2 + temp0_mul_2;

         // third stage
         output3       = output2 + filtVars_d0_3; //   output03 = (output02 + vd04);
         b31_a31       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_3   = b31_a31 * output2;
         temp1_add_3   = temp1_mul_3 + filtVars_d1_3; //   t5      = (((b31 + a31) * output02) + vd05) ;
         a32           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_3   = a32 * filtVars_d0_3; //   t4      = (a32 * vd04);
         a31           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_3     = a31 * filtVars_d0_3; //   vd04      = ((a31 * vd04) + t5) ;
         filtVars_d0_3 = d0_temp_3 + temp1_add_3;
         b32_a32       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_3      = b32_a32 * output2; //   vd05      = (((b32 + a32) * output02) + t4) ;
         filtVars_d1_3 = d1_mul_3 + temp0_mul_3;

         // fourth stage
         output4       = output3 + filtVars_d0_4; //   output03 = (output02 + vd04);
         b41_a41       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_4   = b41_a41 * output3;
         temp1_add_4   = temp1_mul_4 + filtVars_d1_4; //   t5      = (((b31 + a31) * output02) + vd05) ;
         a42           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_4   = a42 * filtVars_d0_4; //   t4      = (a32 * vd04);
         a41           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_4     = a41 * filtVars_d0_4; //   vd04      = ((a31 * vd04) + t5) ;
         filtVars_d0_4 = d0_temp_4 + temp1_add_4;
         b42_a42       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_4      = b42_a42 * output3; //   vd05      = (((b32 + a32) * output02) + t4) ;
         filtVars_d1_4 = d1_mul_4 + temp0_mul_4;

         // five stage
         output5       = output4 + filtVars_d0_5; //   output03 = (output02 + vd04);
         b51_a51       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_5   = b51_a51 * output4;
         temp1_add_5   = temp1_mul_5 + filtVars_d1_5; //   t5      = (((b31 + a31) * output02) + vd05) ;
         a52           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_5   = a52 * filtVars_d0_5; //   t4      = (a32 * vd04);
         a51           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_5     = a51 * filtVars_d0_5; //   vd04      = ((a31 * vd04) + t5) ;
         filtVars_d0_5 = d0_temp_5 + temp1_add_5;
         b52_a52       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_5      = b52_a52 * output4; //   vd05      = (((b32 + a32) * output02) + t4) ;
         filtVars_d1_5 = d1_mul_5 + temp0_mul_5;

         // six stage
         output6       = output5 + filtVars_d0_6; //   output03 = (output02 + vd04);
         b61_a61       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_6   = b61_a61 * output5;
         temp1_add_6   = temp1_mul_6 + filtVars_d1_6; //   t5      = (((b31 + a31) * output02) + vd05) ;
         a62           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_6   = a62 * filtVars_d0_6; //   t4      = (a32 * vd04);
         a61           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_6     = a61 * filtVars_d0_6; //   vd04      = ((a31 * vd04) + t5) ;
         filtVars_d0_6 = d0_temp_6 + temp1_add_6;
         b62_a62       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_6      = b62_a62 * output5; //   vd05      = (((b32 + a32) * output02) + t4) ;
         filtVars_d1_6 = d1_mul_6 + temp0_mul_6;

         // seven stage
         output7       = output6 + filtVars_d0_7; //   output03 = (output02 + vd04);
         b71_a71       = c7x::strm_eng<1, vec>::get_adv();
         temp1_mul_7   = b71_a71 * output6;
         temp1_add_7   = temp1_mul_7 + filtVars_d1_7; //   t5      = (((b31 + a31) * output02) + vd05) ;
         a72           = c7x::strm_eng<0, vec>::get_adv();
         temp0_mul_7   = a72 * filtVars_d0_7; //   t4      = (a32 * vd04);
         a71           = c7x::strm_eng<0, vec>::get_adv();
         d0_temp_7     = a71 * filtVars_d0_7; //   vd04      = ((a31 * vd04) + t5) ;
         filtVars_d0_7 = d0_temp_7 + temp1_add_7;
         b72_a72       = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_7      = b72_a72 * output6; //   vd05      = (((b32 + a32) * output02) + t4) ;
         filtVars_d1_7 = d1_mul_7 + temp0_mul_7;

         vec     output = output7 * outGain;
         __vpred tmp    = c7x::strm_agen<0, vec>::get_vpred();
         vec    *VB1    = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
         __vstore_pred(tmp, VB1, output);
#if DSPLIB_DEBUGPRINT
         if (samp < 5) {
            DSPLIB_debugPrintVector(tmp2);
            DSPLIB_debugPrintVector(output1);
            DSPLIB_debugPrintVector(output2);
            DSPLIB_debugPrintVector(output3);
            DSPLIB_debugPrintVector(output7);
            //            DSPLIB_debugPrintVector(outGain);
            //            DSPLIB_debugPrintVector(output);
            //            DSPLIB_debugPrintVector(outGain);
            //         DSPLIB_debugPrintVector(output3);
         }
#endif
      }

      __vpred tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      vec    *VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_1);

      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_1);

      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_2);

      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_2);

      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_3);

      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_3);

      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_4);

      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_4);

      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_5);

      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_5);

      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_6);

      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_6);

      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d0_7);

      tmp1 = c7x::strm_agen<1, vec>::get_vpred();
      VB2  = c7x::strm_agen<1, vec>::get_adv(pFilterVarLocal);
      __vstore_pred(tmp1, VB2, filtVars_d1_7);
   }
   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
   __SA1_CLOSE();
   __SA2_CLOSE();
   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_cascadeBiquad7Stage_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                                 void *restrict pIn,
                                                                 void *restrict pFilterCoeff,
                                                                 void *restrict pFilterVar,
                                                                 void *restrict pOut);
