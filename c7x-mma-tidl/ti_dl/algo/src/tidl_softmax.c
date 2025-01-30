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

/**
----------------------------------------------------------------------------
@file    tidl_softmax.c
@brief   This file defines private functions for Softmax layer.
@version 0.1 (Feb 2017) : Initial version [ADK]
----------------------------------------------------------------------------
 */

#include "tidl_alg_int.h"
#include "tidl_commonUtils.h"
#include "tidl_device_utils.h"
#include "tidl_softmax.h"
#include "tidl_lut_module.h"
#include "math.h"
#include "float.h"

//#define lutTensorScale (1.0/255.0) /* This is an intermediate variable set by user */
#define lutTensorZP (0.0) /* This is an intermediate variable set by user */
#define SCALE_PRECISION_BITS 8 /* This parameter specifies the precision to which the scale of float should be calculated while converting float to scale and shift */
#define FLOAT_SIMD_WIDTH ((uint32_t)(VEC_NELEM(float))) // This macro defines the max number of float elements that can be present in a full c7x vector. Since the softmax kernel uses float vectors a lot, it is useful to have this macro defined.

template <class Tin, class Tout, class Tacc>
int32_t TIDL_refSoftmaxProcessInt(
    Tin * inData,
    Tout * outData,
    float32_tidl tensorScale,
    int32_t imWidth,
    int32_t inPitch,
    int32_t outPitch,
    int32_t numBatches,
    int32_t inBatchPitch,
    int32_t outBatchPitch,
    sTIDL_DataParams_t  * inDataParams,
    sTIDL_DataParams_t  * outDataParams,
    int32_t axis,
    int32_t outTranspose
)
{
  Tin * inPtr   = inData;
  Tout * outPtr = outData;
  Tin outputTensorZP = outDataParams->tensorZeroPoint;
  float32_tidl outputTensorScale = outDataParams->tensorScale;
  float32_tidl inputTensorScale = inDataParams->tensorScale;
  float32_tidl inputTensorScaleInv = ((float32_tidl)1)/inputTensorScale;
  #if defined (__C7100__) || defined (__C7120__)
  float32_tidl lutTensorScale = 1/255.0;
  #endif
  float32_tidl quantScale = 255.0f;
  if(typeid(Tin) == typeid(int16_t) || typeid(Tin) == typeid(uint16_t))
  {
    #if defined (__C7100__) || defined (__C7120__)
    lutTensorScale = 1/65535.0;
    #endif
    quantScale = 65535.0f;
  }
  int32_t icnt[TIDL_DIM_MAX] = {0};
  int32_t dim[TIDL_DIM_MAX] = {0};
  int32_t ddim[TIDL_DIM_MAX] = {0};
  /*Set dims & counts:*/
  /*DIM0*/
  icnt[0] = inDataParams->dimValues[TIDL_DIM_WIDTH];
  dim[0]  = 1U;
  /*DIM1*/
  icnt[1] = inDataParams->dimValues[TIDL_DIM_HEIGHT];
  dim[1]  = inDataParams->pitch[TIDL_LINE_PITCH];

  if(outTranspose)
  {
    /*DIM0*/
    ddim[0]  = outDataParams->pitch[TIDL_LINE_PITCH];
    /*DIM1*/
    ddim[1]  = 1U;
  }
  else{
    /*DIM0*/
    ddim[0]  = 1U;
    /*DIM1*/
    ddim[1]  = outDataParams->pitch[TIDL_LINE_PITCH];
  }
  /*DIM2*/
  icnt[2] = inDataParams->dimValues[TIDL_DIM_NUMCH];
  dim[2]  = inDataParams->pitch[TIDL_CHANNEL_PITCH];
  ddim[2]  = outDataParams->pitch[TIDL_CHANNEL_PITCH];
  /*DIM3*/
  icnt[3] = inDataParams->dimValues[TIDL_DIM_DIM2];
  dim[3]  = inDataParams->pitch[TIDL_DIM2_PITCH];
  ddim[3]  = outDataParams->pitch[TIDL_DIM2_PITCH];
  /*DIM4*/
  icnt[4] = inDataParams->dimValues[TIDL_DIM_DIM1];
  dim[4]  = inDataParams->pitch[TIDL_DIM1_PITCH];
  ddim[4]  = outDataParams->pitch[TIDL_DIM1_PITCH];
  /*DIM5*/
  icnt[5] = inDataParams->dimValues[TIDL_DIM_BATCH];
  dim[5]  = inDataParams->pitch[TIDL_ROI_PITCH];
  ddim[5]  = outDataParams->pitch[TIDL_ROI_PITCH];

  /*Use softmax's "axis" parameter to swap the innermost dimension*/
  int32_t icnt_temp, dim_temp, ddim_temp;
  icnt_temp = icnt[0];
  dim_temp  = dim[0];
  ddim_temp = ddim[0];
  /* axis dim -> dim 0*/
  icnt[0] = icnt[TIDL_DIM_MAX - 1 - axis];
  dim[0]  = dim[TIDL_DIM_MAX - 1 - axis];
  ddim[0] = ddim[TIDL_DIM_MAX - 1 - axis];
  /* axis dim <- dim 0*/
  icnt[TIDL_DIM_MAX - 1 - axis] = icnt_temp;
  dim[TIDL_DIM_MAX - 1 - axis]  = dim_temp;
  ddim[TIDL_DIM_MAX - 1 - axis] = ddim_temp;

  int32_t i5, i4, i3, i2, i1, i0;
  /*Compute:*/
  //OPENACC(data copyin(inPtr[: 1+ (icnt[5]-1) * dim[5] + (icnt[4]-1) * dim[4] + (icnt[3]-1) * dim[3] + (icnt[2]-1) * dim[2] + (icnt[1]-1) * dim[1] + (icnt[0]-1) * dim[0]]) \
               copy(outPtr[: 1+ (icnt[5]-1) * ddim[5] + (icnt[4]-1) * ddim[4] + (icnt[3]-1) * ddim[3] + (icnt[2]-1) * ddim[2] + (icnt[1]-1) * ddim[1] + (icnt[0]-1) * ddim[0]]))
  //OPENACC(parallel loop collapse(5))
  for(i5 = 0; i5 < icnt[5]; i5++)
  {
    for(i4 = 0; i4 < icnt[4]; i4++)
    {
      for(i3 = 0; i3 < icnt[3]; i3++)
      {
        for(i2 = 0; i2 < icnt[2]; i2++)
        {
          for(i1 = 0; i1 < icnt[1]; i1++)
          {
            Tin xMax = std::numeric_limits<Tin>::lowest();;
            Tin inDataVal;
            int32_t numTiles = imWidth / TIDL_SIMD_WIDTH;
            if(imWidth % TIDL_SIMD_WIDTH != 0)
            {
              numTiles++;
            }
            Tacc denomSum=0;

            /*Find Max:*/
            //OPENACC(loop)
            for(i0 = 0; i0 < icnt[0]; i0++)
            {
              inDataVal = inPtr[ i5 * dim[5] + i4 * dim[4] + i3 * dim[3] + i2 * dim[2] + i1 * dim[1] + i0 * dim[0]];
              xMax = xMax > inDataVal ? xMax : inDataVal;
            }

            //OPENACC(loop)
            for(i0 = 0; i0 < icnt[0]; i0++)
            {
              Tout expOut;
              if(typeid(Tin) == typeid(int16_t) || typeid(Tout) == typeid(uint16_t) )
              {
                int32_t inputLUT = (int32_t)( inPtr[ i5 * dim[5] + i4 * dim[4] + i3 * dim[3] + i2 * dim[2] + i1 * dim[1] + i0 * dim[0]] - xMax);
                expOut = round((exp_taylor(inputLUT*inputTensorScaleInv) * (quantScale)));/*LUT output calculation, out = (exp(-5∗sx)∗(1/sp))+zp */
              }
              else
              #if defined (__C7504__) || defined (__C7524__)
              {
                int32_t inputLUT = (int32_t)( inPtr[ i5 * dim[5] + i4 * dim[4] + i3 * dim[3] + i2 * dim[2] + i1 * dim[1] + i0 * dim[0]] - xMax);
                expOut = round((exp_taylor(inputLUT*inputTensorScaleInv) * (quantScale)));/*LUT output calculation, out = (exp(-5∗sx)∗(1/sp))+zp */
              }
              #else
              {
                uint8_t inputLUT = (uint8_t)( xMax - inPtr[ i5 * dim[5] + i4 * dim[4] + i3 * dim[3] + i2 * dim[2] + i1 * dim[1] + i0 * dim[0]]);
                expOut = round((exp(-1*inputLUT/inputTensorScale)*(1.0/lutTensorScale))+lutTensorZP);/*LUT output calculation, out = (exp(-5∗sx)∗(1/sp))+zp */
              }
              #endif
              denomSum += expOut;
              outPtr[i5 * ddim[5] + i4 * ddim[4] + i3 * ddim[3] + i2 * ddim[2] + i1 * ddim[1] + i0 * ddim[0]] = (Tout)(expOut);
            }

            float mulFactor = __recip(__convert_float(denomSum))*(outputTensorScale);

            int32_t OutScale, OutShift;
            //OPENACC(routine(TIDL_convertFloatToScaleAndShift))
            TIDL_convertFloatToScaleAndShift(mulFactor, &OutScale, &OutShift, SCALE_PRECISION_BITS);
            uint8_t scaleFactor =(uint8_t)OutScale;
            int32_t absShift = abs(OutShift);

            if(OutShift>=0)
            {
              //OPENACC(loop)
              for(i0 = 0; i0 < icnt[0]; i0++)
              {
                Tout expOut = (Tout)outPtr[i5 * ddim[5] + i4 * ddim[4] + i3 * ddim[3] + i2 * ddim[2] + i1 * ddim[1] + i0 * ddim[0]];
                Tacc prod = (expOut*scaleFactor);
                Tacc shiftAcc = prod<<OutShift;
                Tout shiftOut = (shiftAcc > std::numeric_limits<Tout>::max()) ? std::numeric_limits<Tout>::max() : shiftAcc;
                Tout softmaxOut = (Tout)(shiftOut) + outputTensorZP;
                outPtr[i5 * ddim[5] + i4 * ddim[4] + i3 * ddim[3] + i2 * ddim[2] + i1 * ddim[1] + i0 * ddim[0]] = (Tout)(softmaxOut);
              }
              }
              else
             {
                //OPENACC(loop)
                for(i0 = 0; i0 < icnt[0]; i0++)
                {
                  Tacc expOut = (Tout)outPtr[i5 * ddim[5] + i4 * ddim[4] + i3 * ddim[3] + i2 * ddim[2] + i1 * ddim[1] + i0 * ddim[0]];
                  Tacc prod = (expOut*scaleFactor);
                  Tacc shiftAcc = (prod+(1<<(absShift-1)))>>absShift;
                  Tout shiftOut = (shiftAcc > std::numeric_limits<Tout>::max()) ? std::numeric_limits<Tout>::max() : shiftAcc;
                  Tout softmaxOut = (Tout)(shiftOut) + outputTensorZP;
                  outPtr[i5 * ddim[5] + i4 * ddim[4] + i3 * ddim[3] + i2 * ddim[2] + i1 * ddim[1] + i0 * ddim[0]] = (Tout)(softmaxOut);
                }
              }
            }
          }
        }
      }
    }
  return 0;
}

