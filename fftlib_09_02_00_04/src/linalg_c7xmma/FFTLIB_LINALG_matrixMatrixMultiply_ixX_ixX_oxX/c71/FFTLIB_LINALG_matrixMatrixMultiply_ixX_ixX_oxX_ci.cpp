/******************************************************************************/
/*!
 * \file FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX.c
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

#include "../FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_priv.h"
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

#define FFTLIB_PARAM_BASE (SE_SA0_PARAM_OFFSET + SE_PARAM_SIZE)

#define FFTLIB_CONFIGREG_OFFSET (FFTLIB_PARAM_BASE)
#define FFTLIB_OFFSETREG_OFFSET FFTLIB_CONFIGREG_OFFSET + sizeof(FFTLIB_MMA_CONFIG_REG)


/*******************************************************************************
 *
 * INITIALIZATION FUNCTIONS
 *
 ******************************************************************************/

void FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_perfEst(FFTLIB_kernelHandle handle,
                                                            const FFTLIB_bufParams2D_t *src0_addr,
                                                            const FFTLIB_bufParams2D_t *src1_addr,
                                                            const FFTLIB_bufParams2D_t *dst_addr,
                                                            uint64_t *archCycles,
                                                            uint64_t *estCycles,
                                                            int32_t *caseNumber)
{
   // determine the mmaWidth based upon output data type
   int32_t MMA_SIZE;
   if (dst_addr->data_type == FFTLIB_INT8 || dst_addr->data_type == FFTLIB_UINT8) {
      MMA_SIZE = FFTLIB_MMA_SIZE_8_BIT;
   } else if (dst_addr->data_type == FFTLIB_INT16 || dst_addr->data_type == FFTLIB_UINT16){
      MMA_SIZE = FFTLIB_MMA_SIZE_16_BIT;
   } else {
      MMA_SIZE = FFTLIB_MMA_SIZE_32_BIT;
   }
   
   int32_t M = src0_addr->dim_y / MMA_SIZE;
   int32_t remM = src0_addr->dim_y % MMA_SIZE;
   int32_t K = src0_addr->dim_x / MMA_SIZE;
   int32_t remK = src0_addr->dim_x % MMA_SIZE;
   int32_t N = src1_addr->dim_x / MMA_SIZE;
   int32_t remN = src1_addr->dim_x % MMA_SIZE;
   
   if(remM > 0){
      M++;
   }
   
   if(remK > 0){
      K++;
   }
   
   if(remN > 0){
      N++;
   }
   
   int32_t MN = M*N;
//   *idealCycles = (src0_addr->dim_y * src0_addr->dim_x * src1_addr->dim_x)/(MMA_SIZE*MMA_SIZE);
//   if(*idealCycles == 0){
//      *idealCycles = 1;
//   }
   
   uint64_t startupCycles, teardownCycles, caseCycles, overheadCycles;
   uint64_t storeLatency = 24;
   
   startupCycles =
   5 +   // FFTLIB_STATUS FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec overhead
   5  +   // kernel function call
   5  +   // load SE1 params
   17;    // SEOPEN latency on SE1 (the first SE needed)
   
   teardownCycles =
   2 +   // SE/SA/MMA closes
   3;    // stack operations
   
   // 1 output block
   if (MN == 1)
   {
      // case 1: M*N = 1, K = 1
      if (K == 1)
      {
         *caseNumber = 1;
         
         caseCycles =
         MMA_SIZE +        // load Bload
         MMA_SIZE +        // load A,            compute
         MMA_SIZE +        // store Cback
         storeLatency;            // store latency
         overheadCycles =
         1 +                      // load Bload
         0 +                      // load A,            compute
         0 +                      // store Cback
         1;                       // store latency               (PROT)
      }
      // case 2: M*N = 1, K > 1
      else
      {
         *caseNumber = 2;
         caseCycles =
         MMA_SIZE +        // load Bload
         (K-1)*MMA_SIZE +  // load A, load Bload, compute
         MMA_SIZE +        // load A,             compute
         MMA_SIZE +        // store Cback
         storeLatency;            // store latency
         overheadCycles =
         2 +                      // load Bload
         3 +                      // load A, load Bload, compute  (branch around, PROT)
         1 +                      // load A,             compute
         0 +                      // store Cback
         1;                       // store latency                (PROT)
      }
   }
   // > 1 output block
   else
   {
      // case 3: M*N > 1, K = 1
      if (K == 1)
      {
         *caseNumber = 3;
         caseCycles =
         MMA_SIZE +           // load Bload
         MMA_SIZE;            // load A, load Bload, compute
         
         overheadCycles =
         1 + 0 +                     // load Bload
         0 + 2 +                     // load A, load Bload, compute
         3;                          // test MN condition below
         
         if(MN > 2) {
            caseCycles +=
            (M*N-2)*MMA_SIZE +          // load A, load Bload, compute, store Cback
            MMA_SIZE +                  // load A,             compute, store Cback
            MMA_SIZE +                  // store Cback
            storeLatency;               // store latency
            
            overheadCycles +=
            1 +                         // UNPROT before MN loop
            (M*N-2)*0 + 1 +             // load A, load Bload, compute, store Cback  (UNPROT)
            0 +                         // load A,             compute, store Cback  (unroll)
            0 +                         // store Cback                               (unroll)
            0;                          // store latency                             (unroll)
         } else {
            caseCycles +=
            MMA_SIZE +                  // load A,             compute, store Cback
            MMA_SIZE +                  // store Cback
            storeLatency;               // store latency
            
            overheadCycles +=
            0 +                         // load A,             compute, store Cback  (unroll)
            0 +                         // store Cback                               (unroll)
            0;                          // store latency
         }
      }
      // case 4: M*N > 1, K > 1
      else
      {
         *caseNumber = 4;
         
         caseCycles =
         MMA_SIZE +              // load Bload
         K*MMA_SIZE;             // load A, load Bload, compute
         
         overheadCycles =
         1 +                             // branch from case 3 to case 4
         1 +                             // UNPROT
         0 +                             // load Bload
         0 +                             // load A, load Bload, compute
         1 +                             // PROT and branch prep
         2;                              // more branch prep and possible branch to K==2
         
         if(K >= 3){
            caseCycles +=
            0 +                           // (MN-2) loop setup
            (M*N-2)*MMA_SIZE +            //    load A, load Bload, compute, store Cback
            (M*N-2)*MMA_SIZE +            //    load A, load Bload, compute
            (M*N-2)*(K-2)*MMA_SIZE +      //    load A, load Bload, compute
            0 +                           // loop end
            MMA_SIZE +                    // load A, load Bload, compute, store Cback
            MMA_SIZE +                    // load A, load Bload, compute
            (K-3)*MMA_SIZE +              // load A, load Bload, compute
            MMA_SIZE;                     // load A,           , compute
            
            overheadCycles +=
            2 +                            // (MN-2) loop setup
            (M*N-2)*1 +                    //    UNPROT
            (M*N-2)*0 +                    //    load A, load Bload, compute, store Cback
            0 +                            //    load A, load Bload, compute
            (M*N-2)*1 +                    //    load A, load Bload, compute, PROT
            (M*N-2)*1 +                    // loop end (branch)
            1 +                            // load A, load Bload, compute, store Cback, UNPROT
            0 +                            // load A, load Bload, compute  (unroll)
            2 +                            // load A, load Bload, compute  (useless PROT/UNPROT)
            2;                             // load A,           , compute
            
         } else { // K == 2
            caseCycles +=
            0 +                            // (MN-2) loop setup
            (M*N-2)*MMA_SIZE +             //    load A, load Bload, compute, store Cback
            (M*N-2)*MMA_SIZE +             //    load A, load Bload, compute
            0 +                            // loop end
            MMA_SIZE +                     // load A, load Bload, compute, store Cback
            MMA_SIZE;                      // load A,           , compute
            
            overheadCycles +=
            1 +                            // UNPROT
            0 +                            // (MN-2) loop setup
            0 +                            //    load A, load Bload, compute
            (M*N-2)*1 +                    //    load A, load Bload, compute
            1 +                            // loop end, UNPROT
            0 +                            // load A, load Bload, compute, store Cback
            0;                             // load A,           , compute
            
         } // end of if(K >= 3){} else{};
         caseCycles +=
         MMA_SIZE +              // store Cback
         storeLatency;                  // store latency
         overheadCycles +=
         0 +                            // store Cback
         1;                             // store latency
         
         
      }
   }
   
   *archCycles = startupCycles + caseCycles + teardownCycles;
   *estCycles  = startupCycles + caseCycles + overheadCycles + teardownCycles;
   
   return;
}

