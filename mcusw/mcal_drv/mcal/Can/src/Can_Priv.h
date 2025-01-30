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
 *  \file     Can_Priv.h
 *
 *  \brief    This file contains cover page for generating doxygen
 *            API documentation.
 */

/* Protection against multiple inclusion */
#ifndef CAN_PRIV_H_
#define CAN_PRIV_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "Std_Types.h"
#include "Can.h"
#include "CanIf_Cbk.h"

/*  Other Header Files */
#include "Can.h"
#include "Det.h"
#include <hw_include/lld_mcan.h>
#include <hw_include/lld_types.h>
/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define CAN_FD_PAYLOAD_MAX_BYTES           (64U)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */
/**
 * \brief Can FD controller Message RAM Configuration object structure.
 *        This structure will only be used in CAN FD controller i.e. MCAN
 */
typedef struct Can_FdMsgRAMConfigObjType_s
{
    MCAN_MsgRAMConfigParams     configParams;
    /*!< MCAN Message RAM Configuration parameters */
    MCAN_StdMsgIDFilterElement stdMsgIDFilterList[MCAN_RX_BUFFER_MAX_NUM];
    /*!< Standard ID filter elements */
    MCAN_ExtMsgIDFilterElement extMsgIDFilterList[MCAN_RX_BUFFER_MAX_NUM];
    /*!< Extended ID filter elements */
    uint8                 stdFilterNum;
    /*!< Standard ID filter element number */
    uint8                 extFilterNum;
    /*!< Extended ID filter element number */
    uint32                txMbMapping[2U][MCAN_TX_BUFFER_MAX_NUM];
    /*!< Mapping of HTRH
     *   txMbMapping[0U][0-MCAN_TX_BUFFER_MAX_NUM]: HTH to be stored into Buffer
     *   txMbMapping[1U][0-MCAN_TX_BUFFER_MAX_NUM]: HTH to be stored into FIFO
     *   txMbMapping[m][n] = 0xFF : Initialization value */
    uint32                rxMbMapping[2U][MCAN_RX_BUFFER_MAX_NUM];
    /*!< Mapping of HTRH
     *   rxMbMapping[0U][0-MCAN_RX_BUFFER_MAX_NUM]: HRH is stored into Buffer
     *   rxMbMapping[1U][0-MCAN_RX_BUFFER_MAX_NUM]: HRH is stored into FIFO
     *   rxMbMapping[m][n] = 0xFF : Initialization value */
    uint32                hthToMbMapping[MCAN_TX_BUFFER_MAX_NUM];
    /*!< Mapping of HTH to hw mailboxes */
    uint16                txBuffNum;
    /*!< Tx number of Buffer elements */
    uint16                txFIFONum;
    /*!< Tx number of FIFO elements */
    uint16                rxBuffNum;
    /*!< Rx number of Buffer elements */
    uint16                rxFIFONum;
    /*!< Rx number of FIFO elements */
} Can_FdMsgRAMConfigObjType;

/**
 * \brief Can Driver object type strucuture.
 */
typedef struct Can_ControllerTxObjType_s
{
    uint32 hth;
    /**< Mapping of HTH to hw mailboxes */
    uint32 bitPos;
    /**< Mapping of Buffer Number to hw mailboxes */
    uint32 hwHandle;
    /**< Actual hw mailboxes */
} Can_ControllerTxObjType;

/**
 * \brief Can Driver object type strucuture.
 */
typedef struct Can_ControllerRxObjType_s
{
    uint32 hrh;
    /**< Mapping of HRH to hw mailboxes */
    uint32 bitPosLow;
    /**< Mapping of Lower 32 Buffer to hw mailboxes */
    uint32 bitPosHigh;
    /**< Mapping of Higher 32 Buffer to hw mailboxes */
    uint32 hwHandle;
    /**< Actual hw mailboxes */
} Can_ControllerRxObjType;

typedef struct Can_ControllerRxObjFifoType_s
{
    uint32 hrh;
    /**< Mapping of HRH to hw FIFO */
} Can_ControllerRxObjFifoType;


