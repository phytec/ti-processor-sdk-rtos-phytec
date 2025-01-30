/*
*
* Copyright (C) 2024 Texas Instruments Incorporated
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
 *  \file     EthUtils_Target.h
 *
 *  \brief    This file contains Eth ultils target function.declarations.
 */


#ifndef ETHUTILS_TARGET_H_
#define ETHUTILS_TARGET_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#ifdef freertos
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "portmacro.h"
#endif

#include "Eth.h"
#include "EthTrcv.h"
#include "Std_Types.h"

#if (STD_ON == ETH_WRAPPER_ENABLED)
#include "Eth_30_Wrapper.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/** \brief RGMII_ID_MODE shift value in CTRLMMR_MCU_ENET_CTRL register */
#define ENET_CTRL_RGMII_ID_SHIFT            (4U)

/** \brief No internal TX delay in RGMII interface (CTRLMMR_MCU_ENET_CTRL) */
#define ENET_CTRL_RGMII_ID_INTTXDLY         ((boolean)0U)

/** \brief Internal TX delay in RGMII interface (CTRLMMR_MCU_ENET_CTRL) */
#define ENET_CTRL_RGMII_ID_NODELAY          ((boolean)1U)

/** \brief RMII port interface (CTRLMMR_MCU_ENET_CTRL) */
#define ENET_CTRL_MODE_RMII                 (1U)

/** \brief RGMII port interface (CTRLMMR_MCU_ENET_CTRL) */
#define ENET_CTRL_MODE_RGMII                (2U)

/** \brief Packet transmit retry in non-confirmation mode */
#define ETHAPP_TRANSMIT_RETRY_COUNT         (2U)

/** \brief Number of CPSW Statics Block */
#define CPSW_STATS_BLOCK_ELEM_NUM           (128U)

/** \brief one second in unit of nanosecond */
#define SEC_TO_NS                           (1000000000LL)

/** \brief converts 6 bytes seconds valud of 'Eth_TimeStampType' to uint64 seconds integer  */
#define SEC32_TO_SEC64(ts)                  ((((uint64)ts.secondsHi)<<32)+((uint64)ts.seconds))

/** \brief convert 'Eth_TimeStampType' vaule to nanosecond integer */
#define TS2NSEC(ts)                         ((((uint64)SEC32_TO_SEC64(ts))*SEC_TO_NS)+((uint64)ts.nanoseconds))

/** \brief ts1-ts2 in 64-bit nanosecond unit */
#define TS_DIFF64NS(ts2,ts1)                ((sint64)(TS2NSEC(ts2)-TS2NSEC(ts1)))

#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || \
    defined (SOC_J784S4)
#define OS_TIME_SCALE_VALUE                 (1000000U)
#elif defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined(SOC_J722S)
#define OS_TIME_SCALE_VALUE                 (1U)
#else
#error "Unsupported SoC"
#endif /* defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || \
          defined (SOC_J784S4) */

#if (STD_ON == ETH_VIRTUALMAC_SUPPORT)
    #define ETH_RX_IRQ_PACING_HW_TIMER_IRQ_NUM                  (291U)
    #define ETH_TX_IRQ_PACING_HW_TIMER_IRQ_NUM                  (292U)
#else
    #if defined (SOC_J721E) || defined(SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
        #define ETH_RX_IRQ_PACING_HW_TIMER_IRQ_NUM              (40U)
        #define ETH_TX_IRQ_PACING_HW_TIMER_IRQ_NUM              (41U)
    #elif defined (SOC_AM62AX) || defined (SOC_AM62PX) ||  defined(SOC_J722S)
        #define ETH_RX_IRQ_PACING_HW_TIMER_IRQ_NUM              (138U)
        #define ETH_TX_IRQ_PACING_HW_TIMER_IRQ_NUM              (139U)
    #elif defined (SOC_AM62X)
        /*
         * Note: for AM62x only 2 timer available for now.
         * The timer index 1 already use for OS counter.
         * So both Tx/Rx can only use the timer id 0.
         * User should test Rx/Tx Irq pacing seperatly
         */
        #define ETH_RX_IRQ_PACING_HW_TIMER_IRQ_NUM              (138U)
        #define ETH_TX_IRQ_PACING_HW_TIMER_IRQ_NUM              (138U)
    #endif
#endif

#define ETH_DEFAULT_TX_PRIORITY         (0u)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */


typedef boolean (*TestFunc)(void);

