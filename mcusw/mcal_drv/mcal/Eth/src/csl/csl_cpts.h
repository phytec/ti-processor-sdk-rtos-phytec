/**
 * @file  csl_cpts.h
 *
 * @brief
 *  Header file containing various enumerations, structure definitions and function
 *  declarations for the Time synchronization submodule of EMAC.
 *
 *  ============================================================================
 *  @n   (C) Copyright 2024, Texas Instruments, Inc.
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

#ifndef CSL_CPTS_H_
#define CSL_CPTS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "cslr.h"
#include "cslr_cpts.h"

/**
@defgroup CSL_CPTS_SYMBOL  CPTS Symbols Defined
@ingroup CSL_CPTS_API
*/
/**
@defgroup CSL_CPTS_DATASTRUCT  CPTS Data Structures
@ingroup CSL_CPTS_API
*/
/**
@defgroup CSL_CPTS_FUNCTION  CPTS Functions
@ingroup CSL_CPTS_API
*/
/**
@defgroup CSL_CPTS_ENUM CPTS Enumerated Data Types
@ingroup CSL_CPTS_API
*/

/**
@addtogroup CSL_CPTS_SYMBOL
@{
*/

/**
@}
*/

/** @addtogroup CSL_CPTS_DATASTRUCT
 @{ */

/** @brief
 *
 *  Defines CPTS event types.
 */
/**  Time stamp push event */
#define     CSL_CPTS_EVENTTYPE_TS_PUSH          0

/**  Time stamp rollover event (32-bit mode only) */
#define     CSL_CPTS_EVENTTYPE_TS_ROLLOVER      1

/**  Time stamp Half Rollover event (32-bit mode only) */
#define     CSL_CPTS_EVENTTYPE_TS_HALFROLLOVER  2

/**  Hardware Time stamp push event */
#define     CSL_CPTS_EVENTTYPE_HW_TS_PUSH       3

/**  Ethernet receive event */
#define     CSL_CPTS_EVENTTYPE_ETH_RECEIVE      4

/**  Ethernet Transmit event */
#define     CSL_CPTS_EVENTTYPE_ETH_TRANSMIT     5

/**  Time stamp compare event (non-toggle mode only) */
#define     CSL_CPTS_EVENTTYPE_TS_COMP          6

/**  Host event */
#define     CSL_CPTS_EVENTTYPE_HOST             7

/** @brief
 *
 *  Holds the Time sync submodule's version info.
 */
typedef struct {
    /**  Minor version value */
    uint32      minorVer;

    /**  Major version value */
    uint32      majorVer;

    /**  RTL version value */
    uint32      rtlVer;

    /**  Identification value */
    uint32      id;
} CSL_CPTS_VERSION;

/** @brief
 *
 *  Holds Time sync event info contents.
 */
typedef struct {
    /**  32-bit Event Time stamp  */
    uint32      timeStamp;

    /**  Upper 32-bit Event Time stamp provided by new generation CPTS */
    uint32      timeStampHi;

    /**  Event Sequence Id */
    uint32      seqId;

    /**  Event Message Type */
    uint32      msgType;

    /**  Event Type */
    uint32      eventType;

    /**  EMAC Port number */
    uint32      portNo;

    /**  Event Domain */
    uint32      domain;
} CSL_CPTS_EVENTINFO;


/**
 * @defgroup CSL_CPTS_API CPTS API
 *
 */

/**
@defgroup CSL_CPTS_DATASTRUCT  CPTS Data Structures
@ingroup CSL_CPTS_API
*/
/**
@defgroup CSL_CPTS_FUNCTION  CPTS Functions
@ingroup CSL_CPTS_API
*/
/**
@defgroup CSL_CPTS_ENUM CPTS Enumerated Data Types
@ingroup CSL_CPTS_API
*/

/** @addtogroup CSL_CPTS_DATASTRUCT
 @{ */


/** @brief
 *
 *  Defines CPTS timestamp output bits
 */
