/*
*
* Copyright (c) 2024 Texas Instruments Incorporated
*
* All rights reserved not granted herein.
*
* Limited License.
*
* Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive
* license under copyrights and patents it now or hereafter owns or controls to make,
* have made, use, import, offer to sell and sell ("Utilize") this software subject to the
* terms herein.  With respect to the foregoing patent license, such license is granted
* solely to the extent that any such patent is necessary to Utilize the software alone.
* The patent license shall not apply to any combinations which include this software,
* other than combinations with devices manufactured by or for TI ("TI Devices").
* No hardware patent is licensed hereunder.
*
* Redistributions must preserve existing copyright notices and reproduce this license
* (including the above copyright notice and the disclaimer and (if applicable) source
* code license limitations below) in the documentation and/or other materials provided
* with the distribution
*
* Redistribution and use in binary form, without modification, are permitted provided
* that the following conditions are met:
*
* *       No reverse engineering, decompilation, or disassembly of this software is
* permitted with respect to any software provided in binary form.
*
* *       any redistribution and use are licensed by TI for use only with TI Devices.
*
* *       Nothing shall obligate TI to provide you with source code for the software
* licensed and provided to you in object code.
*
* If software source code is provided to you, modification and redistribution of the
* source code are permitted provided that the following conditions are met:
*
* *       any redistribution and use of the source code, including any resulting derivative
* works, are licensed by TI for use only with TI Devices.
*
* *       any redistribution and use of any object code compiled from the source code
* and any resulting derivative works, are licensed by TI for use only with TI Devices.
*
* Neither the name of Texas Instruments Incorporated nor the names of its suppliers
*
* may be used to endorse or promote products derived from this software without
* specific prior written permission.
*
* DISCLAIMER.
*
* THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

/**
 *  \file     Spi_Priv.c
 *
 *  \brief    This file contains SPI MCAL driver internal functions
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "stdint.h"
#include "string.h"
#include "Spi_Cfg.h"
#include "Spi.h"
#include "Spi_Dbg.h"

#include "Spi_Priv.h"
#include <hw_include/soc.h>

#if (STD_ON == SPI_JOB_LOG)

/* There are static inline functions in hw_types.h file. Map them as well */
#define SPI_START_SEC_CODE
#include "Spi_MemMap.h"
#include <hw_include/hw_types.h>
#define SPI_STOP_SEC_CODE
#include "Spi_MemMap.h"

#endif  /* #if (STD_ON == SPI_JOB_LOG) */

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */


/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/** \brief SPI Channel PC Configuration struct declaration */
extern const struct Spi_ChannelConfigType_PC_s Spi_ChannelConfig_PC[SPI_MAX_CHANNELS];

/** \brief SPI Job PC Configuration struct declaration */
extern const struct Spi_JobConfigType_PC_s Spi_JobConfig_PC[SPI_MAX_JOBS];

/** \brief SPI Sequence PC Configuration struct declaration */
extern const struct Spi_SeqConfigType_PC_s Spi_SeqConfig_PC[SPI_MAX_SEQ];
/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */
static void Spi_processJobCompletion(Spi_JobObjType    *jobObj,
                                     Spi_HwUnitObjType *hwUnitObj,
                                     Spi_JobResultType  jobResult);

static Std_ReturnType Spi_queueJobs(Spi_SeqObjType *seqObj);
static void Spi_scheduleJob(Spi_JobObjType *jobObj);
static void Spi_scheduleAllJobsSyncTransmit(Spi_SeqObjType *seqObj);

static void Spi_checkAndSetDrvState(void);
static Std_ReturnType Spi_startSeqCheck(const Spi_SeqObjType *seqObj);
#if (STD_ON == SPI_DEV_ERROR_DETECT)
static Std_ReturnType Spi_checkChConfig(const Spi_ConfigType *cfgPtr);
static Std_ReturnType Spi_checkJobConfig(const Spi_ConfigType *cfgPtr);
static Std_ReturnType Spi_checkSeqConfig(const Spi_ConfigType *cfgPtr);
#endif  /* #if (STD_ON == SPI_DEV_ERROR_DETECT) */

#if (STD_ON == SPI_JOB_LOG)
static void Spi_logJob(const Spi_JobObjType    *jobObj,
                       const Spi_HwUnitObjType *hwUnitObj);
#endif  /* #if (STD_ON == SPI_JOB_LOG) */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

#if (STD_ON == SPI_JOB_LOG)
#define SPI_START_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Spi_MemMap.h"

/** \brief SPI job log object */

VAR(Spi_JobLogType, SPI_VAR_CLEARED) Spi_JobLogObj;

#define SPI_STOP_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Spi_MemMap.h"
#endif  /* #if (STD_ON == SPI_JOB_LOG) */

#define SPI_START_SEC_CONST_32
#include "Spi_MemMap.h"
#define SPI_STOP_SEC_CONST_32
#include "Spi_MemMap.h"

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

#define SPI_START_SEC_CODE
#include "Spi_MemMap.h"

void Spi_hwUnitInit(Spi_HwUnitObjType *hwUnitObj)
{
    hwUnitObj->baseAddr   = Spi_getHwUnitBaseAddr(hwUnitObj->hwUnitCfg.hwUnitId);

    Spi_utilsInitLinkList(&hwUnitObj->jobList);

    /* Init the HW instance */
	Spi_mcspiInit(hwUnitObj);

    /* copy dma mode flag */
    hwUnitObj->enabledmaMode   = hwUnitObj->hwUnitCfg.enabledmaMode;

    return;
}