/** \brief Ethernet test statistics (not to be confused with Ethernet statistics) */
typedef struct EthApp_Stats_s
{
    volatile uint32 txPktCnt;
    /**< Number of data transmitted packets (Eth_Transmit()) */

    volatile uint32 txConfPktCnt;
    /**< Number of confirmed transmit packets (Eth_TxConfirmation()) */

    volatile uint32 txBufLenErr;
    /**< Number of buffer requests with BUFREQ_E_OVFL error (Eth_ProvideTxBuffer()) */

    volatile uint32 txBufProvErr;
    /**< Number of buffer requests with BUFREQ_E_NOT_OK error (Eth_ProvideTxBuffer()) */

    volatile uint32 txPktErr;
    /**< Number of data transmit errors (Eth_Transmit()) */

    volatile uint32 rxPktCnt;
    /**< Number of data received packets (Eth_Receive() or EthIf_RxIndication()) */

    volatile uint32 rxExp3PktCnt;
    /**< Number of data received packets with experimental 3 type (EthIf_RxIndication()) */

    volatile uint32 rxExp3BytesCnt;
    /**< Number of data received packets (Eth_Receive() or EthIf_RxIndication()) */

    volatile uint32 rxExp1PktCnt;
    /**< Number of data received packets with experimental 1 type (EthIf_RxIndication()) */

    volatile uint32 rxExp2PktCnt;
    /**< Number of data received packets with experimental 2 type (EthIf_RxIndication()) */

    volatile uint32 rxVlanPktCnt;
    /**< Number of data received packets with VLAN tag (EthIf_RxIndication()) */

    volatile uint32 rxPktNotRecv;
    /**< Number of times Eth_Receive() returned packet not received */

    volatile uint32 rxPktIndCnt;
    /**< Number of indicated receive packets (EthIf_RxIndication()) */

    volatile uint32 rxEtherTypeErr;
    /**< Number of packets with an unexpected EtherType field (EthIf_RxIndication()) */

    volatile uint32 rxPayloadErr;
    /**< Number of packets with unexpected payload (EthIf_RxIndication()) */

    volatile uint32 rxCtrlIdxErr;
    /**< Number of packets with unexpected controller index (EthIf_RxIndication()) */

    volatile uint32 rxBcastErr;
    /**< Number of packets with unexpected broadcast flag */

    volatile uint32 rxLenErr;
    /**< Number of packets with invalid buffer length */

    volatile uint32 ctrlModeActCnt;
    /**< Number of controller mode changes to ETH_MODE_ACTIVE (Eth_SetControllerMode()) */

    volatile uint32 ctrlModeDownCnt;
    /**< Number of controller mode changes to ETH_MODE_ACTIVE (Eth_SetControllerMode()) */

    volatile uint32 ctrlModeErr;
    /**< Number of controller mode changes with error (Eth_SetControllerMode()) */

    volatile uint32 ctrlModeIndActCnt;
    /**< Number of controller mode changes to ETH_MODE_ACTIVE (EthIf_CtrlModeIndication()) */

    volatile uint32 ctrlModeIndDownCnt;
    /**< Number of controller mode changes to ETH_MODE_DOWN (EthIf_CtrlModeIndication()) */

    volatile uint32 ctrlModeIndErr;
    /**< Invalid controller mode errors (EthIf_CtrlModeIndication()) */

    volatile uint32 filterUcastAddErr;
    /** Number of filter "add" action errors (Eth_UpdatePhysAddrFilter()) */

    volatile uint32 filterUcastRemErr;
    /** Number of filter "add" action (unicast) errors (Eth_UpdatePhysAddrFilter()) */

    volatile uint32 filterUcastRxErr;
    /** Number of errors while receiving unicast packets */

    volatile uint32 filterMcastAddErr;
    /** Number of filter "remove" action (multicast) errors (Eth_UpdatePhysAddrFilter()) */

    volatile uint32 filterMcastRemErr;
    /** Number of filter "remove" action (multicast) errors (Eth_UpdatePhysAddrFilter()) */

    volatile uint32 filterMcastRxErr;
    /** Number of errors while receiving multicast packets */

    volatile uint32 filterBcastErr;
    /** Number of filter "add" action (broadcast) errors (Eth_UpdatePhysAddrFilter()) */

    volatile uint32 filterBcastRxErr;
    /** Number of errors while receiving broadcast packets */

    volatile uint32 filterBcastTxErr;
    /** Number of errors while transmitting broadcast packets */

    volatile uint32 filterNullErr;
    /** Number of filter "add" action (null) errors (Eth_UpdatePhysAddrFilter()) */

    volatile uint32 filterNullRxErr;
    /** Number of errors while receiving unicast packets */

    volatile uint32 etherGetCountValuesErr;
    /**< Ethernet get counter values errors (Eth_GetCounterValues()) */

    volatile uint32 etherGetRxStasErr;
    /**< Ethernet get Rx statistics errors (Eth_GetRxStats()) */

    volatile uint32 etherGetTxStasErr;
    /**< Ethernet get Tx statistics errors (Eth_GetTxStats()) */

    volatile uint32 etherGetTxErrStasErr;
    /**< Ethernet get Tx error statistics errors (Eth_GetTxErrorCounterValues()) */

    volatile uint32 demErr;
    /**< DEM errors (Dem_SetEventStatus()) */

    volatile uint32 detErr;
    /**< DET errors (Det_ReportError()) */

    volatile uint32 qosPacketCnt[ETH_PRIORITY_QUEUE_NUM];
    /**< Number of data received packets for QOS value */

    volatile uint32 qosBytesCnt[ETH_PRIORITY_QUEUE_NUM];
    /**< Number of data received packets for QOS value */
} EthApp_Stats;

