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
 *  \file     Can_Mcan.c
 *
 *  \brief    This file contains CAN MCAL driver functions for the MCAN
 *            peripheral
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "Can_Priv.h"
/*
 * Design: MCAL-5965
 */
#include "EcuM_Cbk.h"
#include "Can_Cfg.h"
#include "SchM_Can.h"


#define CAN_START_SEC_CODE
#include "Can_MemMap.h"
#include <hw_include/lldr.h>
#define CAN_STOP_SEC_CODE
#include "Can_MemMap.h"
/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \name Default Configuration params values
 *
 *  Defines for MCAN Controller default Configuration params
 *  @{
 */
/** \brief  Transmit pause enable */
#define MCAN_TRANSMIT_PAUSE_ENABLE          (MCAN_CCCR_TXP_DISABLE)
/** \brief  Edge Filtering during Bus Integration */
#define MCAN_EFBI                           (MCAN_CCCR_EFBI_DISABLE)
/** \brief  Protocol Exception Handling Enable */
#define MCAN_PROT_EXP_HANDLING_ENABLE       (MCAN_CCCR_PXHD_DISABLE)
/** \brief  Emulation Enable */
#define MCAN_EMU_ENABLE                     (MCAN_MCANSS_CTRL_EMUEN_DISABLE)
/** \brief  Emulation Fast Ack */
#define MCAN_EMU_FAST_ACK                   (MCAN_MCANSS_CTRL_EMUFACK_DISABLE)
/** \brief  Clock Stop Fast Ack */
#define MCAN_CLK_STOP_FAST_ACK              (MCAN_MCANSS_CTRL_CLKFACK_DISABLE)
/** \brief  Start value of the Message RAM Watchdog Counter */
#define MCAN_WDC_PRELOAD                    (MCAN_RWD_WDC_DISABLE)
/** \brief  Transmitter Delay Compensation Filter Window Length */
#define MCAN_TRCV_DELAY_COMP_WIN            (0U)
/** \brief  WakeUp Request Enable */
#define MCAN_WAKEUPREQ_ENABLE               (MCAN_MCANSS_CTRL_WAKEUPREQEN_ENABLE)
/** \brief  Auto WakeUp Enable */
#define MCAN_AUTO_WAKEUP_ENABLE             (MCAN_MCANSS_CTRL_AUTOWAKEUP_ENABLE)
/**
 *  \name Elements Size
 *
 *  Defines for MCAN Message RAM Elements Size in Words
 *  @{
 */
/** \brief  Standard ID Filter Element Size */
#define MCAN_MSG_RAM_STD_ELEM_SIZE          (1U)
/** \brief  Extended ID Filter Element Size */
#define MCAN_MSG_RAM_EXT_ELEM_SIZE          (2U)
/** \brief  Tx/Rx Mailbox Size */
#define MCAN_MSG_RAM_TX_RX_ELEM_SIZE        (18U)
/** \brief  Maximum buffer size */
#define MCAN_MAX_BUFFER_SIZE                ((uint8)32U)

#define XTD_MSGID_MASK                   ((uint32)0x1fffffffU)
#define STD_MSGID_MASK                   ((uint32)0x7ffU)
#define STD_MSGID_SHIFT                  (18U)
#define CAN_ID_STD                       (0x0U)
#define CAN_ID_XTD                       (0x1U)
#define CAN_ID_MIXED                     (0x2U)
#define CAN_MSG_ID_TYPE_MASK             ((uint32)0x80000000U)
#define CAN_MSG_ID_TYPE_STD              ((uint32)0x0U)
#define CAN_MSG_ID_TYPE_EXT              ((uint32)0x80000000U)
#define CAN_MSG_TYPE_MASK                ((uint32)0x40000000U)
#define CAN_MSG_TYPE_CLASSIC_CAN         ((uint32)0x0U)
#define CAN_MSG_TYPE_CAN_FD              ((uint32)0x40000000U)


/**
 *  \name Default Interrupt Enable Mask
 *
 *  Defines for MCAN Controller default Interrupt Mask
 *  NOTE: User can change this mask
 *  @{
 */
#define MCAN_INTR_MASK       ((uint32)(uint32)MCAN_INTR_SRC_RX_FIFO0_NEW_MSG | \
                              (uint32)MCAN_INTR_SRC_RX_FIFO1_NEW_MSG |  \
                              (uint32)MCAN_INTR_SRC_RX_FIFO0_MSG_LOST |  \
                              (uint32)MCAN_INTR_SRC_TRANS_COMPLETE |  \
                              (uint32)MCAN_INTR_SRC_DEDICATED_RX_BUFF_MSG |  \
                              (uint32)MCAN_INTR_SRC_BUS_OFF_STATUS)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */
static void Can_mcanSetUpRxMailboxForMixedIdType(
    Can_FdMsgRAMConfigObjType *msgRamConfig, const Can_MailboxType *mailboxCfg,
    uint32 hwFilterIdx);
static void Can_mcanSetUpRxMailboxForMixedIdExtType(
    Can_FdMsgRAMConfigObjType *msgRamConfig, const Can_MailboxType *mailboxCfg,
    uint32 hwFilterIdx);
static void Can_mcanInit(MCAN_InitParams *initParams,
                  const Can_ControllerType *canControllerCfg,
                  const Can_ControllerType_PC *canControllerCfg_PC);

static void Can_mcanConfig(const Can_ControllerObjType *controllerObj,
                    const Can_ControllerType *canControllerCfg,
                    uint32 baseAddr);

static void Can_mcanSetUpController(uint32 baseAddr);

static void Can_mcanMsgRAMConfigInit(Can_FdMsgRAMConfigObjType *canFDMsgRamConfig);

static void Can_mcanSetUpMSGRAM(
						 Can_ControllerObjType *canController,
                         const Can_MailboxObjType *canMailbox,
                         uint32 maxMbCnt);

static void Can_mcanSetUpTxMailboxBuf(Can_FdMsgRAMConfigObjType *msgRamConfig,
                            const Can_MailboxType *mailboxCfg,
							Can_ControllerObjType *canController,
                            uint32 htrh,
                            uint32 baseAddr);
static void Can_mcanSetUpTxMailboxFifoPoll(
    Can_FdMsgRAMConfigObjType *msgRamConfig,
    const Can_MailboxType *mailboxCfg,
    Can_ControllerObjType *canController,
    uint32 htrh,
    uint32 baseAddr);
static void Can_mcanSetUpTxMailboxFifoIntr(
    Can_FdMsgRAMConfigObjType *msgRamConfig,
    const Can_MailboxType *mailboxCfg,
    Can_ControllerObjType *canController,
    uint32 htrh,
    uint32 baseAddr);
static void Can_mcanSetUpRxMailbox(Can_FdMsgRAMConfigObjType *msgRamConfig,
                            const Can_MailboxType *mailboxCfg,
                            uint32 htrh);

static void Can_mcanMsgRAMConfig(uint32 baseAddr,
                          Can_FdMsgRAMConfigObjType *canFDMsgRamConfig);


static void Can_mcanInitInterrupt(uint32 baseAddr);

static void Can_mcanConfigBaudRate(const Can_ControllerObjType *controllerObj,
                                   const Can_ControllerType *controllerCfg,
                                   uint32 baseAddr);

static void Can_mcanSetId(const Can_PduType *PduInfo, MCAN_TxBufElementNoCpy *elem);

static void Can_mcanRamInitDone(uint32 baseAddr);
static void Can_mcanCancelAllPendingMessages(
    uint32 baseAddr);
static void Can_mcanSetUpMbxList(
    Can_ControllerObjType *canController,
    const Can_MailboxObjType *canMailbox,
    uint32 maxMbCnt);
static void Can_mcanSetUpMbxListFIFO(
    Can_ControllerObjType *canController,
    const Can_MailboxObjType *canMailbox,
    uint32 maxMbCnt);
#if (CAN_RX_POLLING == STD_ON)
static void Can_mcanReadRxBuffPolling(
    Can_ControllerObjType *controllerObj);
#endif
static void Can_mcanReadRxBuffInterrupt(
    Can_ControllerObjType *controllerObj);
static void Can_mcanSetUpMbxListInitInterrupt(
    Can_ControllerObjType *canController,
    const Can_MailboxObjType *canMailbox,
    uint32 maxMbCnt);
static void Can_mcanSetUpMbxListRx(
	Can_ControllerObjType *canController,
	const Can_MailboxType *mailboxCfg, uint32 htrh);
static void Can_mcanSetUpMbxListTx(
	Can_ControllerObjType *canController,
	const Can_MailboxType *mailboxCfg, uint32 htrh);
static void Can_mcanSetUpMbxListFIFORx(
	Can_ControllerObjType *canController,
	const Can_MailboxType *mailboxCfg, uint32 htrh);
static void Can_mcanSetUpMbxListFIFOTxIntr(
	Can_ControllerObjType *canController,
	const Can_MailboxType *mailboxCfg, uint32 htrh);
static void Can_mcanSetUpMbxListFIFOTxPoll(
	Can_ControllerObjType *canController,
	const Can_MailboxType *mailboxCfg, uint32 htrh);
static void Can_mcanSetUpRxMailboxUpdateFifoSize(
    Can_FdMsgRAMConfigObjType *msgRamConfig,
    const Can_MailboxType *mailboxCfg);
static void Can_mcanSetUpRxMailboxBuf(
    Can_FdMsgRAMConfigObjType *msgRamConfig,
    const Can_MailboxType *mailboxCfg,
    uint32 hwFilterIdx);
static void Can_mcanSetUpRxMailboxStdIdFifo(
    Can_FdMsgRAMConfigObjType *msgRamConfig,
    const Can_MailboxType *mailboxCfg,
    uint32 htrh,
    uint32 hwFilterIdx);
static void Can_mcanSetUpRxMailboxExtIdFifo(
    Can_FdMsgRAMConfigObjType *msgRamConfig,
    const Can_MailboxType *mailboxCfg,
    uint32 htrh,
    uint32 hwFilterIdx);
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
#define CAN_START_SEC_CONST_8
#include "Can_MemMap.h"

static const uint8 Can_McanDataSize[16] =
                                        {0U,  1U,  2U,  3U,
                                        4U, 5U,  6U, 7U,
                                        8U, 12U, 16U, 20U,
                                        24U, 32U, 48U, 64U};

#define CAN_STOP_SEC_CONST_8
#include "Can_MemMap.h"

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

#define CAN_START_SEC_CODE
#include "Can_MemMap.h"

void Can_hwUnitConfig(
	Can_DriverObjType *drvObj,
    Can_ControllerObjType *canController,
    const Can_MailboxObjType *canMailbox,
    uint32 maxMbCnt)
{
    uint32 baseAddr;
    const Can_ControllerType *canControllerCfg;
    const Can_ControllerType_PC *canControllerCfg_PC;
    MCAN_InitParams initParams;

    canControllerCfg = &canController->canControllerConfig;
    canControllerCfg_PC = &canController->canControllerConfig_PC;

    baseAddr = canControllerCfg_PC->CntrAddr;

    /* Checks the initialization of Message RAM */
    Can_mcanRamInitDone(baseAddr);

    /* Set Up Can Controller */
    Can_mcanSetUpController(baseAddr);

    /* Initialize Init params */
    Can_mcanInit(&initParams, canControllerCfg, canControllerCfg_PC);

    /* Configure MCAN */
    Can_mcanConfig(canController, canControllerCfg, baseAddr);

    /* Build Mail Box list */
    Can_mcanSetUpMbxListInitInterrupt(canController, canMailbox, maxMbCnt);
}

static void Can_mcanSetUpMbxListInitInterrupt(
    Can_ControllerObjType *canController,
    const Can_MailboxObjType *canMailbox,
    uint32 maxMbCnt)
{
    uint32 baseAddr;
    const Can_ControllerType_PC *canControllerCfg_PC;

    canControllerCfg_PC = &canController->canControllerConfig_PC;

    baseAddr = canControllerCfg_PC->CntrAddr;

    /* Build Mail Box list */
    Can_mcanSetUpMbxList(canController, canMailbox, maxMbCnt);

	Can_mcanSetUpMbxListFIFO(canController, canMailbox, maxMbCnt);

    /* Perform the initialization of Message RAM and the mailboxes */
    Can_mcanSetUpMSGRAM(canController, canMailbox, maxMbCnt);

    /* Interrupt is enabled only if either RX or TX is in Interrupt mode */
    if ((canControllerCfg_PC->RxProcessingType == CAN_TX_RX_PROCESSING_INTERRUPT) ||
        (canControllerCfg_PC->TxProcessingType == CAN_TX_RX_PROCESSING_INTERRUPT) ||
		(canController->txMixedModeIntrEnable == (boolean)TRUE) ||
        (canController->rxMixedModeIntrEnable == (boolean)TRUE))
    {
        Can_mcanInitInterrupt(baseAddr);
    }
}


boolean Can_mcanStart(
    Can_ControllerObjType *controllerObj)
{
    StatusType  timeOutStatus;
    TickType    startCount = 0U, tempCount = 0U, elapsedCount = 0U;
    boolean status = (boolean) FALSE;
    MCAN_ProtocolStatus errStatus;
    uint32  baseAddr;
    errStatus.busOffStatus = MCAN_PSR_BO_NO_BUS_OFF;

    baseAddr = controllerObj->canControllerConfig_PC.CntrAddr;
    /* Put MCAN in operational mode */
    MCAN_setOpMode(baseAddr, MCAN_OPERATION_MODE_NORMAL);
    timeOutStatus = GetCounterValue(CAN_OS_COUNTER_ID, &startCount);
    if (((StatusType) E_OK) == timeOutStatus)
    {
        /* Wait for FSM to to come out of reset */
        do
        {
            /* Below API can change start time, so use temp variable */
            tempCount = startCount;
            timeOutStatus = GetElapsedValue(
                         CAN_OS_COUNTER_ID,
                         &tempCount,
                         &elapsedCount);
            if ((((StatusType) E_OK) != timeOutStatus) ||
                (elapsedCount >= CAN_TIMEOUT_DURATION))
            {
              /* CAN_CoverageGap_01: Dynamic coverage for the below DEM event is not covered.
               * Since it is hardware dependent.
               */

                /* timeout */
#if (CAN_E_HARDWARE_ERROR != CAN_DEM_NO_EVENT)
                (void)Dem_SetEventStatus(
                    CAN_E_HARDWARE_ERROR, DEM_EVENT_STATUS_FAILED);
#endif
                break;
            }
        } while (MCAN_OPERATION_MODE_NORMAL !=
                    (MCAN_OperationMode)MCAN_getOpMode(baseAddr));
    }

    controllerObj->canState = CAN_CS_STARTED;
    CanIf_ControllerModeIndication(
                            controllerObj->canControllerConfig_PC.ControllerId,
                            CAN_CS_STARTED);
    status = (boolean) TRUE;
    /* Check if there is a busoff */
    MCAN_getProtocolStatus(baseAddr, &errStatus);

    /* CAN_CoverageGap_02: Dynamic coverage for the below "if" statement is not covered.
     * Since BusOff is not tested.
     */

    if (MCAN_PSR_BO_BUS_OFF == errStatus.busOffStatus)
    {
        /* BusOff Recovery sequence has started */
        controllerObj->canBusOffRecoveryStatus = (boolean) TRUE;
    }

    return status;
}

boolean Can_mcanStop(
    Can_ControllerObjType * controllerObj)
{
    TickType    startCount = 0U, tempCount = 0U, elapsedCount = 0U;
    boolean timeOutStatus = (boolean) FALSE;
    boolean status = (boolean) FALSE;
    uint32 baseAddr;

    baseAddr = controllerObj->canControllerConfig_PC.CntrAddr;
    /* Put MCAN in SW initialization mode */
    MCAN_setOpMode(baseAddr, MCAN_OPERATION_MODE_SW_INIT);
    timeOutStatus = GetCounterValue(CAN_OS_COUNTER_ID, &startCount);
    if (((StatusType) E_OK) == timeOutStatus)
    {
        /* Wait for FSM to to come out of reset */
        do
        {
            /* Below API can change start time, so use temp variable */
            tempCount = startCount;
            timeOutStatus = GetElapsedValue(
                         CAN_OS_COUNTER_ID,
                         &tempCount,
                         &elapsedCount);
            if ((((StatusType) E_OK) != timeOutStatus) ||
                (elapsedCount >= CAN_TIMEOUT_DURATION))
            {

              /* CAN_CoverageGap_03: Dynamic coverage for the below DEM event is not covered.
               * Since it is hardware dependent.
               */

                /* timeout */
#if (CAN_E_HARDWARE_ERROR != CAN_DEM_NO_EVENT)
                (void)Dem_SetEventStatus(
                    CAN_E_HARDWARE_ERROR, DEM_EVENT_STATUS_FAILED);
#endif
                break;
            }
        }while (MCAN_OPERATION_MODE_SW_INIT != MCAN_getOpMode(baseAddr));
    }

    /* Cancel all pending messages */
    Can_mcanCancelAllPendingMessages(baseAddr);

    controllerObj->canState = CAN_CS_STOPPED;
    CanIf_ControllerModeIndication(
                            controllerObj->canControllerConfig_PC.ControllerId,
                            CAN_CS_STOPPED);
    status = (boolean) TRUE;
    controllerObj->canBusOffRecoveryStatus = (boolean) FALSE;

    return status;
}