/**
 * @brief This function is reference implementation of softmax layer
 *
 * @param inData : Pointer to input memory
 * @param outData : Pointer to output memory
 * @param tensorScale : scale for the input tensor
 * @param imWidth : width of the input buffer
 * @param inPitch : pitch of the input buffer
 * @param outPitch : pitch of the output buffer
 * @param numBatches : no of total Batches
 * @param inBatchPitch : Batch pitch of the input buffer
 * @param inDataParams : parameters of the input data buffer
 * @param outDataParams : parameters of the output data buffer
 * @return  IALG_EOK   - Successful
 *          IALG_EFAIL - Unspecified error
 */
template <class Tin, class Tout>
int32_t TIDL_refSoftmaxProcess(
    Tin * inData,
    Tout * outData,
    float32_tidl tensorScale,
    int32_t imWidth,
    int32_t inPitch,
    int32_t outPitch,
    int32_t numBatches,
    int32_t inBatchPitch,
    int32_t outBatchPitch,
    sTIDL_DataParams_t  * inDataParams,
    sTIDL_DataParams_t  * outDataParams,
    int32_t axis,
    int32_t outTranspose
)
{
  float32_tidl maxValue = 0;
  int32_t   maxIndex = 0;
  int32_t i,j;
  float32_tidl temp;
  float32_tidl denom;
  float32_tidl numer;
  float32_tidl output;
  int32_t max, min = TIDL_SAT_LO_INT8;
  Tin * inPtr   = inData;
  Tout * outPtr = outData;

  int32_t numTiles = imWidth / FLOAT_SIMD_WIDTH;

  if(outDataParams->elementType == TIDL_SinglePrecFloat)
  {
    //OPENACC(data copyin(inPtr[:1+((numBatches-1)*inBatchPitch) + (imWidth-1)]) \
                 copy(outPtr[:1+((numBatches-1)*outBatchPitch) + (imWidth-1)]))
    //OPENACC(parallel loop reduction(max:max))
    for (j = 0; j < numBatches; j++)
    {
      if(inDataParams->elementType == TIDL_SignedChar)
      {
        min = TIDL_SAT_LO_INT8;
      }
      else if(inDataParams->elementType == TIDL_UnsignedChar)
      {
        min = TIDL_SAT_LO_UINT8;
      }
      else if(inDataParams->elementType == TIDL_SignedShort)
      {
        min = TIDL_SAT_LO_INT16;
      }
      else if(inDataParams->elementType == TIDL_UnsignedShort)
      {
        min = TIDL_SAT_LO_UINT16;
      }
      max = min;
      denom = 0.0;
      maxValue = 0;
      float32_tidl inDataVal;
      float32_tidl log2baseE = (float32_tidl)0.693147180559945;
      float32_tidl logEbase2 = (float32_tidl)1/(float32_tidl)0.693147180559945;
      float32_tidl oneBy6  = (1 / (float32_tidl)6);
      float32_tidl oneBy24 = (0.25) * (1 / (float32_tidl)6);
      //OPENACC(loop)
      for (i = 0; i < imWidth; i++)
      {
        inDataVal = inPtr[(j*inBatchPitch) + i];
        max = max > inDataVal ? max : inDataVal;
      }

      if(imWidth % FLOAT_SIMD_WIDTH != 0)
      {
        numTiles++;
      }

      float32_tidl vecSim[20];
      for(int k = 0; k < FLOAT_SIMD_WIDTH; k++)
      {
        vecSim[k] = 0;
      }

      //OPENACC(loop seq collapse(2))
      for (i = 0; i < numTiles; i++)
      {
        for(int k = 0; k < FLOAT_SIMD_WIDTH ; k++)
        {

          // Imitating decdim.
          if((i * FLOAT_SIMD_WIDTH + k) >= imWidth)
          {
            inDataVal = min;
          }
          else
          {
            inDataVal = inPtr[(j*inBatchPitch) + (i * FLOAT_SIMD_WIDTH + k)];
          }

          temp = (inDataVal - max) * (1 / tensorScale);

          if(inDataParams->elementType == TIDL_SinglePrecFloat)
          {
            numer = exp(temp);
          }
          else
          {
            temp = logEbase2 * temp;
            int32_t floorTemp = (int32_t)temp;
            float32_tidl floatRes = temp - floorTemp;
            floatRes = floatRes * log2baseE;

            float32_tidl floatRes2 = floatRes * floatRes;
            float32_tidl floatRes3 = floatRes2 * floatRes;
            float32_tidl floatRes4 = floatRes2 * floatRes2;
            numer = 1 + floatRes + (floatRes2 * 0.5);
            numer = numer + (floatRes3 * oneBy6);
            numer = numer + (floatRes4 * oneBy24);
            if(floorTemp > 0)
            {
              numer = numer * ( (1<<16) << (floorTemp) );
            }
            else
            {
              numer = numer * ( (1<<16) >> (-floorTemp) );
            }

            numer = numer * ( 1 / ((float32_tidl)(1<<16)) );
            if(floorTemp < -16 )
              numer = 0;
          }


          output = numer;

          vecSim[k] += numer;

          if (maxValue <= output)
          {
            maxValue = output;
            maxIndex = i * FLOAT_SIMD_WIDTH + k;
          }
          /*tidl_printf(0,"%4d : %6.2f",i, output*100);*/
          if((i * FLOAT_SIMD_WIDTH + k) < imWidth)
            outPtr[(j*outBatchPitch) + (i * FLOAT_SIMD_WIDTH + k)] = (Tout)(output);
        }
      }

      for(int k = 0; k < FLOAT_SIMD_WIDTH; k++)
      {
        denom += vecSim[k];
      }
      float32_tidl denomInv = (1/denom);

      for(i = 0; i < imWidth; i++)
      {
        outPtr[(j*outBatchPitch + i)] = outPtr[(j*outBatchPitch + i)]  * denomInv;
        output = outPtr[(j*outBatchPitch + i)];
      }
    #ifndef BUILD_WITH_OPENACC
      tidl_printf(2,"-------Max Index %4d : %7.2f -------\n", maxIndex, outPtr[(j*outBatchPitch) + maxIndex]);
    #endif
    }
  }
  else //if((inDataParams->elementType == TIDL_SignedChar || inDataParams->elementType == TIDL_UnsignedChar) && (outDataParams->elementType == TIDL_SignedChar || outDataParams->elementType == TIDL_UnsignedChar))
  {
    if(inDataParams->elementType == TIDL_SignedChar || inDataParams->elementType == TIDL_UnsignedChar)
    {
      int32_t rval = TIDL_refSoftmaxProcessInt<Tin, Tout, uint32_t>(
                                                            inData,
                                                            outData,
                                                            tensorScale,
                                                            imWidth,
                                                            inPitch,
                                                            outPitch,
                                                            numBatches,
                                                            inBatchPitch,
                                                            outBatchPitch,
                                                            inDataParams,
                                                            outDataParams,
                                                            axis,
                                                            outTranspose);
    }
    else if(inDataParams->elementType == TIDL_SignedShort || inDataParams->elementType == TIDL_UnsignedShort)
    {
      int32_t rval = TIDL_refSoftmaxProcessInt<Tin, Tout, uint32_t>(
                                                            inData,
                                                            outData,
                                                            tensorScale,
                                                            imWidth,
                                                            inPitch,
                                                            outPitch,
                                                            numBatches,
                                                            inBatchPitch,
                                                            outBatchPitch,
                                                            inDataParams,
                                                            outDataParams,
                                                            axis,
                                                            outTranspose);
    }
    else
    {
      tidl_printf(0,"Unsupported softmax combination!\n");
    }
  }
  TIDL_enableL1DandL2CacheWb();
  return 0;
}


