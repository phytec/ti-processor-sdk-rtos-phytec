/**
 *   Copyright (c) Texas Instruments Incorporated 2019
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
 *  \file vhwa_m2mVissApi.c
 *
 *  \brief API Implementation
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/drv/vhwa/src/drv/vhwa_m2mVissPriv.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * \brief   Local Function to Allocate Handle Object from the
 *          pool of handle objects.
 *          No protection inside the function, Caller should protect
 *          the function call
 *
 * \param   instObj             Instance object.
 *
 * \return  pointer to handle object on success else NULL.
 *
 **/
static Vhwa_M2mVissHandleObj *vhwaM2mVissAllocHandleObj(
    const Vhwa_M2mVissInstObj *instObj);

/**
 * \brief   Local function to freeup allocated Handle Object and return it
 *          to the pool of free handle objects.
 *          No protection inside the function, Caller should protect
 *          the function call
 *
 * \param   hObj                Handle Object to be freed up.
 *
 **/
static void vhwaM2mVissFreeHandleObj(Vhwa_M2mVissHandleObj *hObj);

/**
 * \brief   Local function to check VISS Create params like Driver Id and Instance Id.
 *
 * \param   drvId               Driver ID
 * \param   drvInstId           Instance ID
 *
 *  \return FVID2_SOK if successful, else suitable error code
 **/
static int32_t vhwaM2mVissCheckCreatePrms(uint32_t drvId, uint32_t drvInstId);

/**
 * \brief   FVID2 Process Request function.
 *
 * \param   handle              FVID2 driver handle.
 *
 * \return  FVID2_SOK on success, else FVID2 error code
 *
 **/
Int32 vhwa_m2mVissProcessReq(Fdrv_Handle handle, Fvid2_FrameList *inFrmList,
    Fvid2_FrameList *outFrmList, uint32_t timeout);


/**
 * \brief   FVID2 Get Process Request function.
 *
 * \param   handle              FVID2 driver handle.
 *
 * \return  FVID2_SOK on success, else FVID2 error code
 *
 **/
Int32 vhwa_m2mVissGetProcessReq(Fdrv_Handle handle,
    Fvid2_FrameList *inProcessList, Fvid2_FrameList *outProcessList,
    UInt32 timeout);

int32_t vhwaVissCheckFrameList(Vhwa_M2mVissHandleObj *hObj,
    const Fvid2_FrameList *inFrmList, const Fvid2_FrameList *outFrmList);
static int32_t vhwaM2mVissSetParams(Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissHandleObj *hObj, Vhwa_M2mVissParams *vsPrms);
static int32_t vhwaM2mVissSubmitRequest(Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissQueueObj *qObj);
static int32_t vhwaM2mVissSetConfigInHW(Vhwa_M2mVissInstObj *instObj,
    const Vhwa_M2mVissQueueObj *qObj);
static void vhwaM2mVissCalcBlankParams(Vhwa_M2mVissHandleObj *hObj,
    const Vhwa_M2mVissParams *vsPrms);
static int32_t vhwaM2mVissCheckParams(Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissHandleObj *hObj, Vhwa_M2mVissParams *vsPrms);
static int32_t vhwaM2mVissCreateQueues(Vhwa_M2mVissInstObj *instObj);
static void vhwaM2mVissDeleteQueues(Vhwa_M2mVissInstObj *instObj);
static int32_t vhwaM2mVissAllocSl2(Vhwa_M2mVissInstObj *instObj,
    const Vhwa_M2mVissSl2Params *sl2AllocPrms);

static void Vhwa_m2mVissUpdateFcConnPrms(Vhwa_M2mVissHandleObj *hObj,
                                    Vhwa_M2mVissFcConPrms *sl2FcPrms);
static void Vhwa_m2mVissUpdateFcPrms(Vhwa_M2mVissHandleObj *hObj,
                                    Vhwa_M2mVissFcUpdatePrms *sl2FcPrms);

void Vhwa_m2mVissGetSl2Prms(Vhwa_M2mVissHandleObj *hObj,
                            Vhwa_M2mVissSl2Prms *sl2Prms);

/* Implementation of FVID2 APIs */

/**
 * \brief   FVID2 Create Function.
 *
 * \param   instObj             Instance object.
 *
 * \return  FVID2 Driver Handle.
 *
 **/
Fdrv_Handle vhwa_m2mVissCreate(UInt32 drvId, UInt32 drvInstId,
    Ptr createArgs, Ptr createStatusArgs, const Fvid2_DrvCbParams *cbPrms);

/**
 * \brief   FVID2 Delete Function.
 *
 * \param   handle              FVID2 driver handle.
 *
 * \return  FVID2_SOK on success, else FVID2 error code
 *
 **/
Int32 vhwa_m2mVissDelete(Fdrv_Handle handle, Ptr deleteArgs);

/**
 * \brief   FVID2 Control Function.
 *
 * \param   handle              FVID2 driver handle.
 *
 * \return  FVID2_SOK on success, else FVID2 error code
 *
 **/
Int32 vhwa_m2mVissControl(Fdrv_Handle handle, UInt32 cmd, Ptr cmdArgs,
    Ptr cmdStatusArgs);

/**
 * \brief   FVID2 Process Request function.
 *
 * \param   handle              FVID2 driver handle.
 *
 * \return  FVID2_SOK on success, else FVID2 error code
 *
 **/
Int32 vhwa_m2mVissProcessReq(Fdrv_Handle handle, Fvid2_FrameList *inFrmList,
    Fvid2_FrameList *outFrmList, uint32_t timeout);

/**
 * \brief   FVID2 Get Process Request function.
 *
 * \param   handle              FVID2 driver handle.
 *
 * \return  FVID2_SOK on success, else FVID2 error code
 *
 **/
Int32 vhwa_m2mVissGetProcessReq(Fdrv_Handle handle,
    Fvid2_FrameList *inProcessList, Fvid2_FrameList *outProcessList,
    UInt32 timeout);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

Vhwa_M2mVissHandleObj gM2mVissHandleObj[VHWA_M2M_VISS_MAX_HANDLES];
Vhwa_M2mVissInstObj   gM2mVissInstObj[VHWA_M2M_VISS_DRV_MAX_INST];

Fvid2_DrvOps gM2mVissFvid2DrvOps = {
    FVID2_VHWA_M2M_VISS_DRV_ID,
    /**< Unique driver Id. */
    vhwa_m2mVissCreate,
    /**< FVID2 create function pointer. */
    vhwa_m2mVissDelete,
    /**< FVID2 delete function pointer. */
    vhwa_m2mVissControl,
    /**< FVID2 control function pointer. */
    NULL, NULL,
    /**< FVID2 queue function pointer. */
    vhwa_m2mVissProcessReq,
    /**< FVID2 process request function pointer. */
    vhwa_m2mVissGetProcessReq,
    /**< FVID2 get processed request function pointer. */
};


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int32_t Vhwa_m2mVissInit(Vhwa_M2mVissInitParams *initPrms)
{
    int32_t              status;
    uint32_t             cnt;
    SemaphoreP_Params    params;
    Vhwa_M2mVissInstObj *instObj = NULL;

    if (NULL != initPrms)
    {
        instObj = &gM2mVissInstObj[0U];

        /* Reset all instance object to 0x0 */
        Fvid2Utils_memset(instObj, 0U, sizeof (Vhwa_M2mVissInstObj));

        /* Mark pool flags as free */
        for (cnt = 0U; cnt < VHWA_M2M_VISS_MAX_HANDLES; cnt++)
        {
            gM2mVissHandleObj[cnt].isUsed = (uint32_t) UFALSE;
        }

        /* Set HTS pipeline */
        instObj->pipeline = VHWA_M2M_VISS_HTS_PIPELINE;

        Viss_getSocInfo(&instObj->socInfo);

        status = Fvid2_registerDriver(&gM2mVissFvid2DrvOps);
        if (FVID2_SOK == status)
        {
            instObj->isRegistered = (uint32_t)UTRUE;
        }
        else
        {
            GT_0trace(VhwaVissTrace, GT_ERR,
                "FVID2 driver registration failed!!\n");
            instObj->isRegistered = (uint32_t)UFALSE;
        }

        /* Create Semaphores */
        if (FVID2_SOK == status)
        {
            /* Allocate lock semaphore */
            SemaphoreP_Params_init(&params);
            params.mode = SemaphoreP_Mode_BINARY;
            instObj->lock = SemaphoreP_create(1U, &params);
            if (NULL == instObj->lock)
            {
                GT_0trace(VhwaVissTrace, GT_ERR,
                    "Failed to allocate instance semaphore!!\n");
                status = FVID2_EALLOC;
            }
        }

        if (FVID2_SOK == status)
        {
            /* Allocate lock semaphore */
            SemaphoreP_Params_init(&params);
            params.mode = SemaphoreP_Mode_BINARY;
            instObj->hwaLock = SemaphoreP_create(1U, &params);
            if (NULL == instObj->hwaLock)
            {
                GT_0trace(VhwaVissTrace, GT_ERR,
                    "Failed to allocate HWA Lock semaphore!!\n");
                status = FVID2_EALLOC;
            }
        }

        /* Create free and request queues */
        if (FVID2_SOK == status)
        {
            status = vhwaM2mVissCreateQueues(instObj);
        }

        if (FVID2_SOK == status)
        {
            /* Initialize UDMA, ie allocate and initialize channels
               required for VISS output */
            status = Vhwa_m2mVissUdmaInit(instObj, initPrms);
            if (FVID2_SOK != status)
            {
                GT_0trace(VhwaVissTrace, GT_ERR,
                    "UDMA Initialization Failed !!\n");
            }
        }

        /* Register ISR handler for the given irq number */
        if (FVID2_SOK == status)
        {
            instObj->irqNum = initPrms->irqInfo.irqNum;
            instObj->vhwaIrqNum = initPrms->irqInfo.vhwaIrqNum;

            status = Vhwa_m2mVissRegisterIsr(instObj);
        }

        /* Init is done, copy init params locally,
           enable VPAC and VISS in VPAC Top */
        if (FVID2_SOK == status)
        {
            Fvid2Utils_memcpy(&instObj->initPrms, initPrms,
                sizeof(Vhwa_M2mVissInitParams));

            /* Enable VISS at VPAC Top*/
            CSL_vpacEnableModule(instObj->socInfo.vpacCntlRegs,
                VPAC_MODULE_VISS0, (uint32_t)UTRUE);

            /* Initialize Addresses of difference submodules */
            Vhwa_m2mVissInitAddresses(instObj);

            instObj->initDone = (uint32_t)UTRUE;
        }

        /* Set up config UDMA to store register settings */
        if (FVID2_SOK == status)
        {
            bool saveConfig = true;

            instObj->initPrms.configThroughUdmaFlag =
                    initPrms->configThroughUdmaFlag;

            if (true == instObj->initPrms.configThroughUdmaFlag)
            {
                /* Enable UDMA config channel */
                status = Vhwa_m2mVissStartConfigCh(instObj);
                if (FVID2_SOK != status)
                {
                    GT_0trace(VhwaVissTrace, GT_ERR,
                            "UDMA enabling config channel failed !!\n");
                }

                if (FVID2_SOK == status)
                {
                    /* Allocate UDMA config channel trmem */
                    status = Vhwa_m2mVissAllocConfigUdmaMem(instObj);
                    if (FVID2_SOK != status)
                    {
                        GT_0trace(VhwaVissTrace, GT_ERR,
                                "UDMA config mem alloc failed !!\n");
                    }
                }

                if (FVID2_SOK == status)
                {
                    /* before reading registers, enable glbce and nsf4 */
                    CSL_vissTopGlbceEnable(instObj->regAddrs.topRegs, UTRUE);
                    CSL_vissTopNsf4Enable(instObj->regAddrs.topRegs, UTRUE);

                    /* Save initial value of registers locally in instance object */
                    /* Initialize the buffer object to read registers */
                    Vhwa_m2mVissInitSaveBuffObject(instObj);

                    /* perform UDMA transfer to read all register data */
                    status = Vhwa_m2mVissSubmitSaveRestoreConfigUDMA(instObj,
                            NULL, saveConfig);
                    if (FVID2_SOK != status)
                    {
                        GT_0trace(VhwaVissTrace, GT_ERR,
                                "UDMA config mem alloc failed !!\n");
                    }

                    /* disable glbce and nsf4 after reading data */
                    CSL_vissTopGlbceEnable(instObj->regAddrs.topRegs, UFALSE);
                    CSL_vissTopNsf4Enable(instObj->regAddrs.topRegs, UFALSE);
                }
            }
        }
    }
    else
    {
        status = FVID2_EBADARGS;
    }

    if (FVID2_SOK != status)
    {
        Vhwa_m2mVissDeInit();
    }

    return (status);
}

