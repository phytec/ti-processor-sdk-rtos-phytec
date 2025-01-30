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
 *  \file vhwa_m2mMscIntr.c
 *
 *  \brief Interrupt handling for MSC module
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

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
extern Vhwa_M2mMscCommonObj   gM2mMscCommonObj;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * \brief   ISR handler for MSC interrupt
 *          Registered to the BIOS for Instance 0
 *
 **/
static void vhwa_m2mMscIsr0(uintptr_t args);

/**
 * \brief   ISR handler for MSC interrupt
 *          Registered to the BIOS for Instance 1
 *
 **/
static void vhwa_m2mMscIsr1(uintptr_t args);

/**
 * \brief   Frame Complete ISR,
 *          It is called from #vhwa_m2mMscIsr0 when MSC completes a frame
 *          and HTS pipeline is done.
 *
 * \param   instObj             Instance object
 *
 **/
static void vhwa_m2mMscFrmDoneIsr(Vhwa_M2mMscInstObj *instObj,
                                    Vhwa_M2mMscCommonObj *comObj);

/**
 * \brief   Error Event ISR,
 *          It is called from #vhwa_m2mMscIsr0 when MSC generates error event.
 *
 * \param   instObj             Instance object
 * \param   errStat             Bit mask of occurred error events
 *
 **/
static void vhwa_m2mMscErrEventIsr(Vhwa_M2mMscInstObj *instObj,
                                    uint32_t errStat);

/**
 * \brief   Function to get status of the error and frame done event
 *
 * \param   instObj             Instance object
 * \param   eeStat              Output parameter with the list of
 *                              error event occurred.
 * \param   frmDoneStat         Status of the frame done event.
 *
 **/
static void vhwa_m2mMscGetIntrStat(const Vhwa_M2mMscInstObj *instObj,
                                   const Vhwa_M2mMscCommonObj *comObj,
                                    uint32_t instId,
                                    uint32_t *eeStat,
                                    uint32_t *frmDoneStat);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */



/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int32_t Vhwa_m2mMscRegisterIsr(Vhwa_M2mMscInstObj *instObj,
                               const Vhwa_M2mMscCommonObj *comObj,
                               uint32_t instId)
{
    int32_t                          status = FVID2_SOK;
    uint32_t                         events;
    uint32_t                         vhwaIrqNum;
    volatile uint32_t                regVal;
    HwiP_Params                      hwiParams;
    volatile CSL_vpac_intd_cfgRegs  *intdRegs = NULL;

    GT_assert(VhwaMscTrace, (NULL != instObj));
    GT_assert(VhwaMscTrace, (NULL != comObj));

    if ((instId != VPAC_MSC_INST_ID_0) &&
        (instId != VPAC_MSC_INST_ID_1))
    {
        status = FVID2_EBADARGS;
    }

    if (FVID2_SOK == status)
    {
        /* Clear out any pending interrupts */
        HwiP_clearInterrupt(comObj->mscInitPrms.irqInfo[instId].irqNum);

        /* Populate the interrupt parameters */
        HwiP_Params_init(&hwiParams);
        hwiParams.arg = (uintptr_t) instObj;

        if (VPAC_MSC_INST_ID_0 == instId)
        {
            /* Register interrupt */
            instObj->intrHandle = HwiP_create(
                            comObj->mscInitPrms.irqInfo[instId].irqNum,
                            &vhwa_m2mMscIsr0, &hwiParams);
            if (NULL == instObj->intrHandle)
            {
                GT_0trace(VhwaMscTrace, GT_ERR,
                    "Interrupt Registration Failed !!\n");
                status = FVID2_EFAIL;
            }
        }
        else
        {
            /* Register interrupt */
            instObj->intrHandle = HwiP_create(
                            comObj->mscInitPrms.irqInfo[instId].irqNum,
                                    &vhwa_m2mMscIsr1, &hwiParams);
            if (NULL == instObj->intrHandle)
            {
                GT_0trace(VhwaMscTrace, GT_ERR,
                    "Interrupt Registration Failed !!\n");
                status = FVID2_EFAIL;
            }
        }
    }

    /* Enable Error Events */
    if (FVID2_SOK == status)
    {
        vhwaIrqNum  = comObj->mscInitPrms.irqInfo[instId].vhwaIrqNum;
        intdRegs    = comObj->socInfo.vpacIntdRegs;

        /* All Error Events are by default enabled */
        events =
            VHWA_MSC_VBUSM_RD_ERR | VHWA_MSC_SL2_WR_ERR;

        /* Enable HTS Interrupt in INTD Module */
        regVal = CSL_REG32_RD(
            &intdRegs->ENABLE_REG_LEVEL_VPAC_OUT[vhwaIrqNum][2U]);
        regVal |= events;
        CSL_REG32_WR(&intdRegs->ENABLE_REG_LEVEL_VPAC_OUT[vhwaIrqNum][2U],
            regVal);
    }

    return (status);
}

