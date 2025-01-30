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

#include <stdio.h>
#include <string.h>
#include <utils/console_io/include/app_log.h>
#include <utils/timer/include/app_timer.h>
#include <utils/misc/include/app_misc.h>
#include <utils/ipc/include/app_ipc.h>
#include <utils/rtos/include/app_rtos.h>
#include <HwiP.h>

#if !defined(MCU_PLUS_SDK)
#include <ipc/ipc.h>
#include <osal.h>
#else
#include <ipc_rpmsg.h>
#include <ipc_notify.h>
#include <CacheP.h>
#include <ClockP.h>
#include <soc.h>
#include <utils/ipc/include/mcu_sdk_ipc.h>

/* Number of a buffers in a VRING, i.e depth of VRING queue */
#define IPC_RPMESSAGE_NUM_VRING_BUF       (256U)
/* Max size of a buffer in a VRING */
#define IPC_RPMESSAGE_MAX_VRING_BUF_SIZE  (512U)
/* Size of each VRING is
 *     2 x number of buffers x size of each buffer
 */
#define IPC_RPMESSAGE_VRING_SIZE          (2 * IPC_RPMESSAGE_NUM_VRING_BUF * IPC_RPMESSAGE_MAX_VRING_BUF_SIZE)
#endif

/* #define APP_IPC_DEBUG */

#define APP_IPC_MAX_TASK_NAME       (12u)

#define IPC_RPMESSAGE_OBJ_SIZE      (256u)
#define IPC_RPMESSAGE_MSG_SIZE      (496U + 32U)
#define IPC_RPMESSAGE_BUF_SIZE(n)   (IPC_RPMESSAGE_MSG_SIZE*(n)+IPC_RPMESSAGE_OBJ_SIZE)

#if !defined (MCU_PLUS_SDK)
#define IPC_VRING_OBJ_SIZE          (256u)
#define APP_IPC_VQ_OBJ_MEM_SIZE     (IPC_MAX_PROCS*IPC_VRING_OBJ_SIZE)
static uint8_t g_app_vq_obj_mem[APP_IPC_VQ_OBJ_MEM_SIZE] __attribute__ ((aligned(1024)));

#define APP_IPC_RPMESSAGE_CTRL_PARAMS_NUM_BUF   (16u)
#define APP_IPC_RPMESSAGE_CTRL_PARAMS_BUF_SIZE  IPC_RPMESSAGE_BUF_SIZE(APP_IPC_RPMESSAGE_CTRL_PARAMS_NUM_BUF)
static uint8_t g_app_rpmessage_ctrl_params_buf[APP_IPC_RPMESSAGE_CTRL_PARAMS_BUF_SIZE] __attribute__ ((aligned(1024)));

#define APP_IPC_RPMESSAGE_RPMSG_TX_NUM_BUF   (16u)
#define APP_IPC_RPMESSAGE_RPMSG_TX_BUF_SIZE  IPC_RPMESSAGE_BUF_SIZE(APP_IPC_RPMESSAGE_RPMSG_TX_NUM_BUF)
static uint8_t g_app_rpmessage_rpmsg_tx_buf[APP_IPC_CPU_MAX][APP_IPC_RPMESSAGE_RPMSG_TX_BUF_SIZE] __attribute__ ((aligned(1024)));

#define APP_IPC_RPMESSAGE_RPMSG_RX_NUM_BUF   (256u)
#define APP_IPC_RPMESSAGE_RPMSG_RX_BUF_SIZE  IPC_RPMESSAGE_BUF_SIZE(APP_IPC_RPMESSAGE_RPMSG_RX_NUM_BUF)
static uint8_t g_app_rpmessage_rpmsg_rx_buf[APP_IPC_RPMESSAGE_RPMSG_RX_BUF_SIZE] __attribute__ ((aligned(1024)));
#endif

/* IMPORTANT NOTE: For C7x,
 * - stack size and stack ptr MUST be 8KB aligned
 * - AND min stack size MUST be 16KB
 * - AND stack assigned for task context is "size - 8KB"
 *       - 8KB chunk for the stack area is used for interrupt handling in this task context
 */
#if defined(R5F)
#define APP_IPC_RPMESSAGE_RX_TASK_STACK_SIZE   (4*1024u)
#else
#define APP_IPC_RPMESSAGE_RX_TASK_STACK_SIZE   (32*1024u)
#endif

#define APP_IPC_RPMESSAGE_RX_TASK_PRI          (10u)

#if defined(R5F) && defined(SAFERTOS)
#define APP_IPC_RPMESSAGE_RX_TASK_ALIGNMENT    APP_IPC_RPMESSAGE_RX_TASK_STACK_SIZE
#else
#define APP_IPC_RPMESSAGE_RX_TASK_ALIGNMENT    (8192u)
#endif

static uint8_t g_app_rpmessage_rx_task_stack[APP_IPC_RPMESSAGE_RX_TASK_STACK_SIZE]
__attribute__ ((section(".bss:taskStackSection")))
__attribute__ ((aligned(APP_IPC_RPMESSAGE_RX_TASK_ALIGNMENT)))
    ;

/* IMPORTANT NOTE: For C7x,
 * - stack size and stack ptr MUST be 8KB aligned
 * - AND min stack size MUST be 16KB
 * - AND stack assigned for task context is "size - 8KB"
 *       - 8KB chunk for the stack area is used for interrupt handling in this task context
 */
#if defined(R5F)
#define APP_IPC_RPMESSAGE_CTRL_TASK_STACK_SIZE   (4*1024u)
#else
#define APP_IPC_RPMESSAGE_CTRL_TASK_STACK_SIZE   (32*1024u)
#endif

#if defined(R5F) && defined(SAFERTOS)
#define APP_IPC_RPMESSAGE_CTRL_TASK_ALIGNMENT    APP_IPC_RPMESSAGE_CTRL_TASK_STACK_SIZE
#else
#define APP_IPC_RPMESSAGE_CTRL_TASK_ALIGNMENT    (8192u)
#endif

static uint8_t g_app_rpmessage_ctrl_task_stack[APP_IPC_RPMESSAGE_CTRL_TASK_STACK_SIZE]
__attribute__ ((section(".bss:taskStackSection")))
__attribute__ ((aligned(APP_IPC_RPMESSAGE_CTRL_TASK_ALIGNMENT)))
    ;

#if defined(FREERTOS) || defined(SAFERTOS)

