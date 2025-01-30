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

#include <TI/tivx.h>
#include <TI/tivx_task.h>
#include <TI/j7_tidl.h>
#include <tivx_utils_file_rd_wr.h>
#include <tivx_utils_graph_perf.h>
#include <tivx_utils_tidl_trace.h>
#include <TI/dl_kernels.h>

#if ! defined (HOST_EMULATION)
#include "utils/perf_stats/include/app_perf_stats.h"
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <assert.h>
#include <sys/stat.h>
#include <float.h>
#include <math.h>
#include "itidl_rt.h"
#include "tidl_rt_ovx_utils.h"
extern char* strdup(const char*);

#define TIVX_TIDL_TRACE_DATA_SIZE  (64 * 1024 * 1024)

#define APP_ASSERT(x)               assert((x))
#define APP_ASSERT_VALID_REF(ref)   (APP_ASSERT(vxGetStatus((vx_reference)(ref))==VX_SUCCESS));

#define ABS_FLT(a) ((a) > 0)?(a):(-(a))
#define MAX(A,B) ((A) > (B) ? (A) : (B))

static uint32_t   tidlrt_debuglevel = 0;
static uint32_t   num_params;
static uint32_t   max_params;

/**
  @struct  sTIDLRTTB_IntHandle_t
  @brief   This structure is internal handle for TIDL RT API wrapper
  */
typedef struct
{
    /** Struct to an application object*/
    AppObj gAppObj;

    /** TIRL RT create time configuration parametes*/
    sTIDLRT_Params_t rtPrms;

} IntHandle_t;

#define TIDLRT_MAX_HANDLES (32)
static uint32_t     tidlrt_handles_state[TIDLRT_MAX_HANDLES] = {0};
static IntHandle_t  tidltr_handles_pool[TIDLRT_MAX_HANDLES];

void * tidlrt_alloc_handle(void)
{
    void * ptr = NULL;
    for (int i = 0; i <TIDLRT_MAX_HANDLES; i++)
    {
        if(tidlrt_handles_state[i] == 0)
        {
            tidlrt_handles_state[i] = 1;
            ptr = (void*)&tidltr_handles_pool[i];
            break;
        }
    }
    return ptr;

}

int32_t tidlrt_free_handle(void * ptr)
{
    int32_t ret = VX_FAILURE;
    if(ptr != NULL)
    {
        for (int i = 0; i <TIDLRT_MAX_HANDLES; i++)
        {
            if(&tidltr_handles_pool[i] == ptr)
            {
                tidlrt_handles_state[i] = 0;
                ret = VX_SUCCESS;
                break;
            }
        }
    }
    if(ret == VX_FAILURE)
    {
        printf("TIDL_RT_OVX_ERR: NOT a Valid TIDLRT handle Memory \n");
    }

    return ret;

}


static vx_user_data_object mapConfig(AppObj *obj, vx_context context, void * netPtr, int32_t capacity, uint32_t *num_input_tensors, uint32_t *num_output_tensors, int32_t * inferenceMode);
static vx_user_data_object mapNetwork(vx_context context, void * netPtr, int32_t capacity);
static int init_tidl_tiovx (AppObj *obj,sTIDLRT_Params_t *prms);

static int deinit_tidl_tiovx(AppObj *obj);
static vx_status create_graph_tidl_tiovx(AppObj *obj);
static void delete_graph_tidl_tiovx(AppObj *obj);
static vx_status run_graph_tidlrt_tiovx(AppObj *obj, sTIDLRT_Tensor_t *in[], sTIDLRT_Tensor_t *out[],
		sTIDLRT_PerfStats_t *stats);
static vx_status verify_graph_tidl_tiovx(AppObj *obj);
static vx_user_data_object setCreateParams(AppObj *obj, sTIDLRT_Params_t *prms, int32_t inferenceMode);
static vx_user_data_object setInArgs(AppObj *obj);
static vx_user_data_object setOutArgs(vx_context context);

static void initParam(vx_reference params[], uint32_t _max_params);
static void addParam(vx_reference params[], vx_reference obj);

static void createInputTensors(AppObj *obj, vx_context context, vx_user_data_object config, vx_tensor *input_tensors);
static void createOutputTensors(AppObj *obj, vx_context context, vx_user_data_object config, vx_tensor *output_tensors);

static vx_size getTensorDataType(vx_int32 tidl_type);
static uint32_t getElementSize(uint32_t data_type);

static vx_status map_cp_in_tidlrt_tensor_tiovx(AppObj *obj, vx_context context, vx_user_data_object config, vx_tensor *input_tensors, void *input_buffer, sTIDLRT_Tensor_t *in[]);
static vx_status memset_out_tensor_tidlrt_tiovx(AppObj *obj, vx_user_data_object config, vx_tensor *output_tensors, sTIDLRT_Tensor_t *out[]);
static vx_status map_cp_out_tensor_tidlrt_tiovx(AppObj *obj, vx_user_data_object config, vx_tensor *output_tensors, sTIDLRT_Tensor_t *out[]);
//static vx_status cp_data_in_tidlrt_tensor_tiovx_slow(AppObj *obj, sTIDLRT_Tensor_t *in[], void *input_buffer, uint32_t id, uint32_t tidlrt_id);
//static vx_status cp_data_out_tensor_tidlrt_tiovx_slow(AppObj *obj, sTIDLRT_Tensor_t *out[], void *output_buffer, uint32_t id, uint32_t elementSize, vx_size data_type, float scale, uint32_t tidlrt_id);
static vx_status set_in_tidlrt_tensor_refs(AppObj *obj, vx_tensor *input_tensors, sTIDLRT_Tensor_t *in[]);
static vx_status set_out_tidlrt_tensor_refs(AppObj *obj, vx_tensor *output_tensors, sTIDLRT_Tensor_t *out[]);

int tidlrt_printf(const char *fmt, ...)
{
    va_list ap;
    int ret = 0;

    if(!tidlrt_debuglevel)
        goto out;

    va_start(ap, fmt);
    ret = vprintf(fmt, ap);
    va_end(ap);

out:
    return ret;
}

static void __attribute__((constructor)) lib_init()
{
    char *debug_str;

    debug_str = getenv("TIDL_RT_DEBUG");
    if(!debug_str)
        tidlrt_debuglevel = 0;
    else
        tidlrt_debuglevel = atoi(debug_str);
}

int32_t getTidlRtFlowCtrlEnv()
{
    int32_t env;
    char *str;
    str = getenv("TIDL_RT_FLOW_CTRL");
    if(!str)
    {
        #ifdef x86_64
        env = 1;
        #else
        env = 0;
        #endif

    }
    else
    {
        env = atoi(str);
    }
    return env;
}

int32_t getAVXKernelEnv()
{
int32_t env = 0;
#ifdef x86_64
    char *str;
    str = getenv("TIDL_RT_AVX_REF");
    if(!str)
    {
        env = 0x00000020;
    }
    else
    {
        env = atoi(str);
        if((env != 0) && (env != 1) && (env != 3))
        {
          printf("TIDL_RT_AVX_REF is expected to be either 0 or 1 or 3. -  %d is not supported. Setting it to zero\n", env);
          env = 0;
        }
        if(env)
        {
            env = env << 5;
        }
   }
#endif
    return env;
}

int32_t TIDLRT_setParamsDefault(sTIDLRT_Params_t *prms)
{
    int32_t status                  = 0;
    prms->netPtr                    = NULL;
    prms->ioBufDescPtr              = NULL;
    prms->net_capacity              = 0;
    prms->io_capacity               = 0;
    prms->flowCtrl                  = getTidlRtFlowCtrlEnv();
    prms->flowCtrl                 |= getAVXKernelEnv();
    prms->traceLogLevel             = 0;
    prms->traceWriteLevel           = 0;
    strcpy(prms->traceBaseName, "/tmp/tidl_trace");
    prms->TIDLWriteBinToFile        = NULL;
    prms->TIDLReadBinFromFile       = NULL;
    prms->TIDLVprintf               = vprintf;
    prms->quantRangeExpansionFactor = 1;
    prms->quantRangeUpdateFactor    = 0;
    prms->stats                     = NULL;
    prms->maxPreEmptDelay           = FLT_MAX;
    prms->targetPriority            = 0;
    prms->coreNum                   = 1;
    prms->coreStartIdx              = 1;

    tidlrt_printf("TIDL_RT_OVX: Set default TIDLRT params done\n");
    return status;
}

