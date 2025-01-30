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
 *  \file vhwa_m2mDofApi.c
 *
 *  \brief API Implementation
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/drv/vhwa/src/drv/vhwa_m2mDofPriv.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** Channel Indexes for HTS Configuration */
#define HTS_DOF_CUR_FRAME_CH_IDX        (1U)
#define HTS_DOF_REFE_FRAME_CH_IDX       (0U)
#define HTS_DOF_TEM_PRE_CH_IDX          (2U)
#define HTS_DOF_PYR_PRE_CH_IDX          (3U)
#define HTS_DOF_SOF_CH_IDX              (4U)
#define HTS_DOF_OUT_CH_IDX              (0U)

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
static Vhwa_M2mDofHandleObj *vhwaM2mDofAllocHandleObj(
    const Vhwa_M2mDofInstObj *instObj);

/**
 * \brief   Create Queues, required for storing pending requests
 *
 * \param   instObj             Instance object.
 *
 * \return  pointer to handle object on success else NULL.
 *
 **/
static int32_t vhwaM2mDofCreateQueues(Vhwa_M2mDofInstObj *instObj);

/**
 * \brief   Delete Queues
 *
 * \param   instObj             Instance object.
 *
 **/
static void vhwaM2mDofDeleteQueues(Vhwa_M2mDofInstObj *instObj);

/**
 * \brief   Local function to freeup allocated Handle Object and return it
 *          to the pool of free handle objects.
 *          No protection inside the function, Caller should protect
 *          the function call
 *
 * \param   hObj                Handle Object to be freed up.
 *
 **/
static void vhwaM2mDofFreeHandleObj(Vhwa_M2mDofHandleObj *hObj);

/**
 * \brief   Function to check given DOF configuration, used in
 *          SET_PARAMS ioctl it first initializes LSE and HTS
 *          config and then uses CSLFL of LSE and HTS to validate
 *          the configuration
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 * \param   dofPrms              DOF configuration to be validated
 *
 * \return  FVID2_SOK if given DOF cofiguration is valid, error code otherwise.
 *
 **/
static int32_t vhwaM2mDofCheckDofCfg(const Vhwa_M2mDofInstObj *instObj,
    Vhwa_M2mDofHandleObj *hObj, Vhwa_M2mDofPrms *dofPrms);

/**
 * \brief   Based on the given DOF config, it initializes LSE configuration.
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 * \param   dofPrms              DOF Configuration
 *
 **/
static void vhwaM2mDofSetLseCfg(Vhwa_M2mDofHandleObj *hObj,
                                const Vhwa_M2mDofPrms *dofPrms);

/**
 * \brief   Based on the given DOF config, it initializes HTS configuration.
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 * \param   dofPrms             DOF Configuration
 *
 **/
static void vhwaM2mDofSetHtsCfg(Vhwa_M2mDofInstObj *instObj,
    Vhwa_M2mDofHandleObj *hObj, const Vhwa_M2mDofPrms *dofPrms);

/**
 * \brief   Set the DMPAC FOCO prameters for reference and current buffers
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 * \param   dofPrms             DOF Configuration
 *
 **/
static void vhwaM2mDofSetFocoParams(Vhwa_M2mDofHandleObj *hObj,
                                       Vhwa_M2mDofPrms *dofPrms);

/**
 * \brief   Set the HTS bandwidth limiter parameters
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 * \param   bwLimit             BW limiter configuration
 *
 **/
static void vhwaM2mDofSetBwLimitParams(Vhwa_M2mDofHandleObj *hObj,
                                       const Vhwa_HtsLimiter *bwLimit);

/*  */
/**
 * \brief   Implementation of SET_PARAMS ioctl.
 *          It uses CheckDofCfg to validate the config.
 *          If it is valid, copies the config into handle object
 *          If it is invalid, it reverts LSE/HTS config to known valid config
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 * \param   dofCfg              DOF configuration to be set
 *
 **/
static int32_t vhwaM2mDofSetParams(Vhwa_M2mDofInstObj *instObj,
    Vhwa_M2mDofHandleObj *hObj, Vhwa_M2mDofPrms *dofCfg);

/**
 * \brief   Sets the error event parameters
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 * \param   eePrms              Error Event parameters
 *
 **/
static int32_t vhwaM2mDofSetEeParams(const Vhwa_M2mDofInstObj *instObj,
    Vhwa_M2mDofHandleObj *hObj, const Dof_ErrEventParams *eePrms);

/**
 * \brief   Calculate and store the Sl2 buffer parameters based on Dof
 *          configuration
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 * \param   dofPrms             DOF configuration to be set
 *
 **/
static int32_t vhwaM2mDofSetSl2Params(const Vhwa_M2mDofInstObj *instObj,
        Vhwa_M2mDofHandleObj *hObj, Vhwa_M2mDofPrms *dofPrms);

/**
 * \brief   Calculate and get the Dof parameters for all pyramid level based on
 *          Base pyramid level configuration and store in internal structure
 *
 * \param   hObj                Handle Object
 *
 **/
static void vhwaM2mDofCalPyrmPrms(Vhwa_M2mDofHandleObj *hObj);

/**
 * \brief   Set Sl2 buffer parameters for DOF for given configuration
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 * \param   dofPrms             DOF configuration to be set
 *
 **/
static void vhwaM2mDofSetBuffParams(Vhwa_M2mDofHandleObj *hObj,
                                    Vhwa_M2mDofPrms *dofPrms);

/**
 * \brief   Enable the FOCO engine. This function called while submitting the
 *          new request
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 *
 **/
static int32_t vhwaM2mDofEnableFoco(const Vhwa_M2mDofInstObj *instObj,
    const Vhwa_M2mDofHandleObj *hObj);


/**
 * \brief   Minimal function to submit request to hw and start DOF operation.
 *          It first sets address in the TR, Submits it to the ring and
 *          starts the pipeline.
 *          It does not start currently DOF, HTS schedulers.
 *
 * \param   instObj             Instance Object, used for getting base address
 * \param   qObj                Queue Object, used for getting handle object
 *
 **/
static int32_t vhwaM2mDofSubmitRequest(Vhwa_M2mDofInstObj *instObj,
    Vhwa_M2mDofQueueObj *qObj);


/* Implementation of FVID2 APIs */

/**
 * \brief   FVID2 Create Function.
 *
 * \param   instObj             Instance object.
 *
 * \return  FVID2 Driver Handle.
 *
 **/
Fdrv_Handle vhwa_m2mDofCreate(UInt32 drvId, UInt32 drvInstId,
    Ptr createArgs, Ptr createStatusArgs, const Fvid2_DrvCbParams *cbPrms);

/**
 * \brief   FVID2 Delete Function.
 *
 * \param   handle              FVID2 driver handle.
 *
 * \return  FVID2_SOK on success, else FVID2 error code
 *
 **/
Int32 vhwa_m2mDofDelete(Fdrv_Handle handle, Ptr deleteArgs);

/**
 * \brief   FVID2 Control Function.
 *
 * \param   handle              FVID2 driver handle.
 *
 * \return  FVID2_SOK on success, else FVID2 error code
 *
 **/
Int32 vhwa_m2mDofControl(Fdrv_Handle handle, UInt32 cmd, Ptr cmdArgs,
    Ptr cmdStatusArgs);

/**
 * \brief   FVID2 Process Request function.
 *
 * \param   handle              FVID2 driver handle.
 *
 * \return  FVID2_SOK on success, else FVID2 error code
 *
 **/
Int32 vhwa_m2mDofProcessReq(Fdrv_Handle handle, Fvid2_FrameList *inFrmList,
    Fvid2_FrameList *outFrmList, uint32_t timeout);

/**
 * \brief   FVID2 Get Process Request function.
 *
 * \param   handle              FVID2 driver handle.
 *
 * \return  FVID2_SOK on success, else FVID2 error code
 *
 **/
Int32 vhwa_m2mDofGetProcessReq(Fdrv_Handle handle,
    Fvid2_FrameList *inProcessList, Fvid2_FrameList *outProcessList,
    UInt32 timeout);

static void Vhwa_m2mDofCalGwPrms(const Dof_Config *dofConfig, uint32_t *refBot,
                                 uint32_t *refTop, uint32_t *curBot,
                                 uint32_t *curTop);
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
/* Default Confidence score configuration */
Dof_ConfScoreParam gConfScoreParam[] = {
    {
        224,
        {
            {{6, 7, 0},{ 14,  13,  0},
                {846,  (uint32_t)-846, (uint32_t)-846, (uint32_t)-846}},
            {{6, 5, 4},{ 30,  178, 33064},
                {(uint32_t)-450,  450, (uint32_t)-450,  450}},
            {{5, 6, 7},{ 499, 3,   28},
                {258,  (uint32_t)-258,  258, (uint32_t)-258}},
            {{5, 6, 0},{ 38,  14,  692},
                {(uint32_t)-152,  152,  152, (uint32_t)-152}},
            {{6, 6, 7},{ 14,  8,   22},
                {191,  (uint32_t)-191,  191, (uint32_t)-191}},
            {{6, 7, 7},{ 14,  13,  52},
                {(uint32_t)-158,  158,  158, (uint32_t)-158}},
            {{5, 7, 0},{ 625, 1,   632},
                {150,  (uint32_t)-150, (uint32_t)-150,  150}},
            {{0, 4, 7},{ 90,  4197,7},
                {(uint32_t)-91,   91,   91,  (uint32_t)-91}},
            {{4, 1, 4},{ 1126,70,  307},
                {(uint32_t)-5120, 5120, 5120, 5120}},
            {{4, 0, 0},{ 1126,0,   0},
                {5028,  5028,(uint32_t)-5028,(uint32_t)-5028}},
            {{6, 6, 5},{ 30,  3,   88},
                {123,  (uint32_t)-123, (uint32_t)-123,  123}},
            {{6, 4, 7},{ 19,  512, 28},
                {(uint32_t)-105,  105, (uint32_t)-105,  105}},
            {{6, 7, 5},{ 14,  13,  499},
                {(uint32_t)-107,  107,  107, (uint32_t)-107}},
            {{5, 5, 6},{ 562, 178, 47},
                {93,   (uint32_t)-93,   93,  (uint32_t)-93}},
            {{5, 0, 7},{ 79,  2234,34},
                {(uint32_t)-71,   71,   71,  (uint32_t)-71}},
            {{5, 5, 5},{ 163, 163, 734},
                {(uint32_t)-16384,0,    0,   (uint32_t)-16384}},

        }
    },
};

Vhwa_M2mDofHandleObj gM2mDofHandleObj[VHWA_M2M_DOF_MAX_HANDLES];
Vhwa_M2mDofInstObj   gM2mDofInstObj;