typedef struct
{
    /* IPC trace buffer address */
    uint8_t *traceBufAddr;

    /* IPC trace buffer size */
    uint32_t traceBufSize;

    /* Timestamp of last IPC trace buffer flush */
    uint64_t traceBufLastFlushTicksInUsecs;

} IpcObj;

static IpcObj gIpcObj;

#define IPC_TRACEBUF_TASK_STACKSIZE          (1U * 1024U)
#define IPC_TRACEBUF_FLUSH_PERIOD_IN_MSEC    (500U)
#define IPC_TRACEBUF_SIZE                    (0x80000U)

extern char Ipc_traceBuffer[IPC_TRACEBUF_SIZE];

static uint8_t gIpcTraceBufFlushBuf[IPC_TRACEBUF_TASK_STACKSIZE] __attribute__ ((section(".bss:taskStackSection"))) __attribute__((aligned(32)));

static void traceBufFlush(void* arg0, void* arg1);

static void traceBufCacheWb(void);

#endif

typedef struct {

    app_ipc_init_prm_t prm;
    uint32_t rpmsg_tx_endpt[APP_IPC_CPU_MAX];
    app_ipc_notify_handler_f ipc_notify_handler;
    app_rtos_task_handle_t task_handle;

#if !defined(MCU_PLUS_SDK)
    RPMessage_Handle rpmsg_tx_handle[APP_IPC_CPU_MAX];
    RPMessage_Handle rpmsg_rx_handle;
#else
    RPMessage_Object rpmsg_tx_handle[APP_IPC_CPU_MAX];
    RPMessage_Object rpmsg_rx_handle;
#endif

    uint32_t task_stack_size;
    uint8_t *task_stack;
    uint32_t task_pri;
    uint8_t  rpmsg_rx_msg_buf[IPC_RPMESSAGE_MSG_SIZE] __attribute__ ((aligned(1024)));
    char     task_name[APP_IPC_MAX_TASK_NAME];
} app_ipc_obj_t;

static app_ipc_obj_t g_app_ipc_obj;

#if defined (SOC_J721E)
static uint32_t g_app_to_ipc_cpu_id[APP_IPC_CPU_MAX] =
{
    IPC_MPU1_0,
    IPC_MCU1_0,
    IPC_MCU1_1,
    IPC_MCU2_0,
    IPC_MCU2_1,
    IPC_MCU3_0,
    IPC_MCU3_1,
    IPC_C66X_1,
    IPC_C66X_2,
    IPC_C7X_1
};

static uint32_t g_ipc_to_app_cpu_id[IPC_MAX_PROCS] =
{
    APP_IPC_CPU_MPU1_0,
    APP_IPC_CPU_MCU1_0,
    APP_IPC_CPU_MCU1_1,
    APP_IPC_CPU_MCU2_0,
    APP_IPC_CPU_MCU2_1,
    APP_IPC_CPU_MCU3_0,
    APP_IPC_CPU_MCU3_1,
    APP_IPC_CPU_C6x_1,
    APP_IPC_CPU_C6x_2,
    APP_IPC_CPU_C7x_1
};
#endif

#if defined (SOC_J721S2)
static uint32_t g_app_to_ipc_cpu_id[APP_IPC_CPU_MAX] =
{
    IPC_MPU1_0,
    IPC_MCU1_0,
    IPC_MCU1_1,
    IPC_MCU2_0,
    IPC_MCU2_1,
    IPC_MCU3_0,
    IPC_MCU3_1,
    IPC_C7X_1,
    IPC_C7X_2
};

static uint32_t g_ipc_to_app_cpu_id[IPC_MAX_PROCS] =
{
    APP_IPC_CPU_MPU1_0,
    APP_IPC_CPU_MCU1_0,
    APP_IPC_CPU_MCU1_1,
    APP_IPC_CPU_MCU2_0,
    APP_IPC_CPU_MCU2_1,
    APP_IPC_CPU_MCU3_0,
    APP_IPC_CPU_MCU3_1,
    APP_IPC_CPU_C7x_1,
    APP_IPC_CPU_C7x_2
};
#endif

#if defined (SOC_J784S4)
static uint32_t g_app_to_ipc_cpu_id[APP_IPC_CPU_MAX] =
{
    IPC_MPU1_0,
    IPC_MCU1_0,
    IPC_MCU1_1,
    IPC_MCU2_0,
    IPC_MCU2_1,
    IPC_MCU3_0,
    IPC_MCU3_1,
    IPC_MCU4_0,
    IPC_MCU4_1,
    IPC_C7X_1,
    IPC_C7X_2,
    IPC_C7X_3,
    IPC_C7X_4
};

static uint32_t g_ipc_to_app_cpu_id[IPC_MAX_PROCS] =
{
    APP_IPC_CPU_MPU1_0,
    APP_IPC_CPU_MCU1_0,
    APP_IPC_CPU_MCU1_1,
    APP_IPC_CPU_MCU2_0,
    APP_IPC_CPU_MCU2_1,
    APP_IPC_CPU_MCU3_0,
    APP_IPC_CPU_MCU3_1,
    APP_IPC_CPU_MCU4_0,
    APP_IPC_CPU_MCU4_1,
    APP_IPC_CPU_C7x_1,
    APP_IPC_CPU_C7x_2,
    APP_IPC_CPU_C7x_3,
    APP_IPC_CPU_C7x_4
};
#endif

#if defined (SOC_AM62A)
#if !defined (MCU_PLUS_SDK)
static uint32_t g_app_to_ipc_cpu_id[APP_IPC_CPU_MAX] =
{
    IPC_MPU1_0,
    IPC_MCU1_0,
    IPC_C7X_1
};

static uint32_t g_ipc_to_app_cpu_id[IPC_MAX_PROCS] =
{
    APP_IPC_CPU_MPU1_0,
    APP_IPC_CPU_MCU1_0,
    APP_IPC_CPU_C7x_1
};
#else
#define APP_MCUSDK_IPC_CPU_INVALID (0xFFFFFFFF)
static uint32_t g_app_to_ipc_cpu_id[APP_IPC_CPU_MAX] =
{
    CSL_CORE_ID_A53SS0_0,
    CSL_CORE_ID_R5FSS0_0,
    CSL_CORE_ID_C75SS0_0
};