void Spi_hwUnitDeInit(Spi_HwUnitObjType *hwUnitObj)
{
    /* De-Init the HW instance */
    Spi_mcspiDeInit(hwUnitObj);

    Spi_utilsDeInitLinkList(&hwUnitObj->jobList);

    return;
}

/*
 * Design: MCAL-6596,MCAL-6637
 */
Std_ReturnType Spi_startSeqAsync(Spi_SeqObjType *seqObj)
{
    uint32             index;
    Std_ReturnType     retVal = (Std_ReturnType) E_OK;
    Spi_UtilsNode     *headNodeObj;
    Spi_JobObjType    *jobObj;
    Spi_HwUnitObjType *hwUnitObj;

    SchM_Enter_Spi_SPI_EXCLUSIVE_AREA_0();

    /* Queue the jobs in this sequence */
    retVal = Spi_queueJobs(seqObj);

    if (((Std_ReturnType) E_OK) == retVal)
    {
        /* Check all the hardware queue and consume any pending job if the
         * hardware is free */
        for (index = 0U; index < (uint32)Spi_DrvObj.maxHwUnit; index++)
        {
            hwUnitObj = &Spi_DrvObj.hwUnitObj[index];
            if (SPI_HW_UNIT_OK == hwUnitObj->hwUnitResult)
            {
                /* Check if we have any job pending in the queue */
                headNodeObj = Spi_utilsGetHeadNode(&hwUnitObj->jobList);
                if (headNodeObj != NULL_PTR)
                {
                    /* Some job pending - schedule it */
                    jobObj = (Spi_JobObjType *) headNodeObj->params.data;
                    Spi_utilsUnLinkNodePri((&(hwUnitObj->jobList)), headNodeObj);
                    hwUnitObj->curJobObj    = jobObj;
                    hwUnitObj->hwUnitResult = SPI_HW_UNIT_PENDING;
                    Spi_scheduleJob(jobObj);
                }
            }
        }
    }

    SchM_Exit_Spi_SPI_EXCLUSIVE_AREA_0();

    return (retVal);
}

/*
 * Design: MCAL-6637,MCAL-6748,MCAL-6465,MCAL-6508,MCAL-6623,MCAL-6745
 */
Std_ReturnType Spi_startSeqSync(Spi_SeqObjType *seqObj)
{
    Std_ReturnType       retVal = ((Std_ReturnType) E_OK);
    Spi_SeqEndNotifyType seqEndNotification = NULL;

#if (STD_ON == SPI_SUPPORT_CONCURRENT_SYNC_TRANSMIT)
    /* Check for start sequence error - needed only for concurrent support
     * on as there is no chance of jobs in pending when this is off */
    retVal = Spi_startSeqCheck(seqObj);
    /* Accept concurrent Spi_SyncTransmit(), only if the sequences to be
     * transmitted use different bus */
    if (((Std_ReturnType) E_OK) == retVal)
    {
        uint32             index;
        Spi_JobObjType    *jobObj;
        Spi_HwUnitObjType *hwUnitObj;
        Spi_JobType        jobId;

        /* Check if the HW unit is free for all the jobs in this sequence.
         * If not reject the submission */
        for (index = 0U; index < seqObj->seqCfg.jobPerSeq; index++)
        {
            jobId     = seqObj->seqCfg.jobList[index];
            jobObj    = &Spi_DrvObj.jobObj[jobId];
            hwUnitObj = jobObj->hwUnitObj;
            if (SPI_HW_UNIT_OK != hwUnitObj->hwUnitResult)
            {
                /* Reject sequence */
                retVal = (Std_ReturnType) E_NOT_OK;
                break;
            }
        }
    }

    if (((Std_ReturnType) E_OK) != retVal)
    {
        Spi_reportDetRuntimeError(SPI_SID_SYNC_TRANSMIT, SPI_E_SEQ_PENDING);
    }
    else
#endif  /* #if (STD_ON == SPI_SUPPORT_CONCURRENT_SYNC_TRANSMIT) */
    {
        /* Set the states */
        seqObj->seqResult      = SPI_SEQ_PENDING;
        seqObj->numJobsPending = seqObj->seqCfg.jobPerSeq;
        Spi_DrvStatus          = SPI_BUSY;

        /* Schedule all the jobs one after another in case of Sync transfer */
        Spi_scheduleAllJobsSyncTransmit(seqObj);

        /* check if any job has previously failed or cancelled
         * and hence the sequence result is already set. So
         * don't overwrite the sequence status with OK for the
         * subsequent job */
        /*SPI_CoverageGap_09:Dynamic coverage for false condition cannot be achievable as
         * seqResult is set to SPI_SEQ_PENDING
         */
        if (seqObj->seqResult == SPI_SEQ_PENDING)
        {
            seqObj->seqResult = SPI_SEQ_OK;
        }
        /* Notify Sequence end - should be done after scheduling */

    #if (SPI_E_HARDWARE_ERROR != SPI_DEM_NO_EVENT)
        (void)Dem_SetEventStatus(SPI_E_HARDWARE_ERROR, DEM_EVENT_STATUS_PASSED);
    #endif

        seqEndNotification = seqObj->seqCfg.Spi_SequenceEndNotification;
        if (NULL != seqEndNotification)
        {
            seqEndNotification();
        }

        /*
         * Check if all hardware is free so that driver can be
         * put in idle state
         */
        Spi_checkAndSetDrvState();
    }

    return (retVal);
}

#if (STD_ON == SPI_CANCEL_API)
/*
 * Design: MCAL-6740,MCAL-6631
 */
