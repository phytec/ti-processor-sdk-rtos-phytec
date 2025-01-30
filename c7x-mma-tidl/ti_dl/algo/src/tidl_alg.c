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
@file    tidl_alg.c
@brief   This file contains TILD alg inteface implementation
@version 0.1 (Oct 2016) : Initial version [ADK]
@version 0.5 (Jan 2017) : Cleaned up [ADK]
----------------------------------------------------------------------------
*/
#include "tidl_alg_int.h"
#include "tidl_types.h"
#include "tidl_layer.h"
#include "itidl_ti.h"
#include "perfsim.h"
#include "tidl_priv_algo.h"
#include "tidl_preEmption.h"
#include "tidl_deviceInfo.h"
#include "tidl_device_functions.h"
#include "tidl_device_utils.h"
#include "tidl_alg_utils.h"
#include "tidl_commonUtils.h"
#include "tidl_function_mapping.h"
#include "workload_ref_exec.h"
#include "gc_helper.h"

#define TEMP_FORCE_OUTPUT_TO_DDR (0)
#define VAILD_DDR_BUFFER         (1U)
#define TIDL_DDR_EXCESS_SCRATCH  (512*1024U)


#ifdef BUILD_WITH_CUDA
#include "tidl_cuda.h"
#endif
#include "math.h"
#include "float.h"
#ifdef HOST_EMULATION
#include "time.h"
#endif

#define TIDL_L3_SCRATCH_SIZE (128U*1024U)
#define TIDL_L3_MSMC_DDR_STAGING_MEM  ( 1025U * 1024U)

#define USE_16BIT_BIAS_FOR_8BIT_MODE (1)

#if (ENABLE_TRACE_PROFILE) && defined(CORE_DSP)
int64_t t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, \
accCycle1,accCycle2,accCycle3,accCycle4,accCycle5,accCycle6;
#endif

#define TIDL_DDR_MEMREC_CACHEABLE        ((IALG_MemSpace)IALG_EXTERNAL)
#if defined (SOC_J784S4)
#define TIDL_DDR_MEMREC_NON_CACHEABLE    ((IALG_MemSpace)IALG_EXTERNAL_NON_CACHEABLE)
#else
#define TIDL_DDR_MEMREC_NON_CACHEABLE    ((IALG_MemSpace)IALG_EXTERNAL)
#endif
/**
* IVISION interface of the algorithm
*/
const int32_t ImplementationID =0;

const IVISION_Fxns TIDL_VISION_FXNS =
{
  {
    (void*) (&ImplementationID),
    &TIDL_activate,
    &TIDL_alloc,
    (int32_t (*)(IALG_Handle, IALG_Cmd, IALG_Status*))NULL,
    &TIDL_deactivate,
    &TIDL_free,
    &TIDL_init,
    (void (*)(IALG_Handle, const IALG_MemRec*, IALG_Handle, const IALG_Params*))NULL,
    &TIDL_numAlloc
  },
  &TIDL_process,
  &TIDL_control
};

void TIDL_play(sTIDL_Network_t *pNet, int32_t mode);

/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION
@fn         TIDL_numAlloc
@brief      Function to return the number of memory table needed by algorithm
@remarks    None
@return     Number of memory table
----------------------------------------------------------------------------
*/
int32_t TIDL_numAlloc(void)
{
  return (int32_t)MAX_NUM_MEMRECS;
}

static int32_t TIDL_getMaxParamSizeInBytes(sTIDL_Layer_t * tidlLayer)
{
  int32_t dataSize = 0;

  if ( ( tidlLayer->layerType == TIDL_ConvolutionLayer ) ||
           ( tidlLayer->layerType == TIDL_Deconv2DLayer) ||
           ( tidlLayer->layerType == TIDL_BatchNormLayer) ||
           ( tidlLayer->layerType == TIDL_BatchReshapeLayer) ||
           ( tidlLayer->layerType == TIDL_InnerProductLayer))
  {
      if ( ( tidlLayer->layerType == TIDL_ConvolutionLayer ) ||
           ( tidlLayer->layerType == TIDL_Deconv2DLayer) )
      {
        dataSize = ( tidlLayer->layerParams.convParams.numOutChannels *
                   tidlLayer->layerParams.convParams.numInChannels *
                   tidlLayer->layerParams.convParams.kernelW *
                   tidlLayer->layerParams.convParams.kernelH )/
                   tidlLayer->layerParams.convParams.numGroups;
      }
  }

  return (dataSize * sizeof(float32_tidl) );

}

static void TIDL_printMemorySizeStats(const IALG_MemRec memRec[],
  int32_t numMemRec, int32_t alloc)
{
  int32_t i, totalSize[IALG_EXTERNAL_NON_CACHEABLE + 1] = {0};
  const char *spaceMapping[IALG_EXTERNAL_NON_CACHEABLE+1];
  const char *attrMapping[IALG_WRITEONCE + 1];
  void * ptr;
  spaceMapping[IALG_DARAM0]   = "L1D";
  spaceMapping[IALG_DARAM1]   = "L2";
  spaceMapping[IALG_SARAM0]   = "L3/MSMC";
  spaceMapping[IALG_EXTERNAL] = "DDR Cacheable";
  spaceMapping[IALG_EXTERNAL_NON_CACHEABLE] = "DDR Non-cacheable";

  attrMapping[IALG_SCRATCH] = "Scratch";
  attrMapping[IALG_PERSIST] = "Persistent";

  tidl_printf(1,"\n--------------------------------------------\n");

  tidl_printf(1,"TIDL Memory size requiement (record wise):\n");
  tidl_printf(1, "%-12s, %-20s, %-12s, %-12s, %-12s, %-12s\n","MemRecNum","Space","Attribute","Alignment", "Size(KBytes)", "BasePtr");

  for( i = 0; i < numMemRec; i++){
    ptr = alloc ? NULL : memRec[i].base;
    tidl_printf(1, "%-12d, %-20s, %-12s, %4d, %-8.2f, 0x%08x\n",i,
      spaceMapping[memRec[i].space], attrMapping[memRec[i].attrs], memRec[i].alignment, memRec[i].size/1024.0, ptr);
      totalSize[memRec[i].space] += memRec[i].size ;
  }
  tidl_printf(1,"--------------------------------------------\n");
  tidl_printf(1,"Total memory size requirement (space wise):\n");
  tidl_printf(1,"%-10s, %-10s\n", "Mem Space", "Size(KBytes)");
  for( i = 0; i <= TIDL_DDR_MEMREC_NON_CACHEABLE ; i++){
    if(totalSize[i]){
      tidl_printf(1,"%-10s, %-8.2f\n", spaceMapping[i], totalSize[i]/1024.0);
    }
  }
  tidl_printf(1,"--------------------------------------------\n");
  tidl_printf(1,"NOTE: Memory requirement in host emulation can be different from the same on EVM\n");
  tidl_printf(1,"      To get the actual TIDL memory requirement make sure to run on EVM with \n");
  tidl_printf(1,"      debugTraceLevel = 2\n\n");
  tidl_printf(1,"--------------------------------------------\n");
  return ;
}

