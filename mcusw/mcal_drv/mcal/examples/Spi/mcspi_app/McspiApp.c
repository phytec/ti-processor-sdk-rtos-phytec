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
 *  \file     McspiApp.c
 *
 *  \brief    This file contains the SPI test example for McSPI
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "McspiApp_Startup.h"
#include "McspiApp.h"
/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
uint32                          SpiApp_TestPassed = E_OK;
__attribute__((section(".bss")))
uint32                          SpiApp_McspiTxBuffer[SPI_MAX_CHANNELS]
                                                [RX_TX_BUF_SIZE_WORD];
__attribute__((section(".bss")))
uint32                          SpiApp_McspiRxBuffer[SPI_MAX_CHANNELS]
                                                [RX_TX_BUF_SIZE_WORD];

volatile uint32                 SpiApp_McspiJobDone[SPI_MAX_JOBS +1],
                                SpiApp_McspiSeqDone[SPI_MAX_SEQ +1];


/** \brief SPI Channel PC Configuration struct declaration */
extern const struct Spi_ChannelConfigType_PC_s Spi_ChannelConfig_PC[SPI_MAX_CHANNELS];

/** \brief SPI Job PC Configuration struct declaration */
extern const struct Spi_JobConfigType_PC_s Spi_JobConfig_PC[SPI_MAX_JOBS];

/** \brief SPI Sequence PC Configuration struct declaration */
extern const struct Spi_SeqConfigType_PC_s Spi_SeqConfig_PC[SPI_MAX_SEQ];
/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
int main(void)
{
    uint32              index, chIndex;
    uint32              testPassed = E_OK, matching;
    uint8              *tempTxPtr, *tempRxPtr;
    uint32              byteLength[SPI_MAX_CHANNELS] = {0U};
    uint32              xferLength[SPI_MAX_CHANNELS] = {0U};
    Std_ReturnType      retVal;
    Spi_StatusType      status;
    Spi_SeqResultType   seqResult;
    Spi_JobResultType   jobResult;
    Spi_ChannelType     chId;
    Spi_SequenceType    seqId;
    Spi_JobType         jobId;
    const Spi_ConfigType *cfgPtr = &SpiDriver;
    SpiApp_TestPassed = E_OK;
#if (SPI_VERSION_INFO_API == STD_ON)
    Std_VersionInfoType versioninfo;
#endif  /* #if (SPI_VERSION_INFO_API == STD_ON) */
#if ((SPI_SCALEABILITY == SPI_LEVEL_1) || \
    (SPI_SCALEABILITY == SPI_LEVEL_2))
    uint32              isPending;
#endif

#ifdef UART_ENABLED
    AppUtils_Init();
#endif
    SpiApp_Startup();
    SpiApp_PowerAndClkSrc();

    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                    APP_NAME ": Sample Application - STARTS !!!\n");



#if (SPI_VERSION_INFO_API == STD_ON)
    /* Get and print version */
    Spi_GetVersionInfo(&versioninfo);
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL," \n");
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"SPI MCAL Version Info\n");
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"---------------------\n");
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"Vendor ID           : %d\n", versioninfo.vendorID);
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"Module ID           : %d\n", versioninfo.moduleID);
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"SW Major Version    : %d\n", versioninfo.sw_major_version);
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"SW Minor Version    : %d\n", versioninfo.sw_minor_version);
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"SW Patch Version    : %d\n", versioninfo.sw_patch_version);
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL," \n");
#endif  /* #if (SPI_VERSION_INFO_API == STD_ON) */

/* Print test case information */
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
        APP_NAME ": CH %d: JOBS %d: SEQ %d: Max HWUNIT %d: NUM OF WORDS %d:!!!\n",
        cfgPtr->maxChannels,
        cfgPtr->maxJobs,
        cfgPtr->maxSeq,
        cfgPtr->maxHwUnit,
        SPI_APP_RX_TX_LENGTH);

    /* SPI driver should be free now - check */
    status = Spi_GetStatus();
    if (status != SPI_IDLE)
    {
        testPassed = E_NOT_OK;
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
        APP_NAME ": SPI driver is not IDLE!!\n");
    }

