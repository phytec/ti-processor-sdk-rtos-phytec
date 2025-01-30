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
 *  \file tidl_pooling.h
 *
 *  \brief This file defines the process function prototype of pooling layer
 */

#ifndef TIDL_POOLING_H_
#define TIDL_POOLING_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "tidl_alg_int.h"
#include "tidl_alg_utils.h"
#include "itidl_ti.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 @struct  TIDL_SpatialPoolingBuffParams_t
 @brief   This structure contains buffer parameters used for transferring input
          from external memory to internal memory for computation and for
          transferring output from internal memory to external memory after
          computation.

 @params  inWidth
          Width of input data pointer in external memory.

 @params  inHeight
          Height of input data pointer in external memory.

 @params  inPitch
          Pitch of input data in external memory.

 @params  outPitch
          Pitch of output data in external memory.

 @params  inChPitch
          Channel Pitch of input data in external memory. It contains the size
          of one complete channel data.

 @params  outChPitch
          Channel Pitch of output data in external memory. It contains the size
          of one complete channel output data.

 @params  scaleValue
          scaleValue used for reciprocal multiplication in Average pooling
          operations.

 @params  roundBits
          Round bits used for final output in pooling operations.

 @params  inElementType
          Indicates the type of input as signed or unsigned.

 @param  scratchSize
         Size of scratch memory.

 @param  scratchMem
         Pointer to the scratch memory.
*/
typedef struct {
  int32_t inWidth;
  int32_t inHeight;
  int32_t inPitch;
  int32_t outPitch;
  int32_t inChPitch;
  int32_t outChPitch;
  int32_t scaleValue;
  int32_t biasTerm;
  uint8_t mmaShift;
  int32_t roundBits;
  int32_t inElementType;
  int32_t outElementType;
  int32_t scratchSize;
  void *scratchMem;
} TIDL_SpatialPoolingBuffParams_t;

/* ========================================================================== */
/*                  Internal/Private Function Declarations                    */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

int32_t TIDL_poolingDspProcess(TIDL_Handle        intAlgHandle,
                            sTIDL_AlgLayer_t     *algLayer,
                            sTIDL_Layer_t        *tidlLayer,
                            void                 *inPtrs[],
                            void                 *outPtrs[],
                            sTIDL_sysMemHandle_t *sysMems);



int32_t TIDL_poolingDspAlloc(const TIDL_LayerSpecificParams *layerSpecificParams,
                             const TIDL_NetworkCommonParams *commonParams,
                             int32_t layerIdx,
                             int32_t memorySize[TIDL_LAYER_MEMORY_MAX]);

int32_t TIDL_poolingDspInit(const TIDL_LayerSpecificParams *layerSpecificParams,
                                const TIDL_NetworkCommonParams  *commonParams,
                                sTIDL_AlgLayer_t               *algLayer,
                                int32_t layerIdx,
                                uint8_t *memory[TIDL_LAYER_MEMORY_MAX],
                                int32_t memorySize[TIDL_LAYER_MEMORY_MAX],
                                void    **outPtr);

int32_t TIDL_poolingDspProcessNew(TIDL_NetworkCommonParams *commonParams,
                              sTIDL_AlgLayer_t     * algLayer,
                              sTIDL_Layer_t        * tidlLayer,
                              void                 * inPtrs[],
                              void                 * outPtrs[],
                              int32_t                layerIdx);



int32_t TIDL_poolingAllocNew(const TIDL_LayerSpecificParams *layerSpecificParams,
                             const TIDL_NetworkCommonParams *commonParams,
                             int32_t layerIdx,
                             int32_t memorySize[TIDL_LAYER_MEMORY_MAX]);

int32_t TIDL_poolingInitNew(const TIDL_LayerSpecificParams *layerSpecificParams,
                                const TIDL_NetworkCommonParams  *commonParams,
                                sTIDL_AlgLayer_t               *algLayer,
                                int32_t layerIdx,
                                uint8_t *memory[TIDL_LAYER_MEMORY_MAX],
                                int32_t memorySize[TIDL_LAYER_MEMORY_MAX],
                                void    **outPtr);

int32_t TIDL_poolingProcessNew(TIDL_NetworkCommonParams *commonParams,
                              sTIDL_AlgLayer_t     * algLayer,
                              sTIDL_Layer_t        * tidlLayer,
                              void                 * inPtrs[],
                              void                 * outPtrs[],
                              int32_t                layerIdx);


//:TODO: See if we should create seaprate header for device specific functions
int32_t TIDL_poolingDeviceSetupKernelHandle(void * handle,
                                            const TIDL_LayerInitParams *initParams,
                                            const sLink_t              *link,
                                            const sGCHelperHandle * gcHelperHandle,
                                            TIDL_bufParams3D_t         *srcAddr,
                                            TIDL_bufParams3D_t         *dstAddr,
                                            int32_t                     funcStyle);

int32_t TIDL_poolingDeviceGetHandleSize(void *linkInitParams,
                                         const sLink_t              *link,
                                         const sGCHelperHandle * gcHelperHandle);

int32_t TIDL_poolingDeviceInit(void                                *linkHandle,
                                     void                                *linkInitParams,
                                     const WorkloadUnitExec_CommonParams *commonParams,
                                     const sLink_t                       *link,
                                     const sGCHelperHandle *gcHelperHandle);

int32_t TIDL_poolingDeviceExec(void                            *linkHandle,
                               const WorkloadUnitExec_LinkExecArgs *linkExecArgs,
                               int32_t                          currFlowStage[],
                               int32_t                          currIterCount);



/* ========================================================================== */
/*                       Static Function Definitions                          */
/* ========================================================================== */

/* None */

#endif /* TIDL_POOLING_H_*/