void Vhwa_m2mVissDeInit(void)
{
    Vhwa_M2mVissInstObj *instObj = NULL;

    instObj = &gM2mVissInstObj[0U];

    if (instObj->openCnt > 0u)
    {
        GT_0trace(VhwaVissTrace, GT_ERR,
            "Warning: All driver handles are not closed!!\n");
    }

    /* Stop UDMA channels */
    (void)Vhwa_m2mVissStopCh(instObj);

    Vhwa_m2mVissUnregisterIsr(instObj);

    (void)Vhwa_m2mVissUdmaDeInit(instObj);

    if ((uint32_t)UTRUE == instObj->isRegistered)
    {
        (void)Fvid2_unRegisterDriver(&gM2mVissFvid2DrvOps);
    }

    vhwaM2mVissDeleteQueues(instObj);

    if ((uint32_t)UTRUE == instObj->isSl2AllocDone)
    {
        Vhwa_m2mVissFreeSl2();
    }

    /* Delete the lock semaphore */
    if (NULL != instObj->lock)
    {
        (void)SemaphoreP_delete(instObj->lock);
        instObj->lock = NULL;
    }

    /* Delete the lock semaphore */
    if (NULL != instObj->hwaLock)
    {
        (void)SemaphoreP_delete(instObj->hwaLock);
        instObj->hwaLock = NULL;
    }

    /* Init all global variables to zero */
    Fvid2Utils_memset(instObj, 0U, sizeof (gM2mVissInstObj));

    instObj->initDone = (uint32_t)UFALSE;
}

int32_t Vhwa_m2mVissAllocSl2(const Vhwa_M2mVissSl2Params *sl2AllocPrms)
{
    int32_t              status = FVID2_SOK;
    Vhwa_M2mVissInstObj *instObj = NULL;

    instObj = &gM2mVissInstObj[0U];

    /* Cannot even lock, if init is not done */
    if ((uint32_t)UFALSE == instObj->initDone)
    {
        GT_0trace(VhwaVissTrace, GT_ERR,
            "Driver init is not done!!\n");
        status = FVID2_EFAIL;
    }
    else
    {
        if ((uint32_t)UTRUE == instObj->isSl2AllocDone)
        {
            GT_0trace(VhwaVissTrace, GT_ERR,
                "SL2 Memory is already allocated !!\n");
            status = FVID2_EFAIL;
        }

        /* Still need to check if provided sl2AllocPrms is not null */
        if (NULL == sl2AllocPrms)
        {
            GT_0trace(VhwaVissTrace, GT_ERR,
                "SL2 Params is null !!\n");
            status = FVID2_EBADARGS;
        }
    }

    if (FVID2_SOK == status)
    {
        /* Lock instance semaphore */
        (void)SemaphoreP_pend(instObj->lock, SemaphoreP_WAIT_FOREVER);

        status = vhwaM2mVissAllocSl2(instObj, sl2AllocPrms);

        /* Release instance semaphore */
        (void)SemaphoreP_post(instObj->lock);
    }

    return (status);
}

void Vhwa_m2mVissFreeSl2(void)
{
    Vhwa_M2mVissInstObj *instObj = NULL;

    instObj = &gM2mVissInstObj[0U];

    /* Lock instance semaphore */
    (void)SemaphoreP_pend(instObj->lock, SemaphoreP_WAIT_FOREVER);

    Vhwa_FreeSl2(instObj->sl2Addr, VHWA_SL2_INST_VPAC);

    instObj->sl2Addr = (uint64_t)NULL;
    instObj->sl2Size = 0x0U;

    /* Mark Sl2 Allocation flag to UFALSE */
    instObj->isSl2AllocDone = (uint32_t)UFALSE;

    /* Release instance semaphore */
    (void)SemaphoreP_post(instObj->lock);
}

/**
 * \brief   Returns handle object for the requested handle count.
 *
 * \param   cnt              count.
 *
 * \return  reference to the handle object.
 *
 **/
Vhwa_M2mVissHandleObj *Vhwa_m2mVissGetHandleObj(uint32_t cnt)
{
    return &gM2mVissHandleObj[cnt];
}
/* ========================================================================== */
/*                          FVID2 Function implementation                     */
/* ========================================================================== */

Fdrv_Handle vhwa_m2mVissCreate(UInt32 drvId, UInt32 drvInstId,
    Ptr createArgs, Ptr createStatusArgs, const Fvid2_DrvCbParams *cbPrms)
{
    int32_t                 status = FVID2_SOK;
    Vhwa_M2mVissInstObj    *instObj = NULL;
    Vhwa_M2mVissHandleObj  *hObj = NULL;
    Vhwa_M2mVissCreateArgs *vissCreateArgs = NULL;
    Fdrv_Handle             handle = NULL;
    Viss_SocInfo           *socInfo = NULL;

    instObj = &gM2mVissInstObj[0U];
    socInfo = &instObj->socInfo;

    /* Check for errors */
    if ((NULL == createArgs) ||
        (NULL == cbPrms))
    {
        GT_0trace(VhwaVissTrace, GT_ERR, "NULL Pointer !!\n");
        status = FVID2_EBADARGS;
    }
    else
    {
        status = vhwaM2mVissCheckCreatePrms(drvId, drvInstId);
    }

    if (FVID2_SOK == status)
    {
        /* Not allowed if init not called */
        if ((uint32_t)UFALSE == instObj->initDone)
        {
            GT_0trace(VhwaVissTrace, GT_ERR, "Init Not Done\n");
            status  = FVID2_EFAIL;
        }
        vissCreateArgs = (Vhwa_M2mVissCreateArgs *)createArgs;
    }

    if (FVID2_SOK == status)
    {
        /* Take the instance semaphore */
        (void)SemaphoreP_pend(instObj->lock, SemaphoreP_WAIT_FOREVER);

        /* Allocate Handle Object */
        hObj = vhwaM2mVissAllocHandleObj(instObj);

        if (NULL != hObj)
        {
            if (0U == instObj->openCnt)
            {
                /* Allocate SL2 Memory if not already allocated */
                if ((uint32_t)UFALSE == instObj->isSl2AllocDone)
                {
                    /* Initialize SL2 parameters with the defaul values */
                    Vhwa_m2mVissInitSl2Params(&instObj->sl2AllocPrms);

                    /* Allocate SL2 Parameters */
                    status = vhwaM2mVissAllocSl2(instObj,
                        &instObj->sl2AllocPrms);
                }

                /* On the first open, enable interrupts in HTS and
                 * start all UTC channels.
                 * Even if all UTC channels are enabled, only channels enabled
                 * in HTS will be used for the transfer */
                if (FVID2_SOK == status)
                {
                    /* Start UDMA Channels on the first handle Create */
                    status = Vhwa_m2mVissStartCh(instObj);
                }
                if (FVID2_SOK == status)
                {
                    /* Enable Pipeline interrupt in INTD */
                    Vhwa_enableHtsIntr(socInfo->vpacIntdRegs,
                        instObj->vhwaIrqNum, instObj->pipeline);
                }
            }

            /* Incrementing openCnt, so that in case of error,
             * delete function can be called directly. */
            instObj->openCnt ++;

            /* Create doneQ, Done Q is specific to handle so that
             * FVID2_Dequeue returns correct request for that handle. */
            if (FVID2_SOK == status)
            {
                status = Fvid2Utils_constructQ(&hObj->doneQObj);
                GT_assert(VhwaVissTrace, (FVID2_SOK == status));

                hObj->doneQ = &hObj->doneQObj;
            }

            /*
             * initialize appBuffInitDone and configInHwDone to false,
             * should be set after receiving valid buffer
             */
            if (true == instObj->initPrms.configThroughUdmaFlag)
            {
                hObj->appBuffInitDone = false;
                hObj->configInHwDone = false;
            }

            if (FVID2_SOK == status)
            {
                /* Allocate Descriptor, Ring memories */
                status = Vhwa_m2mVissAllocUdmaMem(instObj, hObj);
            }

            if (FVID2_SOK == status)
            {
                Fvid2Utils_memcpy(&hObj->createArgs, vissCreateArgs,
                    sizeof(Vhwa_M2mVissCreateArgs));

                Fvid2Utils_memcpy(&hObj->cbPrms, cbPrms,
                    sizeof (hObj->cbPrms));
            }
            else
            {
                /* Some error, so free up the handle object */
                vhwaM2mVissFreeHandleObj(hObj);
            }
        }
        else
        {
            status = FVID2_EALLOC;
        }

        /* Posting here, so that delete can be called */
        (void)SemaphoreP_post(instObj->lock);

        if (FVID2_SOK == status)
        {
            handle = (Fdrv_Handle)hObj;
        }
        else
        {
            /* No Need to check return status */
            (void)vhwa_m2mVissDelete((Fdrv_Handle)hObj, NULL);
        }
    }

    return (handle);
}

