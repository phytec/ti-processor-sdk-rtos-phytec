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
 *  \file     Spi.c
 *
 *  \brief    This file contains SPI MCAL driver
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "string.h"
#include "Spi_Cfg.h"
#include "Spi.h"
#include "Spi_Dbg.h"
#include "Spi_Priv.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* AUTOSAR version information check has to match definition in header file */
#if ((SPI_AR_RELEASE_MAJOR_VERSION != (4U)) || \
    (SPI_AR_RELEASE_MINOR_VERSION != (3U)) ||  \
    (SPI_AR_RELEASE_REVISION_VERSION != (1U)))
    #error "Spi: AUTOSAR Version Numbers of Spi are different!!"
#endif

/* AUTOSAR version information check has to match definition in header file */
#if ((SPI_SW_MAJOR_VERSION != (9U)) || \
    (SPI_SW_MINOR_VERSION != (0U)) ||  \
    (SPI_SW_PATCH_VERSION != (1U)))
    #error "Spi: Software Version Numbers are inconsistent!!"
#endif

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */
#if ((SPI_CHANNELBUFFERS == SPI_IB) || (SPI_CHANNELBUFFERS == SPI_IB_EB))
#if (STD_ON == SPI_DEV_ERROR_DETECT)
static FUNC(Std_ReturnType, SPI_CODE) Spi_WriteIBDetErrCheck(
    Spi_ChannelType Channel,
    P2CONST(Spi_DataBufferType, AUTOMATIC, SPI_APPL_DATA) DataBufferPtr);
#endif /* (STD_ON == SPI_DEV_ERROR_DETECT) */
#endif /* (SPI_CHANNELBUFFERS == SPI_IB) || (SPI_CHANNELBUFFERS == SPI_IB_EB) */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

#define SPI_START_SEC_VAR_INIT_UNSPECIFIED
#include "Spi_MemMap.h"

/** \brief SPI driver status */
/*
 * Design: MCAL-6426
 */

volatile VAR(Spi_StatusType, SPI_VAR_CLEARED) Spi_DrvStatus = SPI_UNINIT;
#define SPI_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "Spi_MemMap.h"

#define SPI_START_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Spi_MemMap.h"
/** \brief SPI driver object */

VAR(Spi_DriverObjType, SPI_VAR_CLEARED) Spi_DrvObj;
#define SPI_STOP_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Spi_MemMap.h"

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

#define SPI_START_SEC_CODE
#include "Spi_MemMap.h"

/*
 * Design: MCAL-6613,MCAL-6620,MCAL-6466,MCAL-6481,MCAL-6654,MCAL-6411,MCAL-6382,MCAL-6530,MCAL-6587,MCAL-6498
 */
FUNC(void, SPI_CODE) Spi_Init(
    P2CONST(Spi_ConfigType, AUTOMATIC, SPI_CONFIG_DATA) CfgPtr)
{
    uint32         index;
    const Spi_ConfigType *ConfigPtr = CfgPtr;
#if (STD_ON == SPI_DEV_ERROR_DETECT)
    Std_ReturnType retVal;
#endif  /* #if (STD_ON == SPI_DEV_ERROR_DETECT) */

#if (STD_ON == SPI_PRE_COMPILE_VARIANT)
    if (NULL_PTR == ConfigPtr)
    {
        ConfigPtr = &SPI_INIT_CONFIG_PC;
    }
#endif  /* (STD_ON == SPI_PRE_COMPILE_VARIANT) */
#if (STD_ON == SPI_DEV_ERROR_DETECT)
    if (SPI_UNINIT != Spi_DrvStatus)
    {
        Spi_reportDetError(SPI_SID_INIT, SPI_E_ALREADY_INITIALIZED);
    }
    else if (NULL_PTR == ConfigPtr)
    {
        Spi_reportDetError(SPI_SID_INIT, SPI_E_PARAM_POINTER);
    }
    else if (ConfigPtr->maxChannels > (uint8)SPI_MAX_CHANNELS)
    {
        Spi_reportDetError(SPI_SID_INIT, SPI_E_PARAM_CHANNEL);
    }
    else if (ConfigPtr->maxJobs > (uint8)SPI_MAX_JOBS)
    {
        Spi_reportDetError(SPI_SID_INIT, SPI_E_PARAM_JOB);
    }
    else if (ConfigPtr->maxSeq > (uint8)SPI_MAX_SEQ)
    {
        Spi_reportDetError(SPI_SID_INIT, SPI_E_PARAM_SEQ);
    }
    else if (ConfigPtr->maxHwUnit > (uint8)SPI_MAX_HW_UNIT)
    {
        Spi_reportDetError(SPI_SID_INIT, SPI_E_PARAM_UNIT);
    }
    else if (ConfigPtr->maxExtDevCfg > (uint8)SPI_MAX_EXT_DEV)
    {
        Spi_reportDetError(SPI_SID_INIT, SPI_E_PARAM_JOB);
    }
    else
#endif  /* #if (STD_ON == SPI_DEV_ERROR_DETECT) */
    {
#if (STD_ON == SPI_DEV_ERROR_DETECT)
        /* Check the configuration */
        retVal = Spi_checkConfig(ConfigPtr);
        if (((Std_ReturnType) E_OK) == retVal)
#endif  /* #if (STD_ON == SPI_DEV_ERROR_DETECT) */
        {
            Spi_resetDrvObj(&Spi_DrvObj);
            Spi_copyConfig(&Spi_DrvObj, ConfigPtr);

            /* Init HW once all config is copied */
            for (index = 0U; index < SPI_MAX_HW_UNIT; index++)
            {
                Spi_hwUnitInit(&Spi_DrvObj.hwUnitObj[index]);
            }

#if (STD_ON == SPI_DMA_ENABLE)
            retVal = Spi_DmaConfig(&Spi_DrvObj, ConfigPtr);
            if (retVal == E_NOT_OK)
            {
                Spi_reportDetError(SPI_SID_INIT, SPI_E_PARAM_JOB);
            }

            if (retVal == E_OK)
#endif
            {
                /* Initialize driver status and object */
                Spi_DrvStatus = SPI_IDLE;
            }
        }
    }

    return;
}

