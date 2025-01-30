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
 *  \file ipc_utils.c
 *
 *  \brief Implementation of ipc utility functions such as queues,
 *          buffer manager, etc...
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <CddIpc/ipc_baremetal_hw/ipc.h>
#include "ipc_utils.h"
#include "ipc_priv.h"

#include <hw_include/lld_types.h>
#include <CddIpc/ipc_baremetal_hw/soc/ipc_soc.h>
#include <CddIpc/ipc_baremetal_hw/include/ipc_types.h>

#if defined (IPC_CFG_PRINT_ENABLE)
/* This is needed for vsnprintf */
#include <stdio.h>
#include <stdarg.h>
#endif

/* ========================================================================== */
/*                            Local Types/Defines                             */
/* ========================================================================== */
#define IPC_UTILS_UN_INITIALIZED        (0U)
/**< State Un Initialized */
#define IPC_UTILS_INITIALIZED           (1U)
/**< State Un Initialized */

/** \brief Utils Object
 */
typedef struct Ipc_UtilsObj_s
{
    uint32                    state;
    /**< State indicator */
}Ipc_UtilsObj;

/* ========================================================================== */
/*                               Globals                                      */
/* ========================================================================== */

#define CDD_IPC_START_SEC_VAR_INIT_UNSPECIFIED
#include "Cdd_Ipc_MemMap.h"

Ipc_UtilsObj gIpcUtilsObj = {IPC_UTILS_UN_INITIALIZED};

#define CDD_IPC_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "Cdd_Ipc_MemMap.h"

/**< Function disable all interrupts */

/* ========================================================================== */
/*                              API's                                         */
/* ========================================================================== */

#define CDD_IPC_START_SEC_CODE
#include "Cdd_Ipc_MemMap.h"

int32_t IpcUtils_Init(Ipc_OsalPrms *pOsalInit)
{
    int32_t rtnVal = IPC_EINVALID_PARAMS;
    if (NULL != pOsalInit)
    {
        gIpcUtilsObj.state          = IPC_UTILS_INITIALIZED;
        rtnVal = IPC_SOK;
    }
    return (rtnVal);
}

/* Queue Related */
void* IpcUtils_QgetHead(IpcUtils_QHandle *handle)
{
    IpcUtils_QElem *elem = NULL;
    IpcUtils_QElem *next = NULL;

    if (IPC_UTILS_INITIALIZED == gIpcUtilsObj.state)
    {
        elem = handle->next;
        next = elem->next;
        handle->next = next;
        next->prev = handle;
    }

    return (elem);
}

uint8 IpcUtils_QisEmpty(IpcUtils_QHandle *handle)
{
    uint8 rtnVal = 0U;
    if (handle->next == handle)
    {
        rtnVal = TRUE;
    }
    return (rtnVal);
}

void IpcUtils_Qput(IpcUtils_QHandle *handle, IpcUtils_QElem *elem)
{
    if ((IPC_UTILS_INITIALIZED == gIpcUtilsObj.state) &&
        (NULL != elem))
    {

        elem->next = handle;
        elem->prev = handle->prev;
        handle->prev->next = elem;
        handle->prev = elem;
    }
    return;
}

void IpcUtils_Qenqueue(IpcUtils_QHandle *handle, IpcUtils_QElem *elem)
{
    if ((IPC_UTILS_INITIALIZED == gIpcUtilsObj.state) &&
        ((NULL != elem) ))
    {
        IpcUtils_QElem *prev = NULL;

        prev = handle->prev;
        elem->next = handle;
        elem->prev = prev;
        prev->next = elem;
        handle->prev = elem;
    }
    return;
}

void* IpcUtils_Qnext(IpcUtils_QElem *qelem)
{
    void *rtnVal = NULL;
    if (NULL != qelem)
    {
        rtnVal = qelem->next;
    }

    return rtnVal;
}

void IpcUtils_Qremove(IpcUtils_QElem *qelem)
{
    if (NULL != qelem)
    {
        qelem->prev->next = qelem->next;
        qelem->next->prev = qelem->prev;
    }

    return;
}

void IpcUtils_Qdelete(IpcUtils_QHandle *handle)
{
    if (NULL != handle)
    {
        handle->next = NULL;
        handle->prev = NULL;
    }

    return;
}

int32_t IpcUtils_Qcreate(IpcUtils_QHandle *handle)
{
    int32_t rtnVal = IPC_EINVALID_PARAMS;
    if ((NULL != handle) && (IPC_UTILS_INITIALIZED == gIpcUtilsObj.state))
    {
        handle->next = handle;
        handle->prev = handle;
        rtnVal = IPC_SOK;
    }

    return (rtnVal);
}

