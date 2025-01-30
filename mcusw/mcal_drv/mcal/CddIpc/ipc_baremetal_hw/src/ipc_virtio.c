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
 *  \file ipc_virtio.c
 *
 *  \brief Implementation of ipc rtos virtio.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <CddIpc/ipc_baremetal_hw/ipc.h>
#include <string.h>

#include "ipc_virtioPrivate.h"
#include "ipc_vring.h"
#include "ipc_mailbox.h"
#include "ipc_utils.h"
#include "ipc_priv.h"

#include <hw_include/arch/r5/lld_cache.h>

/* 512k for each vring */
#define IPC_VRING_ALIGNMENT    (0x1000)
#define IPC_VRING_BUF_CNT      (256)

typedef struct Ipc_VirtioInfo_s
{
    uint32  daTx;       /* Buffer for TX to remote core */
    uint32  daRx;       /* Buffer for RX from remote core */
    uint32  primeBuf;   /* Address for prime buffer */
    uint32  align;      /* Data alignment */
    uint32  num;        /* Number of buffer */
    uint32  selfId;     /* Self procId */
    uint32  remoteId;   /* Remote procId */
    uint32  txNotifyId; /* Notify Id for TX */
    uint32  rxNotifyId; /* Notify Id for RX */
} Ipc_VirtioInfo;


/* TranslationEntry */
typedef struct Ipc_TranslationEntry
{
    uint32  pa;  /* physical address */
    uintptr_t va;  /* virtual address  */
    uint32  len; /* Length of buffer */
}Ipc_TranslationEntry;

#define   IPC_TABLE_MAX_CNT   64

/* IPC PhyAddr to VirtAddr translation table */
typedef struct Ipc_TranslationTable
{
    uint32              count;
    /**< Number of entryies in the translation table */

    Ipc_TranslationEntry  entry[IPC_TABLE_MAX_CNT];
    /**< Entries in the table, there should be 3-entries for
     * each remote cores */

}Ipc_TranslationTable;

typedef struct Ipc_NotifyIdEntry_s
{
    int32_t    txNotifyId;
    int32_t    rxNotifyId;
} Ipc_NotifyIdEntry;

#define CDD_IPC_START_SEC_VAR_INIT_32
#include "Cdd_Ipc_MemMap.h"

int32_t  virtio_cnt = 0;

#define CDD_IPC_STOP_SEC_VAR_INIT_32
#include "Cdd_Ipc_MemMap.h"

/**
 *  \brief Used for defining the size of the virtqueue registry
 */
#define MAX_VIRTQUEUES          (IPC_MAX_PROCS * 2U)
#define MAX_VQ_PAIRS            ((MAX_VIRTQUEUES+1U)/2U)


#define DIV_ROUND_UP(n,d)   (((n) + (d) - 1) / (d))
#define RP_MSG_BUF_SIZE     (512)

/**
 *  \brief The alignment to use between consumer and producer parts of vring.
 *         Note: this is part of the "wire" protocol. If you change this,
 *         you need to update your BIOS image as well
 */
#define RP_MSG_VRING_ALIGN  (4096)

/* With 256 buffers, the vring will occupy 3 pages
 *
 * RP_MSG_RING_SIZE = ((DIV_ROUND_UP(vring_size(RP_MSG_NUM_BUFS,
 *                    RP_MSG_VRING_ALIGN), PAGE_SIZE)) * PAGE_SIZE)
 *
 * The total IPC space needed to communicate with a remote processor
 *
 * RPMSG_IPC_MEM = (RP_MSG_BUFS_SPACE + 2 * RP_MSG_RING_SIZE)
 */

