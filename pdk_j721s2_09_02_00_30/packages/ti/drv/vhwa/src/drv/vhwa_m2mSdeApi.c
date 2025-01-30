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
 *  \file vhwa_m2mSdeApi.c
 *
 *  \brief API Implementation
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/drv/vhwa/src/drv/vhwa_m2mSdePriv.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/** Channel Indexes for HTS Configuration */
#define HTS_SDE_REFE_FRAME_CH_IDX           (0U)
#define HTS_SDE_BASE_FRAME_CH_IDX           (1U)
#define HTS_SDE_OUT_CH_IDX                  (0U)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static void Vhwa_sdeSetFocoParams(Vhwa_M2mSdeHandleObj *hObj,
                                     const Vhwa_M2mSdePrms *sdePrms);

static int32_t Vhwa_sdeEnableFoco(const Vhwa_M2mSdeInstObj *instObj,
                                  const Vhwa_M2mSdeHandleObj *hObj);

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
static Vhwa_M2mSdeHandleObj *Vhwa_sdeAllocHandleObj(
                                const Vhwa_M2mSdeInstObj *instObj);

/**
 * \brief   Create Queues, required for storing pending requests
 *
 * \param   instObj             Instance object.
 *
 * \return  pointer to handle object on success else NULL.
 *
 **/
static int32_t Vhwa_sdeCreateQueues(Vhwa_M2mSdeInstObj *instObj);

/**
 * \brief   Delete Queues
 *
 * \param   instObj             Instance object.
 *
 **/
static void Vhwa_sdeDeleteQueues(Vhwa_M2mSdeInstObj *instObj);

/**
 * \brief   Local function to freeup allocated Handle Object and return it
 *          to the pool of free handle objects.
 *          No protection inside the function, Caller should protect
 *          the function call
 *
 * \param   hObj                Handle Object to be freed up.
 *
 **/
static void Vhwa_sdeFreeHandleObj(Vhwa_M2mSdeHandleObj *hObj);

/**
 * \brief   Function to check given SDE configuration, used in
 *          SET_PARAMS ioctl it first initializes LSE and HTS
 *          config and then uses CSLFL of LSE and HTS to validate
 *          the configuration
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 * \param   sdePrms             SDE configuration to be validated
 *
 * \return  FVID2_SOK if given SDE cofiguration is valid, error code otherwise.
 *
 **/
static int32_t Vhwa_sdeCheckCfg(const Vhwa_M2mSdeInstObj *instObj,
                                Vhwa_M2mSdeHandleObj *hObj,
                                const Vhwa_M2mSdePrms *sdePrms);

/**
 * \brief   Based on the given SDE config, it initializes LSE configuration.
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 * \param   sdePrms              SDE Configuration
 *
 **/
static void Vhwa_sdeSetLseCfg(Vhwa_M2mSdeHandleObj *hObj,
                              Vhwa_M2mSdePrms *sdePrms);

/**
 * \brief   Based on the given SDE config, it initializes HTS configuration.
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 * \param   sdePrms             SDE Configuration
 *
 **/
static void Vhwa_sdeSetHtsCfg(Vhwa_M2mSdeInstObj *instObj,
    Vhwa_M2mSdeHandleObj *hObj, Vhwa_M2mSdePrms *sdePrms);

/**
 * \brief   Calculate and store the Sl2 buffer parameters based on SDE
 *          configuration
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 * \param   sdePrms             SDE configuration to be set
 *
 **/
static int32_t Vhwa_sdeSetSl2Prms(const Vhwa_M2mSdeInstObj *instObj,
                                  Vhwa_M2mSdeHandleObj *hObj,
                                  Vhwa_M2mSdePrms *sdePrms);

/**
 * \brief   Set the HTS bandwidth limiter parameters
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 * \param   bwLimit             BW limiter configuration
 *
 **/
static void Vhwa_sdeSetHtsLimitParams(Vhwa_M2mSdeHandleObj *hObj,
                                      const Vhwa_HtsLimiter *bwLimit);

/**
 * \brief   Implementation of SET_PARAMS ioctl.
 *          It uses CheckSdeCfg to validate the config.
 *          If it is valid, copies the config into handle object
 *          If it is invalid, it reverts LSE/HTS config to known valid config
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 * \param   sdeCfg              SDE configuration to be set
 *
 **/
static int32_t Vhwa_sdeSetParams(Vhwa_M2mSdeInstObj *instObj,
    Vhwa_M2mSdeHandleObj *hObj, Vhwa_M2mSdePrms *sdeCfg);

/**
 * \brief   Sets the error event parameters
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 * \param   eePrms              Error Event parameters
 *
 **/
static int32_t Vhwa_sdeSetEeParams(const Vhwa_M2mSdeInstObj *instObj,
                                   Vhwa_M2mSdeHandleObj *hObj,
                                   const Sde_ErrEventParams *eePrms);

/**
 * \brief   Calculate the TR paramters for the disparity output
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 *
 **/
static void Vhwa_sdeCalOutTrParams(Vhwa_M2mSdeHandleObj *hObj);

/**
 * \brief   Set Sl2 buffer parameters for SDE for given configuration
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 * \param   sdePrms             SDE configuration to be set
 *
 **/
static void Vhwa_sdeSetBuffParams(Vhwa_M2mSdeHandleObj *hObj);


/**
 * \brief   Minimal function to submit request to hw and start SDE operation.
 *          It first sets address in the TR, Submits it to the ring and
 *          starts the pipeline.
 *          It does not start currently SDE, HTS schedulers.
 *
 * \param   instObj             Instance Object, used for getting base address
 * \param   qObj                Queue Object, used for getting handle object
 *
 **/
static int32_t Vhwa_sdeSubmitRequest(Vhwa_M2mSdeInstObj *instObj,
                                     Vhwa_M2mSdeQueueObj *qObj);

/**
 * \brief   Function to configure the hardware register for new request
 *
 * \param   instObj             Instance Object, used for getting base address
 * \param   qObj                Queue Object, used for getting handle object
 *
 **/
static int32_t Vhwa_m2mSdeSetConfigInHW(Vhwa_M2mSdeInstObj *instObj,
                                        const Vhwa_M2mSdeQueueObj *qObj);

/* Implementation of FVID2 APIs */

/**
 * \brief   FVID2 Create Function.
 *
 * \param   instObj             Instance object.
 *
 * \return  FVID2 Driver Handle.
 *
 **/
Fdrv_Handle vhwa_m2mSdeCreate(UInt32 drvId, UInt32 drvInstId,
    Ptr createArgs, Ptr createStatusArgs, const Fvid2_DrvCbParams *cbPrms);

/**
 * \brief   FVID2 Delete Function.
 *
 * \param   handle              FVID2 driver handle.
 *
 * \return  FVID2_SOK on success, else FVID2 error code
 *
 **/
Int32 vhwa_m2mSdeDelete(Fdrv_Handle handle, Ptr deleteArgs);

/**
 * \brief   FVID2 Control Function.
 *
 * \param   handle              FVID2 driver handle.
 *
 * \return  FVID2_SOK on success, else FVID2 error code
 *
 **/
Int32 vhwa_m2mSdeControl(Fdrv_Handle handle, UInt32 cmd, Ptr cmdArgs,
    Ptr cmdStatusArgs);

/**
 * \brief   FVID2 Process Request function.
 *
 * \param   handle              FVID2 driver handle.
 *
 * \return  FVID2_SOK on success, else FVID2 error code
 *
 **/
Int32 vhwa_m2mSdeProcessReq(Fdrv_Handle handle, Fvid2_FrameList *inFrmList,
    Fvid2_FrameList *outFrmList, uint32_t timeout);

/**
 * \brief   FVID2 Get Process Request function.
 *
 * \param   handle              FVID2 driver handle.
 *
 * \return  FVID2_SOK on success, else FVID2 error code
 *
 **/
Int32 vhwa_m2mSdeGetProcessReq(Fdrv_Handle handle,
    Fvid2_FrameList *inProcessList, Fvid2_FrameList *outProcessList,
    UInt32 timeout);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
Vhwa_M2mSdeHandleObj gM2mSdeHandleObj[VHWA_M2M_SDE_MAX_HANDLES];
Vhwa_M2mSdeInstObj   gM2mSdeInstObj;

