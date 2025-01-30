#include "tidl_reshape.h"
using namespace c7x;
#include "gc.h"
#include "gc_helper.h"

int32_t TIDL_reshapeAllocNew(const TIDL_LayerSpecificParams *layerSpecificParams,
                                   const TIDL_NetworkCommonParams *commonParams,
                                   int32_t layerIdx,
                                   int32_t memorySize[TIDL_LAYER_MEMORY_MAX])
{
  int32_t status = IALG_EOK;

  if (commonParams->gcHelperHandle == NULL)
  {
    status = TIDL_deviceCommonRefAlloc(layerSpecificParams, commonParams, layerIdx, memorySize);
  }
  return status;
}

int32_t TIDL_reshapeInitNew(const TIDL_LayerSpecificParams *layerSpecificParams,
                                const TIDL_NetworkCommonParams  *commonParams,
                                sTIDL_AlgLayer_t               *algLayer,
                                int32_t layerIdx,
                                uint8_t *memory[TIDL_LAYER_MEMORY_MAX],
                                int32_t memorySize[TIDL_LAYER_MEMORY_MAX],
                                void    **outPtr)
{
  int32_t status = IALG_EOK;

  if (commonParams->gcHelperHandle == NULL)
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

int32_t TIDL_reshapeProcessNew(TIDL_NetworkCommonParams *commonParams,
                              sTIDL_AlgLayer_t     * algLayer,
                              sTIDL_Layer_t        * tidlLayer,
                              void                 * inPtrs[],
                              void                 * outPtrs[],
                              int32_t                layerIdx)
{
    int32_t  status = TIDL_SUCCESS;

    /*Copy the data for reference flow if perfsimInfo not available*/
    if(commonParams->gcHelperHandle == NULL)   
    {
      int32_t cpSize = tidlLayer->outData.dimValues[TIDL_DIM_BATCH]*tidlLayer->outData.pitch[TIDL_ROI_PITCH];
      int32_t outElementSize     = TIDL_getDatElementSize(tidlLayer->outData.elementType);

      memcpy((void*)((size_t)outPtrs[0]), (void*)((size_t)inPtrs[0]), outElementSize*cpSize);
      TIDL_enableL1DandL2CacheWb();
    }
    else if ((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == TIDL_FLOW_CTRL_REF_ONLY)
    {
      if (inPtrs[0] != outPtrs[0])
      {
        tidl_printf(2, "ForceInPlace Constraint for Reshape Failed, Copying Data!\n");
        sTIDL_Layer_t* layer = &commonParams->createParams->net->TIDLLayers[algLayer->inLayerIdx[0]];
        sTIDL_DataParams_t *inDataParams   = &layer->outData;
        int32_t cpSize = inDataParams->dimValues[TIDL_DIM_HEIGHT]*inDataParams->dimValues[TIDL_DIM_WIDTH];
        int32_t channels = inDataParams->dimValues[TIDL_DIM_NUMCH];
        if (layer->layerType == TIDL_DataConvertLayer)
        {
          sTIDL_dataConvertParams_t* params = &layer->layerParams.dataConvertParams;
          if (params->layout == TIDL_LT_NCHW && params->outLayout == TIDL_LT_NHWC)
          {
            cpSize = layer->outData.dimValues[TIDL_DIM_NUMCH] * layer->outData.dimValues[TIDL_DIM_WIDTH];
            channels = layer->outData.dimValues[TIDL_DIM_HEIGHT];
          }
        }
        int32_t outElementSize     = TIDL_getDatElementSize(tidlLayer->outData.elementType);
        int32_t channelPitch = inDataParams->pitch[TIDL_CHANNEL_PITCH];

        void *in = inPtrs[0];
        void *out = outPtrs[0];
     
        for (int32_t i=0;i<inDataParams->dimValues[TIDL_DIM_BATCH];i++)
        {
          for (int32_t j=0;j<channels;j++)
          {
            memcpy((void*)((uint8_t*)out), (void*)((uint8_t*)in), outElementSize*cpSize);
            in   = (uint8_t*)in + channelPitch * outElementSize;
            out  = (uint8_t*)out + cpSize * outElementSize;           
          }
        }
        TIDL_enableL1DandL2CacheWb();
      }
    }
    else if ((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) != TIDL_FLOW_CTRL_REF_ONLY)
    {
      /** Do a memcpy if NC didn't satisfy the forceInPlace constraint */

      if (inPtrs[0] != outPtrs[0])
      {
        tidl_printf(2, "ForceInPlace Constraint for Reshape Failed, Copying Data!\n");
        sTIDL_Layer_t* layer = &commonParams->createParams->net->TIDLLayers[algLayer->inLayerIdx[0]];
        sTIDL_DataParams_t *inDataParams   = &layer->outData;
        int32_t cpSize = inDataParams->dimValues[TIDL_DIM_HEIGHT]*inDataParams->dimValues[TIDL_DIM_WIDTH];
        int32_t channelPitch = inDataParams->pitch[TIDL_CHANNEL_PITCH];
        int32_t channels = inDataParams->dimValues[TIDL_DIM_NUMCH];
        if (layer->layerType == TIDL_DataConvertLayer)
        {
          /** Channel pitch (=W*C)is differently interpreted for layout data convert, hence copy with dataconvert pitch*/
          sTIDL_dataConvertParams_t* params = &layer->layerParams.dataConvertParams;
          if (params->layout == TIDL_LT_NCHW && params->outLayout == TIDL_LT_NHWC)
          {
            cpSize = layer->outData.dimValues[TIDL_DIM_NUMCH] * layer->outData.dimValues[TIDL_DIM_WIDTH];
            channels = layer->outData.dimValues[TIDL_DIM_HEIGHT];
          }
        }
        int32_t outElementSize     = TIDL_getDatElementSize(tidlLayer->outData.elementType); 
        
        int32_t tensorSize = outElementSize*cpSize;
        void* out = outPtrs[0];
        void* in = inPtrs[0];

        for (int32_t i=0;i<inDataParams->dimValues[TIDL_DIM_BATCH];i++)
        {
          for (int32_t k=0;k<channels;k++)
          {
            int32_t tempBufHeight = (int32_t)tensorSize/TIDL_SAT_HI_UINT16;
            if(tensorSize > TIDL_SAT_HI_UINT16)
            {
                TIDL_memcpy2D((void*)out,
                              (void*)in,
                              TIDL_SAT_HI_UINT16,
                              (uint16_t)tempBufHeight,
                              TIDL_SAT_HI_UINT16,
                              TIDL_SAT_HI_UINT16,
                              commonParams->tidlCommonParams->dmaUtilsContext,
                              (uint8_t*)algLayer->memcpyTr);
                tensorSize = tensorSize - ((tensorSize/TIDL_SAT_HI_UINT16)*TIDL_SAT_HI_UINT16);
            }
            if(tensorSize > 0U)
            {
                TIDL_memcpy2D((((uint8_t *)out)+ tempBufHeight*TIDL_SAT_HI_UINT16),
                              (((uint8_t *)in) + tempBufHeight*TIDL_SAT_HI_UINT16),
                              (uint16_t)tensorSize,
                              1,
                              1,
                              1,
                              commonParams->tidlCommonParams->dmaUtilsContext,
                              (uint8_t*)algLayer->memcpyTr);
            }
            out = (uint8_t*)out + cpSize * outElementSize;
            in  = (uint8_t*)in   + channelPitch * outElementSize;
          }
        }
      }
    }

    return status;
}