void Spi_cancelSequence(Spi_SeqObjType *seqObj)
{
    uint32 jobIndex;
	Spi_JobType jobId;
    Spi_JobObjType      *jobObj;
    Spi_HwUnitObjType   *hwUnitObj;
    Spi_SeqEndNotifyType seqEndNotification = NULL;
    Spi_JobEndNotifyType jobEndNotification = NULL;

    SchM_Enter_Spi_SPI_EXCLUSIVE_AREA_0();

    /* only a pending sequence can be cancelled */
    if (SPI_SEQ_PENDING == seqObj->seqResult)
    {
        /* Cancel the sequence */
        seqObj->seqResult = SPI_SEQ_CANCELLED;

        /* Check all the jobs in the sequence and remove them if they are not
         * already started. If they are started allow the job to complete
         * in the normal way and do the clean-up in the ISR context */
        for (jobIndex = 0U; jobIndex < seqObj->seqCfg.jobPerSeq; jobIndex++)
        {
            jobId     = seqObj->seqCfg.jobList[jobIndex];
            jobObj    = &Spi_DrvObj.jobObj[jobId];
            hwUnitObj = jobObj->hwUnitObj;
            if (jobObj->jobResult == SPI_JOB_QUEUED)
            {
                /* Job is not active, remove from HW job queue */
                Spi_utilsUnLinkNodePri(&hwUnitObj->jobList, &jobObj->nodeObj);
                jobObj->seqObj->numJobsPending--;
                jobObj->jobResult = SPI_JOB_OK;
                /* Notify Job end */
                if (NULL_PTR != jobObj->jobCfg.Spi_JobEndNotification)
                {
                    jobEndNotification = jobObj->jobCfg.Spi_JobEndNotification;
                    jobEndNotification();
                }
            }
        }

        /* If all jobs of the sequence are removed, then notify sequence end
         * callback */
        /* SPI_CoverageGap_10:Dynamic coverage for MC/DC for below code cannot be achievable for
         * as sequence is dependent on jobs, if all jobs done, means sequence
         * should not be a NULL Pointer
         */
        if ((0U == seqObj->numJobsPending) &&
            (NULL_PTR != seqObj->seqCfg.Spi_SequenceEndNotification))
        {
            seqEndNotification = seqObj->seqCfg.Spi_SequenceEndNotification;
            seqEndNotification();
        }
    }

    SchM_Exit_Spi_SPI_EXCLUSIVE_AREA_0();

    return;
}

#endif  /* #if (STD_ON == SPI_CANCEL_API) */

/*
 * Design: MCAL-6673,MCAL-6507,MCAL-6484,MCAL-6441,MCAL-6372,MCAL-6656,
 * MCAL-6715,MCAL-6404,MCAL-6551,MCAL-6747,MCAL-6462,MCAL-6644,MCAL-6718, MCAL-6534,MCAL-6483,MCAL-6592,MCAL-6735,MCAL-6443,MCAL-6616 */
void Spi_processEvents(Spi_HwUnitObjType *hwUnitObj)
{
    Spi_ChannelType     chId;
    Spi_ChannelObjType *chObj;
    Spi_JobResultType   jobResult;
    Spi_JobObjType     *jobObj;

    /* Get the current job/sequence */
    jobObj = hwUnitObj->curJobObj;
    /* SPI_CoverageGap_11:Dynamic coverage for below true condition cannot be achievable as its
     * dependend on Hardware Error during runtime
     */
    if (jobObj == NULL_PTR)
    {
        /* Incase of McSPI, RX interrupt could happen after WCNT interrupt
         * Hence this interrupt could happen after all the data is read
         * and states moved. So don't report DET error. */
        Spi_mcspiClearAllIrqStatus(hwUnitObj->baseAddr);
    }
    else
    {
        chId      = jobObj->jobCfg.channelList[jobObj->curChIdx];
       /* Each port is considered as one controller */
        chObj     = Spi_getCurrChannelObj(chId);
        jobResult = Spi_mcspiContinueTxRx(hwUnitObj, jobObj, chObj);

        if (SPI_JOB_PENDING != jobResult)
        {
            /*
             * Channel completed or failed!!
             *
             * Caution: Since there is no channel status macro, we are
             * reusing the job status macros. Also this can be directly
             * used to assign to job status
             */
            Spi_processChCompletion(hwUnitObj, jobResult);
        }
        /* Else Job is still pending. Do nothing, wait for next interrupt */
    }

    return;
}

/**
 *  \brief Returns the the HW object for the corresponding HW unit ID
 */
/*
 * Design: MCAL-6460,MCAL-6633
 */
Spi_HwUnitObjType *Spi_getHwUnitObj(Spi_HWUnitType HWUnit)
{
    uint32 hwUnitIdx          = 0U;
    uint32 hwUnitCfgFound     = (uint32) FALSE;
    Spi_DriverObjType *drvObj = &Spi_DrvObj;
    Spi_HwUnitObjType *hwUnitObj;

    /* HW unit ID may not be the index, so search for matching HW unit */
    for (hwUnitIdx = 0U; hwUnitIdx < SPI_MAX_HW_UNIT; hwUnitIdx++)
    {
        if (drvObj->hwUnitObj[hwUnitIdx].hwUnitCfg.hwUnitId == HWUnit)
        {
            hwUnitCfgFound = (uint32) TRUE;
            break; /* Match found */
        }
    }

    if (hwUnitCfgFound != ((uint32) TRUE))
    {
        hwUnitObj = NULL;
#if (STD_ON == SPI_DEV_ERROR_DETECT)
        Spi_reportDetError(SPI_SID_INIT, SPI_E_PARAM_UNIT);
#endif
    }
    else
    {
        hwUnitObj = &drvObj->hwUnitObj[hwUnitIdx];
    }

    return hwUnitObj;
}