#if (SPI_HW_STATUS_API == STD_ON)
    /* SPI HW unit should be free now - check */
    for (index = 0U; index < cfgPtr->maxHwUnit; index++)
    {
        status =
            Spi_GetHWUnitStatus(cfgPtr->jobCfg[index].hwUnitId);
        if (status != SPI_IDLE)
        {
            testPassed = E_NOT_OK;
            AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                APP_NAME ": SPI HW %d is not IDLE!!\n",
                            cfgPtr->jobCfg[index].hwUnitId);
        }
    }
#endif  /* #if (SPI_HW_STATUS_API == STD_ON) */

#if (STD_OFF == SPI_DMA_ENABLE)
#if (SPI_SCALEABILITY == SPI_LEVEL_2)
    /* Set Async mode */
    /** \brief Default ASYNC/SYNC Transmit flag - TRUE: Async, FALSE: Sync */
    retVal = Spi_SetAsyncMode(SPI_APP_DEFAULT_ASYNC_MODE);
    if (retVal != E_OK)
    {
        testPassed = E_NOT_OK;
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            APP_NAME ": SPI Set Async Mode Failed!!\n");
    }
#endif  /* #if (SPI_SCALEABILITY == SPI_LEVEL_2) */
#endif  /* #if (STD_OFF == SPI_DMA_ENABLE) */

    /* Init test params */
    for (chIndex = 0U; chIndex < SPI_MAX_CHANNELS; chIndex++)
    {
        /* Init TX buffer with known data and memset RX buffer */
        tempTxPtr = (uint8 *) &SpiApp_McspiTxBuffer[chIndex][0U];
        tempRxPtr = (uint8 *) &SpiApp_McspiRxBuffer[chIndex][0U];
        for (index = 0U; index < (RX_TX_BUF_SIZE_WORD * 4U); index++)
        {
            *tempTxPtr++ = (uint8) (index + 1U);
            *tempRxPtr++ = 0U;
        }

        if (cfgPtr->channelCfg[chIndex].dataWidth <= 8U)
        {
            xferLength[chIndex] = SPI_APP_RX_TX_LENGTH;
        }
        else if (cfgPtr->channelCfg[chIndex].dataWidth <= 16U)
        {
            xferLength[chIndex] = SPI_APP_RX_TX_LENGTH / 2;
        }
        else
        {
            xferLength[chIndex] = SPI_APP_RX_TX_LENGTH / 4;
        }
    }

    for (chIndex = 0U; chIndex < cfgPtr->maxChannels; chIndex++)
    {
        /* Memset RX buffer for every run */
        if (cfgPtr->channelCfg[chIndex].dataWidth <= 8U)
        {
            byteLength[chIndex] = xferLength[chIndex];
        }
        else if (cfgPtr->channelCfg[chIndex].dataWidth <= 16U)
        {
            byteLength[chIndex] = xferLength[chIndex] * 2U;
        }
        else
        {
            byteLength[chIndex] = xferLength[chIndex] * 4U;
        }
        tempRxPtr = (uint8 *) &SpiApp_McspiRxBuffer[chIndex][0U];
        for (index = 0U; index < byteLength[chIndex]; index++)
        {
            *tempRxPtr++ = 0U;
        }

        /* SetUp Buffers */
#if ((SPI_CHANNELBUFFERS == SPI_IB_EB) || (SPI_CHANNELBUFFERS == SPI_EB))
        if (SPI_EB == cfgPtr->channelCfg[chIndex].channelBufType)
        {
            Spi_DataBufferType *srcDataBuf, *destDataBuf;

            /* Setup EB for TX/RX */
            srcDataBuf  = (Spi_DataBufferType *) &SpiApp_McspiTxBuffer[chIndex][0U];
            destDataBuf = (Spi_DataBufferType *) &SpiApp_McspiRxBuffer[chIndex][0U];
            chId        = Spi_ChannelConfig_PC[chIndex].channelId;
            retVal      = Spi_SetupEB(
                chId,
                srcDataBuf,
                destDataBuf,
                xferLength[chIndex]);
            if (retVal != E_OK)
            {
                testPassed = E_NOT_OK;
                AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                    APP_NAME ": SPI Setup EB Failed!!\n");
            }
        }
#endif

#if ((SPI_CHANNELBUFFERS == SPI_IB_EB) || (SPI_CHANNELBUFFERS == SPI_IB))
        if (SPI_IB == cfgPtr->channelCfg[chIndex].channelBufType)
        {
            Spi_DataBufferType *srcDataBuf;

            /* Write TX data to IB */
            srcDataBuf = (Spi_DataBufferType *) &SpiApp_McspiTxBuffer[chIndex][0U];
            chId = Spi_ChannelConfig_PC[chIndex].channelId;
            retVal = Spi_WriteIB(chId, srcDataBuf);
            if (retVal != E_OK)
            {
                testPassed = E_NOT_OK;
                AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                    APP_NAME ": SPI Write IB Failed!!\n");
            }
        }
