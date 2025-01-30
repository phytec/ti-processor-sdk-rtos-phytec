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
 *
 */

/**
 *  \file vhwa_m2mVissUdma.c
 *
 *  \brief Utility APIs for UDMA configuration
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
 * \brief   Local Function to initialize TR header for the
 *          given channel object. It initializes, number of TRs,
 *          completion Ring etc in the TR header.
 *
 * \param   instObj             Instance object.
 * \param   hObj                Handle Object
 * \param   chPrms              Channel Parameters, from which TR start
 *                              address, number of TR etc information is used
 * \param   complRing           Return ring to be set in the TR header
 *
 **/
static void vhwaM2mVissSetupPacketInfo(const Vhwa_M2mVissChParams *chPrms,
    uint32_t complRin);

/**
 * \brief   Local Function to initialize transfer recoder for all
 *          TRs of the given channel object. It traverse through all
 *          enabled regions, uses the icnts, dims calculated parameters
 *          channels params and region params and initializes TR
 *
 * \param   chPrms              Channel Parameters, from which TR start
 *                              address, number of TR etc information is used
 * \param   pTR                 Start address of the TR for this channel
 *
 **/
static void vhwaM2mVissSetupRxTransferRecord(const Vhwa_M2mVissChParams *chPrms,
                                            CSL_UdmapTR9 *pTr);

static void vhwaM2mVissSetupTxTransferRecord(const Vhwa_M2mVissChParams *chPrms,
                                            CSL_UdmapTR9 *pTr);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/**
 * \brief Ring memory, one ring for each output channel.
 */
uint8_t gVissRingMem[VHWA_M2M_VISS_MAX_DMA_CH]
    [VHWA_M2M_VISS_UDMA_RING_MEM_SIZE]
    __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));

/**
 * \brief Completion Ring memory, one ring for each output channel.
 */
uint8_t gVissCompRingMem[VHWA_M2M_VISS_MAX_DMA_CH]
    [VHWA_M2M_VISS_UDMA_RING_MEM_SIZE]
    __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));

/**
 * \brief Teardown Completion Ring memory, one ring for each output channel.
 */
int8_t gVissTdCompRingMem[VHWA_M2M_VISS_MAX_DMA_CH]
    [VHWA_M2M_VISS_UDMA_RING_MEM_SIZE]
    __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));

/**
 * \brief TRPD memory for each handle and for each channel in handle
 */
uint8_t gVissRxTprdMem[VHWA_M2M_VISS_MAX_HANDLES]
    [VHWA_M2M_VISS_MAX_DMA_CH][VHWA_M2M_VISS_UDMA_TRPD_SIZE]
    __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));


/**
 * Global variables for configuration channel.
 */
/**
 * \brief Ring memory, one ring for each output channel.
 */
uint8_t gConfigVissRingMem[VHWA_M2M_VISS_MAX_CONFIG_DMA_CH]
    [VHWA_M2M_VISS_UDMA_RING_MEM_SIZE]
    __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));

/**
 * \brief Completion Ring memory, one ring for each output channel.
 */
uint8_t gConfigVissCompRingMem[VHWA_M2M_VISS_MAX_CONFIG_DMA_CH]
    [VHWA_M2M_VISS_UDMA_RING_MEM_SIZE]
    __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));

/**
 * \brief Teardown Completion Ring memory, one ring for each output channel.
 */
int8_t gConfigVissTdCompRingMem[VHWA_M2M_VISS_MAX_CONFIG_DMA_CH]
    [VHWA_M2M_VISS_UDMA_RING_MEM_SIZE]
    __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));

/**
 * \brief TRPD memory for each handle and for each channel in handle
 */
uint8_t gConfigVissRxTprdMem[VHWA_M2M_VISS_MAX_HANDLES]
    [VHWA_M2M_VISS_MAX_CONFIG_DMA_CH][VHWA_M2M_VISS_UDMA_CONFIG_TRPD_SIZE]
    __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/* ========================================================================== */
/* Below set of functions uses UDMA LLD to allocate, configure, start and     */
/* stop channels.                                                             */
/* ========================================================================== */