static uint32_t g_ipc_to_app_cpu_id[CSL_CORE_ID_MAX] =
{
    APP_MCUSDK_IPC_CPU_INVALID,
    APP_IPC_CPU_MCU1_0,
    APP_IPC_CPU_MPU1_0,
    APP_MCUSDK_IPC_CPU_INVALID,
    APP_MCUSDK_IPC_CPU_INVALID,
    APP_MCUSDK_IPC_CPU_INVALID,
    APP_MCUSDK_IPC_CPU_INVALID,
    APP_IPC_CPU_C7x_1
};
#endif
#endif

#if defined (SOC_J722S)
#define APP_MCUSDK_IPC_CPU_INVALID (0xFFFFFFFF)
static uint32_t g_app_to_ipc_cpu_id[APP_IPC_CPU_MAX] =
{
    CSL_CORE_ID_A53SS0_0,
    CSL_CORE_ID_WKUP_R5FSS0_0,
    CSL_CORE_ID_MAIN_R5FSS0_0,
    CSL_CORE_ID_C75SS0_0,
    CSL_CORE_ID_C75SS1_0,
    CSL_CORE_ID_MCU_R5FSS0_0
};

static uint32_t g_ipc_to_app_cpu_id[CSL_CORE_ID_MAX] =
{
    APP_IPC_CPU_MCU_R5F,
    APP_IPC_CPU_MCU1_0,
    APP_IPC_CPU_MCU2_0,
    APP_IPC_CPU_C7x_1,
    APP_IPC_CPU_C7x_2,
    APP_IPC_CPU_MPU1_0,
    APP_MCUSDK_IPC_CPU_INVALID,
    APP_MCUSDK_IPC_CPU_INVALID,
    APP_MCUSDK_IPC_CPU_INVALID,
    APP_MCUSDK_IPC_CPU_INVALID
};
#endif

#if !defined(MCU_PLUS_SDK)
static void appIpcRpmsgRxHandler(RPMessage_Handle rpmsg_handle,
                        void *arg, void *data,
                        uint16_t len, uint32_t src_cpu_id,
                        uint16_t src_endpt, uint16_t dst_endpt)
#else
static void appIpcRpmsgRxHandler(RPMessage_Object* rpmsg_handle,
                        void *arg, void *data,
                        uint16_t len, uint32_t src_cpu_id,
                        uint16_t src_endpt, uint16_t dst_endpt)
#endif
{
    uint32_t app_cpu_id, payload;
    app_ipc_obj_t *obj = arg;

    if(src_cpu_id<IPC_MAX_PROCS && len == sizeof(payload))
    {
        app_cpu_id = g_ipc_to_app_cpu_id[src_cpu_id];
        payload = *(uint32_t*)data;

        #ifdef APP_IPC_DEBUG
        appLogPrintf("IPC: RX: %s (port %d) -> %s (port %d) msg = 0x%08x\n",
            Ipc_mpGetName(src_cpu_id),
            (uint32_t)src_endpt,
            Ipc_mpGetSelfName(),
            (uint32_t)dst_endpt,
            payload);
        #endif

        if((payload & 0xFFFF0000) == 0xDEAD0000)
        {
            /* echo this message back to src */
            appIpcSendNotifyPort(app_cpu_id, payload, src_endpt);
        }
        else
        {
            if(obj->ipc_notify_handler)
            {
                obj->ipc_notify_handler(app_cpu_id, payload);
            }
        }
    }
}

static void appIpcRpmsgRxTaskMain(void *arg0, void *arg1)
{
    app_ipc_obj_t *obj = &g_app_ipc_obj;
    uint32_t reply_endpt;
#if !defined(MCU_PLUS_SDK)
    uint32_t done = 0, src_cpu_id;
    uint16_t len;
#else
    uint32_t done = 0;
    uint16_t len, src_cpu_id;
#endif
    int32_t status = 0;

    appUtilsTaskInit();

    while(!done)
    {
        len = 0;
	#if !defined(MCU_PLUS_SDK)
        status = RPMessage_recv(obj->rpmsg_rx_handle,
                        &obj->rpmsg_rx_msg_buf,
                        &len,
                        &reply_endpt,
                        &src_cpu_id,
                        APP_RTOS_SEMAPHORE_WAIT_FOREVER
                        );

        if(status == IPC_E_UNBLOCKED)
        {
            done = 1;
        }
        if(status == IPC_SOK)
        {
            appIpcRpmsgRxHandler(obj->rpmsg_rx_handle,
                        obj,
                        obj->rpmsg_rx_msg_buf,
                        len,
                        src_cpu_id,
                        reply_endpt,
                        obj->prm.tiovx_rpmsg_port_id);
        }
	#else
        len = sizeof(obj->rpmsg_rx_msg_buf);
        status = RPMessage_recv(&obj->rpmsg_rx_handle,
                            &obj->rpmsg_rx_msg_buf,
                            &len,
                            &src_cpu_id,
                            &reply_endpt,
                            SystemP_WAIT_FOREVER
                            );
        if(status == SystemP_SUCCESS)
        {
            appIpcRpmsgRxHandler(&obj->rpmsg_rx_handle,
                        obj,
                        obj->rpmsg_rx_msg_buf,
                        len,
                        src_cpu_id,
                        reply_endpt,
                        obj->prm.tiovx_rpmsg_port_id);
        }
	#endif
    }
}

static int32_t appIpcCreateRpmsgRxTask(app_ipc_obj_t *obj)
{
    app_rtos_task_params_t rtos_task_prms;
    int32_t status = 0;

    appRtosTaskParamsInit(&rtos_task_prms);

    rtos_task_prms.stacksize = obj->task_stack_size;
    rtos_task_prms.stack = obj->task_stack;
    rtos_task_prms.priority = obj->task_pri;
    rtos_task_prms.arg0 = NULL;
    rtos_task_prms.arg1 = NULL;
    rtos_task_prms.name = (const char*)&obj->task_name[0];
    rtos_task_prms.taskfxn = &appIpcRpmsgRxTaskMain;

    (void)strncpy(obj->task_name, "IPC_RX", APP_IPC_MAX_TASK_NAME);
    obj->task_name[APP_IPC_MAX_TASK_NAME-1] = 0;

    obj->task_handle = (void*)appRtosTaskCreate(&rtos_task_prms);
    if(obj->task_handle==NULL)
    {
        appLogPrintf("IPC: ERROR: Unable to create RX task \n");
        status = -1;
    }
    return status;
}