#endif
    }

    /* Reset flags */
    for (index = 0U; index < SPI_MAX_JOBS; index++)
    {
        SpiApp_McspiJobDone[index] = FALSE;
    }

    for (index = 0U; index < SPI_MAX_SEQ; index++)
    {
        SpiApp_McspiSeqDone[index] = FALSE;
    }

#if (STD_ON == SPI_DMA_ENABLE)
    for (chIndex = 0U;
         chIndex < cfgPtr->maxChannels;
         chIndex++)
    {
        SpiApp_wbCache((uint8 *)&SpiApp_McspiTxBuffer[chIndex][0U], RX_TX_BUF_SIZE_WORD * sizeof(SpiApp_McspiTxBuffer[chIndex][0U]));
        SpiApp_wbCache((uint8 *)&SpiApp_McspiRxBuffer[chIndex][0U], RX_TX_BUF_SIZE_WORD * sizeof(SpiApp_McspiRxBuffer[chIndex][0U]));
    }
#endif

/*This app either runs in async mode or sync mode
  Different functions for each*/
#if ((SPI_SCALEABILITY == SPI_LEVEL_1) || \
(SPI_SCALEABILITY == SPI_LEVEL_2))
    if (SPI_INTERRUPT_MODE == SPI_APP_DEFAULT_ASYNC_MODE)
    {
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            APP_NAME ": SPI Async transmit in progress!!\n");
        for (index = 0U; index < cfgPtr->maxSeq; index++)
        {
            /* Start the TX/RX */
            seqId  = Spi_SeqConfig_PC[index].seqId;
            retVal = Spi_AsyncTransmit(seqId);
            if (retVal != E_OK)
            {
                testPassed = E_NOT_OK;
                AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                    APP_NAME ": SPI Async transmit Failed!!\n");
            }
        }
    }
    else
    {

        for (index = 0U; index < cfgPtr->maxSeq; index++)
        {
            /* Start the TX/RX */
            seqId  = Spi_SeqConfig_PC[index].seqId;
            retVal = Spi_SyncTransmit(seqId);
            if (retVal != E_OK)
            {
                testPassed = E_NOT_OK;
                AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                    APP_NAME ": SPI Sync transmit Failed!!\n");
            }
        }
    }

    if (SPI_INTERRUPT_MODE == SPI_APP_DEFAULT_ASYNC_MODE)
    {
        /* Wait for TX/RX to get over */
        while (1U)
        {
            isPending = FALSE;
            for (index = 0U; index < cfgPtr->maxSeq;
                 index++)
            {
                if (SpiApp_McspiSeqDone[index] == FALSE)
                {
                    isPending = TRUE;
                    break;
                }
            }
            for (index = 0U; index < cfgPtr->maxJobs;
                 index++)
            {
                if (SpiApp_McspiJobDone[index] == FALSE)
                {
                    isPending = TRUE;
                    break;
                }
            }
            if (FALSE == isPending)
            {
                break;
            }
        }
    }
    else
    {
        while (1U)
        {
            Spi_MainFunction_Handling();

            isPending = FALSE;
            for (index = 0U; index < cfgPtr->maxSeq;
                 index++)
            {
                seqResult = Spi_GetSequenceResult(index);
                if (seqResult != SPI_SEQ_OK)
                {
                    isPending = TRUE;
                    break;
                }
            }
            for (index = 0U; index < cfgPtr->maxJobs;
                 index++)
            {
                jobResult = Spi_GetJobResult(index);
                if (jobResult != SPI_JOB_OK)
                {
                    isPending = TRUE;
                    break;
                }
            }

            /* All jobs/sequences completed */
            if (FALSE == isPending)
            {
                break;
            }
        }
    }
