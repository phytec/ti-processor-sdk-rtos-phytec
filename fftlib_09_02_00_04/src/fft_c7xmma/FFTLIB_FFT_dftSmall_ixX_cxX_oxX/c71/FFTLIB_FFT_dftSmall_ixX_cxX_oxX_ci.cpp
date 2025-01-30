/******************************************************************************/
/*!
 * \file FFTLIB_FFT_dftSmall_ixX_cxX_oxX.c
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


/*******************************************************************************
 *
 * INCLUDES
 *
 ******************************************************************************/

#include "../FFTLIB_FFT_dftSmall_ixX_cxX_oxX_priv.h"
#include "../../../common/c71/FFTLIB_inlines.h"


/*******************************************************************************
 *
 * DEFINES
 *
 ******************************************************************************/
#define SE_PARAM_BASE    (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SE1_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_SA0_PARAM_OFFSET (SE_SE1_PARAM_OFFSET + SE_PARAM_SIZE)
#define NUMBLOCKS_OFFSET    (SE_SA0_PARAM_OFFSET + SE_PARAM_SIZE)

#define MMA_PARAM_BASE (NUMBLOCKS_OFFSET + sizeof(int32_t))
#define MMA_PARAM_OFFSET (MMA_PARAM_BASE)
#define MMA_CONFIGREG_OFFSET (MMA_PARAM_OFFSET)
#define MMA_OFFSETREG_OFFSET (MMA_CONFIGREG_OFFSET + sizeof(FFTLIB_MMA_CONFIG_REG))

void FFTLIB_FFT_dftSmall_ixX_cxX_oxX_perfEst(FFTLIB_kernelHandle handle,
   const FFTLIB_bufParams1D_t *bufParamsX,
   const FFTLIB_bufParams1D_t *bufParamsY,
   const FFTLIB_bufParams1D_t *bufParamsW,
   uint32_t fftSize,
   uint32_t batchSize,
   uint64_t *archCycles,
   uint64_t *estCycles)
{
   // determine the mmaWidth based upon output data type
   int32_t MMA_SIZE;
   uint32_t batchSizePerRow, numRows, numBlocks;
   uint64_t startupCycles, teardownCycles, computeCycles, operationCycles, overheadCycles;
   uint64_t storeLatency = 24;

   if (bufParamsX->data_type == FFTLIB_INT8) {
      MMA_SIZE = FFTLIB_MMA_SIZE_8_BIT;
   } else if (bufParamsX->data_type == FFTLIB_INT16) {
      MMA_SIZE = FFTLIB_MMA_SIZE_16_BIT;
   } else {
      MMA_SIZE = FFTLIB_MMA_SIZE_32_BIT;
   }
   
   batchSizePerRow = MMA_SIZE/(fftSize*2);
   numRows           = batchSize/batchSizePerRow;
   numRows           = numRows*batchSizePerRow == batchSize ?
                       numRows : numRows + 1;
   /* number of rows processed per block is aligned with MMA block size 
    * (rather than smaller size) because of better assembly code accounting 
    * for MMA transfer-receive latency                                        */
   numBlocks         = numRows/MMA_SIZE;
   numBlocks         = numBlocks*MMA_SIZE == numRows ?
                       numBlocks : numBlocks + 1;
   
   startupCycles =
   9  +   // kernel function call
   7  +   // load numBlocks, interleaving check
   5  +   // load SE1 params
   17;    // SEOPEN latency on SE1 (the first SE needed)
   
   teardownCycles =
   2;    // close SE and return

   computeCycles = numBlocks*MMA_SIZE;

   operationCycles = 
   MMA_SIZE +  // Load DFT matrix into B
   computeCycles + 
   MMA_SIZE +  // store last block output
   storeLatency;

   overheadCycles =
   2;  // if check for numBlocks > 1
   
   *archCycles = startupCycles + operationCycles + teardownCycles;
   *estCycles  = startupCycles + operationCycles + overheadCycles + teardownCycles;
   
   return;
}