/*
 * Design: MCAL-6620
 */
void Spi_resetDrvObj(Spi_DriverObjType *drvObj)
{
    uint32 index;
#if (STD_ON == SPI_DMA_ENABLE)
    uint32 dmaEnabledHwCnt = (uint32)SPI_MAX_HW_DMA_UNIT;
#endif
#if ((SPI_CHANNELBUFFERS == SPI_IB) || (SPI_CHANNELBUFFERS == SPI_IB_EB))
    uint32 ibIndex;
#endif

    for (index = 0U; index < SPI_MAX_CHANNELS; index++)
    {
        drvObj->channelObj[index].txBufPtr =
            (const Spi_DataBufferType *) NULL_PTR;
        drvObj->channelObj[index].rxBufPtr =
            NULL;
        drvObj->channelObj[index].numWordsTxRx = 0U;

        drvObj->channelObj[index].curTxBufPtr  =
            (const Spi_DataBufferType *) NULL_PTR;
        drvObj->channelObj[index].curRxBufPtr =
            NULL;
        drvObj->channelObj[index].curTxWords     = 0U;
        drvObj->channelObj[index].curRxWords     = 0U;
        drvObj->channelObj[index].bufWidth       = 4U;
        drvObj->channelObj[index].effTxFifoDepth = 1U;
        drvObj->channelObj[index].dataWidthBitMask = 0xFFFFFFFFU;
#if ((SPI_CHANNELBUFFERS == SPI_IB) || (SPI_CHANNELBUFFERS == SPI_IB_EB))
        for(ibIndex = 0U; ibIndex < SPI_IB_MAX_LENGTH; ibIndex++)
        {
            drvObj->channelObj[index].txIb[ibIndex] = 0U;
            drvObj->channelObj[index].rxIb[ibIndex] = 0U;
        }
#endif
    }
    for (index = 0U; index < SPI_MAX_JOBS; index++)
    {
        drvObj->jobObj[index].jobResult     = SPI_JOB_OK;
        drvObj->jobObj[index].seqObj        = NULL;
        drvObj->jobObj[index].curChIdx      = 0U;
        drvObj->jobObj[index].txFifoTrigLvl = 0U;
        drvObj->jobObj[index].rxFifoTrigLvl = 0U;
        Spi_utilsInitNodeObject(&(drvObj->jobObj[index].nodeObj));
        drvObj->jobObj[index].extDevCfg =
            NULL;
    }
    for (index = 0U; index < SPI_MAX_SEQ; index++)
    {
        drvObj->seqObj[index].seqResult      = SPI_SEQ_OK;
        drvObj->seqObj[index].numJobsPending = 0U;
    }
    for (index = 0U; index < SPI_MAX_HW_UNIT; index++)
    {
        drvObj->hwUnitObj[index].hwUnitResult = SPI_HW_UNIT_OK;
        drvObj->hwUnitObj[index].curJobObj    = NULL;
        /* Init current job to invalid job id */
        drvObj->hwUnitObj[index].curConfiguredJobId = SPI_MAX_JOBS;
    }
    drvObj->isAsynInProgress = (uint32) FALSE;
    drvObj->maxChannels      = 0U;
    drvObj->maxJobs          = 0U;
    drvObj->maxSeq           = 0U;
    drvObj->maxHwUnit        = 0U;

#if (STD_ON == SPI_DMA_ENABLE)
    drvObj->drvHandle = NULL;
    drvObj->dmaEnableHwIndx = 0U;
    for (index = 0U; index < dmaEnabledHwCnt; index++)
    {
        drvObj->dmaObj[index].hwUnitId                   = (Spi_HWUnitType)0U;
        drvObj->dmaObj[index].dmaEnableHwInstId          = 0U;
        drvObj->dmaObj[index].numOfWordsNotEffFifoDepth  = 0U;
        drvObj->dmaObj[index].csPin                      = (Spi_CsPinType)0U;
    }
#endif

#if ((SPI_SCALEABILITY == SPI_LEVEL_0) || \
    (SPI_SCALEABILITY ==  SPI_LEVEL_2))
    drvObj->asyncMode        = SPI_POLLING_MODE;
#endif
#if (SPI_SCALEABILITY == SPI_LEVEL_1)
    drvObj->asyncMode        = SPI_INTERRUPT_MODE;
#endif

#if (STD_ON == SPI_JOB_LOG)
    (void) memset(&Spi_JobLogObj, 0, sizeof (Spi_JobLogObj));
#endif  /* #if (STD_ON == SPI_JOB_LOG) */

    return;
}

/*
 * Design: MCAL-6526,MCAL-6468,MCAL-6666,MCAL-6576,MCAL-6440,MCAL-6614,MCAL-6474
 */
#if (STD_ON == SPI_DEV_ERROR_DETECT)
Std_ReturnType Spi_checkConfig(
    const Spi_ConfigType *cfgPtr)
{
    Std_ReturnType retVal;

    retVal = Spi_checkChConfig(cfgPtr);
    if (((Std_ReturnType) E_OK) == retVal)
    {
        retVal = Spi_checkJobConfig(cfgPtr);
    }
    if (((Std_ReturnType) E_OK) == retVal)
    {
        retVal = Spi_checkSeqConfig(cfgPtr);
    }

    return (retVal);
}

#endif  /* #if (STD_ON == SPI_DEV_ERROR_DETECT) */