template <uint32_t dataType>
FFTLIB_STATUS FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_init_ci(FFTLIB_kernelHandle handle,
                                                                        const FFTLIB_bufParams2D_t *src0_addr,
                                                                        const FFTLIB_bufParams2D_t *src1_addr,
                                                                        const FFTLIB_bufParams2D_t *dst_addr,
                                                                        const FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_InitArgs *pKerInitArgs)
{
   FFTLIB_STATUS status = FFTLIB_SUCCESS;

   // dataType dependent values
   uint32_t MMA_SIZE;
   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN SE_VECLEN;
   uint32_t MMA_TYPE_BYTEWIDTH;
   __MMA_A_CONFIG_ATYPE ATYPE;
   __MMA_C_CONFIG_BTYPE BTYPE;
   __MMA_X_CONFIG_XTYPE XTYPE;
   FFTLIB_MMA_CONFIG_REG mmaConfig;
   
   // if/else structure removed by compiler as conditions are all known at compile time and only one is valid per template value
   if(dataType == FFTLIB_INT32){
      MMA_SIZE = FFTLIB_MMA_SIZE_32_BIT;
      SE_ELETYPE = __SE_ELETYPE_32BIT;
      SE_VECLEN = __SE_VECLEN_16ELEMS;
      MMA_TYPE_BYTEWIDTH = 4;
      ATYPE = __MMA_A_CONFIG_ATYPE_INT32;
      BTYPE = __MMA_C_CONFIG_BTYPE_INT32;
      XTYPE = __MMA_X_CONFIG_XTYPE_INT32;
      
      // initialize the config to one of the common configurations
      mmaConfig = configRegisterStruct_i32s_i32s_o32s;
      
   } else if(dataType == FFTLIB_INT16){
      MMA_SIZE = FFTLIB_MMA_SIZE_16_BIT;
      SE_ELETYPE = __SE_ELETYPE_16BIT;
      SE_VECLEN = __SE_VECLEN_32ELEMS;
      MMA_TYPE_BYTEWIDTH = 2;
      ATYPE = __MMA_A_CONFIG_ATYPE_INT16;
      BTYPE = __MMA_C_CONFIG_BTYPE_INT16;
      XTYPE = __MMA_X_CONFIG_XTYPE_INT16;
      
      // initialize the config to one of the common configurations
      mmaConfig = configRegisterStruct_i16s_i16s_o16s;

   } else if(dataType == FFTLIB_INT8){
      MMA_SIZE = FFTLIB_MMA_SIZE_8_BIT;
      SE_ELETYPE = __SE_ELETYPE_8BIT;
      SE_VECLEN = __SE_VECLEN_64ELEMS;
      MMA_TYPE_BYTEWIDTH = 1;
      ATYPE = __MMA_A_CONFIG_ATYPE_INT8;
      BTYPE = __MMA_C_CONFIG_BTYPE_INT8;
      XTYPE = __MMA_X_CONFIG_XTYPE_INT8;
      
      // initialize the config to one of the common configurations
      mmaConfig = configRegisterStruct_i8s_i8s_o8s;
   } else {
      status = FFTLIB_ERR_INVALID_TYPE;
   }
      
      
   /*************************************************************************/
   /* Matrix multiply cases                                                      */
   /*************************************************************************/
   
   int32_t M = src0_addr->dim_y / MMA_SIZE;
   int32_t remM = src0_addr->dim_y % MMA_SIZE;
   int32_t K = src0_addr->dim_x / MMA_SIZE;
   int32_t remK = src0_addr->dim_x % MMA_SIZE;
   int32_t N = src1_addr->dim_x / MMA_SIZE;
   int32_t remN = src1_addr->dim_x % MMA_SIZE;
   
   if(remM > 0){
      M++;
   }
   
   if(remK > 0){
      K++;
   }
   
   if(remN > 0){
      N++;
   }
   
   // store parameters
   FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_PrivArgs *pKerPrivArgs =
   (FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_PrivArgs*) handle;
   pKerPrivArgs->M = M;
   pKerPrivArgs->K = K;
   pKerPrivArgs->N = N;
   
   int32_t strideAElements = pKerPrivArgs->strideAElements;
   int32_t strideBElements = pKerPrivArgs->strideBElements;
   
   
   /*************************************************************************/
   /* Allocate SE/SA parameters and flags                                   */
   /*************************************************************************/
   
   // SE0 for the A matrix
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();   // SE parameter vector
   
   se0Params.DIMFMT = __SE_DIMFMT_5D;
   se0Params.ELETYPE   = SE_ELETYPE;
   se0Params.VECLEN    = SE_VECLEN;
   se0Params.DECDIM1   = __SE_DECDIM_DIM2;
   se0Params.DECDIM2   = __SE_DECDIM_DIM4;
   se0Params.DECDIM1SD = __SE_DECDIMSD_DIM0;
   se0Params.DECDIM2SD = __SE_DECDIMSD_DIM1;
   
   se0Params.ICNT0 =  (uint32_t) MMA_SIZE;
   se0Params.ICNT1 =  (uint32_t) MMA_SIZE;
   se0Params.DIM1 =  strideAElements;
   se0Params.DECDIM1_WIDTH =  (uint32_t) src0_addr->dim_x;
   se0Params.ICNT2 =  (uint32_t) K;
   se0Params.DIM2 =  (int32_t) MMA_SIZE;
   se0Params.ICNT3 =  (uint32_t) N;
   se0Params.DIM3 =  (int32_t) 0;
   se0Params.DECDIM2_WIDTH =  (uint32_t) src0_addr->dim_y * strideAElements;
   se0Params.ICNT4 =  (uint32_t) M;
   se0Params.DIM4 =  (int32_t) MMA_SIZE * strideAElements;
   //se0Params = __set_SE_FLAGS(se0Params, &se0Params, NULL);
   
   
   /*************************************************************************/
   /* Prepare streaming engine to fetch B matrix input                      */
   /*************************************************************************/
   
   // SE1 for the B matrix
   __SE_TEMPLATE_v1 se1Params = __gen_SE_TEMPLATE_v1();   // SE parameter vector
   
   se1Params.DIMFMT    = __SE_DIMFMT_5D;
   se1Params.ELETYPE   = SE_ELETYPE;
   se1Params.VECLEN    = SE_VECLEN;
   se1Params.DECDIM1   = __SE_DECDIM_DIM3;
   se1Params.DECDIM2   = __SE_DECDIM_DIM2;
   se1Params.DECDIM1SD = __SE_DECDIMSD_DIM0;
   se1Params.DECDIM2SD = __SE_DECDIMSD_DIM1;
   
   //        __set_DIMx(params, icnt, dim);
   se1Params.ICNT0 =       (uint32_t) MMA_SIZE;
   se1Params.ICNT1 =  (uint32_t) MMA_SIZE;
   se1Params.DIM1 =  (int32_t) strideBElements;
   se1Params.DECDIM2_WIDTH =  (uint32_t) src0_addr->dim_x * strideBElements;
   se1Params.ICNT2 =  (uint32_t) K;
   se1Params.DIM2 =  (int32_t) MMA_SIZE*strideBElements;
   se1Params.DECDIM1_WIDTH =  (uint32_t) src1_addr->dim_x;
   se1Params.ICNT3 =  (uint32_t) N;
   se1Params.DIM3 =  (int32_t) MMA_SIZE;
   se1Params.ICNT4 =  (uint32_t) M;
   se1Params.DIM4 =  (int32_t) 0;
   //se1Params = __set_SE_FLAGS(se1Params, &se1Params, NULL);
   
   
   /*************************************************************************/
   /* Prepare streaming address generator to store C matrix output           */
   /*************************************************************************/
   
   // SA flags for storing the C matrix
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();
   
   sa0Params.VECLEN    = __SA_VECLEN_64ELEMS;
   sa0Params.DIMFMT    = __SA_DIMFMT_4D;
   sa0Params.DECDIM1   = __SA_DECDIM_DIM2;
   sa0Params.DECDIM2   = __SA_DECDIM_DIM3;
   sa0Params.DECDIM1SD = __SA_DECDIMSD_DIM0;
   sa0Params.DECDIM2SD = __SA_DECDIMSD_DIM1;
   
   //         __set_DIMx(params,   icnt,                                      dim);
   sa0Params.ICNT0 =  (uint32_t) FFTLIB_BYTE_WIDTH;
   sa0Params.ICNT1 =  (uint32_t) MMA_SIZE;
   sa0Params.DIM1 =  (int32_t) dst_addr->stride_y;
   sa0Params.DECDIM1_WIDTH =  (uint32_t) dst_addr->dim_x*MMA_TYPE_BYTEWIDTH;
   sa0Params.ICNT2 =  (uint32_t) N;
   sa0Params.DIM2 =  (int32_t) FFTLIB_BYTE_WIDTH;
   sa0Params.DECDIM2_WIDTH =  (uint32_t) src0_addr->dim_y*dst_addr->stride_y;
   sa0Params.ICNT3 =  (uint32_t) M;
   sa0Params.DIM3 =  (int32_t) MMA_SIZE*dst_addr->stride_y;
   
   //sa0Params = __set_SA_FLAGS(sa0Params, &saFlags);
   
   /*************************************************************************/
   /* Prepare the MMA for C = A*B                                           */
   /*************************************************************************/
   
   // initialize the offset register
   __HWA_OFFSET_REG mmaOffset  = offsetRegStruct_zeros;
   
   // update data dependent configuration parameters
   mmaConfig.A_ATYPE = ATYPE;
   
   mmaConfig.C_BTYPE = BTYPE;
   mmaConfig.C_OP1PER  = (K-1)*MMA_SIZE;
   mmaConfig.C_CRSWPER = K*MMA_SIZE;
   mmaConfig.C_CWSWPER = K*MMA_SIZE;
   
   mmaConfig.X_XTYPE = XTYPE;
   mmaConfig.X_SHIFT = pKerInitArgs->shift;
   
   *(__SE_TEMPLATE_v1*)((uint8_t *)pKerPrivArgs->bufPblock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SE_TEMPLATE_v1*)((uint8_t *)pKerPrivArgs->bufPblock + SE_SE1_PARAM_OFFSET) = se1Params;
   *(__SA_TEMPLATE_v1*)((uint8_t *)pKerPrivArgs->bufPblock + SE_SA0_PARAM_OFFSET) = sa0Params;
   *((FFTLIB_MMA_CONFIG_REG *)((uint8_t *)pKerPrivArgs->bufPblock + FFTLIB_CONFIGREG_OFFSET)) = mmaConfig;
   *((__HWA_OFFSET_REG *)((uint8_t *)pKerPrivArgs->bufPblock + FFTLIB_OFFSETREG_OFFSET)) = mmaOffset;

   return status;
}