template <uint32_t dataType>
FFTLIB_STATUS FFTLIB_FFT_dftSmall_ixX_cxX_oxX_getSizes(
   const FFTLIB_FFT_dftSmall_ixX_cxX_oxX_InitArgs * pKerInitArgs,
   FFTLIB_bufParams1D_t * bufParamsX, 
   FFTLIB_bufParams1D_t * bufParamsW, 
   FFTLIB_bufParams1D_t * bufParamsY) 
{
    uint32_t fftSize = pKerInitArgs->fftSize;
    uint32_t batchSize = pKerInitArgs->batchSize;
    uint32_t MMA_SIZE;
    FFTLIB_STATUS status  = FFTLIB_SUCCESS;

    if (dataType == FFTLIB_INT32) {
       MMA_SIZE = FFTLIB_MMA_SIZE_32_BIT;
    } else if (dataType == FFTLIB_INT16) {
       MMA_SIZE = FFTLIB_MMA_SIZE_16_BIT;
    } else {
       status = FFTLIB_ERR_NOT_IMPLEMENTED;
    }
    
    if (fftSize > (MMA_SIZE >> 1)) {
       status = FFTLIB_ERR_INVALID_DIMENSION;
    }

    if (status == FFTLIB_SUCCESS) {
       bufParamsX->dim_x = batchSize*fftSize*2;
       bufParamsY->dim_x = batchSize*fftSize*2;
       bufParamsW->dim_x = MMA_SIZE*MMA_SIZE;
    }

    return status;
}

template FFTLIB_STATUS FFTLIB_FFT_dftSmall_ixX_cxX_oxX_getSizes<FFTLIB_INT16>(
   const FFTLIB_FFT_dftSmall_ixX_cxX_oxX_InitArgs * pKerInitArgs,
   FFTLIB_bufParams1D_t * bufParamsX, 
   FFTLIB_bufParams1D_t * bufParamsW, 
   FFTLIB_bufParams1D_t * bufParamsY);
template FFTLIB_STATUS FFTLIB_FFT_dftSmall_ixX_cxX_oxX_getSizes<FFTLIB_INT32>(
   const FFTLIB_FFT_dftSmall_ixX_cxX_oxX_InitArgs * pKerInitArgs,
   FFTLIB_bufParams1D_t * bufParamsX, 
   FFTLIB_bufParams1D_t * bufParamsW, 
   FFTLIB_bufParams1D_t * bufParamsY);