void Spi_copyConfig(Spi_DriverObjType *drvObj,
    const Spi_ConfigType *cfgPtr)
{
    uint32              index;
    Spi_JobObjType     *jobObj;
    Spi_ChannelObjType *chObj;

    drvObj->maxChannels  = cfgPtr->maxChannels;
    drvObj->maxJobs      = cfgPtr->maxJobs;
    drvObj->maxSeq       = cfgPtr->maxSeq;
    drvObj->maxHwUnit    = cfgPtr->maxHwUnit;
    drvObj->maxExtDevCfg = cfgPtr->maxExtDevCfg;
    for (index = 0U; index < (uint32)cfgPtr->maxExtDevCfg; index++)
    {
        (void) memcpy(
            &drvObj->extDevCfg[index],
            &cfgPtr->extDevCfg[index],
            sizeof (Spi_ExternalDeviceConfigType));
    }
    for (index = 0U; index < SPI_MAX_HW_UNIT; index++)
    {
        (void) memcpy(
            &drvObj->hwUnitObj[index].hwUnitCfg,
            &cfgPtr->hwUnitCfg[index],
            sizeof (Spi_HwUnitConfigType));
        /* Init current job to invalid job id */
        drvObj->hwUnitObj[index].curConfiguredJobId = (uint16)drvObj->maxJobs;
    }

    for (index = 0U; index < (uint32)cfgPtr->maxChannels; index++)
    {
        chObj = &drvObj->channelObj[index];
        (void) memcpy(
            &chObj->chCfg,
            &cfgPtr->channelCfg[index],
            sizeof (Spi_ChannelConfigType));

        if (chObj->chCfg.dataWidth < 9U)
        {
            chObj->bufWidth = 1U;
        }
        else if (chObj->chCfg.dataWidth < 17U)
        {
            chObj->bufWidth = 2U;
        }
        else
        {
            chObj->bufWidth = 4U;
        }
        /* Incase of IB, the numWordsTxRx is same as maxBufLength */
        if (SPI_IB == chObj->chCfg.channelBufType)
        {
            chObj->numWordsTxRx = chObj->chCfg.maxBufLength;
        }
        /* Calculate data width mask depending on SPI word size */
        chObj->dataWidthBitMask = Spi_getDataWidthBitMask(
                                        chObj->chCfg.dataWidth);
    }
    for (index = 0U; index < (uint32)cfgPtr->maxJobs; index++)
    {
        jobObj = &drvObj->jobObj[index];
        (void) memcpy(
            &jobObj->jobCfg,
            &cfgPtr->jobCfg[index],
            sizeof (Spi_JobConfigType));
        (void) memcpy(
            &jobObj->jobCfg_PC,
            &Spi_JobConfig_PC[index],
            sizeof (Spi_JobConfigType_PC));
        jobObj->hwUnitObj = Spi_getHwUnitObj(jobObj->jobCfg.hwUnitId);
        jobObj->hwUnitObj->baseAddr = Spi_getHwUnitBaseAddr(jobObj->jobCfg.hwUnitId);
        jobObj->extDevCfg =
            &drvObj->extDevCfg[jobObj->jobCfg_PC.externalDeviceCfgId];
    }
    for (index = 0U; index < (uint32)cfgPtr->maxSeq; index++)
    {
        (void) memcpy(
            &drvObj->seqObj[index].seqCfg,
            &cfgPtr->seqCfg[index],
            sizeof (Spi_SeqConfigType));
        drvObj->seqObj[index].seqCfg_PC.seqId =
			Spi_SeqConfig_PC[index].seqId;
    }

    return;
}

uint32 Spi_getHwUnitBaseAddr(Spi_HWUnitType hwUnitId)
{
    return (Spi_HwUnitBaseAddr[hwUnitId]);
}

uint32 Spi_getDataWidthBitMask(uint32 dataWidth)
{
    uint32 i, fifoBitMask = 0x0U;
    uint32 tmpVar = 0U;

    for (i = 0U; i < dataWidth; i++)
    {
        tmpVar = ((uint32)1U << i);
        fifoBitMask |= tmpVar;
    }

    return fifoBitMask;
}

#if (STD_ON == SPI_DEV_ERROR_DETECT)
void Spi_reportDetError(uint8 apiId, uint8 errorId)
{
    (void) Det_ReportError(SPI_MODULE_ID, SPI_INSTANCE_ID, apiId, errorId);
    return;
}

#endif  /* #if (STD_ON == SPI_DEV_ERROR_DETECT) */

void Spi_reportDetRuntimeError(uint8 apiId, uint8 errorId)
{
    (void) Det_ReportRuntimeError(SPI_MODULE_ID, SPI_INSTANCE_ID, apiId, errorId);
    return;
}

void Spi_processChCompletion(
    Spi_HwUnitObjType *hwUnitObj, Spi_JobResultType  jobResult)
{
    Spi_ChannelType     chId;
    Spi_ChannelObjType *chObj;
    Spi_JobObjType     *jobObj;

    jobObj = hwUnitObj->curJobObj;

    /* Channel completed */
    jobObj->curChIdx++;
    if (jobObj->curChIdx == jobObj->jobCfg.channelPerJob)
    {
        /*
         * Job completed
         */

        /* Stop the hardware */
        /*SPI_CoverageGap_12:Dynamic analysis of below false condition is covered in test case ID 4003*/
        if (hwUnitObj->enabledmaMode != (boolean)TRUE)
        {
            Spi_mcspiStop(hwUnitObj, jobObj);
        }
#if (STD_ON == SPI_DMA_ENABLE)
        else
        {
            Spi_DmaStop(hwUnitObj, jobObj);
        }
#endif
        Spi_processJobCompletion(jobObj, hwUnitObj, jobResult);
    }
    else
    {
        /* Start the next channel */
        chId  = jobObj->jobCfg.channelList[jobObj->curChIdx];
        chObj = Spi_getCurrChannelObj(chId);
        Spi_mcspiConfigCh(hwUnitObj, jobObj, chObj);
        /*SPI_CoverageGap_13:Dynamic analysis of below false condition is covered in test case ID 4003*/
        if (hwUnitObj->enabledmaMode != (boolean)TRUE)
        {
            Spi_mcspiReStart(hwUnitObj, jobObj);
        }
#if (STD_ON == SPI_DMA_ENABLE)
        else
        {
            Spi_DmaTransferReStart(hwUnitObj, jobObj, chObj);
        }
#endif
    }

    return;
}

