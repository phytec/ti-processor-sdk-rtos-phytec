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
 *  \file vhwa_m2mLdcApi.c
 *
 *  \brief API Implementation
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/drv/vhwa/src/drv/vhwa_m2mLdcPriv.h>

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
static Vhwa_M2mLdcHandleObj *vhwaM2mLdcAllocHandleObj(
    const Vhwa_M2mLdcInstObj *instObj);

/**
 * \brief   Create Queues, required for storing pending requests
 *
 * \param   instObj             Instance object.
 *
 * \return  pointer to handle object on success else NULL.
 *
 **/
static int32_t vhwaM2mLdcCreateQueues(Vhwa_M2mLdcInstObj *instObj);

/**
 * \brief   Delete Queues
 *
 * \param   instObj             Instance object.
 *
 **/
static void vhwaM2mLdcDeleteQueues(Vhwa_M2mLdcInstObj *instObj);

/**
 * \brief   Local function to freeup allocated Handle Object and return it
 *          to the pool of free handle objects.
 *          No protection inside the function, Caller should protect
 *          the function call
 *
 * \param   hObj                Handle Object to be freed up.
 *
 **/
static void vhwaM2mLdcFreeHandleObj(Vhwa_M2mLdcHandleObj *hObj);

/**
 * \brief   Function to check given LDC configuration, used in
 *          SET_PARAMS ioctl it first initializes LSE and HTS
 *          config and then uses CSLFL of LSE and HTS to validate
 *          the configuration
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 * \param   ldcCfg              LDC configuration to be validated
 *
 * \return  FVID2_SOK if given LDC cofiguration is valid, error code otherwise.
 *
 **/
static int32_t vhwaM2mLdcCheckLdcCfg(const Vhwa_M2mLdcInstObj *instObj,
    Vhwa_M2mLdcHandleObj *hObj, const Ldc_Config *ldcCfg);

/**
 * \brief   Function to calculate required block size for the
 *          given LDC configuration
 *
 * \param   ldcCfg              LDC configuration to be validated
 * \param   outId               Output ID
 *
 * \return  required block size in bytes.
 *
 **/
static uint32_t vhwaM2mLdcCalcSl2MemSize(const Ldc_Config *ldcCfg,
                                    uint32_t outId);

/**
 * \brief   Function to get block width and height for the given
 *          LDC configuration
 *
 * \param   ldcCfg              LDC configuration, input parameter
 * \param   blockWidth          calculated block width, output parameter
 * \param   blockHeight         calculated block height, output parameter
 *
 * \return  required block size in bytes.
 *
 **/
static void vhwaM2mLdcGetBlockSize(const Ldc_Config *ldcCfg,
                                   uint32_t *blockWidth,
                                   uint32_t *blockHeight);

/**
 * \brief   Function to initialize Region parameters from the given channel
 *
 * \param   chPrms              Channel params,
 *                              Region params from this are updated
 * \param   ldcCfg              LDC Configuration
 *
 **/
static void vhwaM2mLdcInitRegParams(Vhwa_M2mLdcChParams *chPrms,
    Ldc_Config *ldcCfg);

/**
 * \brief   Function to initialize Region parameters from the
 *          given chroma channel. Created a separate function so that
 *          height division is clear
 *
 * \param   chPrms              Channel params,
 *                              Region params from this are updated
 * \param   ldcCfg              LDC Configuration
 *
 **/
static void vhwaM2mLdcInitRegParamsForChroma(Vhwa_M2mLdcChParams *chPrms,
    Ldc_Config *ldcCfg);

/**
 * \brief   Function to calculate maximum block pitch out of enabled regions
 *          in the given channel.
 *          The output is stored in the channel parameters.
 *
 * \param   chPrms              Channel params,
 *
 **/
static void vhwaM2mLdcCalcMaxBlockPitch(Vhwa_M2mLdcChParams *chPrms);

/**
 * \brief   Function to calculate possible number of blocks,
 *          given the number of SL2 lines and block height in each region.
 *          It stored the number of blocks in the channel parameters.
 *
 * \param   chPrms              Channel params
 *
 **/
static void vhwaM2mLdcCalcNumSl2Blocks(Vhwa_M2mLdcChParams *chPrms);

/**
 * \brief   Function to calculate transfer record parameters for each channel.
 *          It internally call another API to calculate TR
 *          parameters for each region
 *
 * \param   instObj             Instance Object
 * \param   chPrms              Channel params
 *
 **/
static void vhwaM2mLdcCalcChTrParams(const Vhwa_M2mLdcInstObj *instObj,
    Vhwa_M2mLdcHandleObj *hObj);

/**
 * \brief   Function to calculate transfer record parameters for
 *          each region. Calculates icnts, dims etc. so that it can
 *          be directly used in the TR creation.
 *          It also calculates the number of TRs required for each region.
 *
 * \param   chPrms              Channel params
 *
 **/
static void vhwaM2mLdcCalcRegionTrParams(Vhwa_M2mLdcChParams *chPrms);

/**
 * \brief   Based on the given LDC config, it initializes LSE configuration.
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 * \param   ldcCfg              LDC Configuration
 *
 **/
static void vhwaM2mLdcSetLseCfg(const Vhwa_M2mLdcInstObj *instObj,
    Vhwa_M2mLdcHandleObj *hObj, const Ldc_Config *ldcCfg);

/**
 * \brief   Based on the given LDC config, it initializes HTS configuration.
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 * \param   ldcCfg              LDC Configuration
 *
 **/
static void vhwaM2mLdcSetHtsCfg(const Vhwa_M2mLdcInstObj *instObj,
    Vhwa_M2mLdcHandleObj *hObj, const Ldc_Config *ldcCfg);

/*  */
/**
 * \brief   Implementation of SET_PARAMS ioctl.
 *          It uses CheckLdcCfg to validate the config.
 *          If it is valid, copies the config into handle object
 *          If it is invalid, it reverts LSE/HTS config to known valid config
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 * \param   ldcCfg              LDC configuration to be set
 *
 **/
static int32_t vhwaM2mLdcSetParams(const Vhwa_M2mLdcInstObj *instObj,
    Vhwa_M2mLdcHandleObj *hObj, const Ldc_Config *ldcCfg);

/**
 * \brief   Function to configure LDS, LSE and HTS in the HW.
 *
 * \param   instObj             Instance Object, used for getting base address
 * \param   qObj                Queue Object, used for getting handle object
 *
 **/
static int32_t vhwaM2mLdcSetConfigInHW(Vhwa_M2mLdcInstObj *instObj,
    const Vhwa_M2mLdcQueueObj *qObj);

/**
 * \brief   Minimal function to submit request to hw and start LDC operation.
 *          It first sets address in the TR, Submits it to the ring and
 *          starts the pipeline.
 *          It does not start currently LDC, HTS schedulers.
 *
 * \param   instObj             Instance Object, used for getting base address
 * \param   qObj                Queue Object, used for getting handle object
 *
 **/
static int32_t vhwaM2mLdcSubmitRequest(Vhwa_M2mLdcInstObj *instObj,
    Vhwa_M2mLdcQueueObj *qObj);

/**
 * \brief   Function to set input and output addresses in LDC and in TR
 *          It uses buffer addresses from the input and output frame lists
 *          set the them in the enable output channels.
 *
 * \param   instObj             Instance Object, used for getting base address
 * \param   hObj                Handle Object
 * \param   socInfo             Soc Information, used to get base
 *                              address for LDC
 * \param   inFrmList           Input Frame List
 * \param   outFrmList          Output Frame List
 *
 **/
static void vhwaM2mLdcSetAddress(Vhwa_M2mLdcHandleObj *hObj,
    Ldc_SocInfo *socInfo, const Fvid2_FrameList *inFrmList,
    const Fvid2_FrameList *outFrmList);

static int32_t vhwaM2mLdcAllocSl2(Vhwa_M2mLdcInstObj *instObj,
    const Vhwa_M2mLdcSl2AllocPrms *sl2AllocPrms);

/**
 * \brief   Local function to check LDC Create params like Driver Id and Instance Id.
 *
 * \param   drvId               Driver ID
 * \param   drvInstId           Instance ID
 *
 *  \return FVID2_SOK if successful, else suitable error code
 **/
static int32_t vhwaM2mLdcCheckCreatePrms(uint32_t drvId, uint32_t drvInstId);

/* Implementation of FVID2 APIs */

/**
 * \brief   FVID2 Create Function.
 *
 * \param   instObj             Instance object.
 *
 * \return  FVID2 Driver Handle.
 *
 **/
Fdrv_Handle vhwa_m2mLdcCreate(UInt32 drvId, UInt32 drvInstId,
    Ptr createArgs, Ptr createStatusArgs, const Fvid2_DrvCbParams *cbPrms);

/**
 * \brief   FVID2 Delete Function.
 *
 * \param   handle              FVID2 driver handle.
 *
 * \return  FVID2_SOK on success, else FVID2 error code
 *
 **/
Int32 vhwa_m2mLdcDelete(Fdrv_Handle handle, Ptr deleteArgs);

/**
 * \brief   FVID2 Control Function.
 *
 * \param   handle              FVID2 driver handle.
 *
 * \return  FVID2_SOK on success, else FVID2 error code
 *
 **/
Int32 vhwa_m2mLdcControl(Fdrv_Handle handle, UInt32 cmd, Ptr cmdArgs,
    Ptr cmdStatusArgs);

/**
 * \brief   FVID2 Process Request function.
 *
 * \param   handle              FVID2 driver handle.
 *
 * \return  FVID2_SOK on success, else FVID2 error code
 *
 **/
Int32 vhwa_m2mLdcProcessReq(Fdrv_Handle handle, Fvid2_FrameList *inFrmList,
    Fvid2_FrameList *outFrmList, uint32_t timeout);

/**
 * \brief   FVID2 Get Process Request function.
 *
 * \param   handle              FVID2 driver handle.
 *
 * \return  FVID2_SOK on success, else FVID2 error code
 *
 **/
Int32 vhwa_m2mLdcGetProcessReq(Fdrv_Handle handle,
    Fvid2_FrameList *inProcessList, Fvid2_FrameList *outProcessList,
    UInt32 timeout);

static void vhwaM2mLdcCalcHtsDepth(Vhwa_M2mLdcHandleObj *hObj);

static void vhwaM2mLdcInitChParamsFromLdcConfig(const Vhwa_M2mLdcInstObj *instObj,
    Vhwa_M2mLdcHandleObj *hObj);

static void vhwaM2mLdcCalcChSl2Params(const Vhwa_M2mLdcInstObj *instObj,
    Vhwa_M2mLdcHandleObj *hObj);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

Vhwa_M2mLdcHandleObj gM2mLdcHandleObj[VHWA_M2M_LDC_MAX_HANDLES];
Vhwa_M2mLdcInstObj   gM2mLdcInstObj;