typedef uint32 CSL_CPTS_TS_OUTPUT_BIT;
#define CPTS_TS_OUTPUT_BIT_DISABLED         ((uint32) 0U)
#define CPTS_TS_OUTPUT_BIT_17               ((uint32) 1U)
#define CPTS_TS_OUTPUT_BIT_18               ((uint32) 2U)
#define CPTS_TS_OUTPUT_BIT_19               ((uint32) 3U)
#define CPTS_TS_OUTPUT_BIT_20               ((uint32) 4U)
#define CPTS_TS_OUTPUT_BIT_21               ((uint32) 5U)
#define CPTS_TS_OUTPUT_BIT_22               ((uint32) 6U)
#define CPTS_TS_OUTPUT_BIT_23               ((uint32) 7U)
#define CPTS_TS_OUTPUT_BIT_24               ((uint32) 8U)
#define CPTS_TS_OUTPUT_BIT_25               ((uint32) 9U)
#define CPTS_TS_OUTPUT_BIT_26               ((uint32) 10U)
#define CPTS_TS_OUTPUT_BIT_27               ((uint32) 11U)
#define CPTS_TS_OUTPUT_BIT_28               ((uint32) 12U)
#define CPTS_TS_OUTPUT_BIT_29               ((uint32) 13U)
#define CPTS_TS_OUTPUT_BIT_30               ((uint32) 14U)
#define CPTS_TS_OUTPUT_BIT_31               ((uint32) 15U)

/** @brief
 *
 *  Defines PPM Correction Direction
 */
typedef uint32 CSL_CPTS_TS_PPM_DIR;
#define CSL_CPTS_TS_PPM_DIR_INCREASE        ((uint32) 0U)
#define CSL_CPTS_TS_PPM_DIR_DECREASE        ((uint32) 1U)

/** @brief
 *
 *  Holds the CPTS control register info.
 */
typedef struct {
    /**  Time Sync Enable:  When disabled (cleared to zero), the RCLK domain is
         held in reset. */
    uint32                cptsEn;

    /**  Interrupt Test: When set, this bit allows the raw interrupt to be written to
         facilitate interrupt test. */
    uint32                intTest;

    /**  TS_COMP Polarity: 0 - TS_COMP is asserted low; 1: TS_COMP is asserted high */
    uint32                tsCompPolarity;

    /**  Host Receive Timestamp Enable: When set, Timestamps enabled on received packets to host */
    uint32                tstampEn;

    /**  Sequence Enable:
         0: The timestamp value increments with the selected RFTCLK
         1: The timestamp for received packets is the sequence number of the received packet
      */
    uint32                seqEn;

    /** 64-bit mode:
        0: The timestamp is 32-bits with the upper 32-bits forced to zero.
        1: The timestamp is 64-bits.
      */
    uint32                ts64bMode;

    /**  TS_COMP Toggle mode - 0: TS_COMP is in non-toggle mode; 1: TS_COMP is in toggle mode */
    uint32                tsCompToggle;

    /**  Hardware push 1-8 enable */
    uint32                tsHwPushEn[8];

    /**  TS_SYNC output timestamp counter bit select */
    CSL_CPTS_TS_OUTPUT_BIT  tsOutputBitSel;

    /**  Disable Timestamp Ethernet receive events:
            0: Receive events are enabled
            1: Receive events are disabled
    */
    uint32                tsDisableRxEvents;

    /**  GENF (and ESTF) clear enable:
            0: A TS_GENFn (or TS_ESTFn) output is not cleared when the associated ts_genf_length[31:0] (or ts_estf_length[31:0]) is cleared to zero
            1: A TS_GENFn (or TS_ESTFn) output is cleared when the associated ts_genf_length[31:0] (or ts_estf_length[31:0]) is cleared to zero
    */
    uint32                tsGenfClrEn;

} CSL_CPTS_CONTROL;

/**
@}
*/

/** @addtogroup CSL_CPTS_FUNCTION
@{ */

/********************************************************************************
********************** Time Synchronization (CPTS) Submodule ********************
********************************************************************************/


/** ============================================================================
 *   @n@b CSL_CPTS_enableCpts
 *
 *   @b Description
 *   @n This function configures the CPTS control register to enable time sync.
 *
 *   @b Arguments
     @verbatim
        pCptsRegs           Pointer to CSL_cptsRegs structure
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n CPTS_CONTROL_REG_CPTS_EN=1
 *
 *   @b Example
 *   @verbatim

        CSL_CPTS_enableCpts (pCptsRegs);

     @endverbatim
 * =============================================================================
 */
