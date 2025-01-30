/*
 *   Copyright (c) Texas Instruments Incorporated 2024
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
 *  \ingroup CSL_IP_MODULE
 *  \defgroup CSL_MCAN MCAN
 *
 *  @{
 */
/**
 *  \file     mcan.h
 *
 *  \brief    This file contains the prototypes of the APIs present in the
 *            device abstraction layer file of MCAN.
 *            This also contains some related macros.
 */

#ifndef MCAN_H_
#define MCAN_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <hw_include/lldr_mcan.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 * \brief  Macro defines mask for all the interrupts status for MCAN.
 */
#define MCAN_INTR_MASK_ALL   (MCAN_IR_RF0N_MASK | \
                              MCAN_IR_RF0W_MASK | \
                              MCAN_IR_RF0F_MASK | \
                              MCAN_IR_RF0L_MASK | \
                              MCAN_IR_RF1N_MASK | \
                              MCAN_IR_RF1W_MASK | \
                              MCAN_IR_RF1F_MASK | \
                              MCAN_IR_RF1L_MASK | \
                              MCAN_IR_HPM_MASK |  \
                              MCAN_IR_TC_MASK |   \
                              MCAN_IR_TCF_MASK |  \
                              MCAN_IR_TFE_MASK |  \
                              MCAN_IR_TEFN_MASK | \
                              MCAN_IR_TEFW_MASK | \
                              MCAN_IR_TEFF_MASK | \
                              MCAN_IR_TEFL_MASK | \
                              MCAN_IR_TSW_MASK |  \
                              MCAN_IR_MRAF_MASK | \
                              MCAN_IR_TOO_MASK |  \
                              MCAN_IR_DRX_MASK |  \
                              MCAN_IR_BEC_MASK |  \
                              MCAN_IR_BEU_MASK |  \
                              MCAN_IR_ELO_MASK |  \
                              MCAN_IR_EP_MASK |   \
                              MCAN_IR_EW_MASK |   \
                              MCAN_IR_BO_MASK |   \
                              MCAN_IR_WDI_MASK |  \
                              MCAN_IR_PEA_MASK |  \
                              MCAN_IR_PED_MASK |  \
                              MCAN_IR_ARA_MASK)

/**
 * \brief  Maximum payload supported by CAn-FD protocol in bytes.
 */
#define MCAN_MAX_PAYLOAD_BYTES                          (64U)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */
/**
 *  \anchor MCAN_IntrLineNum
 *  \name MCAN Interrupt Line no
 *  @{
 */
/**
 *  \brief    Enum to select the MCAN interrupt lines
 */
typedef uint32 MCAN_IntrLineNum;

#define MCAN_INTR_LINE_NUM_0                    (0U)
/**< MCAN interrupt line 0 */
#define MCAN_INTR_LINE_NUM_1                    (1U)
/**< MCAN interrupt line 1 */
/* @} */

/**
 *  \anchor MCAN_IdType
 *  \name MCAN Id Type
 *  @{
 */
/**
 *  \brief    Enum to represent the MCAN Identifier Type
 */
typedef uint32 MCAN_IdType;

#define MCAN_ID_TYPE_11_BIT                     (0U)
/**< 11bit MCAN Identifier */
#define MCAN_ID_TYPE_29_BIT                     (1U)
/**< 29bit MCAN Identifier */
/* @} */

/**
 *  \anchor MCAN_OperationMode
 *  \name MCAN Operation Mode
 *  @{
 */
/**
 *  \brief    Enum to represent the MCAN mode of operation
 */
typedef uint32 MCAN_OperationMode;

#define MCAN_OPERATION_MODE_NORMAL              (0U)
/**< MCAN normal mode */
#define MCAN_OPERATION_MODE_SW_INIT             (1U)
/**< MCAN SW initialization mode */
/* @} */

/**
 *  \anchor MCAN_MemType
 *  \name MCAN Mem type
 *  @{
 */
/**
 *  \brief    Enum to represent the MCAN Message RAM type.
 */
typedef uint32 MCAN_MemType;

#define MCAN_MEM_TYPE_BUF                       (0U)
/**< MCAN Msg RAM buffers */
#define MCAN_MEM_TYPE_FIFO                      (1U)
/**< MCAN Msg RAM FIFO/Queue */
/* @} */

/**
 *  \anchor MCAN_RxFIFONum
 *  \name MCAN Rx FIFO Number
 *  @{
 */
/**
 *  \brief    Enum to represent the MCAN Rx FIFO number
 */
typedef uint32 MCAN_RxFIFONum;

#define MCAN_RX_FIFO_NUM_0                      (0U)
/**< MCAN Rx FIFO 0 */
#define MCAN_RX_FIFO_NUM_1                      (1U)
/**< MCAN Rx FIFO 1 */
/* @} */

/**
 *  \anchor MCAN_PinType
 *  \name MCAN Pin Type
 *  @{
 */
/**
 *  \brief    Enum to represent the MCAN pin type
 */
typedef uint32 MCAN_PinType;

#define MCAN_PIN_TYPE_RX                        (0U)
/**< MCAN Rx Pin */
#define MCAN_PIN_TYPE_TX                        (1U)
/**< MCAN Tx Pin */
/* @} */

/**
 *  \anchor MCAN_ElemSize
 *  \name MCAN Element Size
 *  @{
 */
/**
 *  \brief    Enum to represent FIFO/Buffer element Size
 */
typedef uint32 MCAN_ElemSize;

#define MCAN_ELEM_SIZE_8BYTES                   (0U)
/**< 8 byte data field */
#define MCAN_ELEM_SIZE_12BYTES                  (1U)
/**< 12 byte data field */
#define MCAN_ELEM_SIZE_16BYTES                  (2U)
/**< 16 byte data field */
#define MCAN_ELEM_SIZE_20BYTES                  (3U)
/**< 20 byte data field */
#define MCAN_ELEM_SIZE_24BYTES                  (4U)
/**< 24 byte data field */
#define MCAN_ELEM_SIZE_32BYTES                  (5U)
/**< 32 byte data field */
#define MCAN_ELEM_SIZE_48BYTES                  (6U)
/**< 48 byte data field */
#define MCAN_ELEM_SIZE_64BYTES                  (7U)
/**< 64 byte data field */
/* @} */

/**
 *  \anchor MCAN_TimeOutSelect
 *  \name MCAN Timeout select
 *  @{
 */
/**
 *  \brief    Enum to represent the MCAN time-out counter configuration
 */
typedef uint32 MCAN_TimeOutSelect;

#define MCAN_TIMEOUT_SELECT_CONT                (0U)
/**< Continuous operation Mode */
#define MCAN_TIMEOUT_SELECT_TX_EVENT_FIFO       (1U)
/**< Timeout controlled by Tx Event FIFO */
#define MCAN_TIMEOUT_SELECT_RX_FIFO0            (2U)
/**< Timeout controlled by Rx FIFO 0 */
#define MCAN_TIMEOUT_SELECT_RX_FIFO1            (3U)
/**< Timeout controlled by Rx FIFO 1 */
/* @} */

/**
 *  \anchor MCAN_IntrSrc
 *  \name MCAN Interrupt Source
 *  @{
 */
/**
 * \brief  Enum for MCAN interrupts.
 */
typedef uint32 MCAN_IntrSrc;