/**
 * \brief Can Tx object type strucuture.
 */
typedef struct Can_ControllerObjTxType_s
{
    Can_ControllerTxObjType canControllerObjTxInterruptType[MCAN_TX_BUFFER_MAX_NUM];
    /**< Can Tx object Interrupt type Buffer Mode */
    Can_ControllerTxObjType canControllerObjTxPollingType[MCAN_TX_BUFFER_MAX_NUM];
    /**< Can Tx object Polling type Buffer Mode */
    Can_ControllerTxObjType canControllerObjTxInterruptTypeFifo[MCAN_TX_BUFFER_MAX_NUM];
    /**< Can Tx object Interrupt type FIFO Mode */
    Can_ControllerTxObjType canControllerObjTxPollingTypeFifo[MCAN_TX_BUFFER_MAX_NUM];
    /**< Can Tx object Polling type FIFO Mode */
    uint32 totalBufCntPollMode;
    /**< Total Mailboxes Count Tx object Polling type Buffer Mode */
    uint32 totalBufCntIntrMode;
    /**< Total Mailboxes Count Tx object Interrupt type Buffer Mode */
    uint32 totalFifoCntPollMode;
    /**< Total Maiboxes Count Tx object Polling type FIFO Mode */
    uint32 totalFifoCntIntrMode;
    /**< Total Maiboxes Count Tx object Interrupt type FIFO Mode */
    uint32 txPollModeMask;
    /**< Tx Mask configured in Polled mode */
    uint32 txIntrModeMask;
    /**< Tx Mask configured in interrupt mode */
} Can_ControllerObjTxType;

/**
 * \brief Can Rx object type strucuture.
 */
typedef struct Can_ControllerObjRxType_s
{
    Can_ControllerRxObjType canControllerObjRxInterruptType[MCAN_RX_BUFFER_MAX_NUM];
    /**< Can Rx object Interrupt type Buffer Mode */
    Can_ControllerRxObjType canControllerObjRxPollingType[MCAN_RX_BUFFER_MAX_NUM];
    /**< Can Rx object Polling type Buffer Mode */
    Can_ControllerRxObjFifoType canControllerObjRxInterruptTypeFifo[MCAN_RX_BUFFER_MAX_NUM];
    /**< Can Rx object Interrupt type FIFO Mode */
    Can_ControllerRxObjFifoType canControllerObjRxPollingTypeFifo[MCAN_RX_BUFFER_MAX_NUM];
    /**< Can Rx object Polling type FIFO Mode */
    uint32 totalBufCntPollMode;
    /**< Total Mailboxes Count Rx object Polling type Buffer Mode */
    uint32 totalBufCntIntrMode;
    /**< Total Mailboxes Count Rx object Interrupt type Buffer Mode */
    uint32 totalFifoCntPollMode;
    /**< Total Maiboxes Count Rx object Polling type FIFO Mode */
    uint32 totalFifoCntIntrMode;
    /**< Total Maiboxes Count Rx object Interrupt type FIFO Mode */
    uint32 bufIntrModeMaskLow;
    /**< Mask configured in interrupt mode Lower 32 bits */
    uint32 bufIntrModeMaskHigh;
    /**< Mask configured in interrupt mode Higher 32 bits */
    uint32 bufPollModeMaskLow;
    /**< Mask configured in interrupt mode Lower 32 bits */
    uint32 bufPollModeMaskHigh;
    /**< Mask configured in interrupt mode Higher 32 bits */
} Can_ControllerObjRxType;

/**
 * \brief Can controller object strucuture.
 */