Fvid2_DrvOps gM2mSdeFvid2DrvOps = {
    FVID2_VHWA_M2M_SDE_DRV_ID,
    /**< Unique driver Id. */
    vhwa_m2mSdeCreate,
    /**< FVID2 create function pointer. */
    vhwa_m2mSdeDelete,
    /**< FVID2 delete function pointer. */
    vhwa_m2mSdeControl,
    /**< FVID2 control function pointer. */
    NULL, NULL,
    /**< FVID2 queue function pointer. */
    vhwa_m2mSdeProcessReq,
    /**< FVID2 process request function pointer. */
    vhwa_m2mSdeGetProcessReq,
    /**< FVID2 get processed request function pointer. */
};

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int32_t Vhwa_m2mSdeInit(Vhwa_M2mSdeInitParams *initPrms)
{
    int32_t             status;
    uint32_t            cnt;
    SemaphoreP_Params   params;
    Vhwa_M2mSdeInstObj *instObj = NULL;

    if (NULL != initPrms)
    {
        instObj = &gM2mSdeInstObj;

        /* Reset all instance object to 0x0 */
        Fvid2Utils_memset(instObj, 0U, sizeof (Vhwa_M2mSdeInstObj));

        /* Mark pool flags as free */
        for (cnt = 0U; cnt < VHWA_M2M_SDE_MAX_HANDLES; cnt++)
        {
            gM2mSdeHandleObj[cnt].isUsed = (uint32_t) UFALSE;
        }

        /* Set HTS pipeline */
        instObj->pipeline = VHWA_M2M_SDE_HTS_PIPELINE;

        Sde_getSocInfo((void *)&instObj->socInfo);

        status = Fvid2_registerDriver(&gM2mSdeFvid2DrvOps);
        if (FVID2_SOK == status)
        {
            instObj->isRegistered = (uint32_t)UTRUE;
        }
        else
        {
            GT_0trace(VhwaSdeTrace, GT_ERR,
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
                GT_0trace(VhwaSdeTrace, GT_ERR,
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
                GT_0trace(VhwaSdeTrace, GT_ERR,
                    "Failed to allocate HWA Lock semaphore!!\n");
                status = FVID2_EALLOC;
            }
        }

        /* Create free and request queues */
        if (FVID2_SOK == status)
        {
            status = Vhwa_sdeCreateQueues(instObj);
        }

        if (FVID2_SOK == status)
        {
            /* Initialize UDMA, ie allocate and initialize channels
               required for SDE output */
            status = Vhwa_m2mSdeUdmaInit(instObj, initPrms);
            if (FVID2_SOK != status)
            {
                GT_0trace(VhwaSdeTrace, GT_ERR,
                    "UDMA Initialization Failed !!\n");
            }
        }

        /* Register ISR handler for the given irq number */
        if (FVID2_SOK == status)
        {
            instObj->irqNum = initPrms->irqInfo.irqNum;
            instObj->vhwaIrqNum = initPrms->irqInfo.vhwaIrqNum;

            status = Vhwa_m2mSdeRegisterIsr(instObj);
        }

        /* Init is done, copy init params locally,
           enable DMPAC and SDE in DMPAC Top */
        if (FVID2_SOK == status)
        {
            Fvid2Utils_memcpy(&instObj->initPrms, initPrms,
                sizeof(Vhwa_M2mSdeInitParams));

            /* Enable SDE at DMPAC Top*/
            CSL_dmpacEnableModule(instObj->socInfo.dmpacCntlRegs,
                DMPAC_MODULE_SDE, (uint32_t)UTRUE);

            instObj->initDone = (uint32_t)UTRUE;
            instObj->lastHndlObj = NULL;
        }
    }
    else
    {
        status = FVID2_EBADARGS;
    }

    if (FVID2_SOK != status)
    {
        Vhwa_m2mSdeDeInit();
    }

    return (status);
}

void Vhwa_m2mSdeDeInit(void)
{
    Vhwa_M2mSdeInstObj *instObj = NULL;

    instObj = &gM2mSdeInstObj;

    if (instObj->openCnt > 0u)
    {
        GT_0trace(VhwaSdeTrace, GT_ERR,
            "Warning: All driver handles are not closed!!\n");
    }

    /* Stop UDMA channels */
    (void)Vhwa_m2mSdeStopCh(instObj);

    Vhwa_m2mSdeUnregisterIsr(instObj);

    (void)Vhwa_m2mSdeUdmaDeInit(instObj);

    if ((uint32_t)UTRUE == instObj->isRegistered)
    {
        (void)Fvid2_unRegisterDriver(&gM2mSdeFvid2DrvOps);
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

    Vhwa_sdeDeleteQueues(instObj);

    /* Init all global variables to zero */
    Fvid2Utils_memset(instObj, 0U, sizeof (gM2mSdeInstObj));

    instObj->initDone = (uint32_t)UFALSE;
}

Fdrv_Handle vhwa_m2mSdeCreate(UInt32 drvId, UInt32 drvInstId,
    Ptr createArgs, Ptr createStatusArgs, const Fvid2_DrvCbParams *cbPrms)
{
    int32_t                 status = FVID2_SOK;
    Vhwa_M2mSdeInstObj     *instObj = NULL;
    Vhwa_M2mSdeHandleObj   *hObj = NULL;
    Vhwa_M2mSdeCreateArgs  *sdeCreateArgs = NULL;
    Fdrv_Handle             handle = NULL;
    Sde_SocInfo            *socInfo = NULL;
    Vhwa_M2mSdeSl2AllocPrms sl2AllocPrms;

    instObj = &gM2mSdeInstObj;
    socInfo = &instObj->socInfo;

    /* Check for errors */
    if ((FVID2_VHWA_M2M_SDE_DRV_ID != drvId) ||
        (VHWA_M2M_SDE_DRV_INST_ID != drvInstId) ||
        (NULL == createArgs) ||
        (NULL == cbPrms))
    {
        GT_0trace(VhwaSdeTrace, GT_ERR, "NULL Pointer !!\n");
        status = FVID2_EBADARGS;
    }
    else
    {
        /* Open not allowed if init is not done */
        if ((uint32_t)UFALSE == instObj->initDone)
        {
            GT_0trace(VhwaSdeTrace, GT_ERR,
                "Vhwa_m2mSdeInit is not called\n");
            status  = FVID2_EFAIL;
        }

        sdeCreateArgs = (Vhwa_M2mSdeCreateArgs *)createArgs;
    }

    if (FVID2_SOK == status)
    {
        /* Take the instance semaphore */
        (void)SemaphoreP_pend(instObj->lock, SemaphoreP_WAIT_FOREVER);

        /* Allocate Handle Object */
        hObj = Vhwa_sdeAllocHandleObj(instObj);

        if (NULL != hObj)
        {
            if (0U == instObj->openCnt)
            {
                /* On the first open, enable interrupts in HTS and
                 * start all UTC channels.
                 * Even if all UTC channels are enabled, only channels enabled
                 * in HTS will be used for the transfer */

                /* Check if SL2 is allocated, Allocate the SL2 for default
                   if not allocated */
                if ((uint32_t)UFALSE == instObj->isSl2AllocDone)
                {
                    GT_0trace(VhwaSdeTrace, GT_DEBUG,
                    "Vhwa_m2mSdeAllocSl2 is not called, allocating for default\n");

                    Vhwa_M2mSdeSl2AllocPrmsInit(&sl2AllocPrms);

                    status = Vhwa_m2mSdeAllocSl2(&sl2AllocPrms);
                }



                if (FVID2_SOK == status)
                {
                    /* Start UDMA Channels on the first handle Create */
                    status = Vhwa_m2mSdeStartCh(instObj);
                }

                if (FVID2_SOK == status)
                {
                    /* Enable Pipeline interrupt in INTD */
                    Vhwa_enableDmpacHtsIntr(socInfo->dmpacIntdRegs,
                        instObj->vhwaIrqNum, instObj->pipeline);
                }
            }

            hObj->sl2Prms.sl2OutBuffDepth = VHWA_M2M_SDE_MIN_OUT_BUFF_DEPTH;

            /* Create doneQ, Done Q is specific to handle so that
             * FVID2_Dequeue returns correct request for that handle. */
            if (FVID2_SOK == status)
            {
                status = Fvid2Utils_constructQ(&hObj->doneQObj);
            }
            if (FVID2_SOK == status)
            {
                hObj->doneQ = &hObj->doneQObj;

                /* Allocate Descriptor, Ring memories */
                status = Vhwa_m2mSdeAllocUdmaMem(hObj);
            }

            if (FVID2_SOK == status)
            {
                Fvid2Utils_memcpy(&hObj->createArgs, sdeCreateArgs,
                    sizeof(Vhwa_M2mSdeCreateArgs));

                Fvid2Utils_memcpy(&hObj->cbPrms, cbPrms,
                    sizeof (hObj->cbPrms));

                instObj->openCnt ++;
            }
            else
            {
                /* Some error, so free up the handle object */
                Vhwa_sdeFreeHandleObj(hObj);
            }
        }
        else
        {
            status = FVID2_EALLOC;
        }

        if (FVID2_SOK == status)
        {
            handle = (Fdrv_Handle)hObj;
        }

        (void)SemaphoreP_post(instObj->lock);
    }

    return (handle);
}

Int32 vhwa_m2mSdeDelete(Fdrv_Handle handle, Ptr deleteArgs)
{
    int32_t                 status = FVID2_SOK;
    Vhwa_M2mSdeInstObj     *instObj = NULL;
    Vhwa_M2mSdeHandleObj   *hObj = NULL;
    Sde_SocInfo            *socInfo = NULL;

    if (NULL != handle)
    {
        instObj = &gM2mSdeInstObj;
        hObj    = (Vhwa_M2mSdeHandleObj *)handle;
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
            Vhwa_sdeFreeHandleObj(hObj);

            instObj->openCnt --;

            /* Delete the Done queue */
            if (NULL != hObj->doneQ)
            {
                Fvid2Utils_destructQ(hObj->doneQ);
                hObj->doneQ = NULL;
            }

            /* for the last close, stop all channels and
             * disable HTS interrupt in INTD */
            if (0U == instObj->openCnt)
            {
                /* Disable HTS Interrupt */
                Vhwa_disableDmpacHtsIntr(socInfo->dmpacIntdRegs,
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

Int32 vhwa_m2mSdeControl(Fdrv_Handle handle, UInt32 cmd, Ptr cmdArgs,
    Ptr cmdStatusArgs)
{
    int32_t                 status = FVID2_SOK;
    Vhwa_M2mSdeInstObj     *instObj = NULL;
    Vhwa_M2mSdeHandleObj   *hObj = NULL;
    Vhwa_M2mSdePrms        *sdePrms = NULL;
    Sde_ErrEventParams     *eePrms = NULL;
    Vhwa_HtsLimiter        *htsLimit = NULL;

    if (NULL == handle)
    {
        status = FVID2_EBADARGS;
    }
    else
    {
        instObj = &gM2mSdeInstObj;
        hObj = (Vhwa_M2mSdeHandleObj *)handle;
    }
    if (FVID2_SOK == status)
    {
        /* Take the instance semaphore */
        (void)SemaphoreP_pend(instObj->lock, SemaphoreP_WAIT_FOREVER);

        switch (cmd)
        {
            /* Set SDE Parameters */
            case VHWA_M2M_IOCTL_SDE_SET_PARAMS:
            {
                if (NULL != cmdArgs)
                {
                    sdePrms = (Vhwa_M2mSdePrms *)cmdArgs;
                    status = Vhwa_sdeSetParams(instObj, hObj, sdePrms);
                }
                break;
            }

            /* Enable Register error events callback */
            case VHWA_M2M_IOCTL_SDE_REGISTER_ERR_CB:
            {
                if (NULL != cmdArgs)
                {
                    eePrms = (Sde_ErrEventParams *)cmdArgs;
                    status = Vhwa_sdeSetEeParams(instObj, hObj, eePrms);
                }
                break;
            }

            /* SET HTS Limiter config */
            case VHWA_M2M_IOCTL_SDE_SET_HTS_LIMIT:
            {
                if (NULL != cmdArgs)
                {
                    htsLimit = (Vhwa_HtsLimiter *)cmdArgs;

                    Vhwa_sdeSetHtsLimitParams(hObj, htsLimit);
                }
                break;
            }

            /* Get PSA Signature */
            case VHWA_M2M_IOCTL_SDE_GET_PSA_SIGN:
            {
                if (NULL != cmdArgs)
                {
                    status = CSL_sdeGetPsa(instObj->socInfo.sdeRegs, (uint32_t *)cmdArgs);
                }
                break;
            }

            /* Get Histogram */
            case VHWA_M2M_IOCTL_SDE_GET_HISTOGRAM:
            {
                uint32_t *csHistogram = NULL;
                if (NULL != cmdArgs)
                {
                    csHistogram = (uint32_t *)cmdArgs;
                    status = CSL_sdeGetCsHistogram(instObj->socInfo.sdeRegs, csHistogram);
                }
                break;
            }

            /* Get performance */
            case VHWA_M2M_IOCTL_SDE_GET_PERFORMANCE:
            {
                uint32_t *perf = NULL;
                if (NULL != cmdArgs)
                {
                    perf = (uint32_t *)cmdArgs;
                    *perf = (uint32_t)hObj->perfNum;
                }
                break;
            }

            case VHWA_M2M_IOCTL_SDE_SYNC_START:
            {
                if (NULL != hObj->createArgs.getTimeStamp)
                {
                    hObj->perfNum = hObj->createArgs.getTimeStamp();
                }

                status = CSL_dmpacHtsPipelineStart(instObj->socInfo.htsRegs, &hObj->htsCfg);
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

Int32 vhwa_m2mSdeProcessReq(Fdrv_Handle handle, Fvid2_FrameList *inFrmList,
    Fvid2_FrameList *outFrmList, uint32_t timeout)
{
    int32_t               status = FVID2_SOK;
    uint32_t              semTimeout;
    Vhwa_M2mSdeInstObj   *instObj = NULL;
    Vhwa_M2mSdeHandleObj *hObj = NULL;
    Vhwa_M2mSdeQueueObj  *qObj = NULL;
    SemaphoreP_Status     semStatus;

    if ((NULL == handle) || (NULL == inFrmList) || (NULL == outFrmList))
    {
        status = FVID2_EBADARGS;
    }
    else
    {
        instObj = &gM2mSdeInstObj;
        hObj = (Vhwa_M2mSdeHandleObj *)handle;
    }

    /* Check for null pointers */
    if (FVID2_SOK == status)
    {
        /* Input Buffer Addresses cannot be null */
        if ((NULL == inFrmList->frames[SDE_INPUT_REFERENCE_IMG]) ||
            (0U == inFrmList->frames[SDE_INPUT_REFERENCE_IMG]->addr[0U]) ||
            (NULL == inFrmList->frames[SDE_INPUT_BASE_IMG]) ||
            (0U == inFrmList->frames[SDE_INPUT_BASE_IMG]->addr[0U]) ||
            (NULL == outFrmList->frames[0U]) ||
            (0U == outFrmList->frames[0u]->addr[0U]))
        {
            status = FVID2_EBADARGS;
        }
    }

    if (FVID2_SOK == status)
    {
        /** Steps
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
         * number of pending request counter,
         * */
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

            if (SemaphoreP_TIMEOUT == semStatus)
            {
                status = FVID2_EAGAIN;
            }
            else /* For all other cases, masr as failed */
            {
                status = FVID2_EFAIL;
            }
        }

        if (FVID2_SOK == status)
        {
            /* Get a queue object from the free queue,
             * No need to protect from ISR as it is not accessed from ISR */
            qObj = (Vhwa_M2mSdeQueueObj *) Fvid2Utils_dequeue(instObj->freeQ);

            if (NULL == qObj)
            {
                GT_0trace(VhwaSdeTrace, GT_ERR,
                    "Failed to Free Queue Object!!\n");
                status = FVID2_EALLOC;
            }
            else
            {

                qObj->hObj = hObj;
                /* Copy the application's process list to request objects lists */
                Fvid2_copyFrameList(&qObj->inFrmList, inFrmList);
                Fvid2_copyFrameList(&qObj->outFrmList, outFrmList);
            }
        }

        if (FVID2_SOK == status)
        {
            Vhwa_m2mSdeSetAddress(qObj->hObj, &qObj->inFrmList,
                                    &qObj->outFrmList);
            /* HW is free, submit request to the hardware */
            /* If previous handle and current handles are same, */
            if (instObj->lastHndlObj != qObj->hObj)
            {
                /** Last handle was not same as new handle,
                 *  so Buffer Parameters needs to be configured */
                status = CSL_sdeSetBufParam(instObj->socInfo.sdeRegs,
                                                &hObj->sdeBuffPrms);

                if(FVID2_SOK == status)
                {
                    status = Vhwa_m2mSdeSetConfigInHW(instObj, qObj);
                }
            }
            if (FVID2_SOK == status)
            {
                /** Set the addresses, Configure SDE/HTS, Submit the TR
                 *  Start the pipeline */
                status = Vhwa_sdeSubmitRequest(instObj, qObj);
            }
        }
    }

    return (status);
}

/** \brief Typedef for FVID2 get processed frames function pointer. */
Int32 vhwa_m2mSdeGetProcessReq(Fdrv_Handle handle,
    Fvid2_FrameList *inFrmList, Fvid2_FrameList *outFrmList,
    UInt32 timeout)
{
    int32_t                status = FVID2_SOK;
    uint32_t               cookie;
    Vhwa_M2mSdeInstObj    *instObj = NULL;
    Vhwa_M2mSdeHandleObj  *hObj = NULL;
    Vhwa_M2mSdeQueueObj   *qObj = NULL;

    if (NULL == handle)
    {
        status = FVID2_EBADARGS;
    }
    else
    {
        instObj = &gM2mSdeInstObj;
        hObj = (Vhwa_M2mSdeHandleObj *)handle;
    }

    if (FVID2_SOK == status)
    {
        /* Take the instance semaphore, so that no other
         * handle can submit request from the task context. */
        (void)SemaphoreP_pend(instObj->lock, SemaphoreP_WAIT_FOREVER);

        /* Disable interrupts before accessing queue */
        cookie = HwiP_disable();

        /* Dequeue the completed request from done queue */
        qObj = (Vhwa_M2mSdeQueueObj *) Fvid2Utils_dequeue(hObj->doneQ);

        /* Restore interrupts after updating node list */
        HwiP_restore(cookie);

        /* No buffers in the output queue */
        if (NULL == qObj)
        {
            /* Check if requests are pending with driver */
            if (0U == hObj->numPendReq)
            {
                /* Nothing is queued */
                GT_0trace(VhwaSdeTrace, GT_DEBUG,
                    "Nothing to dequeue. No request pending with driver!!\r\n");
                status = FVID2_ENO_MORE_BUFFERS;
            }
            else
            {
                /* If no request have completed, return try again */
                GT_0trace(VhwaSdeTrace, GT_DEBUG,
                    "Nothing to dequeue. Try again!!\r\n");
                status = FVID2_EAGAIN;
            }
        }
        else /* OutQ has buffer to be returned */
        {
            /* Copy the driver's process list to application's process list */
            Fvid2_copyFrameList(inFrmList, &qObj->inFrmList);
            Fvid2_copyFrameList(outFrmList, &qObj->outFrmList);

            /* Dequeue from Rings */
            status = Vhwa_m2mSdePopRings(instObj, hObj);

            /* Return back the queue object to the free queue,
             * No need to protect from ISR, as it is not accessed in ISR */
            Fvid2Utils_queue(instObj->freeQ, &qObj->qElem, qObj);

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

int32_t Vhwa_m2mSdeAllocSl2(const Vhwa_M2mSdeSl2AllocPrms *sl2AllocPrms)
{
    int32_t status = FVID2_SOK;
    uint32_t pitchSl2;
    uint32_t imgMem, outBuffMem, rcBuffMem, focoBuffMem, tMemReq;
    Vhwa_M2mSdeInstObj *instObj = NULL;

    instObj = &gM2mSdeInstObj;

    /* Cannot even lock, if init is not done */
    if ((uint32_t)UFALSE == instObj->initDone)
    {
        GT_0trace(VhwaSdeTrace, GT_ERR,
            "Driver init is not done!!\n");
        status = FVID2_EFAIL;
    }
    else
    {
        /* Still need to check if provided sl2AllocPrms is not null and SL2 is
         * not allocated */
        if ((NULL == sl2AllocPrms) || (UTRUE == instObj->isSl2AllocDone))
        {
            status = FVID2_EBADARGS;
        }
    }

    /* Lock instance semaphore */
    (void)SemaphoreP_pend(instObj->lock, SemaphoreP_WAIT_FOREVER);

    if (FVID2_SOK == status)
    {
        /* Ref and Base Memory Required */
        /* Image data in SL2 should always starts at a 64 byte aligned boundary*/
        /* If width is multiple of 256 to alivate peak SL2 usage Line
           staggering is required */
        pitchSl2 = Vhwa_calcHorzSizeInBytes(sl2AllocPrms->maxImgWidth,
                                            FVID2_CCSF_BITS12_PACKED);
        pitchSl2 = Vhwa_calcSl2Pitch(pitchSl2);

        imgMem = pitchSl2 * SDE_SL2_REF_BASE_IMG_CIR_BUFF_SIZE;

        /* Set Out buffer Parameters */
        outBuffMem = SDE_PER_OUT_BUFF_SIZE * sl2AllocPrms->disBuffDepth;

        /* Set Row cost Buffer Parameters */
        rcBuffMem= (sl2AllocPrms->maxImgWidth/8u)*2u*8u*8u;

        if(sl2AllocPrms->searchRange == SDE_SR_192)
        {
            rcBuffMem = rcBuffMem * 3u;
        }
        else if(sl2AllocPrms->searchRange == SDE_SR_128)
        {
            rcBuffMem = rcBuffMem * 2u;
        }
        else
        {
            /* Nothing to do here */
        }

        /* Set FOCO Parameters */
        if(FVID2_CCSF_BITS12_PACKED == sl2AllocPrms->inCcsf)
        {
            focoBuffMem = 0;
        }
        else
        {
            focoBuffMem = Vhwa_calcHorzSizeInBytes(sl2AllocPrms->maxImgWidth,
                                                   sl2AllocPrms->inCcsf);
            focoBuffMem = Vhwa_calcSl2Pitch(focoBuffMem);
            focoBuffMem = focoBuffMem * 2u;
        }

        tMemReq = (imgMem*2u) + outBuffMem + rcBuffMem + (focoBuffMem*2u);

        instObj->sl2StartAddr = Vhwa_allocateSl2(tMemReq, VHWA_SL2_INST_DMPAC);
        if(0u == instObj->sl2StartAddr)
        {
            status = FVID2_EALLOC;
        }
        else
        {
            instObj->sl2Size = tMemReq;
            status = FVID2_SOK;
        }
    }
    if(FVID2_SOK == status)
    {
        instObj->isSl2AllocDone = (uint32_t)UTRUE;
    }

    /* Release instance semaphore */
    (void)SemaphoreP_post(instObj->lock);

    return (status);
}

int32_t Vhwa_m2mSdeFreeSl2(void)
{
    int32_t retVal = FVID2_SOK;
    Vhwa_M2mSdeInstObj *instObj = NULL;

    instObj = &gM2mSdeInstObj;

    if (UTRUE == instObj->isSl2AllocDone)
    {
        if(0U == instObj->openCnt)
        {
            /* Lock instance semaphore */
            (void)SemaphoreP_pend(instObj->lock, SemaphoreP_WAIT_FOREVER);

            Vhwa_FreeSl2(instObj->sl2StartAddr, VHWA_SL2_INST_DMPAC);

            instObj->isSl2AllocDone = (uint32_t)UFALSE;
            instObj->sl2Size        = 0u;
            instObj->sl2StartAddr   = 0u;

            /* Release instance semaphore */
            (void)SemaphoreP_post(instObj->lock);
        }
        else
        {
            retVal = FVID2_EFAIL;
        }
    }

    return (retVal);
}

/* ========================================================================== */
/*                           Local Functions                                  */
/* ========================================================================== */

static int32_t Vhwa_sdeSetEeParams(const Vhwa_M2mSdeInstObj *instObj,
                                   Vhwa_M2mSdeHandleObj *hObj,
                                   const Sde_ErrEventParams *eePrms)
{
    int32_t status = FVID2_EBADARGS;

    if ((NULL != instObj) && (NULL != hObj) && (NULL != eePrms))
    {
        Fvid2Utils_memcpy(&hObj->eePrms, eePrms, sizeof(Sde_ErrEventParams));
        status = FVID2_SOK;
    }

    return (status);
}

static void Vhwa_sdeCalOutTrParams(Vhwa_M2mSdeHandleObj *hObj)
{
    Vhwa_M2mSdeOutChPrms *outChPrms = NULL;
    uint32_t numVerBlk,numBlkInRow;
    uint32_t outBuffPitch, numOfMidBlkLeft;
    uint32_t blkRwDone, trIdx, tNumOfBuff, rowDone;
    uint32_t blkLeftInRow, sl2BufIdx, sl2BufLeft;

    GT_assert(VhwaSdeTrace, (NULL != hObj));

    outBuffPitch = hObj->sdePrms.inOutImgFmt[SDE_OUTPUT].pitch[0U];
    outChPrms = &hObj->chPrms.outPrams;
    tNumOfBuff = hObj->sl2Prms.sl2OutBuffDepth;
    trIdx = 0;
    blkRwDone = 0;

    /* Calculate Number of bloacks and TR's for the output */
    numVerBlk = (hObj->sdePrms.sdeCfg.height/8U) - 1U;

    numBlkInRow = hObj->sdePrms.sdeCfg.width / SDE_OUTPUT_BLOCK_WIDTH_IN_PIX;

    if((hObj->sdePrms.sdeCfg.width % SDE_OUTPUT_BLOCK_WIDTH_IN_PIX) != 0u)
    {
        numBlkInRow++;
    }

    outChPrms->numBlkInRow = numBlkInRow;
    outChPrms->numVerBlk = numVerBlk;

    /* Calculate TR params for first block row */
    outChPrms->sicnt1[trIdx] = SDE_OUT_FIRST_LAST_BLK_HEIGHT;
    outChPrms->sicnt2[trIdx] = tNumOfBuff;
    outChPrms->sicnt3[trIdx] = numBlkInRow/tNumOfBuff;
    if((outChPrms->sicnt3[trIdx] * tNumOfBuff) != numBlkInRow)
    {
        outChPrms->sicnt3[trIdx]++;
    }
    outChPrms->dicnt2[trIdx] = numBlkInRow;
    outChPrms->dicnt3[trIdx] = 1U;

    outChPrms->sl2AddrOff[trIdx] = 0;
    outChPrms->bufAddrOff[trIdx] = 0;

    sl2BufIdx = numBlkInRow % tNumOfBuff;
    trIdx++;
    blkRwDone++;
    rowDone = SDE_OUT_FIRST_LAST_BLK_HEIGHT;

    /* Calculate TR params for middle block rows */
    while((sl2BufIdx > 0u) && (blkRwDone < (numVerBlk - 1u)))
    {
        sl2BufLeft = tNumOfBuff - sl2BufIdx;
        /* The TR source address is not aligned with the SL2 output
         * Buffer index split TR into 2 TR's */
        /* TR to transfer remaining blocks to align the SL2 buff index with
         * source buffer */
        outChPrms->sicnt1[trIdx] = SDE_OUT_MIDDLE_BLK_HEIGHT;
        outChPrms->sicnt2[trIdx] = sl2BufLeft;
        if(outChPrms->sicnt2[trIdx] > numBlkInRow)
        {
           outChPrms->sicnt2[trIdx] = numBlkInRow;
        }
        outChPrms->sicnt3[trIdx] = 1U;

        outChPrms->dicnt2[trIdx] = outChPrms->sicnt2[trIdx];
        outChPrms->dicnt3[trIdx] = 1U;

        outChPrms->sl2AddrOff[trIdx] = SDE_PER_OUT_BUFF_SIZE * sl2BufIdx;
        outChPrms->bufAddrOff[trIdx] = rowDone * outBuffPitch;

        sl2BufIdx = sl2BufLeft - outChPrms->sicnt2[trIdx];
        blkLeftInRow = numBlkInRow - outChPrms->sicnt2[trIdx];
        trIdx++;


        if(blkLeftInRow > 0u)
        {
            /* TR to transfer the rest of the blocks in row */
            outChPrms->sicnt1[trIdx] = SDE_OUT_MIDDLE_BLK_HEIGHT;
            outChPrms->sicnt2[trIdx] = tNumOfBuff;
            outChPrms->sicnt3[trIdx] = blkLeftInRow/tNumOfBuff;
            if((outChPrms->sicnt3[trIdx] * tNumOfBuff) != blkLeftInRow)
            {
                outChPrms->sicnt3[trIdx]++;
            }
            outChPrms->dicnt2[trIdx] = blkLeftInRow;
            outChPrms->dicnt3[trIdx] = 1U;

            outChPrms->sl2AddrOff[trIdx] = 0U;
            outChPrms->bufAddrOff[trIdx] = (rowDone * outBuffPitch) +
                            (sl2BufLeft * SDE_OUTPUT_BLOCK_WIDTH_IN_BYTE);

            sl2BufIdx = blkLeftInRow % tNumOfBuff;
            trIdx++;
        }
        blkRwDone++;
        rowDone += SDE_OUT_MIDDLE_BLK_HEIGHT;
    }

    /* Transfer reset of the middle rows blocks in one TR */
    if(blkRwDone < (numVerBlk - 1u))
    {
        numOfMidBlkLeft = (numBlkInRow * (numVerBlk - blkRwDone - 1u));
        outChPrms->sicnt1[trIdx] = SDE_OUT_MIDDLE_BLK_HEIGHT;
        outChPrms->sicnt2[trIdx] = tNumOfBuff;
        outChPrms->sicnt3[trIdx] = numOfMidBlkLeft/tNumOfBuff;
        if((outChPrms->sicnt3[trIdx] * tNumOfBuff) != numOfMidBlkLeft)
        {
            outChPrms->sicnt3[trIdx]++;
        }
        outChPrms->dicnt2[trIdx] = numBlkInRow;
        outChPrms->dicnt3[trIdx] = (numVerBlk - blkRwDone - 1u);

        outChPrms->sl2AddrOff[trIdx] = 0;
        outChPrms->bufAddrOff[trIdx] = outBuffPitch * rowDone;

        sl2BufIdx = numOfMidBlkLeft % tNumOfBuff;
        rowDone += SDE_OUT_MIDDLE_BLK_HEIGHT * (numVerBlk - blkRwDone - 1u);
        trIdx++;
    }

    /* Calculate TR params for last block row */
    if(sl2BufIdx > 0u)
    {
        sl2BufLeft = tNumOfBuff - sl2BufIdx;
        /* The TR source address is not aligned with the SL2 output
         * Buffer index split TR into 2 TR's */
        /* TR to transfer remaining blocks to align the SL2 buff index with
         * source buffer */
        outChPrms->sicnt1[trIdx] = SDE_OUT_FIRST_LAST_BLK_HEIGHT;
        outChPrms->sicnt2[trIdx] = sl2BufLeft;
        if(outChPrms->sicnt2[trIdx] > numBlkInRow)
        {
           outChPrms->sicnt2[trIdx] = numBlkInRow;
        }
        outChPrms->sicnt3[trIdx] = 1U;

        outChPrms->dicnt2[trIdx] = outChPrms->sicnt2[trIdx];
        outChPrms->dicnt3[trIdx] = 1U;

        outChPrms->sl2AddrOff[trIdx] = SDE_PER_OUT_BUFF_SIZE * sl2BufIdx;
        outChPrms->bufAddrOff[trIdx] = rowDone * outBuffPitch;

        blkLeftInRow = numBlkInRow - outChPrms->sicnt2[trIdx];
        trIdx++;
    }
    else
    {
        blkLeftInRow = numBlkInRow;
        sl2BufLeft = 0;
    }
    if(blkLeftInRow > 0u)
    {
        /* TR to transfer the rest of the blocks in row */
        outChPrms->sicnt1[trIdx] = SDE_OUT_FIRST_LAST_BLK_HEIGHT;
        outChPrms->sicnt2[trIdx] = tNumOfBuff;
        outChPrms->sicnt3[trIdx] = blkLeftInRow/tNumOfBuff;
        if((outChPrms->sicnt3[trIdx] * tNumOfBuff) != blkLeftInRow)
        {
            outChPrms->sicnt3[trIdx]++;
        }
        outChPrms->dicnt2[trIdx] = blkLeftInRow;
        outChPrms->dicnt3[trIdx] = 1U;

        outChPrms->sl2AddrOff[trIdx] = 0U;
        outChPrms->bufAddrOff[trIdx] = (rowDone * outBuffPitch) +
                            (sl2BufLeft * SDE_OUTPUT_BLOCK_WIDTH_IN_BYTE);

        trIdx++;
    }

    outChPrms->numTr = trIdx;
}

static Vhwa_M2mSdeHandleObj *Vhwa_sdeAllocHandleObj(
                                    const Vhwa_M2mSdeInstObj *instObj)
{
    uint32_t cnt;
    Vhwa_M2mSdeHandleObj *hObj = NULL;

    if (NULL != instObj)
    {
        for (cnt = 0U; cnt < VHWA_M2M_SDE_MAX_HANDLES; cnt ++)
        {
            if ((uint32_t)UFALSE == gM2mSdeHandleObj[cnt].isUsed)
            {
                hObj = &gM2mSdeHandleObj[cnt];
                Fvid2Utils_memset(hObj, 0x0, sizeof(Vhwa_M2mSdeHandleObj));
                gM2mSdeHandleObj[cnt].isUsed = (uint32_t)UTRUE;
                hObj->hIdx = cnt;
                break;
            }
        }
    }
    return (hObj);
}

static void Vhwa_sdeFreeHandleObj(Vhwa_M2mSdeHandleObj *hObj)
{
    uint32_t cnt;

    if (NULL != hObj)
    {
        for (cnt = 0U; cnt < VHWA_M2M_SDE_MAX_HANDLES; cnt ++)
        {
            if (hObj == &gM2mSdeHandleObj[cnt])
            {
                hObj->isUsed = (uint32_t)UFALSE;;
                break;
            }
        }
    }
}

static void Vhwa_sdeSetLseCfg(Vhwa_M2mSdeHandleObj *hObj,
                              Vhwa_M2mSdePrms *sdePrms)
{
    Fvid2_ColorCompStorageFmt ccsFormat;
    CSL_LseConfig *lseCfg = NULL;
    Sde_Config    *sdeCfg = NULL;
    uint32_t      lineOffset;

    /* NULL Pointer check */
    GT_assert(VhwaSdeTrace, (NULL != hObj));
    GT_assert(VhwaSdeTrace, (NULL != sdePrms));

    lseCfg = &hObj->lseCfg;
    ccsFormat = sdePrms->inOutImgFmt[SDE_INPUT_BASE_IMG].ccsFormat;
    sdeCfg = &sdePrms->sdeCfg;

    /* Initialize LSE configuration with the default values */
    CSL_lseConfigInit(lseCfg);

    lineOffset = Vhwa_calcHorzSizeInBytes(sdeCfg->width, ccsFormat);

    lineOffset = ((lineOffset + 63u) & 0xFFFFFFC0u);

    lseCfg->numInCh = 1;
    lseCfg->numOutCh = 2;

    lseCfg->enableFixedArbMode = 1;

    /* Reference image */
    lseCfg->inChCfg[0U].enable = (uint32_t)UTRUE;
    lseCfg->inChCfg[0U].frameWidth = sdeCfg->width;

    lseCfg->inChCfg[0U].frameHeight = sdeCfg->height;

    lseCfg->inChCfg[0U].enableAddrIncrBy2 = (uint32_t)UFALSE;

    lseCfg->inChCfg[0U].ccsf = ccsFormat;
    lseCfg->inChCfg[0U].startOffset = 0U;

    lseCfg->inChCfg[0U].lineOffset = lineOffset;
    lseCfg->inChCfg[0U].circBufSize = SDE_FOCO_SL2_BUFF_DEPTH;
    lseCfg->inChCfg[0U].bufAddr[0U] =
                        (uint32_t)hObj->sl2Prms.sl2Addr[SDE_INPUT_FOCO_REF_IMG];
    lseCfg->inChCfg[0U].bufAddr[1U] =
                        (uint32_t)hObj->sl2Prms.sl2Addr[SDE_INPUT_FOCO_BASE_IMG];
    lseCfg->inChCfg[0U].numInBuff = 2U;

    /* Assumes No padding required when core is bypassed */
    lseCfg->inChCfg[0U].knTopPadding = 1U;
    lseCfg->inChCfg[0U].knBottomPadding = 2U;
    lseCfg->inChCfg[0U].knLineOffset = 1U;
    lseCfg->inChCfg[0U].knHeight = 4U;

    lseCfg->outChCfg[0U].enable = (uint32_t)UTRUE;
    lseCfg->outChCfg[0U].ccsf = FVID2_CCSF_BITS12_PACKED;
    lseCfg->outChCfg[0U].lineOffset =
                                hObj->sl2Prms.sl2Pitch[SDE_INPUT_REFERENCE_IMG];
    lseCfg->outChCfg[0U].circBufSize = SDE_SL2_REF_BASE_IMG_CIR_BUFF_SIZE;
    lseCfg->outChCfg[0U].bufAddr =
                        (uint32_t)hObj->sl2Prms.sl2Addr[SDE_INPUT_REFERENCE_IMG];

    /* Current image */
    lseCfg->outChCfg[1U].enable = (uint32_t)UTRUE;
    lseCfg->outChCfg[1U].ccsf = FVID2_CCSF_BITS12_PACKED;
    lseCfg->outChCfg[1U].lineOffset = hObj->sl2Prms.sl2Pitch[SDE_INPUT_BASE_IMG];
    lseCfg->outChCfg[1U].circBufSize = SDE_SL2_REF_BASE_IMG_CIR_BUFF_SIZE;
    lseCfg->outChCfg[1U].bufAddr =
                        (uint32_t)hObj->sl2Prms.sl2Addr[SDE_INPUT_BASE_IMG];
}

static void Vhwa_sdeSetHtsCfg(Vhwa_M2mSdeInstObj *instObj,
    Vhwa_M2mSdeHandleObj *hObj, Vhwa_M2mSdePrms *sdePrms)
{
    uint32_t psMaxCnt;
    CSL_DmpacHtsSchConfig *htsCfg = NULL;
    CSL_DmpacHtsSchConfig *focoHtsCfg = NULL;
    Sde_Config *sdeCfg = NULL;

    /* NULL Pointer check */
    GT_assert(VhwaSdeTrace, (NULL != instObj));
    GT_assert(VhwaSdeTrace, (NULL != hObj));
    GT_assert(VhwaSdeTrace, (NULL != sdePrms));

    htsCfg = &hObj->htsCfg;
    sdeCfg = &sdePrms->sdeCfg;
    focoHtsCfg = &hObj->focoHtsCfg;

    CSL_dmpacHtsSchConfigInit(htsCfg);

    htsCfg->schId = CSL_HTS_HWA_SCH_SDE;

    htsCfg->pipeline = instObj->pipeline;
    htsCfg->enableStream = (uint32_t)UFALSE;
    htsCfg->enableHop = (uint32_t)UFALSE;
    htsCfg->enableWdTimer = (uint32_t)UFALSE;

    htsCfg->enableBwLimit = UFALSE;

    /* Default configuration for FOCO HTS */
    CSL_dmpacHtsSchConfigInit(focoHtsCfg);
    focoHtsCfg->schId = CSL_HTS_HWA_SCH_FOCO2;

    if(hObj->isFocoUsed == UTRUE)
    {
        focoHtsCfg->pipeline = instObj->pipeline;
        focoHtsCfg->enableStream = (uint32_t)UFALSE;
        focoHtsCfg->enableHop = (uint32_t)UFALSE;
        focoHtsCfg->enableWdTimer = (uint32_t)UFALSE;
        focoHtsCfg->enableBwLimit = (uint32_t)UFALSE;

        /* Reference frame config */
        focoHtsCfg->consCfg[HTS_SDE_REFE_FRAME_CH_IDX].enable = (uint32_t)UTRUE;
        focoHtsCfg->consCfg[HTS_SDE_REFE_FRAME_CH_IDX].prodId =
                                            CSL_HTS_PROD_IDX_UDMA;

        focoHtsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].enable = (uint32_t)UTRUE;
        focoHtsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].dmaChNum =
                    Udma_chGetNum(instObj->utcChHndl[SDE_INPUT_REFERENCE_IMG]);
        focoHtsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].pipeline =
                                            instObj->pipeline;
        focoHtsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].consId =
                                            CSL_HTS_CONS_IDX_UDMA;
        /* Default mapping for DMA */

        focoHtsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].threshold = 1;
        focoHtsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].cntPreLoad = 0;

        focoHtsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].cntPostLoad = 0;

        focoHtsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].enableHop =
                                                (uint32_t)UTRUE;
        focoHtsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].numHop =
                                                sdeCfg->height;
        focoHtsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].countDec = 1U;
        focoHtsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].depth =
                                                SDE_FOCO_SL2_BUFF_DEPTH;

        focoHtsCfg->prodCfg[HTS_SDE_REFE_FRAME_CH_IDX].enable = (uint32_t)UTRUE;
        focoHtsCfg->prodCfg[HTS_SDE_REFE_FRAME_CH_IDX].consId =
                                                CSL_HTS_CONS_IDX_SDE_REF;

        focoHtsCfg->prodCfg[HTS_SDE_REFE_FRAME_CH_IDX].threshold =
                                        SDE_SL2_REF_BASE_IMG_BUFF_HEIGHT * 2U;
        focoHtsCfg->prodCfg[HTS_SDE_REFE_FRAME_CH_IDX].cntPreLoad = 0;
        focoHtsCfg->prodCfg[HTS_SDE_REFE_FRAME_CH_IDX].cntPostLoad = 0;


        focoHtsCfg->prodCfg[HTS_SDE_REFE_FRAME_CH_IDX].depth =
                                        SDE_SL2_REF_BASE_IMG_BUFF_HEIGHT * 3U;
        focoHtsCfg->prodCfg[HTS_SDE_REFE_FRAME_CH_IDX].countDec =
                                        SDE_SL2_REF_BASE_IMG_BUFF_HEIGHT;

        /* Enable PA */
        focoHtsCfg->prodCfg[HTS_SDE_REFE_FRAME_CH_IDX].paCfg.enable = (uint32_t)UTRUE;
        focoHtsCfg->prodCfg[HTS_SDE_REFE_FRAME_CH_IDX].paCfg.enableBufCtrl = 0U;

        psMaxCnt = sdeCfg->width/SDE_PROCESS_BLOCK_WIDTH;
        if((SDE_PROCESS_BLOCK_WIDTH * psMaxCnt) != sdeCfg->width)
        {
            psMaxCnt++;
        }
        focoHtsCfg->prodCfg[HTS_SDE_REFE_FRAME_CH_IDX].paCfg.psMaxCnt = psMaxCnt;
        focoHtsCfg->prodCfg[HTS_SDE_REFE_FRAME_CH_IDX].paCfg.csMaxCnt =
                                        SDE_SL2_REF_BASE_IMG_BUFF_HEIGHT * 2U;
        focoHtsCfg->prodCfg[HTS_SDE_REFE_FRAME_CH_IDX].paCfg.enableDecCtrl = 1U;
        focoHtsCfg->prodCfg[HTS_SDE_REFE_FRAME_CH_IDX].paCfg.countDec = 1U;


        /* Base frame config */
        focoHtsCfg->consCfg[HTS_SDE_BASE_FRAME_CH_IDX].enable = (uint32_t)UTRUE;
        focoHtsCfg->consCfg[HTS_SDE_BASE_FRAME_CH_IDX].prodId =
                                            CSL_HTS_PROD_IDX_UDMA;

        focoHtsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].enable = (uint32_t)UTRUE;
        focoHtsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].dmaChNum =
                        Udma_chGetNum(instObj->utcChHndl[SDE_INPUT_BASE_IMG]);
        focoHtsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].pipeline =
                                            instObj->pipeline;
        focoHtsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].consId =
                                            CSL_HTS_CONS_IDX_UDMA;
        /* Default mapping for DMA */

        focoHtsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].threshold = 1;
        focoHtsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].cntPreLoad = 0;
        focoHtsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].cntPostLoad = 0;

        focoHtsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].enableHop = (uint32_t)UTRUE;
        focoHtsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].numHop = sdeCfg->height;
        focoHtsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].countDec = 1U;
        focoHtsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].depth =
                                                    SDE_FOCO_SL2_BUFF_DEPTH;


        focoHtsCfg->prodCfg[HTS_SDE_BASE_FRAME_CH_IDX].enable = (uint32_t)UTRUE;
        focoHtsCfg->prodCfg[HTS_SDE_BASE_FRAME_CH_IDX].consId =
                                            CSL_HTS_CONS_IDX_SDE_BASE;

        focoHtsCfg->prodCfg[HTS_SDE_BASE_FRAME_CH_IDX].threshold =
                                        SDE_SL2_REF_BASE_IMG_BUFF_HEIGHT * 2U;

        focoHtsCfg->prodCfg[HTS_SDE_BASE_FRAME_CH_IDX].cntPreLoad = 0;
        focoHtsCfg->prodCfg[HTS_SDE_BASE_FRAME_CH_IDX].cntPostLoad = 0;


        focoHtsCfg->prodCfg[HTS_SDE_BASE_FRAME_CH_IDX].depth =
                                        SDE_SL2_REF_BASE_IMG_BUFF_HEIGHT * 3U;
        focoHtsCfg->prodCfg[HTS_SDE_BASE_FRAME_CH_IDX].countDec =
                                        SDE_SL2_REF_BASE_IMG_BUFF_HEIGHT;

        /* Enable PA */
        focoHtsCfg->prodCfg[HTS_SDE_BASE_FRAME_CH_IDX].paCfg.enable = (uint32_t)UTRUE;
        focoHtsCfg->prodCfg[HTS_SDE_BASE_FRAME_CH_IDX].paCfg.enableBufCtrl = 0U;

        focoHtsCfg->prodCfg[HTS_SDE_BASE_FRAME_CH_IDX].paCfg.psMaxCnt = psMaxCnt;
        focoHtsCfg->prodCfg[HTS_SDE_BASE_FRAME_CH_IDX].paCfg.csMaxCnt =
                                        SDE_SL2_REF_BASE_IMG_BUFF_HEIGHT * 2U;
        focoHtsCfg->prodCfg[HTS_SDE_BASE_FRAME_CH_IDX].paCfg.enableDecCtrl = 1U;
        focoHtsCfg->prodCfg[HTS_SDE_BASE_FRAME_CH_IDX].paCfg.countDec = 1U;


        /* HWA0 Consumer control for RFGW */
        htsCfg->consCfg[HTS_SDE_REFE_FRAME_CH_IDX].enable = (uint32_t)UTRUE;
        htsCfg->consCfg[HTS_SDE_REFE_FRAME_CH_IDX].prodId = CSL_HTS_PROD_IDX_FOCO1_0;

        /* HWA0 Consumer control for CFGW */
        htsCfg->consCfg[HTS_SDE_BASE_FRAME_CH_IDX].enable = (uint32_t)UTRUE;
        htsCfg->consCfg[HTS_SDE_BASE_FRAME_CH_IDX].prodId = CSL_HTS_PROD_IDX_FOCO1_1;

    }
    else
    {
        /* Reference Frame Config */
        htsCfg->consCfg[HTS_SDE_REFE_FRAME_CH_IDX].enable = (uint32_t)UTRUE;
        htsCfg->consCfg[HTS_SDE_REFE_FRAME_CH_IDX].prodId = CSL_HTS_PROD_IDX_UDMA;

        htsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].enable = (uint32_t)UTRUE;
        htsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].dmaChNum =
                    Udma_chGetNum(instObj->utcChHndl[SDE_INPUT_REFERENCE_IMG]);
        htsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].pipeline = instObj->pipeline;
        htsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].consId = CSL_HTS_CONS_IDX_UDMA;
        /* Default mapping for DMA */


        htsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].threshold = 2U;
        htsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].cntPreLoad = 0;
        htsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].cntPostLoad = 0;

        htsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].enableHop = (uint32_t)UTRUE;
        htsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].numHop = sdeCfg->height/SDE_SL2_REF_BASE_IMG_BUFF_HEIGHT;
        htsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].countDec = 1U;
        htsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].depth = SDE_SL2_REF_BASE_IMG_BUFF_COUNT;

        /* Enable PA */
        htsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].paCfg.enable = (uint32_t)UTRUE;
        htsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].paCfg.enableBufCtrl = 0U;

        psMaxCnt = sdeCfg->width/SDE_PROCESS_BLOCK_WIDTH;
        if((SDE_PROCESS_BLOCK_WIDTH * psMaxCnt) != sdeCfg->width)
        {
            psMaxCnt++;
        }
        htsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].paCfg.psMaxCnt = psMaxCnt;
        htsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].paCfg.csMaxCnt = 2U;
        htsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].paCfg.enableDecCtrl = 1U;
        htsCfg->dmaProdCfg[HTS_SDE_REFE_FRAME_CH_IDX].paCfg.countDec = 1U;

        /* Base Frame Config */
        htsCfg->consCfg[HTS_SDE_BASE_FRAME_CH_IDX].enable = (uint32_t)UTRUE;
        htsCfg->consCfg[HTS_SDE_BASE_FRAME_CH_IDX].prodId = CSL_HTS_PROD_IDX_UDMA;

        htsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].enable = (uint32_t)UTRUE;
        htsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].dmaChNum =
                        Udma_chGetNum(instObj->utcChHndl[SDE_INPUT_BASE_IMG]);
        htsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].pipeline = instObj->pipeline;
        htsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].consId = CSL_HTS_CONS_IDX_UDMA;
        /* Default mapping for DMA */

        htsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].threshold = 2;
        htsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].cntPreLoad = 0;
        htsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].cntPostLoad = 0;

        htsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].enableHop = (uint32_t)UTRUE;
        htsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].numHop = sdeCfg->height/SDE_SL2_REF_BASE_IMG_BUFF_HEIGHT;
        htsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].countDec = 1U;
        htsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].depth = SDE_SL2_REF_BASE_IMG_BUFF_COUNT;

        /* Enable PA */
        htsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].paCfg.enable = (uint32_t)UTRUE;
        htsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].paCfg.enableBufCtrl = 0U;
        htsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].paCfg.psMaxCnt = psMaxCnt;
        htsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].paCfg.csMaxCnt = 2U;
        htsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].paCfg.enableDecCtrl = 1U;
        htsCfg->dmaProdCfg[HTS_SDE_BASE_FRAME_CH_IDX].paCfg.countDec = 1U;
    }

    /* Output flow config */
    htsCfg->prodCfg[HTS_SDE_OUT_CH_IDX].enable = (uint32_t)UTRUE;
    htsCfg->prodCfg[HTS_SDE_OUT_CH_IDX].consId = CSL_HTS_CONS_IDX_UDMA;

    htsCfg->prodCfg[HTS_SDE_OUT_CH_IDX].threshold = 1u;
    htsCfg->prodCfg[HTS_SDE_OUT_CH_IDX].cntPreLoad = 0u;
    htsCfg->prodCfg[HTS_SDE_OUT_CH_IDX].cntPostLoad = 0u;

    /* Output 2 lines per dma transfer*/
    htsCfg->prodCfg[HTS_SDE_OUT_CH_IDX].depth = hObj->sl2Prms.sl2OutBuffDepth;
    htsCfg->prodCfg[HTS_SDE_OUT_CH_IDX].countDec = 1U;

    htsCfg->dmaConsCfg[HTS_SDE_OUT_CH_IDX].enable = (uint32_t)UTRUE;
    htsCfg->dmaConsCfg[HTS_SDE_OUT_CH_IDX].dmaChNum =
                                Udma_chGetNum(instObj->utcChHndl[SDE_OUTPUT]);
    htsCfg->dmaConsCfg[HTS_SDE_OUT_CH_IDX].pipeline = instObj->pipeline;
    htsCfg->dmaConsCfg[HTS_SDE_OUT_CH_IDX].enableStream = (uint32_t)UFALSE;
    htsCfg->dmaConsCfg[HTS_SDE_OUT_CH_IDX].prodId = CSL_HTS_PROD_IDX_UDMA;
}