#define MCAN_INTR_SRC_RX_FIFO0_NEW_MSG          (MCAN_IR_RF0N_MASK)
/**< Rx FIFO 0 New Message interrupt */
#define MCAN_INTR_SRC_RX_FIFO0_WATERMARK        (MCAN_IR_RF0W_MASK)
/**< Rx FIFO 0 Watermark Reached interrupt */
#define MCAN_INTR_SRC_RX_FIFO0_FULL             (MCAN_IR_RF0F_MASK)
/**< Rx FIFO 0 Full interrupt */
#define MCAN_INTR_SRC_RX_FIFO0_MSG_LOST         (MCAN_IR_RF0L_MASK)
/**< Rx FIFO 0 Message Lost interrupt */
#define MCAN_INTR_SRC_RX_FIFO1_NEW_MSG          (MCAN_IR_RF1N_MASK)
/**< Rx FIFO 1 New Message interrupt */
#define MCAN_INTR_SRC_RX_FIFO1_WATERMARK        (MCAN_IR_RF1W_MASK)
/**< Rx FIFO 1 Watermark Reached interrupt */
#define MCAN_INTR_SRC_RX_FIFO1_FULL             (MCAN_IR_RF1F_MASK)
/**< Rx FIFO 1 Full interrupt */
#define MCAN_INTR_SRC_RX_FIFO1_MSG_LOST         (MCAN_IR_RF1L_MASK)
/**< Rx FIFO 1 Message Lost interrupt */
#define MCAN_INTR_SRC_HIGH_PRIO_MSG             (MCAN_IR_HPM_MASK)
/**< High Priority Message interrupt */
#define MCAN_INTR_SRC_TRANS_COMPLETE            (MCAN_IR_TC_MASK)
/**< Transmission Completed interrupt */
#define MCAN_INTR_SRC_TRANS_CANCEL_FINISH       (MCAN_IR_TCF_MASK)
/**< Transmission Cancellation Finished interrupt */
#define MCAN_INTR_SRC_TX_FIFO_EMPTY             (MCAN_IR_TFE_MASK)
/**< Tx FIFO Empty interrupt */
#define MCAN_INTR_SRC_TX_EVT_FIFO_NEW_ENTRY     (MCAN_IR_TEFN_MASK)
/**< Tx Event FIFO New Entry interrupt */
#define MCAN_INTR_SRC_TX_EVT_FIFO_WATERMARK     (MCAN_IR_TEFW_MASK)
/**< Tx Event FIFO Watermark Reached interrupt */
#define MCAN_INTR_SRC_TX_EVT_FIFO_FULL          (MCAN_IR_TEFF_MASK)
/**< Tx Event FIFO Full interrupt */
#define MCAN_INTR_SRC_TX_EVT_FIFO_ELEM_LOST     (MCAN_IR_TEFL_MASK)
/**< Tx Event FIFO Element Lost interrupt */
#define MCAN_INTR_SRC_TIMESTAMP_WRAPAROUND      (MCAN_IR_TSW_MASK)
/**< Timestamp Wraparound interrupt */
#define MCAN_INTR_SRC_MSG_RAM_ACCESS_FAILURE    (MCAN_IR_MRAF_MASK)
/**< Message RAM Access Failure interrupt */
#define MCAN_INTR_SRC_TIMEOUT                   (MCAN_IR_TOO_MASK)
/**< Timeout Occurred interrupt */
#define MCAN_INTR_SRC_DEDICATED_RX_BUFF_MSG     (MCAN_IR_DRX_MASK)
/**< Message stored to Dedicated Rx Buffer interrupt */
#define MCAN_INTR_SRC_BIT_ERR_CORRECTED         (MCAN_IR_BEC_MASK)
/**< Bit Error Corrected interrupt */
#define MCAN_INTR_SRC_BIT_ERR_UNCORRECTED       (MCAN_IR_BEU_MASK)
/**< Bit Error Uncorrected interrupt */
#define MCAN_INTR_SRC_ERR_LOG_OVRFLW            (MCAN_IR_ELO_MASK)
/**< Error Logging Overflow interrupt */
#define MCAN_INTR_SRC_ERR_PASSIVE               (MCAN_IR_EP_MASK)
/**< Error Passive interrupt */
#define MCAN_INTR_SRC_WARNING_STATUS            (MCAN_IR_EW_MASK)
/**< Warning Status interrupt */
#define MCAN_INTR_SRC_BUS_OFF_STATUS            (MCAN_IR_BO_MASK)
/**< Bus_Off Status interrupt */
#define MCAN_INTR_SRC_WATCHDOG                  (MCAN_IR_WDI_MASK)
/**< Watchdog Interrupt interrupt */
#define MCAN_INTR_SRC_PROTOCOL_ERR_ARB          (MCAN_IR_PEA_MASK)
/**< Protocol Error in Arbitration Phase interrupt */
#define MCAN_INTR_SRC_PROTOCOL_ERR_DATA         (MCAN_IR_PED_MASK)
/**< Protocol Error in Data Phase interrupt */
#define MCAN_INTR_SRC_RES_ADDR_ACCESS           (MCAN_IR_ARA_MASK)
/**< Access to Reserved Address interrupt */
/* @} */

/**
 *  \anchor MCAN_ECCErrType
 *  \name MCAN Ecc Error Type
 *  @{
 */
/**
 *  \brief    Enum to represent the ECC Error Types
 */
typedef uint32 MCAN_ECCErrType;

#define MCAN_ECC_ERR_TYPE_SEC                   (0U)
/**< ECC Single Error Correction */
#define MCAN_ECC_ERR_TYPE_DED                   (1U)
/**< ECC Single Error Detection */
/* @} */

/**
 *  \anchor MCAN_LpbkMode
 *  \name MCAN Loopback Mode
 *  @{
 */
/**
 *  \brief    Enum to select the MCAN Loopback mode
 */
typedef uint32 MCAN_LpbkMode;

#define MCAN_LPBK_MODE_INTERNAL                 (0U)
/**< Internal Loop Back Mode
 *   This mode can be used for hot self-test and this mode will not
 *   affect bus state.
 */
#define MCAN_LPBK_MODE_EXTERNAL                 (1U)
/**< External Loop Back Mode
 *   In this mode, MCAN the M_CAN treats its own transmitted messages as
 *   received messages and stores them (if they pass acceptance filtering)
 *   into an Rx Buffer or an Rx FIFO. This mode will affect bus state
 */
/* @} */

/**
 *  \anchor MCAN_ComState
 *  \name MCAN Com State
 *  @{
 */
/**
 *  \brief    Enum to represent MCAN's communication state
 */
typedef uint32 MCAN_ComState;

#define MCAN_COM_STATE_SYNCHRONIZING            (0U)
/**< MCAN is synchronizing on CAN communication */
#define MCAN_COM_STATE_IDLE                     (1U)
/**< MCAN is neither receiver nor transmitter */
#define MCAN_COM_STATE_RECEIVER                 (2U)
/**< MCAN is operating as receiver */
#define MCAN_COM_STATE_TRANSMITTER              (3U)
/**< MCAN is operating as transmitter */
/* @} */

/**
 *  \anchor MCAN_ErrCode
 *  \name MCAN Error Code
 *  @{
 */
/**
 *  \brief    Enum to represent MCAN's Error Code
 */
typedef uint32 MCAN_ErrCode;

#define MCAN_ERR_CODE_NO_ERROR                  (0U)
/**< No error occurred since LEC has been reset by
 *   successful reception or transmission.
 */
#define MCAN_ERR_CODE_STUFF_ERROR               (1U)
/**< More than 5 equal bits in a sequence have occurred in a part of
 *   a received message where this is not allowed.
 */
#define MCAN_ERR_CODE_FORM_ERROR                (2U)
/**< A fixed format part of a received frame has the wrong format. */
#define MCAN_ERR_CODE_ACK_ERROR                 (3U)
/**< The message transmitted by the M_CAN was not acknowledged
 *   by another node.
 */