/* Function to initialize and allocate UTC/UDMA(External) Channels */
int32_t Vhwa_m2mVissUdmaInit(Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissInitParams *initPrms)
{
    int32_t                 status;
    uint32_t                cnt;
    uint32_t                chType;
    Udma_ChHandle           chHndl;
    Udma_ChPrms             chPrms;
    Udma_ChUtcPrms          utcPrms;

    /* Check for Null pointer,
     * Internal function so checking with assert */
    GT_assert(VhwaVissTrace, (NULL != instObj));
    GT_assert(VhwaVissTrace, (NULL != initPrms));

    for (cnt = 0U; cnt < VHWA_M2M_VISS_MAX_DMA_CH; cnt ++)
    {
        chHndl = &instObj->utcChObj[cnt];
        instObj->utcChHndl[cnt] = chHndl;

        /* Initialize channel parameters */
        chType = UDMA_CH_TYPE_UTC;
        UdmaChPrms_init(&chPrms, chType);

        chPrms.chNum                = VHWA_VISS_UTC_CH_START + cnt;
        chPrms.utcId                = VHWA_M2M_VISS_UTC_ID;
        chPrms.fqRingPrms.ringMem   = &gVissRingMem[cnt][0U];
        chPrms.cqRingPrms.ringMem   = &gVissCompRingMem[cnt][0];
        chPrms.tdCqRingPrms.ringMem = &gVissTdCompRingMem[cnt][0U];
        chPrms.fqRingPrms.elemCnt   = VHWA_M2M_VISS_UDMA_RING_ENTRIES;
        chPrms.cqRingPrms.elemCnt   = VHWA_M2M_VISS_UDMA_RING_ENTRIES;
        chPrms.tdCqRingPrms.elemCnt = VHWA_M2M_VISS_UDMA_RING_ENTRIES;

        /* Open channel,
         * since chNum is set to ANY in chPrms, it also allocates
         * an external channel and completion channel */
        status = Udma_chOpen(initPrms->udmaDrvHndl, chHndl,
            chType, &chPrms);
        if(UDMA_SOK != status)
        {
            GT_0trace(VhwaVissTrace, GT_ERR, "UDMA channel open failed!!\n");
        }

        if(UDMA_SOK == status)
        {
            /* Intialize UTC channel Paramenters */
            UdmaChUtcPrms_init(&utcPrms);

            utcPrms.chanType = (uint8_t)CSL_UDMAP_CHAN_TYPE_REF_TR_RING;

            utcPrms.busOrderId = 0;
            utcPrms.busPriority = 2;

            /* Configure UTC channel,
             * used to configure other channel specific parameters like
             * priority, orderid, queue id etc */
            status = Udma_chConfigUtc(chHndl, &utcPrms);
            if(UDMA_SOK != status)
            {
                GT_0trace(VhwaVissTrace, GT_ERR,
                            "UDMA UTC channel config failed!!\n");
            }
        }

        if(UDMA_SOK != status)
        {
            break;
        }

        if (cnt < VHWA_M2M_VISS_MAX_INPUTS)
        {
            /* Locally store completion queue number, forward ring handle
             * and completion ring handle */
            instObj->inCqRingNum[cnt] = Udma_chGetCqRingNum(chHndl);
            instObj->inFqRingHndl[cnt] = Udma_chGetFqRingHandle(chHndl);
            instObj->inCqRingHndl[cnt] = Udma_chGetCqRingHandle(chHndl);
            instObj->inUtcCh[cnt] = Udma_chGetNum(chHndl);
        }
        else
        {
            /* Locally store completion queue number, forward ring handle
             * and completion ring handle */
            instObj->outCqRingNum[cnt - VHWA_M2M_VISS_MAX_INPUTS] =
                Udma_chGetCqRingNum(chHndl);
            instObj->outFqRingHndl[cnt - VHWA_M2M_VISS_MAX_INPUTS] =
                Udma_chGetFqRingHandle(chHndl);
            instObj->outCqRingHndl[cnt - VHWA_M2M_VISS_MAX_INPUTS] =
                Udma_chGetCqRingHandle(chHndl);
            instObj->outUtcCh[cnt - VHWA_M2M_VISS_MAX_INPUTS] =
                Udma_chGetNum(chHndl);
        }
    }

    /*
     * If configuration through UDMA enabled,
     * create UDMA channel to write configuration
     */
    if (initPrms->configThroughUdmaFlag == (bool)UTRUE)
    {
        /* completion queue event params */
        Udma_EventPrms cqEventPrms;
        Udma_EventHandle cqEventHandle;
        SemaphoreP_Params semPrms;
        Udma_ChTxPrms txPrms;
        Udma_ChRxPrms rxPrms;
        /*
         * create channel for UDMA to handle configuration,
         * add corresponding structures in instance object
         * since the UDMA channel will be common for all the VISS handles.
         */
        chHndl = &instObj->configChObj;
        instObj->configChHandle = chHndl;

        /* Initialize channel parameters */
        chType = UDMA_CH_TYPE_TR_BLK_COPY;
        UdmaChPrms_init(&chPrms, chType);

        chPrms.fqRingPrms.ringMem = &gConfigVissRingMem[0U][0U];
        chPrms.cqRingPrms.ringMem = &gConfigVissCompRingMem[0U][0];
        chPrms.tdCqRingPrms.ringMem = &gConfigVissTdCompRingMem[0U][0U];
        chPrms.fqRingPrms.elemCnt = VHWA_M2M_VISS_UDMA_RING_ENTRIES;
        chPrms.cqRingPrms.elemCnt = VHWA_M2M_VISS_UDMA_RING_ENTRIES;
        chPrms.tdCqRingPrms.elemCnt = VHWA_M2M_VISS_UDMA_RING_ENTRIES;

        /* Open channel */
        status = Udma_chOpen(initPrms->udmaDrvHndl, chHndl, chType, &chPrms);
        if (UDMA_SOK != status)
        {
            GT_0trace(VhwaVissTrace, GT_ERR,
                    "UDMA channel for configuration open failed!!\n");
        }

        if (UDMA_SOK == status)
        {
            /* Config TX channel */
            UdmaChTxPrms_init(&txPrms, chType);
            status = Udma_chConfigTx(chHndl, &txPrms);
            if (UDMA_SOK != status)
            {
                GT_0trace(VhwaVissTrace, GT_ERR,
                        "UDMA TX channel config failed!!\n");
            }
        }

        if (UDMA_SOK == status)
        {
            /* Config RX channel - which is implicitly paired to TX channel in
             * block copy mode */
            UdmaChRxPrms_init(&rxPrms, chType);
            status = Udma_chConfigRx(chHndl, &rxPrms);
            if (UDMA_SOK != status)
            {
                GT_0trace(VhwaVissTrace, GT_ERR,
                        "UDMA RX channel config failed!!\n");
            }
        }

        SemaphoreP_Params_init(&semPrms);
        instObj->configCqEventSemaphore = SemaphoreP_create(0, &semPrms);
        if (NULL == instObj->configCqEventSemaphore)
        {
            GT_0trace(VhwaVissTrace, GT_ERR,
                    "Failed to allocate completion event semaphore!!\n");
            status = FVID2_EALLOC;
        }

        /* Register ring completion call back function */
        if (UDMA_SOK == status)
        {
            cqEventHandle = &instObj->configUdmaCqEventObj;
            UdmaEventPrms_init(&cqEventPrms);
            cqEventPrms.eventType = UDMA_EVENT_TYPE_DMA_COMPLETION;
            cqEventPrms.eventMode = UDMA_EVENT_MODE_SHARED;
            cqEventPrms.chHandle = chHndl;
            cqEventPrms.masterEventHandle =
                    Udma_eventGetGlobalHandle(initPrms->udmaDrvHndl);
            cqEventPrms.eventCb = &Vhwa_m2mVissConfgUdmaEvenCb;
            status = Udma_eventRegister(initPrms->udmaDrvHndl, cqEventHandle,
                    &cqEventPrms);
            if (UDMA_SOK != status)
            {
                GT_0trace(VhwaVissTrace, GT_ERR,
                        "UDMA CQ event register failed!!\n");
            }
        }

        if (UDMA_SOK == status)
        {
            /* Locally store completion queue number, forward ring handle
             * and completion ring handle */
            instObj->configCqRingNum = Udma_chGetCqRingNum(chHndl);
            instObj->configFqRingHndl = Udma_chGetFqRingHandle(chHndl);
            instObj->configCqRingHndl = Udma_chGetCqRingHandle(chHndl);
            instObj->ConfigcCh = Udma_chGetNum(chHndl);
        }
    }

    /* Convert UDMA status to FVID2 status,
       so that the caller of this function always uses FVID2 status only */
    if(UDMA_SOK == status)
    {
        status = FVID2_SOK;
    }
    else
    {
        status = FVID2_EFAIL;
    }

    return (status);
}

int32_t Vhwa_m2mVissUdmaDeInit(const Vhwa_M2mVissInstObj *instObj)
{
    int32_t             status = FVID2_SOK;
    uint32_t            cnt;
    Udma_ChHandle       chHndl;

    /* Check for Null pointer */
    GT_assert(VhwaVissTrace, (NULL != instObj));

    for (cnt = 0U; cnt < VHWA_M2M_VISS_MAX_DMA_CH; cnt ++)
    {
        chHndl = instObj->utcChHndl[cnt];

        /* Close UDMA Channel */
        status = Udma_chClose(chHndl);
        if(UDMA_SOK != status)
        {
            GT_0trace(VhwaVissTrace, GT_ERR,
                        "[Error] UDMA channel close failed!!\n");
        }
    }

    /*
     * If configuration through UDMA enabled,
     * create UDMA channel to write configuration
     */
    if (instObj->initPrms.configThroughUdmaFlag ==(bool)UTRUE)
    {
        chHndl = instObj->configChHandle;

        /* Close config UDMA channel */
        status = Udma_chClose(chHndl);
        if (UDMA_SOK != status)
        {
            GT_0trace(VhwaVissTrace, GT_ERR,
                    "[Error] Config UDMA channel close failed!!\n");
        }
    }

    /* Convert UDMA status to FVID2 status,
     so that the caller of this function always uses FVID2 status only */
    if (UDMA_SOK == status)
    {
        status = FVID2_SOK;
    }
    else
    {
        status = FVID2_EFAIL;
    }

    return (status);
}