static void Vhwa_sdeSetFocoParams(Vhwa_M2mSdeHandleObj *hObj,
                                     const Vhwa_M2mSdePrms *sdePrms)
{
    Dmpac_FocoConfig *focoCfg = &hObj->focoCfg;
    Sde_Config       *sdeCfg = &hObj->sdePrms.sdeCfg;

    /* NULL Pointer check */
    GT_assert(VhwaSdeTrace, (NULL != hObj));
    GT_assert(VhwaSdeTrace, (NULL != sdePrms));

    if(sdePrms->focoPrms.shiftM1 != 0u)
    {
        focoCfg->shiftEnable = UTRUE;
        focoCfg->shiftM1 = sdePrms->focoPrms.shiftM1 - 1u;
    }
    else
    {
        focoCfg->shiftEnable = UFALSE;
        focoCfg->shiftM1 = 0U;
    }
    focoCfg->dir = sdePrms->focoPrms.dir;
    focoCfg->round = sdePrms->focoPrms.round;
    focoCfg->channelEnable = UTRUE;
    focoCfg->mask = 0xffff;
    focoCfg->trig = sdeCfg->height;

    focoCfg->preload = 0;
    /* Multiple of 2 */
    focoCfg->postload = 0;

}

static int32_t Vhwa_sdeCheckCfg(const Vhwa_M2mSdeInstObj *instObj,
                                Vhwa_M2mSdeHandleObj *hObj,
                                const Vhwa_M2mSdePrms *sdePrms)
{
    int32_t status = FVID2_SOK;

    if ((NULL == instObj) || (NULL == hObj) || (NULL == sdePrms))
    {
        status = FVID2_EINVALID_PARAMS;
    }
    else
    {
        if((sdePrms->sdeCfg.width < SDE_MIN_IMAGE_WIDTH) ||
           (sdePrms->sdeCfg.height < SDE_MIN_IMAGE_HEIGHT) ||
           (sdePrms->sdeCfg.width > SDE_MAX_IMAGE_WIDTH) ||
           (sdePrms->sdeCfg.height > SDE_MAX_IMAGE_HEIGHT) ||
           ((sdePrms->sdeCfg.width & 0xFu) != 0u) ||
           ((sdePrms->sdeCfg.height & 0xFu) != 0u))
        {
            status = FVID2_EBADARGS;
        }

        if((sdePrms->sdeCfg.minDisparity != 0u) &&
           (sdePrms->sdeCfg.minDisparity != 1u))
        {
            status = FVID2_EBADARGS;

        }

        if((sdePrms->sdeCfg.searchRange != SDE_SR_64) &&
           (sdePrms->sdeCfg.searchRange != SDE_SR_128) &&
           (sdePrms->sdeCfg.searchRange != SDE_SR_192))
        {
            status = FVID2_EBADARGS;

        }

        if(sdePrms->inOutImgFmt[SDE_INPUT_BASE_IMG].ccsFormat == FVID2_CCSF_BITS12_PACKED)
        {
            hObj->isFocoUsed = UFALSE;
        }
        else if((sdePrms->inOutImgFmt[SDE_INPUT_BASE_IMG].ccsFormat ==
                                    FVID2_CCSF_BITS12_UNPACKED16) ||
                (sdePrms->inOutImgFmt[SDE_INPUT_BASE_IMG].ccsFormat ==
                                    FVID2_CCSF_BITS8_PACKED) ||
                (sdePrms->inOutImgFmt[SDE_INPUT_BASE_IMG].ccsFormat ==
                                    FVID2_CCSF_BITS12_UNPACKED16_MSB_ALIGNED))
        {
            hObj->isFocoUsed = UTRUE;
        }
        else
        {
            hObj->isFocoUsed = UFALSE;
            status = FVID2_EBADARGS;
        }
    }

    return status;
}