#define MCAN_ERR_CODE_BIT1_ERROR                (4U)
/**< During the transmission of a message (with the exception of
 *   the arbitration field), the device wanted to send a
 *   recessive level (bit of logical value ‘1’))
 *  but the monitored bus value was dominant.
 */
#define MCAN_ERR_CODE_BIT0_ERROR                (5U)
/**< During the transmission of a message (or acknowledge bit,
 *   or active error flag, or overload flag), the device wanted to send
 *   a dominant level (data or identifier bit logical value ‘0’),
 *   but the monitored bus value was recessive. During Bus_Off recovery
 *   this status is set each time a sequence of 11 recessive bits has been
 *   monitored. This enables the CPU to monitor the proceeding of
 *   the Bus_Off recovery sequence (indicating the bus is not stuck at
 *   dominant or continuously disturbed).
 */
#define MCAN_ERR_CODE_CRC_ERROR                 (6U)
/**< The CRC check sum of a received message was incorrect.
 *   The CRC of an incoming message does not match with the
 *   CRC calculated from the received data.
 */
#define MCAN_ERR_CODE_NO_CHANGE                 (7U)
/**< Any read access to the Protocol Status
 *   Register re-initializes
 *   the LEC to ‘7’. When the LEC shows the value ‘7’,
 *   no CAN bus event was detected since the last CPU read access to
 *   the Protocol Status Register.
 */
/* @} */

/**
 * \brief  Structure for bit timing calculation.
 *         Bit timing related to data phase will be valid only in case where
 *         MCAN is put in CAN-FD mode and will be '0' otherwise.
 */
typedef struct
{
    uint32 nomRatePrescalar;
    /**< Nominal Baud Rate Pre-scaler
     *   Range:[0x0-0x1FF]
     */
    uint32 nomTimeSeg1;
    /**< Nominal Time segment before sample point
     *   Range:[0x0-0xFF]
     */
    uint32 nomTimeSeg2;
    /**< Nominal Time segment after sample point
     *   Range:[0x0-0x7F]
     */
    uint32 nomSynchJumpWidth;
    /**< Nominal (Re)Synchronization Jump Width
     *   Range:[0x0-0x7F]
     */
    uint32 dataRatePrescalar;
    /**< Data Baud Rate Pre-scaler
     *   Range:[0x0-0x1F]
     */
    uint32 dataTimeSeg1;
    /**< Data Time segment before sample point
     *   Range:[0x0-0x1F]
     */
    uint32 dataTimeSeg2;
    /**< Data Time segment after sample point
     *   Range:[0x0-0xF]
     */
    uint32 dataSynchJumpWidth;
    /**< Data (Re)Synchronization Jump Width
     *   Range:[0x0-0xF]
     */
}MCAN_BitTimingParams;

/**
 * \brief  Structure for MCAN Transmitter Delay Compensation parameters.
 */
typedef struct
{
    uint32 tdcf;
    /**< Transmitter Delay Compensation Filter Window Length
     *   Range:[0x0-0x7F]
     */
    uint32 tdco;
    /**< Transmitter Delay Compensation Offset
     *   Range:[0x0-0x7F]
     */
}MCAN_TDCConfig;

/**
 * \brief  Structure for MCAN Global Filter Configuration parameters.
 */
typedef struct
{
    uint32 rrfe;
    /**< Reject Remote Frames Extended
     *   0 = Filter remote frames with 29-bit extended IDs
     *   1 = Reject all remote frames with 29-bit extended IDs
     */
    uint32 rrfs;
    /**< Reject Remote Frames Standard
     *   0 = Filter remote frames with 11-bit standard IDs
     *   1 = Reject all remote frames with 11-bit standard IDs
     */
    uint32 anfe;
    /**< Accept Non-matching Frames Extended
     *   0 = Accept in Rx FIFO 0
     *   1 = Accept in Rx FIFO 1
     *   others = Reject
     */
    uint32 anfs;
    /**< Accept Non-matching Frames Standard
     *   0 = Accept in Rx FIFO 0
     *   1 = Accept in Rx FIFO 1
     *   others = Reject
     */
}MCAN_GlobalFiltConfig;

/**
 * \brief  Structure for MCAN initialization parameters.
 */
typedef struct
{
    uint32       fdMode;
    /**< FD Operation Enable
     *   0 = FD operation disabled
     *   1 = FD operation enabled
     */
    uint32       brsEnable;
    /**< Bit Rate Switch Enable
     *   This is valid only when opMode = 1.
     *   0 = Bit rate switching for transmissions disabled
     *   1 = Bit rate switching for transmissions enabled
     */
    uint32       txpEnable;
    /**< Transmit Pause
     *   0 = Transmit pause disabled
     *   1 = Transmit pause enabled
     */
    uint32       efbi;
    /**< FEdge Filtering during Bus Integration
     *   0 = Edge filtering disabled
     *   1 = Two consecutive dominant tq required to detect an edge for
     *       hard synchronization
     */
    uint32       pxhddisable;
    /**< Protocol Exception Handling Disable
     *   0 = Protocol exception handling enabled
     *   1 = Protocol exception handling disabled
     */
    uint32       darEnable;
    /**< Disable Automatic Retransmission
     *   0 = Automatic retransmission of messages not transmitted successfully
     *       enabled
     *   1 = Automatic retransmission disabled
     */
    uint32       wkupReqEnable;
    /**< Wakeup Request Enable
     *   0 = Wakeup request is disabled
     *   1 = Wakeup request is enabled
     */
    uint32       autoWkupEnable;
    /**< Auto-Wakeup Enable
     *   0 = Auto-Wakeup is disabled
     *   1 = Auto-Wakeup is enabled
     */
    uint32       emulationEnable;
    /**< Emulation/Debug Suspend Enable
     *   0 = Emulation/Debug Suspend is disabled
     *   1 = Emulation/Debug Suspend is enabled
     */
    uint32       emulationFAck;
    /**< Emulation/Debug Suspend Fast Ack Enable
     *   0 = Emulation/Debug Suspend does not wait for idle/immediate effect
     *   1 = Emulation/Debug Suspend waits for idle/graceful stop
     */
    uint32       clkStopFAck;
    /**< Clock Stop Fast Ack Enable
     *   0 = Clock Stop does not wait for idle/immediate effect
     *   1 = Clock Stop waits for idle/graceful stop
     */
    uint32       wdcPreload;
    /**< Start value of the Message RAM Watchdog Counter
     *   Range:[0x0-0xFF]
     */
    MCAN_TDCConfig tdcConfig;
    /**< Transmitter Delay Compensation parameters.
     *   Refer struct MCAN_TDCConfig.
     */
    uint32       tdcEnable;
    /**< Transmitter Delay Compensation Enable
     *   0 = Transmitter Delay Compensation is disabled
     *   1 = Transmitter Delay Compensation is enabled
     */
}MCAN_InitParams;

/**
 * \brief  Structure for MCAN configuration parameters.
 */