Int32 vhwa_m2mVissDelete(Fdrv_Handle handle, Ptr deleteArgs)
{
    int32_t                 status = FVID2_SOK;
    Vhwa_M2mVissInstObj     *instObj = NULL;
    Vhwa_M2mVissHandleObj   *hObj = NULL;
    Viss_SocInfo            *socInfo = NULL;

    if (NULL != handle)
    {
        instObj = &gM2mVissInstObj[0U];
        hObj    = (Vhwa_M2mVissHandleObj *)handle;
        socInfo = &instObj->socInfo;

        /* Take the instance semaphore */
        (void)SemaphoreP_pend(instObj->lock, SemaphoreP_WAIT_FOREVER);

        /* Check if all handles are already closed */
        if (0U == instObj->openCnt)
        {
            status = FVID2_EFAIL;
        }

        if ((FVID2_SOK == status) &&
            (0U != hObj->numPendReq))
        {
            /* Clear All pending requests first */
            status = FVID2_EFAIL;
        }

        if (FVID2_SOK == status)
        {
            /* Delete the Done queue */
            if (NULL != hObj->doneQ)
            {
                Fvid2Utils_destructQ(hObj->doneQ);
                hObj->doneQ = NULL;
            }

            vhwaM2mVissFreeHandleObj(hObj);

            instObj->openCnt --;

            /* for the last close, stop all channels and
             * disable HTS interrupt in INTD */
            if (0U == instObj->openCnt)
            {
                /* Disable HTS Interrupt */
                Vhwa_disableHtsIntr(socInfo->vpacIntdRegs,
                            instObj->vhwaIrqNum, instObj->pipeline);

                /* No active objects and last handle object */
                instObj->actQObj = NULL;
                instObj->lastHndlObj = NULL;
            }
        }

        (void)SemaphoreP_post(instObj->lock);
    }

    return (status);
}

Int32 vhwa_m2mVissControl(Fdrv_Handle handle, UInt32 cmd, Ptr cmdArgs,
    Ptr cmdStatusArgs)
{
    int32_t                  status = FVID2_SOK;
    Vhwa_M2mVissInstObj     *instObj = NULL;
    Vhwa_M2mVissHandleObj   *hObj = NULL;
    Vhwa_M2mVissParams      *vsPrms = NULL;
    Vhwa_HtsLimiter         *htsLimit = NULL;

    if (NULL == handle)
    {
        status = FVID2_EBADARGS;
    }
    else
    {
        instObj = &gM2mVissInstObj[0U];
        hObj = (Vhwa_M2mVissHandleObj *)handle;
    }
    if (FVID2_SOK == status)
    {
        /* Take the instance semaphore */
        (void)SemaphoreP_pend(instObj->lock, SemaphoreP_WAIT_FOREVER);

        switch (cmd)
        {
            /* Set VISS Parameters */
            case IOCTL_VHWA_M2M_VISS_SET_PARAMS:
            {
                if (NULL != cmdArgs)
                {
                    vsPrms = (Vhwa_M2mVissParams *)cmdArgs;
                    status = vhwaM2mVissSetParams(instObj, hObj, vsPrms);
                }
                break;
            }

            /* SET HTS Limiter config */
            case IOCTL_VHWA_M2M_VISS_SET_HTS_LIMIT:
            {
                if (NULL != cmdArgs)
                {
                    htsLimit = (Vhwa_HtsLimiter *)cmdArgs;

                    /* Setting directly in the HTS configuration, so that
                     * it will be configured from the next request. */
                    hObj->htsCfg.enableBwLimit   = htsLimit->enableBwLimit;
                    hObj->htsCfg.cycleCnt        = htsLimit->cycleCnt;
                    hObj->htsCfg.tokenCnt        = htsLimit->tokenCnt;
                }
                break;
            }

            /* Get buffer details to write configuration through UDMA */
            case IOCTL_VHWA_M2M_VISS_GET_BUFF_INFO:
            {
                Vhwa_M2mVissConfigAppBuff *appBuff = NULL;
                if (NULL != cmdArgs)
                {
                    appBuff = (Vhwa_M2mVissConfigAppBuff*) cmdArgs;
                    /* calculate the buffer size needed */
                    hObj->appBufInfo.length = vhwa_m2mCalAppBufSize();
                    appBuff->length = hObj->appBufInfo.length;
                    appBuff->isCacheWriteBackPolicy = true;
                    appBuff->configThroughUdmaFlag =
                            instObj->initPrms.configThroughUdmaFlag;
                }
                else
                {
                    status = FVID2_EBADARGS;
                }
                break;
            }

            /* Set buffer details to write configuration through UDMA */
            case IOCTL_VHWA_M2M_VISS_SET_BUFF_INFO:
            {
                Vhwa_M2mVissConfigAppBuff *appBuff = NULL;
                if ((NULL != cmdArgs)
                    && (instObj->initPrms.configThroughUdmaFlag == true))
                {
                    appBuff = (Vhwa_M2mVissConfigAppBuff*) cmdArgs;
                    /* copy the buffer pointer to the handle object */
                    hObj->appBufInfo.bufferPtr = appBuff->bufferPtr;
                    hObj->appBufInfo.isCacheWriteBackPolicy = appBuff->isCacheWriteBackPolicy;
                    /* validate the application buffer */
                    if ((hObj->appBufInfo.length == appBuff->length)
                        && (NULL != hObj->appBufInfo.bufferPtr))
                    {
                        /* process the received buffer */
                        status=Vhwa_m2mVissProcessAppBuf(instObj, hObj);
                    }
                    else
                    {
                        status = FVID2_EBADARGS;
                    }
                }
                else
                {
                    status = FVID2_EBADARGS;
                }
                break;
            }

            /* Ioctl to set RAW FE Configuration */
            case IOCTL_RFE_SET_CONFIG:
            {
                Rfe_Control *rfeCtrl = NULL;

                if (NULL != cmdArgs)
                {
                    rfeCtrl = (Rfe_Control *)cmdArgs;
                    status = Vhwa_m2mVissSetRfeConfig(instObj, hObj, rfeCtrl);
                }
                else
                {
                    status = FVID2_EBADARGS;
                }
                break;
            }

            /* Ioctl to Set FlexCC/FlexCFA configuration */
            case IOCTL_FCP_SET_CONFIG:
            {
                Fcp_Control *fcpCtrl = NULL;

                if (NULL != cmdArgs)
                {
                    fcpCtrl = (Fcp_Control *)cmdArgs;
                    status = Vhwa_m2mVissSetFcpConfig(instObj, hObj, fcpCtrl);
                }
                else
                {
                    status = FVID2_EBADARGS;
                }
                break;
            }

        #if defined VHWA_VPAC_IP_REV_VPAC3
            /* Ioctl to Set FlexCC/FlexCFA configuration */
            case IOCTL_FCP2_SET_CONFIG:
            {
                Fcp_Control *fcpCtrl = NULL;

                if (NULL != cmdArgs)
                {
                    fcpCtrl = (Fcp_Control *)cmdArgs;
                    status = Vhwa_m2mVissSetFcp2Config(instObj, hObj, fcpCtrl);
                }
                else
                {
                    status = FVID2_EBADARGS;
                }
                break;
            }
        #endif

            case IOCTL_FCP_GET_HISTOGRAM:
            {
                Fcp_HistData *histData = NULL;

                if (NULL != cmdArgs)
                {
                    histData = (Fcp_HistData *)cmdArgs;
                    status = Vhwa_m2mVissReadHistogram(instObj, hObj, histData);
                }
                else
                {
                    status = FVID2_EBADARGS;
                }
                break;
            }

            /* Ioctl to set GLBCE Configuration */
            case IOCTL_GLBCE_SET_CONFIG:
            {
                Glbce_Control *glbceCtrl = NULL;

                if (NULL != cmdArgs)
                {
                    if ((uint32_t)UTRUE == hObj->vsPrms.enableGlbce)
                    {
                        glbceCtrl = (Glbce_Control *)cmdArgs;
                        status = Vhwa_m2mVissSetGlbceConfig(instObj,
                            hObj, glbceCtrl);
                    }
                    else
                    {
                        status = FVID2_EINVALID_PARAMS;
                        GT_0trace(VhwaVissTrace, GT_ERR,
                            "GLBCE is disabled in SET_PARAMS!!\n");
                    }
                }
                else
                {
                    status = FVID2_EBADARGS;
                }
                break;
            }

            /* Ioctl to get GLBCE Configuration */
            case IOCTL_GLBCE_GET_CONFIG:
            {
                Glbce_Control *glbceCtrl = NULL;

                if (NULL != cmdArgs)
                {
                    glbceCtrl = (Glbce_Control *)cmdArgs;
                    status = Vhwa_m2mVissGetGlbceConfig(instObj,
                        hObj, glbceCtrl);
                }
                else
                {
                    status = FVID2_EBADARGS;
                }
                break;
            }

            /* Ioctl to set GLBCE Configuration */
            case IOCTL_NSF4_SET_CONFIG:
            {
                Nsf4v_Config *nsf4Cfg = NULL;

                if (NULL != cmdArgs)
                {
                    if ((uint32_t)UTRUE == hObj->vsPrms.enableNsf4)
                    {
                        nsf4Cfg = (Nsf4v_Config *)cmdArgs;
                        status = Vhwa_m2mVissSetNsf4Config(instObj,
                            hObj, nsf4Cfg);
                    }
                    else
                    {
                        status = FVID2_EINVALID_PARAMS;
                        GT_0trace(VhwaVissTrace, GT_ERR,
                            "NSF4 is disabled in SET_PARAMS!!\n");
                    }
                }
                else
                {
                    status = FVID2_EBADARGS;
                }
                break;
            }

        #if defined VHWA_VPAC_IP_REV_VPAC3
            /* Ioctl to get NSF4 histogram */
            case IOCTL_NSF4_GET_HISTOGRAM:
            {
                Nsf4_Histogram *nsf4Hist = NULL;

                if (NULL != cmdArgs)
                {
                    if ((uint32_t)UTRUE == hObj->vsPrms.enableNsf4)
                    {
                        nsf4Hist = (Nsf4_Histogram *)cmdArgs;
                        status = Vhwa_m2mVissGetNsf4Histogram(instObj,
                                            hObj, nsf4Hist);
                    }
                    else
                    {
                        status = FVID2_EINVALID_PARAMS;
                        GT_0trace(VhwaVissTrace, GT_ERR,
                            "NSF4 is disabled in SET_PARAMS!!\n");
                    }
                }
                else
                {
                    status = FVID2_EBADARGS;
                }
                break;
            }
        #endif

            /* Ioctl to set H3A Configuration */
            case IOCTL_H3A_SET_CONFIG:
            {
                H3a_Config *h3aCfg = NULL;

                if (NULL != cmdArgs)
                {
                    /* No need to check if h3a is enabled in SET_PARAMS here,
                     * as it is checked in Vhwa_m2mVissSetH3aParams */
                    h3aCfg = (H3a_Config *)cmdArgs;
                    status = Vhwa_m2mVissSetH3aParams(instObj, hObj, h3aCfg);
                }
                else
                {
                    status = FVID2_EBADARGS;
                }
                break;
            }

        #if defined VHWA_VPAC_IP_REV_VPAC3
            /* Ioctl to set CAC Configuration */
            case IOCTL_CAC_SET_CONFIG:
            {
                Cac_Config *cacCfg = NULL;

                if (NULL != cmdArgs)
                {
                    if ((uint32_t)UTRUE == hObj->vsPrms.enableCac)
                    {
                        cacCfg = (Cac_Config *)cmdArgs;
                        status = Vhwa_m2mVissSetCacConfig(instObj,
                            hObj, cacCfg);
                    }
                    else
                    {
                        status = FVID2_EINVALID_PARAMS;
                        GT_0trace(VhwaVissTrace, GT_ERR,
                            "GLBCE is disabled in SET_PARAMS!!\n");
                    }
                }
                else
                {
                    status = FVID2_EBADARGS;
                }
                break;
            }
        #endif /* #if defined VHWA_VPAC_IP_REV_VPAC3 */

            case IOCTL_VHWA_M2M_VISS_GET_PERFORMANCE:
            {
                uint32_t *perf = NULL;
                if (NULL != cmdArgs)
                {
                    perf = (uint32_t *)cmdArgs;
                    *perf = (uint32_t)hObj->perf;
                }
                break;
            }

            case IOCTL_VHWA_M2M_VISS_SYNC_START:
            {
                if (NULL != hObj->createArgs.getTimeStamp)
                {
                    hObj->perf = hObj->createArgs.getTimeStamp();
                }
                status = CSL_htsPipelineStart(
                                    instObj->regAddrs.htsRegs, &hObj->htsCfg);
                break;
            }

            case IOCTL_VHWA_VISS_FC_CONN_PARAMS:
            {
                Vhwa_M2mVissFcConPrms *pFcSl2Prms = NULL;
                if (NULL != cmdArgs)
                {
                    pFcSl2Prms = (Vhwa_M2mVissFcConPrms *)cmdArgs;
                    Vhwa_m2mVissUpdateFcConnPrms(hObj, pFcSl2Prms);
                }
                  break;
            }

            case IOCTL_VHWA_VISS_FC_UPDATE_PARAMS:
            {
                Vhwa_M2mVissFcUpdatePrms *pFcPrms = NULL;
                if (NULL != cmdArgs)
                {
                    pFcPrms = (Vhwa_M2mVissFcUpdatePrms *)cmdArgs;
                    Vhwa_m2mVissUpdateFcPrms(hObj, pFcPrms);
                }
                  break;
            }

            case IOCTL_VHWA_VISS_GET_SL2_PARAMS:
            {
                Vhwa_M2mVissSl2Prms *pSl2Prms = NULL;
                if (NULL != cmdArgs)
                {
                    pSl2Prms = (Vhwa_M2mVissSl2Prms *)cmdArgs;
                    Vhwa_m2mVissGetSl2Prms(hObj, pSl2Prms);

                }
                  break;
            }

            /* Default Case */
            default:
            {
                status = FVID2_EUNSUPPORTED_CMD;
                break;
            }
        }

        (void)SemaphoreP_post(instObj->lock);
    }

    return (status);
}