int32_t Vhwa_m2mVissStartConfigCh(const Vhwa_M2mVissInstObj *instObj)
{
    int32_t         status = FVID2_EBADARGS;
    Udma_ChHandle   chHndl;

    /* Check for Null pointer */
    GT_assert(VhwaVissTrace, (NULL != instObj));

    /* enable the configuration UDMA channel */
    if (instObj->initPrms.configThroughUdmaFlag == true)
    {
        /* enable UDMA channel to write configuration */
        chHndl = instObj->configChHandle;

        /* UDMA Channel enable */
        status = Udma_chEnable(chHndl);

        if (UDMA_SOK != status)
        {
            GT_0trace(VhwaVissTrace, GT_ERR,
                    "UDMA configuration channel enable failed!!\n");
            status = FVID2_EFAIL;
        }
    }

    /* Convert UDMA status to FVID2 status,
     so that the caller of this function always uses FVID2 status only */
    if (UDMA_SOK == status)
    {
        status = FVID2_SOK;
    }
    else
    {
        status = FVID2_EFAIL;
    }

    return (status);

}

int32_t Vhwa_m2mVissStartCh(const Vhwa_M2mVissInstObj *instObj)
{
    int32_t         status = FVID2_EBADARGS;
    uint32_t        cnt;
    Udma_ChHandle   chHndl;

    /* Check for Null pointer */
    GT_assert(VhwaVissTrace, (NULL != instObj));

    /* By default, all channels are enabled */
    for (cnt = 0u; cnt < VHWA_M2M_VISS_MAX_DMA_CH; cnt ++)
    {
        chHndl = instObj->utcChHndl[cnt];

        /* UDMA Channel enable */
        status = Udma_chEnable(chHndl);

        if (UDMA_SOK != status)
        {
            GT_0trace(VhwaVissTrace, GT_ERR,
                "UDMA channel enable failed!!\n");
            status = FVID2_EFAIL;
            break;
        }
    }

    /* Convert UDMA status to FVID2 status,
       so that the caller of this function always uses FVID2 status only */
    if(UDMA_SOK == status)
    {
        status = FVID2_SOK;
    }
    else
    {
        status = FVID2_EFAIL;
    }

    return (status);
}

int32_t Vhwa_m2mVissStopCh(const Vhwa_M2mVissInstObj *instObj)
{
    int32_t         status = FVID2_EBADARGS;
    uint32_t        cnt;
    Udma_ChHandle   chHndl;

    /* Check for Null pointer */
    GT_assert(VhwaVissTrace, (NULL != instObj));

    /* By default, all channels are disabled, on the last handle close */
    for (cnt = 0u; cnt < VHWA_M2M_VISS_MAX_DMA_CH; cnt ++)
    {
        chHndl = instObj->utcChHndl[cnt];

        /* UDMA Channel disable */
        status = Udma_chDisable(chHndl, UDMA_DEFAULT_CH_DISABLE_TIMEOUT);
        if(UDMA_SOK != status)
        {
            GT_0trace(VhwaVissTrace, GT_ERR,
                "UDMA channel disable failed!!\n");
            status = FVID2_EFAIL;
            break;
        }
    }

    /* Disable the configuration UDMA channel */
    if (instObj->initPrms.configThroughUdmaFlag == true)
    {
        /* enable UDMA channel to write configuration */
        chHndl = instObj->configChHandle;

        /* UDMA Channel disable */
        status = Udma_chDisable(chHndl, UDMA_DEFAULT_CH_DISABLE_TIMEOUT);
        if (UDMA_SOK != status)
        {
            GT_0trace(VhwaVissTrace, GT_ERR,
                    "UDMA configuration channel disable failed!!\n");
            status = FVID2_EFAIL;
        }
    }

    /* Convert UDMA status to FVID2 status,
       so that the caller of this function always uses FVID2 status only */
    if(UDMA_SOK == status)
    {
        status = FVID2_SOK;
    }
    else
    {
        status = FVID2_EFAIL;
    }

    return (status);
}

int32_t Vhwa_m2mVissSubmitRing(Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissHandleObj *hObj)
{
    int32_t               status = UDMA_SOK;
    uint32_t              chCnt;
    Vhwa_M2mVissChParams *chPrms = NULL;

    /* Check for Null pointer */
    GT_assert(VhwaVissTrace, (NULL != instObj));
    GT_assert(VhwaVissTrace, (NULL != hObj));

    /* Submit Request for input channel */
    for (chCnt = 0U; chCnt < VHWA_M2M_VISS_MAX_IN_DMA_CH; chCnt ++)
    {
        chPrms = &hObj->inChPrms[chCnt];
        if ((uint32_t)UTRUE == chPrms->isEnabled)
        {
            /* Submit TRPD to ring */
            status = Udma_ringQueueRaw(instObj->inFqRingHndl[chCnt],
                (uint64_t) chPrms->trMem);

            if(UDMA_SOK != status)
            {
                GT_0trace(VhwaVissTrace, GT_ERR, "Ring queue failed\n");
                break;
            }
        }
    }

    /* Submit Request for output channel */
    if (UDMA_SOK == status)
    {
        for (chCnt = 0U; chCnt < VHWA_M2M_VISS_MAX_OUT_DMA_CH; chCnt ++)
        {
            chPrms = &hObj->outChPrms[chCnt];
            if (((UFALSE == hObj->fcStatus.isFlexConnect) &&
                 ((uint32_t)UTRUE == chPrms->isEnabled)) ||
                ((UTRUE == hObj->fcStatus.isFlexConnect) &&
                 (UTRUE == hObj->fcStatus.outDmaEnable[chCnt]) &&
                 ((uint32_t)UTRUE == chPrms->isEnabled)))
            {
                /* Submit TRPD to ring */
                status = Udma_ringQueueRaw(instObj->outFqRingHndl[chCnt],
                    (uint64_t) chPrms->trMem);

                if(UDMA_SOK != status)
                {
                    GT_0trace(VhwaVissTrace, GT_ERR, "Ring queue failed\n");
                    break;
                }
            }
        }
    }

    if (UDMA_SOK == status)
    {
        status = FVID2_SOK;
    }
    else
    {
        status = FVID2_EFAIL;
    }

    return (status);
}