static void Spi_processJobCompletion(
    Spi_JobObjType    *jobObj,
    Spi_HwUnitObjType *hwUnitObj,
    Spi_JobResultType  jobResult)
{
    Spi_UtilsNode       *headNodeObj;
    Spi_SeqObjType      *seqObj;
    Spi_SeqEndNotifyType seqEndNotification = NULL;
    Spi_JobEndNotifyType jobEndNotification = NULL;

    seqObj = jobObj->seqObj;

    hwUnitObj->curJobObj = NULL;
    seqObj->numJobsPending--;
    jobObj->jobResult  = jobResult;
    jobEndNotification = jobObj->jobCfg.Spi_JobEndNotification;

    /* Check if sequence is complete */
    if (0U == seqObj->numJobsPending)
    {
        /* check if any job has previously failed or cancelled
         * and hence the sequence result is already set. So
         * don't overwrite the sequence status with OK for the
         * subsequent job */
        if (seqObj->seqResult == SPI_SEQ_PENDING)
        {
            seqObj->seqResult = SPI_SEQ_OK;
        }
        seqEndNotification = seqObj->seqCfg.Spi_SequenceEndNotification;
    }

    /* Check if any new job is pending */
    headNodeObj = Spi_utilsGetHeadNode(&hwUnitObj->jobList);
    if (headNodeObj != NULL_PTR)
    {
        Spi_JobObjType *nextJobObj;
        /* Some job pending - schedule it */
        nextJobObj = (Spi_JobObjType *) headNodeObj->params.data;
        Spi_utilsUnLinkNodePri((&(hwUnitObj->jobList)), headNodeObj);

        hwUnitObj->curJobObj    = nextJobObj;
        hwUnitObj->hwUnitResult = SPI_HW_UNIT_PENDING;
        Spi_scheduleJob(nextJobObj);
    }
    else
    {
        /* No new job scheduled, hardware is free!! */
        hwUnitObj->hwUnitResult = SPI_HW_UNIT_OK;

        /*
         * Check if all hardware is free so that driver can be
         * put in idle state
         */
        Spi_checkAndSetDrvState();
    }

#if (SPI_E_HARDWARE_ERROR != SPI_DEM_NO_EVENT)
    (void)Dem_SetEventStatus(SPI_E_HARDWARE_ERROR, DEM_EVENT_STATUS_PASSED);
#endif

    /* Notify Job end - should be done after scheduling */
    if (NULL != jobEndNotification)
    {
        jobEndNotification();
    }

    /* Notify Sequence end - should be done after scheduling */
    if (NULL != seqEndNotification)
    {
        seqEndNotification();
    }

    return;
}

/*
 * Design: MCAL-6560,MCAL-6657,MCAL-6525,MCAL-6720,MCAL-6711,MCAL-6410,MCAL-6445,MCAL-6559,MCAL-6543,MCAL-6704,MCAL-6577,MCAL-6408,MCAL-6450
 */
static Std_ReturnType Spi_queueJobs(Spi_SeqObjType *seqObj)
{
    uint32             index;
    uint8              currSeqId;
    Std_ReturnType     retVal;
    Spi_JobType        jobId;
    Spi_JobObjType    *jobObj;
    Spi_HwUnitObjType *hwUnitObj;
    Spi_UtilsParams    utilsParams;

    /* Check for start sequence error */
    retVal = Spi_startSeqCheck(seqObj);
    if (((Std_ReturnType) E_OK) == retVal)
    {
        /* Queue all the jobs to the respective hardware queue */
        for (index = 0U; index < seqObj->seqCfg.jobPerSeq; index++)
        {
            jobId  = seqObj->seqCfg.jobList[index];
            jobObj = &Spi_DrvObj.jobObj[jobId];

            hwUnitObj = jobObj->hwUnitObj;

            /* Get current sequence ID which is in progress for this HW unit */
            if (hwUnitObj->curJobObj == NULL)
            {
                /* No sequence in progress. Give invalid ID */
                currSeqId = SPI_MAX_SEQ;
            }
            else
            {
                currSeqId = hwUnitObj->curJobObj->seqObj->seqCfg_PC.seqId;
            }

            /* Queue the job to the hardware queue */
            jobObj->seqObj               = seqObj;
            jobObj->jobResult            = SPI_JOB_QUEUED;
            utilsParams.data             = jobObj;
            utilsParams.priority         = (uint32) jobObj->jobCfg.jobPriority;
            utilsParams.seqId            = seqObj->seqCfg_PC.seqId;
            utilsParams.seqInterruptible = seqObj->seqCfg.seqInterruptible;
            Spi_utilsLinkNodePri(
                &hwUnitObj->jobList,
                &jobObj->nodeObj,
                &utilsParams,
                currSeqId);
        }

        /* Set the states */
        seqObj->seqResult           = SPI_SEQ_PENDING;
        seqObj->numJobsPending      = seqObj->seqCfg.jobPerSeq;
        Spi_DrvObj.isAsynInProgress = (uint32) TRUE;
        Spi_DrvStatus = SPI_BUSY;
    }

    return (retVal);
}

