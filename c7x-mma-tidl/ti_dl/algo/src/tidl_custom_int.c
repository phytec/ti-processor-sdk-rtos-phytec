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
 *  \file tidl_custom_int.c
 *
 *  \brief This file contains glue layer for custom layer before calling user
 *         defined APIs
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "tidl_types.h"
#include "tidl_custom_int.h"
#include "../../custom/tidl_custom.h"
#include "tidl_commonUtils.h"

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

/* None */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int32_t TIDL_customProcessNew(TIDL_NetworkCommonParams *commonParams,
                           sTIDL_AlgLayer_t     *algLayer,
                           sTIDL_Layer_t        *tidlLayer,
                           void                 *inPtrs[],
                           void                 *outPtrs[],
                           int32_t              layerIdx)
{
  int32_t status = TIDL_SUCCESS;
  TIDL_CreateParams *createParams = (TIDL_CreateParams*)commonParams->createParams;
 
  sTIDL_sysMemHandle_t *sysMems = commonParams->tidlCommonParams->sysMems;
  int32_t i;
  sTIDL_AlgCustomParams_t* algCustomParams = &algLayer->layerParams.customParams;
  void *params = algCustomParams->paramPtr;
  sPerfSim_t *perfInfoOut = NULL;
  sTIDL_sysMemHandle_t sysMemsLocal[TIDL_SYSMEM_MAX];
  int32_t execMode;

  TIDL_customProcessSpecific customProcessSpecific;
  customProcessSpecific.commonParam = commonParams;
  customProcessSpecific.algLayer = (void *)algLayer;
  customProcessSpecific.tidlLayer = (void *)tidlLayer;

  if((createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == TIDL_FLOW_CTRL_REF_ONLY)
  {
    for(i=0; i<TIDL_SYSMEM_MAX; i++)
    {
      memset(&sysMemsLocal[i], 0, sizeof(sTIDL_sysMemHandle_t));
    }
  }

  /* Get perfInfoOut */
  if(NULL != createParams->net->dataFlowInfo)
  {
    perfInfoOut = (sPerfSim_t *)get_int8_t_pointer((int8_t *)(createParams->net), createParams->net->dataFlowInfo);
  }

  /* Make sure inData has been copied from previous layer's outData */
  tidlLayer->inDataPtr = (sTIDL_DataParams_t *)TIDL_getMemoryChunkFromSysmem(sysMems,
                                                  (uint32_t)tidlLayer->numInBufs* sizeof(sTIDL_DataParams_t),
                                                  128U,
                                                  (uint32_t)TIDL_SYSMEM_L3_SCRATCH,
                                                  (uint32_t)IALG_SCRATCH);
  for(i=0; i<tidlLayer->numInBufs; i++)
  {
    sTIDL_DataParams_t *inDataParams = TIDL_getDataParams(createParams->net,
                                                          tidlLayer->inData[i]);
    (void) memcpy(&tidlLayer->inDataPtr[i], inDataParams, sizeof(sTIDL_DataParams_t));
  }

  if(NULL != commonParams->gcHelperHandle) 
  {
    /* Updating sysMemHandle to provide a simplified view to user */
    /* For L1, base will remain same, offset will be 0 and size is 16 KB */
    sysMemsLocal[TIDL_SYSMEM_L1_SCRATCH].base   = (sysMems + TIDL_SYSMEM_L1_SCRATCH)->base;
    sysMemsLocal[TIDL_SYSMEM_L1_SCRATCH].size   = TIDL_DEFAULT_L1_MEM_SIZE;
    sysMemsLocal[TIDL_SYSMEM_L1_SCRATCH].offset = 0;

    /* For L2, base will remain same, offset will be 0 and size is
     * perfInfoOut->simConfig.sizeL2MemKB - SCRATCH_SIZE
     */
    sysMemsLocal[TIDL_SYSMEM_L2_SCRATCH].base   = (sysMems + TIDL_SYSMEM_L2_SCRATCH)->base;
    sysMemsLocal[TIDL_SYSMEM_L2_SCRATCH].size   = (perfInfoOut->simConfig.sizeL2MemKB - L2MEM_SCRATCH_BUF_SIZEKB)*1024 ;
    sysMemsLocal[TIDL_SYSMEM_L2_SCRATCH].offset = 0;

    /* For L3, base will be sysMems.base + sysMems.offset, offset will be 0 and
     * size will be sysMems.size - sysMems.offset
     */
    sysMemsLocal[TIDL_SYSMEM_L3_SCRATCH].base = (void *)((uintptr_t)((sysMems + TIDL_SYSMEM_L3_SCRATCH)->base) + (uintptr_t)((sysMems + TIDL_SYSMEM_L3_SCRATCH)->offset));
    sysMemsLocal[TIDL_SYSMEM_L3_SCRATCH].size = (sysMems + TIDL_SYSMEM_L3_SCRATCH)->size - (sysMems + TIDL_SYSMEM_L3_SCRATCH)->offset;
    sysMemsLocal[TIDL_SYSMEM_L3_SCRATCH].offset = 0;

    /* For DDR, use ALG_SCRATCH_DDR_MEM_MEMREC parameters */
    // sysMemsLocal[TIDL_SYSMEM_DDR_SCRATCH].base = intAlgHandle->memRec[ALG_SCRATCH_DDR_MEM_MEMREC].base;
    // sysMemsLocal[TIDL_SYSMEM_DDR_SCRATCH].size = intAlgHandle->memRec[ALG_SCRATCH_DDR_MEM_MEMREC].size;
    // sysMemsLocal[TIDL_SYSMEM_DDR_SCRATCH].offset = 0;
  }

  if((createParams->flowCtrl & TIDL_FLOW_CTRL_REF_STAT) == TIDL_FLOW_CTRL_REF_STAT)
  {
    execMode = TIDL_EXEC_MODE_STATS_COLLECTION;
  }
  else if ((createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == TIDL_FLOW_CTRL_REF_ONLY)
  {
    execMode = TIDL_EXEC_MODE_INFER_PROCESS_REF;
  }
  else
  {
    execMode = TIDL_EXEC_MODE_INFER_PROCESS;
  }


  status = createParams->TIDL_CustomLayerProcess(&customProcessSpecific,
                                                 tidlLayer,
                                                 inPtrs,
                                                 outPtrs,
                                                 params,
                                                 commonParams->tidlCommonParams->dmaUtilsContext,
                                                 (const sTIDL_sysMemHandle_t *)sysMemsLocal,
                                                 execMode);

  return status;
}

int32_t TIDL_customAllocNew(const TIDL_LayerSpecificParams *layerSpecificParams,
                             const TIDL_NetworkCommonParams *commonParams,
                             int32_t layerIdx,
                             int32_t memorySize[TIDL_LAYER_MEMORY_MAX])
{
  int32_t status = IALG_EOK;

  if ((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == TIDL_FLOW_CTRL_REF_ONLY)
  {
    status = TIDL_deviceCommonRefAlloc(layerSpecificParams, commonParams, layerIdx, memorySize);
  }

  memorySize[TIDL_LAYER_MEMORY_SCRATCH]    += 0;
  memorySize[TIDL_LAYER_MEMORY_PERSISTENT] += ((uint32_t)TIDL_CUSTOM_LAYER_DDR_SIZE + 128U);
  memorySize[TIDL_LAYER_MEMORY_OUTPUT]     += 0;

  { //Add custom layer learnable parameters to this
    sTIDL_CustomParams_t *layerParams = &commonParams->createParams->net->TIDLLayers[layerIdx].layerParams.customParams;
    int32_t paramsize = layerParams->rsvdPassThroughSize ;
    memorySize[TIDL_LAYER_MEMORY_PERSISTENT] += ((uint32_t)paramsize + 128U);
  }
  return status;
}

int32_t TIDL_customInitNew(const TIDL_LayerSpecificParams *layerSpecificParams,
                                const TIDL_NetworkCommonParams  *commonParams,
                                sTIDL_AlgLayer_t               *algLayer,
                                int32_t layerIdx,
                                uint8_t *memory[TIDL_LAYER_MEMORY_MAX],
                                int32_t memorySize[TIDL_LAYER_MEMORY_MAX],
                                void    **outPtr)
{
  int32_t status = IALG_EOK;
  void *dst;
  int32_t currOffset = 0 ;

  if ((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == TIDL_FLOW_CTRL_REF_ONLY)
  {
    status = TIDL_deviceCommonRefInit(layerSpecificParams,
                              commonParams,
                              algLayer,
                              layerIdx,
                              memory,
                              memorySize,
                              outPtr);
  } 

  if (status == IALG_EOK)
  { 
    sTIDL_CustomParams_t *layerParams = &commonParams->createParams->net->TIDLLayers[layerIdx].layerParams.customParams;
    int32_t paramSize   = layerParams->rsvdPassThroughSize ;
    int32_t paramOffset = layerParams->rsvdPassThrough ;
    sTIDL_AlgCustomParams_t* algCustomParams = &algLayer->layerParams.customParams;

    algLayer->dataFlowMemSize  = TIDL_CUSTOM_LAYER_DDR_SIZE ;
    TIDL_AllocatePtr((intptr_t)memory[TIDL_LAYER_MEMORY_PERSISTENT],
                      &currOffset,
                      algLayer->dataFlowMemSize,
                      TIDL_ALIGNMENT_SIZE,
                      &dst);
    algLayer->dataFlowMemPtr   = (uint8_t *)dst;

    //Copy the learnable or custom layer specific parameters to layer properties
    TIDL_AllocatePtr((intptr_t)memory[TIDL_LAYER_MEMORY_PERSISTENT],
                      &currOffset,
                      paramSize,
                      TIDL_ALIGNMENT_SIZE,
                      &dst);

    algCustomParams->paramPtr = dst ;
    algCustomParams->paramSize = paramSize ;
    if(paramSize && paramOffset){
      void * src   = (void *)((int8_t *)(commonParams->createParams->net) + paramOffset);
      memcpy(dst, src, paramSize);
    }
    memorySize[TIDL_LAYER_MEMORY_SCRATCH]    += 0;
    memorySize[TIDL_LAYER_MEMORY_PERSISTENT] += ((uint32_t)currOffset + 128U);
    memorySize[TIDL_LAYER_MEMORY_OUTPUT]     += 0;
  }

  return status;
}
/* ========================================================================== */
/*                       Static Function Definitions                          */
/* ========================================================================== */

/* None */