int32_t Vhwa_m2mVissPopRings(Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissHandleObj *hObj)
{
    int32_t               status = FVID2_SOK;
    uint32_t              chCnt, repeatCnt;
    uint64_t              ringPopVal;
    Vhwa_M2mVissChParams *chPrms = NULL;
    Vhwa_M2mVissChParams *chPrms2 = NULL;
    Vhwa_M2mVissHandleObj *hObj2 = NULL;

    /* Check for Null pointer */
    GT_assert(VhwaVissTrace, (NULL != instObj));
    GT_assert(VhwaVissTrace, (NULL != hObj));

    /* Enable the channel which are required */
    for (chCnt = 0u; chCnt < VHWA_M2M_VISS_MAX_IN_DMA_CH; chCnt ++)
    {
        repeatCnt = 0u;
        chPrms = &hObj->inChPrms[chCnt];
        if ((uint32_t)UTRUE == chPrms->isEnabled)
        {
            do
            {
                status = Udma_ringDequeueRaw(instObj->inCqRingHndl[chCnt],
                        &ringPopVal);
                if(UDMA_SOK == status)
                {
                    break;
                }
                repeatCnt ++;
            } while (repeatCnt < VHWA_M2M_VISS_MAX_WAIT_LOOP_CNT);

            if ((repeatCnt < VHWA_M2M_VISS_MAX_WAIT_LOOP_CNT) &&
                ((uint32_t)ringPopVal == (uint32_t)chPrms->trMem))
            {
                status = FVID2_SOK;
            }
            else if ((repeatCnt < VHWA_M2M_VISS_MAX_WAIT_LOOP_CNT) &&
                ((uint32_t)ringPopVal != (uint32_t)chPrms->trMem))
            {
                int hcnt = 0U;
                for (hcnt = 0U; hcnt < VHWA_M2M_VISS_MAX_HANDLES; hcnt ++)
                {
                    hObj2 = Vhwa_m2mVissGetHandleObj(hcnt);
                    chPrms2 = &hObj2->inChPrms[chCnt];

                    if (((uint32_t)UTRUE == hObj2->isUsed) && 
                    ((uint32_t)ringPopVal == (uint32_t)chPrms2->trMem))
                    {
                        status = FVID2_SOK;
                        break;
                    }
                }
                if(hcnt >= VHWA_M2M_VISS_MAX_HANDLES)
                {
                    status = FVID2_EFAIL;
                    GT_1trace(VhwaVissTrace, GT_ERR, "UDMA Input Ring dequeue mismatch for ch%d!!\n", chCnt);
                }
            }
            else
            {
                if (repeatCnt >= VHWA_M2M_VISS_MAX_WAIT_LOOP_CNT)
                {
                    status = FVID2_EFAIL;

                    GT_1trace(VhwaVissTrace, GT_ERR, "UDMA Input Ring dequeue failed for ch%d!!\n", chCnt);
                }
            }
        }
    }

    if (FVID2_SOK == status)
    {
        /* Enable the channel which are required */
        for (chCnt = 0u; chCnt < VHWA_M2M_VISS_MAX_OUT_DMA_CH; chCnt ++)
        {
            chPrms = &hObj->outChPrms[chCnt];
            if ((uint32_t)UTRUE == chPrms->isEnabled)
            {
                repeatCnt = 0u;

                do
                {
                    status = Udma_ringDequeueRaw(instObj->outCqRingHndl[chCnt],
                                &ringPopVal);
                    if(UDMA_SOK == status)
                    {
                        break;
                    }
                    repeatCnt ++;
                } while (repeatCnt < VHWA_M2M_VISS_MAX_WAIT_LOOP_CNT);

                if ((repeatCnt < VHWA_M2M_VISS_MAX_WAIT_LOOP_CNT) &&
                    ((uint32_t)ringPopVal == (uint32_t)chPrms->trMem))
                {
                    status = FVID2_SOK;
                }
                else if ((repeatCnt < VHWA_M2M_VISS_MAX_WAIT_LOOP_CNT) &&
                    ((uint32_t)ringPopVal != (uint32_t)chPrms->trMem))
                {
                    int hcnt = 0U;
                    for (hcnt = 0U; hcnt < VHWA_M2M_VISS_MAX_HANDLES; hcnt ++)
                    {
                        hObj2 = Vhwa_m2mVissGetHandleObj(hcnt);
                        chPrms2 = &hObj2->outChPrms[chCnt];

                        if (((uint32_t)UTRUE == hObj2->isUsed) && 
                        ((uint32_t)ringPopVal == (uint32_t)chPrms2->trMem))
                        {
                            status = FVID2_SOK;
                            break;
                        }
                    }
                    if(hcnt >= VHWA_M2M_VISS_MAX_HANDLES)
                    {
                        status = FVID2_EFAIL;
                        GT_1trace(VhwaVissTrace, GT_ERR, "UDMA Output Ring dequeue mismatch for ch%d!!\n", chCnt);
                    }
                }
                else
                {
                    if (repeatCnt >= VHWA_M2M_VISS_MAX_WAIT_LOOP_CNT)
                    {
                        status = FVID2_EFAIL;

                        GT_1trace(VhwaVissTrace, GT_ERR, "UDMA Output Ring dequeue failed for ch%d!!\n", chCnt);
                    }
                }
            }
        }
    }

    return (status);
}

int32_t Vhwa_m2mVissAllocConfigUdmaMem(Vhwa_M2mVissInstObj *instObj)
{
    int32_t status = FVID2_SOK;

    /* If configuration through UDMA enabled, create UDMA memory for configuration channel */
    if (instObj->initPrms.configThroughUdmaFlag == (bool)UTRUE)
    {
        /*
         * Single TR will be submitted to ring queue. TR of each buffer object will
         * be linked together to form a linked list.
         */
        instObj->configTxTrMem = &gConfigVissRxTprdMem[0U][0U][0U];
        Fvid2Utils_memset(instObj->configTxTrMem, 0x0U,
                VHWA_M2M_VISS_UDMA_CONFIG_TRPD_SIZE);
    }

    return (status);
}

int32_t Vhwa_m2mVissAllocUdmaMem(Vhwa_M2mVissInstObj *instObj,
	    Vhwa_M2mVissHandleObj *hObj)
{
    int32_t               status = FVID2_SOK;
    uint32_t              hIdx;
    uint32_t              chCnt = 0U;
    uint32_t              packetInfoSize;
    Vhwa_M2mVissChParams *chPrms = NULL;

    /* Check for Null pointer */
    GT_assert(VhwaVissTrace, (NULL != instObj));
    GT_assert(VhwaVissTrace, (NULL != hObj));

    hIdx           = hObj->hIdx;
    packetInfoSize = sizeof(CSL_UdmapTR15);

    for (chCnt = 0u; chCnt < VHWA_M2M_VISS_MAX_DMA_CH; chCnt ++)
    {
        if (chCnt < VHWA_M2M_VISS_MAX_IN_DMA_CH)
        {
            chPrms = &hObj->inChPrms[chCnt];
        }
        else
        {
            chPrms = &hObj->outChPrms[chCnt - VHWA_M2M_VISS_MAX_IN_DMA_CH];
        }

        chPrms->trMem = &gVissRxTprdMem[hIdx][chCnt][0U];

        Fvid2Utils_memset(chPrms->trMem, 0x0U,
            VHWA_M2M_VISS_UDMA_TRPD_SIZE);

        /* Only 1 TR is required for each channel with one header */
        chPrms->trRespMem =
            (uint32_t *)((uint32_t)chPrms->trMem +
                (packetInfoSize * (VHWA_M2M_VISS_UDMA_NUM_TR_DESC + 1U)));
    }

    return (status);
}