int32_t TIDLRT_setTensorDefault(sTIDLRT_Tensor_t *tensor)
{
    int32_t status                  = 0;
    //tensor->name[]                = {0};
    tensor->elementType             = 0;
    tensor->numDim                  = 0;
    tensor->ptr                     = NULL;
    tensor->dataOffset              = 0;
    tensor->layout                  = 0;
    tensor->zeroPoint               = 0;
    tensor->scale                   = 1.0;
    tensor->memType                 = 0;
    tensor->pitch[TIDL_ROI_PITCH] = -1;
    tensor->pitch[TIDL_CHANNEL_PITCH] = -1;
    tensor->padValues[0] = 0;
    tensor->padValues[1] = 0;
    tensor->padValues[2] = 0;
    tensor->padValues[3] = 0;
    tensor->dimValues[TIDL_DIM_WIDTH]   = -1;
    tensor->dimValues[TIDL_DIM_HEIGHT]  = -1;
    tensor->dimValues[TIDL_DIM_NUMCH]   = -1;
    tensor->dimValues[TIDL_DIM_BATCH]   = -1;



    tidlrt_printf("TIDL_RT_OVX: Set default TIDLRT tensor done\n");
    return status;

}

int32_t TIDLRT_create(sTIDLRT_Params_t *prms, void **handle)
{
    int32_t status = VX_SUCCESS;

    tidl_rt_ovx_Init();

    IntHandle_t *rtHandle = (IntHandle_t*)tidlrt_alloc_handle();
    if (rtHandle==NULL)
    {
        printf("TIDL_RT_OVX: ERROR: Unable to allocate memory for TIDL RT handle\n");
    }

    AppObj *obj = &(rtHandle->gAppObj);
    obj->context = vxCreateContext();
    APP_ASSERT_VALID_REF(obj->context);

    obj->traceLogLevel = prms->traceLogLevel;
    obj->traceWriteLevel = prms->traceWriteLevel;
    if(prms->traceBaseName[0] != '\0')
    {
        if(strcmp(prms->traceSubgraphName, "") != 0)
        {
            char traceBaseNameTmp[TIDLRT_STRING_SIZE];
            int32_t ret = snprintf(traceBaseNameTmp, TIDLRT_STRING_SIZE, "%s_%s_", prms->traceBaseName, prms->traceSubgraphName);
            if(ret < 0)
            {
                printf("Truncation error\n");
            }
            strncpy(prms->traceBaseName, traceBaseNameTmp, TIDLRT_STRING_SIZE);
        }
        strncpy(obj->traceBaseName, prms->traceBaseName, TIDLRT_STRING_SIZE);
    }
    else
    {
        strncpy(obj->traceBaseName, "/tmp/tidl_trace", TIDLRT_STRING_SIZE);
        strncpy(prms->traceBaseName, "/tmp/tidl_trace", TIDLRT_STRING_SIZE);
    }
    obj->maxPreEmptDelay = prms->maxPreEmptDelay;
    obj->targetPriority = prms->targetPriority;
    obj->coreNum = prms->coreNum;
    obj->coreStartIdx = prms->coreStartIdx;

    status = init_tidl_tiovx(obj,prms);

    if (VX_SUCCESS == status)
    {
        status = create_graph_tidl_tiovx(obj);
    }else{
        printf("TIDL_RT_OVX: ERROR: Init function failed\n");
    }

    if (VX_SUCCESS == status)
    {
        status = verify_graph_tidl_tiovx(obj);
    }else{
        printf("TIDL_RT_OVX: ERROR: Create OpenVX graph failed\n");
    }

    if (VX_SUCCESS != status)
    {
        printf("TIDL_RT_OVX: ERROR: Verify OpenVX graph failed\n");
    }

    /* Release the network user data object since it is not needed after Create call is complete */
    void       *ref1Addr[1]; /* Single user data object */
    uint32_t    size[1];
    uint32_t    numPlanes;
    status = tivxReferenceExportHandle((vx_reference)obj->network,
                                                    ref1Addr,
                                                    size,
                                                    1,
                                                    &numPlanes);
    tivxMemFree(ref1Addr[0], size[0], TIVX_MEM_EXTERNAL);
    ref1Addr[0] = NULL;
    status = tivxReferenceImportHandle((vx_reference)obj->network,
                                            (const void **)ref1Addr,
                                            size,
                                            numPlanes);
    vxReleaseUserDataObject(&obj->network);
    /* Network memory freed and network user data object released */
#if ! defined (HOST_EMULATION)
    if(tidlrt_debuglevel == 3)
    {
        printf("******** Printing stats at the end of TIDLRT_create call ****** \n");
        appPerfStatsPrintAll();
    }
#endif
    rtHandle->rtPrms = *prms;
    *handle = rtHandle;
    return status;
}

int32_t TIDLRT_invoke(void *handle, sTIDLRT_Tensor_t *in[], sTIDLRT_Tensor_t *out[])
{
    int32_t status = VX_SUCCESS;
    IntHandle_t * rtHandle = (IntHandle_t*)handle;

    status = run_graph_tidlrt_tiovx(&(rtHandle->gAppObj), in, out, rtHandle->rtPrms.stats);

    return status;
}

int32_t TIDLRT_delete(void *handle)
{
    int32_t status = VX_SUCCESS;
    IntHandle_t * rtHandle = (IntHandle_t*)handle;
    tidlrt_printf("TIDL_RT_OVX: Delete ... \n");

    delete_graph_tidl_tiovx(&(rtHandle->gAppObj));
    status = deinit_tidl_tiovx(&(rtHandle->gAppObj));

    if(status == VX_SUCCESS)
    {
        vxReleaseContext(&rtHandle->gAppObj.context);

        tidlrt_free_handle(handle);
        tidl_rt_ovx_DeInit();
    }

    tidlrt_printf("TIDL_RT_OVX: Delete ... Done \n");
    return status;
}

int32_t TIDLRT_deactivate(void *handle)
{
    int32_t status = VX_SUCCESS;
    return status;
}

static vx_status init_tidl_tiovx(AppObj *obj,sTIDLRT_Params_t *prms)
{

    vx_status status = VX_SUCCESS;

    uint32_t num_input_tensors = 0;
    uint32_t num_output_tensors = 0;
    int32_t inferenceMode = TIDL_inferenceModeDefault;

    tidlrt_printf("TIDL_RT_OVX: Init ... \n");

    tivxTIDLLoadKernels(obj->context);

    /* Create a vx_array object and map the config data*/
    obj->config = mapConfig(obj, obj->context, prms->ioBufDescPtr, prms->io_capacity, &num_input_tensors, &num_output_tensors, &inferenceMode);
    APP_ASSERT_VALID_REF(obj->config);

    obj->kernel = tivxAddKernelTIDL(obj->context, num_input_tensors, num_output_tensors);
    APP_ASSERT_VALID_REF(obj->kernel);

    tidlrt_printf("Host kernel - %p \n", obj->kernel);

    /* Save a copy of number of input/output tensors required as per config */
    obj->num_input_tensors  = num_input_tensors;
    obj->num_output_tensors = num_output_tensors;

    /* Create a vx_tensor object and map the network data */
    obj->network = mapNetwork(obj->context, prms->netPtr, prms->net_capacity);
    APP_ASSERT_VALID_REF(obj->network);

    obj->createParams = setCreateParams(obj, prms, inferenceMode);
    APP_ASSERT_VALID_REF(obj->createParams);

    obj->inArgs = setInArgs(obj);
    APP_ASSERT_VALID_REF(obj->inArgs);

    obj->outArgs = setOutArgs(obj->context);
    APP_ASSERT_VALID_REF(obj->outArgs);

    if(obj->traceWriteLevel) {
        obj->traceData = vxCreateUserDataObject(obj->context, "TIDL_traceData", TIVX_TIDL_TRACE_DATA_SIZE, NULL);
        APP_ASSERT_VALID_REF(obj->traceData);
    }

    tidlrt_printf("TIDL_RT_OVX: Init done.\n");

    return status;
}