/** \brief Ethernet test state */
typedef struct EthApp_State_s
{
    uint8 ctrlIdx;
    /**< Controller index */

    uint8 macAddr[ETH_MAC_ADDR_LEN];
    /**< MAC address of the controller */

    volatile boolean runTest;
    /**< Whether test should be running or not */

    volatile boolean hostUp;
    /**< Whether or not host application eth interface is up and running */

    volatile boolean ackRecv;
    /**< Whether or not host application ack our command receive notification */

    boolean saveRxFrame;
    /**< Whether or not the received frame should be saved */

    EthFrame rxFrame;
    /**< Receive frame to copy receive data into in some tests */

    EthApp_Stats stats;
    /**< Test statistics (not to be confused with Ethernet statistics) */

    boolean verbose;
    /**< Enable verbose */

    boolean veryVerbose;
    /**< Enable very verbose logs */

    boolean saveRxTime;
    /* Enable save Rx Time flags */

    uint16 rxFrameCnt;
    /* Rx frame counter */

    Eth_TimeStampType saveRxTimeStamp[ETH_TEST_ITER_S_COUNT];
    /* Rx frame timestamp */

    boolean saveTxTime;
    /* Enable save Tx Time flags */

    uint16 txFrameCnt;
    /* Tx frame counter */

    Eth_TimeStampType saveTxTimeStamp[ETH_TEST_ITER_S_COUNT];
    /* Tx frame timestamp */

    uint32 useExtTxBuffer;
    /* use extent Tx buffer*/

    boolean qosTest;
    /* Enable Qos test to calculate QOS bandwidth */

    boolean sendMultipleStopCmd;
    /* enable/disable multiple tx of STOP CMD in EthApp_transmit */
    uint8 testIdx;
    /* Select specific test case */
    void* testDoneNotifySem;
    /**< Semaphore Handle to indicate test complete */
} EthApp_State;


typedef struct EthApp_StateSet_s
{
    EthApp_State state[ETH_CTRL_ID_MAX];
} EthApp_StateSet;

/* ========================================================================== */
/*                         Local Variable                                     */
/* ========================================================================== */
/* Broadcast MAC address */
static uint8 BcastAddr[ETH_HW_ADDR_LEN] =
{
    0xffU, 0xffU, 0xffU, 0xffU, 0xffU, 0xffU
};

static uint8 TargetMcastAddr[ETH_HW_ADDR_LEN] =
{
    0x01U, 0x02U, 0x03U, 0x04U, 0x05U, 0x06U
};

/* Null MAC address */
static uint8 NullAddr[ETH_HW_ADDR_LEN] =
{
    0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U
};

/* Unicast MAC address */
static uint8 UcastAddr[ETH_HW_ADDR_LEN] =
{
    0x0aU, 0x0bU, 0x0cU, 0x0dU, 0x0eU, 0x0fU
};

/* Multicast MAC address */
static uint8 McastAddr[ETH_HW_ADDR_LEN] =
{
    0x01U, 0x00U, 0x5eU, 0x0dU, 0x0eU, 0x0fU
};

/* Transmit Multicast MAC address */
static uint8 TxMcastAddr[ETH_HW_ADDR_LEN] =
{
    0x33U, 0x33U, 0x00U, 0x00U, 0x00U, 0x01U
};