Fvid2_DrvOps gM2mDofFvid2DrvOps = {
    FVID2_VHWA_M2M_DOF_DRV_ID,
    /**< Unique driver Id. */
    vhwa_m2mDofCreate,
    /**< FVID2 create function pointer. */
    vhwa_m2mDofDelete,
    /**< FVID2 delete function pointer. */
    vhwa_m2mDofControl,
    /**< FVID2 control function pointer. */
    NULL, NULL,
    /**< FVID2 queue function pointer. */
    vhwa_m2mDofProcessReq,
    /**< FVID2 process request function pointer. */
    vhwa_m2mDofGetProcessReq,
    /**< FVID2 get processed request function pointer. */
};

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int32_t Vhwa_m2mDofInit(Vhwa_M2mDofInitParams *initPrms)
{
    int32_t             status;
    uint32_t            cnt;
    SemaphoreP_Params   params;
    Vhwa_M2mDofInstObj *instObj = NULL;

    if (NULL != initPrms)
    {
        instObj = &gM2mDofInstObj;

        /* Reset all instance object to 0x0 */
        Fvid2Utils_memset(instObj, 0U, sizeof (Vhwa_M2mDofInstObj));

        /* Mark pool flags as free */
        for (cnt = 0U; cnt < VHWA_M2M_DOF_MAX_HANDLES; cnt++)
        {
            gM2mDofHandleObj[cnt].isUsed = (uint32_t) UFALSE;
        }

        /* Set HTS pipeline */
        instObj->pipeline = VHWA_M2M_DOF_HTS_PIPELINE;

        Dof_getSocInfo(&instObj->socInfo);

        status = Fvid2_registerDriver(&gM2mDofFvid2DrvOps);
        if (FVID2_SOK == status)
        {
            instObj->isRegistered = (uint32_t)UTRUE;
        }
        else
        {
            GT_0trace(VhwaDofTrace, GT_ERR,
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
                GT_0trace(VhwaDofTrace, GT_ERR,
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
                GT_0trace(VhwaDofTrace, GT_ERR,
                    "Failed to allocate HWA Lock semaphore!!\n");
                status = FVID2_EALLOC;
            }
        }

        /* Create free and request queues */
        if (FVID2_SOK == status)
        {
            status = vhwaM2mDofCreateQueues(instObj);
        }

        if (FVID2_SOK == status)
        {
            /* Initialize UDMA, ie allocate and initialize channels
               required for DOF output */
            status = Vhwa_m2mDofUdmaInit(instObj, initPrms);
            if (FVID2_SOK != status)
            {
                GT_0trace(VhwaDofTrace, GT_ERR,
                    "UDMA Initialization Failed !!\n");
            }
        }

        /* Register ISR handler for the given irq number */
        if (FVID2_SOK == status)
        {
            instObj->irqNum = initPrms->irqInfo.irqNum;
            instObj->vhwaIrqNum = initPrms->irqInfo.vhwaIrqNum;

            status = Vhwa_m2mDofRegisterIsr(instObj);
        }

        /* Init is done, copy init params locally,
           enable DMPAC and DOF in DMPAC Top */
        if (FVID2_SOK == status)
        {
            Fvid2Utils_memcpy(&instObj->initPrms, initPrms,
                sizeof(Vhwa_M2mDofInitParams));

            /* Enable DOF at DMPAC Top*/
            CSL_dmpacEnableModule(instObj->socInfo.dmpacCntlRegs,
                DMPAC_MODULE_DOF, (uint32_t)UTRUE);

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
        Vhwa_m2mDofDeInit();
    }

    return (status);
}

void Vhwa_m2mDofDeInit(void)
{
    Vhwa_M2mDofInstObj *instObj = NULL;

    instObj = &gM2mDofInstObj;

    if (instObj->openCnt > 0u)
    {
        GT_0trace(VhwaDofTrace, GT_ERR,
            "Warning: All driver handles are not closed!!\n");
    }

    /* Stop UDMA Channels */
    (void)Vhwa_m2mDofStopCh(instObj);

    Vhwa_m2mDofUnregisterIsr(instObj);

    (void)Vhwa_m2mDofUdmaDeInit(instObj);

    if ((uint32_t)UTRUE == instObj->isRegistered)
    {
        (void)Fvid2_unRegisterDriver(&gM2mDofFvid2DrvOps);
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

    vhwaM2mDofDeleteQueues(instObj);

    /* Init all global variables to zero */
    Fvid2Utils_memset(instObj, 0U, sizeof (gM2mDofInstObj));

    instObj->initDone = (uint32_t)UFALSE;
}

int32_t Vhwa_m2mDofAllocSl2(const Vhwa_M2mDofSl2AllocPrms *sl2AllocPrms)
{
    int32_t status = FVID2_SOK;
    uint32_t sl2MemReq = 0;
    uint32_t pitchSl2, sofPitch ,focoBuffPitch;
    Vhwa_M2mDofInstObj *instObj = NULL;

    instObj = &gM2mDofInstObj;

    /* Cannot even lock, if init is not done */
    if ((uint32_t)UFALSE == instObj->initDone)
    {
        GT_0trace(VhwaDofTrace, GT_ERR,
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
        /* Since if width is multiple of 256 to alivate peak SL2 usage Line
           staggering is required */
        pitchSl2 = Vhwa_calcHorzSizeInBytes(sl2AllocPrms->maxImgWidth,
                            FVID2_CCSF_BITS12_PACKED);
        pitchSl2 = Vhwa_calcSl2Pitch(pitchSl2);

        /* RFGW SL2 Memory required */
        sl2MemReq += pitchSl2 * (((sl2AllocPrms->topSR + 7u) & 0xFFFFFFFEu)
                            + 2u + ((sl2AllocPrms->botSR + 10u) & 0xFFFFFFFEu)
                            + sl2AllocPrms->refBuffDepth);

        /* CFGW SL2 Memory required */
        sl2MemReq += pitchSl2 * (6u + 2u + 8u + sl2AllocPrms->currBuffDepth);

        /* Calculate Temporal Predictor memory requirement */
        sl2MemReq += CSL_lseMakePitchAligned(sl2AllocPrms->maxImgWidth * 4U) *
                        DOF_TEMPORAL_PRED_SL2_DEPTH;

        /* Calculate Pyramidal Predictor memory requirement */
        sl2MemReq += CSL_lseMakePitchAligned(sl2AllocPrms->maxImgWidth) *
                        DOF_PYRAM_PRED_SL2_DEPTH;

        /* Calculate SOF memory requirement */
        sofPitch = sl2AllocPrms->maxImgWidth/8u;
        if((sofPitch * 8u) != sl2AllocPrms->maxImgWidth)
        {
            sofPitch++;
        }
        sofPitch = CSL_lseMakePitchAligned(sofPitch);

        sl2MemReq += sofPitch * DOF_SOF_SL2_DEPTH;

        /* Output Buffer Memory Required */
        sl2MemReq += CSL_lseMakePitchAligned(sl2AllocPrms->maxImgWidth * 4U) *
                        sl2AllocPrms->fvBuffDepth;


        /* Memory required for FOCO for Ref and Curr buffer */
        focoBuffPitch = Vhwa_calcHorzSizeInBytes(sl2AllocPrms->maxImgWidth,
                            sl2AllocPrms->inCcsf);
        focoBuffPitch = CSL_lseMakePitchAligned(focoBuffPitch);
        sl2MemReq += focoBuffPitch * DOF_FOCO_SL2_DEPTH * 2u;

        instObj->startAddr = Vhwa_allocateSl2(sl2MemReq, VHWA_SL2_INST_DMPAC);
        if(instObj->startAddr == 0u)
        {
            status = FVID2_EALLOC;

            instObj->isSl2AllocDone = (uint32_t)UFALSE;
        }
        else
        {
            instObj->sl2Size = sl2MemReq;

            instObj->isSl2AllocDone = (uint32_t)UTRUE;
        }
    }

    /* Release instance semaphore */
    (void)SemaphoreP_post(instObj->lock);

    return (status);
}

int32_t Vhwa_m2mDofFreeSl2(void)
{
    int32_t retVal = FVID2_SOK;
    Vhwa_M2mDofInstObj *instObj = NULL;

    instObj = &gM2mDofInstObj;

    if (UTRUE == instObj->isSl2AllocDone)
    {
        if(0U == instObj->openCnt)
        {
            /* Lock instance semaphore */
            (void)SemaphoreP_pend(instObj->lock, SemaphoreP_WAIT_FOREVER);

            Vhwa_FreeSl2(instObj->startAddr, VHWA_SL2_INST_DMPAC);

            instObj->isSl2AllocDone = (uint32_t)UFALSE;
            instObj->sl2Size        = 0u;
            instObj->startAddr      = 0u;

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

void Vhwa_m2mConfScoreParamInit(Dof_ConfScoreParam *csPrms)
{
    (void)memcpy(csPrms, &gConfScoreParam[0], sizeof(Dof_ConfScoreParam));
}


/**
 * \brief   Returns handle object for the requested handle count.
 *
 * \param   cnt              count.
 *
 * \return  reference to the handle object.
 *
 **/
Vhwa_M2mDofHandleObj *Vhwa_m2mDofGetHandleObj(uint32_t cnt)
{
    return &gM2mDofHandleObj[cnt];
}
/* ========================================================================== */
/*                          FVID2 Function implementation                     */
/* ========================================================================== */

Fdrv_Handle vhwa_m2mDofCreate(UInt32 drvId, UInt32 drvInstId,
    Ptr createArgs, Ptr createStatusArgs, const Fvid2_DrvCbParams *cbPrms)
{
    int32_t                 status = FVID2_SOK;
    Vhwa_M2mDofInstObj     *instObj = NULL;
    Vhwa_M2mDofHandleObj   *hObj = NULL;
    Vhwa_M2mDofCreateArgs  *dofCreateArgs = NULL;
    Fdrv_Handle             handle = NULL;
    Dof_SocInfo            *socInfo = NULL;
    Vhwa_M2mDofSl2AllocPrms sl2AllocPrms;

    instObj = &gM2mDofInstObj;
    socInfo = &instObj->socInfo;

    /* Check for errors */
    if ((FVID2_VHWA_M2M_DOF_DRV_ID != drvId) ||
        (VHWA_M2M_DOF_DRV_INST_ID != drvInstId) ||
        (NULL == createArgs) ||
        (NULL == cbPrms))
    {
        GT_0trace(VhwaDofTrace, GT_ERR, "NULL Pointer !!\n");
        status = FVID2_EBADARGS;
    }
    else
    {
        /* Open not allowed if init is not done */
        if ((uint32_t)UFALSE == instObj->initDone)
        {
            GT_0trace(VhwaDofTrace, GT_ERR,
                "Vhwa_m2mDofInit is not called\n");
            status  = FVID2_EFAIL;
        }

        dofCreateArgs = (Vhwa_M2mDofCreateArgs *)createArgs;
    }

    if (FVID2_SOK == status)
    {
        /* Take the instance semaphore */
        (void)SemaphoreP_pend(instObj->lock, SemaphoreP_WAIT_FOREVER);

        /* Allocate Handle Object */
        hObj = vhwaM2mDofAllocHandleObj(instObj);

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
                    GT_0trace(VhwaDofTrace, GT_DEBUG,
                    "Vhwa_m2mDofAllocSl2 is not called, allocating for default\n");

                    Vhwa_M2mDofSl2AllocPrmsInit(&sl2AllocPrms);
                    status = Vhwa_m2mDofAllocSl2(&sl2AllocPrms);
                }

                if (FVID2_SOK == status)
                {
                    /* Start UDMA Channels on the first handle Create */
                    status = Vhwa_m2mDofStartCh(instObj);
                }

                if (FVID2_SOK == status)
                {
                    /* Enable Pipeline interrupt in INTD */
                    Vhwa_enableDmpacHtsIntr(socInfo->dmpacIntdRegs,
                        instObj->vhwaIrqNum, instObj->pipeline);
                }
            }

            /* Fill Sl2 Depth Params with default */
            hObj->sl2Prms.sl2NumLines[DOF_INPUT_REFERENCE_IMG] =
                                                DOF_REF_IMG_SL2_DEPTH;
            hObj->sl2Prms.sl2NumLines[DOF_INPUT_CURRENT_IMG] =
                                                DOF_CURR_IMG_SL2_DEPTH;
            hObj->sl2Prms.sl2NumLines[DOF_INPUT_TEMPORAL_PRED] =
                                                DOF_TEMPORAL_PRED_SL2_DEPTH;
            hObj->sl2Prms.sl2NumLines[DOF_INPUT_PYRAMID_PRED] =
                                                DOF_PYRAM_PRED_SL2_DEPTH;
            hObj->sl2Prms.sl2NumLines[DOF_INPUT_SOF] =
                                                DOF_SOF_SL2_DEPTH;
            hObj->sl2Prms.sl2NumLines[DOF_OUTPUT] =
                                                DOF_OUTPUT_SL2_DEPTH;
            hObj->sl2Prms.sl2NumLines[DOF_INPUT_FOCO_REF_IMG] =
                                                DOF_FOCO_SL2_DEPTH;
            hObj->sl2Prms.sl2NumLines[DOF_INPUT_FOCO_CURR_IMG] =
                                                DOF_FOCO_SL2_DEPTH;


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
                status = Vhwa_m2mDofAllocUdmaMem(hObj);
            }

            if (FVID2_SOK == status)
            {
                Fvid2Utils_memcpy(&hObj->createArgs, dofCreateArgs,
                    sizeof(Vhwa_M2mDofCreateArgs));

                Fvid2Utils_memcpy(&hObj->cbPrms, cbPrms,
                    sizeof (hObj->cbPrms));

                instObj->openCnt ++;
            }
            else
            {
                /* Some error, so free up the handle object */
                vhwaM2mDofFreeHandleObj(hObj);
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

Int32 vhwa_m2mDofDelete(Fdrv_Handle handle, Ptr deleteArgs)
{
    int32_t                 status = FVID2_SOK;
    Vhwa_M2mDofInstObj     *instObj = NULL;
    Vhwa_M2mDofHandleObj   *hObj = NULL;
    Dof_SocInfo            *socInfo = NULL;

    if (NULL != handle)
    {
        instObj = &gM2mDofInstObj;
        hObj    = (Vhwa_M2mDofHandleObj *)handle;
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
            vhwaM2mDofFreeHandleObj(hObj);

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

Int32 vhwa_m2mDofControl(Fdrv_Handle handle, UInt32 cmd, Ptr cmdArgs,
    Ptr cmdStatusArgs)
{
    int32_t                 status = FVID2_SOK;
    Vhwa_M2mDofInstObj     *instObj = NULL;
    Vhwa_M2mDofHandleObj   *hObj = NULL;
    Vhwa_M2mDofPrms      *dofPrms = NULL;
    Dof_ConfScoreParam     *confScoreCfg = NULL;
    Dof_ErrEventParams     *eePrms = NULL;
    Vhwa_HtsLimiter        *bwLimit = NULL;

    if (NULL == handle)
    {
        status = FVID2_EBADARGS;
    }
    else
    {
        instObj = &gM2mDofInstObj;
        hObj = (Vhwa_M2mDofHandleObj *)handle;
    }
    if (FVID2_SOK == status)
    {
        /* Take the instance semaphore */
        (void)SemaphoreP_pend(instObj->lock, SemaphoreP_WAIT_FOREVER);

        switch (cmd)
        {
            /* Set DOF Parameters */
            case VHWA_M2M_IOCTL_DOF_SET_PARAMS:
            {
                if (NULL != cmdArgs)
                {
                    dofPrms = (Vhwa_M2mDofPrms *)cmdArgs;
                    status = vhwaM2mDofSetParams(instObj, hObj, dofPrms);
                }
                break;
            }
            /* Set DOF Parameters */
            case VHWA_M2M_IOCTL_DOF_SET_CONF_SCORE_PARAMS:
            {
                if (NULL != cmdArgs)
                {
                    confScoreCfg = (Dof_ConfScoreParam *)cmdArgs;
                    Fvid2Utils_memcpy(&hObj->confScoreCfg, confScoreCfg,
                                    sizeof(Dof_ConfScoreParam));
                }
                break;
            }

            /* Enable Register error events callback */
            case VHWA_M2M_IOCTL_DOF_REGISTER_ERR_CB:
            {
                if (NULL != cmdArgs)
                {
                    eePrms = (Dof_ErrEventParams *)cmdArgs;
                    status = vhwaM2mDofSetEeParams(instObj, hObj, eePrms);
                }
                break;
            }

            /* SET HTS Limiter config */
            case VHWA_M2M_IOCTL_DOF_SET_HTS_LIMIT:
            {
                if (NULL != cmdArgs)
                {
                    bwLimit = (Vhwa_HtsLimiter *)cmdArgs;

                    vhwaM2mDofSetBwLimitParams(hObj, bwLimit);
                }
                break;
            }

            /* SET Next pyramid level to be processed */
            case VHWA_M2M_IOCTL_DOF_SET_NEXT_PYR:
            {
                if (NULL != cmdArgs)
                {
                    hObj->nextPyrLvl = *((uint32_t *)cmdArgs);
                }
                break;
            }

            /* Get PSA Signature */
            case VHWA_M2M_IOCTL_DOF_GET_PSA_SIGN:
            {
                if (NULL != cmdArgs)
                {
                    status = CSL_dofGetPsa(instObj->socInfo.dofRegs, (uint32_t *)cmdArgs);
                }
                break;
            }

            /* Get Histogram */
            case VHWA_M2M_IOCTL_DOF_GET_HISTOGRAM:
            {
                uint32_t *csHistogram = NULL;
                if (NULL != cmdArgs)
                {
                    csHistogram = (uint32_t *)cmdArgs;
                    status = CSL_dofGetCsHistogram(instObj->socInfo.dofRegs,
                                                    csHistogram);
                }
                break;
            }

            case VHWA_M2M_IOCTL_DOF_GET_PERFORMANCE:
            {
                uint32_t *perf = NULL;
                if (NULL != cmdArgs)
                {
                    perf = (uint32_t *)cmdArgs;
                    *perf = (uint32_t)hObj->perfNum;
                }
                break;
            }

            case VHWA_M2M_IOCTL_DOF_SYNC_START:
            {
                uint32_t *pyrLvl = NULL;
                if (NULL != cmdArgs)
                {
                    pyrLvl = (uint32_t *)cmdArgs;
                    if (NULL != hObj->createArgs.getTimeStamp)
                    {
                        hObj->perfNum = hObj->createArgs.getTimeStamp();
                    }
                    status = CSL_dmpacHtsPipelineStart(instObj->socInfo.htsRegs,
                                                    &hObj->htsCfg[*pyrLvl]);
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

Int32 vhwa_m2mDofProcessReq(Fdrv_Handle handle, Fvid2_FrameList *inFrmList,
    Fvid2_FrameList *outFrmList, uint32_t timeout)
{
    int32_t               status = FVID2_SOK;
    uint32_t              semTimeout;
    Vhwa_M2mDofInstObj   *instObj = NULL;
    Vhwa_M2mDofHandleObj *hObj = NULL;
    Vhwa_M2mDofQueueObj  *qObj = NULL;
    SemaphoreP_Status     semStatus;

    if ((NULL == handle) || (NULL == inFrmList) || (NULL == outFrmList))
    {
        status = FVID2_EBADARGS;
    }
    else
    {
        instObj = &gM2mDofInstObj;
        hObj = (Vhwa_M2mDofHandleObj *)handle;
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
            qObj = (Vhwa_M2mDofQueueObj *) Fvid2Utils_dequeue(instObj->freeQ);

            if (NULL == qObj)
            {
                GT_0trace(VhwaDofTrace, GT_ERR,
                    "Failed to Free Queue Object!!\n");
                status = FVID2_EALLOC;
            }
            else
            {
                /* Get the current pyramid level */
                hObj->curPyrLvl = hObj->nextPyrLvl;

                qObj->hObj = hObj;
                qObj->pyrLvl = hObj->curPyrLvl;
                /* Copy the application's process list to request objects lists */
                Fvid2_copyFrameList(&qObj->inFrmList, inFrmList);
                Fvid2_copyFrameList(&qObj->outFrmList, outFrmList);
            }
        }

        if (FVID2_SOK == status)
        {
            /* HW is free, submit request to the hardware */
            /* If previous handle and current handles are same, */
            if (instObj->lastHndlObj != qObj->hObj)
            {
                /** Last handle was not same as new handle,
                 *  so Buffer Parameters needs to be configured */
                 (void)CSL_dofSetBufParam(instObj->socInfo.dofRegs,
                                                &hObj->dofBuffPrams);

            }

            /** Set the addresses, Configure DOF/HTS, Submit the TR
             *  Start the pipeline */
            status = vhwaM2mDofSubmitRequest(instObj, qObj);
        }
    }

    return (status);
}

/** \brief Typedef for FVID2 get processed frames function pointer. */
Int32 vhwa_m2mDofGetProcessReq(Fdrv_Handle handle,
    Fvid2_FrameList *inFrmList, Fvid2_FrameList *outFrmList,
    UInt32 timeout)
{
    int32_t                status = FVID2_SOK;
    uint32_t               cookie;
    Vhwa_M2mDofInstObj    *instObj = NULL;
    Vhwa_M2mDofHandleObj  *hObj = NULL;
    Vhwa_M2mDofQueueObj   *qObj = NULL;

    if (NULL == handle)
    {
        status = FVID2_EBADARGS;
    }
    else
    {
        instObj = &gM2mDofInstObj;
        hObj = (Vhwa_M2mDofHandleObj *)handle;
    }

    if (FVID2_SOK == status)
    {
        /* Take the instance semaphore, so that no other
         * handle can submit request from the task context. */
        (void)SemaphoreP_pend(instObj->lock, SemaphoreP_WAIT_FOREVER);

        /* Disable interrupts before accessing queue */
        cookie = HwiP_disable();

        /* Dequeue the completed request from done queue */
        qObj = (Vhwa_M2mDofQueueObj *) Fvid2Utils_dequeue(hObj->doneQ);

        /* Restore interrupts after updating node list */
        HwiP_restore(cookie);

        /* No buffers in the output queue */
        if (NULL == qObj)
        {
            /* Check if requests are pending with driver */
            if (0U == hObj->numPendReq)
            {
                /* Nothing is queued */
                GT_0trace(VhwaDofTrace, GT_DEBUG,
                    "Nothing to dequeue. No request pending with driver!!\r\n");
                status = FVID2_ENO_MORE_BUFFERS;
            }
            else
            {
                /* If no request have completed, return try again */
                GT_0trace(VhwaDofTrace, GT_DEBUG,
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
            status = Vhwa_m2mDofPopRings(instObj, hObj, qObj->pyrLvl);

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

static void Vhwa_m2mDofCalGwPrms(const Dof_Config *dofConfig, uint32_t *refBot,
                                 uint32_t *refTop, uint32_t *curBot,
                                 uint32_t *curTop)
{
    *refTop = dofConfig->topSearchRange + 6u; /* 6 = census tr + hamming dist */
    *refTop = (*refTop + 1u) & 0xFFFFFFFEu;    /* should be multiple of 2 */
    *refBot = dofConfig->bottomSearchRange + 6u + 2u + 1u; /* 6 = census tr + hamming dist; 2=prefetch; 1=adv-pred */
    *refBot = (*refBot + 1u) & 0xFFFFFFFEu;    /* should be multiple of 2 */

    *curTop = 6u; /* 6=census tr + hamming dist */
    *curBot = 8u; /* 6=census tr + hamming dist; 1=adv-pred; 1=round-even */
}

/* ========================================================================== */
/*                           Local Functions                                  */
/* ========================================================================== */

/* Function to Allocate Handle Object */
static Vhwa_M2mDofHandleObj *vhwaM2mDofAllocHandleObj(
    const Vhwa_M2mDofInstObj *instObj)
{
    uint32_t cnt;
    Vhwa_M2mDofHandleObj *hObj = NULL;

    if (NULL != instObj)
    {
        for (cnt = 0U; cnt < VHWA_M2M_DOF_MAX_HANDLES; cnt ++)
        {
            if ((uint32_t)UFALSE == gM2mDofHandleObj[cnt].isUsed)
            {
                hObj = &gM2mDofHandleObj[cnt];
                Fvid2Utils_memset(hObj, 0x0, sizeof(Vhwa_M2mDofHandleObj));
                gM2mDofHandleObj[cnt].isUsed = (uint32_t)UTRUE;
                hObj->hIdx = cnt;
                break;
            }
        }
    }
    return (hObj);
}

/* Function to freeup allocated Handle Object */
static void vhwaM2mDofFreeHandleObj(Vhwa_M2mDofHandleObj *hObj)
{
    uint32_t cnt;

    if (NULL != hObj)
    {
        for (cnt = 0U; cnt < VHWA_M2M_DOF_MAX_HANDLES; cnt ++)
        {
            if (hObj == &gM2mDofHandleObj[cnt])
            {
                hObj->isUsed = (uint32_t)UFALSE;;
                break;
            }
        }
    }
}


static int32_t vhwaM2mDofCheckDofCfg(const Vhwa_M2mDofInstObj *instObj,
    Vhwa_M2mDofHandleObj *hObj, Vhwa_M2mDofPrms *dofPrms)
{
    int32_t     status = FVID2_SOK;
    uint32_t    pyrLvl, tHeight, tWidth;
    Dof_Config  *dofCfg;

    if ((NULL != instObj) && (NULL != hObj) && (NULL != dofPrms))
    {
        dofCfg = &dofPrms->coreCfg;
        for(pyrLvl = 0u; pyrLvl < dofPrms->tPrmdLvl; pyrLvl++)
        {
            if(dofPrms->inOutImgFmt[pyrLvl][DOF_INPUT_CURRENT_IMG].ccsFormat ==
                                                    FVID2_CCSF_BITS12_PACKED)
            {
                hObj->isFocoUsed[pyrLvl] = UFALSE;
            }
            else if((dofPrms->inOutImgFmt[pyrLvl][DOF_INPUT_CURRENT_IMG].ccsFormat
                                    == FVID2_CCSF_BITS12_UNPACKED16) ||
                    (dofPrms->inOutImgFmt[pyrLvl][DOF_INPUT_CURRENT_IMG].ccsFormat
                                    == FVID2_CCSF_BITS8_PACKED) ||
                    (dofPrms->inOutImgFmt[pyrLvl][DOF_INPUT_CURRENT_IMG].ccsFormat
                                    == FVID2_CCSF_BITS12_UNPACKED16_MSB_ALIGNED))
            {
                hObj->isFocoUsed[pyrLvl] = UTRUE;
            }
            else
            {
                hObj->isFocoUsed[pyrLvl] = UFALSE;
                status = FVID2_EINVALID_PARAMS;
            }
        }

        if((DOF_PREDICTOR_TEMPORAL == dofPrms->tPredictor) ||
           (DOF_PREDICTOR_PYR_LEFT == dofPrms->tPredictor) ||
           (DOF_PREDICTOR_PYR_COLOCATED == dofPrms->tPredictor) ||
           (DOF_PREDICTOR_TEMPORAL == dofPrms->mPredictor1) ||
           (DOF_PREDICTOR_TEMPORAL == dofPrms->mPredictor2))
        {
            status = FVID2_EINVALID_PARAMS;
        }

        if(dofPrms->flowVectorHeight > dofCfg->height)
        {
            status = FVID2_EINVALID_PARAMS;
        }

        if((dofCfg->height > DOF_MAXIMUM_HEIGHT) ||
           (dofCfg->height < DOF_MINIMUM_HEIGHT) ||
           (dofCfg->width > DOF_MAXIMUM_WIDTH) ||
           (dofCfg->width < DOF_MINIMUM_WIDTH))
        {
            status = FVID2_EINVALID_PARAMS;
        }

        pyrLvl = dofPrms->tPrmdLvl;

        tWidth = dofCfg->width;
        tHeight = dofCfg->height;

        while(pyrLvl-- > 0u)
        {
            if(((tHeight & 0x1u) != 0U) ||
            ((tWidth & 0x1u) != 0U))
            {
                status = FVID2_EINVALID_PARAMS;
            }

            tWidth = tWidth/2u;
            tHeight = tHeight/2u;
        }

        if((dofCfg->horizontalSearchRange > DOF_MAXIMUM_HSR) ||
           (dofCfg->topSearchRange > DOF_MAXIMUM_VSR) ||
           (dofCfg->bottomSearchRange > DOF_MAXIMUM_VSR))
        {
            status = FVID2_EINVALID_PARAMS;
        }
        if((DOF_MAXIMUM_TVSR_WTIH_191_HSR <
           (dofCfg->topSearchRange + dofCfg->bottomSearchRange)) &&
           ((dofCfg->horizontalSearchRange > DOF_MAXIMUM_HSR_WTIH_124_VSR)))
        {
            status = FVID2_EINVALID_PARAMS;
        }
    }
    else
    {
        status = FVID2_EBADARGS;
    }

    return (status);
}

static int32_t vhwaM2mDofSetParams(Vhwa_M2mDofInstObj *instObj,
    Vhwa_M2mDofHandleObj *hObj, Vhwa_M2mDofPrms *dofPrms)
{
    int32_t status = FVID2_EBADARGS;

    if ((NULL != instObj) && (NULL != hObj) && (NULL != dofPrms))
    {
        status = vhwaM2mDofCheckDofCfg(instObj, hObj, dofPrms);

        /* DOF configuration is verified */
        if (FVID2_SOK == status)
        {
            status = vhwaM2mDofSetSl2Params(instObj, hObj, dofPrms);

            if(FVID2_SOK == status)
            {
                Fvid2Utils_memcpy(&hObj->dofPrms, dofPrms,
                                    sizeof(Vhwa_M2mDofPrms));

                /* Calculate the pyramidal paramters from the input
                    configuration */
                vhwaM2mDofCalPyrmPrms(hObj);


                /* Set Sl2 buffer parameters for DOF */
                vhwaM2mDofSetBuffParams(hObj, &hObj->dofPrms);

                /* Reset HTS config based on local correct dof config */
                vhwaM2mDofSetHtsCfg(instObj, hObj, &hObj->dofPrms);

                /* Reset LSE config based on local correct dof config - Required
                   only in case of FOCO */
                vhwaM2mDofSetLseCfg(hObj, &hObj->dofPrms);

                vhwaM2mDofSetFocoParams(hObj, &hObj->dofPrms);

                /* Setup TR Descriptor */
                Vhwa_m2mDofSetTrDesc(instObj, hObj);
            }
        }

        if(FVID2_SOK != status)
        {
            /* Reset HTS config based on local correct DOF config */
            vhwaM2mDofSetHtsCfg(instObj, hObj, &hObj->dofPrms);
            /* Reset LSE config based on local correct DOF config */
            vhwaM2mDofSetLseCfg(hObj, &hObj->dofPrms);
        }
    }

    return (status);
}

static int32_t vhwaM2mDofSetEeParams(const Vhwa_M2mDofInstObj *instObj,
    Vhwa_M2mDofHandleObj *hObj, const Dof_ErrEventParams *eePrms)
{
    int32_t status = FVID2_EBADARGS;

    if ((NULL != instObj) && (NULL != hObj) && (NULL != eePrms))
    {
        Fvid2Utils_memcpy(&hObj->eePrms, eePrms, sizeof(Dof_ErrEventParams));
        status = FVID2_SOK;
    }

    return (status);
}

static void vhwaM2mDofSetFocoParams(Vhwa_M2mDofHandleObj *hObj,
                                       Vhwa_M2mDofPrms *dofPrms)
{
    Dmpac_FocoConfig *focoRefCfg = NULL;
    Dmpac_FocoConfig *focoCurrCfg = NULL;
    Dof_Config       *dofCfg = NULL;
    Vhwa_FocoPrms    *focoPrms = NULL;
    uint32_t         refBot, refTop, curBot, curTop;
    uint32_t         pyrLvl;

    /* Null pointer check */
    GT_assert(VhwaDofTrace, (NULL != hObj));
    GT_assert(VhwaDofTrace, (NULL != dofPrms));

    focoPrms = &dofPrms->focoPrms;

    for(pyrLvl = 0; pyrLvl < dofPrms->tPrmdLvl; pyrLvl++)
    {
        if(UTRUE == hObj->isFocoUsed[pyrLvl])
        {
            focoRefCfg = &hObj->focoCfg[pyrLvl][DOF_INPUT_REFERENCE_IMG];
            focoCurrCfg = &hObj->focoCfg[pyrLvl][DOF_INPUT_CURRENT_IMG];
            dofCfg = &hObj->dofCfg[pyrLvl];

            Vhwa_m2mDofCalGwPrms(dofCfg, &refBot, &refTop, &curBot, &curTop);

            if(focoPrms->shiftM1 != 0u)
            {
                focoRefCfg->shiftEnable = UTRUE;
                focoCurrCfg->shiftEnable = UTRUE;

                focoRefCfg->shiftM1 = focoPrms->shiftM1 - 1U;
                focoCurrCfg->shiftM1 = focoPrms->shiftM1 - 1U;
            }
            else
            {
                focoRefCfg->shiftEnable = UFALSE;
                focoCurrCfg->shiftEnable = UFALSE;

                focoRefCfg->shiftM1 = 0U;
                focoCurrCfg->shiftM1 = 0U;
            }
            focoRefCfg->dir = focoPrms->dir;
            focoRefCfg->round = focoPrms->round;
            focoRefCfg->channelEnable = UTRUE;
            focoRefCfg->mask = 0xffffU;
            if(((refTop + refBot + 2U) > dofCfg->height) &&
                    ((refBot + 2U) >= dofCfg->height))
            {
                focoRefCfg->trig = ((2U * dofCfg->height) - 2U - curBot);
                focoCurrCfg->preload = dofCfg->height - 2U - curBot;
            }
            else
            {
                focoRefCfg->trig = dofCfg->height + (refBot - curBot);
                focoCurrCfg->preload = refBot - curBot;
            }

            focoCurrCfg->dir = focoPrms->dir;
            focoCurrCfg->round = focoPrms->round;
            focoCurrCfg->channelEnable = UTRUE;
            focoCurrCfg->mask = 0xffff;
            focoCurrCfg->trig = dofCfg->height;

            focoRefCfg->preload = 0u;
            focoRefCfg->postload = 0u;
            focoCurrCfg->postload = 0u;
        }
    }

}

static void vhwaM2mDofCalPyrmPrms(Vhwa_M2mDofHandleObj *hObj)
{
    uint32_t pyrLvl, cnt;
    Dof_Config *dofCfg;

    GT_assert(VhwaDofTrace, (NULL != hObj));

    for (pyrLvl = 0u; pyrLvl < DOF_MAX_PYR_LVL_SUPPORTED; pyrLvl++)
    {
        for (cnt = 0u; cnt < VHWA_M2M_DOF_MAX_DMA_CH; cnt ++)
        {
            hObj->chPrms[pyrLvl][cnt].isEnabled = UFALSE;
        }
    }

    for (pyrLvl = 0u; pyrLvl < hObj->dofPrms.tPrmdLvl; pyrLvl++)
    {
        dofCfg = &hObj->dofCfg[pyrLvl];

        Fvid2Utils_memcpy(dofCfg, &hObj->dofPrms.coreCfg,
                        sizeof(Dof_Config));

        dofCfg->pyramidalTopColPred = UFALSE;
        dofCfg->pyramidalTopLeftPred = UFALSE;
        dofCfg->temporalPred = UFALSE;
        dofCfg->delayedLeftPred = UFALSE;

        if(pyrLvl == 0u)
        {
            /* Base Layer */
            if((hObj->dofPrms.bPredictor1 == DOF_PREDICTOR_TEMPORAL) ||
               (hObj->dofPrms.bPredictor2 == DOF_PREDICTOR_TEMPORAL))
            {
                dofCfg->temporalPred = UTRUE;
            }
            if((hObj->dofPrms.bPredictor1 == DOF_PREDICTOR_PYR_COLOCATED) ||
               (hObj->dofPrms.bPredictor2 == DOF_PREDICTOR_PYR_COLOCATED))
            {
                dofCfg->pyramidalTopColPred = UTRUE;
            }
            if((hObj->dofPrms.bPredictor1 == DOF_PREDICTOR_PYR_LEFT) ||
               (hObj->dofPrms.bPredictor2 == DOF_PREDICTOR_PYR_LEFT))
            {
                dofCfg->pyramidalTopLeftPred = UTRUE;
            }
            if((hObj->dofPrms.bPredictor1 == DOF_PREDICTOR_DELEY_LEFT) ||
               (hObj->dofPrms.bPredictor2 == DOF_PREDICTOR_DELEY_LEFT))
            {
                dofCfg->delayedLeftPred      = UTRUE;
            }
            if(dofCfg->temporalPred == (uint32_t)UTRUE)
            {
                hObj->isTempPred = UTRUE;
                hObj->chPrms[pyrLvl][DOF_INPUT_TEMPORAL_PRED].isEnabled = UTRUE;
            }
            else
            {
                hObj->isTempPred = UFALSE;
            }
            if(dofCfg->enableSof == (uint32_t)UTRUE)
            {
                hObj->isSof = UTRUE;
                hObj->chPrms[pyrLvl][DOF_INPUT_SOF].isEnabled = UTRUE;
            }
            else
            {
                hObj->isSof = UFALSE;
            }
        }
        else if(pyrLvl == (hObj->dofPrms.tPrmdLvl - 1u))
        {
            /* Top Layer */
            if(hObj->dofPrms.tPredictor == DOF_PREDICTOR_DELEY_LEFT)
            {
                dofCfg->delayedLeftPred = UTRUE;
            }

            dofCfg->lkConfidanceScore = UFALSE;
            dofCfg->enableSof = UFALSE;
        }
        else
        {
            /* Middle Pyramid layers */
            if((hObj->dofPrms.mPredictor1 == DOF_PREDICTOR_PYR_COLOCATED) ||
               (hObj->dofPrms.mPredictor2 == DOF_PREDICTOR_PYR_COLOCATED))
            {
                dofCfg->pyramidalTopColPred = UTRUE;
            }
            if((hObj->dofPrms.mPredictor1 == DOF_PREDICTOR_PYR_LEFT) ||
               (hObj->dofPrms.mPredictor2 == DOF_PREDICTOR_PYR_LEFT))
            {
                dofCfg->pyramidalTopLeftPred = UTRUE;
            }
            if((hObj->dofPrms.mPredictor1 == DOF_PREDICTOR_DELEY_LEFT) ||
               (hObj->dofPrms.mPredictor2 == DOF_PREDICTOR_DELEY_LEFT))
            {
                dofCfg->delayedLeftPred      = UTRUE;
            }
            dofCfg->lkConfidanceScore = UFALSE;
            dofCfg->enableSof = UFALSE;
        }

        if (((uint32_t)UTRUE == dofCfg->pyramidalTopColPred) ||
            ((uint32_t)UTRUE == dofCfg->pyramidalTopLeftPred))
        {
            hObj->isPyrPred[pyrLvl] = UTRUE;
            hObj->chPrms[pyrLvl][DOF_INPUT_PYRAMID_PRED].isEnabled = UTRUE;
        }
        else
        {
            hObj->isPyrPred[pyrLvl] = UFALSE;
        }

        cnt = pyrLvl;

        while(cnt-- > 0u)
        {
            dofCfg->width   = dofCfg->width/2u;
            dofCfg->height  = dofCfg->height/2u;
        }

        hObj->chPrms[pyrLvl][DOF_OUTPUT].isEnabled = UTRUE;

        if(UTRUE == hObj->isFocoUsed[pyrLvl])
        {
            hObj->chPrms[pyrLvl][DOF_INPUT_FOCO_REF_IMG].isEnabled = UTRUE;
            hObj->chPrms[pyrLvl][DOF_INPUT_FOCO_CURR_IMG].isEnabled = UTRUE;
        }
        else
        {
            hObj->chPrms[pyrLvl][DOF_INPUT_REFERENCE_IMG].isEnabled = UTRUE;
            hObj->chPrms[pyrLvl][DOF_INPUT_CURRENT_IMG].isEnabled = UTRUE;
        }
    }
}

static int32_t vhwaM2mDofEnableFoco(const Vhwa_M2mDofInstObj *instObj,
    const Vhwa_M2mDofHandleObj *hObj)
{
    int32_t status;
    CSL_dmpac_foco_coreRegs *focoRegs = instObj->socInfo.dmpacFocoRegs;

    /* Enable channel 0 for Reference Frame */
    status = CSL_dmpacFocoSetConfig(focoRegs, DMPAC_FOCO_CHANNEL_0,
                    &hObj->focoCfg[hObj->curPyrLvl][DOF_INPUT_REFERENCE_IMG]);

    /* Enable channel 1 for Current Frame */
    if(FVID2_SOK == status)
    {
        status = CSL_dmpacFocoSetConfig(focoRegs, DMPAC_FOCO_CHANNEL_1,
                    &hObj->focoCfg[hObj->curPyrLvl][DOF_INPUT_CURRENT_IMG]);
    }
    return status;
}

static int32_t vhwaM2mDofSubmitRequest(Vhwa_M2mDofInstObj *instObj,
    Vhwa_M2mDofQueueObj *qObj)
{
    int32_t               status;
    uint32_t              pyrLvl;
    Dof_SocInfo          *socInfo = NULL;
    Vhwa_M2mDofHandleObj *hObj = NULL;

    /* Null pointer check */
    GT_assert(VhwaDofTrace, (NULL != instObj));
    GT_assert(VhwaDofTrace, (NULL != qObj));
    GT_assert(VhwaDofTrace, (NULL != qObj->hObj));

    socInfo = &instObj->socInfo;
    hObj = qObj->hObj;
    pyrLvl = hObj->curPyrLvl;


    /* Configure DOF Confidence score parameters */
    (void)CSL_dofSetConfScoreParam(instObj->socInfo.dofRegs, &hObj->confScoreCfg);

    /* Configure DOF Core */
    status = CSL_dofSetConfig(socInfo->dofRegs, &hObj->dofCfg[pyrLvl]);

    CSL_dofEnablePsa(socInfo->dofRegs, hObj->createArgs.enablePsa);

    /* If foco is used set foco paramenters */
    if(hObj->isFocoUsed[pyrLvl] == UTRUE)
    {
        if (FVID2_SOK == status)
        {
            /* Step-2, Configure HWA Common Wrapper LSE */
            status = vhwaM2mDofEnableFoco(instObj, hObj);
        }

        if (FVID2_SOK == status)
        {
            /* Step-2, Configure HWA Common Wrapper LSE */
            status = CSL_lseSetConfig(socInfo->lseRegs, &hObj->lseCfg[pyrLvl]);
        }
    }

    if (FVID2_SOK == status)
    {
        /* Configure HTS */
        status = CSL_dmpacHtsSetThreadConfig(socInfo->htsRegs, &hObj->htsCfg[pyrLvl]);
    }

    if((hObj->isFocoUsed[pyrLvl] == UTRUE) && (FVID2_SOK == status))
    {
        status = CSL_dmpacHtsSetThreadConfig(socInfo->htsRegs,
                                        &hObj->focoHtsCfg[pyrLvl]);
    }

    if (FVID2_SOK == status)
    {
        /* Start HTS Scheduler */
        status = CSL_dmpacHtsThreadStart(socInfo->htsRegs, &hObj->htsCfg[pyrLvl]);

        if((hObj->isFocoUsed[pyrLvl] == UTRUE) && (FVID2_SOK == status))
        {
            status = CSL_dmpacHtsThreadStart(socInfo->htsRegs, &hObj->focoHtsCfg[pyrLvl]);
        }
        else
        {
            (void)CSL_dmpacHtsThreadStopAll(socInfo->htsRegs, &hObj->focoHtsCfg[pyrLvl]);
        }

        if (FVID2_SOK == status)
        {
            /* Set Input and output addresses */
            vhwaM2mDofSetTRAddress(hObj, &qObj->inFrmList,
                        qObj->outFrmList.frames[0U]->addr[0]);

            /* Submit Rings to the Ring Accelerator */
            status = Vhwa_m2mDofSubmitRing(instObj, hObj);
        }
    }

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
        status = CSL_dmpacHtsPipelineStart(socInfo->htsRegs, &hObj->htsCfg[pyrLvl]);
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

    return (status);
}

static void vhwaM2mDofSetLseCfg(Vhwa_M2mDofHandleObj *hObj,
                                const Vhwa_M2mDofPrms *dofPrms)
{
    Fvid2_ColorCompStorageFmt ccsFormat;
    Vhwa_M2mDofSl2Prms *sl2Prms = NULL;
    CSL_LseConfig *lseCfg = NULL;
    Dof_Config    *coreCfg = NULL;
    uint32_t      refBot, refTop, curBot, curTop;
    uint32_t      lineOffset, pyrLvl;

    GT_assert(VhwaDofTrace, (NULL != hObj));
    GT_assert(VhwaDofTrace, (NULL != dofPrms));

    for(pyrLvl = 0; pyrLvl < dofPrms->tPrmdLvl; pyrLvl++)
    {
        if(UTRUE == hObj->isFocoUsed[pyrLvl])
        {
            lseCfg = &hObj->lseCfg[pyrLvl];
            sl2Prms = &hObj->sl2Prms;
            ccsFormat = (Fvid2_ColorCompStorageFmt)dofPrms->inOutImgFmt[pyrLvl]
                                    [DOF_INPUT_CURRENT_IMG].ccsFormat;
            coreCfg = &hObj->dofCfg[pyrLvl];

            Vhwa_m2mDofCalGwPrms(coreCfg, &refBot, &refTop, &curBot, &curTop);

            /* Initialize LSE configuration with the default values */
            CSL_lseConfigInit(lseCfg);

            lineOffset = Vhwa_calcHorzSizeInBytes(coreCfg->width, ccsFormat);

            lineOffset = (lineOffset + 63u) & 0xFFFFFFC0u;

            lseCfg->numInCh = 1;
            lseCfg->numOutCh = 2;

            lseCfg->enableFixedArbMode = 1;

            /* Reference image */
            lseCfg->inChCfg[0U].enable = (uint32_t)UTRUE;
            lseCfg->inChCfg[0U].frameWidth = coreCfg->width;

            if(((refTop + refBot + 2u) > coreCfg->height) &&
                    ((refBot + 2u) >= coreCfg->height))
            {
                lseCfg->inChCfg[0U].frameHeight =
                                    ((2u * coreCfg->height) - 2u - curBot);
            }
            else
            {
                lseCfg->inChCfg[0U].frameHeight =
                                    coreCfg->height + (refBot - curBot);
            }

            lseCfg->inChCfg[0U].enableAddrIncrBy2 = (uint32_t)UFALSE;

            lseCfg->inChCfg[0U].ccsf = ccsFormat;
            lseCfg->inChCfg[0U].startOffset = 0U;

            lseCfg->inChCfg[0U].lineOffset = lineOffset;
            lseCfg->inChCfg[0U].circBufSize = DOF_FOCO_SL2_DEPTH;
            lseCfg->inChCfg[0U].bufAddr[0U] =
                        (uint32_t)sl2Prms->sl2Addr[DOF_INPUT_FOCO_REF_IMG];
            lseCfg->inChCfg[0U].bufAddr[1U] =
                        (uint32_t)sl2Prms->sl2Addr[DOF_INPUT_FOCO_CURR_IMG];
            lseCfg->inChCfg[0U].numInBuff = 2U;

            /* Assumes No padding required when core is bypassed */
            lseCfg->inChCfg[0U].knTopPadding = 1U;
            lseCfg->inChCfg[0U].knBottomPadding = 2U;
            lseCfg->inChCfg[0U].knLineOffset = 1U;
            lseCfg->inChCfg[0U].knHeight = 4U;

            lseCfg->outChCfg[0U].enable = (uint32_t)UTRUE;
            lseCfg->outChCfg[0U].ccsf = FVID2_CCSF_BITS12_PACKED;
            lseCfg->outChCfg[0U].lineOffset = sl2Prms->sl2RefCurPitch;
            lseCfg->outChCfg[0U].circBufSize = refTop + refBot + 2u +
                                sl2Prms->sl2NumLines[DOF_INPUT_CURRENT_IMG];
            lseCfg->outChCfg[0U].bufAddr =
                        (uint32_t)sl2Prms->sl2Addr[DOF_INPUT_REFERENCE_IMG];

            /* Current image */
            lseCfg->outChCfg[1U].enable = (uint32_t)UTRUE;
            lseCfg->outChCfg[1U].ccsf = FVID2_CCSF_BITS12_PACKED;
            lseCfg->outChCfg[1U].lineOffset = sl2Prms->sl2RefCurPitch;
            lseCfg->outChCfg[1U].circBufSize = curTop + curBot + 2u +
                                sl2Prms->sl2NumLines[DOF_INPUT_CURRENT_IMG];
            lseCfg->outChCfg[1U].bufAddr =
                            (uint32_t)sl2Prms->sl2Addr[DOF_INPUT_CURRENT_IMG];
        }
    }
}

static void vhwaM2mDofSetHtsCfg(Vhwa_M2mDofInstObj *instObj,
    Vhwa_M2mDofHandleObj *hObj, const Vhwa_M2mDofPrms *dofPrms)
{
    uint32_t pyrLvl;
    CSL_DmpacHtsSchConfig *htsCfg = NULL;
    CSL_DmpacHtsSchConfig *focoHtsCfg = NULL;
    Dof_Config *dofConfig = NULL;
    uint32_t   refTop,refBot,curTop,curBot;

    GT_assert(VhwaDofTrace, (NULL != instObj));
    GT_assert(VhwaDofTrace, (NULL != hObj));
    GT_assert(VhwaDofTrace, (NULL != dofPrms));

    for(pyrLvl = 0u; pyrLvl < dofPrms->tPrmdLvl; pyrLvl++)
    {
        htsCfg = &hObj->htsCfg[pyrLvl];
        focoHtsCfg = &hObj->focoHtsCfg[pyrLvl];
        dofConfig = &hObj->dofCfg[pyrLvl];

        CSL_dmpacHtsSchConfigInit(htsCfg);

        htsCfg->schId = CSL_HTS_HWA_SCH_DOF;

        htsCfg->pipeline = instObj->pipeline;
        htsCfg->enableStream = (uint32_t)UFALSE;
        htsCfg->enableHop = (uint32_t)UFALSE;
        htsCfg->enableWdTimer = (uint32_t)UFALSE;
        htsCfg->enableBwLimit = (uint32_t)UFALSE;

        htsCfg->enableBwLimit = UFALSE;

        Vhwa_m2mDofCalGwPrms(dofConfig, &refBot, &refTop, &curBot, &curTop);

        CSL_dmpacHtsSchConfigInit(focoHtsCfg);
        focoHtsCfg->schId = CSL_HTS_HWA_SCH_FOCO1;

        if(hObj->isFocoUsed[pyrLvl] == UTRUE)
        {
            focoHtsCfg->pipeline = instObj->pipeline;
            focoHtsCfg->enableStream = (uint32_t)UFALSE;
            focoHtsCfg->enableHop = (uint32_t)UFALSE;
            focoHtsCfg->enableWdTimer = (uint32_t)UFALSE;
            focoHtsCfg->enableBwLimit = (uint32_t)UFALSE;

            /* RFGW Config */
            focoHtsCfg->consCfg[HTS_DOF_REFE_FRAME_CH_IDX].enable = (uint32_t)UTRUE;
            focoHtsCfg->consCfg[HTS_DOF_REFE_FRAME_CH_IDX].prodId = CSL_HTS_PROD_IDX_UDMA;

            focoHtsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].enable = (uint32_t)UTRUE;
            focoHtsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].dmaChNum =
                                Udma_chGetNum(instObj->utcChHndl[DOF_INPUT_FOCO_REF_IMG]);
            focoHtsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].pipeline = instObj->pipeline;
            focoHtsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].consId = CSL_HTS_CONS_IDX_UDMA;
            /* Default mapping for DMA */

            focoHtsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].threshold = 1u;
            focoHtsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].cntPreLoad = 0u;

            if(((refTop + refBot + 2u) > dofConfig->height) &&
                ((refBot + 2u) >= dofConfig->height))
            {
                focoHtsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].cntPostLoad =
                            dofConfig->height - 2u - curBot;
            }
            else
            {
                focoHtsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].cntPostLoad =
                            refBot - curBot;
            }

            focoHtsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].enableHop = (uint32_t)UTRUE;
            focoHtsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].numHop = dofConfig->height;
            focoHtsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].countDec = 1U;
            focoHtsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].depth = DOF_FOCO_SL2_DEPTH;


            focoHtsCfg->prodCfg[HTS_DOF_REFE_FRAME_CH_IDX].enable = (uint32_t)UTRUE;
            focoHtsCfg->prodCfg[HTS_DOF_REFE_FRAME_CH_IDX].consId = CSL_HTS_CONS_IDX_DOF_REF;

            if((refTop + refBot + 2u) > dofConfig->height)
            {
                focoHtsCfg->prodCfg[HTS_DOF_REFE_FRAME_CH_IDX].threshold =
                            dofConfig->height;
                if((refBot + 2u) < dofConfig->height)
                {
                    focoHtsCfg->prodCfg[HTS_DOF_REFE_FRAME_CH_IDX].cntPreLoad =
                           (dofConfig->height - refBot - 2u);
                }
                else
                {
                    focoHtsCfg->prodCfg[HTS_DOF_REFE_FRAME_CH_IDX].cntPreLoad = 0u;
                }

                focoHtsCfg->prodCfg[HTS_DOF_REFE_FRAME_CH_IDX].cntPostLoad =
                        curBot + (2u * dofConfig->medianFilter);
            }
            else
            {
                focoHtsCfg->prodCfg[HTS_DOF_REFE_FRAME_CH_IDX].threshold =
                                 (refTop + refBot + 2u);
                focoHtsCfg->prodCfg[HTS_DOF_REFE_FRAME_CH_IDX].cntPreLoad = refTop;
                focoHtsCfg->prodCfg[HTS_DOF_REFE_FRAME_CH_IDX].cntPostLoad =
                                    curBot + (2u * dofConfig->medianFilter);
            }

            focoHtsCfg->prodCfg[HTS_DOF_REFE_FRAME_CH_IDX].depth =
                        (refTop + refBot + 2u +
                         hObj->sl2Prms.sl2NumLines[DOF_INPUT_REFERENCE_IMG]);
            focoHtsCfg->prodCfg[HTS_DOF_REFE_FRAME_CH_IDX].countDec = 2U;


            /* CFGW Config */
            focoHtsCfg->consCfg[HTS_DOF_CUR_FRAME_CH_IDX].enable = (uint32_t)UTRUE;
            focoHtsCfg->consCfg[HTS_DOF_CUR_FRAME_CH_IDX].prodId = CSL_HTS_PROD_IDX_UDMA;

            focoHtsCfg->dmaProdCfg[HTS_DOF_CUR_FRAME_CH_IDX].enable = (uint32_t)UTRUE;
            focoHtsCfg->dmaProdCfg[HTS_DOF_CUR_FRAME_CH_IDX].dmaChNum =
                            Udma_chGetNum(instObj->utcChHndl[DOF_INPUT_FOCO_CURR_IMG]);
            focoHtsCfg->dmaProdCfg[HTS_DOF_CUR_FRAME_CH_IDX].pipeline = instObj->pipeline;
            focoHtsCfg->dmaProdCfg[HTS_DOF_CUR_FRAME_CH_IDX].consId = CSL_HTS_CONS_IDX_UDMA;
            /* Default mapping for DMA */

            focoHtsCfg->dmaProdCfg[HTS_DOF_CUR_FRAME_CH_IDX].threshold = 1u;
            focoHtsCfg->dmaProdCfg[HTS_DOF_CUR_FRAME_CH_IDX].cntPreLoad =
                           focoHtsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].cntPostLoad;
            focoHtsCfg->dmaProdCfg[HTS_DOF_CUR_FRAME_CH_IDX].cntPostLoad = 0u;

            focoHtsCfg->dmaProdCfg[HTS_DOF_CUR_FRAME_CH_IDX].enableHop = (uint32_t)UTRUE;
            focoHtsCfg->dmaProdCfg[HTS_DOF_CUR_FRAME_CH_IDX].numHop = dofConfig->height;
            focoHtsCfg->dmaProdCfg[HTS_DOF_CUR_FRAME_CH_IDX].countDec = 1U;
            focoHtsCfg->dmaProdCfg[HTS_DOF_CUR_FRAME_CH_IDX].depth = DOF_FOCO_SL2_DEPTH;


            focoHtsCfg->prodCfg[HTS_DOF_CUR_FRAME_CH_IDX].enable = (uint32_t)UTRUE;
            focoHtsCfg->prodCfg[HTS_DOF_CUR_FRAME_CH_IDX].consId = CSL_HTS_CONS_IDX_DOF_CURR;

            focoHtsCfg->prodCfg[HTS_DOF_CUR_FRAME_CH_IDX].threshold =
                                        (curTop + curBot + 2u);

            focoHtsCfg->prodCfg[HTS_DOF_CUR_FRAME_CH_IDX].cntPreLoad = curTop;
            focoHtsCfg->prodCfg[HTS_DOF_CUR_FRAME_CH_IDX].cntPostLoad =
                                        curBot + (2u * dofConfig->medianFilter);


            focoHtsCfg->prodCfg[HTS_DOF_CUR_FRAME_CH_IDX].depth =
                         (curTop + curBot + 2u +
                         hObj->sl2Prms.sl2NumLines[DOF_INPUT_CURRENT_IMG]);
            focoHtsCfg->prodCfg[HTS_DOF_CUR_FRAME_CH_IDX].countDec = 2U;

            /* Consumer control for RFGW */
            htsCfg->consCfg[HTS_DOF_REFE_FRAME_CH_IDX].enable = (uint32_t)UTRUE;
            htsCfg->consCfg[HTS_DOF_REFE_FRAME_CH_IDX].prodId = CSL_HTS_PROD_IDX_FOCO0_0;

            /* Consumer control for CFGW */
            htsCfg->consCfg[HTS_DOF_CUR_FRAME_CH_IDX].enable = (uint32_t)UTRUE;
            htsCfg->consCfg[HTS_DOF_CUR_FRAME_CH_IDX].prodId = CSL_HTS_PROD_IDX_FOCO0_1;

        }
        else
        {
            /* RFGW Config */
            htsCfg->consCfg[HTS_DOF_REFE_FRAME_CH_IDX].enable = (uint32_t)UTRUE;
            htsCfg->consCfg[HTS_DOF_REFE_FRAME_CH_IDX].prodId = CSL_HTS_PROD_IDX_UDMA;

            htsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].enable = (uint32_t)UTRUE;
            htsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].dmaChNum =
                        Udma_chGetNum(instObj->utcChHndl[DOF_INPUT_REFERENCE_IMG]);
            htsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].pipeline = instObj->pipeline;
            htsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].consId = CSL_HTS_CONS_IDX_UDMA;
            /* Default mapping for DMA */

            if((refTop + refBot + 2u) > dofConfig->height)
            {
                htsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].threshold =
                            dofConfig->height/2U;
                if((refBot + 2u) < dofConfig->height)
                {
                    htsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].cntPreLoad =
                           (dofConfig->height - refBot - 2u)/2U;
                }
                else
                {
                    htsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].cntPreLoad = 0;
                }
                if(refBot < dofConfig->height)
                {
                    htsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].cntPostLoad =
                            (refBot/2U)
                            + dofConfig->medianFilter;
                }
                else
                {
                    htsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].cntPostLoad =
                            ((dofConfig->height - 2u)/2U)
                            + dofConfig->medianFilter;
                }
            }
            else
            {
                htsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].threshold =
                            (refTop + refBot + 2u)/2U;
                htsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].cntPreLoad =
                            refTop/2U;
                htsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].cntPostLoad =
                            (refBot/2U) + dofConfig->medianFilter/*1*/;
            }

            htsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].enableHop = (uint32_t)UTRUE;
            htsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].numHop =
                            dofConfig->height/2U;
            htsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].countDec = 1U;
            htsCfg->dmaProdCfg[HTS_DOF_REFE_FRAME_CH_IDX].depth =
                        (refTop + refBot + 2u +
                         hObj->sl2Prms.sl2NumLines[DOF_INPUT_REFERENCE_IMG])/2U;


            /* CFGW Config */
            htsCfg->consCfg[HTS_DOF_CUR_FRAME_CH_IDX].enable = (uint32_t)UTRUE;
            htsCfg->consCfg[HTS_DOF_CUR_FRAME_CH_IDX].prodId = CSL_HTS_PROD_IDX_UDMA;

            htsCfg->dmaProdCfg[HTS_DOF_CUR_FRAME_CH_IDX].enable = (uint32_t)UTRUE;
            htsCfg->dmaProdCfg[HTS_DOF_CUR_FRAME_CH_IDX].dmaChNum =
                        Udma_chGetNum(instObj->utcChHndl[DOF_INPUT_CURRENT_IMG]);
            htsCfg->dmaProdCfg[HTS_DOF_CUR_FRAME_CH_IDX].pipeline = instObj->pipeline;
            htsCfg->dmaProdCfg[HTS_DOF_CUR_FRAME_CH_IDX].consId = CSL_HTS_CONS_IDX_UDMA;
            /* Default mapping for DMA */

            htsCfg->dmaProdCfg[HTS_DOF_CUR_FRAME_CH_IDX].threshold =
                        (curTop + curBot + 2U)/2U;
            htsCfg->dmaProdCfg[HTS_DOF_CUR_FRAME_CH_IDX].cntPreLoad = curTop/2U;
            htsCfg->dmaProdCfg[HTS_DOF_CUR_FRAME_CH_IDX].cntPostLoad =
                            (curBot/2U) + dofConfig->medianFilter;

            htsCfg->dmaProdCfg[HTS_DOF_CUR_FRAME_CH_IDX].enableHop = (uint32_t)UTRUE;
            htsCfg->dmaProdCfg[HTS_DOF_CUR_FRAME_CH_IDX].numHop = dofConfig->height/2U;
            htsCfg->dmaProdCfg[HTS_DOF_CUR_FRAME_CH_IDX].countDec = 1U;
            htsCfg->dmaProdCfg[HTS_DOF_CUR_FRAME_CH_IDX].depth =
                        htsCfg->dmaProdCfg[HTS_DOF_CUR_FRAME_CH_IDX].threshold +
                        (hObj->sl2Prms.sl2NumLines[DOF_INPUT_CURRENT_IMG]/2U);
        }

        if((UTRUE == hObj->isTempPred) && (0u == pyrLvl))
        {
            /* Temporal predictor config */
            htsCfg->consCfg[HTS_DOF_TEM_PRE_CH_IDX].enable = (uint32_t)UTRUE;
            htsCfg->consCfg[HTS_DOF_TEM_PRE_CH_IDX].prodId = CSL_HTS_PROD_IDX_UDMA;

            htsCfg->dmaProdCfg[HTS_DOF_TEM_PRE_CH_IDX].enable = (uint32_t)UTRUE;
            htsCfg->dmaProdCfg[HTS_DOF_TEM_PRE_CH_IDX].dmaChNum =
                Udma_chGetNum(instObj->utcChHndl[DOF_INPUT_TEMPORAL_PRED]);
            htsCfg->dmaProdCfg[HTS_DOF_TEM_PRE_CH_IDX].pipeline = instObj->pipeline;
            htsCfg->dmaProdCfg[HTS_DOF_TEM_PRE_CH_IDX].consId = CSL_HTS_CONS_IDX_UDMA;
            /* Default mapping for DMA */

            htsCfg->dmaProdCfg[HTS_DOF_TEM_PRE_CH_IDX].threshold = 1u;
            htsCfg->dmaProdCfg[HTS_DOF_TEM_PRE_CH_IDX].cntPreLoad = 0u;
            htsCfg->dmaProdCfg[HTS_DOF_TEM_PRE_CH_IDX].cntPostLoad = dofConfig->medianFilter;

            htsCfg->dmaProdCfg[HTS_DOF_TEM_PRE_CH_IDX].enableHop = (uint32_t)UTRUE;
            htsCfg->dmaProdCfg[HTS_DOF_TEM_PRE_CH_IDX].numHop = dofConfig->height/2u;
            htsCfg->dmaProdCfg[HTS_DOF_TEM_PRE_CH_IDX].countDec = 1U;
            htsCfg->dmaProdCfg[HTS_DOF_TEM_PRE_CH_IDX].depth =
                                       DOF_TEMPORAL_PRED_SL2_DEPTH;
        }

        if (UTRUE == hObj->isPyrPred[pyrLvl])
        {
            /* Pyramidal predictor config */
            htsCfg->consCfg[HTS_DOF_PYR_PRE_CH_IDX].enable = (uint32_t)UTRUE;
            htsCfg->consCfg[HTS_DOF_PYR_PRE_CH_IDX].prodId = CSL_HTS_PROD_IDX_UDMA;


            htsCfg->dmaProdCfg[HTS_DOF_PYR_PRE_CH_IDX].enable = (uint32_t)UTRUE;
            htsCfg->dmaProdCfg[HTS_DOF_PYR_PRE_CH_IDX].dmaChNum =
                    Udma_chGetNum(instObj->utcChHndl[DOF_INPUT_PYRAMID_PRED]);
            htsCfg->dmaProdCfg[HTS_DOF_PYR_PRE_CH_IDX].pipeline = instObj->pipeline;
            htsCfg->dmaProdCfg[HTS_DOF_PYR_PRE_CH_IDX].consId = CSL_HTS_CONS_IDX_UDMA;
            /* Default mapping for DMA */

            htsCfg->dmaProdCfg[HTS_DOF_PYR_PRE_CH_IDX].threshold = 1U;
            htsCfg->dmaProdCfg[HTS_DOF_PYR_PRE_CH_IDX].cntPreLoad = 0u;
            htsCfg->dmaProdCfg[HTS_DOF_PYR_PRE_CH_IDX].cntPostLoad = dofConfig->medianFilter;

            htsCfg->dmaProdCfg[HTS_DOF_PYR_PRE_CH_IDX].enableHop = (uint32_t)UTRUE;
            htsCfg->dmaProdCfg[HTS_DOF_PYR_PRE_CH_IDX].numHop = dofConfig->height/2u;
            htsCfg->dmaProdCfg[HTS_DOF_PYR_PRE_CH_IDX].countDec = 1U;
            htsCfg->dmaProdCfg[HTS_DOF_PYR_PRE_CH_IDX].depth =
                                       DOF_PYRAM_PRED_SL2_DEPTH;
        }

        if((UTRUE == hObj->isSof) && (0u == pyrLvl))
        {
            /* SOF config */
            htsCfg->consCfg[HTS_DOF_SOF_CH_IDX].enable = (uint32_t)UTRUE;
            htsCfg->consCfg[HTS_DOF_SOF_CH_IDX].prodId = CSL_HTS_PROD_IDX_UDMA;

            htsCfg->dmaProdCfg[HTS_DOF_SOF_CH_IDX].enable = (uint32_t)UTRUE;
            htsCfg->dmaProdCfg[HTS_DOF_SOF_CH_IDX].dmaChNum =
                    Udma_chGetNum(instObj->utcChHndl[DOF_INPUT_SOF]);
            htsCfg->dmaProdCfg[HTS_DOF_SOF_CH_IDX].pipeline = instObj->pipeline;
            htsCfg->dmaProdCfg[HTS_DOF_SOF_CH_IDX].consId = CSL_HTS_CONS_IDX_UDMA;
            /* Default mapping for DMA */

            htsCfg->dmaProdCfg[HTS_DOF_SOF_CH_IDX].threshold = 1U;
            htsCfg->dmaProdCfg[HTS_DOF_SOF_CH_IDX].cntPreLoad = 0u;
            htsCfg->dmaProdCfg[HTS_DOF_SOF_CH_IDX].cntPostLoad =
                                                dofConfig->medianFilter;

            htsCfg->dmaProdCfg[HTS_DOF_SOF_CH_IDX].enableHop = (uint32_t)UTRUE;
            htsCfg->dmaProdCfg[HTS_DOF_SOF_CH_IDX].numHop = dofConfig->height/2u;
            htsCfg->dmaProdCfg[HTS_DOF_SOF_CH_IDX].countDec = 1U;
            if(dofConfig->medianFilter == UTRUE)
            {
                htsCfg->dmaProdCfg[HTS_DOF_SOF_CH_IDX].depth =
                                       (DOF_SOF_SL2_DEPTH/2u) - 1U;
            }
            else
            {
                htsCfg->dmaProdCfg[HTS_DOF_SOF_CH_IDX].depth =
                                       DOF_SOF_SL2_DEPTH/2u;
            }
        }

        /* Output flow config */
        htsCfg->prodCfg[HTS_DOF_OUT_CH_IDX].enable = (uint32_t)UTRUE;
        htsCfg->prodCfg[HTS_DOF_OUT_CH_IDX].consId = CSL_HTS_CONS_IDX_UDMA;

        htsCfg->prodCfg[HTS_DOF_OUT_CH_IDX].threshold = 1u;
        htsCfg->prodCfg[HTS_DOF_OUT_CH_IDX].cntPreLoad = 0u;
        htsCfg->prodCfg[HTS_DOF_OUT_CH_IDX].cntPostLoad = 0u;

        /* Output 2 lines per dma transfer*/
        htsCfg->prodCfg[HTS_DOF_OUT_CH_IDX].depth =
                hObj->sl2Prms.sl2NumLines[DOF_OUTPUT]/2U;
        htsCfg->prodCfg[HTS_DOF_OUT_CH_IDX].countDec = 1U;

        htsCfg->dmaConsCfg[HTS_DOF_OUT_CH_IDX].enable = (uint32_t)UTRUE;
        htsCfg->dmaConsCfg[HTS_DOF_OUT_CH_IDX].dmaChNum =
                Udma_chGetNum(instObj->utcChHndl[DOF_OUTPUT]);
        htsCfg->dmaConsCfg[HTS_DOF_OUT_CH_IDX].pipeline = instObj->pipeline;
        htsCfg->dmaConsCfg[HTS_DOF_OUT_CH_IDX].enableStream = (uint32_t)UFALSE;
        htsCfg->dmaConsCfg[HTS_DOF_OUT_CH_IDX].prodId = CSL_HTS_PROD_IDX_UDMA;
    }
}