/*
 * Design: MCAL-6605,MCAL-6680,MCAL-6453,MCAL-6653,MCAL-6722,MCAL-6378,MCAL-6568,MCAL-6734,MCAL-6585,MCAL-6490
 */
FUNC(Std_ReturnType, SPI_CODE) Spi_DeInit(void)
{
    uint32         index;
    Std_ReturnType retVal = (Std_ReturnType) E_NOT_OK;

#if (STD_ON == SPI_DEV_ERROR_DETECT)
    if (SPI_UNINIT == Spi_DrvStatus)
    {
        Spi_reportDetError(SPI_SID_DEINIT, SPI_E_UNINIT);
    }
    else
#endif  /* #if (STD_ON == SPI_DEV_ERROR_DETECT) */
    {
        if (SPI_BUSY == Spi_DrvStatus)
        {
            /* Do nothing */
        }
        else
        {
            Spi_DrvStatus = SPI_UNINIT;
            /* Deinit the hardware modules */
            for (index = 0U; index < (uint32)Spi_DrvObj.maxHwUnit; index++)
            {
                Spi_hwUnitDeInit(&Spi_DrvObj.hwUnitObj[index]);
            }
            Spi_resetDrvObj(&Spi_DrvObj);
#if (STD_ON == SPI_DMA_ENABLE)
            /* Deinit the hardware modules */
            /*Dynamic coverage for Spi_DmaFreeChannel cannot be achievable as
            maxHwunit is set to 0 in Spi_resetDrvObj */
            for (index = 0U; index < (uint32)Spi_DrvObj.maxHwUnit; index++)
            {
                Spi_DmaFreeChannel(&Spi_DrvObj.dmaObj[index]);
            }
            (void)Udma_deinit(Spi_DrvObj.drvHandle);
#endif
            retVal        = ((Std_ReturnType) E_OK);
        }
    }

    return (retVal);
}

/*
 * Design: MCAL-6702,MCAL-6582,MCAL-6688,MCAL-6426,MCAL-6714,MCAL-6467,MCAL-6545
 */
FUNC(Spi_StatusType, SPI_CODE) Spi_GetStatus(void)
{
    return (Spi_DrvStatus);
}

/*
 * Design: MCAL-6618,MCAL-6386,MCAL-6626,MCAL-6563,MCAL-6506,MCAL-6585,MCAL-6490
 */
FUNC(Spi_JobResultType, SPI_CODE)
    Spi_GetJobResult(Spi_JobType Job)
{
    Spi_JobResultType jobResult = SPI_JOB_FAILED;

#if (STD_ON == SPI_DEV_ERROR_DETECT)
    if (SPI_UNINIT == Spi_DrvStatus)
    {
        Spi_reportDetError(SPI_SID_GET_JOB_RESULT, SPI_E_UNINIT);
    }
    else if (Job >= (uint16)Spi_DrvObj.maxJobs)
    {
        Spi_reportDetError(SPI_SID_GET_JOB_RESULT, SPI_E_PARAM_JOB);
    }
    else
#endif  /* #if (STD_ON == SPI_DEV_ERROR_DETECT) */
    {
        jobResult = Spi_DrvObj.jobObj[Job].jobResult;
    }

    return  (jobResult);
}

/*
 * Design: MCAL-6459,MCAL-6635,MCAL-6532,MCAL-6696,MCAL-6504,MCAL-6585,MCAL-6444,MCAL-6694,MCAL-6434,MCAL-6490
 */