static void appIpcDeleteRpmsgRxTask(app_ipc_obj_t *obj)
{
#if !defined(MCU_PLUS_SDK)
    uint32_t sleep_time = 16U;

    RPMessage_unblock(obj->rpmsg_rx_handle);

    /* confirm task termination */
    while ( ! appRtosTaskIsTerminated(obj->task_handle) )
    {
        appLogWaitMsecs(sleep_time);
        sleep_time >>= 1U;
        if (sleep_time == 0U)
        {
            /* Force delete after timeout */
            break;
        }
    }
#else
    RPMessage_unblock(&obj->rpmsg_rx_handle);
#endif

    appRtosTaskDelete(&obj->task_handle);
}

void appIpcInitPrmSetDefault(app_ipc_init_prm_t *prm)
{
    uint32_t cpu_id = 0;

    prm->num_cpus = 0;
    for(cpu_id=0; cpu_id<APP_IPC_CPU_MAX; cpu_id++)
    {
        prm->enabled_cpu_id_list[cpu_id] = APP_IPC_CPU_INVALID;
    }
    prm->tiovx_rpmsg_port_id = APP_IPC_TIOVX_RPMSG_PORT_ID;
    prm->tiovx_obj_desc_mem = NULL;
    prm->tiovx_obj_desc_mem_size = 0;
    prm->ipc_vring_mem = NULL;
    prm->ipc_vring_mem_size = 0;
    prm->tiovx_log_rt_mem = NULL;
    prm->tiovx_log_rt_mem_size = 0;
    prm->self_cpu_id = APP_IPC_CPU_INVALID;
    prm->ipc_resource_tbl = NULL;
    prm->enable_tiovx_ipc_announce = 1;
}

uint32_t getVringIndexPDK(uint32_t numProc, uint32_t selfId, uint32_t remoteId)
{
    uint32_t cnt = 0, a , b, i;

    if(remoteId > selfId){
        a = selfId;
        b = remoteId;
    }
    else{
        a = remoteId;
        b = selfId;
    }

    for(i = 0; i < a; i++)
    {
        cnt += (numProc - i - 1);
    }

    cnt += (b - a - 1);
    cnt *= 4;

    if(remoteId > selfId) {
        return cnt / 2;
    }

    return (cnt / 2 + 1);
}

