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
 *  \file tidl_scatterElements.c
 *
 *  \brief This file defines kernel functions for Batch Norm layer
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "tidl_alg_int.h"
#include "tidl_commonUtils.h"
#include "tidl_scatterElements.h"
#include "math.h"
#include <limits>

//#define TIDL_REF_BATCH_NORM_DEBUG
/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define TIDL_SCATTERND_DST_TO_SRC_TABLE_SIZE_DDR (200U*1024U)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                  Internal/Private Function Declarations                    */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/* ========================================================================== */
/*                       Static Function Definitions                          */
/* ========================================================================== */


template <class Tin, class Tacc>
void TIDL_refScatterElementsOutputUpdation(Tacc *accPtr, Tin *update, int32_t targetIndex, int32_t updateIndex, int32_t reduction, Tacc minValue, Tacc maxValue)
{
  if(reduction == TIDL_ScatterElementsAdd)
  {
    Tacc sum = accPtr[targetIndex] + (Tacc)update[updateIndex];
    if(sum > maxValue)
    {
      accPtr[targetIndex] = (Tacc)maxValue;
    }
    else if(sum < minValue)
    {
      accPtr[targetIndex] = (Tacc)minValue;
    }
    else
    {
      accPtr[targetIndex] = sum;
    }
  }
  else if(reduction == TIDL_ScatterElementsMax)
  {
    accPtr[targetIndex] = accPtr[targetIndex] > (Tacc)update[updateIndex] ? accPtr[targetIndex] : (Tacc)update[updateIndex];
  }
  else if(reduction == TIDL_ScatterElementsMin)
  {
    accPtr[targetIndex] = accPtr[targetIndex] < (Tacc)update[updateIndex] ? accPtr[targetIndex] : (Tacc)update[updateIndex];
  }
  else
  {
    accPtr[targetIndex] = (Tacc)update[updateIndex];
  }
}

/**
 * @brief This is main function perform scatterElements on differnt elementTypes
 *
 * @tparam Tin  : template for input
 * @tparam Tw   : template for weights
 * @tparam Tb   : template for Bias
 * @tparam Tout : template for output
 * @tparam Tsat : template for saturate values
 * @param inPtr : Input pointer on which bacthNorm is applied
 * @param outPtr : Output pointer after scatterElements opreation
 * @param weightsPtr : Pointer to weights buffer
 * @param slopePtr : Pointer to the Slope buffer
 * @param biasPtr  : Pointer to the Bias values
 * @param intAlgHandle : tidl algorothm handle
 * @param layerIdx :index of the current layer
 * @param params : copy of bacthNorm parameters
 * @param algLayer : Pointer to the layer specific parameters
 * @param inDataParams : parameters of the input data buffer
 * @param outDataParams : parameters of the output data buffer
 * @param satLow : min value for the saturation
 * @param satHigh : max value for the saturation
 * @return  IALG_EOK   - Successful
 *          IALG_EFAIL - Unspecified error
 */