FUNC(Spi_SeqResultType, SPI_CODE) Spi_GetSequenceResult(
    Spi_SequenceType Sequence)
{
    Spi_SeqResultType seqResult = SPI_SEQ_FAILED;

#if (STD_ON == SPI_DEV_ERROR_DETECT)
    if (SPI_UNINIT == Spi_DrvStatus)
    {
        Spi_reportDetError(SPI_SID_GET_SEQ_RESULT, SPI_E_UNINIT);
    }
    else if (Sequence >= Spi_DrvObj.maxSeq)
    {
        Spi_reportDetError(SPI_SID_GET_SEQ_RESULT, SPI_E_PARAM_SEQ);
    }
    else
#endif  /* #if (STD_ON == SPI_DEV_ERROR_DETECT) */
    {
        seqResult = Spi_DrvObj.seqObj[Sequence].seqResult;
    }

    return (seqResult);
}

#if (STD_ON == SPI_VERSION_INFO_API)
/*
 * Design: MCAL-6562,MCAL-6630,MCAL-6479
 */
FUNC(void, SPI_CODE) Spi_GetVersionInfo(
    P2VAR(Std_VersionInfoType, AUTOMATIC, SPI_APPL_DATA) versioninfo)
{
#if (STD_ON == SPI_DEV_ERROR_DETECT)
    if (NULL_PTR == versioninfo)
    {
        Spi_reportDetError(SPI_SID_GET_VERSION_INFO, SPI_E_PARAM_POINTER);
    }
    else
#endif  /* #if (STD_ON == SPI_DEV_ERROR_DETECT) */
    {
        versioninfo->vendorID         = SPI_VENDOR_ID;
        versioninfo->moduleID         = SPI_MODULE_ID;
        versioninfo->sw_major_version = (uint8) SPI_SW_MAJOR_VERSION;
        versioninfo->sw_minor_version = (uint8) SPI_SW_MINOR_VERSION;
        versioninfo->sw_patch_version = (uint8) SPI_SW_PATCH_VERSION;
    }

    return;
}
#endif  /* #if (STD_ON == SPI_VERSION_INFO_API) */

#if (STD_ON == SPI_HW_STATUS_API)
/*
 * Design: MCAL-6727,MCAL-6599,MCAL-6552,MCAL-6520,MCAL-6640,MCAL-6488,MCAL-6452,MCAL-6741,MCAL-6585,MCAL-6490,MCAL-6431
 */
FUNC(Spi_StatusType, SPI_CODE) Spi_GetHWUnitStatus(
    Spi_HWUnitType HWUnit)
{
    Spi_HwUnitObjType *hwUnitObj;
    Spi_StatusType     hwUnitStatus = SPI_UNINIT;

#if (STD_ON == SPI_DEV_ERROR_DETECT)
    if (SPI_UNINIT == Spi_DrvStatus)
    {
        Spi_reportDetError(SPI_SID_GET_HW_UNIT_STATUS, SPI_E_UNINIT);
    }
    else if (HWUnit >= (uint8)SPI_HW_UNIT_CNT)
    {
        Spi_reportDetError(SPI_SID_GET_HW_UNIT_STATUS, SPI_E_PARAM_UNIT);
    }
    else
#endif  /* #if (STD_ON == SPI_DEV_ERROR_DETECT) */
    {
        SchM_Enter_Spi_SPI_EXCLUSIVE_AREA_0();

        /* HW unit ID may not be the index, so search for matching HW unit */
        hwUnitObj = Spi_getHwUnitObj(HWUnit);
        if (NULL_PTR != hwUnitObj)
        {
            if (SPI_HW_UNIT_OK == hwUnitObj->hwUnitResult)
            {
                hwUnitStatus = SPI_IDLE;
            }
            else
            {
                hwUnitStatus = SPI_BUSY;
            }
        }

        SchM_Exit_Spi_SPI_EXCLUSIVE_AREA_0();
    }

    return (hwUnitStatus);
}
#endif  /* #if (STD_ON == SPI_HW_STATUS_API) */

#if ((SPI_CHANNELBUFFERS == SPI_IB) || (SPI_CHANNELBUFFERS == SPI_IB_EB))
/*
 * Design: MCAL-6477,MCAL-6407,MCAL-6567,MCAL-6473,MCAL-6725,MCAL-6676,MCAL-6564,MCAL-6724
 */
