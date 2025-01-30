/*
 *  Copyright (c) Texas Instruments Incorporated 2024
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 *  \file ipc_api.c
 *
 *  \brief File containing the IPC driver APIs.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <CddIpc/ipc_baremetal_hw/ipc.h>
#include <string.h>
#include "ipc_priv.h"
#include "ipc_virtioPrivate.h"
#include "ipc_utils.h"
#include "ipc_mailbox.h"
#include "Cdd_Ipc.h"
/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define MAXCALLBACKS           IPC_MAX_PROCS

#define MSGBUFFERSIZE          (IPC_MAX_DATA_PAYLOAD + sizeof(RPMessage_MsgElem))
#define SERVICENAMELEN         32U
#define HEAPALIGNMENT          8U

struct RPMessage_Object_s;
typedef struct RPMessage_Object_s*            RPMessage_EndptPool[MAXENDPOINTS];

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

#define CNTRLMSG_ANNOUNCE (0x00000000U)

#define SemaphoreP_NO_WAIT       ((uint32)0U)

/* Message Header: Must match mp_msg_hdr in virtio_rp_msg.h on Linux side. */
typedef struct RPMessage_MsgHeader_s
{
    uint32  srcAddr;             /* source endpoint addr       */
    uint32  dstAddr;             /* destination endpoint addr  */
    uint32  srcProcId;           /* reserved                   */
    uint16  dataLen;             /* data length                */
    uint16  flags;               /* bitmask of different flags */
    uint8   payload[];           /* Data payload               */
}RPMessage_MsgHeader;


/* common fields of a control message */
typedef struct RPMessage_CntrlMsg_s
{
    uint32  type;
}RPMessage_CntrlMsg;

/* announcement message */
typedef struct RPMessage_Announcement_s
{
    UCHAR                 name[SERVICENAMELEN];
    uint32             endPt;
    RPMessage_CntrlMsg   ctrl;
} RPMessage_Announcement;


/* list element for the announcedEndpts list */
typedef struct RPMessage_NameEntry_s
{
    IpcUtils_QElem   elem;
    uint32         procId;
    uint32         endPt;
    UCHAR             name[SERVICENAMELEN];
} RPMessage_NameEntry;


/* list element for the waitingTasks list */
typedef struct RPMessage_Waiter_s
{
    IpcUtils_QElem     elem;
    void               *semHandle;
    uint32           procId;
    uint32           endPt;
    UCHAR               name[SERVICENAMELEN];
    uint32           token;
} RPMessage_Waiter;
/**
 *  \brief Element to hold payload copied onto receiver's queue.
 */
typedef struct RPMessage_MsgElem_s
{
    IpcUtils_QElem    elem;          /**< Allow list linking.          */
    uint32          len;           /**< Length of data               */
    uint32          src;           /**< Src address/endPt of the msg */
    uint32          procId;        /**< Processor sending the msg    */
} RPMessage_MsgElem;

/* The RPMessage Object */
typedef struct RPMessage_Object_s
{
    uint32             endPt;        /**< Unique id (procId | queueIndex) */
    void                 *semHandle;   /**< I/O Completion                 */
    RPMessage_Callback   cb;           /**< RPMessage Callback */
    void*                arg;          /**< Callback argument */
    IpcUtils_QHandle     queue;        /**< Queue of pending messages             */
    uint8              unblocked;    /**< Use with signal to unblock _receive() */
    IpcUtils_HeapParams  heapParams;   /* Save the heapBuf params to delete Heap   */
    IpcUtils_HeapHandle  heap;         /* Heap from which to allocate free messages */
    RPMessage_EndptPool  *pool;        /**< track associated pool for cleanup */
    void*                recv_buffer;  /**< Application buffer from RPMessage_recv() */
    RPMessage_MsgElem    payload;      /**< Message info for RPMessage_recv() */
} RPMessage_Object;

/**
 *  \brief This struct is used as private data supplied to the VQ
 *         callback function.  It is also passed to the SWI
 *         functions through arg0. */
typedef struct RPMessage_CallbackData_s
{
    Virtio_Handle        vq;
    RPMessage_EndptPool  *pool;
    Ipc_OsalHIsrHandle   swi;
} RPMessage_CallbackData;

/**
 *  \brief Module_State
 */
typedef struct RPMessage_Module_s
{
    /* Instance gate: */
    Ipc_OsalHIsrGateHandle gateSwi;

    /* Lookup VQ handle from procId for sending messages. */
    /* Array indexed by procId.                           */
    Virtio_Handle tx_VQs[IPC_MAX_PROCS];

    /* Array of callback structs. Used only for RX VirtQueues */
    RPMessage_CallbackData VQ_callbacks[MAXCALLBACKS];

    uint32 numCallbacks;

    /* Global endpoint pool */
    RPMessage_EndptPool globalPool;

    /* Lookup endpoint pool from procId.  Array indexed by procId. */
    /* RPMessage_EndptPool pools[MAXREMOTEPROCS];            */

    /* Queue to track announcements from other processors */
    IpcUtils_QHandle announcedEndpts;

    /* Queue to hold the semaphores of waiting tasks */
    IpcUtils_QHandle waitingTasks;

    /* Array of RPMessage Object, memory supplied by application */
    RPMessage_Object*   objArry;

    /* Current object Count */
    uint16 objCnt;

    /* NameEntry count */
    uint16  nameEntryCnt;

    RPMessage_NameEntry  nameEntry[IPC_MAX_NAME_ENTRY];

} RPMessage_Module;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static RPMessage_Object* RPMessage_rawCreate(RPMessage_Params *params,
        RPMessage_EndptPool* pool, uint32 *endPt);

