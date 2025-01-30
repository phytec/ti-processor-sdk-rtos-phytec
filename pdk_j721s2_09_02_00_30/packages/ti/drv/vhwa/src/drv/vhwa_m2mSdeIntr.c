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
 *  \file vhwa_m2mSdeIntr.c
 *
 *  \brief SDE Interrupt API Implementation
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/drv/vhwa/src/drv/vhwa_m2mSdePriv.h>

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
 * \brief   ISR handler for SDE interrupt
 *          Registered to the BIOS
 *
 **/
static void vhwaM2mSdeIsr(uintptr_t args);

/**
 * \brief   Frame Complete ISR,
 *          It is called from #vhwaM2mSdeIsr when SDE completes a frame
 *          and HTS pipeline is done.
 *
 * \param   instObj             Instance object
 *
 **/
static void vhwaM2mSdeFrmDoneIsr(Vhwa_M2mSdeInstObj *instObj);

/**
 * \brief   Error Event ISR,
 *          It is called from #vhwaM2mSdeIsr when SDE generates error event.
 *
 * \param   instObj             Instance object
 * \param   errStat             Bit mask of occurred error events
 *
 **/
static void vhwaM2mSdeErrEventIsr(const Vhwa_M2mSdeInstObj *instObj,
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
static void vhwaM2mSdeGetIntrStat(const Vhwa_M2mSdeInstObj *instObj,
    uint32_t *eeStat, uint32_t *frmDoneStat);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */



/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int32_t Vhwa_m2mSdeRegisterIsr(Vhwa_M2mSdeInstObj *instObj)
{
    int32_t                          status = FVID2_SOK;
    uint32_t                         events;
    uint32_t                         vhwaIrqNum;
    volatile uint32_t                regVal;
    HwiP_Params                      hwiParams;
    volatile CSL_dmpac_intd_cfgRegs  *intdRegs = NULL;

    GT_assert(VhwaSdeTrace, (NULL != instObj));

    /* Clear out any pending interrupts */
    HwiP_clearInterrupt(instObj->irqNum);

    /* Populate the interrupt parameters */
    HwiP_Params_init(&hwiParams);
    hwiParams.arg = (uintptr_t) instObj;

    /* Register interrupt */
    instObj->intrHndl = HwiP_create(instObj->irqNum,
        &vhwaM2mSdeIsr, &hwiParams);
    if (NULL == instObj->intrHndl)
    {
        GT_0trace(VhwaSdeTrace, GT_ERR,
            "Interrupt Registration Failed !!\n");
        status = FVID2_EFAIL;
    }

    /* Enable Error Events */
    if (FVID2_SOK == status)
    {
        vhwaIrqNum  = instObj->vhwaIrqNum;
        intdRegs    = instObj->socInfo.dmpacIntdRegs;

        /* All Error Events are by default enabled */
        /* TODO Enable Error Events */
        events =
            VHWA_SDE_RD_ERR | VHWA_SDE_WR_ERR |
            VHWA_SDE_FOCO0_SL2_WR_ERR | VHWA_SDE_FOCO0_VBUSM_RD_ERR;

        /* Enable HTS Interrupt in INTD Module */
        regVal = CSL_REG32_RD(
            &intdRegs->ENABLE_REG_LEVEL_DMPAC_OUT[vhwaIrqNum ][0U]);
        regVal |= events;
        CSL_REG32_WR(&intdRegs->ENABLE_REG_LEVEL_DMPAC_OUT[vhwaIrqNum ][0U],
            regVal);
    }

    return (status);
}

void Vhwa_m2mSdeUnregisterIsr(Vhwa_M2mSdeInstObj *instObj)
{
    uint32_t                         events;
    volatile uint32_t                regVal;
    uint32_t                         vhwaIrqNum;
    CSL_dmpac_intd_cfgRegs           *intdRegs = NULL;

    if (NULL != instObj)
    {
        vhwaIrqNum  = instObj->vhwaIrqNum;
        intdRegs    = instObj->socInfo.dmpacIntdRegs;

        /* If not already done, disable HTS Interrupt */
        Vhwa_disableDmpacHtsIntr(intdRegs, instObj->vhwaIrqNum, instObj->pipeline);

        /* All Error Events are by default enabled */
        events =
            VHWA_SDE_RD_ERR | VHWA_SDE_WR_ERR |
            VHWA_SDE_FOCO0_SL2_WR_ERR | VHWA_SDE_FOCO0_VBUSM_RD_ERR;

        /* Disable HTS Interrupt in INTD Module */
        regVal = CSL_REG32_RD(
            &intdRegs->ENABLE_CLR_REG_LEVEL_DMPAC_OUT[vhwaIrqNum][0U]);
        regVal |= events;
        CSL_REG32_WR(&intdRegs->ENABLE_CLR_REG_LEVEL_DMPAC_OUT[vhwaIrqNum][0U],
            regVal);

        if (NULL != instObj->intrHndl)
        {
            /* Un-register interrupt */
            (void)HwiP_delete(instObj->intrHndl);
            instObj->intrHndl = NULL;
        }
    }
}

static void vhwaM2mSdeIsr(uintptr_t args)
{
    uint32_t            errStat;
    uint32_t            frmDoneStat;
    Vhwa_M2mSdeInstObj *instObj = NULL;

    GT_assert(VhwaSdeTrace, (NULL != (Vhwa_M2mSdeInstObj *)args));

    instObj = (Vhwa_M2mSdeInstObj *)args;

    instObj->totalIntrCnt ++;

    vhwaM2mSdeGetIntrStat(instObj, &errStat, &frmDoneStat);

    if (0U != errStat)
    {
        /* First check if any of the registered error events has come,
           then first call the callback for the error events */
        vhwaM2mSdeErrEventIsr(instObj, errStat);
    }

    if (0U != frmDoneStat)
    {
        /* Now process the Frame Done ISR */
        vhwaM2mSdeFrmDoneIsr(instObj);
    }
    else
    {
        instObj->errIntrInvalidStatus ++;
    }
}


static void vhwaM2mSdeErrEventIsr(const Vhwa_M2mSdeInstObj *instObj,
    uint32_t errStat)
{
    Sde_ErrEventParams              *eePrms = NULL;
    Vhwa_M2mSdeHandleObj            *hObj = NULL;
    uint32_t                        errStatus;

    if (NULL != instObj)
    {
        /* Active Handle Object must not be null */
        GT_assert(VhwaSdeTrace, (NULL != instObj->actQObj));
        /* Handle Object in QObject must not be null */
        GT_assert(VhwaSdeTrace, (NULL != instObj->actQObj->hObj));

        hObj   = instObj->actQObj->hObj;
        eePrms = &hObj->eePrms;

        /* Check if any of the error event has occurred */
        errStatus = errStat & eePrms->errEvents;

        /* Process only if error events are enabled for this handle */
        if (0U != errStatus)
        {
            /* Call Application callback, for the completed handle object */
            if(NULL != eePrms->cbFxn)
            {
                eePrms->cbFxn((Fvid2_Handle)hObj, errStatus, eePrms->appData);
            }
        }
    }
}

static void vhwaM2mSdeFrmDoneIsr(Vhwa_M2mSdeInstObj *instObj)
{
    Vhwa_M2mSdeHandleObj *doneHndlObj = NULL;
    Vhwa_M2mSdeQueueObj  *qObj = NULL;

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
        GT_assert(VhwaSdeTrace, (NULL != instObj->actQObj));

        qObj = instObj->actQObj;

        if (NULL != qObj)
        {
            /* Handle Object in QObject must not be null */
            GT_assert(VhwaSdeTrace, (NULL != qObj->hObj));

            doneHndlObj = qObj->hObj;

            if (0u == CSL_dmpacHtsPipelineStatus(instObj->socInfo.htsRegs,
                &doneHndlObj->htsCfg))
            {
                /* Get the timestmap for performance numbers */
                if (NULL != doneHndlObj->createArgs.getTimeStamp)
                {
                    doneHndlObj->perfNum =
                        doneHndlObj->createArgs.getTimeStamp() - doneHndlObj->perfNum;
                }

                /* Move completed qObject to done queue */
                Fvid2Utils_queue(doneHndlObj->doneQ, &qObj->qElem, qObj);

                instObj->lastHndlObj = doneHndlObj;
                instObj->actQObj = NULL;

                /* New request can now be submitted to the SDE IP */
                (void)SemaphoreP_post(instObj->hwaLock);

                /* Call Application callback, for the completed handle object */
                if(NULL != doneHndlObj->cbPrms.fdmCbFxn)
                {
                    (void)doneHndlObj->cbPrms.fdmCbFxn(doneHndlObj->cbPrms.fdmData);
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

static void vhwaM2mSdeGetIntrStat(const Vhwa_M2mSdeInstObj *instObj,
    uint32_t *eeStat, uint32_t *frmDoneStat)
{
    uint32_t                        vhwaIrqNum;
    volatile uint32_t               regVal;
    volatile CSL_dmpac_intd_cfgRegs *intdRegs = NULL;

    if (NULL != instObj)
    {
        vhwaIrqNum  = instObj->vhwaIrqNum;
        intdRegs    = instObj->socInfo.dmpacIntdRegs;

        /* Check if any of the error event has occurred */
        regVal = CSL_REG32_RD(
            &intdRegs->STATUS_REG_LEVEL_DMPAC_OUT[vhwaIrqNum][0U]);

        *eeStat = 0U;
        if (0U != regVal)
        {
            /* First clear all interrupts, so that the interrupt does
             * not come again for these events */
            CSL_REG32_WR(
                &intdRegs->STATUS_CLR_REG_LEVEL_DMPAC_OUT[vhwaIrqNum][0U],
                regVal);

            /* Store the error status */
            *eeStat = regVal;
        }

        regVal = CSL_REG32_RD(
            &intdRegs->STATUS_REG_LEVEL_DMPAC_OUT[vhwaIrqNum][1U]);

        *frmDoneStat = 0U;
        if (0U != (regVal & ((uint32_t)1u << instObj->pipeline)))
        {
            /* Clear the interrupt */
            regVal = ((uint32_t)1u << instObj->pipeline);
            CSL_REG32_WR(
                &intdRegs->STATUS_CLR_REG_LEVEL_DMPAC_OUT[vhwaIrqNum][1U],
                regVal);

            *frmDoneStat = 1U;
        }
    }
}