static int32_t Vhwa_sdeSetSl2Prms(const Vhwa_M2mSdeInstObj *instObj,
                                  Vhwa_M2mSdeHandleObj *hObj,
                                  Vhwa_M2mSdePrms *sdePrms)
{
    int32_t status = FVID2_SOK;
    uint32_t pitchSl2;
    uint32_t imgMem, outBuffMem, rcBuffMem, focoBuffMem, tMemReq;
    uint64_t sl2StartAddr;
    Sde_Config         *sdeCfg = NULL;
    Vhwa_M2mSdeSl2Prms *sl2Prms = NULL;

    GT_assert(VhwaSdeTrace, (NULL != instObj));
    GT_assert(VhwaSdeTrace, (NULL != hObj));
    GT_assert(VhwaSdeTrace, (NULL != sdePrms));

    sdeCfg = &sdePrms->sdeCfg;
    sl2Prms = &hObj->sl2Prms;

    pitchSl2 = Vhwa_calcHorzSizeInBytes(sdeCfg->width,
                                        FVID2_CCSF_BITS12_PACKED);
    pitchSl2 = Vhwa_calcSl2Pitch(pitchSl2);

    imgMem = pitchSl2 * SDE_SL2_REF_BASE_IMG_CIR_BUFF_SIZE;

    /* Set Out buffer Parameters */
    outBuffMem = SDE_PER_OUT_BUFF_SIZE * sl2Prms->sl2OutBuffDepth;

    /* Set Row cost Buffer Parameters */
    rcBuffMem= (sdeCfg->width/8u)*2u*8u*8u;

    if(sdeCfg->searchRange == SDE_SR_192)
    {
        rcBuffMem = rcBuffMem * 3u;
    }
    else if(sdeCfg->searchRange == SDE_SR_128)
    {
        rcBuffMem = rcBuffMem * 2u;
    }
    else
    {
        /* Nothing to do here */
    }

    /* Set FOCO Parameters */
    if(FVID2_CCSF_BITS12_PACKED ==
                            sdePrms->inOutImgFmt[SDE_INPUT_BASE_IMG].ccsFormat)
    {
        focoBuffMem = 0;
    }
    else
    {
        focoBuffMem = Vhwa_calcHorzSizeInBytes(sdeCfg->width,
                           sdePrms->inOutImgFmt[SDE_INPUT_BASE_IMG].ccsFormat);
        focoBuffMem = Vhwa_calcSl2Pitch(focoBuffMem);
        focoBuffMem = focoBuffMem * 2u;
    }

    tMemReq = (imgMem*2u) + outBuffMem + rcBuffMem + (focoBuffMem*2u);

    if(tMemReq > instObj->sl2Size)
    {
        status = FVID2_EALLOC;
    }
    else
    {
        sl2StartAddr = instObj->sl2StartAddr;

        sl2Prms->sl2Addr[SDE_INPUT_REFERENCE_IMG] = sl2StartAddr;
        sl2Prms->sl2Pitch[SDE_INPUT_REFERENCE_IMG] = pitchSl2;
        sl2StartAddr = sl2StartAddr + imgMem;
        sl2Prms->sl2Addr[SDE_INPUT_BASE_IMG] = sl2StartAddr;
        sl2Prms->sl2Pitch[SDE_INPUT_BASE_IMG] = pitchSl2;
        sl2StartAddr = sl2StartAddr + imgMem;

        sl2Prms->sl2Addr[SDE_OUTPUT] = sl2StartAddr;
        sl2StartAddr = sl2StartAddr + outBuffMem;

        sl2Prms->sl2RwCostBuffAddr = sl2StartAddr;
        sl2StartAddr = sl2StartAddr + rcBuffMem;

        if(focoBuffMem > 0u)
        {
            sl2Prms->sl2Addr[SDE_INPUT_FOCO_REF_IMG] = sl2StartAddr;
            sl2StartAddr = sl2StartAddr + focoBuffMem;
            sl2Prms->sl2Addr[SDE_INPUT_FOCO_BASE_IMG] = sl2StartAddr;
        }
        else
        {
            sl2Prms->sl2Addr[SDE_INPUT_FOCO_REF_IMG] = 0;
            sl2Prms->sl2Addr[SDE_INPUT_FOCO_BASE_IMG] = 0;
        }
    }

    return (status);
}