template <class Tin, class Tindx, class Tout, class Tacc>
static int32_t TIDL_refScatterElements(Tin  *data,
                                     Tindx* indices,
                                     Tin  *update,
                                     Tout *outPtr,
                                     Tacc *accPtr,
                                     TIDL_Handle intAlgHandle,
                                     int32_t layerIdx,
                                     sTIDL_ScatterElementsParams_t *params,
                                     sTIDL_AlgLayer_t *algLayer,
                                     const sTIDL_Layer_t  *tidlLayer,
                                     const sTIDL_DataParams_t *inDataParams,
                                     const sTIDL_DataParams_t *inIndicesParams,
                                     const sTIDL_DataParams_t *inUpdateParams,
                                     const sTIDL_DataParams_t *outDataParams)
{
  int32_t status = TIDL_SUCCESS;
  int32_t i0, i1, i2, i3;

  int32_t inIndicesNumCols    = (int32_t)inIndicesParams->dimValues[TIDL_DIM_WIDTH];
  int32_t inIndicesNumRows    = (int32_t)inIndicesParams->dimValues[TIDL_DIM_HEIGHT];

  int32_t inChs              = (int32_t)inDataParams->dimValues[TIDL_DIM_NUMCH];
  int32_t inChPitch          = (int32_t)inDataParams->pitch[TIDL_DIM_NUMCH];

  int32_t outNumCols          = (int32_t)outDataParams->dimValues[TIDL_DIM_WIDTH];
  int32_t outNumRows          = (int32_t)outDataParams->dimValues[TIDL_DIM_HEIGHT];
  int32_t outChs              = (int32_t)outDataParams->dimValues[TIDL_DIM_NUMCH];
  int32_t outBatches          = (int32_t)outDataParams->dimValues[TIDL_DIM_BATCH];

  int32_t inDataPitch         = (int32_t)inDataParams->pitch[TIDL_LINE_PITCH];
  int32_t inDataChPitch       = (int32_t)inDataParams->pitch[TIDL_CHANNEL_PITCH];

  int32_t inIndicesPitch      = (int32_t)inIndicesParams->pitch[TIDL_LINE_PITCH];
  int32_t inIndicesChPitch    = (int32_t)inIndicesParams->pitch[TIDL_CHANNEL_PITCH];

  int32_t inUpdatePitch       = (int32_t)inUpdateParams->pitch[TIDL_LINE_PITCH];
  int32_t inUpdateChPitch     = (int32_t)inUpdateParams->pitch[TIDL_CHANNEL_PITCH];

  int32_t outPitch            = (int32_t)outDataParams->pitch[TIDL_LINE_PITCH];
  int32_t outChPitch          = (int32_t)outDataParams->pitch[TIDL_CHANNEL_PITCH];
  int32_t outRoiPitch         = (int32_t)outDataParams->pitch[TIDL_ROI_PITCH];

  data               = (Tin *)data + (inDataParams->padH*inDataPitch) + inDataParams->padW;
  indices            = (Tindx *)indices + (inIndicesParams->padH*inIndicesPitch) + inIndicesParams->padW;
  update             = (Tin *)update + (inUpdateParams->padH*inUpdatePitch) + inUpdateParams->padW;

  Tout *output= (Tout *)outPtr + (outDataParams->padH*outPitch) + outDataParams->padW;

  int32_t numTotRoi = inDataParams->dimValues[TIDL_DIM_BATCH];

  int32_t numInChannels = inDataParams->dimValues[TIDL_DIM_NUMCH];
  int32_t numOutChannels = outDataParams->dimValues[TIDL_DIM_NUMCH];
  int32_t axis = params->axis;

  int32_t index;
  Tin updateVal;

  int32_t isScatterND = (params->axis == -1)?1:0;
  int32_t n, c, h, w, targetIndex, updateIndex;

  if(isScatterND)
  {
    int32_t dataDim = outDataParams->numDim;
    int32_t indDim = inIndicesParams->dimValues[TIDL_DIM_WIDTH];

    int32_t outSize = outChs*outChPitch;
    Tacc minValueAcc = std::numeric_limits<Tacc>::lowest();
    Tacc maxValueAcc = std::numeric_limits<Tacc>::max();

    if (dataDim == indDim)
    {
      OPENACC(data copyin(indices[:(inIndicesNumRows-1)*inIndicesNumCols + 4]))
      OPENACC(parallel loop)
      //element update
      for(i0=0;i0<inIndicesNumRows;i0++)
      {
        n = (int32_t)indices[i0*inIndicesNumCols + 0];
        c = (int32_t)indices[i0*inIndicesNumCols + 1];
        h = (int32_t)indices[i0*inIndicesNumCols + 2];
        w = (int32_t)indices[i0*inIndicesNumCols + 3];

        if((n >= outBatches) || (c >= outChs) || (h >= outNumRows) || w >= outNumCols)
        {
          #ifndef _OPENACC
          tidl_printf(0, "Index out of bounds! skipping the update");
          #endif
          continue;
        }
        targetIndex = n*outRoiPitch + c*outChPitch + h*outPitch + w;
        TIDL_refScatterElementsOutputUpdation(accPtr, update, targetIndex, i0,  params->reduction, minValueAcc, maxValueAcc);
      }
    }
    else if ((dataDim - indDim) == 1)
    {
      //we start with zero-initialized buffer
      int32_t inSize = inChs*inChPitch;
      int32_t cpSize = outSize;
      if (inSize != outSize)
      {
        tidl_printf(0, "ScatterElements: inBatch pitch not equal to outBatch pitch, zeroing the output instead of copy\n");
        OPENACC(data copyin(data[:cpSize]) copyout(accPtr[:cpSize]))
        OPENACC(parallel loop)
        // memset((Tacc*)accPtr, 0, sizeof(accPtr[0])*cpSize);
        for(int32_t k = 0; k < cpSize; k++)
        {
          accPtr[k] = (Tacc)data[0];
        }
      }
      else
      {
        OPENACC(data copyin(data[:cpSize]) copyout(accPtr[:cpSize]))
        OPENACC(parallel loop)
        for(int32_t k = 0; k < cpSize; k++)
        {
          accPtr[k] = data[k];
        }
      }

      OPENACC(data copyin(indices[:(inIndicesNumRows-1)*inIndicesNumCols + 3]))
      OPENACC(parallel loop)
      //line update
      for (i0=0;i0<inIndicesNumRows;i0++)
      {
        if (dataDim == 4 && indDim == 3)
        {
          n = (int32_t)indices[i0*inIndicesNumCols + 0];
          c = (int32_t)indices[i0*inIndicesNumCols + 1];
          h = (int32_t)indices[i0*inIndicesNumCols + 2];
        }
        else if (dataDim == 3 && indDim == 2)
        {
          n = 0;
          c = (int32_t)indices[i0*inIndicesNumCols + 0];
          h = (int32_t)indices[i0*inIndicesNumCols + 1];
        }
        else if (dataDim == 2 && indDim == 1)
        {
          n = 0;
          c = 0;
          h = (int32_t)indices[i0*inIndicesNumCols + 0];
        }

        if((n >= outBatches) || (c >= outChs) || (h >= outNumRows))
        {
          #ifndef _OPENACC
          tidl_printf(0, "Index out of bounds! skipping the update");
          #endif
          continue;
        }
        targetIndex = n * outRoiPitch + c * outChPitch + h * outPitch;
        updateIndex = i0 * outDataParams->dimValues[TIDL_DIM_WIDTH];
        OPENACC(loop)
        for (i1=0;i1<outDataParams->dimValues[TIDL_DIM_WIDTH];i1++)
        {
          TIDL_refScatterElementsOutputUpdation(accPtr, update, targetIndex, updateIndex, params->reduction, minValueAcc, maxValueAcc);
          targetIndex++;
          updateIndex++;
        }
      }
    }
    else if ((dataDim - indDim) == 2)
    {
      OPENACC(data copyin(indices[:(inIndicesNumRows-1)*inIndicesNumCols+2]))
      OPENACC(parallel loop)
      //channel update
      for (i0=0;i0<inIndicesNumRows;i0++)
      {
        n = (int32_t)indices[i0*inIndicesNumCols + 0];
        c = (int32_t)indices[i0*inIndicesNumCols + 1];

        if((n >= outBatches) || (c >= outChs))
        {
          #ifndef _OPENACC
          tidl_printf(0, "Index out of bounds! skipping the update");
          #endif
          continue;
        }
        targetIndex = n * outRoiPitch + c * outChPitch;
        updateIndex = i0*outDataParams->dimValues[TIDL_DIM_HEIGHT]*outDataParams->dimValues[TIDL_DIM_WIDTH];
        OPENACC(loop collapse(2))
        for (i1=0;i1<outDataParams->dimValues[TIDL_DIM_HEIGHT];i1++)
        {
          for (i2=0;i2<outDataParams->dimValues[TIDL_DIM_WIDTH];i2++)
          {
            TIDL_refScatterElementsOutputUpdation(accPtr, update, targetIndex, updateIndex, params->reduction, minValueAcc, maxValueAcc);
            targetIndex++;
            updateIndex++;
          }
        }
      }
    }

    OPENACC(data copyin(accPtr[:outSize]) copyout(output[:outSize]))
    {
    if(params->reduction == TIDL_ScatterElementsAdd)
    {
      Tout temp;
      Tacc outAcc;
      temp = std::numeric_limits<Tout>::lowest();
      int32_t minValueOutput = (int32_t)temp;
      temp = std::numeric_limits<Tout>::max();
      int32_t maxValueOutput = (int32_t)temp;

      uint8_t cScale, shift;
      float32_tidl fVal = (outDataParams->tensorScale/ inUpdateParams->tensorScale);
      TIDL_getMMAv2_ScaleAndShift(fVal, &cScale, &shift);

      /* Prepare output from accPtr */
      OPENACC(parallel loop)
      for(int32_t k = 0; k < outSize; k++)
      {
        outAcc = accPtr[k];
        if (outDataParams->elementType == TIDL_SinglePrecFloat)
        {
          OPENACC(routine(TIDL_floatSat))
          outAcc = TIDL_floatSat(outAcc, (sTIDL_Layer_t  *)tidlLayer);
        }
        else
        {
          int64_t tempAcc;
          tempAcc = outAcc * cScale;
          outAcc = (Tacc)TIDL_roundSat((int64_t)tempAcc, shift, minValueOutput, maxValueOutput);
        }
        output[k] = outAcc;
      }
    }
    else
    {
      OPENACC(parallel loop)
      for(int32_t k = 0; k < outSize; k++)
      {
        output[k] = accPtr[k];
      }
    }
    }
  }

  else
  {
    /**
     * First tensor, data : initialization data. NxCxW*H
     * Second tensor, Indices : Location at which update has to happen. NxCxP
     * Third tensor, update : values to be scattered.NxCxP
     * Values are taken from update and placed in data buffer at indices location.
     * Final data buffer is copied into output buffer output of shape NxCxW*H
     * output[index[i][j][k]][j][k] = update[i][j][k]  # if axis == 0
     * output[i][index[i][j][k]][k] = update[i][j][k]  # if axis == 1
     * output[i][j][index[i][j][k]] = update[i][j][k]  # if axis == 2
     */
    OPENACC(data copyin(indices[: 1 + (numTotRoi-1)*inIndicesChPitch*numInChannels + (inIndicesNumRows-1)*inIndicesChPitch + (inIndicesNumRows-1)*inIndicesPitch + (inIndicesNumCols-1)],
                        update[: 1 + (numTotRoi-1)*inUpdateChPitch*numInChannels + (numInChannels-1)*inUpdateChPitch + (inIndicesNumRows-1)*inUpdatePitch + (inIndicesNumCols-1)])
                 copyout(data[: 1 + (numTotRoi-1)*inDataChPitch*numOutChannels + (numInChannels-1)*inDataChPitch + (inIndicesNumRows-1)*inDataPitch + (inIndicesNumCols-1)]))
    OPENACC(parallel loop collapse(4))
    for(i0=0; i0<numTotRoi; i0++)
    {
      for(i1=0; i1<numInChannels; i1++)
      {
        for(i2=0; i2<inIndicesNumRows; i2++)
        {
          for(i3=0; i3<inIndicesNumCols; i3++)
          {
            /*indices and updates should be of same dimension*/
            index = (int32_t)indices[i0*inIndicesChPitch*numInChannels + i1*inIndicesChPitch + i2*inIndicesPitch + i3];
            updateVal = update[i0*inUpdateChPitch*numInChannels + i1*inUpdateChPitch + i2*inUpdatePitch + i3];

            if(index >= inDataPitch)
            {
              #ifndef _OPENACC
              tidl_printf(0,"something is wrong as index is higher than the allowed \n");
              #endif
              continue;
            }

            if(index <= 0)
            {
              continue;
            }

            if(axis == TIDL_DIM_BATCH)
            {
              data[index + i1*inDataChPitch + i2*inDataPitch + i3] = updateVal;
            }
            else if(axis == TIDL_DIM_NUMCH)
            {
              data[i0*inDataChPitch*numOutChannels + index + i2*inDataPitch + i3] = updateVal;
            }
            else if(axis == TIDL_DIM_HEIGHT)
            {
              data[i0*inDataChPitch*numOutChannels + i1*inDataChPitch + index + i3] = updateVal;
            }
            else if(axis == TIDL_DIM_WIDTH)
            {
              data[i0*inDataChPitch*numOutChannels + i1*inDataChPitch + i2*inDataPitch + index] = updateVal;
            }
            else
            {
              status = TIDL_ERR_FAILURE;
            }
          }
        }
      }
    }

    OPENACC(data copyout(output[:(numTotRoi-1)*outChPitch*numOutChannels + (numOutChannels-1)*outChPitch + (outNumRows-1)*outPitch + 1])
                 copyin(data[:(numTotRoi-1)*outNumCols*outNumRows*numOutChannels + (numOutChannels-1)*outNumCols*outNumRows + (outNumRows-1)*outNumCols + 1]))
    OPENACC(parallel loop collapse(3))
    /*reshape of data also happens here as in data is assumed to be linear*/
    for(i0=0; i0<numTotRoi; i0++)
    {
      for(i1=0; i1<numOutChannels; i1++)
      {
        for(i2=0; i2<outNumRows; i2++)
        {
          memcpy(output + i0*outChPitch*numOutChannels + i1*outChPitch + i2*outPitch,
                data + i0*outNumCols*outNumRows*numOutChannels + i1*outNumCols*outNumRows + i2*outNumCols,
                outNumCols*sizeof(output[0]));
        }
      }
    }
  }
  return status;
}

 /**
 * @brief scatterElements layer reference implementation
 *
 * @param intAlgHandle : tidl algorothm handle
 * @param algLayer : Pointer to the layer specific parameters
 * @param tidlLayer : Pointer to the common layer parameters
 * @param params : copy of batch norm layer parameters
 * @param inPtr : Pointer to input buffers to be processed
 * @param outPtr : Pointer to output buffers to be processed
 * @param inDataParams : pointer to input data parameters
 * @param outDataParams : pointer to output data parameters
 * @return  IALG_EOK   - Successful
 *          IALG_EFAIL - Unspecified error
 */