/**
 * @brief This function is the floating point reference implementation of softmax layer
 *
 * @param tidlLayer : Pointer to the common layer parameters
 * @param inPtrs : Pointer to input memory
 * @param outPtr : Pointer to output memory
 * @param inDataParams : parameters of the input data buffer
 * @param outDataParams : parameters of the output data buffer
 * @param basePrmPtr : Copy of softmax layer parameters
 */
int32_t TIDL_SoftMaxProcessFloat(   sTIDL_Layer_t        * tidlLayer,
                                    void                 * inPtrs,
                                    void                 * outPtr,
                                    sTIDL_DataParams_t   * inDataParams[],
                                    sTIDL_DataParams_t   * outDataParams,
                                    uint8_t              * basePrmPtr,
                                    int32_t               outTranspose)
{
  float32_tidl *in   = (float32_tidl*)inPtrs;
  float32_tidl *out  = (float32_tidl*)outPtr;

  int32_t inOffset  = 0;

  int32_t axis = tidlLayer->layerParams.softMaxParams.axis;
  int32_t icnt[TIDL_DIM_MAX] = {0};
  int32_t dim[TIDL_DIM_MAX] = {0};
  int32_t ddim[TIDL_DIM_MAX] = {0};


  float32_tidl max = -FLT_MAX;
  float32_tidl denom = 1.0;
  float32_tidl inDataVal, output;
  float32_tidl * inPtr = &in[inOffset];
  float32_tidl temp, numer;

  /*Set dims & counts:*/
  if(outTranspose)
  {
    /*DIM0*/
    icnt[0] = inDataParams[0]->dimValues[TIDL_DIM_WIDTH];
    dim[0]  = 1U;
    ddim[0]  = outDataParams->pitch[TIDL_LINE_PITCH];
    /*DIM1*/
    icnt[1] = inDataParams[0]->dimValues[TIDL_DIM_HEIGHT];
    dim[1]  = inDataParams[0]->pitch[TIDL_LINE_PITCH];
    ddim[1]  = 1U;
  }
  else{
    /*DIM0*/
    icnt[0] = inDataParams[0]->dimValues[TIDL_DIM_WIDTH];
    dim[0]  = 1U;
    ddim[0]  = 1U;
    /*DIM1*/
    icnt[1] = inDataParams[0]->dimValues[TIDL_DIM_HEIGHT];
    dim[1]  = inDataParams[0]->pitch[TIDL_LINE_PITCH];
    ddim[1]  = outDataParams->pitch[TIDL_LINE_PITCH];
  }
  /*DIM2*/
  icnt[2] = inDataParams[0]->dimValues[TIDL_DIM_NUMCH];
  dim[2]  = inDataParams[0]->pitch[TIDL_CHANNEL_PITCH];
  ddim[2]  = outDataParams->pitch[TIDL_CHANNEL_PITCH];
  /*DIM3*/
  icnt[3] = inDataParams[0]->dimValues[TIDL_DIM_DIM2];
  dim[3]  = inDataParams[0]->pitch[TIDL_DIM2_PITCH];
  ddim[3]  = outDataParams->pitch[TIDL_DIM2_PITCH];
  /*DIM4*/
  icnt[4] = inDataParams[0]->dimValues[TIDL_DIM_DIM1];
  dim[4]  = inDataParams[0]->pitch[TIDL_DIM1_PITCH];
  ddim[4]  = outDataParams->pitch[TIDL_DIM1_PITCH];
  /*DIM5*/
  icnt[5] = inDataParams[0]->dimValues[TIDL_DIM_BATCH];
  dim[5]  = inDataParams[0]->pitch[TIDL_ROI_PITCH];
  ddim[5]  = outDataParams->pitch[TIDL_ROI_PITCH];

  /*Use softmax's "axis" parameter to swap the innermost dimension*/
  int32_t icnt_temp, dim_temp, ddim_temp;
  icnt_temp = icnt[0];
  dim_temp  = dim[0];
  ddim_temp = ddim[0];
  /* axis dim -> dim 0*/
  icnt[0] = icnt[TIDL_DIM_MAX - 1 - axis];
  dim[0]  = dim[TIDL_DIM_MAX - 1 - axis];
  ddim[0] = ddim[TIDL_DIM_MAX - 1 - axis];
  /* axis dim <- dim 0*/
  icnt[TIDL_DIM_MAX - 1 - axis] = icnt_temp;
  dim[TIDL_DIM_MAX - 1 - axis]  = dim_temp;
  ddim[TIDL_DIM_MAX - 1 - axis] = ddim_temp;

  int32_t i0, i1, i2, i3, i4, i5;

  //OPENACC(data copyin(inPtr[: 1+ (icnt[5]-1) * dim[5] + (icnt[4]-1) * dim[4] + (icnt[3]-1) * dim[3] + (icnt[2]-1) * dim[2] + (icnt[1]-1) * dim[1] + (icnt[0]-1) * dim[0]]) \
               copy(out[: 1+ (icnt[5]-1) * ddim[5] + (icnt[4]-1) * ddim[4] + (icnt[3]-1) * ddim[3] + (icnt[2]-1) * ddim[2] + (icnt[1]-1) * ddim[1] + (icnt[0]-1) * ddim[0]]))
  //OPENACC(parallel loop collapse(5))
  for(i5 = 0; i5 < icnt[5]; i5++)
  {
    for(i4 = 0; i4 < icnt[4]; i4++)
    {
      for(i3 = 0; i3 < icnt[3]; i3++)
      {
        for(i2 = 0; i2 < icnt[2]; i2++)
        {
          for(i1 = 0; i1 < icnt[1]; i1++)
          {
            max = -FLT_MAX;
            denom = 0.0;
            //OPENACC(loop)
            /*Find Max:*/
            for(i0 = 0; i0 < icnt[0]; i0++)
            {
              inDataVal = inPtr[ i5 * dim[5] + i4 * dim[4] + i3 * dim[3] + i2 * dim[2] + i1 * dim[1] + i0 * dim[0]];
              max = max > inDataVal ? max : inDataVal;
            }
            //OPENACC(loop)
            /*Find Denominator*/
            for(i0 = 0; i0 < icnt[0]; i0++)
            {
              inDataVal = inPtr[ i5 * dim[5] + i4 * dim[4] + i3 * dim[3] + i2 * dim[2] + i1 * dim[1] + i0 * dim[0]];
              /*Subtract "max" for mathematical stability*/
              temp = (inDataVal - max);
              denom += (float32_tidl)exp((float64_tidl)(temp));
            }
            //OPENACC(loop)
            /*Calculate softmax at a per element granularity*/
            for(i0 = 0; i0 < icnt[0]; i0++)
            {
              inDataVal = inPtr[ i5 * dim[5] + i4 * dim[4] + i3 * dim[3] + i2 * dim[2] + i1 * dim[1] + i0 * dim[0]];
              temp = (inDataVal - max);
              numer = (float32_tidl)exp((float64_tidl)(temp));
              output = (numer / denom);
              out[i5 * ddim[5] + i4 * ddim[4] + i3 * ddim[3] + i2 * ddim[2] + i1 * ddim[1] + i0 * ddim[0]] = output;
            }
          }
        }
      }
    }
  }
  return IALG_EOK;
}