template <typename dataType>
FFTLIB_STATUS FFTLIB_FFT_dftSmall_ixX_cxX_oxX_twGen(
   const FFTLIB_FFT_dftSmall_ixX_cxX_oxX_InitArgs * pKerInitArgs,
   void *restrict pW,
   const FFTLIB_bufParams1D_t * bufParamsW) 
{
    FFTLIB_STATUS    status = FFTLIB_SUCCESS;
    uint32_t j, k, l, n;
    uint32_t batchSizePerRow;
    const FFTLIB_D64 PI = 3.141592654;
    FFTLIB_D64 twF2sScale;
    int32_t index;
    FFTLIB_bufParams1D_t bufParamsXLocal, bufParamsYLocal, bufParamsWLocal; 
    dataType *pWLocal = (dataType *)pW;
    uint32_t fftSize = pKerInitArgs->fftSize;
    uint32_t batchSize = pKerInitArgs->batchSize;
    uint32_t interleave = pKerInitArgs->interleave;
    uint32_t MMA_SIZE;

    if (sizeof(dataType) == 4) {
       status = FFTLIB_FFT_dftSmall_ixX_cxX_oxX_getSizes<FFTLIB_INT32>(pKerInitArgs,
                                                                       &bufParamsXLocal,
                                                                       &bufParamsWLocal,
                                                                       &bufParamsYLocal);
       MMA_SIZE = FFTLIB_MMA_SIZE_32_BIT;
    } else if (sizeof(dataType) == 2) {
       status = FFTLIB_FFT_dftSmall_ixX_cxX_oxX_getSizes<FFTLIB_INT16>(pKerInitArgs,
                                                                       &bufParamsXLocal,
                                                                       &bufParamsWLocal,
                                                                       &bufParamsYLocal);
       MMA_SIZE = FFTLIB_MMA_SIZE_16_BIT;
    } else {
       status = FFTLIB_ERR_NOT_IMPLEMENTED;
    }

    if (bufParamsW->dim_x != bufParamsWLocal.dim_x) {
        status = FFTLIB_ERR_INVALID_DIMENSION;
    }
       
    if (status == FFTLIB_SUCCESS) {
       batchSizePerRow = (MMA_SIZE >> 1)/fftSize;
       for (j = 0; j < MMA_SIZE; j++) {
           for (k = 0; k < MMA_SIZE; k++) {
               pWLocal[j*MMA_SIZE+k] = 0;
           }
       }
       twF2sScale = ((uint32_t)1 << ((uint32_t)(sizeof(dataType)*8-2))) - 1;
       twF2sScale += ((uint32_t)1 << ((uint32_t)(sizeof(dataType)*8-2)));
       twF2sScale += 0.5;
       if (interleave) {
          for (j = 0; j < batchSizePerRow; j++) {
             for ( k = 0; k < fftSize; k++) {
                for (l = 0; l < batchSizePerRow; l++) {
                   index = (j*fftSize+k)*2*MMA_SIZE+l*fftSize*2;
                   for (n = 0; n < fftSize; n++) {

                      /* Fill only block diagonal entries                    */
                      if (l == j) {
                         pWLocal[index+n*2] =
                            FFTLIB_UTIL_cos_i64f_oxX<dataType>(2*PI*k*n/fftSize, twF2sScale); 
                         pWLocal[index+n*2+1] =
                            -FFTLIB_UTIL_sin_i64f_oxX<dataType>(2*PI*k*n/fftSize, twF2sScale);
                         pWLocal[index+MMA_SIZE+n*2] =
                            FFTLIB_UTIL_sin_i64f_oxX<dataType>(2*PI*k*n/fftSize, twF2sScale);
                         pWLocal[index+MMA_SIZE+n*2+1] =
                            FFTLIB_UTIL_cos_i64f_oxX<dataType>(2*PI*k*n/fftSize, twF2sScale);
                      }
                   } 
                }
             }
          }
       } else {
          for (j = 0; j < batchSizePerRow; j++) {
             for ( k = 0; k < fftSize; k++) {
                for (l = 0; l < batchSize; l++) {
                   index = (j*fftSize*2+k)*MMA_SIZE+l*fftSize*2;
                   for (n = 0; n < fftSize; n++) {

                      /* Fill only block diagonal entries                    */
                      if (l == j) {
                         pWLocal[index+n] =
                            FFTLIB_UTIL_cos_i64f_oxX<dataType>(2*PI*k*n/fftSize, twF2sScale); 
                         pWLocal[index+fftSize+n] =
                            -FFTLIB_UTIL_sin_i64f_oxX<dataType>(2*PI*k*n/fftSize, twF2sScale);
                         pWLocal[index+fftSize*MMA_SIZE+n] =
                            FFTLIB_UTIL_sin_i64f_oxX<dataType>(2*PI*k*n/fftSize, twF2sScale);
                         pWLocal[index+fftSize*MMA_SIZE+fftSize+n] =
                            FFTLIB_UTIL_cos_i64f_oxX<dataType>(2*PI*k*n/fftSize, twF2sScale);
                      }
                   } 
                }
             }
          }
       }
    }
    return status;
}

template FFTLIB_STATUS FFTLIB_FFT_dftSmall_ixX_cxX_oxX_twGen<int16_t>(
   const FFTLIB_FFT_dftSmall_ixX_cxX_oxX_InitArgs * pKerInitArgs,
   void *restrict pW,
   const FFTLIB_bufParams1D_t * bufParamsW);