static int32_t TIDL_refScatterElementsProcess(TIDL_Handle intAlgHandle,
                                        sTIDL_AlgLayer_t *algLayer,
                                        const sTIDL_Layer_t    *tidlLayer,
                                        sTIDL_ScatterElementsParams_t *params,
                                        void *data,
                                        void *indices,
                                        void *update,
                                        void *outPtr,
                                        const sTIDL_DataParams_t *inDataParams,
                                        const sTIDL_DataParams_t *inIndicesParams,
                                        const sTIDL_DataParams_t *inUpdateParams,
                                        const sTIDL_DataParams_t *outDataParams)
{
  int32_t status = TIDL_SUCCESS;
  int32_t layerIdx = algLayer->layerIdx;
  void *accPtr = algLayer->layerParams.scatterElementsParams.accPtr;

  if(TIDL_SignedChar == ((int32_t)inDataParams->elementType))
  {

    if(TIDL_SignedWord != ((int32_t)inIndicesParams->elementType))
    {
      tidl_printf(0,"Indice data type should int32");
      status = TIDL_ERR_FAILURE;
      return status;
    }
    if(params->reduction == TIDL_ScatterElementsAdd)
    {
      status = TIDL_refScatterElements((int8_t *)data,
                                    (int32_t *)indices,
                                    (int8_t *)update,
                                    (int8_t *)outPtr,
                                    (int16_t *)accPtr,
                                    intAlgHandle,
                                    layerIdx,
                                    params,
                                    algLayer,
                                    tidlLayer,
                                    inDataParams,
                                    inIndicesParams,
                                    inUpdateParams,
                                    outDataParams);
    }
    else
    {
      status = TIDL_refScatterElements((int8_t *)data,
                                    (int32_t *)indices,
                                    (int8_t *)update,
                                    (int8_t *)outPtr,
                                    (int8_t *)accPtr,
                                    intAlgHandle,
                                    layerIdx,
                                    params,
                                    algLayer,
                                    tidlLayer,
                                    inDataParams,
                                    inIndicesParams,
                                    inUpdateParams,
                                    outDataParams);
    }

  }
  else if(TIDL_UnsignedChar == ((int32_t)inDataParams->elementType))
  {

    if(TIDL_SignedWord != ((int32_t)inIndicesParams->elementType))
    {
      tidl_printf(0,"Indice data type should int32");
      status = TIDL_ERR_FAILURE;
      return status;
    }

    if(params->reduction == TIDL_ScatterElementsAdd)
    {
      status = TIDL_refScatterElements((uint8_t *)data,
                                    (int32_t *)indices,
                                    (uint8_t *)update,
                                    (uint8_t *)outPtr,
                                    (uint16_t *) accPtr,
                                    intAlgHandle,
                                    layerIdx,
                                    params,
                                    algLayer,
                                    tidlLayer,
                                    inDataParams,
                                    inIndicesParams,
                                    inUpdateParams,
                                    outDataParams);
    }
    else
    {
      status = TIDL_refScatterElements((uint8_t *)data,
                                    (int32_t *)indices,
                                    (uint8_t *)update,
                                    (uint8_t *)outPtr,
                                    (uint8_t *) accPtr,
                                    intAlgHandle,
                                    layerIdx,
                                    params,
                                    algLayer,
                                    tidlLayer,
                                    inDataParams,
                                    inIndicesParams,
                                    inUpdateParams,
                                    outDataParams);
    }

  }
  else if(TIDL_SignedShort == ((int32_t)inDataParams->elementType))
  {

    if(TIDL_SignedWord != ((int32_t)inIndicesParams->elementType))
    {
      tidl_printf(0,"Indice data type should int32");
      status = TIDL_ERR_FAILURE;
      return status;
    }

    if(params->reduction == TIDL_ScatterElementsAdd)
    {
      status = TIDL_refScatterElements((int16_t *)data,
                                    (int32_t *)indices,
                                    (int16_t *)update,
                                    (int16_t *)outPtr,
                                    (int32_t *) accPtr,
                                    intAlgHandle,
                                    layerIdx,
                                    params,
                                    algLayer,
                                    tidlLayer,
                                    inDataParams,
                                    inIndicesParams,
                                    inUpdateParams,
                                    outDataParams);
    }
    else
    {
      status = TIDL_refScatterElements((int16_t *)data,
                                    (int32_t *)indices,
                                    (int16_t *)update,
                                    (int16_t *)outPtr,
                                    (int16_t *) accPtr,
                                    intAlgHandle,
                                    layerIdx,
                                    params,
                                    algLayer,
                                    tidlLayer,
                                    inDataParams,
                                    inIndicesParams,
                                    inUpdateParams,
                                    outDataParams);
    }

  }
  else if(TIDL_UnsignedShort == ((int32_t)inDataParams->elementType))
  {

    if(TIDL_SignedWord != ((int32_t)inIndicesParams->elementType))
    {
      tidl_printf(0,"Indice data type should int32");
      status = TIDL_ERR_FAILURE;
      return status;
    }
    if(params->reduction == TIDL_ScatterElementsAdd)
    {
      status = TIDL_refScatterElements((uint16_t *)data,
                                    (int32_t *)indices,
                                    (uint16_t *)update,
                                    (uint16_t *)outPtr,
                                    (uint32_t *) accPtr,
                                    intAlgHandle,
                                    layerIdx,
                                    params,
                                    algLayer,
                                    tidlLayer,
                                    inDataParams,
                                    inIndicesParams,
                                    inUpdateParams,
                                    outDataParams);
    }
    else
    {
      status = TIDL_refScatterElements((uint16_t *)data,
                                    (int32_t *)indices,
                                    (uint16_t *)update,
                                    (uint16_t *)outPtr,
                                    (uint16_t *) accPtr,
                                    intAlgHandle,
                                    layerIdx,
                                    params,
                                    algLayer,
                                    tidlLayer,
                                    inDataParams,
                                    inIndicesParams,
                                    inUpdateParams,
                                    outDataParams);
    }

  }
  else if(TIDL_SinglePrecFloat == ((int32_t)inDataParams->elementType))
  {
    /*In stat collection , all input tensor is assumed as float.
      hence making indices as float in this flow
    */
    if((TIDL_SinglePrecFloat != ((int32_t)inIndicesParams->elementType)) &&
       (TIDL_SignedWord      != ((int32_t)inIndicesParams->elementType)))
    {
      tidl_printf(0,"Indice data type should float32");
      status = TIDL_ERR_FAILURE;
      return status;
    }

    if((TIDL_SinglePrecFloat == ((int32_t)inIndicesParams->elementType)))
    {
      status = TIDL_refScatterElements((float32_tidl *)data,
                                      (float32_tidl *)indices,
                                      (float32_tidl *)update,
                                      (float32_tidl *)outPtr,
                                      (float32_tidl *) accPtr,
                                      intAlgHandle,
                                      layerIdx,
                                      params,
                                      algLayer,
                                      tidlLayer,
                                      inDataParams,
                                      inIndicesParams,
                                      inUpdateParams,
                                      outDataParams);
    }

    if((TIDL_SignedWord == ((int32_t)inIndicesParams->elementType)))
    {
      status = TIDL_refScatterElements((float32_tidl *)data,
                                      (int32_t *)indices,
                                      (float32_tidl *)update,
                                      (float32_tidl *)outPtr,
                                      (float32_tidl *) accPtr,
                                      intAlgHandle,
                                      layerIdx,
                                      params,
                                      algLayer,
                                      tidlLayer,
                                      inDataParams,
                                      inIndicesParams,
                                      inUpdateParams,
                                      outDataParams);
    }

  }
  else
  {
    status = TIDL_ERR_FAILURE;
  }
  TIDL_enableL1DandL2CacheWb();
  return status;
}

