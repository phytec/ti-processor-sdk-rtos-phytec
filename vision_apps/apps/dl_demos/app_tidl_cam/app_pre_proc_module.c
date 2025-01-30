/*
 *
 * Copyright (c) 2017 Texas Instruments Incorporated
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

#include "app_pre_proc_module.h"

static void createOutputTensors(vx_context context, vx_user_data_object config, vx_tensor output_tensors[]);
static inline vx_enum get_vx_tensor_datatype(int32_t tidl_datatype);

vx_status app_init_pre_proc(vx_context context, PreProcObj *preProcObj, char *objName)
{
    vx_status status = VX_SUCCESS;

    preProcObj->config = vxCreateUserDataObject(context, "PreProcConfig", sizeof(tivxOCPreProcParams), NULL);
    status = vxGetStatus((vx_reference)preProcObj->config);

    if(status == VX_SUCCESS)
    {
        status = vxCopyUserDataObject(preProcObj->config, 0, sizeof(tivxOCPreProcParams),\
                  &preProcObj->params, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);
    }

    if(preProcObj->en_out_pre_proc_write == 1)
    {
        char file_path[TIVX_FILEIO_FILE_PATH_LENGTH];
        char file_prefix[TIVX_FILEIO_FILE_PREFIX_LENGTH];

        strcpy(file_path, preProcObj->output_file_path);
        preProcObj->file_path   = vxCreateArray(context, VX_TYPE_UINT8, TIVX_FILEIO_FILE_PATH_LENGTH);
        vxAddArrayItems(preProcObj->file_path, TIVX_FILEIO_FILE_PATH_LENGTH, &file_path[0], 1);

        strcpy(file_prefix, "pre_proc_output");
        preProcObj->file_prefix = vxCreateArray(context, VX_TYPE_UINT8, TIVX_FILEIO_FILE_PREFIX_LENGTH);
        vxAddArrayItems(preProcObj->file_prefix, TIVX_FILEIO_FILE_PREFIX_LENGTH, &file_prefix[0], 1);

        preProcObj->write_cmd = vxCreateUserDataObject(context, "tivxFileIOWriteCmd", sizeof(tivxFileIOWriteCmd), NULL);
    }
    else
    {
        preProcObj->file_path   = NULL;
        preProcObj->file_prefix = NULL;
        preProcObj->write_node  = NULL;
        preProcObj->write_cmd   = NULL;
    }

    return status;
}

vx_status app_update_pre_proc(vx_context context, PreProcObj *preProcObj, vx_user_data_object config, vx_int32 num_cameras)
{
    vx_status status = VX_SUCCESS;

    vx_map_id map_id_config;
    sTIDL_IOBufDesc_t *ioBufDesc;
    tivxTIDLJ7Params *tidlParams;
    vx_tensor output_tensors[APP_MAX_TENSORS];
    vx_int32 i;

    vxMapUserDataObject(config, 0, sizeof(tivxTIDLJ7Params), &map_id_config,
                    (void **)&tidlParams, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0);

    ioBufDesc = (sTIDL_IOBufDesc_t *)&tidlParams->ioBufDesc;
    memcpy(&preProcObj->params.ioBufDesc, ioBufDesc, sizeof(sTIDL_IOBufDesc_t));

    vxUnmapUserDataObject(config, map_id_config);

    preProcObj->num_input_tensors = ioBufDesc->numInputBuf;
    preProcObj->num_output_tensors = ioBufDesc->numOutputBuf;

    createOutputTensors(context, config, output_tensors);

    for(i = 0; i < preProcObj->num_input_tensors; i++)
    {
        preProcObj->output_tensor_arr[i] = vxCreateObjectArray(context, (vx_reference)output_tensors[i], num_cameras);
        vxReleaseTensor(&output_tensors[i]);
    }

    return status;
}

void app_deinit_pre_proc(PreProcObj *preProcObj)
{
    vxReleaseUserDataObject(&preProcObj->config);

    vx_int32 i;
    for(i = 0; i < preProcObj->num_input_tensors; i++)
    {
        vxReleaseObjectArray(&preProcObj->output_tensor_arr[i]);
    }
    if(preProcObj->en_out_pre_proc_write == 1)
    {
        vxReleaseArray(&preProcObj->file_path);
        vxReleaseArray(&preProcObj->file_prefix);
        vxReleaseUserDataObject(&preProcObj->write_cmd);
    }
}

void app_delete_pre_proc(PreProcObj *preProcObj)
{
    if(preProcObj->node != NULL)
    {
        vxReleaseNode(&preProcObj->node);
    }
    if(preProcObj->write_node != NULL)
    {
        vxReleaseNode(&preProcObj->write_node);
    }
}

vx_status app_create_graph_pre_proc(vx_graph graph, PreProcObj *preProcObj, vx_object_array input_arr)
{
    vx_status status = VX_SUCCESS;

    vx_image  input   = (vx_image)vxGetObjectArrayItem((vx_object_array)input_arr, 0);
    vx_tensor output  = (vx_tensor)vxGetObjectArrayItem((vx_object_array)preProcObj->output_tensor_arr[0], 0);

    preProcObj->node = tivxOCPreProcNode(graph,
                                         preProcObj->config,
                                         input,
                                         output);

    APP_ASSERT_VALID_REF(preProcObj->node);

    status = vxSetNodeTarget(preProcObj->node, VX_TARGET_STRING, TIVX_TARGET_DSP1);
    vxSetReferenceName((vx_reference)preProcObj->node, "pre_proc_node");

    vx_bool replicate[] = {vx_false_e, vx_true_e, vx_true_e};
    vxReplicateNode(graph, preProcObj->node, replicate, 3);

    vxReleaseImage(&input);
    vxReleaseTensor(&output);

    if(preProcObj->en_out_pre_proc_write == 1)
    {
        APP_PRINTF("Adding Pre Proc write node on graph .. \n");
        status = app_create_graph_pre_proc_write_output(graph, preProcObj);
        APP_PRINTF("Pre Proc write node added! \n");
    }

    return(status);
}

vx_status app_create_graph_pre_proc_write_output(vx_graph graph, PreProcObj *preProcObj)
{
    vx_status status = VX_SUCCESS;

    vx_tensor output = (vx_tensor)vxGetObjectArrayItem(preProcObj->output_tensor_arr[0], 0);

    preProcObj->write_node = tivxWriteTensorNode(graph, output, preProcObj->file_path, preProcObj->file_prefix);
    status = vxGetStatus((vx_reference)preProcObj->write_node);
    vxSetNodeTarget(preProcObj->write_node, VX_TARGET_STRING, TIVX_TARGET_MPU_0);
    vxSetReferenceName((vx_reference)preProcObj->write_node, "pre_proc_write_node");

    vx_bool replicate[] = { vx_true_e, vx_false_e, vx_false_e};
    vxReplicateNode(graph, preProcObj->write_node, replicate, 3);

    vxReleaseTensor(&output);

    return (status);
}

vx_status app_send_cmd_pre_proc_write_node(PreProcObj *preProcObj, vx_uint32 start_frame, vx_uint32 num_frames, vx_uint32 num_skip)
{
    vx_status status = VX_SUCCESS;

    tivxFileIOWriteCmd write_cmd;

    write_cmd.start_frame = start_frame;
    write_cmd.num_frames = num_frames;
    write_cmd.num_skip = num_skip;

    status = vxCopyUserDataObject(preProcObj->write_cmd, 0, sizeof(tivxFileIOWriteCmd),\
                  &write_cmd, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);

    if(status == VX_SUCCESS)
    {
        vx_reference refs[2];

        refs[0] = (vx_reference)preProcObj->write_cmd;

        status = tivxNodeSendCommand(preProcObj->write_node, TIVX_CONTROL_CMD_SEND_TO_ALL_REPLICATED_NODES,
                                 TIVX_FILEIO_CMD_SET_FILE_WRITE,
                                 refs, 1u);

        if(VX_SUCCESS != status)
        {
            printf("Pre proc write node send command failed!\n");
        }

        APP_PRINTF("Pre proc write node send command success!\n");
    }

    return (status);
}

vx_status writePreProcOutput(char* file_name, PreProcObj *preProcObj)
{
    vx_status status = VX_SUCCESS;

    vx_tensor output;
    vx_size numCh;
    vx_int32 ch;

    vxQueryObjectArray((vx_object_array)preProcObj->output_tensor_arr[0], VX_OBJECT_ARRAY_NUMITEMS, &numCh, sizeof(vx_size));

    for(ch = 0; ch < numCh; ch++)
    {
        vx_size num_dims;
        void *data_ptr;
        vx_map_id map_id;

        vx_size    start[APP_MAX_TENSOR_DIMS];
        vx_size    tensor_strides[APP_MAX_TENSOR_DIMS];
        vx_size    tensor_sizes[APP_MAX_TENSOR_DIMS];
        vx_char new_name[APP_MAX_FILE_PATH];

        /* Write Y plane */
        output  = (vx_tensor)vxGetObjectArrayItem((vx_object_array)preProcObj->output_tensor_arr[0], ch);

        vxQueryTensor(output, VX_TENSOR_NUMBER_OF_DIMS, &num_dims, sizeof(vx_size));

        if(num_dims != 3)
        {
            printf("Number of dims are != 3! exiting.. \n");
            break;
        }

        vxQueryTensor(output, VX_TENSOR_DIMS, tensor_sizes, 3 * sizeof(vx_size));

        start[0] = start[1] = start[2] = 0;

        tensor_strides[0] = 1;
        tensor_strides[1] = tensor_strides[0];
        tensor_strides[2] = tensor_strides[1] * tensor_strides[1];

        status = tivxMapTensorPatch(output, num_dims, start, tensor_sizes, &map_id, tensor_strides, &data_ptr, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);

        snprintf(new_name, APP_MAX_FILE_PATH, "%s_%dx%d_ch%d.yuv", file_name, (uint32_t)tensor_sizes[0], (uint32_t)tensor_sizes[1], ch);

        FILE *fp = fopen(new_name, "wb");
        if(NULL == fp)
        {
            printf("Unable to open file %s for writing!\n", new_name);
            break;
        }

        if(VX_SUCCESS == status)
        {
            fwrite(data_ptr, 1, tensor_sizes[0] * tensor_sizes[1] * tensor_sizes[2], fp);

            tivxUnmapTensorPatch(output, map_id);
        }
        vxReleaseTensor(&output);

        /* Write U V planes */
        output  = (vx_tensor)vxGetObjectArrayItem((vx_object_array)preProcObj->output_tensor_arr[1], ch);

        vxQueryTensor(output, VX_TENSOR_NUMBER_OF_DIMS, &num_dims, sizeof(vx_size));

        if(num_dims != 3)
        {
            printf("Number of dims are != 3! exiting.. \n");
            fclose(fp);
            break;
        }

        vxQueryTensor(output, VX_TENSOR_DIMS, tensor_sizes, 3 * sizeof(vx_size));

        start[0] = start[1] = start[2] = 0;

        tensor_strides[0] = 1;
        tensor_strides[1] = tensor_strides[0];
        tensor_strides[2] = tensor_strides[1] * tensor_strides[1];

        status = tivxMapTensorPatch(output, num_dims, start, tensor_sizes, &map_id, tensor_strides, &data_ptr, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);

        if(VX_SUCCESS == status)
        {

            fwrite(data_ptr, 1, tensor_sizes[0] * tensor_sizes[1] * tensor_sizes[2], fp);

            tivxUnmapTensorPatch(output, map_id);
        }
        vxReleaseTensor(&output);

        fclose(fp);
    }

  return(status);
}