FUNC(Std_ReturnType, SPI_CODE) Spi_WriteIB(
    Spi_ChannelType Channel,
    P2CONST(Spi_DataBufferType, AUTOMATIC, SPI_APPL_DATA) DataBufferPtr)
{
    Std_ReturnType      retVal = (Std_ReturnType) E_OK;
    Spi_ChannelObjType *chObj;
    const uint8        *tempBuf8;
    uint8              *tempIbPtr8;
    const uint16       *tempBuf16;
    uint16             *tempIbPtr16;
    const uint32       *tempBuf32;
    uint32             *tempIbPtr32;
    uint32              index;

#if (STD_ON == SPI_DEV_ERROR_DETECT)
    if ((Std_ReturnType) E_NOT_OK == Spi_WriteIBDetErrCheck(Channel,
        DataBufferPtr))
    {
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else
#endif  /* #if (STD_ON == SPI_DEV_ERROR_DETECT) */
    {
        SchM_Enter_Spi_SPI_EXCLUSIVE_AREA_0();

        chObj = &Spi_DrvObj.channelObj[Channel];
        if (1U == chObj->bufWidth)
        {
            tempIbPtr8 = ((uint8 *) &chObj->txIb[0U]);
            for (index = 0U; index < (uint32)chObj->chCfg.maxBufLength; index++)
            {
                if (NULL_PTR != DataBufferPtr)
                {
                    tempBuf8 = (const uint8 *) DataBufferPtr;
                    tempBuf8 += index;
                }
                else
                {
                    tempBuf8 = ((const uint8 *) &chObj->chCfg.defaultTxData);
                }
                tempIbPtr8[index] = *tempBuf8;
            }
        }
        else if (2U == chObj->bufWidth)
        {
            tempIbPtr16 = ((uint16 *) &chObj->txIb[0U]);
            for (index = 0U; index < (uint32)chObj->chCfg.maxBufLength; index++)
            {
                if (NULL_PTR != DataBufferPtr)
                {
                    tempBuf16 = (const uint16 *) DataBufferPtr;
                    tempBuf16 += index;
                }
                else
                {
                    tempBuf16 = ((const uint16 *) &chObj->chCfg.defaultTxData);
                }
                tempIbPtr16[index] = *tempBuf16;
            }
        }
        else
        {
            tempIbPtr32 = ((uint32 *) &chObj->txIb[0U]);
            for (index = 0U; index < (uint32)chObj->chCfg.maxBufLength; index++)
            {
                if (NULL_PTR != DataBufferPtr)
                {
                    tempBuf32 = (const uint32 *) DataBufferPtr;
                    tempBuf32 += index;
                }
                else
                {
                    tempBuf32 = ((const uint32 *) &chObj->chCfg.defaultTxData);
                }
                tempIbPtr32[index] = *tempBuf32;
            }
        }

        SchM_Exit_Spi_SPI_EXCLUSIVE_AREA_0();
    }

    return  (retVal);
}

/*
 * Design: MCAL-6482,MCAL-6469,MCAL-6464,MCAL-6400,MCAL-6501,MCAL-6447,MCAL-6731,MCAL-6672
 */
FUNC(Std_ReturnType, SPI_CODE) Spi_ReadIB(
    Spi_ChannelType Channel,
    P2VAR(Spi_DataBufferType, AUTOMATIC, SPI_APPL_DATA) DataBufferPointer)
{
    Std_ReturnType      retVal = (Std_ReturnType) E_OK;
    Spi_ChannelObjType *chObj;
    uint8              *tempBuf8;
    uint8              *tempIbPtr8;
    uint16             *tempBuf16;
    uint16             *tempIbPtr16;
    uint32             *tempBuf32;
    uint32             *tempIbPtr32;
    uint32              index;

#if (STD_ON == SPI_DEV_ERROR_DETECT)
    if (SPI_UNINIT == Spi_DrvStatus)
    {
        Spi_reportDetError(SPI_SID_READ_IB, SPI_E_UNINIT);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else if ((Channel >= Spi_DrvObj.maxChannels) ||
             (Spi_DrvObj.channelObj[Channel].chCfg.channelBufType != SPI_IB))
    {
        Spi_reportDetError(SPI_SID_READ_IB, SPI_E_PARAM_CHANNEL);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else if ((NULL_PTR == DataBufferPointer) ||
            (((uint32) DataBufferPointer & 0x03U) != 0U))
    {
        Spi_reportDetError(SPI_SID_READ_IB, SPI_E_PARAM_POINTER);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else
#endif  /* #if (STD_ON == SPI_DEV_ERROR_DETECT) */
    {
        SchM_Enter_Spi_SPI_EXCLUSIVE_AREA_0();

        chObj = &Spi_DrvObj.channelObj[Channel];
        if (1U == chObj->bufWidth)
        {
            tempBuf8 = (uint8 *) DataBufferPointer;
            tempIbPtr8 = ((uint8 *) &chObj->rxIb[0U]);
            for (index = 0U; index < (uint32)chObj->chCfg.maxBufLength; index++)
            {
                tempBuf8[index] = tempIbPtr8[index];
            }
        }
        else if (2U == chObj->bufWidth)
        {
            tempBuf16 = (uint16 *) DataBufferPointer;
            tempIbPtr16 = ((uint16 *) &chObj->rxIb[0U]);
            for (index = 0U; index < (uint32)chObj->chCfg.maxBufLength; index++)
            {
                tempBuf16[index] = tempIbPtr16[index];
            }
        }
        else
        {
            tempBuf32 = (uint32 *) DataBufferPointer;
            tempIbPtr32 = ((uint32 *) &chObj->rxIb[0U]);
            for (index = 0U; index < (uint32)chObj->chCfg.maxBufLength; index++)
            {
                tempBuf32[index] = tempIbPtr32[index];
            }
        }

        SchM_Exit_Spi_SPI_EXCLUSIVE_AREA_0();
    }

    return  (retVal);
}
#endif  /* #if SPI_IB || SPI_IB_EB */

#if ((SPI_CHANNELBUFFERS == SPI_EB) || (SPI_CHANNELBUFFERS == SPI_IB_EB))
/*
 * Design: MCAL-6601,MCAL-6700,MCAL-6713,MCAL-6387,MCAL-6628,MCAL-6417,MCAL-6584,MCAL-6627,MCAL-6665,MCAL-6385,MCAL-6708,MCAL-6638,MCAL-6585,MCAL-6548,MCAL-6490
 */
FUNC(Std_ReturnType, SPI_CODE) Spi_SetupEB(
    Spi_ChannelType Channel,
    P2CONST(Spi_DataBufferType, AUTOMATIC, SPI_APPL_DATA) SrcDataBufferPtr,
    P2VAR(Spi_DataBufferType, AUTOMATIC, SPI_APPL_DATA) DesDataBufferPtr,
    Spi_NumberOfDataType Length)
{
    Std_ReturnType      retVal = (Std_ReturnType) E_OK;
    Spi_ChannelObjType *chObj;

#if (STD_ON == SPI_DEV_ERROR_DETECT)
    if (SPI_UNINIT == Spi_DrvStatus)
    {
        Spi_reportDetError(SPI_SID_SETUP_EB, SPI_E_UNINIT);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else if ((Channel >= Spi_DrvObj.maxChannels) ||
             (Spi_DrvObj.channelObj[Channel].chCfg.channelBufType != SPI_EB))
    {
        Spi_reportDetError(SPI_SID_SETUP_EB, SPI_E_PARAM_CHANNEL);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else
#endif  /* #if (STD_ON == SPI_DEV_ERROR_DETECT) */
    {
        SchM_Enter_Spi_SPI_EXCLUSIVE_AREA_0();

        chObj = &Spi_DrvObj.channelObj[Channel];

#if (STD_ON == SPI_DEV_ERROR_DETECT)
        /* Buffers must always be 32-bit aligned - MCAL-1364 */
        if ((((uint32) SrcDataBufferPtr & 0x03U) != 0U) ||
            (((uint32) DesDataBufferPtr & 0x03U) != 0U))
        {
            Spi_reportDetError(SPI_SID_SETUP_EB, SPI_E_PARAM_POINTER);
            retVal = (Std_ReturnType) E_NOT_OK;
        }

        if (((Std_ReturnType) E_OK) == retVal)
        {
            /* now check length */
            if ((Length == 0U) || (Length > chObj->chCfg.maxBufLength))
            {
                Spi_reportDetError(SPI_SID_SETUP_EB, SPI_E_PARAM_LENGTH);
                retVal = (Std_ReturnType) E_NOT_OK;
            }
        }
#endif  /* #if (STD_ON == SPI_DEV_ERROR_DETECT) */

        if (((Std_ReturnType) E_OK) == retVal)
        {
            chObj->txBufPtr     = SrcDataBufferPtr;
            chObj->rxBufPtr     = DesDataBufferPtr;
            chObj->numWordsTxRx = Length;
            retVal = ((Std_ReturnType) E_OK);
        }

        SchM_Exit_Spi_SPI_EXCLUSIVE_AREA_0();
    }

    return  (retVal);
}
#endif  /* #if ((SPI_CHANNELBUFFERS == SPI_EB) || (SPI_CHANNELBUFFERS ==
         *SPI_IB_EB)) */

#if ((SPI_SCALEABILITY == SPI_LEVEL_1) || (SPI_SCALEABILITY == \
                                                   SPI_LEVEL_2))
/*
 * Design: MCAL-6395,MCAL-6606,MCAL-6721,MCAL-6598,MCAL-6589,MCAL-6525,MCAL-6637,MCAL-6720,MCAL-6524,MCAL-6566,MCAL-6373,MCAL-6645,MCAL-6644,MCAL-6711,MCAL-6410,MCAL-6414,MCAL-6737,MCAL-6695,MCAL-6737,MCAL-6695,MCAL-6629,MCAL-6585,MCAL-6490
 */
FUNC(Std_ReturnType, SPI_CODE) Spi_AsyncTransmit(
    Spi_SequenceType Sequence)
{
    Std_ReturnType retVal = (Std_ReturnType) E_OK;

#if (STD_ON == SPI_DEV_ERROR_DETECT)
    if (SPI_UNINIT == Spi_DrvStatus)
    {
        Spi_reportDetError(SPI_SID_ASYNC_TRANSMIT, SPI_E_UNINIT);
		retVal = (Std_ReturnType) E_NOT_OK;
    }
    else if (Sequence >= Spi_DrvObj.maxSeq)
    {
        Spi_reportDetError(SPI_SID_ASYNC_TRANSMIT, SPI_E_PARAM_SEQ);
		retVal = (Std_ReturnType) E_NOT_OK;
    }
    else
#endif  /* #if (STD_ON == SPI_DEV_ERROR_DETECT) */
    {
     /* SPI_CoverageGap_01:Dynamic coverage for false condition cannot be achievable as retVal
      * is default set to E_OK.
      */
    if (retVal == (Std_ReturnType) E_OK)
    {
			if (Spi_GetSequenceResult(Sequence) == SPI_SEQ_PENDING)
			{
				Spi_reportDetRuntimeError(SPI_SID_ASYNC_TRANSMIT, SPI_E_SEQ_PENDING);
				retVal = (Std_ReturnType) E_NOT_OK;
			}
			else
			{
				/* Call the start sequence API */
				retVal = Spi_startSeqAsync(&(Spi_DrvObj.seqObj[Sequence]));
				if (((Std_ReturnType) E_NOT_OK) == retVal)
				{
					Spi_reportDetRuntimeError(SPI_SID_ASYNC_TRANSMIT, SPI_E_SEQ_PENDING);
				}
			}
		}
    }

    return (retVal);
}
#endif  /* #if ((SPI_SCALEABILITY == SPI_LEVEL_1) ||
         *(SPI_SCALEABILITY == SPI_LEVEL_2)) */

#if (STD_ON == SPI_CANCEL_API)
/*
 * Design: MCAL-6740,MCAL-6631,MCAL-6579,MCAL-6514,MCAL-6739,MCAL-6749,MCAL-6585,MCAL-6490
 */
FUNC(void, SPI_CODE) Spi_Cancel(Spi_SequenceType Sequence)
{
#if (STD_ON == SPI_DEV_ERROR_DETECT)
    if (SPI_UNINIT == Spi_DrvStatus)
    {
        Spi_reportDetError(SPI_SID_CANCEL, SPI_E_UNINIT);
    }
    else if (Sequence >= Spi_DrvObj.maxSeq)
    {
        Spi_reportDetError(SPI_SID_CANCEL, SPI_E_PARAM_SEQ);
    }
    else
#endif  /* #if (STD_ON == SPI_DEV_ERROR_DETECT) */
    {
        Spi_cancelSequence(&Spi_DrvObj.seqObj[Sequence]);
    }

    return;
}
#endif  /* #if (STD_ON == SPI_CANCEL_API) */

#if ((SPI_SCALEABILITY == SPI_LEVEL_0) || (SPI_SCALEABILITY == \
                                                   SPI_LEVEL_2))