typedef struct Virtio_Object_s
{
    uint16          id;
    /**< Id for this VirtQueue_Object */

    Virtio_callback   callback;
    /**< The function to call when buffers are consumed (can be NULL) */

    uint32*         callback_priv;
    /**< The private data to pass to the callback (can be NULL) */

    struct VRing      vring;
    /**< Shared state */

    uint16          last_avail_idx;
    /**< Last available index; updated by VirtQueue_getAvailBuf */

    uint16          last_used_idx;
    /**< Last available index; updated by VirtQueue_addUsedBuf */

    uint16          procId;
    /** Will eventually be used to kick remote processor */

    uint8          status;
    /**< Address of the vring status variable in shared memory.
     For TX vq, the address is for this proc's variable.
     For RX vq, the address is for the peer proc's variable.*/

    VIRTIO_DIR        direction;
    /**< Indicates TX or RX direction */

    void*             rscTable;
    /**< Address of resource table. This address is provided by
    application. If the remote core is not A72 or Linux is not
    running, it should be NULL */

    uint32        timeoutCnt;
    /**< Timeout count used for mailbox send. If the mailbox FIFO
     * is full, then how long it should try.
     * 0xFFFFFFFF  --> ForEver
     * */

} Virtio_Object;

/**
 *  \brief Vring parameter
 */
typedef struct Vring_Params_s
{
    uint32  num;
    uintptr_t addr;
    uint32  align;
} Vring_Params;

#define CDD_IPC_START_SEC_VAR_NO_INIT_PTR
#include "Cdd_Ipc_MemMap.h"

static Virtio_Object *queueRegistry[MAX_VIRTQUEUES];

#define CDD_IPC_STOP_SEC_VAR_NO_INIT_PTR
#include "Cdd_Ipc_MemMap.h"

/* Application provides the memory for storing local Virtio */
#define CDD_IPC_START_SEC_VAR_INIT_PTR
#include "Cdd_Ipc_MemMap.h"

static void*    vqBaseAddr = NULL;


#define CDD_IPC_STOP_SEC_VAR_INIT_PTR
#include "Cdd_Ipc_MemMap.h"

#define CDD_IPC_START_SEC_VAR_INIT_32
#include "Cdd_Ipc_MemMap.h"

static uint32 vqSizeLeft = 0U;

#define CDD_IPC_STOP_SEC_VAR_INIT_32
#include "Cdd_Ipc_MemMap.h"

#define CDD_IPC_START_SEC_VAR_INIT_PTR
#include "Cdd_Ipc_MemMap.h"

static void*    rscTable   = NULL;

#define CDD_IPC_STOP_SEC_VAR_INIT_PTR
#include "Cdd_Ipc_MemMap.h"

#define CDD_IPC_START_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Cdd_Ipc_MemMap.h"

static Ipc_TranslationTable  vrTranslationTable;

#define CDD_IPC_STOP_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Cdd_Ipc_MemMap.h"

Virtio_Handle Virtio_create(uint32 vqId, uint32 procId, Virtio_callback callback, Vring_Params *params,
   VIRTIO_DIR direction, uint8 status, uint32 timeoutCnt);
void Virtio_prime(Virtio_Object *vq, uintptr_t addr, uint32 num);
int32_t VirtioIPC_createVirtioCorePair(Ipc_VirtioInfo* vqInfo, uint32 timeoutCnt);
void Ipc_updateVirtioInfo(uint32 numProc, void *baseAddr, uint32 vrBufSize, Ipc_VirtioInfo* info);

#define CDD_IPC_START_SEC_CODE
#include "Cdd_Ipc_MemMap.h"

void* Ipc_allocVirtio(void)
{
    void  *ptr = NULL;
    if(vqSizeLeft >= sizeof(Virtio_Object))
    {
        ptr = (void*)((uintptr_t)vqBaseAddr + (((uint32)virtio_cnt) * sizeof(Virtio_Object)));
        virtio_cnt++;
        vqSizeLeft -= sizeof(Virtio_Object);
    }
    else
    {
      /* Nothing to do */
    }

    return ptr;
}

/**
 * \brief    Add entry to the translation able
 **/

/**
 * \brief Get virual address for given physical address
 **/
