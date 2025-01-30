/*
* module name       :TIDL
*
* module descripton :TI Deep learning Library module is TIs CNN/DNN
*                    acceleration on TI DSP
*
* Copyright (C) 2019-2020 Texas Instruments Incorporated - http://www.ti.com/
*
* ALL RIGHTS RESERVED
*
*
*/

/**
 ----------------------------------------------------------------------------
 @file    tidl_stalgo_workload.c
 @brief   This file defines APIS for TIDL ST algorithms for the workload flow.
 @version 0.1 (Mar 2020) : Intial Version
 ----------------------------------------------------------------------------
*/

#include "tidl_stalgo.h"
#include "tidl_priv_algo.h"
#include "tidl_commonUtils.h"
#include "gc_helper.h"

/* ========================================================================== */
/*                         Public  Function Declarations                      */
/* ========================================================================== */

void TIDL_privSetTensorHeights(TIDL_CreateParams *createParams,
                            sTIDL_AlgLayer_t     * alglayerParams,
                            sTIDL_AlgLayer_t     * algLayer,
                            const sWorkloadUnit_t *workloadUnit,
                            int32_t inHeightOrig[TIDL_NUM_IN_BUFS],
                            int32_t inPitchOrig[TIDL_NUM_IN_BUFS],
                            int32_t inPitchRoiOrig[TIDL_NUM_IN_BUFS],
                            int32_t outHeightOrig[TIDL_NUM_OUT_BUFS],
                            int32_t outPitchOrig[TIDL_NUM_OUT_BUFS],
                            int32_t outPitchRoiOrig[TIDL_NUM_OUT_BUFS])
{
  sTIDL_Layer_t  *tidlLayer = &createParams->net->TIDLLayers[algLayer->layerIdx];
  int32_t numInputBuf   = tidlLayer->numInBufs;

  algLayer->procType = 0;

  if ((algLayer->gcHelperHandle != NULL) && (workloadUnit != NULL))
  {
    sWLMetaDataID_t wlMetaData;
    getWLMetaDataID(workloadUnit->metadata, &wlMetaData);
  
    if (((!(wlMetaData.tileType == NO_TILE)) && 
        (!(tidlLayer->layerType == TIDL_PoolingLayer && tidlLayer->layerParams.poolParams.kernelH == 0)) ) ||
        (createParams->net->inferenceMode == TIDL_inferenceModeLowLatency && createParams->net->TIDLLayers[algLayer->layerIdx].multiCoreMode != TIDL_NOT_MULTI_CORE) )/*we Over Write Data Params with WL info only for layers that are runnig on multiple cores*/
                                                                                                                                                                      /*as layers running on single core will have Correct Data Params in net */
    {
      algLayer->procType = TIDL_getAlgProcType(workloadUnit);

      /* Save old height values and write the tile height to the net structure */
      for(int32_t i = 0; i < numInputBuf; i++)
      {
        sTIDL_Layer_t *inTidlLayer = &createParams->net->TIDLLayers[algLayer->inLayerIdx[i]];
        inHeightOrig[i] = inTidlLayer->outData.dimValues[TIDL_DIM_HEIGHT];
        /* For Layout conversion data convert layer, NC updates layout dimensions resulting in incorrect inTileHeight -- use from layer data params since this layer
        anyways runs on single core */
        if(! ((tidlLayer->layerType == TIDL_DataConvertLayer) &&
              (tidlLayer->layerParams.dataConvertParams.layout != tidlLayer->layerParams.dataConvertParams.outLayout)))
        {
          inTidlLayer->outData.dimValues[TIDL_DIM_HEIGHT] = wlMetaData.inTileHeight;
        }
        inPitchOrig[i] = inTidlLayer->outData.pitch[TIDL_CHANNEL_PITCH];
        inPitchRoiOrig[i] = inTidlLayer->outData.pitch[TIDL_CHANNEL_PITCH] * inTidlLayer->outData.dimValues[TIDL_DIM_NUMCH];
        sLink_t* pLink;
        /* Fetch pitch from input src link rather than using algLayer->inWorkloadIdx since the latter won't work for cases
        where current layer is multi core and previous layer is single core for coreId > coreStartIdx. Example : Crop split */
        for(int linkIdx = 0; linkIdx < workloadUnit->numLinks; linkIdx++)
        {
          pLink = getLinkPtr(workloadUnit, NOT_VALID, linkIdx);
          for(int jointIdx = 0; jointIdx < pLink->numSrc; jointIdx++)
          {
            const sBufParams_t* pInBufParams = getBufParamsFromBufIndex(algLayer->gcHelperHandle, pLink->src[jointIdx].bufDBindex);
            if((pInBufParams != NULL) && (pInBufParams->type == BUF_FM_FULL) && (pLink->subType == LINK_X_FM_IN_FULL_TO_PART)) /*Input Feature Map Link*/
            {
              sMetaDataID_t metaDataId;
              getMetaDataID(pInBufParams->dataId, &metaDataId);
              
              if(inTidlLayer->layerType == TIDL_ReshapeLayer)
              {
                if(inTidlLayer->multiCoreMode  == TIDL_NOT_MULTI_CORE)
                {
                  inTidlLayer->outData.pitch[TIDL_CHANNEL_PITCH] = inHeightOrig[i] * inTidlLayer->outData.dimValues[TIDL_DIM_WIDTH];
                }
                else
                {
                  inTidlLayer->outData.pitch[TIDL_CHANNEL_PITCH] = inTidlLayer->outData.dimValues[TIDL_DIM_HEIGHT] * inTidlLayer->outData.dimValues[TIDL_DIM_WIDTH];
                }
              }
              else if(inTidlLayer->outData.dataId == metaDataId.layerId)
              {
                inTidlLayer->outData.pitch[TIDL_CHANNEL_PITCH] = pInBufParams->bufWidth;
              }
              inTidlLayer->outData.pitch[TIDL_ROI_PITCH] = inTidlLayer->outData.pitch[TIDL_CHANNEL_PITCH] * inTidlLayer->outData.dimValues[TIDL_DIM_NUMCH];
            }
          }
        }
      }
      
      outHeightOrig[0] = tidlLayer->outData.dimValues[TIDL_DIM_HEIGHT];
      tidlLayer->outData.dimValues[TIDL_DIM_HEIGHT] = wlMetaData.outTileHeight;  

      const sBufParams_t* pOutBufParams = getBufParamsFromBufIndex(algLayer->gcHelperHandle, algLayer->outBufIdxWl);
      outPitchOrig[0] = tidlLayer->outData.pitch[TIDL_CHANNEL_PITCH];
      outPitchRoiOrig[0] = tidlLayer->outData.pitch[TIDL_ROI_PITCH];
      tidlLayer->outData.pitch[TIDL_CHANNEL_PITCH] = pOutBufParams->bufWidth;
      tidlLayer->outData.pitch[TIDL_ROI_PITCH] = tidlLayer->outData.pitch[TIDL_CHANNEL_PITCH] * tidlLayer->outData.dimValues[TIDL_DIM_NUMCH];
    }
  }

  return;
}