extern void CSL_CPTS_enableCpts (
    CSL_cptsRegs    *pCptsRegs
);

/** ============================================================================
 *   @n@b CSL_CPTS_disableCpts
 *
 *   @b Description
 *   @n This function configures the CPTS control register to disable time sync.
 *
 *   @b Arguments
     @verbatim
        pCptsRegs           Pointer to CSL_cptsRegs structure
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n CPTS_CONTROL_REG_CPTS_EN=0
 *
 *   @b Example
 *   @verbatim

        CSL_CPTS_disableCpts (pCptsRegs);

     @endverbatim
 * =============================================================================
 */
extern void CSL_CPTS_disableCpts (
    CSL_cptsRegs    *pCptsRegs
);

/** ============================================================================
 *   @n@b CSL_CPTS_setCntllReg
 *
 *   @b Description
 *   @n This function sets up the contents of CPTS control register
 *
 *   @b Arguments
     @verbatim
        pCptsRegs           Pointer to CSL_cptsRegs structure
        pCntlCfg            CSL_CPTS_CONTROL contain settings for
                            CPTS control register.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n CPTS_CONTROL_REG_CPTS_EN
 *      CPTS_CONTROL_REG_INT_TEST
 *      CPTS_CONTROL_REG_TS_COMP_POLARITY
 *      CPTS_CONTROL_REG_TSTAMP_EN
 *      CPTS_CONTROL_REG_SEQUENCE_EN
 *      CPTS_CONTROL_REG_MODE
 *      CPTS_CONTROL_REG_TS_COMP_TOG
 *      CPTS_CONTROL_REG_HW1_TS_PUSH_EN
 *      CPTS_CONTROL_REG_HW2_TS_PUSH_EN
 *      CPTS_CONTROL_REG_HW3_TS_PUSH_EN
 *      CPTS_CONTROL_REG_HW4_TS_PUSH_EN
 *      CPTS_CONTROL_REG_HW5_TS_PUSH_EN
 *      CPTS_CONTROL_REG_HW6_TS_PUSH_EN
 *      CPTS_CONTROL_REG_HW7_TS_PUSH_EN
 *      CPTS_CONTROL_REG_HW8_TS_PUSH_EN
 *      CPTS_CONTROL_REG_TS_SYNC_SEL
 *      CPTS_CONTROL_REG_TS_RX_NO_EVENT
 *      CPTS_CONTROL_REG_TS_GENF_CLR_EN
 *
 *   @b Example
 *   @verbatim
        CSL_CPTS_CONTROL     cntlCfg;

        CSL_CPTS_setCntlReg (pCptsRegs, &cntlCfg);

     @endverbatim
 * =============================================================================
 */
extern void CSL_CPTS_setCntlReg (
    CSL_cptsRegs        *pCptsRegs,
    const CSL_CPTS_CONTROL*   pCntlCfg
);

/** ============================================================================
 *   @n@b CSL_CPTS_TSEventPush
 *
 *   @b Description
 *   @n This function writes an 1 to the Time Stamp Event Push register to
 *      generate a timestamp event. The time stamp value is the time of the write
 *      of this register, not the time of the event read. The time stamp value can then
 *      be read on interrupt via the event registers.
 *
 *   @b Arguments
     @verbatim
        pCptsRegs           Pointer to CSL_cptsRegs structure
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Affects
 *   @n CPTS_TS_PUSH_REG_TS_PUSH
 *
 *   @b Example
 *   @verbatim
        CSL_CPTS_TSEventPush (pCptsRegs);
     @endverbatim
 *
 *   @note: Software should not push a second time stamp event onto the event
 *          FIFO until the first time stamp value has been read from the event FIFO
 *          (there should be only one time stamp event in the event FIFO at any
 *          given time).
 * =============================================================================
 */
extern void CSL_CPTS_TSEventPush (
    CSL_cptsRegs        *pCptsRegs
);