template FFTLIB_STATUS FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_init_ci<FFTLIB_INT8>(FFTLIB_kernelHandle handle,
                                                                                           const FFTLIB_bufParams2D_t *src0_addr,
                                                                                           const FFTLIB_bufParams2D_t *src1_addr,
                                                                                           const FFTLIB_bufParams2D_t *dst_addr,
                                                                                              const FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_InitArgs *pKerInitArgs);

template FFTLIB_STATUS FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_init_ci<FFTLIB_INT16>(FFTLIB_kernelHandle handle,
                                                                                               const FFTLIB_bufParams2D_t *src0_addr,
                                                                                               const FFTLIB_bufParams2D_t *src1_addr,
                                                                                               const FFTLIB_bufParams2D_t *dst_addr,
                                                                                               const FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_InitArgs *pKerInitArgs);

template FFTLIB_STATUS FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_init_ci<FFTLIB_INT32>(FFTLIB_kernelHandle handle,
                                                                                               const FFTLIB_bufParams2D_t *src0_addr,
                                                                                               const FFTLIB_bufParams2D_t *src1_addr,
                                                                                               const FFTLIB_bufParams2D_t *dst_addr,
                                                                                               const FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_InitArgs *pKerInitArgs);

/*******************************************************************************
 *
 * EXECUTION FUNCTIONS
 *
 ******************************************************************************/