typedef struct
{
    uint32              monEnable;
    /**< Bus Monitoring Mode
     *   0 = Bus Monitoring Mode is disabled
     *   1 = Bus Monitoring Mode is enabled
     */
    uint32              asmEnable;
    /**< Restricted Operation Mode
     *   0 = Normal CAN operation
     *   1 = Restricted Operation Mode active
     *   This mode should not be combined with test modes.
     */
    uint32              tsPrescalar;
    /**< Timestamp Counter Prescaler.
     *   Range:[0x0-0xF]
     */
    uint32              tsSelect;
    /**< Timestamp source selection.
     *   00= Timestamp counter value always 0x0000
     *   01= Timestamp counter value incremented according to tsPrescalar
     *   10= External timestamp counter value used
     *   11= Same as “00”
     */
    uint32              timeoutSelect;
    /**< Time-out counter source select.
     *   Refer enum MCAN_TimeOutSelect.
     */
    uint32              timeoutPreload;
    /**< Start value of the Timeout Counter (down-counter).
     *   The Timeout Counter is decremented in multiples of CAN bit times [1-16]
     *   depending on the configuration of the tsPrescalar.
     *   Range:[0x0-0xFFFF]
     */
    uint32              timeoutCntEnable;
    /**< Time-out Counter Enable
     *   0 = Time-out Counter is disabled
     *   1 = Time-out Counter is enabled
     */
    MCAN_GlobalFiltConfig filterConfig;
    /**< Global Filter Configuration parameters.
     *    Refer struct MCAN_GlobalFiltConfig.
     */
}MCAN_ConfigParams;

/**
 * \brief  Structure for MCAN error logging counters status.
 */
typedef struct
{
    uint32 transErrLogCnt;
    /**< Transmit Error Counter */
    uint32 recErrCnt;
    /**< Receive Error Counter */
    uint32 rpStatus;
    /**< Receive Error Passive
     *   0 = The Receive Error Counter is below the error passive level(128)
     *   1 = The Receive Error Counter has reached the error passive level(128)
     */
    uint32 canErrLogCnt;
    /**< CAN Error Logging */
}MCAN_ErrCntStatus;

/**
 * \brief  Structure for MCAN protocol status.
 */
typedef struct
{
    uint32 lastErrCode;
    /**< Last Error Code
     *   Refer enum MCAN_ErrCode
     */
    uint32 act;
    /**< Activity - Monitors the module’s CAN communication state.
     *   refer enum MCAN_ComState
     */
    uint32 errPassive;
    /**< Error Passive
     *   0 = The M_CAN is in the Error_Active state
     *   1 = The M_CAN is in the Error_Passive state
     */
    uint32 warningStatus;
    /**< Warning Status
     *   0 = Both error counters are below the Error_Warning limit of 96
     *   1 = At least one of error counter has reached the Error_Warning
     *       limit of 96
     */
    uint32 busOffStatus;
    /**< Bus_Off Status
     *   0 = The M_CAN is not Bus_Off
     *   1 = The M_CAN is in Bus_Off state
     */
    uint32 dlec;
    /**< Data Phase Last Error Code
     *   Refer enum MCAN_ErrCode
     */
    uint32 resi;
    /**< ESI flag of last received CAN FD Message
     *   0 = Last received CAN FD message did not have its ESI flag set
     *   1 = Last received CAN FD message had its ESI flag set
     */
    uint32 rbrs;
    /**< BRS flag of last received CAN FD Message
     *   0 = Last received CAN FD message did not have its BRS flag set
     *   1 = TLast received CAN FD message had its BRS flag set
     */
    uint32 rfdf;
    /**< Received a CAN FD Message
     *   0 = Since this bit was reset by the CPU, no CAN FD message has been
     *       received
     *   1 = Message in CAN FD format with FDF flag set has been received
     */
    uint32 pxe;
    /**< Protocol Exception Event
     *   0 = No protocol exception event occurred since last read access
     *   1 = Protocol exception event occurred
     */
    uint32 tdcv;
    /**< Transmitter Delay Compensation Value */
}MCAN_ProtocolStatus;

/**
 * \brief  Structure for MCAN Message RAM Configuration Parameters.
 *         Message RAM can contain following sections:
 *         Standard ID filters, Extended ID filters, TX FIFO(or TX Q),
 *         TX Buffers, TX EventFIFO, RX FIFO0, RX FIFO1, RX Buffer.
 *         Note: If particular section in the RAM is not used then it's size
 *         should be initialized to '0'
 *         (Number of buffers in case of Tx/Rx buffer).
 */
typedef struct
{
    uint32 flssa;
    /**< Standard ID Filter List Start Address */
    uint32 lss;
    /**< List Size: Standard ID
     *   0 = No standard Message ID filter
     *   1-127 = Number of standard Message ID filter elements
     *   others = Values greater than 128 are interpreted as 128
     */
    uint32 flesa;
    /**< Extended ID Filter List Start Address */
    uint32 lse;
    /**< List Size: Extended ID
     *   0 = No standard Message ID filter
     *   1-64 = Number of standard Message ID filter elements
     *   others = Values greater than 64 are interpreted as 64
     */
    uint32 txStartAddr;
    /**< Tx Buffers Start Address */
    uint32 txBufNum;
    /**< Number of Dedicated Transmit Buffers
     *   0 = No Dedicated Tx Buffers
     *   1-32 = Number of Dedicated Tx Buffers
     *   others = Values greater than 32 are interpreted as 32
     */
    uint32 txFIFOSize;
    /**< Transmit FIFO/Queue Size
     *   0 = No Tx FIFO/Queue
     *   1-32 = Number of Tx Buffers used for Tx FIFO/Queue
     *   others = Values greater than 32 are interpreted as 32
     */
    uint32 txBufMode;
    /**< Tx FIFO/Queue Mode
     *   0 = Tx FIFO operation
     *   1 = Tx Queue operation
     */
    uint32 txBufElemSize;
    /**< Tx Buffer Element Size */
    uint32 txEventFIFOStartAddr;
    /**< Tx Event FIFO Start Address */
    uint32 txEventFIFOSize;
    /**< Event FIFO Size
     *   0 = Tx Event FIFO disabled
     *   1-32 = Number of Tx Event FIFO elements
     *   others = Values greater than 32 are interpreted as 32
     */
    uint32 txEventFIFOWaterMark;
    /**< Tx Event FIFO Watermark
     *   0 = Watermark interrupt disabled
     *   1-32 = Level for Tx Event FIFO watermark interrupt
     *   others = Watermark interrupt disabled
     */
    uint32 rxFIFO0startAddr;
    /**< Rx FIFO0 Start Address */
    uint32 rxFIFO0size;
    /**< Rx FIFO0 Size
     *   0 = No Rx FIFO
     *   1-64 = Number of Rx FIFO elements
     *   others = Values greater than 64 are interpreted as 64
     */
    uint32 rxFIFO0waterMark;
    /**< Rx FIFO0 Watermark
     *   0 = Watermark interrupt disabled
     *   1-63 = Level for Rx FIFO 0 watermark interrupt
     *   others = Watermark interrupt disabled
     */
    uint32 rxFIFO0OpMode;
    /**< Rx FIFO0 Operation Mode
     *   0 = FIFO blocking mode
     *   1 = FIFO overwrite mode
     */
    uint32 rxFIFO1startAddr;
    /**< Rx FIFO1 Start Address */
    uint32 rxFIFO1size;
    /**< Rx FIFO1 Size
     *   0 = No Rx FIFO
     *   1-64 = Number of Rx FIFO elements
     *   others = Values greater than 64 are interpreted as 64
     */
    uint32 rxFIFO1waterMark;
    /**< Rx FIFO1 Watermark
     *   0 = Watermark interrupt disabled
     *   1-63 = Level for Rx FIFO 1 watermark interrupt
     *   others = Watermark interrupt disabled
     */
    uint32 rxFIFO1OpMode;
    /**< Rx FIFO1 Operation Mode
     *   0 = FIFO blocking mode
     *   1 = FIFO overwrite mode
     */
    uint32 rxBufStartAddr;
    /**< Rx Buffer Start Address */
    uint32 rxBufElemSize;
    /**< Rx Buffer Element Size */
    uint32 rxFIFO0ElemSize;
    /**< Rx FIFO0 Element Size */
    uint32 rxFIFO1ElemSize;
    /**< Rx FIFO1 Element Size */
}MCAN_MsgRAMConfigParams;

