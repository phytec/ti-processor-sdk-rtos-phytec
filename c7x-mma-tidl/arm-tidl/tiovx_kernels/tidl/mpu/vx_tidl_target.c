/*
*
* Copyright (c) 2023 Texas Instruments Incorporated
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

#include <stdio.h>
#include <TI/tivx.h>
#include <TI/dl_kernels.h>
#include <TI/j7_tidl.h>
#include <TI/tivx_target_kernel.h>
#include <tivx_alg_ivision_if.h>
#include "itidl_ti.h"
#include "tivx_tidl_trace.h"
#include "tivx_kernels_target_utils.h"
#include "app_fileio.h"

/* #define TIVX_TIDL_MPU_TARGET_DEBUG */

typedef struct
{
    vx_user_data_object tidlNodeParams[TIVX_KERNEL_TIDL_NUM_BASE_PARAMETERS];
    vx_tensor  inputTensors[TIDL_MAX_NUM_CORES][TIDL_MAX_ALG_IN_BUFS];
    vx_tensor  outputTensors[TIDL_MAX_NUM_CORES][TIDL_MAX_ALG_OUT_BUFS];
    vx_user_data_object outArgs[TIDL_MAX_NUM_CORES];

    uint32_t num_input_tensors;
    uint32_t num_output_tensors;
    uint32_t num_virtual_cores;
    uint32_t num_cores;
    int32_t  core_start_idx;
    uint32_t num_super_batches;
    uint32_t inference_mode;
    uint32_t trace_log_level;

    vx_graph graph;
    vx_node node[TIDL_MAX_NUM_CORES];
    vx_kernel kernel;

    void     *addrInput[TIDL_MAX_NUM_CORES][TIDL_MAX_ALG_IN_BUFS][MAX_TENSOR_DIMS];
    uint32_t sizeInput[TIDL_MAX_NUM_CORES][TIDL_MAX_ALG_IN_BUFS][MAX_TENSOR_DIMS];
    uint32_t numEntriesInput[TIDL_MAX_NUM_CORES][TIDL_MAX_ALG_IN_BUFS];

    void     *addrOutput[TIDL_MAX_NUM_CORES][TIDL_MAX_ALG_OUT_BUFS][MAX_TENSOR_DIMS];
    uint32_t sizeOutput[TIDL_MAX_NUM_CORES][TIDL_MAX_ALG_OUT_BUFS][MAX_TENSOR_DIMS];
    uint32_t numEntriesOutput[TIDL_MAX_NUM_CORES][TIDL_MAX_ALG_OUT_BUFS];

} tivxTIDLNestedKernelObj;

static const uint32_t max_entries = MAX_TENSOR_DIMS;

/* Target names used to register current target */
static char target_name[][TIVX_TARGET_MAX_NAME] =
{
    TIVX_TARGET_MPU_1,
    TIVX_TARGET_MPU_2,
    TIVX_TARGET_MPU_3,
};

/* Target names for nodes created on current target */
static const char* child_target_name[] = {
            TIVX_TARGET_DSP_C7_1_PRI_1,
            TIVX_TARGET_DSP_C7_1_PRI_2,
            TIVX_TARGET_DSP_C7_1_PRI_3,
            TIVX_TARGET_DSP_C7_1_PRI_4,
            TIVX_TARGET_DSP_C7_1_PRI_5,
            TIVX_TARGET_DSP_C7_1_PRI_6,
            TIVX_TARGET_DSP_C7_1_PRI_7,
            TIVX_TARGET_DSP_C7_1_PRI_8,
#if defined(SOC_J784S4) || defined(SOC_J722S)
            TIVX_TARGET_DSP_C7_2_PRI_1,
            TIVX_TARGET_DSP_C7_2_PRI_2,
            TIVX_TARGET_DSP_C7_2_PRI_3,
            TIVX_TARGET_DSP_C7_2_PRI_4,
            TIVX_TARGET_DSP_C7_2_PRI_5,
            TIVX_TARGET_DSP_C7_2_PRI_6,
            TIVX_TARGET_DSP_C7_2_PRI_7,
            TIVX_TARGET_DSP_C7_2_PRI_8,
#endif
#if defined(SOC_J784S4)
            TIVX_TARGET_DSP_C7_3_PRI_1,
            TIVX_TARGET_DSP_C7_3_PRI_2,
            TIVX_TARGET_DSP_C7_3_PRI_3,
            TIVX_TARGET_DSP_C7_3_PRI_4,
            TIVX_TARGET_DSP_C7_3_PRI_5,
            TIVX_TARGET_DSP_C7_3_PRI_6,
            TIVX_TARGET_DSP_C7_3_PRI_7,
            TIVX_TARGET_DSP_C7_3_PRI_8,
            TIVX_TARGET_DSP_C7_4_PRI_1,
            TIVX_TARGET_DSP_C7_4_PRI_2,
            TIVX_TARGET_DSP_C7_4_PRI_3,
            TIVX_TARGET_DSP_C7_4_PRI_4,
            TIVX_TARGET_DSP_C7_4_PRI_5,
            TIVX_TARGET_DSP_C7_4_PRI_6,
            TIVX_TARGET_DSP_C7_4_PRI_7,
            TIVX_TARGET_DSP_C7_4_PRI_8,
#endif
    };