static int32_t RPMessage_processAnnounceMsg(RPMessage_Announcement *amsg, uint32 procId);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

#define CDD_IPC_START_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Cdd_Ipc_MemMap.h"

static RPMessage_Module module;

static Ipc_Object gIpcObject;
/**< IPC Object */

#define CDD_IPC_STOP_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Cdd_Ipc_MemMap.h"

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

static RPMessage_Object* RPMessage_lookupEndpt(void* p, uint32 e);
static void RPMessage_assignEndpt(void* p, uint32 e, RPMessage_Object* obj);

static uint8 is_aligned(const void* ptr, uint32 byteCnt);

#define CDD_IPC_START_SEC_CODE
#include "Cdd_Ipc_MemMap.h"

inline static RPMessage_Object* RPMessage_lookupEndpt(void* p, uint32 e)
{
    return (((RPMessage_Object**)(p))[(e)]);
}

inline static void RPMessage_assignEndpt(void* p, uint32 e, RPMessage_Object* obj)
{
    (((RPMessage_Object**)(p))[(e)] = (obj));
}

/*CDD_IPC_CoverageGap_13: The function is_aligned() is a inline static function. We don’t have access to the argument.*/
inline static uint8 is_aligned(const void* ptr, uint32 byteCnt)
{
    uint8 retVal;
    uint32 bufAddr = (uint32)(uintptr_t)ptr;
    retVal = (0U == (bufAddr%byteCnt)) ? TRUE : FALSE;
    return retVal;
}



int32_t RPMessageParams_init(RPMessage_Params *params)
{
    int32_t    retVal = IPC_SOK;

    if(params == NULL)
    {
        retVal = IPC_EBADARGS;
    }
    else
    {
        /*CDD_IPC_CoverageGap_14: The argument passed is a local pointer. So we dont have access to make it NULL. Hence it cannot be covered.*/
        params->requestedEndpt = RPMESSAGE_ANY;
        params->numBufs        = RPMessage_Buffer_Count_Default;
        params->stackBuffer    = NULL;
        params->stackSize      = 0U;
    }

    return (retVal);
}

uint32 RPMessage_getObjMemRequired(void)
{
    uint32 objSize = sizeof(RPMessage_Object);
    objSize = ((objSize + HEAPALIGNMENT-1U) & ~(HEAPALIGNMENT-1U));
    return objSize;
}


/*
 *  ======== RPMessage_enqueMsg ========
 */
/* Always assumed to be called in SWI context */

static int32_t RPMessage_enqueMsg(RPMessage_EndptPool *pool, RPMessage_MsgHeader *msg)
{
    int32_t             status = IPC_SOK;
    int32_t             key;
    uint32            size;
    RPMessage_MsgElem  *payload;
    void                *msgData;
    RPMessage_Object   *obj = NULL;
    Ipc_OsalPrms        *pOsalPrms = &gIpcObject.initPrms.osalPrms;

    /* Protect from RPMessage_delete */
    if ((NULL != pOsalPrms->lockHIsrGate) &&
        (NULL != pOsalPrms->unLockHIsrGate))
    {
        key = pOsalPrms->lockHIsrGate(module.gateSwi);
        obj = RPMessage_lookupEndpt(pool, msg->dstAddr);
        pOsalPrms->unLockHIsrGate(module.gateSwi, key);
    }

#ifdef IPC_EXCLUDE_CTRL_TASKS
    if (msg->dstAddr == IPC_CTRL_ENDPOINT_ID)
    {
        /* This message is for the ctrl endpoint */
        RPMessage_Announcement *amsg = (RPMessage_Announcement*)msg->payload;
        if (amsg->ctrl.type == CNTRLMSG_ANNOUNCE)
        {
#if DEBUG_PRINT
            SystemP_printf("RPMessage_enqueMsg ...CNTRLMSG_ANNOUNCE\n");
#endif
            status = RPMessage_processAnnounceMsg(
                (RPMessage_Announcement*)amsg, msg->srcProcId);
            if(status != IPC_SOK)
            {
                SystemP_printf("RPMessage_processAnnounceMsg: Failed");
            }
        }
        if ((status == IPC_SOK) && (NULL != gIpcObject.initPrms.newMsgFxn))
        {
            gIpcObject.initPrms.newMsgFxn(msg->srcAddr, msg->dstAddr, msg->srcProcId);
        }
    }
#endif /* IPC_EXCLUDE_CTRL_TASKS */
    if (NULL != obj)
    {
        key = pOsalPrms->lockHIsrGate(module.gateSwi);

        if (NULL != obj->recv_buffer)
        {
            memcpy(obj->recv_buffer, (void *)msg->payload, msg->dataLen);
            obj->recv_buffer = NULL;
            obj->payload.len = msg->dataLen;
            obj->payload.src = msg->srcAddr;
            obj->payload.procId = msg->srcProcId;

            pOsalPrms->unLockHIsrGate(module.gateSwi, key);

            if (NULL != gIpcObject.initPrms.newMsgFxn)
            {
                gIpcObject.initPrms.newMsgFxn(msg->srcAddr, msg->dstAddr, msg->srcProcId);
            }

            if (NULL != pOsalPrms->unlockMutex)
            {
                pOsalPrms->unlockMutex(obj->semHandle);
            }
        }
        else
        {
            if( obj->endPt != msg->dstAddr)
            {
                SystemP_printf("WARNING: %d != %d\n", obj->endPt, msg->dstAddr);
            }

            /* Allocate a buffer to copy the payload: */
            size = msg->dataLen + sizeof(RPMessage_MsgElem);

            /* HeapBuf_alloc() is non-blocking, so needs protection: HIsrGate has already been taken*/
            payload = (RPMessage_MsgElem *)IpcUtils_HeapAlloc(&obj->heap, size, 0);

            if (payload != NULL)
            {
                /* Message data starts on the first byte after the
                 * RPMessage_MsgElem structure in the allocated object.
                 */
                msgData = (void *)((uint32)payload + sizeof(*payload));
                memcpy(msgData, msg->payload, msg->dataLen);
                payload->len = msg->dataLen;
                payload->src = msg->srcAddr;
                payload->procId = msg->srcProcId;

                IpcUtils_Qput(&obj->queue, &payload->elem);

                pOsalPrms->unLockHIsrGate(module.gateSwi, key);
                if (NULL != gIpcObject.initPrms.newMsgFxn)
                {
                    gIpcObject.initPrms.newMsgFxn(msg->srcAddr, msg->dstAddr, msg->srcProcId);
                }

                if (NULL != pOsalPrms->unlockMutex)
                {
                    pOsalPrms->unlockMutex(obj->semHandle);
                }
            }
            else
            {
                pOsalPrms->unLockHIsrGate(module.gateSwi, key);
                SystemP_printf("IpcUtils_HeapAlloc failed: Failed to allocate buffer for payload. (%d %d %d %d %d)\n",
                    msg->srcProcId, msg->srcAddr, msg->dstAddr, msg->dataLen, obj->endPt);
                status = IPC_EFAIL;
            }
        }
    }

    return status;
}