template <int32_t MMA_SIZE>
FFTLIB_STATUS FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec_ci(
      FFTLIB_kernelHandle handle,
      const void *src0,
      const void *src1,
      void *dst)
{
   // m,k,n are the counters for M, K, N
   int32_t mn,k;
   
   // load private arguments into variables
   FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_PrivArgs *pKerPrivArgs =
   (FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_PrivArgs*) handle;
   int32_t M = pKerPrivArgs->M;
   int32_t K = pKerPrivArgs->K;
   int32_t N = pKerPrivArgs->N;
   
   int32_t MN = M*N;

   
   /*************************************************************************/
   /* Allocate SE/SA parameters and flags                                   */
   /*************************************************************************/

   // SE0 for the src0 matrix
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();   // SE parameter vector

   // SE1 for the src1 matrix
   __SE_TEMPLATE_v1 se1Params = __gen_SE_TEMPLATE_v1();   // SE parameter vector

   // SA0 for storing the dst matrix
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   /*************************************************************************/
   /* Matrix multiply                                                       */
   /*************************************************************************/

   // load streaming engine and MMA parameters
   se0Params = *(__SE_TEMPLATE_v1*)((uint8_t *)pKerPrivArgs->bufPblock + SE_SE0_PARAM_OFFSET);
   se1Params = *(__SE_TEMPLATE_v1*)((uint8_t *)pKerPrivArgs->bufPblock + SE_SE1_PARAM_OFFSET);
   sa0Params = *(__SA_TEMPLATE_v1*)((uint8_t *)pKerPrivArgs->bufPblock + SE_SA0_PARAM_OFFSET);
   FFTLIB_MMA_CONFIG_REG mmaConfig = *((FFTLIB_MMA_CONFIG_REG *)((uint8_t *)pKerPrivArgs->bufPblock + FFTLIB_CONFIGREG_OFFSET));
   __HWA_OFFSET_REG mmaOffset = *((__HWA_OFFSET_REG *)((uint8_t *)pKerPrivArgs->bufPblock + FFTLIB_OFFSETREG_OFFSET));

   // open SE0
   __SE0_OPEN(src0, se0Params);
   // open SE1 for loading the B matrix
   __SE1_OPEN(src1, se1Params);
   // open SA0
   __SA0_OPEN(sa0Params);
   // open MMA
   __HWAOPEN(mmaConfig, mmaOffset, __MMA_OPEN_FSM_RESET);
   __HWAADV();

   // 1 output block
   if ((M <= 1) && (N <= 1))
   {
      // case 1: M <= 1, K <= 1, N <= 1,
      if (K == 1)
      {
         FFTLIB_DEBUGPRINTFN(1, "Case 1%s", "\n");
         // load    Bload(0, 0)
         FFTLIB_UTIL_SE1Bload(MMA_SIZE);
         
         // swap    Bload(0, 0) to Bfore(0, 0)
         // load    Afore(0, 0)
         // compute Cfore(0, 0) = Afore(0, 0)*Bfore(0, 0)
         FFTLIB_UTIL_SE0AloadComputeC(MMA_SIZE);
         
         // swap  Cfore(0, 0) to Cback(0, 0)
         // store Cback(0, 0)
         FFTLIB_UTIL_SA0Cstore(MMA_SIZE, (uint8_t *)dst);
      }
      // case 2: M*N = 1, K > 1
      else
      {
         FFTLIB_DEBUGPRINTFN(1, "Case 2%s", "\n");
         // load    Bload(0, 0)
         FFTLIB_UTIL_SE1Bload(MMA_SIZE);
         
         // swap    Bload(k,     0) to Bfore(k, 0)
         // load    Afore(0,     k)
         // load    Bload(k + 1, 0)
         // compute Cfore(0,     0) += Afore(0, k)*Bfore(k, 0)
         for (k = 0; k < (K - 1); k++)
         {
            FFTLIB_UTIL_SE0AloadSE1BloadComputeC(MMA_SIZE);
         }
         
         // swap    Bload(K - 1, 0) to Bfore(K - 1, 0)
         // load    Afore(0, K - 1)
         // compute Cfore(0,     0) += Afore(0, K - 1)*Bfore(K - 1, 0)
         FFTLIB_UTIL_SE0AloadComputeC(MMA_SIZE);
         
         // swap  Cfore(0, 0) to Cback(0, 0)
         // store Cback(0, 0)
         FFTLIB_UTIL_SA0Cstore(MMA_SIZE, (uint8_t *)dst);
      }
   }
   // > 1 output block
   else
   {
      // case 3: M*N > 1, K = 1
      FFTLIB_DEBUGPRINTFN(1, "Case 3%s", "\n");
      if (K == 1)
      {
         
         // load    Bload(0, 0)
         FFTLIB_UTIL_SE1Bload(MMA_SIZE);
         
         // swap    Bload(0, 0) to Bfore(0, 0)
         // load    Afore(0, 0)
         // load    Bload(0, 1)
         // compute Cfore(0, 0) = Afore(0, 0)*Bfore(0, 0)
         FFTLIB_UTIL_SE0AloadSE1BloadComputeC(MMA_SIZE);
         
         // guarantee this loop runs at least 2 iterations.  The compiler needs the total trip
         // count to be >= 24 due to MMA store latency.  For 8- and 16-bit data a single block is
         // sufficient.  However, for 32-bit data a block is only 16 iterations and
         // compiler generates a separate case for MN == 3 and MN > 3 to deal with
         // this.  That branch-around then breaks PE scheduling that would place overlap the
         // unrolled loop.  By explicitly coding for this case we can allow for PE scheduling.
         // Adding the extra case added zero cycles to the 8- and 16- implementations, so able
         // to maintain a separate template.
         if(MN > 3) {
            _nassert (MN > 3);
            for (mn = 1; mn < (MN - 1); mn++)
            {
               FFTLIB_UTIL_SE0AloadSE1BloadComputeCSA0Cstore(MMA_SIZE, (uint8_t *)dst);
            }
            
            // swap    Bload(0,     N - 1) to Bfore(0,     N - 1)
            // swap    Cfore(M - 1, N - 2) to Cback(M - 1, N - 2)
            // load    Afore(M - 1,     0)
            // store   Cback(M - 1, N - 2)
            // compute Cfore(M - 1, N - 1) = Afore(M - 1, 0)*Bfore(0, N - 1)
            // Note: this loop is fully unrolled to allow for prolog-epilog (PE) merging with the loop above it.
            FFTLIB_UTIL_SE0AloadComputeCSA0Cstore_unroll<MMA_SIZE>(MMA_SIZE, (uint8_t *)dst);

            
            // swap  Cfore(M - 1, N - 1) to Cback(M - 1, N - 1)
            // store Cback(M - 1, N - 1)
            // Note: this loop is fully unrolled to allow for prolog-epilog (PE) merging with the loop above it.
            FFTLIB_UTIL_SA0Cstore_unroll<MMA_SIZE>(MMA_SIZE, (uint8_t *)dst);

         } else if(MN == 3) {
            _nassert(MN == 3);
            // Work-around for JIRA CODEGEN-4452
            // In the 32-bit case, the utility function would only iterate 16 times while the latency for the store is
            // 24 cycles.  The compiler uses a strategy called iteration pruning to schedule this loop, but it is still
            // not able to fully PE schedule.  Instead, we fully unroll the loop so it can PE schedule with the
            // function following it (which is also unrolled).
            //
            // However, for the 8- and 16-bit versions, the loop runs 64 and 32 iterations, enough to cover the 24 cycle
            // store latency.  From a program cache perspective, it is better to keep these as a loop.  In fact, the
            // 8-bit case may have worse performance if fully unrolled b/c the compiler has a limit on the number
            // of store operations in a single block of code.  For 8-bit data, this code has 3*64 store operations (all
            // three unrolled functions) which ran into this limit.
            if(MMA_SIZE == FFTLIB_MMA_SIZE_32_BIT){
               FFTLIB_UTIL_SE0AloadSE1BloadComputeCSA0Cstore_unroll<MMA_SIZE>(MMA_SIZE, (uint8_t *)dst);
            } else {
               FFTLIB_UTIL_SE0AloadSE1BloadComputeCSA0Cstore(MMA_SIZE, (uint8_t *)dst);
            }
            // swap    Bload(0,     N - 1) to Bfore(0,     N - 1)
            // swap    Cfore(M - 1, N - 2) to Cback(M - 1, N - 2)
            // load    Afore(M - 1,     0)
            // store   Cback(M - 1, N - 2)
            // compute Cfore(M - 1, N - 1) = Afore(M - 1, 0)*Bfore(0, N - 1)
            FFTLIB_UTIL_SE0AloadComputeCSA0Cstore_unroll<MMA_SIZE>(MMA_SIZE, (uint8_t *)dst);
            
            // swap  Cfore(M - 1, N - 1) to Cback(M - 1, N - 1)
            // store Cback(M - 1, N - 1)

            FFTLIB_UTIL_SA0Cstore_unroll<MMA_SIZE>(MMA_SIZE, (uint8_t *)dst);
         } else {  // effectively MN == 2
            _nassert(MN==2);
            // swap    Bload(0,     N - 1) to Bfore(0,     N - 1)
            // swap    Cfore(M - 1, N - 2) to Cback(M - 1, N - 2)
            // load    Afore(M - 1,     0)
            // store   Cback(M - 1, N - 2)
            // compute Cfore(M - 1, N - 1) = Afore(M - 1, 0)*Bfore(0, N - 1)
            FFTLIB_UTIL_SE0AloadComputeCSA0Cstore_unroll<MMA_SIZE>(MMA_SIZE, (uint8_t *)dst);
            
            // swap  Cfore(M - 1, N - 1) to Cback(M - 1, N - 1)
            // store Cback(M - 1, N - 1)
            FFTLIB_UTIL_SA0Cstore_unroll<MMA_SIZE>(MMA_SIZE, (uint8_t *)dst);
         }

      }
      // case 4: M*N > 1, K > 1
      else
      {
         FFTLIB_DEBUGPRINTFN(1, "Case 4%s", "\n");
         _nassert(K >= 2);
         // load    Bload(0, 0)
         FFTLIB_UTIL_SE1Bload(MMA_SIZE);
         
         //
         // mn = 0
         //
         
         // swap    Bload(k,     0) to Bfore(k, 0)
         // load    Afore(0,     k)
         // load    Bload(k + 1, 0); note that Bload(K, 0) -> Bload(0, 1)
         // compute Cfore(0,     0) += Afore(0, k)*Bfore(k, 0)
         _nassert(K >= 2);
         for (k = 0; k < K; k++)
         {
            FFTLIB_UTIL_SE0AloadSE1BloadComputeC(MMA_SIZE);
         }
         
         if(K >= 3){
            _nassert(K >= 3);
            //
            // mn = 1 to MN - 2
            //
            for (mn = 1; mn < (MN - 1); mn++)
            {
               // swap    Bload(curr) to Bfore(curr)
               // swap    Cfore(prev) to Cback(prev)
               // load    Afore(curr)
               // load    Bload(next)
               // store   Cback(prev)
               // compute Cfore(curr) = Afore(curr)*Bfore(curr)
               FFTLIB_UTIL_SE0AloadSE1BloadComputeCSA0Cstore(MMA_SIZE, (uint8_t *)dst);
               
               // unroll one iteration of K to allow for loop merging with prior loop
               FFTLIB_UTIL_SE0AloadSE1BloadComputeC_unroll<MMA_SIZE>(MMA_SIZE);

               
               // swap    Bload(curr) to Bfore(curr)
               // load    Afore(curr)
               // load    Bload(next)
               // compute Cfore(curr) += Afore(curr)*Bfore(curr)
               for (k = 2; k < K; k++)
               {
                  FFTLIB_UTIL_SE0AloadSE1BloadComputeC(MMA_SIZE);
               }
            }
            
            //
            // mn = MN - 1
            //
            
            // swap    Bload(0,     N - 1) to Bfore(0,     N - 1)
            // swap    Cfore(M - 1, N - 2) to Cback(M - 1, N - 2)
            // load    Afore(M - 1,     0)
            // load    Bload(1,     N - 1)
            // store   Cback(M - 1, N - 2)
            // compute Cfore(M - 1, N - 1) = Afore(M - 1, 0)*Bfore(0, N - 1)
            FFTLIB_UTIL_SE0AloadSE1BloadComputeCSA0Cstore(MMA_SIZE, (uint8_t *)dst);
            
            // swap    Bload(k,     N - 1) to Bfore(k, N - 1)
            // load    Afore(M - 1,     k)
            // load    Bload(k + 1, N - 1)
            // compute Cfore(M - 1, N - 1) += Afore(M - 1, k)*Bfore(k, N - 1)
            // peel off an iteration for loop merging with above
            FFTLIB_UTIL_SE0AloadSE1BloadComputeC_unroll<MMA_SIZE>(MMA_SIZE);

            for (k = 2; k < (K - 1); k++)
            {
               FFTLIB_UTIL_SE0AloadSE1BloadComputeC(MMA_SIZE);
            }
            
            // swap    Bload(K - 1, N - 1) to Bfore(K - 1, N - 1)
            // load    Afore(M - 1, K - 1)
            // compute Cfore(M - 1, N - 1) += Afore(M - 1, K - 1)*Bfore(K - 1, N - 1)
            FFTLIB_UTIL_SE0AloadComputeC(MMA_SIZE);
         } else { // K == 2
            
            //
            // mn = 1 to MN - 2
            //
            //#pragma NO_COALESCE_LOOP  // resolves performance issue introduced with alpha-5 compiler where the compiler makes a poor coalesce choice
            for (mn = 1; mn < (MN - 1); mn++)
            {
               // swap    Bload(curr) to Bfore(curr)
               // swap    Cfore(prev) to Cback(prev)
               // load    Afore(curr)
               // load    Bload(next)
               // store   Cback(prev)
               // compute Cfore(curr) = Afore(curr)*Bfore(curr)
               if(MMA_SIZE == FFTLIB_MMA_SIZE_32_BIT){
                  // For the 32-bit case, the xfer-rcv-store latency is too long to be covered by the subsequent block.
                  // Use this if condition to unroll it only for the 32-bit case.  The compiler should recognize the "if"
                  // can be evaluated at compile time during templating and only include the appropriate code in the
                  // generated template.
                  FFTLIB_UTIL_SE0AloadSE1BloadComputeCSA0Cstore_unroll<MMA_SIZE>(MMA_SIZE, (uint8_t *)dst);
               } else {
                  FFTLIB_UTIL_SE0AloadSE1BloadComputeCSA0Cstore(MMA_SIZE, (uint8_t *)dst);
               }
               
               // only one iteration of K, fully unroll it to allow for loop merging with prior loop
               FFTLIB_UTIL_SE0AloadSE1BloadComputeC_unroll<MMA_SIZE>(MMA_SIZE);
            }
            
            //
            // mn = MN - 1
            //
            
            // swap    Bload(0,     N - 1) to Bfore(0,     N - 1)
            // swap    Cfore(M - 1, N - 2) to Cback(M - 1, N - 2)
            // load    Afore(M - 1,     0)
            // load    Bload(1,     N - 1)
            // store   Cback(M - 1, N - 2)
            // compute Cfore(M - 1, N - 1) = Afore(M - 1, 0)*Bfore(0, N - 1)
            FFTLIB_UTIL_SE0AloadSE1BloadComputeCSA0Cstore(MMA_SIZE, (uint8_t *)dst);
            
            // swap    Bload(K - 1, N - 1) to Bfore(K - 1, N - 1)
            // load    Afore(M - 1, K - 1)
            // compute Cfore(M - 1, N - 1) += Afore(M - 1, K - 1)*Bfore(K - 1, N - 1)
            FFTLIB_UTIL_SE0AloadComputeC_unroll<MMA_SIZE>(MMA_SIZE);

         } // end of if(K >= 3){} else{};
         
         // swap  Cfore(M - 1, N - 1) to Cback(M - 1, N - 1)
         // store Cback(M - 1, N - 1)
         FFTLIB_UTIL_SA0Cstore(MMA_SIZE, (uint8_t *)dst);
      }
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
   __HWACLOSE(0);

   return FFTLIB_SUCCESS;
}

// explicit instantiations for different execute functions
template FFTLIB_STATUS FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec_ci <FFTLIB_MMA_SIZE_8_BIT>  (FFTLIB_kernelHandle handle,
                                                                                          const void *src0,
                                                                                          const void *src1,
                                                                                          void *dst);

template FFTLIB_STATUS FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec_ci <FFTLIB_MMA_SIZE_16_BIT> (FFTLIB_kernelHandle handle,
                                                                                          const void *src0,
                                                                                          const void *src1,
                                                                                          void *dst);

template FFTLIB_STATUS FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec_ci <FFTLIB_MMA_SIZE_32_BIT> (FFTLIB_kernelHandle handle,
                                                                                          const void *src0,
                                                                                          const void *src1,
                                                                                          void *dst);