/**
 * \brief  Structure for MCAN High Priority Message.
 */
typedef struct
{
    uint32 bufIdx;
    /**< Buffer Index
     *   Only valid when MSI[1] = ‘1’.
     */
    uint32 msi;
    /**< Message Storage Indicator
     *   MSI[1:0]
     *       00  = No FIFO selected
     *       01  = FIFO message lost
     *       10  = Message stored in FIFO 0
     *       11  = Message stored in FIFO 1
     */
    uint32 filterIdx;
    /**< Filter Index */
    uint32 filterList;
    /**< Indicates the filter list of the matching filter element
     *   0 = Standard Filter List
     *   1 = Extended Filter List
     */
}MCAN_HighPriorityMsgInfo;

/**
 * \brief  Structure for MCAN new data flag for Rx buffer.
 */
typedef struct
{
    uint32 statusLow;
    /**< New data flag for Rx buffer no. 0 to 31 */
    uint32 statusHigh;
    /**< New data flag for Rx buffer no. 32 to 63 */
}MCAN_RxNewDataStatus;

/**
 * \brief  Structure for MCAN Rx FIFO Status.
 */
typedef struct
{
    uint32 num;
    /**< Rx FIFO number
     *   Refer enum MCAN_RxFIFONum
     */
    uint32 fillLvl;
    /**< Rx FIFO Fill Level */
    uint32 getIdx;
    /**< Rx FIFO Get Index */
    uint32 putIdx;
    /**< Rx FIFO Put Index */
    uint32 fifoFull;
    /**< Rx FIFO Full
     *   0 = Rx FIFO not full
     *   1 = Rx FIFO full
     */
    uint32 msgLost;
    /**< Rx FIFO Message Lost */
}MCAN_RxFIFOStatus;

/**
 * \brief  Structure for MCAN Tx FIFO Status.
 */
typedef struct
{
    uint32 freeLvl;
    /**< Tx FIFO Free Level */
    uint32 getIdx;
    /**< Tx FIFO Get Index
     *   Read as zero when Tx Queue operation is configured.
     */
    uint32 putIdx;
    /**< Tx FIFO/Queue Put Index */
    uint32 fifoFull;
    /**< Tx FIFO/Queue Full
     *   0 = Tx FIFO/Queue not full
     *   1 = Tx FIFO/Queue full
     */
}MCAN_TxFIFOStatus;

/**
 * \brief  Structure for MCAN Tx Event FIFO Status.
 */
typedef struct
{
    uint32 fillLvl;
    /**< Event FIFO Fill Level */
    uint32 getIdx;
    /**< Event FIFO Gut Index */
    uint32 putIdx;
    /**< Event FIFO Put Index */
    uint32 fifoFull;
    /**< Event FIFO Full
     *   0 = Tx Event FIFO not full
     *   1 = Tx Event FIFO full
     */
    uint32 eleLost;
    /**< Tx Event FIFO Element Lost
     *   0 = No Tx Event FIFO element lost
     *   1 = Tx Event FIFO element lost, also set after write attempt to
     *       Tx Event FIFO of size zero.
     */
}MCAN_TxEventFIFOStatus;

/**
 * \brief  Structure for ECC Error forcing.
 */
typedef struct
{
    uint32 errType;
    /**< Error type to be forced
     *   Refer enum  MCAN_ECCErrType.
     */
    uint32 rowNum;
    /**< Row address where error needs to be applied. */
    uint32 bit1;
    /**< Column/Data bit that needs to be flipped when
     *   force_sec or force_ded is set
     */
    uint32 bit2;
    /**< Data bit that needs to be flipped when force_ded is set */
    uint32 errOnce;
    /**< Force Error once
     *   1: The error will inject an error to the specified row only once
     */
    uint32 errForce;
    /**< Force error on the next RAM read */
}MCAN_ECCErrForceParams;

/**
 * \brief  Structure for ECC Error Status.
 */
typedef struct
{
    uint32 secErr;
    /**< Single Bit Error Status
     *   0 = No Single Bit Error pending
     *   1 = Single Bit Error pending
     */
    uint32 dedErr;
    /**< Double Bit Error Status
     *   0 = No Double Bit Error pending
     *   1 = Double Bit Error pending
     */
    uint32 row;
    /**< Indicates the row/address where the single or double bit
     *   error occurred.
     */
    uint32 bit1;
    /**< Indicates the bit position in the ram data that is in error
     */
    uint32 bit2;
    /**< Indicates the bit position in the ram data that is in error
     *   Valid only in case of DED.
     */
}MCAN_ECCErrStatus;

/**
 * \brief  Structure for accessing Revision ID and Core Release Info.
 *         of MCAN module.
 */
typedef struct
{
    uint32 scheme;
    /**< Scheme */
    uint32 bu;
    /**< Business Unit: 10 = Processors */
    uint32 modId;
    /**< Module ID */
    uint32 rtlRev;
    /**< RTL revision */
    uint32 major;
    /**< Major revision */
    uint32 custom;
    /**< Custom revision */
    uint32 minor;
    /**< Minor revision */
    uint32 day;
    /**< Time Stamp Day. Two digits, BCD-coded. */
    uint32 mon;
    /**< Time Stamp Month. Two digits, BCD-coded. */
    uint32 year;
    /**< Time Stamp Year. Single digit, BCD-coded. */
    uint32 subStep;
    /**< Sub-step of Core Release Single digit, BCD-coded. */
    uint32 step;
    /**< Step of Core Release.Two digits, BCD-coded Single digit, BCD-coded. */
    uint32 rel;
    /**< Core Release. Single digit, BCD-coded. */
}MCAN_RevisionId;

/**
 * \brief  Structure for accessing Revision ID of ECC AGGR.
 */
typedef struct
{
    uint32 scheme;
    /**< Scheme */
    uint32 bu;
    /**< Business Unit: 10 = Processors */
    uint32 modId;
    /**< Module ID */
    uint32 rtlRev;
    /**< RTL revision */
    uint32 major;
    /**< Major revision */
    uint32 custom;
    /**< Custom revision */
    uint32 minor;
    /**< Minor revision */
}MCAN_ECCAggrRevisionId;

/**
 * \brief  Structure for MCAN ECC configuration parameters.
 */
typedef struct
{
    uint32 enable;
    /**< Enable/disable ECC
     *   0 = Disable ECC
     *   1 = Enable ECC
     */
    uint32 enableChk;
    /**< Enable/disable ECC Check
     *   0 = Disable ECC Check
     *   1 = Enable ECC Check
     */
    uint32 enableRdModWr;
    /**< Enable/disable Read Modify Write operation
     *   0 = Disable Read Modify Write operation
     *   1 = Enable Read Modify Write operation
     */
}MCAN_ECCConfigParams;

/**
 * \brief  Structure for accessing Revision ID of ECC wrapper.
 */
typedef struct
{
    uint32 scheme;
    /**< Scheme */
    uint32 bu;
    /**< Business Unit: 10 = Processors */
    uint32 modId;
    /**< Module ID */
    uint32 rtlRev;
    /**< RTL revision */
    uint32 major;
    /**< Major revision */
    uint32 custom;
    /**< Custom revision */
    uint32 minor;
    /**< Minor revision */
}MCAN_ECCWrapRevisionId;

/**
 * \brief  Structure for MCAN Tx Buffer element.
 */