/**
 * \brief RPMessage SWI function to talk to Linux
 */
static void RPMessage_swiLinuxFxn(uintptr_t arg0, uintptr_t arg1)
{
    RPMessage_CallbackData *cbdata = (RPMessage_CallbackData*)arg0;
    RPMessage_MsgHeader    *msg;
    Int16                  token;
    int32_t                len;
    uint8                usedBufAdded = FALSE;
    int32_t                key;

    key = gIpcObject.initPrms.osalPrms.lockHIsrGate(module.gateSwi);

    /* Process all available buffers: */
    token = Virtio_getAvailBuf(cbdata->vq, (void **)&msg, &len);
    while (token  >= 0)
    {
        gIpcObject.initPrms.osalPrms.unLockHIsrGate(module.gateSwi, key);

        /*Linux core not at core ID 0 for AM62X and AM62AX,
         *hence added a conditional check here to 
         *support IPC communication with A53 core*/
        #if defined(SOC_AM62X)
        msg->srcProcId = CDD_IPC_CORE_A53SS_1;
        #elif defined(SOC_AM62AX) || defined(SOC_AM62PX) || defined(SOC_J722S)
        msg->srcProcId = CDD_IPC_CORE_A53SS0_0;
        #endif

        /* Pass to desitination queue (which is on this proc): */
        RPMessage_enqueMsg(cbdata->pool, msg);

        key = gIpcObject.initPrms.osalPrms.lockHIsrGate(module.gateSwi);

        Virtio_addUsedBuf(cbdata->vq, token, MSGBUFFERSIZE);
	usedBufAdded = TRUE;
	token = Virtio_getAvailBuf(cbdata->vq, (void **)&msg, &len);
    }

    if (TRUE == usedBufAdded)
    {
       /* Tell host we've processed the buffers: */
       Virtio_kick(cbdata->vq);
    }

    gIpcObject.initPrms.osalPrms.unLockHIsrGate(module.gateSwi, key);
}

/**
 * \brief RPMessage SWI function
 */
static void RPMessage_swiFxn(uintptr_t arg0, uintptr_t arg1)
{
    RPMessage_CallbackData *cbdata = (RPMessage_CallbackData*)arg0;
    RPMessage_MsgHeader    *msg;
    int32_t   key;
    uint16  token;

    key = gIpcObject.initPrms.osalPrms.lockHIsrGate(module.gateSwi);

    /* Process all available buffers: */
    msg = (RPMessage_MsgHeader *) Virtio_getUsedBuf(cbdata->vq, &token);
    while (msg != NULL)
    {
        gIpcObject.initPrms.osalPrms.unLockHIsrGate(module.gateSwi, key);

        /* Pass to desitination queue (which is on this proc): */
        RPMessage_enqueMsg(cbdata->pool, msg);

        key = gIpcObject.initPrms.osalPrms.lockHIsrGate(module.gateSwi);

        Virtio_addAvailBuf(cbdata->vq, msg, token);
        msg = (RPMessage_MsgHeader *) Virtio_getUsedBuf(cbdata->vq, &token);
    }

    gIpcObject.initPrms.osalPrms.unLockHIsrGate(module.gateSwi, key);
}

/**
 *  \brief virtio_callback - This function executes in HWI context
 */
/*CDD_IPC_CoverageGap_15: 1. its a static function.
     *2.Default values are assigned.*/
static void virtio_callback(uint32* priv)
{
    RPMessage_CallbackData *cbdata = (RPMessage_CallbackData*)priv;

    if (NULL != cbdata)
    {
        gIpcObject.initPrms.osalPrms.postHIsr(&cbdata->swi);
    }
    else
    {
      /* Do Nothing */
    }
}

/**
 *  \brief RPMessage_announce : Announces the availabilty of an
 *          endpoint to all processors or only one.
 */