int32_t IpcUtils_HeapCreate(IpcUtils_HeapHandle *pHndl,
                            IpcUtils_HeapParams *param)
{
    uint8 *tempBufPtr;
    uint32 idx;
    int32_t rtnVal = IPC_EINVALID_PARAMS;

    if (((NULL != param) && (IPC_UTILS_INITIALIZED == gIpcUtilsObj.state)) &&
        ((NULL != param->buf) && (NULL != pHndl)))
    {
        if (param->bufSize >= (param->numBlocks * param->blockSize))
        {
            rtnVal = IPC_SOK;
        }
    }

    if (IPC_SOK == rtnVal)
    {
        /*
            . Setup Heap Handle variables
            . Create Q
            . Slice buffer provided and insert them into Q
        */
        pHndl->numFreeBlocks = 0U;
        pHndl->blockSize = param->blockSize;

        rtnVal = IpcUtils_Qcreate(&pHndl->qHandle);
        if (IPC_SOK == rtnVal)
        {
            tempBufPtr = (uint8 *)param->buf;
            for (idx = 0U; idx < param->numBlocks; idx++)
            {
                /* Will flag MISRA C Violation for tempBufPtr casting,
                    no fix? */
                IpcUtils_Qput(&pHndl->qHandle, (IpcUtils_QElem *) tempBufPtr);

                tempBufPtr += param->blockSize;
                pHndl->numFreeBlocks++;
            }
        }
    }
    return rtnVal;
}

void IpcUtils_HeapDelete(IpcUtils_HeapHandle *pHndl)
{
    if (NULL != pHndl)
    {
        IpcUtils_Qdelete(&pHndl->qHandle);
        pHndl->numFreeBlocks = 0U;
    }
    return;
}

void *IpcUtils_HeapAlloc(IpcUtils_HeapHandle *pHndl, uint32 size,
                            uint32 align)
{
    void *rtnVal = NULL;
    if ((NULL != pHndl) && (IpcUtils_QisEmpty(&pHndl->qHandle) == 0U))
    {
        rtnVal = IpcUtils_QgetHead(&pHndl->qHandle);
        pHndl->numFreeBlocks--;
    }

    return (rtnVal);
}

void IpcUtils_HeapFree(IpcUtils_HeapHandle *pHndl, void* block, uint32 size)
{
    if ((NULL != pHndl) && (NULL != block))
    {
        /* Will flag MISRA C Violation for tempBufPtr casting,
            no fix? */
        IpcUtils_Qenqueue(&pHndl->qHandle, (IpcUtils_QElem *) block);
        pHndl->numFreeBlocks++;
    }

    return;
}

void SystemP_printf(const char *format, ...)
{
#if defined (IPC_CFG_PRINT_ENABLE)
    va_list     vaArgPtr;
    char       *buf;
    Ipc_Object      *pObj  = NULL;
    Ipc_OsalPrms    *pOsal = NULL;

    pObj = getIpcObjInst(0U);
    pOsal = &pObj->initPrms.osalPrms;

    if(NULL != pObj->initPrms.printFxn)
    {
        if(NULL != pOsal->lockMutex)
        {
            pOsal->lockMutex(pObj->printLock, SemaphoreP_WAIT_FOREVER);
        }

        buf = &pObj->printBuf[0];
        (void) va_start(vaArgPtr, format);
        (void) vsnprintf(
            buf, IPC_PRINT_BUF_LEN, (const char *) format, vaArgPtr);
        va_end(vaArgPtr);

        pObj->initPrms.printFxn("[IPC] ");
        pObj->initPrms.printFxn(buf);

        /* This assumes that both lock/unlock will be both provided or not
         * provided. Any other combo will result in invalid lock operation */
        if(NULL != pOsal->unlockMutex)
        {
            pOsal->unlockMutex(pObj->printLock);
        }
    }
#endif

    return;
}

uint32 IpcUtils_strnlen(const unsigned char *string, uint32 stringlen)
{
        uint32 len;
        uint32 offset = 0U;

        for (len = 0; len < stringlen; len++)
        {
                if ((*string+offset) == 0U)
                {
                        break;
                }
                else
                {
                  /*  Added to avoid MISRA violation */
                }
                 offset++;
        }
        return (len);
}

#define CDD_IPC_STOP_SEC_CODE
#include "Cdd_Ipc_MemMap.h"

/* ========================================================================== */
/*                          Local Functions                                   */
/* ========================================================================== */
