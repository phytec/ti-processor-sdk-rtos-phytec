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
 @file    tidl_stalgo.c
 @brief   This file defines APIS for TIDL ST algorithms.
 @version 0.1 (Mar 2020) : Intial Version
 ----------------------------------------------------------------------------
*/
#include "stdlib.h"
#include "stdint.h"
#include "tidl_stalgo.h"
#include "tidl_priv_algo.h"
#include "tidl_commonUtils.h"
#include "tidl_deviceInfo.h"
#include "tidl_device_functions.h"
#include "tidl_device_utils.h"
#include "tidl_function_mapping.h"

//#ifdef __ENABLE_OTF_ST

int32_t TIDL_privGetContextSize(void)
{
  return (sizeof(sTIDL_PrivAlgoHanlde_t));
}

int32_t TIDL_getInProcHeight(int32_t outTileHeight, sTIDL_Layer_t *currentLayer, int32_t calcTileHeight, int32_t targetDevice, sTIDL_virtualPad *virtualPad)
{
  int32_t retVal = 0, paddingLines = 0;
  if(currentLayer->layerType == TIDL_ConvolutionLayer)
  {

    int32_t strideH    = currentLayer->layerParams.convParams.strideH;
    /* TODO : Need to re-visit this code */
    retVal = (outTileHeight * strideH);
    if(1 == calcTileHeight)
    {
      paddingLines = (currentLayer->layerParams.convParams.kernelH/2 - currentLayer->layerParams.convParams.padH);
      /* In case of dilated covolution kernel might have more padding which will make this value negative */
      if(paddingLines < 0)
      {
        paddingLines = 0;
      }
      retVal += paddingLines;
    }
    if(TIDL_isPadOTF(targetDevice) == TRUE) //Pad
    {
      /*
      If device is AEP:
      In this case 2 options calcTileHeight = 1 -> Actual requirement in physical domain
      */
     if(calcTileHeight == 1)
     {
      /*Physical domain requirement*/
      retVal   = ((outTileHeight - 1) * strideH) + ((currentLayer->layerParams.convParams.kernelH - 1) * currentLayer->layerParams.convParams.dilationH + 1) - virtualPad->padT - virtualPad->padB;
     }
     else
     {
      /*Virtual domain requirement*/
      retVal   = ((outTileHeight - 1) * strideH) + ((currentLayer->layerParams.convParams.kernelH - 1) * currentLayer->layerParams.convParams.dilationH + 1);
     }
    }

  }
  else if(currentLayer->layerType == TIDL_Deconv2DLayer)
  {
    int32_t strideH    = currentLayer->layerParams.convParams.strideH;
    /* TODO : Need to re-visit this code */
    if (strideH != 0)
    {
      retVal = ((outTileHeight / strideH));
    }
    else
    {
      tidl_printf(0, "This deconvolution layer is not currently supported for superTiling. \n");
      retVal = -1;
    }
  }
  else if ((currentLayer->layerType == TIDL_ArgMaxLayer) ||
           (currentLayer->layerType == TIDL_EltWiseLayer) ||
           (currentLayer->layerType == TIDL_BatchNormLayer) ||
           (currentLayer->layerType == TIDL_BatchReshapeLayer) ||
           (currentLayer->layerType == TIDL_ConcatLayer) ||
           (currentLayer->layerType == TIDL_SliceLayer))
  {
    retVal = outTileHeight;
  }
  else if(currentLayer->layerType == TIDL_ResizeLayer)
  {
    int32_t resizeRatio = currentLayer->layerParams.resizeParams.resizeRatio[TIDL_DIM_HEIGHT];
    int32_t resizeType  = currentLayer->layerParams.resizeParams.mode;
    retVal = ((outTileHeight / resizeRatio));

    if((1 == calcTileHeight) && (TIDL_ResizeBilinear == resizeType))
    {
      /* Assume 4x resize and calculate number of lines needed to produce 4 valid output lines, say 0,1,2,3.
       * If there are two valid input lines: 0, 1 then 0 line will be copied to get -1,0,1.
       * Then using -1 and 0 kernel will produce 2 valid lines: 0,1
       * Then using 0 and 1 kernel will produce 2 valid lines: 2,3
       * Hence total valid input is one more valid line */
      retVal += 1;
    }
  }
  else if(currentLayer->layerType == TIDL_ColorConversionLayer)
  {
    retVal = outTileHeight / 2;
  }
  else if(currentLayer->layerType == TIDL_PoolingLayer)
  {
    int32_t strideH    = currentLayer->layerParams.poolParams.strideH;
    /* TODO : Need to re-visit this code */
    retVal = (outTileHeight * strideH);
    if(1 == calcTileHeight)
    {
      /*
      if(poolParams.strideH > 1)
      {
          TIDLPCLayers.outData.dimValues[TIDL_DIM_HEIGHT] = ceil(((TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT] +
            poolParams.padH*2.0) - (poolParams.kernelH)) / poolParams.strideH) + 1;
      }
      else
      {
        if(poolParams.padH > 0)
        {
          TIDLPCLayers.outData.dimValues[TIDL_DIM_HEIGHT] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT];
        }
        else
        {
         TIDLPCLayers.outData.dimValues[TIDL_DIM_HEIGHT] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT] - (poolParams.kernelH-1);
        }
      }
      */
      // Calculating input tile height from ouput tile height using above condtitions */
      if (strideH > 1)
      {
        retVal = ((outTileHeight - 1) * strideH ) + currentLayer->layerParams.poolParams.kernelH - (currentLayer->layerParams.poolParams.padH*2.0);
      }
      else
      {
        if(currentLayer->layerParams.poolParams.padH > 0)
        {
          retVal = outTileHeight;
        }
        else
        {
          retVal = outTileHeight + (currentLayer->layerParams.poolParams.kernelH-1);
        }
      }
    }
    if(TIDL_isPadOTF(targetDevice) == TRUE)
    {  
      if(calcTileHeight == 1)
      {
        /*Physical domain requirement*/
        retVal   = ((outTileHeight - 1) * strideH) + currentLayer->layerParams.poolParams.kernelH - virtualPad->padT - virtualPad->padB;
      }
      else
      {
        /*Virtual domain requirement*/
        retVal   = ((outTileHeight - 1) * strideH) + currentLayer->layerParams.poolParams.kernelH;
      }
      
      #if 0
      /*
        If device is AEP:
        In this case 2 options calcTileHeight = 1 -> Actual requirement in physical domain
        0 -> Used in offset calculations: Subtracts by overlap needed so that base calculations are correct.
        Xin = (Xout - 1)*stride + Fr (Virtual Domain Calc)
      */
      retVal   = ((outTileHeight - 1) * strideH) + currentLayer->layerParams.poolParams.kernelH; //Lines needed to generate outlines..
      if(retVal < 0)
      {
        retVal = 0;
      }
      #endif
    }
  }
  else
  {
    tidl_printf(0, "This layer is not currently supported for superTiling. \n");
    retVal = -1;
  }
  return retVal;
}

int32_t TIDL_privGetNumHandles()
{
  int32_t numHandles = 1;
  return (numHandles);
}

void TIDL_privMoveToNextLayerIdx(TIDL_Handle  intAlgHandle, sTIDL_PrivAlgoHanlde_t *handle)
{
  sTIDL_Layer_t *TIDLLayer = &intAlgHandle->createParams->net->TIDLLayers[handle->layerIdx];
  if(TIDLLayer->layersGroupId ==  intAlgHandle->createParams->currLayersGroupId)
  {
    handle->algLayerIdx++;
  }
  handle->layerIdx++;

  if(handle->layerIdx == intAlgHandle->createParams->net->numLayers)
  {
    handle->layerIdx = 0;
    handle->algLayerIdx = 0;
    handle->stileIdx++;
    if(handle->stileIdx == handle->numTilesStg[handle->stgIdx])
    {
      handle->stileIdx = 0;
      handle->layerIdx = 0;
      handle->algLayerIdx = 0;
      handle->stgIdx++;
      if(handle->stgIdx == handle->numStGroups)
      {
        handle->layerIdx = -1;
      }
    }
  }
  return;
}
