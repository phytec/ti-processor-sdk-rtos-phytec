/*
*
* Copyright (c) {2015 - 2020} Texas Instruments Incorporated
*
* All rights reserved not granted herein.
*
* Limited License.
*
* Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive
* license under copyrights and patents it now or hereafter owns or controls to make,
* have made, use, import, offer to sell and sell ("Utilize") this software subject to the
* terms herein.  With respect to the foregoing patent license, such license is granted
* solely to the extent that any such patent is necessary to Utilize the software alone.
* The patent license shall not apply to any combinations which include this software,
* other than combinations with devices manufactured by or for TI ("TI Devices").
* No hardware patent is licensed hereunder.
*
* Redistributions must preserve existing copyright notices and reproduce this license
* (including the above copyright notice and the disclaimer and (if applicable) source
* code license limitations below) in the documentation and/or other materials provided
* with the distribution
*
* Redistribution and use in binary form, without modification, are permitted provided
* that the following conditions are met:
*
* *       No reverse engineering, decompilation, or disassembly of this software is
* permitted with respect to any software provided in binary form.
*
* *       any redistribution and use are licensed by TI for use only with TI Devices.
*
* *       Nothing shall obligate TI to provide you with source code for the software
* licensed and provided to you in object code.
*
* If software source code is provided to you, modification and redistribution of the
* source code are permitted provided that the following conditions are met:
*
* *       any redistribution and use of the source code, including any resulting derivative
* works, are licensed by TI for use only with TI Devices.
*
* *       any redistribution and use of any object code compiled from the source code
* and any resulting derivative works, are licensed by TI for use only with TI Devices.
*
* Neither the name of Texas Instruments Incorporated nor the names of its suppliers
*
* may be used to endorse or promote products derived from this software without
* specific prior written permission.
*
* DISCLAIMER.
*
* THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/
#ifndef TIDL_CUDA_
#define TIDL_CUDA_

#ifdef BUILD_WITH_CUDA
template <class Tin, class Tw, class Tb, class Tacc>
int TIDL_cudnnConvolve2d(
  Tin       *pInchanneli,
  Tw        *pCoeffsi,
  Tb        *pBias,
  Tacc      *accPtri,
  Tacc      *min,
  Tacc      *max,
  int32_t  numTotRoi,
  int32_t  numGroups,
  int32_t  numInChannels,
  int32_t  numOutChannels,
  int32_t  inChPitch,
  int32_t  outChPitch,
  int32_t  width,
  int32_t  height,
  int32_t  inImPitch,
  int32_t  outImPitch,
  int32_t  coeffsWidth,
  int32_t  coeffsHeight,
  int32_t  dilationWidth,
  int32_t  dilationHeight,
  int32_t  strideWidth,
  int32_t  strideHeight,
  int p_w,
  int p_h
);

template <class Tin, class Tw, class Tb, class Tacc>
int TIDL_cudaConvolve2d(
  Tin*     pInChannel,
  Tw*      pCoeffs,
  Tb*      pBias,
  Tacc *   accPtr,
  Tacc *   min,
  Tacc *   max,
  int32_t  numTotRoi,
  int32_t  numGroups,
  int32_t  numInChannels,
  int32_t  numOutChannels,
  int32_t  inChPitch,
  int32_t  outChPitch,
  int32_t  width,
  int32_t  height,
  int32_t  inImPitch,
  int32_t  outImPitch,
  int32_t  coeffsWidth,
  int32_t  coeffsHeight,
  int32_t  dilationWidth,
  int32_t  dilationHeight,
  int32_t  strideWidth,
  int32_t  strideHeight,
  int32_t enableBias,
  int32_t isOTFpad,
  int32_t leftPad,
  int32_t topPad,
  int32_t padVal,
  int32_t startRowNumberInTensor,
  int32_t inHeight,
  int32_t inWidth,
  int32_t outHeight,
  int32_t outWidth
  );
void TIDL_cudaSetInitFlag(int32_t layerIdx);
template <class Tin, class Tacc>
int TIDL_cudaEltsum(Tin* pIn, Tacc* pAcc, int scale, int numOutChannels, int height, int width, int outChPitch, int inChPitch, int inPitch, int outPitch, int maxcalls, int callno);
template <class Tx>
void minmaxGPU(const int numTotRoi, const Tx* x, int *min, int *max);
template <class Tx>
void minmax_Thrust(const int N,const Tx* x,Tx *min,Tx *max);
void TIDL_cudaFreeConvPtrs();
void TIDL_cudaFreeConvCudaPtrs();
//void TIDL_cudaFreeEltPtrs();
void TIDL_freeMaxPtrs();
void TIDL_cudaFreeConcatPtrs();
void TIDL_minmaxElt(int *min, int*max, int numTotRoi);
void TIDL_minmaxCat(int *min, int*max, int numTotRoi);
template<class Tout>
int TIDL_cudaEltSatCpy(Tout* pOut, int numTotRoi, int numOutChannels, int height, int width, int outChPitch, int outPitch, int satLow, int satHigh, int roundBits);
template<class Tout,class Tacc>
int TIDL_cudaCatSatCopy(Tout* pOut, Tacc* pAcc, int numTotRoi, int numOutChannels, int height, int width, int outChPitch, int outPitch, int satLow, int satHigh, int roundBits, int maxcalls);
template <class Tout>
int TIDL_cudaSaturate(Tout **devPtrOf, int numTotRoi, int numOutChannels, int height, int width,int outChPitch, int outImPitch, int outRoundBits, int satLow, int satHigh);
template <class Tacc, class Tout>
int TIDL_cudaSaturateV1(Tout **devPtrOf, int N, int C, int H, int W, int outChPitch, int outImPitch, int outRoundBits, Tacc satLow, Tacc satHigh);
template <class Tacc, class Tout>
int TIDL_cudaSaturateFixedPoint(Tout **devPtrOf, int N, int C, int H, int W, int outChPitch, int outImPitch, uint8_t *outRoundBits, int32_t satLow, int32_t satHigh, int32_t enablePerChannelShift, int32_t precisionAdjustmentShift);
template <class Tacc, class Tout, class Tin>
int TIDL_cudaSaturateFixedPointAsym(Tout **devPtrOf, int N, int C, int H, int W, int outChPitch, int outImPitch, int32_t satLow, int32_t satHigh, uint8_t* pDerivedScales, uint8_t* pDerivedShifts, int32_t padFillValue);
template<class Tout>
int TIDL_cudaOutputTx(Tout *devPtrOf, int sizeOstream, int numTotRoi, int numOutChannels, int height, int width, int outChPitch, int outImPitch, int strideHeight, int strideWidth, Tout* pOutChanne);
template <class Tin> int
TIDL_cudaMaxpool( Tin *inData, Tin *outData, int numOutChannels, int height, int width, int kernelH, int kernelW, int inChPitch, int outChPitch, int inPitch, int outPitch, int initValue, int strideW, int strideH);
template<class Tin,class Tacc>
int TIDL_cudaConcatenate(Tin* pIn, Tacc* pAcc, int scale, int numOutChannels, int height, int width, int inChPitch, int outChPitch, int inPitch, int outPitch, int maxcalls, int callno, int ctotal, int axis);
extern int CUDNNLC;
#endif
#endif