static int deinit_tidl_tiovx(AppObj *obj)
{
    uint32_t id;
    void       *ref1Addr[MAX_TENSOR_DIMS];
    uint32_t    size[MAX_TENSOR_DIMS];
    uint32_t    numPlanes;
    vx_status status = VX_SUCCESS;
    tidlrt_printf("TIDL_RT_OVX: DeInit ... \n");

    if(obj->traceWriteLevel)
        vxReleaseUserDataObject(&obj->traceData);

    vxReleaseUserDataObject(&obj->config);
    vxReleaseUserDataObject(&obj->outArgs);
    vxReleaseUserDataObject(&obj->inArgs);
    vxReleaseUserDataObject(&obj->createParams);

    /* Network is released at the end of graph creation, so no need to release here
    vxReleaseUserDataObject(&obj->network);
    */

    for(id = 0; id < obj->num_input_tensors; id++) {
        if(obj->shared_input_tensors[id] == 1) {
            status = tivxReferenceExportHandle((vx_reference)obj->input_tensors[id],
                                                    ref1Addr,
                                                    size,
                                                    MAX_TENSOR_DIMS,
                                                    &numPlanes);
            ref1Addr[0] = NULL;
            status = tivxReferenceImportHandle((vx_reference)obj->input_tensors[id],
                                            (const void **)ref1Addr,
                                            size,
                                            numPlanes);
        }
        vxReleaseTensor(&obj->input_tensors[id]);
    }

    for(id = 0; id < obj->num_output_tensors; id++) {
         if(obj->shared_output_tensors[id] == 1) {
            status = tivxReferenceExportHandle((vx_reference)obj->output_tensors[id],
                                                    ref1Addr,
                                                    size,
                                                    MAX_TENSOR_DIMS,
                                                    &numPlanes);
            ref1Addr[0] = NULL;
            status = tivxReferenceImportHandle((vx_reference)obj->output_tensors[id],
                                            (const void **)ref1Addr,
                                            size,
                                            numPlanes);
         }
         vxReleaseTensor(&obj->output_tensors[id]);
    }

    tivxTIDLUnLoadKernels(obj->context);

    tidlrt_printf("TIDL_RT_OVX: DeInit... Done.\n");
    return status;
}

static void delete_graph_tidl_tiovx(AppObj *obj)
{
    tidlrt_printf("TIDL_RT_OVX: Delete TIDL graph ... \n");

    vxReleaseNode(&obj->tidl_node);
    vxRemoveKernel(obj->kernel);
    vxReleaseGraph(&obj->graph);

    tidlrt_printf("TIDL_RT_OVX: Delete TIDL graph ... Done.\n");
}

static vx_user_data_object mapConfig(AppObj *obj, vx_context context, void * ioBufDescPtr, int32_t capacity, uint32_t *num_input_tensors, uint32_t *num_output_tensors, int32_t * inferenceMode)
{
    vx_status status = VX_SUCCESS;

    tivxTIDLJ7Params  *tidlParams = NULL;
    sTIDL_IOBufDesc_t *ioBufDesc = NULL;
    vx_user_data_object   config = NULL;
    vx_map_id map_id;


    tidlrt_printf("TIDL_RT_OVX: Mapping config file ...\n");

    if( capacity != sizeof(sTIDL_IOBufDesc_t) )
    {
        printf("TIDL_RT_OVX: ERROR: Config file size (%d bytes) does not match size of sTIDL_IOBufDesc_t (%d bytes)\n", capacity, (vx_uint32)sizeof(sTIDL_IOBufDesc_t));
        return NULL;
    }

    /* Create a user struct type for handling config data*/
    config = vxCreateUserDataObject(context, "tivxTIDLJ7Params", sizeof(tivxTIDLJ7Params), NULL );

    status = vxGetStatus((vx_reference)config);

    if (VX_SUCCESS == status)
    {
        status = vxMapUserDataObject(config, 0, sizeof(tivxTIDLJ7Params), &map_id,
                (void **)&tidlParams, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST, 0);

        if (VX_SUCCESS == status)
        {
            if(tidlParams == NULL)
            {
                printf("TIDL_RT_OVX: ERROR: Map of config object failed\n");
                return NULL;
            }

            tivx_tidl_j7_params_init(tidlParams);
            tidlParams->optimize_ivision_activation = 1;
            ioBufDesc = (sTIDL_IOBufDesc_t *)&tidlParams->ioBufDesc;

            memcpy(&obj->ioBufDesc, ioBufDescPtr, capacity); //PC-- can memcpy be avoid here?
            memcpy(ioBufDesc, ioBufDescPtr, capacity); //PC-- can memcpy be avoid here?

            *num_input_tensors  = obj->ioBufDesc.numInputBuf;
            *num_output_tensors = obj->ioBufDesc.numOutputBuf;
            *inferenceMode = obj->ioBufDesc.inferenceMode;

            vxUnmapUserDataObject(config, map_id);
        }
    }

    tidlrt_printf("TIDL_RT_OVX: Mapping config file ... Done. %d bytes\n", (uint32_t)capacity);
    tidlrt_printf("TIDL_RT_OVX: Tensors, input = %d, output = %d\n", *num_input_tensors, *num_output_tensors);

    return config;
}

static vx_user_data_object mapNetwork(vx_context context, void * netPtr, int32_t capacity)
{
    vx_status status;
    vx_user_data_object  network;
    vx_map_id  map_id;
    void *network_buffer = NULL;

    tidlrt_printf("TIDL_RT_OVX: Mapping network file\n");

    network = vxCreateUserDataObject(context, "TIDL_network", capacity, NULL );

    status = vxGetStatus((vx_reference)network);

    if (VX_SUCCESS == status)
    {
        status = vxMapUserDataObject(network, 0, capacity, &map_id,
                (void **)&network_buffer, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST, 0);


        if (VX_SUCCESS == status)
        {

            if(network_buffer) {
                memcpy(network_buffer, netPtr, capacity); //PC-- can memcpy be avoid here?
            } else {
                printf("TIDL_RT_OVX: ERROR: Unable copy netPtr in mapped network_buffer\n");
            }

            vxUnmapUserDataObject(network, map_id);
        }
    }



    tidlrt_printf("TIDL_RT_OVX: Mapping network file... Done %d bytes\n", (uint32_t)capacity);

    return network;
}

static vx_user_data_object setCreateParams(AppObj *obj, sTIDLRT_Params_t *prms, int32_t inferenceMode)
{
    vx_status status;

    vx_user_data_object  createParams;
    vx_map_id  map_id;
    vx_uint32  capacity;
    void *createParams_buffer = NULL;

    capacity = sizeof(TIDL_CreateParams);
    createParams = vxCreateUserDataObject(obj->context, "TIDL_CreateParams", capacity, NULL );

    status = vxGetStatus((vx_reference)createParams);

    if (VX_SUCCESS == status)
    {
        status = vxMapUserDataObject(createParams, 0, capacity, &map_id,
                (void **)&createParams_buffer, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST, 0);

        if (VX_SUCCESS == status)
        {
            if(createParams_buffer)
            {
                TIDL_CreateParams *params = createParams_buffer;
                TIDL_createParamsInit(params);
                params->isInbufsPaded                 = 1;
                params->traceLogLevel                 = obj->traceLogLevel;
                params->traceWriteLevel               = obj->traceWriteLevel;
                params->maxPreEmptDelay               = obj->maxPreEmptDelay;
                params->targetPriority                = obj->targetPriority;
                params->coreId                        = obj->coreNum - 1;
                params->coreStartIdx                  = obj->coreStartIdx - 1;
                uint32_t reservedCtrl                 = prms->flowCtrl >> 16;
                prms->flowCtrl                        = (prms->flowCtrl & 0xFFFF);
                params->flowCtrl                      = prms->flowCtrl;
                params->reservedCtrl                  = reservedCtrl;
                strncpy(params->traceBaseName, prms->traceBaseName, TIDL_STRING_SIZE);
                params->quantRangeExpansionFactor     = prms->quantRangeExpansionFactor;
                params->quantRangeUpdateFactor        = prms->quantRangeUpdateFactor ;
            }
            else
            {
                printf("TIDL_RT_OVX: ERROR: Unable to allocate memory for create time params! %d bytes\n", capacity);
            }

            vxUnmapUserDataObject(createParams, map_id);
        }
    }

    return createParams;
}