void Vhwa_m2mMscUnregisterIsr(Vhwa_M2mMscInstObj *instObj,
                              const  Vhwa_M2mMscCommonObj *comObj,
                               uint32_t instId)
{
    uint32_t                         events;
    volatile uint32_t                regVal;
    uint32_t                         vhwaIrqNum;
    CSL_vpac_intd_cfgRegs           *intdRegs = NULL;

    if (NULL != instObj)
    {
        vhwaIrqNum  = comObj->mscInitPrms.irqInfo[instId].vhwaIrqNum;
        intdRegs    = comObj->socInfo.vpacIntdRegs;

        /* If not already done, disable HTS Interrupt */
        Vhwa_disableHtsIntr(intdRegs, vhwaIrqNum, instObj->pipeline);

        /* All Error Events are by default enabled */
        events =
            VHWA_MSC_VBUSM_RD_ERR | VHWA_MSC_SL2_WR_ERR;;

        /* Disable HTS Interrupt in INTD Module */
        regVal = CSL_REG32_RD(
            &intdRegs->ENABLE_CLR_REG_LEVEL_VPAC_OUT[vhwaIrqNum][2U]);
        regVal |= events;
        CSL_REG32_WR(&intdRegs->ENABLE_CLR_REG_LEVEL_VPAC_OUT[vhwaIrqNum][2U],
            regVal);

        if (NULL != instObj->intrHandle)
        {
            /* Un-register interrupt */
            (void)HwiP_delete(instObj->intrHandle);
            instObj->intrHandle = NULL;
        }
    }
}

static void vhwa_m2mMscIsr0(uintptr_t args)
{
    uint32_t            errStat;
    uint32_t            frmDoneStat;
    uint32_t            instId;
    Vhwa_M2mMscInstObj *instObj = NULL;
    Vhwa_M2mMscCommonObj *comObj;

    GT_assert(VhwaMscTrace, (NULL != (Vhwa_M2mMscInstObj *)args));

    instObj = (Vhwa_M2mMscInstObj *)args;
    comObj = &gM2mMscCommonObj;
    instId = VPAC_MSC_INST_ID_0;

    instObj->totalIntrCnt ++;

    vhwa_m2mMscGetIntrStat(instObj, comObj, instId, &errStat, &frmDoneStat);

    if (0U != errStat)
    {
        /* First check if any of the registered error events has come,
           then first call the callback for the error events */
        vhwa_m2mMscErrEventIsr(instObj, errStat);
    }

    if (0U != frmDoneStat)
    {
        /* Now process the Frame Done ISR */
        vhwa_m2mMscFrmDoneIsr(instObj, comObj);
    }
    else
    {
        instObj->errIntrInvalidStatus ++;
    }
}