template FFTLIB_STATUS FFTLIB_FFT_dftSmall_ixX_cxX_oxX_twGen<int32_t>(
   const FFTLIB_FFT_dftSmall_ixX_cxX_oxX_InitArgs * pKerInitArgs,
   void *restrict pW,
   const FFTLIB_bufParams1D_t * bufParamsW);

template <uint32_t dataType>
FFTLIB_STATUS FFTLIB_FFT_dftSmall_ixX_cxX_oxX_init_ci(
   FFTLIB_kernelHandle handle,
   const FFTLIB_bufParams1D_t *bufParamsX,
   const FFTLIB_bufParams1D_t *bufParamsW,
   const FFTLIB_bufParams1D_t *bufParamsY,
   const FFTLIB_FFT_dftSmall_ixX_cxX_oxX_InitArgs *pKerInitArgs)
{
   FFTLIB_STATUS    status = FFTLIB_SUCCESS;
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;
   uint32_t batchSizePerRow, numRows, numBlocks;
   FFTLIB_MMA_CONFIG_REG mmaConfig;
   __HWA_OFFSET_REG mmaOffset;
   FFTLIB_FFT_dftSmall_ixX_cxX_oxX_PrivArgs *pKerPrivArgs =
      (FFTLIB_FFT_dftSmall_ixX_cxX_oxX_PrivArgs*) handle;
   uint8_t *pBlock = pKerPrivArgs->bufPblock;
   uint32_t fftSize = pKerInitArgs->fftSize;
   uint32_t batchSize = pKerInitArgs->batchSize;
   uint32_t shift = pKerInitArgs->shiftVector[0];
   uint32_t MMA_SIZE;
   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN SE_VECLEN;
   FFTLIB_MMA_CONFIG_REG mmaConfigDefault;

   if (dataType == FFTLIB_INT32) {
      MMA_SIZE = FFTLIB_MMA_SIZE_32_BIT;
      SE_ELETYPE = __SE_ELETYPE_32BIT;
      SE_VECLEN = __SE_VECLEN_16ELEMS;
      mmaConfigDefault = configRegisterStruct_i32s_i32s_o32s;
   } else if (dataType == FFTLIB_INT16) {
      MMA_SIZE = FFTLIB_MMA_SIZE_16_BIT;
      SE_ELETYPE = __SE_ELETYPE_16BIT;
      SE_VECLEN = __SE_VECLEN_32ELEMS;
      mmaConfigDefault = configRegisterStruct_i16s_i16s_o16s;
   } else {
      status = FFTLIB_ERR_NOT_IMPLEMENTED;
   }

   if (status == FFTLIB_SUCCESS) {
      batchSizePerRow = (MMA_SIZE >> 1)/fftSize;
      numRows           = batchSize/batchSizePerRow;
      numRows           = numRows*batchSizePerRow == batchSize ?
                          numRows : numRows + 1;
      /* number of rows processed per block is aligned with MMA block size 
       * (rather than smaller size) because of better assembly code accounting 
       * for MMA transfer-receive latency                                        */
      numBlocks         = numRows/MMA_SIZE;
      numBlocks         = numBlocks*MMA_SIZE == numRows ?
                          numBlocks : numBlocks + 1;

      /**********************************************************************/
      /* Prepare streaming engine 0 to fetch input data                     */
      /**********************************************************************/
      se0Params  = __gen_SE_TEMPLATE_v1();
      se0Params.ICNT0 =                                 batchSizePerRow*fftSize*2;
      se0Params.ICNT1 =                                     MMA_SIZE*numBlocks;
      se0Params.DIM1 =                                     batchSizePerRow*fftSize*2;

      se0Params.DECDIM1_WIDTH =                                         fftSize*2*batchSize;
      se0Params.DECDIM1   = __SE_DECDIM_DIM1;
      se0Params.ELETYPE   = SE_ELETYPE;
      se0Params.VECLEN    = SE_VECLEN;
      se0Params.DIMFMT    = __SE_DIMFMT_2D;

      /**********************************************************************/
      /* Prepare streaming engine 1 to fetch DFT matrix                     */
      /**********************************************************************/
      se1Params  = __gen_SE_TEMPLATE_v1();
      se1Params.ICNT0 =                                MMA_SIZE;
      se1Params.ICNT1 =                                     MMA_SIZE;
      se1Params.DIM1 =                                     MMA_SIZE;
      /* Load two copies: one into B-back and the other into B-fore        */
      se1Params.ICNT2 =  2;
      se1Params.DIM2 =  0;

      se1Params.ELETYPE   = SE_ELETYPE;
      se1Params.VECLEN    = SE_VECLEN;
      se1Params.DIMFMT    = __SE_DIMFMT_3D;

      /**********************************************************************/
      /* Prepare SA template to store output                                */
      /**********************************************************************/
      sa0Params  = __gen_SA_TEMPLATE_v1();
      sa0Params.ICNT0 =                                fftSize*2*batchSizePerRow*FFTLIB_sizeof(bufParamsY->data_type);
      sa0Params.ICNT1 =                                      numBlocks*MMA_SIZE;
      sa0Params.DIM1 =                                     batchSizePerRow*fftSize*2*FFTLIB_sizeof(bufParamsY->data_type);
      sa0Params.DECDIM1_WIDTH =                                         fftSize*2*batchSize*FFTLIB_sizeof(bufParamsY->data_type);

      sa0Params.DECDIM1   = __SA_DECDIM_DIM1;
      sa0Params.VECLEN    = __SA_VECLEN_64ELEMS;
      sa0Params.DIMFMT    = __SA_DIMFMT_2D;

      /**********************************************************************/
      /* Prepare MMA                                                        */
      /**********************************************************************/
      mmaConfig            = mmaConfigDefault;
      mmaOffset            = offsetRegStruct_zeros;

      mmaConfig.B_BSWPER  = 0xFFFFFFFFu;

      mmaConfig.C_BSWPER  = 0xFFFFFFFFu;
      mmaConfig.C_CWSWPER  = MMA_SIZE;
      mmaConfig.C_CRSWPER  = MMA_SIZE;
      mmaConfig.C_CRRSTPER = MMA_SIZE;
      mmaConfig.C_CWRSTPER = MMA_SIZE;

      mmaConfig.X_SHIFT    = shift;
      mmaConfig.X_CSWPER = MMA_SIZE;
      mmaConfig.X_CRRSTPER = MMA_SIZE;

      *((__SE_TEMPLATE_v1*)((uint8_t *)pBlock + SE_SE0_PARAM_OFFSET)) = se0Params;
      *((__SE_TEMPLATE_v1*)((uint8_t *)pBlock + SE_SE1_PARAM_OFFSET)) = se1Params;
      *((__SA_TEMPLATE_v1*)((uint8_t *)pBlock + SE_SA0_PARAM_OFFSET)) = sa0Params;
      *((FFTLIB_MMA_CONFIG_REG *)((uint8_t *)pBlock + MMA_CONFIGREG_OFFSET)) = mmaConfig;
      *((__HWA_OFFSET_REG *)((uint8_t *)pBlock + MMA_OFFSETREG_OFFSET)) = mmaOffset;

      *((int32_t *)((uint8_t *)pBlock + NUMBLOCKS_OFFSET)) = numBlocks;
   }

   return status;
}