static vx_user_data_object setInArgs(AppObj *obj)
{
    vx_status status;

    vx_user_data_object  inArgs;
    vx_map_id  map_id;
    vx_uint32  capacity;
    void *inArgs_buffer = NULL;

    capacity = sizeof(TIDL_InArgs);
    inArgs = vxCreateUserDataObject(obj->context, "TIDL_InArgs", capacity, NULL );

    status = vxGetStatus((vx_reference)inArgs);

    if (VX_SUCCESS == status)
    {
        status = vxMapUserDataObject(inArgs, 0, capacity, &map_id,
                (void **)&inArgs_buffer, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST, 0);

        if (VX_SUCCESS == status)
        {
            if(inArgs_buffer)
            {
                TIDL_InArgs *prms = inArgs_buffer;
                prms->iVisionInArgs.size         = sizeof(TIDL_InArgs);
                prms->iVisionInArgs.subFrameInfo = 0;
                if(obj->traceLogLevel)
                {
                    prms->enableLayerPerfTraces = 1;
                }
            }
            else
            {
                printf("TIDL_RT_OVX: Unable to allocate memory for inArgs! %d bytes\n", capacity);
            }

            vxUnmapUserDataObject(inArgs, map_id);
        }
    }

    return inArgs;
}

static vx_user_data_object setOutArgs(vx_context context)
{
    vx_status status;

    vx_user_data_object  outArgs;
    vx_map_id  map_id;
    vx_uint32  capacity;
    void *outArgs_buffer = NULL;

    capacity = sizeof(TIDL_outArgs);
    outArgs = vxCreateUserDataObject(context, "TIDL_outArgs", capacity, NULL );

    status = vxGetStatus((vx_reference)outArgs);

    if (VX_SUCCESS == status)
    {
        status = vxMapUserDataObject(outArgs, 0, capacity, &map_id,
                (void **)&outArgs_buffer, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST, 0);

        if (VX_SUCCESS == status)
        {
            if(outArgs_buffer)
            {
                TIDL_outArgs *prms = outArgs_buffer;
                prms->iVisionOutArgs.size         = sizeof(TIDL_outArgs);
            }
            else
            {
                printf("TIDL_RT_OVX: Unable to allocate memory for outArgs! %d bytes\n", capacity);
            }

            vxUnmapUserDataObject(outArgs, map_id);
        }
    }

    return outArgs;
}

static void initParam(vx_reference params[], uint32_t _max_params)
{
    num_params  = 0;
    max_params = _max_params;
}

static void addParam(vx_reference params[], vx_reference obj)
{
    APP_ASSERT(num_params <= max_params);

    params[num_params] = obj;

    num_params++;
}

static vx_status create_graph_tidl_tiovx(AppObj *obj)
{
    vx_status status = VX_SUCCESS;
    char tidlrt_string[TIDLRT_STRING_SIZE];
    vx_reference params[TIDL_MAX_PARAMS];
    uint32_t i;

    tidlrt_printf("TIDL_RT_OVX: Creating graph ... \n");

    /* Create OpenVx Graph */
    obj->graph = vxCreateGraph(obj->context);
    APP_ASSERT_VALID_REF(obj->graph);
    
    #ifndef BUILD_WITH_OPENACC
    sprintf(tidlrt_string, "TIDL subgraph %s", (char*)obj->ioBufDesc.outDataName[0]);
    #endif

    vxSetReferenceName((vx_reference)obj->graph, tidlrt_string);

    /* Create array of input tensors */
    createInputTensors(obj, obj->context, obj->config, obj->input_tensors);

    /* Create array of output tensors */
    createOutputTensors(obj, obj->context, obj->config, obj->output_tensors);

    /* Initialize param array */
    initParam(params, TIDL_MAX_PARAMS);

    /* The 1st param MUST be config */
    addParam(params, (vx_reference)obj->config);

    /* The 2nd param MUST be network */
    addParam(params, (vx_reference)obj->network);

    /* The 3rd param MUST be create params */
    addParam(params, (vx_reference)obj->createParams);

    /* The 4th param MUST be inArgs */
    addParam(params, (vx_reference)obj->inArgs);

    /* The 5th param MUST be outArgs */
    addParam(params, (vx_reference)obj->outArgs);

    if(obj->traceWriteLevel)
        addParam(params, (vx_reference)obj->traceData);
    else
        addParam(params, (vx_reference)NULL); //The 6th param MUST be NULL if trace data dump is not enabled

    /* Create TIDL Node */
    obj->tidl_node = tivxTIDLNode(obj->graph, obj->kernel, params, obj->input_tensors, obj->output_tensors);
    /* TODO need to check enum is available or not */
     const char* priors[] = {
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
#ifdef SOC_J784S4
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


#if defined(SOC_J784S4) || defined(SOC_J722S)
    const char* mpuTargets[] = {
        TIVX_TARGET_MPU_1,
        TIVX_TARGET_MPU_2,
        TIVX_TARGET_MPU_3,
    };
    if((obj->coreNum < 1) || (obj->coreNum > TIDL_MAX_NUM_CORES))
    {
        printf("TIDL_RT_OVX: ERROR: Invalid core number specified - %d , please specify 1 <= coreNum <= %d\n", obj->coreNum, TIDL_MAX_NUM_CORES);
        return VX_ERROR_INVALID_VALUE;
    }
    if((obj->coreStartIdx < 1) || (obj->coreStartIdx > TIDL_MAX_NUM_CORES-1))
    {
        printf("TIDL_RT_OVX: ERROR: Invalid coreStartIdx specified - %d , please specify 0 <= coreStartIdx <= %d\n", obj->coreStartIdx, TIDL_MAX_NUM_CORES-1);
        return VX_ERROR_INVALID_VALUE;
    }
    if(obj->ioBufDesc.inferenceMode == TIDL_inferenceModeDefault)
    {
        if(obj->coreStartIdx != 1)
        {
            printf("TIDL_RT_OVX: WARNING : coreStartIdx is not applicable for inferenceMode = 0, please use coreNum to specify desired core for inference \n");
        }
    }
    if((obj->ioBufDesc.inferenceMode == TIDL_inferenceModeHighThroughput) || (obj->ioBufDesc.inferenceMode == TIDL_inferenceModeLowLatency))
    {
      vxSetNodeTarget(obj->tidl_node, VX_TARGET_STRING, mpuTargets[0]); /* setting target to mpu_1 */
    }
    else /* default mode - set DSP targets */
    {
      vxSetNodeTarget(obj->tidl_node, VX_TARGET_STRING, priors[8 * (obj->coreNum - 1) + obj->targetPriority]);
    }
#else
    if(obj->coreNum != 1)
    {
        printf("TIDL_RT_OVX: ERROR: Invalid core number specified - %d , expected core number is 1\n", obj->coreNum);
        return VX_ERROR_INVALID_VALUE;
    }
    if(obj->coreStartIdx != 1)
    {
        printf("TIDL_RT_OVX: ERROR: Invalid coreStartIdx specified - %d , expected coreStartIdx is 1\n", obj->coreStartIdx);
        return VX_ERROR_INVALID_VALUE;
    }
    vxSetNodeTarget(obj->tidl_node, VX_TARGET_STRING, priors[8 * (obj->coreNum - 1) + obj->targetPriority]);
#endif

    APP_ASSERT_VALID_REF(obj->tidl_node);

    /* Set names for diferent OpenVX objects */
    vxSetReferenceName((vx_reference)obj->config, "Config");
    vxSetReferenceName((vx_reference)obj->network, "Network");
    vxSetReferenceName((vx_reference)obj->createParams, "CreateParams");
    vxSetReferenceName((vx_reference)obj->inArgs, "InArgs");
    vxSetReferenceName((vx_reference)obj->outArgs, "OutArgs");

    for(i = 0; i < obj->num_input_tensors; i++) {
        char tensor_name[] = "InputTensor_";
        char ref_name[64];
        snprintf(ref_name, 64, "%s%d", tensor_name, i);
        vxSetReferenceName((vx_reference)obj->input_tensors[i], ref_name);
    }

    for(i = 0; i < obj->num_output_tensors; i++) {
        char tensor_name[] = "OutputTensor_";
        char ref_name[64];
        snprintf(ref_name, 64, "%s%d", tensor_name, i);
        vxSetReferenceName((vx_reference)obj->output_tensors[i], ref_name);
    }

    vxSetReferenceName((vx_reference)obj->kernel, "TIDLKernel");
    vxSetReferenceName((vx_reference)obj->tidl_node, "TIDLNode");


    tidlrt_printf("TIDL_RT_OVX: Creating graph ... Done.\n");
    obj->firstProcessCall = 1;

    return status;
}

static vx_status verify_graph_tidl_tiovx(AppObj *obj)
{
    vx_status status = VX_SUCCESS;

    /* Verify the TIDL Graph */

    status = vxVerifyGraph(obj->graph);
    if(status!=VX_SUCCESS)
    {
        printf("TIDL_RT_OVX: ERROR: Verifying TIDL graph ... Failed !!!\n");
        return status;
    }
    tidlrt_printf("TIDL_RT_OVX: Verifying TIDL graph ... Done.\n");

    return status;
}

static void get_time_u64(uint64_t *t)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    *t = (uint64_t)ts.tv_sec * (uint64_t)1000000000ull + (uint64_t)ts.tv_nsec;
}