/* CDD_IPC_CoverageGap_22: The function Ipc_physToVirt() is not called in the CDD driver code.*/
int32_t Ipc_physToVirt(uint32 pa, uintptr_t *va)
{
    uint32                n;
    uint32                offset;
    Ipc_TranslationEntry   *e;
    Std_ReturnType status = IPC_EFAIL;
    *va = 0U;

    if(vrTranslationTable.count == 0U)
    {
        *va = pa;
    }

    for (n = 0; n < vrTranslationTable.count; n++)
    {
        e = &vrTranslationTable.entry[n];
        if ((pa >= e->pa) && (pa < (e->pa + e->len)))
        {
            offset = pa - e->pa;
            *va = e->va + offset;
            status = IPC_SOK;
        }
        else
        {
          /* Do nothing */
        }
    }

    return (status);
}

/**
 * \brief Get physical address from given virual address
 **/
/* CDD_IPC_CoverageGap_23: The function Ipc_virtToPhys() is not called in the CDD driver code.*/
int8_t Ipc_virtToPhys(uintptr_t va, uint32 *pa)
{
    uint32              n;
    uint32              offset;
    Ipc_TranslationEntry *e;
    Std_ReturnType status = IPC_EFAIL;
    *pa = 0U;

    if(vrTranslationTable.count == 0U)
    {
        *pa = va;
    }

    for (n = 0; n < vrTranslationTable.count; n++)
    {
        e = &vrTranslationTable.entry[n];
        if ((va >= e->va) && (va < (e->va + e->len)))
        {
            offset = va - e->va;
            *pa = e->pa + offset;
            status = IPC_SOK;
        }
        else
        {
          /* Do Nothing */
        }
    }

    return (status);
}
/*CDD_IPC_CoverageGap_37: Ipc_getVqObjMemoryRequiredPerCore - This API is used in ETH driver, so it will be covered in ETH LDRA - MCAL-9188(JIRA task)*/
uint32 Ipc_getVqObjMemoryRequiredPerCore(void)
{
    /* Two VRing (TX/RX) for each core combination */
    return (sizeof(Virtio_Object) * 2U);
}

/**
 * \brief    Checks if remote is A72 with Linux
 *
 * \param  procId  [IN] Remote proc Id
 *
 * \return  1 if remote core is A72 & valid resource table
 *          0 otherwise
 *
 */

uint8 Virtio_isRemoteLinux(uint16 procId)
{
    uint8  retVal;

    /*Conditional check added 
     *as different SOC's have 
     *different Linux core nomenclature's*/
    #if defined(SOC_J7200) || defined(SOC_J721E) ||defined (SOC_J721S2) || defined (SOC_J784S4)
    if((procId == IPC_MPU1_0) && (rscTable != NULL))
    #elif defined(SOC_AM62X)
    if((procId == IPC_A53SS_1) && (rscTable != NULL))
    #elif defined(SOC_AM62AX) || defined(SOC_AM62PX) || defined(SOC_J722S)
    if((procId == CDD_IPC_CORE_A53SS0_0) && (rscTable != NULL))
    #endif
    {
        retVal = 1;
    }
    else
    {
        retVal = 0;
    }
    return retVal;
}

/**
 * \brief  Checks if remote is ready
 *
 * \return  1 if remote proc is is not A72 or resource table is null
 *            or linux vdev status is 0x7
 *          0 if linux vdev status is not 0x7
 *
 */
/* CDD_IPC_CoverageGap_24: This function Ipc_isRemoteReady() checks for RemoteLinux which is never true. Hence, this part cannot be covered.*/
uint8 Ipc_isRemoteReady(uint16 procId)
{
    uint8            retVal = TRUE;
    Ipc_ResourceTable *rsc    = NULL;

    if(TRUE == Virtio_isRemoteLinux(procId))
    {
        rsc = (Ipc_ResourceTable*)rscTable;
        //CacheP_Inv(rsc, sizeof(Ipc_ResourceTable));
        CSL_armR5CacheInv(rsc, sizeof(Ipc_ResourceTable));
        if(rsc->rpmsg_vdev.status != 0x7U)
        {
            retVal = FALSE;
        }
        else
        {
          /* Do Nothing */
        }
    }
    else
    {
      /* Do Nothing */
    }

    return retVal;
}

/**
 *  \brief Initializes the registry
 */