/* Unicast MAC address */
static uint8 UcastOpenFilterAddr[ETH_HW_ADDR_LEN] =
{
    0x08U, 0x00U, 0x28U, 0x01U, 0xf6U, 0x7cU
};

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

void EthApp_Startup(void);

Std_ReturnType EthApp_initDma(void);
Std_ReturnType EthApp_closeDma(void);

Std_ReturnType EthApp_initDriver(void);
Std_ReturnType EthApp_initAll(void);
Std_ReturnType EthApp_deinitAll(void);
Std_ReturnType EthApp_showStatsAll(void);
boolean EthApp_waitForHostAll(void);

Std_ReturnType EthApp_init(uint8 ctrlIdx);
Std_ReturnType EthApp_trcvInit(const EthTrcv_ConfigType *cfg);
Std_ReturnType EthApp_deinit(uint8 ctrlIdx);

BufReq_ReturnType EthApp_provideTxBuffer(uint8 ctrlIdx,
    uint8 priority,
    Eth_BufIdxType* bufIdxPtr,
    uint8** bufPtr,
    uint16* lenBytePtr);

boolean EthApp_transmit(uint8 ctrlIdx,
    uint32 num,
    uint16 len,
    boolean txConfirmation,
    boolean vlan,
    uint8 priority,
    uint8 *dstAddr);

void EthApp_send(uint8 ctrlIdx,
    EthFrame *frame,
    uint16 len,
    uint8 priority);

void EthApp_sendCmd(uint8 ctrlIdx, uint8 cmd);

boolean EthApp_receive(uint8 ctrlIdx, uint32 num);
boolean EthApp_waitForHost(uint8 ctrlIdx);

void EthApp_transmitFlush(uint8 ctrlIdx);
void EthApp_receiveAllFifo(uint8 ctrlIdx);
void EthApp_flushRecvQueue(uint8 ctrlIdx);
void EthApp_notifyCmdRecv(uint8 ctrlIdx);
void EthApp_delay(uint32 msecs);

#if (STD_ON == ETHTRCV_GETLINKSTATE_API)
Std_ReturnType EthApp_getTranceiverLinkState(uint8 trcvIdx);
#endif /* (STD_ON == ETHTRCV_GETLINKSTATE_API) */

Std_ReturnType EthApp_showStats(uint8 ctrlIdx);

#if (STD_ON == ETH_GLOBALTIMESUPPORT_API)
void EthApp_GetCurrentTime(uint8 ctrlIdx,
    Eth_TimeStampQualType *timeQualPtr, Eth_TimeStampType *tsPtr);

void EthApp_EnableEgressTimeStamp(uint8 ctrlIdx, Eth_BufIdxType bufIdx);
#endif

Std_ReturnType EthApp_SetControllerMode(uint8 ctrlIdx, Eth_ModeType ctrlMode);

Std_ReturnType EthApp_GetControllerMode(uint8 ctrlIdx, Eth_ModeType *ctrlMode);

Std_ReturnType EthApp_Transmit(uint8 ctrlIdx,
             Eth_BufIdxType bufIdx,
             Eth_FrameType frameType,
             boolean txConfirmation,
             uint16 lenByte,
             P2CONST(uint8, AUTOMATIC, ETH_APPL_DAT) physAddrPtr);

void EthApp_TxConfirmationInt(uint8 ctrlIdx);

/* ========================================================================== */
/*                          User callback function Declarations               */
/* ========================================================================== */
void EthApp_CtrlModeIndication(uint8 CtrlIdx, Eth_ModeType ControllerMode);
void EthApp_TrcvModeIndication(uint8 CtrlIdx, EthTrcv_ModeType TrcvMode);

void EthApp_RxIndication(uint8 ctrlIdx,
    Eth_FrameType FrameType,
    boolean IsBroadcast,
    uint8 *PhysAddrPtr,
    Eth_DataType *DataPtr,
    uint16 lenByte);

void EthApp_TxConfirmation(uint8 ctrlIdx,
    Eth_BufIdxType bufIdx,
    Std_ReturnType result);

Std_ReturnType Det_ReportError(uint16 moduleId,
    uint8 instanceId,
    uint8 apiId,
    uint8 errorId);

FUNC(Std_ReturnType, DEM_CODE) Dem_SetEventStatus(
    VAR(Dem_EventIdType, AUTOMATIC) EventId,
    VAR(Dem_EventStatusType, AUTOMATIC) EventStatus);


#ifdef __cplusplus
}
#endif

#endif /* ETHUTILS_TARGET_H_ */