static vx_status run_graph_tidlrt_tiovx(AppObj *obj, sTIDLRT_Tensor_t *in[], sTIDLRT_Tensor_t *out[],
		sTIDLRT_PerfStats_t *stats)
{

    vx_status status = VX_SUCCESS;
    void      *input_buffer = NULL;

    tidlrt_printf("TIDL_RT_OVX: Running Graph ... \n");

    if(stats)
    {
        get_time_u64(&stats->cpIn_time_start);
        set_in_tidlrt_tensor_refs(obj, obj->input_tensors, in);
        set_out_tidlrt_tensor_refs(obj, obj->output_tensors, out);

        status = map_cp_in_tidlrt_tensor_tiovx(obj, obj->context, obj->config, obj->input_tensors, input_buffer, in);
        status = memset_out_tensor_tidlrt_tiovx(obj, obj->config, obj->output_tensors, out);
        get_time_u64(&stats->cpIn_time_end);
        tidlrt_printf("TIDL_RT_OVX: memset_out_tensor_tidlrt_tiovx  ... Done.\n");

        if(status==VX_SUCCESS)
        {
            get_time_u64(&stats->proc_time_start);
            status = vxProcessGraph(obj->graph);
            get_time_u64(&stats->proc_time_end);

            tidlrt_printf("TIDL_RT_OVX: vxProcessGraph  ... Done.\n");
        }


        if(status==VX_SUCCESS)
        {
            get_time_u64(&stats->cpOut_time_start);
            status = map_cp_out_tensor_tidlrt_tiovx(obj, obj->config, obj->output_tensors, out);
            get_time_u64(&stats->cpOut_time_end);
            tidlrt_printf("TIDL_RT_OVX: map_cp_out_tensor_tidlrt_tiovx  ... Done.\n");
        }
    }
    else
    {
        set_in_tidlrt_tensor_refs(obj, obj->input_tensors, in);
        set_out_tidlrt_tensor_refs(obj, obj->output_tensors, out);
        status = map_cp_in_tidlrt_tensor_tiovx(obj, obj->context, obj->config, obj->input_tensors, input_buffer, in);
        if(status==VX_SUCCESS)
        {
            status = memset_out_tensor_tidlrt_tiovx(obj, obj->config, obj->output_tensors, out);
        }
        if(status==VX_SUCCESS)
        {
            status = vxProcessGraph(obj->graph);
        }
        if(status==VX_SUCCESS)
        {
            status = map_cp_out_tensor_tidlrt_tiovx(obj, obj->config, obj->output_tensors, out);
        }
    }

    if(status!=VX_SUCCESS)
    {
        printf("ERROR: Running TIDL graph ... Failed !!!\n");
    }
    tidlrt_printf("TIDL_RT_OVX: Ran TIDL graph ... Done.\n");

    // if(obj->traceWriteLevel) {
        // tivx_utils_tidl_trace_write(obj->traceData, obj->traceBaseName);
    // }
    obj->firstProcessCall = 0;

    return status;
}

static vx_size getTensorDataType(vx_int32 tidl_type)
{
    vx_size openvx_type = VX_TYPE_INVALID;

    if (tidl_type == TIDL_UnsignedChar)
    {
        openvx_type = VX_TYPE_UINT8;
    }
    else if(tidl_type == TIDL_SignedChar)
    {
        openvx_type = VX_TYPE_INT8;
    }
    else if(tidl_type == TIDL_UnsignedShort)
    {
        openvx_type = VX_TYPE_UINT16;
    }
    else if(tidl_type == TIDL_SignedShort)
    {
        openvx_type = VX_TYPE_INT16;
    }
    else if(tidl_type == TIDL_UnsignedWord)
    {
        openvx_type = VX_TYPE_UINT32;
    }
    else if(tidl_type == TIDL_SignedWord)
    {
        openvx_type = VX_TYPE_INT32;
    }
    else if(tidl_type == TIDL_UnsignedDoubleWord)
    {
        openvx_type = VX_TYPE_UINT64;
    }
    else if(tidl_type == TIDL_SignedDoubleWord)
    {
        openvx_type = VX_TYPE_INT64;
    }
    else if(tidl_type == TIDL_SinglePrecFloat)
    {
        openvx_type = VX_TYPE_FLOAT32;
    }

    return openvx_type;
}

static uint32_t getElementSize(uint32_t data_type)
{
    uint32_t elementSize=1;
    if((data_type == VX_TYPE_INT8) || (data_type == VX_TYPE_UINT8))
    {
        elementSize = sizeof(vx_int8);
    }
    else if((data_type == VX_TYPE_INT16) || (data_type == VX_TYPE_UINT16))
    {
        elementSize = sizeof(vx_int16);
    }
    else if((data_type == VX_TYPE_INT32) || (data_type == VX_TYPE_UINT32) || (data_type == VX_TYPE_FLOAT32))
    {
        elementSize = sizeof(vx_int32);
    }
    else if((data_type == VX_TYPE_INT64) || (data_type == VX_TYPE_UINT64))
    {
        elementSize = sizeof(vx_int64);
    }
    return elementSize;
}