int32_t RPMessage_announce(uint32 remoteProcId, uint32 endPt, const UCHAR* name)
{
    RPMessage_EndptPool     *pool = &module.globalPool;
    RPMessage_Announcement  msg;
    Virtio_Handle           vq;
    uint32                c;
    uint32                procId;
    int32_t                 status = IPC_SOK;
    size_t                  namelen;

#if  DEBUG_PRINT
    SystemP_printf("RPMessage_announce : remote %d, endpt %d, name %s\n",
        remoteProcId, endPt, name);
#endif

    namelen = IpcUtils_strnlen(name , sizeof(name));
    if((namelen >= SERVICENAMELEN) || (namelen == 0U))
    {
        status = IPC_EFAIL;
    }

    if(IPC_EFAIL != status)
    {
        msg.ctrl.type = CNTRLMSG_ANNOUNCE;
        msg.endPt = endPt;
        strncpy((char *)msg.name, (char *)name, SERVICENAMELEN-1U);
        msg.name[SERVICENAMELEN-1U] = '\0';

        if(remoteProcId == RPMESSAGE_ALL)
        {
            for(c = 0; c < module.numCallbacks; c++)
            {
                /* Find callback for RX VQs that have matching pool. */
                if(module.VQ_callbacks[c].pool == pool)
                {
                    vq = module.VQ_callbacks[c].vq;
                    procId = Virtio_getProcId(vq);
#if DEBUG_PRINT
                    SystemP_printf("RPMessage_announce.....c%d ProcId %d\n", c, procId);
#endif
                    status = RPMessage_send(NULL, procId, IPC_CTRL_ENDPOINT_ID,
                            IPC_CTRL_ENDPOINT_ID, &msg, sizeof(msg));
                    if (status != IPC_SOK)
                    {
                        SystemP_printf("RPMessage_announce.....failed to send c %d (%s)\n", c, Ipc_mpGetName(procId));
                        /* even if failed to annouce to one CPU continue to other CPUs */
                        continue;
                    }
                }
            }
        }
        else
        {
            status = RPMessage_send(NULL, remoteProcId, IPC_CTRL_ENDPOINT_ID,
                    IPC_CTRL_ENDPOINT_ID, &msg, sizeof(msg));
            if (status != IPC_SOK)
            {
                SystemP_printf("RPMessage_announce.....failed to send\n");
            }
        }
    }

    return status;
}

/**
 *  \brief RPMessage_processAnnounceMsg : Handle an endpoint annoucement
 *         message from another processor
 */
static int32_t RPMessage_processAnnounceMsg(RPMessage_Announcement *amsg, uint32 procId)
{
    int32_t key;
    RPMessage_NameEntry *p;
#ifndef IPC_EXCLUDE_CTRL_TASKS
    RPMessage_Waiter *w;
    IpcUtils_QElem *elem, *head;
#endif /* IPC_EXCLUDE_CTRL_TASKS */
    int32_t rtnVal = IPC_SOK;
    Ipc_OsalPrms *pOsalPrms = &gIpcObject.initPrms.osalPrms;

#if DEBUG_PRINT
    SystemP_printf("RPMessage_processAnnounceMsg : announcement from %d for endPt %d\n",
             procId, amsg->endPt );
#endif

#ifndef IPC_EXCLUDE_CTRL_TASKS
    if (((NULL == pOsalPrms->lockHIsrGate) ||
        (NULL == pOsalPrms->unLockHIsrGate)) ||
        (NULL == pOsalPrms->unlockMutex))
    {
        rtnVal = IPC_EFAIL;
    }
#else
    if ((NULL == pOsalPrms->lockHIsrGate) ||
        (NULL == pOsalPrms->unLockHIsrGate))
    {
            rtnVal = IPC_EFAIL;
    }
#endif /* IPC_EXCLUDE_CTRL_TASKS */

    if (IPC_SOK == rtnVal)
    {
        key = pOsalPrms->lockHIsrGate(module.gateSwi);

        /* Save the annoucement for future calls to */
        /* RPMessage_peerIsReady().                 */
        if(module.nameEntryCnt == (IPC_MAX_NAME_ENTRY-1u))
        {
            pOsalPrms->unLockHIsrGate(module.gateSwi, key);
            SystemP_printf("RPMessage_processAnnounceMsg : all remote core done\n");
            rtnVal = IPC_EFAIL;
        }

        if(rtnVal != IPC_EFAIL)
        {
            p = &module.nameEntry[module.nameEntryCnt];
            p->endPt = amsg->endPt;
            p->procId = procId;
            strncpy((char *)p->name, (char *)amsg->name, SERVICENAMELEN -1U);
            p->name[SERVICENAMELEN-1U] = '\0';
            module.nameEntryCnt++;

            /* No interrupt / SWI protection, required here again.
               We are already in SWI protected region */
            IpcUtils_Qput(&module.announcedEndpts, &p->elem);

#ifndef IPC_EXCLUDE_CTRL_TASKS
            /* Wakeup all the tasks that are waiting on the */
            /* announced name.                              */
            if (!IpcUtils_QisEmpty(&module.waitingTasks))
            {
                /* No interrupt / SWI protection, required here again.
                   We are already in SWI protected region */
                elem = head = (IpcUtils_QElem *) IpcUtils_QgetHeadNode(&module.waitingTasks);
                do
                {
                    w = (RPMessage_Waiter*)elem;
                    if( (NULL != w) &&
                            (strncmp(w->name, amsg->name, SERVICENAMELEN-1) == 0) &&
                            (w->procId == procId || w->procId == RPMESSAGE_ANY))
                    {
                        /* Update the waiter's entry with actual */
                        /* announcement values.                   */
                        w->procId = procId;
                        w->endPt = amsg->endPt;
#if DEBUG_PRINT
                        SystemP_printf("RPMessage_processAnnounceMsg :Semphore Handle 0x%x\n",
                                (uint32)w->semHandle);
#endif

                        pOsalPrms->unlockMutex(w->semHandle);
                    }
                    elem = (IpcUtils_QElem *) IpcUtils_Qnext(elem);
                } while (elem != head);
            }
#endif /* IPC_EXCLUDE_CTRL_TASKS */
            pOsalPrms->unLockHIsrGate(module.gateSwi, key);
        }
    }

    return (rtnVal);
}