int32_t Ipc_initVirtIO(Ipc_VirtIoParams *vqParam)
{
    int32_t   retVal = IPC_SOK;

    if( (vqParam == NULL) ||
        (vqParam->vringBufSize < IPC_VRING_BUFFER_SIZE) ||
		(vqParam->vqBufSize == 0U))
    {
        SystemP_printf("Ipc_initVirtIO : Invalid Memory, Failed\n");
        retVal = IPC_EFAIL;
    }

    if(retVal == IPC_SOK)
    {
        /* Initialize the global queue registry */
        memset(queueRegistry, 0, sizeof(queueRegistry));

        vqBaseAddr = (void*)vqParam->vqObjBaseAddr;
        vqSizeLeft = vqParam->vqBufSize;

        /* Initialize the shared page table and VRing */
        retVal = VirtioIPC_init(vqParam);
    }

    return retVal;
}

/**
 *  \brief Saves the resourcetable to be used CDD_IPC_CORE_A53SS0_0
 *  vring for remotecore A72 running Linux
 */
 /**
 *  \brief Placeholder for MMU Address translation
 */
/* CDD_IPC_CoverageGap_25: The MACRO – IPC_EXCLUDE_CTRL_TASKS 
 *should not be defined for this part of code to be covered. It is already defined in driver makefile. */
static inline void * mapPAtoVA(uint32 pa)
{
    uintptr_t va = 0U;

    Ipc_physToVirt(pa, &va);

    return (void*)va;
}

/**
 *  \brief Placeholder for MMU Address translation
 */
static inline uint32 mapVAtoPA(void * va)
{
    uint32 pa;

    Ipc_virtToPhys((uintptr_t)va, &pa);

    return pa;
}

/**
 *  \brief Check the vq status
 */

uint8 Virtio_isReady(Virtio_Handle vq)
{
    return (uint8)(vq->status);
}

/**
 *  \brief Get the processor Id
 */
uint32 Virtio_getProcId(Virtio_Handle vq)
{
    return vq->procId;
}

/**
 *  \brief Virtio kick
 */
void Virtio_kick(Virtio_Handle vq)
{
    uint32 selfId = Ipc_mpGetSelfId();

#ifdef QNX_OS
    asm("   DSB ST");
#endif

    /* For now, simply interrupt remote processor */
    if (0U == (vq->vring.avail->flags & VRING_AVAIL_F_NO_INTERRUPT))
    {
        if (IPC_SOK != Ipc_mailboxSend(selfId, vq->procId, vq->id, vq->timeoutCnt))
        {
            SystemP_printf("Virtio_kick: Ipc_mailboxSend failed\n");
        }
        else
        {
          /* Do Nothing */
        }
    }
    else
    {
      /* Do Nothing */
    }
}

/**
 *  \brief Virtio_addUsedBuf
 */
int32_t Virtio_addUsedBuf(Virtio_Handle vq, int16_t head, int32_t len)
{
    struct vring_used_elem *used;

    /*
    * The virtqueue contains a ring of used buffers.  Get a pointer to the
    * next entry in that used ring.
    */
    used = &vq->vring.used->ring[vq->vring.used->idx % vq->vring.num];
    used->id = head;
    used->len = len;

#ifdef QNX_OS
    asm("   DMB ST");
#endif

    vq->vring.used->idx++;

    return (0);
}

/**
 *  \brief Virtio_addAvailBuf
 */
void Virtio_addAvailBuf(Virtio_Handle vq, void *buf, uint16 head)
{
    uint16 avail;

    avail = (uint16)(vq->vring.avail->idx % vq->vring.num);

    vq->vring.desc[head].addr   = mapVAtoPA(buf);
    vq->vring.desc[head].len    = RP_MSG_BUF_SIZE;
    vq->vring.desc[head].flags  = 2;
    vq->vring.avail->ring[avail] = head;

    /*
     * Descriptors and available array need to be set before we expose the
     * new available array entries.
     */
#ifdef QNX_OS
    asm("   DMB ST");
#endif

    vq->vring.avail->idx++;
}

/**
 *  \brief Virtio_getUsedBuf ========
 */
