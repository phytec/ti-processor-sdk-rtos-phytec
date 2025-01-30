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
 *  \file vhwa_m2mVissIntr.c
 *
 *  \brief Implementation for Interrupt related APIs
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
 * \brief   ISR handler for VISS interrupt
 *          Registered to the BIOS
 *
 **/
static void vhwaM2mVissIsr(uintptr_t args);

/**
 * \brief   Frame Complete ISR,
 *          It is called from #vhwaM2mVissIsr when VISS completes a frame
 *          and HTS pipeline is done.
 *
 * \param   instObj             Instance object
 *
 **/
static void vhwaM2mVissFrmDoneIsr(Vhwa_M2mVissInstObj *instObj);

/**
 * \brief   Function to get status of the error and frame done event
 *
 * \param   instObj             Instance object
 * \param   eeStat              Output parameter with the list of
 *                              error event occurred.
 * \param   frmDoneStat         Status of the frame done event.
 *
 **/
static void vhwaM2mVissGetIntrStat(const Vhwa_M2mVissInstObj *instObj,
    uint32_t *frmDoneStat);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */



/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int32_t Vhwa_m2mVissRegisterIsr(Vhwa_M2mVissInstObj *instObj)
{
    int32_t                          status = FVID2_SOK;
    HwiP_Params                      hwiParams;

    /* Check for Null Pointer */
    GT_assert(VhwaVissTrace, (NULL != instObj));

    /* Clear out any pending interrupts */
    HwiP_clearInterrupt(instObj->irqNum);

    /* Populate the interrupt parameters */
    HwiP_Params_init(&hwiParams);
    hwiParams.arg = (uintptr_t) instObj;

    /* Register interrupt */
    instObj->intrHndl = HwiP_create(instObj->irqNum,
        &vhwaM2mVissIsr, &hwiParams);
    if (NULL == instObj->intrHndl)
    {
        GT_0trace(VhwaVissTrace, GT_ERR,
            "Interrupt Registration Failed !!\n");
        status = FVID2_EFAIL;
    }

    return (status);
}

void Vhwa_m2mVissUnregisterIsr(Vhwa_M2mVissInstObj *instObj)
{
    CSL_vpac_intd_cfgRegs           *intdRegs = NULL;

    /* Check for Null Pointer */
    GT_assert(VhwaVissTrace, (NULL != instObj));

    intdRegs    = instObj->socInfo.vpacIntdRegs;

    /* If not already done, disable HTS Interrupt */
    Vhwa_disableHtsIntr(intdRegs, instObj->vhwaIrqNum, instObj->pipeline);

    if (NULL != instObj->intrHndl)
    {
        /* Un-register interrupt */
        (void)HwiP_delete(instObj->intrHndl);
        instObj->intrHndl = NULL;
    }
}

static void vhwaM2mVissIsr(uintptr_t args)
{
    uint32_t             frmDoneStat;
    Vhwa_M2mVissInstObj *instObj = NULL;

    GT_assert(VhwaVissTrace, (NULL != (Vhwa_M2mVissInstObj *)args));

    instObj = (Vhwa_M2mVissInstObj *)args;

    instObj->totalIntrCnt ++;

    vhwaM2mVissGetIntrStat(instObj, &frmDoneStat);

    if (0U != frmDoneStat)
    {
        /* Now process the Frame Done ISR */
        vhwaM2mVissFrmDoneIsr(instObj);
    }
    else
    {
        instObj->errIntrInvalidStatus ++;
    }
}


static void vhwaM2mVissFrmDoneIsr(Vhwa_M2mVissInstObj *instObj)
{
    Vhwa_M2mVissHandleObj *doneHndlObj = NULL;
    Vhwa_M2mVissQueueObj  *qObj = NULL;

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
        GT_assert(VhwaVissTrace, (NULL != instObj->actQObj));

        qObj = instObj->actQObj;

        if (NULL != qObj)
        {
            /* Handle Object in QObject must not be null */
            GT_assert(VhwaVissTrace, (NULL != qObj->hObj));

            doneHndlObj = qObj->hObj;

            if (0u == CSL_htsPipelineStatus(instObj->regAddrs.htsRegs,
                &doneHndlObj->htsCfg))
            {
                /* Get the timestmap for performance numbers */
                if (NULL != doneHndlObj->createArgs.getTimeStamp)
                {
                    doneHndlObj->perf =
                        doneHndlObj->createArgs.getTimeStamp() -
                        doneHndlObj->perf;
                }

                /* Move completed qObject to done queue */
                Fvid2Utils_queue(doneHndlObj->doneQ,
                    &qObj->qElem, qObj);

                instObj->lastHndlObj = doneHndlObj;
                instObj->actQObj = NULL;

                /* New request can now be submitted to the VISS IP */
                (void)SemaphoreP_post(instObj->hwaLock);

                /* Call Application callback, for the completed handle object */
                if(NULL != doneHndlObj->cbPrms.fdmCbFxn)
                {
                    (void)doneHndlObj->cbPrms.fdmCbFxn(
                        doneHndlObj->cbPrms.fdmData);
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

static void vhwaM2mVissGetIntrStat(const Vhwa_M2mVissInstObj *instObj,
    uint32_t *frmDoneStat)
{
    uint32_t                        vhwaIrqNum;
    volatile uint32_t               regVal;
    volatile CSL_vpac_intd_cfgRegs *intdRegs = NULL;

    if (NULL != instObj)
    {
        vhwaIrqNum  = instObj->vhwaIrqNum;
        intdRegs    = instObj->socInfo.vpacIntdRegs;

        regVal = CSL_REG32_RD(
            &intdRegs->STATUS_REG_LEVEL_VPAC_OUT[vhwaIrqNum][3U]);

        *frmDoneStat = 0U;
        if (0U != (regVal & ((uint32_t)1u << instObj->pipeline)))
        {
            /* Clear the interrupt */
            regVal = ((uint32_t)1u << instObj->pipeline);
            CSL_REG32_WR(
                &intdRegs->STATUS_CLR_REG_LEVEL_VPAC_OUT[vhwaIrqNum][3U],
                regVal);

            *frmDoneStat = 1U;
        }
    }
}