static int32_t vhwaM2mDofSetSl2Params(const Vhwa_M2mDofInstObj *instObj,
        Vhwa_M2mDofHandleObj *hObj, Vhwa_M2mDofPrms *dofPrms)
{
    int32_t status = FVID2_SOK;
    uint32_t pitchSl2, width, pyrLvl, isFocoUsed;
    uint32_t rfgw, cfgw, sofMem, pyrMem, tempMem, focoBuffMem, outMem, tMemReq;
    uint64_t sl2StartAddr;
    Dof_Config          *coreCfg;;
    Vhwa_M2mDofSl2Prms  *sl2Prms;

    GT_assert(VhwaDofTrace, (NULL != instObj));
    GT_assert(VhwaDofTrace, (NULL != hObj));
    GT_assert(VhwaDofTrace, (NULL != dofPrms));

    coreCfg = &dofPrms->coreCfg;
    sl2Prms = &hObj->sl2Prms;
    sl2StartAddr = instObj->startAddr;

    isFocoUsed = UFALSE;

    width = coreCfg->width;

    /* Set ref and Base buffer Parameters */
    pitchSl2 = Vhwa_calcHorzSizeInBytes(width,
                        FVID2_CCSF_BITS12_PACKED);
    pitchSl2 = Vhwa_calcSl2Pitch(pitchSl2);

    /* RFGW */
    rfgw = pitchSl2 * (((coreCfg->topSearchRange + 7u) & 0xFFFFFFFEu)
                    + 2u + ((coreCfg->bottomSearchRange + 10u) & 0xFFFFFFFEu)
                    + sl2Prms->sl2NumLines[DOF_INPUT_REFERENCE_IMG]);

    /* CFGW */
    cfgw = pitchSl2 * (6u + 2u + 8u + sl2Prms->sl2NumLines[DOF_INPUT_REFERENCE_IMG]);

    /* Calculate Temporal Predictor memory requirement */
    tempMem = CSL_lseMakePitchAligned(width * 4U) *
                DOF_TEMPORAL_PRED_SL2_DEPTH;

    /* Calculate Pyramidal Predictor memory requirement */
    pyrMem = CSL_lseMakePitchAligned(width) * DOF_PYRAM_PRED_SL2_DEPTH;

    /* Calculate SOF memory requirement */
    sofMem = width/8u;
    if((sofMem * 8u) != width)
    {
        sofMem++;
    }

    sofMem = CSL_lseMakePitchAligned(sofMem) * DOF_SOF_SL2_DEPTH;

    /* Output Buffer Memory Required */
    outMem = CSL_lseMakePitchAligned(width * 4U) *
                sl2Prms->sl2NumLines[DOF_OUTPUT];

    for (pyrLvl = 0u; pyrLvl < dofPrms->tPrmdLvl; pyrLvl++)
    {
        if(FVID2_CCSF_BITS12_PACKED !=
                dofPrms->inOutImgFmt[pyrLvl][DOF_INPUT_CURRENT_IMG].ccsFormat)
        {
            isFocoUsed = UTRUE;
            break;
        }
    }
    /* Set FOCO Parameters */
    if(UFALSE == isFocoUsed)
    {
        focoBuffMem = 0u;
    }
    else
    {
        focoBuffMem = Vhwa_calcHorzSizeInBytes(width,
                dofPrms->inOutImgFmt[pyrLvl][DOF_INPUT_CURRENT_IMG].ccsFormat);
        focoBuffMem = CSL_lseMakePitchAligned(focoBuffMem) * DOF_FOCO_SL2_DEPTH;
    }

    tMemReq = rfgw + cfgw + sofMem + pyrMem + tempMem +
              (focoBuffMem * 2u) + outMem;

    if(tMemReq <= instObj->sl2Size)
    {
        sl2Prms->sl2Addr[DOF_INPUT_REFERENCE_IMG] = sl2StartAddr;
        sl2StartAddr = sl2StartAddr + rfgw;
        sl2Prms->sl2Addr[DOF_INPUT_CURRENT_IMG] = sl2StartAddr;
        sl2Prms->sl2RefCurPitch = pitchSl2;
        sl2StartAddr = sl2StartAddr + cfgw;

        sl2Prms->sl2Addr[DOF_INPUT_TEMPORAL_PRED] = sl2StartAddr;
        sl2StartAddr = sl2StartAddr + tempMem;

        sl2Prms->sl2Addr[DOF_INPUT_PYRAMID_PRED] = sl2StartAddr;
        sl2StartAddr = sl2StartAddr + pyrMem;

        sl2Prms->sl2Addr[DOF_INPUT_SOF] = sl2StartAddr;
        sl2StartAddr = sl2StartAddr + sofMem;

        sl2Prms->sl2Addr[DOF_OUTPUT] = sl2StartAddr;
        sl2StartAddr = sl2StartAddr + outMem;

        sl2Prms->sl2Addr[DOF_INPUT_FOCO_REF_IMG] = sl2StartAddr;
        sl2StartAddr = sl2StartAddr + focoBuffMem;
        sl2Prms->sl2Addr[DOF_INPUT_FOCO_CURR_IMG] = sl2StartAddr;
    }
    else
    {
        status = FVID2_EFAIL;
    }

    return status;
}