#endif

    /* SPI driver should be free now - check */
    status = Spi_GetStatus();
    if (status != SPI_IDLE)
    {
        testPassed = E_NOT_OK;
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            APP_NAME ": SPI driver is not IDLE!!\n");
    }
    for (index = 0U; index < cfgPtr->maxSeq; index++)
    {
        seqId     = Spi_SeqConfig_PC[index].seqId;
        seqResult = Spi_GetSequenceResult(seqId);
        if (seqResult != SPI_SEQ_OK)
        {
            if (seqResult == SPI_SEQ_CANCELLED)
            {
                AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                    APP_NAME ": SPI Sequence was cancelled!!\n");
            }
            else
            {
                testPassed = E_NOT_OK;
                AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                    APP_NAME ": SPI Sequence is not OK!!\n");
            }
        }
    }

    for (index = 0U; index < cfgPtr->maxJobs; index++)
    {
        jobId     = Spi_JobConfig_PC[index].jobId;
        jobResult = Spi_GetJobResult(jobId);
        if (jobResult != SPI_JOB_OK)
        {
            testPassed = E_NOT_OK;
            AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                APP_NAME ": SPI Job is not OK!!\n");
        }
    }

#if (STD_ON == SPI_DMA_ENABLE)
    for (chIndex = 0U;
         chIndex < cfgPtr->maxChannels;
         chIndex++)
    {
        SpiApp_invCache((uint8 *)&SpiApp_McspiRxBuffer[chIndex][0U], RX_TX_BUF_SIZE_WORD * sizeof(SpiApp_McspiRxBuffer[chIndex][0U]));
    }
#endif

#if (STD_ON == SPI_REGISTER_READBACK_API)
    for (index = 0U; index < cfgPtr->maxHwUnit; index++)
    {
        Spi_RegisterReadbackType  RegReadback, *RegRbPtr;
        RegRbPtr = &RegReadback;
        retVal = Spi_RegisterReadback(cfgPtr->jobCfg[index].hwUnitId, RegRbPtr);
        if (retVal != E_OK)
        {
            AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                APP_NAME ": SPI configuration Register Readback Failed!!\n");
        }
        else
        {
            AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
                ": SPI Hwunit %d configuration Register Readback values  \n",
                (cfgPtr->jobCfg[index].hwUnitId) );
            AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
                ": MCSPI_HL_REV 			: 0x%x\n", RegRbPtr->mcspiHlRev);
            AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
                ": MCSPI_HL_HWINFO 		: 0x%x\n", RegRbPtr->mcspiHlHwInfo);
            AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
                ": MCSPI_HL_SYSCONFIG 		: 0x%x\n", RegRbPtr->mcspiHlSysConfig);
            AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
                ": MCSPI_REVISION 			: 0x%x\n", RegRbPtr->mcspiRev);
            AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
                ": MCSPI_SYSSTATUS 		: 0x%x\n", RegRbPtr->mcspiSysStatus);
            AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
                ": MCSPI_SYST 			: 0x%x\n", RegRbPtr->mcspiSyst);
            AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
                ": MCSPI_MODULCTRL 		: 0x%x\n", RegRbPtr->mcspiModulctrl);
            AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
                ": MCSPI_SYSCONFIG 		: 0x%x\n", RegRbPtr->mcspiSysConfig);
            AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
                ": MCSPI_CH0CONF 			: 0x%x\n", RegRbPtr->mcspiCh0config);
            AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
                ": MCSPI_CH1CONF 			: 0x%x\n", RegRbPtr->mcspiCh1config);
            AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
                ": MCSPI_CH2CONF 			: 0x%x\n", RegRbPtr->mcspiCh2config);
            AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
                ": MCSPI_CH3CONF 			: 0x%x\n", RegRbPtr->mcspiCh3config);
            AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
                ": MCSPI_IRQENABLE 		: 0x%x\n", RegRbPtr->mcspiIrqenable);
        }
    }
#endif

#if ((SPI_CHANNELBUFFERS == SPI_IB_EB) || (SPI_CHANNELBUFFERS == SPI_IB))
    for (chIndex = 0U; chIndex < cfgPtr->maxChannels; chIndex++)
    {
        if (SPI_IB == cfgPtr->channelCfg[chIndex].channelBufType)
        {
            Spi_DataBufferType *destDataBuf;

            /* Read RX data from IB to app buffer */
            destDataBuf = (Spi_DataBufferType *) &SpiApp_McspiRxBuffer[chIndex][0U];
            chId = Spi_ChannelConfig_PC[chIndex].channelId;
            retVal = Spi_ReadIB(chId, destDataBuf);
            if (retVal != E_OK)
            {
                testPassed = E_NOT_OK;
                AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                    APP_NAME ": SPI Read IB Failed!!\n");
            }
        }
    }