int32_t appIpcInit(app_ipc_init_prm_t *prm)
{
    int32_t status = 0;
    uint32_t cpu_id = 0;
    app_ipc_obj_t *obj = &g_app_ipc_obj;

    appLogPrintf("IPC: Init ... !!!\n");

    obj->prm = *prm;

#if !defined(MCU_PLUS_SDK)
    for(cpu_id=0; cpu_id<APP_IPC_CPU_MAX; cpu_id++)
    {
        obj->rpmsg_tx_handle[cpu_id] = NULL;
    }
    obj->task_handle = NULL;
    obj->rpmsg_rx_handle = NULL;
#endif

    obj->ipc_notify_handler = NULL;
    obj->task_stack = g_app_rpmessage_rx_task_stack;
    obj->task_stack_size = APP_IPC_RPMESSAGE_RX_TASK_STACK_SIZE;
    obj->task_pri = APP_IPC_RPMESSAGE_RX_TASK_PRI;

    if(prm->num_cpus>APP_IPC_CPU_MAX)
    {
        appLogPrintf("IPC: ERROR: Invalid number of CPUs !!!\n");
        status = -1;
    }
    if(prm->tiovx_obj_desc_mem==NULL||prm->tiovx_obj_desc_mem_size==0)
    {
        appLogPrintf("IPC: ERROR: Invalid tiovx obj desc memory address or size !!!\n");
        status = -1;
    }
    if(prm->ipc_vring_mem==NULL||prm->ipc_vring_mem_size==0)
    {
        appLogPrintf("IPC: ERROR: Invalid ipc vring memory address or size !!!\n");
        status = -1;
    }
    if(prm->self_cpu_id>=APP_IPC_CPU_MAX)
    {
        appLogPrintf("IPC: ERROR: Invalid self cpu id !!!\n");
        status = -1;
    }

#if !defined(MCU_PLUS_SDK)
    if(APP_IPC_VQ_OBJ_MEM_SIZE < Ipc_getVqObjMemoryRequiredPerCore()*IPC_MAX_PROCS)
    {
        appLogPrintf("IPC: ERROR: APP_IPC_VQ_OBJ_MEM_SIZE is less than Ipc_getVqObjMemoryRequiredPerCore()*IPC_MAX_PROCS (%d < %d) !!!\n",
            APP_IPC_VQ_OBJ_MEM_SIZE,
            Ipc_getVqObjMemoryRequiredPerCore()*IPC_MAX_PROCS
            );
        status = -1;
    }
#endif

    if(status==0)
    {
        for(cpu_id=0; cpu_id<prm->num_cpus; cpu_id++)
        {
            if(prm->enabled_cpu_id_list[cpu_id]>=APP_IPC_CPU_MAX)
            {
                appLogPrintf("IPC: ERROR: Invalid cpu id in enabled_cpu_id_list @ index %d !!!\n", cpu_id);
                status = -1;
            }
        }
    }

#if !defined (MCU_PLUS_SDK)
    if(status==0)
    {
        status = Ipc_init(NULL);
        if(status!=0)
        {
            appLogPrintf("IPC: ERROR: Ipc_init failed !!!\n");
        }
    }
    if(status==0)
    {
        uint32_t ipc_proc_list[IPC_MAX_PROCS];
        uint32_t ipc_num_proc = 0;

        for(cpu_id=0; cpu_id<prm->num_cpus; cpu_id++)
        {
            if(prm->enabled_cpu_id_list[cpu_id] != prm->self_cpu_id)
            {
                ipc_proc_list[ipc_num_proc] = g_app_to_ipc_cpu_id[prm->enabled_cpu_id_list[cpu_id]];
                ipc_num_proc++;
            }
        }

        /* +1 because self CPU is also participating in IPC */
        appLogPrintf("IPC: %d CPUs participating in IPC !!!\n", ipc_num_proc+1);

        status = Ipc_mpSetConfig(
                    g_app_to_ipc_cpu_id[prm->self_cpu_id],
                    ipc_num_proc,
                    ipc_proc_list
                    );
        if(status!=0)
        {
            appLogPrintf("IPC: ERROR: Ipc_mpSetConfig failed !!!\n");
        }

        if((status==0) && prm->ipc_resource_tbl != NULL)
        {
            /* If A72 remote core is running Linux OS, then load resource table */
            Ipc_loadResourceTable((void*)prm->ipc_resource_tbl);

            appLogPrintf("IPC: Waiting for HLOS to be ready ... !!!\n");
            /* Wait for Linux VDev ready... */
            for(cpu_id=0; cpu_id<ipc_num_proc; cpu_id++)
            {
                while(!Ipc_isRemoteReady(ipc_proc_list[cpu_id]))
                {
                    //appRtosTaskSleep(100);
                }
            }
            appLogPrintf("IPC: HLOS is ready !!!\n");
        }
    }

    if(status==0)
    {
        Ipc_VirtIoParams  vq_prm;

        vq_prm.vqObjBaseAddr = g_app_vq_obj_mem;
        vq_prm.vqBufSize     = APP_IPC_VQ_OBJ_MEM_SIZE;
        vq_prm.vringBaseAddr = prm->ipc_vring_mem;
        vq_prm.vringBufSize  = prm->ipc_vring_mem_size;
        vq_prm.timeoutCnt    = 100;
        status = Ipc_initVirtIO(&vq_prm);
        if(status!=0)
        {
            appLogPrintf("IPC: ERROR: Ipc_initVirtIO failed !!!\n");
        }
    }
    if(status==0)
    {
        RPMessage_Params cntrl_prm;

        /* Initialize the param */
        RPMessageParams_init(&cntrl_prm);

        /* Set memory for HeapMemory for control task */
        cntrl_prm.buf = g_app_rpmessage_ctrl_params_buf;
        cntrl_prm.bufSize = APP_IPC_RPMESSAGE_CTRL_PARAMS_BUF_SIZE;
        cntrl_prm.numBufs = APP_IPC_RPMESSAGE_CTRL_PARAMS_NUM_BUF;
        cntrl_prm.stackSize   = APP_IPC_RPMESSAGE_CTRL_TASK_STACK_SIZE;
        cntrl_prm.stackBuffer = &g_app_rpmessage_ctrl_task_stack[0];
        status = RPMessage_init(&cntrl_prm);
        if(status!=0)
        {
            appLogPrintf("IPC: ERROR: RPMessage_init failed !!!\n");
        }
    }
    if(status==0)
    {
        RPMessage_Params rpmsg_prm;

        for(cpu_id=0; cpu_id<APP_IPC_CPU_MAX; cpu_id++)
        {
            if(appIpcIsCpuEnabled(cpu_id))
            {
                RPMessageParams_init(&rpmsg_prm);

                rpmsg_prm.requestedEndpt = RPMESSAGE_ANY;
                rpmsg_prm.buf = g_app_rpmessage_rpmsg_tx_buf[cpu_id];
                rpmsg_prm.bufSize = APP_IPC_RPMESSAGE_RPMSG_TX_BUF_SIZE;
                rpmsg_prm.numBufs = APP_IPC_RPMESSAGE_RPMSG_TX_NUM_BUF;

                obj->rpmsg_tx_handle[cpu_id] =
                    RPMessage_create(&rpmsg_prm, &obj->rpmsg_tx_endpt[cpu_id]);

                if(obj->rpmsg_tx_handle[cpu_id]==NULL)
                {
                    appLogPrintf("IPC: ERROR: Unable to create rpmessage tx handle for cpu %d !!!\n", cpu_id);
                    status = -1;
                }
            }
            if(status!=0)
                break;
        }
    }
    if(status==0)
    {
        RPMessage_Params rpmsg_prm;
        uint32_t rx_endpt;

        RPMessageParams_init(&rpmsg_prm);

        rpmsg_prm.requestedEndpt = prm->tiovx_rpmsg_port_id;
        rpmsg_prm.buf = g_app_rpmessage_rpmsg_rx_buf;
        rpmsg_prm.bufSize = APP_IPC_RPMESSAGE_RPMSG_RX_BUF_SIZE;
        rpmsg_prm.numBufs = APP_IPC_RPMESSAGE_RPMSG_RX_NUM_BUF;

        obj->rpmsg_rx_handle =
            RPMessage_create(&rpmsg_prm, &rx_endpt);

        if(obj->rpmsg_rx_handle==NULL)
        {
            appLogPrintf("IPC: ERROR: Unable to create rpmessage rx handle !!!\n");
            status = -1;
        }

        /* NOTE: RPMessage_setCallback is not yet implemented */
    }
#else
    if(status==0)
    {
        IpcNotify_Params notifyParams;
        uint32_t ipc_num_proc = 0;

        /* initialize parameters to default */
        IpcNotify_Params_init(&notifyParams);

        /* specify the core on which this API is called */
        notifyParams.selfCoreId = g_app_to_ipc_cpu_id[prm->self_cpu_id];

        /* list the cores that will do IPC Notify with this core
        * Make sure to NOT list 'self' core in the list below
        */

        for(cpu_id=0; cpu_id<prm->num_cpus; cpu_id++)
        {
            if(prm->enabled_cpu_id_list[cpu_id] != prm->self_cpu_id)
            {
                notifyParams.coreIdList[ipc_num_proc] = g_app_to_ipc_cpu_id[prm->enabled_cpu_id_list[cpu_id]];
                ipc_num_proc++;
            }
        }

        /* +1 because self CPU is also participating in IPC */
        appLogPrintf("IPC: %d CPUs participating in IPC !!!\n", ipc_num_proc+1);

        notifyParams.numCores = ipc_num_proc;

        if (prm->ipc_resource_tbl != NULL)
        {
            notifyParams.linuxCoreId = CSL_CORE_ID_A53SS0_0;
        }

        /* initialize the IPC Notify module */
        status = IpcNotify_init(&notifyParams);
        if(status != SystemP_SUCCESS)
        {
            appLogPrintf("IPC: ERROR: IpcNotify_init failed !!!\n");
        }
    }
    if(status == SystemP_SUCCESS)
    {
        RPMessage_Params rpmsgParams;
        uint32_t src_cpu_id, dst_cpu_id, vringId;
        uint32_t rxTxMap[CSL_CORE_ID_MAX][CSL_CORE_ID_MAX];
        uint32_t self_core_id = g_app_to_ipc_cpu_id[prm->self_cpu_id];

        /* initialize parameters to default */
        RPMessage_Params_init(&rpmsgParams);

        /* VRING mapping from source core to destination core, '-1' means NO VRING */
        /* for each name, construct a N x N object mapping SRC CPU to DST CPU VRING ID,
        Assign VRING IDs to each SRC/DST pair, skip assignment when SRC == DST */
        vringId = 0U;
        for( src_cpu_id=0; src_cpu_id<prm->num_cpus; src_cpu_id++ )
        {
            uint32_t src_core_id = g_app_to_ipc_cpu_id[prm->enabled_cpu_id_list[src_cpu_id]];
            for( dst_cpu_id=0; dst_cpu_id<prm->num_cpus; dst_cpu_id++ )
            {
                uint32_t dst_core_id = g_app_to_ipc_cpu_id[prm->enabled_cpu_id_list[dst_cpu_id]];
                if(src_core_id != dst_core_id)  /* NO VRING for a CPU to itself */
                {
                    if (prm->ipc_resource_tbl != NULL)
                    {
                        rxTxMap[src_core_id][dst_core_id] = vringId;
                        vringId++;
                    }
                    else
                    {
                        rxTxMap[src_core_id][dst_core_id] = getVringIndexPDK(APP_IPC_CPU_MAX, src_core_id, dst_core_id);
                    }
                }
            }
        }

        /* Update Tx buffers */
        for( dst_cpu_id=0; dst_cpu_id<prm->num_cpus; dst_cpu_id++ )
        {
            uint32_t dst_core_id = g_app_to_ipc_cpu_id[prm->enabled_cpu_id_list[dst_cpu_id]];
            if (dst_core_id != self_core_id)
            {
                uint32_t offset = IPC_RPMESSAGE_VRING_SIZE * rxTxMap[self_core_id][dst_core_id];
                rpmsgParams.vringTxBaseAddr[dst_core_id] = (uintptr_t)(((uint8_t*)prm->ipc_vring_mem) + offset);
            }
        }

        /* Update Rx buffers */
        for( src_cpu_id=0; src_cpu_id<prm->num_cpus; src_cpu_id++ )
        {
            uint32_t src_core_id = g_app_to_ipc_cpu_id[prm->enabled_cpu_id_list[src_cpu_id]];
            if (src_core_id != self_core_id)
            {
                uint32_t offset = IPC_RPMESSAGE_VRING_SIZE * rxTxMap[src_core_id][self_core_id];
                rpmsgParams.vringRxBaseAddr[src_core_id] = (uintptr_t)(((uint8_t*)prm->ipc_vring_mem) + offset);
            }
        }

        rpmsgParams.vringSize = IPC_RPMESSAGE_VRING_SIZE;
        rpmsgParams.vringNumBuf = IPC_RPMESSAGE_NUM_VRING_BUF;
        rpmsgParams.vringMsgSize = IPC_RPMESSAGE_MAX_VRING_BUF_SIZE;

        if (prm->ipc_resource_tbl != NULL)
        {
            rpmsgParams.linuxResourceTable = prm->ipc_resource_tbl;
            rpmsgParams.linuxCoreId = CSL_CORE_ID_A53SS0_0;
        }
        else
        {
            /* Note: this is needed with QNX */
            rpmsgParams.vringAllocationPDK = 1u;
        }

        /* initialize the IPC RP Message module */
        status = RPMessage_init(&rpmsgParams);
        if(status != SystemP_SUCCESS)
        {
            appLogPrintf("IPC: ERROR: RPMessage_init failed !!!\n");
        }
    }
    if(status == SystemP_SUCCESS)
    {
        if (prm->ipc_resource_tbl != NULL)
        {
            appLogPrintf("IPC: Waiting for HLOS to be ready ... !!!\n");
            /* This API MUST be called by applications when its ready to talk to Linux */
            status = RPMessage_waitForLinuxReady(SystemP_WAIT_FOREVER);
            appLogPrintf("IPC: HLOS is ready !!!\n");
        }
    }
    if(status == SystemP_SUCCESS)
    {
        RPMessage_CreateParams rpmsg_createTx_prms;

        for(cpu_id=0; cpu_id<APP_IPC_CPU_MAX; cpu_id++)
        {
            if(appIpcIsCpuEnabled(cpu_id))
            {
                RPMessage_CreateParams_init(&rpmsg_createTx_prms);
                rpmsg_createTx_prms.localEndPt = cpu_id;

                status = RPMessage_construct(&obj->rpmsg_tx_handle[cpu_id], &rpmsg_createTx_prms);
                if(status != SystemP_SUCCESS)
                {
                    appLogPrintf("IPC: ERROR: Unable to create rpmessage tx handle for cpu %d !!!\n", cpu_id);
                    break;
                }
            }
        }
    }
    if(status==0)
    {
        RPMessage_CreateParams rpmsg_createRx_prms;

        RPMessage_CreateParams_init(&rpmsg_createRx_prms);
        rpmsg_createRx_prms.localEndPt = prm->tiovx_rpmsg_port_id;

        status = RPMessage_construct(&obj->rpmsg_rx_handle, &rpmsg_createRx_prms);

        if(SystemP_SUCCESS !=status)
        {
            appLogPrintf("IPC: ERROR: Unable to create rpmessage rx handle !!!\n");
        }

        /* NOTE: RPMessage_setCallback is not yet implemented */
    }
#endif

    if((status==0) && (prm->enable_tiovx_ipc_announce == 1))
    {
        /* use "rpmsg-proto" or "rpmsg_chrdev" depending on protocol selected in user space on Linux A72 */
#if !defined (MCU_PLUS_SDK)
        status = RPMessage_announce(RPMESSAGE_ALL, prm->tiovx_rpmsg_port_id, "rpmsg_chrdev");
#else
        status = RPMessage_announce(CSL_CORE_ID_A53SS0_0, prm->tiovx_rpmsg_port_id, "rpmsg_chrdev");
#endif
        if(status != 0)
        {
            appLogPrintf("IPC: RPMessage_announce() for rpmsg-proto failed\n");
            status = -1;
        }
    }

    if(status==0)
    {
        status = appIpcCreateRpmsgRxTask(obj);
        if(status!=0)
        {
            appLogPrintf("IPC: ERROR: appIpcCreateRpmsgRxTask failed !!!\n");
        }
    }
    appLogPrintf("IPC: Init ... Done !!!\n");

    return status;
}