static int32_t Vhwa_sdeSetParams(Vhwa_M2mSdeInstObj *instObj,
    Vhwa_M2mSdeHandleObj *hObj, Vhwa_M2mSdePrms *sdePrms)
{
    int32_t status = FVID2_EBADARGS;

    if ((NULL != instObj) && (NULL != hObj) && (NULL != sdePrms))
    {
        status = Vhwa_sdeCheckCfg(instObj, hObj, sdePrms);

        if(FVID2_SOK == status)
        {
            status = Vhwa_sdeSetSl2Prms(instObj, hObj, sdePrms);
        }

        if (FVID2_SOK == status)
        {
            Fvid2Utils_memcpy(&hObj->sdePrms, sdePrms, sizeof(Vhwa_M2mSdePrms));

            /* Set SDE buff params */
            Vhwa_sdeSetBuffParams(hObj);

            /* Calculate TR information for output channel */
            Vhwa_sdeCalOutTrParams(hObj);

            /* HTS config */
            Vhwa_sdeSetHtsCfg(instObj, hObj, &hObj->sdePrms);

            if(hObj->isFocoUsed == UTRUE)
            {
                /* LSE config */
                Vhwa_sdeSetLseCfg(hObj, &hObj->sdePrms);

                Vhwa_sdeSetFocoParams(hObj, &hObj->sdePrms);
            }

            /* Setup TR Descriptor */
            Vhwa_m2mSdeSetTrDesc(instObj, hObj);
        }
        else
        {
            /* Reset HTS config based on local correct LDC config */
            Vhwa_sdeSetHtsCfg(instObj, hObj, &hObj->sdePrms);
            /* Reset LSE config based on local correct LDC config */
            Vhwa_sdeSetLseCfg(hObj, &hObj->sdePrms);
        }
    }

    return (status);
}