void *Virtio_getUsedBuf(Virtio_Handle vq, uint16 *token)
{
    uint16 head;
    void     *buf = NULL;

    /* There's nothing available? */
    if (vq->last_used_idx != vq->vring.used->idx)
    {
#ifdef QNX_OS
        asm("   DMB SY");
#endif

        head = (uint16)(vq->vring.used->ring[vq->last_used_idx % vq->vring.num].id);
        vq->last_used_idx++;

        buf = mapPAtoVA(vq->vring.desc[head].addr);
        *token = head;
    }

    return (buf);
}

/**
 *  \brief VirtQueue_getAvailBuf
 */
int16_t Virtio_getAvailBuf(Virtio_Handle vq, void **buf, int32_t *len)
{
    uint16 head;
    int16_t  retVal;

    /* There's nothing available? */
    if (vq->last_avail_idx == vq->vring.avail->idx)
    {
        /* We need to know about added buffers */
        vq->vring.used->flags &= (uint16)~VRING_USED_F_NO_NOTIFY;
        retVal = -1;
    }
    else
    {
        /*
         * Grab the next descriptor number they're advertising, and increment
         * the index we've seen.
         */

#ifdef QNX_OS
        /* Only get avail array entries after they have been exposed. */
        asm("   DMB SY");
#endif

        head = vq->vring.avail->ring[vq->last_avail_idx % vq->vring.num];
        vq->last_avail_idx++;

        *buf = mapPAtoVA(vq->vring.desc[head].addr);
        *len = (int32_t)vq->vring.desc[head].len;
        retVal = (int16_t)head;
    }

    return (retVal);
}



/**
 *  \brief Virtio_enableCallback
 */

/**
 *  \brief Virtio_isr
 */
void Virtio_isr(uint32* msg, uint32 priv)
{
    Virtio_Object *vq;

    vq = queueRegistry[(2U*priv)+1U];
    if ((vq != NULL) && (vq->callback != NULL))
    {
      if((vq && vq->procId) == (priv && vq->callback))
      {
          vq->callback(vq->callback_priv);
      }
    }
}

/**
 *  \brief Virtio_create
 */
 Virtio_Handle Virtio_create(uint32 vqId, uint32 procId,
    Virtio_callback callback, Vring_Params *params,
    VIRTIO_DIR direction, uint8 status, uint32 timeoutCnt)
{
    int32_t retVal;
    Virtio_Object *vq = NULL;

    if (procId < IPC_MAX_PROCS)
    {
        vq = (Virtio_Object *)Ipc_allocVirtio();
    }

    if (NULL != vq)
    {
        vq->callback       = callback;
        vq->id             = (uint16)vqId;
        vq->procId         = (uint16)procId;
        vq->status         = status;
        vq->last_avail_idx = 0;
        vq->last_used_idx  = 0;
        vq->direction      = direction;
        vq->timeoutCnt     = timeoutCnt;

        vring_init(&(vq->vring), params->num, (void*)params->addr, params->align);

        /* Each processor clears only its TX vq memory. */
        if (direction == VIRTIO_TX)
        {
            memset((void*)params->addr, 0, vring_size(params->num, params->align));
            /* Don't trigger a mailbox message every time remote rpoc */
            /* makes another buffer available.                        */
            vq->vring.used->flags |= VRING_USED_F_NO_NOTIFY;
            queueRegistry[(2U)*procId] = vq;
        }

        if (direction == VIRTIO_RX)
        {
            uint32 selfId = Ipc_mpGetSelfId();
            queueRegistry[(2U*procId)+1U] = vq;
            retVal = Ipc_mailboxRegister((uint16)selfId, (uint16)procId, Virtio_isr, procId, timeoutCnt);
            if (retVal != IPC_SOK)
            {
                 SystemP_printf("Virtio_create : Failed to register mailbox\n");
            }
        }
    }

    return (vq);
}


/**
 *  \brief Virtio_getHandle
 */

Virtio_Handle Virtio_getHandle(uint32 procId, VIRTIO_DIR dir)
{
    uint32 vqIndex = (2U*procId) + (uint32)dir;
    return queueRegistry[vqIndex];
}