int32_t appIpcDeInit()
{
    int32_t status = 0;
    uint32_t cpu_id;
    app_ipc_obj_t *obj = &g_app_ipc_obj;

    appLogPrintf("IPC: Deinit ... !!!\n");

    appIpcDeleteRpmsgRxTask(obj);

#if !defined(MCU_PLUS_SDK)
    for(cpu_id=0; cpu_id<APP_IPC_CPU_MAX; cpu_id++)
    {
        if(obj->rpmsg_tx_handle[cpu_id]!=NULL)
        {
            RPMessage_delete(&obj->rpmsg_tx_handle[cpu_id]);
            obj->rpmsg_tx_handle[cpu_id] = NULL;
        }
    }
    if(obj->rpmsg_rx_handle!=NULL)
    {
        RPMessage_delete(&obj->rpmsg_rx_handle);
        obj->rpmsg_rx_handle = NULL;
    }

    RPMessage_deInit();
    Ipc_deinit();
#else
    for(cpu_id=0; cpu_id<APP_IPC_CPU_MAX; cpu_id++)
    {
        RPMessage_destruct(&obj->rpmsg_tx_handle[cpu_id]);
    }
    RPMessage_destruct(&obj->rpmsg_rx_handle);

    IpcNotify_deInit();
    RPMessage_deInit();
#endif

    appLogPrintf("IPC: Deinit ... Done !!!\n");

    return status;
}