/**
 * @brief This function is reference implementation of softmax layer
 *
 * @param tidlLayer : Pointer to the common layer parameters
 * @param inPtrs : Pointer to input memory
 * @param outPtr : Pointer to output memory
 * @param inDataParams : parameters of the input data buffer
 * @param outDataParams : parameters of the output data buffer
 * @param basePrmPtr : Copy of softmax layer parameters
 */
int32_t TIDL_softmaxtProcessSP(
  sTIDL_Layer_t        * tidlLayer,
  void                 * inPtrs,
  void                 * outPtr,
  sTIDL_DataParams_t   * inDataParams[],
  sTIDL_DataParams_t   * outDataParams,
  uint8_t              * basePrmPtr
  )
{
  int32_t   i, maxIndex = 0;
  float32_tidl *in   = (float32_tidl*)inPtrs;
  float32_tidl *out  = (float32_tidl*)outPtr;

  int32_t imWidth   = inDataParams[0]->dimValues[TIDL_DIM_WIDTH];

  int32_t inOffset  = 0;
  int32_t outOffset = 0;

  float32_tidl max = -FLT_MAX;
  float32_tidl denom = 0.0;
  float32_tidl maxValue = 0;
  float32_tidl inDataVal, output;
  float32_tidl * inPtr = &in[inOffset];
  float32_tidl temp, numer;

  //OPENACC(parallel loop)
  for (i = 0; i < imWidth; i++)
  {
    inDataVal = inPtr[i];
    max = max > inDataVal ? max : inDataVal;
  }
  //OPENACC(parallel loop)
  for (i = 0; i < imWidth; i++)
  {
    inDataVal = inPtr[i] ;
    temp = (inDataVal - max);
    denom += (float32_tidl)exp((float64_tidl)(temp));
  }
  //OPENACC(parallel loop)
  for (i = 0; i < imWidth; i++)
  {
    inDataVal = inPtr[i] ;
    temp = (inDataVal - max);
    numer = (float32_tidl)exp((float64_tidl)(temp));
    output = (numer / denom);
    if (maxValue <= output)
    {
      maxValue = output;
      maxIndex = i;
    }
    out[outOffset+i] = (output);
  }
  tidl_printf(1,"-------Max Index %4d : %7.2f -------\n", maxIndex, out[outOffset+maxIndex]);
  return IALG_EOK;
}

