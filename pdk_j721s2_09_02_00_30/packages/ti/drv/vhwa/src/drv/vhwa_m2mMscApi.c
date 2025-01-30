/**
 *   Copyright (c) Texas Instruments Incorporated 2018
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
 *  \file vhwa_m2mMscApi.c
 *
 *  \brief API Implementation
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/drv/vhwa/src/drv/vhwa_m2mMscPriv.h>


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
 * \brief   Function to initialize the internal driver paraeters
 *
 * \param   pInitPrms         Init parametes required for initialize
 *
 * \return  FVID2_SOK in case of sucess, Error otherwise
 *
 **/
static int32_t Vhwa_mscInit(const Vhwa_M2mMscInitParams *pInitPrms);

/**
 * \brief   Function to de-initialize the internal driver paraeters
 *
 * \return  FVID2_SOK in case of sucess, Error otherwise
 *
 **/
static int32_t Vhwa_mscDeInit(void);

/**
 * \brief   Create Queues, required for storing pending requests
 *
 * \param   instObj             Instance object.
 *
 * \return  FVID2_SOK in case of sucess, Error otherwise
 *
 **/
static int32_t Vhwa_mscCreateQueue(Vhwa_M2mMscInstObj *instObj);

/**
 * \brief   Delete Queues
 *
 * \param   instObj             Instance object.
 *
 * \return  FVID2_SOK in case of sucess, Error otherwise
 *
 **/
static int32_t Vhwa_mscDeleteQueue(Vhwa_M2mMscInstObj *instObj);

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
static Vhwa_M2mMscHandleObj *Vhwa_mscAllocHdlObj(Vhwa_M2mMscInstObj *instObj,
                                                 uint32_t instId);

/**
 * \brief   Local Function to free Handle Object
 *          No protection inside the function, Caller should protect
 *          the function call
 *
 * \param   instObj      Instance object.
 *
 */
static void Vhwa_mscFreeHandleObj(Vhwa_M2mMscHandleObj *hObj);

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
static void Vhwa_mscInitHandleObj(const Vhwa_M2mMscInstObj *instObj,
                                  Vhwa_M2mMscHandleObj *hObj);

/**
 * \brief   Local function to set the MSC filter coefficients
 *
 *
 * \param   comObj             Common driver object
 * \param   pCoeffCfg          MSC coeff parameters
 *
 * \return  FVID2_SOK in case of sucess, Error otherwise
 *
 **/
static int32_t Vhwa_mscSetFilterCoeff(Vhwa_M2mMscCommonObj *comObj,
                                      const Msc_Coeff *pCoeffCfg);

/**
 * \brief   Implementation of SET_PARAMS ioctl.
 *          It uses #Vhwa_mscCheckMscCfg to validate the config.
 *          If it is valid, copies the config into handle object
 *          If it is invalid, it reverts LSE/HTS config to known valid config
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 * \param   comObj              Common driver Object
 * \param   mscPrms             MSC configuration to be set
 *
 * \return  FVID2_SOK in case of sucess, Error otherwise
 *
 **/
static int32_t Vhwa_mscSetParams(Vhwa_M2mMscInstObj *instObj,
                                 Vhwa_M2mMscHandleObj *hObj,
                                 const Vhwa_M2mMscCommonObj *comObj,
                                 Vhwa_M2mMscParams *mscPrms);

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
static int32_t vhwa_m2mMscSetEeParams(const Vhwa_M2mMscInstObj *instObj,
    Vhwa_M2mMscHandleObj *hObj, const Msc_ErrEventParams *eePrms);

/**
 * \brief   Local function to initilize the MSc with default configuration
 *
 * \param   mscPrms            MSC parameters to be initialized
 *
 **/
static void Vhwa_mscInitMscCfg(Vhwa_M2mMscParams *mscPrms);

/**
 * \brief   Based on the given MSC config, it initializes HTS configuration.
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 * \param   mscPrms             MSC Configuration
 *
 **/
static void Vhwa_mscSetHtsCfg(const Vhwa_M2mMscInstObj *instObj,
                              Vhwa_M2mMscHandleObj *hObj,
                              const Vhwa_M2mMscParams *mscPrms);

/**
 * \brief   Based on the given MSC config, it initializes LSE configuration.
 *
 * \param   instObj        Instance Object
 * \param   hObj           Handle Object
 * \param   mscPrms        MSC Configuration
 *
 **/
static void Vhwa_mscSetLseCfg(Vhwa_M2mMscHandleObj *hObj,
                              const Vhwa_M2mMscParams *mscPrms);

/**
 * \brief   Local function to check the MSC configuration
 *
 * \param   instObj             Instance Object
 * \param   hObj                Handle Object
 * \param   mscPrms             MSC Configuration
 *
 * \return  FVID2_SOK in case of correct configuration, Error otherwise
 *
 **/
static int32_t Vhwa_mscCheckMscCfg(Vhwa_M2mMscHandleObj *hObj,
                                   Vhwa_M2mMscParams *mscPrms);

/**
 * \brief   Local function to set CSL paramenter for Luma and Chroma based on
 *          MSC configuration
 *
 * \param   hObj                Handle Object
 * \param   mscPrms             MSC Configuration
 *
 **/
static void Vhwa_mscCalPrms(Vhwa_M2mMscHandleObj *hObj,
                                 const Vhwa_M2mMscParams *mscPrms);

/**
 * \brief   Local function to set HTS Bandwidth limiter
 *
 * \param   hObj                Handle Object
 * \param   bwLimit             Bandwidth limiter Configuration
 *
 **/
static void Vhwa_mscSetHtsLimit(Vhwa_M2mMscHandleObj *hObj,
                               const Vhwa_HtsLimiter *htsLimit);

/**
 * \brief   Funtion to Update the Parameters with Flexconnect changes
 *
 * \param   hObj                Handle Object
 * \param   sl2FcPrms             Bandwidth limiter Configuration
 *
 **/
static void Vhwa_m2mMscUpdateFcConnPrms(Vhwa_M2mMscHandleObj *hObj,
                                Vhwa_M2mMscFcConPrms *sl2FcPrms);

static void Vhwa_m2mVissUpdateFcPrms(Vhwa_M2mMscHandleObj *hObj,
                                    Vhwa_M2mMscFcUpdatePrms *fcPrms);

static void Vhwa_m2mMscFcGetPrms(Vhwa_M2mMscHandleObj *hObj,
                                Vhwa_M2mMscFcGetPrms *fcPrms);
/**
 * \brief   Local function to check MSC Create params like Driver Id and Instance Id.
 *
 * \param   drvId               Driver ID
 * \param   drvInstId           Instance ID
 *
 *  \return FVID2_SOK if successful, else suitable error code
 **/
static int32_t Vhwa_m2mMscCheckCreatePrms(uint32_t drvId, uint32_t drvInstId);

/* Implementation of FVID2 APIs */
/**
 * \brief   FVID2 Create Function.
 *
 * \param   instObj             Instance object.
 *
 * \return  FVID2 Driver Handle.
 *
 **/
static Fdrv_Handle Vhwa_m2mMscCreate(uint32_t drvId, uint32_t drvInstId,
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
static int32_t Vhwa_m2mMscDelete(Fdrv_Handle handle, Ptr deleteArgs);

/**
 * \brief   FVID2 Control Function.
 *
 * \param   handle              FVID2 driver handle.
 *
 * \return  FVID2_SOK on success, else FVID2 error code
 *
 **/
static int32_t Vhwa_m2mMscControl(Fdrv_Handle handle, uint32_t cmd,
                                  Ptr cmdArgs, Ptr cmdStatusArgs);

/**
 * \brief   FVID2 Process Request function.
 *
 * \param   handle              FVID2 driver handle.
 *
 * \return  FVID2_SOK on success, else FVID2 error code
 *
 **/
static int32_t Vhwa_m2mMscProcessReq(Fdrv_Handle handle,
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
static int32_t Vhwa_m2mMscGetProcessReq(Fdrv_Handle handle,
                                        Fvid2_FrameList *inProcessList,
                                        Fvid2_FrameList *outProcessList,
                                        uint32_t timeout);


static void Vhwa_mscUpdateYuv422iCslPrms(Vhwa_M2mMscHandleObj *hObj,
                                         CSL_MscConfig *cslMscCfg);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
Vhwa_M2mMscHandleObj   gM2mMscHandleObj[VHWA_M2M_MSC_MAX_INST]
                                       [VHWA_M2M_MSC_MAX_HANDLES];
Vhwa_M2mMscInstObj     gM2mMscInstObj[VHWA_M2M_MSC_MAX_INST];
Vhwa_M2mMscCommonObj   gM2mMscCommonObj;

Fvid2_DrvOps gM2mMscFvid2DrvOps = {
    FVID2_VHWA_M2M_MSC_DRV_ID,
    /**< Unique driver Id. */
    Vhwa_m2mMscCreate,
    /**< FVID2 create function pointer. */
    Vhwa_m2mMscDelete,
    /**< FVID2 delete function pointer. */
    Vhwa_m2mMscControl,
    /**< FVID2 control function pointer. */
    NULL, NULL,
    /**< FVID2 queue function pointer. */
    Vhwa_m2mMscProcessReq,
    /**< FVID2 process request function pointer. */
    Vhwa_m2mMscGetProcessReq,
    /**< FVID2 get processed request function pointer. */
};

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int32_t Vhwa_m2mMscInit(const Vhwa_M2mMscInitParams *pInitPrms)
{
    int32_t retVal = FVID2_SOK;

    /* Check for Null */
    if(NULL == pInitPrms)
    {
        GT_0trace(VhwaMscTrace, GT_ERR, "Bad Arguments\n");
        retVal = FVID2_EBADARGS;
    }

    if(FVID2_SOK == retVal)
    {
        /* Initialize all the global variables to 0 */
        Fvid2Utils_memset(&gM2mMscCommonObj, 0U, sizeof (Vhwa_M2mMscCommonObj));
        Fvid2Utils_memset(gM2mMscInstObj, 0U, sizeof(gM2mMscInstObj));
        Fvid2Utils_memset(gM2mMscHandleObj, 0U, sizeof (gM2mMscHandleObj));

        /* Initialize the internal objects */
        retVal = Vhwa_mscInit(pInitPrms);
    }
    if(retVal == FVID2_SOK)
    {
        retVal = Fvid2_registerDriver(&gM2mMscFvid2DrvOps);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VhwaMscTrace, GT_ERR,
                      "Registering to FVID2 driver manager failed\r\n");

            /* Uninitialize the internal objects */
            (void)Vhwa_mscDeInit();
        }
    }

    return (retVal);
}

int32_t Vhwa_m2mMscDeInit(void)
{
    int32_t retVal = FVID2_SOK;

    /* Unregister the Driver manager */
    retVal = Fvid2_unRegisterDriver(&gM2mMscFvid2DrvOps);

    if (FVID2_SOK != retVal)
    {
        GT_0trace(VhwaMscTrace, GT_ERR,
                  "Unregistering from FVID2 driver manager failed!!\r\n");
    }

    retVal += Vhwa_mscDeInit();

    return retVal;
}