#define TIDL_MAX_TARGETS (sizeof(target_name)/sizeof(target_name[0]))

static tivx_target_kernel vx_tidl_nested_target_kernel[TIDL_MAX_TARGETS] = {NULL};

/* OpenVX Node callbacks */
static vx_status VX_CALLBACK tivxKernelTIDLCreate(tivx_target_kernel_instance kernel,
  tivx_obj_desc_t *obj_desc[], uint16_t num_params, void *priv_arg);
static vx_status VX_CALLBACK tivxKernelTIDLProcess(tivx_target_kernel_instance kernel,
    tivx_obj_desc_t *obj_desc[], uint16_t num_params, void *priv_arg);
static vx_status VX_CALLBACK tivxKernelTIDLDelete(tivx_target_kernel_instance kernel,
    tivx_obj_desc_t *obj_desc[], uint16_t num_params, void *priv_arg);

static vx_status copyTensorHandles(vx_tensor src, vx_tensor dst);

/* Copy data pointer of the source vx_tensor data object to destination vx_tensor object*/
static vx_status copyTensorHandles(vx_tensor src, vx_tensor dst)
{
    vx_status status = VX_SUCCESS;
    void            *addr[MAX_TENSOR_DIMS] = {NULL};
    uint32_t        size[MAX_TENSOR_DIMS];
    uint32_t        num_entries;

    status = tivxReferenceExportHandle((vx_reference)src,
                                         addr,
                                         size,
                                         MAX_TENSOR_DIMS,
                                         &num_entries);
    if(status==VX_SUCCESS)
    {
        status = tivxReferenceImportHandle((vx_reference)dst,
                                           (const void **)addr,
                                           (const uint32_t *)size,
                                           num_entries);
    }

    return status;
}

/* Updates outArgs performance profile points to report MAX latency across all cores */
static vx_status tivxUpdateOutArgs(tivxTIDLNestedKernelObj * prms, tivx_obj_desc_t *obj_desc[])
{
    vx_status status = (vx_status)VX_SUCCESS;

    vx_context context = vxCreateContext();
    vx_user_data_object outArgs = (vx_user_data_object)obj_desc[TIVX_KERNEL_TIDL_IN_OUT_ARGS_IDX]->host_ref;

    /* Map copy of outArgs */
    vx_map_id map_id_out_args, map_id_prms_out_args;
    TIDL_outArgs *outArgsBuffer = NULL, *prmsOutArgsBuffer = NULL;
    status = vxMapUserDataObject(outArgs, 0, sizeof(TIDL_outArgs), &map_id_out_args,
                                    (void **)&outArgsBuffer, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0);

    if(status == (vx_status)VX_SUCCESS)
    {
        memset(outArgsBuffer, 0, sizeof(TIDL_outArgs)); /* Initialize all data values in outArgs to 0 */
        
        for(int i = 0; i < prms->num_cores; i++)
        {
            if(status == (vx_status)VX_SUCCESS)
            {
                status = vxMapUserDataObject(prms->outArgs[i], 0, sizeof(TIDL_outArgs), &map_id_prms_out_args,
                                                (void **)&prmsOutArgsBuffer, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0);
                
            }
            if(status == (vx_status)VX_SUCCESS)
            {
                if(i == 0)
                {
                    /* outArgs from RT application obj_desc is not populated yet, so copy over first DSP node's outArgs, and then update profile points later based on all cores */
                    memcpy(outArgsBuffer, prmsOutArgsBuffer, sizeof(TIDL_outArgs));
                }

                for(int j = 0; j < prmsOutArgsBuffer->numLayers; j++)
                {
                    for(int k = 0; k < TIDL_PROFILE_MAX; k++)
                    {
                        /* MAX of profile points across all cores */
                        outArgsBuffer->metaDataLayer[j].profilePoint[k] = 
                            (outArgsBuffer->metaDataLayer[j].profilePoint[k] > prmsOutArgsBuffer->metaDataLayer[j].profilePoint[k])
                            ?  outArgsBuffer->metaDataLayer[j].profilePoint[k] : prmsOutArgsBuffer->metaDataLayer[j].profilePoint[k];
                    }
                }
                vxUnmapUserDataObject(prms->outArgs[i], map_id_prms_out_args);
            }
        }
        vxUnmapUserDataObject(outArgs, map_id_out_args);
    }
    vxReleaseContext(&context);
    return status;
}