#ifndef IPC_EXCLUDE_CTRL_TASKS

/**
 *  \brief RPMessage_checkForMessages : This function checks for
 *         messages in all virtio that are associated with a endpoint
 *         pool.  This operation has the side effect that it may find
 *         messages for end points that do not yet have an endpoint
 *         created for them.  Those messages will be dropped.
 */

static void RPMessage_checkForMessages(RPMessage_EndptPool *pool)
{
    uint32   c;

    for(c = 0; c < module.numCallbacks; c++)
    {
        /* Find callback for RX VQs that have matching pool. */
        if(module.VQ_callbacks[c].pool == pool)
        {
            if(TRUE == Virtio_isReady(module.VQ_callbacks[c].vq))
            {
                RPMessage_swiFxn((uintptr_t)&module.VQ_callbacks[c], 0);
            }
        }
    }
}
#endif /* IPC_EXCLUDE_CTRL_TASKS */

#ifndef IPC_EXCLUDE_CTRL_TASKS

/**
 *  \brief RPMessage_ctrlMsgTask : Create an endpoint to receive
 *         control messages and dispatch any control messages
 *         received.
 *         This function runs in its own task.
 */
#ifdef QNX_OS
static void RPMessage_ctrlMsgTask(void* arg0, void* arg1)
#else
static void RPMessage_ctrlMsgTask(uint32* arg0, uint32* arg1)
#endif
{
#ifdef QNX_OS
    RPMessage_Object *obj = (RPMessage_Object *)(uintptr_t)(((uint64 *)arg0));
#else
    RPMessage_Object *obj = (RPMessage_Object *)arg0;
#endif
    uint32      remoteEndpoint;
    uint32      remoteProcId;
    int32_t       status;
    uint16      len = MSGBUFFERSIZE;
    uint32      buffer[MSGBUFFERSIZE/sizeof(uint32)]; /* alignment */
    RPMessage_Announcement *amsg = (RPMessage_Announcement*)&buffer;

    UNUSED(arg1);

    while(1)
    {
        status = RPMessage_recv((RPMessage_Handle)obj, (void*)amsg, &len,
                                      &remoteEndpoint, &remoteProcId,
                                      IPC_RPMESSAGE_TIMEOUT_FOREVER);
        if(status != IPC_SOK)
        {
            SystemP_printf("RPMessage_recv failed with code %d for endPt %d\n"
                       "RPMessage_ctrlMsgTask terminating early",
                       status, obj->endPt);
            break;
        }

        if(amsg->ctrl.type == CNTRLMSG_ANNOUNCE)
        {
#if DEBUG_PRINT
            SystemP_printf("RPMessage_ctrlMsgTask ...CNTRLMSG_ANNOUNCE\n");
#endif
            status = RPMessage_processAnnounceMsg(
                (RPMessage_Announcement*)amsg, remoteProcId);
            if(status != IPC_SOK)
            {
                SystemP_printf("RPMessage_processAnnounceMsg: Failed");
                break;
            }
        }
        else
        {
            SystemP_printf("Invalid cntrl msg type 0x%x from procId %d\n",
                       amsg->ctrl.type, remoteProcId);
        }
    }
}

/**
 * \brief Kick off the control message task.
 * Typically this is called from Main context.
 * It is best to call this before setting the
 * VQ callbacks to avoid dropping cntrl messages.
 */

#endif /* IPC_EXCLUDE_CTRL_TASKS */

