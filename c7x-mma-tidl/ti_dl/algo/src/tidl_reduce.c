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
 *  \file tidl_reduce.c
 *
 *  \brief This file defines kernel functions for Batch Norm layer
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "tidl_alg_int.h"
#include "tidl_commonUtils.h"
#include "tidl_reduce.h"
#include "math.h"
#include <limits>

//#define TIDL_REF_BATCH_NORM_DEBUG
/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

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

/**
 * @brief This is main function perform reduce on differnt elementTypes
 *
 * @tparam Tin  : template for input
 * @tparam Tw   : template for weights
 * @tparam Tb   : template for Bias
 * @tparam Tout : template for output
 * @tparam Tsat : template for saturate values
 * @param inPtr : Input pointer on which bacthNorm is applied
 * @param outPtr : Output pointer after reduce opreation
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
template <class Tin, class Tout>
static int32_t TIDL_refReduceCore(Tin  *inPtr,
                                     Tout *outPtr,
                                     TIDL_Handle intAlgHandle,
                                     int32_t layerIdx,
                                     sTIDL_ReduceParams_t *params,
                                     sTIDL_AlgLayer_t *algLayer,
                                     const sTIDL_DataParams_t *inDataParams,
                                     const sTIDL_DataParams_t *outDataParams)
{
  int32_t status = TIDL_SUCCESS;
  int32_t i0, i1, i2, i3;
  int32_t numCols    = (int32_t)inDataParams->dimValues[TIDL_DIM_WIDTH];
  int32_t numRows    = (int32_t)inDataParams->dimValues[TIDL_DIM_HEIGHT];
  int32_t inPitch    = (int32_t)inDataParams->pitch[TIDL_LINE_PITCH];
  int32_t inChPitch  = (int32_t)inDataParams->pitch[TIDL_CHANNEL_PITCH];
  int32_t outPitch   = (int32_t)outDataParams->pitch[TIDL_LINE_PITCH];

  Tin  *inData = (Tin *)inPtr;
  Tout *outData= (Tout *)outPtr;

  int32_t numTotRoi = inDataParams->dimValues[TIDL_DIM_BATCH];
  Tin  targetVal;

  int32_t numInChannels = inDataParams->dimValues[TIDL_DIM_NUMCH];

  if(TIDL_SUCCESS == status)
  {
    if(params->axis == TIDL_DIM_HEIGHT) /*across height in NCHW format*/
    {
      OPENACC(data copyin(inData[:1+(numInChannels*inChPitch*(numTotRoi-1)) + ((numInChannels-1)* inChPitch) + ((numRows-1) * inPitch) + (numCols-1)]) \
                   copyout(outData[: 1+ (numRows*outPitch*(numTotRoi-1)) + ((numInChannels-1)* outPitch) + (numCols-1)]))
      OPENACC(parallel loop collapse(3))
      for(i0=0; i0<numTotRoi; i0++)
      {
        for(i1=0; i1<numInChannels; i1++)
        {
          for(i3=0; i3<numCols; i3++)
          {
            targetVal = inData[(numInChannels*inChPitch*i0) + (i1* inChPitch) + (0 * inPitch) + i3];
            if(params->ops == TIDL_inReduceOpMin) // reduceMin
            {
              OPENACC(loop)
              for(i2=1; i2<numRows; i2++)
              {
                if(inData[(numInChannels*inChPitch*i0) + (i1* inChPitch) + (i2 * inPitch) + i3] < targetVal)
                {
                  targetVal = inData[(numInChannels*inChPitch*i0) + (i1* inChPitch) + (i2 * inPitch) + i3];
                }
              }
            }
            if(params->ops == TIDL_inReduceOpMax) // reduceMax
            {
              OPENACC(loop)
              for(i2=1; i2<numRows; i2++)
              {
                  if(inData[(numInChannels*inChPitch*i0) + (i1* inChPitch) + (i2 * inPitch) + i3] > targetVal)
                  {
                    targetVal = inData[(numInChannels*inChPitch*i0) + (i1* inChPitch) + (i2 * inPitch) + i3];
                  }
              }
            }
            outData[(numRows*outPitch*i0) + (i1* outPitch) + i3] = targetVal;
          }
        }
      }
    }
    else
    {
      tidl_printf(0,"Not supported axis for reduce layer \n");
    }
  }

  return status;
}

 /**
 * @brief Reduce layer reference implementation
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
static int32_t TIDL_refReduceProcess(TIDL_Handle intAlgHandle,
                                        sTIDL_AlgLayer_t *algLayer,
                                        const sTIDL_Layer_t    *tidlLayer,
                                        sTIDL_ReduceParams_t *params,
                                        void *inPtr,
                                        void *outPtr,
                                        const sTIDL_DataParams_t *inDataParams,
                                        const sTIDL_DataParams_t *outDataParams)
{
  int32_t status = TIDL_SUCCESS;
  int32_t layerIdx = algLayer->layerIdx;

  if(TIDL_SignedChar == ((int32_t)inDataParams->elementType))
  {
    status = TIDL_refReduceCore((int8_t *)inPtr,
                                    (int8_t *)outPtr,
                                    intAlgHandle,
                                    layerIdx,
                                    params,
                                    algLayer,
                                    inDataParams,
                                    outDataParams);
  }
  else if(TIDL_UnsignedChar == ((int32_t)inDataParams->elementType))
  {
    status = TIDL_refReduceCore((uint8_t *)inPtr,
                                    (uint8_t *)outPtr,
                                    intAlgHandle,
                                    layerIdx,
                                    params,
                                    algLayer,
                                    inDataParams,
                                    outDataParams);
  }
  else if(TIDL_SignedShort == ((int32_t)inDataParams->elementType))
  {
    status = TIDL_refReduceCore((int16_t *)inPtr,
                                    (int16_t *)outPtr,
                                    intAlgHandle,
                                    layerIdx,
                                    params,
                                    algLayer,
                                    inDataParams,
                                    outDataParams);
  }
  else if(TIDL_UnsignedShort == ((int32_t)inDataParams->elementType))
  {
    status = TIDL_refReduceCore((uint16_t *)inPtr,
                                    (uint16_t *)outPtr,
                                    intAlgHandle,
                                    layerIdx,
                                    params,
                                    algLayer,
                                    inDataParams,
                                    outDataParams);
  }
  else if(TIDL_SinglePrecFloat == ((int32_t)inDataParams->elementType))
  {
    status = TIDL_refReduceCore((float32_tidl *)inPtr,
                                    (float32_tidl *)outPtr,
                                    intAlgHandle,
                                    layerIdx,
                                    params,
                                    algLayer,
                                    inDataParams,
                                    outDataParams);
  }
  else
  {
    status = TIDL_ERR_FAILURE;
  }
  TIDL_enableL1DandL2CacheWb();
  return status;
}

int32_t TIDL_reduceProcessNew(TIDL_NetworkCommonParams *commonParams,
                              sTIDL_AlgLayer_t     * algLayer,
                              sTIDL_Layer_t        * tidlLayer,
                              void                 * inPtrs[],
                              void                 * outPtrs[],
                              int32_t                layerIdx)
{
  int32_t  status      = TIDL_SUCCESS;
  uint8_t  (*inPtr)[]     = (uint8_t (*)[])(inPtrs[0]);
  int8_t   (*outPtr)[]    = (int8_t (*)[])(outPtrs[0]);

  TIDL_Obj intAlgObj;
  TIDL_CreateParams createParams;
  memcpy(&createParams, commonParams->createParams, sizeof(TIDL_CreateParams));
  intAlgObj.createParams = (TIDL_CreateParams *) &createParams;
  const sLink_t  *link, *procLink;
  const sBufParams_t *dstBufParams;
  int32_t outWidth, outChannels, tidlChannelPitch, tidlLinePitch, tidlDimNumch;
  sWorkloadUnit_t *workloadUnit = (sWorkloadUnit_t *)algLayer->workloadUnit;

  if ((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) != TIDL_FLOW_CTRL_REF_ONLY)
  {
    TIDL_GetTensorDimensions( commonParams->net,
                              algLayer->workloadUnit,
                              layerIdx,
                              NULL,
                              NULL,
                              NULL,
                              &outWidth,
                              NULL,
                              &outChannels);

    for (int32_t linkIdx =0 ; linkIdx < algLayer->workloadUnit->numLinks; linkIdx++)
    {
      link = getLinkPtr(workloadUnit, NOT_VALID, linkIdx);
      if(link->type == LINK_PROC)
      {
        procLink = link;
      }
    }

    dstBufParams = (const sBufParams_t *) TIDL_getBufferParams(&(procLink->sink), commonParams->gcHelperHandle);

    tidlChannelPitch = procLink->sink.flowDim[FLOW_PIPELINE][0];
    tidlLinePitch = outWidth + dstBufParams->padParams.cols;
    tidlDimNumch = outChannels;

    status = TIDL_deviceUtilsCommonProcess(commonParams,
                                            algLayer,
                                            tidlLayer,
                                            inPtrs,
                                            outPtrs,
                                            layerIdx);

    if((dstBufParams->dataType == DATA_8_BIT) || (dstBufParams->dataType == DATA_8S_BIT))
    {
      int8_t* outLastLinePtr = ((int8_t*)outPtrs[0]) + (tidlDimNumch * tidlChannelPitch - tidlLinePitch);

      OPENACC(data copyout(outLastLinePtr[:algLayer->scratchSize-1]))
      OPENACC(parallel loop)
      for(int32_t i0 = 0; i0 < algLayer->scratchSize; i0++)
      {
        outLastLinePtr[i0] = ((int8_t*)algLayer->layerParams.reduceParams.ptrToScratch)[i0];
      }
    }
    else if((dstBufParams->dataType == DATA_16_BIT) || (dstBufParams->dataType == DATA_16S_BIT))
    {
      int16_t* outLastLinePtr = (int16_t*)(((int8_t*)outPtrs[0]) + (tidlDimNumch * tidlChannelPitch - tidlLinePitch)*2);

      OPENACC(data copyout(outLastLinePtr[:algLayer->scratchSize-1]))
      OPENACC(parallel loop)
      for(int32_t i0 = 0; i0 < algLayer->scratchSize; i0++)
      {
        outLastLinePtr[i0] = ((int16_t*)algLayer->layerParams.reduceParams.ptrToScratch)[i0];
      }
    }
  }
  else /* if ((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == TIDL_FLOW_CTRL_REF_ONLY) */
  {
    sTIDL_ReduceParams_t   *params = &tidlLayer->layerParams.reduceParams;
    sTIDL_DataParams_t        *inDataParams;
    inDataParams   = &commonParams->createParams->net->TIDLLayers[algLayer->inLayerIdx[0]].outData;

    status = TIDL_refReduceProcess(&intAlgObj,
                                      algLayer,
                                      tidlLayer,
                                      params,
                                      inPtr,
                                      outPtr,
                                      inDataParams,
                                      &tidlLayer->outData);
  }

  if((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_STAT) == TIDL_FLOW_CTRL_REF_STAT)
  {
    int32_t layerIdx;
    layerIdx = algLayer->layerIdx;
    TIDL_UpdateScaleFactors(&intAlgObj, layerIdx, 0, 0, 1);
  }

  return status;
}