static void createInputTensors(AppObj *obj, vx_context context, vx_user_data_object config, vx_tensor *input_tensors)
{
    vx_status status = VX_SUCCESS;
    uint32_t input_buf_size = 0;
    uint32_t capacity;
    uint32_t id;

    vx_map_id map_id_input;

    vx_size    start[MAX_TENSOR_DIMS];
    vx_size    input_strides[MAX_TENSOR_DIMS];
    vx_size    input_sizes[MAX_TENSOR_DIMS];

    sTIDL_IOBufDesc_t *ioBufDesc = &obj->ioBufDesc;

    void *input_buffer;

    for(id = 0; id < ioBufDesc->numInputBuf; id++) {

        input_sizes[0] = ioBufDesc->inWidth[id]  + ioBufDesc->inPadL[id] + ioBufDesc->inPadR[id];
        input_sizes[1] = ioBufDesc->inHeight[id] + ioBufDesc->inPadT[id] + ioBufDesc->inPadB[id];
        input_sizes[2] = ioBufDesc->inNumChannels[id] * ioBufDesc->inDIM1[id] * ioBufDesc->inDIM2[id];
        input_sizes[3] = ioBufDesc->inNumBatches[id];

        vx_size data_type = getTensorDataType(ioBufDesc->inElementType[id]);

        capacity = input_sizes[0] * input_sizes[1] * input_sizes[2] * input_sizes[3];

        start[0] = start[1] = start[2] = start[3] = 0;

        input_strides[0] = sizeof(vx_int8);

        if((data_type == VX_TYPE_INT8) || (data_type == VX_TYPE_UINT8))
        {
            input_strides[0] = sizeof(vx_int8);
        }
        else if((data_type == VX_TYPE_INT16) || (data_type == VX_TYPE_UINT16))
        {
            input_strides[0] = sizeof(vx_int16);
        }
        else if((data_type == VX_TYPE_INT32) || (data_type == VX_TYPE_UINT32) || (data_type == VX_TYPE_FLOAT32))
        {
            input_strides[0] = sizeof(vx_int32);
        }
        else if((data_type == VX_TYPE_INT64) || (data_type == VX_TYPE_UINT64))
        {
            input_strides[0] = sizeof(vx_int64);
        }
        input_strides[1] = input_sizes[0] * input_strides[0];
        input_strides[2] = input_sizes[1] * input_strides[1];
        input_strides[3] = input_sizes[2] * input_strides[2];

        tidlrt_printf("TIDL_RT_OVX: input_sizes[0] = %d, dim = %d padL = %d padR = %d\n", (uint32_t)input_strides[1], ioBufDesc->inWidth[id], ioBufDesc->inPadL[id], ioBufDesc->inPadR[id]);
        tidlrt_printf("TIDL_RT_OVX: input_sizes[1] = %d, dim = %d padT = %d padB = %d\n", (uint32_t)input_strides[2], ioBufDesc->inHeight[id], ioBufDesc->inPadT[id], ioBufDesc->inPadB[id]);
        tidlrt_printf("TIDL_RT_OVX: input_sizes[2] = %d, dim = %d \n", (uint32_t)input_sizes[2], ioBufDesc->inNumChannels[id]);
        tidlrt_printf("TIDL_RT_OVX: input_sizes[3] = %d, dim = %d \n", (uint32_t)input_sizes[3], ioBufDesc->inNumBatches[id]);


        if(data_type != VX_TYPE_INVALID) {
            input_tensors[id] = vxCreateTensor(context, 4, input_sizes, data_type, 0);
            status = tivxMapTensorPatch(input_tensors[id], 4, start, input_sizes, &map_id_input, input_strides, &input_buffer, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);
            if(status != VX_SUCCESS)
            {
              tidlrt_printf("TIDL_RT_OVX: Error in tivxMapTensorPatch\n");
            }
            /* Reset the input buffer, this will take care of padding requirement for TIDL */
            memset(input_buffer, 0, (capacity*input_strides[0]));
            input_buf_size += ioBufDesc->inNumBatches[id]*ioBufDesc->inNumChannels[id]*ioBufDesc->inWidth[id]*ioBufDesc->inHeight[id];
            tidlrt_printf("TIDL_RT_OVX: input_buffer = %p %d\n", input_buffer, capacity);
            tivxUnmapTensorPatch(input_tensors[id], map_id_input);
	}
    }

    return;
}

static void createOutputTensors(AppObj *obj, vx_context context, vx_user_data_object config, vx_tensor *output_tensors)
{
    vx_size output_sizes[MAX_TENSOR_DIMS];

    uint32_t id;

    sTIDL_IOBufDesc_t *ioBufDesc = &obj->ioBufDesc;

    for(id = 0; id < ioBufDesc->numOutputBuf; id++) {

        output_sizes[0] = ioBufDesc->outWidth[id]  + ioBufDesc->outPadL[id] + ioBufDesc->outPadR[id];
        output_sizes[1] = ioBufDesc->outHeight[id] + ioBufDesc->outPadT[id] + ioBufDesc->outPadB[id];
        output_sizes[2] = (ioBufDesc->outNumChannels[id] + ioBufDesc->outPadCh[id] + 1) * ioBufDesc->outDIM1[id] * ioBufDesc->outDIM2[id];
        output_sizes[3] = ioBufDesc->outNumBatches[id];

        vx_size data_type = getTensorDataType(ioBufDesc->outElementType[id]);

        if(data_type != VX_TYPE_INVALID)
            output_tensors[id] = vxCreateTensor(context, 4, output_sizes, data_type, 0);
    }


    return;
}

static inline uint32_t uclamp(float f, uint32_t min_val, uint32_t max_val)
{
    uint32_t val = (uint32_t) f;
    val = val < min_val ? min_val : val;
    val = val > max_val ? max_val : val;
    return val;
}

static inline int32_t clamp(float f, int32_t min_val, int32_t max_val)
{
    int32_t val = (int32_t) f;
    val = val < min_val ? min_val : val;
    val = val > max_val ? max_val : val;
    return val;
}

static inline uint64_t uclamp_64(float f, uint64_t min_val, uint64_t max_val)
{
    uint64_t val = (uint64_t) f;
    val = val < min_val ? min_val : val;
    val = val > max_val ? max_val : val;
    return val;
}

static inline int64_t clamp_64(float f, int64_t min_val, int64_t max_val)
{
    int64_t val = (int64_t) f;
    val = val < min_val ? min_val : val;
    val = val > max_val ? max_val : val;
    return val;
}

static inline uint8_t sat_uint8(float f)
{
    return uclamp(f, 0, 255);
}

static inline int8_t sat_int8(float f)
{
    return clamp(f, -128, 127);
}

static inline uint16_t sat_uint16(float f)
{
    return uclamp(f, 0, 65535);
}

static inline int16_t sat_int16(float f)
{
    return clamp(f, -32768, 32767);
}

static inline uint32_t sat_uint32(float f)
{
    return uclamp_64(f, 0ull, 4294967295ull);
}

static inline int32_t sat_int32(float f)
{
    return clamp_64(f, -2147483648ll, 2147483647ll);
}

static uint32_t get_tidlrt_id(uint32_t id, char *name,
        sTIDLRT_Tensor_t *tensors[], uint32_t num_tensors)
{
    for (uint32_t tidlrt_id = 0; tidlrt_id < num_tensors; tidlrt_id++)
    {
        if (strncmp((char *) tensors[tidlrt_id]->name, name, TIDL_STRING_SIZE)
                == 0)
            return tidlrt_id;
    }
    return id;
}

static vx_status set_in_tidlrt_tensor_refs(AppObj *obj, vx_tensor *input_tensors, sTIDLRT_Tensor_t *in[])
{
    vx_status status = VX_SUCCESS;
    uint32_t id;
    sTIDL_IOBufDesc_t *ioBufDesc = &obj->ioBufDesc;
    uint32_t numPlanes;
    void       *ref1Addr[MAX_TENSOR_DIMS];
    uint32_t    size[MAX_TENSOR_DIMS];

    for (id = 0;  id < ioBufDesc->numInputBuf; id++)
    {
        obj->shared_input_tensors[id] = 0;

        uint32_t tidlrt_id = get_tidlrt_id(id, (char *) ioBufDesc->inDataName[id], in, ioBufDesc->numInputBuf);

        if(is_tidlrt_in_tensor_same(ioBufDesc, in, id, tidlrt_id) && in[tidlrt_id]->memType == TIDLRT_MEM_SHARED)
        {
            status = tivxReferenceExportHandle((vx_reference)input_tensors[id],
                                                    ref1Addr,
                                                    size,
                                                    MAX_TENSOR_DIMS,
                                                    &numPlanes);
            if(numPlanes == 1)
            {
                if(obj->firstProcessCall)
                {
                    tivxMemFree(ref1Addr[0], size[0], TIVX_MEM_EXTERNAL);
                }
                ref1Addr[0] = in[tidlrt_id]->ptr;

                status = tivxReferenceImportHandle((vx_reference)input_tensors[id],
                                                (const void **)ref1Addr,
                                                size,
                                                numPlanes);
                tivxMemBufferUnmap(ref1Addr[0], size[0], (vx_enum)VX_MEMORY_TYPE_HOST, (vx_enum)VX_READ_AND_WRITE);
            }
            else
            {
                printf(" Wrning: Num Planes expected to be 1\n ");
            }
            obj->shared_input_tensors[id] = 1;
        }
    }

    return status;
}