Int32 vhwa_m2mVissProcessReq(Fdrv_Handle handle, Fvid2_FrameList *inFrmList,
    Fvid2_FrameList *outFrmList, uint32_t timeout)
{
    int32_t                status = FVID2_SOK;
    uint32_t               semTimeout;
    Vhwa_M2mVissInstObj   *instObj = NULL;
    Vhwa_M2mVissHandleObj *hObj = NULL;
    Vhwa_M2mVissQueueObj  *qObj = NULL;
    SemaphoreP_Status      semStatus;

    if ((NULL == handle) || (NULL == inFrmList) || (NULL == outFrmList))
    {
        status = FVID2_EBADARGS;
    }
    else
    {
        instObj = &gM2mVissInstObj[0U];
        hObj = (Vhwa_M2mVissHandleObj *)handle;
    }

    /* Check for null pointers */
    if (FVID2_SOK == status)
    {
        status = vhwaVissCheckFrameList(hObj, inFrmList, outFrmList);
    }

    if (FVID2_SOK == status)
    {
        /** Steps
         *  1, lock the HW semaphore
         *  1, Get a queue object from the free queue.
         *  2, Copy frame list into queue object and set
         *     buffer addresses in the TR.
         *  3, Check to see if the curActive Handle is null or not
         *  4, If it is not null, move the queue object to reqQ
         *  5, If it is null, ie HW is free, check to see
         *     if prevHandle is same as this handle.
         *  6, If they are same ,
         *     submit the TR without any configuration
         *  7, If they are not same,
         *     set the config and then submit the TR.
         *
         */
        if (FVID2_TIMEOUT_FOREVER == timeout)
        {
            semTimeout = SemaphoreP_WAIT_FOREVER;
        }
        else if (FVID2_TIMEOUT_NONE == timeout)
        {
            semTimeout = SemaphoreP_NO_WAIT;
        }
        else
        {
            semTimeout = timeout;
        }

        /* This request is going to be submitted, so increment
         * number of pending request counter */
        hObj->numPendReq ++;

        /* Take the instance semaphore, so that no other
         * handle can submit request from the task context. */
        semStatus = SemaphoreP_pend(instObj->hwaLock, semTimeout);

        if (SemaphoreP_OK == semStatus)
        {
            status = FVID2_SOK;
        }
        else /* Any other status */
        {
            /* Failed, so reducing number of pending request */
            hObj->numPendReq --;

            /* Convert Semaphore status to FVID2 status */
            if (SemaphoreP_FAILURE == semStatus)
            {
                status = FVID2_EFAIL;
            }
            else if (SemaphoreP_TIMEOUT == semStatus)
            {
                status = FVID2_EAGAIN;
            }
            else /* For all other cases, it is failed */
            {
                status = FVID2_EFAIL;
            }
        }

        if (FVID2_SOK == status)
        {
            /* Get a queue object from the free queue,
             * No need to protect from ISR as it is not accessed from ISR */
            qObj = (Vhwa_M2mVissQueueObj *) Fvid2Utils_dequeue(instObj->freeQ);

            if (NULL == qObj)
            {
                GT_0trace(VhwaVissTrace, GT_ERR,
                    "Failed to Free Queue Object!!\n");
                status = FVID2_EALLOC;
            }
            else
            {
                qObj->hObj = hObj;
                /* Copy the application's process list to request
                 * objects lists */
                Fvid2_copyFrameList(&qObj->inFrmList, inFrmList);
                Fvid2_copyFrameList(&qObj->outFrmList, outFrmList);
            }
        }

        if (FVID2_SOK == status)
        {
            /* HW is free, submit request to the hardware */
            /* If previous handle and current handles are same, */
            if (instObj->lastHndlObj == hObj)
            {
                if (true == instObj->initPrms.configThroughUdmaFlag)
                {
                    /* Update only dirty buffer object */
                    hObj->sameAsPrevHandle = true;
                }
                /** Set the addresses, Submit the TR
                 *  Start the pipeline */
                status = vhwaM2mVissSubmitRequest(instObj, qObj);
            }
            else
            {
                /** Last handle was not same as new handle,
                 *  so require to recofigure all HW IPs */
                status = vhwaM2mVissSetConfigInHW(instObj, qObj);

                if (FVID2_SOK == status)
                {
                    if (true == instObj->initPrms.configThroughUdmaFlag)
                    {
                        /* Update only dirty buffer object */
                        hObj->sameAsPrevHandle = false;
                    }
                    /** Last handle was not same as new handle,
                     *  so require to recofigure all HW IPs */
                    status = vhwaM2mVissSubmitRequest(instObj, qObj);
                }
            }
        }
    }

    return (status);
}