boolean Can_mcanSleep(
    Can_ControllerObjType * controllerObj)
{
    boolean status = (boolean) FALSE;

    controllerObj->canState = CAN_CS_SLEEP;
    CanIf_ControllerModeIndication(
                            controllerObj->canControllerConfig_PC.ControllerId,
                            CAN_CS_SLEEP);
    status = (boolean) TRUE;
    controllerObj->canBusOffRecoveryStatus =
        (boolean) FALSE;

    return status;
}

Std_ReturnType Can_mcanTestLoopBackModeEnable(
    uint32 baseAddr, uint8 mode)
{
    StatusType  status;
    TickType    startCount = 0U, tempCount = 0U, elapsedCount = 0U;
    VAR(Std_ReturnType, AUTOMATIC) retVal = (Std_ReturnType) E_NOT_OK;

    /* Put MCAN in SW initialization mode */
    MCAN_setOpMode(baseAddr, MCAN_OPERATION_MODE_SW_INIT);
    status = GetCounterValue(CAN_OS_COUNTER_ID, &startCount);
    if (((StatusType) E_OK) == status)
    {
        /* Wait for FSM to to come out of reset */
        do
        {
            /* Below API can change start time, so use temp variable */
            tempCount = startCount;
            status = GetElapsedValue(
                         CAN_OS_COUNTER_ID,
                         &tempCount,
                         &elapsedCount);
            if ((((StatusType) E_OK) != status) ||
                (elapsedCount >= CAN_TIMEOUT_DURATION))
            {
#if (CAN_E_HARDWARE_ERROR != CAN_DEM_NO_EVENT)
                (void)Dem_SetEventStatus(
                    CAN_E_HARDWARE_ERROR, DEM_EVENT_STATUS_FAILED);
#endif
                break;
            }
        } while (MCAN_OPERATION_MODE_SW_INIT != MCAN_getOpMode(baseAddr));
    }
    if (0U == mode)
    {
        /* Enable Digital Loopback */
        MCAN_lpbkModeEnable(baseAddr,
            (uint32)MCAN_LPBK_MODE_INTERNAL, (uint32)TRUE);
        retVal = (Std_ReturnType) E_OK;
    }
    else
    {
        /* Enable Analog Loopback */
        MCAN_lpbkModeEnable(baseAddr,
            (uint32)MCAN_LPBK_MODE_EXTERNAL, (uint32)TRUE);
        retVal = (Std_ReturnType) E_OK;
    }
    /* Put MCAN in normal mode of operation */
    MCAN_setOpMode(baseAddr, MCAN_OPERATION_MODE_NORMAL);
    status = GetCounterValue(CAN_OS_COUNTER_ID, &startCount);
    if (((StatusType) E_OK) == status)
    {
        /* Wait for FSM to to come out of reset */
        do
        {
            /* Below API can change start time, so use temp variable */
            tempCount = startCount;
            status = GetElapsedValue(
                         CAN_OS_COUNTER_ID,
                         &tempCount,
                         &elapsedCount);
            if ((((StatusType) E_OK) != status) ||
                (elapsedCount >= CAN_TIMEOUT_DURATION))
            {
#if (CAN_E_HARDWARE_ERROR != CAN_DEM_NO_EVENT)
                (void)Dem_SetEventStatus(
                    CAN_E_HARDWARE_ERROR, DEM_EVENT_STATUS_FAILED);
#endif
                break;
            }
        } while (MCAN_OPERATION_MODE_NORMAL != MCAN_getOpMode(baseAddr));
    }
    else
    {
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    return retVal;
}

Std_ReturnType Can_mcanTestLoopBackModeDisable(
    uint32 baseAddr, uint8 mode)
{
    StatusType  status;
    TickType    startCount = 0U, tempCount = 0U, elapsedCount = 0U;
    VAR(Std_ReturnType, AUTOMATIC) retVal = (Std_ReturnType) E_NOT_OK;

    /* Put MCAN in SW initialization mode */
    MCAN_setOpMode(baseAddr, MCAN_OPERATION_MODE_SW_INIT);
    status = GetCounterValue(CAN_OS_COUNTER_ID, &startCount);
    if (((StatusType) E_OK) == status)
    {
        /* Wait for FSM to to come out of reset */
        do
        {
            /* Below API can change start time, so use temp variable */
            tempCount = startCount;
            status = GetElapsedValue(
                         CAN_OS_COUNTER_ID,
                         &tempCount,
                         &elapsedCount);
            if ((((StatusType) E_OK) != status) ||
                (elapsedCount >= CAN_TIMEOUT_DURATION))
            {
#if (CAN_E_HARDWARE_ERROR != CAN_DEM_NO_EVENT)
                (void)Dem_SetEventStatus(
                    CAN_E_HARDWARE_ERROR, DEM_EVENT_STATUS_FAILED);
#endif
                break;
            }
        } while (MCAN_OPERATION_MODE_SW_INIT != MCAN_getOpMode(baseAddr));
    }
    if (0U == mode)
    {
        /* Enable Digital Loopback */
        MCAN_lpbkModeEnable(baseAddr,
            (uint32)MCAN_LPBK_MODE_INTERNAL, (uint32)FALSE);
        retVal = (Std_ReturnType) E_OK;
    }
    else
    {
        /* Enable Analog Loopback */
        MCAN_lpbkModeEnable(baseAddr,
            (uint32)MCAN_LPBK_MODE_EXTERNAL, (uint32)FALSE);
        retVal = (Std_ReturnType) E_OK;
    }
    /* Put MCAN in normal mode of operation */
    MCAN_setOpMode(baseAddr, MCAN_OPERATION_MODE_NORMAL);
    status = GetCounterValue(CAN_OS_COUNTER_ID, &startCount);
    if (((StatusType) E_OK) == status)
    {
        /* Wait for FSM to to come out of reset */
        do
        {
            /* Below API can change start time, so use temp variable */
            tempCount = startCount;
            status = GetElapsedValue(
                         CAN_OS_COUNTER_ID,
                         &tempCount,
                         &elapsedCount);
            if ((((StatusType) E_OK) != status) ||
                (elapsedCount >= CAN_TIMEOUT_DURATION))
            {
#if (CAN_E_HARDWARE_ERROR != CAN_DEM_NO_EVENT)
                (void)Dem_SetEventStatus(
                    CAN_E_HARDWARE_ERROR, DEM_EVENT_STATUS_FAILED);
#endif
                break;
            }
        } while (MCAN_OPERATION_MODE_NORMAL != MCAN_getOpMode(baseAddr));
    }
    else
    {
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    return retVal;
}

Std_ReturnType Can_writeGetFreeMsgObj(
    const Can_MailboxType *mailboxCfg,
    const Can_ControllerObjType *canController,
    uint32 *msgObj,
    uint32 htrh)
{
    Std_ReturnType retVal = E_OK;
    uint32         messageBox, baseAddr;
    uint32         bitIndex;
    const Can_FdMsgRAMConfigObjType *canFDMsgRamConfig;
    uint8          canTxStatus;
#if (CAN_MULTIPLEXED_TRANSMISSION_ENABLE == STD_ON)
    uint32    loopCnt, fifoNum;
    Can_TxRxProcessingType txProcessingType;
#endif

    canFDMsgRamConfig = &canController->canFDMsgRamConfig;
    baseAddr = mailboxCfg->Controller->CntrAddr;

    /* Among 32 Tx mailboxes, Software allocates mailboxes in following
       sequential order similar to the MCAN IP.
       First it allocates for total number of buffers, then it allocates
       for FIFO in interrupt Mode and then for Polling Mode */
#if (CAN_MULTIPLEXED_TRANSMISSION_ENABLE == STD_ON)
    if(mailboxCfg->CanHwObjectCount > 1U)
    {
        txProcessingType = mailboxCfg->Controller->TxProcessingType;
        if ((txProcessingType == CAN_TX_RX_PROCESSING_POLLING) ||
            ((txProcessingType == CAN_TX_RX_PROCESSING_MIXED) &&
            (mailboxCfg->CanHardwareObjectUsesPolling == (boolean)TRUE)))
        {
            loopCnt = (uint32)(canFDMsgRamConfig->txBuffNum) +
                        canController->canControllerTxType.totalFifoCntIntrMode;
            fifoNum = (uint32)(canFDMsgRamConfig->txBuffNum) +
                        canController->canControllerTxType.totalFifoCntIntrMode +
                        canController->canControllerTxType.totalFifoCntPollMode;
        }
        else
        {
            loopCnt = (uint32)(canFDMsgRamConfig->txBuffNum);
            fifoNum = (uint32)(canFDMsgRamConfig->txBuffNum) +
                        canController->canControllerTxType.totalFifoCntIntrMode;
        }
        for (; loopCnt < (uint32)fifoNum; loopCnt++)
        {
            canTxStatus = canController->canTxStatus[loopCnt];
            bitIndex = (uint32)((uint32)1U << loopCnt);
            if(bitIndex == (MCAN_txBufCancellationStatus(baseAddr) & bitIndex))
            {
				canTxStatus = 0U;
            }

            /* CAN_CoverageGap_04: Dynamic coverage for the below "else if" statement is not covered.
             * Since this is an additional CAN_BUSY check.
             */

            else if (canTxStatus == 1U)
            {
                retVal = (Std_ReturnType)CAN_BUSY;
            }
             /* CAN_CoverageGap_05: Dynamic coverage for the below "else" statement is not covered.
             * Since this is an additional CAN_BUSY check.
             */
            else
            {
                *msgObj = loopCnt;
                retVal = E_OK;
                break;
            }
        }
    }
    else
#endif
    {
        for(messageBox = 0U ;
            messageBox < (uint32) (canFDMsgRamConfig->txBuffNum) ;
            messageBox++)
        {
            if(htrh == canFDMsgRamConfig->txMbMapping[0U][messageBox])
            {
                break;
            }
        }
        if(messageBox < (uint32) canFDMsgRamConfig->txBuffNum)
        {
            canTxStatus = canController->canTxStatus[messageBox];
            bitIndex = ((uint32)1U << messageBox);

            /* CAN_CoverageGap_06: Dynamic coverage for the below "if" statement is not
             * covered. Since the number of Tx messages configured are less, so
             * there is no pending messages.
             */

            /*- Check for pending message:
             *     - pending message, return CAN_BUSY
             *     - no pending message, start new transmission
             */
            if(bitIndex == (MCAN_txBufCancellationStatus(baseAddr) & bitIndex))
            {
                canTxStatus = 0U;
            }
             /* CAN_CoverageGap_07: Dynamic coverage for the below "else if" statement is not
             * covered. Since the number of Tx messages configured are less, so
             * there is no pending messages.
             */
            else if (canTxStatus == 1U)
            {
                retVal = (Std_ReturnType)CAN_BUSY;
            }
            else
            {
                *msgObj = messageBox;
                retVal = E_OK;
            }
        }
        else
        {
            retVal = E_OK;
        }
    }

    return retVal;
}

void Can_writeTxMailbox(
    const Can_MailboxType *mailboxCfg,
    Can_ControllerObjType *controllerObj,
	uint32   hth,
    uint32   messageBox,
    const Can_PduType *pduInfo)
{
    uint32         loopCnt, idx = 0U, baseAddr;
    uint32         memType;
    MCAN_TxBufElementNoCpy elem = {0U};

    baseAddr = mailboxCfg->Controller->CntrAddr;
    /* Frame Tx message element, unused fields like rtr, esi of
     * MCAN_TxBufElementNoCpy is set to 0 during initialization.
     * Classic CAN is taken care during initialization of elem strutcure to 0 */
    if(CAN_MSG_TYPE_CAN_FD == (pduInfo->id & CAN_MSG_TYPE_MASK))
	{
        elem.brs = 1U;
        elem.fdf = 1U;
    }
    Can_mcanSetId(pduInfo, &elem);

    /* Copy payload into message element */
    elem.data = pduInfo->sdu;
    if (elem.data != NULL)
    {
        for(loopCnt = 15U ; loopCnt >= 1U ; loopCnt--)
        {
            if ((pduInfo->length <= Can_McanDataSize[loopCnt]) &&
			    (pduInfo->length > Can_McanDataSize[loopCnt - 1U]))
            {
                idx = loopCnt;
                break;
            }
        }
        /* If the above loop exits w/o hitting the if condition then the
           payload is 0 Bytes and idx = 0. As idx = 0 from the decalration,
           hence no need to handle that case separately. */
        elem.dlc = idx;

        /* Add padding value to unused to data in payload */
        loopCnt = (uint32)pduInfo->length;
        for(; loopCnt < (uint32) Can_McanDataSize[idx] ; loopCnt++)
        {
            elem.data[loopCnt] = mailboxCfg->CanFdPaddingValue;
        }
    }

    memType = (uint32)MCAN_MEM_TYPE_BUF;
    MCAN_writeMsgRamNoCpy(baseAddr, memType, messageBox, &elem);
    (void) MCAN_txBufAddReq(baseAddr, messageBox);
}

void Can_mcanSetBaudrate(
    uint32 baseAddr, const Can_BaudConfigType *setBaud)
{
    MCAN_BitTimingParams configParams;
    uint8 tmpVal;
    int32_t status;

    configParams.nomRatePrescalar = ((uint32)setBaud->BrpValue);
    configParams.nomRatePrescalar -= 1U;

    /* Setup bit timing */
    tmpVal = (setBaud->Pseg1 + setBaud->PropSeg) - 1U;
    configParams.nomTimeSeg1 = (uint32)tmpVal;
    configParams.nomTimeSeg2 = (uint32)((uint32)setBaud->Pseg2 - 1U);
    configParams.nomSynchJumpWidth = (uint32)((uint32)setBaud->Sjw - 1U);
    tmpVal =       (setBaud->BaudFdRateConfig.Pseg1 +
                    setBaud->BaudFdRateConfig.PropSeg) - 1U;
    configParams.dataTimeSeg1 = (uint32)tmpVal;
    configParams.dataTimeSeg2 =
                        (uint32)setBaud->BaudFdRateConfig.Pseg2 - 1U;
    configParams.dataSynchJumpWidth =
                        (uint32)setBaud->BaudFdRateConfig.Sjw - 1U;
    configParams.dataRatePrescalar =
                        (uint32)setBaud->BaudFdRateConfig.BrpValue - 1U;
    status = MCAN_setBitTime(baseAddr, &configParams);
    if(LLD_STW_SOK == status)
    {
        /* Bit timings config successful */
    }
    else
    {
      /* CAN_CoverageGap_08: Dynamic coverage for the below "DET" is not
       * covered. Since this is an additional check.
       */
#if (CAN_DEV_ERROR_DETECT == STD_ON)
        (void) Det_ReportError((uint16) CAN_MODULE_ID,
                               (uint8) CAN_INSTANCE_ID,
                               (uint8) CAN_SETBAUDRATE_ID,
                               (uint8) CAN_E_PARAM_BAUDRATE);
#endif
    }
}

void Can_mcanDisableInterrupts(
    Can_ControllerObjType *controllerObj)
{
    uint8 InterruptCounter;
    uint32 baseAddr;

    InterruptCounter = controllerObj->canInterruptCounter;
    baseAddr = controllerObj->canControllerConfig_PC.CntrAddr;

    /* Disable Interrupt Sources as well as Interrupt Line */
    MCAN_enableIntr(baseAddr, MCAN_INTR_MASK, (uint32)FALSE);
    MCAN_enableIntrLine(baseAddr,
                        (uint32)MCAN_INTR_LINE_NUM_0,
                        (uint32)FALSE);
    controllerObj->canInterruptCounter = InterruptCounter + 1U;
}

void Can_mcanEnableInterrupts(
    Can_ControllerObjType *canController)
{
    uint8 InterruptCounter;
    uint32 baseAddr;

    InterruptCounter = canController->canInterruptCounter;
    baseAddr = canController->canControllerConfig_PC.CntrAddr;

    if (InterruptCounter != 0U)
    {
        canController->canInterruptCounter = InterruptCounter - 1U;
        InterruptCounter = canController->canInterruptCounter;
        if (InterruptCounter == 0U)
        {
            /* Enable Interrupt Sources as well as Interrupt Line */
            MCAN_enableIntr(baseAddr, MCAN_INTR_MASK, (uint32)TRUE);
            MCAN_enableIntrLine(baseAddr,
                                (uint32)MCAN_INTR_LINE_NUM_0,
                                (uint32)TRUE);
        }   /*End of if (InterruptCounter == 0U)*/
    }
}

void Can_mcanTxDonePolling(
    Can_ControllerObjType *canController,
    const Can_MailboxObjType *canMailbox)
{
    uint32    messageBox = 0U, tranStatus, baseAddr, bitPos;
    uint32    totalBufCntPollMode, loopCnt, txPollModeMask;
#if (CAN_MULTIPLEXED_TRANSMISSION_ENABLE == STD_ON)
    uint32 mbNum, totalBufCntIntrMode;
#endif

    baseAddr = canController->canControllerConfig_PC.CntrAddr;
	totalBufCntPollMode = canController->canControllerTxType.totalBufCntPollMode;
    txPollModeMask = canController->canControllerTxType.txPollModeMask;
	tranStatus = MCAN_getTxBufTransmissionStatus(baseAddr);
    if ((tranStatus & txPollModeMask) != 0U)
    {
        /* Only 32 Tx Mailboxes are supported by hw */
        for(loopCnt = 0U; loopCnt < totalBufCntPollMode; loopCnt++)
        {
            messageBox = canController->canControllerTxType.canControllerObjTxPollingType[loopCnt].hwHandle;
            bitPos = canController->canControllerTxType.canControllerObjTxPollingType[loopCnt].bitPos;
            if ((canController->canTxStatus[messageBox] == 1U) &&
                ((tranStatus & bitPos) == bitPos))
            {
                /* Clear the status as data is already sent */
                canController->canTxStatus[messageBox] = 0U;
                /* Call TX confirmation */
                CanIf_TxConfirmation(canMailbox->canTxRxPduId[messageBox]);
            }
        }
#if (CAN_MULTIPLEXED_TRANSMISSION_ENABLE == STD_ON)
        totalBufCntIntrMode = canController->canControllerTxType.totalBufCntIntrMode;
        loopCnt = totalBufCntIntrMode + totalBufCntPollMode +
                    canController->canControllerTxType.totalFifoCntIntrMode;
        mbNum = (loopCnt + canController->canControllerTxType.totalFifoCntPollMode);
        for(; loopCnt < mbNum; loopCnt++)
        {
            messageBox = canController->canControllerTxType.canControllerObjTxPollingType[loopCnt].hwHandle;
            bitPos = canController->canControllerTxType.canControllerObjTxPollingType[loopCnt].bitPos;
            tranStatus = MCAN_getTxBufTransmissionStatus(baseAddr);
            if ((canController->canTxStatus[messageBox] == 1U) &&
                ((tranStatus & bitPos) == bitPos))
            {
                /* Call TX confirmation */
                CanIf_TxConfirmation(canMailbox->canTxRxPduId[messageBox]);
                /* Clear the status as data is already sent */
                canController->canTxStatus[messageBox] = 0U;
            }
        }
#endif
    }
}

/* CAN_CoverageGap_09: Dynamic coverage for the below function Can_MainFunction_BusOffProcess
 * is not covered. Since BusOff is not tested.
 */

void Can_mcan_BusOffProcess(
    Can_ControllerObjType *canController)
{
    uint32 initStatus, baseAddr;
	uint8  TxStatusCount = 0;
    MCAN_ProtocolStatus protStatus;
    protStatus.busOffStatus = MCAN_PSR_BO_NO_BUS_OFF;


    baseAddr = canController->canControllerConfig_PC.CntrAddr;
    initStatus = MCAN_getOpMode(baseAddr);
    MCAN_getProtocolStatus(baseAddr, &protStatus);
    if ((MCAN_PSR_BO_BUS_OFF == protStatus.busOffStatus) &&
        (MCAN_CCCR_INIT_MODE_INIT == initStatus))
    {
        /* Check if recovery has already started */
        if (canController->canBusOffRecoveryStatus != (boolean) TRUE)
        {
            /* Cancel all pending messages */
            Can_mcanCancelAllPendingMessages(baseAddr);
		    for (TxStatusCount= 0 ; TxStatusCount < MCAN_MAX_BUFFER_SIZE ; TxStatusCount ++)
			{				
				/* Clearing the transmission status pending messages */	
				canController->canTxStatus[TxStatusCount] = 0;			
			}
            /* BusOff Recovery sequence has not started */
            canController->canState = CAN_CS_STOPPED;
            /*Call the bus off indication function*/
            CanIf_ControllerBusOff(canController->
                                   canControllerConfig_PC.ControllerId);
        }
    }
}

void Can_mcan_ModeProcess(
    Can_ControllerObjType *canController)
{
    MCAN_ProtocolStatus protStatus;
    uint32 baseAddr;
    Can_ControllerStateType    controller_state;
    protStatus.busOffStatus = MCAN_PSR_BO_NO_BUS_OFF;

    controller_state = canController->canState;
    baseAddr = canController->canControllerConfig_PC.CntrAddr;
    switch (controller_state)
    {
        case CAN_CS_STARTED:
        {

      /* CAN_CoverageGap_10: Dynamic coverage for the below "if" statement
       * is not covered. Since BusOff is not tested.
       */

            /* If BusOff recovery sequence has started, check if CAN
             * controller is started. If Not
             * restart CAN controller */
            if (canController->canBusOffRecoveryStatus == (boolean) TRUE)
            {
                if (canController->canControllerConfig_PC.CntrActive ==
                                                            (boolean) TRUE)
                {
                    /* Restart CAN controller */
                    MCAN_setOpMode(baseAddr, MCAN_OPERATION_MODE_NORMAL);
                    MCAN_getProtocolStatus(baseAddr, &protStatus);
                    if (MCAN_PSR_BO_BUS_OFF == protStatus.busOffStatus)
                    {
                        /* Busoff recovery has completed */
                        canController->canBusOffRecoveryStatus =
                                                            (boolean) FALSE;
                    }
                }
            }
            CanIf_ControllerModeIndication(
                            canController->canControllerConfig_PC.ControllerId,
                            CAN_CS_STARTED);
            break;
        }

        case CAN_CS_STOPPED:
        {
            if (MCAN_OPERATION_MODE_SW_INIT == MCAN_getOpMode(baseAddr))
            {
                CanIf_ControllerModeIndication(
                            canController->canControllerConfig_PC.ControllerId,
                            CAN_CS_STOPPED);
            }
            break;
        }

        case CAN_CS_SLEEP:
        {
            if (MCAN_CCCR_CSA_ACK == (MCAN_getClkStopAck(baseAddr) &
                                                            MCAN_CCCR_CSA_ACK))
            {
                CanIf_ControllerModeIndication(
                            canController->canControllerConfig_PC.ControllerId,
                            CAN_CS_SLEEP);
            }
            break;
        }

        default:
        {
            break;
        }
    } /*End of Switch*/
}

#if (CAN_RX_POLLING == STD_ON)
void Can_mcan_ReadProcess(
    Can_ControllerObjType *controllerObj,
    const Can_MailboxObjType *canMailbox,
    uint32 maxMbCnt)
{
    /* Read Messages stored in  buffers */
    Can_mcanReadRxBuffPolling(controllerObj);

    /* Empty FIFO messages */
    Can_mcanReadRxFIFO1(controllerObj,
                       canMailbox);
}
#endif

static void Can_mcanReadRxBuffInterrupt(
    Can_ControllerObjType *controllerObj)
{
    uint32 canIdentifier = 0U, bitPos, buffNum, status, baseAddr;
    uint32 hrh = 0U, canDlc;
    uint32 totalBufCntIntrMode, hwBufNum = 0U;
    MCAN_RxNewDataStatus newDataStatus;
    MCAN_RxNewDataStatus newDataStatusTemp = {0U};
    MCAN_RxBufElementNoCpy elem;
    uint32 bufIntrModeMaskLow, bufIntrModeMaskHigh;
    Can_ControllerObjRxType *objRxInterruptType =
	&controllerObj->canControllerRxType;
    Can_ControllerRxObjType *objRxMbxType =
	&controllerObj->canControllerRxType.canControllerObjRxInterruptType[0U];
    totalBufCntIntrMode = objRxInterruptType->totalBufCntIntrMode;
    bufIntrModeMaskLow = objRxInterruptType->bufIntrModeMaskLow;
    bufIntrModeMaskHigh = objRxInterruptType->bufIntrModeMaskHigh;
    baseAddr = controllerObj->canControllerConfig_PC.CntrAddr;
    /* Scan newData Register (32 bits x 2 Registers) and
     * read appropriate mailbox if any message is received */
    SchM_Enter_Can_CAN_EXCLUSIVE_AREA_0();
    MCAN_getNewDataStatus(baseAddr, &newDataStatus);
    SchM_Exit_Can_CAN_EXCLUSIVE_AREA_0();
    if (((bufIntrModeMaskLow & newDataStatus.statusLow) != 0U) ||
        ((bufIntrModeMaskHigh & newDataStatus.statusHigh) != 0U))
    {
        for(buffNum = 0U ; buffNum < totalBufCntIntrMode ; buffNum++)
        {
            hwBufNum = objRxMbxType[buffNum].hwHandle;
            if(hwBufNum < 32U)
            {
                status = newDataStatus.statusLow;
                bitPos = objRxMbxType[buffNum].bitPosLow;
            }

            /* CAN_CoverageGap_11: Dynamic coverage for the below "else" statement
             * is not covered. Since it is tested for 32 buffers.
             */

            else
            {
                status = newDataStatus.statusHigh;
                bitPos = objRxMbxType[buffNum].bitPosHigh;
            }
            if(bitPos == (status & bitPos))
            {
                if(hwBufNum < 32U)
                {
                    newDataStatusTemp.statusLow |= bitPos;
                }

                /* CAN_CoverageGap_12: Dynamic coverage for the below "else" statement
                 * is not covered. Since it is tested for 32 buffers.
                 */

                else
                {
                    newDataStatusTemp.statusHigh |= bitPos;
                }
                /* Initiallise the elem's data field by the canSduPtr, now
                   MCAN_readMsgRamNoCpy will directly write payload to canSduPtr */
                elem.data = controllerObj->canSduPtr;
                SchM_Enter_Can_CAN_EXCLUSIVE_AREA_0();
                MCAN_readMsgRamNoCpy(baseAddr,
                                MCAN_MEM_TYPE_BUF,
                                hwBufNum,
                                MCAN_RX_FIFO_NUM_0,
                                &elem);
                SchM_Exit_Can_CAN_EXCLUSIVE_AREA_0();
                canDlc = Can_McanDataSize[elem.dlc];
                if(1U == elem.xtd)
                {
                    /* Received frame with Extended ID - set MSB to '1' */
                    canIdentifier = (uint32)(elem.id | CAN_MSG_ID_TYPE_EXT);
                }
                else
                {
                    /* Received frame with Standard ID */
                    canIdentifier = (uint32)((elem.id >> STD_MSGID_SHIFT) & STD_MSGID_MASK);
                }
                if(1U == elem.fdf)
                {
                    /* Received CAN-FD frame - set 30th bit to '1' */
                    canIdentifier = (uint32)(canIdentifier | CAN_MSG_TYPE_CAN_FD);
                }

                /* Copy CanId, HOH and ControllerId */
                hrh = objRxMbxType[buffNum].hrh;
                controllerObj->mailboxCfg.Hoh = (uint8)hrh;
                controllerObj->mailboxCfg.CanId = canIdentifier;
                controllerObj->mailboxCfg.ControllerId =
                    controllerObj->canControllerConfig_PC.ControllerId;

                /* Copy SDU data */
                controllerObj->pduInfo.SduDataPtr = controllerObj->canSduPtr;
                controllerObj->pduInfo.SduLength  = (uint16)canDlc;

                /* Call Receive indication */
                CanIf_RxIndication(&controllerObj->mailboxCfg,
                    &controllerObj->pduInfo);
            }
        }

        /* Clear NewData status to accept new messages */
        SchM_Enter_Can_CAN_EXCLUSIVE_AREA_0();
        MCAN_clearNewDataStatus(baseAddr, &newDataStatusTemp);
        SchM_Exit_Can_CAN_EXCLUSIVE_AREA_0();
    }
}

void Can_mcanReadRxFIFO(
    Can_ControllerObjType *controllerObj,
    const Can_MailboxObjType *canMailbox)
{
    uint32 loopCnt, fillLvl, canIdentifier = 0U, htrh = 0U, baseAddr;
    uint8  canDlc;
    MCAN_RxFIFOStatus fifoStatus = {0};
    MCAN_RxBufElementNoCpy elem = {0};
    Can_HwFilterType  *pHwFilter;
	uint32 totalFifoCntIntrMode, fifoIndx;


	Can_ControllerObjRxType *objRxInterruptType =
		&controllerObj->canControllerRxType;
	Can_ControllerRxObjFifoType *objRxMbxType =
		&controllerObj->canControllerRxType.canControllerObjRxInterruptTypeFifo[0U];

    totalFifoCntIntrMode = objRxInterruptType->totalFifoCntIntrMode;

    baseAddr = controllerObj->canControllerConfig_PC.CntrAddr;
    fifoStatus.num = (uint32)MCAN_RX_FIFO_NUM_0;
    SchM_Enter_Can_CAN_EXCLUSIVE_AREA_0();
    MCAN_getRxFIFOStatus(baseAddr, &fifoStatus);
    SchM_Exit_Can_CAN_EXCLUSIVE_AREA_0();
    fillLvl = fifoStatus.fillLvl;
    for(loopCnt = 0U ; loopCnt < fillLvl ; loopCnt++)
    {
        SchM_Enter_Can_CAN_EXCLUSIVE_AREA_0();
        MCAN_getRxFIFOStatus(baseAddr, &fifoStatus);
        SchM_Exit_Can_CAN_EXCLUSIVE_AREA_0();

        /* Initiallise the elem's data field by the canSduPtr, now
           MCAN_readMsgRamNoCpy will directly write payload to canSduPtr */
        elem.data = controllerObj->canSduPtr;
        SchM_Enter_Can_CAN_EXCLUSIVE_AREA_0();
        MCAN_readMsgRamNoCpy(baseAddr,
                        MCAN_MEM_TYPE_FIFO,
                        fifoStatus.getIdx,
                        (uint32)MCAN_RX_FIFO_NUM_0,
                        &elem);
        (void) MCAN_writeRxFIFOAck(baseAddr,
                                   (uint32)MCAN_RX_FIFO_NUM_0,
                                   fifoStatus.getIdx);
        SchM_Exit_Can_CAN_EXCLUSIVE_AREA_0();
        canDlc = Can_McanDataSize[elem.dlc];



        if(1U == elem.xtd)
        {
            /* Received frame with Extended ID - set MSB to '1' */
            canIdentifier = (uint32)(elem.id | CAN_MSG_ID_TYPE_EXT);

        }
        else
        {
            /* Received frame with Standard ID */
            canIdentifier = (uint32)((elem.id >> STD_MSGID_SHIFT) & STD_MSGID_MASK);

        }
        if(1U == elem.fdf)
        {
            /* Received CAN-FD frame - set 30th bit to '1' */
           canIdentifier = (uint32)(canIdentifier | CAN_MSG_TYPE_CAN_FD);

        }

         for (fifoIndx = 0U; fifoIndx < totalFifoCntIntrMode; fifoIndx++)
        {
            uint32 hwFilterIdx, flag = 0U;
	        htrh = objRxMbxType[fifoIndx].hrh;
            for (hwFilterIdx = 0U;
                 ((hwFilterIdx < canMailbox[htrh].mailBoxConfig.HwFilterCnt) && (flag == 0U));
                 hwFilterIdx++)
            {
                pHwFilter = canMailbox[htrh].mailBoxConfig.HwFilterList[hwFilterIdx];
		        uint32 tmpIdValue = pHwFilter->IDValue;

		        if (pHwFilter->Mask != (uint32) 0)
		        {
                    /* ANDing with XTD_MSGID_MASK as that is maximum range for
                     * both standard and extended type of identifier.
                     * Can Identifier value should match.
                     * MBIdType for the mailbox corresponding to the htrh should match with the incoming
                     * elem.xtd flag. Or the   MBIdType should be for mixed mode and hence receive both.
                     * standard and extended frames.*/
                    if (((canIdentifier & XTD_MSGID_MASK) ==
                        (tmpIdValue & (uint32)XTD_MSGID_MASK)) &&
                        ((elem.xtd == canMailbox[htrh].mailBoxConfig.MBIdType) ||
                        (canMailbox[htrh].mailBoxConfig.MBIdType == MIXED_MODE_MB_ID)))
                    {
                        flag = 1U;

                    }
                }
                /* Open filtering, hence only match if ID received is EXT or STD */
                else
                {
                    if ((elem.xtd == canMailbox[htrh].mailBoxConfig.MBIdType) ||
                        (canMailbox[htrh].mailBoxConfig.MBIdType == MIXED_MODE_MB_ID))
                    {
                        /* Fix the matching filter from the list of filters in
			             * the filter list */
			            flag = 1U;
		            }
                }
            }
            if (flag == 1U)
	        {
	            /* Fix the htrh value */
                break;
	        }
        }

        /* Copy CanId, HOH and ControllerId */
        controllerObj->mailboxCfg.Hoh = (uint8)htrh;
        controllerObj->mailboxCfg.CanId = canIdentifier;
        controllerObj->mailboxCfg.ControllerId =
            controllerObj->canControllerConfig_PC.ControllerId;

        /* Copy SDU data */
        controllerObj->pduInfo.SduDataPtr = controllerObj->canSduPtr;
        controllerObj->pduInfo.SduLength  = (uint16)canDlc;

        /* Call Receive indication */
        CanIf_RxIndication(&controllerObj->mailboxCfg,
            &controllerObj->pduInfo);
    }
}

#if (CAN_RX_POLLING == STD_ON)
void Can_mcanReadRxFIFO1(
    Can_ControllerObjType *controllerObj,
    const Can_MailboxObjType *canMailbox)
{
    uint32 loopCnt, fillLvl = 0U, canIdentifier = 0U, htrh = 0U, baseAddr;
    uint8        canDlc;
    MCAN_RxFIFOStatus fifoStatus = {0U};
    MCAN_RxBufElementNoCpy elem = {0U};
    Can_HwFilterType  *pHwFilter;
	uint32 totalFifoCntPollMode, fifoIndx;

	Can_ControllerObjRxType *objRxPollingType =
		&controllerObj->canControllerRxType;
	Can_ControllerRxObjFifoType *objRxMbxType =
		&controllerObj->canControllerRxType.canControllerObjRxPollingTypeFifo[0U];

    totalFifoCntPollMode = objRxPollingType->totalFifoCntPollMode;
    baseAddr = controllerObj->canControllerConfig_PC.CntrAddr;
    fifoStatus.num = (uint32)MCAN_RX_FIFO_NUM_1;
    MCAN_getRxFIFOStatus(baseAddr, &fifoStatus);
    fillLvl = fifoStatus.fillLvl;
    for(loopCnt = 0U ; loopCnt < fillLvl ; loopCnt++)
    {
        MCAN_getRxFIFOStatus(baseAddr, &fifoStatus);
        /* Initiallise the elem's data field by the canSduPtr, now
           MCAN_readMsgRamNoCpy will directly write payload to canSduPtr */
        elem.data = controllerObj->canSduPtr;
        MCAN_readMsgRamNoCpy(baseAddr,
                        MCAN_MEM_TYPE_FIFO,
                        fifoStatus.getIdx,
                        (uint32)MCAN_RX_FIFO_NUM_1,
                        &elem);
        (void) MCAN_writeRxFIFOAck(baseAddr,
                                   (uint32)MCAN_RX_FIFO_NUM_1,
                                   fifoStatus.getIdx);
        canDlc = Can_McanDataSize[elem.dlc];
        if(1U == elem.xtd)
        {
            /* Received frame with Extended ID - set MSB to '1' */
            canIdentifier = (uint32)(elem.id | CAN_MSG_ID_TYPE_EXT);
        }

        /* CAN_CoverageGap_13: Dynamic coverage for the below "else" statement
        * is not covered. Since extended identifier is tested.
        */

        else
        {
            /* Received frame with Standard ID */
            canIdentifier = (uint32)((elem.id >> STD_MSGID_SHIFT) & STD_MSGID_MASK);
        }
        if(1U == elem.fdf)
        {
            /* Received CAN-FD frame - set 30th bit to '1' */
            canIdentifier = (uint32)(canIdentifier | CAN_MSG_TYPE_CAN_FD);
        }

        for (fifoIndx = 0U; fifoIndx < totalFifoCntPollMode; fifoIndx++)
        {
            uint32 hwFilterIdx, flag = 0U;

			htrh = (uint32) objRxMbxType[fifoIndx].hrh;
            for (hwFilterIdx = 0U;
                 hwFilterIdx < canMailbox[htrh].mailBoxConfig.HwFilterCnt;
                 hwFilterIdx++)
            {
		        pHwFilter = canMailbox[htrh].mailBoxConfig.HwFilterList[hwFilterIdx];
			    uint32 tmpIdValue = pHwFilter->IDValue;
			    if (pHwFilter->Mask != 0U)
		        {
                    /* ANDing with XTD_MSGID_MASK as that is maximum range for
                     * both standard and extended type of identifier.
                     * Can Identifier value should match.
                     * MBIdType for the mailbox corresponding to the htrh should match with the incoming
                     * elem.xtd flag. Or the   MBIdType should be for mixed mode and hence receive both.
                     * standard and extended frames.*/
                    if (((canIdentifier & XTD_MSGID_MASK) ==
                        (tmpIdValue & (uint32)XTD_MSGID_MASK)) &&
                        ((elem.xtd == canMailbox[htrh].mailBoxConfig.MBIdType) ||
                        (canMailbox[htrh].mailBoxConfig.MBIdType == MIXED_MODE_MB_ID)))
                    {
                        flag = 1U;                        
                    }
                }

                /* CAN_CoverageGap_14: Dynamic coverage for the below "else" statement
                * is not covered. Since mixed mode is not tested.
                */

                /* Open filtering, hence only match if ID received is EXT or STD */
                else
                {
                    if ((elem.xtd == canMailbox[htrh].mailBoxConfig.MBIdType) ||
                        (canMailbox[htrh].mailBoxConfig.MBIdType == MIXED_MODE_MB_ID))
                    {
                        /* Fix the matching filter from the list of filters in
			             * the filter list */
			            flag = 1U;			      
		            }
                }
                if (flag == 1U)
			    {
                    break;
			    }
            }
			if (flag == 1U)
			{
                break;
			}
        }

        /* Copy CanId, HOH and ControllerId */
        controllerObj->mailboxCfg.Hoh = (uint8)htrh;
        controllerObj->mailboxCfg.CanId = canIdentifier;
        controllerObj->mailboxCfg.ControllerId =
            controllerObj->canControllerConfig_PC.ControllerId;

        /* Copy SDU data */
        controllerObj->pduInfo.SduDataPtr = controllerObj->canSduPtr;
        controllerObj->pduInfo.SduLength  = (uint16)canDlc;

        /* Call Receive indication */
        CanIf_RxIndication(&controllerObj->mailboxCfg,
            &controllerObj->pduInfo);
    }
}

static void Can_mcanReadRxBuffPolling(
    Can_ControllerObjType *controllerObj)
{
    uint32 canIdentifier = 0U, bitPos, buffNum, status, baseAddr, intrStatus = 0U;
    uint32 hrh = 0U, canDlc;
	uint8 controllerId = 0U;
    uint32 totalBufCntPollMode, hwBufNum = 0U;
    MCAN_RxNewDataStatus newDataStatus;
    MCAN_RxNewDataStatus newDataStatusTemp = {0U};
    MCAN_RxBufElementNoCpy elem;
    uint32 bufPollModeMaskLow, bufPollModeMaskHigh;

	Can_ControllerObjRxType *objRxPollingType =
		&controllerObj->canControllerRxType;
	Can_ControllerRxObjType *objRxMbxType =
		&controllerObj->canControllerRxType.canControllerObjRxPollingType[0U];
    controllerId = controllerObj->canControllerConfig_PC.ControllerId;
    totalBufCntPollMode = objRxPollingType->totalBufCntPollMode;
    bufPollModeMaskLow = objRxPollingType->bufPollModeMaskLow;
    bufPollModeMaskHigh = objRxPollingType->bufPollModeMaskHigh;
    baseAddr = controllerObj->canControllerConfig_PC.CntrAddr;
    /* Scan newData Register (32 bits x 2 Registers) and
     * read appropriate mailbox if any message is received */
    MCAN_getNewDataStatus(baseAddr, &newDataStatus);
	SchM_Enter_Can_CAN_EXCLUSIVE_AREA_0();
	intrStatus = MCAN_getIntrStatus(baseAddr);
	MCAN_clearIntrStatus(baseAddr, intrStatus);
    SchM_Exit_Can_CAN_EXCLUSIVE_AREA_0();
	
	    if((uint32)MCAN_INTR_SRC_RX_FIFO1_MSG_LOST ==
       (intrStatus & (uint32)MCAN_INTR_SRC_RX_FIFO1_MSG_LOST))
    {
        (void) Det_ReportRuntimeError((uint16) CAN_MODULE_ID,
                                       (uint8) controllerId,
                                       (uint8) CAN_RXPROCESS_ID,
                                       (uint8) CAN_E_DATALOST);
    }
	
    if (((bufPollModeMaskLow & newDataStatus.statusLow) != 0U) ||
        ((bufPollModeMaskHigh & newDataStatus.statusHigh) != 0U))
    {
        for(buffNum = 0U ; buffNum < totalBufCntPollMode ; buffNum++)
        {
            hwBufNum = objRxMbxType[buffNum].hwHandle;
            if(hwBufNum < 32U)
            {
                status = newDataStatus.statusLow;
                bitPos = objRxMbxType[buffNum].bitPosLow;
            }

            /* CAN_CoverageGap_15: Dynamic coverage for the below "else" statement
             * is not covered. Since it is tested for 32 buffers.
             */

            else
            {
                status = newDataStatus.statusHigh;
                bitPos = objRxMbxType[buffNum].bitPosHigh;
            }

            if(bitPos == (status & bitPos))
            {
                if(hwBufNum < 32U)
                {
                    newDataStatusTemp.statusLow |= bitPos;
                }

                /* CAN_CoverageGap_16: Dynamic coverage for the below "else" statement
                 * is not covered. Since it is tested for 32 buffers.
                 */

                else
                {
                    newDataStatusTemp.statusHigh |= bitPos;
                }
                /* Initiallise the elem's data field by the canSduPtr, now
                   MCAN_readMsgRamNoCpy will directly write payload to canSduPtr */
                elem.data = controllerObj->canSduPtr;
                MCAN_readMsgRamNoCpy(baseAddr,
                                MCAN_MEM_TYPE_BUF,
                                hwBufNum,
                                MCAN_RX_FIFO_NUM_0,
                                &elem);
                canDlc = Can_McanDataSize[elem.dlc];

                /* CAN_CoverageGap_17: Dynamic coverage for the below "if" statement
                 * is not covered. Since it is tested for standard ID.
                 */

                if(1U == elem.xtd)
                {
                    /* Received frame with Extended ID - set MSB to '1' */
                    canIdentifier = (uint32)(elem.id | CAN_MSG_ID_TYPE_EXT);
                }
                else
                {
                    /* Received frame with Standard ID */
                    canIdentifier = (uint32)((elem.id >> STD_MSGID_SHIFT) & STD_MSGID_MASK);
                }
                if(1U == elem.fdf)
                {
                    /* Received CAN-FD frame - set 30th bit to '1' */
                    canIdentifier = (uint32)(canIdentifier | CAN_MSG_TYPE_CAN_FD);
                }

                /* Copy CanId, HOH and ControllerId */
                hrh = objRxMbxType[buffNum].hrh;
                controllerObj->mailboxCfg.Hoh = hrh;
                controllerObj->mailboxCfg.CanId = canIdentifier;
                controllerObj->mailboxCfg.ControllerId =
                    controllerObj->canControllerConfig_PC.ControllerId;

                /* Copy SDU data */
                controllerObj->pduInfo.SduDataPtr = controllerObj->canSduPtr;
                controllerObj->pduInfo.SduLength  = (uint16)canDlc;

                /* Call Receive indication */
                CanIf_RxIndication(&controllerObj->mailboxCfg,
                    &controllerObj->pduInfo);
            }
        }

        /* Clear NewData status to accept new messages */
        MCAN_clearNewDataStatus(baseAddr, &newDataStatusTemp);
    }
}
#endif

/*
 * Design: MCAL-5945,MCAL-5943,MCAL-5928,MCAL-6041,MCAL-6012,MCAL-6057,MCAL-5929
 */
void Can_mcanProcessISR(
    Can_ControllerObjType *canController,
    const Can_MailboxObjType *canMailbox)
{
    uint32 intrStatus, baseAddr, initStatus;
    uint8 controllerId;
	uint8 TxStatusCount = 0;
	MCAN_ProtocolStatus protStatus;
    protStatus.busOffStatus = MCAN_PSR_BO_NO_BUS_OFF;
    controllerId = canController->canControllerConfig_PC.ControllerId;
    baseAddr = canController->canControllerConfig_PC.CntrAddr;
    SchM_Enter_Can_CAN_EXCLUSIVE_AREA_0();
	MCAN_getProtocolStatus(baseAddr, &protStatus);
    intrStatus = MCAN_getIntrStatus(baseAddr);
	initStatus = MCAN_getOpMode(baseAddr);
    /*
     * Design: MCAL-6047
     */
    MCAN_clearIntrStatus(baseAddr, intrStatus);
    SchM_Exit_Can_CAN_EXCLUSIVE_AREA_0();
    if((uint32)MCAN_INTR_SRC_TRANS_COMPLETE ==
       (intrStatus & (uint32)MCAN_INTR_SRC_TRANS_COMPLETE))
    {
        Can_mcanProcessISRTx(canController, canMailbox);
    }
    if(((uint32)MCAN_INTR_SRC_DEDICATED_RX_BUFF_MSG ==
       (intrStatus & (uint32)MCAN_INTR_SRC_DEDICATED_RX_BUFF_MSG)))
    {
        /* Read Messages stored in  buffers */
        Can_mcanReadRxBuffInterrupt(canController);
    }
    if(((uint32)MCAN_INTR_SRC_RX_FIFO0_NEW_MSG ==
       (intrStatus & (uint32)MCAN_INTR_SRC_RX_FIFO0_NEW_MSG)))
    {
        /* Empty FIFO messages */
        Can_mcanReadRxFIFO(canController,
                           canMailbox);
    }

    /* CAN_CoverageGap_18: Dynamic coverage for the below "if" statement
    * is not covered. Since BusOff is not tested.
    */
    /* Process Error Interrupts */
    if((uint32)MCAN_INTR_SRC_BUS_OFF_STATUS ==
       (intrStatus & (uint32)MCAN_INTR_SRC_BUS_OFF_STATUS))
    {
	    if ((MCAN_PSR_BO_BUS_OFF == protStatus.busOffStatus) &&
        (MCAN_CCCR_INIT_MODE_INIT == initStatus))
		{
			/* Cancel all pending messages */
			Can_mcanCancelAllPendingMessages(baseAddr);
			for (TxStatusCount= 0 ; TxStatusCount < MCAN_MAX_BUFFER_SIZE ; TxStatusCount ++)
			{				
				/* Clearing the transmission status pending messages */	
				canController->canTxStatus[TxStatusCount] = 0;			
			}
			
			/* In BUS OFF state call the notification function */
			CanIf_ControllerBusOff(controllerId);
			canController->canState =  CAN_CS_STOPPED;
		}
    }
    if((uint32)MCAN_INTR_SRC_RX_FIFO0_MSG_LOST ==
       (intrStatus & (uint32)MCAN_INTR_SRC_RX_FIFO0_MSG_LOST))
    {
        (void) Det_ReportRuntimeError((uint16) CAN_MODULE_ID,
                                       (uint8) controllerId,
                                       (uint8) CAN_RXPROCESS_ID,
                                       (uint8) CAN_E_DATALOST);
    }
}

void Can_mcanProcessISRTx(
    Can_ControllerObjType *canController,
    const Can_MailboxObjType *canMailbox)
{
    uint32 txStatus, bitPos, hth, baseAddr, totalBufCntIntrMode;
    uint32 messageBox = 0U, txIntrModeMask;
    uint32 loopCnt;
    const Can_MailboxObjType *mailboxObj;
    const Can_MailboxType *mailboxCfg;
#if (CAN_MULTIPLEXED_TRANSMISSION_ENABLE == STD_ON)
    uint32 totalFifoCntIntrMode, totalmbxCnt, totalBufCntPollMode;
#endif

	Can_ControllerObjTxType *objTxInterruptType =
		&canController->canControllerTxType;
	Can_ControllerTxObjType *objTxMbxType =
		&canController->canControllerTxType.canControllerObjTxInterruptType[0U];
    baseAddr = canController->canControllerConfig_PC.CntrAddr;
    totalBufCntIntrMode = objTxInterruptType->totalBufCntIntrMode;

    SchM_Enter_Can_CAN_EXCLUSIVE_AREA_0();
    txStatus = MCAN_getTxBufTransmissionStatus(baseAddr);
    SchM_Exit_Can_CAN_EXCLUSIVE_AREA_0();
    txIntrModeMask = canController->canControllerTxType.txIntrModeMask;

    if ((txStatus & txIntrModeMask) != 0U)
    {

        /* Buffer Handling */
        for(loopCnt = 0U; loopCnt < totalBufCntIntrMode; loopCnt++)
        {

            bitPos = objTxMbxType[loopCnt].bitPos;
            if(bitPos == (txStatus & bitPos))
            {
                hth = objTxMbxType[loopCnt].hth;

                mailboxObj = &canMailbox[hth];
                mailboxCfg = &canMailbox[hth].mailBoxConfig;
                messageBox = objTxMbxType[loopCnt].hwHandle;

                if((CAN_MAILBOX_DIRECTION_TX == mailboxCfg->MBDir) &&
                   (canController->canTxStatus[messageBox] == 1U))
                {

                    canController->canTxStatus[messageBox] = 0U;
                    /* clear for next transmission. */
                    CanIf_TxConfirmation(mailboxObj->canTxRxPduId[messageBox]);
                }
            }
        }
        /* FIFO Handling */
#if (CAN_MULTIPLEXED_TRANSMISSION_ENABLE == STD_ON)
        totalFifoCntIntrMode = objTxInterruptType->totalFifoCntIntrMode;

        totalBufCntPollMode = objTxInterruptType->totalBufCntPollMode;

        loopCnt = totalBufCntIntrMode + totalBufCntPollMode;

        totalmbxCnt = (loopCnt + totalFifoCntIntrMode);

        for(; loopCnt < totalmbxCnt; loopCnt++)
        {
			hth = objTxMbxType[loopCnt].hth;
			mailboxObj = &canMailbox[hth];
            messageBox = objTxMbxType[loopCnt].hwHandle;
            bitPos = objTxMbxType[loopCnt].bitPos;

            if ((canController->canTxStatus[messageBox] == 1U) &&
                ((txStatus & bitPos) == bitPos))
            {

                /* Call TX confirmation */
                CanIf_TxConfirmation(mailboxObj->canTxRxPduId[messageBox]);
                /* Clear the status as data is already sent */
                canController->canTxStatus[messageBox] = 0U;
            }
        }
#endif
    }
}

void Can_mcanHwDeInit(
    const Can_ControllerObjType *canController)
{
    uint32 baseAddr;

    baseAddr = canController->canControllerConfig_PC.CntrAddr;

    /* Disable Interrupt Sources as well as Interrupt Line */
    MCAN_enableIntr(baseAddr, MCAN_INTR_MASK, (uint32)FALSE);
    MCAN_enableIntrLine(baseAddr,
                        (uint32)MCAN_INTR_LINE_NUM_0,
                        (uint32)FALSE);
    /* Clear Interrupt status */
    MCAN_clearIntrStatus(baseAddr, (uint32)MCAN_INTR_MASK_ALL);
}

Can_ErrorStateType Can_mcanGetProtocolStatus(
    const Can_ControllerObjType *canController)
{
    uint32 baseAddr;
    MCAN_ProtocolStatus   errStatus;
    Can_ErrorStateType    errorState;
    errStatus.busOffStatus = 0U;
    errStatus.errPassive   = 0U;

    baseAddr = canController->canControllerConfig_PC.CntrAddr;
    /* Check Protocol Staus Register */
    MCAN_getProtocolStatus(baseAddr, &errStatus);

    /* CAN_CoverageGap_19: Dynamic coverage for the below function "if and else if" statement
    * is not covered. Since BusOff is not tested.
    */
    if (1U == errStatus.busOffStatus)
    {
		errorState = CAN_ERRORSTATE_BUSOFF;
	}
    /* CAN_CoverageGap_20: Dynamic coverage for the below function "if and else if" statement
    * is not covered. Since BusOff is not tested.
    */
	else if (1U == errStatus.errPassive)
	{
		errorState = CAN_ERRORSTATE_PASSIVE;
	}
	else
	{
		errorState = CAN_ERRORSTATE_ACTIVE;
	}

	return errorState;
}
/* ========================================================================== */
/*                 Internal Functions                                         */
/* ========================================================================== */
static void Can_mcanInit(MCAN_InitParams *initParams,
                  const Can_ControllerType *canControllerCfg,
                  const Can_ControllerType_PC *canControllerCfg_PC)
{
    uint32 timeQuanta, baseAddr;

    baseAddr =  canControllerCfg_PC->CntrAddr;
    /* Initialize MCAN Init params */
    initParams->fdMode          = (uint32)MCAN_CCCR_FDOE_ENABLE;
    if((boolean)TRUE ==
        canControllerCfg->DefaultBaud->BaudFdRateConfig.BrsSwitch)
    {
        initParams->brsEnable       = (uint32)MCAN_CCCR_BRSE_ENABLE;
    }
    else
    {
        initParams->brsEnable       = (uint32)MCAN_CCCR_BRSE_DISABLE;
    }

    /* Auto Retransmission is always enabled */
    initParams->darEnable       = (uint32)MCAN_CCCR_DAR_DISABLE;

    if((uint16)0U !=
                canControllerCfg->DefaultBaud->BaudFdRateConfig.TrcvCompDelay)
    {
        timeQuanta =
          (uint32)canControllerCfg->DefaultBaud->BaudFdRateConfig.TrcvCompDelay;
        timeQuanta = ((uint32)CLK_CAN_FD_FREQ * timeQuanta) / 1000U;
        initParams->tdcEnable       = (uint32)MCAN_DBTP_TDC_ENABLE;
        initParams->tdcConfig.tdco  = timeQuanta;
        initParams->tdcConfig.tdcf  = (uint32)MCAN_TRCV_DELAY_COMP_WIN;
    }

    /* CAN_CoverageGap_21: Dynamic coverage for the below "else" statement
    * is not covered. Since Transmitter Delay Compensation is enabled and tested.
    */
    else
    {
        initParams->tdcEnable       = (uint32)MCAN_DBTP_TDC_DISABLE;
        initParams->tdcConfig.tdco  =
          (uint32)canControllerCfg->DefaultBaud->BaudFdRateConfig.TrcvCompDelay;
        initParams->tdcConfig.tdcf  = (uint32)MCAN_TRCV_DELAY_COMP_WIN;
    }
    initParams->txpEnable       = (uint32)MCAN_TRANSMIT_PAUSE_ENABLE;
    initParams->efbi            = (uint32)MCAN_EFBI;
    initParams->pxhddisable     = (uint32)MCAN_PROT_EXP_HANDLING_ENABLE;
    initParams->emulationEnable = (uint32)MCAN_EMU_ENABLE;
    initParams->emulationFAck   = (uint32)MCAN_EMU_FAST_ACK;
    initParams->clkStopFAck     = (uint32)MCAN_CLK_STOP_FAST_ACK;
    initParams->wdcPreload      = (uint32)MCAN_WDC_PRELOAD;
    initParams->wkupReqEnable   = (uint32)MCAN_WAKEUPREQ_ENABLE;
    initParams->autoWkupEnable  = (uint32)MCAN_AUTO_WAKEUP_ENABLE;
    (void) MCAN_init(baseAddr, initParams);
}

static void Can_mcanConfig(
    const Can_ControllerObjType *controllerObj,
    const Can_ControllerType *canControllerCfg,
    uint32 baseAddr)
{
    MCAN_ConfigParams mcanConfigParams;
    MCAN_ECCConfigParams eccConfigParams;
    MCAN_RxNewDataStatus newDataStatus;

    /* Initialize Configuration parameters */
    mcanConfigParams.monEnable                      = 0U;
    mcanConfigParams.asmEnable                      = 0U;
    mcanConfigParams.tsPrescalar                    = 0U;
    mcanConfigParams.tsSelect                       = 0U;
    mcanConfigParams.timeoutSelect                  = 0U;
    mcanConfigParams.timeoutPreload                 = 0U;
    mcanConfigParams.timeoutCntEnable               = 0U;
    mcanConfigParams.filterConfig.rrfe              = 0x1U;
    mcanConfigParams.filterConfig.rrfs              = 0x1U;
    mcanConfigParams.filterConfig.anfe              = 0x3U;
    mcanConfigParams.filterConfig.anfs              = 0x3U;
    (void) MCAN_config(baseAddr, &mcanConfigParams);
    /* Configure ECC */
    eccConfigParams.enable         =
                            (uint32)MCAN_ECC_AGGR_CONTROL_ECC_ENABLE_DISABLE;
    eccConfigParams.enableChk      =
                            (uint32)MCAN_ECC_AGGR_CONTROL_ECC_CHECK_DISABLE;
    eccConfigParams.enableRdModWr  =
                            (uint32)MCAN_ECC_AGGR_CONTROL_ENABLE_RMW_DISABLE;
    MCAN_eccConfig(baseAddr, &eccConfigParams);

    /* Clear all pending error flags and status */
    MCAN_clearIntrStatus(baseAddr, MCAN_INTR_MASK);
    newDataStatus.statusLow  = MCAN_NDAT1_CLEAR;
    newDataStatus.statusHigh = MCAN_NDAT2_CLEAR;
    MCAN_clearNewDataStatus(baseAddr, &newDataStatus);
    /* Configure Baud rate */
    Can_mcanConfigBaudRate(controllerObj, canControllerCfg, baseAddr);
}

static void Can_mcanInitInterrupt(
    uint32 baseAddr)
{
        MCAN_enableIntr(baseAddr, MCAN_INTR_MASK, (uint32)TRUE);
        MCAN_selectIntrLine(baseAddr,
                            MCAN_INTR_MASK,
                            (uint32)MCAN_INTR_LINE_NUM_0);
        MCAN_enableIntrLine(baseAddr,
                            (uint32)MCAN_INTR_LINE_NUM_0,
                            (uint32)TRUE);
}


/*******************************************************************************
 * Can_mcanConfigBaudRate
 ******************************************************************************/
/*! \brief      This function will configure the baud rate.
 *
 *  \param[in]  uint8 controller -  The controller for which the state change
 *              needs to be performed/
 *              uint32 baseAddr- DCAN base address.
 *
 *
 *  \context
 ******************************************************************************/
static void Can_mcanConfigBaudRate(
    const Can_ControllerObjType *controllerObj,
    const Can_ControllerType *controllerCfg,
    uint32 baseAddr)
{
    MCAN_BitTimingParams configParams;
    uint8 tmpVal;
    int32_t status;

	configParams.nomRatePrescalar =
							(uint32)controllerCfg->DefaultBaud->BrpValue;
    configParams.nomRatePrescalar -= (uint32)1U;

    /* Setup bit timing */
    tmpVal = (controllerCfg->DefaultBaud->Pseg1 +
                    controllerCfg->DefaultBaud->PropSeg) - 1U;
    configParams.nomTimeSeg1 = (uint32)tmpVal;
    configParams.nomTimeSeg2 = (uint32)controllerCfg->DefaultBaud->Pseg2 - 1U;
    configParams.nomSynchJumpWidth =
                                (uint32)controllerCfg->DefaultBaud->Sjw - 1U;
    tmpVal =       (controllerCfg->DefaultBaud->BaudFdRateConfig.Pseg1 +
                     controllerCfg->DefaultBaud->BaudFdRateConfig.PropSeg) - 1U;
    configParams.dataTimeSeg1 = (uint32)tmpVal;

    configParams.dataTimeSeg2 =
                 (uint32)controllerCfg->DefaultBaud->BaudFdRateConfig.Pseg2 - 1U;
    configParams.dataSynchJumpWidth =
                  (uint32)controllerCfg->DefaultBaud->BaudFdRateConfig.Sjw - 1U;
    configParams.dataRatePrescalar =
             (uint32)controllerCfg->DefaultBaud->BaudFdRateConfig.BrpValue - 1U;

    status = MCAN_setBitTime(baseAddr, &configParams);
    if(LLD_STW_SOK == status)
    {
        /* Bit timings config successful */
    }

    /* CAN_CoverageGap_22: Dynamic coverage for the below "DET" is not
     * covered. Since this is an additional check.
     */
    else
    {
#if (CAN_DEV_ERROR_DETECT == STD_ON)
        (void) Det_ReportError((uint16) CAN_MODULE_ID,
                               (uint8) CAN_INSTANCE_ID,
                               (uint8) CAN_INIT_ID,
                               (uint8) CAN_E_PARAM_BAUDRATE);
#endif
    }
}

static void Can_mcanMsgRAMConfigInit(
    Can_FdMsgRAMConfigObjType *canFDMsgRamConfig)
{
    uint32 loopCnt = 0U;

    canFDMsgRamConfig->stdFilterNum      = 0U;
    canFDMsgRamConfig->extFilterNum      = 0U;

    /* Initialize Message RAM configuration parameters */
    canFDMsgRamConfig->configParams.flssa                  = 0U;
    canFDMsgRamConfig->configParams.lss                    = 0U;
    canFDMsgRamConfig->configParams.flesa                  = 0U;
    canFDMsgRamConfig->configParams.lse                    = 0U;
    canFDMsgRamConfig->configParams.txStartAddr            = 0U;
    canFDMsgRamConfig->configParams.txBufNum               = 0U;
    canFDMsgRamConfig->configParams.txFIFOSize             = 0U;
    canFDMsgRamConfig->configParams.txBufMode              = 1U;
    canFDMsgRamConfig->configParams.txBufElemSize          =
                                                (uint32)MCAN_ELEM_SIZE_64BYTES;
    canFDMsgRamConfig->configParams.txEventFIFOStartAddr   = 0U;
    canFDMsgRamConfig->configParams.txEventFIFOSize        = 0U;
    canFDMsgRamConfig->configParams.txEventFIFOWaterMark   = 0U;
    canFDMsgRamConfig->configParams.rxFIFO0startAddr       = 0U;
    canFDMsgRamConfig->configParams.rxFIFO0size            = 0U;
    canFDMsgRamConfig->configParams.rxFIFO0waterMark       = 0U;
    canFDMsgRamConfig->configParams.rxFIFO0OpMode          = 0U;
    canFDMsgRamConfig->configParams.rxFIFO1startAddr       = 0U;
    canFDMsgRamConfig->configParams.rxFIFO1size            = 0U;
    canFDMsgRamConfig->configParams.rxFIFO1waterMark       = 0U;
    canFDMsgRamConfig->configParams.rxFIFO1OpMode          = 0U;
    canFDMsgRamConfig->configParams.rxBufStartAddr         = 0U;
    canFDMsgRamConfig->configParams.rxBufElemSize          =
                                                (uint32)MCAN_ELEM_SIZE_64BYTES;
    canFDMsgRamConfig->configParams.rxFIFO0ElemSize        =
                                                (uint32)MCAN_ELEM_SIZE_64BYTES;
    canFDMsgRamConfig->configParams.rxFIFO1ElemSize        =
                                                (uint32)MCAN_ELEM_SIZE_64BYTES;

    canFDMsgRamConfig->txBuffNum                           = 0U;
    canFDMsgRamConfig->txFIFONum                           = 0U;
    canFDMsgRamConfig->rxBuffNum                           = 0U;
    canFDMsgRamConfig->rxFIFONum                           = 0U;

    for(loopCnt = 0U ; loopCnt < MCAN_RX_BUFFER_MAX_NUM ; loopCnt++)
    {
        canFDMsgRamConfig->stdMsgIDFilterList[loopCnt].sfid1 = 0U;
        canFDMsgRamConfig->stdMsgIDFilterList[loopCnt].sfid2 = 0U;
        canFDMsgRamConfig->stdMsgIDFilterList[loopCnt].sfec  = 0U;
        canFDMsgRamConfig->stdMsgIDFilterList[loopCnt].sft   = 0x3U;
        canFDMsgRamConfig->extMsgIDFilterList[loopCnt].efid1 = 0U;
        canFDMsgRamConfig->extMsgIDFilterList[loopCnt].efid2 = 0U;
        canFDMsgRamConfig->extMsgIDFilterList[loopCnt].efec  = 0U;
        canFDMsgRamConfig->extMsgIDFilterList[loopCnt].eft   = 0x3U;
        canFDMsgRamConfig->rxMbMapping[0U][loopCnt]          = 0xFFU;
        canFDMsgRamConfig->rxMbMapping[1U][loopCnt]          = 0xFFU;
    }
    for(loopCnt = 0U ; loopCnt < MCAN_TX_BUFFER_MAX_NUM ; loopCnt++)
    {
        canFDMsgRamConfig->txMbMapping[0U][loopCnt]          = 0xFFU;
        canFDMsgRamConfig->txMbMapping[1U][loopCnt]          = 0xFFU;
        canFDMsgRamConfig->hthToMbMapping[loopCnt]           = 0xFFU;
    }
}

static void Can_mcanCancelAllPendingMessages(
    uint32 baseAddr)
{
    TickType    startCount = 0U, tempCount = 0U, elapsedCount = 0U;
    boolean timeOutStatus = (boolean) FALSE;
    uint32 txBufPendStatus, bitIndex;
    SchM_Enter_Can_CAN_EXCLUSIVE_AREA_0();
    txBufPendStatus = MCAN_getTxBufReqPend(baseAddr);
    SchM_Exit_Can_CAN_EXCLUSIVE_AREA_0();
    for (bitIndex = 0U; bitIndex < 32U; bitIndex++)
    {
        /* For each bit set call cancel request */
        uint32 bitPos = (uint32)1U << bitIndex;
        if (bitPos == (txBufPendStatus & bitPos))
        {
            /* Clear pending Tx request */
            /* Return Value is ignored as buffNum is always less than 32 */
            SchM_Enter_Can_CAN_EXCLUSIVE_AREA_0();
            (void)MCAN_txBufCancellationReq(baseAddr, bitIndex);
            timeOutStatus = GetCounterValue(CAN_OS_COUNTER_ID, &startCount);
            SchM_Exit_Can_CAN_EXCLUSIVE_AREA_0();
            if (((StatusType) E_OK) == timeOutStatus)
            {
                /* Wait for FSM to to come out of reset */
                do
                {
                    /* Below API can change start time, so use temp variable */
                    tempCount = startCount;
                    SchM_Enter_Can_CAN_EXCLUSIVE_AREA_0();
                    timeOutStatus = GetElapsedValue(
                                 CAN_OS_COUNTER_ID,
                                 &tempCount,
                                 &elapsedCount);
                    SchM_Exit_Can_CAN_EXCLUSIVE_AREA_0();
                    if ((((StatusType) E_OK) != timeOutStatus) ||
                        (elapsedCount >= CAN_TIMEOUT_DURATION))
                    {

                      /* CAN_CoverageGap_23: Dynamic coverage for the below DEM event is not covered.
                       * Since it is hardware dependent.
                       */

                        /* timeout */
#if (CAN_E_HARDWARE_ERROR != CAN_DEM_NO_EVENT)
                        (void)Dem_SetEventStatus(
                            CAN_E_HARDWARE_ERROR, DEM_EVENT_STATUS_FAILED);
#endif
                        break;
                    }
                }while ((MCAN_txBufCancellationStatus(baseAddr) & bitPos) !=
                        bitPos);
            }
        }
    }

    return;
}

static void Can_mcanSetUpController(uint32 baseAddr)
{
    StatusType  status;
    TickType    startCount = 0U, tempCount = 0U, elapsedCount = 0U;

    /* Put MCAN in SW initialization mode */
    MCAN_setOpMode(baseAddr, MCAN_OPERATION_MODE_SW_INIT);
    status = GetCounterValue(CAN_OS_COUNTER_ID, &startCount);
    if (((StatusType) E_OK) == status)
    {
        /* Wait for FSM to to come out of reset */
        do
        {
            /* Below API can change start time, so use temp variable */
            tempCount = startCount;
            status = GetElapsedValue(
                         CAN_OS_COUNTER_ID,
                         &tempCount,
                         &elapsedCount);
            if ((((StatusType) E_OK) != status) ||
                (elapsedCount >= CAN_TIMEOUT_DURATION))
            {

              /* CAN_CoverageGap_24: Dynamic coverage for the below DEM event is not covered.
               * Since it is hardware dependent.
               */

                /* timeout */
#if (CAN_E_HARDWARE_ERROR != CAN_DEM_NO_EVENT)
                (void)Dem_SetEventStatus(
                    CAN_E_HARDWARE_ERROR, DEM_EVENT_STATUS_FAILED);
#endif
                break;
            }
        } while (MCAN_OPERATION_MODE_SW_INIT != MCAN_getOpMode(baseAddr));
    }
}

static void Can_mcanSetUpMbxList(
    Can_ControllerObjType *canController,
    const Can_MailboxObjType *canMailbox,
    uint32 maxMbCnt)
{
    uint32 htrh;
    uint8 controller;
    const Can_MailboxType *mailboxCfg;
    const Can_MailboxObjType *mailBoxCfgList;
    controller = canController->canControllerConfig_PC.ControllerId;
    mailBoxCfgList = &canMailbox[0U];
    /* Initialize Message RAM params according to configured mailboxes */
    for (htrh = 0U; htrh < maxMbCnt; htrh++)
    {
        mailboxCfg = &mailBoxCfgList[htrh].mailBoxConfig;
        if (mailboxCfg->Controller->ControllerId == controller)
        {
            if(CAN_MAILBOX_DIRECTION_RX == mailboxCfg->MBDir)
            {
				Can_mcanSetUpMbxListRx(canController, mailboxCfg, htrh);
            }
            if(CAN_MAILBOX_DIRECTION_TX == mailboxCfg->MBDir)
            {
				Can_mcanSetUpMbxListTx(canController, mailboxCfg, htrh);
            }
        }
	}
}

static void Can_mcanSetUpMbxListTx(
	Can_ControllerObjType *canController,
	const Can_MailboxType *mailboxCfg, uint32 htrh)
{
	Can_TxRxProcessingType txProcessingType;
	uint32 loopIndx = 0U;

	txProcessingType = mailboxCfg->Controller->TxProcessingType;
	if (mailboxCfg->CanHwObjectCount == 1U)
	{
		if (txProcessingType == CAN_TX_RX_PROCESSING_INTERRUPT)
		{
			loopIndx = canController->canControllerTxType.totalBufCntIntrMode;
			canController->canControllerTxType.
				canControllerObjTxInterruptType[loopIndx].hth = htrh;
			canController->canControllerTxType.
				canControllerObjTxInterruptType[loopIndx].bitPos = (uint32)1U << mailboxCfg->HwHandle;
			canController->canControllerTxType.
				canControllerObjTxInterruptType[loopIndx].hwHandle = mailboxCfg->HwHandle;
            canController->canControllerTxType.txIntrModeMask |= (uint32)1U << mailboxCfg->HwHandle;
			canController->canControllerTxType.totalBufCntIntrMode += 1U;
		}
		else if ((txProcessingType == CAN_TX_RX_PROCESSING_POLLING) ||
				((txProcessingType == CAN_TX_RX_PROCESSING_MIXED) &&
				(mailboxCfg->CanHardwareObjectUsesPolling == (boolean)TRUE)))
		{
			loopIndx = canController->canControllerTxType.totalBufCntPollMode;
			canController->canControllerTxType.
				canControllerObjTxPollingType[loopIndx].hth = htrh;
			canController->canControllerTxType.
				canControllerObjTxPollingType[loopIndx].bitPos = (uint32)1U << mailboxCfg->HwHandle;
			canController->canControllerTxType.
				canControllerObjTxPollingType[loopIndx].hwHandle = mailboxCfg->HwHandle;
            canController->canControllerTxType.txPollModeMask |= (uint32)1U << mailboxCfg->HwHandle;
			canController->canControllerTxType.totalBufCntPollMode += 1U;
		}
		else
		{
			loopIndx = canController->canControllerTxType.totalBufCntIntrMode;
			canController->canControllerTxType.
				canControllerObjTxInterruptType[loopIndx].hth = htrh;
			canController->canControllerTxType.
				canControllerObjTxInterruptType[loopIndx].bitPos = (uint32)1U << mailboxCfg->HwHandle;
			canController->canControllerTxType.
				canControllerObjTxInterruptType[loopIndx].hwHandle = mailboxCfg->HwHandle;
            canController->canControllerTxType.txIntrModeMask |= (uint32)1U << mailboxCfg->HwHandle;
			canController->canControllerTxType.totalBufCntIntrMode += 1U;
			canController->txMixedModeIntrEnable = (boolean)TRUE;
		}
	}
}

static void Can_mcanSetUpMbxListRx(
	Can_ControllerObjType *canController,
	const Can_MailboxType *mailboxCfg, uint32 htrh)
{
	Can_TxRxProcessingType rxProcessingType;
	uint32 loopIndx = 0U;

	rxProcessingType = mailboxCfg->Controller->RxProcessingType;
	if (mailboxCfg->CanHwObjectCount == 1U)
	{
		if (rxProcessingType == CAN_TX_RX_PROCESSING_INTERRUPT)
		{
			loopIndx = canController->canControllerRxType.totalBufCntIntrMode;
			canController->canControllerRxType.
				canControllerObjRxInterruptType[loopIndx].hrh = htrh;
			if(mailboxCfg->HwHandle < 32U)
			{
				canController->canControllerRxType.
					canControllerObjRxInterruptType[loopIndx].bitPosLow = (uint32)1U << mailboxCfg->HwHandle;
                canController->canControllerRxType.bufIntrModeMaskLow |= (uint32)1U << mailboxCfg->HwHandle;
			}

      /* CAN_CoverageGap_25: Dynamic coverage for the below "else" statement
       * is not covered. Since it is tested for 32 HwHandle.
       */

			else
			{
				canController->canControllerRxType.
					canControllerObjRxInterruptType[loopIndx].bitPosHigh = (uint32)1U << (mailboxCfg->HwHandle - 32U);
                canController->canControllerRxType.bufIntrModeMaskHigh |= (uint32)1U << (mailboxCfg->HwHandle - 32U);
			}
			canController->canControllerRxType.
				canControllerObjRxInterruptType[loopIndx].hwHandle = mailboxCfg->HwHandle;
			canController->canControllerRxType.totalBufCntIntrMode += 1U;
		}
		else if ((rxProcessingType == CAN_TX_RX_PROCESSING_POLLING) ||
				 ((rxProcessingType == CAN_TX_RX_PROCESSING_MIXED) &&
			(mailboxCfg->CanHardwareObjectUsesPolling == (boolean)TRUE)))
		{
			loopIndx = canController->canControllerRxType.totalBufCntPollMode;
			canController->canControllerRxType.
				canControllerObjRxPollingType[loopIndx].hrh = htrh;
			if(mailboxCfg->HwHandle < 32U)
			{
				canController->canControllerRxType.
					canControllerObjRxPollingType[loopIndx].bitPosLow = (uint32)1U << mailboxCfg->HwHandle;
                canController->canControllerRxType.bufPollModeMaskLow |= (uint32)1U << mailboxCfg->HwHandle;
			}

      /* CAN_CoverageGap_26: Dynamic coverage for the below "else" statement
       * is not covered. Since it is tested for 32 HwHandle.
       */

			else
			{
				canController->canControllerRxType.
					canControllerObjRxPollingType[loopIndx].bitPosHigh = (uint32)1U << (mailboxCfg->HwHandle - 32U);
                canController->canControllerRxType.bufPollModeMaskHigh |= (uint32)1U << (mailboxCfg->HwHandle - 32U);
			}
			canController->canControllerRxType.
				canControllerObjRxPollingType[loopIndx].hwHandle = mailboxCfg->HwHandle;
			canController->canControllerRxType.totalBufCntPollMode += 1U;
		}
		else
		{
			loopIndx = canController->canControllerRxType.totalBufCntIntrMode;
			canController->canControllerRxType.
				canControllerObjRxInterruptType[loopIndx].hrh = htrh;
			if(mailboxCfg->HwHandle < 32U)
			{
				canController->canControllerRxType.
					canControllerObjRxInterruptType[loopIndx].bitPosLow = (uint32)1U << mailboxCfg->HwHandle;
                canController->canControllerRxType.bufIntrModeMaskLow |= (uint32)1U << mailboxCfg->HwHandle;
			}
			else
			{
				canController->canControllerRxType.
					canControllerObjRxInterruptType[loopIndx].bitPosHigh = (uint32)1U << (mailboxCfg->HwHandle - 32U);
                canController->canControllerRxType.bufIntrModeMaskHigh |= (uint32)1U << (mailboxCfg->HwHandle - 32U);
			}
			canController->canControllerRxType.
				canControllerObjRxInterruptType[loopIndx].hwHandle = mailboxCfg->HwHandle;
			canController->canControllerRxType.totalBufCntIntrMode += 1U;
			canController->rxMixedModeIntrEnable = (boolean)TRUE;
		}
	}
 }

static void Can_mcanSetUpMbxListFIFO(
    Can_ControllerObjType *canController,
    const Can_MailboxObjType *canMailbox,
    uint32 maxMbCnt)
{
    uint32 htrh;
    uint8 controller;
    const Can_MailboxType *mailboxCfg;
    const Can_MailboxObjType *mailBoxCfgList;
    controller = canController->canControllerConfig_PC.ControllerId;
    mailBoxCfgList = &canMailbox[0U];
    /* Initialize Message RAM params according to configured mailboxes */
    for (htrh = 0U; htrh < maxMbCnt; htrh++)
    {
        mailboxCfg = &mailBoxCfgList[htrh].mailBoxConfig;
        if (mailboxCfg->Controller->ControllerId == controller)
        {
            if(CAN_MAILBOX_DIRECTION_RX == mailboxCfg->MBDir)
            {
                Can_mcanSetUpMbxListFIFORx(canController, mailboxCfg, htrh);
            }
            if(CAN_MAILBOX_DIRECTION_TX == mailboxCfg->MBDir)
            {
				Can_mcanSetUpMbxListFIFOTxIntr(canController, mailboxCfg, htrh);
            }
        }
	}
    for (htrh = 0U; htrh < maxMbCnt; htrh++)
    {
        mailboxCfg = &mailBoxCfgList[htrh].mailBoxConfig;
        if (mailboxCfg->Controller->ControllerId == controller)
        {
            if(CAN_MAILBOX_DIRECTION_TX == mailboxCfg->MBDir)
            {
				Can_mcanSetUpMbxListFIFOTxPoll(canController, mailboxCfg, htrh);
            }
        }
	}
}

static void Can_mcanSetUpMbxListFIFORx(
	Can_ControllerObjType *canController,
	const Can_MailboxType *mailboxCfg, uint32 htrh)
{
	Can_TxRxProcessingType rxProcessingType;
	uint32 loopIndx = 0U, tmpLoopCnt = 0U;

	rxProcessingType = mailboxCfg->Controller->RxProcessingType;
	if (mailboxCfg->CanHwObjectCount > 1U)
	{
		if (rxProcessingType == CAN_TX_RX_PROCESSING_INTERRUPT)
		{
			loopIndx = canController->canControllerRxType.totalFifoCntIntrMode;
			tmpLoopCnt = loopIndx;
			for (; tmpLoopCnt < (loopIndx + mailboxCfg->CanHwObjectCount); tmpLoopCnt++)
			{
				canController->canControllerRxType.
					canControllerObjRxInterruptTypeFifo[tmpLoopCnt].hrh = htrh;
			}
			canController->canControllerRxType.totalFifoCntIntrMode += mailboxCfg->CanHwObjectCount;
		}
		else if ((rxProcessingType == CAN_TX_RX_PROCESSING_POLLING) ||
				((rxProcessingType == CAN_TX_RX_PROCESSING_MIXED) &&
				(mailboxCfg->CanHardwareObjectUsesPolling == (boolean)TRUE)))
		{
			loopIndx = canController->canControllerRxType.totalFifoCntPollMode;
			tmpLoopCnt = loopIndx;
			for (; tmpLoopCnt < (loopIndx + mailboxCfg->CanHwObjectCount); tmpLoopCnt++)
			{
				canController->canControllerRxType.
					canControllerObjRxPollingTypeFifo[tmpLoopCnt].hrh = htrh;
			}
			canController->canControllerRxType.totalFifoCntPollMode += mailboxCfg->CanHwObjectCount;
		}
		else
		{
			loopIndx = canController->canControllerRxType.totalFifoCntIntrMode;
			tmpLoopCnt = loopIndx;
			for (; tmpLoopCnt < (loopIndx + mailboxCfg->CanHwObjectCount); tmpLoopCnt++)
			{
				canController->canControllerRxType.
					canControllerObjRxInterruptTypeFifo[tmpLoopCnt].hrh = htrh;
			}
			canController->canControllerRxType.totalFifoCntIntrMode += mailboxCfg->CanHwObjectCount;
			canController->rxMixedModeIntrEnable = (boolean)TRUE;
		}
	}
}

static void Can_mcanSetUpMbxListFIFOTxIntr(
	Can_ControllerObjType *canController,
	const Can_MailboxType *mailboxCfg, uint32 htrh)
{
	Can_TxRxProcessingType txProcessingType;
	uint32 loopIndx = 0U, tmpLoopCnt = 0U;

	txProcessingType = mailboxCfg->Controller->TxProcessingType;
	if (mailboxCfg->CanHwObjectCount > 1U)
	{
		if ((txProcessingType == CAN_TX_RX_PROCESSING_INTERRUPT) ||
            ((txProcessingType == CAN_TX_RX_PROCESSING_MIXED) &&
				(mailboxCfg->CanHardwareObjectUsesPolling == (boolean)FALSE)))
		{
			loopIndx = canController->canControllerTxType.totalBufCntIntrMode  +
                       canController->canControllerTxType.totalBufCntPollMode  +
					   canController->canControllerTxType.totalFifoCntIntrMode +
                       canController->canControllerTxType.totalFifoCntPollMode;
			tmpLoopCnt = loopIndx;
			for (; tmpLoopCnt < (loopIndx + mailboxCfg->CanHwObjectCount); tmpLoopCnt++)
			{
				canController->canControllerTxType.
					canControllerObjTxInterruptType[tmpLoopCnt].hth = htrh;
				canController->canControllerTxType.
					canControllerObjTxInterruptType[tmpLoopCnt].bitPos = (uint32)1U << tmpLoopCnt;
				canController->canControllerTxType.
					canControllerObjTxInterruptType[tmpLoopCnt].hwHandle = tmpLoopCnt;
                canController->canControllerTxType.txIntrModeMask |= (uint32)1U << tmpLoopCnt;
			}
			canController->canControllerTxType.totalFifoCntIntrMode += mailboxCfg->CanHwObjectCount;
		}
	}
}

static void Can_mcanSetUpMbxListFIFOTxPoll(
	Can_ControllerObjType *canController,
	const Can_MailboxType *mailboxCfg, uint32 htrh)
{
	Can_TxRxProcessingType txProcessingType;
	uint32 loopIndx = 0U, tmpLoopCnt = 0U;

	txProcessingType = mailboxCfg->Controller->TxProcessingType;
	if (mailboxCfg->CanHwObjectCount > 1U)
	{
		if ((txProcessingType == CAN_TX_RX_PROCESSING_POLLING) ||
				((txProcessingType == CAN_TX_RX_PROCESSING_MIXED) &&
				(mailboxCfg->CanHardwareObjectUsesPolling == (boolean)TRUE)))
		{
			loopIndx = canController->canControllerTxType.totalBufCntPollMode  +
                       canController->canControllerTxType.totalBufCntIntrMode  +
					   canController->canControllerTxType.totalFifoCntPollMode +
                       canController->canControllerTxType.totalFifoCntIntrMode;
			tmpLoopCnt = loopIndx;
			for (; tmpLoopCnt < (loopIndx + mailboxCfg->CanHwObjectCount); tmpLoopCnt++)
			{
				canController->canControllerTxType.
					canControllerObjTxPollingType[tmpLoopCnt].hth = htrh;
				canController->canControllerTxType.
					canControllerObjTxPollingType[tmpLoopCnt].bitPos = (uint32)1U << tmpLoopCnt;
				canController->canControllerTxType.
					canControllerObjTxPollingType[tmpLoopCnt].hwHandle = tmpLoopCnt;
                canController->canControllerTxType.txPollModeMask |= (uint32)1U << tmpLoopCnt;
			}
			canController->canControllerTxType.totalFifoCntPollMode += mailboxCfg->CanHwObjectCount;
		}
	}
}

static void Can_mcanSetUpMSGRAM(
    Can_ControllerObjType *canController,
    const Can_MailboxObjType *canMailbox,
    uint32 maxMbCnt)
{
    uint32 htrh, baseAddr;
    uint8 controller;
    const Can_MailboxType *mailboxCfg;
    const Can_MailboxObjType *mailBoxCfgList;
    Can_FdMsgRAMConfigObjType *msgRamConfig;

    controller = canController->canControllerConfig_PC.ControllerId;
    mailBoxCfgList = &canMailbox[0U];
    baseAddr = canController->canControllerConfig_PC.CntrAddr;
    msgRamConfig = &canController->canFDMsgRamConfig;
    Can_mcanMsgRAMConfigInit(msgRamConfig);
    /* Initialize Message RAM params according to configured mailboxes */
    for (htrh = 0U; htrh < maxMbCnt; htrh++)
    {
        mailboxCfg = &mailBoxCfgList[htrh].mailBoxConfig;
        if (mailboxCfg->Controller->ControllerId == controller)
        {
            if(CAN_MAILBOX_DIRECTION_RX == mailboxCfg->MBDir)
            {
                Can_mcanSetUpRxMailbox(msgRamConfig,
                                       mailboxCfg,
                                       htrh);
            }
            if(CAN_MAILBOX_DIRECTION_TX == mailboxCfg->MBDir)
            {
                Can_mcanSetUpTxMailboxBuf(msgRamConfig,
                                       mailboxCfg,
                                       canController,
                                       htrh,
                                       baseAddr);
            }
        }
    }
    /* Initialize Message RAM params according to configured mailboxes */
    for (htrh = 0U; htrh < maxMbCnt; htrh++)
    {
        mailboxCfg = &mailBoxCfgList[htrh].mailBoxConfig;
        if (mailboxCfg->Controller->ControllerId == controller)
        {
            if(CAN_MAILBOX_DIRECTION_TX == mailboxCfg->MBDir)
            {
                Can_mcanSetUpTxMailboxFifoIntr(msgRamConfig,
                                       mailboxCfg,
                                       canController,
                                       htrh,
                                       baseAddr);
            }
        }
    }
    /* Initialize Message RAM params according to configured mailboxes */
    for (htrh = 0U; htrh < maxMbCnt; htrh++)
    {
        mailboxCfg = &mailBoxCfgList[htrh].mailBoxConfig;
        if (mailboxCfg->Controller->ControllerId == controller)
        {
            if(CAN_MAILBOX_DIRECTION_TX == mailboxCfg->MBDir)
            {
                Can_mcanSetUpTxMailboxFifoPoll(msgRamConfig,
                                       mailboxCfg,
                                       canController,
                                       htrh,
                                       baseAddr);
            }
        }
    }
    msgRamConfig->configParams.lss = (uint32) msgRamConfig->stdFilterNum;
    msgRamConfig->configParams.lse = (uint32) msgRamConfig->extFilterNum;
    /* Configure Message RAM sections:
     * STD/EXT filters, Tx Buffers/FIFO and Rx Buffers/FIFO */
    Can_mcanMsgRAMConfig(baseAddr, msgRamConfig);
}

static void Can_mcanSetUpTxMailboxBuf(
    Can_FdMsgRAMConfigObjType *msgRamConfig,
    const Can_MailboxType *mailboxCfg,
    Can_ControllerObjType *canController,
    uint32 htrh,
    uint32 baseAddr)
{
    Can_ControllerType_PC *canControllerCfg_PC;
    canControllerCfg_PC = &canController->canControllerConfig_PC;
    if(mailboxCfg->CanHwObjectCount == 1U)
    {
        msgRamConfig->txMbMapping[0U][mailboxCfg->HwHandle] = htrh;
        msgRamConfig->txBuffNum += 1U;
        msgRamConfig->configParams.txBufNum++;
        /* Prepare Transmission Interrupt Enable Mask */
		if ((canControllerCfg_PC->TxProcessingType == CAN_TX_RX_PROCESSING_INTERRUPT) ||
			((canControllerCfg_PC->TxProcessingType == CAN_TX_RX_PROCESSING_MIXED) &&
             (mailboxCfg->CanHardwareObjectUsesPolling == (boolean)FALSE)))
        {
            (void) MCAN_txBufTransIntrEnable(baseAddr,
                                             mailboxCfg->HwHandle,
                                             (uint32)TRUE);
        }
    }
}

static void Can_mcanSetUpTxMailboxFifoIntr(
    Can_FdMsgRAMConfigObjType *msgRamConfig,
    const Can_MailboxType *mailboxCfg,
    Can_ControllerObjType *canController,
    uint32 htrh,
    uint32 baseAddr)
{
#if (CAN_MULTIPLEXED_TRANSMISSION_ENABLE == STD_ON)
    Can_ControllerType_PC *canControllerCfg_PC;
    canControllerCfg_PC = &canController->canControllerConfig_PC;
    uint32 intrCnt;
    if(mailboxCfg->CanHwObjectCount > 1U)
    {
        /* Prepare Transmission Interrupt Enable Mask */
		if ((canControllerCfg_PC->TxProcessingType == CAN_TX_RX_PROCESSING_INTERRUPT) ||
			((canControllerCfg_PC->TxProcessingType == CAN_TX_RX_PROCESSING_MIXED) &&
             (mailboxCfg->CanHardwareObjectUsesPolling == (boolean)FALSE)))
		{
            msgRamConfig->txMbMapping[1U][mailboxCfg->HwHandle] = htrh;
            msgRamConfig->txFIFONum += (uint16) mailboxCfg->CanHwObjectCount;
			for(intrCnt = 0U ;
				intrCnt < (uint32)mailboxCfg->CanHwObjectCount;
				intrCnt++)
			{
					(void) MCAN_txBufTransIntrEnable(baseAddr,
												(msgRamConfig->configParams.txBufNum +
                                                 msgRamConfig->configParams.txFIFOSize +
                                                 intrCnt),
												(uint32)TRUE);
            }
            msgRamConfig->configParams.txFIFOSize +=
                                            (uint32)mailboxCfg->CanHwObjectCount;
        }
    }
#endif
}

static void Can_mcanSetUpTxMailboxFifoPoll(
    Can_FdMsgRAMConfigObjType *msgRamConfig,
    const Can_MailboxType *mailboxCfg,
    Can_ControllerObjType *canController,
    uint32 htrh,
    uint32 baseAddr)
{
#if (CAN_MULTIPLEXED_TRANSMISSION_ENABLE == STD_ON)
    Can_ControllerType_PC *canControllerCfg_PC;
    canControllerCfg_PC = &canController->canControllerConfig_PC;
    if(mailboxCfg->CanHwObjectCount > 1U)
    {
		if ((canControllerCfg_PC->TxProcessingType == CAN_TX_RX_PROCESSING_POLLING) ||
			((canControllerCfg_PC->TxProcessingType == CAN_TX_RX_PROCESSING_MIXED) &&
             (mailboxCfg->CanHardwareObjectUsesPolling == (boolean)TRUE)))
		{
            msgRamConfig->txMbMapping[1U][mailboxCfg->HwHandle] = htrh;
            msgRamConfig->txFIFONum += (uint16) mailboxCfg->CanHwObjectCount;
            msgRamConfig->configParams.txFIFOSize +=
                                            (uint32)mailboxCfg->CanHwObjectCount;
        }
    }
#endif
}

static void Can_mcanSetUpRxMailbox(
    Can_FdMsgRAMConfigObjType *msgRamConfig,
    const Can_MailboxType *mailboxCfg,
    uint32 htrh)
{
    uint32 hwFilterIdx;

    for (hwFilterIdx = 0U; hwFilterIdx < mailboxCfg->HwFilterCnt; hwFilterIdx++)
    {
        if(mailboxCfg->CanHwObjectCount > 1U)
        {
            /* Get Filter Mask for HRH - STD */
            if (CAN_ID_STD == mailboxCfg->MBIdType)
            {
				Can_mcanSetUpRxMailboxStdIdFifo(msgRamConfig, mailboxCfg, htrh, hwFilterIdx);
			}
            /* Get Filter Mask for HRH - EXT */
            else if (CAN_ID_XTD == mailboxCfg->MBIdType)
            {
                Can_mcanSetUpRxMailboxExtIdFifo(msgRamConfig, mailboxCfg, htrh, hwFilterIdx);
            }
            /* Get Filter Mask for HRH (+1 for STD and XTD both in
               case of MIXED) */
            else
            {
                Can_mcanSetUpRxMailboxForMixedIdType(msgRamConfig, mailboxCfg,
                                                     hwFilterIdx);
            }
            /* Set Mailbox entry */
            msgRamConfig->rxMbMapping[1U][msgRamConfig->rxFIFONum] = htrh;
            msgRamConfig->rxFIFONum += 1U;
            Can_mcanSetUpRxMailboxUpdateFifoSize(msgRamConfig, mailboxCfg);
        }
        else
        {
            Can_mcanSetUpRxMailboxBuf(msgRamConfig, mailboxCfg, hwFilterIdx);
            msgRamConfig->rxMbMapping[0U][msgRamConfig->rxBuffNum] = htrh;
        }
    }

    if(mailboxCfg->CanHwObjectCount == 1U)
    {
        msgRamConfig->rxBuffNum += 1U;
    }
}

static void Can_mcanSetUpRxMailboxUpdateFifoSize(
    Can_FdMsgRAMConfigObjType *msgRamConfig,
    const Can_MailboxType *mailboxCfg)
{
    Can_TxRxProcessingType    rxProcessingType;

    rxProcessingType = mailboxCfg->Controller->RxProcessingType;
    if (rxProcessingType == CAN_TX_RX_PROCESSING_INTERRUPT)
    {
        msgRamConfig->configParams.rxFIFO0size +=
                                    (uint32)mailboxCfg->CanHwObjectCount;
    }
    else if ((rxProcessingType == CAN_TX_RX_PROCESSING_POLLING) ||
            ((rxProcessingType == CAN_TX_RX_PROCESSING_MIXED) &&
            (mailboxCfg->CanHardwareObjectUsesPolling == (boolean)TRUE)))
    {
        msgRamConfig->configParams.rxFIFO1size +=
                                    (uint32)mailboxCfg->CanHwObjectCount;
    }
    else
    {
        msgRamConfig->configParams.rxFIFO0size +=
                                    (uint32)mailboxCfg->CanHwObjectCount;
    }
}

static void Can_mcanSetUpRxMailboxBuf(
    Can_FdMsgRAMConfigObjType *msgRamConfig,
    const Can_MailboxType *mailboxCfg,
    uint32 hwFilterIdx)
{
	uint32 tmpIdValue;
    uint8 idx = 0U;
    Can_HwFilterType  *pHwFilter;

	pHwFilter = mailboxCfg->HwFilterList[hwFilterIdx];
	tmpIdValue 		  = pHwFilter->IDValue;

    /* Get Filter Mask for HRH - STD */
    if(CAN_ID_STD == mailboxCfg->MBIdType)
    {
        idx = msgRamConfig->stdFilterNum;
        msgRamConfig->stdMsgIDFilterList[idx].sfid1 =
                                    (tmpIdValue &  STD_MSGID_MASK);
        msgRamConfig->stdMsgIDFilterList[idx].sfid2 =
                                        (uint32)msgRamConfig->rxBuffNum;
        msgRamConfig->stdMsgIDFilterList[idx].sfec = 0x7U;
        msgRamConfig->stdMsgIDFilterList[idx].sft = 0x0U;
        msgRamConfig->stdFilterNum++;
    }
    /* Get Filter Mask for HRH - EXT */
    else if (CAN_ID_XTD == mailboxCfg->MBIdType)
    {
        idx = msgRamConfig->extFilterNum;
        msgRamConfig->extMsgIDFilterList[idx].efid1 =
                                    (tmpIdValue & XTD_MSGID_MASK);
        msgRamConfig->extMsgIDFilterList[idx].efid2 =
                                    (uint32)msgRamConfig->rxBuffNum;
        msgRamConfig->extMsgIDFilterList[idx].efec = 0x7U;
        msgRamConfig->extMsgIDFilterList[idx].eft = 0x0U;
        msgRamConfig->extFilterNum++;
    }
    /* Get Filter Mask for HRH
     (+1 for STD and XTD both in case of MIXED) */
    else
    {
        idx = msgRamConfig->stdFilterNum;
        msgRamConfig->stdMsgIDFilterList[idx].sfid1 =
                                ((tmpIdValue >> STD_MSGID_SHIFT) &
                                    STD_MSGID_MASK);
        msgRamConfig->stdMsgIDFilterList[idx].sfid2 =
                                        (uint32)msgRamConfig->rxBuffNum;
        msgRamConfig->stdMsgIDFilterList[idx].sfec = 0x7U;
        msgRamConfig->stdMsgIDFilterList[idx].sft = 0x0U;
        msgRamConfig->stdFilterNum++;

        idx = msgRamConfig->extFilterNum;
        msgRamConfig->extMsgIDFilterList[idx].efid1 =
                                    (tmpIdValue & XTD_MSGID_MASK);
        msgRamConfig->extMsgIDFilterList[idx].efid2 =
                                    (uint32)msgRamConfig->rxBuffNum;
        msgRamConfig->extMsgIDFilterList[idx].efec = 0x7U;
        msgRamConfig->extMsgIDFilterList[idx].eft = 0x0U;
        msgRamConfig->extFilterNum++;
    }
}

static void Can_mcanSetUpRxMailboxStdIdFifo(
    Can_FdMsgRAMConfigObjType *msgRamConfig,
    const Can_MailboxType *mailboxCfg,
    uint32 htrh,
    uint32 hwFilterIdx)
{
    Can_HwFilterType  *pHwFilter;
    Can_TxRxProcessingType    rxProcessingType;
	uint32 tmpIdValue;
	uint32 tmpFilterMask;
    uint8 idx = 0U;

    rxProcessingType = mailboxCfg->Controller->RxProcessingType;
	pHwFilter = mailboxCfg->HwFilterList[hwFilterIdx];
	tmpIdValue 		  = pHwFilter->IDValue;
	tmpFilterMask 	  = pHwFilter->Mask;

	idx               = msgRamConfig->stdFilterNum;
	msgRamConfig->stdMsgIDFilterList[idx].sfid1 =
							((tmpIdValue & STD_MSGID_MASK));
	if (mailboxCfg->CanHandleType == (uint8)CAN_FULL)
	{
		msgRamConfig->stdMsgIDFilterList[idx].sfid2 =
								(STD_MSGID_MASK);
		msgRamConfig->stdMsgIDFilterList[idx].sft = 0x0U;
	}
  /* CAN_CoverageGap_27: Dynamic coverage for the below "else" statement
   * is not covered. Since can handle type FULL is tested.
   */
	else
	{
		msgRamConfig->stdMsgIDFilterList[idx].sfid2 =
								((tmpFilterMask &  STD_MSGID_MASK));
		msgRamConfig->stdMsgIDFilterList[idx].sft = 0x2U;
	}

	if (rxProcessingType == CAN_TX_RX_PROCESSING_INTERRUPT)
	{
		msgRamConfig->stdMsgIDFilterList[idx].sfec = 0x1U;
	}


	else if ((rxProcessingType == CAN_TX_RX_PROCESSING_POLLING) ||
			((rxProcessingType == CAN_TX_RX_PROCESSING_MIXED) &&
			(mailboxCfg->CanHardwareObjectUsesPolling == (boolean)TRUE)))
	{
		msgRamConfig->stdMsgIDFilterList[idx].sfec = 0x2U;
	}
	else
	{
		msgRamConfig->stdMsgIDFilterList[idx].sfec = 0x1U;
	}
	msgRamConfig->stdFilterNum++;
}

static void Can_mcanSetUpRxMailboxExtIdFifo(
    Can_FdMsgRAMConfigObjType *msgRamConfig,
    const Can_MailboxType *mailboxCfg,
    uint32 htrh,
    uint32 hwFilterIdx)
{
    Can_HwFilterType  *pHwFilter;
    Can_TxRxProcessingType    rxProcessingType;
	uint32 tmpIdValue;
	uint32 tmpFilterMask;
    uint8 idx = 0U;

    rxProcessingType = mailboxCfg->Controller->RxProcessingType;
	pHwFilter = mailboxCfg->HwFilterList[hwFilterIdx];
	tmpIdValue 		  = pHwFilter->IDValue;
	tmpFilterMask 	  = pHwFilter->Mask;

    idx = msgRamConfig->extFilterNum;
    msgRamConfig->extMsgIDFilterList[idx].efid1 =
                                (tmpIdValue & XTD_MSGID_MASK);

    if (mailboxCfg->CanHandleType == (uint8)CAN_FULL)
    {
        msgRamConfig->extMsgIDFilterList[idx].efid2 =
                                    (XTD_MSGID_MASK);
        msgRamConfig->extMsgIDFilterList[idx].eft = 0x0U;
    }
    else
    {
        msgRamConfig->extMsgIDFilterList[idx].efid2 =
                                    (tmpFilterMask & XTD_MSGID_MASK);
        msgRamConfig->extMsgIDFilterList[idx].eft = 0x2U;
    }

    if (rxProcessingType == CAN_TX_RX_PROCESSING_INTERRUPT)
    {
        msgRamConfig->extMsgIDFilterList[idx].efec = 0x1U;
    }
    else if ((rxProcessingType == CAN_TX_RX_PROCESSING_POLLING) ||
            ((rxProcessingType == CAN_TX_RX_PROCESSING_MIXED) &&
            (mailboxCfg->CanHardwareObjectUsesPolling == (boolean)TRUE)))
    {
        msgRamConfig->extMsgIDFilterList[idx].efec = 0x2U;
    }
    else
    {
        msgRamConfig->extMsgIDFilterList[idx].efec = 0x1U;
    }
    msgRamConfig->extFilterNum++;
}

static void Can_mcanMsgRAMConfig(
    uint32 baseAddr, Can_FdMsgRAMConfigObjType *canFDMsgRamConfig)
{
    uint32 startAddr, loopCnt, txMbNum, rxMbNum;

    txMbNum = (uint32)canFDMsgRamConfig->txBuffNum + (uint32)canFDMsgRamConfig->txFIFONum;
    rxMbNum = (uint32)canFDMsgRamConfig->rxBuffNum;
    if ((MCAN_TX_BUFFER_MAX_NUM >= txMbNum) &&
        (MCAN_RX_BUFFER_MAX_NUM >= rxMbNum) &&
		(MCAN_RX_FIFO_0_MAX_NUM >= canFDMsgRamConfig->configParams.rxFIFO0size) &&
		(MCAN_RX_FIFO_1_MAX_NUM >= canFDMsgRamConfig->configParams.rxFIFO1size))
    {
        /* Calculate start address for Message RAM sections */
        startAddr = 0U;
        canFDMsgRamConfig->configParams.flssa                  = 0U;
        startAddr += (uint32)(((uint32)canFDMsgRamConfig->stdFilterNum +
               (uint32)1U) * ((uint32)MCAN_MSG_RAM_STD_ELEM_SIZE * (uint32)4U));
        canFDMsgRamConfig->configParams.flesa                  = startAddr;
        startAddr += (uint32)(((uint32)canFDMsgRamConfig->extFilterNum +
               (uint32)1U) * ((uint32)MCAN_MSG_RAM_EXT_ELEM_SIZE * (uint32)4U));
        canFDMsgRamConfig->configParams.txStartAddr            = startAddr;
        startAddr += (uint32)(((uint32)canFDMsgRamConfig->configParams.txBufNum +
        (uint32)1U) * ((uint32)MCAN_MSG_RAM_TX_RX_ELEM_SIZE * (uint32)4U));
        startAddr +=
            (uint32)(((uint32)canFDMsgRamConfig->configParams.txFIFOSize +
            (uint32)1U) * ((uint32)MCAN_MSG_RAM_TX_RX_ELEM_SIZE * (uint32)4U));
        canFDMsgRamConfig->configParams.rxFIFO0startAddr       = startAddr;
        startAddr +=
        (uint32)(((uint32)canFDMsgRamConfig->configParams.rxFIFO0size +
        (uint32)1U) * ((uint32)MCAN_MSG_RAM_TX_RX_ELEM_SIZE * (uint32)4U));
        canFDMsgRamConfig->configParams.rxFIFO1startAddr       = startAddr;
        startAddr +=
        (uint32)(((uint32)canFDMsgRamConfig->configParams.rxFIFO1size +
        (uint32)1U) * ((uint32)MCAN_MSG_RAM_TX_RX_ELEM_SIZE * (uint32)4U));
        canFDMsgRamConfig->configParams.rxBufStartAddr            = startAddr;

        /* Configure Message RAM */
        (void) MCAN_msgRAMConfig(baseAddr, &canFDMsgRamConfig->configParams);

        /* Configure Standard ID filters */
        for(loopCnt = 0U ; loopCnt < (uint32) canFDMsgRamConfig->stdFilterNum ;loopCnt++)
        {
            MCAN_addStdMsgIDFilter(
                            baseAddr,
                            loopCnt,
                            &canFDMsgRamConfig->stdMsgIDFilterList[loopCnt]);
        }
        /* Configure Extended ID filters */
        for(loopCnt = 0U ; loopCnt < (uint32) canFDMsgRamConfig->extFilterNum ;loopCnt++)
        {
            MCAN_addExtMsgIDFilter(
                            baseAddr,
                            loopCnt,
                            &canFDMsgRamConfig->extMsgIDFilterList[loopCnt]);
        }
    }
#if (CAN_DEV_ERROR_DETECT == STD_ON)
    else
    {
        (void) Det_ReportError((uint16) CAN_MODULE_ID,
                               (uint8) CAN_INSTANCE_ID,
                               (uint8)CAN_INIT_ID,
                               (uint8) CAN_E_PARAM_HANDLE);
    }
#endif
}

static void Can_mcanSetId(
    const Can_PduType *PduInfo, MCAN_TxBufElementNoCpy *elem)
{
    if ((PduInfo->id & CAN_MSG_ID_TYPE_EXT) == CAN_MSG_ID_TYPE_EXT)
    {
        elem->xtd = CAN_ID_XTD;
        elem->id = PduInfo->id & XTD_MSGID_MASK;
    }
    else
    {
        elem->xtd = CAN_ID_STD;
        elem-> id = (PduInfo->id & STD_MSGID_MASK) << STD_MSGID_SHIFT;
    }
}

static void Can_mcanSetUpRxMailboxForMixedIdType(
    Can_FdMsgRAMConfigObjType *msgRamConfig, const Can_MailboxType *mailboxCfg,
    uint32 hwFilterIdx)
{
    uint8  idx;
    Can_HwFilterType  *pHwFilter;
    uint32 tmpIdValue, tmpFilterMask;
    Can_TxRxProcessingType    rxProcessingType;

    rxProcessingType = mailboxCfg->Controller->RxProcessingType;

    pHwFilter = mailboxCfg->HwFilterList[hwFilterIdx];

    tmpIdValue = pHwFilter->IDValue;
	tmpFilterMask = pHwFilter->Mask;

    idx = msgRamConfig->stdFilterNum;

    msgRamConfig->stdMsgIDFilterList[idx].sfid1 =
                            ((tmpIdValue >> STD_MSGID_SHIFT) &
                                STD_MSGID_MASK);
	if (mailboxCfg->CanHandleType == (uint8)CAN_FULL)
	{
		msgRamConfig->stdMsgIDFilterList[idx].sfid2 =
								(STD_MSGID_MASK);
		msgRamConfig->stdMsgIDFilterList[idx].sft = 0x0U;
	}
	else
	{
		msgRamConfig->stdMsgIDFilterList[idx].sfid2 =
								((tmpFilterMask &  STD_MSGID_MASK));
		msgRamConfig->stdMsgIDFilterList[idx].sft = 0x2U;
	}

    if (rxProcessingType == CAN_TX_RX_PROCESSING_INTERRUPT)
    {
        msgRamConfig->stdMsgIDFilterList[idx].sfec = 0x1U;
    }
    else if ((rxProcessingType == CAN_TX_RX_PROCESSING_POLLING) ||
			((rxProcessingType == CAN_TX_RX_PROCESSING_MIXED) &&
			(mailboxCfg->CanHardwareObjectUsesPolling == (boolean)TRUE)))
    {
        msgRamConfig->stdMsgIDFilterList[idx].sfec = 0x2U;
    }

    /* CAN_CoverageGap_28: Dynamic coverage for the below "else" statement
     * is not covered. Since RXProcessing Type CAN_TX_RX_PROCESSING_INTERRUPT
     * is tested.
     */
    else
    {
        msgRamConfig->stdMsgIDFilterList[idx].sfec = 0x1U;
    }
    msgRamConfig->stdFilterNum++;

    /* To reduce cyclomatic complexity Extended Id created in diff function */
    Can_mcanSetUpRxMailboxForMixedIdExtType(msgRamConfig, mailboxCfg, hwFilterIdx);
}

static void Can_mcanSetUpRxMailboxForMixedIdExtType(
    Can_FdMsgRAMConfigObjType *msgRamConfig, const Can_MailboxType *mailboxCfg,
    uint32 hwFilterIdx)
{
    uint8  idx;
    Can_HwFilterType  *pHwFilter;
    uint32 tmpIdValue, tmpFilterMask;
    Can_TxRxProcessingType    rxProcessingType;

    rxProcessingType = mailboxCfg->Controller->RxProcessingType;

    pHwFilter = mailboxCfg->HwFilterList[hwFilterIdx];

    tmpIdValue = pHwFilter->IDValue;
	tmpFilterMask = pHwFilter->Mask;

    idx = msgRamConfig->extFilterNum;
    msgRamConfig->extMsgIDFilterList[idx].efid1 =
                                (tmpIdValue & XTD_MSGID_MASK);

	if (mailboxCfg->CanHandleType == (uint8)CAN_FULL)
	{
		msgRamConfig->extMsgIDFilterList[idx].efid2 =
									(XTD_MSGID_MASK);
		msgRamConfig->extMsgIDFilterList[idx].eft = 0x0U;
	}
	else
	{
		msgRamConfig->extMsgIDFilterList[idx].efid2 =
									(tmpFilterMask & XTD_MSGID_MASK);
		msgRamConfig->extMsgIDFilterList[idx].eft = 0x2U;
	}

    if (rxProcessingType == CAN_TX_RX_PROCESSING_INTERRUPT)
    {
        msgRamConfig->extMsgIDFilterList[idx].efec = 0x1U;
    }
    else if ((rxProcessingType == CAN_TX_RX_PROCESSING_POLLING) ||
			((rxProcessingType == CAN_TX_RX_PROCESSING_MIXED) &&
			(mailboxCfg->CanHardwareObjectUsesPolling == (boolean)TRUE)))
    {
        msgRamConfig->extMsgIDFilterList[idx].efec = 0x2U;
    }

    /* CAN_CoverageGap_29: Dynamic coverage for the below "else" statement
     * is not covered. Since RXProcessing Type CAN_TX_RX_PROCESSING_INTERRUPT
     * is tested.
     */
    else
    {
        msgRamConfig->extMsgIDFilterList[idx].efec = 0x1U;
    }
    msgRamConfig->extFilterNum++;
}

/*******************************************************************************
 * Can_mcanRamInitDone
 ******************************************************************************/
/*! \brief      Checks the initialization of Message RAM done or not.
 *
 *  \param[in]  uint32 baseAddr -  Base Address of controller
 *
 ******************************************************************************/
static void Can_mcanRamInitDone(uint32 baseAddr)
{
    StatusType  status;
    TickType    startCount = 0U, tempCount = 0U, elapsedCount = 0U;

    status = GetCounterValue(CAN_OS_COUNTER_ID, &startCount);
    if (((StatusType) E_OK) == status)
    {
        /* Wait for FSM to to come out of reset */
        do
        {
            /* Below API can change start time, so use temp variable */
            tempCount = startCount;
            status = GetElapsedValue(
                         CAN_OS_COUNTER_ID,
                         &tempCount,
                         &elapsedCount);
            if ((((StatusType) E_OK) != status) ||
                (elapsedCount >= CAN_TIMEOUT_DURATION))
            {
              /* CAN_CoverageGap_30: Dynamic coverage for the below DEM event is not covered.
               * Since it is hardware dependent.
               */

                /* timeout */
#if (CAN_E_HARDWARE_ERROR != CAN_DEM_NO_EVENT)
                (void)Dem_SetEventStatus(
                    CAN_E_HARDWARE_ERROR, DEM_EVENT_STATUS_FAILED);
#endif
                break;
            }
        } while (MCAN_isMemInitDone(baseAddr) != (uint32) TRUE);
    }
}

#define CAN_STOP_SEC_CODE
#include "Can_MemMap.h"