/*
 * Design: MCAL-6721
 */
static void Spi_scheduleJob(Spi_JobObjType *jobObj)
{
    uint32              isIntrMode;
    Spi_ChannelType     chId;
    Spi_ChannelObjType *chObj;
    Spi_HwUnitObjType  *hwUnitObj;

    hwUnitObj = jobObj->hwUnitObj;

    if (hwUnitObj->enabledmaMode == (boolean)TRUE)
    {
        isIntrMode = (uint32) FALSE;
    }
    else
    {
        /*SPI_CoverageGap_14:Dynamic analysis of below false condition is covered in test case ID 4003*/
        if (SPI_POLLING_MODE == Spi_DrvObj.asyncMode)
        {
            isIntrMode = (uint32) FALSE;
        }
        else
        {
            isIntrMode = (uint32) TRUE;
        }
    }
    /* Set the states */
    jobObj->jobResult = SPI_JOB_PENDING;
    jobObj->curChIdx  = 0U;

    /* Log the job */
#if (STD_ON == SPI_JOB_LOG)
    Spi_logJob(jobObj, hwUnitObj);
#endif  /* #if (STD_ON == SPI_JOB_LOG) */

    /* Configure the hardware if previously configured job ID is different */
    if (hwUnitObj->curConfiguredJobId != jobObj->jobCfg_PC.jobId)
    {
        Spi_mcspiConfigJob(hwUnitObj, jobObj);
        hwUnitObj->curConfiguredJobId = jobObj->jobCfg_PC.jobId;
    }

    /* Start the first channel */
    chId  = jobObj->jobCfg.channelList[jobObj->curChIdx];
    chObj = Spi_getCurrChannelObj(chId);
    Spi_mcspiConfigCh(hwUnitObj, jobObj, chObj);

    if (hwUnitObj->enabledmaMode != (boolean)TRUE)
    {
        Spi_mcspiStart(hwUnitObj, jobObj, isIntrMode);
    }
#if (STD_ON == SPI_DMA_ENABLE)
    else
    {
        Spi_DmaTransferStart(hwUnitObj, jobObj, chObj);
    }
#endif
    return;
}

static void Spi_scheduleAllJobsSyncTransmit(
    Spi_SeqObjType *seqObj)
{
    uint32               index;
    Spi_JobObjType      *jobObj;
    Spi_HwUnitObjType   *hwUnitObj;
    Spi_JobType          jobId;
    Spi_JobResultType    jobResult;
    Spi_JobEndNotifyType jobEndNotification = NULL;

    /* Set state of all jobs in sequence to queued */
    for (index = 0U; index < seqObj->seqCfg.jobPerSeq; index++)
    {
        jobId  = seqObj->seqCfg.jobList[index];
        jobObj = &Spi_DrvObj.jobObj[jobId];
        /* Queue the job to the hardware queue */
        jobObj->seqObj = seqObj;
        /* Set the states */
        jobObj->jobResult = SPI_JOB_QUEUED;
    }

    for (index = 0U; index < seqObj->seqCfg.jobPerSeq; index++)
    {
        jobId  = seqObj->seqCfg.jobList[index];
        jobObj = &Spi_DrvObj.jobObj[jobId];
        /* Queue the job to the hardware queue */
        jobObj->seqObj = seqObj;
        /* Set the states */
        jobObj->jobResult = SPI_JOB_PENDING;
        jobObj->curChIdx  = 0U;
        hwUnitObj         = jobObj->hwUnitObj;

#if (STD_ON == SPI_JOB_LOG)
        /* Log the job */
        Spi_logJob(jobObj, hwUnitObj);
#endif  /* #if (STD_ON == SPI_JOB_LOG) */

        hwUnitObj->curJobObj    = jobObj;
        hwUnitObj->hwUnitResult = SPI_HW_UNIT_PENDING;

        /* Configure the hardware if previously configured job ID is
         * different */
        if (hwUnitObj->curConfiguredJobId != jobObj->jobCfg_PC.jobId)
        {
			Spi_mcspiConfigJob(hwUnitObj, jobObj);
            hwUnitObj->curConfiguredJobId = jobObj->jobCfg_PC.jobId;
        }

		jobResult = Spi_mcspiXferJob
						((const Spi_HwUnitObjType *) hwUnitObj, jobObj);

        hwUnitObj->curJobObj    = NULL;
        hwUnitObj->hwUnitResult = SPI_HW_UNIT_OK;

        seqObj->numJobsPending--;
        jobObj->jobResult  = jobResult;
        jobEndNotification = jobObj->jobCfg.Spi_JobEndNotification;
        /* Notify Job end  */
        if (NULL != jobEndNotification)
        {
            jobEndNotification();
        }
        /*SPI_CoverageGap_15:Dynamic coverage cannot be possible as its dependent on the Hardware
         *failure during runtime
         */
        /* Fail the sequence if job fails */
        if (SPI_JOB_FAILED == jobResult)
        {
            seqObj->seqResult = SPI_SEQ_FAILED;
        }
    }

    return;
}