static vx_status VX_CALLBACK tivxKernelTIDLProcess
(
    tivx_target_kernel_instance kernel,
    tivx_obj_desc_t *obj_desc[],
    uint16_t num_params,
    void *priv_arg
)
{
    vx_status status = VX_SUCCESS;

    VX_PRINT(VX_ZONE_INFO, "MPU_1 tivxKernelTIDLProcess - numParams = %d \n", num_params);

    tivxTIDLNestedKernelObj *prms = NULL;

    if(status==VX_SUCCESS)
    {
        uint32_t size;

        status = tivxGetTargetKernelInstanceContext(kernel,
            (void **)&prms, &size);
        if ((VX_SUCCESS != status) || (NULL == prms) ||
            (sizeof(tivxTIDLNestedKernelObj) != size))
        {
            status = VX_FAILURE;
        }
    }
    
    /* Update the input/output references in graph with host pointers from application before processing */
    if(status==VX_SUCCESS)
    {
        if(prms->inference_mode == TIDL_inferenceModeHighThroughput)
        {
            int32_t numBatches = (int32_t)(prms->num_virtual_cores) * (int32_t)(prms->num_super_batches);
            int32_t numInputsPerCore = prms->num_input_tensors/numBatches;
            int32_t numOutputsPerCore = prms->num_output_tensors/numBatches;
            for(int k = 0; k < prms->num_super_batches; k++)
            {
                for(int j = 0; j < prms->num_virtual_cores; j++)
                {
                    for(int i = 0; i < numInputsPerCore; i++)
                    {
                        status |= copyTensorHandles((vx_tensor)obj_desc[TIVX_KERNEL_TIDL_IN_FIRST_TENSOR + k * prms->num_virtual_cores * numInputsPerCore + j * numInputsPerCore + i]->host_ref,
                                            prms->inputTensors[j][i]);
                    }
                    for(int i = 0; i < numOutputsPerCore; i++)
                    {
                        status |= copyTensorHandles((vx_tensor)obj_desc[TIVX_KERNEL_TIDL_IN_FIRST_TENSOR + prms->num_input_tensors + k * prms->num_virtual_cores * numOutputsPerCore + j * numOutputsPerCore + i]->host_ref,
                                            prms->outputTensors[j][i]);
                    }
                }
                if(status==VX_SUCCESS)
                {
                    VX_PRINT(VX_ZONE_INFO, "MPU_1 vxProcessGraph call to DSP target - super batch - %d \n", prms->num_super_batches);
                    status = vxProcessGraph(prms->graph);  /* Run graph processing num_super_batches times to infer all batches */
                }
                if(status != VX_SUCCESS)
                {
                    break;
                }
            }
        }
        else if(prms->inference_mode == TIDL_inferenceModeLowLatency)
        {
            /* Copy over the input and output tensor pointers from application to all nodes - TIDL expects single input buffer and 
            writes single output buffer irrespective of number of cores used for processing in this mode  */
            for(int j = 0; j < prms->num_cores; j++)
            {
                if(prms->num_cores <= TIDL_MAX_NUM_CORES) /* To avoid -Werror=array-bounds warning*/
                {
                    for(int i = 0; i < prms->num_input_tensors/(int32_t)(prms->num_virtual_cores); i++)
                    {
                        vx_tensor input = (vx_tensor)obj_desc[TIVX_KERNEL_TIDL_IN_FIRST_TENSOR + i]->host_ref;
                        status |= copyTensorHandles((vx_tensor)input, prms->inputTensors[j][i]);
                    }
                    for(int i = 0; i < prms->num_output_tensors/(int32_t)(prms->num_virtual_cores); i++)
                    {
                        vx_tensor output = (vx_tensor)obj_desc[TIVX_KERNEL_TIDL_IN_FIRST_TENSOR + prms->num_input_tensors + i]->host_ref;
                        status |= copyTensorHandles((vx_tensor)output, prms->outputTensors[j][i]);
                    }
                }
            }
        }
    }

    if((status==VX_SUCCESS) && (prms->inference_mode != TIDL_inferenceModeHighThroughput))
    {
        /* TIDL_inferenceModeHighThroughput processing called multiple times as part of above conditions */
        VX_PRINT(VX_ZONE_INFO, "MPU_1 vxProcessGraph call to DSP target \n");
        status = vxProcessGraph(prms->graph);
    }

    if(prms->trace_log_level > 0)
    {
        if(status == (vx_status)VX_SUCCESS)
        {
            status = tivxUpdateOutArgs(prms, obj_desc);
        }
    }

    return status;
}