void Vhwa_m2mVissSetTrDesc(const Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissHandleObj *hObj)
{
    uint32_t               chCnt;
    uint8_t               *pTrMem;
    CSL_UdmapTR9          *pTr9;
    uint32_t               packetInfoSize;
    Vhwa_M2mVissChParams   *chPrms;

    packetInfoSize = sizeof(CSL_UdmapTR15);

    /* Check for Null pointer */
    GT_assert(VhwaVissTrace, (NULL != instObj));
    GT_assert(VhwaVissTrace, (NULL != hObj));

    for (chCnt = 0U; chCnt < VHWA_M2M_VISS_MAX_IN_DMA_CH; chCnt ++)
    {
        chPrms = &hObj->inChPrms[chCnt];
        if ((uint32_t)UTRUE == chPrms->isEnabled)
        {
            pTrMem = chPrms->trMem;
            pTr9 = (CSL_UdmapTR9 *)((uint32_t)pTrMem + packetInfoSize);

            /* Invalidate cache before changing contents */
            CacheP_Inv(pTrMem, VHWA_M2M_VISS_UDMA_TRPD_SIZE);

            /* Setup Packet Info for the Tx Channel */
            vhwaM2mVissSetupPacketInfo(chPrms, instObj->inCqRingNum[chCnt]);

            /* Setup Transfer Record */
            vhwaM2mVissSetupTxTransferRecord(chPrms, pTr9);

            /* Write back contents into memory after changing */
            CacheP_wb(pTrMem, VHWA_M2M_VISS_UDMA_TRPD_SIZE);
        }
    }
    for (chCnt = 0U; chCnt < VHWA_M2M_VISS_MAX_OUT_DMA_CH; chCnt ++)
    {
        chPrms = &hObj->outChPrms[chCnt];
        if ((uint32_t)UTRUE == chPrms->isEnabled)
        {
            pTrMem = chPrms->trMem;
            pTr9 = (CSL_UdmapTR9 *)((uint32_t)pTrMem + packetInfoSize);

            /* Invalidate cache before changing contents */
            CacheP_Inv(pTrMem, VHWA_M2M_VISS_UDMA_TRPD_SIZE);

            /* Setup Packet Info for the Tx Channel */
            vhwaM2mVissSetupPacketInfo(chPrms, instObj->outCqRingNum[chCnt]);

            /* Setup Transfer Record */
            vhwaM2mVissSetupRxTransferRecord(chPrms, pTr9);

            /* Write back contents into memory after changing */
            CacheP_wb(pTrMem, VHWA_M2M_VISS_UDMA_TRPD_SIZE);
        }
    }
}

void Vhwa_m2mVissSetH3aTrDesc(const Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissHandleObj *hObj)
{
    uint8_t               *pTrMem;
    CSL_UdmapTR9          *pTr9;
    uint32_t               packetInfoSize;
    Vhwa_M2mVissChParams   *chPrms;

    packetInfoSize = sizeof(CSL_UdmapTR15);

    /* Check for Null pointer */
    GT_assert(VhwaVissTrace, (NULL != instObj));
    GT_assert(VhwaVissTrace, (NULL != hObj));

    chPrms = &hObj->outChPrms[VHWA_M2M_VISS_OUT_H3A_IDX];
    if ((uint32_t)UTRUE == chPrms->isEnabled)
    {
        pTrMem = chPrms->trMem;
        pTr9 = (CSL_UdmapTR9 *)((uint32_t)pTrMem + packetInfoSize);

        /* Invalidate cache before changing contents */
        CacheP_Inv(pTrMem, VHWA_M2M_VISS_UDMA_TRPD_SIZE);

        /* Setup Packet Info for the Tx Channel */
        vhwaM2mVissSetupPacketInfo(chPrms,
                        instObj->outCqRingNum[VHWA_M2M_VISS_OUT_H3A_IDX]);

        /* Setup Transfer Record */
        vhwaM2mVissSetupRxTransferRecord(chPrms, pTr9);

        /* Write back contents into memory after changing */
        CacheP_wb(pTrMem, VHWA_M2M_VISS_UDMA_TRPD_SIZE);
    }
}

void Vhwa_m2mVissSetAddress(Vhwa_M2mVissHandleObj *hObj)
{
    uint32_t              chCnt;
    uint32_t              packetInfoSize;
    uint8_t              *pTrMem = NULL;
    CSL_UdmapTR9         *pTr9 = NULL;
    Vhwa_M2mVissChParams *chPrms = NULL;

    /* Check for Null pointer */
    GT_assert(VhwaVissTrace, (NULL != hObj));

    packetInfoSize = sizeof(CSL_UdmapTR15);

    /* Check for Null pointer */
    GT_assert(VhwaVissTrace, (NULL != hObj));

    /* Enable the channel which are required */
    for (chCnt = 0; chCnt < VHWA_M2M_VISS_MAX_IN_DMA_CH; chCnt ++)
    {
        chPrms = &hObj->inChPrms[chCnt];
        if ((uint32_t)UTRUE == chPrms->isEnabled)
        {
            pTrMem = chPrms->trMem;
            pTr9 = (CSL_UdmapTR9 *)((uint32_t)pTrMem + packetInfoSize);

            /* Invalidate cache before changing contents */
            CacheP_Inv(pTrMem, VHWA_M2M_VISS_UDMA_TRPD_SIZE);

            /* Set the addresses in TRs */
            pTr9->addr = chPrms->bufAddr;

            /* Clear TR Response */
            *(uint32_t *)chPrms->trRespMem = 0xFFFFFFFFU;

            /* Write back contents into memory after changing */
            CacheP_wb(pTrMem, VHWA_M2M_VISS_UDMA_TRPD_SIZE);
        }
    }
    for (chCnt = 0; chCnt < VHWA_M2M_VISS_MAX_OUT_DMA_CH; chCnt ++)
    {
        chPrms = &hObj->outChPrms[chCnt];
        if ((uint32_t)UTRUE == chPrms->isEnabled)
        {
            pTrMem = chPrms->trMem;
            pTr9 = (CSL_UdmapTR9 *)((uint32_t)pTrMem + packetInfoSize);

            /* Invalidate cache before changing contents */
            CacheP_Inv(pTrMem, VHWA_M2M_VISS_UDMA_TRPD_SIZE);

            /* Set the addresses in TRs */
            pTr9->daddr = chPrms->bufAddr;

            /* Clear TR Response */
            *(uint32_t *)chPrms->trRespMem = 0xFFFFFFFFU;

            /* Write back contents into memory after changing */
            CacheP_wb(pTrMem, VHWA_M2M_VISS_UDMA_TRPD_SIZE);
        }
    }
}


/* ========================================================================== */
/*                             Local Functions                                */
/* ========================================================================== */