typedef struct
{
    uint32 id;
    /**< Identifier */
    uint32 rtr;
    /**< Remote Transmission Request
     *   0 = Transmit data frame
     *   1 = Transmit remote frame
     */
    uint32 xtd;
    /**< Extended Identifier
     *   0 = 11-bit standard identifier
     *   1 = 29-bit extended identifier
     */
    uint32 esi;
    /**< Error State Indicator
     *   0 = ESI bit in CAN FD format depends only on error passive flag
     *   1 = ESI bit in CAN FD format transmitted recessive
     */
    uint32 dlc;
    /**< Data Length Code
     *   0-8  = CAN + CAN FD: transmit frame has 0-8 data bytes
     *   9-15 = CAN: transmit frame has 8 data bytes
     *   9-15 = CAN FD: transmit frame has 12/16/20/24/32/48/64 data bytes
     */
    uint32 brs;
    /**< Bit Rat Switching
     *   0 = CAN FD frames transmitted without bit rate switching
     *   1 = CAN FD frames transmitted with bit rate switching
     */
    uint32 fdf;
    /**< FD Format
     *   0 = Frame transmitted in Classic CAN format
     *   1 = Frame transmitted in CAN FD format
     */
    uint32 efc;
    /**< Event FIFO Control
     *   0 = Don’t store Tx events
     *   1 = Store Tx events
     */
    uint32 mm;
    /**< Message Marker */
    uint8  data[MCAN_MAX_PAYLOAD_BYTES];
    /**< Data bytes.
     *   Only first dlc number of bytes are valid.
     */
}MCAN_TxBufElement;

/**
 * \brief  Structure for MCAN Tx Buffer element which takes data as a pointer.
 *         Using this the MCAL CAN driver can optimize one full copy of CAN
 *         payload by a simple pointer assignment.
 *         Please note that as the data field is a pointer so wrong value for
 *         payload size passed from application will lead to data corruption.
 */
typedef struct
{
    uint32 id;
    /**< Identifier */
    uint32 rtr;
    /**< Remote Transmission Request
     *   0 = Transmit data frame
     *   1 = Transmit remote frame
     */
    uint32 xtd;
    /**< Extended Identifier
     *   0 = 11-bit standard identifier
     *   1 = 29-bit extended identifier
     */
    uint32 esi;
    /**< Error State Indicator
     *   0 = ESI bit in CAN FD format depends only on error passive flag
     *   1 = ESI bit in CAN FD format transmitted recessive
     */
    uint32 dlc;
    /**< Data Length Code
     *   0-8  = CAN + CAN FD: transmit frame has 0-8 data bytes
     *   9-15 = CAN: transmit frame has 8 data bytes
     *   9-15 = CAN FD: transmit frame has 12/16/20/24/32/48/64 data bytes
     */
    uint32 brs;
    /**< Bit Rat Switching
     *   0 = CAN FD frames transmitted without bit rate switching
     *   1 = CAN FD frames transmitted with bit rate switching
     */
    uint32 fdf;
    /**< FD Format
     *   0 = Frame transmitted in Classic CAN format
     *   1 = Frame transmitted in CAN FD format
     */
    uint32 efc;
    /**< Event FIFO Control
     *   0 = Don’t store Tx events
     *   1 = Store Tx events
     */
    uint32 mm;
    /**< Message Marker */
    uint8  *data;
    /**< Data bytes.
     *   Only first dlc number of bytes are valid.
     */
}MCAN_TxBufElementNoCpy;

/**
 * \brief  Structure for MCAN Rx Buffer element.
 */
typedef struct
{
    uint32 id;
    /**< Identifier */
    uint32 rtr;
    /**< Remote Transmission Request
     *   0 = Received frame is a data frame
     *   1 = Received frame is a remote frame
     */
    uint32 xtd;
    /**< Extended Identifier
     *   0 = 11-bit standard identifier
     *   1 = 29-bit extended identifier
     */
    uint32 esi;
    /**< Error State Indicator
     *   0 = Transmitting node is error active
     *   1 = Transmitting node is error passive
     */
    uint32 rxts;
    /**< Rx Timestamp */
    uint32 dlc;
    /**< Data Length Code
     *   0-8  = CAN + CAN FD: received frame has 0-8 data bytes
     *   9-15 = CAN: received frame has 8 data bytes
     *   9-15 = CAN FD: received frame has 12/16/20/24/32/48/64 data bytes
     */
    uint32 brs;
    /**< Bit Rat Switching
     *   0 = Frame received without bit rate switching
     *   1 = Frame received with bit rate switching
     */
    uint32 fdf;
    /**< FD Format
     *   0 = Standard frame format
     *   1 = CAN FD frame format (new DLC-coding and CRC)
     */
    uint32 fidx;
    /**< Filter Index */
    uint32 anmf;
    /**< Accepted Non-matching Frame
     *   0 = Received frame matching filter index FIDX
     *   1 = Received frame did not match any Rx filter element
     */
    uint8  data[MCAN_MAX_PAYLOAD_BYTES];
    /**< Data bytes.
     *   Only first dlc number of bytes are valid.
     */
}MCAN_RxBufElement;

/**
 * \brief  Structure for MCAN Rx Buffer element which takes data as a pointer.
 *         Using this the MCAL CAN driver can optimize one full copy of CAN
 *         payload by a simple pointer assignment.
 *         Please note that as the data field is a pointer so wrong value for
 *         payload size passed from application will lead to data corruption.
 */
typedef struct
{
    uint32 id;
    /**< Identifier */
    uint32 rtr;
    /**< Remote Transmission Request
     *   0 = Received frame is a data frame
     *   1 = Received frame is a remote frame
     */
    uint32 xtd;
    /**< Extended Identifier
     *   0 = 11-bit standard identifier
     *   1 = 29-bit extended identifier
     */
    uint32 esi;
    /**< Error State Indicator
     *   0 = Transmitting node is error active
     *   1 = Transmitting node is error passive
     */
    uint32 rxts;
    /**< Rx Timestamp */
    uint32 dlc;
    /**< Data Length Code
     *   0-8  = CAN + CAN FD: received frame has 0-8 data bytes
     *   9-15 = CAN: received frame has 8 data bytes
     *   9-15 = CAN FD: received frame has 12/16/20/24/32/48/64 data bytes
     */
    uint32 brs;
    /**< Bit Rat Switching
     *   0 = Frame received without bit rate switching
     *   1 = Frame received with bit rate switching
     */
    uint32 fdf;
    /**< FD Format
     *   0 = Standard frame format
     *   1 = CAN FD frame format (new DLC-coding and CRC)
     */
    uint32 fidx;
    /**< Filter Index */
    uint32 anmf;
    /**< Accepted Non-matching Frame
     *   0 = Received frame matching filter index FIDX
     *   1 = Received frame did not match any Rx filter element
     */
    uint8  *data;
    /**< Data bytes.
     *   Only first dlc number of bytes are valid.
     */
}MCAN_RxBufElementNoCpy;

/**
 * \brief  Structure for MCAN Tx Event FIFO element.
 */