/**
 *  \brief Virtio_setCallback
 */
int32_t Virtio_setCallback(uint32 procId, Virtio_callback callback, uint32* priv)
{
    int32_t status = 1;
    Virtio_Object *vq = Virtio_getHandle(procId, VIRTIO_RX);

    if (NULL != vq)
    {
        vq->callback = callback;
        vq->callback_priv = priv;
        status = 0;
    }

    return status;
}


/**
 *  \brief Setups vring buffers.  The buffers are "allocated" from
 *  a single block of memory beginning at addr.  It is assumed that
 *  addr has the appropriate alignment.
 */
void Virtio_prime(Virtio_Object *vq, uintptr_t addr, uint32 num)
{
    uint32 i;
    uint8 *buf;

    /* Add buffers to the vring's available ring. */
    buf = (uint8 *)addr;
    for (i = 0; i < num; i++)
    {
        Virtio_addAvailBuf(vq, (void *)(uintptr_t)buf, (uint16)i);
        buf += RP_MSG_BUF_SIZE;
    }
}

#define CDD_IPC_STOP_SEC_CODE
#include "Cdd_Ipc_MemMap.h"

/* This array serves as a way to look up virtQueues when creating higher
 * level transports, such as RPMessage.  So far, the convention is
 * that the first pair of vrings defined for a remote proc will be
 * used for the primary RPMessage transport.
 *
 * Since none of this info is derived, it's all in the shared page, the
 * search could operate directly on the shared page instead of using
 * this array.
 */
static struct VirtioIPC_Vqdev
{
    uint32 virtioId;
    uint32 rank;    /* zero for primary... */
    uint32 tx_vqId;
    uint32 rx_vqId;
    uint32 procId;
} g_vqTable[MAX_VQ_PAIRS];

#define CDD_IPC_START_SEC_VAR_INIT_32
#include "Cdd_Ipc_MemMap.h"

static uint32 g_vqItemCnt = 0;

#define CDD_IPC_STOP_SEC_VAR_INIT_32
#include "Cdd_Ipc_MemMap.h"

/**
 *  \brief VirtioIPC_getVirtQueues :
 *           Look up the VirtQueues by procId.
 *           Rank is 0 for primary VQs.
 */

#define CDD_IPC_START_SEC_CODE
#include "Cdd_Ipc_MemMap.h"

uint8 VirtioIPC_getVirtQueues(uint32 type, uint32 procId, uint32 rank,
                             uint32 *tx_vqId, uint32 *rx_vqId)
{
    int32_t i;
    uint8 retVal = FALSE;

    for (i = 0; (uint32)i < g_vqItemCnt; i++)
    {
        if ((type   == g_vqTable[i].virtioId) &&
            (rank   == g_vqTable[i].rank)     &&
            (procId == g_vqTable[i].procId))
        {
            *tx_vqId = g_vqTable[i].tx_vqId;
            *rx_vqId = g_vqTable[i].rx_vqId;
            retVal   = TRUE;
            break;
        }
    }

    return retVal;
}

/**
 * \brief Creates Virtio for TX and RX for given given core pair.
 * Must be called multiple times with for each remote core.
 * It will result in a TX/RX pair of Virtio being created.
 * One assumption made is that all vrings will be accessed as Virtio.
 * The Virtio callback to the higher level transport is set to
 * Null.  It must be changed when higher level transport is initialized.
 */