static void vhwaM2mVissSetupPacketInfo(const Vhwa_M2mVissChParams *chPrms,
    uint32_t complRingNum)
{
    uint32_t             descType = CSL_UDMAP_CPPI5_PD_DESCINFO_DTYPE_VAL_TR;
    CSL_UdmapCppi5TRPD  *pTrpd = NULL;

    /* Check for Null pointer */
    GT_assert(VhwaVissTrace, (NULL != chPrms));

    pTrpd = (CSL_UdmapCppi5TRPD *) chPrms->trMem;

    CSL_udmapCppi5SetDescType(pTrpd, descType);
    CSL_udmapCppi5TrSetReload(pTrpd, UFALSE, 0U);
    CSL_udmapCppi5SetPktLen(pTrpd, descType, 1U);
    /* Flow ID and Packet ID */
    CSL_udmapCppi5SetIds(pTrpd, descType, 0U, 0xFFF);
    CSL_udmapCppi5SetSrcTag(pTrpd, 0x0025);             /* Not used */
    CSL_udmapCppi5SetDstTag(pTrpd, 0x1234);             /* Not used */
    CSL_udmapCppi5TrSetEntryStride(pTrpd,
        CSL_UDMAP_CPPI5_TRPD_PKTINFO_RECSIZE_VAL_64B);
    CSL_udmapCppi5SetReturnPolicy(pTrpd, descType,
        /* Don't care for TR */
        CSL_UDMAP_CPPI5_PD_PKTINFO2_RETPOLICY_VAL_ENTIRE_PKT,
        /* Early return */
        UFALSE, CSL_UDMAP_CPPI5_PD_PKTINFO2_RETPUSHPOLICY_VAL_TO_TAIL,
        complRingNum);
}

static void vhwaM2mVissSetupTxTransferRecord(const Vhwa_M2mVissChParams *chPrms,
                                            CSL_UdmapTR9 *pTr)
{
    uint32_t icnt1, icnt2;

    pTr->flags =
        CSL_FMK(UDMAP_TR_FLAGS_TYPE,
            CSL_UDMAP_TR_FLAGS_TYPE_4D_BLOCK_MOVE_REPACKING) |
        CSL_FMK(UDMAP_TR_FLAGS_STATIC, UFALSE) |
        CSL_FMK(UDMAP_TR_FLAGS_EOL, (uint32_t)UFALSE) |   /* NA */
        CSL_FMK(UDMAP_TR_FLAGS_EVENT_SIZE,
            CSL_UDMAP_TR_FLAGS_EVENT_SIZE_ICNT1_DEC) |
        CSL_FMK(UDMAP_TR_FLAGS_TRIGGER0,
            CSL_UDMAP_TR_FLAGS_TRIGGER_LOCAL_EVENT) |
        CSL_FMK(UDMAP_TR_FLAGS_TRIGGER0_TYPE,
            CSL_UDMAP_TR_FLAGS_TRIGGER_TYPE_ICNT1_DEC) |
                    /* TODO: Check this */
        CSL_FMK(UDMAP_TR_FLAGS_TRIGGER1,
            CSL_UDMAP_TR_FLAGS_TRIGGER_NONE) |
        CSL_FMK(UDMAP_TR_FLAGS_TRIGGER1_TYPE,
            CSL_UDMAP_TR_FLAGS_TRIGGER_TYPE_ICNT1_DEC) |
        /* CmdId: This will come back in TR response */
        CSL_FMK(UDMAP_TR_FLAGS_CMD_ID, (uint32_t)0x25U) |
        CSL_FMK(UDMAP_TR_FLAGS_SA_INDIRECT, (uint32_t)0U) |
        CSL_FMK(UDMAP_TR_FLAGS_DA_INDIRECT, (uint32_t)0U) |
        CSL_FMK(UDMAP_TR_FLAGS_EOP, (uint32_t)1U);

    pTr->fmtflags = 0x0U |
        CSL_FMK(UDMAP_TR_FMTFLAGS_AMODE,
            CSL_UDMAP_TR_FMTFLAGS_AMODE_LINEAR)                         |
        CSL_FMK(UDMAP_TR_FMTFLAGS_DIR,
            CSL_UDMAP_TR_FMTFLAGS_DIR_SRC_USES_AMODE)                   |
        CSL_FMK(UDMAP_TR_FMTFLAGS_ELYPE, CSL_UDMAP_TR_FMTFLAGS_ELYPE_1);

    pTr->icnt0    = (uint16_t)chPrms->widthInBytes;
    pTr->icnt1    = (uint16_t)chPrms->height;
    pTr->icnt2    = 1U;
    pTr->icnt3    = 1U;
    pTr->dim1     = (int32_t)chPrms->imgPitch;
    pTr->dim2     = 0;
    pTr->dim3     = 0;

    icnt1         = chPrms->sl2Depth;
    icnt2         = chPrms->height / icnt1;

    /* Increment icnt2 if height is not multiple of the number of SL2 lines */
    if ((icnt2 * icnt1) != chPrms->height)
    {
        icnt2 ++;
    }

    /* icnt0 and dicnt0 must be same for the TR9 since Trigger and event
       are set for ICNT1 */
    pTr->dicnt0   = (uint16_t)chPrms->widthInBytes;
    pTr->dicnt1   = (uint16_t)icnt1;
    pTr->dicnt2   = (uint16_t)icnt2;
    pTr->dicnt3   = 1u;

    pTr->ddim1    = (int32_t)chPrms->sl2Pitch;
    pTr->ddim2    = 0;
    pTr->ddim3    = 0;

    pTr->addr     = chPrms->sl2Addr;
    pTr->daddr    = chPrms->sl2Addr;

    /* Clear TR response memory */
    *(uint32_t*)chPrms->trRespMem = 0xFFFFFFFFU;
}