static void vhwa_m2mMscIsr1(uintptr_t args)
{
    uint32_t            errStat;
    uint32_t            frmDoneStat;
    uint32_t            instId;
    Vhwa_M2mMscInstObj *instObj = NULL;
    Vhwa_M2mMscCommonObj *comObj;

    GT_assert(VhwaMscTrace, (NULL != (Vhwa_M2mMscInstObj *)args));

    instObj = (Vhwa_M2mMscInstObj *)args;
    comObj = &gM2mMscCommonObj;
    instId = VPAC_MSC_INST_ID_1;

    instObj->totalIntrCnt ++;

    vhwa_m2mMscGetIntrStat(instObj, comObj, instId, &errStat, &frmDoneStat);

    if (0U != errStat)
    {
        /* First check if any of the registered error events has come,
           then first call the callback for the error events */
        vhwa_m2mMscErrEventIsr(instObj, errStat);
    }

    if (0U != frmDoneStat)
    {
        /* Now process the Frame Done ISR */
        vhwa_m2mMscFrmDoneIsr(instObj, comObj);
    }
    else
    {
        instObj->errIntrInvalidStatus ++;
    }
}


static void vhwa_m2mMscErrEventIsr(Vhwa_M2mMscInstObj *instObj,
                                    uint32_t errStat)
{
    uint32_t                        errStatus;
    Msc_ErrEventParams              *eePrms = NULL;
    Vhwa_M2mMscHandleObj            *hObj = NULL;

    if (NULL != instObj)
    {
        if ((NULL == instObj->activeQObj) || (NULL == instObj->activeQObj->hObj))
        {
            instObj->errNoActQObj ++;
            if (NULL != instObj->lastHndlObj)
            {
                hObj = instObj->lastHndlObj;
                eePrms = &hObj->eePrms;
            }
            else
            {
                hObj = NULL;
                instObj->errNoLastHndlObj ++;
            }
        }
        else
        {
            hObj   = instObj->activeQObj->hObj;
            eePrms = &hObj->eePrms;
        }

        if(NULL != hObj)
        {
            /* Check if any of the error event has occurred */
            errStatus = errStat & eePrms->errEvents;

            /* Process only if error events are enabled for this handle */
            if (0U != errStatus)
            {
                /* Call Application callback, for the completed handle object */
                if (NULL != eePrms->cbFxn)
                {
                    eePrms->cbFxn((Fvid2_Handle)hObj, errStatus, eePrms->appData);
                }
            }
        }
    }
}