static void createOutputTensors(vx_context context, vx_user_data_object config, vx_tensor output_tensors[])
{
    vx_size   input_sizes[APP_MAX_TENSOR_DIMS];
    vx_map_id map_id_config;

    vx_uint32 id;

    tivxTIDLJ7Params *tidlParams;
    sTIDL_IOBufDesc_t *ioBufDesc;

    vxMapUserDataObject(config, 0, sizeof(tivxTIDLJ7Params), &map_id_config,
                      (void **)&tidlParams, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0);

    ioBufDesc = (sTIDL_IOBufDesc_t *)&tidlParams->ioBufDesc;
    for(id = 0; id < ioBufDesc->numInputBuf; id++) 
    {
        input_sizes[0] = ioBufDesc->inWidth[id]  + ioBufDesc->inPadL[id] + ioBufDesc->inPadR[id];
        input_sizes[1] = ioBufDesc->inHeight[id] + ioBufDesc->inPadT[id] + ioBufDesc->inPadB[id];
        input_sizes[2] = ioBufDesc->inNumChannels[id];

        vx_enum data_type = get_vx_tensor_datatype(ioBufDesc->inElementType[id]);
        output_tensors[id] = vxCreateTensor(context, 3, input_sizes, data_type, 0);
    }

    vxUnmapUserDataObject(config, map_id_config);

    return;
}

static inline vx_enum get_vx_tensor_datatype(int32_t tidl_datatype)
{
    vx_enum tiovx_datatype = VX_TYPE_INVALID;

    if(tidl_datatype == TIDL_UnsignedChar)
    {
        tiovx_datatype = VX_TYPE_UINT8;
    }
    else if(tidl_datatype == TIDL_SignedChar)
    {
        tiovx_datatype = VX_TYPE_INT8;
    }
    else if(tidl_datatype == TIDL_UnsignedShort)
    {
        tiovx_datatype = VX_TYPE_UINT16;
    }
    else if(tidl_datatype == TIDL_SignedShort)
    {
        tiovx_datatype = VX_TYPE_INT16;
    }
    else if(tidl_datatype == TIDL_UnsignedWord)
    {
        tiovx_datatype = VX_TYPE_UINT32;
    }
    else if(tidl_datatype == TIDL_SignedWord)
    {
        tiovx_datatype = VX_TYPE_INT32;
    }
    else if(tidl_datatype == TIDL_SinglePrecFloat)
    {
        tiovx_datatype = VX_TYPE_FLOAT32;
    }

    return (tiovx_datatype);
}