static void Vhwa_sdeSetBuffParams(Vhwa_M2mSdeHandleObj *hObj)
{
    CSL_SdeBufParams    *sdeBuffParams;

    sdeBuffParams = &hObj->sdeBuffPrms;

    sdeBuffParams->baseImageAddress =
                    (uint32_t)hObj->sl2Prms.sl2Addr[SDE_INPUT_REFERENCE_IMG];
    sdeBuffParams->baseImageWidth =
                    (uint32_t)hObj->sl2Prms.sl2Pitch[SDE_INPUT_REFERENCE_IMG];
    sdeBuffParams->refImageAddress =
                    (uint32_t)hObj->sl2Prms.sl2Addr[SDE_INPUT_BASE_IMG];
    sdeBuffParams->refImageWidth =
                    (uint32_t)hObj->sl2Prms.sl2Pitch[SDE_INPUT_BASE_IMG];
    sdeBuffParams->disparityAddress =
                    (uint32_t)hObj->sl2Prms.sl2Addr[SDE_OUTPUT];
    sdeBuffParams->numOutputBufs = (uint32_t)hObj->sl2Prms.sl2OutBuffDepth;
    sdeBuffParams->rowCostBufferAddress =
                    (uint32_t)hObj->sl2Prms.sl2RwCostBuffAddr;
}