static Fdrv_Handle Vhwa_m2mMscCreate(uint32_t drvId, uint32_t vpacDrvInstId,
    Ptr createPrms, Ptr createStatusArgs, const Fvid2_DrvCbParams *cbPrms)
{
    int32_t                 retVal = FVID2_SOK;
    Vhwa_M2mMscInstObj     *instObj = NULL;
    Vhwa_M2mMscHandleObj   *hObj = NULL;
    Vhwa_M2mMscCreatePrms  *mscCreatePrms = NULL;
    Fdrv_Handle             handle;
    uint32_t                drvInstId = vpacDrvInstId;

    /* Check for params and Null pointer */
    if ((NULL == createPrms) || (NULL == cbPrms))
    {
        GT_0trace(VhwaMscTrace, GT_ERR, "Bad Arguments\n");
        retVal = FVID2_EBADARGS;
    }
    else
    {
        retVal = Vhwa_m2mMscCheckCreatePrms(drvId, vpacDrvInstId);
    }

    if (FVID2_SOK == retVal)
    {
        /* Create is allowed only after init is done */
        if (UFALSE == gM2mMscCommonObj.initDone)
        {
            GT_0trace(VhwaMscTrace, GT_ERR,
                "Error: Driver Initialization Pending\n");
            retVal  = FVID2_EINVALID_PARAMS;
        }
    }
#if defined VHWA_M2M_VPAC_INSTANCE
#if (VHWA_M2M_VPAC_INSTANCE == 1)
    if (FVID2_SOK == retVal)
    {
        /* Translate instance id to be compatible with existing driver */
        if (VHWA_M2M_VPAC_1_MSC_DRV_INST_ID_0 == vpacDrvInstId)
        {
            drvInstId = VPAC_MSC_INST_ID_0;
        }
        else
        {
            drvInstId = VPAC_MSC_INST_ID_1;
        }
    }
#endif
#endif

    if(FVID2_SOK == retVal)
    {
        instObj = &gM2mMscInstObj[drvInstId];
    }

    if (NULL != instObj)
    {
        /* Lock instance semaphore */
        (void)SemaphoreP_pend(instObj->lockSem, SemaphoreP_WAIT_FOREVER);
    }

    if (FVID2_SOK == retVal)
    {
        mscCreatePrms = (Vhwa_M2mMscCreatePrms *)createPrms;

        if (instObj->openCnt >= VHWA_M2M_MSC_MAX_HANDLES)
        {
            GT_1trace(VhwaMscTrace, GT_ERR,
                      "Only %d handles supported per instance!\r\n",
                      VHWA_M2M_MSC_MAX_HANDLES);
            retVal = FVID2_EALLOC;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Allocate Handle Object */
        hObj = Vhwa_mscAllocHdlObj(instObj, drvInstId);

        if (NULL == hObj)
        {
            retVal = FVID2_EALLOC;
        }
        else
        {
            if (0U == instObj->openCnt)
            {
                /* Allocate SL2 Memory if not already allocated */
                if ((uint32_t)UFALSE == gM2mMscCommonObj.isSl2AllocDone)
                {
                    /* Initialize SL2 parameters with the defaul values */
                    Vhwa_M2mMscSl2AllocPrmsInit(&gM2mMscCommonObj.sl2AllocPrms);

                    /* Allocate SL2 Parameters */
                    retVal = Vhwa_m2mMscAllocSl2(&gM2mMscCommonObj.sl2AllocPrms);
                }

                /* On the first open, start all UTC channels.
                 * Even if all UTC channels are enabled, only channels enabled
                 * in HTS will be used for the transfer */

                if (FVID2_SOK == retVal)
                {
                    /* Start UDMA Channels on the first handle Create */
                    retVal = Vhwa_m2mMscStartCh(instObj, &gM2mMscCommonObj);
                }
                if (FVID2_SOK == retVal)
                {
                    Vhwa_enableHtsIntr(gM2mMscCommonObj.socInfo.vpacIntdRegs,
                        gM2mMscCommonObj.mscInitPrms.irqInfo[drvInstId].vhwaIrqNum,
                        instObj->pipeline);
                }
            }
        }
    }
    if (FVID2_SOK == retVal)
    {
        /* Allocate UDMA memory for this handle */
        retVal = Vhwa_m2mMscAllocUdmaMem(hObj);
    }
    if (FVID2_SOK == retVal)
    {
        retVal = Fvid2Utils_constructQ(&hObj->outLlObj);
        GT_assert(VhwaMscTrace, (FVID2_SOK == retVal));

        hObj->outQ = &hObj->outLlObj;
    }
    if (FVID2_SOK == retVal)
    {
        Fvid2Utils_memcpy(&hObj->createPrms, mscCreatePrms,
            sizeof(Vhwa_M2mMscCreatePrms));

        /* Copy Callback Parameters */
        Fvid2Utils_memcpy(&hObj->fdmCbPrms, cbPrms, sizeof (hObj->fdmCbPrms));
        Vhwa_mscInitHandleObj(instObj, hObj);

        instObj->openCnt++;

        /* Setting it to UTRUE so that next request will update in HW. */
        hObj->isCfgUpdated = 1u;

        handle = (Fdrv_Handle)hObj;
    }
    else
    {
        if(NULL != hObj)
        {
            /* Free up allocated resources in case of error */
            Vhwa_mscFreeHandleObj(hObj);
        }
        handle = NULL;
    }

    if (NULL != instObj)
    {
        /* Release instance semaphore */
        (void)SemaphoreP_post(instObj->lockSem);
    }

    return (handle);
}

static int32_t Vhwa_m2mMscDelete(Fdrv_Handle handle, Ptr deleteArgs)
{
    int32_t                 retVal = FVID2_SOK;
    Vhwa_M2mMscInstObj     *instObj = NULL;
    Vhwa_M2mMscHandleObj   *hObj = NULL;

    if (NULL == handle)
    {
        GT_0trace(VhwaMscTrace, GT_ERR, "NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }
    else
    {
        hObj = (Vhwa_M2mMscHandleObj *)handle;
        instObj = &gM2mMscInstObj[hObj->instId];
    }
    if (NULL != instObj)
    {
        /* Take the instance semaphore */
        (void)SemaphoreP_pend(instObj->lockSem, SemaphoreP_WAIT_FOREVER);

        /* Check if all handles are already closed */
        if (0U == instObj->openCnt)
        {
            retVal = FVID2_EFAIL;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Check if still some request are pending with driver */
        if (0U != hObj->numPendReq)
        {
            GT_1trace(
                VhwaMscTrace, GT_ERR,
                "Still %d request pending. Dequeue all before closing!!\r\n",
                hObj->numPendReq);

            retVal = FVID2_EAGAIN;
        }
        else
        {
            /* Delete the free Q */
            Fvid2Utils_destructQ(hObj->outQ);
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Free allocated handle object */
        Vhwa_mscFreeHandleObj(hObj);

        /* Decrement number of handle open count */
        instObj->openCnt--;

        if (0U == instObj->openCnt)
        {
            /* Disable HTS Interrupt */
            Vhwa_disableHtsIntr(gM2mMscCommonObj.socInfo.vpacIntdRegs,
                gM2mMscCommonObj.mscInitPrms.irqInfo[hObj->instId].vhwaIrqNum,
                instObj->pipeline);

            instObj->lastHndlObj = NULL;
        }
    }

    if (NULL != instObj)
    {
        /* release the instance semaphore */
        (void)SemaphoreP_post(instObj->lockSem);
    }

    return (retVal);
}

static int32_t Vhwa_m2mMscControl(Fdrv_Handle handle, uint32_t cmd, Ptr cmdArgs,
    Ptr cmdStatusArgs)
{
    int32_t                 retVal = FVID2_SOK;
    uint32_t                cnt;
    Vhwa_M2mMscInstObj     *instObj = NULL;
    Vhwa_M2mMscHandleObj   *hObj = NULL;
    Msc_Coeff              *pCoeffCfg = NULL;
    Vhwa_M2mMscParams      *mscPrms = NULL;
    Msc_ErrEventParams     *eePrms = NULL;
    Vhwa_M2mMscCommonObj   *comObj = NULL;
    Vhwa_HtsLimiter        *htsLimit = NULL;

    if (NULL == handle)
    {
        GT_0trace(VhwaMscTrace, GT_ERR, "Handle NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }
    else
    {
        hObj = (Vhwa_M2mMscHandleObj *)handle;
        instObj = &gM2mMscInstObj[hObj->instId];
        comObj = &gM2mMscCommonObj;
    }
    if (FVID2_SOK == retVal)
    {
        /* Take the instance semaphore */
        (void)SemaphoreP_pend(instObj->lockSem, SemaphoreP_WAIT_FOREVER);

        switch (cmd)
        {
            /* Set Filter Coefficients */
            case VHWA_M2M_IOCTL_MSC_SET_COEFF:
            {
                if (NULL != cmdArgs)
                {
                    pCoeffCfg = (Msc_Coeff *)cmdArgs;
                    retVal = Vhwa_mscSetFilterCoeff(comObj, pCoeffCfg);
                }
                break;
            }

            /* Set MSC Parameters */
            case VHWA_M2M_IOCTL_MSC_SET_PARAMS:
            {
                if (NULL != cmdArgs)
                {
                    mscPrms = (Vhwa_M2mMscParams *)cmdArgs;
                    retVal = Vhwa_mscSetParams(instObj, hObj, comObj, mscPrms);
                }
                break;
            }

            /* Set Bandwidth limiter */
            case VHWA_M2M_IOCTL_MSC_SET_BW_LIMITER:
            {
                if (NULL != cmdArgs)
                {
                    htsLimit = (Vhwa_HtsLimiter *)cmdArgs;
                    Vhwa_mscSetHtsLimit(hObj, htsLimit);
                }
                break;
            }

            case VHWA_M2M_IOCTL_MSC_GET_PERF:
            {
                Vhwa_M2mMscPerf *perf = NULL;

                if (NULL != cmdArgs)
                {
                    perf = (Vhwa_M2mMscPerf *)cmdArgs;
                    for (cnt = 0u; cnt < VHWA_M2M_MSC_MAX_COMP; cnt++)
                    {
                        perf->perf[cnt] = (uint32_t)hObj->perf[cnt];
                        perf->swOvhd[cnt] = hObj->swOvhd[cnt];
                    }
                }
                break;
            }

            /* Get Performance numbers */
            case VHWA_M2M_IOCTL_MSC_GET_PSA_SIGN:
            {
                Vhwa_M2mMscPsaSign *psa = NULL;

                if (NULL != cmdArgs)
                {
                    psa = (Vhwa_M2mMscPsaSign *)cmdArgs;
                    *psa = hObj->psa;
                }
                break;
            }

            /* Enable Register error events callback */
            case VHWA_M2M_IOCTL_MSC_REGISTER_ERR_CB:
            {
                if (NULL != cmdArgs)
                {
                    eePrms = (Msc_ErrEventParams *)cmdArgs;
                    retVal = vhwa_m2mMscSetEeParams(instObj, hObj, eePrms);
                }
                break;
            }

            case VHWA_M2M_IOCTL_MSC_SYNC_START:
            {
                if (NULL != hObj->createPrms.getTimeStamp)
                {
                    hObj->perf[0u] = hObj->createPrms.getTimeStamp();
                }

                retVal = CSL_htsPipelineStart(comObj->socInfo.htsRegs, &hObj->htsCfg[0]);
                break;
            }

            case IOCTL_VHWA_MSC_GET_SL2_PARAMS:
            {
                Vhwa_M2mMscSl2Params *pSl2Prms = NULL;
                if (NULL != cmdArgs)
                {
                    pSl2Prms = (Vhwa_M2mMscSl2Params *)cmdArgs;

                    Fvid2Utils_memcpy(pSl2Prms, &comObj->sl2Prms,
                                sizeof(Vhwa_M2mMscSl2Params));


                }
                break;
            }

            case IOCTL_VHWA_MSC_FC_GET_PARAMS:
            {
                Vhwa_M2mMscFcGetPrms *pfcPrms = NULL;
                if (NULL != cmdArgs)
                {
                    pfcPrms = (Vhwa_M2mMscFcGetPrms *)cmdArgs;
                    Vhwa_m2mMscFcGetPrms(hObj, pfcPrms);


                }
                break;
            }

            case IOCTL_VHWA_MSC_FC_CONN_PARAMS:
            {
                Vhwa_M2mMscFcConPrms *pFcConnPrms = NULL;
                if (NULL != cmdArgs)
                {
                    pFcConnPrms = (Vhwa_M2mMscFcConPrms *)cmdArgs;

                    Vhwa_m2mMscUpdateFcConnPrms(hObj, pFcConnPrms);


                }
                break;
            }

            case IOCTL_VHWA_MSC_UPDATE_FC_PARAMS:
            {
                Vhwa_M2mMscFcUpdatePrms *pFcPrms = NULL;
                if (NULL != cmdArgs)
                {
                    pFcPrms = (Vhwa_M2mMscFcUpdatePrms *)cmdArgs;

                    Vhwa_m2mVissUpdateFcPrms(hObj, pFcPrms);


                }
                break;
            }

            /* Default Case */
            default:
            {
                retVal = FVID2_EUNSUPPORTED_CMD;
                break;
            }
        }

        /* Release instance semaphore */
        (void)SemaphoreP_post(instObj->lockSem);
    }

    return (retVal);
}

static int32_t Vhwa_m2mMscProcessReq(Fdrv_Handle handle,
    Fvid2_FrameList *inFrmList, Fvid2_FrameList *outFrmList, uint32_t timeout)
{
    int32_t                 retVal = FVID2_SOK;
    uint32_t                cnt, chCnt;
    uint32_t                semTimeout;
    Vhwa_M2mMscInstObj     *instObj = NULL;
    Vhwa_M2mMscHandleObj   *hObj = NULL;
    Vhwa_M2mMscCommonObj   *comObj = NULL;
    Vhwa_M2mMscQueueObj    *qObj = NULL;
    Vhwa_M2mMscParams      *mscPrms = NULL;
    Fvid2_Frame            *frm = NULL;
    SemaphoreP_Status       semStatus;

    if (NULL == handle)
    {
        retVal = FVID2_EBADARGS;
    }
    else
    {
        hObj = (Vhwa_M2mMscHandleObj *)handle;
        instObj = &gM2mMscInstObj[hObj->instId];
        comObj = &gM2mMscCommonObj;

        mscPrms = &hObj->mscPrms;
    }

    if ((FVID2_SOK == retVal) && (UFALSE == hObj->fcStatus.isFlexConnect))
    {
        /* Input Buffer Addresses cannot be null */
        if ((NULL == inFrmList->frames[0U]) ||
            (0U == inFrmList->frames[0u]->addr[0U]) ||
            (0U == inFrmList->numFrames) )
        {
            retVal = FVID2_EBADARGS;
        }
        else
        {
            /* For YUV420 input, chroma buffer cannot be null */
            if ((FVID2_DF_YUV420SP_UV == mscPrms->inFmt.dataFormat) ||
                (FVID2_DF_YUV420SP_VU == mscPrms->inFmt.dataFormat) ||
                (FVID2_DF_YUV422SP_UV == mscPrms->inFmt.dataFormat) ||
                (FVID2_DF_YUV422SP_VU == mscPrms->inFmt.dataFormat))
            {
                if (0U == inFrmList->frames[0u]->addr[1U])
                {
                    retVal = FVID2_EBADARGS;
                }
            }
        }

        for (chCnt = 0u; chCnt < MSC_MAX_OUTPUT; chCnt ++)
        {
            if (UTRUE == hObj->outChPrms[chCnt].buffEnable)
            {
                frm = outFrmList->frames[chCnt];
                if (NULL == frm)
                {
                    retVal = FVID2_EBADARGS;
                }
                else
                {
                    for (cnt = 0U; cnt < hObj->numIter; cnt ++)
                    {
                        if (0U == frm->addr[cnt])
                        {
                            retVal = FVID2_EBADARGS;
                            break;
                        }
                    }
                }
            }
            if (FVID2_SOK != retVal)
            {
                break;
            }
        }
    }

    if (FVID2_SOK == retVal)
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

        /* Take the instance semaphore, so that no other
         * handle can submit request from the task context. */
        semStatus = SemaphoreP_pend(instObj->hwaLock, semTimeout);

        if (SemaphoreP_OK == semStatus)
        {
            /* This request is going to be submitted, so increment
             * number of pending request counter,
             * */
            hObj->numPendReq = (hObj->numPendReq + 1U);

            retVal = FVID2_SOK;
        }
        else /* Any other retVal */
        {
            /* Convert Semaphore retVal to FVID2 status */
            if (SemaphoreP_FAILURE == semStatus)
            {
                retVal = FVID2_EFAIL;
            }
            else if (SemaphoreP_TIMEOUT == semStatus)
            {
                retVal = FVID2_EAGAIN;
            }
            else /* For all other cases, it is failed */
            {
                retVal = FVID2_EFAIL;
            }
        }

        for (cnt = 0U; (cnt < MSC_MAX_OUTPUT) && (SemaphoreP_OK == semStatus);
                cnt ++)
        {
            if (0U != (hObj->scalarUsed & ((uint32_t)1U << cnt)))
            {
                semStatus = SemaphoreP_pend(comObj->scLockSem[cnt],
                                            SemaphoreP_WAIT_FOREVER);
            }
        }

        if (FVID2_SOK == retVal)
        {
            /* Get a queue object from the free queue,
             * No need to protect from ISR as it is not accessed from ISR */
            qObj = (Vhwa_M2mMscQueueObj *) Fvid2Utils_dequeue(instObj->freeQ);

            if (NULL == qObj)
            {
                GT_0trace(VhwaMscTrace, GT_ERR,
                    "Failed to Free Queue Object!!\n");
                retVal = FVID2_EALLOC;
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

        if (FVID2_SOK == retVal)
        {
            Vhwa_m2mMscSetAddress(qObj->hObj, &qObj->inFrmList,
                                    &qObj->outFrmList);

            /* HW is free, submit request to the hardware */
            /* If previous handle and current handles are same, */
            if ((instObj->lastHndlObj == qObj->hObj) &&
                (0u == hObj->isCfgUpdated))
            {
                /* Update MSC Height Dependent config */
                retVal = Vhwa_m2mMscSetFrameSize(qObj, comObj, 0U);

                if (FVID2_SOK == retVal)
                {
                    /** Set the addresses, Submit the TR
                     *  Start the pipeline */
                    retVal = Vhwa_m2mMscSubmitRequest(instObj, qObj,
                                                comObj, 0U);
                }
            }
            else
            {
                /** Last handle was not same as new handle,
                 *  so require to recofigure all HW IPs */
                retVal = Vhwa_m2mMscSetConfigInHW(qObj, comObj, 0U);

                if (FVID2_SOK == retVal)
                {
                    /* Cfg is successfully set in the HW, so
                     * resetting this flag */
                    hObj->isCfgUpdated = 0u;

                    /** Last handle was not same as new handle,
                     *  so require to recofigure all HW IPs */
                    retVal = Vhwa_m2mMscSubmitRequest(instObj,
                        qObj, comObj, 0U);
                }
            }
        }
    }

    return (retVal);
}

/** \brief Typedef for FVID2 get processed frames function pointer. */
static int32_t Vhwa_m2mMscGetProcessReq(Fdrv_Handle handle,
    Fvid2_FrameList *inFrmList, Fvid2_FrameList *outFrmList,
    uint32_t timeout)
{
    int32_t                 retVal = FVID2_SOK;
    uint32_t                cnt, cookie;
    Vhwa_M2mMscInstObj     *instObj = NULL;
    Vhwa_M2mMscHandleObj   *hObj = NULL;
    Vhwa_M2mMscQueueObj    *qObj = NULL;

    if ((NULL == handle) || (NULL == inFrmList) || (NULL == outFrmList))
    {
        GT_0trace(VhwaMscTrace, GT_ERR, "NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }
    else
    {
        hObj = (Vhwa_M2mMscHandleObj *) handle;
        instObj = &gM2mMscInstObj[hObj->instId];
    }

    if (FVID2_SOK == retVal)
    {
        /* Disable interrupts before accessing queue */
        cookie = HwiP_disable();

        /* Dequeue the completed request from done queue */
        qObj = (Vhwa_M2mMscQueueObj *) Fvid2Utils_dequeue(hObj->outQ);
        if (NULL == qObj)
        {
            /* Restore interrupts after updating node list */
            HwiP_restore(cookie);

            /* Check if requests are pending with driver */
            if (0U == hObj->numPendReq)
            {
                /* Nothing is queued */
                GT_0trace(
                    VhwaMscTrace, GT_DEBUG,
                    "Nothing to dequeue. No request pending with driver!!\r\n");
                retVal = FVID2_ENO_MORE_BUFFERS;
            }
            else
            {
                /* If no request have completed, return try again */
                GT_0trace(VhwaMscTrace, GT_DEBUG,
                          "Nothing to dequeue. Try again!!\r\n");
                retVal = FVID2_EAGAIN;
            }
        }
        else
        {
            /* Copy the driver's process list to application's process list */
            Fvid2_copyFrameList(inFrmList, &qObj->inFrmList);
            Fvid2_copyFrameList(outFrmList, &qObj->outFrmList);

            /* Return back the queue object to the free queue */
            Fvid2Utils_queue(instObj->freeQ, &qObj->qElem, qObj);

            /* Restore interrupts after updating node list */
            HwiP_restore(cookie);

            for (cnt = 0; cnt < hObj->numIter; cnt++)
            {
                /* Dequeue from Rings */
                retVal = Vhwa_m2mMscPopRings(hObj, &gM2mMscCommonObj, cnt);
            }

            /* Decrement the pending request count. */
            if (0U != hObj->numPendReq)
            {
                hObj->numPendReq = hObj->numPendReq - 1U;
            }
        }
    }

    return (retVal);
}


int32_t Vhwa_m2mMscAllocSl2(const Vhwa_M2mMscSl2AllocPrms *sl2AllocPrms)
{
    int32_t                 retVal = FVID2_SOK;
    uint32_t                sl2MemReq = 0;
    uint32_t                cnt, cnt1, channelCnt;
    uint32_t                inPitchSl2[VHWA_M2M_MSC_MAX_INST][VHWA_M2M_MSC_MAX_IN_CHANNEL];
    uint32_t                outPitchSl2[MSC_MAX_OUTPUT];
    uint64_t                sl2Addr;
    Vhwa_M2mMscCommonObj   *comObj = NULL;

    comObj = &gM2mMscCommonObj;

    channelCnt = VHWA_M2M_MSC_MAX_IN_CHANNEL;

    if (NULL == sl2AllocPrms)
    {
        GT_0trace(VhwaMscTrace, GT_ERR, "Bad Arguments\n");
        retVal = FVID2_EBADARGS;
    }
    else
    {
        if ((uint32_t)UFALSE == comObj->initDone)
        {
            GT_0trace(VhwaMscTrace, GT_ERR,
                "Driver init is not done!!\n");
            retVal = FVID2_EFAIL;
        }

        if ((uint32_t)UTRUE == comObj->isSl2AllocDone)
        {
            GT_0trace(VhwaMscTrace, GT_ERR,
                "SL2 Memory is already allocated !!\n");
            retVal = FVID2_EFAIL;
        }
    }
    for (cnt = 0; (cnt < VHWA_M2M_MSC_MAX_INST) && (FVID2_SOK == retVal);
            cnt++)
    {
        for (cnt1 = 0; (cnt1 < channelCnt) && (FVID2_SOK == retVal);
            cnt1++)
        {
            if (sl2AllocPrms->inBuffDepth[cnt][cnt1] < MSC_M2M_MIN_IN_SL2_LINES)
            {
                GT_0trace(VhwaMscTrace, GT_ERR, "Bad Arguments\n");
                retVal = FVID2_EBADARGS;
            }
        }
    }
    for (cnt = 0; (cnt < MSC_MAX_OUTPUT) && (FVID2_SOK == retVal); cnt++)
    {
        if (sl2AllocPrms->outBuffDepth[cnt] < MSC_M2M_MIN_OUT_SL2_LINES)
        {
            GT_0trace(VhwaMscTrace, GT_ERR, "Bad Arguments\n");
            retVal = FVID2_EBADARGS;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Calculate Sl2 buffer size required for input */
        for (cnt = 0; cnt < VHWA_M2M_MSC_MAX_INST; cnt++)
        {
            for (cnt1 = 0; (cnt1 < channelCnt) && (FVID2_SOK == retVal);
                cnt1++)
            {
                inPitchSl2[cnt][cnt1] = Vhwa_calcHorzSizeInBytes(
                                            sl2AllocPrms->maxInWidth[cnt][cnt1],
                                            sl2AllocPrms->inCcsf[cnt][cnt1]);
                inPitchSl2[cnt][cnt1] = Vhwa_calcSl2Pitch(inPitchSl2[cnt][cnt1]);

                sl2MemReq += inPitchSl2[cnt][cnt1] *
                                sl2AllocPrms->inBuffDepth[cnt][cnt1];
            }
        }

        /* Calculate Sl2 buffer size required for output */
        for (cnt = 0; cnt < MSC_MAX_OUTPUT; cnt++)
        {
            outPitchSl2[cnt] = Vhwa_calcHorzSizeInBytes(
                sl2AllocPrms->maxOutWidth[cnt], sl2AllocPrms->outCcsf[cnt]);
            outPitchSl2[cnt] = Vhwa_calcSl2Pitch(outPitchSl2[cnt]);

            sl2MemReq += outPitchSl2[cnt] * sl2AllocPrms->outBuffDepth[cnt];
        }

        /* Allocate Sl2 Memory */
        sl2Addr = Vhwa_allocateSl2(sl2MemReq,
                                    VHWA_SL2_INST_VPAC);
        if (0U == sl2Addr)
        {
            retVal = FVID2_EALLOC;
        }
        else
        {
            comObj->sl2Prms.sl2StartAddr = sl2Addr;
            comObj->sl2Prms.sl2MemSize = sl2MemReq;
            comObj->isSl2AllocDone = (uint32_t)UTRUE;
            Fvid2Utils_memcpy(&comObj->sl2AllocPrms, sl2AllocPrms,
                sizeof(Vhwa_M2mMscSl2AllocPrms));
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Calculate Sl2 buffer size required for input */
        for (cnt = 0; cnt < VHWA_M2M_MSC_MAX_INST; cnt++)
        {
            for (cnt1 = 0; (cnt1 < channelCnt) && (FVID2_SOK == retVal);
            cnt1++)
            {
                comObj->sl2Prms.inSl2Addr[cnt][cnt1] = sl2Addr;

                comObj->sl2Prms.inSl2BuffDepth[cnt][cnt1] =
                                            sl2AllocPrms->inBuffDepth[cnt][cnt1];

                sl2Addr += (uint64_t)inPitchSl2[cnt][cnt1] *
                               (uint64_t)sl2AllocPrms->inBuffDepth[cnt][cnt1];
            }
        }

        /* Calculate Sl2 buffer size required for output */
        for (cnt = 0; cnt < MSC_MAX_OUTPUT; cnt++)
        {
            comObj->sl2Prms.outSl2Addr[cnt] = sl2Addr;
            comObj->sl2Prms.outSl2BuffDepth[cnt] =
                                            sl2AllocPrms->outBuffDepth[cnt];

            sl2Addr += (uint64_t)outPitchSl2[cnt] *
                       (uint64_t)sl2AllocPrms->outBuffDepth[cnt];
        }
    }

    return retVal;
}


int32_t Vhwa_m2mMscFreeSl2(void)
{
    int32_t               retVal = FVID2_SOK;
    Vhwa_M2mMscCommonObj *comObj;

    comObj = &gM2mMscCommonObj;

    Vhwa_FreeSl2(comObj->sl2Prms.sl2StartAddr, VHWA_SL2_INST_VPAC);

    comObj->isSl2AllocDone = (uint32_t)UFALSE;

    return retVal;
}

/**
 * \brief   Returns handle object for the requested handle count.
 *
 * \param   cnt              count.
 *          instance         instance
 *
 * \return  reference to the handle object.
 *
 **/
Vhwa_M2mMscHandleObj *Vhwa_m2mMscGetHandleObj(uint32_t instance, uint32_t cnt)
{
    return &gM2mMscHandleObj[instance][cnt];
}
/* ========================================================================== */
/*                           Local Functions                                  */
/* ========================================================================== */
static int32_t Vhwa_mscInit(const Vhwa_M2mMscInitParams *pInitPrms)
{
    int32_t             retVal = FVID2_SOK;
    uint32_t            cnt;
    SemaphoreP_Params   semParams[VHWA_M2M_MSC_MAX_INST];
    SemaphoreP_Params   hwaSemParams[VHWA_M2M_MSC_MAX_INST];
    SemaphoreP_Params   scSemParams[MSC_MAX_OUTPUT];
    Vhwa_M2mMscInstObj *instObj = NULL;
    Msc_SocInfo        *socInfo = NULL;

    /* Check for Null pointer */
    GT_assert(VhwaMscTrace, (NULL != pInitPrms));

    for (cnt = 0; (cnt < VHWA_M2M_MSC_MAX_INST) && (retVal == FVID2_SOK); cnt++)
    {
        /* Initialize the semaphore parameters and create semaphore pool */
        SemaphoreP_Params_init(&semParams[cnt]);
        semParams[cnt].mode = SemaphoreP_Mode_BINARY;
        gM2mMscInstObj[cnt].lockSem = SemaphoreP_create(1U, &semParams[cnt]);
        if (NULL == gM2mMscInstObj[cnt].lockSem)
        {
            retVal = FVID2_EALLOC;
        }
        /* Initialize the semaphore parameters and create semaphore pool */
        SemaphoreP_Params_init(&hwaSemParams[cnt]);
        hwaSemParams[cnt].mode = SemaphoreP_Mode_BINARY;
        gM2mMscInstObj[cnt].hwaLock = SemaphoreP_create(1U, &hwaSemParams[cnt]);
        if (NULL == gM2mMscInstObj[cnt].hwaLock)
        {
            retVal = FVID2_EALLOC;
        }
    }

    for (cnt = 0; (cnt < MSC_MAX_OUTPUT) && (retVal == FVID2_SOK); cnt++)
    {
        /* Initialize the semaphore parameters and create semaphore pool */
        SemaphoreP_Params_init(&scSemParams[cnt]);
        scSemParams[cnt].mode = SemaphoreP_Mode_BINARY;
        gM2mMscCommonObj.scLockSem[cnt] = SemaphoreP_create(
            1U, &scSemParams[cnt]);
        if (NULL == gM2mMscCommonObj.scLockSem[cnt])
        {
            retVal = FVID2_EALLOC;
        }
    }

    if(FVID2_SOK == retVal)
    {
        /* Copy Init Parameters */
        Fvid2Utils_memcpy(&gM2mMscCommonObj.mscInitPrms, pInitPrms,
                            sizeof(Vhwa_M2mMscInitParams));

        gM2mMscCommonObj.initDone = (uint32_t)UTRUE;
        gM2mMscCommonObj.isSl2AllocDone = (uint32_t)UFALSE;

        socInfo = &gM2mMscCommonObj.socInfo;

        Msc_getSocInfo(socInfo);

        /* Initialize UDMA */
        retVal  = Vhwa_m2mMscUdmaChInit(&gM2mMscCommonObj);
    }

    for (cnt = 0; (cnt < VHWA_M2M_MSC_MAX_INST) && (FVID2_SOK == retVal); cnt++)
    {
        instObj = &gM2mMscInstObj[cnt];

        retVal = Vhwa_mscCreateQueue(instObj);
    }

    if(FVID2_SOK == retVal)
    {
        /* Enable Module and interrupt */
        CSL_vpacEnableModule(socInfo->vpacCntlRegs, VPAC_MODULE_MSC,
            (uint32_t)UTRUE);

        /* Enable the MSC Module */
        CSL_mscEnable(socInfo->mscRegs);

        for (cnt = 0; (cnt < VHWA_M2M_MSC_MAX_INST) && (retVal == FVID2_SOK);
                        cnt++)
        {
            instObj = &gM2mMscInstObj[cnt];

            /* Register interrupt */
            if (cnt == VPAC_MSC_INST_ID_0)
            {
                /* Set HTS pipeline */
                instObj->pipeline = VHWA_M2M_MSC0_HTS_PIPELINE;

                retVal = Vhwa_m2mMscRegisterIsr(
                    instObj, &gM2mMscCommonObj, cnt);
            }
            else
            {
                /* Set HTS pipeline */
                instObj->pipeline = VHWA_M2M_MSC1_HTS_PIPELINE;

                retVal = Vhwa_m2mMscRegisterIsr(
                    instObj, &gM2mMscCommonObj, cnt);
            }
        }
    }

    return (retVal);
}

static int32_t Vhwa_mscDeInit(void)
{
    int32_t             retVal;
    uint32_t            cnt;
    Vhwa_M2mMscInstObj *instObj = NULL;

    /* Stop UDMA channels */
    retVal = Vhwa_m2mMscStopCh(&gM2mMscCommonObj);

    for (cnt = 0; (cnt < VHWA_M2M_MSC_MAX_INST) && (FVID2_SOK == retVal); cnt++)
    {
        /* Check if any handle is open */
        GT_assert(VhwaMscTrace, (0U != gM2mMscInstObj[cnt].openCnt));

        instObj = &gM2mMscInstObj[cnt];

        Vhwa_m2mMscUnregisterIsr(instObj, &gM2mMscCommonObj, cnt);

        /* Delete Semaphore */
        (void)SemaphoreP_delete(gM2mMscInstObj[cnt].lockSem);
        gM2mMscInstObj[cnt].lockSem = NULL;

        (void)SemaphoreP_delete(gM2mMscInstObj[cnt].hwaLock);
        gM2mMscInstObj[cnt].hwaLock = NULL;

        /* Free up allocated resources */
        (void)Vhwa_mscDeleteQueue(instObj);
    }

    for (cnt = 0; cnt < MSC_MAX_OUTPUT; cnt++)
    {
        /* Delete Semaphore */
        (void)SemaphoreP_delete(gM2mMscCommonObj.scLockSem[cnt]);
    }

    if ((uint32_t)UTRUE == gM2mMscCommonObj.isSl2AllocDone)
    {
        (void)Vhwa_m2mMscFreeSl2();
    }

    /* Deinitialize the UDMA channels */
    retVal += Vhwa_m2mMscUdmaChDeInit(&gM2mMscCommonObj);


    /* Disable Module and interrupt */
    CSL_vpacEnableModule(gM2mMscCommonObj.socInfo.vpacCntlRegs,
        VPAC_MODULE_MSC, (uint32_t)UFALSE);

    gM2mMscCommonObj.initDone = (uint32_t)UFALSE;

    return (retVal);
}

static Vhwa_M2mMscHandleObj *Vhwa_mscAllocHdlObj(Vhwa_M2mMscInstObj *instObj,
                                                uint32_t instId)
{
    uint32_t              cnt;
    Vhwa_M2mMscHandleObj *hObj = NULL;

    /* Check for Null pointer */
    GT_assert(VhwaMscTrace, (NULL != instObj));

    for (cnt = 0U; cnt < VHWA_M2M_MSC_MAX_HANDLES; cnt ++)
    {
        if ((uint32_t)UFALSE == gM2mMscHandleObj[instId][cnt].isUsed)
        {
            /* Allocate Handle Object */
            hObj = &gM2mMscHandleObj[instId][cnt];
            Fvid2Utils_memset(hObj, 0x0, sizeof(Vhwa_M2mMscHandleObj));
            gM2mMscHandleObj[instId][cnt].isUsed = (uint32_t)UTRUE;
            hObj->idx = cnt;
            hObj->instId = instId;
            break;
        }
    }

    return (hObj);
}

static int32_t Vhwa_mscCreateQueue(Vhwa_M2mMscInstObj *instObj)
{
    int32_t              retVal;
    uint32_t             qCnt;
    Vhwa_M2mMscQueueObj *qObj;

    /* NULL pointer check */
    GT_assert(VhwaMscTrace, (NULL != instObj));

    instObj->activeQObj = NULL;
    instObj->freeQ = NULL;

    /* Create Free queue */
    retVal = Fvid2Utils_constructQ(&instObj->freeLlObj);
    GT_assert(VhwaMscTrace, (retVal == FVID2_SOK));

    instObj->freeQ = &instObj->freeLlObj;

    /* Initialize and queue the allocate queue object to free Q */
    for (qCnt = 0U; qCnt < VHWA_MSC_QUEUE_LEN_PER_INST; qCnt++)
    {
        qObj = &instObj->mscQObj[qCnt];
        qObj->hObj = NULL;
        Fvid2Utils_queue(instObj->freeQ, &qObj->qElem, qObj);
    }

    return retVal;
}

static int32_t Vhwa_mscDeleteQueue(Vhwa_M2mMscInstObj *instObj)
{
    int32_t              retVal = FVID2_SOK;
    Vhwa_M2mMscQueueObj *qObj;

    /* NULL pointer check */
    GT_assert(VhwaMscTrace, (NULL != instObj));

    if (NULL != instObj->freeQ)
    {
        /* Free-up all the queued free queue objects */
        do
        {
            qObj = (Vhwa_M2mMscQueueObj *) Fvid2Utils_dequeue(instObj->freeQ);
        } while (NULL != qObj);

        /* Delete the free Q */
        Fvid2Utils_destructQ(instObj->freeQ);
        instObj->freeQ = NULL;
    }

    return (retVal);
}

static void Vhwa_mscFreeHandleObj(Vhwa_M2mMscHandleObj *hObj)
{
    uint32_t cnt;

    /* Check for Null pointer */
    GT_assert(VhwaMscTrace, (NULL != hObj));

    for (cnt = 0U; cnt < VHWA_M2M_MSC_MAX_HANDLES; cnt ++)
    {
        /* Freeup allocated Handle Object */
        if (hObj == &gM2mMscHandleObj[hObj->instId][cnt])
        {
            hObj->isUsed = (uint32_t)UFALSE;;
            break;
        }
    }

}

static void Vhwa_mscInitHandleObj(const Vhwa_M2mMscInstObj *instObj,
    Vhwa_M2mMscHandleObj *hObj)
{
    /* Check for Null pointer */
    GT_assert(VhwaMscTrace, (NULL != hObj));
    GT_assert(VhwaMscTrace, (NULL != instObj));

    /* Initialize MSC with Default configuration */
    Vhwa_mscInitMscCfg(&hObj->mscPrms);
    /* Set HTS Configuration for default MSC configuration */
    Vhwa_mscSetHtsCfg(instObj, hObj, &hObj->mscPrms);
    /* Set LSE Configuration for default MSC configuration */
    Vhwa_mscSetLseCfg(hObj, &hObj->mscPrms);
}

static int32_t Vhwa_mscSetFilterCoeff(Vhwa_M2mMscCommonObj *comObj,
                                      const Msc_Coeff *pCoeffCfg)
{
    int32_t retVal;

    /* Check for Null pointer */
    GT_assert(VhwaMscTrace, (NULL != comObj));
    GT_assert(VhwaMscTrace, (NULL != pCoeffCfg));

    retVal = CSL_mscSetCoeff(comObj->socInfo.mscRegs, pCoeffCfg);

    return (retVal);
}

static int32_t Vhwa_mscSetParams(Vhwa_M2mMscInstObj *instObj,
    Vhwa_M2mMscHandleObj *hObj, const Vhwa_M2mMscCommonObj *comObj,
    Vhwa_M2mMscParams *mscPrms)
{
    int32_t retVal;
    uint32_t cnt;

    /* Check for Null pointer */
    GT_assert(VhwaMscTrace, (NULL != hObj));
    GT_assert(VhwaMscTrace, (NULL != instObj));
    GT_assert(VhwaMscTrace, (NULL != comObj));

    retVal = Vhwa_mscCheckMscCfg(hObj, mscPrms);

    if (FVID2_SOK == retVal)
    {
        Fvid2Utils_memcpy(&hObj->mscPrms, mscPrms, sizeof(Vhwa_M2mMscParams));

        /* Set and calculate the parameters */
        Vhwa_mscCalPrms(hObj, mscPrms);

        /* Initialize HTS configuration based on the MSC Config */
        Vhwa_mscSetHtsCfg(instObj, hObj, &hObj->mscPrms);
        /* Initialize LSE configuration based on the MSC Config */
        Vhwa_mscSetLseCfg(hObj, &hObj->mscPrms);

        for (cnt = 0u; (cnt < hObj->numIter) && (FVID2_SOK == retVal); cnt++)
        {
            /* Verify HTS Configuration */
            retVal = CSL_htsCheckThreadConfig(&hObj->htsCfg[cnt]);
        }
        for (cnt = 0u; (cnt < hObj->numIter) && (FVID2_SOK == retVal); cnt++)
        {
            /* Verify LSE Configuration */
            retVal = CSL_lseCheckConfig(&hObj->lseCfg[cnt]);
        }

        if (FVID2_SOK == retVal)
        {
            /* Setup TR Descriptor */
            Vhwa_mscM2mSetTrDesc(hObj, comObj);
        }
    }

    if(FVID2_SOK != retVal)
    {
        /* Initialize MSC with Default configuration */
        Vhwa_mscInitMscCfg(&hObj->mscPrms);
        /* Set HTS Configuration for default MSC configuration */
        Vhwa_mscSetHtsCfg(instObj, hObj, &hObj->mscPrms);
        /* Set LSE Configuration for default MSC configuration */
        Vhwa_mscSetLseCfg(hObj, &hObj->mscPrms);
    }
    else
    {
        /* Protecting it this variable using hwaLock, as this is
         * where this variable is being accessed. */
        (void)SemaphoreP_pend(instObj->hwaLock, SemaphoreP_WAIT_FOREVER);
        hObj->isCfgUpdated = 1u;
        (void)SemaphoreP_post(instObj->hwaLock);
    }

    return (retVal);
}

static int32_t vhwa_m2mMscSetEeParams(const Vhwa_M2mMscInstObj *instObj,
    Vhwa_M2mMscHandleObj *hObj, const Msc_ErrEventParams *eePrms)
{
    int32_t status = FVID2_EBADARGS;

    if ((NULL != instObj) && (NULL != hObj) && (NULL != eePrms))
    {
        Fvid2Utils_memcpy(&hObj->eePrms, eePrms, sizeof(Msc_ErrEventParams));
        status = FVID2_SOK;
    }

    return (status);
}

static void Vhwa_mscInitMscCfg(Vhwa_M2mMscParams *mscPrms)
{
    uint32_t      chCnt;
    Fvid2_Format *fmt = NULL;

    /* Check for Null pointer */
    GT_assert(VhwaMscTrace, (NULL != mscPrms));

    /* Initialize MSC Configuration with the detault configuration */
    fmt = &mscPrms->inFmt;

    fmt->width = VHWA_MSC_DEF_WIDTH;
    fmt->height = VHWA_MSC_DEF_HEIGHT;
    fmt->dataFormat = FVID2_DF_LUMA_ONLY;
    fmt->ccsFormat = FVID2_CCSF_BITS12_PACKED;
    fmt->pitch[0U] = (VHWA_MSC_DEF_WIDTH * 3U) / 2U;

    for (chCnt = 0u; chCnt < MSC_MAX_OUTPUT; chCnt ++)
    {
        Fvid2Utils_memcpy(&mscPrms->outFmt[chCnt], fmt, sizeof(Fvid2_Format));
    }

    mscPrms->loopBack = UFALSE;
    mscPrms->enableLineSkip = UFALSE;

    Msc_ConfigInit(&mscPrms->mscCfg);
}


static int32_t Vhwa_mscCheckMscCfg(Vhwa_M2mMscHandleObj *hObj,
                                   Vhwa_M2mMscParams *mscPrms)
{
    int32_t         retVal = FVID2_SOK;
    uint32_t        cnt, minPitch;
    Fvid2_Format   *inFmt = NULL;
    Fvid2_Format   *outFmt = NULL;
    Msc_ScConfig   *scCfg = NULL;

    /* Check for Null pointer */
    GT_assert(VhwaMscTrace, (NULL != hObj));
    GT_assert(VhwaMscTrace, (NULL != mscPrms));

    inFmt = &mscPrms->inFmt;

    /* Check for Valid data format */
    if(0U == gM2mMscCommonObj.socInfo.isDualChannel)
    {
       if ((FVID2_DF_LUMA_ONLY != inFmt->dataFormat) &&
            (FVID2_DF_CHROMA_ONLY != inFmt->dataFormat) &&
            (FVID2_DF_YUV420SP_UV != inFmt->dataFormat) &&
            (FVID2_DF_YUV420SP_VU != inFmt->dataFormat))
        {
            retVal = FVID2_EINVALID_PARAMS;
        }
    }
    else if (1U == gM2mMscCommonObj.socInfo.isDualChannel)
    {
        if ((FVID2_DF_LUMA_ONLY != inFmt->dataFormat) &&
            (FVID2_DF_CHROMA_ONLY != inFmt->dataFormat) &&
            (FVID2_DF_YUV420SP_UV != inFmt->dataFormat) &&
            (FVID2_DF_YUV420SP_VU != inFmt->dataFormat) &&
            (FVID2_DF_YUV422SP_UV != inFmt->dataFormat) &&
            (FVID2_DF_YUV422SP_VU != inFmt->dataFormat) &&
            (FVID2_DF_YUV422I_UYVY != inFmt->dataFormat) &&
            (FVID2_DF_YUV422I_YUYV != inFmt->dataFormat) &&
            (FVID2_DF_R_GBI != inFmt->dataFormat) &&
            (FVID2_DF_RGI_B != inFmt->dataFormat) &&
            (FVID2_DF_2PLANES != inFmt->dataFormat))
        {
            retVal = FVID2_EINVALID_PARAMS;
        }
    }
    else
    {
      retVal = FVID2_EINVALID_PARAMS;
    }
    if ((inFmt->width > MAX_SUPPORTED_WIDTH_HEIGHT) ||
        (inFmt->height > MAX_SUPPORTED_WIDTH_HEIGHT))
    {
        retVal = FVID2_EINVALID_PARAMS;
    }
    /* Check for Second channel pitch */
    minPitch = Vhwa_m2mMscCalcHorzSizeInBytes(inFmt->width, inFmt->ccsFormat,
                                              inFmt->dataFormat);
    if (inFmt->pitch[0] < minPitch)
    {
        retVal = FVID2_EINVALID_PARAMS;
    }
    else
    {
        hObj->inChPrms[0].sl2Pitch = Vhwa_calcSl2Pitch(minPitch);
    }

    for (cnt = 0U; cnt < MSC_MAX_OUTPUT; cnt++)
    {
        scCfg = &mscPrms->mscCfg.scCfg[cnt];
        outFmt = &mscPrms->outFmt[cnt];

        if (UTRUE == scCfg->enable)
        {
            /* Check for Valid data format */
            if(0U == gM2mMscCommonObj.socInfo.isDualChannel)
            {
                if ((FVID2_DF_LUMA_ONLY != outFmt->dataFormat) &&
                    (FVID2_DF_CHROMA_ONLY != outFmt->dataFormat) &&
                    (FVID2_DF_YUV420SP_UV != outFmt->dataFormat) &&
                    (FVID2_DF_YUV420SP_VU != outFmt->dataFormat))
                {
                    retVal = FVID2_EINVALID_PARAMS;
                }
            }
            else if (1U == gM2mMscCommonObj.socInfo.isDualChannel)
            {
                if ((FVID2_DF_LUMA_ONLY != outFmt->dataFormat) &&
                    (FVID2_DF_CHROMA_ONLY != outFmt->dataFormat) &&
                    (FVID2_DF_YUV420SP_UV != outFmt->dataFormat) &&
                    (FVID2_DF_YUV420SP_VU != outFmt->dataFormat) &&
                    (FVID2_DF_YUV422SP_UV != outFmt->dataFormat) &&
                    (FVID2_DF_YUV422SP_VU != outFmt->dataFormat) &&
                    (FVID2_DF_YUV422I_UYVY != outFmt->dataFormat) &&
                    (FVID2_DF_YUV422I_YUYV != outFmt->dataFormat) &&
                    (FVID2_DF_R_GBI != outFmt->dataFormat) &&
                    (FVID2_DF_RGI_B != outFmt->dataFormat) &&
                    (FVID2_DF_2PLANES != outFmt->dataFormat))
                {
                    retVal = FVID2_EINVALID_PARAMS;
                }
            }
            else
            {
              /*Do Nothing*/
            }
    }
        if ((uint32_t)UTRUE == scCfg->enable)
        {
            if((0U == (cnt & 0x1U)) &&
               ((FVID2_DF_YUV422I_UYVY == outFmt->dataFormat) ||
                (FVID2_DF_YUV422I_YUYV == outFmt->dataFormat)) &&
               (UTRUE == mscPrms->mscCfg.scCfg[cnt + 1U].enable))
            {
                retVal = FVID2_EINVALID_PARAMS;
            }

            if(((8u == cnt) || (9u == cnt)) &&
               ((FVID2_DF_YUV422I_UYVY == outFmt->dataFormat) ||
                (FVID2_DF_YUV422I_YUYV == outFmt->dataFormat)))
            {
                retVal = FVID2_EINVALID_PARAMS;
            }

            /* check for Input and output height */
            if (((FVID2_DF_GBI == outFmt->dataFormat) &&
                 (scCfg->outWidth > (inFmt->width * 2u))) ||
                ((FVID2_DF_GBI != outFmt->dataFormat) &&
                 (scCfg->outWidth > inFmt->width)) ||
                (scCfg->outHeight > inFmt->height))
            {
                retVal = FVID2_EINVALID_PARAMS;
            }

            /* check for ROI parameters */
            if (((FVID2_DF_GBI == outFmt->dataFormat) &&
                 ((scCfg->inRoi.cropStartX + scCfg->inRoi.cropWidth) >
                    ((inFmt->width*2u)+1U))) ||
                ((FVID2_DF_GBI != outFmt->dataFormat) &&
                 ((scCfg->inRoi.cropStartX + scCfg->inRoi.cropWidth) >
                    (inFmt->width+1U))) ||
                ((scCfg->inRoi.cropStartY + scCfg->inRoi.cropHeight) >
                    (inFmt->height+1U)))
            {
                retVal = FVID2_EINVALID_PARAMS;
            }

            /* check for phase parameters */
            if ((scCfg->filtMode == 1U) &&
               ((scCfg->horzAccInit > 4095U) ||
                (scCfg->vertAccInit > 4095U)))
            {
                retVal = FVID2_EINVALID_PARAMS;
            }

            /* TODO - Check if required pich will fit into allocated SL2 memory */
            minPitch = Vhwa_m2mMscCalcHorzSizeInBytes(scCfg->outWidth,
                                                      outFmt->ccsFormat,
                                                      outFmt->dataFormat);
            if (outFmt->pitch[0] < minPitch)
            {
                retVal = FVID2_EINVALID_PARAMS;
            }
            else
            {
                hObj->outChPrms[cnt].sl2Pitch = Vhwa_calcSl2Pitch(minPitch);
            }
        }
    }

    return (retVal);
}

static void Vhwa_mscSetLseCfg(Vhwa_M2mMscHandleObj *hObj,
                              const Vhwa_M2mMscParams *mscPrms)
{
    Vhwa_M2mMscCreatePrms *createPrms = NULL;
    Vhwa_M2mMscCommonObj  *comObj = NULL;
    CSL_LseConfig         *lseCfg = NULL;
    CSL_MscConfig         *cslMscCfg = NULL;
    uint32_t               lseThrId;
    uint32_t               inIdx;
    uint32_t               cnt, itrCnt;

    /* Check for Null pointer */
    GT_assert(VhwaMscTrace, (NULL != hObj));
    GT_assert(VhwaMscTrace, (NULL != mscPrms));

    for (itrCnt = 0; itrCnt < hObj->numIter; itrCnt++)
    {
        cslMscCfg = &hObj->cslMscCfg[itrCnt];
        lseCfg = &hObj->lseCfg[itrCnt];

        /* Initialize LSE configuration with the default values */
        CSL_lseConfigInit(lseCfg);

        comObj = &gM2mMscCommonObj;
        createPrms = &hObj->createPrms;
        inIdx = hObj->instId;

        if (VPAC_MSC_INST_ID_0 == hObj->instId)
        {
            lseThrId = CSL_LSE_THREAD_ID_0;
        }
        else
        {
            lseThrId = CSL_LSE_THREAD_ID_1;
        }


        /* For MSC, number of input and output channels are 1 */
        lseCfg->numInCh = 1U;
        lseCfg->numOutCh = 0U;

        lseCfg->enablePsa = createPrms->enablePsa;

        /* LSE Input channel 0 is used by MSC instance 0 and
           Input channel 1 is used by MSC instance 1 */
        for (cnt = 0u; cnt < CSL_LSE_MAX_INPUT_CH; cnt ++)
        {
            if (inIdx == cnt)
            {
                lseCfg->inChCfg[cnt].bypass = (uint32_t)UFALSE;
            }
            else
            {
                lseCfg->inChCfg[cnt].bypass = (uint32_t)UTRUE;
            }
        }

        lseCfg->inChCfg[inIdx].enable = (uint32_t)UTRUE;
        lseCfg->inChCfg[inIdx].frameWidth = cslMscCfg->inWidth;

        /* Use calculated hieght parameters */
        lseCfg->inChCfg[inIdx].frameHeight = cslMscCfg->inHeight;

        lseCfg->inChCfg[inIdx].enableAddrIncrBy2 = mscPrms->enableLineSkip;

        lseCfg->inChCfg[inIdx].ccsf =
            (Fvid2_ColorCompStorageFmt)mscPrms->inFmt.ccsFormat;
        lseCfg->inChCfg[inIdx].startOffset = 0U;
        lseCfg->inChCfg[inIdx].lineOffset =
                        hObj->inChPrms[0].sl2Pitch;

        lseCfg->inChCfg[inIdx].circBufSize =
                        comObj->sl2Prms.inSl2BuffDepth[inIdx][0];
        lseCfg->inChCfg[inIdx].bufAddr[0U] =
                    (uint32_t)comObj->sl2Prms.inSl2Addr[inIdx][0];

    #if defined VHWA_VPAC_IP_REV_VPAC
        lseCfg->inChCfg[inIdx].numInBuff = 1U;
    #elif defined VHWA_VPAC_IP_REV_VPAC3
        lseCfg->inChCfg[inIdx].skipAltLine[0] = 0u;
        lseCfg->inChCfg[inIdx].skipOddLine[0] = 0u;

        if ((FVID2_DF_YUV422I_YUYV ==
                (Fvid2_DataFormat)mscPrms->inFmt.dataFormat) ||
            (FVID2_DF_YUV422I_UYVY ==
                (Fvid2_DataFormat)mscPrms->inFmt.dataFormat))
        {
            lseCfg->inChCfg[inIdx].enableYuv422I = (uint32_t)UTRUE;
            lseCfg->inChCfg[inIdx].yuv422DataFmt =
                                (Fvid2_DataFormat)mscPrms->inFmt.dataFormat;
        }
        else
        {
            lseCfg->inChCfg[inIdx].enableYuv422I = (uint32_t)UFALSE;
        }

        if ((2u == hObj->numInChUsed) ||
             (FVID2_DF_YUV422I_UYVY == mscPrms->inFmt.dataFormat) ||
             (FVID2_DF_YUV422I_YUYV == mscPrms->inFmt.dataFormat))
        {
            lseCfg->inChCfg[inIdx].numInBuff = 2U;
            lseCfg->inChCfg[inIdx].bufAddr[1U] =
                        (uint32_t)comObj->sl2Prms.inSl2Addr[inIdx][1];

            if(hObj->inChPrms[1].height == (hObj->inChPrms[0].height/2u))
            {
                lseCfg->inChCfg[inIdx].skipAltLine[1] = 1u;
                lseCfg->inChCfg[inIdx].skipOddLine[1] = 1u;
            }
            else
            {
                lseCfg->inChCfg[inIdx].skipAltLine[1] = 0u;
                lseCfg->inChCfg[inIdx].skipOddLine[1] = 0u;
            }

            if((UTRUE == mscPrms->secChPrms.enable)               ||
               (FVID2_DF_YUV422I_UYVY ==
                    (Fvid2_DataFormat)mscPrms->inFmt.dataFormat) ||
               (FVID2_DF_YUV422I_YUYV ==
                    (Fvid2_DataFormat)mscPrms->inFmt.dataFormat))
            {
                lseCfg->inChCfg[inIdx].buffConfig[0].enable = UTRUE;
                lseCfg->inChCfg[inIdx].buffConfig[0].ccsf =
                                                    hObj->inChPrms[1].ccsf;
                lseCfg->inChCfg[inIdx].buffConfig[0].width =
                                                    hObj->inChPrms[1].width;

                if((FVID2_DF_YUV422I_UYVY ==
                        (Fvid2_DataFormat)mscPrms->inFmt.dataFormat) ||
                   (FVID2_DF_YUV422I_YUYV ==
                        (Fvid2_DataFormat)mscPrms->inFmt.dataFormat))
                {
                    lseCfg->inChCfg[inIdx].buffConfig[0].skipSl2Read = UTRUE;
                }
                else
                {
                    lseCfg->inChCfg[inIdx].buffConfig[0].skipSl2Read = UFALSE;
                }
            }
        }
        else
        {
            lseCfg->inChCfg[inIdx].numInBuff = 1U;
        }
    #endif /* #elif defined VHWA_VPAC_IP_REV_VPAC3 */

        if (MSC_TAP_SEL_5TAPS == cslMscCfg->mscCfg.tapSel)
        {
            /* Configuration for 5 TAP filter */
            lseCfg->inChCfg[inIdx].knTopPadding = 2U;
            lseCfg->inChCfg[inIdx].knLineOffset = 0U;
            lseCfg->inChCfg[inIdx].knHeight = 5U;
            lseCfg->inChCfg[inIdx].knBottomPadding = 2U;
        }
        else if (MSC_TAP_SEL_4TAPS == cslMscCfg->mscCfg.tapSel)
        {
            /* Configuration for 4 TAP filter, applicable for vertical
               Scaling only */
            lseCfg->inChCfg[inIdx].knTopPadding = 1U;
            lseCfg->inChCfg[inIdx].knLineOffset = 1U;
            lseCfg->inChCfg[inIdx].knHeight = 4U;
            lseCfg->inChCfg[inIdx].knBottomPadding = 2U;
        }
        else
        {
            /* Configuration for 3 TAP filter, applicable for vertical
               Scaling only */
            lseCfg->inChCfg[inIdx].knTopPadding = 0U;
            lseCfg->inChCfg[inIdx].knLineOffset = 2U;
            lseCfg->inChCfg[inIdx].knHeight = 3U;
            lseCfg->inChCfg[inIdx].knBottomPadding = 2U;
        }

        for (cnt = 0; cnt < MSC_MAX_OUTPUT ; cnt ++)
        {
            if (0U != (hObj->scalarUsed & ((uint32_t)1U << cnt)))
            {
                lseCfg->outChCfg[cnt].enable = (uint32_t)UTRUE;
                lseCfg->outChCfg[cnt].ccsf =
                    (Fvid2_ColorCompStorageFmt)mscPrms->outFmt[cnt].ccsFormat;
                lseCfg->outChCfg[cnt].lineOffset =
                                hObj->outChPrms[cnt].sl2Pitch;
                lseCfg->outChCfg[cnt].circBufSize =
                                comObj->sl2Prms.outSl2BuffDepth[cnt];
                lseCfg->outChCfg[cnt].bufAddr =
                                (uint32_t)comObj->sl2Prms.outSl2Addr[cnt];

                lseCfg->outChCfg[cnt].thrId = lseThrId;

            #if defined VHWA_VPAC_IP_REV_VPAC3
                lseCfg->outChCfg[cnt].enableRounding = UFALSE;

                if ((2u == hObj->numInChUsed) ||
                    (FVID2_DF_YUV422I_UYVY == mscPrms->inFmt.dataFormat) ||
                    (FVID2_DF_YUV422I_YUYV == mscPrms->inFmt.dataFormat))
                {
                    if((FVID2_DF_LUMA_ONLY == mscPrms->outFmt[cnt].dataFormat) ||
                        (FVID2_DF_R == mscPrms->outFmt[cnt].dataFormat) ||
                        (FVID2_DF_PLANE_1 == mscPrms->outFmt[cnt].dataFormat) ||
                        (((FVID2_DF_YUV422I_UYVY ==
                                mscPrms->outFmt[cnt].dataFormat) ||
                          (FVID2_DF_YUV422I_YUYV ==
                                mscPrms->outFmt[cnt].dataFormat)) &&
                            (0u == (cnt & 0x1U))))
                    {
                        lseCfg->outChCfg[cnt].chThrId = 0u;
                    }
                    else
                    {
                        lseCfg->outChCfg[cnt].chThrId = 1u;
                    }

                    if(((FVID2_DF_YUV420SP_UV == mscPrms->inFmt.dataFormat) ||
                         (FVID2_DF_YUV420SP_VU == mscPrms->inFmt.dataFormat)) &&
                        (FVID2_DF_CHROMA_ONLY == mscPrms->outFmt[cnt].dataFormat))

                    {
                        lseCfg->outChCfg[cnt].skipAltLine = 1u;
                        lseCfg->outChCfg[cnt].skipOddLine = 1u;
                        lseCfg->outChCfg[cnt].enableYuv422Out = (uint32_t)UFALSE;
                    }
                    else if(((FVID2_DF_YUV422I_UYVY ==
                                    mscPrms->outFmt[cnt].dataFormat) ||
                             (FVID2_DF_YUV422I_YUYV ==
                                    mscPrms->outFmt[cnt].dataFormat)) &&
                            (0u == (cnt & 0x1U)))
                    {
                        lseCfg->outChCfg[cnt].skipAltLine = 0u;
                        lseCfg->outChCfg[cnt].skipOddLine = 0u;

                        lseCfg->outChCfg[cnt].enableYuv422Out = (uint32_t)UTRUE;
                        lseCfg->outChCfg[cnt].yuv422DataFmt =
                                (Fvid2_DataFormat)mscPrms->outFmt[cnt].dataFormat;
                    }
                    else
                    {
                        lseCfg->outChCfg[cnt].skipAltLine = 0u;
                        lseCfg->outChCfg[cnt].skipOddLine = 0u;
                        lseCfg->outChCfg[cnt].enableYuv422Out = (uint32_t)UFALSE;
                    }
                }
                else
                {
                    lseCfg->outChCfg[cnt].chThrId = 0u;
                    lseCfg->outChCfg[cnt].enableYuv422Out = (uint32_t)UFALSE;
                    lseCfg->outChCfg[cnt].skipAltLine = 0u;
                    lseCfg->outChCfg[cnt].skipOddLine = 0u;
                }
            #endif /* #if defined VHWA_VPAC_IP_REV_VPAC3 */

                lseCfg->numOutCh++;
                lseCfg->outChCfg[cnt].bypass = (uint32_t)UFALSE;
            }
            else
            {
                lseCfg->outChCfg[cnt].bypass = (uint32_t)UTRUE;
            }
        }
    }
}

static void Vhwa_mscSetHtsCfg(const Vhwa_M2mMscInstObj *instObj,
                              Vhwa_M2mMscHandleObj *hObj,
                              const Vhwa_M2mMscParams *mscPrms)
{
    CSL_HtsSchConfig      *htsCfg = NULL;
    Vhwa_M2mMscCommonObj  *comObj = NULL;
    uint32_t               cnt, itrCnt, chCnt;

    /* Check for Null pointer */
    GT_assert(VhwaMscTrace, (NULL != hObj));
    GT_assert(VhwaMscTrace, (NULL != instObj));
    GT_assert(VhwaMscTrace, (NULL != mscPrms));

    for (itrCnt = 0; itrCnt < hObj->numIter; itrCnt++)
    {
        htsCfg = &hObj->htsCfg[itrCnt];
        comObj = &gM2mMscCommonObj;

        CSL_htsSchConfigInit(htsCfg);

        if (hObj->instId == (uint32_t)VPAC_MSC_INST_ID_0)
        {
            htsCfg->schId = CSL_HTS_HWA_SCH_MSC0;
        }
        else if (hObj->instId == (uint32_t)VPAC_MSC_INST_ID_1)
        {
            htsCfg->schId = CSL_HTS_HWA_SCH_MSC1;
        }
        else
        {
            /* Invalid MSC instance Execution will not reach this block */
        }
        htsCfg->pipeline = instObj->pipeline;

        htsCfg->enableStream = (uint32_t)UFALSE;
        htsCfg->enableHop = (uint32_t)UFALSE;
        htsCfg->enableWdTimer = (uint32_t)UFALSE;

        htsCfg->enableBwLimit = UFALSE;

        /* Clear reset flag for DMA producer for HWA */
        htsCfg->dmaProdCfg[0U].bypass = (uint32_t)UTRUE;
        htsCfg->dmaProdCfg[1U].bypass = (uint32_t)UTRUE;

        for (chCnt = 0; chCnt < hObj->numInChUsed; chCnt++)
        {
            if(UTRUE == hObj->inChPrms[chCnt].buffEnable)
            {
                htsCfg->consCfg[chCnt].enable = (uint32_t)UTRUE;
                htsCfg->consCfg[chCnt].prodId = CSL_HTS_PROD_IDX_UDMA;

                /* Clear reset flag for DMA producer for HWA */
                htsCfg->dmaProdCfg[chCnt].bypass = (uint32_t)UFALSE;

                htsCfg->dmaProdCfg[chCnt].enable = (uint32_t)UTRUE;
                htsCfg->dmaProdCfg[chCnt].dmaChNum =
                        Udma_chGetNum(comObj->inChHandle[hObj->instId][chCnt]);

                htsCfg->dmaProdCfg[chCnt].pipeline = htsCfg->pipeline;
                htsCfg->dmaProdCfg[chCnt].consId = CSL_HTS_CONS_IDX_UDMA;

                if (MSC_TAP_SEL_5TAPS == mscPrms->mscCfg.tapSel)
                {
                    /* Configuration for 5 TAP filter */
                    htsCfg->dmaProdCfg[chCnt].threshold = 5U;
                    htsCfg->dmaProdCfg[chCnt].cntPreLoad = 2U;
                    htsCfg->dmaProdCfg[chCnt].cntPostLoad = 2U;
                }
                else if (MSC_TAP_SEL_4TAPS == mscPrms->mscCfg.tapSel)
                {
                    /* Configuration for 4 TAP filter, applicable for vertical
                       Scaling only */
                    htsCfg->dmaProdCfg[chCnt].threshold = 4U;
                    htsCfg->dmaProdCfg[chCnt].cntPreLoad = 1U;
                    htsCfg->dmaProdCfg[chCnt].cntPostLoad = 2U;
                }
                else
                {
                    /* Configuration for 3 TAP filter, applicable for vertical
                       Scaling only */
                    htsCfg->dmaProdCfg[chCnt].threshold = 3U;
                    htsCfg->dmaProdCfg[chCnt].cntPreLoad = 0U;
                    htsCfg->dmaProdCfg[chCnt].cntPostLoad = 2U;
                }

                htsCfg->dmaProdCfg[chCnt].enableHop = (uint32_t)UTRUE;

                /* Use Calculated height */
                if (1U == gM2mMscCommonObj.socInfo.isDualChannel)
                {
                    htsCfg->dmaProdCfg[chCnt].numHop =
                                                hObj->inChPrms[chCnt].height;
                }
                else
                {
                    htsCfg->dmaProdCfg[chCnt].numHop =
                                                hObj->cslMscCfg[itrCnt].inHeight;
                }

                if(UTRUE == mscPrms->enableLineSkip)
                {
                    htsCfg->dmaProdCfg[chCnt].countDec = 2U;
                }
                else
                {
                    htsCfg->dmaProdCfg[chCnt].countDec = 1U;
                }

                htsCfg->dmaProdCfg[chCnt].depth =
                                    comObj->sl2Prms.inSl2BuffDepth[hObj->instId][0];

                /* Keep DMA producer stream enable UFALSE as in case of OTF DMA
                   will be diabled producer */
                htsCfg->dmaProdCfg[chCnt].enableStream = (uint32_t)UFALSE;

                if((1u == chCnt) &&
                    (hObj->inChPrms[1].height == (hObj->inChPrms[0].height/2u)) &&
                    (1U == gM2mMscCommonObj.socInfo.isDualChannel))
                {
                    htsCfg->dmaProdCfg[chCnt].paCfg.enable = (uint32_t)UTRUE;
                    htsCfg->dmaProdCfg[chCnt].paCfg.enableBufCtrl = 0U;

                    htsCfg->dmaProdCfg[chCnt].paCfg.psMaxCnt = 2u;
                    htsCfg->dmaProdCfg[chCnt].paCfg.csMaxCnt =
                                            htsCfg->dmaProdCfg[chCnt].threshold;
                    htsCfg->dmaProdCfg[chCnt].paCfg.enableDecCtrl = 1U;
                    htsCfg->dmaProdCfg[chCnt].paCfg.countDec = 1U;
                }
            }
        }

        for (cnt = 0; cnt < MSC_MAX_OUTPUT; cnt++)
        {
            if (UTRUE == hObj->outChPrms[cnt].buffEnable)
            {
                htsCfg->prodCfg[cnt].enable = (uint32_t)UTRUE;
                htsCfg->prodCfg[cnt].consId = CSL_HTS_CONS_IDX_UDMA;

                htsCfg->prodCfg[cnt].threshold = 1u;
                htsCfg->prodCfg[cnt].cntPreLoad = 0u;
                htsCfg->prodCfg[cnt].cntPostLoad = 0u;

                htsCfg->prodCfg[cnt].depth =
                            comObj->sl2Prms.outSl2BuffDepth[cnt];
                htsCfg->prodCfg[cnt].countDec  =1U;

                htsCfg->dmaConsCfg[cnt].enable = (uint32_t)UTRUE;
                htsCfg->dmaConsCfg[cnt].dmaChNum =
                                Udma_chGetNum(comObj->outChHandle[cnt]);
                htsCfg->dmaConsCfg[cnt].pipeline = htsCfg->pipeline;
                htsCfg->dmaConsCfg[cnt].enableStream = (uint32_t)UFALSE;
                htsCfg->dmaConsCfg[cnt].prodId = CSL_HTS_PROD_IDX_UDMA;

                htsCfg->dmaConsCfg[cnt].bypass = (uint32_t)UFALSE;
            }
            else
            {
                htsCfg->dmaConsCfg[cnt].bypass = (uint32_t)UTRUE;
            }
        }
        htsCfg->dmaConsCfg[10].bypass = (uint32_t)UTRUE;
    }
}

static void Vhwa_mscUpdateYuv422iCslPrms(Vhwa_M2mMscHandleObj *hObj,
                                         CSL_MscConfig *cslMscCfg)
{
    uint32_t cnt;
    Vhwa_M2mMscParams *mscPrms;

    mscPrms = &hObj->mscPrms;

    for (cnt = 0; cnt < MSC_MAX_OUTPUT; )
    {
        if((UTRUE == mscPrms->mscCfg.scCfg[cnt].enable) &&
           ((FVID2_DF_YUV422I_YUYV == mscPrms->outFmt[cnt].dataFormat) ||
           (FVID2_DF_YUV422I_UYVY == mscPrms->outFmt[cnt].dataFormat)))
        {
            Fvid2Utils_memcpy(&cslMscCfg->mscCfg.scCfg[cnt+1U],
                              &cslMscCfg->mscCfg.scCfg[cnt],
                              sizeof(Msc_ScConfig));

            Fvid2Utils_memcpy(&mscPrms->outFmt[cnt+1U],
                              &mscPrms->outFmt[cnt],
                              sizeof(Fvid2_Format));

            cslMscCfg->mscCfg.scCfg[cnt].isInterleaveFormat = UFALSE;
            cslMscCfg->mscCfg.scCfg[cnt+1U].isInterleaveFormat = UTRUE;

            hObj->scalarUsed |= ((uint32_t)1U << (cnt + 1U));
        }
        if(FVID2_DF_CHROMA_ONLY == mscPrms->outFmt[cnt].dataFormat)
        {
            cslMscCfg->mscCfg.scCfg[cnt].isInterleaveFormat = UTRUE;
        }
        if(FVID2_DF_CHROMA_ONLY == mscPrms->outFmt[cnt].dataFormat)
        {
            cslMscCfg->mscCfg.scCfg[cnt].isInterleaveFormat = UTRUE;
        }

        cnt = cnt + 2u;
    }

}

static void Vhwa_mscCalPrms(Vhwa_M2mMscHandleObj *hObj,
                            const Vhwa_M2mMscParams *mscPrms)
{
    uint32_t       itrCnt, cnt;
    uint32_t       pitch;
    CSL_MscConfig *cslMscCfg = NULL;

    /* Check for Null pointer */
    GT_assert(VhwaMscTrace, (NULL != hObj));
    GT_assert(VhwaMscTrace, (NULL != mscPrms));

    if ((FVID2_DF_YUV420SP_UV == mscPrms->inFmt.dataFormat) ||
        (FVID2_DF_YUV420SP_VU == mscPrms->inFmt.dataFormat))
    {
        if(0U == gM2mMscCommonObj.socInfo.isDualChannel)
        {
            /* Single channel per MSC */
            hObj->numIter = 2U;
            hObj->numInChUsed = 1u;
            hObj->inChPrms[0].buffEnable = UTRUE;
            hObj->inChPrms[1].buffEnable = UFALSE;
        }
        else
        {
            /* 2 cahnnels for LUMA and Chroma needs to be processed */
            hObj->numIter = 1U;
            hObj->numInChUsed = 2u;
            hObj->inChPrms[0].buffEnable = UTRUE;
            hObj->inChPrms[1].buffEnable = UTRUE;
        }
    }
    else if ((FVID2_DF_YUV422SP_UV == mscPrms->inFmt.dataFormat) ||
             (FVID2_DF_YUV422SP_VU == mscPrms->inFmt.dataFormat) ||
             (FVID2_DF_R_GBI == mscPrms->inFmt.dataFormat) ||
             (FVID2_DF_2PLANES == mscPrms->inFmt.dataFormat))
    {
        hObj->numIter = 1U;
        hObj->numInChUsed = 2u;
        hObj->inChPrms[0].buffEnable = UTRUE;
        hObj->inChPrms[1].buffEnable = UTRUE;
    }
    else if ((FVID2_DF_YUV422I_YUYV == mscPrms->inFmt.dataFormat) ||
             (FVID2_DF_YUV422I_UYVY == mscPrms->inFmt.dataFormat))
    {
        hObj->numIter = 1U;
        hObj->numInChUsed = 1u;
        hObj->inChPrms[0].buffEnable = UTRUE;
        hObj->inChPrms[1].buffEnable = UFALSE;
    }
    else
    {
        /* Only Chroma or Luma needs to be processed */
        hObj->numIter = 1U;
        hObj->numInChUsed = 1u;
        hObj->inChPrms[0].buffEnable = UTRUE;
        hObj->inChPrms[1].buffEnable = UFALSE;
    }

    hObj->scalarUsed = 0U;

    /* Update the Required Scalar Parameter */
    for (cnt = 0U; cnt < MSC_MAX_OUTPUT; cnt++)
    {
        if (mscPrms->mscCfg.scCfg[cnt].enable == UTRUE)
        {
            hObj->scalarUsed |= ((uint32_t)1U << cnt);
            hObj->outChPrms[cnt].buffEnable = UTRUE;

            gM2mMscCommonObj.mscProdStatus |= ((uint32_t)1U << cnt);
        }
        else
        {
            hObj->outChPrms[cnt].buffEnable = UFALSE;
        }
    }

    for (itrCnt = 0; itrCnt < hObj->numIter; itrCnt++)
    {
        cslMscCfg = &hObj->cslMscCfg[itrCnt];

        Fvid2Utils_memcpy(&cslMscCfg->mscCfg, &mscPrms->mscCfg, sizeof(Msc_Config));

        cslMscCfg->inWidth = mscPrms->inFmt.width;

        hObj->inChPrms[0].width = mscPrms->inFmt.width;
        hObj->inChPrms[0].ccsf = mscPrms->inFmt.ccsFormat;
        hObj->inChPrms[0].height = mscPrms->inFmt.height;

        if (1u == gM2mMscCommonObj.socInfo.isDualChannel)
        {
            cslMscCfg->inHeight =  mscPrms->inFmt.height;
            if (FVID2_DF_R_GBI == mscPrms->inFmt.dataFormat)
            {
                hObj->inChPrms[1].width = (mscPrms->inFmt.width * 2u);
            }
            else
            {
                hObj->inChPrms[1].width = mscPrms->inFmt.width;
            }

            if(UTRUE == mscPrms->secChPrms.enable)
            {
                hObj->inChPrms[1].ccsf = mscPrms->secChPrms.ccsf;
            }
            else
            {
                hObj->inChPrms[1].ccsf = mscPrms->inFmt.ccsFormat;
            }

            if((FVID2_DF_YUV422SP_UV == mscPrms->inFmt.dataFormat) ||
               (FVID2_DF_YUV422SP_VU == mscPrms->inFmt.dataFormat) ||
               (FVID2_DF_R_GBI == mscPrms->inFmt.dataFormat) ||
               (FVID2_DF_2PLANES == mscPrms->inFmt.dataFormat))
            {
                hObj->inChPrms[1].height = mscPrms->inFmt.height;
            }
            else if ((FVID2_DF_YUV422I_YUYV == mscPrms->inFmt.dataFormat) ||
                     (FVID2_DF_YUV422I_UYVY == mscPrms->inFmt.dataFormat))
            {
                hObj->inChPrms[1].height = mscPrms->inFmt.height;

                Vhwa_mscUpdateYuv422iCslPrms(hObj, cslMscCfg);

            }
            else if((FVID2_DF_YUV420SP_UV == mscPrms->inFmt.dataFormat) ||
                    (FVID2_DF_YUV420SP_VU == mscPrms->inFmt.dataFormat))
            {
                hObj->inChPrms[1].height = mscPrms->inFmt.height/2u;
            }
            else
            {
              /*Do Nothing*/
            }

            if((FVID2_DF_YUV422SP_UV == mscPrms->inFmt.dataFormat) ||
               (FVID2_DF_YUV422SP_VU == mscPrms->inFmt.dataFormat) ||
               (FVID2_DF_YUV420SP_UV == mscPrms->inFmt.dataFormat) ||
               (FVID2_DF_YUV420SP_VU == mscPrms->inFmt.dataFormat) ||
               (FVID2_DF_R_GBI == mscPrms->inFmt.dataFormat) ||
               (FVID2_DF_CHROMA_ONLY == mscPrms->inFmt.dataFormat))
            {
                for (cnt = 0; cnt < MSC_MAX_OUTPUT; cnt++)
                {
                    if((FVID2_DF_CHROMA_ONLY == mscPrms->outFmt[cnt].dataFormat) ||
                        (FVID2_DF_GBI == mscPrms->outFmt[cnt].dataFormat))
                    {
                        cslMscCfg->mscCfg.scCfg[cnt].isInterleaveFormat = UTRUE;
                    }
                    else
                    {
                        cslMscCfg->mscCfg.scCfg[cnt].isInterleaveFormat = UFALSE;
                    }
                }
            }
        }
        else if (((FVID2_DF_YUV420SP_UV == mscPrms->inFmt.dataFormat) ||
                  (FVID2_DF_YUV420SP_VU == mscPrms->inFmt.dataFormat)) &&
                 (itrCnt == 1U))
        {
            /* Height will be 1/2 for Chroma */
            cslMscCfg->inHeight =  mscPrms->inFmt.height/2U;
            /* This is used by HTS to set the chroma channel height */
            hObj->inChPrms[1].height = mscPrms->inFmt.height/2u;

            for (cnt = 0; cnt < MSC_MAX_OUTPUT; cnt++)
            {
                cslMscCfg->mscCfg.scCfg[cnt].outHeight =
                        cslMscCfg->mscCfg.scCfg[cnt].outHeight/2U;
                cslMscCfg->mscCfg.scCfg[cnt].inRoi.cropStartY =
                        cslMscCfg->mscCfg.scCfg[cnt].inRoi.cropStartY/2U;
                cslMscCfg->mscCfg.scCfg[cnt].inRoi.cropHeight =
                        cslMscCfg->mscCfg.scCfg[cnt].inRoi.cropHeight/2U;

                cslMscCfg->mscCfg.scCfg[cnt].isInterleaveFormat = UTRUE;

            }
            (void)CSL_mscUpdateUVPrms(cslMscCfg);
        }
        else
        {
            for (cnt = 0; cnt < MSC_MAX_OUTPUT; cnt++)
            {
                if(FVID2_DF_CHROMA_ONLY == mscPrms->outFmt[cnt].dataFormat)
                {
                    cslMscCfg->mscCfg.scCfg[cnt].isInterleaveFormat = UTRUE;
                }
                else
                {
                    cslMscCfg->mscCfg.scCfg[cnt].isInterleaveFormat = UFALSE;
                }

            }
            cslMscCfg->inHeight =  mscPrms->inFmt.height;
        }

        pitch = Vhwa_m2mMscCalcHorzSizeInBytes(
                                                hObj->inChPrms[1].width,
                                                hObj->inChPrms[1].ccsf,
                                                mscPrms->inFmt.dataFormat);

        hObj->inChPrms[1].sl2Pitch = Vhwa_calcSl2Pitch(pitch);

        if(hObj->inChPrms[1].sl2Pitch > hObj->inChPrms[0].sl2Pitch)
        {
            hObj->inChPrms[0].sl2Pitch = hObj->inChPrms[1].sl2Pitch;
        }
        else
        {
            hObj->inChPrms[1].sl2Pitch = hObj->inChPrms[0].sl2Pitch;
        }

    }
}

static void Vhwa_mscSetHtsLimit(Vhwa_M2mMscHandleObj *hObj,
                               const Vhwa_HtsLimiter *htsLimit)
{
    uint32_t          cnt;
    CSL_HtsSchConfig *htsCfg = NULL;

    /* Check for Null pointer */
    GT_assert(VhwaMscTrace, (NULL != hObj));
    GT_assert(VhwaMscTrace, (NULL != htsLimit));

    for (cnt = 0; cnt < hObj->numIter; cnt++)
    {
        htsCfg = &hObj->htsCfg[cnt];

        /* Configure  HTS BW limiter */
        htsCfg->enableBwLimit = htsLimit->enableBwLimit;
        htsCfg->cycleCnt      = htsLimit->cycleCnt;
        htsCfg->tokenCnt      = htsLimit->tokenCnt;
    }
}

int32_t Vhwa_m2mMscSetConfigInHW(const Vhwa_M2mMscQueueObj *qObj,
                                Vhwa_M2mMscCommonObj *comObj,
                                uint32_t intCnt)
{
    int32_t               status;
    Vhwa_M2mMscHandleObj *hObj = NULL;

    /* Null pointer check */
    GT_assert(VhwaMscTrace, (NULL != qObj));
    GT_assert(VhwaMscTrace, (NULL != comObj));
    GT_assert(VhwaMscTrace, (NULL != qObj->hObj));

    hObj = qObj->hObj;

    /* Configure MSC Core */
    status = CSL_mscSetConfig(comObj->socInfo.mscRegs,
                &hObj->cslMscCfg[intCnt]);

    if (FVID2_SOK == status)
    {
        /* Configure HWA Common Wrapper LSE */
        status = CSL_lseSetConfig(comObj->socInfo.lseRegs, &hObj->lseCfg[intCnt]);
    }

    if (FVID2_SOK == status)
    {
        /* Configure HWA Common Wrapper HTS */
        status = CSL_htsSetThreadConfig(comObj->socInfo.htsRegs, &hObj->htsCfg[intCnt]);
    }

    return (status);
}

int32_t Vhwa_m2mMscSetFrameSize(const Vhwa_M2mMscQueueObj *qObj,
                                Vhwa_M2mMscCommonObj *comObj,
                                uint32_t intCnt)
{
    int32_t               status;
    Vhwa_M2mMscHandleObj *hObj = NULL;

    /* Null pointer check */
    GT_assert(VhwaMscTrace, (NULL != qObj));
    GT_assert(VhwaMscTrace, (NULL != comObj));
    GT_assert(VhwaMscTrace, (NULL != qObj->hObj));

    hObj = qObj->hObj;

    /* Configure MSC Core */
    status = CSL_mscSetFrameSize(comObj->socInfo.mscRegs,
                &hObj->cslMscCfg[intCnt]);

    if (FVID2_SOK == status)
    {
        /* Configure HWA Common Wrapper LSE */
        status = CSL_lseSetUpdateConfig(comObj->socInfo.lseRegs,
            &hObj->lseCfg[intCnt]);
    }

    if (FVID2_SOK == status)
    {
        /* Configure HWA Common Wrapper HTS */
        status = CSL_htsSetThreadUpdateConfig(comObj->socInfo.htsRegs,
            &hObj->htsCfg[intCnt]);
    }

    if (FVID2_SOK == status)
    {
        /* Start HTS Channels */
        status = CSL_htsThreadStart(comObj->socInfo.htsRegs, &hObj->htsCfg[intCnt]);
    }

    return (status);
}

int32_t Vhwa_m2mMscSubmitRequest(Vhwa_M2mMscInstObj *instObj,
                                 Vhwa_M2mMscQueueObj *qObj,
                                 Vhwa_M2mMscCommonObj *comObj,
                                 uint32_t intCnt)
{
    int32_t               retVal;
    Vhwa_M2mMscHandleObj *hObj = NULL;

    GT_assert(VhwaMscTrace, (NULL != instObj));
    GT_assert(VhwaMscTrace, (NULL != qObj));
    GT_assert(VhwaMscTrace, (NULL != comObj));
    GT_assert(VhwaMscTrace, (NULL != qObj->hObj));

    hObj = qObj->hObj;

    /* Submit Rings to the Ring Accelerator */
    retVal = Vhwa_mscM2mSubmitRing(instObj, hObj, comObj, intCnt);

    if (FVID2_SOK == retVal)
    {
        /* Start Channels in LSE */
        CSL_lseStartChannels(comObj->socInfo.lseRegs, &hObj->lseCfg[intCnt]);

        /* Start Channels in HTS */
        retVal = CSL_htsThreadStart(comObj->socInfo.htsRegs, &hObj->htsCfg[intCnt]);
    }
    if (FVID2_SOK == retVal)
    {
        /* Better to set Active object to this q object, so that if
             * interrupt comes immediately, activeQObj would be set..
             * If pipeline start fails, it would be set to NULL.*/
        instObj->activeQObj = qObj;

        instObj->totalReqCnt ++;

        #ifndef VHWA_USE_PIPELINE_COMMON_ENABLE
        if (NULL != hObj->createPrms.getTimeStamp)
        {
            hObj->perf[intCnt] = hObj->createPrms.getTimeStamp();
        }
        if (UFALSE == hObj->fcStatus.isFlexConnect)
        {
            /* Start HTS pipeline */
            retVal = CSL_htsPipelineStart(comObj->socInfo.htsRegs,
                                          &hObj->htsCfg[intCnt]);
        }
        #endif
        if (FVID2_SOK != retVal)
        {
            instObj->activeQObj = NULL;
            retVal = FVID2_EFAIL;
        }
        else
        {
            retVal = FVID2_SOK;
        }
    }

    return (retVal);
}

void Vhwa_m2mMscUpdatePipeline(uint32_t instId, uint32_t htsPipeline)
{
    Vhwa_M2mMscInstObj *instObj = NULL;
    Vhwa_M2mMscCommonObj   *comObj = NULL;

    comObj = &gM2mMscCommonObj;


    instObj = &gM2mMscInstObj[instId];

    /* Disable HTS Interrupt */
    Vhwa_disableHtsIntr(comObj->socInfo.vpacIntdRegs,
                comObj->mscInitPrms.irqInfo[instId].vhwaIrqNum,
                instObj->pipeline);

    /* Set HTS pipeline */
    instObj->pipeline = htsPipeline;

    /* Enable pipeline Interrupt */
    Vhwa_enableHtsIntr(comObj->socInfo.vpacIntdRegs,
                comObj->mscInitPrms.irqInfo[instId].vhwaIrqNum,
                instObj->pipeline);
}

static void Vhwa_m2mMscFcGetPrms(Vhwa_M2mMscHandleObj *hObj,
                                Vhwa_M2mMscFcGetPrms *fcPrms)
{
    fcPrms->htsCfg = &hObj->htsCfg[0];
    fcPrms->inSl2Pitch[0] = hObj->inChPrms[0].sl2Pitch;
#if defined VHWA_VPAC_IP_REV_VPAC3
    fcPrms->inSl2Pitch[1] = hObj->inChPrms[1].sl2Pitch;
#endif /* #if defined VHWA_VPAC_IP_REV_VPAC3 */
}



static void Vhwa_m2mMscUpdateFcConnPrms(Vhwa_M2mMscHandleObj *hObj,
                                Vhwa_M2mMscFcConPrms *sl2FcPrms)
{
    uint8_t              *pTrMem = NULL;
    uint16_t             height;
    CSL_UdmapTR9         *pTr9;

    hObj->htsCfg[0].prodCfg[sl2FcPrms->ctrlIdx].enable = UTRUE;
    hObj->htsCfg[0].prodCfg[sl2FcPrms->ctrlIdx].consId = sl2FcPrms->htsConsId;
    hObj->htsCfg[0].prodCfg[sl2FcPrms->ctrlIdx].depth = sl2FcPrms->sl2Depth;

    hObj->htsCfg[0].prodCfg[sl2FcPrms->ctrlIdx].threshold =
                                            sl2FcPrms->htsThreshold;
    hObj->htsCfg[0].prodCfg[sl2FcPrms->ctrlIdx].cntPreLoad =
                                            sl2FcPrms->htsDmaProdPreLoad;
    hObj->htsCfg[0].prodCfg[sl2FcPrms->ctrlIdx].cntPostLoad =
                                            sl2FcPrms->htsDmaProdPostLoad;
    hObj->htsCfg[0].prodCfg[sl2FcPrms->ctrlIdx].countDec =
                                            sl2FcPrms->htsDmaProdCntDec;

    hObj->htsCfg[0].prodCfg[sl2FcPrms->ctrlIdx].isMaskSelect =
                                            sl2FcPrms->enableMaskSel;
    hObj->htsCfg[0].prodCfg[sl2FcPrms->ctrlIdx].maskSelect = sl2FcPrms->maskSel;

    hObj->outChPrms[sl2FcPrms->outputIdx].sl2Pitch = sl2FcPrms->sl2Pitch;

    hObj->htsCfg[0].prodCfg[sl2FcPrms->outputIdx].depth = sl2FcPrms->sl2Depth;
    hObj->lseCfg[0].outChCfg[sl2FcPrms->outputIdx].bufAddr = (uint32_t)sl2FcPrms->sl2Addr;
    hObj->lseCfg[0].outChCfg[sl2FcPrms->outputIdx].circBufSize = sl2FcPrms->sl2Depth;

    height = (uint16_t)hObj->cslMscCfg[0].mscCfg.scCfg[sl2FcPrms->outputIdx].outHeight;

    pTrMem = hObj->outChPrms[sl2FcPrms->outputIdx].trMem[0];
    pTr9 = (CSL_UdmapTR9 *)((uint32_t)pTrMem + sizeof(CSL_UdmapTR15));
    CacheP_Inv(pTrMem, VHWA_MSC_UDMA_TRPD_SIZE);

    pTr9->addr    = sl2FcPrms->sl2Addr;
    pTr9->icnt1   = (uint16_t)sl2FcPrms->sl2Depth;
    pTr9->icnt2   = (uint16_t)(height/sl2FcPrms->sl2Depth);

    if ((pTr9->icnt2 * pTr9->icnt1) != height)
    {
        pTr9->icnt2 ++;
    }

    CacheP_wb(pTrMem, VHWA_MSC_UDMA_TRPD_SIZE);

}

static void Vhwa_m2mVissUpdateFcPrms(Vhwa_M2mMscHandleObj *hObj,
                                    Vhwa_M2mMscFcUpdatePrms *fcPrms)
{
    uint32_t cnt = 0;

    for (cnt = 0; cnt < MSC_MAX_OUTPUT; cnt++)
    {
        hObj->htsCfg[0].dmaConsCfg[cnt].enable = fcPrms->outDmaEnable[cnt];
        hObj->fcStatus.outDmaEnable[cnt] = fcPrms->outDmaEnable[cnt];
    }

    /* DMA related Parameters */
    hObj->fcStatus.isFlexConnect = fcPrms->isFlexConnect;
    hObj->fcStatus.inDmaEnable[0] = fcPrms->inDmaEnable[0];
#if defined VHWA_VPAC_IP_REV_VPAC3
    hObj->fcStatus.inDmaEnable[1] = fcPrms->inDmaEnable[1];
#endif /* #if defined VHWA_VPAC_IP_REV_VPAC3 */
}

static int32_t Vhwa_m2mMscCheckCreatePrms(uint32_t drvId, uint32_t drvInstId)
{
    int32_t             retVal = FVID2_SOK;

    if (FVID2_VHWA_M2M_MSC_DRV_ID != drvId)
    {
        GT_0trace(VhwaMscTrace, GT_ERR, "Invalid Driver ID !!\n");
        retVal = FVID2_EINVALID_PARAMS;
    }
    else
    {
        /* Check for correct instance ID */
#if defined VHWA_M2M_VPAC_INSTANCE
#if (VHWA_M2M_VPAC_INSTANCE == 0)
        if ((VHWA_M2M_VPAC_0_MSC_DRV_INST_ID_0 != drvInstId) &&
            (VHWA_M2M_VPAC_0_MSC_DRV_INST_ID_1 != drvInstId))
        {
            retVal = FVID2_EINVALID_PARAMS;
            GT_0trace(VhwaMscTrace, GT_ERR, "Invalid/unsupported Instance Id\n");
        }
#endif
#endif

#if defined VHWA_M2M_VPAC_INSTANCE
#if (VHWA_M2M_VPAC_INSTANCE == 1)
        if ((VHWA_M2M_VPAC_1_MSC_DRV_INST_ID_0 != drvInstId) &&
            (VHWA_M2M_VPAC_1_MSC_DRV_INST_ID_1 != drvInstId))
        {
            retVal = FVID2_EINVALID_PARAMS;
            GT_0trace(VhwaMscTrace, GT_ERR, "Invalid/unsupported Instance Id\n");
        }
#endif
#endif
    }

    return (retVal);
}