static vx_status set_out_tidlrt_tensor_refs(AppObj *obj, vx_tensor *output_tensors, sTIDLRT_Tensor_t *out[])
{
    vx_status status = VX_SUCCESS;
    uint32_t id;
    sTIDL_IOBufDesc_t *ioBufDesc = &obj->ioBufDesc;
    uint32_t numPlanes;
    void       *ref1Addr[MAX_TENSOR_DIMS];
    uint32_t    size[MAX_TENSOR_DIMS];

    for (id = 0;  id < ioBufDesc->numOutputBuf; id++)
    {
        obj->shared_output_tensors[id] = 0;

        uint32_t tidlrt_id = get_tidlrt_id(id, (char *) ioBufDesc->outDataName[id], out, ioBufDesc->numOutputBuf);

        if(is_tidlrt_out_tensor_same(ioBufDesc, out, id, tidlrt_id) && out[tidlrt_id]->memType == TIDLRT_MEM_SHARED)
        {
            status = tivxReferenceExportHandle((vx_reference)output_tensors[id],
                                                    ref1Addr,
                                                    size,
                                                    MAX_TENSOR_DIMS,
                                                    &numPlanes);
            if(numPlanes == 1)
            {
                if(obj->firstProcessCall)
                {
                    tivxMemFree(ref1Addr[0], size[0], TIVX_MEM_EXTERNAL);
                }
                ref1Addr[0] = out[tidlrt_id]->ptr;

                status = tivxReferenceImportHandle((vx_reference)output_tensors[id],
                                                (const void **)ref1Addr,
                                                size,
                                                numPlanes);
                tivxMemBufferMap(ref1Addr[0], size[0], (vx_enum)VX_MEMORY_TYPE_HOST, (vx_enum)VX_READ_AND_WRITE);
            }
            else
            {
                printf(" Wrning: Num Planes expected to be 1\n ");
            }
            obj->shared_output_tensors[id] = 1;
        }
    }
    return status;
}

static vx_status map_cp_in_tidlrt_tensor_tiovx(AppObj *obj, vx_context context, vx_user_data_object config, vx_tensor *input_tensors, void *input_buffer, sTIDLRT_Tensor_t *in[])
{
    vx_status status = VX_SUCCESS;
    uint32_t input_buf_size = 0;
    uint32_t capacity;
    uint32_t id, tidlrt_id;

    vx_map_id map_id_input;

    vx_size    start[MAX_TENSOR_DIMS];
    vx_size    input_strides[MAX_TENSOR_DIMS];
    vx_size    input_sizes[MAX_TENSOR_DIMS];

    sTIDL_IOBufDesc_t *ioBufDesc = &obj->ioBufDesc;

    for(id = 0; id < ioBufDesc->numInputBuf; id++)
    {
        tidlrt_id = get_tidlrt_id(id, (char *) ioBufDesc->inDataName[id], in, ioBufDesc->numInputBuf);

        if((in[tidlrt_id]->memType != TIDLRT_MEM_SHARED) || (is_tidlrt_in_tensor_same(ioBufDesc, in, id, tidlrt_id) == 0))
        {
            vx_size data_type = getTensorDataType(ioBufDesc->inElementType[id]);

            input_sizes[0] = ioBufDesc->inWidth[id]  + ioBufDesc->inPadL[id] + ioBufDesc->inPadR[id];
            input_sizes[1] = ioBufDesc->inHeight[id] + ioBufDesc->inPadT[id] + ioBufDesc->inPadB[id];
            input_sizes[2] = ioBufDesc->inNumChannels[id] * ioBufDesc->inDIM1[id]* ioBufDesc->inDIM2[id];
            input_sizes[3] = ioBufDesc->inNumBatches[id];
            

            capacity = input_sizes[0] * input_sizes[1] * input_sizes[2] * input_sizes[3];

            start[0] = start[1] = start[2] = start[3] = 0;

            input_strides[0] = sizeof(vx_int8);

            if((data_type == VX_TYPE_INT8) || (data_type == VX_TYPE_UINT8))
            {
                input_strides[0] = sizeof(vx_int8);
            }
            else if((data_type == VX_TYPE_INT16) || (data_type == VX_TYPE_UINT16))
            {
                input_strides[0] = sizeof(vx_int16);
            }
            else if((data_type == VX_TYPE_INT32) || (data_type == VX_TYPE_UINT32) || (data_type == VX_TYPE_FLOAT32))
            {
                input_strides[0] = sizeof(vx_int32);
            }
            else if((data_type == VX_TYPE_INT64) || (data_type == VX_TYPE_UINT64))
            {
                input_strides[0] = sizeof(vx_int64);
            }
            input_strides[1] = input_sizes[0] * input_strides[0];
            input_strides[2] = input_sizes[1] * input_strides[1];
            input_strides[3] = input_sizes[2] * input_strides[2];

            tidlrt_printf("TIDL_RT_OVX: input_sizes[0] = %d, dim = %d padL = %d padR = %d\n", (uint32_t)input_strides[1], ioBufDesc->inWidth[id], ioBufDesc->inPadL[id], ioBufDesc->inPadR[id]);
            tidlrt_printf("TIDL_RT_OVX: input_sizes[1] = %d, dim = %d padT = %d padB = %d\n", (uint32_t)input_strides[2], ioBufDesc->inHeight[id], ioBufDesc->inPadT[id], ioBufDesc->inPadB[id]);
            tidlrt_printf("TIDL_RT_OVX: input_sizes[2] = %d, dim = %d \n", (uint32_t)input_sizes[2], ioBufDesc->inNumChannels[id]);
            tidlrt_printf("TIDL_RT_OVX: input_sizes[3] = %d, dim = %d \n", (uint32_t)input_sizes[3], ioBufDesc->inNumBatches[id]);

            status = tivxMapTensorPatch(input_tensors[id], 4, start, input_sizes, &map_id_input, input_strides, &input_buffer, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);
            /* Reset the input buffer, this will take care of padding requirement for TIDL */
            input_buf_size += ioBufDesc->inNumBatches[id]*ioBufDesc->inNumChannels[id]*ioBufDesc->inWidth[id]*ioBufDesc->inHeight[id] * ioBufDesc->inDIM1[id]* ioBufDesc->inDIM2[id];
            if (tidlrt_debuglevel > 0)
            {
                char *tidl_name = (char *) ioBufDesc->inDataName[id];
                char *ext_name  = (char *) in[tidlrt_id]->name;
                if (strncmp(tidl_name, ext_name, TIDL_STRING_SIZE) != 0)
                    tidlrt_printf("TIDL_RT_OVX: input %d name MISMATCH: %s != %s\n",
                                id, tidl_name, ext_name);
            }
            status = cp_data_in_tidlrt_tensor_tiovx(obj, in, input_buffer, id, tidlrt_id);
            tidlrt_printf("TIDL_RT_OVX: input_buffer = %p %d\n", input_buffer, capacity);
            tivxUnmapTensorPatch(input_tensors[id], map_id_input);
        }
    }

    return status;
}