typedef struct Can_ControllerObjType_s
{
    boolean               canBusOffRecoveryStatus;
    /*!< Bus off recovery status */
    uint8                 canTxStatus[MCAN_TX_BUFFER_MAX_NUM];
    /*!< TxRXx Status of Mailbox */
    uint8                 canInterruptCounter;
    /*!< Total number of interrupts for this controller */
    uint16                canBaud;
    /*!< Set BRP value */
    uint8                 canSduPtr[CAN_FD_PAYLOAD_MAX_BYTES];
    /*!< SDU pointer */
    Can_ControllerType    canControllerConfig;
    /**< Can Controller */
    Can_ControllerType_PC canControllerConfig_PC;
    /**< Can Controller Pre-Compile Configuration */
    uint32                maxBaudConfigID;
    /**< Max Baud Config Index in BaudRateConfigList */
    Can_ControllerStateType   canState;
    /**< State of Can controller */
    Can_HwType            mailboxCfg;
    /**< Structure which includes HOH, CAN Controller ID and specific CanId */
    PduInfoType           pduInfo;
    /**< Structure which includes pointer to the SDU and it's length */
    Can_FdMsgRAMConfigObjType   canFDMsgRamConfig;
    /*!< MCAN Message RAM configuration parameters */
    Can_ControllerObjTxType canControllerTxType;
    /**< Can Tx mailbox object type strucuture. */
    Can_ControllerObjRxType canControllerRxType;
    /**< Can Rx mailbox object type strucuture. */
    boolean txMixedModeIntrEnable;
    /**< Tx Interrupt Enable flag mixed mode */
    boolean rxMixedModeIntrEnable;
    /**< Rx Interrupt Enable flag mixed mode */
} Can_ControllerObjType;

/**
 * \brief Can HWObject to Mailbox mapping type
 */
typedef uint32 Can_MbHTRHMapping_Type;

/**
 * \brief Can Mailbox type strucuture.
 */
typedef struct Can_MailboxObjType_s
{
    Can_MailboxType    mailBoxConfig;
    /**< Mailbox Config Info */
    Can_MailboxType_PC mailBoxConfig_PC;
    /**< Mailbox Config Info Pre-Compile Configuration */
    PduIdType          canTxRxPduId[MCAN_TX_BUFFER_MAX_NUM];
    /**< Tx/Rx Pdu ID Info */
} Can_MailboxObjType;

/**
 * \brief Can Driver object type strucuture.
 */
typedef struct Can_DriverObjType_s
{
    Can_ControllerObjType canController[CAN_NUM_CONTROLLER];
    /**< List of enabled Controllers */
    Can_MailboxObjType    canMailbox[CAN_NUM_MAILBOXES];
    /**< MB array for all controllers */
    uint8                 canMaxControllerCount;
    /**< MaxCount of Controller in Controller List */
    uint32                maxMbCnt;
    /**< MaxMbCount in MB list in all controller */
    uint32                controllerIDMap[CAN_MAX_CONTROLLER];
    /**< CAN Controller ID mapping */
} Can_DriverObjType;

/* ========================================================================== */
/*                         GLOBAL VARIABLES                                   */
/* ========================================================================== */
extern VAR(uint8, CAN_VAR_INIT) Can_DrvState;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
FUNC(void, CAN_CODE) Can_MessageRamInit(uint32 instance);

FUNC(void, CAN_CODE) Can_hwUnitConfig(Can_DriverObjType *drvObj,
									  Can_ControllerObjType *canController,
                                      const Can_MailboxObjType *canMailbox,
                                      uint32 maxMbCnt);

boolean Can_hwUnitStart(Can_ControllerObjType *controllerObj);

boolean Can_hwUnitStop(Can_ControllerObjType *controllerObj);

boolean Can_hwUnitSleep(Can_ControllerObjType *controllerObj);

boolean Can_hwUnitWkUp(Can_ControllerObjType * controllerObj);

Std_ReturnType Can_writeGetFreeMsgObj(const Can_MailboxType *mailboxCfg,
                                      const Can_ControllerObjType *controllerObj,
                                      uint32 *msgObj,
                                      uint32 htrh);

void Can_writeTxMailbox(
    const Can_MailboxType *mailboxCfg,
    Can_ControllerObjType *controllerObj,
	uint32   hth,
    uint32   messageBox,
    const Can_PduType *pduInfo);