static inline int32_t TIDL_softmaxForceRef(const TIDL_NetworkCommonParams  *commonParams,
                                          int32_t layerIdx)
{
  const sTIDL_Layer_t *tidlLayer = &commonParams->net->TIDLLayers[layerIdx];
  int32_t forceRef = 0;

  if (((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_DSP_NATC) == TIDL_FLOW_CTRL_DSP_NATC) &&
      (tidlLayer->outData.elementType == TIDL_SinglePrecFloat))
  {
    /* NatC not currently supported for float output */
    forceRef = 1;
  }

  return forceRef;
}

static int32_t TIDL_softmaxInitLUTParams(sTIDL_Network_t *net,
                                         int32_t layerIdx,
                                         sTIDL_AlgLayer_t *algLayer,
                                         sTIDL_sysMemHandle_t sysMems[TIDL_SYSMEM_MAX])
{
  int32_t status = IALG_EOK;

  sTIDL_Layer_t *tidlLayer  = &net->TIDLLayers[layerIdx];
  sTIDL_DataParams_t * inDataParams = TIDL_getDataParams(net, tidlLayer->inData[0]);
  sTIDL_DataParams_t * outDataParams = &tidlLayer->outData;

  if(TIDL_LUTSupportsDatatype(inDataParams->elementType, outDataParams->elementType) && TIDL_DeviceSupportsLUT(net->deviceName) )
  {
    void *memPtr = NULL;
    /* LUT implementation requires that the L1D memory be reserved fully for the kernel which it will use for the L1D during process call.
      By allocating all the L1D scratch memory upfront, we prevent any following part of the init code from using the L1D mem as scratch
      space. */
    memPtr = (void*) TIDL_getMemoryChunkFromSysmem(sysMems,
                                              sysMems[TIDL_SYSMEM_L1_SCRATCH].size,
                                              TIDL_ALIGNMENT_SIZE,
                                              (uint32_t)TIDL_SYSMEM_L1_SCRATCH,
                                              (uint32_t)IALG_SCRATCH);

    if (memPtr == NULL)
    {
      tidl_printf(0, "LUT implementation of batchNorm expects L1 mem to be reserved entirely for the LUT. Please make sure L1 memory is not used for this layer \n");
      status = TIDL_ERR_FAILURE;
    }
  }

  return status;
}