int32_t appIpcRegisterNotifyHandler(app_ipc_notify_handler_f handler)
{
    int32_t status = 0;
    app_ipc_obj_t *obj = &g_app_ipc_obj;

    obj->ipc_notify_handler = handler;

    return status;
}

int32_t appIpcSendNotifyPort(uint32_t dest_cpu_id, uint32_t payload, uint32_t port_id)
{
    int32_t status = -1;
    app_ipc_obj_t *obj = &g_app_ipc_obj;

#if !defined(MCU_PLUS_SDK)
    if( (dest_cpu_id<APP_IPC_CPU_MAX) && (obj->rpmsg_tx_handle[dest_cpu_id] != NULL))
#else
    if( (dest_cpu_id<APP_IPC_CPU_MAX) && (&obj->rpmsg_tx_handle[dest_cpu_id] != NULL))
#endif
    {
        uint32_t ipc_cpu_id = g_app_to_ipc_cpu_id[dest_cpu_id];

#if !defined(MCU_PLUS_SDK)
        #ifdef APP_IPC_DEBUG
        appLogPrintf("IPC: TX: %s (port %d) -> %s (port %d) msg = 0x%08x\n",
            Ipc_mpGetSelfName(),
            obj->prm.tiovx_rpmsg_port_id,
            Ipc_mpGetName(ipc_cpu_id),
            port_id,
            payload);
        #endif

        status = RPMessage_send(
                    obj->rpmsg_rx_handle,
                    ipc_cpu_id,
                    port_id, /* dst end pt */
                    obj->prm.tiovx_rpmsg_port_id, /* src endpt */
                    &payload,
                    sizeof(payload)
                    );
        if(status!=0)
        {
            appLogPrintf("IPC: TX: FAILED: %s (port %d) -> %s (port %d) msg = 0x%08x\n",
                Ipc_mpGetSelfName(),
                obj->prm.tiovx_rpmsg_port_id,
                Ipc_mpGetName(ipc_cpu_id),
                port_id,
                payload);
        }
#else
#ifdef APP_IPC_DEBUG
        appLogPrintf("IPC: TX: %s (port %d) -> %s (port %d) msg = 0x%08x\n",
            Ipc_mpGetSelfName(),
            obj->prm.tiovx_rpmsg_port_id,
            Ipc_mpGetName(ipc_cpu_id),
            port_id,
            payload);
#endif

        status = RPMessage_send(
                    &payload,
                    sizeof(payload),
                    ipc_cpu_id,
                    port_id,    /* dst end pt */
                    obj->prm.tiovx_rpmsg_port_id, /* src endpt */
                    SystemP_WAIT_FOREVER
                    );
        if(status!=0)
        {
            appLogPrintf("IPC: TX: FAILED: %s (port %d) -> %s (port %d) msg = 0x%08x\n",
                Ipc_mpGetSelfName(),
                obj->prm.tiovx_rpmsg_port_id,
                Ipc_mpGetName(ipc_cpu_id),
                port_id,
                payload);
        }
#endif
    }
    return status;
}

int32_t appIpcSendNotify(uint32_t dest_cpu_id, uint32_t payload)
{
    int32_t status = -1;
    app_ipc_obj_t *obj = &g_app_ipc_obj;

    #if !defined(MCU_PLUS_SDK)
    if( (dest_cpu_id<APP_IPC_CPU_MAX) && (obj->rpmsg_tx_handle[dest_cpu_id] != NULL))
    #else
    if(dest_cpu_id<APP_IPC_CPU_MAX)
    #endif
    {
        status = appIpcSendNotifyPort(dest_cpu_id, payload,
            (uint32_t)obj->prm.tiovx_rpmsg_port_id);
    }

    return status;
}

uint32_t appIpcGetSelfCpuId()
{
    app_ipc_obj_t *obj = &g_app_ipc_obj;

    return obj->prm.self_cpu_id;
}

uint32_t appIpcGetHostPortId(uint16_t cpu_id)
{
    app_ipc_obj_t *obj = &g_app_ipc_obj;

    return obj->prm.tiovx_rpmsg_port_id;
}

uint32_t appIpcIsCpuEnabled(uint32_t cpu_id)
{
    uint32_t is_enabled = 0, cur_cpu_id;
    app_ipc_obj_t *obj = &g_app_ipc_obj;

    if(cpu_id>=APP_IPC_CPU_MAX)
    {
        is_enabled = 0;
    }
    for(cur_cpu_id=0; cur_cpu_id<obj->prm.num_cpus; cur_cpu_id++)
    {
        if(cpu_id==obj->prm.enabled_cpu_id_list[cur_cpu_id])
        {
            is_enabled = 1;
            break;
        }
    }
    return is_enabled;
}

int32_t appIpcGetTiovxObjDescSharedMemInfo(void **addr, uint32_t *size)
{
    int32_t status = 0;
    app_ipc_obj_t *obj = &g_app_ipc_obj;

    if(obj->prm.tiovx_obj_desc_mem==NULL||obj->prm.tiovx_obj_desc_mem_size==0)
    {
        *addr = NULL;
        *size = 0;
        status = -1;
    }
    else
    {
        *addr = obj->prm.tiovx_obj_desc_mem;
        *size = obj->prm.tiovx_obj_desc_mem_size;
    }

    return status;
}

