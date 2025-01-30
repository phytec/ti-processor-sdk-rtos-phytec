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
 *  \file vhwa_m2mNfApi.c
 *
 *  \brief API Implementation
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/drv/vhwa/src/drv/vhwa_m2mNfPriv.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * \brief   Create Queues, required for storing pending requests
 *
 * \param   instObj             Instance object.
 *
 * \return  FVID2_SOK in case of sucess, Error otherwise
 *
 **/
static int32_t Vhwa_nfCreateQueue(Vhwa_M2mNfInstObj *instObj);

/**
 * \brief   Delete Queues
 *
 * \param   instObj             Instance object.
 *
 * \return  FVID2_SOK in case of sucess, Error otherwise
 *
 **/
static int32_t Vhwa_nfDeleteQueue(Vhwa_M2mNfInstObj *instObj);

/**
 * \brief   Local Function to Allocate Handle Object from the
 *          pool of handle objects.
 *          No protection inside the function, Caller should protect
 *          the function call
 *
 * \param   instObj      Instance object.
 * \param   instId       Instance ID
 *
 * \return  pointer to handle object on success else NULL.
 *
 */
static Vhwa_M2mNfHandleObj *Vhwa_nfAllocHdlObj(Vhwa_M2mNfInstObj *instObj);

/**
 * \brief   Local Function to free Handle Object
 *          No protection inside the function, Caller should protect
 *          the function call
 *
 * \param   instObj      Instance object.
 *
 */
static void Vhwa_nfFreeHandleObj(Vhwa_M2mNfHandleObj *hObj);

/**
 * \brief   Local function to Inititalize the handle object.
 *
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 *
 * \return  FVID2_SOK in case of sucess, Error otherwise
 *
 **/
static void Vhwa_nfInitHandleObj(Vhwa_M2mNfInstObj *instObj,
                                  Vhwa_M2mNfHandleObj *hObj);

/**
 * \brief   Local function to set the NF filter coefficients
 *
 *
 * \param   comObj             Common driver object
 * \param   pCoeffCfg          NF coeff parameters
 *
 * \return  FVID2_SOK in case of sucess, Error otherwise
 *
 **/
static int32_t Vhwa_nfSetFilterTables(Vhwa_M2mNfInstObj *instObj,
                                      const Nf_WgtTableConfig *pCoeffCfg);

/**
 * \brief   Implementation of SET_PARAMS ioctl.
 *          It uses #Vhwa_nfCheckNfCfg to validate the config.
 *          If it is valid, copies the config into handle object
 *          If it is invalid, it reverts LSE/HTS config to known valid config
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 * \param   comObj              Common driver Object
 * \param   nfPrms             NF configuration to be set
 *
 * \return  FVID2_SOK in case of sucess, Error otherwise
 *
 **/
static int32_t Vhwa_nfSetParams(Vhwa_M2mNfInstObj *instObj,
                                 Vhwa_M2mNfHandleObj *hObj,
                                 Vhwa_M2mNfConfig *nfPrms);

/**
 * \brief   Implementation of REGISTER_ERR_CB ioctl.
 *
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 * \param   eePrms              Error Parameters
 *
 * \return  FVID2_SOK in case of sucess, Error otherwise
 *
 **/
static int32_t Vhwa_nfSetEeParams(const Vhwa_M2mNfInstObj *instObj,
    Vhwa_M2mNfHandleObj *hObj, const Nf_ErrEventParams *eePrms);

/**
 * \brief   Based on the given NF config, it initializes HTS configuration.
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 * \param   nfPrms             NF Configuration
 *
 **/
static void Vhwa_nfSetHtsCfg(Vhwa_M2mNfInstObj *instObj,
                              Vhwa_M2mNfHandleObj *hObj);

/**
 * \brief   Based on the given NF config, it initializes LSE configuration.
 *
 * \param   instObj        Instance Object
 * \param   hObj           Handle Object
 * \param   nfPrms        NF Configuration
 *
 **/
static void Vhwa_nfSetLseCfg(const Vhwa_M2mNfInstObj *instObj,
                              Vhwa_M2mNfHandleObj *hObj);

/**
 * \brief   Local function to check the NF configuration
 *
 * \param   instObj             Instance Object
 * \param   nfPrms             NF Configuration
 *
 * \return  FVID2_SOK in case of correct configuration, Error otherwise
 *
 **/
static int32_t Vhwa_nfCheckNfCfg(Vhwa_M2mNfInstObj *instObj,
                                 Vhwa_M2mNfConfig *nfPrms);

/**
 * \brief   Local function to set CSL paramenter for Luma and Chroma based on
 *          NF configuration
 *
 * \param   hObj                Handle Object
 * \param   nfPrms             NF Configuration
 *
 **/
static void Vhwa_nfSetCfgParams(Vhwa_M2mNfHandleObj *hObj,
                                const Vhwa_M2mNfConfig *nfPrms);

/**
 * \brief   Local function to set HTS Bandwidth limiter
 *
 * \param   hObj                Handle Object
 * \param   bwLimit             Bandwidth limiter Configuration
 *
 **/
static void Vhwa_nfSetBwLimit(Vhwa_M2mNfHandleObj *hObj,
                              const Vhwa_HtsLimiter *bwLimit);

/**
 * \brief   Local function to check NF Create params like Driver Id and Instance Id.
 *
 * \param   drvId               Driver ID
 * \param   drvInstId           Instance ID
 *
 *  \return FVID2_SOK if successful, else suitable error code
 **/
static int32_t Vhwa_nfCheckCreatePrms(uint32_t drvId, uint32_t drvInstId);

/* Implementation of FVID2 APIs */
/**
 * \brief   FVID2 Create Function.
 *
 * \param   instObj             Instance object.
 *
 * \return  FVID2 Driver Handle.
 *
 **/
Fdrv_Handle Vhwa_m2mNfCreate(uint32_t drvId, uint32_t drvInstId,
                                     Ptr createPrms, Ptr createStatusArgs,
                                     const Fvid2_DrvCbParams *cbPrms);

/**
 * \brief   FVID2 Delete Function.
 *
 * \param   handle              FVID2 driver handle.
 *
 * \return  FVID2_SOK on success, else FVID2 error code
 *
 **/
int32_t Vhwa_m2mNfDelete(Fdrv_Handle handle, Ptr deleteArgs);

/**
 * \brief   FVID2 Control Function.
 *
 * \param   handle              FVID2 driver handle.
 *
 * \return  FVID2_SOK on success, else FVID2 error code
 *
 **/
int32_t Vhwa_m2mNfControl(Fdrv_Handle handle, uint32_t cmd,
                                  Ptr cmdArgs, Ptr cmdStatusArgs);

/**
 * \brief   FVID2 Process Request function.
 *
 * \param   handle              FVID2 driver handle.
 *
 * \return  FVID2_SOK on success, else FVID2 error code
 *
 **/