/**
*******************************************************************************
*  \brief      Function to supply memory requirement for Softmax layer to user
*
*
*  \param [in]  specificParams :  Pointer to specific parameter required for all the layers
*
*  \param [in]  commonParams :  Pointer to common parameter required for all the layers
*
*  \param [in]  layerIdx           : Layer Id of the layer
*
*  \param [out]  memorySize           : Output memory requirement of the layer
*
*  \return     Error:  IALG_EOK ( Successful) else failure
*
*  \remarks
*******************************************************************************
*/
int32_t TIDL_softmaxAllocNew(const TIDL_LayerSpecificParams *layerSpecificParams,
                                   const TIDL_NetworkCommonParams *commonParams,
                                   int32_t layerIdx,
                                   int32_t memorySize[TIDL_LAYER_MEMORY_MAX])
{
  int32_t status = IALG_EOK;
  int32_t forceRef = TIDL_softmaxForceRef(commonParams, layerIdx);

  if (((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) != TIDL_FLOW_CTRL_REF_ONLY) && (forceRef != 1))
  {
    status = TIDL_deviceUtilsCommonAlloc(layerSpecificParams, commonParams, layerIdx, memorySize);
  }
  else
  {
    status = TIDL_deviceCommonRefAlloc(layerSpecificParams, commonParams, layerIdx, memorySize);
  }

  return status;
}

