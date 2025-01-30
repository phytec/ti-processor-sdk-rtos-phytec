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

#include "app_post_proc_module.h"

vx_status app_init_post_proc(vx_context context, PostProcObj *postProcObj, char *objName, vx_int32 num_cameras, vx_int32 bufq_depth)
{
    vx_status status = VX_SUCCESS;

    postProcObj->config = vxCreateUserDataObject(context, "PostProcConfig", sizeof(tivxOCPostProcParams), NULL);
    status = vxGetStatus((vx_reference)postProcObj->config);

    if(status == VX_SUCCESS)
    {
        status = vxCopyUserDataObject(postProcObj->config, 0, sizeof(tivxOCPostProcParams),\
                  &postProcObj->params, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);
    }

    vx_user_data_object output = vxCreateUserDataObject(context, "PostProcOutput", sizeof(tivxOCPostProcOutput), NULL);
    status = vxGetStatus((vx_reference)output);

    vx_int32 q;
    for(q = 0; q < bufq_depth; q++)
    {
        if(status == VX_SUCCESS)
        {
            postProcObj->output_arr[q] = vxCreateObjectArray(context, (vx_reference)output, num_cameras);
            status = vxGetStatus((vx_reference)postProcObj->output_arr[q]);
            if(status == VX_SUCCESS)
            {
                /* Keep the first entry of each object-array as its required later to enqueue/dequeue references */
                postProcObj->results[q] = (vx_user_data_object) vxGetObjectArrayItem((vx_object_array)postProcObj->output_arr[q], 0);
            }
        }
        else
        {
            printf("Unable to create output object array at depth %d\n", q);
        }
    }
    vxReleaseUserDataObject(&output);

    return status;
}

vx_status app_update_post_proc(vx_context context, PostProcObj *postProcObj, vx_user_data_object config)
{
    vx_status status = VX_SUCCESS;

    vx_map_id map_id_config;
    sTIDL_IOBufDesc_t *ioBufDesc;
    tivxTIDLJ7Params *tidlParams;

    vxMapUserDataObject(config, 0, sizeof(tivxTIDLJ7Params), &map_id_config,
                    (void **)&tidlParams, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0);

    ioBufDesc = (sTIDL_IOBufDesc_t *)&tidlParams->ioBufDesc;
    memcpy(&postProcObj->params.ioBufDesc, ioBufDesc, sizeof(sTIDL_IOBufDesc_t));

    postProcObj->num_input_tensors = ioBufDesc->numInputBuf;
    postProcObj->num_output_tensors = ioBufDesc->numOutputBuf;

    vxUnmapUserDataObject(config, map_id_config);

    return status;
}

void app_deinit_post_proc(PostProcObj *postProcObj, vx_int32 bufq_depth)
{
    vxReleaseUserDataObject(&postProcObj->config);

    vx_int32 q;
    for(q = 0; q < bufq_depth; q++)
    {
      vxReleaseObjectArray(&postProcObj->output_arr[q]);
      vxReleaseUserDataObject(&postProcObj->results[q]);
    }
}

void app_delete_post_proc(PostProcObj *postProcObj)
{
    if(postProcObj->node != NULL)
    {
        vxReleaseNode(&postProcObj->node);
    }
}

vx_status app_create_graph_post_proc(vx_graph graph, PostProcObj *postProcObj, vx_object_array out_args_arr, vx_object_array out_tensor_arr)
{
    vx_status status = VX_SUCCESS;

    vx_user_data_object  in_args  = (vx_user_data_object)vxGetObjectArrayItem((vx_object_array)out_args_arr, 0);
    vx_tensor in_tensor = (vx_tensor)vxGetObjectArrayItem((vx_object_array)out_tensor_arr, 0);
    vx_user_data_object result  = (vx_user_data_object)vxGetObjectArrayItem((vx_object_array)postProcObj->output_arr[0], 0);

    postProcObj->node = tivxOCPostProcNode(graph,
                                           postProcObj->config,
                                           in_args,
                                           in_tensor,
                                           result);

    APP_ASSERT_VALID_REF(postProcObj->node);

    status = vxSetNodeTarget(postProcObj->node, VX_TARGET_STRING, TIVX_TARGET_DSP2);
    vxSetReferenceName((vx_reference)postProcObj->node, "post_proc_node");

    vx_bool replicate[] = {vx_false_e, vx_true_e, vx_true_e, vx_true_e};
    vxReplicateNode(graph, postProcObj->node, replicate, 4);

    vxReleaseUserDataObject(&in_args);
    vxReleaseTensor(&in_tensor);
    vxReleaseUserDataObject(&result);

    return(status);
}