template FFTLIB_STATUS FFTLIB_FFT_dftSmall_ixX_cxX_oxX_init_ci<FFTLIB_INT16>(
    FFTLIB_kernelHandle handle,
    const FFTLIB_bufParams1D_t *bufParamsX,
    const FFTLIB_bufParams1D_t *bufParamsW,
    const FFTLIB_bufParams1D_t *bufParamsY,
    const FFTLIB_FFT_dftSmall_ixX_cxX_oxX_InitArgs *pKerInitArgs);

template FFTLIB_STATUS FFTLIB_FFT_dftSmall_ixX_cxX_oxX_init_ci<FFTLIB_INT32>(
    FFTLIB_kernelHandle handle,
    const FFTLIB_bufParams1D_t *bufParamsX,
    const FFTLIB_bufParams1D_t *bufParamsW,
    const FFTLIB_bufParams1D_t *bufParamsY,
    const FFTLIB_FFT_dftSmall_ixX_cxX_oxX_InitArgs *pKerInitArgs);

/*******************************************************************************
 *
 * EXECUTION FUNCTIONS
 *
 ******************************************************************************/

template <typename dataType, uint32_t MMA_SIZE>
FFTLIB_STATUS FFTLIB_FFT_dftSmall_ixX_cxX_oxX_exec_ci(
   FFTLIB_kernelHandle handle,
   const void *restrict pX,
   const void *restrict pW,
   void *restrict pY)
{
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;
   FFTLIB_MMA_CONFIG_REG mmaConfig;
   __HWA_OFFSET_REG mmaOffset;
   int32_t numBlocks;
   int32_t loopCount;
   FFTLIB_FFT_dftSmall_ixX_cxX_oxX_PrivArgs *pKerPrivArgs =
      (FFTLIB_FFT_dftSmall_ixX_cxX_oxX_PrivArgs*) handle;
   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se1Params = *((__SE_TEMPLATE_v1*)((uint8_t *)pBlock + SE_SE1_PARAM_OFFSET));
   __SE1_OPEN((const void *)pW, se1Params);

   mmaConfig = *((FFTLIB_MMA_CONFIG_REG *)((uint8_t *)pBlock + MMA_CONFIGREG_OFFSET));
   mmaOffset = *((__HWA_OFFSET_REG *)((uint8_t *)pBlock + MMA_OFFSETREG_OFFSET));
   __HWAOPEN(mmaConfig, mmaOffset, __MMA_OPEN_FSM_RESET);
   __HWAADV();

   se0Params = *((__SE_TEMPLATE_v1*)((uint8_t *)pBlock + SE_SE0_PARAM_OFFSET));
   __SE0_OPEN((const void *)pX, se0Params);

   sa0Params = *((__SA_TEMPLATE_v1*)((uint8_t *)pBlock + SE_SA0_PARAM_OFFSET));
   __SA0_OPEN(sa0Params);

   numBlocks = *((int32_t *)((uint8_t *)pBlock + NUMBLOCKS_OFFSET));

   FFTLIB_UTIL_SE1Bload(MMA_SIZE);

   FFTLIB_UTIL_SE0AloadComputeC(MMA_SIZE);

   loopCount = (numBlocks-1)*MMA_SIZE;

   if (numBlocks > 1) {
       _nassert(loopCount > 0);
       FFTLIB_UTIL_SE0AloadComputeCSA0Cstore(loopCount, (uint8_t *)pY);

       FFTLIB_UTIL_SA0Cstore_unroll<MMA_SIZE>(MMA_SIZE, (uint8_t *)pY);
   } else {
       FFTLIB_UTIL_SA0Cstore_unroll<MMA_SIZE>(MMA_SIZE, (uint8_t *)pY);
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
   __HWACLOSE(0);

   return FFTLIB_SUCCESS;
}

template FFTLIB_STATUS FFTLIB_FFT_dftSmall_ixX_cxX_oxX_exec_ci <int16_t, FFTLIB_MMA_SIZE_16_BIT>(
   FFTLIB_kernelHandle handle,
   const void *restrict pX,
   const void *restrict pW,
   void *restrict pY);

template FFTLIB_STATUS FFTLIB_FFT_dftSmall_ixX_cxX_oxX_exec_ci <int32_t, FFTLIB_MMA_SIZE_32_BIT>(
   FFTLIB_kernelHandle handle,
   const void *restrict pX,
   const void *restrict pW,
   void *restrict pY);