void TIDL_privRestoreTensorHeights(TIDL_CreateParams *createParams,
                            sTIDL_AlgLayer_t     * algLayer,
                            int32_t inHeightOrig[TIDL_NUM_IN_BUFS],
                            int32_t inPitchOrig[TIDL_NUM_IN_BUFS],
                            int32_t inPitchRoiOrig[TIDL_NUM_IN_BUFS],
                            int32_t outHeightOrig[TIDL_NUM_OUT_BUFS],
                            int32_t outPitchOrig[TIDL_NUM_OUT_BUFS],
                            int32_t outPitchRoiOrig[TIDL_NUM_OUT_BUFS])
{
  sTIDL_Layer_t  *tidlLayer = &createParams->net->TIDLLayers[algLayer->layerIdx];
  int32_t numInputBuf   = tidlLayer->numInBufs;

  // Situation might become an issue if concat axis is along height and no st since workload can contain only one height
  if ((algLayer->gcHelperHandle != NULL))
  {
    if (((!(algLayer->wlPadParams.tileType == NO_TILE)) && 
        (!(tidlLayer->layerType == TIDL_PoolingLayer && tidlLayer->layerParams.poolParams.kernelH == 0)) ) ||
        (createParams->net->inferenceMode == TIDL_inferenceModeLowLatency && createParams->net->TIDLLayers[algLayer->layerIdx].multiCoreMode != TIDL_NOT_MULTI_CORE) )
    {
      for(int32_t i=0; i < numInputBuf; i++)
      {
        sTIDL_Layer_t *inTidlLayer = &createParams->net->TIDLLayers[algLayer->inLayerIdx[i]];
        inTidlLayer->outData.dimValues[TIDL_DIM_HEIGHT] = inHeightOrig[i];
        inTidlLayer->outData.pitch[TIDL_CHANNEL_PITCH] = inPitchOrig[i];
        inTidlLayer->outData.pitch[TIDL_ROI_PITCH] = inPitchRoiOrig[i];
      }
      tidlLayer->outData.dimValues[TIDL_DIM_HEIGHT] = outHeightOrig[0]; 
      tidlLayer->outData.pitch[TIDL_CHANNEL_PITCH] = outPitchOrig[0]; 
      tidlLayer->outData.pitch[TIDL_ROI_PITCH] = outPitchRoiOrig[0]; 
    }
  }

  return;
}

int32_t TIDL_getAlgProcType(const sWorkloadUnit_t *workloadUnit)
{
  int32_t procType = 0;
  
  if (workloadUnit != NULL)
  {
    sWLMetaDataID_t wlMetaData;
    getWLMetaDataID(workloadUnit->metadata, &wlMetaData);

    if ( wlMetaData.tileType == FIRST_TILE )
    {
      procType = TIDL_ST_TYPE_FIRST;
    }
    else if ( wlMetaData.tileType == LAST_TILE )
    {
      procType = TIDL_ST_TYPE_LAST;
    }
    else if ( wlMetaData.tileType == NORMAL_TILE )
    {
      procType  = TIDL_ST_TYPE_NORM;
    }
  }
  
  return procType;
}