int32_t VirtioIPC_createVirtioCorePair(Ipc_VirtioInfo* vqInfo, uint32 timeoutCnt)
{
    Vring_Params   params;
    Virtio_Handle  tx_vq, rx_vq;
    uint8        status = 0;
    int32_t        retVal = IPC_SOK;

    /* TX VQ */
    params.num   = vqInfo->num;
    params.addr  = (uintptr_t)mapPAtoVA(vqInfo->daTx);
    params.align = vqInfo->align;
    tx_vq = Virtio_create(vqInfo->txNotifyId, vqInfo->remoteId, NULL,
                  &params, VIRTIO_TX, status, timeoutCnt);
    if (tx_vq == NULL)
    {
        SystemP_printf("TX Virtio creation failure: %d\n", vqInfo->txNotifyId);
        retVal = IPC_EFAIL;
    }
    if(retVal == IPC_SOK)
    {
    /* CDD_IPC_CoverageGap_26: The condition will always be false as the array is not filled before this condition. So this is not covered.*/
        /* Prime the transmit buffer */
        uint32 *primeBuf = (uint32 *)mapPAtoVA(vqInfo->primeBuf);
        Virtio_prime(tx_vq, (uintptr_t)primeBuf, vqInfo->num);

        /* RX VQ */
        params.num   = vqInfo->num;
        params.addr  = (uintptr_t)mapPAtoVA(vqInfo->daRx);
        params.align = vqInfo->align;
        rx_vq = Virtio_create(vqInfo->rxNotifyId, vqInfo->remoteId, NULL,
                  &params, VIRTIO_RX, status, timeoutCnt);
        /*CDD_IPC_CoverageGap_27: SelfId and RemoteId are fixed enums which cannot be changed.
         *Also, testing is done only for cores 2_1 and 2_0. So this is not covered .*/
        if (rx_vq == NULL)
        {
            SystemP_printf("RX Virtio creation failure: %d\n", vqInfo->rxNotifyId);
            retVal = IPC_EFAIL;
        }
    }

    if(retVal == IPC_SOK)
    {
    /* CDD_IPC_CoverageGap_26: The condition will always be false as the array is not filled before this condition. So this is not covered.*/
        /* Add to global registry */
        g_vqTable[g_vqItemCnt].virtioId = VIRTIO_ID_RPMSG;
        g_vqTable[g_vqItemCnt].rank     = 0;
        g_vqTable[g_vqItemCnt].tx_vqId  = vqInfo->txNotifyId;
        g_vqTable[g_vqItemCnt].rx_vqId  = vqInfo->rxNotifyId;
        g_vqTable[g_vqItemCnt].procId   = vqInfo->remoteId;
        g_vqItemCnt++;
    }

    return retVal;
}

uint8 Ipc_isRemoteVirtioCreated(uint32 remoteId)
{
    uint8      vqCreated = FALSE;
    uint32  index     = 0;

    for(index = 0; index < g_vqItemCnt; index++)
    {
        if (remoteId == g_vqTable[index].procId)
        {
        /*CDD_IPC_CoverageGap_28: SelfId and RemoteId are fixed enums which cannot be changed.
         *Also, testing is done only for cores 2_1 and 2_0. So this is not covered .*/  
            vqCreated = TRUE;
            break;
        }
    }
    return vqCreated;
}