/** \brief Typedef for FVID2 get processed frames function pointer. */
Int32 vhwa_m2mVissGetProcessReq(Fdrv_Handle handle,
    Fvid2_FrameList *inFrmList, Fvid2_FrameList *outFrmList,
    UInt32 timeout)
{
    int32_t                status = FVID2_SOK;
    uint32_t               cookie;
    Vhwa_M2mVissInstObj    *instObj = NULL;
    Vhwa_M2mVissHandleObj  *hObj = NULL;
    Vhwa_M2mVissQueueObj   *qObj = NULL;

    if (NULL == handle)
    {
        status = FVID2_EBADARGS;
    }
    else
    {
        instObj = &gM2mVissInstObj[0U];
        hObj = (Vhwa_M2mVissHandleObj *)handle;
    }

    if (FVID2_SOK == status)
    {
        /* Take the instance semaphore, so that no other
         * handle can submit request from the task context. */
        (void)SemaphoreP_pend(instObj->lock, SemaphoreP_WAIT_FOREVER);

        /* Disable interrupts before accessing queue */
        cookie = HwiP_disable();

        /* Dequeue the completed request from done queue */
        qObj = (Vhwa_M2mVissQueueObj *) Fvid2Utils_dequeue(hObj->doneQ);

        /* Restore interrupts after updating node list */
        HwiP_restore(cookie);

        /* No buffers in the output queue */
        if (NULL == qObj)
        {
            /* Check if requests are pending with driver */
            if (0U == hObj->numPendReq)
            {
                /* Nothing is queued */
                GT_0trace(VhwaVissTrace, GT_DEBUG,
                    "Nothing to dequeue. No request pending with driver!!\r\n");
                status = FVID2_ENO_MORE_BUFFERS;
            }
            else
            {
                /* If no request have completed, return try again */
                GT_0trace(VhwaVissTrace, GT_DEBUG,
                    "Nothing to dequeue. Try again!!\r\n");
                status = FVID2_EAGAIN;
            }
        }
        else /* OutQ has buffer to be returned */
        {
            /* Copy the driver's process list to application's process list */
            Fvid2_copyFrameList(inFrmList, &qObj->inFrmList);
            Fvid2_copyFrameList(outFrmList, &qObj->outFrmList);

            /* Return back the queue object to the free queue,
             * No need to protect from ISR, as it is not accessed in ISR */
            Fvid2Utils_queue(instObj->freeQ, &qObj->qElem, qObj);

            /* Dequeue from Rings */
            status = Vhwa_m2mVissPopRings(instObj, hObj);

            /* Decrement the pending request count. */
            if (0U != hObj->numPendReq)
            {
                hObj->numPendReq--;
            }
        }

        (void)SemaphoreP_post(instObj->lock);
    }

    return (status);
}

/* ========================================================================== */
/*                           Local Functions                                  */
/* ========================================================================== */

static Vhwa_M2mVissHandleObj *vhwaM2mVissAllocHandleObj(
    const Vhwa_M2mVissInstObj *instObj)
{
    uint32_t cnt;
    Vhwa_M2mVissHandleObj *hObj = NULL;

    if (NULL != instObj)
    {
        for (cnt = 0U; cnt < VHWA_M2M_VISS_MAX_HANDLES; cnt ++)
        {
            if ((uint32_t)UFALSE == gM2mVissHandleObj[cnt].isUsed)
            {
                hObj = &gM2mVissHandleObj[cnt];
                Fvid2Utils_memset(hObj, 0x0, sizeof(Vhwa_M2mVissHandleObj));
                gM2mVissHandleObj[cnt].isUsed = (uint32_t)UTRUE;
                hObj->hIdx = cnt;
                break;
            }
        }
    }
    return (hObj);
}

/* Function to freeup allocated Handle Object */
static void vhwaM2mVissFreeHandleObj(Vhwa_M2mVissHandleObj *hObj)
{
    uint32_t cnt;

    if (NULL != hObj)
    {
        for (cnt = 0U; cnt < VHWA_M2M_VISS_MAX_HANDLES; cnt ++)
        {
            if (hObj == &gM2mVissHandleObj[cnt])
            {
                hObj->isUsed = (uint32_t)UFALSE;;
                break;
            }
        }
    }
}

static int32_t vhwaM2mVissSetParams(Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissHandleObj *hObj, Vhwa_M2mVissParams *vsPrms)
{
    int32_t status = FVID2_EBADARGS;

    /* Check for Null Pointer */
    GT_assert(VhwaVissTrace, (NULL != instObj));
    GT_assert(VhwaVissTrace, (NULL != hObj));
    GT_assert(VhwaVissTrace, (NULL != vsPrms));

    {
        /* Calculate Blanking information */
        vhwaM2mVissCalcBlankParams(hObj, vsPrms);

        status = vhwaM2mVissCheckParams(instObj, hObj, vsPrms);

        if (FVID2_SOK != status)
        {
            /* Revert Blanking information */
            vhwaM2mVissCalcBlankParams(hObj, &hObj->vsPrms);

            /* Revert SL2 Size */
            hObj->totalSl2Size = 0x0U;

            /* Reset VISS Config to old working config */
            (void)Vhwa_m2mVissSetInChParams(instObj, hObj, &hObj->vsPrms);
            /* Reset VISS Config to old working config */
            (void)Vhwa_m2mVissSetOutChParams(instObj, hObj, &hObj->vsPrms);
            /* Reset LSE config based on local correct viss config */
            Vhwa_m2mVissSetLseCfg(instObj, hObj, &hObj->vsPrms);
            /* Reset HTS config based on local correct viss config */
            Vhwa_m2mVissSetHtsCfg(instObj, hObj, &hObj->vsPrms);
        }
        else
        {
            /* Copy Parameters in handle object */
            (void)memcpy(&hObj->vsPrms, vsPrms, sizeof(Vhwa_M2mVissParams));

            /* Setup TR Descriptor */
            Vhwa_m2mVissSetTrDesc(instObj, hObj);

            hObj->isPrmsSet = (uint32_t)UTRUE;

            hObj->fcpCfg.width = vsPrms->inFmt.width;
            hObj->fcpCfg.height = vsPrms->inFmt.height;
            hObj->rfeCfg.width = vsPrms->inFmt.width;
            hObj->rfeCfg.height = vsPrms->inFmt.height;
            hObj->nsf4vCfg.width = vsPrms->inFmt.width;
            hObj->nsf4vCfg.height = vsPrms->inFmt.height;
            hObj->eeCfg.width = vsPrms->inFmt.width;
            hObj->eeCfg.height = vsPrms->inFmt.height;
        #if defined VHWA_VPAC_IP_REV_VPAC3
            hObj->fcp2Cfg.width = vsPrms->inFmt.width;
            hObj->fcp2Cfg.height = vsPrms->inFmt.height;
            hObj->cacCfg.width = vsPrms->inFmt.width;
            hObj->cacCfg.height = vsPrms->inFmt.height;
            hObj->fcp2EeCfg.width = vsPrms->inFmt.width;
            hObj->fcp2EeCfg.height = vsPrms->inFmt.height;
        #endif

            /* Do GLBCE init sequence, so that GLBCE registers are
             * accessible */
            Vhwa_m2mVissGlbceInit(instObj, hObj);

            /* Similarly enable NSF4 at VISS Top, so that
             * NSF4v registers are accessible, for set_config */
            Vhwa_m2mVissNsf4Init(instObj, hObj);

        #if defined VHWA_VPAC_IP_REV_VPAC3
            if ((uint32_t)UTRUE == hObj->vsPrms.enableCac)
            {
                Vhwa_m2mVissCacInit(instObj, hObj);
            }
        #endif

        }

        if ((FVID2_SOK == status)
                && (true == instObj->initPrms.configThroughUdmaFlag))
        {
            /* configuration has been changed */
            hObj->configInHwDone = false;
        }
    }

    return (status);
}