static vx_status tivxGetDspPtrsCmd(tivxTIDLNestedKernelObj * prms, vx_user_data_object controlGetArgs, vx_user_data_object * controlSetArgs)
{
    vx_status status = (vx_status)VX_SUCCESS;

    vx_map_id  map_id_get, map_id_set;
    void *controlGetArgsBuffer = NULL;
    void *controlSetArgsBuffer = NULL;
    status = vxMapUserDataObject(controlGetArgs, 0, sizeof(TIDL_controlGetArgs), &map_id_get,
                                    (void **)&controlGetArgsBuffer, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0);
    status |= vxMapUserDataObject(*controlSetArgs, 0, sizeof(TIDL_controlSetArgs), &map_id_set,
                                    (void **)&controlSetArgsBuffer, VX_READ_AND_WRITE, VX_MEMORY_TYPE_HOST, 0);
    
    if(status == (vx_status)VX_SUCCESS)
    {
        for(int i = 0; i < prms->num_cores; i++)
        {
            vx_reference refs[2];
            refs[0] = (vx_reference)(controlGetArgs);
            
            status = tivxNodeSendCommand(prms->node[i], 0, TIVX_TIDL_CMD_GET_C7X_PTRS, refs, 1u);

            TIDL_controlGetArgs * getArgs = (TIDL_controlGetArgs *)(controlGetArgsBuffer);
            TIDL_controlSetArgs * setArgs = (TIDL_controlSetArgs *)(controlSetArgsBuffer);
            setArgs->ctrlSetArgs[i].syncBufferPtr[TIDL_Virtual] = getArgs->ctrlGetArgs.syncBufferPtr[TIDL_Virtual];
            setArgs->ctrlSetArgs[i].syncBufferPtr[TIDL_Physical] = getArgs->ctrlGetArgs.syncBufferPtr[TIDL_Physical];
            setArgs->ctrlSetArgs[i].ncScratchPtr[TIDL_Physical][TIDL_L1] = getArgs->ctrlGetArgs.ncScratchPtr[TIDL_Physical][TIDL_L1];
            setArgs->ctrlSetArgs[i].ncScratchPtr[TIDL_Physical][TIDL_L2] = getArgs->ctrlGetArgs.ncScratchPtr[TIDL_Physical][TIDL_L2];
            setArgs->ctrlSetArgs[i].ncScratchPtr[TIDL_Physical][TIDL_MSMC] = getArgs->ctrlGetArgs.ncScratchPtr[TIDL_Physical][TIDL_MSMC];
            setArgs->ctrlSetArgs[i].ncScratchPtr[TIDL_Physical][TIDL_DDR] = getArgs->ctrlGetArgs.ncScratchPtr[TIDL_Physical][TIDL_DDR];
            setArgs->ctrlSetArgs[i].ncScratchPtr[TIDL_Physical][TIDL_DDR_PERSIST] = getArgs->ctrlGetArgs.ncScratchPtr[TIDL_Physical][TIDL_DDR_PERSIST];
        }

        if(status == VX_SUCCESS)
        {
            vxUnmapUserDataObject(controlGetArgs, map_id_get);
            vxUnmapUserDataObject(*controlSetArgs, map_id_set);
        }
    }
    return status;
}

static vx_status tivxSetDspPtrsCmd(tivxTIDLNestedKernelObj * prms, vx_user_data_object argsControl)
{
    vx_status status = (vx_status)VX_SUCCESS;

    for(int i = 0; i < prms->num_cores; i++)
    {
        vx_reference refs[2];
        refs[0] = (vx_reference)argsControl;
        status |= tivxNodeSendCommand(prms->node[i], 0, TIVX_TIDL_CMD_SET_C7X_PTRS, refs, 1u);
    }
    return status;
}

static vx_status tivxInitCmd(tivxTIDLNestedKernelObj * prms)
{
    vx_status status = (vx_status)VX_SUCCESS;
    for(int i = 0; i < prms->num_cores; i++)
    {
        status |= tivxNodeSendCommand(prms->node[i], 0, TIVX_TIDL_CMD_INIT, NULL, 0u);
    }
    return status;
}