static void vhwa_m2mMscFrmDoneIsr(Vhwa_M2mMscInstObj *instObj,
                                    Vhwa_M2mMscCommonObj *comObj)
{
    int32_t               retVal;
    uint32_t              cnt;
    Vhwa_M2mMscHandleObj *hObj = NULL;
    Vhwa_M2mMscQueueObj  *qObj = NULL;

    if (NULL != instObj)
    {
        /**
         * Steps
         * 1, Clear the interrupt
         * 2, Move the queue object to doneQ
         * 3, Mark Active Queue Object to NULL and lastHandle to
         *    completed handle
         * 4, If the reqQ is not empty, Remove one object from reqQ
         * 5, if the last handle is same as this handle, submit the TR
         * 6, If they are not same, set the config and then submit the TR
         * 7, move the new object to actQ
         * 8, Call the application callback
         */
        /* Active Handle Object must not be null */
        GT_assert(VhwaMscTrace, (NULL != instObj->activeQObj));

        qObj = instObj->activeQObj;

        if (NULL != qObj)
        {
            /* Handle Object in QObject must not be null */
            GT_assert(VhwaMscTrace, (NULL != qObj->hObj));

            hObj = qObj->hObj;

            if (0u == CSL_htsPipelineStatus(comObj->socInfo.htsRegs,
                    &hObj->htsCfg[hObj->curIterCnt]))
            {
                /* Get the timestmap for performance numbers */
                if (NULL != hObj->createPrms.getTimeStamp)
                {
                    hObj->perf[hObj->curIterCnt] =
                        hObj->createPrms.getTimeStamp() -
                        hObj->perf[hObj->curIterCnt];
                }

                /**
                 * Steps
                 * 1, Clear the interrupt
                 * 2, Move the queue object to doneQ
                 * 3, Mark Active Queue Object to NULL and lastHandle to
                 *    completed handle
                 * 4, If the reqQ is not empty, Remove one object from reqQ
                 * 5, if the last handle is same as this handle, submit the TR
                 * 6, If they are not same, set the config and then
                 *    submit the TR
                 * 7, move the new object to actQ
                 * 8, Call the application callback
                 */

                hObj->curIterCnt++;
                if (hObj->curIterCnt < hObj->numIter)
                {
                    /* TODO Add seperate submit function for chroma */
                    /* Process Chroma */
                    retVal = Vhwa_m2mMscSetFrameSize(qObj, comObj,
                                                     hObj->curIterCnt);

                    if (FVID2_SOK == retVal)
                    {
                        retVal = Vhwa_m2mMscSubmitRequest(instObj, qObj, comObj,
                                                          hObj->curIterCnt);
                    }
                    if (FVID2_SOK != retVal)
                    {
                        GT_0trace(VhwaMscTrace, GT_ERR,
                            "ISR0 Submiit request failed\n");
                    }
                }
                else
                {
                    hObj->curIterCnt = 0;

                    /* Move completed qObject to done queue */
                    Fvid2Utils_queue(hObj->outQ, &qObj->qElem, qObj);

                    instObj->lastHndlObj = hObj;
                    instObj->activeQObj = NULL;

                    /* Free the output scalar */
                    for (cnt = 0U; cnt < MSC_MAX_OUTPUT; cnt ++)
                    {
                        if (0u != (hObj->scalarUsed & ((uint32_t)1u << cnt)))
                        {
                            (void)SemaphoreP_post(comObj->scLockSem[cnt]);
                        }
                    }

                    /* New request can now be submitted to the MSC IP */
                    (void)SemaphoreP_post(instObj->hwaLock);

                    /* Call Application callback, for the completed handle object */
                    if (NULL != hObj->fdmCbPrms.fdmCbFxn)
                    {
                        (void)hObj->fdmCbPrms.fdmCbFxn(hObj->fdmCbPrms.fdmData);
                    }

                    CSL_lseStopChannels(comObj->socInfo.lseRegs, &hObj->lseCfg[0]);

                    CSL_htsThreadStop(comObj->socInfo.htsRegs, &hObj->htsCfg[0]);
                }
            }
            else
            {
                instObj->errInvalidHtsPipelineStatus ++;
            }
        }
        else
        {
            instObj->errIntrRepeateIntr ++;
        }
    }
}

static void vhwa_m2mMscGetIntrStat(const Vhwa_M2mMscInstObj *instObj,
                                   const Vhwa_M2mMscCommonObj *comObj,
                                    uint32_t instId,
                                    uint32_t *eeStat,
                                    uint32_t *frmDoneStat)
{
    uint32_t                        vhwaIrqNum;
    volatile uint32_t               regVal;
    volatile CSL_vpac_intd_cfgRegs *intdRegs = NULL;

    if (NULL != instObj)
    {
        vhwaIrqNum  = comObj->mscInitPrms.irqInfo[instId].vhwaIrqNum;
        intdRegs    = comObj->socInfo.vpacIntdRegs;

        /* Check if any of the error event has occurred */
        regVal = CSL_REG32_RD(
            &intdRegs->STATUS_REG_LEVEL_VPAC_OUT[vhwaIrqNum][2U]);

        *eeStat = 0U;
        if (0U != regVal)
        {
            /* First clear all interrupts, so that the interrupt does
             * not come again for these events */
            CSL_REG32_WR(
                &intdRegs->STATUS_CLR_REG_LEVEL_VPAC_OUT[vhwaIrqNum][2U],
                regVal);

            /* Store the error status */
            *eeStat = regVal;
        }

        regVal = CSL_REG32_RD(
            &intdRegs->STATUS_REG_LEVEL_VPAC_OUT[vhwaIrqNum][3U]);

        *frmDoneStat = 0U;
        if (0U != (regVal & ((uint32_t)1u << instObj->pipeline)))
        {
            /* Clear the interrupt */
            regVal = (uint32_t)1u << instObj->pipeline;
            CSL_REG32_WR(
                &intdRegs->STATUS_CLR_REG_LEVEL_VPAC_OUT[vhwaIrqNum][3U],
                regVal);

            *frmDoneStat = 1U;
        }
    }
}