static void vhwaM2mDofSetBuffParams(Vhwa_M2mDofHandleObj *hObj,
                                    Vhwa_M2mDofPrms *dofPrms)
{
    uint32_t curTop, curBot, refTop, refBot;
    Dof_Config *coreCfg;

    GT_assert(VhwaDofTrace, (NULL != hObj));
    GT_assert(VhwaDofTrace, (NULL != dofPrms));

    coreCfg = &dofPrms->coreCfg;

    Vhwa_m2mDofCalGwPrms(coreCfg, &refBot, &refTop, &curBot, &curTop);

    /* Current Frame Buffer config */
    hObj->dofBuffPrams.currImageAddress =
                    (uint32_t)hObj->sl2Prms.sl2Addr[DOF_INPUT_CURRENT_IMG];
    hObj->dofBuffPrams.currBufWidth = hObj->sl2Prms.sl2RefCurPitch;
    /* line level staggering is done while organizing the growing window */
    hObj->dofBuffPrams.currBufHeight = (curTop + curBot + 2u +
                        hObj->sl2Prms.sl2NumLines[DOF_INPUT_CURRENT_IMG]);

    /* Reference Frame buffer config */
    hObj->dofBuffPrams.refImageAddress =
                    (uint32_t)hObj->sl2Prms.sl2Addr[DOF_INPUT_REFERENCE_IMG];
    hObj->dofBuffPrams.refBufWidth = hObj->sl2Prms.sl2RefCurPitch;

    hObj->dofBuffPrams.refBufHeight =
                        refTop + refBot + 2u +
                        hObj->sl2Prms.sl2NumLines[DOF_INPUT_REFERENCE_IMG];

    hObj->dofBuffPrams.spatialPredAddress =
                    (uint32_t)hObj->sl2Prms.sl2Addr[DOF_INPUT_PYRAMID_PRED];
    hObj->dofBuffPrams.spatialPredDepth = DOF_PYRAM_PRED_SL2_DEPTH;
    hObj->dofBuffPrams.temporalPredAddress =
                    (uint32_t)hObj->sl2Prms.sl2Addr[DOF_INPUT_TEMPORAL_PRED];
    hObj->dofBuffPrams.temporalPredDepth = DOF_TEMPORAL_PRED_SL2_DEPTH;
    hObj->dofBuffPrams.binaryMapAddress =
                    (uint32_t)hObj->sl2Prms.sl2Addr[DOF_INPUT_SOF];
    hObj->dofBuffPrams.binaryMapDepth = DOF_SOF_SL2_DEPTH;
    hObj->dofBuffPrams.outFlowAddress =
                    (uint32_t)hObj->sl2Prms.sl2Addr[DOF_OUTPUT];
    hObj->dofBuffPrams.outFlowpDepth = hObj->sl2Prms.sl2NumLines[DOF_OUTPUT];

}