static RPMessage_Object* RPMessage_rawCreate(
        RPMessage_Params *params,
        RPMessage_EndptPool* pool,
        uint32 *endPt)
{
    RPMessage_Object *obj   = NULL;
    uint8           found = FALSE;
    uint32          i;
    uint32          queueIndex = 0;
    int32_t           key = 0;
    uint32         objSize = RPMessage_getObjMemRequired();
    Ipc_OsalPrms     *pOsalPrms = &gIpcObject.initPrms.osalPrms;
    int32_t           status = IPC_SOK;

    /* Returning NULL, here Fix ME TBD
        This function error checks would require an overhaul */
    if ( (NULL == pOsalPrms->lockHIsrGate) ||
         (NULL == pOsalPrms->unLockHIsrGate))
    {
        status = IPC_EFAIL;
    }
    else
    {
        key = pOsalPrms->lockHIsrGate(module.gateSwi);
    }

    if(NULL == endPt)
    {
        SystemP_printf("RPMessage_rawCreate ...NULL endPt\n");
        status = IPC_EFAIL;
    }

    if(IPC_SOK == status)
    {
        /* Allocate the endPt */
        if (params->requestedEndpt == RPMESSAGE_ANY)
        {
            /* Search the array for a free slot above reserved: */
            for (i = RPMessage_MAX_RESERVED_ENDPOINT + 1U;
                    (i < MAXENDPOINTS) && (found == 0U); i++)
            {
                if (RPMessage_lookupEndpt(pool, i) == NULL)
                {
                    queueIndex = i;
                    found = TRUE;
                    break;
                }
            }
        }
        else if (params->requestedEndpt <= RPMessage_MAX_RESERVED_ENDPOINT)
        {
            if (RPMessage_lookupEndpt(pool, params->requestedEndpt) == NULL)
            {
            /* CDD_IPC_CoverageGap_17: Default value for (params->requestedEndpt) is always assigned to the variable. */
                queueIndex = params->requestedEndpt;
                found = TRUE;
            }
            else
            {
              ;
            }
        }
        else
        {
              ;  /* do nothing*/
        }
    }

    if ((FALSE == found) ||
        (params->numBufs < 1U) ||
        (params->bufSize < ((params->numBufs * MSGBUFFERSIZE) + objSize)))
    {
        status = IPC_EFAIL;
    }

    if(IPC_SOK == status)
    {
        obj = (RPMessage_Object *) params->buf;
        if (NULL != obj)
        {
            obj->heapParams.blockSize    = MSGBUFFERSIZE;
            obj->heapParams.numBlocks    = params->numBufs;
            obj->heapParams.buf          = (void*)((uintptr_t)params->buf + objSize);
            obj->heapParams.bufSize      = params->bufSize - objSize ;
            obj->heapParams.align        = HEAPALIGNMENT;
            /* CDD_IPC_CoverageGap_18: Default value for IpcUtils_HeapCreate(&obj->heap, &obj->heapParams) is always assigned to the variable. */
            status = IpcUtils_HeapCreate(&obj->heap, &obj->heapParams);
            if (IPC_SOK == status)
            {
                /* Allocate a semaphore to signal when messages received: */
                if (NULL != pOsalPrms->createMutex)
                {
                    obj->semHandle = pOsalPrms->createMutex();
                }

                /* Create our queue of to be received messages: */
                IpcUtils_Qcreate(&obj->queue);

                /* Store our endPt, and object: */
                obj->endPt = queueIndex;
                RPMessage_assignEndpt(pool, queueIndex, obj);

                /* See RPMessage_unblock() */
                obj->unblocked   = FALSE;
                obj->pool        = pool;
                obj->recv_buffer = NULL;
                *endPt           = queueIndex;
            }
            else
            {
                obj = NULL;
            }
        }
    }

    if ((NULL != pOsalPrms->lockHIsrGate) &&
        (NULL != pOsalPrms->unLockHIsrGate))
    {
    /* CDD_IPC_CoverageGap_16: Default value for (pOsalPrms->lockHIsrGate & pOsalPrms->unLockHIsrGate) 
     * is always assigned to the variable. So it cannot be covered.*/
        pOsalPrms->unLockHIsrGate(module.gateSwi, key);
    }

    return (obj);
}
/**
 *  \brief RPMessage_init : Initializing the framework
 */
/* CDD_IPC_CoverageGap_19: Already check is provided which makes this part of code inaccessible*/
int32_t RPMessage_init(RPMessage_Params *params)
{
    uint32         p;
    uint32         c;
    uint32         tx_vqId, rx_vqId;
    int32_t          retVal = IPC_SOK;
    Ipc_OsalPrms     *pOsalPrms = &gIpcObject.initPrms.osalPrms;

    /* Clear module state */
    memset(&module, 0, sizeof(module));

    if((NULL == params) || (0U == params->bufSize) || (NULL == params->buf))
    {
        SystemP_printf("RPMessage_init ...Invalid params\n");
        retVal = IPC_EFAIL;
    }

#ifndef IPC_EXCLUDE_CTRL_TASKS
    if( retVal != IPC_EFAIL)
    {
        if((0 == params->stackSize) || (NULL == params->stackBuffer))
        {
            SystemP_printf("RPMessage_init ...Invalid params\n");
            retVal = IPC_EFAIL;
        }
    }
#endif /* IPC_EXCLUDE_CTRL_TASKS */

    if( retVal != IPC_EFAIL)
    {
        /* Gate to protect module object and lists: */
        if ((1U == is_aligned(params->buf,HEAPALIGNMENT)) &&
                (NULL != pOsalPrms->createHIsrGate) &&
                (NULL != pOsalPrms->createHIsr))
        {
            module.gateSwi = pOsalPrms->createHIsrGate();
        }
        else
        {
            retVal = IPC_EFAIL;
            SystemP_printf("RPMessage_init : Memory misalignment  or invalid HIstGate fxn\n");
        }
    }

    if( retVal != IPC_EFAIL)
    {
        IpcUtils_Qcreate(&module.announcedEndpts);
        IpcUtils_Qcreate(&module.waitingTasks);

        /* Connect to the VQs by setting the VQ callbacks and
         * creating the assocated SWIs.  Once the callbacks
         * are installed is it possilbe to drop messages if
         * the dest endpoint has not yet been created.
         */
        for(p = 0; ((p < IPC_MAX_PROCS) && (IPC_SOK == retVal)); p++)
        {
            if(TRUE == VirtioIPC_getVirtQueues(VIRTIOIPC_RPMSG, p, 0, &tx_vqId, &rx_vqId))
            {
                uintptr_t arg0 = NULL;
                c = module.numCallbacks;
                module.numCallbacks++;

                /* setup the receiving path */
                module.VQ_callbacks[c].pool = &module.globalPool;
                module.VQ_callbacks[c].vq = Virtio_getHandle(p, VIRTIO_RX);

                arg0 = (uintptr_t)&module.VQ_callbacks[c];
                if (TRUE == Virtio_isRemoteLinux((uint16)p))
                {
                     pOsalPrms->createHIsr(&module.VQ_callbacks[c].swi,
                             &RPMessage_swiLinuxFxn, (void *)arg0);

                }
                else
                {
                    pOsalPrms->createHIsr(&module.VQ_callbacks[c].swi,
                            &RPMessage_swiFxn, (void *)arg0);
                }
                Virtio_setCallback(p, virtio_callback, (uint32 *)&module.VQ_callbacks[c]);

                /* setup the sending path */
                module.tx_VQs[p] = Virtio_getHandle(p, VIRTIO_TX);

                if (NULL != pOsalPrms->registerIntr)
                {
                    Ipc_mailboxEnableNewMsgInt((uint32)Ipc_mpGetSelfId(), (uint32)p);
                }
            }
            else
            {
                module.tx_VQs[p] = NULL;
            }
        }

#ifndef IPC_EXCLUDE_CTRL_TASKS
        /*
         * At this point, the control end point would not have been created.
         *  Let apps, decide when to process pending messages
         */
        RPMessage_checkForMessages(&module.globalPool);

#endif /* IPC_EXCLUDE_CTRL_TASKS */
    }

    return retVal;
}