#endif

    /* Check RX buffer against TX buffer */
    matching = TRUE;
    for (chIndex = 0U;
         chIndex < cfgPtr->maxChannels;
         chIndex++)
    {
        tempTxPtr = (uint8 *) &SpiApp_McspiTxBuffer[chIndex][0U];
        tempRxPtr = (uint8 *) &SpiApp_McspiRxBuffer[chIndex][0U];
        for (index = 0U; index < byteLength[chIndex]; index++)
        {
            if (*tempTxPtr++ != *tempRxPtr++)
            {
                matching = FALSE;
            }
        }
    }
    if (TRUE != matching)
    {
        testPassed = E_NOT_OK;
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            APP_NAME ": McSPI data mismatch error!!\n");
    }

    /* DeInit */
    if (testPassed == E_OK)
    {
        retVal = Spi_DeInit();
        if (retVal != E_OK)
        {
            testPassed = E_NOT_OK;
            AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                APP_NAME ": SPI Deinit Failed!!\n");
        }
    }

    if (testPassed == E_OK)
    {
        /* SPI driver should be uninit now - check */
        status = Spi_GetStatus();
        if (status != SPI_UNINIT)
        {
            testPassed = E_NOT_OK;
            AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                APP_NAME ": SPI driver is not in uninit state!!\n");
        }
    }

    if (testPassed == E_OK)
    {
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            APP_NAME ": SPI Stack Usage: %d bytes\n", AppUtils_GetStackUsage());
        if (AppUtils_SpiCheckStackAndSectionCorruption() != E_OK)
        {
            testPassed = E_NOT_OK;
            AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                APP_NAME ": SPI Stack/section corruption!!!\n");
        }
    }

    if (E_OK == testPassed)
    {
        SpiApp_TestPassed = testPassed;
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            APP_NAME ": All tests have passed!!!\n");
    }
    else
    {
        SpiApp_TestPassed = testPassed;
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            APP_NAME ": Some tests have Failed!!!\n");
    }

    return (SpiApp_TestPassed);
}

void SpiApp_McuMcspiJob0EndNotification(void)
{
    SpiApp_McspiJobDone[0U] = TRUE;

    return;
}

void SpiApp_McuMcspiSeq0EndNotification(void)
{
    SpiApp_McspiSeqDone[0U] = TRUE;

    return;
}

void SpiApp_McuMcspi3Job0EndNotification(void)
{
    SpiApp_McspiJobDone[0U] = TRUE;

    return;
}

void SpiApp_McuMcspi3Seq0EndNotification(void)
{
    SpiApp_McspiSeqDone[0U] = TRUE;

    return;
}

void SpiApp_McuMcspiJob1EndNotification(void)
{
    SpiApp_McspiJobDone[1U] = TRUE;

    return;
}

void SpiApp_McuMcspiSeq1EndNotification(void)
{
    SpiApp_McspiSeqDone[1U] = TRUE;

    return;
}

#if defined (SOC_AM62X)
void SpiApp_McspiJob0EndNotification(void)
{
    SpiApp_McspiJobDone[2U] = TRUE;

    return;
}

void SpiApp_McspiSeq0EndNotification(void)
{
    SpiApp_McspiSeqDone[2U] = TRUE;

    return;
}

void SpiApp_McspiJob1EndNotification(void)
{
    SpiApp_McspiJobDone[3U] = TRUE;

    return;
}

void SpiApp_McspiSeq1EndNotification(void)
{
    SpiApp_McspiSeqDone[3U] = TRUE;

    return;
}

void SpiApp_McspiJob2EndNotification(void)
{
    SpiApp_McspiJobDone[4U] = TRUE;

    return;
}

void SpiApp_McspiSeq2EndNotification(void)
{
    SpiApp_McspiSeqDone[4U] = TRUE;

    return;
}

#endif

void SpiApp_wbInvCache(uint8 *buf, uint16 len)
{
#if (STD_ON == SPI_DMA_ENABLE)
     CacheP_wbInv(buf, len);
#endif
}

void SpiApp_wbCache(uint8 *buf, uint16 len)
{
#if (STD_ON == SPI_DMA_ENABLE)
     CacheP_wb(buf, len);
#endif
}

void SpiApp_invCache(uint8 *buf, uint16 len)
{
#if (STD_ON == SPI_DMA_ENABLE)
     CacheP_Inv(buf, len);
#endif
}