/**
*******************************************************************************
*  \brief      Function to set memory needed by convolution layer
*
*
*  \param [in]  specificParams :  Pointer to specific parameter required for all the layers
*
*  \param [in]  commonParams :  Pointer to common parameter required for all the layers
*
*  \param [in]  algLayer :  Pointer to layer specific internal parameters
*
*  \param [in]  layerIdx           : Layer Id of the layer
*
*  \param [in]  memory           : Allocated memory pointers
*
*  \param [out]  memorySize           : Output memory requirement of the layer
*
*  \param [out]  outPtr           : Output memory pointer. Applicable only for reference flow
*
*  \return     Error:  IALG_EOK ( Successful) else failure
*
*  \remarks
*******************************************************************************
*/
int32_t TIDL_softmaxInitNew(const TIDL_LayerSpecificParams *layerSpecificParams,
                                const TIDL_NetworkCommonParams  *commonParams,
                                sTIDL_AlgLayer_t               *algLayer,
                                int32_t layerIdx,
                                uint8_t *memory[TIDL_LAYER_MEMORY_MAX],
                                int32_t memorySize[TIDL_LAYER_MEMORY_MAX],
                                void    **outPtr)
{
  int32_t status = IALG_EOK;
  int32_t forceRef = TIDL_softmaxForceRef(commonParams, layerIdx);

  if (((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) != TIDL_FLOW_CTRL_REF_ONLY) && (forceRef != 1))
  {
    /* presently, L1D would be allocated for the LUT table in the kernel code from x0000.
    We are here blocking L1D here for the LUT table and the layer handle would be stored in MSMC probably.
    LUT will be initialized from the beginning of the L1D.
    Degradation on configuration 64x197x197 softamx is of 0.23 ms.
    Suggestion: This degration can be handled by allocating/initializing LUT at a place mutually excluse with the handles in L1D
    i.e x2000 or x3000 instead of x0000 in case of ahp(16K L1D)*/
    status = TIDL_softmaxInitLUTParams(commonParams->net,
                                      layerIdx,
                                      algLayer,
                                      commonParams->tidlCommonParams->sysMems);

    if (status == IALG_EOK)
    {
      status = TIDL_deviceUtilsCommonInit(layerSpecificParams,
                                        commonParams,
                                        algLayer,
                                        layerIdx,
                                        memory,
                                        memorySize,
                                        outPtr);
    }
  }
  else
  {
    status = TIDL_deviceCommonRefInit(layerSpecificParams,
                                 commonParams,
                                 algLayer,
                                 layerIdx,
                                 memory,
                                 memorySize,
                                 outPtr);
  }

  return status;
}