/* CDD_IPC_CoverageGap_29: The condition will always be false as the array is not filled before this condition. So this is not covered.*/
void Ipc_updateVirtioInfo(uint32 numProc, void *baseAddr, uint32 vrBufSize,
                      Ipc_VirtioInfo* info)
{
    uint32 cnt = 0, a , b, i;
#ifdef QNX_OS
    uint32 revPrimeBuf;
#endif

    if(info->remoteId > info->selfId)
    {
    
        a = info->selfId;
        b = info->remoteId;
    }
    else
    {
        a = info->remoteId;
        b = info->selfId;
    }

    for(i = 0; i < a; i++)
    {
        cnt += (numProc - i - 1U);
    }
    cnt += ((b - a) - 1U);
    cnt *= 4U;

    if(info->remoteId > info->selfId)
    {
        info->daTx       = (uint32)((uintptr_t)baseAddr + (cnt * vrBufSize));
        info->daRx       = (uint32)((uintptr_t)baseAddr + ((cnt+2U) * vrBufSize));
        info->primeBuf   = (uint32)((uintptr_t)baseAddr + ((cnt+1U) * vrBufSize));
#ifdef QNX_OS
        revPrimeBuf      = (uint32)((uintptr_t)baseAddr + ((cnt+3U) * vrBufSize));
#endif
        info->txNotifyId = 1;
        info->rxNotifyId = 0;
    }
    else
    {
        info->daTx       = (uint32)((uintptr_t)baseAddr + ((cnt+2U) * vrBufSize));
        info->daRx       = (uint32)((uintptr_t)baseAddr + (cnt * vrBufSize));
        info->primeBuf   = (uint32)((uintptr_t)baseAddr + ((cnt+3U) * vrBufSize));
#ifdef QNX_OS
        revPrimeBuf      = (uint32)((uintptr_t)baseAddr + ((cnt+1U) * vrBufSize));
#endif
        info->txNotifyId = 0;
        info->rxNotifyId = 1;
    }

    /* If remote core is Linux, and vdev is ready
     * update the address
     */
    if(TRUE == Virtio_isRemoteLinux((uint16)(info->remoteId)))
    {
        /*CDD_IPC_CoverageGap_30: This function checks for RemoteLinux which is never true. Hence, this part cannot be covered.*/
        Ipc_ResourceTable *rsc = (Ipc_ResourceTable*)rscTable;
        info->daTx      = rsc->rpmsg_vring0.da;
        info->daRx      = rsc->rpmsg_vring1.da;
        info->primeBuf  = info->daTx + (2U*vrBufSize);

#if defined(SOC_AM62X)  ||  defined(SOC_AM62AX) || defined(SOC_AM62PX) || defined(SOC_J722S)
        info->txNotifyId = 0;
        info->rxNotifyId = 1;
#endif

    }

#ifdef QNX_OS
    /* For QNX, mmap to local process space */
    Ipc_addTranslationEntry(info->daTx, vrBufSize);
    Ipc_addTranslationEntry(info->daRx, vrBufSize);
    Ipc_addTranslationEntry(info->primeBuf, vrBufSize);
    Ipc_addTranslationEntry(revPrimeBuf, vrBufSize);
#endif
#if DEBUG_PRINT
    SystemP_printf("Virtio: remote %d, daTx 0x%x, daRx 0x%x, prime 0x%x\n",
        info->remoteId, info->daTx, info->daRx, info->primeBuf);
#endif

}

int32_t VirtioIPC_init(Ipc_VirtIoParams *vqParams)
{
    int32_t          retVal = IPC_SOK;
    uint32         selfId;
    uint32        numProcessors = Ipc_mpGetNumProcessors();
    Ipc_VirtioInfo   vqInfo;
    uint32         procId;
    uint32         index;

    selfId = Ipc_mpGetSelfId();

    vqInfo.align = IPC_VRING_ALIGNMENT;
    vqInfo.num   = IPC_VRING_BUF_CNT;
    vqInfo.selfId = selfId;

    for(index = 0; index < numProcessors; index++)
    {
        procId = Ipc_mpGetRemoteProcId(index);

        /* if virtio is already created for this core then ignore */
        /*CDD_IPC_CoverageGap_31: Valid memory is always shared between remote and cdd ipc driver. 
         *So this condition will always be False. Check is already added for Invalid memory. So this part cannot be covered.*/
        if(TRUE == Ipc_isRemoteVirtioCreated(procId))
        {
            continue;
        }

        if(FALSE == Ipc_isRemoteReady((uint16)procId))
        {
            /* Linux on A72 is not ready, go to next core
             * Virtio is not created at this moment, it must be
             * created on demand.
             */
            continue;
        }
        vqInfo.remoteId   = procId;
        Ipc_updateVirtioInfo(IPC_MAX_PROCS, vqParams->vringBaseAddr, IPC_VRING_SIZE, &vqInfo);
        retVal = VirtioIPC_createVirtioCorePair(&vqInfo, vqParams->timeoutCnt);
        if(retVal != IPC_SOK)
        {
            SystemP_printf("VirtioIPC_init: Failed to create VirtIO for procId..\n",
                 procId);
            break;
        }
    }
    return retVal;
}

/**
 *  \brief Saves the resourcetable to be used for
 *  vring for remotecore A72/A53 running Linux
 */

int32_t Ipc_loadResourceTable(void *rsctable){
	rscTable = rsctable;
	return IPC_SOK;
}

#define CDD_IPC_STOP_SEC_CODE
#include "Cdd_Ipc_MemMap.h"