/*
 * Design: MCAL-6409,MCAL-6634,MCAL-6508,MCAL-6623,MCAL-6748,MCAL-6523,MCAL-6509,MCAL-6465,MCAL-6371,MCAL-6402,MCAL-6745,MCAL-6706
 */
FUNC(Std_ReturnType, SPI_CODE) Spi_SyncTransmit(
    Spi_SequenceType Sequence)
{
    Std_ReturnType   retVal = (Std_ReturnType) E_OK;
#if (STD_OFF == SPI_SUPPORT_CONCURRENT_SYNC_TRANSMIT)
    Spi_SequenceType i;
#endif

#if (STD_ON == SPI_DEV_ERROR_DETECT)
    if (SPI_UNINIT == Spi_DrvStatus)
    {
        Spi_reportDetError(SPI_SID_SYNC_TRANSMIT, SPI_E_UNINIT);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else if (Sequence >= Spi_DrvObj.maxSeq)
    {
        Spi_reportDetError(SPI_SID_SYNC_TRANSMIT, SPI_E_PARAM_SEQ);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else
#endif  /* #if (STD_ON == SPI_DEV_ERROR_DETECT) */
    {
        /* SPI_CoverageGap_02:Dynamic coverage of below fail condition cannot be achievable as
         * default retVal is always E_OK.
         */
        if (retVal == (Std_ReturnType) E_OK)
		    {
            if (Spi_GetSequenceResult(Sequence) == SPI_SEQ_PENDING)
            {
                Spi_reportDetRuntimeError(SPI_SID_SYNC_TRANSMIT, SPI_E_SEQ_PENDING);
                retVal = (Std_ReturnType) E_NOT_OK;
            }
            else
            {
#if (STD_OFF == SPI_SUPPORT_CONCURRENT_SYNC_TRANSMIT)
                for (i = (Spi_SequenceType) 0;
                     i < (Spi_SequenceType) Spi_DrvObj.maxSeq;
                     i++)
                {
                     if (Spi_GetSequenceResult(i) == SPI_SEQ_PENDING)
                     {
                         Spi_reportDetRuntimeError(SPI_SID_SYNC_TRANSMIT, SPI_E_SEQ_IN_PROCESS);
                         retVal = (Std_ReturnType) E_NOT_OK;
                         break;
                     }
                }
#endif
            }
      }
      if (((Std_ReturnType) E_OK) == retVal)
      {
          SchM_Enter_Spi_SPI_EXCLUSIVE_AREA_0();

          /* Call the start sequence API */
          Spi_SeqObjType *seqObjTmp = &(Spi_DrvObj.seqObj[Sequence]);
          retVal = Spi_startSeqSync(seqObjTmp);

          SchM_Exit_Spi_SPI_EXCLUSIVE_AREA_0();
      }
    }

    return (retVal);
}
#endif

#if (SPI_SCALEABILITY == SPI_LEVEL_2)
/*
 * Design: MCAL-6553,MCAL-6491,MCAL-6510,MCAL-6416,MCAL-6379,MCAL-6617,MCAL-6660,MCAL-6675,MCAL-6405
 */
FUNC(Std_ReturnType, SPI_CODE) Spi_SetAsyncMode(
    Spi_AsyncModeType Mode)
{
    Std_ReturnType retVal = (Std_ReturnType) E_OK;

#if (STD_ON == SPI_DEV_ERROR_DETECT)
    if (SPI_UNINIT == Spi_DrvStatus)
    {
        /* Note: This error return is not mentioned in spec */
        Spi_reportDetError(SPI_SID_SET_ASYNC_MODE, SPI_E_UNINIT);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else
#endif  /* #if (STD_ON == SPI_DEV_ERROR_DETECT) */
    {
        if ((uint32) FALSE == Spi_DrvObj.isAsynInProgress)
        {
            Spi_DrvObj.asyncMode = Mode;
        }
        else
        {
            /* Return error incase any ASYNC transmit is in progress */
            retVal = (Std_ReturnType) E_NOT_OK;
        }
    }

    return (retVal);
}
#endif  /* #if (SPI_SCALEABILITY == SPI_LEVEL_2) */

/*
 * Design: MCAL-6555
 */
FUNC(void, SPI_CODE) Spi_MainFunction_Handling(void)
{
    uint32 hwUnitIdx;
    Spi_HwUnitObjType *hwUnitObj;

#if (STD_ON == SPI_DEV_ERROR_DETECT)
    if (SPI_UNINIT == Spi_DrvStatus)
    {
        /* Note: This error return is not mentioned in spec */
        Spi_reportDetError(SPI_SID_MAINFUNCTION_HANDLING, SPI_E_UNINIT);
    }
    else
#endif  /* #if (STD_ON == SPI_DEV_ERROR_DETECT) */
    {
        /* Check all hardware units for any pending sequence */
        for (hwUnitIdx = 0U; hwUnitIdx < (uint32)Spi_DrvObj.maxHwUnit; hwUnitIdx++)
        {
            hwUnitObj = &Spi_DrvObj.hwUnitObj[hwUnitIdx];
            if ((SPI_POLLING_MODE == Spi_DrvObj.asyncMode) &&
                (NULL_PTR != hwUnitObj->curJobObj))
            {
                Spi_processEvents(hwUnitObj);
            }
        }
    }

    return;
}

#if (STD_ON == SPI_REGISTER_READBACK_API)
/*
 * Design: MCAL-6664,MCAL-6433,MCAL-6586
 */
FUNC(Std_ReturnType, SPI_CODE) Spi_RegisterReadback(
    Spi_HWUnitType HWUnit,
    P2VAR(Spi_RegisterReadbackType, AUTOMATIC, SPI_APPL_DATA) RegRbPtr)
{
    Spi_HwUnitObjType *hwUnitObj;
    Std_ReturnType     retVal = ((Std_ReturnType) E_OK);

    if ((SPI_UNINIT == Spi_DrvStatus) || (NULL_PTR == RegRbPtr))
    {
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else
    {
        (void) memset(RegRbPtr, 0, sizeof (Spi_RegisterReadbackType));

        /* HW unit ID may not be the index, so search for matching HW unit */
        hwUnitObj = Spi_getHwUnitObj(HWUnit);
        if (NULL_PTR != hwUnitObj)
        {
            Spi_mcspiRegReadback(hwUnitObj, RegRbPtr);
        }
        else
        {
            retVal = (Std_ReturnType) E_NOT_OK;
        }
    }

    return (retVal);
}
#endif  /* #if (STD_ON == SPI_REGISTER_READBACK_API) */

#if (STD_ON == SPI_SAFETY_API)
/*
 * Design: MCAL-6538,MCAL-6392
 */
FUNC(Std_ReturnType, SPI_CODE)
    Spi_dataOverflowUnderflowIntrEnable(Spi_HWUnitType HWUnit, uint32 intFlags)
{
    uint32 baseAddr;
    Spi_HwUnitObjType *hwUnitObj;
    Std_ReturnType     retVal = ((Std_ReturnType) E_OK);
    hwUnitObj = Spi_getHwUnitObj(HWUnit);
    if (NULL_PTR != hwUnitObj)
    {
        baseAddr = hwUnitObj->baseAddr;
        McSPIIntEnable(baseAddr,intFlags);
    }
    else
    {
        retVal = (Std_ReturnType) E_NOT_OK;
    }

    return (retVal);
}

/*
 * Design: MCAL-6538,MCAL-6392
 */
FUNC(Std_ReturnType, SPI_CODE)
    Spi_dataOverflowUnderflowIntrDisable(Spi_HWUnitType HWUnit, uint32 intFlags)
{
    uint32 baseAddr;
    Spi_HwUnitObjType *hwUnitObj;
    Std_ReturnType     retVal = ((Std_ReturnType) E_OK);
    hwUnitObj = Spi_getHwUnitObj(HWUnit);
    if (NULL_PTR != hwUnitObj)
    {
        baseAddr = hwUnitObj->baseAddr;
        McSPIIntDisable(baseAddr,intFlags);
    }
    else
    {
        retVal = (Std_ReturnType) E_NOT_OK;
    }

    return (retVal);
}

/*
 * Design: MCAL-6538,MCAL-6392
 */
FUNC(Mcspi_IrqStatusType, SPI_CODE)
    Spi_dataOverflowUnderflowIntrGetStatus(
    Spi_HWUnitType HWUnit, uint32 intFlags)
{
    uint32 baseAddr;
    uint32 regVal;
    Spi_HwUnitObjType *hwUnitObj;
    Mcspi_IrqStatusType  spiIrqstatus    =  SPI_NO_EVENT;
    hwUnitObj = Spi_getHwUnitObj(HWUnit);
    if (NULL_PTR != hwUnitObj)
    {
        baseAddr = hwUnitObj->baseAddr;
        regVal = McSPIIntStatusGet(baseAddr);
        regVal  = regVal & intFlags;
        /* SPI_CoverageGap_03:Dynamic coverage for SPI_EVENT_PENDING is not achievable as driver
         * Design is not support the data over flow and under flow error.
         */
        if (regVal == intFlags)
        {
            spiIrqstatus = SPI_EVENT_PENDING;
        }
        else
        {
            spiIrqstatus = SPI_NO_EVENT;
        }
    }
    else
    {
        spiIrqstatus = SPI_STATUS_READ_FAIL;
        /* SPI_CoverageGap_04:Dynamic coverage for else fail part cant be achievable */
    }

    return (spiIrqstatus);
}

/*
 * Design: MCAL-6538,MCAL-6392
 */
FUNC(Std_ReturnType, SPI_CODE)
    Spi_dataOverflowUnderflowIntrStatusClear(
    Spi_HWUnitType HWUnit, uint32 intFlags)
{
    uint32 baseAddr;
    Spi_HwUnitObjType *hwUnitObj;
    Std_ReturnType     retVal = ((Std_ReturnType) E_OK);
    hwUnitObj = Spi_getHwUnitObj(HWUnit);
    if (NULL_PTR != hwUnitObj)
    {
        baseAddr = hwUnitObj->baseAddr;
        McSPIIntStatusClear(baseAddr, intFlags);
    }
    else
    {
        retVal = (Std_ReturnType) E_NOT_OK;
    }

    return (retVal);
}
#endif /*#if (STD_ON == SPI_SAFETY_API) */

#if ((SPI_CHANNELBUFFERS == SPI_IB) || (SPI_CHANNELBUFFERS == SPI_IB_EB))
#if (STD_ON == SPI_DEV_ERROR_DETECT)
static
    FUNC(Std_ReturnType, SPI_CODE) Spi_WriteIBDetErrCheck(
    Spi_ChannelType Channel,
    P2CONST(Spi_DataBufferType, AUTOMATIC, SPI_APPL_DATA) DataBufferPtr)
{
    Std_ReturnType retVal = (Std_ReturnType) E_OK;
    if (SPI_UNINIT == Spi_DrvStatus)
    {
        Spi_reportDetError(SPI_SID_WRITE_IB, SPI_E_UNINIT);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else if ((Channel >= Spi_DrvObj.maxChannels) ||
             (Spi_DrvObj.channelObj[Channel].chCfg.channelBufType != SPI_IB))
    {
        Spi_reportDetError(SPI_SID_WRITE_IB, SPI_E_PARAM_CHANNEL);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else if (((uint32) DataBufferPtr & 0x03U) != 0U)
    {
        Spi_reportDetError(SPI_SID_WRITE_IB, SPI_E_PARAM_POINTER);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else
    {
        /* Nothing to be done here - MISRA C Fix */
    }

    return retVal;
}
#endif /* (STD_ON == SPI_DEV_ERROR_DETECT) */
#endif /* (SPI_CHANNELBUFFERS == SPI_IB) || (SPI_CHANNELBUFFERS == SPI_IB_EB) */

#define SPI_STOP_SEC_CODE
#include "Spi_MemMap.h"