static void vhwaM2mVissSetupRxTransferRecord(const Vhwa_M2mVissChParams *chPrms,
                                            CSL_UdmapTR9 *pTr)
{
    uint32_t icnt1, icnt2;

    pTr->flags =
        CSL_FMK(UDMAP_TR_FLAGS_TYPE,
            CSL_UDMAP_TR_FLAGS_TYPE_4D_BLOCK_MOVE_REPACKING) |
        CSL_FMK(UDMAP_TR_FLAGS_STATIC, UFALSE) |
        CSL_FMK(UDMAP_TR_FLAGS_EOL, (uint32_t)UFALSE) |   /* NA */
        CSL_FMK(UDMAP_TR_FLAGS_EVENT_SIZE,
            CSL_UDMAP_TR_FLAGS_EVENT_SIZE_ICNT1_DEC) |
        CSL_FMK(UDMAP_TR_FLAGS_TRIGGER0,
            CSL_UDMAP_TR_FLAGS_TRIGGER_LOCAL_EVENT) |
        CSL_FMK(UDMAP_TR_FLAGS_TRIGGER0_TYPE,
            CSL_UDMAP_TR_FLAGS_TRIGGER_TYPE_ICNT1_DEC) |
        CSL_FMK(UDMAP_TR_FLAGS_TRIGGER1, CSL_UDMAP_TR_FLAGS_TRIGGER_NONE) |
        CSL_FMK(UDMAP_TR_FLAGS_TRIGGER1_TYPE,
            CSL_UDMAP_TR_FLAGS_TRIGGER_TYPE_ICNT1_DEC) |
        /* CmdId: This will come back in TR response */
        CSL_FMK(UDMAP_TR_FLAGS_CMD_ID, (uint32_t)0x25U) |
        CSL_FMK(UDMAP_TR_FLAGS_SA_INDIRECT, (uint32_t)0U) |
        CSL_FMK(UDMAP_TR_FLAGS_DA_INDIRECT, (uint32_t)0U) |
            CSL_FMK(UDMAP_TR_FLAGS_EOP, (uint32_t)1u);

    pTr->fmtflags = 0x0U |
        CSL_FMK(UDMAP_TR_FMTFLAGS_AMODE,
            CSL_UDMAP_TR_FMTFLAGS_AMODE_LINEAR)                         |
        CSL_FMK(UDMAP_TR_FMTFLAGS_DIR,
            CSL_UDMAP_TR_FMTFLAGS_DIR_DST_USES_AMODE)                   |
        CSL_FMK(UDMAP_TR_FMTFLAGS_ELYPE, CSL_UDMAP_TR_FMTFLAGS_ELYPE_1);

    icnt1 = chPrms->sl2Depth;
    icnt2 = chPrms->height / icnt1;

    /* Increment icnt2 if height is not multiple of the number of SL2 lines */
    if ((icnt2 * icnt1) != chPrms->height)
    {
        icnt2 ++;
    }

    pTr->icnt0   = (uint16_t)chPrms->widthInBytes;
    pTr->icnt1   = (uint16_t)icnt1;
    pTr->icnt2   = (uint16_t)icnt2;
    pTr->icnt3   = 1u;

    pTr->dim1    = (int32_t)chPrms->sl2Pitch;
    pTr->dim2    = 0;
    pTr->dim3    = 0;

    /* icnt0 and dicnt0 must be same for the TR9 since Trigger and event
       are set for ICNT1 */
    pTr->dicnt0    = (uint16_t)chPrms->widthInBytes;
    pTr->dicnt1    = (uint16_t)chPrms->height;
    pTr->dicnt2    = 1U;
    pTr->dicnt3    = 1U;

    pTr->ddim1     = (int32_t)chPrms->imgPitch;
    pTr->ddim2     = 0;
    pTr->ddim3     = 0;

    pTr->addr      = chPrms->sl2Addr;
    pTr->daddr     = chPrms->sl2Addr;

    /* Clear TR response memory */
    *(uint32_t*)chPrms->trRespMem = 0xFFFFFFFFU;
}


uint32_t Vhwa_m2mVissCalcHorzSizeInBytes(uint32_t width, uint32_t ccsf,
    uint32_t dataFmt)
{
    uint32_t sizeInBytes, tWidth;

    if ((FVID2_DF_YUV422I_UYVY == (Fvid2_DataFormat)dataFmt) ||
        (FVID2_DF_YUV422I_YUYV == (Fvid2_DataFormat)dataFmt))
    {
        tWidth = width * 2U;
    }
    else
    {
        tWidth = width;
    }
    sizeInBytes = Vhwa_calcHorzSizeInBytes(tWidth, ccsf);

    return (sizeInBytes);
}

void Vhwa_m2mVissmakeTrpd(void *pTrpd, uint32_t trType, uint32_t trCnt,
        uint32_t cqRingNum)
{
    uint32_t descType = CSL_UDMAP_CPPI5_PD_DESCINFO_DTYPE_VAL_TR;
    uint32_t trSizeEncoded = UdmaUtils_getTrSizeEncoded(trType);

    /* Setup descriptor */
    CSL_udmapCppi5SetDescType(pTrpd, descType);
    CSL_udmapCppi5TrSetReload((CSL_UdmapCppi5TRPD*) pTrpd, 0U, 0U);
    CSL_udmapCppi5SetPktLen(pTrpd, descType, trCnt);
    /* Flow ID and Packet ID */
    CSL_udmapCppi5SetIds(pTrpd, descType, 0U, UDMA_DEFAULT_FLOW_ID);
    CSL_udmapCppi5SetSrcTag(pTrpd, 0x0000);
    CSL_udmapCppi5SetDstTag(pTrpd, 0x0000);
    CSL_udmapCppi5TrSetEntryStride((CSL_UdmapCppi5TRPD*) pTrpd, trSizeEncoded);
    CSL_udmapCppi5SetReturnPolicy(pTrpd, descType,
            CSL_UDMAP_CPPI5_PD_PKTINFO2_RETPOLICY_VAL_ENTIRE_PKT,
            CSL_UDMAP_CPPI5_PD_PKTINFO2_EARLYRET_VAL_NO,
            CSL_UDMAP_CPPI5_PD_PKTINFO2_RETPUSHPOLICY_VAL_TO_TAIL, cqRingNum);

    return;
}