int32_t TIDL_layerAlloc(const TIDL_LayerSpecificParams *layerSpecificParams,
                                   const TIDL_NetworkCommonParams *commonParams,
                                   int32_t layerIdx,
                                   int32_t memorySize[TIDL_LAYER_MEMORY_MAX],
                                   IALG_MemRec memRec[])//:TODO: Remove memrec from here eventually
{
  int32_t status = IALG_EOK;

  const TIDL_CreateParams * createParams = commonParams->createParams;
  //:TODO: Below generic flow code should be removed after migrating to workload based flow
  /* Request memory for genericFlowHandle and KernelHandle for supported layers */
  int32_t isNewGenericFlow = TIDL_isNewGenericFlowEnabled(createParams, layerIdx);
  if(((createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0) && isNewGenericFlow)
  {
    uint32_t genricFlowHandleSize = sizeof(TIDL_GenericHandle);
    int32_t numSubHandles = 1U;
    uint32_t kerHandleSize = TIDL_getKernelHandleSize(createParams, layerIdx, numSubHandles);
    int32_t numGenericFlowHandles = TIDL_getNumGenericFlowHandles(&createParams->net->TIDLLayers[layerIdx]);

    memRec[ALG_LAYERS_PARAMS_BUFF_MEMREC].size += (genricFlowHandleSize*numGenericFlowHandles);
    memRec[ALG_LAYERS_PARAMS_BUFF_MEMREC].size += ((kerHandleSize*numGenericFlowHandles));
  }

  if ( (createParams->net->TIDLLayers[layerIdx].layerType == TIDL_DetectionOutputLayer) ||
                        (createParams->net->TIDLLayers[layerIdx].layerType == TIDL_OdPostProcessingLayer))
  {

    status = TIDL_detectOutAlloc(createParams, layerIdx, memRec, commonParams->TIDLLayersBufPtr);
  }
  else if (createParams->net->TIDLLayers[layerIdx].layerType == TIDL_RoiPoolingLayer)
  {
    status = TIDL_roiPoolingAlloc(createParams, layerIdx, memRec, commonParams->TIDLLayersBufPtr);
  }
  else if ((createParams->net->TIDLLayers[layerIdx].layerType == TIDL_OdOutputReformatLayer) ||
           (createParams->net->TIDLLayers[layerIdx].layerType == TIDL_SqueezeLayer) ||
           (createParams->net->TIDLLayers[layerIdx].layerType == TIDL_ReshapeLayer))
  {
    status = TIDL_flattenAlloc(createParams, layerIdx, memRec, commonParams->TIDLLayersBufPtr);
  }
  else
  {
    status = TIDL_E_UNSUPPORTED_LAYER;
  }
  return status;
}

static int32_t TIDL_layerInit(const TIDL_LayerSpecificParams *layerSpecificParams,
                                const TIDL_NetworkCommonParams *commonParams,
                                sTIDL_AlgLayer_t               *algLayer,
                                int32_t layerIdx,
                                uint8_t *memory[TIDL_LAYER_MEMORY_MAX],
                                int32_t memorySize[TIDL_LAYER_MEMORY_MAX],
                                void    **outPtr,
                                const IALG_MemRec memRec[],//:TODO: Should be removed
                                int32_t *paramMemTabOffset,//:TODO: Should be removed
                                int32_t *dataMemTabOffset,//:TODO: Should be removed
                                TIDL_Handle algHandle)//:TODO: Need to see why this was needed
{
  int32_t status = IALG_EOK;

  const TIDL_CreateParams * createParams = commonParams->createParams;

  int32_t targetDevice = createParams->net->deviceName;
  int32_t numSubHandles = 1U;
  TIDL_forceTargetDeviceNonOTF(&targetDevice, createParams->net->TIDLLayers[layerIdx].layerType, -1);

  /* Check if new GenericFlow is enabled for this layer */
  //:TODO: this code will be removed once we completely migrate to workload based mechanism
  int32_t isNewGenericFlow = TIDL_isNewGenericFlowEnabled(createParams, layerIdx);
  if(((createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0) && isNewGenericFlow )
  {
    int32_t numGenericFlowHandles = TIDL_getNumGenericFlowHandles(&createParams->net->TIDLLayers[layerIdx]);

    for (int32_t j = 0; j < numGenericFlowHandles; j++)
    {
      /* Memory for KernelHandle is allocated next to GenericFlow handle so that
          it is easy to copy them to L1D in a single DMA transfer in the Process call */
      algLayer->kernelHandle[0][j] =
        get_uint8_t_pointer((uint8_t*)(void*)memRec[ALG_LAYERS_PARAMS_BUFF_MEMREC].base, *paramMemTabOffset);
      algLayer->kerHandleSize = TIDL_getKernelHandleSize(createParams, layerIdx, numSubHandles);
      *paramMemTabOffset += algLayer->kerHandleSize;
    }
  }
  if(status != IALG_EOK)
  {
    goto Exit;
  }

  if ((createParams->net->TIDLLayers[layerIdx].layerType == TIDL_DetectionOutputLayer) ||
      (createParams->net->TIDLLayers[layerIdx].layerType == TIDL_OdPostProcessingLayer))
  {
      int32_t numODLayer = algHandle->numODLayer;
      sODLayerInfo_t *odLayerInfo = algHandle->odLayerInfo;
      if(TIDL_OBJ_DET_MAX_HEADS == numODLayer)
      {
        status = TIDL_E_COMMON;
        tidl_printf(0,"OD Heads exceeds the max limit of %d\n", TIDL_OBJ_DET_MAX_HEADS);
      }
      else
      {
        odLayerInfo[numODLayer].i_OD = layerIdx ;
        odLayerInfo[numODLayer].algLayer_OD = algLayer;
        status = TIDL_detectOutInit(createParams, layerIdx,
          algLayer,
          paramMemTabOffset, dataMemTabOffset,
          memRec, outPtr,commonParams->TIDLLayersBufPtr);
        numODLayer++;
      }
      algHandle->numODLayer = numODLayer;
  }
  else if (createParams->net->TIDLLayers[layerIdx].layerType == TIDL_RoiPoolingLayer)
  {
    status = TIDL_roiPoolingInit(createParams, layerIdx,
      algLayer,
      paramMemTabOffset, dataMemTabOffset,
      memRec, outPtr, commonParams->TIDLLayersBufPtr);
  }
  else if ((createParams->net->TIDLLayers[layerIdx].layerType == TIDL_OdOutputReformatLayer) ||
          (createParams->net->TIDLLayers[layerIdx].layerType == TIDL_SqueezeLayer) ||
          (createParams->net->TIDLLayers[layerIdx].layerType == TIDL_ReshapeLayer)
          )
  {
    status = TIDL_flattenInit(createParams, layerIdx,
      algLayer,
      paramMemTabOffset, dataMemTabOffset,
      memRec, outPtr, commonParams->TIDLLayersBufPtr);
  }
  else if (createParams->net->TIDLLayers[layerIdx].layerType == TIDL_DataLayer)
  {
    /* Nothing is required to be done for data layers*/
  }
  else
  {
      status = TIDL_E_UNSUPPORTED_LAYER;
  }
  Exit:
    return status;
}


int32_t getLayerExecutionNumber(int32_t dataId, int32_t layerExecutionOrder[], int32_t numLayers)
{
  int32_t execLayerNum = NOT_VALID;

  for(int32_t i=0; i < numLayers; i++)
  {
    if (dataId == layerExecutionOrder[i]){
      execLayerNum = i ;
      break;
    }
  }
  return execLayerNum;
}

/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION
@fn         TIDL_alloc
@brief      Function to supply memory requirement to user
            refer algAlloc function for details in XADIS (ialg.h file)
@param      params       : Input Parameters
@param      parentFxns   : Parent function pointer
@param      memRec       : memory table populated by the Algorithm
@remarks    None
@return     IALG_EOK   - Successful
            IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/

static sTIDL_LayerBuf_t TIDLLayersBuf = {0};
int32_t TIDL_alloc(const IALG_Params *params,
IALG_Fxns **parentFxns,
IALG_MemRec memRec[])
{
  int32_t status = IALG_EOK;
  int32_t l1MemSize, l2MemSize, l3MemSize;
  int32_t i, j;
  const TIDL_CreateParams * createParams =
  (const TIDL_CreateParams*)(const void*)params;
  int32_t coreId = createParams->coreId;
  int32_t relativeCoreId = GET_RELATIVE_COREIDX(coreId, createParams->coreStartIdx);
  sPerfSim_t * perfInfoOut = NULL;
  sGraphCompilerOutArgs_t *gcOutArgs = NULL;
  sGraphCompilerOutArgsCore_t *gcOut = NULL;
  sWorkloadUnit_t         *workloadUnit = NULL;
  sGCHelperHandle           *gcHelperHandle = NULL;
  sGCHelperHandle            gcHelperHandleObj;
  TIDL_LayerSpecificParams layerSpecificParams;
  TIDL_NetworkCommonParams commonParams;
  int32_t memorySize[TIDL_LAYER_MEMORY_MAX]={0};
  sWorkloadSuperGroup_t *wlSuperGrp = NULL;

  status = TIDL_initDebugTraceParams(createParams->traceLogLevel, createParams->traceWriteLevel, createParams->TIDLVprintf, createParams->TIDLWriteBinToFile,
                                      createParams->TIDLReadBinFromFile, createParams->traceBaseName, createParams->tracePtr);
  tidl_printf(2,"TIDL_initDebugTraceParams Done \n");

  if (status != IALG_EOK)
  {
    status = TIDL_E_INVALID_DEBUG_TRACE_PRM;
  }

  if(status == IALG_EOK)
  {
    if (createParams->net->netVersion != TIDL_NET_VERSION)
    {
      status = TIDL_E_INVALID_NET_VERSION;
    }
  }

  if((status == IALG_EOK) && ((createParams->flowCtrl & TIDL_FLOW_CTRL_REF_STAT) == 0))
  {
    if (createParams->net->isQuantStatsAvailable == 0)
    {
      status = TIDL_E_QUANT_STATS_NOT_AVAILABALE;
    }
  }

  if(status == IALG_EOK)
  {
    if (createParams->isInbufsPaded == 0)
    {
      tidl_printf(1,"Padding of input buffer in Library is not supported now\n");
      status = IALG_EFAIL;
    }

  }
  if((createParams->flowCtrl & TIDL_FLOW_CTRL_REF_COMP) == TIDL_FLOW_CTRL_REF_COMP)
  {
    tidl_printf(0,"WARNING : TIDL_FLOW_CTRL_REF_COMP Support is deprecated in Inference Software now \n");
  }

  if (createParams->net->dataFlowInfo != 0)
  {
    perfInfoOut = (sPerfSim_t *)get_int8_t_pointer((int8_t *)(createParams->net), createParams->net->dataFlowInfo);
  }

  TIDL_play(createParams->net, 1);

  if ( createParams->net->graphCompilerInfoOffset != 0 )
  {
    gcOutArgs  = (sGraphCompilerOutArgs_t *)get_int8_t_pointer((int8_t *)(createParams->net), createParams->net->graphCompilerInfoOffset);
    gcOut = (sGraphCompilerOutArgsCore_t *)getGCDataBase(gcOutArgs , DATABASE_GCOUT_1+relativeCoreId);
    sGCDataBase_t *gcDatbase = &gcOut->dataBase;
    sGCCommonDataBase_t *gcCommonDataBase = (sGCCommonDataBase_t *)getGCDataBase(gcOutArgs,DATABASE_COMMON);

    sBufParams_t* bufNodesDB = (sBufParams_t*) &gcCommonDataBase->dbPayLoad[gcCommonDataBase->dbParams[DB_COMMON_BUF].startOffset];
    sWorkloadUnit_t* workLoadUnitDB = (sWorkloadUnit_t*) &gcDatbase->dbPayLoad[gcDatbase->dbParams[DB_WL].startOffset];
    // sWorkloadUnitAuxilary_t* auxilaryWorkloadDB = (sWorkloadUnitAuxilary_t*) gcDatbase->dbPayLoad[gcDatbase->dbParams[DB_BUF].size +
    //                                                                       gcDatbase->dbParams[DB_WL].size];
    sWorkloadUnitAuxilary_t* auxilaryWorkloadDB = (sWorkloadUnitAuxilary_t*) &gcDatbase->dbPayLoad[gcDatbase->dbParams[DB_AUXWL].startOffset];

    gcHelperHandle = &gcHelperHandleObj;
    gcOutArgs->commonDatabaseInfo.bufDB.pBufList = bufNodesDB;
    gcOut->dataBaseInfo.wlDB.pWLList = workLoadUnitDB;
    gcOut->dataBaseInfo.wlAuxDB.pWLAuxList = auxilaryWorkloadDB;
    initGCHelperHandle(gcHelperHandle, createParams->net, &gcOut->dataBaseInfo, &gcOutArgs->commonDatabaseInfo);

    wlSuperGrp = &gcOut->superWorkload;

    status = TIDL_applyConstraintsOnGC(createParams, &gcOut->superWorkload, gcHelperHandle);
    if(status != IALG_EOK)
    {
      tidl_printf(0,"TIDL_applyConstaintsonGC failed!\n");
      status = IALG_EFAIL;
    }
  }

  if((createParams->flowCtrl & TIDL_FLOW_CTRL_REF_STAT) == TIDL_FLOW_CTRL_REF_STAT)
  {
    perfInfoOut  = NULL;
    gcHelperHandle = NULL;
  }

  if(NULL == perfInfoOut)
  {
    l1MemSize = TIDL_DEFAULT_L1_MEM_SIZE;
    l2MemSize = TIDL_DEFAULT_L2_MEM_SIZE;
    l3MemSize = TIDL_DEFAULT_L3_MEM_SIZE;
  }
  else
  {
    l1MemSize = TIDL_DEFAULT_L1_MEM_SIZE;
    l2MemSize = (perfInfoOut->simConfig.sizeL2MemKB * 1024);
    l3MemSize = perfInfoOut->simConfig.sizeL3MemKB * 1024;
  }

  if(status == IALG_EOK)
  {
    memRec[ALG_HANDLE_MEMREC].size          = (uint32_t)sizeof(TIDL_Obj);
    memRec[ALG_HANDLE_MEMREC].space         = TIDL_DDR_MEMREC_NON_CACHEABLE;
    memRec[ALG_HANDLE_MEMREC].attrs         = IALG_PERSIST;
    memRec[ALG_HANDLE_MEMREC].alignment     = 128;

    memRec[ALG_SCRATCH_L1_MEM_MEMREC].size       = (uint32_t)l1MemSize;
    memRec[ALG_SCRATCH_L1_MEM_MEMREC].attrs      = IALG_SCRATCH;
    memRec[ALG_SCRATCH_L1_MEM_MEMREC].alignment  = 128;
    memRec[ALG_SCRATCH_L1_MEM_MEMREC].space      = (IALG_MemSpace)IALG_DARAM0;

    memRec[ALG_SCRATCH_L2_MEM_MEMREC].size       = (uint32_t)l2MemSize;
    memRec[ALG_SCRATCH_L2_MEM_MEMREC].attrs      = IALG_SCRATCH;
    memRec[ALG_SCRATCH_L2_MEM_MEMREC].alignment  = 128;
    memRec[ALG_SCRATCH_L2_MEM_MEMREC].space      = IALG_DARAM1;

    memRec[ALG_SCRATCH_L3_MEM_MEMREC].size       = (uint32_t)l3MemSize;
    memRec[ALG_SCRATCH_L3_MEM_MEMREC].space      = (IALG_MemSpace)IALG_SARAM0;
    memRec[ALG_SCRATCH_L3_MEM_MEMREC].attrs      = IALG_SCRATCH;
    memRec[ALG_SCRATCH_L3_MEM_MEMREC].alignment  = 128;

    memRec[ALG_CREATE_PARAM_MEMREC].size        = (uint32_t)sizeof(TIDL_CreateParams);
    memRec[ALG_CREATE_PARAM_MEMREC].space       = TIDL_DDR_MEMREC_CACHEABLE;
    memRec[ALG_CREATE_PARAM_MEMREC].attrs       = IALG_PERSIST;
    memRec[ALG_CREATE_PARAM_MEMREC].alignment   = 128;

    memRec[ALG_LAYERS_PARAMS_BUFF_MEMREC].size     = 256 *1024; // Minimum 256 KB to allow some last unaligned transfers
    memRec[ALG_LAYERS_PARAMS_BUFF_MEMREC].space    = TIDL_DDR_MEMREC_CACHEABLE; /* Supposed to be read only during process call, so mark cacheable */
    memRec[ALG_LAYERS_PARAMS_BUFF_MEMREC].attrs    = IALG_PERSIST;
    memRec[ALG_LAYERS_PARAMS_BUFF_MEMREC].alignment = 128;

    memRec[ALG_LAYERS_MEMREC].size                 = 0;
    memRec[ALG_LAYERS_MEMREC].space                = TIDL_DDR_MEMREC_CACHEABLE;
    memRec[ALG_LAYERS_MEMREC].attrs                = IALG_PERSIST;
    memRec[ALG_LAYERS_MEMREC].alignment            = 128;

    memRec[ALG_REF_SCRATCH_BUFF_MEMREC].size      = 256;
    memRec[ALG_REF_SCRATCH_BUFF_MEMREC].space     = TIDL_DDR_MEMREC_NON_CACHEABLE;
    memRec[ALG_REF_SCRATCH_BUFF_MEMREC].attrs     = IALG_SCRATCH;
    memRec[ALG_REF_SCRATCH_BUFF_MEMREC].alignment = 128;

    memRec[ALG_SCRATCH_DATA_BUFF_MEMREC].size      = 256;
    memRec[ALG_SCRATCH_DATA_BUFF_MEMREC].space     = TIDL_DDR_MEMREC_NON_CACHEABLE;
    memRec[ALG_SCRATCH_DATA_BUFF_MEMREC].attrs     = IALG_SCRATCH;
    memRec[ALG_SCRATCH_DATA_BUFF_MEMREC].alignment = 128;

    memRec[ALG_SCRATCH_DDR_MEM_MEMREC].size        = 256 + TIDL_DDR_EXCESS_SCRATCH; // Alloc at allocResize
    memRec[ALG_SCRATCH_DDR_MEM_MEMREC].space       = TIDL_DDR_MEMREC_CACHEABLE;
    memRec[ALG_SCRATCH_DDR_MEM_MEMREC].attrs       = IALG_SCRATCH;
    memRec[ALG_SCRATCH_DDR_MEM_MEMREC].alignment   = 128;

    /* each Workload maintains a state value - Core 0 has max. number of workloads, allocate same memory for all cores */
    if(createParams->net->inferenceMode == TIDL_inferenceModeLowLatency)
    {
      sGraphCompilerOutArgsCore_t * gcOutCore = NULL;
      gcOutCore = (sGraphCompilerOutArgsCore_t *)getGCDataBase(gcOutArgs , DATABASE_GCOUT_1);
      
      memRec[ALG_PERSIST_SYNC_MEMREC].size        = gcOutCore->dataBaseInfo.wlDB.count * sizeof(int8_t) + (int8_t)TIDL_GLOBAL_SYNC_OFFSET;
      memRec[ALG_PERSIST_SYNC_MEMREC].space       = TIDL_DDR_MEMREC_NON_CACHEABLE;
      memRec[ALG_PERSIST_SYNC_MEMREC].attrs       = IALG_PERSIST;
      memRec[ALG_PERSIST_SYNC_MEMREC].alignment   = 128;
    }
    else
    {
      /* Synchronization memory is required only for low latency mode. size = 0 cannot be requested, so setting size = 1*/
      memRec[ALG_PERSIST_SYNC_MEMREC].size        = 1;
      memRec[ALG_PERSIST_SYNC_MEMREC].space       = TIDL_DDR_MEMREC_NON_CACHEABLE;
      memRec[ALG_PERSIST_SYNC_MEMREC].attrs       = IALG_PERSIST;
      memRec[ALG_PERSIST_SYNC_MEMREC].alignment   = 128;
    }

    /* ALG_SCRATCH_DATA_BUFF_MEMREC Will be filled
    as max of all the temp buffer requirement by all the layers */

    /* Allocate Memory to store newdataID's in TIDL_Handle to use in Process call */
    memRec[ALG_LAYERS_MEMREC].size +=
      (uint32_t)(sizeof(sTIDL_LayerBuf_t) +  128U);

    /* Initialize newDataId of inData with dataId, so that this can be used later
        to find the free output buffer for Re-using
    */
    if(createParams->optimiseExtMem != TIDL_OptimiseExtMemL0)
    {
      for(i = 0; i <  createParams->net->numLayers; i++)
      {
        const sTIDL_Layer_t *TIDLLayerCur = &createParams->net->TIDLLayers[i];
        for(j = 0; j < TIDLLayerCur->numInBufs; j++)
        {
          TIDLLayersBuf.newInDataId[i][j] = TIDLLayerCur->inData[j];
        }
      }
    }

    commonParams.createParams     = createParams;
    commonParams.gcHelperHandle       = gcHelperHandle;
    commonParams.net              = createParams->net;
    commonParams.TIDLLayersBufPtr = &TIDLLayersBuf;
    commonParams.tidlCommonParams = NULL;/* Not required during alloc */

    sGetLayerIdContext_t getLayerIdContext;
    int32_t layerId;

    getLayerIdInit(&getLayerIdContext,
                        relativeCoreId,
                        createParams->currLayersGroupId,
                        createParams->net,
                        gcHelperHandle,
                        wlSuperGrp);

    while(1)
    {
      layerId = getLayerIdToExecute(&getLayerIdContext,
                                   1,
                                   NULL,
                                   NULL,
                                   &workloadUnit);
      if ( layerId == NOT_VALID)
      {
        break;
      }

      //:TODO: Eventually this should be removed once process also  uses the same loop structure
      if ( createParams->net->TIDLLayers[layerId].layerType == TIDL_DataLayer )
      {
        continue;
      }

      if ( gcOutArgs != NULL )
      {
        int32_t isWorkLoadSupported;
        isWorkLoadSupported = TIDL_isWorkLoadUnitFlowSupported(layerId,
                                                               createParams->net,
                                                               createParams->net->deviceName,
                                                               workloadUnit);
        /* Force workloadUnit to be NULL to going to old flow */
        if (isWorkLoadSupported == 0 )
        {
          workloadUnit = NULL;
        }
      }
      layerSpecificParams.workloadUnit = workloadUnit;
      layerSpecificParams.isMixedPrecEnabled = 1U;

      memset(memorySize, 0, sizeof(memorySize));

      status = TIDL_layerAllocNew(&layerSpecificParams,
                               &commonParams,
                               layerId,
                               memorySize);

#if ENABLE_OLD_FLOW
      /* Go to old flow if new mapping is not available */
      if ( status == NOT_VALID )
      {
#if ENABLE_BACKWARDS_COMPATIBILITY
        TIDL_CreateParams * createParamsCopy = (TIDL_CreateParams *)createParams;
        int32_t flowCtrlOrig = createParams->flowCtrl;
        if (gcOutArgs != NULL) createParamsCopy->flowCtrl = TIDL_FLOW_CTRL_REF_ONLY;
#endif
        status = TIDL_layerAlloc(&layerSpecificParams,
                                 &commonParams,
                                 layerId,
                                 memorySize,
                                 memRec);//:TODO: memRec shouldnt be used eventually
#if ENABLE_BACKWARDS_COMPATIBILITY
        createParamsCopy->flowCtrl = flowCtrlOrig;
#endif
      }
#endif

      if ( status != IALG_EOK)
      {
        break;
      }

      memRec[ALG_LAYERS_MEMREC].size +=   (uint32_t)(sizeof(sTIDL_AlgLayer_t) +  128U);

      /* Memory used for output buffers, needed only for reference flow without NC */
      memRec[ALG_SCRATCH_DATA_BUFF_MEMREC].size  += (memorySize[TIDL_LAYER_MEMORY_OUTPUT] + 128U);
      /* Scratch memory used by layer, needed only for reference flow without NC */
      memRec[ALG_LAYERS_PARAMS_BUFF_MEMREC].size += (memorySize[TIDL_LAYER_MEMORY_PERSISTENT] + 128U);

      if ( TIDL_ALIGN_CEIL(memorySize[TIDL_LAYER_MEMORY_SCRATCH], 128U) > memRec[ALG_REF_SCRATCH_BUFF_MEMREC].size )
      {
        /* Scratch buffer is common across all the layers hence only request the maximum scratch size */
        memRec[ALG_REF_SCRATCH_BUFF_MEMREC].size   = TIDL_ALIGN_CEIL(memorySize[TIDL_LAYER_MEMORY_SCRATCH], 128U);
      }
    }
  }
  if(status == IALG_EOK)
  {
    memRec[ALG_REF_OUTPUT_BUFF_MEMREC].size          = 128;
    memRec[ALG_REF_OUTPUT_BUFF_MEMREC].space       = TIDL_DDR_MEMREC_NON_CACHEABLE;
    memRec[ALG_REF_OUTPUT_BUFF_MEMREC].attrs         = IALG_SCRATCH;
    memRec[ALG_REF_OUTPUT_BUFF_MEMREC].alignment  = 128;

    int32_t maxOutFeatMapSize = 0;
    int32_t outFeatMapSize;
    int32_t outFeatMapSize1;
    int32_t maxParamSize = 0;

    for (i = 0; i < createParams->net->numLayers; i++)
    {
      int32_t paramSize;
      sTIDL_DataParams_t * dataParams = TIDL_getDataParams(createParams->net,
      createParams->net->TIDLLayers[i].outData.dataId);
      outFeatMapSize = dataParams->dimValues[TIDL_DIM_BATCH] *
                              dataParams->dimValues[TIDL_DIM_DIM1] *
                              dataParams->dimValues[TIDL_DIM_DIM2] *
                              dataParams->dimValues[TIDL_DIM_NUMCH] *
                              dataParams->pitch[TIDL_CHANNEL_PITCH] *
                              (TIDL_getDatElementSize(dataParams->elementType));

      outFeatMapSize1 = dataParams->dimValues[TIDL_DIM_BATCH] *
                              dataParams->dimValues[TIDL_DIM_DIM1] *
                              dataParams->dimValues[TIDL_DIM_DIM2] *
                              dataParams->dimValues[TIDL_DIM_NUMCH] *
                              dataParams->dimValues[TIDL_DIM_HEIGHT] *
                              dataParams->dimValues[TIDL_DIM_WIDTH] *
                              (TIDL_getDatElementSize(dataParams->elementType));

      outFeatMapSize = (outFeatMapSize > outFeatMapSize1) ? outFeatMapSize : outFeatMapSize1;

      if ( outFeatMapSize > maxOutFeatMapSize)
      {
        maxOutFeatMapSize = outFeatMapSize;
      }

      paramSize = TIDL_getMaxParamSizeInBytes(&createParams->net->TIDLLayers[i]);

      maxParamSize = (paramSize > maxParamSize) ? paramSize : maxParamSize;

    }
    if((createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == TIDL_FLOW_CTRL_REF_ONLY )
    {
      /*maxOutFeatMapSize is in bytes*/
      memRec[ALG_REF_OUTPUT_BUFF_MEMREC].size          = (2U * maxOutFeatMapSize) + TRACE_STRINGS_MEM_SIZE;/* Twice for input and output */
      /*Allocating extra space for traces, ideally there should not be a reference/target specific condition for this:*/
      if(createParams->traceWriteLevel == 3)
      {
        memRec[ALG_REF_OUTPUT_BUFF_MEMREC].size          = (uint32_t)((sizeof(float32_tidl) + 1U)* (uint32_t)maxOutFeatMapSize) + TRACE_STRINGS_MEM_SIZE;
      }
      else if(createParams->traceWriteLevel == 2)
      {
        memRec[ALG_REF_OUTPUT_BUFF_MEMREC].size          = (2U*(uint32_t)maxOutFeatMapSize) + TRACE_STRINGS_MEM_SIZE;
      }
      else if(createParams->traceWriteLevel == 1)
      {
        memRec[ALG_REF_OUTPUT_BUFF_MEMREC].size          = (uint32_t)maxOutFeatMapSize + TRACE_STRINGS_MEM_SIZE;
      }
    }
    else
    {

#if !ENABLE_BACKWARDS_COMPATIBILITY
      memRec[ALG_REF_SCRATCH_BUFF_MEMREC].size         = 128U;
#else
      memRec[ALG_REF_OUTPUT_BUFF_MEMREC].size          = (2U * sizeof(float32_tidl) * maxOutFeatMapSize) + TRACE_STRINGS_MEM_SIZE;/* Twice for input and output */
#endif
      if(createParams->traceWriteLevel == 3)
      {
        memRec[ALG_REF_OUTPUT_BUFF_MEMREC].size          = (uint32_t)((sizeof(float32_tidl) + 1U)* (uint32_t)maxOutFeatMapSize) + TRACE_STRINGS_MEM_SIZE;
      }
      else if(createParams->traceWriteLevel == 2)
      {
        memRec[ALG_REF_OUTPUT_BUFF_MEMREC].size          = (2U*(uint32_t)maxOutFeatMapSize) + TRACE_STRINGS_MEM_SIZE;
      }
      else if(createParams->traceWriteLevel == 1)
      {
        memRec[ALG_REF_OUTPUT_BUFF_MEMREC].size          = (uint32_t)maxOutFeatMapSize + TRACE_STRINGS_MEM_SIZE;
      }
      else
      {
        memRec[ALG_REF_OUTPUT_BUFF_MEMREC].size          = 128U + TRACE_STRINGS_MEM_SIZE;
      }

    }

    if((createParams->flowCtrl & TIDL_FLOW_CTRL_REF_STAT) == TIDL_FLOW_CTRL_REF_STAT)
    {
      /*Disabling histogram based clipping for Asymmetric, per-channel mode*/
      if ( ( createParams->net->calibrationOption & TIDL_CalibOptionActivationRange) ==
        TIDL_CalibOptionActivationRange && (createParams->net->quantizationStyle != TIDL_QuantStyleAsymNP2))
      {
        memRec[ALG_REF_OUTPUT_BUFF_MEMREC].size += 128;
        memRec[ALG_REF_OUTPUT_BUFF_MEMREC].size += (createParams->net->numLayers *
                                                   TIDL_NUM_ACTIVATION_HISTOGRAM_BINS *
                                                   sizeof(int32_t));
        /* To store the min and max value of each layer */
        memRec[ALG_REF_OUTPUT_BUFF_MEMREC].size += 128;
        memRec[ALG_REF_OUTPUT_BUFF_MEMREC].size += (createParams->net->numLayers *
                                                   2 * sizeof(float32_tidl));
      }
    }

    if((createParams->flowCtrl & TIDL_FLOW_CTRL_REF_STAT) == TIDL_FLOW_CTRL_REF_STAT)
    {
      if ( ( createParams->net->calibrationOption & TIDL_CalibOptionActivationRange) ==
        TIDL_CalibOptionActivationRange && (createParams->net->quantizationStyle != TIDL_QuantStyleAsymNP2))
      {
        memRec[ALG_REF_OUTPUT_BUFF_MEMREC].size += 128;
        memRec[ALG_REF_OUTPUT_BUFF_MEMREC].size += (createParams->net->numLayers *
                                                   TIDL_NUM_ACTIVATION_HISTOGRAM_BINS *
                                                   sizeof(int32_t));
        /* To store the min and max value of each layer */
        memRec[ALG_REF_OUTPUT_BUFF_MEMREC].size += 128;
        memRec[ALG_REF_OUTPUT_BUFF_MEMREC].size += (createParams->net->numLayers *
                                                   2 * sizeof(float32_tidl));
      }
    }

    if(createParams->traceWriteLevel == 4)
    {
      maxParamSize = maxParamSize + TRACE_STRINGS_MEM_SIZE;
      memRec[ALG_REF_OUTPUT_BUFF_MEMREC].size = (maxParamSize >
                                                memRec[ALG_REF_OUTPUT_BUFF_MEMREC].size) ?
                                                maxParamSize : memRec[ALG_REF_OUTPUT_BUFF_MEMREC].size;
    }

    memRec[ALG_SCRATCH_DATA_BUFF_EXT_MEMREC].space        = TIDL_DDR_MEMREC_NON_CACHEABLE;
    memRec[ALG_SCRATCH_DATA_BUFF_EXT_MEMREC].attrs        = IALG_SCRATCH;
    memRec[ALG_SCRATCH_DATA_BUFF_EXT_MEMREC].alignment    = 128;
    memRec[ALG_SCRATCH_DATA_BUFF_EXT_MEMREC].size         = 128;

    /* memTab to allocate memory to backup context memory for pre-emption */
    if (TIDL_checkIfPreEmptionEnabled(createParams))
    {
      /* Making this memTab as PERSIST to avoid context memory DDR to DDR copy */
      memRec[ALG_SCRATCH_DATA_BUFF_EXT_MEMREC].attrs         = IALG_PERSIST;
      memRec[ALG_CONTEXT_MEM_MEMREC].size =
        TIDL_getContextMemSize(createParams->targetPriority, perfInfoOut);
    }
    else{
      memRec[ALG_CONTEXT_MEM_MEMREC].size = TIDL_PREEMPT_CONTEXT_MEMSIZE_DUMMY;
    }
    memRec[ALG_CONTEXT_MEM_MEMREC].space       = TIDL_DDR_MEMREC_NON_CACHEABLE;
    memRec[ALG_CONTEXT_MEM_MEMREC].attrs       = IALG_PERSIST;
    memRec[ALG_CONTEXT_MEM_MEMREC].alignment   = 128;

    if (perfInfoOut !=  NULL)
    {
      memRec[ALG_LAYERS_PARAMS_BUFF_MEMREC].size += (gcOut->memoryRequirementPerCore[DDR_PERSIST] + 128);
      memRec[ALG_SCRATCH_DATA_BUFF_EXT_MEMREC].size = (gcOut->memoryRequirementPerCore[DDR] + 128);
    }

    /* memTab to allocate memory for net structure along with compiled information*/
    memRec[ALG_PERSIST_DDR_NET_MEMREC].size        = TIDL_getSizeNet(createParams)  + TIDL_ALIGNMENT_SIZE ;
    memRec[ALG_PERSIST_DDR_NET_MEMREC].space       = TIDL_DDR_MEMREC_CACHEABLE;
    memRec[ALG_PERSIST_DDR_NET_MEMREC].attrs       = IALG_PERSIST;
    memRec[ALG_PERSIST_DDR_NET_MEMREC].alignment   = 128;

    if((createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == TIDL_FLOW_CTRL_REF_ONLY)
    {
      for( i = 0; i < MAX_NUM_MEMRECS; i++)
      {
        memRec[i].space = TIDL_DDR_MEMREC_CACHEABLE;
      }
    }
  }

  if ( tidl_getTraceLogLevel() >= 2 )
  {
    TIDL_printMemorySizeStats(memRec, MAX_NUM_MEMRECS, 1);
  }

  TIDL_play(createParams->net, 0);

  return (status);
}
/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION
@fn         TIDL_init
@brief      Function to accept memory pointers from user and initialize algo
            Refer algInit function for details in XADIS (ialg.h file)
@param      handle   : Algorithm Instance handle
@param      memRec   : memory table populated by the Algorithm
@param      parent   : Parent function pointer
@param      params   : Input Parameters
@remarks    None
@return     IALG_EOK   - Successful
            IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/
int32_t TIDL_init(IALG_Handle handle,
const IALG_MemRec memRec[],
IALG_Handle parent,
const IALG_Params * params)
{
  int32_t status = IALG_EOK;

  TIDL_Handle algHandle    = (TIDL_Handle)handle;
  algHandle->numMemRecs    = (uint32_t)MAX_NUM_MEMRECS;
  const TIDL_CreateParams * createParams;
  int32_t coreId, relativeCoreId;

  #ifdef DEBUG_PREEMPTION_HOST_EMULATION
  gHandleForPremptTest = handle;
  #endif

  if((memRec != NULL)) /* TIDL init call from ivision API */
  {
    tidl_printf(1, "TIDL init call from ivision API \n");
    if ( tidl_getTraceLogLevel() >= 2 )
    {
      TIDL_printMemorySizeStats(memRec, MAX_NUM_MEMRECS, 0);
    }
    createParams = (const TIDL_CreateParams*)(const void*)params;

    (void)memcpy(algHandle->memRec,memRec, sizeof(IALG_MemRec)*algHandle->numMemRecs);
    algHandle->ivision = &TIDL_VISION_FXNS;
    algHandle->createParams  = (TIDL_CreateParams *)createParams;

    if(((createParams->net->inferenceMode == TIDL_inferenceModeLowLatency)) && ((createParams->flowCtrl & TIDL_FLOW_CTRL_REF_STAT) != TIDL_FLOW_CTRL_REF_STAT))
    {
      /* TODO_NG : Remove TIDL_inferenceModeHighThroughput after low latency mode is stable*/
      tidl_printf(1, "Returning from TIDL ivision API call \n");
      /* In case of low latency mode, control API is called from nested graph which does the actual init of pointers
         Current init is only used to copy over memRecs and ivision function ptrs */
      /* Import tool still used the TIDL test executable and not TIDL-RT executable for init flow for import tool is same as other inference modes */
      return status;
    }
  }
  else /* TIDL control call for init */
  {
    tidl_printf(1, "TIDL control init call \n");
    memRec = (IALG_MemRec *)(algHandle->memRec);
    createParams = (TIDL_CreateParams *)algHandle->createParams;
  }

  TIDL_play(createParams->net, 1);

  coreId = createParams->coreId;
  relativeCoreId = GET_RELATIVE_COREIDX(coreId, createParams->coreStartIdx);

  int32_t currAlgLayer = 0;
  int32_t paramMemTabOffset = 0;
  int32_t dataMemTabOffset = 0;
  sPerfSim_t * perfInfoOut;

  int32_t i,j;
  int32_t intAlghandleMemSize;
  int32_t dmaContextSize;
  sGraphCompilerOutArgs_t *gcOutArgs = NULL;
  sGraphCompilerOutArgsCore_t *gcOut = NULL;
  sWorkloadUnit_t         *workloadUnit = NULL;
  sGCHelperHandle         *gcHelperHandle = NULL;
  int32_t msmcSramSize;
  int32_t l2SramSize;

  algHandle->perfSimOutput = NULL ;
  algHandle->isPreEmptionEnable = 0;
  TIDL_LayerSpecificParams layerSpecificParams;
  TIDL_NetworkCommonParams commonParams;
  WorkloadUnitExec_CommonParams tidlCommonParams;

  uint8_t *memory[TIDL_LAYER_MEMORY_MAX] = {0};
  int32_t memorySize[TIDL_LAYER_MEMORY_MAX]={0};
  int32_t inHeightOrig[TIDL_NUM_IN_BUFS] = {0};
  int32_t inChPitchOrig[TIDL_NUM_IN_BUFS] = {0};
  int32_t inRoiPitchOrig[TIDL_NUM_IN_BUFS] = {0};
  int32_t outHeightOrig[TIDL_NUM_OUT_BUFS] = {0};
  int32_t outChPitchOrig[TIDL_NUM_OUT_BUFS] = {0};
  int32_t outRoiPitchOrig[TIDL_NUM_OUT_BUFS] = {0};
  sWorkloadSuperGroup_t *wlSuperGrp = NULL;

  status = TIDL_initDebugTraceParams(createParams->traceLogLevel, createParams->traceWriteLevel, createParams->TIDLVprintf, createParams->TIDLWriteBinToFile,
                                      createParams->TIDLReadBinFromFile, createParams->traceBaseName, createParams->tracePtr);
  if (status != IALG_EOK)
  {
    status = TIDL_E_INVALID_DEBUG_TRACE_PRM;
  }

  if (status == IALG_EOK)
  {
    algHandle->TIDLLayersBuf =
    (sTIDL_LayerBuf_t *) (void *)memRec[ALG_LAYERS_MEMREC].base;

    algHandle->alglayerParams =
    (sTIDL_AlgLayer_t *) (void *)(get_int8_t_pointer((int8_t*)(void*)memRec[ALG_LAYERS_MEMREC].base,
                  ALIGN_SIZE((int32_t)sizeof(sTIDL_LayerBuf_t), 128)));

    algHandle->udmaDrvObj = createParams->udmaDrvObj;

    if((createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0)
    {
      algHandle->createParams->quantRangeUpdateFactor = 0.0f;
      if ( algHandle->udmaDrvObj == NULL )
      {
        status = IALG_EFAIL;
      }
    }

  }

  if (status == IALG_EOK)
  {
    for(i = 0; i <  TIDL_MAX_DATA_BUFS; i++)
    {
      algHandle->dataBuf[i] =  NULL;
    }
  }

  if (status == IALG_EOK)
  {
    /* analyze the network */
    perfInfoOut  = NULL;
    msmcSramSize = 128;
    l2SramSize = 128;
    algHandle->gcHelperHandle = NULL;
    algHandle->wlSuperGrp = NULL;
    algHandle->groupId    = 0;

    if(algHandle->createParams->net->graphCompilerInfoOffset != 0)
    {

      sGCCommonDataBase_t     *gcCommonDataBase = NULL;
      sGCDataBase_t *gcDatabase;
      perfInfoOut =  (sPerfSim_t *) TIDL_getNetObjBufPtr( algHandle->createParams->net, memRec, TIDL_NET_COMPILER_BUF);
      msmcSramSize = (perfInfoOut->simConfig.sizeL3MemKB - L3MEM_SCRATCH_BUF_SIZEKB) * 1024;
      l2SramSize   = (perfInfoOut->simConfig.sizeL2MemKB - L2MEM_SCRATCH_BUF_SIZEKB)* 1024;
      //TODO: [8.5 rebase]. Make this similar to perfsimInfo
	    gcOutArgs    = (sGraphCompilerOutArgs_t *) TIDL_getNetObjBufPtr( algHandle->createParams->net, memRec, TIDL_NET_GC_BUF);
      gcOut = (sGraphCompilerOutArgsCore_t *)getGCDataBase(gcOutArgs , DATABASE_GCOUT_1+relativeCoreId);
      gcDatabase   = &gcOut->dataBase;
      gcHelperHandle = &algHandle->gcHelperHandleObj;
      gcCommonDataBase = (sGCCommonDataBase_t *)getGCDataBase(gcOutArgs,DATABASE_COMMON);
      wlSuperGrp  = &gcOut->superWorkload;
      algHandle->gcHelperHandle = gcHelperHandle;
      algHandle->wlSuperGrp = wlSuperGrp;
      sBufParams_t* bufNodesDB = (sBufParams_t*) &gcCommonDataBase->dbPayLoad[gcCommonDataBase->dbParams[DB_COMMON_BUF].startOffset];
      sWorkloadUnit_t* workLoadUnitDB = (sWorkloadUnit_t*) &gcDatabase->dbPayLoad[gcDatabase->dbParams[DB_WL].startOffset];
      // sWorkloadUnitAuxilary_t* auxilaryWorkloadDB = (sWorkloadUnitAuxilary_t*) gcDatabase->dbPayLoad[gcDatabase->dbParams[DB_BUF].size +
      //                                                                     gcDatabase->dbParams[DB_WL].size];
      sWorkloadUnitAuxilary_t* auxilaryWorkloadDB = (sWorkloadUnitAuxilary_t*) &gcDatabase->dbPayLoad[gcDatabase->dbParams[DB_AUXWL].startOffset];
      gcOutArgs->commonDatabaseInfo.bufDB.pBufList = bufNodesDB;
      gcOut->dataBaseInfo.wlDB.pWLList = workLoadUnitDB;
      gcOut->dataBaseInfo.wlAuxDB.pWLAuxList = auxilaryWorkloadDB;

      initGCHelperHandle(gcHelperHandle, createParams->net, &gcOut->dataBaseInfo, &gcOutArgs->commonDatabaseInfo);
    }

    if((createParams->flowCtrl & TIDL_FLOW_CTRL_REF_STAT) == TIDL_FLOW_CTRL_REF_STAT)
    {
      perfInfoOut  = NULL;
    }

    algHandle->perfSimOutput = perfInfoOut;
  }


  if(status == IALG_EOK)
  {
    status = TIDL_AsymQuantizationCheck(algHandle);
  }

  if(status == IALG_EOK)
  {
    status = TIDL_OTFPadErrorCheck(algHandle);
  }

  /* Initialize newDataId of inData with dataId, so that this can be used later
      to find the free output buffer for Re-using
  */
  if (status == IALG_EOK)
  {
    if(createParams->optimiseExtMem != TIDL_OptimiseExtMemL0)
    {
      for(i = 0; i <  createParams->net->numLayers; i++)
      {
        const sTIDL_Layer_t *TIDLLayerCur = &createParams->net->TIDLLayers[i];
        for(j = 0; j < TIDLLayerCur->numInBufs; j++)
        {
          TIDLLayersBuf.newInDataId[i][j] = TIDLLayerCur->inData[j];
        }
      }
    }

    if((createParams->flowCtrl & TIDL_FLOW_CTRL_REF_STAT) == TIDL_FLOW_CTRL_REF_STAT)
    {
      algHandle->createParams->net->dataFlowInfo = 0;
    }
    if (((createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0) && (algHandle->createParams->net->dataFlowInfo == 0))
    {
      status = TIDL_E_DATAFLOW_INFO_NULL;
    }
  }



  if(status == IALG_EOK)
  {
    /* Setup System memories */
    algHandle->sysMems[TIDL_SYSMEM_L1_SCRATCH].base =
        (void *)  memRec[ALG_SCRATCH_L1_MEM_MEMREC].base;
    algHandle->sysMems[TIDL_SYSMEM_L1_SCRATCH].size =
        (int32_t ) memRec[ALG_SCRATCH_L1_MEM_MEMREC].size;

    algHandle->sysMems[TIDL_SYSMEM_L2_SCRATCH].base =
      (void *)  memRec[ALG_SCRATCH_L2_MEM_MEMREC].base;
    algHandle->sysMems[TIDL_SYSMEM_L2_SCRATCH].size =
        (int32_t )memRec[ALG_SCRATCH_L2_MEM_MEMREC].size;

    algHandle->sysMems[TIDL_SYSMEM_L3_SCRATCH].base =
        (void *)  memRec[ALG_SCRATCH_L3_MEM_MEMREC].base;
    algHandle->sysMems[TIDL_SYSMEM_L3_SCRATCH].size =
        (int32_t ) memRec[ALG_SCRATCH_L3_MEM_MEMREC].size;

    algHandle->sysMems[TIDL_SYSMEM_DDR_SCRATCH].base =
        (void *)  memRec[ALG_SCRATCH_DDR_MEM_MEMREC].base;
    algHandle->sysMems[TIDL_SYSMEM_DDR_SCRATCH].size =
        (int32_t)  memRec[ALG_SCRATCH_DDR_MEM_MEMREC].size;

    /* Reset all memories for the first time */
    TIDL_resetSysmem(algHandle->sysMems);
    intAlghandleMemSize = (int32_t)(ALIGN_SIZE((uint32_t)sizeof(TIDL_Obj), 128U));

    algHandle->sysScratchPtr.l3BasePtr[relativeCoreId] = (uint8_t *)TIDL_getMemoryChunkFromSysmem(algHandle->sysMems ,
                                    (uint32_t)msmcSramSize,
                                    128U,
                                    TIDL_SYSMEM_L3_SCRATCH, (uint32_t)IALG_PERSIST);

    if ( algHandle->sysScratchPtr.l3BasePtr[relativeCoreId] == NULL )
    {
      status = IALG_EFAIL;//:TODO: Add new error types for this
      tidl_printf(0, "Not able to allocate MSMC memory\n");
    }
    algHandle->intAlgHandle = (uint8_t *)TIDL_getMemoryChunkFromSysmem(algHandle->sysMems ,
                                    (uint32_t)intAlghandleMemSize,
                                    128U,
                                    TIDL_SYSMEM_L3_SCRATCH, (uint32_t)IALG_PERSIST);
    if ( algHandle->intAlgHandle == NULL )
    {
      status = IALG_EFAIL;//:TODO: Add new error types for this
      tidl_printf(0, "Not able to allocate intAlgHandle memory\n");
    }
  }

  if(status == IALG_EOK)
  {
    algHandle->sysScratchPtr.l2BasePtr[relativeCoreId] = (uint8_t *)TIDL_getMemoryChunkFromSysmem(algHandle->sysMems ,
                                    (uint32_t)l2SramSize,
                                    128U,
                                    TIDL_SYSMEM_L2_SCRATCH, (uint32_t)IALG_PERSIST);
    if ( algHandle->sysScratchPtr.l2BasePtr[relativeCoreId] == NULL )
    {
      status = IALG_EFAIL;//:TODO: Add new error types for this
      tidl_printf(0, "Not able to allocate L2 memory\n");
    }
  }

  if (status == IALG_EOK)
  {
    uint32_t privContextSize = TIDL_privGetContextSize();

    algHandle->privContext = (uint8_t *)TIDL_getMemoryChunkFromSysmem(algHandle->sysMems,
      (uint32_t)privContextSize,
      128U,
      TIDL_SYSMEM_L3_SCRATCH, (uint32_t)IALG_PERSIST);

    if (algHandle->privContext == NULL)
    {
      status = IALG_EFAIL;
      tidl_printf(0, "Not able to allocate priv algo context memory\n");
    }
  }

  if((createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0)
  {
    if(status == IALG_EOK)
    {
      if (gTempUseMsmcStageForWt == 1)
      {
        algHandle->layerShare.wtMemBasePtr = (uint8_t *)TIDL_getMemoryChunkFromSysmem(algHandle->sysMems ,
                                          TIDL_WT_STAGING_MEM,
                                          128U,
                                          TIDL_SYSMEM_L3_SCRATCH, (uint32_t)IALG_PERSIST);
        algHandle->layerShare.currPtrOffset = 0;
        algHandle->layerShare.lastPtrOffset = 0;
        algHandle->layerShare.isWtDmaPending = 0;
        algHandle->layerShare.totStageMemAvail = (int32_t)TIDL_WT_STAGING_MEM;

        algHandle->weightStageSync.wtMemBasePtr = algHandle->layerShare.wtMemBasePtr;
        algHandle->weightStageSync.currPtrOffset= (uint32_t)algHandle->layerShare.currPtrOffset;
        algHandle->weightStageSync.lastPtrOffset= (uint32_t)algHandle->layerShare.lastPtrOffset;
        algHandle->weightStageSync.isWtDmaPending= (uint32_t)algHandle->layerShare.isWtDmaPending;
        algHandle->weightStageSync.totStageMemAvail= (uint32_t)algHandle->layerShare.totStageMemAvail;
        if ( algHandle->layerShare.wtMemBasePtr == NULL )
        {
          status = IALG_EFAIL;//:TODO: Add new error types for this
          tidl_printf(0, "Not able to allocate wt stage memory\n");
        }
      }
    }
    if(status == IALG_EOK)
    {
      dmaContextSize = DmaUtilsAutoInc3d_getContextSize((uint32_t)TIDL_DMA_CHANNEL_MAX);

      algHandle->dmaUtilsContext = (uint8_t *)TIDL_getMemoryChunkFromSysmem(algHandle->sysMems ,
                                      (uint32_t) dmaContextSize,
                                        128U,
                                        TIDL_SYSMEM_L3_SCRATCH, (uint32_t)IALG_PERSIST);

      if ( algHandle->dmaUtilsContext == NULL )
      {
        status = IALG_EFAIL;//:TODO: Add new error types for this
        tidl_printf(0, "Not able to allocate DMAUTILS context memory\n");
      }

    }

    if(status == IALG_EOK)
    {
      algHandle->zeroVector1k  = (uint8_t *)TIDL_getMemoryChunkFromSysmem(algHandle->sysMems ,
                                                                          (TIDL_1K_ZERO_VECTOR),
                                                                          128U,
                                                                          TIDL_SYSMEM_L2_SCRATCH,
                                                                          (uint32_t)IALG_PERSIST);

      if ( algHandle->zeroVector1k  == NULL )
      {
        tidl_printf(0, "Not able to allocate zero vector in L2  memory\n");
        status = IALG_EFAIL;//:TODO: Add new error types for this
      }
      algHandle->memcpyTr = (uint8_t *)TIDL_getMemoryChunkFromSysmem(algHandle->sysMems ,
                                                                          (TIDL_MEMCPY_TR_MEM),
                                                                          128U,
                                                                          TIDL_SYSMEM_L2_SCRATCH,
                                                                          (uint32_t)IALG_PERSIST);

      if ( algHandle->zeroVector1k  == NULL )
      {
        tidl_printf(0, "Not able to allocate zero vector in L2  memory\n");
        status = IALG_EFAIL;//:TODO: Add new error types for this
      }
    }

    if(status == IALG_EOK)
    {
      algHandle->isPreEmptionEnable = TIDL_checkIfPreEmptionEnabled(createParams);
      if (algHandle->isPreEmptionEnable)
      {
        /* Allocate memory for preemption handle */
        algHandle->preEmptHandle = (uint8_t *)TIDL_getMemoryChunkFromSysmem(algHandle->sysMems,
          (uint32_t)TIDL_getHandleSizeForPreemption(algHandle->createParams->net->numLayers),
          128U,
          TIDL_SYSMEM_L3_SCRATCH, (uint32_t)IALG_PERSIST);

        if (algHandle->preEmptHandle == NULL)
        {
          algHandle->preEmptHandle = (uint8_t *)TIDL_getMemoryChunkFromSysmem(algHandle->sysMems,
          (uint32_t)TIDL_getHandleSizeForPreemption(algHandle->createParams->net->numLayers),
          128U,
          TIDL_SYSMEM_DDR_SCRATCH, (uint32_t)IALG_PERSIST);
          
          tidl_printf(2, "WARNING: Preemption handle allocated in DDR!\n");
        }
        if (algHandle->preEmptHandle == NULL)
        {
          status = IALG_EFAIL;//:TODO: Add new error types for this
          tidl_printf(0, "Not able to allocate preEmption Handle\n");
        }
      }
    }
  }

  if(status == IALG_EOK)
  {
  //:TODO: Remove this once inner product buffers are correclty given by perfsim
    algHandle->sysMems[TIDL_SYSMEM_L2_SCRATCH].base =
      (void *)  memRec[ALG_SCRATCH_L2_MEM_MEMREC].base;
    algHandle->sysMems[TIDL_SYSMEM_L2_SCRATCH].size =
        (int32_t )memRec[ALG_SCRATCH_L2_MEM_MEMREC].size - TIDL_1K_ZERO_VECTOR - TIDL_MEMCPY_TR_MEM;


    algHandle->sysScratchPtr.l1BasePtr[relativeCoreId]   = (uint8_t *) memRec[ALG_SCRATCH_L1_MEM_MEMREC].base;
    algHandle->sysScratchPtr.ddrBasePtr[relativeCoreId] = (uint8_t *) memRec[ALG_SCRATCH_DATA_BUFF_EXT_MEMREC].base;

    //:TODO: Find the right place
    if (algHandle->createParams->net->inferenceMode == TIDL_inferenceModeLowLatency)
    {
      int32_t coreIdx;
      for ( coreIdx = 0 ; coreIdx < MAX_CORES ;coreIdx++)
      {
        algHandle->sysScratchPtr.l1BasePtr[coreIdx] = (uint8_t *)algHandle->controlArgs.ctrlSetArgs[coreIdx].ncScratchPtr[TIDL_Virtual][TIDL_L1];
        algHandle->sysScratchPtr.l2BasePtr[coreIdx] = (uint8_t *)algHandle->controlArgs.ctrlSetArgs[coreIdx].ncScratchPtr[TIDL_Virtual][TIDL_L2];
        algHandle->sysScratchPtr.l3BasePtr[coreIdx] = (uint8_t *)algHandle->controlArgs.ctrlSetArgs[coreIdx].ncScratchPtr[TIDL_Virtual][TIDL_MSMC];
        algHandle->sysScratchPtr.ddrBasePtr[coreIdx] = (uint8_t *)algHandle->controlArgs.ctrlSetArgs[coreIdx].ncScratchPtr[TIDL_Virtual][TIDL_DDR];
        algHandle->sysScratchPtr.ddrPersistentBasePtr[coreIdx] = (uint8_t *)algHandle->controlArgs.ctrlSetArgs[coreIdx].ncScratchPtr[TIDL_Virtual][TIDL_DDR_PERSIST];
      }
    }
  }

  /* #######################################################################################################################################*/
  /* ##############################  No new Persistent memory records should be allocated in INIT after this point   #######################*/
  /* INIT for each layer resets sysMem resulting in offset set to 0 without any base ptr movement and any persistent memories allocated here on
      will be allocated at offset 0 and thereby overlap with layer level allocated scratch memories and can be potentially overwritten */
  /* ########################################################################################################################################*/
  algHandle->dmaChannelAllocContext.currChannelIdx = 0;
  algHandle->dmaChannelAllocContext.totalNumChannels = TIDL_DMA_CHANNEL_MEMCPY;

  algHandle->numODLayer = 0;

  tidlCommonParams.dmaChannelAllocContext = &algHandle->dmaChannelAllocContext;
  tidlCommonParams.dmaUtilsContext        = algHandle->dmaUtilsContext;
  tidlCommonParams.sysMems                = &algHandle->sysMems[0];
  tidlCommonParams.sysScratchPtr          = &algHandle->sysScratchPtr;
  tidlCommonParams.zeroVector1k           = algHandle->zeroVector1k;

  commonParams.createParams     = createParams;
  commonParams.gcHelperHandle       = gcHelperHandle;
  commonParams.net              = createParams->net;
  commonParams.TIDLLayersBufPtr = &TIDLLayersBuf;//:TODO: Check if this needs to be a global variable
  commonParams.tidlCommonParams   = &tidlCommonParams;

  if (status == IALG_EOK)
  {
    int32_t layerId;
    int32_t wlRepeatIter;
    sGetLayerIdContext_t getLayerIdContext;

    if( perfInfoOut != NULL)
    {
      algHandle->sysScratchPtr.ddrPersistentBasePtr[relativeCoreId] =  (uint8_t *)memRec[ALG_LAYERS_PARAMS_BUFF_MEMREC].base + paramMemTabOffset;
      paramMemTabOffset += gcOut->memoryRequirementPerCore[DDR_PERSIST];
      paramMemTabOffset = ALIGN_SIZE(paramMemTabOffset, 128);
    }

    getLayerIdInit(&getLayerIdContext,
                        relativeCoreId,
                        createParams->currLayersGroupId,
                        createParams->net,
                        gcHelperHandle,
                        wlSuperGrp);

    while(1)
    {
      layerId = getLayerIdToExecute(&getLayerIdContext,
                                   1,
                                   &wlRepeatIter,
                                   &currAlgLayer,
                                   &workloadUnit);

      if ( layerId == NOT_VALID)
      {
        break;
      }

      //:TODO: Eventually this should be removed once process also  uses the same loop structure
      if ( createParams->net->TIDLLayers[layerId].layerType == TIDL_DataLayer )
      {
        continue;
      }

      tidl_printf(1,"Alg Init for Layer # - %4d\n", layerId);
      memory[TIDL_LAYER_MEMORY_OUTPUT]     = (uint8_t *)memRec[ALG_SCRATCH_DATA_BUFF_MEMREC].base + dataMemTabOffset;
      memory[TIDL_LAYER_MEMORY_SCRATCH]    = (uint8_t *)memRec[ALG_REF_SCRATCH_BUFF_MEMREC].base;
      memory[TIDL_LAYER_MEMORY_PERSISTENT] = (uint8_t *)memRec[ALG_LAYERS_PARAMS_BUFF_MEMREC].base + paramMemTabOffset;

      sTIDL_AlgLayer_t *algLayer = get_AlgLayer_t_pointer(algHandle->alglayerParams,currAlgLayer);

      TIDL_getPadParams(&algLayer->wlPadParams, &createParams->net->TIDLLayers[layerId], createParams->flowCtrl, workloadUnit, gcHelperHandle);

      int32_t isWorkLoadSupported;

      isWorkLoadSupported = TIDL_isWorkLoadUnitFlowSupported(layerId,
                                                               createParams->net,
                                                               createParams->net->deviceName,
                                                               workloadUnit);

      /* Force workload unit to be NULL for going to old flow */
      if (isWorkLoadSupported == 0 )
      {
        workloadUnit = NULL;
      }

      /*setting defualt values for multiCoreCtxtLinkIndex */
      for(int i = 0;i < MAX_MULTI_CORE_CTX_COPY_LINK;i++)
      {
        algLayer->multiCoreCtxtLinkIndex[i] = NOT_VALID;
      }

      if(isWorkLoadSupported && createParams->net->inferenceMode == TIDL_inferenceModeLowLatency)
      {
        TIDL_setMultiCoreCtxtLinkIndex(algHandle, algLayer, currAlgLayer);
      }

      if((createParams->flowCtrl & TIDL_FLOW_CTRL_REF_STAT) == 0)
      {
        if (algHandle->createParams->quantRangeUpdateFactor == 0.0f)
        {
          TIDL_UpdateScaleFactors(algHandle, layerId, 0, 0, 0);
        }
      }

      for (j = 0; j < createParams->net->TIDLLayers[layerId].numInBufs; j++)
      {
        int32_t inDataId        = createParams->net->TIDLLayers[layerId].inData[j];
        algLayer->inLayerIdx[j] = TIDL_getLayerNum(createParams->net, inDataId);
        algLayer->isInData[j]   = TIDL_isInDataBuff(createParams->net, inDataId, createParams->currLayersGroupId);
        algLayer->isInOutData[j]   = TIDL_isOutDataBuff(createParams->net, inDataId, createParams->currLayersGroupId);

        algLayer->inWorkloadIdx[j]   = -1;
        //if (createParams->net->TIDLLayers[algLayer->inLayerIdx[j]].layerType != TIDL_DataLayer && gcHelperHandle != NULL)
        if (gcHelperHandle != NULL)
        {
          //TODO: Might have to make this multi-core safe
          /* Workload db is expected to be ordered by group execution order. Hence, the producer workload is expected to be
            nearby to the consumer workload in the array. Parsing db backwards from the current workload may result in quickly
            finding the producer workload. */
          sWorkloadUnit_t *pWLUnit =  getWLUnitPtr(gcHelperHandle , currAlgLayer);
          int32_t bufDBindex = NOT_VALID;
          for ( int32_t linkIdx = 0; linkIdx < pWLUnit->numLinks; linkIdx++)
          {
            const sLink_t * pLink = getLinkPtr(pWLUnit, NOT_VALID, linkIdx);
            if (( pLink->subType == LINK_X_FM_IN_FULL_TO_PART) || ( pLink->subType == LINK_P))
            {
              for (int32_t srcIdx = 0; srcIdx < pLink->numSrc; srcIdx++)
              {
                bufDBindex = pLink->src[srcIdx].bufDBindex;
                const sBufParams_t * bufParams = getBufParamsFromBufIndex(gcHelperHandle, bufDBindex);
                sMetaDataID_t metaDataId;
                getMetaDataID(bufParams->dataId, &metaDataId);

                if ( ( bufParams->type == BUF_FM_FULL) && (metaDataId.layerId == inDataId) )
                {
                  algLayer->inWorkloadIdx[j] = bufDBindex;
                }
              }
            }
          }

          // for (int32_t k = currAlgLayer-1; k >= 0; k--)
          // {
          //   sWorkloadUnit_t *pWLUnit =  getWLUnitPtr(gcHelperHandle ,k);
          //   if (pWLUnit->layerId == algLayer->inLayerIdx[j])
          //   {
          //     algLayer->inWorkloadIdx[j] = k;
          //     break;
          //   }
          // }
        }
      }
      //TODO: Is this really needed now that we know we have only one output?
      for (j = 0; j < createParams->net->TIDLLayers[layerId].numOutBufs; j++)
      {
        int32_t outDataId        = createParams->net->TIDLLayers[layerId].outData.dataId;
        algLayer->isOutData[j]   = TIDL_isOutDataBuff(createParams->net, outDataId, createParams->currLayersGroupId);
        if(gcHelperHandle != NULL)
        {
          sWorkloadUnit_t *pWLUnit =  getWLUnitPtr(gcHelperHandle, currAlgLayer);
          algLayer->outBufIdxWl  = WorkloadRefExec_GetOutBufIdx(pWLUnit, gcHelperHandle);
        }
      }
      algLayer->workloadUnit = workloadUnit;
      algLayer->gcHelperHandle   = gcHelperHandle;
      algLayer->layerIdx = layerId;
      algLayer->memcpyTr = algHandle->memcpyTr;
      algLayer->isOptimalForPreemption = 0 ;
      if(perfInfoOut != NULL)
      {
        algLayer->execLayerNum = getLayerExecutionNumber(algLayer->layerIdx,
          perfInfoOut->layerExecutionOrder, createParams->net->numLayers) ;
      }
      else
      {
        algLayer->execLayerNum = algLayer->layerIdx ;
      }

      void **outPtr = &algHandle->dataBuf[createParams->net->TIDLLayers[layerId].outData.dataId];

      layerSpecificParams.workloadUnit = workloadUnit;
      layerSpecificParams.isMixedPrecEnabled = 1U;

      /* Reset DMA resources and scratch memory resources for every iteration */
      TIDL_ResetDmaChannel(&algHandle->dmaChannelAllocContext);
      TIDL_resetSysmem(algHandle->sysMems);
      memset(memorySize, 0, sizeof(memorySize));

      if (createParams->net->TIDLLayers[layerId].layerType == TIDL_PoolingLayer)
      {
        //:TODO: Remove this from here. Should be happening at init time. Need to remove the tidlLayer changing heights from
        // init routine and use things directly from workload in NatC before attempting this change.
        algLayer->layerParams.poolParams.orgInTensorHeight = createParams->net->TIDLLayers[algLayer->inLayerIdx[0]].outData.dimValues[TIDL_DIM_HEIGHT];

      }

      /* TODO: Remove once workload migration is completed */
      if (gcHelperHandle != NULL && ( (createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == TIDL_FLOW_CTRL_REF_ONLY))
      {
        sWorkloadUnit_t *pWLUnit =  getWLUnitPtr(gcHelperHandle, currAlgLayer);
        TIDL_privSetTensorHeights(algHandle->createParams, algHandle->alglayerParams ,algLayer, pWLUnit, inHeightOrig, inChPitchOrig, inRoiPitchOrig, outHeightOrig, outChPitchOrig, outRoiPitchOrig);
      }

      status = TIDL_layerInitNew(&layerSpecificParams,
                                &commonParams,
                                algLayer,
                                layerId,
                                memory,
                                memorySize,
                                outPtr);

      /*TODO: Remove after workload migration */
      if (gcHelperHandle != NULL && ( (createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == TIDL_FLOW_CTRL_REF_ONLY))
      {
        TIDL_privRestoreTensorHeights(algHandle->createParams, algLayer, inHeightOrig, inChPitchOrig, inRoiPitchOrig, outHeightOrig, outChPitchOrig, outRoiPitchOrig);
      }

#if ENABLE_OLD_FLOW
      if ( status == NOT_VALID )
      {
#if ENABLE_BACKWARDS_COMPATIBILITY
        int32_t flowCtrlOrig = algHandle->createParams->flowCtrl;
        TIDL_CreateParams * createParamsCopy = (TIDL_CreateParams *)commonParams.createParams;
        if (perfInfoOut != NULL)
        {
          algHandle->createParams->flowCtrl = TIDL_FLOW_CTRL_REF_ONLY;
          createParamsCopy->flowCtrl = TIDL_FLOW_CTRL_REF_ONLY;
        }
#endif
        status = TIDL_layerInit(&layerSpecificParams,
                                &commonParams,
                                algLayer,
                                layerId,
                                memory,
                                memorySize,
                                outPtr,
                                memRec,
                                &paramMemTabOffset,
                                &dataMemTabOffset,
                                algHandle);//:TODO: memRec shouldnt be used eventually
#if ENABLE_BACKWARDS_COMPATIBILITY
        algHandle->createParams->flowCtrl = flowCtrlOrig;
        createParamsCopy->flowCtrl = flowCtrlOrig;
#endif
      }
#endif

      if ( status != IALG_EOK)
      {
        break;
      }

      dataMemTabOffset  += memorySize[TIDL_LAYER_MEMORY_OUTPUT];
      paramMemTabOffset += memorySize[TIDL_LAYER_MEMORY_PERSISTENT];

      dataMemTabOffset  = ALIGN_SIZE(dataMemTabOffset, 128);
      paramMemTabOffset = ALIGN_SIZE(paramMemTabOffset, 128);

      if ( paramMemTabOffset > (int32_t)memRec[ALG_LAYERS_PARAMS_BUFF_MEMREC].size )
      {
        status = TIDL_E_COMMON;
        tidl_printf(0,"Memory used for  ALG_LAYERS_PARAMS_BUFF_MEMREC is greater than requested \n");
      }

      if ( dataMemTabOffset > (int32_t)memRec[ALG_SCRATCH_DATA_BUFF_MEMREC].size )
      {
        status = TIDL_E_COMMON;
        tidl_printf(0,"Memory used for  ALG_SCRATCH_DATA_BUFF_MEMREC is greater than requested \n");
      }

      algLayer->metaData.totalOps = 0;
      algLayer->metaData.actualOps = 0;
      for (int32_t k = 0; k < TIDL_PROFILE_MAX; k++)
      {
        algLayer->metaData.profilePoint[k] = 0;
      }
    }
  }

  if(status == IALG_EOK)
  {
    if(createParams->optimiseExtMem != TIDL_OptimiseExtMemL0)
    {
      for(i = 0; i <  createParams->net->numLayers; i++)
      {
        const sTIDL_Layer_t *TIDLLayerCur = &createParams->net->TIDLLayers[i];
        for(j = 0; j < TIDLLayerCur->numInBufs; j++)
        {
          algHandle->TIDLLayersBuf->newInDataId[i][j] = TIDLLayersBuf.newInDataId[i][j];
        }
        algHandle->TIDLLayersBuf->newOutDataId[i][0] = TIDLLayersBuf.newOutDataId[i][0];
      }
    }
    algHandle->procCallCounter = 0;

    {
      int32_t ptrOffset = 0;
      algHandle->activationHistPtr = NULL;
      algHandle->activationRangePtr = NULL;

      if((createParams->flowCtrl & TIDL_FLOW_CTRL_REF_STAT) == TIDL_FLOW_CTRL_REF_STAT)
      {
        if (( createParams->net->calibrationOption & TIDL_CalibOptionActivationRange ) ==
                  TIDL_CalibOptionActivationRange && (createParams->net->quantizationStyle != TIDL_QuantStyleAsymNP2))
        {
          int32_t histogramSize = (createParams->net->numLayers *
                                   TIDL_NUM_ACTIVATION_HISTOGRAM_BINS *
                                   sizeof(int32_t));

          algHandle->activationHistPtr = (int32_t *)get_uint8_t_pointer((uint8_t *)memRec[ALG_REF_OUTPUT_BUFF_MEMREC].base, ptrOffset);
          memset(algHandle->activationHistPtr, 0, histogramSize);
          ptrOffset += ALIGN_SIZE(histogramSize, 128);
          if ( createParams->net->calibrationParams.activationRangeMethod ==
            TIDL_ActivationRangeMethodGlobalHistogram )
          {
            int32_t layerIdx;
            int32_t activationRangePtrSize = (createParams->net->numLayers *
                                     2 * sizeof(float32_tidl));
            algHandle->activationRangePtr = (float32_tidl *) get_uint8_t_pointer((uint8_t *)memRec[ALG_REF_OUTPUT_BUFF_MEMREC].base, ptrOffset);
            ptrOffset += ALIGN_SIZE(activationRangePtrSize, 128);
            for ( layerIdx = 0; layerIdx < createParams->net->numLayers; layerIdx++)
            {
              /* Copy the activation range during init these will come from the previous
              run of TIDL. Multiply it by a factor to account for any variation across
              iteration */
              algHandle->activationRangePtr[layerIdx * 2] =
                createParams->net->TIDLLayers[layerIdx].outData.minTensorValue;
              algHandle->activationRangePtr[layerIdx * 2 + 1] =
                createParams->net->TIDLLayers[layerIdx].outData.maxTensorValue;

            }
          }
        }
      }

      algHandle->refScratchBuf = (uint8_t *)get_uint8_t_pointer((uint8_t *)memRec[ALG_REF_OUTPUT_BUFF_MEMREC].base,
                                                              ptrOffset);
      algHandle->refScratchBufSize = memRec[ALG_REF_OUTPUT_BUFF_MEMREC].size - ptrOffset;
    }
  }

  if(status == IALG_EOK)
  {
    if(algHandle->perfSimOutput)
    {
     sPerfSimConfig_t *pSimConfig = &algHandle->perfSimOutput->simConfig;
     algHandle->ddrBytesPerCPUCycle =  pSimConfig->numEMIFPorts * pSimConfig->freqDDR *
     (pSimConfig->busWidthDDR/8)* (pSimConfig->ddrEfficiency /pSimConfig->freqMHz) ;
    }

    algHandle->preEmptContextInfo.contextMemPtr = (uint8_t*)memRec[ALG_CONTEXT_MEM_MEMREC].base;
    algHandle->preEmptContextInfo.contextMemSize = memRec[ALG_CONTEXT_MEM_MEMREC].size;
    algHandle->preEmptContextInfo.earlyPreEmpt = 0;
    status = TIDL_addPriorityObject(algHandle,&memRec[ALG_CONTEXT_MEM_MEMREC],
      algHandle->createParams->targetPriority);

    IALG_MemRec * pMemRec = (IALG_MemRec *)TIDL_getContextMemRec(algHandle);
    algHandle->preEmptContextInfo.contextMemPtr  = (uint8_t*) pMemRec->base;
    algHandle->preEmptContextInfo.contextMemSize = (int32_t)  pMemRec->size;

    if (algHandle->isPreEmptionEnable)
    {
      /* Initialize preemption handle */
      if(status == IALG_EOK)
      {
        int32_t numWL  = gcOut->dataBaseInfo.wlDB.count;
        algHandle->preemptPerfHW = 1;
        /* Avoided initializing preEmptHandle in L3, instead initializing it in the contextMemPtr (DDR)
           and copy it to preEmptHandle in L3 in the activate function
        */
        TIDL_initializeHandleForPreemption(algHandle->preEmptContextInfo.contextMemPtr, algHandle->gcHelperHandle,
        algHandle->perfSimOutput,
        algHandle->createParams->net->numLayers, numWL, algHandle->createParams->maxPreEmptDelay,
        algHandle->sysScratchPtr.l2BasePtr[relativeCoreId], algHandle->ddrBytesPerCPUCycle,
        algHandle->perfSimOutput->simConfig.freqMHz, algHandle->alglayerParams);
        /* To copy this contextMemPtr to preEmptHandle in the activate function */
        algHandle->preEmptContextInfo.contextBufInfo[0].src = (uint8_t *) algHandle->preEmptHandle;
        algHandle->preEmptContextInfo.contextBufInfo[0].dst = (uint8_t *) algHandle->preEmptContextInfo.contextMemPtr;
        algHandle->preEmptContextInfo.contextBufInfo[0].size = TIDL_getHandleSizeForPreemption(algHandle->createParams->net->numLayers);
        algHandle->preEmptContextInfo.earlyPreEmpt = 1;
        algHandle->preEmptContextInfo.numContextBufs = 1;
      }
      tidl_printf(1, "TIDL_initializeHandleForPreemption is completed \n");
    }
  }

  if(status == IALG_EOK)
  {
    if((algHandle->createParams->net->inferenceMode == TIDL_inferenceModeLowLatency))
    {
      /* Set the Global sync buf values to default state */
      volatile int8_t * syncPtr = (int8_t *)algHandle->controlArgs.ctrlSetArgs[relativeCoreId].syncBufferPtr[TIDL_Virtual];
      memset((void *)syncPtr, TIDL_SYNC_WL_STATE_DEFAULT, TIDL_GLOBAL_SYNC_OFFSET * sizeof(int8_t));
    }
    int32_t numODLayer = algHandle->numODLayer;
    sODLayerInfo_t *odLayerInfo = algHandle->odLayerInfo;
    sTIDL_Network_t *origNet = createParams->net ; 
#if ENABLE_BACKWARDS_COMPATIBILITY
    int32_t flowCtrlOrig = algHandle->createParams->flowCtrl;
    if (perfInfoOut != NULL)
    {
      algHandle->createParams->flowCtrl = TIDL_FLOW_CTRL_REF_ONLY;
    }
#endif
    TIDL_MoveNetToPrivateMemory(algHandle, memRec, relativeCoreId);
    //Need to do init again specifically for OD as the parameters are assigned quite a lot
    while(numODLayer){
      numODLayer--;
      TIDL_odFindValidLocAndScoreKernelInit(algHandle->createParams,
        odLayerInfo[numODLayer].algLayer_OD,
        &algHandle->createParams->net->TIDLLayers[odLayerInfo[numODLayer].i_OD],0,0,0);
    }
#if ENABLE_BACKWARDS_COMPATIBILITY
    algHandle->createParams->flowCtrl = flowCtrlOrig;
#endif
    /*-----------------------------------------------------------------------*/
    /* State to indicate that internal context of algorithm is not active    */
    /*-----------------------------------------------------------------------*/
    algHandle->algState = (uint8_t)ALG_NOT_ACTIVE;
    TIDL_play(origNet, 0);
  }

  TIDL_enableL1DandL2CacheWb();

  return (status);
}

/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION
@fn         TIDL_control
@brief      Function to accept any run time updates with recreation of object
            Refer algControl function for details in ivision.h file
@param      handle   : Algorithm Instance handle
@param      cmd      : Command to be processed
@param      inParams : Input Parameters
@param      outParams: Output Parameters
@remarks    None
@return     IALG_EOK   - Successful
            IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/
int32_t TIDL_control(IVISION_Handle Handle,
IALG_Cmd cmd,
const IALG_Params *inParams,
IALG_Params *outParams)
{
  int32_t status = IALG_EOK;

  tidl_printf(1, "TIDL control call ...\n");

  TIDL_Handle algHandle = (TIDL_Handle)(void*)Handle;

  switch (cmd)
  {
    case TIVX_TIDL_CMD_GET_C7X_PTRS:
    {
      if(outParams == NULL)
      {
        status = IALG_EFAIL;
        return status;
      }
      TIDL_controlGetArgs * outArgs = (TIDL_controlGetArgs *)outParams;

      outArgs->ctrlGetArgs.ncScratchPtr[TIDL_Virtual][TIDL_L1] = (void*) algHandle->memRec[ALG_SCRATCH_L1_MEM_MEMREC].base;
      outArgs->ctrlGetArgs.ncScratchPtr[TIDL_Virtual][TIDL_L2] = (void*) algHandle->memRec[ALG_SCRATCH_L2_MEM_MEMREC].base;
      outArgs->ctrlGetArgs.ncScratchPtr[TIDL_Virtual][TIDL_MSMC] = (void*) algHandle->memRec[ALG_SCRATCH_L3_MEM_MEMREC].base;
      outArgs->ctrlGetArgs.ncScratchPtr[TIDL_Virtual][TIDL_DDR] = (void*) algHandle->memRec[ALG_SCRATCH_DATA_BUFF_EXT_MEMREC].base;
      outArgs->ctrlGetArgs.ncScratchPtr[TIDL_Virtual][TIDL_DDR_PERSIST] = (void*) algHandle->memRec[ALG_LAYERS_PARAMS_BUFF_MEMREC].base;
      outArgs->ctrlGetArgs.syncBufferPtr[TIDL_Virtual] = (void *)algHandle->memRec[ALG_PERSIST_SYNC_MEMREC].base;
      break;
    }
    case TIVX_TIDL_CMD_SET_C7X_PTRS:
    {
      if(inParams == NULL)
      {
        status = IALG_EFAIL;
        return status;
      }
      TIDL_controlSetArgs * inArgs = (TIDL_controlSetArgs *)inParams;
      memcpy((void *)(&algHandle->controlArgs), (void *)inArgs, sizeof(TIDL_controlSetArgs));

      break;
    }
    case TIVX_TIDL_CMD_INIT:
    {
      status = TIDL_init((IALG_Handle)Handle, NULL, NULL, NULL);
      break;
    }
    default:
    {
        tidl_printf(0, "Invalid Node Cmd Id\n");
        status = IALG_EFAIL;
        break;
    }
  }
  return (status);
}

/* #define INTERNAL_PROFILING */
#ifdef INTERNAL_PROFILING
typedef struct
{
  char   fmt[20];
  char   fmtVal[20];
  char   string[64];
  void*  pValue ;
} sTIDLProfilePrintInfo_t ;

static TIDL_LayerMetaData gLayerMetaData = {0};

static sTIDLProfilePrintInfo_t gProfilePrintInfo[] =
{
  {"%6s,"   , "%6d,"     , "Layer"                 ,  NULL},
  {"%15s,"  , "%15llu,"  , "Layer Cycles"          ,  &gLayerMetaData.profilePoint[TIDL_PROFILE_LAYER]},
  {"%15s,"  , "%15llu,"  , "kernelOnlyCycles"      ,  &gLayerMetaData.profilePoint[TIDL_PROFILE_KERNEL_ONLY]},
  {"%15s,"  , "%15llu,"  , "coreLoopCycles"        ,  &gLayerMetaData.profilePoint[TIDL_PROFILE_CORE_LOOP]},
  {"%15s,"  , "%15llu,"  , "LayerSetupCycles"      ,  &gLayerMetaData.profilePoint[TIDL_PROFILE_LAYER_SETUP]},
  {"%15s,"  , "%15llu,"  , "dmaPipeupCycles"       ,  &gLayerMetaData.profilePoint[TIDL_PROFILE_DMA_PIPEUP]},
  {"%18s,"  , "%18llu,"  , "dmaPipeDownCycles"     ,  &gLayerMetaData.profilePoint[TIDL_PROFILE_DMA_PIPEDOWN]},
  {"%15s,"  , "%15llu,"  , "PrefetchCycles"        ,  &gLayerMetaData.profilePoint[TIDL_PROFILE_PREFETCH]},
  {"%18s,"  , "%18llu,"  , "copyKerCoeffCycles"    ,  &gLayerMetaData.profilePoint[TIDL_PROFILE_KERNEL_COPY]},
  {"%15s,"  , "%15llu,"  , "LayerDeinitCycles"     ,  &gLayerMetaData.profilePoint[TIDL_PROFILE_LAYER_DEINIT]},
  {"%15s,"  , "%15llu,"  , "LastBlockCycles"       ,  &gLayerMetaData.profilePoint[TIDL_PROFILE_LAST_BLOCK_CYCLES]},
  {"%15s,"  , "%15llu,"  , "paddingTrigger"        ,  &gLayerMetaData.profilePoint[TIDL_PROFILE_LAYER_PADDING_TRIGGER]},
  {"%15s,"  , "%15llu,"  , "paddingWait"           ,  &gLayerMetaData.profilePoint[TIDL_PROFILE_LAYER_PADDING_WAIT]},
  {"%15s,"  , "%15llu,"  , "LayerWithoutPad"       ,  &gLayerMetaData.profilePoint[TIDL_PROFILE_LAYER_WITHOUT_PADDING]},
  {"%15s,"  , "%15llu,"  , "LayerHandleCopy"       ,  &gLayerMetaData.profilePoint[TIDL_PROFILE_LAYER_SETUP_HANDLE_COPY]},
  {"%15s,"  , "%15llu,"  , "BackupCycles"          ,  &gLayerMetaData.profilePoint[TIDL_PROFILE_BACKUP]},
  {"%15s,"  , "%15llu,"  , "RestoreCycles"         ,  &gLayerMetaData.profilePoint[TIDL_PROFILE_RESTORE]},
  {"%15s,"  , "%15llu,"  , "Multic7xContextCopyCycles",  &gLayerMetaData.profilePoint[TIDL_PROFILE_CONTEXT_CROSSCORE_COPY]}
};

void TIDL_printProfileData(
  int32_t numLayers,
  uint64_t tStartNetwork, uint64_t tEndNetwork, TIDL_outArgs * tidlOutArgs)
{
  int32_t numProfilePoints, i, j;
  uint64_t sumOfLayerCycles = 0;

  for ( i= 0; i < numLayers; i++)
  {
    if ( i ==  0)
    {
      uint64_t netWorkCycles;
      netWorkCycles = (uint64_t)(tEndNetwork - tStartNetwork);
      numProfilePoints = (int32_t)(sizeof(gProfilePrintInfo)/sizeof(sTIDLProfilePrintInfo_t));
      tidl_printf(0,"\n");
      tidl_printf(0,"Network Cycles %llu \n", netWorkCycles);
      for ( j= 0; j < numProfilePoints; j++)
      {
        tidl_printf(0,gProfilePrintInfo[j].fmt, gProfilePrintInfo[j].string);
      }
      tidl_printf(0,"\n");
    }

    gLayerMetaData = tidlOutArgs->metaDataLayer[i];
    gProfilePrintInfo[0].pValue = &tidlOutArgs->metaDataLayer[i].layerExecId;

    if (tidlOutArgs->metaDataLayer[i].layerExecId != -1) /* Print for layers other than data layers */
    {
      for ( j= 0; j < numProfilePoints; j++)
      {
        tidl_printf(0,gProfilePrintInfo[j].fmtVal, *((uint32_t *)gProfilePrintInfo[j].pValue));
      }
      tidl_printf(0,"\n");
    }
    sumOfLayerCycles += gLayerMetaData.profilePoint[TIDL_PROFILE_LAYER];
  }

  tidl_printf(0," Sum of Layer Cycles %llu \n", sumOfLayerCycles);
}

#endif
/**
 * @brief This is the layer processing function and should be called
 *          for each layer process.
 *
 * @param intAlgHandle  : Algorithm Instance handle
 * @param algLayer : This structure contains the layer specific parameters
 * @param TIDLLayer : This structure contains the common layer parameters
 *          in TIDL
 * @param inPtrs : Input Buffer Pointers
 * @param outPtrs : Output Buffer Pointers descriptor
 * @param sysMems : This structure has the system memory handles in TIDL
 * @param i : Layer number to be processed
 * @return int32_t : return value
 */
int32_t TIDL_layerProcess(
  TIDL_Handle          intAlgHandle,
  sTIDL_AlgLayer_t     * algLayer,
  sTIDL_Layer_t        * TIDLLayer,
  void                 * inPtrs[],
  void                 * outPtrs[],
  sTIDL_sysMemHandle_t * sysMems,
  int32_t i,
  TIDL_NetworkCommonParams   *commonParams)
{
  int32_t status = IALG_EOK;

  if ((TIDLLayer->layerType == TIDL_DetectionOutputLayer) || (TIDLLayer->layerType == TIDL_OdPostProcessingLayer))
  {
    status = TIDL_detectionOutputProcess(intAlgHandle, algLayer, TIDLLayer, inPtrs, outPtrs, sysMems);
  }
  else if (TIDLLayer->layerType == TIDL_RoiPoolingLayer)
  {
    status = TIDL_roiPoolingProcess(intAlgHandle, algLayer, TIDLLayer, inPtrs, outPtrs, sysMems);
  }
  else if(TIDLLayer->layerType == TIDL_OdOutputReformatLayer)
  {
    status = TIDL_odOutputReformatProcess(intAlgHandle, algLayer, TIDLLayer, inPtrs, outPtrs, sysMems);
  }
  else if(TIDLLayer->layerType == TIDL_SqueezeLayer)
  {
    // copy input data to output buffer. NC can allocate output and input buffer same
    status = TIDL_squeezeProcess(intAlgHandle, algLayer, TIDLLayer, inPtrs, outPtrs, sysMems);
  }
  else if(TIDLLayer->layerType == TIDL_ReshapeLayer)
  {
    /*Copy the data for reference flow if perfsimInfo not available*/
    if(intAlgHandle->perfSimOutput == NULL)
    {
      int32_t cpSize = TIDLLayer->outData.dimValues[TIDL_DIM_BATCH]*TIDLLayer->outData.pitch[TIDL_ROI_PITCH];
      int32_t outElementSize     = TIDL_getDatElementSize(TIDLLayer->outData.elementType);

      memcpy((void*)((size_t)outPtrs[0]), (void*)((size_t)inPtrs[0]), outElementSize*cpSize);
    }
  }
  else
  {
    status = TIDL_E_UNSUPPORTED_LAYER;
  }

  return (status);
}


/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION
@fn         TIDL_process
@brief      This is the main processing function and should be called
            for each frame.
            Refer process function for details in ivision.h file

@param      handle : Algorithm Instance handle
@param      inBufs : Input Buffer Pointers descriptor
@param      outBufs: Output Buffer Pointers descriptor
@param      inArgs : Input Arguments
@param      outArgs: Output Arguments
@remarks    None
@return     IALG_EOK   - Successful
            IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/

int32_t gCurrLayerNumber;
int32_t gLayerNumber = -1;
/* CHECK_MISRA("-16.7")  -> Disable rule 16.7
Pointer parameter inBufs and outBufs are not used to modify the addressed
object but is not declared as a pointer to const
Deviation is allowed because this function is of type IVISION_Fxns :
algProcess in order to be ASSIGNED IVISION_Fxns */
int32_t TIDL_process(IVISION_Handle Handle,
IVISION_InBufs *inBufs,
IVISION_OutBufs *outBufs,
IVISION_InArgs *inArgs,
IVISION_OutArgs *outArgs)
{
  /** Init phase performs a lot of writes to DDR which can get cached in L1/L2D
   * so need to writeback/invalidate cache to ensure correct data is fetched in case DMA is done e.g. DMA transfer for handle from DDR to MSMC can result in
   * corrupt data if this wb/inv is not done
  */
  TIDL_enableL1DandL2CacheWb();
  /*RESET_MISRA("16.7")  -> Reset rule 16.7  */
  TIDL_Handle algHandle = (TIDL_Handle)(void*)(Handle);
  int32_t coreId = algHandle->createParams->coreId;
  int32_t relativeCoreId = GET_RELATIVE_COREIDX(coreId, algHandle->createParams->coreStartIdx);
  TIDL_Handle intAlgHandle = NULL;
  int32_t twStatus      = IALG_EOK;
  int32_t status        = IALG_EOK;
  TIDL_InArgs * tidlInArgs = (TIDL_InArgs*)(void*)inArgs;
  TIDL_outArgs * tidlOutArgs = (TIDL_outArgs*)(void*)outArgs;

  int32_t currAlgLayer = 0;
  int32_t i;
  TIDL_NetworkCommonParams   commonParams;
  WorkloadUnitExec_CommonParams tidlCommonParams;
  // WorkloadUnitExec_RefParams  wlRefParams;
  uint64_t tStart, tEnd;
  uint64_t tStartNetwork, tEndNetwork;
  uint64_t tStart1, tEnd1;
  int32_t oldIntState = 0;
  int32_t lockState = UNLOCKED;

  TIDL_getTscTime(&tStartNetwork);

  status = TIDL_initDebugTraceParams(algHandle->createParams->traceLogLevel,
  algHandle->createParams->traceWriteLevel,
  algHandle->createParams->TIDLVprintf,
  algHandle->createParams->TIDLWriteBinToFile,
  algHandle->createParams->TIDLReadBinFromFile,
  algHandle->createParams->traceBaseName,
  algHandle->createParams->tracePtr);

  if (status != IALG_EOK)
  {
    status = TIDL_E_INVALID_DEBUG_TRACE_PRM;
  }

  /*-----------------------------------------------------------------------*/
  /* Activate the algorithm to make sure that now onwards internal memory  */
  /* handle can be utilized                                                */
  /*-----------------------------------------------------------------------*/
  if (status == IALG_EOK)
  {
    /* Disable interrupts here */
    if (algHandle->isPreEmptionEnable)
    {
      tidl_printf(1, "TIDL_process is started with handle : %x \n", algHandle);
      oldIntState = TIDL_lockForPreemption(algHandle, algHandle->createParams, &lockState);
    }
    else
    {
      TIDL_switchHandles((IALG_Handle)(void*)Handle);
    }

    intAlgHandle = (TIDL_Handle)(void *)algHandle->intAlgHandle;
    intAlgHandle->isPadDmaPending = 0;

    /*Scale factors should never be updated for TIDL_QuantStyleAsymNP2_TFL*/
    if((intAlgHandle->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_STAT) == 0)
    {
      if ((intAlgHandle->createParams->net->quantizationStyle == TIDL_QuantStyleNP2Fixed) ||
        (intAlgHandle->createParams->net->quantizationStyle == TIDL_QuantStyleP2Dynamic))
      {
        for (i = 0; i < intAlgHandle->createParams->net->numLayers; i++)
        {
          if ((intAlgHandle->createParams->quantRangeUpdateFactor != 0.0f) &&
              (intAlgHandle->createParams->net->TIDLLayers[i].layersGroupId ==
               intAlgHandle->createParams->currLayersGroupId))
          {
            TIDL_UpdateScaleFactors(intAlgHandle, i, 0, 0, 0);
          }
        }
      }
    }

    for (i = 0; i < (int32_t)inBufs->numBufs; i++)
    {
      sTIDL_DataParams_t *inDataParams;
      int32_t layeType = TIDL_getLayerTypeForOutDataID(intAlgHandle->createParams->net,inBufs->bufDesc[i]->bufferId);
      if (layeType != TIDL_DataLayer)
      {
        inDataParams = TIDL_getDataParams(intAlgHandle->createParams->net,inBufs->bufDesc[i]->bufferId);
        inDataParams->tensorScale = tidlInArgs->scale[i];
      }
    }

    if (intAlgHandle->isPreEmptionEnable)
    {
      status = TIDL_updateContextMemPtrsForPreemption(&intAlgHandle->preEmptContextInfo, intAlgHandle->gcHelperHandle,
      intAlgHandle->privContext,
      intAlgHandle->preEmptHandle, intAlgHandle->perfSimOutput, intAlgHandle->sysScratchPtr.l2BasePtr[relativeCoreId],
      intAlgHandle->sysScratchPtr.l3BasePtr[relativeCoreId], 0, intAlgHandle->createParams->net->numLayers);
      TIDL_unlockForPreemption(oldIntState, intAlgHandle->createParams, &lockState);
    }

    if (status == IALG_EOK)
    {
      sGetLayerIdContext_t getLayerIdContext;
      sWorkloadUnit_t         *workloadUnit = NULL;
      const sTensorPosition_t *tensorPosition = NULL;
      const sTensorDims_t     *tensorDims     = NULL;
      int32_t layerId;
      int32_t wlRepeatIter;

      getLayerIdInit(&getLayerIdContext,
                          relativeCoreId,
                          intAlgHandle->createParams->currLayersGroupId,
                          intAlgHandle->createParams->net,
                          intAlgHandle->gcHelperHandle,
                          intAlgHandle->wlSuperGrp);

      tidlCommonParams.dmaChannelAllocContext = &intAlgHandle->dmaChannelAllocContext;
      tidlCommonParams.dmaUtilsContext        = intAlgHandle->dmaUtilsContext;
      tidlCommonParams.sysMems                = &intAlgHandle->sysMems[0];
      tidlCommonParams.sysScratchPtr          = &intAlgHandle->sysScratchPtr;
      tidlCommonParams.zeroVector1k           = intAlgHandle->zeroVector1k;

      commonParams.createParams     = intAlgHandle->createParams;
      commonParams.gcHelperHandle   = (const sGCHelperHandle *)intAlgHandle->gcHelperHandle;
      commonParams.net              = intAlgHandle->createParams->net;
      commonParams.TIDLLayersBufPtr = intAlgHandle->TIDLLayersBuf;//:TODO: Check if this needs to be a global variable
      commonParams.tidlCommonParams   = &tidlCommonParams;

      int32_t layer, numProfile;
      if(tidlInArgs->enableLayerPerfTraces == 1)
      {
        for(layer = 0; layer < intAlgHandle->createParams->net->numLayers; layer++)
        {
          for(numProfile = 0; numProfile < TIDL_PROFILE_MAX; numProfile++)
          {
            tidlOutArgs->metaDataLayer[layer].profilePoint[numProfile] = 0;
          }
        }
      }

      TIDL_controlSetArgs * args = (TIDL_controlSetArgs *)(&intAlgHandle->controlArgs);

      /* Reset the sync buffer to -1 at the start of process call */
      if((algHandle->createParams->net->inferenceMode == TIDL_inferenceModeLowLatency))
      {
        sGraphCompilerOutArgsCore_t * gcOutCore = NULL;
        sGraphCompilerOutArgs_t * gcOutArgs  = (sGraphCompilerOutArgs_t *)get_int8_t_pointer((int8_t *)(algHandle->createParams->net), 
                                                algHandle->createParams->net->graphCompilerInfoOffset);
        gcOutCore = (sGraphCompilerOutArgsCore_t *)getGCDataBase(gcOutArgs , DATABASE_GCOUT_1);                                        
        volatile int8_t * syncPtr = (int8_t *)args->ctrlSetArgs[relativeCoreId].syncBufferPtr[TIDL_Virtual] + (int8_t)TIDL_GLOBAL_SYNC_OFFSET;
        /* Set all workloads' states to default - core 0 has max. number of workloads, so take number of workloads from core 0 db info */
        memset((void *)syncPtr, TIDL_SYNC_WL_STATE_DEFAULT, gcOutCore->dataBaseInfo.wlDB.count * sizeof(int8_t));
        
        TIDL_syncWl(args->ctrlSetArgs, {},TIDL_SYNC_POINT_GLOBAL_SYNC, 0, 
                    relativeCoreId, algHandle->createParams->net->numCores);
      }

      while(1)
      {
        sTIDL_AlgLayer_t *algLayer;
        sTIDL_Layer_t *tidlLayer;
        void * inPtrs[TIDL_NUM_IN_BUFS] = {0};
        void * outPtrs[TIDL_NUM_OUT_BUFS] = {0};

        /* Should probably keep this in context mem to prevent overwrite during preemption */
        layerId = getLayerIdToExecute(&getLayerIdContext,
                                     0,
                                     &wlRepeatIter,
                                     &currAlgLayer,
                                     &workloadUnit);

        tidl_printf(2, "Coreid %d Layerid to execute = %d \n", relativeCoreId, layerId);
        if ( layerId == NOT_VALID)
        {
          break;
        }

        if(algHandle->createParams->net->inferenceMode == TIDL_inferenceModeLowLatency)
        {
          int32_t numCores = algHandle->createParams->net->numCores;
          TIDL_syncWl(args->ctrlSetArgs, workloadUnit->dependId, TIDL_SYNC_POINT_START, workloadUnit->wlExecId ,relativeCoreId, numCores);
        }

        TIDL_getTscTime(&tStart); /* Start layer cycles count after Sync is done and layer execution starts*/

        tidl_printf(1,"Core %d Alg Process for Layer # - %4d, layer type %d \n", relativeCoreId, layerId, intAlgHandle->createParams->net->TIDLLayers[layerId].layerType);

        //:TODO: Eventually this should be removed once process also  uses the same loop structure
        if ( intAlgHandle->createParams->net->TIDLLayers[layerId].layerType == TIDL_DataLayer )
        {
          continue;
        }

        /* Disable interrupts here */
        if (intAlgHandle->isPreEmptionEnable)
        {
          oldIntState = TIDL_lockForPreemption(algHandle, intAlgHandle->createParams, &lockState);
        }

        /*  Critical section starts here */
        tidlLayer = &intAlgHandle->createParams->net->TIDLLayers[layerId];
        int32_t targetDevice = intAlgHandle->createParams->net->deviceName;
        TIDL_forceTargetDeviceNonOTF(&targetDevice, tidlLayer->layerType, -1);

        /* Layer specific internal parameters */
        algLayer = get_AlgLayer_t_pointer(algHandle->alglayerParams, currAlgLayer);
        intAlgHandle->alglayerParams[currAlgLayer].metaData.layerExecId = algLayer->layerIdx;
        intAlgHandle->currAlgLayer = currAlgLayer;

#if ENABLE_BACKWARDS_COMPATIBILITY
        int32_t flowCtrlOrig = intAlgHandle->createParams->flowCtrl;
        if (algLayer->workloadUnit == NULL)
        {
          intAlgHandle->createParams->flowCtrl = TIDL_FLOW_CTRL_REF_ONLY;
        }
#endif
      int32_t isRestoreHeightsRequired = 0;
      if( tidlLayer->layerType != TIDL_ConstDataLayer)
      {

        /* Reset DMA resources and scratch memory resources for every iteration */
        TIDL_ResetDmaChannel(&intAlgHandle->dmaChannelAllocContext);
        TIDL_resetSysmem(intAlgHandle->sysMems);

        int32_t isRefExecFlowEnabled = WorkloadRefExec_enableRefExecFlow(intAlgHandle->createParams->flowCtrl, tidlLayer, algLayer);
        WorkloadRefExec_RefExecParams  wlRefParams;

        if(isRefExecFlowEnabled == 1)
        {
          TIDL_privSetTensorHeights(intAlgHandle->createParams, intAlgHandle->alglayerParams ,algLayer, workloadUnit,
                                      wlRefParams.origInHeight, wlRefParams.origInChPitch, wlRefParams.origInRoiPitch, &wlRefParams.origOutHeight, 
                                      &wlRefParams.origOutChPitch, &wlRefParams.origOutRoiPitch);
          isRestoreHeightsRequired = 1;
        }
        status = TIDL_getLayerInPtrs(inBufs, outBufs, intAlgHandle, algLayer, tidlLayer, inPtrs, layerId);

        if(status == IALG_EOK)
        {
          status = TIDL_getLayerOutPtrs(outBufs, intAlgHandle, algLayer, tidlLayer, outPtrs, layerId);
        }

        if(status == IALG_EOK)
        {
          TIDL_getTscTime(&tStart1);
          TIDL_getTscTime(&tEnd1);
          TIDL_updateprofileData(intAlgHandle->alglayerParams[currAlgLayer].metaData.profilePoint,
                                TIDL_PROFILE_LAYER_PADDING_WAIT,
                                tStart1,
                                tEnd1);

          tidl_printf(1,"Processing Layer # - %4d\n", layerId);

          algLayer->wlRepeatIter = wlRepeatIter;

          // TODO: Change this for ST
          algLayer->procType = TIDL_getAlgProcType(workloadUnit);
          algLayer->memcpyTr = intAlgHandle->memcpyTr;
          status = WorkloadRefExec_Process(intAlgHandle, &commonParams, workloadUnit, algLayer, tidlLayer, inPtrs, outPtrs, layerId, relativeCoreId);

          if (intAlgHandle->createParams->traceWriteLevel > 0)
          {
            if (algLayer->gcHelperHandle != NULL)
            {
              /* Need to find a better way to load this */
              // sWorkloadUnitAuxilary_t* auxilaryWorkloadDB = (sWorkloadUnitAuxilary_t*) algLayer->gcHelperHandle->dbPayLoad[algLayer->gcHelperHandle->dbParams[DB_BUF].size +
              //                                                             algLayer->gcHelperHandle->dbParams[DB_WL].size];
              sWorkloadUnitAuxilary_t* auxilaryWorkloadDB = (sWorkloadUnitAuxilary_t*) algLayer->gcHelperHandle->pAuxWLDB->pWLAuxList;
              tensorDims     = &auxilaryWorkloadDB[currAlgLayer].outSubTensorDims;
              tensorPosition = &auxilaryWorkloadDB[currAlgLayer].outSubTensorPosition;
            }
            if((isRefExecFlowEnabled == 0) && (intAlgHandle->createParams->net->inferenceMode  == TIDL_inferenceModeLowLatency))
            {
              TIDL_privSetTensorHeights(intAlgHandle->createParams, intAlgHandle->alglayerParams ,algLayer, workloadUnit,
                                          wlRefParams.origInHeight, wlRefParams.origInChPitch, wlRefParams.origInRoiPitch, &wlRefParams.origOutHeight, &wlRefParams.origOutChPitch, &wlRefParams.origOutRoiPitch);
              isRestoreHeightsRequired = 1;
            }
            twStatus = WorkloadRefExec_writeTraceDataBuf(outPtrs[0], intAlgHandle, tidlLayer, algLayer, tensorPosition, tensorDims);
          }

          if(twStatus != IALG_EOK)
          {
            tidl_printf(0,"TIDL_process : Trace write failed\n");
          }
          tidl_writeTraceParamBuf(intAlgHandle->createParams->net,
                                  algLayer,
                                  tidlLayer,
                                  tidlLayer->outData.dataId,
                                  (int8_t *)intAlgHandle->memRec[ALG_REF_OUTPUT_BUFF_MEMREC].base,
                                  intAlgHandle->memRec[ALG_REF_OUTPUT_BUFF_MEMREC].size);
        }
        if(isRestoreHeightsRequired == 1)
        {
          TIDL_privRestoreTensorHeights(intAlgHandle->createParams, algLayer, wlRefParams.origInHeight, wlRefParams.origInChPitch, 
                                        wlRefParams.origInRoiPitch, &wlRefParams.origOutHeight, &wlRefParams.origOutChPitch, &wlRefParams.origOutRoiPitch);
        }

        //:TODO: Need to handle copying persistent weights when group changes
        intAlgHandle->groupId = getLayerIdContext.groupIdx;

#if ENABLE_BACKWARDS_COMPATIBILITY
        intAlgHandle->createParams->flowCtrl = flowCtrlOrig;
#endif
        TIDL_getTscTime(&tEnd);
        TIDL_updateprofileData(intAlgHandle->alglayerParams[currAlgLayer].metaData.profilePoint, TIDL_PROFILE_LAYER,  tStart, tEnd);

        tidl_printf(1, "End of Layer # - %4d with outPtrs[0] = %p\n", layerId, (int8_t*)outPtrs[0]);

        if (intAlgHandle->isPreEmptionEnable)
        {
          TIDL_updateLayerCyclesForPreemption((void*)intAlgHandle->preEmptHandle, intAlgHandle->preemptPerfHW, (tEnd-tStart),
                                                  algLayer->execLayerNum, algLayer->procType,
                                                  intAlgHandle->perfSimOutput->simConfig.freqMHz);
        }

      }

        if ( status != IALG_EOK)
        {
          break;
        }

        if(algHandle->createParams->net->inferenceMode == TIDL_inferenceModeLowLatency)
        {
          int32_t numCores = algHandle->createParams->net->numCores;
          /* Update current workload completion in the state array */
          TIDL_syncWl(args->ctrlSetArgs, {},TIDL_SYNC_POINT_COMPLETE, workloadUnit->wlExecId, relativeCoreId, numCores);
        }


        if (intAlgHandle->isPreEmptionEnable)
        {
          //if (TIDL_isThisLayerOptimalForPreemption(intAlgHandle->preEmptHandle, currExecutionLyr))
          if(algLayer->isOptimalForPreemption)
          {
            int32_t currExecutionLyr = algLayer->execLayerNum;

            int32_t nextExecutionLyr = currExecutionLyr + 1;
            status = TIDL_updateContextMemPtrsForPreemption(&intAlgHandle->preEmptContextInfo, intAlgHandle->gcHelperHandle,
            intAlgHandle->privContext,
            intAlgHandle->preEmptHandle, intAlgHandle->perfSimOutput, intAlgHandle->sysScratchPtr.l2BasePtr[relativeCoreId],
            intAlgHandle->sysScratchPtr.l3BasePtr[relativeCoreId], nextExecutionLyr, intAlgHandle->createParams->net->numLayers);
            TIDL_unlockForPreemption(oldIntState, intAlgHandle->createParams, &lockState);
          }
        }
        /* Update tidlOutArgs with algLayer metaData*/
        if(tidlInArgs->enableLayerPerfTraces == 1)
        {
          /* Put this inside while loop since some layers may execute on single core in low latency mode
            Corresponding algLayer will be present only if layer executes on current core */
          tidlOutArgs->metaDataLayer[layerId].actualOps = algLayer->metaData.actualOps;
          tidlOutArgs->metaDataLayer[layerId].layerExecId = layerId;
          tidlOutArgs->metaDataLayer[layerId].totalOps = algLayer->metaData.totalOps;
          for(int32_t k = 0; k < TIDL_PROFILE_MAX; k++)
          {
            tidlOutArgs->metaDataLayer[layerId].profilePoint[k] += algLayer->metaData.profilePoint[k]; /* += for layers which have multiple workloads */
            algLayer->metaData.profilePoint[k] = 0;
          }
        }
      }
    }
  }

  /* Disable interrupts here */
  if (intAlgHandle->isPreEmptionEnable)
  {
    int32_t numWL = intAlgHandle->gcHelperHandle->pWLDB->count;
    oldIntState = TIDL_lockForPreemption(algHandle, intAlgHandle->createParams, &lockState);
    /* Recalculate pre-emption layer list using HW performance */
    TIDL_reCalculateListForPreemption((void*)intAlgHandle->preEmptHandle,
        &intAlgHandle->preemptPerfHW,
        intAlgHandle->createParams->net->numLayers,
        numWL,
        intAlgHandle->createParams->maxPreEmptDelay,
        intAlgHandle->sysScratchPtr.l2BasePtr[relativeCoreId],
        intAlgHandle->ddrBytesPerCPUCycle,
        intAlgHandle->perfSimOutput->simConfig.freqMHz,
        intAlgHandle->alglayerParams);
  }

  if(status == IALG_EOK)
  {
    if(intAlgHandle->procCallCounter < 256U)
    {
      intAlgHandle->procCallCounter++;
    }

    for (i = 0; i < (int32_t)outBufs->numBufs; i++)
    {
      sTIDL_DataParams_t *dataParams;
      dataParams = TIDL_getDataParams(intAlgHandle->createParams->net,outBufs->bufDesc[i]->bufferId);
      tidlOutArgs->scale[i] = dataParams->tensorScale;
    }
    tidlOutArgs->numOutBufs = (int32_t)outBufs->numBufs;
    tidlOutArgs->numLayers = intAlgHandle->createParams->net->numLayers;
    tidlOutArgs->privArgs  = (void*) intAlgHandle->createParams->net ;
  }

  TIDL_getTscTime(&tEndNetwork);

  if((tidlInArgs->enableLayerPerfTraces == 1) && (NULL != intAlgHandle))
  {
    /* Set layerExec Id = -1 for Data Layers */
    for(int i = 0; i < intAlgHandle->createParams->net->numLayers; i++)
    {
      if(intAlgHandle->createParams->net->TIDLLayers[i].layerType == TIDL_DataLayer)
      {
        tidlOutArgs->metaDataLayer[i].layerExecId = -1;
      }
    }
#ifdef INTERNAL_PROFILING
    TIDL_printProfileData(intAlgHandle->createParams->net->numLayers, tStartNetwork, tEndNetwork, tidlOutArgs);
#endif
  }

  /* Set earlyPreEmpt to 0 indicate that no pre-emption from TIDL Process function */
  if (intAlgHandle->isPreEmptionEnable)
  {
    intAlgHandle->preEmptContextInfo.earlyPreEmpt = 0;
    intAlgHandle->preEmptContextInfo.numContextBufs = 0;
    TIDL_unlockForPreemption(oldIntState, intAlgHandle->createParams, &lockState);
  }
  if((algHandle->createParams->net->inferenceMode == TIDL_inferenceModeLowLatency))
  {
    /* Set global sync state to TIDL_SYNC_WL_STATE_DEFAULT for next frame */
    volatile int8_t * syncPtr = (int8_t *)intAlgHandle->controlArgs.ctrlSetArgs[relativeCoreId].syncBufferPtr[TIDL_Virtual];
    memset((void *)syncPtr, TIDL_SYNC_WL_STATE_DEFAULT, TIDL_GLOBAL_SYNC_OFFSET * sizeof(int8_t));
  }

  TIDL_enableL1DandL2CacheWb();

  tidl_printf(1, "TIDL_process is completed with handle : %x \n", algHandle);

  return status;
}

/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION
@fn         TIDL_activate
@brief      This is the function to do activation of algorithm.
            All scratch memory  are restored in this
            Refer algFree function for details in ialg.h file.
@param      handle : Algorithm Instance handle
@remarks    None
@return     None
----------------------------------------------------------------------------
*/
void  TIDL_activate(IALG_Handle handle)
{
  TIDL_Handle algHandle = (TIDL_Handle)(void*)(handle);

  /*----------------------------------------------------------------*/
  /* Do Activation only if it is deactivated or just after creation */
  /*----------------------------------------------------------------*/
  int32_t status = IALG_EOK;
  if(algHandle->algState == (uint8_t)ALG_NOT_ACTIVE)
  {
    tidl_printf(1, "TIDL_activate is called with handle : %x \n", algHandle);

    /* Initialize the DMA Utils Handle */
    if((algHandle->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == TIDL_FLOW_CTRL_REF_ONLY)
    {
      algHandle->dmaUtilsContext = NULL;
    }
    else
    {
      status = TIDL_initDmaUtils(algHandle);
    }

    if(status != IALG_EOK)
    {
      tidl_printf(0, "TIDL_initDmaUtils returned Error Code for handle: %x \n", algHandle);
      return;
    }

    TIDL_Handle intAlgHandle = (TIDL_Handle)algHandle->intAlgHandle;

    status = TIDL_memcpy2D(intAlgHandle,
                  (const void  *)algHandle->memRec[ALG_HANDLE_MEMREC].base,
                  algHandle->memRec[ALG_HANDLE_MEMREC].size,
                  1U,
                  0,
                  0,
                  algHandle->dmaUtilsContext,
                  algHandle->memcpyTr);

    if(status != IALG_EOK)
    {
      tidl_printf(0, "TIDL_memcpy2D returned Error Code for handle: %x \n", algHandle);
      return;
    }

    TIDL_mmaHwaStatusWorkAround();

    /* Copy persistent weight buffers for all layers*/
    TIDL_copyPersistentWeightsToIntMem(intAlgHandle);

    /* Copy Pre-emption ContextMemory to Internal memory*/
    if (intAlgHandle->isPreEmptionEnable)
    {
      TIDL_restoreContextMemToIntMemForPreemption(&intAlgHandle->preEmptContextInfo,
        intAlgHandle->dmaUtilsContext, intAlgHandle->memcpyTr, intAlgHandle->createParams);
    }

    algHandle->algState = (uint8_t)ALG_ACTIVE;
  }

  // Map the scratch area on the GPU. All kernels accessing this
  // scratch area should be using the "data present()" clause instead
  // of copy or create.
  // ALG_REF_SCRATCH_BUFF_MEMREC=8
  #if BUILD_WITH_OPENACC // Scratch buffer has issues to be resolved
  uint8_t* scratch_buffer_base = (uint8_t*) algHandle->memRec[ALG_REF_SCRATCH_BUFF_MEMREC].base;
  uint32_t scratch_buffer_size = (uint32_t) algHandle->memRec[ALG_REF_SCRATCH_BUFF_MEMREC].size;
  OPENACC(enter data create(scratch_buffer_base[:scratch_buffer_size]))
  #endif // Scratch buffer

  // Map the coefficients memory (persistant contents) area into the GPU
  // ALG_PERSIST_DDR_NET_MEMREC = 14
  #if BUILD_WITH_OPENACC
  uint8_t* coeff_buffer_base = (uint8_t*) algHandle->memRec[ALG_PERSIST_DDR_NET_MEMREC].base;
  uint32_t coeff_buffer_size = (uint32_t) algHandle->memRec[ALG_PERSIST_DDR_NET_MEMREC].size;
  // fprintf(stderr, "mapping 0x%lx - 0x%lx host memory area to GPU (ALG_PERSIST_DDR_NET_MEMREC=%d)\n", coeff_buffer_base, coeff_buffer_base+coeff_buffer_size,ALG_PERSIST_DDR_NET_MEMREC);
  OPENACC(enter data copyin(coeff_buffer_base[:coeff_buffer_size]))
  #endif

  return;
}



/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION
@fn         TIDL_deactivate
@brief      This is the function to do de-activation of algorithm.
            All scratch memory are stoed to external memory in this
            Refer algFree function for details in ialg.h file.
@param      handle : Algorithm Instance handle
@remarks    None
@return     None
----------------------------------------------------------------------------
*/
void  TIDL_deactivate(IALG_Handle handle)
{
  TIDL_Handle algHandle = (TIDL_Handle)(void*)(handle);
  TIDL_Handle intAlgHandle = (TIDL_Handle)(void*)(algHandle->intAlgHandle);

  if(algHandle->algState == (uint8_t)ALG_ACTIVE)
  {
    tidl_printf(1, "TIDL_deactivate is called with handle : %x \n", algHandle);

    if(NULL != intAlgHandle)
    {
      TIDL_memcpy2D(intAlgHandle->memRec[ALG_HANDLE_MEMREC].base,
                  intAlgHandle,
                  intAlgHandle->memRec[ALG_HANDLE_MEMREC].size,
                  1U,
                  0,
                  0,
                  intAlgHandle->dmaUtilsContext,
                  intAlgHandle->memcpyTr);

      /* Backup Pre-emption ContextMemory to external memory*/
      if (intAlgHandle->isPreEmptionEnable)
      {
        TIDL_storeContextMemToExtMemForPreemption(&intAlgHandle->preEmptContextInfo,
          intAlgHandle->dmaUtilsContext, intAlgHandle->memcpyTr);
      }
      /* Release the dma resources*/
      if (intAlgHandle->dmaUtilsContext != NULL )
      {
        (void)DmaUtilsAutoInc3d_deinit(intAlgHandle->dmaUtilsContext);
      }
    }
    algHandle->algState = (uint8_t)ALG_NOT_ACTIVE;

    TIDL_enableL1DandL2CacheWb();
  }

  // Unmap the accPtr scratch area from the GPU.
  // ALG_REF_SCRATCH_BUFF_MEMREC=8
  #if BUILD_WITH_OPENACC // Scratch buffer has issues to be resolved
  if(algHandle->algState == (uint8_t)ALG_ACTIVE)
  {
    uint8_t* scratch_buffer_base = (uint8_t*) algHandle->memRec[ALG_REF_SCRATCH_BUFF_MEMREC].base;
    uint32_t scratch_buffer_size = (uint32_t) algHandle->memRec[ALG_REF_SCRATCH_BUFF_MEMREC].size;
    OPENACC(exit data delete(scratch_buffer_base[:scratch_buffer_size]))
  }
  #endif // Scratch buffer

  // Unmap the coefficients area from GPU.
  // ALG_PERSIST_DDR_NET_MEMREC = 14
  #if BUILD_WITH_OPENACC
  if(algHandle->algState == (uint8_t)ALG_ACTIVE)
  {
    uint8_t* coeff_buffer_base = (uint8_t*) algHandle->memRec[ALG_PERSIST_DDR_NET_MEMREC].base;
    uint32_t coeff_buffer_size = (uint32_t) algHandle->memRec[ALG_PERSIST_DDR_NET_MEMREC].size;
    OPENACC(exit data delete(coeff_buffer_base[:coeff_buffer_size]))
  }
  #endif

  return;
}

/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION
@fn         TIDL_free
@brief      This is the function which user can call to get back
            the memory given to algo.
            Refer algFree function for details in ialg.h file.
@param      handle : Algorithm Instance handle
@param      memTab : memory table populated by the Algorithm
@remarks    None
@return     IALG_EOK   - Successful
            IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/
int32_t TIDL_free(IALG_Handle handle, IALG_MemRec *memRec)
{
  int32_t status            = IALG_EOK;
  TIDL_Handle algHandle = (TIDL_Handle)(void*)(handle);
  int32_t relativeCoreId = GET_RELATIVE_COREIDX(algHandle->createParams->coreId, algHandle->createParams->coreStartIdx);

  /* Removed TIDL_deactivate from test bench and added here to release DMA resources before freeing handle */
  TIDL_deactivate((IALG_Handle)handle);

  /* Since we are freeing this handle, we need to set TIDL_lastActiveHandle to NULL if it matches,
   * so that a deactivate is not called by the preemption logic later */
  if (TIDL_getLastActiveHandle(relativeCoreId) == handle)
  {
    TIDL_setLastActiveHandle(relativeCoreId, NULL);
  }

  /* For stats collection update the final calculated tensor scales which should be used during
  inference */
  if ((algHandle->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_STAT) == TIDL_FLOW_CTRL_REF_STAT)
  {
    if (algHandle->createParams->quantRangeUpdateFactor == 0.0f)
    {
      int32_t layerIdx;
      for ( layerIdx = 0; layerIdx < algHandle->createParams->net->numLayers; layerIdx++)
      {
        TIDL_UpdateScaleFactors(algHandle, layerIdx, 0, 0, 0);
      }
    }
  }
  (void)memcpy(memRec, algHandle->memRec, sizeof(IALG_MemRec)*algHandle->numMemRecs);

  status = TIDL_removePriorityObject(algHandle, &algHandle->memRec[ALG_CONTEXT_MEM_MEMREC]);
#ifdef BUILD_WITH_CUDA
  TIDL_cudaFreeConvCudaPtrs();
#endif
  return status;
}