static int32_t vhwaM2mDofCreateQueues(Vhwa_M2mDofInstObj *instObj)
{
    int32_t              status;
    uint32_t             qCnt;
    Vhwa_M2mDofQueueObj *qObj;

    /* NULL pointer check */
    GT_assert(VhwaDofTrace, (NULL != instObj));

    instObj->freeQ = NULL;

    /* Create Free queue */
    status = Fvid2Utils_constructQ(&instObj->freeQObj);
    GT_assert(VhwaDofTrace, (FVID2_SOK == status));

    instObj->freeQ = &instObj->freeQObj;

    /* Initialize and queue the allocate queue object to free Q */
    for(qCnt = 0U; qCnt < VHWA_M2M_DOF_UDMA_RING_ENTRIES; qCnt ++)
    {
        qObj = &instObj->dofQObj[qCnt];

        Fvid2Utils_memset(qObj, 0x0, sizeof(Vhwa_M2mDofQueueObj));

        Fvid2Utils_queue(instObj->freeQ, &qObj->qElem, qObj);
    }

    return (status);
}

static void vhwaM2mDofDeleteQueues(Vhwa_M2mDofInstObj *instObj)
{
    Vhwa_M2mDofQueueObj *qObj = NULL;

    /* NULL pointer check */
    GT_assert(VhwaDofTrace, (NULL != instObj));

    if(NULL != instObj->freeQ)
    {
        /* Free-up all the queued free queue objects */
        do
        {
            qObj = (Vhwa_M2mDofQueueObj *) Fvid2Utils_dequeue(instObj->freeQ);
        } while (NULL != qObj);

        /* Delete the free Q */
        Fvid2Utils_destructQ(instObj->freeQ);
        instObj->freeQ = NULL;
    }
}

static void vhwaM2mDofSetBwLimitParams(Vhwa_M2mDofHandleObj *hObj,
                                       const Vhwa_HtsLimiter *bwLimit)
{
    uint32_t pyrLvl;

    for (pyrLvl = 0u; pyrLvl < hObj->dofPrms.tPrmdLvl; pyrLvl++)
    {
        /* Setting directly in the HTS configuration, so that
         * it will be configured from the next request. */
        hObj->htsCfg[pyrLvl].enableBwLimit   = bwLimit->enableBwLimit;
        hObj->htsCfg[pyrLvl].cycleCnt        = bwLimit->cycleCnt;
        hObj->htsCfg[pyrLvl].tokenCnt        = bwLimit->tokenCnt;
    }
}