void Vhwa_m2mVissSetConfigTransferRecord(
        Vhwa_M2mVissConfigBuffobj *bufferObjHolder, bool sameAsPrevHandle,
        CSL_UdmapTR15 *pTr)
{
    CSL_UdmapTR15 *pTrLocal;
    uint32_t buffObjectCnt = 0;

    /* Check for Null pointer */
    GT_assert(VhwaVissTrace, (NULL != bufferObjHolder));
    GT_assert(VhwaVissTrace, (NULL != pTr));
    pTrLocal = pTr;

    if (true == sameAsPrevHandle)
    {
        for (buffObjectCnt = 0; buffObjectCnt < (uint32_t)BUFF_ID_MAXBUFID;
                buffObjectCnt++)
        {
            if (true == bufferObjHolder[buffObjectCnt].isModified)
            {
                /* Setup TR */
                pTrLocal->flags =
                        CSL_FMK(UDMAP_TR_FLAGS_TYPE,
                                (uint32_t)15U)                                                                          |
                                CSL_FMK(UDMAP_TR_FLAGS_STATIC, (uint32_t)0U)                                           |
                                CSL_FMK(UDMAP_TR_FLAGS_EOL,(uint32_t) 0U)                                              |
                                CSL_FMK(UDMAP_TR_FLAGS_EVENT_SIZE, CSL_UDMAP_TR_FLAGS_EVENT_SIZE_COMPLETION) |
                                CSL_FMK(UDMAP_TR_FLAGS_TRIGGER0, CSL_UDMAP_TR_FLAGS_TRIGGER_NONE)            |
                                CSL_FMK(UDMAP_TR_FLAGS_TRIGGER0_TYPE, CSL_UDMAP_TR_FLAGS_TRIGGER_TYPE_ALL)   |
                                CSL_FMK(UDMAP_TR_FLAGS_TRIGGER1, CSL_UDMAP_TR_FLAGS_TRIGGER_NONE)            |
                                CSL_FMK(UDMAP_TR_FLAGS_TRIGGER1_TYPE, CSL_UDMAP_TR_FLAGS_TRIGGER_TYPE_ALL)   |
                                CSL_FMK(UDMAP_TR_FLAGS_CMD_ID, (uint32_t)0x25U)                                        |
                                CSL_FMK(UDMAP_TR_FLAGS_SA_INDIRECT, (uint32_t)0U)                                      |
                                CSL_FMK(UDMAP_TR_FLAGS_DA_INDIRECT, (uint32_t)0U)                                      |
                                CSL_FMK(UDMAP_TR_FLAGS_EOP, (uint32_t)1U);
                pTrLocal->icnt0 = (uint16_t)bufferObjHolder[buffObjectCnt].length;
                pTrLocal->icnt1 = 1U;
                pTrLocal->icnt2 = 1U;
                pTrLocal->icnt3 = 1U;
                pTrLocal->dim1 = (int32_t)pTrLocal->icnt0;
                pTrLocal->dim2 = 0;
                pTrLocal->dim3 = 0;
                pTrLocal->addr =
                        (uint64_t) bufferObjHolder[buffObjectCnt].srcPtr;
                pTrLocal->fmtflags = 0x00000000U; /* Linear addressing, 1 byte per elem.
                 Replace with CSL-FL API */
                pTrLocal->dicnt0 =(uint16_t) bufferObjHolder[buffObjectCnt].length;
                pTrLocal->dicnt1 = 1U;
                pTrLocal->dicnt2 = 1U;
                pTrLocal->dicnt3 = 1U;
                pTrLocal->ddim1 = (int32_t)pTrLocal->dicnt0;
                pTrLocal->ddim2 = 0;
                pTrLocal->ddim3 = 0;
                pTrLocal->daddr =
                        (uint64_t) bufferObjHolder[buffObjectCnt].dstPtr;

                pTrLocal++;
            }
        }
    }
    else
    {
        for (buffObjectCnt = 0; buffObjectCnt <(uint32_t) BUFF_ID_MAXBUFID; buffObjectCnt++)
        {
            if (true == bufferObjHolder[buffObjectCnt].isUsed)
            {
                /* Setup TR */
                pTrLocal->flags =
                        CSL_FMK(UDMAP_TR_FLAGS_TYPE,
                                (uint32_t)15U) |
                                CSL_FMK(UDMAP_TR_FLAGS_STATIC, (uint32_t)0U)                                           |
                                CSL_FMK(UDMAP_TR_FLAGS_EOL, (uint32_t)0U)                                              |
                                CSL_FMK(UDMAP_TR_FLAGS_EVENT_SIZE, CSL_UDMAP_TR_FLAGS_EVENT_SIZE_COMPLETION) |
                                CSL_FMK(UDMAP_TR_FLAGS_TRIGGER0, CSL_UDMAP_TR_FLAGS_TRIGGER_NONE)            |
                                CSL_FMK(UDMAP_TR_FLAGS_TRIGGER0_TYPE, CSL_UDMAP_TR_FLAGS_TRIGGER_TYPE_ALL)   |
                                CSL_FMK(UDMAP_TR_FLAGS_TRIGGER1, CSL_UDMAP_TR_FLAGS_TRIGGER_NONE)            |
                                CSL_FMK(UDMAP_TR_FLAGS_TRIGGER1_TYPE, CSL_UDMAP_TR_FLAGS_TRIGGER_TYPE_ALL)   |
                                CSL_FMK(UDMAP_TR_FLAGS_CMD_ID, (uint32_t)0x25U)                                        |
                                CSL_FMK(UDMAP_TR_FLAGS_SA_INDIRECT, (uint32_t)0U)                                      |
                                CSL_FMK(UDMAP_TR_FLAGS_DA_INDIRECT, (uint32_t)0U)                                      |
                                CSL_FMK(UDMAP_TR_FLAGS_EOP, (uint32_t)1U);
                pTrLocal->icnt0 = (uint16_t)bufferObjHolder[buffObjectCnt].length;
                pTrLocal->icnt1 = 1U;
                pTrLocal->icnt2 = 1U;
                pTrLocal->icnt3 = 1U;
                pTrLocal->dim1 = (int32_t)pTrLocal->icnt0;
                pTrLocal->dim2 = 0;
                pTrLocal->dim3 = 0;
                pTrLocal->addr =
                        (uint64_t) bufferObjHolder[buffObjectCnt].srcPtr;
                pTrLocal->fmtflags = 0x00000000U; /* Linear addressing, 1 byte per elem.
                 Replace with CSL-FL API */
                pTrLocal->dicnt0 =(uint16_t) bufferObjHolder[buffObjectCnt].length;
                pTrLocal->dicnt1 = 1U;
                pTrLocal->dicnt2 = 1U;
                pTrLocal->dicnt3 = 1U;
                pTrLocal->ddim1 = (int32_t)pTrLocal->dicnt0;
                pTrLocal->ddim2 = 0;
                pTrLocal->ddim3 = 0;
                pTrLocal->daddr =
                        (uint64_t) bufferObjHolder[buffObjectCnt].dstPtr;

                pTrLocal++;
            }
        }
    }
}

void Vhwa_m2mVissSetConfigTRDesc(Vhwa_M2mVissInstObj *instObj,
        Vhwa_M2mVissConfigBuffobj *bufferObjHolder, bool sameAsPrevHandle,
        uint32_t numOfTr)
{
    uint8_t *pTrMem = NULL;
    CSL_UdmapTR15 *pTr15 = NULL;
    CSL_UdmapCppi5TRPD *pTrpd = NULL;
    uint32_t packetInfoSize;

    packetInfoSize = sizeof(CSL_UdmapTR15);

    pTrMem = instObj->configTxTrMem;

    pTr15 = (CSL_UdmapTR15*) ((uint32_t) pTrMem + packetInfoSize);
    pTrpd = (CSL_UdmapCppi5TRPD*) pTrMem;

    /* Invalidate cache before changing contents */
    CacheP_Inv(pTrMem, VHWA_M2M_VISS_UDMA_CONFIG_TRPD_SIZE);

    /* Make TRPD */
    Vhwa_m2mVissmakeTrpd(pTrpd, UDMA_TR_TYPE_15, numOfTr,
            instObj->configCqRingNum);

    instObj->configTxTrRespMem = (uint8_t*) ((uint32_t) pTrMem
            + (packetInfoSize * (numOfTr + 1U)));

    /* Setup Transfer Record */
    Vhwa_m2mVissSetConfigTransferRecord(bufferObjHolder, sameAsPrevHandle,
            pTr15);

    /* Clear TR response memory */
    *(uint32_t*) instObj->configTxTrRespMem = 0xFFFFFFFFU;

    /* Write back contents into memory after changing */
    CacheP_wb(pTrMem, VHWA_M2M_VISS_UDMA_CONFIG_TRPD_SIZE);
}

int32_t Vhwa_m2mVissSubmitConfigUDMARing(Vhwa_M2mVissInstObj *instObj,
        const uint8_t *trMem)
{
    int32_t status = UDMA_SOK;
    /* Check for Null pointer */
    GT_assert(VhwaVissTrace, (NULL != instObj));
    GT_assert(VhwaVissTrace, (NULL != trMem));

    /* Submit TRPD to ring */
    status = Udma_ringQueueRaw(instObj->configFqRingHndl, (uint64_t) trMem);

    if (UDMA_SOK != status)
    {
        GT_0trace(VhwaVissTrace, GT_ERR, "Ring queue failed\n");
    }

    if (UDMA_SOK == status)
    {
        status = FVID2_SOK;
    }
    else
    {
        status = FVID2_EFAIL;
    }

    return (status);
}
