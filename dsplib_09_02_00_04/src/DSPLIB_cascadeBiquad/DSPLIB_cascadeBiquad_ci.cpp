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

/*******************************************************************************
 *
 * DEFINES
 *
 ******************************************************************************/

template <typename dataType>
DSPLIB_STATUS DSPLIB_cascadeBiquad_init_ci(DSPLIB_kernelHandle                  handle,
                                           const DSPLIB_bufParams2D_t          *bufParamsIn,
                                           const DSPLIB_bufParams1D_t          *bufParamsFilterCoeff,
                                           const DSPLIB_bufParams2D_t          *bufParamsFilterVar,
                                           const DSPLIB_bufParams2D_t          *bufParamsOut,
                                           const DSPLIB_cascadeBiquad_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS    status = DSPLIB_SUCCESS;
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;

   DSPLIB_cascadeBiquad_PrivArgs *pKerPrivArgs       = (DSPLIB_cascadeBiquad_PrivArgs *) handle;
   uint8_t                       *pBlock             = pKerPrivArgs->bufPblock;
   uint32_t                       dataSize           = pKerPrivArgs->initArgs.dataSize;
   uint32_t                       numChannels        = pKerPrivArgs->initArgs.numChannels;
   uint32_t                       numStages          = pKerPrivArgs->initArgs.numStages;
   uint32_t                       dataBufferInPitch  = pKerPrivArgs->dataBufferInPitch;
   uint32_t                       dataBufferOutPitch = pKerPrivArgs->dataBufferOutPitch;
   uint32_t                       filterVarPitch     = pKerPrivArgs->filterVarPitch;

   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;
   __SA_VECLEN  SA_VECLEN;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   SE_VECLEN  = c7x::se_veclen<vec>::value;
   SA_VECLEN  = c7x::sa_veclen<vec>::value;
   SE_ELETYPE = c7x::se_eletype<vec>::value;

   uint32_t eleCount = c7x::element_count_of<vec>::value;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT SE_VECLEN: %d, SA_VECLEN: %d, SE_ELETYPE: %d "
          "numChannels %d numStages %d\n",
          SE_VECLEN, SA_VECLEN, SE_ELETYPE, numChannels, numStages);
   printf("DSPLIB_DEBUGPRINT SE_VECLEN: %d, SA_VECLEN: %d, SE_ELETYPE: %d "
          "eleCount %d\n",
          SE_VECLEN, SA_VECLEN, SE_ELETYPE, eleCount);
#endif

      uint32_t icnt1Param = (numChannels > eleCount) ? eleCount : numChannels;
      icnt1Param          = (icnt1Param == 0) ? 1 : icnt1Param;
      uint32_t icnt2Param =
          (numChannels % eleCount == 0) ? (numChannels / eleCount) : (numChannels + eleCount) / eleCount;

      /**********************************************************************/
      /* Prepare streaming engine 1 to fetch the input                      */
      /**********************************************************************/
      se1Params = __gen_SE_TEMPLATE_v1();

      se1Params.ICNT0         = icnt1Param;
      se1Params.ICNT1         = dataSize;
      se1Params.DIM1          = (int32_t) dataBufferInPitch;
      se1Params.ICNT2         = icnt2Param;
      se1Params.DIM2          = eleCount;
      se1Params.ELETYPE       = SE_ELETYPE;
      se1Params.VECLEN        = SE_VECLEN;
      se1Params.DECDIM1_WIDTH = numChannels;
      se1Params.DECDIM1       = __SE_DECDIM_DIM2;
      se1Params.DIMFMT        = __SE_DIMFMT_3D;
      /**********************************************************************/
      /* Prepare streaming engine 0 to fetch filter variables               */
      /**********************************************************************/
      se0Params               = __gen_SE_TEMPLATE_v1();
      se0Params.ICNT0         = icnt1Param;
      se0Params.ICNT1         = numStages * 2;
      se0Params.DIM1          = (int32_t) filterVarPitch;
      se0Params.ICNT2         = icnt2Param;
      se0Params.DIM2          = eleCount;
      se0Params.ELETYPE       = SE_ELETYPE;
      se0Params.VECLEN        = SE_VECLEN;
      se0Params.DECDIM1_WIDTH = numChannels;
      se0Params.DECDIM1       = __SE_DECDIM_DIM2;
      se0Params.DIMFMT        = __SE_DIMFMT_3D;

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

      *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
      *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET) = se1Params;
      *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
      *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET) = sa1Params;

   return status;
}
template <typename dataType> static inline dataType fast_recip(dataType x)
{
   typedef typename c7x::make_full_vector<dataType>::type vec;

   vec a   = (vec) x;
   vec f   = __recip(a);
   f       = f * (2 - a * f);
   f       = f * (2 - a * f);
   f       = f * (2 - a * f);
   vec out = f * (2 - a * f);
   return out.s[0];
}

