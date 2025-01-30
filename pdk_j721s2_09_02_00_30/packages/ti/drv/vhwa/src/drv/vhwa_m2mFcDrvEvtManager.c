/**
 *   Copyright (c) Texas Instruments Incorporated 2021
 *   All rights reserved.
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
 *  \file vhwa_m2mFcEventManager.c
 *
 *  \brief
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/drv/vhwa/src/drv/vhwa_m2mFcDrvPriv.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
static void Vhwa_m2mFcDoneIsr(Vhwa_M2mFcDrvInstObj *instObj);

int32_t Vhwa_m2mVissFrameComplCb(Fvid2_Handle handle, void *appData)
{
    Vhwa_M2mFcDrvInstObj   *instObj = (Vhwa_M2mFcDrvInstObj *)appData;

    if(NULL != instObj->actQObj)
    {
        if(VHWA_FC_NODE_VISS == (uint32_t)instObj->actQObj->hObj->lastNode)
        {
            Vhwa_m2mFcDoneIsr(instObj);
        }
    }

    return FVID2_SOK;
}

int32_t Vhwa_m2mMsc1FrameComplCb(Fvid2_Handle handle, void *appData)
{
    Vhwa_M2mFcDrvInstObj   *instObj = (Vhwa_M2mFcDrvInstObj *)appData;

    if(NULL != instObj->actQObj)
    {
        if(VHWA_FC_NODE_MSC1 == (uint32_t)instObj->actQObj->hObj->lastNode)
        {
            Vhwa_m2mFcDoneIsr(instObj);
        }
    }

    return FVID2_SOK;
}

int32_t Vhwa_m2mMsc0FrameComplCb(Fvid2_Handle handle, void *appData)
{
    Vhwa_M2mFcDrvInstObj   *instObj = (Vhwa_M2mFcDrvInstObj *)appData;

    if(NULL != instObj->actQObj)
    {
        if(VHWA_FC_NODE_MSC0 == (uint32_t)instObj->actQObj->hObj->lastNode)
        {
            Vhwa_m2mFcDoneIsr(instObj);
        }
    }

    return FVID2_SOK;
}

static void Vhwa_m2mFcDoneIsr(Vhwa_M2mFcDrvInstObj *instObj)
{
    Vhwa_M2mFcHandleObj *hObj = NULL;
    Vhwa_M2mFcQueueObj *qObj = NULL;

    qObj = instObj->actQObj;
    hObj = qObj->hObj;

    if (NULL != hObj->createPrms.getTimeStamp)
    {
       hObj->perfNum = hObj->createPrms.getTimeStamp() - hObj->perfNum;
    }

    /* Move completed qObject to done queue */
    Fvid2Utils_queue(hObj->doneQ, &qObj->qElem, qObj);

    instObj->lastHndlObj = hObj;
    instObj->actQObj = NULL;

    /* New request can now be submitted to the NF IP */
    (void)SemaphoreP_post(instObj->pLock);

    if(NULL != hObj->cbPrms.fdmCbFxn)
    {
        (void)hObj->cbPrms.fdmCbFxn(hObj->cbPrms.fdmData);
    }
}