static void vhwaM2mVissCalcBlankParams(Vhwa_M2mVissHandleObj *hObj,
    const Vhwa_M2mVissParams *vsPrms)
{
    uint32_t            latency;
    #if defined VHWA_VPAC_IP_REV_VPAC3
        uint32_t            mvLatency;
    #endif

    /* Check For Null pointers */
    GT_assert(VhwaVissTrace, (NULL != hObj));
    GT_assert(VhwaVissTrace, (NULL != vsPrms));

    latency = 0u;
    if ((NULL != hObj) && (NULL != vsPrms))
    {
        latency = latency + CSL_RAWFE_HORZ_LATENCY; /* For RAW FE */
        latency = latency + CSL_FLEX_CFA_HORZ_LATENCY; /* For CFA */
        latency = latency + CSL_FLEX_CC_HORZ_LATENCY; /* For FCC */

        #if defined VHWA_VPAC_IP_REV_VPAC3
            mvLatency = latency;
        #endif


        if ((uint32_t)UTRUE == vsPrms->enableGlbce)
        {
            latency = latency + CSL_GLBCE_HORZ_LATENCY;
        }
        if ((uint32_t)UTRUE == vsPrms->enableNsf4)
        {
            latency = latency + CSL_NSF4V_HORZ_LATENCY;
        }
    #if defined VHWA_VPAC_IP_REV_VPAC3
        if ((uint32_t)UTRUE == vsPrms->enableCac)
        {
            latency = latency + CSL_CAC_HORZ_LATENCY;
        }
    #endif

        if ((0U != (VHWA_M2M_VISS_EE_ON_LUMA12 & vsPrms->edgeEnhancerMode)) ||
            (0U != (VHWA_M2M_VISS_EE_ON_LUMA8 & vsPrms->edgeEnhancerMode)))
        {
            latency = latency + CSL_FLEX_EE_HORZ_LATENCY;
        }

        /* Horizontal blanking is max of all enabled modules
         * horizontal blanking, FlexCC requires maximum horizontal
         * blanking out of RAWFE, GLBCE, CFA, EE */
        hObj->maxLseHorzBlanking = CSL_FLEX_CC_HORZ_BLANKING;
        /* If NSF4v is enabled, NSF4 requires maximum blanking */
        if (UTRUE == vsPrms->enableNsf4)
        {
            hObj->maxLseHorzBlanking = CSL_NSF4V_HORZ_BLANKING;
        }

    #if defined VHWA_VPAC_IP_REV_VPAC3
        /* If NSF4v is not enabled, CAC requires maximum blanking */
        if (UTRUE == vsPrms->enableCac)
        {
            hObj->maxLseHorzBlanking = CSL_CAC_HORZ_BLANKING;
        }
    #endif

        hObj->maxLseHorzBlanking  = VHWA_VISS_MAX_HORZ_BLANKING;

        /* Calculate the threshold value */
        hObj->thr = Vhwa_ceil(latency,
                (vsPrms->inFmt.width + hObj->maxLseHorzBlanking));

    #if defined VHWA_VPAC_IP_REV_VPAC3
        if(UTRUE == vsPrms->enableMVPipe)
        {
            if (VHWA_VISS_MV_PIPE_INPUT_GLBC == vsPrms->mvPipeInSel)
            {
                if ((uint32_t)UTRUE == vsPrms->enableGlbce)
                {
                    mvLatency = mvLatency + CSL_GLBCE_HORZ_LATENCY;
                }
                if ((uint32_t)UTRUE == vsPrms->enableNsf4)
                {
                    mvLatency = mvLatency + CSL_NSF4V_HORZ_LATENCY;
                }
                if ((uint32_t)UTRUE == vsPrms->enableCac)
                {
                    mvLatency = mvLatency + CSL_CAC_HORZ_LATENCY;
                }
            }
            else if (VHWA_VISS_MV_PIPE_INPUT_NSF4 == vsPrms->mvPipeInSel)
            {
                if ((uint32_t)UTRUE == vsPrms->enableNsf4)
                {
                    mvLatency = mvLatency + CSL_NSF4V_HORZ_LATENCY;
                }
                if ((uint32_t)UTRUE == vsPrms->enableCac)
                {
                    mvLatency = mvLatency + CSL_CAC_HORZ_LATENCY;
                }
            }
            else if (VHWA_VISS_MV_PIPE_INPUT_CAC == vsPrms->mvPipeInSel)
            {
                if ((uint32_t)UTRUE == vsPrms->enableCac)
                {
                    mvLatency = mvLatency + CSL_CAC_HORZ_LATENCY;
                }
            }
            else
            {
              /*Do Nothing*/
            }

            if ((0U != (VHWA_M2M_VISS_EE_ON_MV_LUMA12 & vsPrms->edgeEnhancerMode)) ||
                (0U != (VHWA_M2M_VISS_EE_ON_MV_LUMA8 & vsPrms->edgeEnhancerMode)))
            {
                mvLatency = mvLatency + CSL_FLEX_EE_HORZ_LATENCY;
            }

        }
        hObj->mvPipeDelay = latency - mvLatency;
    #endif /* #if defined VHWA_VPAC_IP_REV_VPAC3 */

/* Vertical blanking is sum of all enabled modules vertical blanking Calculate vertical blanking */
        hObj->maxLseVertBlanking = 0;
    #if defined VHWA_VPAC_IP_REV_VPAC3
        hObj->maxLseVertBlankingMV = 0;
    #endif

    /* HV Vertical Blanking */
        hObj->maxLseVertBlanking += CSL_FLEX_CC_VERT_BLANKING; /* For FCC */
        if (UTRUE == vsPrms->enableDpc)
        {
            hObj->maxLseVertBlanking += CSL_RAWFE_DPC_VERT_BLANKING;
        }
        if (UTRUE == vsPrms->enableGlbce)
        {
            hObj->maxLseVertBlanking += CSL_GLBCE_VERT_BLANKING;
        }
        if (UTRUE == vsPrms->enableNsf4)
        {
            hObj->maxLseVertBlanking += CSL_NSF4V_VERT_BLANKING;
        }
    #if defined VHWA_VPAC_IP_REV_VPAC3
        if (UTRUE == vsPrms->enableCac)
        {
            hObj->maxLseVertBlanking += CSL_CAC_VERT_BLANKING;
        }

    /* MV Vertical Blanking */

        if(UTRUE == vsPrms->enableMVPipe)
        {
            /*Flex CC blanking values are added. */
            hObj->maxLseVertBlankingMV += CSL_FLEX_CC_VERT_BLANKING; /* For FCC */
            
            /*If DPC enabled then blanking values are added. */
            if (UTRUE == vsPrms->enableDpc)
            {
                hObj->maxLseVertBlankingMV += CSL_RAWFE_DPC_VERT_BLANKING;
            }

            /*For individual modules that are enabled, blanking values are calculated. */
            if (VHWA_VISS_MV_PIPE_INPUT_GLBC == vsPrms->mvPipeInSel)
            {
            /*If GLBCE enabled then blanking values are added.  */               
                if ((uint32_t)UTRUE == vsPrms->enableGlbce)
                {
                    hObj->maxLseVertBlankingMV += CSL_GLBCE_VERT_BLANKING;
                }
            /*If NSF4V enabled then blanking values are added. */

                if ((uint32_t)UTRUE == vsPrms->enableNsf4)
                {
                    hObj->maxLseVertBlankingMV += CSL_NSF4V_VERT_BLANKING;
                }
            /*If CAC enabled then blanking values are added. */

                if ((uint32_t)UTRUE == vsPrms->enableCac)
                {
                    hObj->maxLseVertBlankingMV += CSL_CAC_VERT_BLANKING;
                }
            }
            else if (VHWA_VISS_MV_PIPE_INPUT_NSF4 == vsPrms->mvPipeInSel)
            {
            /*If NSF4V enabled then blanking values are added. */

                if ((uint32_t)UTRUE == vsPrms->enableNsf4)
                {
                    hObj->maxLseVertBlankingMV += CSL_NSF4V_VERT_BLANKING;
                }
            /*If CAC enabled then blanking values are added. */

                if ((uint32_t)UTRUE == vsPrms->enableCac)
                {
                    hObj->maxLseVertBlankingMV += CSL_CAC_VERT_BLANKING;
                }
            }
            else if (VHWA_VISS_MV_PIPE_INPUT_CAC == vsPrms->mvPipeInSel)
            {
            /*If CAC enabled then blanking values are added. */
                if ((uint32_t)UTRUE == vsPrms->enableCac)
                {
                    hObj->maxLseVertBlankingMV += CSL_CAC_VERT_BLANKING;
                }
            }
            else
            {
              /*Do Nothing*/
            }
        }
    #endif /* #if defined VHWA_VPAC_IP_REV_VPAC3 */
    }
}

static int32_t vhwaM2mVissCheckParams(Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissHandleObj *hObj, Vhwa_M2mVissParams *vsPrms)
{
    int32_t         status = FVID2_SOK;

    if ((NULL != instObj) && (NULL != hObj) && (NULL != vsPrms))
    {
        hObj->totalSl2Size = 0x0U;

        status = Vhwa_m2mVissSetInChParams(instObj, hObj, vsPrms);

        if (FVID2_SOK == status)
        {
            status = Vhwa_m2mVissSetOutChParams(instObj, hObj, vsPrms);
        }

        if (FVID2_SOK == status)
        {
            /* Initialize LSE configuration based on the new VISS Params */
            Vhwa_m2mVissSetLseCfg(instObj, hObj, vsPrms);
            /* Initialize HTS configuration based on the new VISS Params */
            Vhwa_m2mVissSetHtsCfg(instObj, hObj, vsPrms);
        }

        if (FVID2_SOK == status)
        {
            /* Check New HTS Configuration */
            status = CSL_htsCheckThreadConfig(&hObj->htsCfg);
        }
        if (FVID2_SOK == status)
        {
            /* Check LSE HTS Configuration */
            status = CSL_lseCheckConfig(&hObj->lseCfg);
        }
    }
    else
    {
        status = FVID2_EBADARGS;
    }

    return (status);
}

static int32_t vhwaM2mVissSetConfigInHW(Vhwa_M2mVissInstObj *instObj,
    const Vhwa_M2mVissQueueObj *qObj)
{
    int32_t status = FVID2_SOK;
    Vhwa_M2mVissRegAddr *regAddrs = NULL;
    Vhwa_M2mVissHandleObj *hObj = NULL;
    /* enable buffer object if used */
    bool enableBufObj = false;

    /* Null pointer check */
    GT_assert(VhwaVissTrace, (NULL != instObj));
    GT_assert(VhwaVissTrace, (NULL != qObj));
    GT_assert(VhwaVissTrace, (NULL != qObj->hObj));

    hObj = qObj->hObj;

    if (true == instObj->initPrms.configThroughUdmaFlag)
    {
        if (true == hObj->appBuffInitDone)
        {
            /* we will be writing configuration to buffer object */
            regAddrs = &hObj->buffRegAddrs;
            enableBufObj = true;
        }
        else
        {
            status = FVID2_EFAIL;
        }

    }
    else
    {
        regAddrs = &instObj->regAddrs;
    }

    /*
     * perform config HW if,
     * 1) configTroughUDMA is false. 2) configInHwDone is false.
     */
    if ((false == enableBufObj) || (false == hObj->configInHwDone))
    {
        /* Step-1, Configure HWA Core,
         * This changes the frame size and paths, as required by this handle */
        if (FVID2_SOK == status)
        {
            status = Vhwa_memVissSetVissSizeAndPath(instObj, hObj);
        }

        if (FVID2_SOK == status)
        {
            /* Step-2, Configure HWA Wrapper LSE */
            status = CSL_lseSetConfig(regAddrs->lseRegs, &hObj->lseCfg);
            /* Enable used buffer object */
            if ((FVID2_SOK == status) && (true == enableBufObj))
            {
                hObj->bufferObjHolder[BUFF_ID_LSE_REGS].isModified = true;
            }
        }

        if (FVID2_SOK == status)
        {
            /* Step-3, Configure HWA Common Wrapper HTS */
            status = CSL_htsSetThreadConfig(regAddrs->htsRegs, &hObj->htsCfg);
            /* Enable used buffer object */
            if ((FVID2_SOK == status) && (true == enableBufObj))
            {
                hObj->bufferObjHolder[BUFF_ID_HTS_HWA0_REGS].isModified = true;
                hObj->bufferObjHolder[BUFF_ID_HTS_DMA_0_4_REGS].isModified
                            = true;
                hObj->bufferObjHolder[BUFF_ID_HTS_DMA_240_245_REGS].isModified
                            = true;
            }
        }

        if (FVID2_SOK == status)
        {
            /* Step-5, Start HTS Channels */
            status = CSL_htsThreadStart(regAddrs->htsRegs, &hObj->htsCfg);
            /* Enable used buffer object */
            if ((FVID2_SOK == status) && (true == enableBufObj))
            {
                hObj->bufferObjHolder[BUFF_ID_HTS_HWA0_REGS].isModified = true;
                hObj->bufferObjHolder[BUFF_ID_HTS_DMA_0_4_REGS].isModified
                            = true;
                hObj->bufferObjHolder[BUFF_ID_HTS_DMA_240_245_REGS].isModified
                            = true;
            }
        }

        if (FVID2_SOK == status)
        {
            if (true == enableBufObj)
            {
                /*
                 * if configThroughUDMA is true,
                 * no need to config HW every time
                 */
                hObj->configInHwDone = true;
            }
        }
    }

    return (status);
}

