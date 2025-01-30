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

/**********************************************************************************************************************/
/*                                                                                                                    */
/* INCLUDES                                                                                                           */
/*                                                                                                                    */
/**********************************************************************************************************************/

#include "VXLIB_meanStdDev_priv.h"
#include "VXLIB_types.h"
#include "c7x_scalable.h"

/**********************************************************************************************************************/
/*                                                                                                                    */
/* DEFINES                                                                                                            */
/*                                                                                                                    */
/**********************************************************************************************************************/

#define SE_PARAM_BASE (0x0000)
#define SE0_PARAM_OFFSET (SE_PARAM_BASE)

// this method computes the square root of a scalar value
static inline double sqrtdp(double a)
{

   double half  = 0.5;
   double oneP5 = 1.5;

   double y, p0, p1, d0;
   double double_max = std::numeric_limits<double>::max();

   p0 = __recip_sqrt(a);
   d0 = p0 * a;
   p1 = oneP5 - d0 * p0 * half;
   y  = a * p0 * p1;

   if (a < 0)
      y = 0;

   if (a > double_max)
      y = double_max;

   return y;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_meanStdDev_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method initializes the kernel-specific parameters
// mainly, the streaming engine and streaming address generators
template <uint32_t dTypeIn, uint32_t dTypeOut>
VXLIB_STATUS VXLIB_meanStdDev_init_ci(VXLIB_kernelHandle               handle,
                                      const VXLIB_bufParams2D_t       *bufParamsIn,
                                      const VXLIB_meanStdDev_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   // structs to hold SE and SA parameters
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_meanStdDev_PrivArgs *pKerPrivArgs = (VXLIB_meanStdDev_PrivArgs *) handle;

   // obtain image size and compute number of blocks to process
   size_t width    = pKerPrivArgs->width;
   size_t height   = pKerPrivArgs->height;
   size_t strideIn = pKerPrivArgs->strideInElements;

   size_t elemCount = 0;
   if (VXLIB_MEANSTDDEV_I8U_O32F)
      elemCount = c7x::element_count_of<c7x::uchar_qvec>::value;
   else if (VXLIB_MEANSTDDEV_I16U_O32F)
      elemCount = c7x::element_count_of<c7x::ushort_qvec>::value;
   else
      status = VXLIB_ERR_INVALID_TYPE;

   size_t wBlocks          = VXLIB_ceilingDiv(width, elemCount);
   size_t numBlocks        = height * wBlocks;
   pKerPrivArgs->numBlocks = numBlocks;

   uint8_t *pBlock = pKerPrivArgs->bufPblock; // address to retrieve to store SE/SA params

   if (strideIn == width) {

      se0Params.DIMFMT = __SE_DIMFMT_1D;
      se0Params.ICNT0  = width * height;
   }

   else {

      se0Params.DIMFMT = __SE_DIMFMT_3D;
      se0Params.ICNT0  = elemCount;

      se0Params.DIM1  = strideIn;
      se0Params.ICNT1 = height;

      se0Params.DIM2  = elemCount;
      se0Params.ICNT2 = VXLIB_ceilingDiv(width, elemCount);

      se0Params.DECDIM1       = __SE_DECDIM_DIM2;
      se0Params.DECDIM1_WIDTH = width;
      se0Params.DECDIM1SD     = __SE_DECDIMSD_DIM0;
   }

   se0Params.PROMOTE = __SE_PROMOTE_4X_ZEROEXT;

   if (VXLIB_MEANSTDDEV_I8U_O32F) {
      se0Params.ELETYPE = c7x::se_eletype<c7x::uchar_vec>::value;
      se0Params.VECLEN  = c7x::se_veclen<c7x::uchar_qvec>::value;
   }

   else if (VXLIB_MEANSTDDEV_I16U_O32F) {
      se0Params.ELETYPE = c7x::se_eletype<c7x::ushort_vec>::value;
      se0Params.VECLEN  = c7x::se_veclen<c7x::ushort_qvec>::value;
   }
   else
      status = VXLIB_ERR_INVALID_TYPE;

   /**************************/
   /* Store SE and SA params */
   /**************************/

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET) = se0Params;

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_meanStdDev_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS
VXLIB_meanStdDev_init_ci<VXLIB_MEANSTDDEV_DTYPE_I8U_O32F>(VXLIB_kernelHandle               handle,
                                                          const VXLIB_bufParams2D_t       *bufParamsIn,
                                                          const VXLIB_meanStdDev_InitArgs *pKerInitArgs);

template VXLIB_STATUS
VXLIB_meanStdDev_init_ci<VXLIB_MEANSTDDEV_DTYPE_I16U_O32F>(VXLIB_kernelHandle               handle,
                                                           const VXLIB_bufParams2D_t       *bufParamsIn,
                                                           const VXLIB_meanStdDev_InitArgs *pKerInitArgs);

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_meanStdDev_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method calculates the mean and standard deviation of an input image
template <typename dTypeIn, typename dTypeOut>
VXLIB_STATUS VXLIB_meanStdDev_exec_ci(VXLIB_kernelHandle handle,
                                      void *restrict pIn,
                                      void *restrict pOut0,
                                      void *restrict pOut1,
                                      void *restrict pPixelsProcessed,
                                      void *restrict pCurrentSum,
                                      void *restrict pCurrentSqSum)
{

   // status
   VXLIB_STATUS status = VXLIB_SUCCESS;

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_meanStdDev_PrivArgs *pKerPrivArgs = (VXLIB_meanStdDev_PrivArgs *) handle;

   // structs to retrieve SE and SA paramters
   __SE_TEMPLATE_v1 se0Params;

   // vector typedefs
   typedef typename std::conditional<std::is_same<dTypeIn, uint8_t>::value, uint32_t, uint64_t>::type dTypeAcc;

   typedef typename std::conditional<std::is_same<dTypeIn, uint8_t>::value, c7x::uint_vec, c7x::ulong_vec>::type
       dTypeAccVec;

   // create local pointers
   dTypeIn *restrict pInLocal    = (dTypeIn *) pIn;
   dTypeOut *restrict pOut0Local = (dTypeOut *) pOut0;
   dTypeOut *restrict pOut1Local = (dTypeOut *) pOut1;

   uint32_t *restrict pPixelsProcessedLocal = (uint32_t *) pPixelsProcessed;
   dTypeAcc *restrict pCurrentSumLocal      = (dTypeAcc *) pCurrentSum;
   dTypeAcc *restrict pCurrentSqSumLocal    = (dTypeAcc *) pCurrentSqSum;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_meanStdDev_exec_ci\n");
#endif

   // address of SE and SA parameters
   uint8_t *pBlock    = pKerPrivArgs->bufPblock;
   size_t   numBlocks = pKerPrivArgs->numBlocks;
   size_t   elemCount;

   if (VXLIB_MEANSTDDEV_I8U_O32F_TEMPLATE(dTypeIn, dTypeOut))
      elemCount = c7x::element_count_of<c7x::uchar_qvec>::value;
   else if (VXLIB_MEANSTDDEV_I16U_O32F_TEMPLATE(dTypeIn, dTypeOut))
      elemCount = c7x::element_count_of<c7x::ushort_qvec>::value;
   else
      status = VXLIB_ERR_INVALID_TYPE;

   size_t width  = pKerPrivArgs->width;
   size_t height = pKerPrivArgs->height;
   size_t stride = pKerPrivArgs->strideInElements;

   if (stride == width)
      numBlocks = VXLIB_ceilingDiv((height * width), elemCount);

   // retrieve SE and SA parameters
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);

   // open SEs to fetch Input samples
   __SE0_OPEN(pInLocal, se0Params);

   // declare variables and accumulators
   dTypeAccVec a;
   dTypeAccVec acc, acc_sq;

   acc    = (dTypeAccVec) (0);
   acc_sq = (dTypeAccVec) (0);

   uint32_t N = width * height;

   for (int32_t counter = 0; counter < numBlocks; counter++) {

      // fetch se0++
      a = c7x::strm_eng<0, dTypeAccVec>::get_adv();

      //  accumulate sum
      acc += a;

      // accumulate square sum via dotprod or mult
      acc_sq += (a * a);
   }

   // horizontal add sum
   dTypeAcc sum = __horizontal_add(acc);

   // horizontal add sq sum
   dTypeAcc sum_sq = __horizontal_add(acc_sq);

   // update pixels processed
   *pPixelsProcessedLocal += N;

   //  update sum
   *pCurrentSumLocal += sum;

   // update square sum
   *pCurrentSqSumLocal += sum_sq;

   //  calculate mean from sum

   double sum_f    = (double) *pCurrentSumLocal;
   double sum_sq_f = (double) *pCurrentSqSumLocal;

   *pOut0Local = (dTypeOut) (sum_f / (*pPixelsProcessedLocal));

   // calculate standard deviation from sum and square sum
   double variance = (sum_sq_f - ((sum_f * sum_f) / (*pPixelsProcessedLocal))) / (*pPixelsProcessedLocal);

   *pOut1Local = (dTypeOut) sqrtdp(variance);

   // close SE/SA
   __SE0_CLOSE();

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_meanStdDev_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS VXLIB_meanStdDev_exec_ci<VXLIB_MEANSTDDEV_TYPENAME_I8U_O32F>(VXLIB_kernelHandle handle,
                                                                                   void *restrict pIn,
                                                                                   void *restrict pOut0,
                                                                                   void *restrict pOut1,
                                                                                   void *restrict pPixelsProcessed,
                                                                                   void *restrict pCurrentSum,
                                                                                   void *restrict pCurrentSqSum);

template VXLIB_STATUS VXLIB_meanStdDev_exec_ci<VXLIB_MEANSTDDEV_TYPENAME_I16U_O32F>(VXLIB_kernelHandle handle,
                                                                                    void *restrict pIn,
                                                                                    void *restrict pOut0,
                                                                                    void *restrict pOut1,
                                                                                    void *restrict pPixelsProcessed,
                                                                                    void *restrict pCurrentSum,
                                                                                    void *restrict pCurrentSqSum);