typedef struct
{
    uint32 id;
    /**< Identifier */
    uint32 rtr;
    /**< Remote Transmission Request
     *   0 = Data frame transmitted
     *   1 = Remote frame transmitted
     */
    uint32 xtd;
    /**< Extended Identifier
     *   0 = 11-bit standard identifier
     *   1 = 29-bit extended identifier
     */
    uint32 esi;
    /**< Error State Indicator
     *   0 = Transmitting node is error active
     *   1 = Transmitting node is error passive
     */
    uint32 txts;
    /**< Tx Timestamp */
    uint32 dlc;
    /**< Data Length Code
     *   0-8  = CAN + CAN FD: frame with 0-8 data bytes transmitted
     *   9-15 = CAN: frame with 8 data bytes transmitted
     *   9-15 = CAN FD: frame with 12/16/20/24/32/48/64 data bytes transmitted
     */
    uint32 brs;
    /**< Bit Rat Switching
     *   0 = Frame transmitted without bit rate switching
     *   1 = Frame transmitted with bit rate switching
     */
    uint32 fdf;
    /**< FD Format
     *   0 = Standard frame format
     *   1 = CAN FD frame format (new DLC-coding and CRC)
     */
    uint32 et;
    /**< Event Type
     *   00 = Reserved
     *   01 = Tx event
     *   10 = Transmission in spite of cancellation
     *        (always set for transmissions in DAR mode)
     *   11 = Reserved
     */
    uint32 mm;
    /**< Message Marker */
}MCAN_TxEventFIFOElement;

/**
 * \brief  Structure for MCAN Standard Message ID Filter Element.
 */
typedef struct
{
    uint32 sfid2;
    /**< Standard Filter ID 2 */
    uint32 sfid1;
    /**< Standard Filter ID 1 */
    uint32 sfec;
    /**< Standard Filter Element Configuration
     *   000 = Disable filter element
     *   001 = Store in Rx FIFO 0 if filter matches
     *   010 = Store in Rx FIFO 1 if filter matches
     *   011 = Reject ID if filter matches
     *   100 = Set priority if filter matches
     *   101 = Set priority and store in FIFO 0 if filter matches
     *   110 = Set priority and store in FIFO 1 if filter matches
     *   111 = Store into Rx Buffer or as debug message,
     *         configuration of SFT[1:0] ignored.
     *   If SFEC = “100”, “101”, or “110” a match sets high priority
     *   message event is generated.
     */
    uint32 sft;
    /**< Standard Filter Type
     *   00 = Range filter from SFID1 to SFID2 (SFID2 ≥ SFID1)
     *   01 = Dual ID filter for SFID1 or SFID2
     *   10 = Classic filter: SFID1 = filter, SFID2 = mask
     *   11 = Filter element disabled
     */
}MCAN_StdMsgIDFilterElement;

/**
 * \brief  Structure for MCAN Extended Message ID Filter Element.
 */
typedef struct
{
    uint32 efid1;
    /**< Extended Filter ID 1 */
    uint32 efec;
    /**< Extended Filter Element Configuration
     *   000 = Disable filter element
     *   001 = Store in Rx FIFO 0 if filter matches
     *   010 = Store in Rx FIFO 1 if filter matches
     *   011 = Reject ID if filter matches
     *   100 = Set priority if filter matches
     *   101 = Set priority and store in FIFO 0 if filter matches
     *   110 = Set priority and store in FIFO 1 if filter matches
     *   111 = Store into Rx Buffer or as debug message,
     *         configuration of SFT[1:0] ignored.
     *   If EFEC = “100”, “101”, or “110” a match sets high priority
     *   message event is generated.
     */
    uint32 efid2;
    /**< Extended Filter ID 2 */
    uint32 eft;
    /**< Extended Filter Type
     *   00 = Range filter from EFID1 to EFID2 (EFID2 ≥ EFID1)
     *   01 = Dual ID filter for EFID1 or EFID2
     *   10 = Classic filter: EFID1 = filter, EFID2 = mask
     *   11 = Range filter from EFID1 to EFID2 (EFID2 ≥ EFID1),
     *        XIDAM mask not applied
     */
}MCAN_ExtMsgIDFilterElement;

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */


/**
 * \brief   This function checks if the memory initialization is done for
 *          MCAN module.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 *
 * \retval  state           Returns TRUE if memory initialization is done.
 *                          Else returns FALSE.
 */
uint32 MCAN_isMemInitDone(uint32 baseAddr);

/**
 * \brief   This API will set MCAN module mode of operation.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 * \param   mode            Mode of operation.
 *                          Refer enum MCAN_OperationMode.
 *
 * \return  None.
 */
void MCAN_setOpMode(uint32 baseAddr, uint32 mode);

/**
 * \brief   This API will return MCAN module mode of operation.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 *
 * \return  mode            Mode of operation.
 *                          Refer enum MCAN_OperationMode.
 */
uint32 MCAN_getOpMode(uint32 baseAddr);

/**
 * \brief   This API will initialize MCAN module.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 * \param   initParams      Initialization parameters.
 *                          Refer struct MCAN_InitParams.
 *
 * \return  status          Initialization status.
 */
int32_t MCAN_init(uint32 baseAddr, const MCAN_InitParams *initParams);

/**
 * \brief   This API will configure MCAN module.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 * \param   configParams    configuration parameters.
 *                          Refer struct MCAN_ConfigParams.
 *
 * \return  status          Configuration status.
 */
int32_t MCAN_config(uint32 baseAddr, const MCAN_ConfigParams *configParams);

/**
 * \brief   This API will enable/disable ECC on the Message RAM.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 * \param   configParams    MCAN ECC Configuration Parameters.
 *                          Refer struct MCAN_ECCConfigParams.
 *
 * \return  None.
 */
void MCAN_eccConfig(uint32                    baseAddr,
                    const MCAN_ECCConfigParams *configParams);

/**
 * \brief   This API will configure a bit timings for MCAN module.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 * \param   configParams    Configuration parameters for MCAN bit timing.
 *                          Refer struct MCAN_BitTimingParams.
 *
 * \return  status          Bit Timings configuration status.
 */
int32_t MCAN_setBitTime(uint32                    baseAddr,
                        const MCAN_BitTimingParams *configParams);

/**
 * \brief   This API will configure Different sections of Message RAM.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 * \param   msgRAMConfigParams
 *                          Message RAM Configuration parameters.
 *                          Refer struct MCAN_MsgRAMConfigParams.
 *
 * \return  status          Configuration Status.
 */
int32_t MCAN_msgRAMConfig(uint32                       baseAddr,
                          const MCAN_MsgRAMConfigParams *msgRAMConfigParams);


/**
 * \brief   This API is used to write Tx message to message RAM. This uses
 *          the MCAN_TxBufElementNoCpy structure element which has data as a
 *          pointer instead of an array.
 *          Note that as the data is a pointer here hence corruption of data is
 *          possible in case you exceed the payload size.
 *
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 * \param   memType         Part of message ram to which given message to write.
 *                          Refer enum MCAN_MemType.
 * \param   bufNum          Buffer  number where message to write.
 *                          This parameter will ignored if memType is FIFO/Q.
 * \param   elem            Message Object.
 *                          Refer struct MCAN_TxBufElementNoCpy.
 *
 * \return  None.
 */
void MCAN_writeMsgRamNoCpy(uint32                 baseAddr,
                      uint32                 memType,
                      uint32                 bufNum,
                      const MCAN_TxBufElementNoCpy *elem);

/**
 * \brief   This API will set Tx Buffer Add Request.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 * \param   bufNum          Tx Buffer number for which request is to be added.
 *
 * \return  status          Configuration Status.
 */
int32_t MCAN_txBufAddReq(uint32 baseAddr, uint32 bufNum);

/**
 * \brief   This API will return New Data Message Status.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 * \param   newDataStatus   Rx Buffer new data status.
 *                          Refer struct MCAN_RxNewDataStatus.
 *
 * \return  None.
 */
void  MCAN_getNewDataStatus(uint32              baseAddr,
                            MCAN_RxNewDataStatus *newDataStatus);