/* Added new Functions */

int32_t TIDL_scatterElementsRefInit(const TIDL_LayerSpecificParams *layerSpecificParams,
                                const TIDL_NetworkCommonParams  *commonParams,
                                sTIDL_AlgLayer_t               *algLayer,
                                int32_t layerIdx,
                                uint8_t *memory[TIDL_LAYER_MEMORY_MAX],
                                int32_t memorySize[TIDL_LAYER_MEMORY_MAX],
                                void    **outPtr)
{
  int32_t status = IALG_EOK;
  int32_t currOffset = 0;
  int32_t scratchDataSize = 0;
  int32_t outDataSize;
  int32_t paramMemSize = 0;

  outDataSize = TIDL_refGetOutDataSize(commonParams, layerSpecificParams, layerIdx);

  sTIDL_Layer_t *tidlLayer = &commonParams->createParams->net->TIDLLayers[layerIdx];
  int32_t outChPitch          = (int32_t)tidlLayer->outData.pitch[TIDL_CHANNEL_PITCH];
  int32_t outChs              = (int32_t)tidlLayer->outData.dimValues[TIDL_DIM_NUMCH];
  /* memory needed for accumulate( twice of output data)[ScatterND]*/
  int32_t accSize  =  2 * sizeof(TIDL_getDatElementSize(tidlLayer->outData.elementType));
  scratchDataSize = outChs * outChPitch * accSize;

  TIDL_AllocatePtr((intptr_t)memory[TIDL_LAYER_MEMORY_SCRATCH],
                                    &currOffset,
                                    scratchDataSize,
                                    128,
                                    &algLayer->layerParams.scatterElementsParams.accPtr );

  algLayer->scratchSize = scratchDataSize;
  currOffset = 0;
  if (outPtr != NULL )
  {
    if (TIDL_isOutDataBuff(commonParams->net,
            commonParams->net->TIDLLayers[layerIdx].outData.dataId,
            commonParams->createParams->currLayersGroupId) == 1)
    {
      *outPtr = NULL;
    }
    else if (outDataSize != 0)
    {
      TIDL_AllocatePtr((intptr_t)memory[TIDL_LAYER_MEMORY_OUTPUT],
                       &currOffset,
                       outDataSize,
                       128,
                       outPtr);
    }
    else
    {
      *outPtr = NULL;
    }
  }

  memorySize[TIDL_LAYER_MEMORY_SCRATCH]    = scratchDataSize + TIDL_ALIGNMENT_SIZE;
  memorySize[TIDL_LAYER_MEMORY_PERSISTENT] = paramMemSize + TIDL_ALIGNMENT_SIZE;
  memorySize[TIDL_LAYER_MEMORY_OUTPUT]     = outDataSize + TIDL_ALIGNMENT_SIZE;

  return status;
}