void Can_hwUnitDisableInterrupts(Can_ControllerObjType *controllerObj);

void Can_hwUnitEnableInterrupts(Can_ControllerObjType *controllerObj);

void Can_hwUnitSetBaudrate(uint32 baseAddr,
                           const Can_BaudConfigType *setBaud);

void Can_MainFunction_BusOffProcess(Can_ControllerObjType *canController);

void Can_MainFunction_ModeProcess(Can_ControllerObjType *canController);

#if (CAN_RX_POLLING == STD_ON)
void Can_MainFunction_ReadProcess(Can_DriverObjType *can_DriverObj);
#endif

Std_ReturnType Can_hwUnitTestLoopBackModeEnable(uint32 baseAddr,
                                                uint8 mode);

Std_ReturnType Can_hwUnitTestLoopBackModeDisable(uint32 baseAddr,
                                                 uint8 mode);

/* MCAN related functions */
void Can_mcanConfigController(Can_DriverObjType *drvObj,
							  Can_ControllerObjType *canController,
                              const Can_MailboxObjType *canMailbox,
                              uint32 maxMbCnt);

boolean Can_mcanStart(Can_ControllerObjType *controllerObj);

boolean Can_mcanStop(Can_ControllerObjType * controllerObj);

boolean Can_mcanSleep(Can_ControllerObjType * controllerObj);

boolean Can_mcanWkUp(Can_ControllerObjType * controllerObj);

Std_ReturnType Can_mcanTestLoopBackModeEnable(uint32 baseAddr, uint8 mode);

Std_ReturnType Can_mcanTestLoopBackModeDisable(uint32 baseAddr, uint8 mode);

Std_ReturnType Can_mcanWriteGetFreeMsgObj(const Can_MailboxType *mailboxCfg,
                                          const Can_ControllerObjType *canController,
                                          uint32 *msgObj,
                                          uint32 htrh);

void Can_mcanWriteTxMailbox(const Can_MailboxType *mailboxCfg,
                            Can_ControllerObjType *controllerObj,
                            uint32   messageBox,
                            const Can_PduType *pduInfo,
                            uint32   hth);

void Can_mcanSetBaudrate(uint32 baseAddr, const Can_BaudConfigType *setBaud);

void Can_mcanDisableInterrupts(Can_ControllerObjType *controllerObj);

void Can_mcanEnableInterrupts(Can_ControllerObjType *canController);

void Can_mcanTxDonePolling(Can_ControllerObjType *canController,
                           const Can_MailboxObjType *canMailbox);

void Can_mcan_BusOffProcess(Can_ControllerObjType *canController);

void Can_mcan_ModeProcess(Can_ControllerObjType *canController);

#if (CAN_RX_POLLING == STD_ON)
void Can_mcan_ReadProcess(Can_ControllerObjType *controllerObj,
                          const Can_MailboxObjType *canMailbox,
                          uint32 maxMbCnt);
#endif

void Can_mcanReadRxFIFO(Can_ControllerObjType *controllerObj,
                        const Can_MailboxObjType *canMailbox);
void Can_mcanReadRxFIFO1(Can_ControllerObjType *controllerObj,
                        const Can_MailboxObjType *canMailbox);
void Can_mcanProcessISR(Can_ControllerObjType *canController,
                        const Can_MailboxObjType *canMailbox);

void Can_mcanProcessISRTx(Can_ControllerObjType *canController,
                          const Can_MailboxObjType *canMailbox);

void Can_mcanProcessISRRx(Can_ControllerObjType *controllerObj,
                          const Can_MailboxObjType *canMailbox,
                          uint32 maxMbCnt);
void Can_hwDeInit(const Can_ControllerObjType *canController);
void Can_mcanHwDeInit(const Can_ControllerObjType *canController);
Can_ErrorStateType Can_mcanGetProtocolStatus(
    const Can_ControllerObjType *canController);
#ifdef __cplusplus
}
#endif

#endif /* CAN_PRIV_ */

/*  End of File: Can_Priv.h */