static vx_status VX_CALLBACK tivxKernelTIDLCreate
(
  tivx_target_kernel_instance kernel,
  tivx_obj_desc_t *obj_desc[],
  uint16_t num_params,
  void *priv_arg
)
{
    #ifdef TIVX_TIDL_MPU_TARGET_DEBUG
    tivx_set_debug_zone(VX_ZONE_INFO);
    #endif

    vx_status status = (vx_status)VX_SUCCESS;
    uint32_t i;
    vx_reference nestedNodeParams[16];

    VX_PRINT(VX_ZONE_INFO, "MPU_1 tivxKernelTIDLCreate - num_params = %d\n", num_params);

    for (i = 0U; i < num_params; i ++)
    {
        if (NULL == obj_desc[i] && i != TIVX_KERNEL_TIDL_IN_TRACE_DATA_IDX) /* i = 5 corresponds to trace data dump which can be NULL */
        {
            status = VX_FAILURE;
            break;
        }
    }

    if (VX_SUCCESS == status)
    {
        tivxTIDLNestedKernelObj * prms = NULL;
        vx_context context;

        prms = (tivxTIDLNestedKernelObj *)tivxMemAlloc(sizeof(tivxTIDLNestedKernelObj), TIVX_MEM_EXTERNAL);

        if(prms == NULL)
        {
            status = VX_FAILURE;
        }
        
        /* Create nested graph */
        if(status == VX_SUCCESS)
        {
            context = vxCreateContext();
            prms->graph = vxCreateGraph(context);
            status = vxGetStatus((vx_reference)prms->graph);
        }
        if(status == VX_SUCCESS)
        {
            /* Read vx_user_data_object parameters passed from host */
            prms->tidlNodeParams[0] = (vx_user_data_object)(obj_desc[TIVX_KERNEL_TIDL_IN_CONFIG_IDX]->host_ref);
            prms->tidlNodeParams[1] =  (vx_user_data_object)(obj_desc[TIVX_KERNEL_TIDL_IN_NETWORK_IDX]->host_ref);
            prms->tidlNodeParams[2] =  (vx_user_data_object)(obj_desc[TIVX_KERNEL_TIDL_IN_CREATE_PARAMS_IDX]->host_ref);
            prms->tidlNodeParams[3] =  (vx_user_data_object)(obj_desc[TIVX_KERNEL_TIDL_IN_IN_ARGS_IDX]->host_ref);
            prms->tidlNodeParams[4] =  (vx_user_data_object)(obj_desc[TIVX_KERNEL_TIDL_IN_OUT_ARGS_IDX]->host_ref);
            if(obj_desc[TIVX_KERNEL_TIDL_IN_TRACE_DATA_IDX] != NULL)
            {
                prms->tidlNodeParams[5] =  (vx_user_data_object)(obj_desc[TIVX_KERNEL_TIDL_IN_TRACE_DATA_IDX]->host_ref);
            }
            else
            {
                prms->tidlNodeParams[5] = (vx_user_data_object)NULL;
            }

            /******  Find number of input and output tensors from TIDL io buffer descriptor ******/
            tivxTIDLJ7Params  *tidlConfigParams = NULL;
            sTIDL_IOBufDesc_t *ioBufDesc = NULL;
            vx_user_data_object config = prms->tidlNodeParams[0];
            vx_map_id map_id_config;

            vxMapUserDataObject(config, 0, sizeof(tivxTIDLJ7Params), &map_id_config,
                (void **)&tidlConfigParams, (vx_enum)VX_READ_ONLY, (vx_enum)VX_MEMORY_TYPE_HOST, 0);

            ioBufDesc = (sTIDL_IOBufDesc_t *)&tidlConfigParams->ioBufDesc;

            prms->num_input_tensors  = (uint32_t)ioBufDesc->numInputBuf;
            prms->num_output_tensors = (uint32_t)ioBufDesc->numOutputBuf;
            prms->num_virtual_cores = (uint32_t)ioBufDesc->numVirtualCores;
            prms->num_cores = (uint32_t)ioBufDesc->numCores;
            prms->num_super_batches = (uint32_t)ioBufDesc->numSuperBatches;
            prms->inference_mode = (uint32_t)ioBufDesc->inferenceMode;

            vxUnmapUserDataObject(config, map_id_config);
            /***********************************************************************************/
            TIDL_CreateParams  *createParams = NULL;
            vx_user_data_object create_params = prms->tidlNodeParams[TIVX_KERNEL_TIDL_IN_CREATE_PARAMS_IDX];
            vx_map_id map_id_create_params;
            vxMapUserDataObject(create_params, 0, sizeof(TIDL_CreateParams), &map_id_create_params,
                (void **)&createParams, (vx_enum)VX_READ_ONLY, (vx_enum)VX_MEMORY_TYPE_HOST, 0);
            /* Read coreStartIdx from createParams*/
            prms->core_start_idx = createParams->coreStartIdx;
            prms->trace_log_level = createParams->traceLogLevel;
            vxUnmapUserDataObject(create_params, map_id_create_params);
            /***********************************************************************************/

            /* Save inputs and outputs vx_tensors to be used for node creation */
            for(int j = 0; j < prms->num_virtual_cores; j++)
            {
                for(int i = 0; i < prms->num_input_tensors/(int32_t)(prms->num_virtual_cores); i++)
                {   
                    prms->inputTensors[j][i] = (vx_tensor)(obj_desc[TIVX_KERNEL_TIDL_IN_FIRST_TENSOR + j * prms->num_input_tensors/(int32_t)(prms->num_virtual_cores) + i]->host_ref);
                }
                for(int i = 0; i < prms->num_output_tensors/(int32_t)(prms->num_virtual_cores); i++)
                {
                    prms->outputTensors[j][i] = (vx_tensor)(obj_desc[TIVX_KERNEL_TIDL_IN_FIRST_TENSOR + prms->num_input_tensors + j * prms->num_output_tensors/(int32_t)(prms->num_virtual_cores) + i]->host_ref);
                }
                
                /* Create new reference using existing reference as example for input/output tensors */
                for(int i = 0; i < (prms->num_input_tensors)/(int32_t)(prms->num_virtual_cores); i++)
                {
                    prms->inputTensors[j][i] = (vx_tensor)tivxCreateReferenceFromExemplar(context, (vx_reference)prms->inputTensors[j][i]);
                }
                for(int i = 0; i < (prms->num_output_tensors)/(int32_t)(prms->num_virtual_cores); i++)
                {
                    prms->outputTensors[j][i] = (vx_tensor)tivxCreateReferenceFromExemplar(context, (vx_reference)prms->outputTensors[j][i]);
                }
            }

            if(prms->inference_mode == TIDL_inferenceModeLowLatency) /* Create num_cores (4) copies from num_virtual_cores (1) input/output tensors for individual cores */
            {
                for(int j = 1; j < prms->num_cores ; j++)
                {
                    if(prms->num_cores <= TIDL_MAX_NUM_CORES) /* To avoid -Werror=array-bounds warning*/
                    {
                        for(int i = 0; i < prms->num_input_tensors/(int32_t)(prms->num_virtual_cores); i++)
                        {
                            prms->inputTensors[j][i] = (vx_tensor)tivxCreateReferenceFromExemplar(context, (vx_reference)prms->inputTensors[0][i]);
                        }
                        for(int i = 0; i < (prms->num_output_tensors)/(int32_t)(prms->num_virtual_cores); i++)
                        {
                            prms->outputTensors[j][i] = (vx_tensor)tivxCreateReferenceFromExemplar(context, (vx_reference)prms->outputTensors[0][i]);
                        }
                    }
                }
            }

            if(status == VX_SUCCESS)
            {
                /* Typecast params to vx_reference for node creation API */
                for(int i = 0; i < TIVX_KERNEL_TIDL_NUM_BASE_PARAMETERS; i++)
                {
                    nestedNodeParams[i] = (vx_reference)prms->tidlNodeParams[i];
                }
                
                /* vx_kernel kernel = vxGetKernelByName(context, TIVX_KERNEL_TIDL_NAME); */
                prms->kernel = tivxAddKernelTIDL(context, prms->num_input_tensors/(int32_t)(prms->num_virtual_cores), prms->num_output_tensors/(int32_t)(prms->num_virtual_cores));

                VX_PRINT(VX_ZONE_INFO, "Kernel created for nested graph on MPU_1 -- %p \n", prms->kernel);

                vx_user_data_object create_params = prms->tidlNodeParams[TIVX_KERNEL_TIDL_IN_CREATE_PARAMS_IDX];
                vx_map_id map_id_create_params;
                TIDL_CreateParams * createParams = NULL;
                vxMapUserDataObject(create_params, 0, sizeof(TIDL_CreateParams), &map_id_create_params,
                (void **)&createParams, (vx_enum)VX_READ_AND_WRITE, (vx_enum)VX_MEMORY_TYPE_HOST, 0);
#if (HOST_EMULATION)
                char traceBaseNameOrig[TIDL_STRING_SIZE], traceBaseName[TIDL_STRING_SIZE];
                strncpy(traceBaseNameOrig, (char *)createParams->traceBaseName, TIDL_STRING_SIZE);
#endif
                vx_user_data_object create_params_copy[TIDL_MAX_NUM_CORES];
                TIDL_CreateParams * mappedObjects[TIDL_MAX_NUM_CORES];
                vx_map_id mappedIds[TIDL_MAX_NUM_CORES];

                for(int i = 0; i < prms->num_cores; i++)
                {
                    /* create copies of create Params to pass different parameters to different cores, e.g. coreId, traceBaseName, etc.*/
                    create_params_copy[i] = vxCreateUserDataObject(context, "TIDL_CreateParams", sizeof(TIDL_CreateParams), NULL);
                    /* Create out_args copy for each core, it is not yet populated so no need to copy data from existing outArgs to the copied ones */
                    prms->outArgs[i] = (vx_user_data_object)tivxCreateReferenceFromExemplar(context, (vx_reference)prms->tidlNodeParams[TIVX_KERNEL_TIDL_IN_OUT_ARGS_IDX]);
                }

                for(int i = 0; i < prms->num_cores; i++)
                {
                    VX_PRINT(VX_ZONE_INFO, "Creating node %d with DSP target \n", i);

                    /************* Update createParams with current core's id **********************/
                        
                    /* Better way to do this would be use of tivxGetSelfCpuId() as part of DSP create callbacks to get coreId but
                    that would not work for host emulation mode of TIDL 
                    Hence, creating copies of createParams for all cores, where each core's id is populated in its own copy of createParams */
                    createParams->coreId = prms->core_start_idx + i;

                    status = vxCopyUserDataObject(create_params_copy[i], 0, sizeof(TIDL_CreateParams), createParams, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);
                    vxMapUserDataObject(create_params_copy[i], 0, sizeof(TIDL_CreateParams), &mappedIds[i],
                    (void **)&mappedObjects[i], (vx_enum)VX_WRITE_ONLY, (vx_enum)VX_MEMORY_TYPE_HOST, 0);
#if (HOST_EMULATION)
                    /* Update trace name with core id for host emulation mode. For target mode, this is done by the remote file io utility in app_utils */
                    char fileName[TIDL_STRING_SIZE], dirName[TIDL_STRING_SIZE];
                    strcpy(fileName, traceBaseNameOrig);
                    strcpy(dirName, traceBaseNameOrig);
                    char * fileNamePtr = getFileNameFromPath(fileName);
                    getDirFromPath(dirName);
                    snprintf((char *)traceBaseName, TIDL_STRING_SIZE, "%s/C7x_%d_%s", dirName, (prms->core_start_idx + i + 1), (char *)fileNamePtr);
                    strncpy(mappedObjects[i]->traceBaseName, (char *)traceBaseName, TIDL_STRING_SIZE);
#endif
                    vxUnmapUserDataObject(create_params_copy[i], mappedIds[i]);
                    nestedNodeParams[TIVX_KERNEL_TIDL_IN_CREATE_PARAMS_IDX] = (vx_reference)create_params_copy[i];
                    nestedNodeParams[TIVX_KERNEL_TIDL_IN_OUT_ARGS_IDX] = (vx_reference)prms->outArgs[i];
                    
                    prms->node[i] = tivxTIDLNode(prms->graph, prms->kernel, nestedNodeParams, prms->inputTensors[i], prms->outputTensors[i]);
                    vxSetNodeTarget(prms->node[i], VX_TARGET_STRING, child_target_name[(prms->core_start_idx + i) * 8]); /* setting target to DSP cores - multiple priorities currently not supported */
                    status = vxGetStatus((vx_reference)prms->node[i]);
                    if(status == VX_SUCCESS)
                    {
                        VX_PRINT(VX_ZONE_INFO, "Node successfully created for target %s \n", child_target_name[i*8]);
                    }
                    vxReleaseUserDataObject(&create_params_copy[i]);
                }
                vxUnmapUserDataObject(create_params, map_id_create_params);
                vxReleaseContext(&context);
            }
        }

        if(status == VX_SUCCESS)
        {
            status = vxVerifyGraph(prms->graph);
        }
        
        if(status == VX_SUCCESS)
        {
            /* Save input/output tensor pointers used as part of graph creation to be deleted in Delete callback
               Must be done after vxVerifyGraph since buffer is allocated during vxVerifyGraph */
            for(int j = 0; j < prms->num_cores; j++)
            {
                for(int i = 0; i < (prms->num_input_tensors/(int32_t)(prms->num_virtual_cores)); i++)
                {
                    status |= tivxReferenceExportHandle((vx_reference)prms->inputTensors[j][i],
                                                            prms->addrInput[j][i],
                                                            prms->sizeInput[j][i],
                                                            max_entries,
                                                            &prms->numEntriesInput[j][i]);
                }
                for(int i = 0; i < (prms->num_output_tensors/(int32_t)(prms->num_virtual_cores)); i++)
                {
                    status |= tivxReferenceExportHandle((vx_reference)prms->outputTensors[j][i],
                                                        prms->addrOutput[j][i],
                                                        prms->sizeOutput[j][i],
                                                        max_entries,
                                                        &prms->numEntriesOutput[j][i]);
                }
            }
        }

        if(status == VX_SUCCESS)
        {
            tivxSetTargetKernelInstanceContext(kernel, prms,  sizeof(tivxTIDLNestedKernelObj));
        }
        if(status == VX_SUCCESS)
        {
            if(prms->inference_mode == TIDL_inferenceModeLowLatency)
            {
                vx_status status;

                vx_user_data_object  controlGetArgs, controlSetArgs;

                vx_context context = vxCreateContext();

                controlGetArgs = vxCreateUserDataObject(context, "TIDL_controlGetArgs", sizeof(TIDL_controlGetArgs), NULL);
                controlSetArgs = vxCreateUserDataObject(context, "TIDL_controlSetArgs", sizeof(TIDL_controlSetArgs), NULL);

                status = vxGetStatus((vx_reference)controlGetArgs);
                status |= vxGetStatus((vx_reference)controlSetArgs);
                
                if (status == VX_SUCCESS)
                {
                    VX_PRINT(VX_ZONE_INFO, "Calling get c7x ptrs cmd \n");
                    status = tivxGetDspPtrsCmd(prms, controlGetArgs, &controlSetArgs);
                }

                if(status == VX_SUCCESS)
                {
                    VX_PRINT(VX_ZONE_INFO, "Calling set c7x ptrs cmd \n");
                    status = tivxSetDspPtrsCmd(prms, controlSetArgs);
                }
                if(status == VX_SUCCESS)
                {
                    VX_PRINT(VX_ZONE_INFO, "Calling Init cmd \n");
                    status = tivxInitCmd(prms);
                }
                vxReleaseUserDataObject(&controlGetArgs);
                vxReleaseUserDataObject(&controlSetArgs);
                vxReleaseContext(&context);
            }
        }
        VX_PRINT(VX_ZONE_INFO, "MPU_1 tivxKernelTIDLCreate -- done \n");
    }

    return (status);
}