/**
 * \brief   This API clear New Data Message Status.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 * \param   newDataStatus
 *                          Rx Buffer new data status.
 *                          Refer struct MCAN_RxNewDataStatus.
 *
 * \return  None.
 */
void  MCAN_clearNewDataStatus(uint32                    baseAddr,
                              const MCAN_RxNewDataStatus *newDataStatus);

/**
 * \brief   This API is used to read received message from message RAM. This
 *          uses the MCAN_RxBufElementNoCpy structure element which has data
 *          as pointer instead of an array.
 *          Note that as the data is a pointer here hence corruption of data is
 *          possible in case you exceed the payload size.
 *
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 * \param   memType         Part of message ram to which given message to write.
 *                          Refer enum MCAN_MemType.
 * \param   bufNum          Buffer  number from where message is to read.
 *                          This parameter will ignored if memType is FIFO/Q.
 *  \param  fifoNum         FIFOs number from where message is to read.
 *                          Refer enum MCAN_RxFIFONum.
 *                          This parameter will ignored if memType is buffer.
 * \param   elem            Message Object.
 *                          Refer struct MCAN_RxBufElementNoCpy.
 *
 * \return  None.
 */
void MCAN_readMsgRamNoCpy(uint32           baseAddr,
                     uint32           memType,
                     uint32           bufNum,
                     uint32           fifoNum,
                     MCAN_RxBufElementNoCpy *elem);


/**
 * \brief   This API is used to add Standard Message ID Filter Element.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 * \param   filtNum         Filter number.
 * \param   elem            Filter Object.
 *                          Refer struct MCAN_StdMsgIDFilterElement.
 *
 * \return  None.
 */
void MCAN_addStdMsgIDFilter(uint32                          baseAddr,
                            uint32                          filtNum,
                            const MCAN_StdMsgIDFilterElement *elem);

/**
 * \brief   This API is used to add Extended Message ID Filter Element.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 * \param   filtNum         Filter  number.
 * \param   elem            Filter Object.
 *                          Refer struct MCAN_ExtMsgIDFilterElement.
 *
 * \return  None.
 */
void MCAN_addExtMsgIDFilter(uint32                          baseAddr,
                            uint32                          filtNum,
                            const MCAN_ExtMsgIDFilterElement *elem);

/**
 * \brief   This API will enable/disable Loop Back Test Mode for
 *          MCAN module.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 * \param   lpbkMode        Loopback mode for MCAN.
 *                          Refer enum MCAN_LpbkMode.
 * \param   enable          Loop Back Mode is enabled if it is TRUE.
 *                          Loop Back Mode is disabled  if it is FALSE.
 *
 * \return  None.
 * \note    This API can be called only when MCAN is in Software
 *          Initialization mode of operation.
 */
void MCAN_lpbkModeEnable(uint32 baseAddr,
                         uint32 lpbkMode,
                         uint32 enable);


/**
 * \brief   This API will return protocol status for MCAN module.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 * \param   protStatus      Protocol Status.
 *                          Refer struct MCAN_ProtocolStatus.
 *
 * \return  None.
 */
void  MCAN_getProtocolStatus(uint32             baseAddr,
                             MCAN_ProtocolStatus *protStatus);

/**
 * \brief   This API is used to enable/disable interrupts.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 * \param   intrMask        Interrupts to enable.
 *                          Refer enum MCAN_IntrSrc.
 * \param   enable          Interrupt is enabled if it is TRUE.
 *                          Interrupt is disabled  if it is FALSE.
 *
 * \return  None.
 */
void MCAN_enableIntr(uint32 baseAddr, uint32 intrMask, uint32 enable);

/**
 * \brief   This API is used to select interrupt line.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 * \param   intrMask        Interrupt Number for which interrupt
 *                          line is to be selected. Refer enum MCAN_IntrSrc.
 * \param   lineNum         Interrupt Line to select.
 *                          Refer enum MCAN_IntrLineNum,
 *
 * \return  None.
 */
void MCAN_selectIntrLine(uint32 baseAddr,
                         uint32 intrMask,
                         uint32 lineNum);


/**
 * \brief   This API is used to enable/disable selected interrupt line.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 * \param   lineNum         Interrupt Line to select.
 *                          Refer enum MCAN_IntrLineNum,
 * \param   enable          Interrupt Line is enabled if it is 1.
 *                          Interrupt Line is disabled  if it is 0.
 *
 * \return  None.
 */
void MCAN_enableIntrLine(uint32 baseAddr,
                         uint32 lineNum,
                         uint32 enable);

/**
 * \brief   This API will return interrupt status.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 *
 * \return  status          Interrupt Status.
 */
uint32 MCAN_getIntrStatus(uint32 baseAddr);

/**
 * \brief   This API is used to clear the interrupt status.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 * \param   intrMask        Interrupts to clear status.
 *
 * \return  None.
 */
void MCAN_clearIntrStatus(uint32 baseAddr, uint32 intrMask);


/**
 * \brief   This API will Rx FIFO status.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 * \param   fifoStatus      Rx FIFO Status.
 *                          Refer struct MCAN_RxFIFOStatus.
 *
 * \return  None.
 */
void MCAN_getRxFIFOStatus(uint32           baseAddr,
                          MCAN_RxFIFOStatus *fifoStatus);

/**
 * \brief   This API will write Rx FIFO Acknowledgement.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 *                          Refer enum MCAN_RxFIFONum.
 * \param   fifoNum         FIFO Number.
 * \param   idx             Rx FIFO Acknowledge Index
 *
 * \return  status          Acknowledgement Status.
 */
int32_t MCAN_writeRxFIFOAck(uint32 baseAddr,
                            uint32 fifoNum,
                            uint32 idx);

/**
 * \brief   This API will return Tx Buffer Request Pending status.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 *
 * \return  status          Tx Buffer Request Pending status.
 */
uint32 MCAN_getTxBufReqPend(uint32 baseAddr);

/**
 * \brief   This API will set Tx Buffer Cancellation Request.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 * \param   buffNum         Tx Buffer number for which request is to be added.
 *
 * \return  status          Configuration Status.
 */
int32_t MCAN_txBufCancellationReq(uint32 baseAddr, uint32 buffNum);

/**
 * \brief   This API will return Tx Buffer Transmission Occurred status.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 *
 * \return  status          Tx Buffer Transmission Occurred status.
 */
uint32 MCAN_getTxBufTransmissionStatus(uint32 baseAddr);

/**
 * \brief   This API will return Transmit Buffer Cancellation Finished status.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 *
 * \return  status          Transmit Buffer Cancellation Finished status.
 */
uint32 MCAN_txBufCancellationStatus(uint32 baseAddr);

/**
 * \brief   This API is used to enable/disable Tx Buffer Transmission Interrupt.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 * \param   bufNum          Buffer number for which interrupt is to enable.
 * \param   enable          Interrupt is enabled if it is TRUE.
 *                          Interrupt is disabled  if it is FALSE.
 *
 * \return  status          Configuration status.
 */
int32_t MCAN_txBufTransIntrEnable(uint32 baseAddr,
                                  uint32 bufNum,
                                  uint32 enable);


/* ========================================================================== */
/*                          Advance Functions                                 */
/* ========================================================================== */

/**
 * \brief   This API will return clock stop acknowledgement
 *          for MCAN module.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 *
 * \return  ack             Clock Stop Acknowledge
 *                          0= No clock stop acknowledged
 *                          1= M_CAN may be set in power down
 */
uint32 MCAN_getClkStopAck(uint32 baseAddr);

#ifdef __cplusplus
}

#endif /*extern "C" */

#endif
 /** @} */