void appIpcGetTiovxLogRtSharedMemInfo(void **shm_base, uint32_t *shm_size)
{
    app_ipc_obj_t *obj = &g_app_ipc_obj;

    if(obj->prm.tiovx_log_rt_mem==NULL||obj->prm.tiovx_log_rt_mem_size==0)
    {
        *shm_base = NULL;
        *shm_size = 0;
    }
    else
    {
        *shm_base = obj->prm.tiovx_log_rt_mem;
        *shm_size = obj->prm.tiovx_log_rt_mem_size;
    }
}

#define APP_IPC_HW_SPIN_LOCK_MAX        (256u)
#if defined(SOC_AM62A) || defined(SOC_J722S)
#define APP_IPC_HW_SPIN_LOCK_MMR_BASE   ((uint32_t)0x2A000000u)
#else
#define APP_IPC_HW_SPIN_LOCK_MMR_BASE   ((uint32_t)0x30E00000u)
#endif
#define APP_IPC_HW_SPIN_LOCK_OFFSET(x)  ((uint32_t)0x800u + (uint32_t)4u*(uint32_t)(x))

static uintptr_t key;

int32_t appIpcHwLockAcquire(uint32_t hw_lock_id, uint32_t timeout)
{
    int32_t status = -1;

    if( hw_lock_id < APP_IPC_HW_SPIN_LOCK_MAX)
    {
        volatile uint32_t *reg_addr;

        reg_addr =
                (volatile uint32_t*)(uintptr_t)(
                    APP_IPC_HW_SPIN_LOCK_MMR_BASE +
                    APP_IPC_HW_SPIN_LOCK_OFFSET(hw_lock_id)
                        );

        key = HwiP_disable();
        /* spin until lock is free */
        while( *reg_addr == 1u )
        {
            HwiP_restore(key);
            appRtosTaskYield();
            key = HwiP_disable();
            /* keep spinning */
        }
        status = 0;
    }

    return status;
}

int32_t appIpcHwLockRelease(uint32_t hw_lock_id)
{
    int32_t status = -1;

    if(hw_lock_id < APP_IPC_HW_SPIN_LOCK_MAX)
    {
        volatile uint32_t *reg_addr;

        reg_addr =
                (volatile uint32_t*)(uintptr_t)(
                    APP_IPC_HW_SPIN_LOCK_MMR_BASE +
                    APP_IPC_HW_SPIN_LOCK_OFFSET(hw_lock_id)
                        );

        *reg_addr = 0; /* free the lock */
        HwiP_restore(key);
        status = 0;
    }

    return status;
}

uint32_t appIpcGetIpcCpuId(uint32_t app_cpu_id)
{
#if !defined (MCU_PLUS_SDK)
    uint32_t ipc_cpu_id = IPC_MP_INVALID_ID;
    if(app_cpu_id < APP_IPC_CPU_MAX)
    {
        ipc_cpu_id = g_app_to_ipc_cpu_id[app_cpu_id];
    }
#else
    uint32_t ipc_cpu_id = APP_MCUSDK_IPC_CPU_INVALID;
    if(app_cpu_id < APP_IPC_CPU_MAX)
    {
        ipc_cpu_id = g_app_to_ipc_cpu_id[app_cpu_id];
    }
#endif
    return ipc_cpu_id;
}


uint32_t appIpcGetAppCpuId(char *name)
{
    uint32_t ipc_cpu_id;
    uint32_t app_cpu_id = APP_IPC_CPU_INVALID;

#if !defined(MCU_PLUS_SDK)
    ipc_cpu_id = Ipc_mpGetId(name);
#else
    ipc_cpu_id = SOC_getCoreId(name);
#endif

    if(ipc_cpu_id < IPC_MAX_PROCS)
    {
        app_cpu_id = g_ipc_to_app_cpu_id[ipc_cpu_id];
    }
    return app_cpu_id;
}

char *appIpcGetCpuName(uint32_t app_cpu_id)
{
    char *name = "unknown";
    if(app_cpu_id < APP_IPC_CPU_MAX)
    {
#if !defined(MCU_PLUS_SDK)
        name = (char*)Ipc_mpGetName(g_app_to_ipc_cpu_id[app_cpu_id]);
#else
        name = (char*)SOC_getCoreName(g_app_to_ipc_cpu_id[app_cpu_id]);
#endif
    }
    return name;
}

#if defined(FREERTOS) || defined(SAFERTOS)

static void traceBufFlush(void* arg0, void* arg1)
{
    appUtilsTaskInit();

    while (1)
    {
        appRtosTaskSleepInMsecs(IPC_TRACEBUF_FLUSH_PERIOD_IN_MSEC);
        traceBufCacheWb();
    }
}

static void traceBufCacheWb(void)
{
    #if !defined(MCU_PLUS_SDK)
    uint64_t newticksInUsecs = TimerP_getTimeInUsecs();
    #else
    uint64_t newticksInUsecs = ClockP_getTimeUsec();
    #endif

    /* Don't keep flusing cache */
    if ((newticksInUsecs - gIpcObj.traceBufLastFlushTicksInUsecs) >=
        (uint64_t)(IPC_TRACEBUF_FLUSH_PERIOD_IN_MSEC * 1000))
    {
        gIpcObj.traceBufLastFlushTicksInUsecs = newticksInUsecs;

        /* Flush the cache of the traceBuf buffer */
        if (gIpcObj.traceBufAddr != NULL)
        {
	#if !defined(MCU_PLUS_SDK)
            CacheP_wb((const void *)gIpcObj.traceBufAddr,
                      gIpcObj.traceBufSize);
	#else
	        CacheP_wb((void *)gIpcObj.traceBufAddr,
                      gIpcObj.traceBufSize,
                      CacheP_TYPE_L1D);
	#endif
        }
    }
}

int32_t appIpcCreateTraceBufFlushTask(void)
{
    int32_t status = -1;
    app_rtos_task_params_t taskParams;

    gIpcObj.traceBufAddr = (uint8_t *)Ipc_traceBuffer;
    gIpcObj.traceBufSize = IPC_TRACEBUF_SIZE;
    gIpcObj.traceBufLastFlushTicksInUsecs = 0ULL;

    /* Task to flush IPC traceBuf */
    appRtosTaskParamsInit(&taskParams);
    taskParams.priority  = 0;
    taskParams.stack     = &gIpcTraceBufFlushBuf[0];
    taskParams.stacksize = sizeof(gIpcTraceBufFlushBuf);
    taskParams.name      = (const char*)"IPC tracebuf flush";
    taskParams.taskfxn   = &traceBufFlush;

    appRtosTaskCreate(&taskParams);

    return status;
}
#endif