int32_t TIDL_scatterElementsAllocNew(const TIDL_LayerSpecificParams *layerSpecificParams,
                                   const TIDL_NetworkCommonParams *commonParams,
                                   int32_t layerIdx,
                                   int32_t memorySize[TIDL_LAYER_MEMORY_MAX])
{
  int32_t status = IALG_EOK;

  if ((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0)
  {
    int32_t handleSize = 0;
    int32_t scratchMemSize = TIDL_SCATTERND_DST_TO_SRC_TABLE_SIZE_DDR; // TODO: Protect for ScatterND sum.
    sTIDL_Layer_t *tidlLayer = &commonParams->createParams->net->TIDLLayers[layerIdx];
    handleSize = TIDL_getScatterElementsKernelHandleSize(tidlLayer);

    memorySize[TIDL_LAYER_MEMORY_SCRATCH]    += scratchMemSize;
    memorySize[TIDL_LAYER_MEMORY_PERSISTENT] += handleSize;
    memorySize[TIDL_LAYER_MEMORY_OUTPUT]     += 0;
  }
  else
  {
    int32_t scratchDataSize = 0;
    int32_t outDataSize;
    int32_t paramMemSize = 0;

    outDataSize = TIDL_refGetOutDataSize(commonParams, layerSpecificParams, layerIdx);

    sTIDL_Layer_t *tidlLayer = &commonParams->createParams->net->TIDLLayers[layerIdx];
    int32_t outChPitch          = (int32_t)tidlLayer->outData.pitch[TIDL_CHANNEL_PITCH];
    int32_t outChs              = (int32_t)tidlLayer->outData.dimValues[TIDL_DIM_NUMCH];
    int32_t accSize  =  2 * sizeof(TIDL_getDatElementSize(tidlLayer->outData.elementType));
    scratchDataSize = outChs * outChPitch * accSize;

    memorySize[TIDL_LAYER_MEMORY_SCRATCH]    += (scratchDataSize + TIDL_ALIGNMENT_SIZE);
    memorySize[TIDL_LAYER_MEMORY_PERSISTENT] += (paramMemSize + TIDL_ALIGNMENT_SIZE);
    memorySize[TIDL_LAYER_MEMORY_OUTPUT]     += (outDataSize + TIDL_ALIGNMENT_SIZE);

  }

  return status;
}

int32_t TIDL_scatterElementsInitNew(const TIDL_LayerSpecificParams *layerSpecificParams,
                                const TIDL_NetworkCommonParams  *commonParams,
                                sTIDL_AlgLayer_t               *algLayer,
                                int32_t layerIdx,
                                uint8_t *memory[TIDL_LAYER_MEMORY_MAX],
                                int32_t memorySize[TIDL_LAYER_MEMORY_MAX],
                                void    **outPtr)
{
  int32_t status = IALG_EOK;

  if ((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0)
  {
    int32_t handleSize = 0;
    void *handle;
    int32_t currOffset = 0;
    sTIDL_Layer_t *tidlLayer = &commonParams->createParams->net->TIDLLayers[layerIdx];

    handleSize = TIDL_getScatterElementsKernelHandleSize(tidlLayer);

    TIDL_AllocatePtr((intptr_t)memory[TIDL_LAYER_MEMORY_PERSISTENT],
                &currOffset,
                handleSize,
                TIDL_ALIGNMENT_SIZE,
                &handle);

    algLayer->workloadHandle = handle;

    status = TIDL_scatterElementsKernelInitNew(commonParams->createParams,
                                              algLayer,
                                              tidlLayer,
                                              handle);

    memorySize[TIDL_LAYER_MEMORY_SCRATCH]    += 0;
    memorySize[TIDL_LAYER_MEMORY_PERSISTENT] += handleSize;
    memorySize[TIDL_LAYER_MEMORY_OUTPUT]     += 0;
  }
  else
  {
    status = TIDL_scatterElementsRefInit(layerSpecificParams,
                                 commonParams,
                                 algLayer,
                                 layerIdx,
                                 memory,
                                 memorySize,
                                 outPtr);
  }

  return status;
}

int32_t TIDL_scatterElementsProcessNew(TIDL_NetworkCommonParams *commonParams,
                                          sTIDL_AlgLayer_t     * algLayer,
                                          sTIDL_Layer_t        * tidlLayer,
                                          void                 * inPtrs[],
                                          void                 * outPtrs[],
                                          int32_t                layerIdx)
{
  int32_t status = IALG_EOK;
  /*It is import tool duty to provide in data in following order
  data    = 0;
  indices = 1;
  update  = 2;
  this order is same as onnx operator
  update is treated as feature, indices are treated as weights. data is treated as initializer.
  */
  int32_t dataIdx    = 0;
  int32_t indicesIdx = 1;
  int32_t voxelIdx   = 2;
  uint8_t  (*data)[]   = (uint8_t (*)[])(inPtrs[dataIdx]);
  uint8_t  (*indices)[]= (uint8_t (*)[])(inPtrs[indicesIdx]);
  uint8_t  (*update)[] = (uint8_t (*)[])(inPtrs[voxelIdx]);

  int8_t   (*outPtr)[]    = (int8_t (*)[])(outPtrs[0]);

  sTIDL_ScatterElementsParams_t   *params = &tidlLayer->layerParams.scatterElementsParams;
  sTIDL_DataParams_t        *inDataParams;
  sTIDL_DataParams_t        *inIndicesParams;
  sTIDL_DataParams_t        *inUpdateParams;

  inDataParams   = &commonParams->createParams->net->TIDLLayers[algLayer->inLayerIdx[0]].outData;
  inIndicesParams= &commonParams->createParams->net->TIDLLayers[algLayer->inLayerIdx[1]].outData;
  inUpdateParams = &commonParams->createParams->net->TIDLLayers[algLayer->inLayerIdx[2]].outData;

  TIDL_Obj intAlgObj;
  TIDL_CreateParams createParams;
  memcpy(&createParams, commonParams->createParams, sizeof(TIDL_CreateParams));
  intAlgObj.createParams = (TIDL_CreateParams *) &createParams;
  sTIDL_sysMemHandle_t *sysMems = commonParams->tidlCommonParams->sysMems;

  if ((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0)
  {
    status = TIDL_scatterElementsDspProcessNew(commonParams,
                                            algLayer,
                                            tidlLayer,
                                            inPtrs,
                                            outPtrs,
                                            sysMems);
    /* This layer does not use standard WL flow, so need to handle layer level cache writeback explicitly in this layer */ 
    TIDL_enableL1DCacheWb();
  }
  else
  {
    status = TIDL_refScatterElementsProcess(&intAlgObj,
                                              algLayer,
                                              tidlLayer,
                                              params,
                                              data,
                                              indices,
                                              update,
                                              outPtr,
                                              inDataParams,
                                              inIndicesParams,
                                              inUpdateParams,
                                              &tidlLayer->outData);
  }

  if((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_STAT) == TIDL_FLOW_CTRL_REF_STAT)
  {
    int32_t layerIdx = algLayer->layerIdx;
    TIDL_UpdateScaleFactors(&intAlgObj, layerIdx, 0, 0, 1);
  }

  status = WorkloadUnitExec_padBufferExec(outPtrs[0], algLayer->wlRepeatIter, tidlLayer, commonParams->tidlCommonParams,
                                        &algLayer->wlPadParams, algLayer->workloadUnit, algLayer->gcHelperHandle,
                                        tidlLayer->outData.pitch[TIDL_CHANNEL_PITCH] * TIDL_getDatElementSize(tidlLayer->outData.elementType), 
                                        tidlLayer->outData.pitch[TIDL_ROI_PITCH] * TIDL_getDatElementSize(tidlLayer->outData.elementType));

  return status;
}