static vx_status VX_CALLBACK tivxKernelTIDLDelete(
    tivx_target_kernel_instance kernel, tivx_obj_desc_t *obj_desc[],
    uint16_t num_params, void *priv_arg)
{
    VX_PRINT(VX_ZONE_INFO, "MPU_1 tivxKernelTIDLDelete \n");
    
    vx_status status = (vx_status)VX_SUCCESS;

    if (VX_SUCCESS == status)
    {
        uint32_t size = sizeof(tivxTIDLNestedKernelObj);
        tivxTIDLNestedKernelObj *prms = NULL;

        status = tivxGetTargetKernelInstanceContext(kernel,
            (void **)&prms, &size);
        
        if(status==VX_SUCCESS)
        {
            for(int j = 0; j < prms->num_cores; j++)
            {
                for(int i = 0; i < (prms->num_input_tensors/(int32_t)(prms->num_virtual_cores)); i++)
                {
                    status |= tivxReferenceImportHandle((vx_reference)prms->inputTensors[j][i],
                                                    (const void **)prms->addrInput[j][i],
                                                    (const uint32_t *) prms->sizeInput[j][i],
                                                    prms->numEntriesInput[j][i]);
                }
                for(int i = 0; i < (prms->num_output_tensors/(int32_t)(prms->num_virtual_cores)); i++)
                {
                    status |= tivxReferenceImportHandle((vx_reference)prms->outputTensors[j][i],
                                                    (const void **)prms->addrOutput[j][i],
                                                    (const uint32_t *) prms->sizeOutput[j][i],
                                                    prms->numEntriesOutput[j][i]);
                }
            }
        }
        
        if(status==VX_SUCCESS)
        {
            status = vxRemoveKernel(prms->kernel);
            status |= vxReleaseGraph(&prms->graph);
        }

        if(status == VX_SUCCESS)
        {
            for(int j = 0; j < prms->num_cores; j++)
            {
                for(int i = 0; i < prms->num_input_tensors/(int32_t)(prms->num_virtual_cores); i++)
                {
                    status |= vxReleaseTensor(&prms->inputTensors[j][i]);
                }
                for(int i = 0; i < prms->num_output_tensors/(int32_t)(prms->num_virtual_cores); i++)
                {
                    status |= vxReleaseTensor(&prms->outputTensors[j][i]);
                }
                status |= vxReleaseUserDataObject(&prms->outArgs[j]);
            }
        }
    }

    return (status);
}

/* Public Functions */

void tivxAddTargetKernelTIDL_mpu()
{
    vx_enum self_cpu;

    self_cpu = tivxGetSelfCpuId();

    if (self_cpu == TIVX_CPU_ID_MPU_0)
    {
        uint32_t i;

        for (i = 0; i < TIDL_MAX_TARGETS; i++)
        {
            vx_tidl_nested_target_kernel[i] = tivxAddTargetKernelByName
                                    (
                                      TIVX_KERNEL_TIDL_NAME,
                                      target_name[i],
                                      tivxKernelTIDLProcess,
                                      tivxKernelTIDLCreate,
                                      tivxKernelTIDLDelete,
                                      NULL,
                                      NULL
                                    );
        }
    }
}

void tivxRemoveTargetKernelTIDL_mpu()
{
    uint32_t i;
    VX_PRINT(VX_ZONE_INFO, "Removing MPU_1 target kernel \n");
    for (i = 0; i < TIDL_MAX_TARGETS; i++)
    {
        tivxRemoveTargetKernel(vx_tidl_nested_target_kernel[i]);
    }
    VX_PRINT(VX_ZONE_INFO, "Removing MPU_1 target kernel  -- done \n");
}
