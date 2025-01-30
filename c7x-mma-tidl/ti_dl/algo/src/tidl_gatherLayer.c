/*
*
* Copyright (c) {2022 - 2023} Texas Instruments Incorporated
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
 *  \file tidl_gatherLayer.c
 *
 *  \brief This file defines kernel functions for Gather layer
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "tidl_alg_int.h"
#include "tidl_commonUtils.h"
#include "tidl_gatherLayer.h"
#include "math.h"
#include <limits>


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

template <class Tin, class Tindx, class Tout>
static int32_t TIDL_refGather(Tin  *data,
                                     Tindx* indices,
                                     Tout *outPtr,
                                     TIDL_Handle intAlgHandle,
                                     int32_t layerIdx,
                                     sTIDL_GatherLayerParams_t *params,
                                     sTIDL_AlgLayer_t *algLayer,
                                     const sTIDL_DataParams_t *inDataParams,
                                     const sTIDL_DataParams_t *inIndicesParams,
                                     const sTIDL_DataParams_t *outDataParams)
{
  int32_t status = TIDL_SUCCESS;
  int32_t i0, i1;

  int32_t inIndicesNumCols    = (int32_t)inIndicesParams->dimValues[TIDL_DIM_WIDTH];

  int32_t inDataPitch         = (int32_t)inDataParams->pitch[TIDL_LINE_PITCH];
  int32_t inIndicesPitch      = (int32_t)inIndicesParams->pitch[TIDL_LINE_PITCH];
  int32_t outPitch            = (int32_t)outDataParams->pitch[TIDL_LINE_PITCH];


  data               = (Tin *)data + (inDataParams->padH*inDataPitch) + inDataParams->padW;
  indices            = (Tindx *)indices + (inIndicesParams->padH*inIndicesPitch) + inIndicesParams->padW;

  Tout *output= (Tout *)outPtr + (outDataParams->padH*outPitch) + outDataParams->padW;

  int32_t inDataNumRows = inDataParams->dimValues[TIDL_DIM_HEIGHT];
  int32_t inDataNumCols = inDataParams->dimValues[TIDL_DIM_WIDTH];

  int32_t src_index, dst_index, line_index;

  for(i0 = 0; i0 < inIndicesNumCols; i0++)
  {
    line_index = (int32_t)indices[i0];
    if (line_index < 0 || line_index > inDataNumRows)
    {
      tidl_printf(0,"Invalid Index\n");
      continue;
    }
    dst_index = i0 * inDataNumCols;
    src_index = line_index * inDataPitch;
    for (i1 = 0; i1 < inDataNumCols; i1++)
    {
      output[dst_index++] = (Tout)data[src_index++];
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
static int32_t TIDL_refGatherProcess(TIDL_Handle intAlgHandle,
                                        sTIDL_AlgLayer_t *algLayer,
                                        const sTIDL_Layer_t    *tidlLayer,
                                        sTIDL_GatherLayerParams_t *params,
                                        void *data,
                                        void *indices,
                                        void *outPtr,
                                        const sTIDL_DataParams_t *inDataParams,
                                        const sTIDL_DataParams_t *inIndicesParams,
                                        const sTIDL_DataParams_t *outDataParams)
{
  int32_t status = TIDL_SUCCESS;
  int32_t layerIdx = algLayer->layerIdx;

  if(TIDL_SignedChar == ((int32_t)inDataParams->elementType))
  {

    if(TIDL_SignedWord != ((int32_t)inIndicesParams->elementType))
    {
      tidl_printf(0,"Indice data type should int32");
      status = TIDL_ERR_FAILURE;
      return status;
    }
    status = TIDL_refGather((int8_t *)data,
                                    (int32_t *)indices,
                                    (int8_t *)outPtr,
                                    intAlgHandle,
                                    layerIdx,
                                    params,
                                    algLayer,
                                    inDataParams,
                                    inIndicesParams,
                                    outDataParams);
  }
  else if(TIDL_UnsignedChar == ((int32_t)inDataParams->elementType))
  {

    if(TIDL_SignedWord != ((int32_t)inIndicesParams->elementType))
    {
      tidl_printf(0,"Indice data type should int32");
      status = TIDL_ERR_FAILURE;
      return status;
    }

    status = TIDL_refGather((uint8_t *)data,
                                    (int32_t *)indices,
                                    (uint8_t *)outPtr,
                                    intAlgHandle,
                                    layerIdx,
                                    params,
                                    algLayer,
                                    inDataParams,
                                    inIndicesParams,
                                    outDataParams);
  }
  else if(TIDL_SignedShort == ((int32_t)inDataParams->elementType))
  {

    if(TIDL_SignedWord != ((int32_t)inIndicesParams->elementType))
    {
      tidl_printf(0,"Indice data type should int32");
      status = TIDL_ERR_FAILURE;
      return status;
    }

    status = TIDL_refGather((int16_t *)data,
                                    (int32_t *)indices,
                                    (int16_t *)outPtr,
                                    intAlgHandle,
                                    layerIdx,
                                    params,
                                    algLayer,
                                    inDataParams,
                                    inIndicesParams,
                                    outDataParams);
  }
  else if(TIDL_UnsignedShort == ((int32_t)inDataParams->elementType))
  {

    if(TIDL_SignedWord != ((int32_t)inIndicesParams->elementType))
    {
      tidl_printf(0,"Indice data type should int32");
      status = TIDL_ERR_FAILURE;
      return status;
    }

    status = TIDL_refGather((uint16_t *)data,
                                    (int32_t *)indices,
                                    (uint16_t *)outPtr,
                                    intAlgHandle,
                                    layerIdx,
                                    params,
                                    algLayer,
                                    inDataParams,
                                    inIndicesParams,
                                    outDataParams);
  }
  else if(TIDL_SinglePrecFloat == ((int32_t)inDataParams->elementType))
  {
    /*In stat collection , all input tensor is assumed as float.
      hence making indices as float in this flow
    */
    if(TIDL_SignedWord == ((int32_t)inIndicesParams->elementType))
    {
      status = TIDL_refGather((float32_tidl *)data,
                                      (int32_t *)indices,
                                      (float32_tidl *)outPtr,
                                      intAlgHandle,
                                      layerIdx,
                                      params,
                                      algLayer,
                                      inDataParams,
                                      inIndicesParams,
                                      outDataParams);
    }
    else
    {
      status = TIDL_refGather((float32_tidl *)data,
                                      (float32_tidl *)indices,
                                      (float32_tidl *)outPtr,
                                      intAlgHandle,
                                      layerIdx,
                                      params,
                                      algLayer,
                                      inDataParams,
                                      inIndicesParams,
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
/**
 * @brief Gather layer Process function
 *
 * @param intAlgHandle : tidl algorothm handle
 * @param algLayer : Pointer to the layer specific parameters
 * @param tidlLayer : Pointer to the common layer parameters
 * @param inPtrs : Pointer to input buffers to be processed
 * @param outPtrs : Pointer to output buffers to be processed
 * @param sysMems : sysMem Handle
 * @return  IALG_EOK   - Successful
 *          IALG_EFAIL - Unspecified error
 */
int32_t TIDL_gatherLayerProcessNew(TIDL_NetworkCommonParams *commonParams,
                                          sTIDL_AlgLayer_t     * algLayer,
                                          sTIDL_Layer_t        * tidlLayer,
                                          void                 * inPtrs[],
                                          void                 * outPtrs[],
                                          int32_t                layerIdx)
{

  int32_t  status      = TIDL_SUCCESS;

  int32_t dataIdx    = 0;
  int32_t indicesIdx = 1;


  uint8_t  (*data)[]   = (uint8_t (*)[])(inPtrs[dataIdx]);
  uint8_t  (*indices)[]= (uint8_t (*)[])(inPtrs[indicesIdx]);
 

  int8_t   (*outPtr)[]    = (int8_t (*)[])(outPtrs[0]);
  uint32_t flowCtrl = commonParams->createParams->flowCtrl; //  L3

  sTIDL_GatherLayerParams_t   *params = &tidlLayer->layerParams.gatherParams;
  sTIDL_DataParams_t        *inDataParams;
  sTIDL_DataParams_t        *inIndicesParams;
 
  inDataParams   = &commonParams->createParams->net->TIDLLayers[algLayer->inLayerIdx[0]].outData;
  inIndicesParams= &commonParams->createParams->net->TIDLLayers[algLayer->inLayerIdx[1]].outData;

  TIDL_Obj intAlgObj;
  TIDL_CreateParams createParams;
  memcpy(&createParams, commonParams->createParams, sizeof(TIDL_CreateParams));
  intAlgObj.createParams = (TIDL_CreateParams *) &createParams;
  sTIDL_sysMemHandle_t *sysMems = commonParams->tidlCommonParams->sysMems;

  if ((flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) != TIDL_FLOW_CTRL_REF_ONLY)
  {
    status = TIDL_gatherLayerDspProcessNew(commonParams, algLayer, tidlLayer, inPtrs, outPtrs, sysMems);
    /* This layer does not use standard WL flow, so need to handle layer level cache writeback explicitly in this layer */ 
    TIDL_enableL1DCacheWb();
  }
  else /* if ((flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == TIDL_FLOW_CTRL_REF_ONLY) */
  {
    status = TIDL_refGatherProcess(&intAlgObj,
                                      algLayer,
                                      tidlLayer,
                                      params,
                                      data,
                                      indices,
                                      outPtr,
                                      inDataParams,
                                      inIndicesParams,
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

int32_t TIDL_gatherLayerAllocNew(const TIDL_LayerSpecificParams *layerSpecificParams,
                                   const TIDL_NetworkCommonParams *commonParams,
                                   int32_t layerIdx,
                                   int32_t memorySize[TIDL_LAYER_MEMORY_MAX])
{
  int32_t status = IALG_EOK;

  if ((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0)
  {
    int32_t handleSize = 0;

    handleSize = 0;

    memorySize[TIDL_LAYER_MEMORY_SCRATCH]    += 0;
    memorySize[TIDL_LAYER_MEMORY_PERSISTENT] += handleSize;
    memorySize[TIDL_LAYER_MEMORY_OUTPUT]     += 0;
  }
  else
  {
    status = TIDL_deviceCommonRefAlloc(layerSpecificParams, commonParams, layerIdx, memorySize);
  }

  return status;
}

int32_t TIDL_gatherLayerInitNew(const TIDL_LayerSpecificParams *layerSpecificParams,
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
    void * handle;
    int32_t currOffset = 0;
    sTIDL_Layer_t *tidlLayer = &commonParams->createParams->net->TIDLLayers[layerIdx];

    handleSize = 0;

    TIDL_AllocatePtr((intptr_t)memory[TIDL_LAYER_MEMORY_PERSISTENT],
                &currOffset,
                handleSize,
                TIDL_ALIGNMENT_SIZE,
                &handle);

    algLayer->workloadHandle = handle;

    status = TIDL_gatherLayerKernelInitNew(commonParams->createParams,
                                              algLayer,
                                              tidlLayer, 
                                              handle);

    memorySize[TIDL_LAYER_MEMORY_SCRATCH]    += 0;
    memorySize[TIDL_LAYER_MEMORY_PERSISTENT] += handleSize;
    memorySize[TIDL_LAYER_MEMORY_OUTPUT]     += 0;
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