static int32_t vhwaM2mVissSubmitRequest(Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissQueueObj *qObj)
{
    int32_t status = FVID2_SOK;
    Vhwa_M2mVissHandleObj *hObj = NULL;

    /* Null pointer check */
    GT_assert(VhwaVissTrace, (NULL != instObj));
    GT_assert(VhwaVissTrace, (NULL != qObj));
    GT_assert(VhwaVissTrace, (NULL != qObj->hObj));

    hObj = qObj->hObj;

    /* Start the core */
    status = Vhwa_memVissSetVissStartModules(instObj, hObj);

    if (FVID2_SOK == status)
    {
        /*
         * before submitting the request to VISS, write configuration
         * if configThroughUdmaFlag is true
         */
        if (true == instObj->initPrms.configThroughUdmaFlag)
        {
            if (true == hObj->appBuffInitDone)
            {
                /*
                 * Before submitting the VISS input buffers write the
                 * config UDMA buffer to registers through UDMA channel.
                 */
                status = Vhwa_m2mVissSubmitConfUDMABuf(instObj, hObj);
                if (FVID2_SOK != status)
                {
                    status = FVID2_EFAIL;
                }
            }
            else
            {
                status = FVID2_EFAIL;
            }
        }
    }

    if (FVID2_SOK == status)
    {
        /* Configure Buffer Addresses in TR Record */
        Vhwa_m2mVissSetAddress(hObj);

        /* Submit Rings to the Ring Accelerator */
        status = Vhwa_m2mVissSubmitRing(instObj, hObj);
    }

    if (FVID2_SOK == status)
    {
        /* Better to set Active object to this q object, so that if
         * interrupt comes immediately, actQObj would be set..
         * If pipeline start fails, it would be set to NULL.*/
        instObj->actQObj = qObj;

        instObj->totalReqCnt++;

#ifndef VHWA_USE_PIPELINE_COMMON_ENABLE
        if (NULL != hObj->createArgs.getTimeStamp)
        {
            hObj->perf = hObj->createArgs.getTimeStamp();
        }

        /* Before starting pipeline make sure, config UDMA is complete */
        if (true == instObj->initPrms.configThroughUdmaFlag)
        {
            status = Vhwa_m2mVissCheckConfUDMAComp(instObj, hObj);
        }

        /* Start HTS pipeline */
        if ((FVID2_SOK == status) && (UFALSE == hObj->fcStatus.isFlexConnect))
        {
			status = CSL_htsPipelineStart(
					instObj->regAddrs.htsRegs, &hObj->htsCfg);
        }
#endif

        if (FVID2_SOK != status)
        {
            instObj->actQObj = NULL;
            status = FVID2_EFAIL;
        }
        else
        {
            status = FVID2_SOK;
        }
    }

    return (status);
}

int32_t vhwaVissCheckFrameList(Vhwa_M2mVissHandleObj *hObj,
    const Fvid2_FrameList *inFrmList, const Fvid2_FrameList *outFrmList)
{
    int32_t                 status = FVID2_SOK;
    uint32_t                cnt;
    uint32_t                frmIdx;
    uint32_t                frmAddrIdx;
    Vhwa_M2mVissChParams   *chPrms = NULL;

    GT_assert(VhwaVissTrace, (NULL != hObj));
    GT_assert(VhwaVissTrace, (NULL != inFrmList));
    GT_assert(VhwaVissTrace, (NULL != outFrmList));

    /* Cannot continue if parameters are not set */
    if ((uint32_t)UFALSE == hObj->isPrmsSet)
    {
        status = FVID2_EINVALID_PARAMS;
    }

    /* Input Buffer Addresses cannot be null */
    for (cnt = 0U; cnt < VHWA_M2M_VISS_MAX_INPUTS; cnt ++)
    {
        chPrms = &hObj->inChPrms[cnt];
        if ((uint32_t)UTRUE == chPrms->isEnabled)
        {
            /* Both Frame and frame address must not be null */
            if ((NULL == inFrmList->frames[cnt]) ||
                (0U == inFrmList->frames[cnt]->addr[0U]))
            {
                status = FVID2_EBADARGS;
                break;
            }
            else
            {
                /* Storing Buffer address here, so does not have to run
                 * this loop again */
                chPrms->bufAddr = inFrmList->frames[cnt]->addr[0U];
            }
        }
    }

    /* Output Buffer Addresses cannot be null */
    for (cnt = 0U; cnt < VHWA_M2M_VISS_MAX_OUTPUTS; cnt ++)
    {
        chPrms     = &hObj->outChPrms[cnt];
        frmIdx     = chPrms->frmIdx;
        frmAddrIdx = chPrms->frmAddrIdx;

        if ((uint32_t)UTRUE == chPrms->isEnabled)
        {
            /* Both Frame and frame address must not be null */
            if (((NULL == outFrmList->frames[frmIdx]) ||
                 (0U == outFrmList->frames[frmIdx]->addr[frmAddrIdx])) &&
                 (UFALSE == hObj->fcStatus.isFlexConnect))
            {
                status = FVID2_EBADARGS;
                break;
            }
            else
            {
                /* Storing Buffer address here, so does not have to run
                 * this loop again */
                chPrms->bufAddr = outFrmList->frames[frmIdx]->addr[frmAddrIdx];
            }
        }
    }

    return (status);
}


static int32_t vhwaM2mVissCreateQueues(Vhwa_M2mVissInstObj *instObj)
{
    int32_t              status;
    uint32_t             qCnt;
    Vhwa_M2mVissQueueObj *qObj;

    /* NULL pointer check */
    GT_assert(VhwaVissTrace, (NULL != instObj));

    instObj->reqQ  = NULL;
    instObj->freeQ = NULL;

    /* Create Free queue */
    status = Fvid2Utils_constructQ(&instObj->freeQObj);
    GT_assert(VhwaVissTrace, (FVID2_SOK == status));

    instObj->freeQ = &instObj->freeQObj;

    /* Create input queue */
    status = Fvid2Utils_constructQ(&instObj->reqQObj);
    GT_assert(VhwaVissTrace, (FVID2_SOK == status));

    instObj->reqQ = &instObj->reqQObj;

    /* Initialize and queue the allocate queue object to free Q */
    for(qCnt = 0U; qCnt < VHWA_M2M_VISS_UDMA_RING_ENTRIES; qCnt ++)
    {
        qObj = &instObj->vissQObj[qCnt];

        Fvid2Utils_memset(qObj, 0x0, sizeof(Vhwa_M2mVissQueueObj));

        Fvid2Utils_queue(instObj->freeQ, &qObj->qElem, qObj);
    }

    return (status);
}

static void vhwaM2mVissDeleteQueues(Vhwa_M2mVissInstObj *instObj)
{
    Vhwa_M2mVissQueueObj *qObj = NULL;

    /* NULL pointer check */
    GT_assert(VhwaVissTrace, (NULL != instObj));

    if(NULL != instObj->freeQ)
    {
        /* Free-up all the queued free queue objects */
        do
        {
            qObj = (Vhwa_M2mVissQueueObj *) Fvid2Utils_dequeue(instObj->freeQ);
        } while (NULL != qObj);

        /* Delete the free Q */
        Fvid2Utils_destructQ(instObj->freeQ);
        instObj->freeQ = NULL;
    }

    if(NULL != instObj->reqQ)
    {
        /* Free-up all object from input queue */
        do
        {
            qObj = (Vhwa_M2mVissQueueObj *) Fvid2Utils_dequeue(instObj->reqQ);
        } while (NULL != qObj);

        /* Delete the input Q */
        Fvid2Utils_destructQ(instObj->reqQ);
        instObj->reqQ = NULL;
    }
}

static int32_t vhwaM2mVissAllocSl2(Vhwa_M2mVissInstObj *instObj,
    const Vhwa_M2mVissSl2Params *sl2AllocPrms)
{
    int32_t              status = FVID2_SOK;
    uint32_t             cnt;
    uint32_t             sl2MemSize;
    uint64_t             sl2Addr;
    uint32_t             width;

    /* Error Checking */
    for (cnt = 0U; cnt < VHWA_M2M_VISS_MAX_INPUTS; cnt ++)
    {
        /* Check min width for input */
        if ((0U != sl2AllocPrms->maxInWidth[cnt]) &&
            (VHWA_M2M_VISS_MIN_INPUT_DEPTH > sl2AllocPrms->inDepth))
        {
            GT_2trace(VhwaVissTrace, GT_ERR,
                "Incorrect Depth %d for input %d !!\n",
                sl2AllocPrms->inDepth, cnt);
            status = FVID2_EFAIL;
        }
    }
    for (cnt = 0U; cnt < VHWA_M2M_VISS_MAX_OUTPUTS; cnt ++)
    {
        /* Check min width for output */
        if ((0U != sl2AllocPrms->maxOutWidth[cnt]) &&
            (VHWA_M2M_VISS_MIN_OUTPUT_DEPTH > sl2AllocPrms->outDepth[cnt]))
        {
            GT_2trace(VhwaVissTrace, GT_ERR,
                "Incorrect Depth %d for output %d !!\n",
                sl2AllocPrms->outDepth[cnt], cnt);
            status = FVID2_EFAIL;
        }
    }

    if (FVID2_SOK == status)
    {
        sl2MemSize = 0U;
        /* Calculate the total size required */
        for (cnt = 0U; cnt < VHWA_M2M_VISS_MAX_INPUTS; cnt ++)
        {
            if (0U != sl2AllocPrms->maxInWidth[cnt])
            {
                width = Vhwa_calcHorzSizeInBytes(
                    sl2AllocPrms->maxInWidth[cnt],
                    sl2AllocPrms->inCcsf);

                /* Align block width in bytes to SL2 pitch ie 64byte aligned */
                width = Vhwa_calcSl2Pitch(width);

                /* calculate size based on the depth required */
                sl2MemSize = sl2MemSize +
                    (width * sl2AllocPrms->inDepth);
            }
        }
        /* Calculate the total size required for output */
        for (cnt = 0U; cnt < VHWA_M2M_VISS_MAX_OUTPUTS; cnt ++)
        {
            if (0U != sl2AllocPrms->maxOutWidth[cnt])
            {
                width = Vhwa_calcHorzSizeInBytes(
                    sl2AllocPrms->maxOutWidth[cnt],
                    sl2AllocPrms->outCcsf[cnt]);

                /* Align block width in bytes to SL2 pitch ie 64byte aligned */
                width = Vhwa_calcSl2Pitch(width);

                /* calculate size based on the depth required */
                sl2MemSize = sl2MemSize +
                    (width * sl2AllocPrms->outDepth[cnt]);
            }
        }

        /* Allocate SL2 memory for calculated size */
        sl2Addr = Vhwa_allocateSl2(sl2MemSize, VHWA_SL2_INST_VPAC);

        if (0U == sl2Addr)
        {
            GT_1trace(VhwaVissTrace, GT_ERR,
                "Could not allocate SL2 memory for output%d!!\n", cnt);
            status = FVID2_EALLOC;
        }
        else
        {
            instObj->sl2Size = sl2MemSize;
            instObj->sl2Addr = sl2Addr;
        }
    }

    if (FVID2_SOK != status)
    {
        /* Error in memory allocation, open will not be allowed. */
        instObj->isSl2AllocDone = (uint32_t)UFALSE;
    }
    else
    {
        /* Memory is allocated successfully, now open is allowed. */
        instObj->isSl2AllocDone = (uint32_t)UTRUE;
        Fvid2Utils_memcpy(&instObj->sl2AllocPrms, sl2AllocPrms,
            sizeof(Vhwa_M2mVissSl2Params));
    }

    return (status);
}