int32_t TIDL_softmaxProcessNew(TIDL_NetworkCommonParams *commonParams,
                              sTIDL_AlgLayer_t     * algLayer,
                              sTIDL_Layer_t        * tidlLayer,
                              void                 * inPtrs[],
                              void                 * outPtrs[],
                              int32_t                layerIdx)
{
  int32_t status = IALG_EOK;
  int32_t forceRef = TIDL_softmaxForceRef(commonParams, layerIdx);

  if (((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) != TIDL_FLOW_CTRL_REF_ONLY) && (forceRef != 1))
  {
    status = TIDL_deviceUtilsCommonProcess(commonParams,
                                   algLayer,
                                   tidlLayer,
                                   inPtrs,
                                   outPtrs,
                                   layerIdx);

  }
  else
  {
    sTIDL_DataParams_t *inDataParams = TIDL_getDataParams(commonParams->createParams->net, tidlLayer->inData[0]);

    int32_t  imWidth           = inDataParams->dimValues[TIDL_DIM_WIDTH];
    int32_t  inPitch           = (uint16_t)inDataParams->pitch[TIDL_LINE_PITCH];
    int32_t  outPitch          = (uint16_t)tidlLayer->outData.pitch[TIDL_LINE_PITCH];
    int32_t  inBatchPitch      = (int32_t)inDataParams->pitch[TIDL_ROI_PITCH];
    int32_t  numBatches        = (int32_t)inDataParams->dimValues[TIDL_DIM_BATCH]; /*Batches  */
    int32_t  outBatchPitch     = tidlLayer->outData.pitch[TIDL_ROI_PITCH];
    int32_t outElementType     = tidlLayer->outData.elementType;
    int32_t axis               = tidlLayer->layerParams.softMaxParams.axis;
    int32_t outTranspose          = tidlLayer->layerParams.softMaxParams.outTranspose;
    int32_t inElementSize      = TIDL_getDatElementSize(inDataParams->elementType);
    int32_t outElementSize     = TIDL_getDatElementSize(tidlLayer->outData.elementType);
    if(commonParams->createParams->net->weightsElementSize == 4)
    {
      status = TIDL_SoftMaxProcessFloat(tidlLayer,  (void*)((int8_t*)inPtrs[0]), (void*)((int8_t*)outPtrs[0]), &inDataParams, &tidlLayer->outData, (uint8_t*)commonParams->createParams->net, outTranspose);
    }
    else if(outElementType == TIDL_SinglePrecFloat)
    {
      if (inDataParams->elementType == TIDL_SignedChar)
      {

        TIDL_refSoftmaxProcess(
            (int8_t*)(inPtrs[0]),
            (float32_tidl*)(outPtrs[0]),
            inDataParams->tensorScale,
            imWidth,
            inPitch,
            outPitch,
            numBatches,
            inBatchPitch,
            outBatchPitch,
            inDataParams,
            &tidlLayer->outData,
            axis,
            outTranspose
        );
      }
      else if (inDataParams->elementType == TIDL_UnsignedChar)
      {

        TIDL_refSoftmaxProcess(
            (uint8_t*)(inPtrs[0]),
            (float32_tidl*)(outPtrs[0]),
            inDataParams->tensorScale,
            imWidth,
            inPitch,
            outPitch,
            numBatches,
            inBatchPitch,
            outBatchPitch,
            inDataParams,
            &tidlLayer->outData,
            axis,
            outTranspose
        );
      }
      else if (inDataParams->elementType == TIDL_SignedShort)
      {

        TIDL_refSoftmaxProcess(
            (int16_t*)(inPtrs[0]),
            (float32_tidl*)(outPtrs[0]),
            inDataParams->tensorScale,
            imWidth,
            inPitch,
            outPitch,
            numBatches,
            inBatchPitch,
            outBatchPitch,
            inDataParams,
            &tidlLayer->outData,
            axis,
            outTranspose
        );
      }
      else if (inDataParams->elementType == TIDL_UnsignedShort)
      {

        TIDL_refSoftmaxProcess(
            (uint16_t*)(inPtrs[0]),
            (float32_tidl*)(outPtrs[0]),
            inDataParams->tensorScale,
            imWidth,
            inPitch,
            outPitch,
            numBatches,
            inBatchPitch,
            outBatchPitch,
            inDataParams,
            &tidlLayer->outData,
            axis,
            outTranspose
        );
      }
    }
    else if(outElementType == TIDL_SignedChar)
    {
      if (inDataParams->elementType == TIDL_SignedChar)
      {

        TIDL_refSoftmaxProcess(
            (int8_t*)(inPtrs[0]),
            (int8_t*)(outPtrs[0]),
            inDataParams->tensorScale,
            imWidth,
            inPitch,
            outPitch,
            numBatches,
            inBatchPitch,
            outBatchPitch,
            inDataParams,
            &tidlLayer->outData,
            axis,
            outTranspose
        );
      }
      else if (inDataParams->elementType == TIDL_UnsignedChar)
      {

        TIDL_refSoftmaxProcess(
            (uint8_t*)(inPtrs[0]),
            (int8_t*)(outPtrs[0]),
            inDataParams->tensorScale,
            imWidth,
            inPitch,
            outPitch,
            numBatches,
            inBatchPitch,
            outBatchPitch,
            inDataParams,
            &tidlLayer->outData,
            axis,
            outTranspose
        );
      }
    }
    else if(outElementType == TIDL_UnsignedChar)
    {
      if (inDataParams->elementType == TIDL_SignedChar)
      {

        TIDL_refSoftmaxProcess(
            (int8_t*)(inPtrs[0]),
            (uint8_t*)(outPtrs[0]),
            inDataParams->tensorScale,
            imWidth,
            inPitch,
            outPitch,
            numBatches,
            inBatchPitch,
            outBatchPitch,
            inDataParams,
            &tidlLayer->outData,
            axis,
            outTranspose
        );
      }
      else if (inDataParams->elementType == TIDL_UnsignedChar)
      {

        TIDL_refSoftmaxProcess(
            (uint8_t*)(inPtrs[0]),
            (uint8_t*)(outPtrs[0]),
            inDataParams->tensorScale,
            imWidth,
            inPitch,
            outPitch,
            numBatches,
            inBatchPitch,
            outBatchPitch,
            inDataParams,
            &tidlLayer->outData,
            axis,
            outTranspose
        );
      }
    }
    // 16-bit:
    else if(outElementType == TIDL_UnsignedShort)
    {
      if (inDataParams->elementType == TIDL_SignedShort)
      {

        TIDL_refSoftmaxProcess(
            (int16_t*)(inPtrs[0]),
            (uint16_t*)(outPtrs[0]),
            inDataParams->tensorScale,
            imWidth,
            inPitch,
            outPitch,
            numBatches,
            inBatchPitch,
            outBatchPitch,
            inDataParams,
            &tidlLayer->outData,
            axis,
            outTranspose
        );
      }
      else if (inDataParams->elementType == TIDL_UnsignedShort)
      {

        TIDL_refSoftmaxProcess(
            (uint16_t*)(inPtrs[0]),
            (uint16_t*)(outPtrs[0]),
            inDataParams->tensorScale,
            imWidth,
            inPitch,
            outPitch,
            numBatches,
            inBatchPitch,
            outBatchPitch,
            inDataParams,
            &tidlLayer->outData,
            axis,
            outTranspose
        );
      }
    }
    else
    {
      tidl_printf(0,"Unsupported combination!\n");
    }
  }

 return status;
}