Fvid2_DrvOps gM2mLdcFvid2DrvOps = {
    FVID2_VHWA_M2M_LDC_DRV_ID,
    /**< Unique driver Id. */
    vhwa_m2mLdcCreate,
    /**< FVID2 create function pointer. */
    vhwa_m2mLdcDelete,
    /**< FVID2 delete function pointer. */
    vhwa_m2mLdcControl,
    /**< FVID2 control function pointer. */
    NULL, NULL,
    /**< FVID2 queue function pointer. */
    vhwa_m2mLdcProcessReq,
    /**< FVID2 process request function pointer. */
    vhwa_m2mLdcGetProcessReq,
    /**< FVID2 get processed request function pointer. */
};

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int32_t Vhwa_m2mLdcInit(Vhwa_M2mLdcInitParams *initPrms)
{
    int32_t             status;
    uint32_t            cnt;
    SemaphoreP_Params   params;
    Vhwa_M2mLdcInstObj *instObj = NULL;

    if (NULL != initPrms)
    {
        instObj = &gM2mLdcInstObj;

        /* Reset all instance object to 0x0 */
        Fvid2Utils_memset(instObj, 0U, sizeof (Vhwa_M2mLdcInstObj));

        /* Mark pool flags as free */
        for (cnt = 0U; cnt < VHWA_M2M_LDC_MAX_HANDLES; cnt++)
        {
            gM2mLdcHandleObj[cnt].isUsed = (uint32_t) UFALSE;
        }

        /* Set HTS pipeline */
        instObj->pipeline = VHWA_M2M_LDC_HTS_PIPELINE;

        Ldc_getSocInfo(&instObj->socInfo);

        status = Fvid2_registerDriver(&gM2mLdcFvid2DrvOps);
        if (FVID2_SOK == status)
        {
            instObj->isRegistered = (uint32_t)UTRUE;
        }
        else
        {
            GT_0trace(VhwaLdcTrace, GT_ERR,
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
                GT_0trace(VhwaLdcTrace, GT_ERR,
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
                GT_0trace(VhwaLdcTrace, GT_ERR,
                    "Failed to allocate HWA Lock semaphore!!\n");
                status = FVID2_EALLOC;
            }
        }

        /* Create free and request queues */
        if (FVID2_SOK == status)
        {
            status = vhwaM2mLdcCreateQueues(instObj);
        }

        if (FVID2_SOK == status)
        {
            /* Initialize UDMA, ie allocate and initialize channels
               required for LDC output */
            status = Vhwa_m2mLdcUdmaInit(instObj, initPrms);
            if (FVID2_SOK != status)
            {
                GT_0trace(VhwaLdcTrace, GT_ERR,
                    "UDMA Initialization Failed !!\n");
            }
        }

        /* Register ISR handler for the given irq number */
        if (FVID2_SOK == status)
        {
            instObj->irqNum = initPrms->irqInfo.irqNum;
            instObj->vhwaIrqNum = initPrms->irqInfo.vhwaIrqNum;

            status = Vhwa_m2mLdcRegisterIsr(instObj);
        }

        /* Init is done, copy init params locally,
           enable VPAC and LDC in VPAC Top */
        if (FVID2_SOK == status)
        {
            Fvid2Utils_memcpy(&instObj->initPrms, initPrms,
                sizeof(Vhwa_M2mLdcInitParams));

            /* Enable LDC at VPAC Top*/
            CSL_vpacEnableModule(instObj->socInfo.vpacCntlRegs,
                VPAC_MODULE_LDC, (uint32_t)UTRUE);

            instObj->initDone = (uint32_t)UTRUE;
        }
    }
    else
    {
        status = FVID2_EBADARGS;
    }

    if (FVID2_SOK != status)
    {
        Vhwa_m2mLdcDeInit();
    }

    return (status);
}

void Vhwa_m2mLdcDeInit(void)
{
    Vhwa_M2mLdcInstObj *instObj = NULL;

    instObj = &gM2mLdcInstObj;

    if (instObj->openCnt > 0u)
    {
        GT_0trace(VhwaLdcTrace, GT_ERR,
            "Warning: All driver handles are not closed!!\n");
    }

    if ((uint32_t)UTRUE == instObj->isSl2AllocDone)
    {
        Vhwa_m2mLdcFreeSl2();
    }

    /* Stop UDMA channels */
    (void)Vhwa_m2mLdcStopCh(instObj);

    Vhwa_m2mLdcUnregisterIsr(instObj);

    (void)Vhwa_m2mLdcUdmaDeInit(instObj);

    if ((uint32_t)UTRUE == instObj->isRegistered)
    {
        (void)Fvid2_unRegisterDriver(&gM2mLdcFvid2DrvOps);
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

    vhwaM2mLdcDeleteQueues(instObj);

    /* Init all global variables to zero */
    Fvid2Utils_memset(instObj, 0U, sizeof (gM2mLdcInstObj));

    instObj->initDone = (uint32_t)UFALSE;
}

int32_t Vhwa_m2mLdcAllocSl2(const Vhwa_M2mLdcSl2AllocPrms *sl2AllocPrms)
{
    int32_t             status = FVID2_SOK;
    Vhwa_M2mLdcInstObj *instObj = NULL;

    instObj = &gM2mLdcInstObj;

    /* Cannot even lock, if init is not done */
    if ((uint32_t)UFALSE == instObj->initDone)
    {
        GT_0trace(VhwaLdcTrace, GT_ERR,
            "Driver init is not done!!\n");
        status = FVID2_EFAIL;
    }
    else
    {
        if ((uint32_t)UTRUE == instObj->isSl2AllocDone)
        {
            GT_0trace(VhwaLdcTrace, GT_ERR,
                "SL2 Memory is already allocated !!\n");
            status = FVID2_EFAIL;
        }

        /* Still need to check if provided sl2AllocPrms is not null */
        if (NULL == sl2AllocPrms)
        {
            GT_0trace(VhwaLdcTrace, GT_ERR,
                "SL2 Params is null !!\n");
            status = FVID2_EBADARGS;
        }
    }

    if (FVID2_SOK == status)
    {
        /* Lock instance semaphore */
        (void)SemaphoreP_pend(instObj->lock, SemaphoreP_WAIT_FOREVER);

        status = vhwaM2mLdcAllocSl2(instObj, sl2AllocPrms);

        /* Release instance semaphore */
        (void)SemaphoreP_post(instObj->lock);
    }

    return (status);
}

void Vhwa_m2mLdcFreeSl2(void)
{
    uint32_t            cnt;
    Vhwa_M2mLdcInstObj *instObj = NULL;

    instObj = &gM2mLdcInstObj;

    /* Lock instance semaphore */
    (void)SemaphoreP_pend(instObj->lock, SemaphoreP_WAIT_FOREVER);

    for (cnt = 0U; cnt < LDC_MAX_OUTPUT; cnt ++)
    {
        if (0U != instObj->sl2Size[cnt])
        {
            Vhwa_FreeSl2(instObj->sl2Addr[cnt], VHWA_SL2_INST_VPAC);
            instObj->sl2Size[cnt] = 0U;
        }
    }

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
Vhwa_M2mLdcHandleObj *Vhwa_m2mLdcGetHandleObj(uint32_t cnt)
{
    return &gM2mLdcHandleObj[cnt];
}
/* ========================================================================== */
/*                          FVID2 Function implementation                     */
/* ========================================================================== */

Fdrv_Handle vhwa_m2mLdcCreate(UInt32 drvId, UInt32 drvInstId,
    Ptr createArgs, Ptr createStatusArgs, const Fvid2_DrvCbParams *cbPrms)
{
    int32_t                 status = FVID2_SOK;
    Vhwa_M2mLdcInstObj     *instObj = NULL;
    Vhwa_M2mLdcHandleObj   *hObj = NULL;
    Vhwa_M2mLdcCreateArgs  *ldcCreateArgs = NULL;
    Fdrv_Handle             handle = NULL;
    Ldc_SocInfo            *socInfo = NULL;

    instObj = &gM2mLdcInstObj;
    socInfo = &instObj->socInfo;

    /* Check for errors */
    if ((NULL == createArgs) ||
        (NULL == cbPrms))
    {
        GT_0trace(VhwaLdcTrace, GT_ERR, "NULL Pointer !!\n");
        status = FVID2_EBADARGS;
    }
    else
    {
        status = vhwaM2mLdcCheckCreatePrms(drvId, drvInstId);
    }

    if (FVID2_SOK == status)
    {
        /* Open not allowed if init is not done */
        if ((uint32_t)UFALSE == instObj->initDone)
        {
            GT_0trace(VhwaLdcTrace, GT_ERR,
                "Vhwa_m2mLdcInit is not called\n");
            status  = FVID2_EFAIL;
        }

        ldcCreateArgs = (Vhwa_M2mLdcCreateArgs *)createArgs;
    }

    if (FVID2_SOK == status)
    {
        /* Take the instance semaphore */
        (void)SemaphoreP_pend(instObj->lock, SemaphoreP_WAIT_FOREVER);

        /* Allocate Handle Object */
        hObj = vhwaM2mLdcAllocHandleObj(instObj);

        if (NULL != hObj)
        {
            if (0U == instObj->openCnt)
            {
                /* Allocate SL2 Memory if not already allocated */
                if ((uint32_t)UFALSE == instObj->isSl2AllocDone)
                {
                    /* Initialize SL2 parameters with the defaul values */
                    Vhwa_M2mLdcSl2AllocPrmsInit(&instObj->sl2AllocPrms);

                    /* Allocate SL2 Parameters */
                    status = vhwaM2mLdcAllocSl2(instObj,
                        &instObj->sl2AllocPrms);
                }

                if (FVID2_SOK == status)
                {
                    /* On the first open, enable interrupts in HTS and
                     * start all UTC channels.
                     * Even if all UTC channels are enabled,
                     * only channels enabled
                     * in HTS will be used for the transfer */

                    /* Start UDMA Channels on the first handle Create */
                    status = Vhwa_m2mLdcStartCh(instObj);
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
                status = Vhwa_m2mLdcAllocUdmaMem(hObj);
            }

            if (FVID2_SOK == status)
            {
                Fvid2Utils_memcpy(&hObj->createArgs, ldcCreateArgs,
                    sizeof(Vhwa_M2mLdcCreateArgs));

                Fvid2Utils_memcpy(&hObj->cbPrms, cbPrms,
                    sizeof (hObj->cbPrms));

                instObj->openCnt ++;

                /* Setting it to UTRUE so that next request will update in HW. */
                hObj->isCfgUpdated = 1u;

            }
            else
            {
                /* Some error, so free up the handle object */
                vhwaM2mLdcFreeHandleObj(hObj);
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

Int32 vhwa_m2mLdcDelete(Fdrv_Handle handle, Ptr deleteArgs)
{
    int32_t                 status = FVID2_SOK;
    Vhwa_M2mLdcInstObj     *instObj = NULL;
    Vhwa_M2mLdcHandleObj   *hObj = NULL;
    Ldc_SocInfo            *socInfo = NULL;

    if (NULL != handle)
    {
        instObj = &gM2mLdcInstObj;
        hObj    = (Vhwa_M2mLdcHandleObj *)handle;
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
            vhwaM2mLdcFreeHandleObj(hObj);

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

Int32 vhwa_m2mLdcControl(Fdrv_Handle handle, UInt32 cmd, Ptr cmdArgs,
    Ptr cmdStatusArgs)
{
    int32_t                 status = FVID2_SOK;
    Vhwa_M2mLdcInstObj     *instObj = NULL;
    Vhwa_M2mLdcHandleObj   *hObj = NULL;
    Ldc_Config             *ldcCfg = NULL;
    Ldc_ErrEventParams     *eePrms = NULL;
    Ldc_RdBwLimitConfig    *rdBwLtCfg = NULL;
    Vhwa_HtsLimiter        *htsLimit = NULL;
    Ldc_RemapLutCfg        *lutCfg = NULL;

    if (NULL == handle)
    {
        status = FVID2_EBADARGS;
    }
    else
    {
        instObj = &gM2mLdcInstObj;
        hObj = (Vhwa_M2mLdcHandleObj *)handle;
    }
    if (FVID2_SOK == status)
    {
        /* Take the instance semaphore */
        (void)SemaphoreP_pend(instObj->lock, SemaphoreP_WAIT_FOREVER);

        switch (cmd)
        {
            /* Set LDC Parameters */
            case IOCTL_VHWA_M2M_LDC_SET_PARAMS:
            {
                if (NULL != cmdArgs)
                {
                    ldcCfg = (Ldc_Config *)cmdArgs;
                    status = vhwaM2mLdcSetParams(instObj, hObj, ldcCfg);
                }
                break;
            }

            /* Enable Register error events callback */
            case IOCTL_VHWA_M2M_LDC_REGISTER_ERR_CB:
            {
                if (NULL != cmdArgs)
                {
                    eePrms = (Ldc_ErrEventParams *)cmdArgs;

                    /* Copy Error Events in Handle object, so if the error
                     * comes on the next frame, it will be processed */
                    Fvid2Utils_memcpy(&hObj->eePrms, eePrms,
                        sizeof(Ldc_ErrEventParams));
                }
                break;
            }

            /* SET Read BW limit config */
            case IOCTL_VHWA_M2M_LDC_SET_RD_BW_LIMIT:
            {
                if (NULL != cmdArgs)
                {
                    rdBwLtCfg = (Ldc_RdBwLimitConfig *)cmdArgs;

                    status = CSL_ldcSetBwLimitConfig(
                        instObj->socInfo.ldcRegs, rdBwLtCfg);
                }
                break;
            }

            /* SET HTS Limiter config */
            case IOCTL_VHWA_M2M_LDC_SET_HTS_LIMIT:
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

            case IOCTL_VHWA_M2M_LDC_GET_PERFORMANCE:
            {
                uint32_t *perf = NULL;
                if (NULL != cmdArgs)
                {
                    perf = (uint32_t *)cmdArgs;
                    *perf = (uint32_t)hObj->perfNum;
                }
                break;
            }

            case IOCTL_VHWA_M2M_LDC_SYNC_START:
            {
                if (NULL != hObj->createArgs.getTimeStamp)
                {
                    hObj->perfNum = hObj->createArgs.getTimeStamp();
                }

                status = CSL_htsPipelineStart(instObj->socInfo.htsRegs, &hObj->htsCfg);
                break;
            }

            case IOCTL_LDC_SET_LUMA_TONEMAP_LUT_CFG:
            {
                lutCfg = (Ldc_RemapLutCfg *)cmdArgs;
                if (NULL != lutCfg)
                {
                    status = CSL_ldcSetLumaToneMapLutCfg(
                        instObj->socInfo.ldcRegs,
                        instObj->socInfo.lumaLutBaseAddr, lutCfg);
                }
                else
                {
                    status = FVID2_EBADARGS;
                }
                break;
            }

            case IOCTL_LDC_SET_CHROMA_TONEMAP_LUT_CFG:
            {
                lutCfg = (Ldc_RemapLutCfg *)cmdArgs;
                if (NULL != lutCfg)
                {
                    status = CSL_ldcSetChromaToneMapLutCfg(
                        instObj->socInfo.ldcRegs,
                        instObj->socInfo.chromaLutBaseAddr, lutCfg);
                }
                else
                {
                    status = FVID2_EBADARGS;
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


Int32 vhwa_m2mLdcProcessReq(Fdrv_Handle handle, Fvid2_FrameList *inFrmList,
    Fvid2_FrameList *outFrmList, uint32_t timeout)
{
    int32_t               status = FVID2_SOK;
    uint32_t              chId;
    uint32_t              outCnt;
    uint32_t              chCnt;
    uint32_t              semTimeout;
    Vhwa_M2mLdcInstObj   *instObj = NULL;
    Vhwa_M2mLdcHandleObj *hObj = NULL;
    Vhwa_M2mLdcQueueObj  *qObj = NULL;
    Ldc_Config           *ldcCfg = NULL;
    Fvid2_Frame          *frm = NULL;
    SemaphoreP_Status     semStatus;

    if ((NULL == handle) || (NULL == inFrmList) || (NULL == outFrmList))
    {
        status = FVID2_EBADARGS;
    }
    else
    {
        instObj = &gM2mLdcInstObj;
        hObj = (Vhwa_M2mLdcHandleObj *)handle;
        ldcCfg = &hObj->ldcCfg;
    }

    /* Check for null pointers */
    if (FVID2_SOK == status)
    {
        /* Input Buffer Addresses cannot be null */
        if ((NULL == inFrmList->frames[0U]) ||
            (0U == inFrmList->frames[0u]->addr[0U]))
        {
            status = FVID2_EBADARGS;
        }
        else
        {
            /* For YUV420 input, chroma buffer cannot be null */
            if ((FVID2_DF_YUV420SP_UV == ldcCfg->inFmt.dataFormat) ||
                (FVID2_DF_YUV422SP_UV == ldcCfg->inFmt.dataFormat))
            {
                if (0U == inFrmList->frames[0u]->addr[1U])
                {
                    status = FVID2_EBADARGS;
                }
            }
        }

        chId = 0u;
        for (outCnt = 0u; outCnt < LDC_MAX_OUTPUT; outCnt ++)
        {
            frm = outFrmList->frames[outCnt];
            if ((uint32_t)UTRUE == ldcCfg->enableOutput[outCnt])
            {
                if (NULL == frm)
                {
                    status = FVID2_EBADARGS;
                }
                else
                {
                    for (chCnt = 0U; chCnt < VHWA_MAX_CH_PER_OUTPUT; chCnt ++)
                    {
                        if (((uint32_t)UTRUE == hObj->chPrms[chId].isEnabled) &&
                            (0U == frm->addr[chCnt]))
                        {
                            status = FVID2_EBADARGS;
                            break;
                        }

                        chId ++;
                    }
                }
            }
            else
            {
                chId = chId + VHWA_MAX_CH_PER_OUTPUT;
            }
            if (FVID2_SOK != status)
            {
                break;
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
            qObj = (Vhwa_M2mLdcQueueObj *) Fvid2Utils_dequeue(instObj->freeQ);

            if (NULL == qObj)
            {
                GT_0trace(VhwaLdcTrace, GT_ERR,
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
            /* HW is free, submit request to the hardware */
            /* If previous handle and current handles are same, */
            if ((instObj->lastHndlObj == qObj->hObj) &&
                (0u == hObj->isCfgUpdated))
            {
                /** Set the addresses, Submit the TR
                 *  Start the pipeline */
                status = vhwaM2mLdcSubmitRequest(instObj, qObj);
            }
            else
            {
                /** Last handle was not same as new handle,
                 *  so require to recofigure all HW IPs */
                status = vhwaM2mLdcSetConfigInHW(instObj, qObj);

                if (FVID2_SOK == status)
                {
                    /* Cfg is successfully set in the HW, so
                     * resetting this flag */
                    hObj->isCfgUpdated = 0u;

                    /** Last handle was not same as new handle,
                     *  so require to recofigure all HW IPs */
                    status = vhwaM2mLdcSubmitRequest(instObj, qObj);
                }
            }
        }
    }

    return (status);
}

/** \brief Typedef for FVID2 get processed frames function pointer. */
Int32 vhwa_m2mLdcGetProcessReq(Fdrv_Handle handle,
    Fvid2_FrameList *inFrmList, Fvid2_FrameList *outFrmList,
    UInt32 timeout)
{
    int32_t                status = FVID2_SOK;
    uint32_t               cookie;
    Vhwa_M2mLdcInstObj    *instObj = NULL;
    Vhwa_M2mLdcHandleObj  *hObj = NULL;
    Vhwa_M2mLdcQueueObj   *qObj = NULL;

    if (NULL == handle)
    {
        status = FVID2_EBADARGS;
    }
    else
    {
        instObj = &gM2mLdcInstObj;
        hObj = (Vhwa_M2mLdcHandleObj *)handle;
    }

    if (FVID2_SOK == status)
    {
        /* Take the instance semaphore, so that no other
         * handle can submit request from the task context. */
        (void)SemaphoreP_pend(instObj->lock, SemaphoreP_WAIT_FOREVER);

        /* Disable interrupts before accessing queue */
        cookie = HwiP_disable();

        /* Dequeue the completed request from done queue */
        qObj = (Vhwa_M2mLdcQueueObj *) Fvid2Utils_dequeue(hObj->doneQ);

        /* Restore interrupts after updating node list */
        HwiP_restore(cookie);

        /* No buffers in the output queue */
        if (NULL == qObj)
        {
            /* Check if requests are pending with driver */
            if (0U == hObj->numPendReq)
            {
                /* Nothing is queued */
                GT_0trace(VhwaLdcTrace, GT_DEBUG,
                    "Nothing to dequeue. No request pending with driver!!\r\n");
                status = FVID2_ENO_MORE_BUFFERS;
            }
            else
            {
                /* If no request have completed, return try again */
                GT_0trace(VhwaLdcTrace, GT_DEBUG,
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
            status = Vhwa_m2mLdcPopRings(instObj, hObj);

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

static void vhwaM2mLdcCalcHtsDepth(Vhwa_M2mLdcHandleObj *hObj)
{
    uint32_t chCnt, hCnt, vCnt;
    Vhwa_M2mLdcChParams *chPrms = NULL;
    Vhwa_M2mLdcRegionParams *regPrms = NULL;

    for (chCnt = 0u; chCnt < VHWA_M2M_LDC_OUT_DMA_CH; chCnt ++)
    {
        chPrms = &hObj->chPrms[chCnt];
        if ((uint32_t)UTRUE == chPrms->isEnabled)
        {
            chPrms->minHtsDepth = 0xFFFFFFFFu;
            for (vCnt = 0u; vCnt < LDC_MAX_VERT_REGIONS; vCnt ++)
            {
                for (hCnt = 0u; hCnt < LDC_MAX_HORZ_REGIONS; hCnt ++)
                {
                    regPrms = &chPrms->regPrms[vCnt][hCnt];
                    if ((uint32_t)UTRUE == regPrms->isEnabled)
                    {
                        if (chPrms->minHtsDepth > regPrms->numSl2Blocks)
                        {
                            chPrms->minHtsDepth = regPrms->numSl2Blocks;
                        }
                    }
                }
            }
        }
    }
}

static void vhwaM2mLdcInitRegParams(Vhwa_M2mLdcChParams *chPrms,
    Ldc_Config *ldcCfg)
{
    uint32_t                 hCnt;
    uint32_t                 vCnt;
    Ldc_RegionConfig        *regCfg = NULL;
    Vhwa_M2mLdcRegionParams *regPrms = NULL;

    /* By default, all regions are disabled */
    for (vCnt = 0u; vCnt < LDC_MAX_VERT_REGIONS; vCnt ++)
    {
        for (hCnt = 0u; hCnt < LDC_MAX_HORZ_REGIONS; hCnt ++)
        {
            regPrms = &chPrms->regPrms[vCnt][hCnt];
            regPrms->isEnabled = (uint32_t)UFALSE;
        }
    }

    if ((uint32_t)UFALSE == ldcCfg->enableMultiRegions)
    {
        /* For non-multi region mode, regParams[0][0] is used */
        regPrms = &chPrms->regPrms[0u][0u];

        regPrms->width       = ldcCfg->outputFrameWidth;
        regPrms->height      = ldcCfg->outputFrameHeight;
        regPrms->blockWidth  = ldcCfg->outputBlockWidth;
        regPrms->blockHeight = ldcCfg->outputBlockHeight;

        /* Calculate pitch required for block */
        regPrms->blockWidthInBytes =
            Vhwa_m2mLdcCalcHorzSizeInBytes(regPrms->blockWidth,
                chPrms->ccsf, chPrms->dataFmt);

        if(FVID2_DF_RGI_B == chPrms->dataFmt)
        {
            regPrms->blockWidthInBytes = regPrms->blockWidthInBytes*2u;
        }

        regPrms->isEnabled = (uint32_t)UTRUE;
    }
    else
    {
        for (vCnt = 0u; vCnt < LDC_MAX_VERT_REGIONS; vCnt ++)
        {
            for (hCnt = 0u; hCnt < LDC_MAX_HORZ_REGIONS; hCnt ++)
            {
                regPrms = &chPrms->regPrms[vCnt][hCnt];
                regCfg = &ldcCfg->regCfg;

                regPrms->width       = regCfg->width[hCnt];
                regPrms->height      = regCfg->height[vCnt];
                if ((uint32_t)UTRUE == regCfg->enable[vCnt][hCnt])
                {
                    regPrms->blockWidth  = regCfg->blockWidth[vCnt][hCnt];
                    regPrms->blockHeight = regCfg->blockHeight[vCnt][hCnt];

                    /* Calculate pitch required for block */
                    regPrms->blockWidthInBytes =
                        Vhwa_m2mLdcCalcHorzSizeInBytes(regPrms->blockWidth,
                            chPrms->ccsf, chPrms->dataFmt);

                    if(FVID2_DF_RGI_B == chPrms->dataFmt)
                    {
                        regPrms->blockWidthInBytes = regPrms->blockWidthInBytes*2u;
                    }

                    regPrms->isEnabled = (uint32_t)UTRUE;
                }
            }
        }
    }
}

static void vhwaM2mLdcInitRegParamsForChroma(Vhwa_M2mLdcChParams *chPrms,
    Ldc_Config *ldcCfg)
{
    uint32_t                 hCnt;
    uint32_t                 vCnt;
    Ldc_RegionConfig        *regCfg = NULL;
    Vhwa_M2mLdcRegionParams *regPrms = NULL;

    /* By default, all regions are disabled. */
    for (vCnt = 0u; vCnt < LDC_MAX_VERT_REGIONS; vCnt ++)
    {
        for (hCnt = 0u; hCnt < LDC_MAX_HORZ_REGIONS; hCnt ++)
        {
            regPrms = &chPrms->regPrms[vCnt][hCnt];
            regPrms->isEnabled = (uint32_t)UFALSE;
        }
    }

    if ((uint32_t)UFALSE == ldcCfg->enableMultiRegions)
    {
        /* For non-multi region mode, regParams[0][0] is used */
        regPrms              = &chPrms->regPrms[0u][0u];

        regPrms->width       = ldcCfg->outputFrameWidth;
        regPrms->blockWidth  = ldcCfg->outputBlockWidth;
        if(FVID2_DF_YUV420SP_UV == chPrms->dataFmt)
        {
            regPrms->height      = ldcCfg->outputFrameHeight / 2u;
            regPrms->blockHeight = ldcCfg->outputBlockHeight / 2u;
        }
        else
        {
            regPrms->height      = ldcCfg->outputFrameHeight;
            regPrms->blockHeight = ldcCfg->outputBlockHeight;
        }

        /* Calculate pitch required for block */
        regPrms->blockWidthInBytes =
            Vhwa_m2mLdcCalcHorzSizeInBytes(regPrms->blockWidth,
                chPrms->ccsf, chPrms->dataFmt);

        regPrms->isEnabled = (uint32_t)UTRUE;
    }
    else
    {
        for (vCnt = 0u; vCnt < LDC_MAX_VERT_REGIONS; vCnt ++)
        {
            for (hCnt = 0u; hCnt < LDC_MAX_HORZ_REGIONS; hCnt ++)
            {
                regPrms = &chPrms->regPrms[vCnt][hCnt];
                regCfg = &ldcCfg->regCfg;

                regPrms->width       = regCfg->width[hCnt];
                if(FVID2_DF_YUV420SP_UV == chPrms->dataFmt)
                {
                    regPrms->height      = regCfg->height[vCnt] / 2u;
                }
                else
                {
                    regPrms->height      = regCfg->height[vCnt];
                }
                if ((uint32_t)UTRUE == regCfg->enable[vCnt][hCnt])
                {
                    regPrms->blockWidth  = regCfg->blockWidth[vCnt][hCnt];
                    if(FVID2_DF_YUV420SP_UV == chPrms->dataFmt)
                    {
                        regPrms->blockHeight = regCfg->blockHeight[vCnt][hCnt] / 2u;
                    }
                    else
                    {
                        regPrms->blockHeight = regCfg->blockHeight[vCnt][hCnt];
                    }

                    /* Calculate pitch required for block */
                    regPrms->blockWidthInBytes =
                        Vhwa_m2mLdcCalcHorzSizeInBytes(regPrms->blockWidth,
                            chPrms->ccsf, chPrms->dataFmt);

                    regPrms->isEnabled = (uint32_t)UTRUE;
                }
                else
                {
                    regPrms->isEnabled = (uint32_t)UFALSE;
                }
            }
        }
    }
}

static void vhwaM2mLdcInitChParamsFromLdcConfig(const Vhwa_M2mLdcInstObj *instObj,
    Vhwa_M2mLdcHandleObj *hObj)
{
    uint32_t             outCnt;
    uint32_t             chId;
    uint32_t             sl2MemSize;
    Ldc_Config          *ldcCfg = NULL;
    Vhwa_M2mLdcChParams *chPrms = NULL;

    if ((NULL != instObj) && (NULL != hObj))
    {
        ldcCfg = &hObj->ldcCfg;

        /* Enable the channel which are required */
        chId = 0u;
        for (outCnt = 0u; outCnt < LDC_MAX_OUTPUT; outCnt ++)
        {
            /* Enable channels only if output is enabled */
            if ((uint32_t)UTRUE == ldcCfg->enableOutput[outCnt])
            {
                sl2MemSize = vhwaM2mLdcCalcSl2MemSize(ldcCfg, outCnt);

                switch(ldcCfg->outFmt[outCnt].dataFormat)
                {
                    case FVID2_DF_YUV420SP_UV:
                    {
                        /* For YUV420 output, two channels are required */
                        chPrms              = &hObj->chPrms[chId];
                        chPrms->isEnabled   = (uint32_t)UTRUE;
                        chPrms->pitch       = ldcCfg->outFmt[outCnt].pitch[0u];
                        chPrms->ccsf        = ldcCfg->outFmt[outCnt].ccsFormat;
                        chPrms->dataFmt     = ldcCfg->outFmt[outCnt].dataFormat;
                        chPrms->sl2Addr     = instObj->sl2Addr[outCnt];
                        chPrms->sl2Size     = sl2MemSize;
                        vhwaM2mLdcInitRegParams(chPrms, ldcCfg);

                        /* Enable Channel for Chroma */
                        chPrms              = &hObj->chPrms[chId + 1u];
                        chPrms->isEnabled   = (uint32_t)UTRUE;
                        chPrms->pitch       = ldcCfg->outFmt[outCnt].pitch[1u];
                        if(UTRUE == ldcCfg->indChPrms.enable)
                        {
                            chPrms->ccsf        = ldcCfg->indOutChCcsf[outCnt];
                        }
                        else
                        {
                            chPrms->ccsf        = ldcCfg->outFmt[outCnt].ccsFormat;
                        }
                        chPrms->dataFmt     = ldcCfg->outFmt[outCnt].dataFormat;
                        chPrms->sl2Addr     = instObj->sl2Addr[outCnt] +
                                                                sl2MemSize;
                        chPrms->sl2Size     = sl2MemSize / 2U;
                        vhwaM2mLdcInitRegParamsForChroma(chPrms, ldcCfg);
                        break;
                    }
                    case FVID2_DF_YUV422SP_UV:
                    {
                        /* For YUV422 output, two channels are required */
                        chPrms              = &hObj->chPrms[chId];
                        chPrms->isEnabled   = (uint32_t)UTRUE;
                        chPrms->pitch       = ldcCfg->outFmt[outCnt].pitch[0u];
                        chPrms->ccsf        = ldcCfg->outFmt[outCnt].ccsFormat;
                        chPrms->dataFmt     = ldcCfg->outFmt[outCnt].dataFormat;
                        chPrms->sl2Addr     = instObj->sl2Addr[outCnt];
                        chPrms->sl2Size     = sl2MemSize;
                        vhwaM2mLdcInitRegParams(chPrms, ldcCfg);

                        /* Enable Channel for Chroma */
                        chPrms              = &hObj->chPrms[chId + 1u];
                        chPrms->isEnabled   = (uint32_t)UTRUE;
                        chPrms->pitch       = ldcCfg->outFmt[outCnt].pitch[1u];
                        if(UTRUE == ldcCfg->indChPrms.enable)
                        {
                            chPrms->ccsf        = ldcCfg->indOutChCcsf[outCnt];
                        }
                        else
                        {
                            chPrms->ccsf        = ldcCfg->outFmt[outCnt].ccsFormat;
                        }
                        chPrms->dataFmt     = ldcCfg->outFmt[outCnt].dataFormat;
                        chPrms->sl2Addr     = instObj->sl2Addr[outCnt] +
                                                                sl2MemSize;
                        chPrms->sl2Size     = sl2MemSize;
                        vhwaM2mLdcInitRegParamsForChroma(chPrms, ldcCfg);
                        break;
                    }
                    case FVID2_DF_2PLANES:
                    {
                        /* For Y1 + Y2 output, two channels are required */
                        chPrms              = &hObj->chPrms[chId];
                        chPrms->isEnabled   = (uint32_t)UTRUE;
                        chPrms->pitch       = ldcCfg->outFmt[outCnt].pitch[0u];
                        chPrms->ccsf        = ldcCfg->outFmt[outCnt].ccsFormat;
                        chPrms->dataFmt     = ldcCfg->outFmt[outCnt].dataFormat;
                        chPrms->sl2Addr     = instObj->sl2Addr[outCnt];
                        chPrms->sl2Size     = sl2MemSize;
                        vhwaM2mLdcInitRegParams(chPrms, ldcCfg);

                        /* Enable Channel for Y2 */
                        chPrms              = &hObj->chPrms[chId + 1u];
                        chPrms->isEnabled   = (uint32_t)UTRUE;
                        chPrms->pitch       = ldcCfg->outFmt[outCnt].pitch[1u];
                        if(UTRUE == ldcCfg->indChPrms.enable)
                        {
                            chPrms->ccsf        = ldcCfg->indOutChCcsf[outCnt];
                        }
                        else
                        {
                            chPrms->ccsf        = ldcCfg->outFmt[outCnt].ccsFormat;
                        }
                        chPrms->dataFmt     = ldcCfg->outFmt[outCnt].dataFormat;
                        chPrms->sl2Addr     = instObj->sl2Addr[outCnt] +
                                                                sl2MemSize;
                        chPrms->sl2Size     = sl2MemSize;
                        vhwaM2mLdcInitRegParams(chPrms, ldcCfg);
                        break;
                    }
                    case FVID2_DF_RGB:
                    {
                        /* For R + GB output, channel 0/1/3 channels are required */
                        chPrms              = &hObj->chPrms[chId];
                        chPrms->isEnabled   = (uint32_t)UTRUE;
                        chPrms->pitch       = ldcCfg->outFmt[outCnt].pitch[0u];
                        chPrms->ccsf        = ldcCfg->outFmt[outCnt].ccsFormat;
                        chPrms->dataFmt     = ldcCfg->outFmt[outCnt].dataFormat;
                        chPrms->sl2Addr     = instObj->sl2Addr[outCnt];
                        chPrms->sl2Size     = sl2MemSize;
                        vhwaM2mLdcInitRegParams(chPrms, ldcCfg);

                        /* Enable Channel for Y2 */
                        chPrms              = &hObj->chPrms[chId+1u];
                        chPrms->isEnabled   = (uint32_t)UTRUE;
                        chPrms->pitch       = ldcCfg->outFmt[outCnt].pitch[1u];
                        if(UTRUE == ldcCfg->indChPrms.enable)
                        {
                            chPrms->ccsf        = ldcCfg->indOutChCcsf[outCnt];
                        }
                        else
                        {
                            chPrms->ccsf        = ldcCfg->outFmt[outCnt].ccsFormat;
                        }
                        chPrms->dataFmt     = ldcCfg->outFmt[outCnt].dataFormat;
                        chPrms->sl2Addr     = instObj->sl2Addr[outCnt] +
                                                sl2MemSize;
                        chPrms->sl2Size     = sl2MemSize;
                        vhwaM2mLdcInitRegParams(chPrms, ldcCfg);

                        /* Enable Channel for Y3 */
                        chPrms              = &hObj->chPrms[chId+3u];
                        chPrms->isEnabled   = (uint32_t)UTRUE;
                        chPrms->pitch       = ldcCfg->outFmt[outCnt].pitch[2u];
                        if(UTRUE == ldcCfg->indChPrms.enable)
                        {
                            chPrms->ccsf        = ldcCfg->indOutChCcsf[outCnt];
                        }
                        else
                        {
                            chPrms->ccsf        = ldcCfg->outFmt[outCnt].ccsFormat;
                        }
                        chPrms->dataFmt     = ldcCfg->outFmt[outCnt].dataFormat;
                        chPrms->sl2Addr     = instObj->sl2Addr[outCnt] +
                                                      ((uint64_t)sl2MemSize * (uint64_t)2u);
                        chPrms->sl2Size     = sl2MemSize;
                        vhwaM2mLdcInitRegParams(chPrms, ldcCfg);
                        break;
                    }
                    case FVID2_DF_RGI_B:
                    {
                        /* For RG+B output, channel 0 and 3 channels are required */
                        chPrms              = &hObj->chPrms[chId];
                        chPrms->isEnabled   = (uint32_t)UTRUE;
                        chPrms->pitch       = ldcCfg->outFmt[outCnt].pitch[0u];
                        chPrms->ccsf        = ldcCfg->outFmt[outCnt].ccsFormat;
                        chPrms->dataFmt     = ldcCfg->outFmt[outCnt].dataFormat;
                        chPrms->sl2Addr     = instObj->sl2Addr[outCnt];
                        chPrms->sl2Size     = sl2MemSize * 2u;
                        vhwaM2mLdcInitRegParams(chPrms, ldcCfg);

                        /* Enable Channel 3 for Y3 */
                        chPrms              = &hObj->chPrms[chId + 3u];
                        chPrms->isEnabled   = (uint32_t)UTRUE;
                        chPrms->pitch       = ldcCfg->outFmt[outCnt].pitch[1u];
                        if(UTRUE == ldcCfg->indChPrms.enable)
                        {
                            chPrms->ccsf        = ldcCfg->indOutChCcsf[outCnt];
                        }
                        else
                        {
                            chPrms->ccsf        = ldcCfg->outFmt[outCnt].ccsFormat;
                        }
                        chPrms->dataFmt     = ldcCfg->outFmt[outCnt].dataFormat;
                        chPrms->sl2Addr     = instObj->sl2Addr[outCnt] +
                                                        ((uint64_t)sl2MemSize * (uint64_t)2u);
                        chPrms->sl2Size     = sl2MemSize;
                        vhwaM2mLdcInitRegParamsForChroma(chPrms, ldcCfg);
                        break;
                    }
                    case FVID2_DF_YUV422I_YUYV:
                    case FVID2_DF_YUV422I_UYVY:
                    {
                        chPrms              = &hObj->chPrms[chId];
                        chPrms->isEnabled   = (uint32_t)UTRUE;
                        chPrms->pitch       = ldcCfg->outFmt[outCnt].pitch[0u];
                        chPrms->ccsf        = ldcCfg->outFmt[outCnt].ccsFormat;
                        chPrms->dataFmt     = ldcCfg->outFmt[outCnt].dataFormat;
                        chPrms->sl2Addr     = instObj->sl2Addr[outCnt];
                        chPrms->sl2Size     = sl2MemSize;
                        vhwaM2mLdcInitRegParams(chPrms, ldcCfg);
                        break;
                    }
                    case FVID2_DF_LUMA_ONLY:
                    {
                        /* For chroma only mode, only one channel
                           needs to be enabled */
                        chPrms              = &hObj->chPrms[chId];
                        chPrms->isEnabled   = (uint32_t)UTRUE;
                        chPrms->pitch       = ldcCfg->outFmt[outCnt].pitch[0u];
                        chPrms->ccsf        = ldcCfg->outFmt[outCnt].ccsFormat;
                        chPrms->dataFmt     = ldcCfg->outFmt[outCnt].dataFormat;
                        chPrms->sl2Addr     = instObj->sl2Addr[outCnt];
                        chPrms->sl2Size     = sl2MemSize;
                        vhwaM2mLdcInitRegParams(chPrms, ldcCfg);
                        break;
                    }
                    case FVID2_DF_CHROMA_ONLY:
                    {
                        /* For chroma only mode, only second channel
                           needs to be enabled */
                        chPrms              = &hObj->chPrms[chId + 1u];
                        chPrms->isEnabled   = (uint32_t)UTRUE;
                        /* PS: Pitch is taken from offset1 in outFmt */
                        chPrms->pitch       = ldcCfg->outFmt[outCnt].pitch[1u];
                        chPrms->ccsf        = ldcCfg->outFmt[outCnt].ccsFormat;
                        chPrms->dataFmt     = ldcCfg->outFmt[outCnt].dataFormat;
                        chPrms->sl2Addr     = instObj->sl2Addr[outCnt];
                        chPrms->sl2Size     = sl2MemSize;
                        /* Chroma only mode, so no need to divide height by 2
                           as Application has provided correct height
                           for Chroma */
                        vhwaM2mLdcInitRegParams(chPrms, ldcCfg);
                        break;
                    }
                    default:
                    {
                        /* Nothing to do here */
                        break;
                    }
                }
            }

            chId += VHWA_MAX_CH_PER_OUTPUT;
        }
    }
}

static void vhwaM2mLdcCalcChSl2Params(const Vhwa_M2mLdcInstObj *instObj,
    Vhwa_M2mLdcHandleObj *hObj)
{
    uint32_t cnt;
    Vhwa_M2mLdcChParams *chPrms = NULL;

    if ((NULL != hObj) && (NULL != instObj))
    {
        for (cnt = 0U; cnt < VHWA_M2M_LDC_OUT_DMA_CH; cnt ++)
        {
            chPrms = &hObj->chPrms[cnt];
            if ((uint32_t)UTRUE == chPrms->isEnabled)
            {
                /* Calculated max block size */
                vhwaM2mLdcCalcMaxBlockPitch(chPrms);

                /* Calculate the number of lines available in SL2 */
                chPrms->sl2NumLines = chPrms->sl2Size /
                    chPrms->maxSl2BlockPitch;

                /* Calculate the number of SL2 blocks */
                vhwaM2mLdcCalcNumSl2Blocks(chPrms);
            }
        }
    }
}

/* Function to Allocate Handle Object */
static Vhwa_M2mLdcHandleObj *vhwaM2mLdcAllocHandleObj(
    const Vhwa_M2mLdcInstObj *instObj)
{
    uint32_t cnt;
    Vhwa_M2mLdcHandleObj *hObj = NULL;

    if (NULL != instObj)
    {
        for (cnt = 0U; cnt < VHWA_M2M_LDC_MAX_HANDLES; cnt ++)
        {
            if ((uint32_t)UFALSE == gM2mLdcHandleObj[cnt].isUsed)
            {
                hObj = &gM2mLdcHandleObj[cnt];
                Fvid2Utils_memset(hObj, 0x0, sizeof(Vhwa_M2mLdcHandleObj));
                gM2mLdcHandleObj[cnt].isUsed = (uint32_t)UTRUE;
                hObj->hIdx = cnt;
                break;
            }
        }
    }
    return (hObj);
}

/* Function to freeup allocated Handle Object */
static void vhwaM2mLdcFreeHandleObj(Vhwa_M2mLdcHandleObj *hObj)
{
    uint32_t cnt;

    if (NULL != hObj)
    {
        for (cnt = 0U; cnt < VHWA_M2M_LDC_MAX_HANDLES; cnt ++)
        {
            if (hObj == &gM2mLdcHandleObj[cnt])
            {
                hObj->isUsed = (uint32_t)UFALSE;;
                break;
            }
        }
    }
}


static int32_t vhwaM2mLdcCheckLdcCfg(const Vhwa_M2mLdcInstObj *instObj,
    Vhwa_M2mLdcHandleObj *hObj, const Ldc_Config *ldcCfg)
{
    int32_t     status = FVID2_SOK;
    uint32_t    outId;
    uint32_t    sl2Size;

    if ((NULL != instObj) && (NULL != hObj) && (NULL != ldcCfg))
    {
        /* Check if SL2 has sufficient size to run this block size */
        for (outId = 0U; outId < LDC_MAX_OUTPUT; outId ++)
        {
            if ((uint32_t)UTRUE == ldcCfg->enableOutput[outId])
            {
                sl2Size = vhwaM2mLdcCalcSl2MemSize(ldcCfg, outId);

                /* Above API does not provide sl2 size for YUV420 data */
                if ((FVID2_DF_YUV420SP_UV == ldcCfg->outFmt[outId].dataFormat))
                {
                    sl2Size = sl2Size * 3U / 2U;
                }
                else if ((FVID2_DF_YUV422SP_UV == ldcCfg->outFmt[outId].dataFormat) ||
                         (FVID2_DF_2PLANES == ldcCfg->outFmt[outId].dataFormat))
                {
                    sl2Size = sl2Size * 2U;
                }
                else if ((FVID2_DF_RGI_B == ldcCfg->outFmt[outId].dataFormat) ||
                         (FVID2_DF_RGB == ldcCfg->outFmt[outId].dataFormat))
                {
                    /* 2nd Channel will required twice the SL2 size */
                    sl2Size = sl2Size * 3U;
                }
                else
                {
                    /*Do Nothing*/
                }

                /* SL2 size must be greater than the required for the
                 * max block size */
                if (instObj->sl2Size[outId] < sl2Size)
                {
                    status = FVID2_EALLOC;
                }
            }
        }

        if (0U != hObj->numPendReq)
        {
            status = FVID2_EDEVICE_INUSE;
        }

        if (FVID2_SOK == status)
        {
            /* Initialize HTS configuration based on the new NF Config */
            vhwaM2mLdcSetHtsCfg(instObj, hObj, ldcCfg);
            /* Initialize LSE configuration based on the new NF Config */
            vhwaM2mLdcSetLseCfg(instObj, hObj, ldcCfg);
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

static int32_t vhwaM2mLdcSetParams(const Vhwa_M2mLdcInstObj *instObj,
    Vhwa_M2mLdcHandleObj *hObj, const Ldc_Config *ldcCfg)
{
    int32_t status = FVID2_EBADARGS;

    if ((NULL != instObj) && (NULL != hObj) && (NULL != ldcCfg))
    {
        status = vhwaM2mLdcCheckLdcCfg(instObj, hObj, ldcCfg);

        /* LDC configuration is verified */
        if (FVID2_SOK == status)
        {
            Fvid2Utils_memcpy(&hObj->ldcCfg, ldcCfg, sizeof(Ldc_Config));

            /* Initialize Channel parameters, based on the output format */
            vhwaM2mLdcInitChParamsFromLdcConfig(instObj, hObj);

            /* Calculate the SL2 information for each channel */
            vhwaM2mLdcCalcChSl2Params(instObj, hObj);

            /* Calculate the HTS depth,
             * HTS depth should be configured with
             * minimum of all region depth/Sl2 number of blocks.
             * Number of blocks for each regions are already calculated in
             * vhwaM2mLdcCalcChSl2Params. This API calculates min of
             * them all. */
            vhwaM2mLdcCalcHtsDepth(hObj);

            /* Calculate TR information for each channel */
            vhwaM2mLdcCalcChTrParams(instObj, hObj);

            /* Now set the HTS and LSE configuration, based on
             * the calculated parameters */

            /* Again set the HTS Configuration,
             * Based on the calculated parameters. */
            vhwaM2mLdcSetHtsCfg(instObj, hObj, ldcCfg);

            /* Set the LSE Configuration,
             * Based on the calculated parameters. */
            vhwaM2mLdcSetLseCfg(instObj, hObj, ldcCfg);

            /* Now create TR and initialized it from the calculated parameters
             * */
            Vhwa_m2mLdcSetTrDesc(instObj, hObj);

            /* Now set flag so that params will be loaded in HW next time
             * */
            hObj->isCfgUpdated = 1u;

        }
        else /* incorrect LDC configuration, restore HTS/LSE config */
        {
            /* Reset HTS config based on local correct LDC config */
            vhwaM2mLdcSetHtsCfg(instObj, hObj, &hObj->ldcCfg);
            /* Reset LSE config based on local correct LDC config */
            vhwaM2mLdcSetLseCfg(instObj, hObj, &hObj->ldcCfg);
        }
    }

    return (status);
}

static int32_t vhwaM2mLdcSetConfigInHW(Vhwa_M2mLdcInstObj *instObj,
    const Vhwa_M2mLdcQueueObj *qObj)
{
    int32_t               status;
    Ldc_SocInfo          *socInfo = NULL;
    Vhwa_M2mLdcHandleObj *hObj = NULL;

    /* Null pointer check */
    GT_assert(VhwaLdcTrace, (NULL != instObj));
    GT_assert(VhwaLdcTrace, (NULL != qObj));
    GT_assert(VhwaLdcTrace, (NULL != qObj->hObj));

    socInfo = &instObj->socInfo;
    hObj = qObj->hObj;

    /* Configure LDC Core */
    status = CSL_ldcSetConfig(socInfo->ldcRegs, &hObj->ldcCfg);

    if (FVID2_SOK == status)
    {
        /* Configure HWA Common Wrapper LSE */
        status = CSL_lseSetConfig(socInfo->lseRegs, &hObj->lseCfg);
    }

    if (FVID2_SOK == status)
    {
        /* Configure HTS */
        status = CSL_htsSetThreadConfig(socInfo->htsRegs, &hObj->htsCfg);
    }

    if (FVID2_SOK == status)
    {
        /* Start HTS Scheduler */
        status = CSL_htsThreadStart(socInfo->htsRegs, &hObj->htsCfg);
    }

    return (status);
}

static int32_t vhwaM2mLdcSubmitRequest(Vhwa_M2mLdcInstObj *instObj,
    Vhwa_M2mLdcQueueObj *qObj)
{
    int32_t               status;
    Ldc_SocInfo          *socInfo = NULL;
    Vhwa_M2mLdcHandleObj *hObj = NULL;

    /* Null pointer check */
    GT_assert(VhwaLdcTrace, (NULL != instObj));
    GT_assert(VhwaLdcTrace, (NULL != qObj));
    GT_assert(VhwaLdcTrace, (NULL != qObj->hObj));

    socInfo = &instObj->socInfo;
    hObj = qObj->hObj;

    /* Set Input and output addresses */
    vhwaM2mLdcSetAddress(hObj, socInfo, &qObj->inFrmList,
        &qObj->outFrmList);

    /* Submit Rings to the Ring Accelerator */
    status = Vhwa_m2mLdcSubmitRing(instObj, hObj);

    if (FVID2_SOK == status)
    {
        /* Start the core */
        status = CSL_ldcStart(socInfo->ldcRegs);

        if (FVID2_SOK == status)
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
            status = CSL_htsPipelineStart(socInfo->htsRegs, &hObj->htsCfg);
            #endif
        }

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

static uint32_t vhwaM2mLdcCalcSl2MemSize(const Ldc_Config *cfg,
                                    uint32_t outId)
{
    uint32_t blockWidth;
    uint32_t blockHeight;
    uint32_t blockSize;

    GT_assert(VhwaLdcTrace, (NULL != cfg));
    GT_assert(VhwaLdcTrace, (outId < LDC_MAX_OUTPUT));

    /* Size has to be calculated from max of block width and
     * max of block height,
     * this is because pitch is common for all regions,
     * so, for example, 128x16 block size > 16x64, but
     * for 16x64 block size, sl2 memory required is 128x64.
     * so it is calculated from max of block width and max of block height */
    vhwaM2mLdcGetBlockSize(cfg, &blockWidth, &blockHeight);

    if ((FVID2_DF_YUV420SP_UV == cfg->outFmt[outId].dataFormat) ||
        (FVID2_DF_YUV422SP_UV == cfg->outFmt[outId].dataFormat) ||
        (FVID2_DF_2PLANES == cfg->outFmt[outId].dataFormat) ||
        (FVID2_DF_RGI_B == cfg->outFmt[outId].dataFormat) ||
        (FVID2_DF_RGB == cfg->outFmt[outId].dataFormat) ||
        (FVID2_DF_YUV422I_UYVY == cfg->outFmt[outId].dataFormat) ||
        (FVID2_DF_LUMA_ONLY == cfg->outFmt[outId].dataFormat) ||
        (FVID2_DF_CHROMA_ONLY == cfg->outFmt[outId].dataFormat))
    {
        if (FVID2_DF_YUV422I_UYVY == cfg->outFmt[outId].dataFormat)
        {
            blockWidth = blockWidth * 2U;
        }
        if (FVID2_CCSF_BITS12_PACKED == cfg->outFmt[outId].ccsFormat)
        {
            blockWidth = blockWidth * 3U /2U;
        }
        else if (FVID2_CCSF_BITS12_UNPACKED16 ==
                    cfg->outFmt[outId].ccsFormat)
        {
            blockWidth = blockWidth * 2U;
        }
        else /* 8bit packed */
        {
            /* Nothing to do here */
        }
    }
    else
    {
    }

    /* Make block width aligned to SL2 pitch requirement */
    blockWidth = Vhwa_calcSl2Pitch(blockWidth);

    /* TODO: Add support for Depth more than 2 */
    blockSize = blockWidth * blockHeight * 2U;

    return (blockSize);
}

static void vhwaM2mLdcGetBlockSize(const Ldc_Config *cfg,
                                   uint32_t *blockWidth,
                                   uint32_t *blockHeight)
{
    uint32_t cnt0, cnt1, width, height;

    GT_assert(VhwaLdcTrace, (NULL != cfg));
    GT_assert(VhwaLdcTrace, (NULL != blockWidth));
    GT_assert(VhwaLdcTrace, (NULL != blockHeight));

    width = 0U;
    height = 0U;

    if (UTRUE == cfg->enableMultiRegions)
    {
        for (cnt0 = 0; cnt0 < LDC_MAX_VERT_REGIONS; cnt0 ++)
        {
            for (cnt1 = 0; cnt1 < LDC_MAX_HORZ_REGIONS; cnt1 ++)
            {
                if (UTRUE == cfg->regCfg.enable[cnt0][cnt1])
                {
                    if (width < cfg->regCfg.blockWidth[cnt0][cnt1])
                    {
                        width = cfg->regCfg.blockWidth[cnt0][cnt1];
                    }
                    if (height < cfg->regCfg.blockHeight[cnt0][cnt1])
                    {
                        height = cfg->regCfg.blockHeight[cnt0][cnt1];
                    }
                }
            }
        }
    }
    else
    {
        width = cfg->outputBlockWidth;
        height = cfg->outputBlockHeight;
    }

    *blockWidth = width;
    *blockHeight = height;
}

static void vhwaM2mLdcCalcNumSl2Blocks(Vhwa_M2mLdcChParams *chPrms)
{
    uint32_t                 hCnt;
    uint32_t                 vCnt;
    Vhwa_M2mLdcRegionParams *regPrms = NULL;

    for (vCnt = 0u; vCnt < LDC_MAX_VERT_REGIONS; vCnt ++)
    {
        for (hCnt = 0u; hCnt < LDC_MAX_HORZ_REGIONS; hCnt ++)
        {
            regPrms = &chPrms->regPrms[vCnt][hCnt];
            if ((uint32_t)UTRUE == regPrms->isEnabled)
            {
                regPrms->numSl2Blocks = chPrms->sl2NumLines /
                    regPrms->blockHeight;
            }
        }
    }
}

static void vhwaM2mLdcCalcMaxBlockPitch(Vhwa_M2mLdcChParams *chPrms)
{
    uint32_t                 hCnt;
    uint32_t                 vCnt;
    uint32_t                 maxBlockWidth = 0u;
    Vhwa_M2mLdcRegionParams *regPrms = NULL;

    for (vCnt = 0u; vCnt < LDC_MAX_VERT_REGIONS; vCnt ++)
    {
        for (hCnt = 0u; hCnt < LDC_MAX_HORZ_REGIONS; hCnt ++)
        {
            regPrms = &chPrms->regPrms[vCnt][hCnt];

            if ((uint32_t)UTRUE == regPrms->isEnabled)
            {
                if (regPrms->blockWidthInBytes > maxBlockWidth)
                {
                    maxBlockWidth = regPrms->blockWidthInBytes;
                }
            }
        }
    }

    /* Block pitch requires to be LSE pitch aligned/64 bytes */
    chPrms->maxSl2BlockPitch = CSL_lseMakePitchAligned(maxBlockWidth);
}

static void vhwaM2mLdcCalcChTrParams(const Vhwa_M2mLdcInstObj *instObj,
    Vhwa_M2mLdcHandleObj *hObj)
{
    uint32_t             chcnt;
    Vhwa_M2mLdcChParams *chPrms = NULL;

    if ((NULL != hObj) && (NULL != instObj))
    {
        /* Calculate Number of TRs for each region and channels and
           calculate the address offset for each channel for each TR */
        for (chcnt = 0u; chcnt < VHWA_M2M_LDC_OUT_DMA_CH; chcnt ++)
        {
            chPrms = &hObj->chPrms[chcnt];

            if ((uint32_t)UTRUE == chPrms->isEnabled)
            {
                vhwaM2mLdcCalcRegionTrParams(chPrms);
            }
        }
    }
}

static void vhwaM2mLdcCalcRegionTrParams(Vhwa_M2mLdcChParams *chPrms)
{
    uint32_t                 hCnt;
    uint32_t                 vCnt;
    uint32_t                 numTr;
    uint32_t                 numHorzBlocks;
    uint32_t                 numVertBlocks;
    uint32_t                 totalBlocks;
    uint32_t                 remBlocks;
    uint32_t                 remHorzBlocks;
    uint32_t                 width = 0U;
    uint32_t                 height = 0U;
    uint32_t                 startAddrOffset;
    Vhwa_M2mLdcRegionParams *regPrms = NULL;

    /* This function calculates the total number of TRs required in
     * each region and the parameters for each TRs, like icnt, dims,
     * start offsets etc.
     * It uses below logic,
     * 1, Minimum 1 TR is required for each region.
     * 2, Second TR is required if frame height is not
     *    multiple of region height. In this case, first TR is
     *    used to transfer all block rows, except last row and
     *    second TR is used to transfer last row.
     * 3, Third TR is required if the last row of blocks does not
     *    start from SL2 start address ie second last row does not
     *    end on the last SL2 block. In this case,
     *    second TR is used to transfer one or two block such that
     *    start address of the next TR is SL2 start address. Then third
     *    TR is used to transfer remaining blocks of the last row.
     * */

    chPrms->totalTRs = 0u;
    for (vCnt = 0u; vCnt < LDC_MAX_VERT_REGIONS; vCnt ++)
    {
        width = 0u;

        /* Calculate the total height,
           used in calculating start offset */
        if (vCnt > 0u)
        {
            height = height + chPrms->regPrms[vCnt - 1u][0u].height;
        }

        for (hCnt = 0u; hCnt < LDC_MAX_HORZ_REGIONS; hCnt ++)
        {
            /* Calculate the total width
               Used in calculating start offset */
            if (hCnt > 0u)
            {
                width = width + chPrms->regPrms[0u][hCnt - 1u].width;
            }

            regPrms = &chPrms->regPrms[vCnt][hCnt];

            /* This is the start address offset for each region */
            startAddrOffset = (height * chPrms->pitch) +
                Vhwa_m2mLdcCalcHorzSizeInBytes(width, chPrms->ccsf,
                    chPrms->dataFmt);

            numTr = 0u;
            if ((uint32_t)UTRUE == regPrms->isEnabled)
            {
                /* Calculate total number of 2D Blocks in a frame,
                 * On horizontal side, make sure pitch is multiple
                 * of block width, even through frame width is non
                 * multiple of block width */
                numHorzBlocks = Vhwa_ceil(regPrms->width, regPrms->blockWidth);
                numVertBlocks = regPrms->height / regPrms->blockHeight;
                totalBlocks = numHorzBlocks * numVertBlocks;

                regPrms->sicnt2[numTr] = regPrms->numSl2Blocks;
                regPrms->sicnt3[numTr] =
                    Vhwa_ceil(totalBlocks, regPrms->numSl2Blocks);
                regPrms->dicnt2[numTr] = numHorzBlocks;
                regPrms->dicnt3[numTr] = numVertBlocks;

                /* Calculate the buffer offset in the output buffer */
                regPrms->bufAddrOff[numTr] = startAddrOffset;

                /* SL2 Address for the first block is always start address */
                regPrms->sl2Addr[numTr] = chPrms->sl2Addr;

                /* Minimum 1 TR is required for this transfer */
                numTr ++;

                /* More than one TR is required if output frame
                   height is not multiple of block height */
                if ((numVertBlocks * regPrms->blockHeight) != regPrms->height)
                {
                    /* Calculate the address offset in the output buffer */
                    regPrms->bufAddrOff[numTr] = startAddrOffset +
                        (chPrms->pitch * (numVertBlocks *
                            regPrms->blockHeight));

                    remBlocks = totalBlocks % regPrms->numSl2Blocks;
                    if (0u == remBlocks)
                    {
                        /* Since the last row blocks starts from the
                           SL2 start address, only one TR is required to
                           transfer last row of blocks */
                        regPrms->sicnt2[numTr] = regPrms->numSl2Blocks;
                        regPrms->sicnt3[numTr] =
                            Vhwa_ceil(numHorzBlocks, regPrms->numSl2Blocks);
                        regPrms->dicnt2[numTr] = numHorzBlocks;
                        regPrms->dicnt3[numTr] = 1u;

                        /* SL2 Offset is still 0, since last blocks of row
                           starts at start address of sl2 */
                        regPrms->sl2Addr[numTr] = chPrms->sl2Addr;

                        numTr ++;
                    }
                    else
                    {
                        /* Since the last row blocks does not start from the
                           SL2 start address, one TR is required to
                           bring TR start address to SL2 start address
                           and another TR require for the remaining
                           blocks of the last row. */

                        /* SL2 address is where the last block of the last row
                           ended in SL2 */
                        regPrms->sl2Addr[numTr] = chPrms->sl2Addr +
                                        ((uint64_t)chPrms->maxSl2BlockPitch *
                                         (uint64_t)regPrms->blockHeight *
                                         (uint64_t)remBlocks);

                        remBlocks = regPrms->numSl2Blocks - remBlocks;

                        regPrms->sicnt2[numTr] = remBlocks;
                        regPrms->sicnt3[numTr] = 1u;
                        regPrms->dicnt2[numTr] = remBlocks;
                        regPrms->dicnt3[numTr] = 1u;

                        /* One TR for flushing remainder of blocks to bring back
                           to start address of SL2 */
                        numTr ++;

                        /* Last TR required only if there are remaining
                           blocks to be transferred */
                        remHorzBlocks = numHorzBlocks - remBlocks;
                        if (0u != remHorzBlocks)
                        {
                            regPrms->sicnt2[numTr] = regPrms->numSl2Blocks;
                            regPrms->sicnt3[numTr] =
                                Vhwa_ceil(remHorzBlocks, regPrms->numSl2Blocks);
                            regPrms->dicnt2[numTr] = remHorzBlocks;
                            regPrms->dicnt3[numTr] = 1u;

                            regPrms->bufAddrOff[numTr] =
                                regPrms->bufAddrOff[numTr - 1u] +
                                (remBlocks * regPrms->blockWidthInBytes);

                            /* SL2 address for this region is start of
                             * SL2 address as extra TR is added to start
                             * from the first location. */
                            regPrms->sl2Addr[numTr] = chPrms->sl2Addr;

                            numTr ++;
                        }
                    }
                }
            }

            regPrms->numTr = numTr;
            chPrms->totalTRs += numTr;
        }
    }
}

static void vhwaM2mLdcSetLseCfg(const Vhwa_M2mLdcInstObj *instObj,
    Vhwa_M2mLdcHandleObj *hObj, const Ldc_Config *ldcCfg)
{
    uint32_t               chCnt;
    Vhwa_M2mLdcCreateArgs *createArgs = NULL;
    CSL_LseConfig         *lseCfg = NULL;
    Vhwa_M2mLdcChParams   *chPrms = NULL;

    if ((NULL != instObj) && (NULL != hObj) && (NULL != ldcCfg))
    {
        lseCfg = &hObj->lseCfg;
        createArgs = &hObj->createArgs;

        /* Initialize LSE configuration with the default values */
        CSL_lseConfigInit(lseCfg);

        /* For LDC, Max number of output channels are 4 */
        lseCfg->numOutCh = VHWA_M2M_LDC_OUT_DMA_CH;

        /* Enable PSA if it is enabled in create params */
        lseCfg->enablePsa = createArgs->enablePsa;

        /* Enable the channel which are required */
        for (chCnt = 0u; chCnt < VHWA_M2M_LDC_OUT_DMA_CH; chCnt ++)
        {
            chPrms = &hObj->chPrms[chCnt];
            if ((uint32_t)UTRUE == chPrms->isEnabled)
            {
                lseCfg->outChCfg[chCnt].enable = (uint32_t)UTRUE;
                lseCfg->outChCfg[chCnt].ccsf =
                    (Fvid2_ColorCompStorageFmt)chPrms->ccsf;
                lseCfg->outChCfg[chCnt].lineOffset = chPrms->maxSl2BlockPitch;

                lseCfg->outChCfg[chCnt].circBufSize = chPrms->sl2NumLines;
                lseCfg->outChCfg[chCnt].bufAddr = (uint32_t)chPrms->sl2Addr;
                lseCfg->outChCfg[chCnt].useMultiBprParams = (uint32_t)UFALSE;
                lseCfg->outChCfg[chCnt].enableTDoneEoBPR = (uint32_t)UTRUE;
                lseCfg->outChCfg[chCnt].bpr = 1u;
                lseCfg->outChCfg[chCnt].enableVertWrap = (uint32_t)UFALSE;

                if ((FVID2_DF_YUV422I_UYVY ==
                        (Fvid2_DataFormat)chPrms->dataFmt) ||
                    (FVID2_DF_YUV422I_YUYV ==
                    (Fvid2_DataFormat)chPrms->dataFmt))
                {
                    lseCfg->outChCfg[chCnt].enableYuv422Out = (uint32_t)UTRUE;
                    lseCfg->outChCfg[chCnt].yuv422DataFmt =
                        (Fvid2_DataFormat)chPrms->dataFmt;
                }
                else if(FVID2_DF_RGI_B == (Fvid2_DataFormat)chPrms->dataFmt)
                {
                    lseCfg->outChCfg[chCnt].enableYuv422Out = (uint32_t)UTRUE;
                    lseCfg->outChCfg[chCnt].yuv422DataFmt =
                                                        FVID2_DF_YUV422I_YUYV;
                }
                else
                {
                    lseCfg->outChCfg[chCnt].enableYuv422Out = (uint32_t)UFALSE;
                }
            }
        }
    }
}

static void vhwaM2mLdcSetHtsCfg(const Vhwa_M2mLdcInstObj *instObj,
    Vhwa_M2mLdcHandleObj *hObj, const Ldc_Config *ldcCfg)
{
    uint32_t             chCnt;
    CSL_HtsSchConfig    *htsCfg = NULL;
    Vhwa_M2mLdcChParams *chPrms = NULL;

    if ((NULL != instObj) && (NULL != hObj) && (NULL != ldcCfg))
    {
        htsCfg = &hObj->htsCfg;

        CSL_htsSchConfigInit(htsCfg);

        htsCfg->schId           = CSL_HTS_HWA_SCH_LDC0;

        htsCfg->pipeline        = instObj->pipeline;
        htsCfg->enableHop       = (uint32_t)UFALSE;
        htsCfg->numHop          = 0u;

        /* Enable the channel which are required */
        for (chCnt = 0; chCnt < VHWA_M2M_LDC_OUT_DMA_CH; chCnt ++)
        {
            chPrms = &hObj->chPrms[chCnt];
            if ((uint32_t)UTRUE == chPrms->isEnabled)
            {
                htsCfg->prodCfg[chCnt].enable       = (uint32_t)UTRUE;
                htsCfg->prodCfg[chCnt].consId       = CSL_HTS_CONS_IDX_UDMA;

                htsCfg->prodCfg[chCnt].threshold    = 1u;
                htsCfg->prodCfg[chCnt].cntPreLoad   = 0u;
                htsCfg->prodCfg[chCnt].cntPostLoad  = 0u;

                htsCfg->prodCfg[chCnt].depth        = chPrms->minHtsDepth;
                htsCfg->prodCfg[chCnt].countDec     = 1U;

                htsCfg->dmaConsCfg[chCnt].enable    = (uint32_t)UTRUE;
                htsCfg->dmaConsCfg[chCnt].dmaChNum  =
                    instObj->utcCh[chCnt];
                htsCfg->dmaConsCfg[chCnt].pipeline  = instObj->pipeline;
                htsCfg->dmaConsCfg[chCnt].enableStream = (uint32_t)UFALSE;
                htsCfg->dmaConsCfg[chCnt].prodId    = CSL_HTS_PROD_IDX_UDMA;
                    /* Default mapping for DMA */
            }
        }
    }
}

static int32_t vhwaM2mLdcCreateQueues(Vhwa_M2mLdcInstObj *instObj)
{
    int32_t              status;
    uint32_t             qCnt;
    Vhwa_M2mLdcQueueObj *qObj;

    /* NULL pointer check */
    GT_assert(VhwaLdcTrace, (NULL != instObj));

    instObj->reqQ  = NULL;
    instObj->freeQ = NULL;

    /* Create Free queue */
    status = Fvid2Utils_constructQ(&instObj->freeQObj);
    GT_assert(VhwaLdcTrace, (FVID2_SOK == status));

    instObj->freeQ = &instObj->freeQObj;

    /* Create input queue */
    status = Fvid2Utils_constructQ(&instObj->reqQObj);
    GT_assert(VhwaLdcTrace, (FVID2_SOK == status));

    instObj->reqQ = &instObj->reqQObj;

    /* Initialize and queue the allocate queue object to free Q */
    for(qCnt = 0U; qCnt < VHWA_M2M_LDC_UDMA_RING_ENTRIES; qCnt ++)
    {
        qObj = &instObj->ldcQObj[qCnt];

        Fvid2Utils_memset(qObj, 0x0, sizeof(Vhwa_M2mLdcQueueObj));

        Fvid2Utils_queue(instObj->freeQ, &qObj->qElem, qObj);
    }

    return (status);
}

static void vhwaM2mLdcDeleteQueues(Vhwa_M2mLdcInstObj *instObj)
{
    Vhwa_M2mLdcQueueObj *qObj = NULL;

    /* NULL pointer check */
    GT_assert(VhwaLdcTrace, (NULL != instObj));

    if(NULL != instObj->freeQ)
    {
        /* Free-up all the queued free queue objects */
        do
        {
            qObj = (Vhwa_M2mLdcQueueObj *) Fvid2Utils_dequeue(instObj->freeQ);
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
            qObj = (Vhwa_M2mLdcQueueObj *) Fvid2Utils_dequeue(instObj->reqQ);
        } while (NULL != qObj);

        /* Delete the input Q */
        Fvid2Utils_destructQ(instObj->reqQ);
        instObj->reqQ = NULL;
    }
}

static void vhwaM2mLdcSetAddress(Vhwa_M2mLdcHandleObj *hObj,
    Ldc_SocInfo *socInfo, const Fvid2_FrameList *inFrmList,
    const Fvid2_FrameList *outFrmList)
{
    uint32_t     chId;
    uint32_t     chCnt;
    uint32_t     outCnt;
    Fvid2_Frame *frm = NULL;
    Ldc_Config  *ldcCfg = NULL;

    /* NULL pointer check */
    GT_assert(VhwaLdcTrace, (NULL != hObj));
    GT_assert(VhwaLdcTrace, (NULL != socInfo));
    GT_assert(VhwaLdcTrace, (NULL != inFrmList));
    GT_assert(VhwaLdcTrace, (NULL != outFrmList));

    ldcCfg = &hObj->ldcCfg;
    frm = inFrmList->frames[0U];

    /* Configure Input Buffer address in LDC */
    CSL_ldcSetInAddress(socInfo->ldcRegs, frm->addr[0], frm->addr[1]);

    chId = 0u;
    for (outCnt = 0u; outCnt < LDC_MAX_OUTPUT; outCnt ++)
    {
        frm = outFrmList->frames[outCnt];
        for (chCnt = 0u;
                ((uint32_t)UTRUE == ldcCfg->enableOutput[outCnt]) &&
                (chCnt < VHWA_MAX_CH_PER_OUTPUT); chCnt ++)
        {
            if ((uint32_t)UTRUE == hObj->chPrms[chId].isEnabled)
            {
                hObj->chPrms[chId].startAddr = frm->addr[chCnt];
            }

            chId ++;
        }
    }

    /* For Y1 + Y2 + Y3 channel 3 is also required */
    if(FVID2_DF_RGB == ldcCfg->outFmt[0].dataFormat)
    {
        hObj->chPrms[3u].startAddr = outFrmList->frames[0]->addr[2u];
    }
    else if(FVID2_DF_RGI_B == ldcCfg->outFmt[0].dataFormat)
    {
        hObj->chPrms[3u].startAddr = outFrmList->frames[0]->addr[1u];
    }
    else
    {
        /*Do Nothing*/
    }

    /* Configure Buffer Addresses in TR Record */
    Vhwa_m2mLdcSetOutputAddress(hObj);
}

static int32_t vhwaM2mLdcAllocSl2(Vhwa_M2mLdcInstObj *instObj,
    const Vhwa_M2mLdcSl2AllocPrms *sl2AllocPrms)
{
    int32_t             status = FVID2_SOK;
    uint32_t            cnt;
    uint32_t            size;
    uint32_t            sl2MemSize = 0U;
    uint64_t            sl2Addr;
    uint32_t            blockWidthInBytes;

    GT_assert(VhwaLdcTrace, (NULL != instObj));
    GT_assert(VhwaLdcTrace, (NULL != sl2AllocPrms));

    for (cnt = 0U; cnt < LDC_MAX_OUTPUT; cnt ++)
    {
        if ((uint32_t)UTRUE == sl2AllocPrms->enableOutput[cnt])
        {
            /* Minimum 2 blocks are required */
            if (sl2AllocPrms->maxNumBlocks[cnt] <
                    VHWA_M2M_LDC_MIN_SL2_BLOCKS)
            {
                GT_0trace(VhwaLdcTrace, GT_ERR,
                    "Incorrect value for maxNumBlocks !!\n");
                status = FVID2_EINVALID_PARAMS;
            }
        }
    }

    if (FVID2_SOK == status)
    {
        for (cnt = 0U; cnt < LDC_MAX_OUTPUT; cnt ++)
        {
            instObj->sl2Size[cnt] = 0u;
            instObj->sl2Addr[cnt] = 0u;

            if ((uint32_t)UTRUE == sl2AllocPrms->enableOutput[cnt])
            {
                /* Calculate block width in terms of bytes,
                   using storage format */
                blockWidthInBytes = Vhwa_calcHorzSizeInBytes(
                    sl2AllocPrms->maxBlockWidth,
                    sl2AllocPrms->outCcsf[cnt]);

                /* Align block width in bytes to SL2 pitch ie 64byte aligned */
                blockWidthInBytes = Vhwa_calcSl2Pitch(blockWidthInBytes);

                size = blockWidthInBytes * sl2AllocPrms->maxBlockHeight *
                    sl2AllocPrms->maxNumBlocks[cnt];

                /* For YUV420 output, two channels are required and for chroma
                   channel in YUV420, size is exactly half of luma size.
                   Assumes there are always two channels, no support
                   for allocation for single channel */
            #if defined VHWA_VPAC_IP_REV_VPAC
                sl2MemSize = size + (size / 2U);
            #elif defined VHWA_VPAC_IP_REV_VPAC3
                /* Assuming R+GB usecase */
                sl2MemSize = size + (size * 2U);
            #endif

                /* Allocate SL2 memory for calculated size */
                sl2Addr = Vhwa_allocateSl2(sl2MemSize, VHWA_SL2_INST_VPAC);

                if (0U == sl2Addr)
                {
                    GT_1trace(VhwaLdcTrace, GT_ERR,
                        "Could not allocate SL2 memory for output%d!!\n", cnt);
                    status = FVID2_EALLOC;
                    break;
                }
                else
                {
                    instObj->sl2Size[cnt] = sl2MemSize;
                    instObj->sl2Addr[cnt] = sl2Addr;
                }
            }
        }
    }

    if (FVID2_SOK != status)
    {
        /* error occured, so free up the memory allocated for SL2 */
        for (cnt = 0U; cnt < LDC_MAX_OUTPUT; cnt ++)
        {
            if (((uint32_t)UTRUE == sl2AllocPrms->enableOutput[cnt]) &&
                (0U != instObj->sl2Size[cnt]))
            {
                Vhwa_FreeSl2(instObj->sl2Addr[cnt], VHWA_SL2_INST_VPAC);

                instObj->sl2Size[cnt] = 0U;
                instObj->sl2Addr[cnt] = 0U;
            }
        }
        /* Error in memory allocation, open will not be allowed. */
        instObj->isSl2AllocDone = (uint32_t)UFALSE;
    }
    else
    {
        /* Memory is allocated successfully, now open is allowed. */
        instObj->isSl2AllocDone = (uint32_t)UTRUE;

        Fvid2Utils_memcpy(&instObj->sl2AllocPrms, sl2AllocPrms,
            sizeof(Vhwa_M2mLdcSl2AllocPrms));
    }

    return (status);
}

static int32_t vhwaM2mLdcCheckCreatePrms(uint32_t drvId, uint32_t drvInstId)
{
    int32_t             status = FVID2_SOK;

    if (FVID2_VHWA_M2M_LDC_DRV_ID != drvId)
    {
        GT_0trace(VhwaLdcTrace, GT_ERR, "Invalid Driver ID !!\n");
        status = FVID2_EINVALID_PARAMS;
    }
    else
    {
        /* Check for correct instance ID */
#if defined VHWA_M2M_VPAC_INSTANCE
#if (VHWA_M2M_VPAC_INSTANCE == 0)
        if (VHWA_M2M_VPAC_0_LDC_DRV_INST_ID_0 != drvInstId)
        {
            status = FVID2_EINVALID_PARAMS;
            GT_0trace(VhwaLdcTrace, GT_ERR, "Invalid/unsupported Instance Id\n");
        }
#endif
#endif

#if defined VHWA_M2M_VPAC_INSTANCE 
#if (VHWA_M2M_VPAC_INSTANCE == 1)
        if (VHWA_M2M_VPAC_1_LDC_DRV_INST_ID_0 != drvInstId)
        {
            status = FVID2_EINVALID_PARAMS;
            GT_0trace(VhwaLdcTrace, GT_ERR, "Invalid/unsupported Instance Id\n");
        }
#endif
#endif
    }

    return (status);
}