void Vhwa_m2mVissConfgUdmaEvenCb(Udma_EventHandle eventHandle,
                               uint32_t eventType,
                               void *appData)
{
    Vhwa_M2mVissInstObj *instObj = &gM2mVissInstObj[0U];

    if (UDMA_EVENT_TYPE_DMA_COMPLETION == eventType)
    {
        (void)SemaphoreP_post(instObj->configCqEventSemaphore);
    }
    else
    {
        GT_0trace(VhwaVissTrace, GT_ERR,"Vhwa_m2mVissConfgUdmaEvenCb fail !!\n");
    }

    return;
}

void Vhwa_m2mVissGetSl2Prms(Vhwa_M2mVissHandleObj *hObj,
                            Vhwa_M2mVissSl2Prms *sl2Prms)
{
    uint32_t cnt;
    Vhwa_M2mVissChParams   *chPrms = NULL;
    Vhwa_M2mVissInstObj    *instObj = &gM2mVissInstObj[0U];

    sl2Prms->sl2MemSize = instObj->sl2Size;

    for (cnt=0; cnt < VHWA_M2M_VISS_MAX_IN_DMA_CH; cnt++)
    {
        chPrms = &hObj->inChPrms[cnt];

        sl2Prms->inSl2Addr[cnt] = chPrms->sl2Addr;
        sl2Prms->inWidthInBytes[cnt] = chPrms->sl2Pitch;
        sl2Prms->inSl2BuffDepth[cnt] = chPrms->sl2Depth;

    }

    for (cnt=0; cnt < VHWA_M2M_VISS_MAX_OUT_DMA_CH; cnt++)
    {
        chPrms = &hObj->outChPrms[cnt];

        sl2Prms->outSl2Addr[cnt] = chPrms->sl2Addr;
        sl2Prms->outWidthInBytes[cnt] = chPrms->sl2Pitch;
        sl2Prms->outSl2BuffDepth[cnt] = chPrms->sl2Depth;

    }
}

void Vhwa_m2mVissUpdatePipeline(uint32_t htsPipeline)
{
    Vhwa_M2mVissInstObj *instObj = NULL;

    instObj = &gM2mVissInstObj[0U];

    /* Disable HTS Interrupt */
    Vhwa_disableHtsIntr(instObj->socInfo.vpacIntdRegs,
                instObj->vhwaIrqNum, instObj->pipeline);

    instObj->pipeline = htsPipeline;

    /* Enable Pipeline interrupt in INTD */
    Vhwa_enableHtsIntr(instObj->socInfo.vpacIntdRegs,
                instObj->vhwaIrqNum, instObj->pipeline);
}

static void Vhwa_m2mVissUpdateFcPrms(Vhwa_M2mVissHandleObj *hObj,
                                    Vhwa_M2mVissFcUpdatePrms *fcPrms)
{
    hObj->htsCfg.dmaConsCfg[0].enable = fcPrms->dmaConsEnable[0];
    hObj->htsCfg.dmaConsCfg[1].enable = fcPrms->dmaConsEnable[1];
    hObj->htsCfg.dmaConsCfg[2].enable = fcPrms->dmaConsEnable[2];
    hObj->htsCfg.dmaConsCfg[3].enable = fcPrms->dmaConsEnable[3];
    hObj->htsCfg.dmaConsCfg[4].enable = fcPrms->dmaConsEnable[4];

    hObj->fcStatus.outDmaEnable[0] = fcPrms->dmaConsEnable[0];
    hObj->fcStatus.outDmaEnable[1] = fcPrms->dmaConsEnable[1];
    hObj->fcStatus.outDmaEnable[2] = fcPrms->dmaConsEnable[2];
    hObj->fcStatus.outDmaEnable[3] = fcPrms->dmaConsEnable[3];
    hObj->fcStatus.outDmaEnable[4] = fcPrms->dmaConsEnable[4];

    /* DMA related Parameters */
    hObj->fcStatus.isFlexConnect = fcPrms->isFlexConnect;
}


static void Vhwa_m2mVissUpdateFcConnPrms(Vhwa_M2mVissHandleObj *hObj,
                                    Vhwa_M2mVissFcConPrms *sl2FcPrms)
{
    uint8_t              *pTrMem = NULL;
    CSL_UdmapTR9          *pTr9;

    hObj->htsCfg.prodCfg[sl2FcPrms->ctrlIdx].enable = UTRUE;
    hObj->htsCfg.prodCfg[sl2FcPrms->ctrlIdx].isMaskSelect =
                                                sl2FcPrms->enableMaskSel;
    hObj->htsCfg.prodCfg[sl2FcPrms->ctrlIdx].maskSelect = sl2FcPrms->maskSel;

    hObj->htsCfg.prodCfg[sl2FcPrms->ctrlIdx].consId = sl2FcPrms->htsConsId;
    hObj->htsCfg.prodCfg[sl2FcPrms->ctrlIdx].depth = sl2FcPrms->sl2Depth;

    hObj->htsCfg.prodCfg[sl2FcPrms->ctrlIdx].threshold =
                                                sl2FcPrms->htsThreshold;
    hObj->htsCfg.prodCfg[sl2FcPrms->ctrlIdx].cntPreLoad =
                                                sl2FcPrms->htsDmaProdPreLoad;
    hObj->htsCfg.prodCfg[sl2FcPrms->ctrlIdx].cntPostLoad =
                                                sl2FcPrms->htsDmaProdPostLoad;
    hObj->htsCfg.prodCfg[sl2FcPrms->ctrlIdx].countDec  =
                                                sl2FcPrms->htsDmaProdCntDec;

    hObj->htsCfg.prodCfg[sl2FcPrms->outputIdx].depth = sl2FcPrms->sl2Depth;

    hObj->outChPrms[sl2FcPrms->outputIdx].sl2Addr = sl2FcPrms->sl2Addr;
    hObj->outChPrms[sl2FcPrms->outputIdx].sl2Depth = sl2FcPrms->sl2Depth;
    hObj->outChPrms[sl2FcPrms->outputIdx].sl2Pitch = sl2FcPrms->sl2Pitch;

    hObj->lseCfg.outChCfg[sl2FcPrms->outputIdx].bufAddr = (uint32_t)sl2FcPrms->sl2Addr;
    hObj->lseCfg.outChCfg[sl2FcPrms->outputIdx].circBufSize = sl2FcPrms->sl2Depth;
    hObj->lseCfg.outChCfg[sl2FcPrms->outputIdx].lineOffset = sl2FcPrms->sl2Pitch;


    pTrMem = hObj->outChPrms[sl2FcPrms->outputIdx].trMem;
    pTr9 = (CSL_UdmapTR9 *)((uint32_t)pTrMem + sizeof(CSL_UdmapTR15));
    CacheP_Inv(pTrMem, VHWA_M2M_VISS_UDMA_TRPD_SIZE);

    pTr9->addr    = sl2FcPrms->sl2Addr;
    pTr9->icnt1   = (uint16_t)sl2FcPrms->sl2Depth;
    pTr9->icnt2   =
        (uint16_t)(hObj->outChPrms[sl2FcPrms->outputIdx].height/sl2FcPrms->sl2Depth);

    pTr9->dim1 = (int32_t)sl2FcPrms->sl2Pitch;
    hObj->outChPrms[sl2FcPrms->outputIdx].sl2Pitch = sl2FcPrms->sl2Pitch;
    hObj->lseCfg.outChCfg[sl2FcPrms->outputIdx].lineOffset = sl2FcPrms->sl2Pitch;

    if ((pTr9->icnt2 * pTr9->icnt1) != hObj->outChPrms[sl2FcPrms->outputIdx].height)
    {
        pTr9->icnt2 ++;
    }

    pTr9->dim1 = (int32_t)sl2FcPrms->sl2Pitch;

    CacheP_wb(pTrMem, VHWA_M2M_VISS_UDMA_TRPD_SIZE);

}

static int32_t vhwaM2mVissCheckCreatePrms(uint32_t drvId, uint32_t drvInstId)
{
    int32_t     status = FVID2_SOK;

    if (FVID2_VHWA_M2M_VISS_DRV_ID != drvId)
    {
        GT_0trace(VhwaVissTrace, GT_ERR, "Invalid Driver ID !!\n");
        status = FVID2_EINVALID_PARAMS;
    }
    else
    {
        /* Check for correct instance ID */
#if defined VHWA_M2M_VPAC_INSTANCE
#if (VHWA_M2M_VPAC_INSTANCE == 0)
        if (VHWA_M2M_VPAC_0_VISS_DRV_INST_ID_0 != drvInstId)
        {
            status = FVID2_EINVALID_PARAMS;
            GT_0trace(VhwaLdcTrace, GT_ERR, "Invalid/unsupported Instance Id\n");
        }
#endif
#endif

#if defined VHWA_M2M_VPAC_INSTANCE
#if (VHWA_M2M_VPAC_INSTANCE == 1)
        if (VHWA_M2M_VPAC_1_VISS_DRV_INST_ID_0 != drvInstId)
        {
            status = FVID2_EINVALID_PARAMS;
            GT_0trace(VhwaLdcTrace, GT_ERR, "Invalid/unsupported Instance Id\n");
        }
#endif
#endif
    }

    return (status);
}