static int32_t Vhwa_sdeEnableFoco(const Vhwa_M2mSdeInstObj *instObj,
                                  const Vhwa_M2mSdeHandleObj *hObj)
{
    int32_t status;
    CSL_dmpac_foco_coreRegs *focoRegs = instObj->socInfo.dmpacFocoRegs;

    /* Enable channel 0 for Reference Frame */
    status = CSL_dmpacFocoSetConfig(focoRegs, DMPAC_FOCO_CHANNEL_0,
                                    &hObj->focoCfg);

    /* Enable channel 1 for Current Frame */
    if(CSL_PASS == status)
    {
        status = CSL_dmpacFocoSetConfig(focoRegs, DMPAC_FOCO_CHANNEL_1,
                                        &hObj->focoCfg);
    }
    return status;
}

static int32_t Vhwa_m2mSdeSetConfigInHW(Vhwa_M2mSdeInstObj *instObj,
                                        const Vhwa_M2mSdeQueueObj *qObj)
{
    int32_t               status;
    Vhwa_M2mSdeHandleObj *hObj = NULL;
    Sde_SocInfo          *socInfo = NULL;

    /* Null pointer check */
    GT_assert(VhwaSdeTrace, (NULL != instObj));
    GT_assert(VhwaSdeTrace, (NULL != qObj));
    GT_assert(VhwaSdeTrace, (NULL != qObj->hObj));

    hObj = qObj->hObj;
    socInfo = &instObj->socInfo;

    /* Configure SL2 Buffer parameters */
    status = CSL_sdeSetBufParam(socInfo->sdeRegs, &hObj->sdeBuffPrms);
    if(FVID2_SOK == status)
    {

        status = CSL_sdeSetConfig(socInfo->sdeRegs,
                                        &hObj->sdePrms.sdeCfg);
    }

    CSL_sdeEnablePsa(socInfo->sdeRegs, hObj->createArgs.enablePsa);

    if(hObj->isFocoUsed == UTRUE)
    {
        if (FVID2_SOK == status)
        {
            /* Step-2, Configure HWA Common Wrapper LSE */
            status = Vhwa_sdeEnableFoco(instObj, hObj);
        }

        if (FVID2_SOK == status)
        {
            /* Step-2, Configure HWA Common Wrapper LSE */
            status = CSL_lseSetConfig(socInfo->lseRegs, &hObj->lseCfg);
        }
    }

    if (FVID2_SOK == status)
    {
        /* Configure HTS */
        status = CSL_dmpacHtsSetThreadConfig(socInfo->htsRegs, &hObj->htsCfg);
    }

    if((hObj->isFocoUsed == UTRUE) && (FVID2_SOK == status))
    {
        status = CSL_dmpacHtsSetThreadConfig(socInfo->htsRegs,
                                        &hObj->focoHtsCfg);
    }

    if (FVID2_SOK == status)
    {
        /* Start HTS Scheduler */
        status = CSL_dmpacHtsThreadStart(socInfo->htsRegs, &hObj->htsCfg);

        if((hObj->isFocoUsed == UTRUE) && (FVID2_SOK == status))
        {
            status = CSL_dmpacHtsThreadStart(socInfo->htsRegs, &hObj->focoHtsCfg);
        }
        else
        {
            (void)CSL_dmpacHtsThreadStopAll(socInfo->htsRegs, &hObj->focoHtsCfg);
        }

    }

    return (status);
}