template DSPLIB_STATUS DSPLIB_cascadeBiquad_init_ci<float>(DSPLIB_kernelHandle                  handle,
                                                           const DSPLIB_bufParams2D_t          *bufParamsIn,
                                                           const DSPLIB_bufParams1D_t          *bufParamsFilterCoeff,
                                                           const DSPLIB_bufParams2D_t          *bufParamsFilterVar,
                                                           const DSPLIB_bufParams2D_t          *bufParamsOut,
                                                           const DSPLIB_cascadeBiquad_InitArgs *pKerInitArgs);
/*******************************************************************************
 *
 * EXECUTION FUNCTIONS
 *
 ******************************************************************************/

template <typename dataType>
DSPLIB_STATUS DSPLIB_cascadeBiquad_exec_ci(DSPLIB_kernelHandle handle,
                                           void *restrict pIn,
                                           void *restrict pFilterCoeff,
                                           void *restrict pFilterVar,
                                           void *restrict pOut)
{
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;

   DSPLIB_cascadeBiquad_PrivArgs *pKerPrivArgs = (DSPLIB_cascadeBiquad_PrivArgs *) handle;
   uint8_t                       *pBlock       = pKerPrivArgs->bufPblock;
   uint32_t                       dataSize     = pKerPrivArgs->initArgs.dataSize;
   uint32_t                       numStages    = pKerPrivArgs->initArgs.numStages;
   uint32_t                       numChannels  = pKerPrivArgs->initArgs.numChannels;

   dataType *pInLocal          = (dataType *) pIn;
   dataType *pFilterCoeffLocal = (dataType *) pFilterCoeff;
   dataType *pFilterVarLocal   = (dataType *) pFilterVar;
   dataType *pOutLocal         = (dataType *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_cascadeBiquad_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int                                                    eleCount = c7x::element_count_of<vec>::value;

   vec input;
   // vec filter;

#if DSPLIB_DEBUGPRINT
   printf("Enter pFilter %p\n", pFilterCoeff);
#endif

   // Filter coefficient
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   __SE0_OPEN(pFilterVarLocal, se0Params);

   // Input samples
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
   __SE1_OPEN(pInLocal, se1Params);

   // Output samples
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   __SA0_OPEN(sa0Params);

   // Filter variables read and write
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);
   __SA1_OPEN(sa1Params);

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT dataSize %d numChannels %d numStages %d pOutLocal "
          "%p\n",
          dataSize, numChannels, numStages, pOutLocal);
#endif

   vec output1, output2, output3;
   vec temp1_mul_1, temp0_mul_1, d0_temp_1, d1_mul_1;
   vec temp1_mul_2, temp0_mul_2, d0_temp_2, d1_mul_2;
   vec temp1_mul_3, temp0_mul_3, d0_temp_3, d1_mul_3;
   vec temp1_add_1, temp1_add_2, temp1_add_3;

   dataType coeff0 = pFilterCoeffLocal[0];
   // printf("\ncoeff0: %10g\n", coeff0);
   vec b10 = (vec) (coeff0); // pFilterCoeffLocal[0];
   vec r   = (vec) (fast_recip<dataType>(coeff0));
   vec b11 = pFilterCoeffLocal[1] * r;
   vec b12 = pFilterCoeffLocal[2] * r;
   vec a11 = (vec) (-1 * pFilterCoeffLocal[3]);
   vec a12 = (vec) (-1 * pFilterCoeffLocal[4]);

   vec b20, b21, b22, a21, a22;
   if (numStages >= 2) {
      coeff0 = pFilterCoeffLocal[5];
      b20    = (vec) (coeff0); // pFilterCoeffLocal[5];
      r      = (vec) fast_recip<dataType>(coeff0);
      b21    = pFilterCoeffLocal[6] * r;
      b22    = pFilterCoeffLocal[7] * r;
      a21    = -pFilterCoeffLocal[8];
      a22    = -pFilterCoeffLocal[9];
   }
   else {
      // if second stage is unused, create coefficients of unity transfer function
      b20 = (vec) 1.0;
      b21 = (vec) 0;
      b22 = (vec) 0;
      a21 = (vec) 0;
      a22 = (vec) 0;
   }

   vec b30, b31, b32, a31, a32;
   if (numStages >= 3) {
      coeff0 = pFilterCoeffLocal[10];
      b30    = (vec) (coeff0); // pFilterCoeffLocal[10];
      r      = (vec) fast_recip<dataType>(coeff0);
      b31    = pFilterCoeffLocal[11] * r;
      b32    = pFilterCoeffLocal[12] * r;
      a31    = -pFilterCoeffLocal[13];
      a32    = -pFilterCoeffLocal[14];
   }
   else {
      // if third stage is unused, create coefficients of unity transfer function
      b30 = (vec) 1.0;
      b31 = (vec) 0;
      b32 = (vec) 0;
      a31 = (vec) 0;
      a32 = (vec) 0;
   }
   /* Get the filter states into corresponding regs */

   vec outGain = b10 * b20 * b30; // Final output gain rescales based on b0 term

   vec b11_a11 = b11 + a11;
   vec b12_a12 = b12 + a12;
   vec b21_a21 = b21 + a21;
   vec b22_a22 = b22 + a22;
   vec b31_a31 = b31 + a31;
   vec b32_a32 = b32 + a32;