static void Spi_checkAndSetDrvState(void)
{
    uint32 hwUnitIdx, isSomeHwBusy;

    /* Since check is done across HW unit, all interrupts
     * are disabled */
    isSomeHwBusy = (uint32) FALSE;
    for (hwUnitIdx = 0U; hwUnitIdx < (uint32)Spi_DrvObj.maxHwUnit; hwUnitIdx++)
    {
        if (Spi_DrvObj.hwUnitObj[hwUnitIdx].hwUnitResult ==
            SPI_HW_UNIT_PENDING)
        {
            isSomeHwBusy = (uint32) TRUE;
            break;
        }
    }

    if ((((uint32) FALSE)) == isSomeHwBusy)
    {
        Spi_DrvObj.isAsynInProgress = (uint32) FALSE;
        Spi_DrvStatus = SPI_IDLE;
    }

    return;
}

static Std_ReturnType Spi_startSeqCheck(
    const Spi_SeqObjType *seqObj)
{
    uint32          index;
    Std_ReturnType  retVal = (Std_ReturnType) E_OK;
    Spi_JobObjType *jobObj;
    Spi_JobType     jobId;

    /* check all jobs if they are allowed to be queued */
    for (index = 0U; index < seqObj->seqCfg.jobPerSeq; index++)
    {
        jobId  = seqObj->seqCfg.jobList[index];
        jobObj = &Spi_DrvObj.jobObj[jobId];
        if (SPI_JOB_PENDING == jobObj->jobResult)
        {
            /* the sequence is rejected due to common used jobs */
            retVal = (Std_ReturnType) E_NOT_OK;
            break;
        }
    }

    return (retVal);
}

#if (STD_ON == SPI_DEV_ERROR_DETECT)
static Std_ReturnType Spi_checkChConfig(
    const Spi_ConfigType *cfgPtr)
{
    uint32         index;
    Std_ReturnType retVal = (Std_ReturnType) E_OK;

    for (index = 0U; index < (uint32)cfgPtr->maxChannels; index++)
    {
        /* ID is used as index, can't exceed array size */
        if (Spi_ChannelConfig_PC[index].channelId >= cfgPtr->maxChannels)
        {
            Spi_reportDetError(SPI_SID_INIT, SPI_E_PARAM_CHANNEL);
            retVal = (Std_ReturnType) E_NOT_OK;
        }
    }

    return (retVal);
}

static Std_ReturnType Spi_checkJobConfig(
    const Spi_ConfigType *cfgPtr)
{
    uint32         index;
    Std_ReturnType retVal = (Std_ReturnType) E_OK;

    for (index = 0U; index < (uint32)cfgPtr->maxJobs; index++)
    {
        /* ID is used as index, can't exceed array size */
        if ((Spi_JobConfig_PC[index].jobId >= (uint16)cfgPtr->maxJobs) ||
            (Spi_JobConfig_PC[index].externalDeviceCfgId >= (uint8)SPI_MAX_EXT_DEV) ||
            (cfgPtr->jobCfg[index].channelPerJob == 0U) ||
            (cfgPtr->jobCfg[index].channelPerJob >
             SPI_MAX_CHANNELS_PER_JOB )
            )
        {
            Spi_reportDetError(SPI_SID_INIT, SPI_E_PARAM_JOB);
            retVal = (Std_ReturnType) E_NOT_OK;
        }
    }

    return (retVal);
}

static Std_ReturnType Spi_checkSeqConfig(
    const Spi_ConfigType *cfgPtr)
{
    uint32         index;
    Std_ReturnType retVal = (Std_ReturnType) E_OK;

    for (index = 0U; index < (uint32)cfgPtr->maxSeq; index++)
    {
        /* ID is used as index, can't exceed array size */
        if ((Spi_SeqConfig_PC[index].seqId >= cfgPtr->maxSeq) ||
            (cfgPtr->seqCfg[index].jobPerSeq == 0U) ||
            (cfgPtr->seqCfg[index].jobPerSeq > SPI_MAX_JOBS_PER_SEQ))
        {
            Spi_reportDetError(SPI_SID_INIT, SPI_E_PARAM_SEQ);
            retVal = (Std_ReturnType) E_NOT_OK;
        }
    }

    return (retVal);
}

#endif  /* #if (STD_ON == SPI_DEV_ERROR_DETECT) */

#if (STD_ON == SPI_JOB_LOG)
static void Spi_logJob(const Spi_JobObjType    *jobObj,
                       const Spi_HwUnitObjType *hwUnitObj)
{
    Spi_JobLogEntryType *logEntry;
    uint32 timeStamp;
    StatusType status;

    logEntry = &Spi_JobLogObj.logEntry[Spi_JobLogObj.logIndex];
    status = GetCounterValue(SPI_OS_COUNTER_ID, &timeStamp);
    /* SPI_CoverageGap_16:Dynamic coverage of below code for fail condn cant be achievable as its
     * dependent on Hardware 
     */
    if (((StatusType) E_OK) == status)
    {
        logEntry->timeStamp = timeStamp;
        logEntry->jobId     = jobObj->jobCfg_PC.jobId;
        logEntry->seqId     = jobObj->seqObj->seqCfg_PC.seqId;
        logEntry->hwUnitId = hwUnitObj->hwUnitCfg.hwUnitId;

        /* Move to next entry */
        Spi_JobLogObj.logIndex++;
        Spi_JobLogObj.totalLog++;
        if (Spi_JobLogObj.logIndex >= SPI_MAX_JOB_LOG)
        {
            Spi_JobLogObj.logIndex = 0U;
        }
    }
    return;
}

#endif  /* #if (STD_ON == SPI_JOB_LOG) */

/* This API is to get address of current SPI Channel object */
Spi_ChannelObjPtrType Spi_getCurrChannelObj(
    Spi_ChannelType chId)
{
    return (&Spi_DrvObj.channelObj[chId]);
}

#define SPI_STOP_SEC_CODE
#include "Spi_MemMap.h"