/** ============================================================================
 *   @n@b CSL_CPTS_setTSAddVal
 *
 *   @b Description
 *   @n This function sets the Time Stamp Add Value to adjust the 64-bit
 *      timestamp value. The tsAddVal[2:0] is added to 1 to comprise the
 *      timestamp increment value. The timestamp increment value is added to the
 *      current timestamp (time_stamp[63:0]) on each RCLK.
 *
 *      Note that this function is only applicable for 64-bit timestamp mode.
 *      In 32-bit timestamp mode, 0 is written.
 *
 *   @b Arguments
     @verbatim
        pCptsRegs           Pointer to CSL_cptsRegs structure
        tsAddVal            Time stamp add value [0, 7]
 *
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None.
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Affects
 *   @n CPTS_TS_ADD_VAL_REG_ADD_VAL
 *
 *   @b Example
 *   @verbatim
        uint32          tsAddVal;

        tsAddVal   =   3;

        CSL_CPTS_setTSAddVal (pCptsRegs, tsAddVal);
     @endverbatim
 * =============================================================================
 */
extern void CSL_CPTS_setTSAddVal (
    CSL_cptsRegs        *pCptsRegs,
    uint32            tsAddVal
);

/** ============================================================================
 *   @n@b CSL_CPTS_isRawInterruptStatusBitSet
 *
 *   @b Description
 *   @n This function checks the Time Sync Raw Pending Interrupt Register to
 *      determine if there is one or more events in the event FIFO.
 *
 *   @b Arguments
     @verbatim
        pCptsRegs           Pointer to CSL_cptsRegs structure
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  TRUE                   One or more events in FIFO. Raw interrupt status
 *                              bit set.
 *   @n  FALSE                  No events in FIFO. Raw interrupt status bit cleared.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Reads
 *   @n CPTS_INTSTAT_RAW_REG_TS_PEND_RAW
 *
 *   @b Example
 *   @verbatim
        if (CSL_CPTS_isRawInterruptStatusBitSet (pCptsRegs) == TRUE)
        {
           // interrupt set
        }
        else
        {
            // interrupt bit not set
        }
     @endverbatim
 * =============================================================================
 */
extern uint32 CSL_CPTS_isRawInterruptStatusBitSet (
    const CSL_cptsRegs  *pCptsRegs
);

/** ============================================================================
 *   @n@b CSL_CPTS_popEvent
 *
 *   @b Description
 *   @n This function sets up the Event pop bit in Event pop register. This
 *      initiates an event to be popped off the event FIFO. Popping an event discards
 *      the event and causes the next event, if any, to be moved to the top of
 *      the FIFO ready to be read by software on interrupt.
 *
 *   @b Arguments
     @verbatim
        pCptsRegs           Pointer to CSL_cptsRegs structure
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n CPTS_EVENT_POP_REG_EVENT_POP=1
 *
 *   @b Example
 *   @verbatim

        CSL_CPTS_popEvent (pCptsRegs);

     @endverbatim
 * =============================================================================
 */
extern void CSL_CPTS_popEvent (
    CSL_cptsRegs    *pCptsRegs
);

/** ============================================================================
 *   @n@b CSL_CPTS_getEventInfo
 *
 *   @b Description
 *   @n This function retrieves the contents of the Event Low, Event Middle and Event High
 *      registers.
 *
 *   @b Arguments
     @verbatim
        pCptsRegs           Pointer to CSL_cptsRegs structure
        pEventInfo          CSL_CPTS_EVENTINFO structure that needs to be filled
                            with time sync event info read from hardware.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Reads
 *   @n CPTS_EVENT_LOW_REG_TIME_STAMP,
 *      CPTS_EVENT_MIDDLE_REG_SEQUENCE_ID,
 *      CPTS_EVENT_MIDDLE_REG_MESSAGE_TYPE,
 *      CPTS_EVENT_MIDDLE_REG_EVENT_TYPE,
 *      CPTS_EVENT_MIDDLE_REG_PORT_NUMBER,
 *      CPTS_EVENT_HIGH_REG_DOMAIN
 *
 *   @b Example
 *   @verbatim
        CSL_CPTS_EVENTINFO          eventInfo;

        CSL_CPTS_getEventInfo (pCptsRegs, &eventInfo);
     @endverbatim
 * =============================================================================
 */
extern void CSL_CPTS_getEventInfo (
    const CSL_cptsRegs  *pCptsRegs,
    CSL_CPTS_EVENTINFO* pEventInfo
);

/**
@}
*/

#ifdef __cplusplus
}
#endif

#endif