static int32_t Vhwa_sdeSubmitRequest(Vhwa_M2mSdeInstObj *instObj,
                                     Vhwa_M2mSdeQueueObj *qObj)
{
    int32_t status;
    Vhwa_M2mSdeHandleObj *hObj = NULL;

    GT_assert(VhwaSdeTrace, (NULL != instObj));
    GT_assert(VhwaSdeTrace, (NULL != qObj));
    GT_assert(VhwaSdeTrace, (NULL != qObj->hObj));

    hObj = qObj->hObj;

    /* Submit Rings to the Ring Accelerator */
    status = Vhwa_m2mSdeSubmitRing(instObj, hObj);

    if (CSL_PASS == status)
    {
        /* Better to set Active object to this q object, so that if
         * interrupt comes immediately, actQObj would be set..
         * If pipeline start fails, it would be set to NULL.*/
        instObj->actQObj = qObj;

        instObj->totalReqCnt ++;

        #ifndef VHWA_USE_PIPELINE_COMMON_ENABLE
        if (NULL != hObj->createArgs.getTimeStamp)
        {
            hObj->perfNum = hObj->createArgs.getTimeStamp();
        }

        /* Start HTS pipeline */
        status = CSL_dmpacHtsPipelineStart(instObj->socInfo.htsRegs, &hObj->htsCfg);
        #endif
    }

    if (CSL_PASS != status)
    {
        instObj->actQObj = NULL;
        status = FVID2_EFAIL;
    }
    else
    {
        status = FVID2_SOK;
    }

    return (status);
}

static int32_t Vhwa_sdeCreateQueues(Vhwa_M2mSdeInstObj *instObj)
{
    int32_t              status;
    uint32_t             qCnt;
    Vhwa_M2mSdeQueueObj *qObj;

    /* NULL pointer check */
    GT_assert(VhwaSdeTrace, (NULL != instObj));

    instObj->freeQ = NULL;

    /* Create Free queue */
    status = Fvid2Utils_constructQ(&instObj->freeQObj);
    GT_assert(VhwaSdeTrace, (FVID2_SOK == status));

    instObj->freeQ = &instObj->freeQObj;

    /* Initialize and queue the allocate queue object to free Q */
    for(qCnt = 0U; qCnt < VHWA_M2M_SDE_UDMA_RING_ENTRIES; qCnt ++)
    {
        qObj = &instObj->sdeQObj[qCnt];

        Fvid2Utils_memset(qObj, 0x0, sizeof(Vhwa_M2mSdeQueueObj));

        Fvid2Utils_queue(instObj->freeQ, &qObj->qElem, qObj);
    }

    return (status);
}

static void Vhwa_sdeDeleteQueues(Vhwa_M2mSdeInstObj *instObj)
{
    Vhwa_M2mSdeQueueObj *qObj = NULL;

    /* NULL pointer check */
    GT_assert(VhwaSdeTrace, (NULL != instObj));

    if(NULL != instObj->freeQ)
    {
        /* Free-up all the queued free queue objects */
        do
        {
            qObj = (Vhwa_M2mSdeQueueObj *) Fvid2Utils_dequeue(instObj->freeQ);
        } while (NULL != qObj);

        /* Delete the free Q */
        Fvid2Utils_destructQ(instObj->freeQ);
        instObj->freeQ = NULL;
    }
}

static void Vhwa_sdeSetHtsLimitParams(Vhwa_M2mSdeHandleObj *hObj,
                                      const Vhwa_HtsLimiter *htsLimit)
{
    /* Setting directly in the HTS configuration, so that
     * it will be configured from the next request. */
    hObj->htsCfg.enableBwLimit   = htsLimit->enableBwLimit;
    hObj->htsCfg.cycleCnt        = htsLimit->cycleCnt;
    hObj->htsCfg.tokenCnt        = htsLimit->tokenCnt;
}


/**
 * \brief   Returns handle object for the requested handle count.
 *
 * \param   cnt              count.
 *
 * \return  reference to the handle object.
 *
 **/
Vhwa_M2mSdeHandleObj *Vhwa_m2mSdeGetHandleObj(uint32_t cnt)
{
    return &gM2mSdeHandleObj[cnt];
}