#if DSPLIB_DEBUGPRINT
   DSPLIB_debugPrintVector(outGain);
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
      vec filtVars_d0_1 = c7x::strm_eng<0, vec>::get_adv();
      vec filtVars_d1_1 = c7x::strm_eng<0, vec>::get_adv();
      vec filtVars_d0_2 = c7x::strm_eng<0, vec>::get_adv();
      vec filtVars_d1_2 = c7x::strm_eng<0, vec>::get_adv();
      vec filtVars_d0_3 = c7x::strm_eng<0, vec>::get_adv();
      vec filtVars_d1_3 = c7x::strm_eng<0, vec>::get_adv();

#if DSPLIB_DEBUGPRINT
//      DSPLIB_debugPrintVector(filtVars_d0_1);
//      DSPLIB_debugPrintVector(filtVars_d1_1);
//      DSPLIB_debugPrintVector(filtVars_d0_2);
//      DSPLIB_debugPrintVector(filtVars_d1_2);
//      DSPLIB_debugPrintVector(filtVars_d0_3);
//      DSPLIB_debugPrintVector(filtVars_d1_3);
#endif
      for (uint32_t samp = 0; samp < dataSize; samp++) {
         // multi channel second stage
         input = c7x::strm_eng<1, vec>::get();
#if DSPLIB_DEBUGPRINT
//      DSPLIB_debugPrintVector(input);
#endif
         output1       = input + filtVars_d0_1; //   output1  = input1 + vd01;
         temp1_mul_1   = input * b11_a11;       //   t1       = (((b11 + a11) * input1) + vd11) ;
         temp1_add_1   = temp1_mul_1 + filtVars_d1_1;
         temp0_mul_1   = a12 * filtVars_d0_1; //   t0       = (a12 * vd01);
         d0_temp_1     = a11 * filtVars_d0_1; //   vd01     =  ((a11 * vd01) + t1) ;
         filtVars_d0_1 = d0_temp_1 + temp1_add_1;
         input         = c7x::strm_eng<1, vec>::get_adv();
         d1_mul_1      = b12_a12 * input; //   vd11     = (((b12 + a12) * input1) + t0) ;
         filtVars_d1_1 = d1_mul_1 + temp0_mul_1;

         // multi channel second stage
         output2       = output1 + filtVars_d0_2; //   output2  = (output01 + vd02);
         temp1_mul_2   = b21_a21 * output1;       //   t3       = (((b21 + a21) * output01) + vd12);
         temp1_add_2   = temp1_mul_2 + filtVars_d1_2;
         temp0_mul_2   = a22 * filtVars_d0_2; //   t2       = (a22 * vd02);
         d0_temp_2     = a21 * filtVars_d0_2; //   vd02     = ((a21 * vd02) + t3) ;
         filtVars_d0_2 = d0_temp_2 + temp1_add_2;
         d1_mul_2      = b22_a22 * output1; //   vd12     = (((b22 + a22) * output01) + t2) ;
         filtVars_d1_2 = d1_mul_2 + temp0_mul_2;

         // multi channel third stage
         output3       = output2 + filtVars_d0_3; //   output3  = (output02 + vd03);
         temp1_mul_3   = b31_a31 * output2;
         temp1_add_3   = temp1_mul_3 + filtVars_d1_3; //   t5       = (((b31 + a31) * output02) + vd13) ;
         temp0_mul_3   = a32 * filtVars_d0_3;         //   t4       = (a32 * vd03);
         d0_temp_3     = a31 * filtVars_d0_3;         //   vd03     = ((a31 * vd03) + t5) ;
         filtVars_d0_3 = d0_temp_3 + temp1_add_3;
         d1_mul_3      = b32_a32 * output2; //   vd13      = (((b32 + a32) * output02) + t4) ;
         filtVars_d1_3 = d1_mul_3 + temp0_mul_3;

         __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
         vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
#if DSPLIB_DEBUGPRINT
//         if (samp == 0 || samp == 1) {
//            DSPLIB_debugPrintVector(input);
//            DSPLIB_debugPrintVector(output1);
//            DSPLIB_debugPrintVector(output2);
//            DSPLIB_debugPrintVector(output3);
//         }
#endif
         vec output = output3 * outGain;
#if DSPLIB_DEBUGPRINT
         //         if (samp == 0 || samp == 1) {
         DSPLIB_debugPrintVector(output);
//         }
#endif
         __vstore_pred(tmp, VB1, output);
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
   }
   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
   __SA1_CLOSE();

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_cascadeBiquad_exec_ci<float>(DSPLIB_kernelHandle handle,
                                                           void *restrict pIn,
                                                           void *restrict pFilterCoeff,
                                                           void *restrict pFilterVar,
                                                           void *restrict pOut);