int32_t Vhwa_m2mNfProcessReq(Fdrv_Handle handle,
                                     Fvid2_FrameList *inFrmList,
                                     Fvid2_FrameList *outFrmList,
                                     uint32_t timeout);

/**
 * \brief   FVID2 Get Process Request function.
 *
 * \param   handle              FVID2 driver handle.
 *
 * \return  FVID2_SOK on success, else FVID2 error code
 *
 **/
int32_t Vhwa_m2mNfGetProcessReq(Fdrv_Handle handle,
                                        Fvid2_FrameList *inProcessList,
                                        Fvid2_FrameList *outProcessList,
                                        uint32_t timeout);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
Vhwa_M2mNfHandleObj   gM2mNfHandleObj[VHWA_M2M_NF_MAX_HANDLES];
Vhwa_M2mNfInstObj     gM2mNfInstObj;

Fvid2_DrvOps gM2mNfFvid2DrvOps = {
    FVID2_VHWA_M2M_NF_DRV_ID,
    /**< Unique driver Id. */
    Vhwa_m2mNfCreate,
    /**< FVID2 create function pointer. */
    Vhwa_m2mNfDelete,
    /**< FVID2 delete function pointer. */
    Vhwa_m2mNfControl,
    /**< FVID2 control function pointer. */
    NULL, NULL,
    /**< FVID2 queue function pointer. */
    Vhwa_m2mNfProcessReq,
    /**< FVID2 process request function pointer. */
    Vhwa_m2mNfGetProcessReq,
    /**< FVID2 get processed request function pointer. */
};

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int32_t Vhwa_m2mNfInit(Vhwa_M2mNfInitPrms *initPrms)
{
    int32_t             status;
    uint32_t            cnt;
    SemaphoreP_Params   params;
    Vhwa_M2mNfInstObj *instObj = NULL;

    if (NULL != initPrms)
    {
        instObj = &gM2mNfInstObj;

        /* Reset all instance object to 0x0 */
        Fvid2Utils_memset(instObj, 0U, sizeof (Vhwa_M2mNfInstObj));

        /* Mark pool flags as free */
        for (cnt = 0U; cnt < VHWA_M2M_NF_MAX_HANDLES; cnt++)
        {
            gM2mNfHandleObj[cnt].isUsed = (uint32_t) UFALSE;
        }

        /* Set HTS pipeline */
        instObj->pipeline = VHWA_M2M_NF_HTS_PIPELINE;

        Nf_getSocInfo(&instObj->socInfo);

        status = Fvid2_registerDriver(&gM2mNfFvid2DrvOps);
        if (FVID2_SOK == status)
        {
            instObj->isRegistered = (uint32_t)UTRUE;
        }
        else
        {
            GT_0trace(VhwaNfTrace, GT_ERR,
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
                GT_0trace(VhwaNfTrace, GT_ERR,
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
                GT_0trace(VhwaNfTrace, GT_ERR,
                    "Failed to allocate HWA Lock semaphore!!\n");
                status = FVID2_EALLOC;
            }
        }

        /* Create free and request queues */
        if (FVID2_SOK == status)
        {
            status = Vhwa_nfCreateQueue(instObj);
        }

        if (FVID2_SOK == status)
        {
            /* Initialize UDMA, ie allocate and initialize channels
               required for NF output */
            status = Vhwa_m2mNfUdmaInit(instObj, initPrms);
            if (FVID2_SOK != status)
            {
                GT_0trace(VhwaNfTrace, GT_ERR,
                    "UDMA Initialization Failed !!\n");
            }
        }

        /* Register ISR handler for the given irq number */
        if (FVID2_SOK == status)
        {
            instObj->irqNum = initPrms->irqInfo.irqNum;
            instObj->vhwaIrqNum = initPrms->irqInfo.vhwaIrqNum;

            status = Vhwa_m2mNfRegisterIsr(instObj);
        }

        /* Init is done, copy init params locally,
           enable DMPAC and NF in DMPAC Top */
        if (FVID2_SOK == status)
        {
            Fvid2Utils_memcpy(&instObj->initPrms, initPrms,
                sizeof(Vhwa_M2mNfInitPrms));

            /* Enable NF at DMPAC Top*/
            CSL_vpacEnableModule(instObj->socInfo.vpacCntlRegs,
                VPAC_MODULE_NF, (uint32_t)UTRUE);

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
        Vhwa_m2mNfDeInit();
    }

    return (status);
}

void Vhwa_m2mNfDeInit(void)
{
    Vhwa_M2mNfInstObj *instObj = NULL;

    instObj = &gM2mNfInstObj;

    if (instObj->openCnt > 0u)
    {
        GT_0trace(VhwaNfTrace, GT_ERR,
            "Warning: All driver handles are not closed!!\n");
    }

    /* Stop UDMA channels */
    (void)Vhwa_m2mNfStopCh(instObj);

    Vhwa_m2mNfUnregisterIsr(instObj);

    (void)Vhwa_m2mNfUdmaDeInit(instObj);

    if ((uint32_t)UTRUE == instObj->isRegistered)
    {
        (void)Fvid2_unRegisterDriver(&gM2mNfFvid2DrvOps);
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

    (void)Vhwa_nfDeleteQueue(instObj);

    /* Init all global variables to zero */
    Fvid2Utils_memset(instObj, 0U, sizeof (gM2mNfInstObj));

    instObj->initDone = (uint32_t)UFALSE;
}

Fdrv_Handle Vhwa_m2mNfCreate(UInt32 drvId, UInt32 drvInstId,
    Ptr createArgs, Ptr createStatusArgs, const Fvid2_DrvCbParams *cbPrms)
{
    int32_t                 status = FVID2_SOK;
    Vhwa_M2mNfInstObj     *instObj = NULL;
    Vhwa_M2mNfHandleObj   *hObj = NULL;
    Vhwa_M2mNfCreatePrms  *nfCreatePrms = NULL;
    Fdrv_Handle             handle = NULL;
    Nf_SocInfo            *socInfo = NULL;
    Vhwa_M2mNfSl2AllocPrms sl2AllocPrms;

    instObj = &gM2mNfInstObj;
    socInfo = &instObj->socInfo;

    /* Check for errors */
    if ((NULL == createArgs) ||
        (NULL == cbPrms))
    {
        GT_0trace(VhwaNfTrace, GT_ERR, "NULL Pointer !!\n");
        status = FVID2_EBADARGS;
    }
    else
    {
        status = Vhwa_nfCheckCreatePrms(drvId, drvInstId);
    }

    if (FVID2_SOK == status)
    {
        /* Open not allowed if init is not done */
        if ((uint32_t)UFALSE == instObj->initDone)
        {
            GT_0trace(VhwaNfTrace, GT_ERR,
                "Vhwa_m2mNfInit is not called\n");
            status  = FVID2_EFAIL;
        }

        nfCreatePrms = (Vhwa_M2mNfCreatePrms *)createArgs;
    }

    if (FVID2_SOK == status)
    {
        /* Take the instance semaphore */
        (void)SemaphoreP_pend(instObj->lock, SemaphoreP_WAIT_FOREVER);

        /* Allocate Handle Object */
        hObj = Vhwa_nfAllocHdlObj(instObj);

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
                    GT_0trace(VhwaNfTrace, GT_DEBUG,
                    "Vhwa_m2mNfAllocSl2 is not called, allocating for default\n");

                    Vhwa_M2mNfSl2AllocPrmsInit(&sl2AllocPrms);
                    status = Vhwa_m2mNfAllocSl2(&sl2AllocPrms);
                }

                if (FVID2_SOK == status)
                {
                    /* Start UDMA Channels on the first handle Create */
                    status = Vhwa_m2mNfStartCh(instObj);
                }

                if (FVID2_SOK == status)
                {
                    /* Enable Pipeline interrupt in INTD */
                    Vhwa_enableHtsIntr(socInfo->vpacIntdRegs,
                        instObj->vhwaIrqNum, instObj->pipeline);
                }
            }

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
                status = Vhwa_m2mNfAllocUdmaMem(hObj);
            }

            if (FVID2_SOK == status)
            {
                Fvid2Utils_memcpy(&hObj->createPrms, nfCreatePrms,
                    sizeof(Vhwa_M2mNfCreatePrms));

                Fvid2Utils_memcpy(&hObj->cbPrms, cbPrms,
                    sizeof (hObj->cbPrms));

                Vhwa_nfInitHandleObj(instObj, hObj);

                instObj->openCnt ++;
            }
            else
            {
                /* Some error, so free up the handle object */
                Vhwa_nfFreeHandleObj(hObj);
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

Int32 Vhwa_m2mNfDelete(Fdrv_Handle handle, Ptr deleteArgs)
{
    int32_t                 status = FVID2_SOK;
    Vhwa_M2mNfInstObj     *instObj = NULL;
    Vhwa_M2mNfHandleObj   *hObj = NULL;
    Nf_SocInfo            *socInfo = NULL;

    if (NULL != handle)
    {
        instObj = &gM2mNfInstObj;
        hObj    = (Vhwa_M2mNfHandleObj *)handle;
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
            Vhwa_nfFreeHandleObj(hObj);

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

Int32 Vhwa_m2mNfControl(Fdrv_Handle handle, UInt32 cmd, Ptr cmdArgs,
    Ptr cmdStatusArgs)
{
    int32_t                 status = FVID2_SOK;
    Vhwa_M2mNfInstObj     *instObj = NULL;
    Vhwa_M2mNfHandleObj   *hObj = NULL;
    Vhwa_M2mNfConfig      *nfCfg = NULL;
    Nf_ErrEventParams     *eePrms = NULL;
    Vhwa_HtsLimiter       *bwLimit = NULL;
    Nf_WgtTableConfig     *filterPrms = NULL;

    if (NULL == handle)
    {
        status = FVID2_EBADARGS;
    }
    else
    {
        instObj = &gM2mNfInstObj;
        hObj = (Vhwa_M2mNfHandleObj *)handle;
    }
    if (FVID2_SOK == status)
    {
        /* Take the instance semaphore */
        (void)SemaphoreP_pend(instObj->lock, SemaphoreP_WAIT_FOREVER);

        switch (cmd)
        {
            /* Set NF Parameters */
            case IOCTL_VHWA_M2M_NF_SET_PARAMS:
            {
                if (NULL != cmdArgs)
                {
                    nfCfg = (Vhwa_M2mNfConfig *)cmdArgs;
                    status = Vhwa_nfSetParams(instObj, hObj, nfCfg);
                }
                break;
            }

            /* Set NF Parameters */
            case IOCTL_VHWA_M2M_NF_SET_FILTER_COEFF:
            {
                if (NULL != cmdArgs)
                {
                    filterPrms = (Nf_WgtTableConfig *)cmdArgs;
                    status = Vhwa_nfSetFilterTables(instObj, filterPrms);
                }
                break;
            }

            /* Enable Register error events callback */
            case IOCTL_VHWA_M2M_NF_REGISTER_ERR_CB:
            {
                if (NULL != cmdArgs)
                {
                    eePrms = (Nf_ErrEventParams *)cmdArgs;
                    status = Vhwa_nfSetEeParams(instObj, hObj, eePrms);
                }
                break;
            }

            /* SET HTS Limiter config */
            case IOCTL_VHWA_M2M_NF_SET_HTS_LIMIT:
            {
                if (NULL != cmdArgs)
                {
                    bwLimit = (Vhwa_HtsLimiter *)cmdArgs;
                    Vhwa_nfSetBwLimit(hObj, bwLimit);
                }
                break;
            }

            /* Get PSA Signature */
            case IOCTL_VHWA_M2M_NF_GET_PSA_SIGN:
            {
                Vhwa_M2mNfPsaSign *psa = NULL;

                if (NULL != cmdArgs)
                {
                    psa = (Vhwa_M2mNfPsaSign *)cmdArgs;
                    *psa = hObj->psa;
                }
                break;
            }

            /* Get IP Performance */
            case IOCTL_VHWA_M2M_NF_GET_PERFORMANCE:
            {
                Vhwa_M2mNfPerf *perf = NULL;

                if (NULL != cmdArgs)
                {
                    perf = (Vhwa_M2mNfPerf *)cmdArgs;
                    perf->perf[0u] = (uint32_t)hObj->perfNum[0u];
                    perf->perf[1u] = (uint32_t)hObj->perfNum[1u];
                }
                break;
            }

            case IOCTL_VHWA_M2M_NF_SYNC_START:
            {
                if (NULL != hObj->createPrms.getTimeStamp)
                {
                    hObj->perfNum[0u] = hObj->createPrms.getTimeStamp();
                }

                status = CSL_htsPipelineStart(instObj->socInfo.htsRegs, &hObj->htsCfg[0]);
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

Int32 Vhwa_m2mNfProcessReq(Fdrv_Handle handle, Fvid2_FrameList *inFrmList,
    Fvid2_FrameList *outFrmList, uint32_t timeout)
{
    int32_t             status = FVID2_SOK;
    uint32_t            semTimeout;
    Vhwa_M2mNfInstObj   *instObj = NULL;
    Vhwa_M2mNfHandleObj *hObj = NULL;
    Vhwa_M2mNfQueueObj  *qObj = NULL;
    Vhwa_M2mNfConfig    *nfCfg = NULL;
    SemaphoreP_Status     semStatus;

    if ((NULL == handle) || (NULL == inFrmList) || (NULL == outFrmList))
    {
        status = FVID2_EBADARGS;
    }
    else
    {
        instObj = &gM2mNfInstObj;
        hObj = (Vhwa_M2mNfHandleObj *)handle;
        nfCfg = &hObj->nfCfg[0];
    }

    /* Check for null pointers */
    if (FVID2_SOK == status)
    {
        /* Input Buffer Addresses cannot be null */
        if ((NULL == inFrmList->frames[0U]) ||
            (0U == inFrmList->frames[0u]->addr[0U]) ||
            (NULL == outFrmList->frames[0U]) ||
            (0U == outFrmList->frames[0u]->addr[0U]))
        {
            status = FVID2_EBADARGS;
        }
        else
        {
            /* For YUV420 input, chroma buffer cannot be null */
            if ((FVID2_DF_YUV420SP_UV == nfCfg->inFmt.dataFormat) ||
                (FVID2_DF_YUV420SP_VU == nfCfg->inFmt.dataFormat))
            {
                if (0U == inFrmList->frames[0u]->addr[1U])
                {
                    status = FVID2_EBADARGS;
                }
            }
            if ((FVID2_DF_YUV420SP_UV == nfCfg->outFmt.dataFormat) ||
                (FVID2_DF_YUV420SP_VU == nfCfg->outFmt.dataFormat))
            {
                if (0U == outFrmList->frames[0u]->addr[1U])
                {
                    status = FVID2_EBADARGS;
                }
            }
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
            qObj = (Vhwa_M2mNfQueueObj *) Fvid2Utils_dequeue(instObj->freeQ);

            if (NULL == qObj)
            {
                hObj->numPendReq --;

                GT_0trace(VhwaNfTrace, GT_ERR,
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
            Vhwa_m2mNfSetTRAddress(qObj->hObj, &qObj->inFrmList,
                                    &qObj->outFrmList);

            /* HW is free, submit request to the hardware */
            /* If previous handle and current handles are same, */
            if (instObj->lastHndlObj == qObj->hObj)
            {
                /** Last handle was same as new handle,
                 *  so require to recofigure all HW IPs */
                status = Vhwa_m2mNfSetFrameSize(instObj, qObj, 0U);
            }
            else
            {
                /** Last handle was not same as new handle,
                 *  so require to recofigure all HW IPs */
                status = Vhwa_m2mNfSetConfigInHW(instObj, qObj, 0U);
            }
            if (FVID2_SOK == status)
            {
                /** Set the addresses, Submit the TR
                 *  Start the pipeline */
                status = Vhwa_m2mNfSubmitRequest(instObj, qObj, 0U);
            }
            else
            {
                hObj->numPendReq --;
            }
        }
    }

    return (status);
}

/** \brief Typedef for FVID2 get processed frames function pointer. */
Int32 Vhwa_m2mNfGetProcessReq(Fdrv_Handle handle,
    Fvid2_FrameList *inFrmList, Fvid2_FrameList *outFrmList,
    UInt32 timeout)
{
    int32_t                status = FVID2_SOK;
    uint32_t               cookie, cnt;
    Vhwa_M2mNfInstObj    *instObj = NULL;
    Vhwa_M2mNfHandleObj  *hObj = NULL;
    Vhwa_M2mNfQueueObj   *qObj = NULL;

    if (NULL == handle)
    {
        status = FVID2_EBADARGS;
    }
    else
    {
        instObj = &gM2mNfInstObj;
        hObj = (Vhwa_M2mNfHandleObj *)handle;
    }

    if (FVID2_SOK == status)
    {
        /* Take the instance semaphore, so that no other
         * handle can submit request from the task context. */
        (void)SemaphoreP_pend(instObj->lock, SemaphoreP_WAIT_FOREVER);

        /* Disable interrupts before accessing queue */
        cookie = HwiP_disable();

        /* Dequeue the completed request from done queue */
        qObj = (Vhwa_M2mNfQueueObj *) Fvid2Utils_dequeue(hObj->doneQ);

        /* Restore interrupts after updating node list */
        HwiP_restore(cookie);

        /* No buffers in the output queue */
        if (NULL == qObj)
        {
            /* Check if requests are pending with driver */
            if (0U == hObj->numPendReq)
            {
                /* Nothing is queued */
                GT_0trace(VhwaNfTrace, GT_DEBUG,
                    "Nothing to dequeue. No request pending with driver!!\r\n");
                status = FVID2_ENO_MORE_BUFFERS;
            }
            else
            {
                /* If no request have completed, return try again */
                GT_0trace(VhwaNfTrace, GT_DEBUG,
                    "Nothing to dequeue. Try again!!\r\n");
                status = FVID2_EAGAIN;
            }
        }
        else /* OutQ has buffer to be returned */
        {
            /* Copy the driver's process list to application's process list */
            Fvid2_copyFrameList(inFrmList, &qObj->inFrmList);
            Fvid2_copyFrameList(outFrmList, &qObj->outFrmList);

            for (cnt = 0; cnt < hObj->numIter; cnt++)
            {
                /* Dequeue from Rings */
                status = Vhwa_m2mNfPopRings(instObj, hObj, cnt);
            }

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


int32_t Vhwa_m2mNfAllocSl2(const Vhwa_M2mNfSl2AllocPrms *sl2AllocPrms)
{
    int32_t retVal = FVID2_SOK;
    uint32_t sl2MemReq = 0;
    uint32_t inPitchSl2,outPitchSl2;
    uint64_t sl2Addr;
    Vhwa_M2mNfInstObj *instObj = NULL;

    if(NULL == sl2AllocPrms)
    {
        GT_0trace(VhwaNfTrace, GT_ERR, "Bad Arguments\n");
        retVal = FVID2_EBADARGS;
    }

    if(FVID2_SOK == retVal)
    {
        if((sl2AllocPrms->inBuffDepth < VHWA_M2M_NF_MIN_RD_BUFFER_DEPTH) ||
           (sl2AllocPrms->outBuffDepth < VHWA_M2M_NF_MIN_WR_BUFFER_DEPTH))
        {
            GT_0trace(VhwaNfTrace, GT_ERR, "Bad Arguments\n");
            retVal = FVID2_EBADARGS;
        }

        instObj = &gM2mNfInstObj;
    }

    if(FVID2_SOK == retVal)
    {
        /* Calculate Sl2 buffer size required for input */
        inPitchSl2 = Vhwa_calcHorzSizeInBytes(sl2AllocPrms->maxImgWidth,
                                                sl2AllocPrms->inCcsf);
        inPitchSl2 = Vhwa_calcSl2Pitch(inPitchSl2);

        sl2MemReq += inPitchSl2 * sl2AllocPrms->inBuffDepth;

        /* Calculate Sl2 buffer size required for output */
        outPitchSl2 = Vhwa_calcHorzSizeInBytes(sl2AllocPrms->maxImgWidth,
                                                sl2AllocPrms->outCcsf);
        outPitchSl2 = Vhwa_calcSl2Pitch(outPitchSl2);

        sl2MemReq += outPitchSl2 * sl2AllocPrms->outBuffDepth;

        /* Allocate Sl2 Memory */
        sl2Addr = Vhwa_allocateSl2(sl2MemReq,
                                    VHWA_SL2_INST_VPAC);
        if(0u == sl2Addr)
        {
            retVal = FVID2_EALLOC;
        }
        else
        {
            instObj->sl2Prms.sl2StartAddr = sl2Addr;
            instObj->sl2Prms.sl2MemSize = sl2MemReq;
        }
    }

    if(FVID2_SOK == retVal)
    {
        /* Calculate Sl2 buffer size required for input */
        instObj->sl2Prms.sl2Addr[0] = sl2Addr;
        instObj->sl2Prms.sl2NumLines[0] = sl2AllocPrms->inBuffDepth;

        sl2Addr += (uint64_t)inPitchSl2 * (uint64_t)sl2AllocPrms->inBuffDepth;

        instObj->sl2Prms.sl2Addr[1] = sl2Addr;
        instObj->sl2Prms.sl2NumLines[1] = sl2AllocPrms->outBuffDepth;

        instObj->isSl2AllocDone = UTRUE;
    }

    return retVal;
}


void Vhwa_m2mNfFreeSl2(void)
{
    Vhwa_M2mNfInstObj *instObj = &gM2mNfInstObj;

    Vhwa_FreeSl2(instObj->sl2Prms.sl2StartAddr, VHWA_SL2_INST_VPAC);

    instObj->isSl2AllocDone = UFALSE;
}

/**
 * \brief   Returns handle object for the requested handle count.
 *
 * \param   cnt              count.
 *
 * \return  reference to the handle object.
 *
 **/
Vhwa_M2mNfHandleObj *Vhwa_m2mNfGetHandleObj(uint32_t cnt)
{
    return &gM2mNfHandleObj[cnt];
}
/* ========================================================================== */
/*                           Local Functions                                  */
/* ========================================================================== */
static Vhwa_M2mNfHandleObj *Vhwa_nfAllocHdlObj(Vhwa_M2mNfInstObj *instObj)
{
    uint32_t cnt;
    Vhwa_M2mNfHandleObj *hObj = NULL;

    /* Check for Null pointer */
    GT_assert(VhwaNfTrace, (NULL != instObj));

    for (cnt = 0U; cnt < VHWA_M2M_NF_MAX_HANDLES; cnt ++)
    {
        if ((uint32_t)UFALSE == gM2mNfHandleObj[cnt].isUsed)
        {
            /* Allocate Handle Object */
            hObj = &gM2mNfHandleObj[cnt];
            Fvid2Utils_memset(hObj, 0x0, sizeof(Vhwa_M2mNfHandleObj));
            gM2mNfHandleObj[cnt].isUsed = (uint32_t)UTRUE;
            hObj->hIdx = cnt;
            break;
        }
    }

    return (hObj);
}

static int32_t Vhwa_nfCreateQueue(Vhwa_M2mNfInstObj *instObj)
{
    int32_t retVal;
    uint32_t qCnt;
    Vhwa_M2mNfQueueObj *qObj;

    /* NULL pointer check */
    GT_assert(VhwaNfTrace, (NULL != instObj));

    instObj->actQObj = NULL;
    instObj->freeQ = NULL;

    /* Create Free queue */
    retVal = Fvid2Utils_constructQ(&instObj->freeQObj);
    GT_assert(VhwaNfTrace, (retVal == FVID2_SOK));

    instObj->freeQ = &instObj->freeQObj;


    /* Initialize and queue the allocate queue object to free Q */
    for(qCnt=0U; qCnt < VHWA_NF_QUEUE_LEN_PER_INST; qCnt++)
    {
        qObj = &instObj->nfQObj[qCnt];
        qObj->hObj = NULL;
        Fvid2Utils_queue(instObj->freeQ, &qObj->qElem, qObj);
    }

    return retVal;
}

static int32_t Vhwa_nfDeleteQueue(Vhwa_M2mNfInstObj *instObj)
{
    int32_t retVal = FVID2_SOK;
    Vhwa_M2mNfQueueObj *qObj;

    /* NULL pointer check */
    GT_assert(VhwaNfTrace, (NULL != instObj));

    if(NULL != instObj->freeQ)
    {
        /* Free-up all the queued free queue objects */
        do
        {
            qObj = (Vhwa_M2mNfQueueObj *) Fvid2Utils_dequeue(instObj->freeQ);
        } while (NULL != qObj);

        /* Delete the free Q */
        Fvid2Utils_destructQ(instObj->freeQ);
        instObj->freeQ = NULL;
    }

    return (retVal);
}

static void Vhwa_nfFreeHandleObj(Vhwa_M2mNfHandleObj *hObj)
{
    uint32_t cnt;

    /* Check for Null pointer */
    GT_assert(VhwaNfTrace, (NULL != hObj));

    for (cnt = 0U; cnt < VHWA_M2M_NF_MAX_HANDLES; cnt ++)
    {
        /* Freeup allocated Handle Object */
        if (hObj == &gM2mNfHandleObj[cnt])
        {
            hObj->isUsed = (uint32_t)UFALSE;;
            break;
        }
    }

}

static void Vhwa_nfInitHandleObj(Vhwa_M2mNfInstObj *instObj,
    Vhwa_M2mNfHandleObj *hObj)
{
    /* Check for Null pointer */
    GT_assert(VhwaNfTrace, (NULL != hObj));
    GT_assert(VhwaNfTrace, (NULL != instObj));

    hObj->numIter = 1U;
    /* Initialize NF with Default configuration */
    Vhwa_M2mNfConfigInit(&hObj->nfCfg[0]);

    /* Set HTS Configuration for default NF configuration */
    Vhwa_nfSetHtsCfg(instObj, hObj);
    /* Set LSE Configuration for default NF configuration */
    Vhwa_nfSetLseCfg(instObj, hObj);
}

static int32_t Vhwa_nfSetFilterTables(Vhwa_M2mNfInstObj *instObj,
                                      const Nf_WgtTableConfig *pCoeffCfg)
{
    int32_t retVal;

    /* Check for Null pointer */
    GT_assert(VhwaNfTrace, (NULL != pCoeffCfg));

    retVal = CSL_nfSetWgtTableConfig(instObj->socInfo.nfRegs, pCoeffCfg);

    return (retVal);
}

static int32_t Vhwa_nfSetParams(Vhwa_M2mNfInstObj *instObj,
    Vhwa_M2mNfHandleObj *hObj, Vhwa_M2mNfConfig *nfPrms)
{
    int32_t retVal;
    uint32_t cnt;

    /* Check for Null pointer */
    GT_assert(VhwaNfTrace, (NULL != hObj));
    GT_assert(VhwaNfTrace, (NULL != instObj));
    GT_assert(VhwaNfTrace, (NULL != nfPrms));

    retVal = Vhwa_nfCheckNfCfg(instObj, nfPrms);

    if (FVID2_SOK == retVal)
    {
        /* Set the CSL parameters */
        Vhwa_nfSetCfgParams(hObj, nfPrms);

        /* Initialize HTS configuration based on the NF Config */
        Vhwa_nfSetHtsCfg(instObj, hObj);
        /* Initialize LSE configuration based on the NF Config */
        Vhwa_nfSetLseCfg(instObj, hObj);

        for(cnt = 0u; (cnt < hObj->numIter) && (FVID2_SOK == retVal); cnt++)
        {
            /* Verify HTS Configuration */
            retVal = CSL_htsCheckThreadConfig(&hObj->htsCfg[cnt]);
        }
        for(cnt = 0u; (cnt < hObj->numIter) && (FVID2_SOK == retVal); cnt++)
        {
            /* Verify LSE Configuration */
            retVal = CSL_lseCheckConfig(&hObj->lseCfg[cnt]);
        }

        if(FVID2_SOK == retVal)
        {
            /* Setup TR Descriptor */
            Vhwa_m2mNfSetTrDesc(instObj, hObj);
        }
    }

    if(FVID2_SOK != retVal)
    {
        hObj->numIter = 1;
        /* Initialize NF with Default configuration */
        Vhwa_M2mNfConfigInit(&hObj->nfCfg[0]);
        /* Set HTS Configuration for default NF configuration */
        Vhwa_nfSetHtsCfg(instObj, hObj);
        /* Set LSE Configuration for default NF configuration */
        Vhwa_nfSetLseCfg(instObj, hObj);
    }

    return (retVal);
}

static int32_t Vhwa_nfSetEeParams(const Vhwa_M2mNfInstObj *instObj,
    Vhwa_M2mNfHandleObj *hObj, const Nf_ErrEventParams *eePrms)
{
    int32_t status = FVID2_EBADARGS;

    if ((NULL != instObj) && (NULL != hObj) && (NULL != eePrms))
    {
        Fvid2Utils_memcpy(&hObj->eePrms, eePrms, sizeof(Nf_ErrEventParams));
        status = FVID2_SOK;
    }

    return (status);
}

static int32_t Vhwa_nfCheckNfCfg(Vhwa_M2mNfInstObj *instObj,
    Vhwa_M2mNfConfig *nfCfg)
{
    int32_t retVal = FVID2_SOK;
    uint32_t minPitch;
    Fvid2_Format *inFmt = NULL;
    Fvid2_Format *outFmt = NULL;

    /* Check for Null pointer */
    GT_assert(VhwaNfTrace, (NULL != instObj));
    GT_assert(VhwaNfTrace, (NULL != nfCfg));

    inFmt = &nfCfg->inFmt;

    /* Check for Valid data format */
    if ((FVID2_DF_LUMA_ONLY != inFmt->dataFormat) &&
        (FVID2_DF_CHROMA_ONLY != inFmt->dataFormat) &&
        (FVID2_DF_YUV420SP_UV != inFmt->dataFormat) &&
        (FVID2_DF_YUV420SP_VU != inFmt->dataFormat))
    {
        retVal = FVID2_EINVALID_PARAMS;
    }

    if((inFmt->width > VHWA_M2M_NF_MAX_WIDTH_HEIGHT) ||
        (inFmt->height > VHWA_M2M_NF_MAX_WIDTH_HEIGHT))
    {
        retVal = FVID2_EINVALID_PARAMS;
    }

    minPitch = Vhwa_calcHorzSizeInBytes(inFmt->width, inFmt->ccsFormat);
    if (inFmt->pitch[0] < minPitch)
    {
        retVal = FVID2_EINVALID_PARAMS;
    }
    else
    {
        instObj->sl2Prms.sl2Pitch[0] = Vhwa_calcSl2Pitch(minPitch);
    }

    outFmt = &nfCfg->outFmt;
    /* check for Input and output Width/height */
    if((0u == nfCfg->nfCfg.skipMode) &&
       ((outFmt->width != inFmt->width) ||
        (outFmt->height != inFmt->height)))
    {
        retVal = FVID2_EINVALID_PARAMS;
    }
    if((0u != nfCfg->nfCfg.skipMode) &&
       ((outFmt->width != (inFmt->width/2u)) ||
        (outFmt->height != (inFmt->height/2u))))
    {
        retVal = FVID2_EINVALID_PARAMS;
    }

    minPitch = Vhwa_calcHorzSizeInBytes(outFmt->width,
                                        outFmt->ccsFormat);
    if (outFmt->pitch[0] < minPitch)
    {
        retVal = FVID2_EINVALID_PARAMS;
    }
    else
    {
        instObj->sl2Prms.sl2Pitch[1] = Vhwa_calcSl2Pitch(minPitch);
    }

    if(outFmt->dataFormat != inFmt->dataFormat)
    {
        retVal = FVID2_EINVALID_PARAMS;
    }

    return (retVal);
}

static void Vhwa_nfSetLseCfg(const Vhwa_M2mNfInstObj *instObj,
                             Vhwa_M2mNfHandleObj *hObj)
{
    Vhwa_M2mNfCreatePrms *createPrms = NULL;
    CSL_LseConfig *lseCfg = NULL;
    Vhwa_M2mNfConfig *nfCfg = NULL;
    uint32_t itrCnt;

    /* Check for Null pointer */
    GT_assert(VhwaNfTrace, (NULL != hObj));
    GT_assert(VhwaNfTrace, (NULL != instObj));

    for (itrCnt = 0; itrCnt < hObj->numIter; itrCnt++)
    {
        nfCfg = &hObj->nfCfg[itrCnt];
        lseCfg = &hObj->lseCfg[itrCnt];

        createPrms = &hObj->createPrms;

        /* Initialize LSE configuration with the default values */
        CSL_lseConfigInit(lseCfg);

        /* For NF, number of input and output channels are 1 */
        lseCfg->numInCh = 1U;
        lseCfg->numOutCh = 1U;

        lseCfg->enablePsa = createPrms->enablePsa;
        lseCfg->inChCfg[0U].enable = (uint32_t)UTRUE;
        lseCfg->inChCfg[0U].frameWidth = nfCfg->inFmt.width;
        lseCfg->inChCfg[0U].frameHeight = nfCfg->inFmt.height;
        if(nfCfg->outFmt.height == nfCfg->inFmt.height)
        {
            lseCfg->inChCfg[0U].enableAddrIncrBy2 = (uint32_t)UFALSE;
        }
        else
        {
            lseCfg->inChCfg[0U].enableAddrIncrBy2 = (uint32_t)UTRUE;
       }
        lseCfg->inChCfg[0U].ccsf =
            (Fvid2_ColorCompStorageFmt)nfCfg->inFmt.ccsFormat;
        lseCfg->inChCfg[0U].startOffset = 0U;
        lseCfg->inChCfg[0U].lineOffset = instObj->sl2Prms.sl2Pitch[0];
        lseCfg->inChCfg[0U].circBufSize = instObj->sl2Prms.sl2NumLines[0];
        lseCfg->inChCfg[0U].bufAddr[0U] = (uint32_t)instObj->sl2Prms.sl2Addr[0];
        lseCfg->inChCfg[0U].numInBuff = 1U;

        /* Assumes 5 tap filter, requires change once config is set */
        lseCfg->inChCfg[0U].knTopPadding = 2U;
        lseCfg->inChCfg[0U].knLineOffset = 0U;
        lseCfg->inChCfg[0U].knHeight = 5U;
        if (nfCfg->outFmt.height == nfCfg->inFmt.height)
        {
            lseCfg->inChCfg[0U].knBottomPadding = 2U;
        }
        else
        {
            lseCfg->inChCfg[0U].knBottomPadding = 1U;
        }

        lseCfg->outChCfg[0U].enable = (uint32_t)UTRUE;
        lseCfg->outChCfg[0U].ccsf =
            (Fvid2_ColorCompStorageFmt)nfCfg->outFmt.ccsFormat;
        lseCfg->outChCfg[0U].lineOffset = instObj->sl2Prms.sl2Pitch[1];
        lseCfg->outChCfg[0U].circBufSize = instObj->sl2Prms.sl2NumLines[1];
        lseCfg->outChCfg[0U].bufAddr = (uint32_t)instObj->sl2Prms.sl2Addr[1];
    }
}

static void Vhwa_nfSetHtsCfg(Vhwa_M2mNfInstObj *instObj,
                                Vhwa_M2mNfHandleObj *hObj)
{
    CSL_HtsSchConfig *htsCfg = NULL;
    Vhwa_M2mNfConfig *nfCfg = NULL;
    uint32_t itrCnt;

    /* Check for Null pointer */
    GT_assert(VhwaNfTrace, (NULL != hObj));
    GT_assert(VhwaNfTrace, (NULL != instObj));

    for (itrCnt = 0; itrCnt < hObj->numIter; itrCnt++)
    {
        htsCfg = &hObj->htsCfg[itrCnt];
        nfCfg = &hObj->nfCfg[itrCnt];

        CSL_htsSchConfigInit(htsCfg);

        htsCfg->schId = CSL_HTS_HWA_SCH_NF;
        htsCfg->pipeline = instObj->pipeline;
        htsCfg->enableStream = (uint32_t)UFALSE;
        htsCfg->enableHop = (uint32_t)UFALSE;
        htsCfg->enableWdTimer = (uint32_t)UFALSE;

        htsCfg->enableBwLimit = UFALSE;

        htsCfg->consCfg[0U].enable = (uint32_t)UTRUE;
        htsCfg->consCfg[0U].prodId = CSL_HTS_PROD_IDX_UDMA;

        htsCfg->prodCfg[0U].enable = (uint32_t)UTRUE;
        htsCfg->prodCfg[0U].consId = CSL_HTS_CONS_IDX_UDMA;

        htsCfg->prodCfg[0U].threshold = 1u;
        htsCfg->prodCfg[0U].cntPreLoad = 0u;
        htsCfg->prodCfg[0U].cntPostLoad = 0u;

        htsCfg->prodCfg[0U].depth = instObj->sl2Prms.sl2NumLines[1];
        htsCfg->prodCfg[0U].countDec  =1U;

        htsCfg->dmaConsCfg[0U].enable = (uint32_t)UTRUE;
        htsCfg->dmaConsCfg[0U].dmaChNum = Udma_chGetNum(instObj->utcChHndl[1]);
        htsCfg->dmaConsCfg[0U].pipeline = instObj->pipeline;
        htsCfg->dmaConsCfg[0U].enableStream = (uint32_t)UFALSE;
        htsCfg->dmaConsCfg[0U].prodId = CSL_HTS_PROD_IDX_UDMA;
            /* Default mapping for DMA */

        htsCfg->dmaProdCfg[0U].enable = (uint32_t)UTRUE;
        htsCfg->dmaProdCfg[0U].dmaChNum = Udma_chGetNum(instObj->utcChHndl[0]);
        htsCfg->dmaProdCfg[0U].pipeline = instObj->pipeline;
        htsCfg->dmaProdCfg[0U].consId = CSL_HTS_CONS_IDX_UDMA;
            /* Default mapping for DMA */


        htsCfg->dmaProdCfg[0U].threshold = 5U;
        htsCfg->dmaProdCfg[0U].cntPreLoad = 2U;
        htsCfg->dmaProdCfg[0U].cntPostLoad = 2U;

        htsCfg->dmaProdCfg[0U].enableHop = (uint32_t)UTRUE;
        htsCfg->dmaProdCfg[0U].numHop = nfCfg->inFmt.height;

        if (nfCfg->outFmt.height == nfCfg->inFmt.height)
        {
            htsCfg->dmaProdCfg[0U].countDec = 1U;
        }
        else
        {
            htsCfg->dmaProdCfg[0U].countDec = 2U;
        }

        htsCfg->dmaProdCfg[0U].depth = instObj->sl2Prms.sl2NumLines[0];
    }
}

static void Vhwa_nfSetCfgParams(Vhwa_M2mNfHandleObj *hObj,
                                const Vhwa_M2mNfConfig *nfCfg)
{
    uint32_t itrCnt;
    Vhwa_M2mNfConfig *nfConfig = NULL;


    /* Check for Null pointer */
    GT_assert(VhwaNfTrace, (NULL != hObj));
    GT_assert(VhwaNfTrace, (NULL != nfCfg));


    if ((FVID2_DF_YUV420SP_UV == nfCfg->inFmt.dataFormat) ||
        (FVID2_DF_YUV420SP_VU == nfCfg->inFmt.dataFormat))
    {
        /* Both Luma and Chroma Needs to be processed */
        hObj->numIter = 2U;
    }
    else
    {
        /* Only Chroma/Luma needs to be processed */
        hObj->numIter = 1U;
    }

    for (itrCnt = 0; itrCnt < hObj->numIter; itrCnt++)
    {
        nfConfig = &hObj->nfCfg[itrCnt];

        Fvid2Utils_memcpy(nfConfig, nfCfg, sizeof(Vhwa_M2mNfConfig));

        if(((FVID2_DF_YUV420SP_UV == nfConfig->inFmt.dataFormat) ||
            (FVID2_DF_YUV420SP_VU == nfConfig->inFmt.dataFormat)) && (itrCnt == 1U))
        {
            /* Chroma */
            nfConfig->nfCfg.interleaveMode = UTRUE;

            /* Height will be 1/2 for Chroma */
            nfConfig->inFmt.height =  nfConfig->inFmt.height/2U;
            nfConfig->outFmt.height =  nfConfig->outFmt.height/2U;
        }
        else if(FVID2_DF_CHROMA_ONLY == nfConfig->inFmt.dataFormat)
        {
            nfConfig->nfCfg.interleaveMode = UTRUE;
        }
        else
        {
            nfConfig->nfCfg.interleaveMode = UFALSE;
        }
    }
}

static void Vhwa_nfSetBwLimit(Vhwa_M2mNfHandleObj *hObj,
                              const Vhwa_HtsLimiter *bwLimit)
{
    uint32_t cnt;
    CSL_HtsSchConfig *htsCfg = NULL;

    /* Check for Null pointer */
    GT_assert(VhwaNfTrace, (NULL != hObj));
    GT_assert(VhwaNfTrace, (NULL != bwLimit));

    for (cnt = 0; cnt < hObj->numIter; cnt++)
    {
        htsCfg = &hObj->htsCfg[cnt];

        /* Configure  HTS BW limiter */
        htsCfg->enableBwLimit = bwLimit->enableBwLimit;
        htsCfg->cycleCnt      = bwLimit->cycleCnt;
        htsCfg->tokenCnt      = bwLimit->tokenCnt;
    }
}

int32_t Vhwa_m2mNfSetConfigInHW(Vhwa_M2mNfInstObj *instObj,
                                const Vhwa_M2mNfQueueObj *qObj,
                                uint32_t itrCnt)
{
    int32_t              status;
    Vhwa_M2mNfHandleObj *hObj = NULL;

    /* Null pointer check */
    GT_assert(VhwaNfTrace, (NULL != instObj));
    GT_assert(VhwaNfTrace, (NULL != qObj));
    GT_assert(VhwaNfTrace, (NULL != qObj->hObj));

    hObj = qObj->hObj;

    /* Configure NF Core */
    CSL_nfSetConfig(instObj->socInfo.nfRegs,
                &hObj->nfCfg[itrCnt].nfCfg);

    /* Configure HWA Common Wrapper LSE */
    status = CSL_lseSetConfig(instObj->socInfo.lseRegs, &hObj->lseCfg[itrCnt]);

    if (FVID2_SOK == status)
    {
        /* Configure HWA Common Wrapper HTS */
        status = CSL_htsSetThreadConfig(instObj->socInfo.htsRegs, &hObj->htsCfg[itrCnt]);
    }

    if (FVID2_SOK == status)
    {
        /* Start HTS Channels */
        status = CSL_htsThreadStart(instObj->socInfo.htsRegs,
                                    &hObj->htsCfg[itrCnt]);
    }

    return (status);
}

int32_t Vhwa_m2mNfSetFrameSize(Vhwa_M2mNfInstObj *instObj,
                               const Vhwa_M2mNfQueueObj *qObj,
                               uint32_t itrCnt)
{
    int32_t               status;
    Vhwa_M2mNfHandleObj *hObj = NULL;

    /* Null pointer check */
    GT_assert(VhwaNfTrace, (NULL != instObj));
    GT_assert(VhwaNfTrace, (NULL != qObj));
    GT_assert(VhwaNfTrace, (NULL != qObj->hObj));

    hObj = qObj->hObj;

    /* Configure NF Core */
    CSL_nfUpdateConfig(instObj->socInfo.nfRegs,
                &hObj->nfCfg[itrCnt].nfCfg);

    /* Configure HWA Common Wrapper LSE */
    status = CSL_lseSetUpdateConfig(instObj->socInfo.lseRegs,
            &hObj->lseCfg[itrCnt]);

    if (FVID2_SOK == status)
    {
        /* Configure HWA Common Wrapper HTS */
        status = CSL_htsSetThreadUpdateConfig(instObj->socInfo.htsRegs,
            &hObj->htsCfg[itrCnt]);
    }

    if (FVID2_SOK == status)
    {
        /* Start HTS Channels */
        status = CSL_htsThreadStart(instObj->socInfo.htsRegs,
                                    &hObj->htsCfg[itrCnt]);
    }

    return (status);
}

int32_t Vhwa_m2mNfSubmitRequest(Vhwa_M2mNfInstObj *instObj,
                                    Vhwa_M2mNfQueueObj *qObj,
                                    uint32_t itrCnt)
{
    int32_t retVal;
    Vhwa_M2mNfHandleObj *hObj = NULL;

    GT_assert(VhwaNfTrace, (NULL != instObj));
    GT_assert(VhwaNfTrace, (NULL != qObj));
    GT_assert(VhwaNfTrace, (NULL != qObj->hObj));

    hObj = qObj->hObj;

    /* Submit Rings to the Ring Accelerator */
    retVal = Vhwa_m2mNfSubmitRing(instObj, hObj, itrCnt);

    if (FVID2_SOK == retVal)
    {
        /* Better to set Active object to this q object, so that if
             * interrupt comes immediately, actQObj would be set..
             * If pipeline start fails, it would be set to NULL.*/
        instObj->actQObj = qObj;

        instObj->totalReqCnt ++;

        #ifndef VHWA_USE_PIPELINE_COMMON_ENABLE
        if (NULL != hObj->createPrms.getTimeStamp)
        {
            hObj->perfNum[itrCnt] = hObj->createPrms.getTimeStamp();
        }
        /* Start HTS pipeline */
        retVal = CSL_htsPipelineStart(instObj->socInfo.htsRegs,
                                      &hObj->htsCfg[itrCnt]);
        #endif
        if (FVID2_SOK != retVal)
        {
            instObj->actQObj = NULL;
            retVal = FVID2_EFAIL;
        }
        else
        {
            retVal = FVID2_SOK;
        }
    }

    return (retVal);
}

static int32_t Vhwa_nfCheckCreatePrms(uint32_t drvId, uint32_t drvInstId)
{
    int32_t     status = FVID2_SOK;

    if (FVID2_VHWA_M2M_NF_DRV_ID != drvId)
    {
        GT_0trace(VhwaNfTrace, GT_ERR, "Invalid Driver ID !!\n");
        status = FVID2_EINVALID_PARAMS;
    }
    else
    {
        /* Check for correct instance ID */
#if defined VHWA_M2M_VPAC_INSTANCE
#if (VHWA_M2M_VPAC_INSTANCE == 0)
        if (VHWA_M2M_VPAC_0_NF_DRV_INST_ID_0 != drvInstId)
        {
            status = FVID2_EINVALID_PARAMS;
            GT_0trace(VhwaLdcTrace, GT_ERR, "Invalid/unsupported Instance Id\n");
        }
#endif
#endif

#if defined VHWA_M2M_VPAC_INSTANCE
#if (VHWA_M2M_VPAC_INSTANCE == 1)
        if (VHWA_M2M_VPAC_1_NF_DRV_INST_ID_0 != drvInstId)
        {
            status = FVID2_EINVALID_PARAMS;
            GT_0trace(VhwaLdcTrace, GT_ERR, "Invalid/unsupported Instance Id\n");
        }
#endif
#endif
    }

    return (status);
}