/*
 *  ======== RPMessage_create ========
 */
RPMessage_Handle RPMessage_create(RPMessage_Params *params, uint32 *endPt)
{
    RPMessage_Object *obj = NULL;
    RPMessage_Params *defaultParams = params;

    if(defaultParams == NULL)
    {
        RPMessageParams_init(defaultParams);
    }

    if (defaultParams != NULL)
    {
      if( 0U == is_aligned(params->buf,HEAPALIGNMENT))
      {
          SystemP_printf("RPMessage_create : Memory alignment failed\n");
      }
      else
      {
          obj = RPMessage_rawCreate(params, &module.globalPool, endPt);
      }
    }

    return (RPMessage_Handle)obj;
}

/**
 *  \brief RPMessage_deinit : Tear down the module
 */

/*
 *  ======== RPMessage_delete ========
 */

/*
 *  ======== RPMessage_recv ========
 */

/*
 *  ======== RPMessage_recvNb ========
 */
int32_t RPMessage_recvNb(RPMessage_Handle handle, void* data, uint16 *len,
                   uint32 *rplyEndPt, uint32 *rplyProcId,boolean isCtrlMsg)
{
    RPMessage_Object    *obj;
    RPMessage_MsgElem   *payload;
    uint32            key;
    int32_t             status = IPC_EBADARGS;
    Ipc_OsalPrms *pOsalPrms = &gIpcObject.initPrms.osalPrms;
    void                *msgData;

    if ((handle != NULL) && (NULL != data) && (NULL != len) &&
        (NULL != rplyEndPt) && (NULL != rplyProcId))
    {
        obj = (RPMessage_Object *)handle;
        status = IPC_ETIMEOUT;
        if (FALSE == IpcUtils_QisEmpty(&obj->queue))
        {
            key = pOsalPrms->lockHIsrGate(module.gateSwi);

            payload = (RPMessage_MsgElem *)IpcUtils_QgetHead(&obj->queue);
            if ((NULL != payload) && (payload != (RPMessage_MsgElem *)&obj->queue))
            {
                if(NULL != pOsalPrms->lockMutex)
                {
                    /* This is to avoid semaphore count overflow, in cases where
                     * RPMessage_recvNb() is used in non-baremetal scenario.
                     * Because in non-baremetal case whenever a new message is received,
                     * RPMessage_enqueMsg internal API will post the semaphore (call unlockMutex)
                     * This may cause overflow and hence call lockMutex with timeout zero.
                     */
                    pOsalPrms->lockMutex(obj->semHandle, SemaphoreP_NO_WAIT);
                }

                if (((isCtrlMsg == TRUE)  && ((payload->len) < (CDD_IPC_MAX_CTRL_MSG_LEN))) || ((payload->len) < (CDD_IPC_MAX_MSG_LEN)))
                {
                     /* Message data starts on the first byte after the
                     * RPMessage_MsgElem structure in the dequeued object.
                     */
                    msgData = (void *)((uint32)payload + sizeof(*payload));
                    /* Now, copy payload to client and free our internal msg */
                    memcpy(data, msgData, payload->len);
                    *len = (uint16)(payload->len);
                    *rplyEndPt = payload->src;
                    *rplyProcId = payload->procId;

                    IpcUtils_HeapFree(&obj->heap, (void *)payload,
                        (payload->len + sizeof(RPMessage_MsgElem)));

                    status = IPC_SOK;
                }
                else
                {
                  status = IPC_EFAIL;
                }
            }
            else
            {
                /* No elements in the queue, but why? */
                status = IPC_EFAIL;
            }
            pOsalPrms->unLockHIsrGate(module.gateSwi, key);
        }
    }

    return (status);
}


/*
 *  ======== RPMessage_rawSend ========
 */