static vx_status memset_out_tensor_tidlrt_tiovx(AppObj *obj, vx_user_data_object config, vx_tensor *output_tensors, sTIDLRT_Tensor_t *out[])
{
    vx_status status = VX_SUCCESS;
    uint32_t id;
    vx_size output_sizes[MAX_TENSOR_DIMS];
    sTIDL_IOBufDesc_t *ioBufDesc;
    void *output_buffer;
    vx_map_id map_id_output;

    ioBufDesc = &obj->ioBufDesc;
    for(id = 0; id < ioBufDesc->numOutputBuf; id++)
    {
        uint32_t tidlrt_id = get_tidlrt_id(id, (char *) ioBufDesc->outDataName[id], out, ioBufDesc->numOutputBuf);

        if((out[tidlrt_id]->memType != TIDLRT_MEM_SHARED) || (is_tidlrt_out_tensor_same(ioBufDesc, out, id, tidlrt_id) == 0))
        {
            vx_size data_type = getTensorDataType(ioBufDesc->outElementType[id]);
            output_sizes[0] = ioBufDesc->outWidth[id]  + ioBufDesc->outPadL[id] + ioBufDesc->outPadR[id];
            output_sizes[1] = ioBufDesc->outHeight[id] + ioBufDesc->outPadT[id] + ioBufDesc->outPadB[id];
            output_sizes[2] = ioBufDesc->outNumChannels[id] + ioBufDesc->outPadCh[id] + 1;
            output_sizes[3] = ioBufDesc->outNumBatches[id];

            status = vxGetStatus((vx_reference)output_tensors[id]);

            if (VX_SUCCESS == status)
            {
                vx_size output_strides[MAX_TENSOR_DIMS];
                vx_size start[MAX_TENSOR_DIMS];

                start[0] = start[1] = start[2] = start[3] = 0;

                output_strides[0] = sizeof(vx_int8);

                if((data_type == VX_TYPE_INT8) ||
                        (data_type == VX_TYPE_UINT8))
                {
                    output_strides[0] = sizeof(vx_int8);
                }
                else if((data_type == VX_TYPE_INT16) ||
                        (data_type == VX_TYPE_UINT16))
                {
                    output_strides[0] = sizeof(vx_int16);
                }
                else if((data_type == VX_TYPE_FLOAT32))
                {
                    output_strides[0] = sizeof(vx_float32);
                }

                output_strides[1] = output_sizes[0] * output_strides[0];
                output_strides[2] = output_sizes[1] * output_strides[1];
                output_strides[3] = output_sizes[2] * output_strides[2];

                status = tivxMapTensorPatch(output_tensors[id], 4, start, output_sizes, &map_id_output, output_strides, &output_buffer, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);
                {
                int32_t outWidth  = ioBufDesc->outWidth[id]  + ioBufDesc->outPadL[id];
                int32_t outHeight = ioBufDesc->outHeight[id] + ioBufDesc->outPadT[id] + ioBufDesc->outPadB[id];;
                int32_t elementSizeBytes  = output_strides[0];
                if((outWidth * outHeight * elementSizeBytes) <  (16*1024))
                {
                    memset( output_buffer, 0, outWidth * outHeight * elementSizeBytes);
                }
                }
                tivxUnmapTensorPatch(output_tensors[id], map_id_output);
            }
        }
        else
        {
           tidlrt_printf("TIDL_RT_OVX: Shared Mem is used for Input Buff\n");
        }
    }

    return status;
}

static vx_status map_cp_out_tensor_tidlrt_tiovx(AppObj *obj, vx_user_data_object config, vx_tensor *output_tensors, sTIDLRT_Tensor_t *out[])
{
    vx_status status = VX_SUCCESS;
    uint32_t id, tidlrt_id;
    uint32_t elementSize =0;
    vx_size output_sizes[MAX_TENSOR_DIMS];
    sTIDL_IOBufDesc_t *ioBufDesc;
    void *output_buffer;
    vx_map_id map_id_output;
    vx_map_id  map_id_out_args;
    vx_uint32  capacity;
    void *outArgs_buffer = NULL;
    TIDL_outArgs *outArgs = NULL;

    ioBufDesc = &obj->ioBufDesc;

    capacity = sizeof(TIDL_outArgs);
    if (VX_SUCCESS == status)
    {
        status = vxMapUserDataObject(obj->outArgs, 0, capacity, &map_id_out_args,
                (void **)&outArgs_buffer, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0);

        if (VX_SUCCESS == status)
        {
            if(outArgs_buffer)
            {
                outArgs = outArgs_buffer;
                if(obj->traceLogLevel)
                {    
                    TIDLRT_LogMetaData(outArgs, obj->traceBaseName);
                }
            }
            else
            {
                printf("TIDL_RT_OVX: Unable to read outArgs! %d bytes\n", capacity);
            }
        }
    }



    for(id = 0; id < ioBufDesc->numOutputBuf; id++)
    {
        vx_size data_type = getTensorDataType(ioBufDesc->outElementType[id]);
        status = vxGetStatus((vx_reference)obj->output_tensors[id]);

        if (VX_SUCCESS == status)
        {
            elementSize = getElementSize(data_type);
        }
    }

    for(id = 0; id < ioBufDesc->numOutputBuf; id++)
    {
        tidlrt_id = get_tidlrt_id(id, (char *) ioBufDesc->outDataName[id], out, ioBufDesc->numOutputBuf);

        if((out[tidlrt_id]->memType != TIDLRT_MEM_SHARED) || (is_tidlrt_out_tensor_same(ioBufDesc, out, id, tidlrt_id) == 0))
        {
            vx_size data_type = getTensorDataType(ioBufDesc->outElementType[id]);

            output_sizes[0] = ioBufDesc->outWidth[id]  + ioBufDesc->outPadL[id] + ioBufDesc->outPadR[id];
            output_sizes[1] = ioBufDesc->outHeight[id] + ioBufDesc->outPadT[id] + ioBufDesc->outPadB[id];
            output_sizes[2] = (ioBufDesc->outNumChannels[id] + ioBufDesc->outPadCh[id] + 1) * ioBufDesc->outDIM1[id] * ioBufDesc->outDIM2[id];
            output_sizes[3] = ioBufDesc->outNumBatches[id];

            status = vxGetStatus((vx_reference)output_tensors[id]);

            if (VX_SUCCESS == status)
            {
                vx_size output_strides[MAX_TENSOR_DIMS];
                vx_size start[MAX_TENSOR_DIMS];

                start[0] = start[1] = start[2] = start[3] = 0;

                output_strides[0] = sizeof(vx_int8);

                if((data_type == VX_TYPE_INT8) ||
                        (data_type == VX_TYPE_UINT8))
                {
                    output_strides[0] = sizeof(vx_int8);
                }
                else if((data_type == VX_TYPE_INT16) ||
                        (data_type == VX_TYPE_UINT16))
                {
                    output_strides[0] = sizeof(vx_int16);
                }
                else if((data_type == VX_TYPE_FLOAT32))
                {
                    output_strides[0] = sizeof(vx_float32);
                }

                output_strides[1] = output_sizes[0] * output_strides[0];
                output_strides[2] = output_sizes[1] * output_strides[1];
                output_strides[3] = output_sizes[2] * output_strides[2];

                status = tivxMapTensorPatch(output_tensors[id], 4, start, output_sizes, &map_id_output, output_strides, &output_buffer, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
                status = cp_data_out_tensor_tidlrt_tiovx(obj, out, output_buffer, id, elementSize, data_type, outArgs->scale[id], tidlrt_id);

                tidlrt_printf("TIDL_RT_OVX: elementSize = %d, OutElementType = %d scale[%d] = %f, rtelemType = %d, rtZp = %d, rtScale = %f\n",
                        elementSize, data_type, id, outArgs->scale[id],
                        out[tidlrt_id]->elementType, out[tidlrt_id]->zeroPoint,
                        out[tidlrt_id]->scale);

                tivxUnmapTensorPatch(output_tensors[id], map_id_output);
            }

            tidlrt_printf("TIDL_RT_OVX: output_sizes[0] = %d, outWidht = %d padL = %d padR = %d\n", (uint32_t)output_sizes[0], ioBufDesc->outWidth[id], ioBufDesc->outPadL[id], ioBufDesc->outPadR[id]);
            tidlrt_printf("TIDL_RT_OVX: output_sizes[1] = %d, outHeight = %d padT = %d padB = %d\n", (uint32_t)output_sizes[1], ioBufDesc->outHeight[id], ioBufDesc->outPadT[id], ioBufDesc->outPadB[id]);
            tidlrt_printf("TIDL_RT_OVX: output_sizes[2] = %d, outNumChannels = %d \n", (uint32_t)output_sizes[2], ioBufDesc->outNumChannels[id]);
            tidlrt_printf("TIDL_RT_OVX: output_sizes[3] = %d, outNumBatches = %d \n", (uint32_t)output_sizes[3], ioBufDesc->outNumBatches[id]);
            }
            else
            {
                tidlrt_printf("TIDL_RT_OVX: Shared Mem is used for Output Buff\n");
            }
        }
    if(outArgs)
    {
        vxUnmapUserDataObject(obj->outArgs, map_id_out_args);
    }

    return status;
}
