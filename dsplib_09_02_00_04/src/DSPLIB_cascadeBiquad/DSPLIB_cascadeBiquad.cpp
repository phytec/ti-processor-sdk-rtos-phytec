/******************************************************************************
 *                                                                             *
 * module name       :DSPLIB                                                   *
 *                                                                             *
 * module descripton :Matrix Multiply Accelerator Library module for C7x+MMA   *
 *                                                                             *
 * Copyright (C) 2017-2018 Texas Instruments Incorporated - http://www.ti.com/ *
 * ALL RIGHTS RESERVED                                                         *
 *                                                                             *
 ******************************************************************************/

/**
******************************************************************************
*  @file     DSPLIB_cascadeBiquad.c
*
*  @brief    DFT computation
*
*  @version  0.1 - Jan 2018 : Initial Version
*
*******************************************************************************
*/

#include "DSPLIB_cascadeBiquad_priv.h"

int32_t DSPLIB_cascadeBiquad_getHandleSize(DSPLIB_cascadeBiquad_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(DSPLIB_cascadeBiquad_PrivArgs);
   return privBufSize;
}

DSPLIB_STATUS DSPLIB_cascadeBiquad_init_checkParams(DSPLIB_kernelHandle                  handle,
                                                    const DSPLIB_bufParams2D_t          *bufParamsIn,
                                                    const DSPLIB_bufParams1D_t          *bufParamsFilterCoeff,
                                                    const DSPLIB_bufParams2D_t          *bufParamsFilterVar,
                                                    const DSPLIB_bufParams2D_t          *bufParamsOut,
                                                    const DSPLIB_cascadeBiquad_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;
   //   DSPLIB_bufParams1D_t bufParamsFilterCoeffLocal;
   //   DSPLIB_bufParams2D_t bufParamsInLocal, bufParamsOutLocal, bufParamsFilterVarLocal;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_cascadeBiquad_init_checkParams\n");
#endif
   if (handle == NULL) {
      status = DSPLIB_ERR_NULL_POINTER;
   }

   if (status == DSPLIB_SUCCESS) {
      if ((bufParamsIn->data_type != DSPLIB_INT16) && (bufParamsIn->data_type != DSPLIB_INT32) &&
          (bufParamsIn->data_type != DSPLIB_INT8) && (bufParamsIn->data_type != DSPLIB_FLOAT32)) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if (bufParamsIn->data_type != bufParamsOut->data_type) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else if (bufParamsIn->data_type != bufParamsFilterCoeff->data_type) {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
      else {
         /* Nothing to do here */
      }
   }

   return status;
}

DSPLIB_STATUS DSPLIB_cascadeBiquad_exec_checkParams(DSPLIB_kernelHandle handle,
                                                    const void *restrict pIn,
                                                    const void *restrict pFilterCoeff,
                                                    const void *restrict pFilterVar,
                                                    const void *restrict pOut)
{
   DSPLIB_STATUS status;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_cascadeBiquad_exec_checkParams\n");
#endif
   if ((pIn == NULL) || (pFilterCoeff == NULL) || (pFilterVar == NULL) || (pOut == NULL)) {
      status = DSPLIB_ERR_NULL_POINTER;
   }
   else {
      status = DSPLIB_SUCCESS;
   }

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

int32_t DSPLIB_cascadeBiquad_get_coefficientsBufferSize(uint32_t numStages)
{
   int32_t size     = 0;
   int     eleCount = 0;

   eleCount = 16;

   int32_t offset = ((numStages * 5 + eleCount) / eleCount) * eleCount * sizeof(DSPLIB_F32);
   size           = offset * 2;

   return size;
}
template <typename dataType>
dataType DSPLIB_cascadeBiquad_store_coefficients(uint32_t numStages, void *restrict pFilterCoeff)
{

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT DSPLIB_cascadeBiquad_store_coefficients\n");
#endif

   int32_t loadCounter    = 0;
   int32_t storeCounter_0 = 0;
   int32_t storeCounter_1 = 0;

   dataType *pFilterCoeffLocal = (dataType *) pFilterCoeff;

   float outGain = 1;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int                                                    eleCount = c7x::element_count_of<vec>::value;
   int32_t                                                offset   = ((numStages * 5 + eleCount) / eleCount) * eleCount;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT eleCount %d offset %d numStages %d\n", eleCount, offset, numStages);
#endif

   for (uint32_t coeffCount = 0; coeffCount < numStages * 5; coeffCount = coeffCount + 5) {
      float coeff0 = pFilterCoeffLocal[loadCounter];
      // std::cout << coeff0 << std::endl;
      loadCounter++;
      float b10 = coeff0; // pFilterCoeffLocal[0];
      float r   = fast_recip<dataType>(b10);
      float b11 = pFilterCoeffLocal[loadCounter] * r;
      loadCounter++;
      float b12 = pFilterCoeffLocal[loadCounter] * r;
      loadCounter++;
      float a11 = -pFilterCoeffLocal[loadCounter];
      loadCounter++;
      float a12 = -pFilterCoeffLocal[loadCounter];
      loadCounter++;
      float b11_a11 = b11 + a11;
      float b12_a12 = b12 + a12;
      outGain       = outGain * b10;
#if DSPLIB_DEBUGPRINT
      printf("DSPLIB_DEBUGPRINT coeffCount %f b10 %f outGain %f\n", coeffCount, b10, outGain);
#endif

      pFilterCoeffLocal[storeCounter_0] = b11_a11;
      storeCounter_0++;
      pFilterCoeffLocal[storeCounter_0] = b12_a12;
      storeCounter_0++;
      pFilterCoeffLocal[offset + storeCounter_1] = a12;
      storeCounter_1++;
      pFilterCoeffLocal[offset + storeCounter_1] = a11;
      storeCounter_1++;
#if DSPLIB_DEBUGPRINT
      printf("DSPLIB_DEBUGPRINT a11 %f a12 %f b11 %f b12 %f b11_a11 %f b12_a12 %f \n", a11, a12, b11, b12, b11_a11,
             b12_a12);
#endif
   }

   return outGain;
}

DSPLIB_STATUS DSPLIB_cascadeBiquad_init(DSPLIB_kernelHandle            handle,
                                        DSPLIB_bufParams2D_t          *bufParamsIn,
                                        DSPLIB_bufParams1D_t          *bufParamsFilterCoeff,
                                        DSPLIB_bufParams2D_t          *bufParamsFilterVar,
                                        DSPLIB_bufParams2D_t          *bufParamsOut,
                                        DSPLIB_cascadeBiquad_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS                  status;
   DSPLIB_cascadeBiquad_PrivArgs *pKerPrivArgs = (DSPLIB_cascadeBiquad_PrivArgs *) handle;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT Enter DSPLIB_cascadeBiquad_init\n");
#endif
   pKerPrivArgs->initArgs = *pKerInitArgs;

   int32_t ele_size = 1;

   if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
      ele_size = 4;
   }
   else {
      status = DSPLIB_ERR_INVALID_TYPE;
   }

   pKerPrivArgs->dataBufferInPitch  = bufParamsIn->stride_y / ele_size;
   pKerPrivArgs->dataBufferOutPitch = bufParamsOut->stride_y / ele_size;
   pKerPrivArgs->filterVarPitch     = bufParamsFilterVar->stride_y / ele_size;
   pKerPrivArgs->filterCoeff        = bufParamsFilterCoeff->dim_x;

   if (pKerInitArgs->funcStyle == DSPLIB_FUNCTION_NATC) {
      status = DSPLIB_cascadeBiquad_init_cn(handle, bufParamsIn, bufParamsFilterCoeff, bufParamsFilterVar, bufParamsOut,
                                            pKerInitArgs);
      if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
         pKerPrivArgs->execute = DSPLIB_cascadeBiquad_exec_cn<float>;
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }
   else {
      if (bufParamsIn->data_type == DSPLIB_FLOAT32) {
#if DSPLIB_DEBUGPRINT
         printf("DSPLIB_DEBUGPRINT bufParamsIn->data_type == DSPLIB_FLOAT32\n");
#endif
         if (pKerPrivArgs->initArgs.numStages == 3) {

            status = DSPLIB_cascadeBiquad_init_ci<float>(handle, bufParamsIn, bufParamsFilterCoeff, bufParamsFilterVar,
                                                         bufParamsOut, pKerInitArgs);
            pKerPrivArgs->execute = DSPLIB_cascadeBiquad_exec_ci<float>;
         }
         else if (pKerPrivArgs->initArgs.numStages == 7) {
            status                = DSPLIB_cascadeBiquad7Stage_init_ci<float>(handle, bufParamsIn, bufParamsFilterCoeff,
                                                               bufParamsFilterVar, bufParamsOut, pKerInitArgs);
            pKerPrivArgs->execute = DSPLIB_cascadeBiquad7Stage_exec_ci<float>;
         }
         else {
            status = DSPLIB_ERR_NOT_IMPLEMENTED;
         }
      }
      else {
         status = DSPLIB_ERR_INVALID_TYPE;
      }
   }

   return status;
}

DSPLIB_STATUS DSPLIB_cascadeBiquad_exec(DSPLIB_kernelHandle handle,
                                        void *restrict pIn,
                                        void *restrict pFilterCoeff,
                                        void *restrict pFilterVar,
                                        void *restrict pOut)
{
   DSPLIB_STATUS status;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT Enter DSPLIB_cascadeBiquad_exec\n");
#endif

   DSPLIB_cascadeBiquad_PrivArgs *pKerPrivArgs = (DSPLIB_cascadeBiquad_PrivArgs *) handle;

   uint32_t numStages = pKerPrivArgs->initArgs.numStages;

   if (pKerPrivArgs->initArgs.funcStyle == DSPLIB_FUNCTION_OPTIMIZED && numStages == 7) {
      pKerPrivArgs->outGain = DSPLIB_cascadeBiquad_store_coefficients<float>(numStages, pFilterCoeff);
   }
   else {
   }

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT pKerPrivArgs->outGain %f\n", pKerPrivArgs->outGain);
#endif
   status = pKerPrivArgs->execute(handle, pIn, pFilterCoeff, pFilterVar, pOut);

   return status;
}