static int32_t RPMessage_rawSend(Virtio_Handle vq,
                      uint32 dstEndPt,
                      uint32 srcEndPt,
                      void*    data,
                      uint16 len)
{
    int32_t               status = IPC_SOK;
    int16_t               token = 0;
    int32_t               key = 0;
    int32_t               length = 0;
    uint32              bufSize;
    RPMessage_MsgHeader*  msg = NULL;
    Ipc_OsalPrms          *pOsalPrms = &gIpcObject.initPrms.osalPrms;

    if(NULL == data)
    {
        SystemP_printf("RPMessage_rawSend ...NULL data\n");
        status = IPC_EFAIL;
    }

    bufSize = sizeof(RPMessage_MsgHeader) + len;

    if ((NULL != pOsalPrms->lockHIsrGate) &&
        (NULL != pOsalPrms->unLockHIsrGate))
    {
        /* Send to remote processor: */
        key = pOsalPrms->lockHIsrGate(module.gateSwi);
        token = Virtio_getAvailBuf(vq, (void **)&msg, &length);
        pOsalPrms->unLockHIsrGate(module.gateSwi, key);
    }

    if(!msg)
    {
        SystemP_printf("RPMessage_rawSend ...NULL MsgHdr\n");
        status = IPC_EFAIL;
    }
    if((uint32)length < bufSize)
    {
        SystemP_printf("RPMessage_rawSend ...length %d, reqrd %d\n", length, bufSize);
        status = IPC_EFAIL;
    }
    if(status != IPC_EFAIL)
    {
        if (token >= 0)
        {
            /* Copy the payload and set message header: */
            memcpy((void *)msg->payload, data, len);
            msg->dataLen = len;
            msg->dstAddr = dstEndPt;
            msg->srcAddr = srcEndPt;
            msg->flags = 0;
            msg->srcProcId = Ipc_mpGetSelfId();

            key = pOsalPrms->lockHIsrGate(module.gateSwi);
            Virtio_addUsedBuf(vq, token, bufSize);
            Virtio_kick(vq);
            pOsalPrms->unLockHIsrGate(module.gateSwi, key);
        }
        else
        {
            SystemP_printf("RPMessage_rawSend ...Invalid token %d\n", token);
            status = IPC_EFAIL;
        }
    }

    return (status);
}
/*
 *  ======== RPMessage_send ========
 */
/* CDD_IPC_CoverageGap_20: Already check is provided which makes this part of code inaccessible. */
int32_t RPMessage_send(RPMessage_Handle handle, uint32 procId, uint32 dstEndPt,
     uint32 srcEndPt, void* data, uint16 len)
{
    Virtio_Handle       vq;
    uint32            endPt;
    int32_t             status = IPC_SOK;

    if(procId >= IPC_MAX_PROCS)
    {
        status = IPC_EFAIL;
    }

    if(procId == Ipc_mpGetSelfId())
    {
        status = IPC_EFAIL;
    }

    if(status != IPC_EFAIL)
    {
        vq = module.tx_VQs[procId];
        if(NULL == vq)
        {
            SystemP_printf("RPMessage_send ...NULL vq\n");
            status = IPC_EFAIL;
        }
        else
        {
            endPt = (handle == NULL) ? srcEndPt : handle->endPt;
            status = RPMessage_rawSend(vq, dstEndPt, endPt, data, len);
        }
    }

    return status;
}


/*
 *  ======== RPMessage_unblock ========
 */

void IpcInitPrms_init(uint32 instId, Ipc_InitPrms *initPrms)
{
    if(NULL != initPrms)
    {
        memset(initPrms, 0, sizeof(*initPrms));
        initPrms->instId                = instId;

        initPrms->virtToPhyFxn          = NULL;
        initPrms->phyToVirtFxn          = NULL;
        initPrms->newMsgFxn             = NULL;
        initPrms->printFxn              = NULL;


    }

    return;
}

Ipc_Object *getIpcObjInst(uint32 instId)
{
    return &gIpcObject;
}

int32_t Ipc_init(Ipc_InitPrms *cfg)
{
    int32_t retVal = IPC_EINVALID_PARAMS;

    memset(&gIpcObject, 0, sizeof(gIpcObject));

    if (NULL == cfg)
    {
        IpcInitPrms_init(0U, &gIpcObject.initPrms);

        retVal = IPC_SOK;
    }
    else
    {
        if (0U == cfg->instId)
        {
            if ((NULL == cfg->osalPrms.disableAllIntr) ||
                (NULL == cfg->osalPrms.restoreAllIntr))
            {
                retVal = IPC_EINVALID_PARAMS;
            }
            else
            {
                memcpy(&gIpcObject.initPrms, cfg, sizeof(gIpcObject.initPrms));
                retVal = IPC_SOK;
            }
        }
    }
    if (IPC_SOK == retVal)
    {
        retVal = IpcUtils_Init(&gIpcObject.initPrms.osalPrms);
        Ipc_mailboxModuleStartup();
    }
#if defined (IPC_CFG_PRINT_ENABLE)
    if (IPC_SOK == retVal)
    {
    /*CDD_IPC_CoverageGap_21: Default value for the osalPrms are set. */
        Ipc_OsalPrms *pOsalPrms;
        pOsalPrms = &gIpcObject.initPrms.osalPrms;

        if ((NULL != pOsalPrms->createMutex) &&
            (NULL != pOsalPrms->deleteMutex) &&
            (NULL != pOsalPrms->lockMutex) &&
            (NULL != pOsalPrms->unlockMutex))
        {
            gIpcObject.printLock = pOsalPrms->createMutex();
            if(NULL == gIpcObject.printLock)
            {
                retVal = IPC_EALLOC;
            }
            else
            {
                pOsalPrms->unlockMutex(gIpcObject.printLock);
            }
        }
    }
#endif
    gIpcObject.instId = 0U;

    return (retVal);
}

void Ipc_newMessageIsr(uint32 srcProcId)
{
    Ipc_mailboxIsr(srcProcId);

    return;
}

#define CDD_IPC_STOP_SEC_CODE
#include "Cdd_Ipc_MemMap.h"