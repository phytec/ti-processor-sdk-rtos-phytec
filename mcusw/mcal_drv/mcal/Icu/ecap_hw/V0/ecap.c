/**
 *  \file   ecap.c
 *
 *  \brief  ECAP APIs.
 *
 *   This file contains the device abstraction layer APIs for ECAP
 */

/*
* Copyright (C) 2024 Texas Instruments Incorporated - http://www.ti.com/
*/
/*
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
/** \brief This is to disable HW_SYNC_BARRIER for register access */
#define MEM_BARRIER_DISABLE

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <hw_include/lld_ecap.h>
#include "Icu/ecap_hw/V0/hw_ecap.h"
#include <hw_include/hw_types.h>

/*******************************************************************************
*                        API FUNCTION DEFINITIONS
*******************************************************************************/


/**
 * \brief   This function enables capture loading.
 *
 * \param   baseAdd   It is the Memory address of the ECAP instance used.
 *
 * \return  None.
 *
 **/
void ECAPCaptureLoadingEnable(uint32 baseAdd)
{
    uint32 temp_addr = baseAdd + ECAP_ECCTL1;
    uint16 value     = HW_RD_REG16(temp_addr);
    value |= (uint16)ECAP_ECCTL1_CAPLDEN;
    HW_WR_REG16(temp_addr, value);
}

/**
 * \brief   This function disables capture loading
 *
 * \param   baseAdd   It is the Memory address of the ECAP instance used.
 *
 * \return  None.
 *
 **/
void ECAPCaptureLoadingDisable(uint32 baseAdd)
{
    uint32 temp_addr = baseAdd + ECAP_ECCTL1;
    uint16 value     = HW_RD_REG16(temp_addr);
    value &= ((uint16)(~ECAP_ECCTL1_CAPLDEN));
    HW_WR_REG16(temp_addr, value);
}

/**
 * \brief   This function configures prescale value.
 *
 * \param   baseAdd   It is the Memory address of the ECAP instance used.
 * \param   prescale  It is the value which is used to prescale the incoming
 *                    input.
 *
 *          prescale can take any integer value between 0 and 62
 *
 * \return  None.
 *
 **/
void ECAPPrescaleConfig(uint32 baseAdd, uint32 prescale)
{
    uint32 temp_addr = baseAdd + ECAP_ECCTL1;
    uint16 value     = HW_RD_REG16(temp_addr);
    value             &= ((uint16)0xffffc1ffU);
    HW_WR_REG16(temp_addr,  value);
    value              = HW_RD_REG16(temp_addr);
    value             |= (uint16)(prescale << ECAP_ECCTL1_PRESCALE_SHIFT);
    HW_WR_REG16(temp_addr, value);
}

/**
 * \brief   This function configures ecapture module to operate in capture
 *          mode or in APWM mode.
 *
 * \param   baseAdd     It is the Memory address of the ECAP instance used.
 * \param   modeSelect  It is the value which determines whether ecapture
 *                      module to operate in capture mode or in APWM mode.\n
 *
 *         modeSelect can take following macros.
 *
 *         ECAP_CAPTURE_MODE - Capture Mode.\n
 *         ECAP_APWM_MODE    - APWM Mode.\n
 *
 * \return  None.
 *
 **/
void ECAPOperatingModeSelect(uint32 baseAdd, uint32 modeSelect)
{
    uint32 temp_addr = baseAdd + ECAP_ECCTL2;
    uint16 value;

    if(modeSelect!=(uint32) 0)
    {
         value = HW_RD_REG16(temp_addr);
         value &= ((uint16)~ECAP_ECCTL2_CAP_APWM);
         HW_WR_REG16(temp_addr, value);
    }
    else{
      /* Do Nothing. Added to avoid MISRA C error . */   
    }
}

/**
 * \brief   This function returns time-stamp for a given capture event.
 *
 * \param   baseAdd     It is the Memory address of the ECAP instance used.
 * \param   capEvtFlag  It is the value which determines for which capture event
 *                      time-stam has to returned.
 *
 *         capEvtFlag can take following macros.
 *
 *         ECAP_CAPTURE_EVENT_1 - Capture Event 1.\n
 *         ECAP_CAPTURE_EVENT_2 - Capture Event 2.\n
 *         ECAP_CAPTURE_EVENT_3 - Capture Event 3.\n
 *         ECAP_CAPTURE_EVENT_4 - Capture Event 4.\n
 *
 * \return  Returns the time-stamp for given capure event.
 *
 **/
uint32 ECAPTimeStampRead(uint32 baseAdd, uint32 capEvtFlag)
{
    return(HW_RD_REG32(baseAdd + capEvtFlag));
}

/**
 * \brief   This function configures Capture Event polarity.
 *
 * \param   baseAdd     It is the Memory address of the ECAP instance used.
 * \param   capEvt1pol  It determines whether Capture Event1 has to be generated
 *                      on rising edge or falling edge of pulse.
 *
 * \param   capEvt2pol  It determines whether Capture Event2 has to be generated
 *                      on rising edge or falling edge of pulse.
 *
 * \param   capEvt3pol  It determines whether Capture Event3 has to be generated
 *                      on rising edge or falling edge of pulse.
 *
 * \param   capEvt4pol  It determines whether Capture Event4 has to be generated
 *                      on rising edge or falling edge of pulse.
 *
 *                      0 - falling edge
 *                      1 - rising  edge
 * \return  None.
 *
 **/
void ECAPCapeEvtPolarityConfig(uint32 baseAdd, uint32 capEvt1pol,
                               uint32 capEvt2pol, uint32 capEvt3pol,
                               uint32 capEvt4pol)
{
    uint32 temp_addr = baseAdd + ECAP_ECCTL1;
    uint16 value = HW_RD_REG16(temp_addr);

    value &= ~CSL_ECAP_ECCTL1_CAP1POL_MASK;
    value |= (uint16)((capEvt1pol << 0) & CSL_ECAP_ECCTL1_CAP1POL_MASK);
    value &= ~CSL_ECAP_ECCTL1_CAP2POL_MASK;
    value |= (uint16)((capEvt2pol << 2) & CSL_ECAP_ECCTL1_CAP2POL_MASK);
    value &= ~CSL_ECAP_ECCTL1_CAP3POL_MASK;
    value |= (uint16)((capEvt3pol << 4) & CSL_ECAP_ECCTL1_CAP3POL_MASK);
    value &= ~CSL_ECAP_ECCTL1_CAP4POL_MASK;
    value |= (uint16)((capEvt4pol << 6) & CSL_ECAP_ECCTL1_CAP4POL_MASK);
    HW_WR_REG16(temp_addr, value);
}

/**
 * \brief   This function enables reset of the counters upon Capture Events.
 *
 * \param   baseAdd      It is the Memory address of the ECAP instance used.
 * \param   CounterRst1  It determines whether counter has to be reset upon
 *                       Capture Event1.
 *
 * \param   CounterRst2  It determines whether counter has to be reset upon
 *                       Capture Event2.
 *
 * \param   CounterRst3  It determines whether counter has to be reset upon
 *                       Capture Event3.
 *
 * \param   CounterRst4  It determines whether counter has to be reset upon
 *                       Capture Event4.
 *
 *                       0 - Don't reset counter upon capture event.\n
 *                       1 - Reset upon counter capture event.\n
 *
 * \return  None.
 *
 **/
void ECAPCaptureEvtCntrRstConfig(uint32 baseAdd, uint32 CounterRst1,
                                 uint32 CounterRst2,uint32 CounterRst3,
                                 uint32 CounterRst4)
{
    uint32 temp_addr = baseAdd + ECAP_ECCTL1;
    uint16 value = HW_RD_REG16(temp_addr);
    value &= ~CSL_ECAP_ECCTL1_CTRRST1_MASK;
    value |= (uint16)((CounterRst1 << 1) & CSL_ECAP_ECCTL1_CTRRST1_MASK);
    value &= ~CSL_ECAP_ECCTL1_CTRRST2_MASK;
    value |= (uint16)((CounterRst2 << 3) & CSL_ECAP_ECCTL1_CTRRST2_MASK);
    value &= ~CSL_ECAP_ECCTL1_CTRRST3_MASK;
    value |= (uint16)((CounterRst3 << 5) & CSL_ECAP_ECCTL1_CTRRST3_MASK);
    value &= ~CSL_ECAP_ECCTL1_CTRRST4_MASK;
    value |= (uint16)((CounterRst4 << 7) & CSL_ECAP_ECCTL1_CTRRST4_MASK);
    HW_WR_REG16(temp_addr, value);
}

/**
 * \brief   This function configures ECAP to Continuous mode.
 *
 * \param   baseAdd   It is the Memory address of the ECAP instance used.
 *
 * \return  None.\n
 *
 * This API is valid only if ECAP is configured to Capture Mode.It has
 * no significance when ECAP is configured in APWM mode.
 **/
void ECAPContinousModeConfig(uint32 baseAdd)
{
    uint32 temp_addr = baseAdd + ECAP_ECCTL2;
    uint16 value = HW_RD_REG16(temp_addr);
    value  &= ((uint16)(~ECAP_ECCTL2_CONT_ONESHT));
    HW_WR_REG16(temp_addr, value);
}

/**
 * \brief   This function configures counter to stop or free running
 *          based on its input argument flag.
 *
 * \param   baseAdd   It is the Memory address of the ECAP instance used.
 * \param   flag      It is the value which determine counter to be configured
 *                    to stop or free running.\n
 *
 *         flag can take following macros.\n
 *
 *         ECAP_COUNTER_STOP.\n
 *         ECAP_COUNTER_FREE_RUNNING.\n
 *
 * \return  None.
 *
 **/
void ECAPCounterControl(uint32 baseAdd, uint32 flag)
{
    uint32 temp_addr = baseAdd + ECAP_ECCTL2;
    uint16 value = HW_RD_REG16(temp_addr);

    if(flag!=(uint32)0)
    {
         value  |= ((uint16)ECAP_ECCTL2_TSCTRSTOP);
    }
    else
    {
         value  &= ((uint16)(~ECAP_ECCTL2_TSCTRSTOP));
    }

    HW_WR_REG16(temp_addr, value);
}

/**
 * \brief   This function configures Sync-In and Sync-Out.
 *
 * \param   baseAdd   It is the Memory address of the ECAP instance used.
 * \param   syncIn    It is the value which determines whether to disable
 *                    syncIn or to enable counter to be loaded from
 *                    CNTPHS register upon a SYNCI signal.\n
 *
 *         syncIn can take following macros.\n
 *
 *         ECAP_SYNC_IN_DISABLE.\n
 *         ECAP_ENABLE_COUNTER - Enables counter to load from CNTPHS
 *                               register upon SYNCI signal.\n
 *
 * \param   syncOut   It is the value which select type of syncOut signal
 *                    (i.e select syncIn event to be the Sync-Out signal,
 *                         select PRD_eq event to be Sync-Out signal).\n
 *
 *
 *         syncOut can take following macros.\n
 *
 *         ECAP_SYNC_IN - Select syncIn event to be the Sync-Out signal.\n
 *         ECAP_PRD_EQ  - Select PRD_eq event to be Sync-Out signal.\n
 *         ECAP_SYNC_OUT_DISABLE - Disable syncOut signal.\n
 *
 * \return  None.
 *
 **/
void ECAPSyncInOutSelect(uint32 baseAdd, uint32 syncIn,
                         uint32 syncOut)
{
    uint32 temp_addr = baseAdd + ECAP_ECCTL2;
    uint16 value = HW_RD_REG16(temp_addr);
    value  &= ((uint16)0xffffffdfU);
    value  |= ((uint16)syncIn);
    value  &= ((uint16)0xffffff3fU);
    value  |= ((uint16)syncOut);
    HW_WR_REG16(temp_addr, value);
}

/**
 * \brief   This function enables the generation of interrupts if any of
 *          event interrupt are enable and corresponding event interrupt
 *          flag is set.
 *
 * \param   baseAdd   It is the Memory address of the ECAP instance used.
 *
 * \return  None.
 *
 **/
void ECAPGlobalIntEnable(uint32 baseAdd)
{
    uint32 temp_addr = baseAdd + ECAP_ECCLR;
    uint16 value = HW_RD_REG16(temp_addr);
    value  |= ((uint16)ECAP_ECCLR_INT);
    HW_WR_REG16(temp_addr, value);
}

/**
 * \brief   This function enables the specified interrups
 *
 * \param   baseAdd   It is the Memory address of the ECAP instance used.
 * \param   flag      It is the value which specifies the interrupts to
 *                    be enabled.\n
 *
 *          flag can take following macros
 *
 *          ECAP_CEVT1_INT   - Enable Capture Event 1 interrupt.\n
 *          ECAP_CEVT2_INT   - Enable Capture Event 2 interrupt.\n
 *          ECAP_CEVT3_INT   - Enable Capture Event 3 interrupt.\n
 *          ECAP_CEVT4_INT   - Enable Capture Event 4 interrupt.\n
 *          ECAP_CNTOVF_INT  - Enable Counter Overflow interrupt.\n
 *          ECAP_PRDEQ_INT   - Enable Period equal interrupt.\n
 *          ECAP_CMPEQ_INT   - Enable Compare equal interrupt.\n
 *
 * \return  None.
 *
 **/
void ECAPIntEnable(uint32 baseAdd, uint32 flag)
{
    uint32 temp_addr = baseAdd + ECAP_ECEINT;
    uint16 value = HW_RD_REG16(temp_addr);
    value  |= ((uint16)flag);
    HW_WR_REG16(temp_addr, value);
}

/**
 * \brief   This function disables the specified interrups
 *
 * \param   baseAdd   It is the Memory address of the ECAP instance used.
 * \param   flag      It is the value which specifies the interrupts to
 *                    be disabled.\n
 *
 *          flag can take following macros.
 *
 *          ECAP_CEVT1_INT   - Enable Capture Event 1 interrupt.\n
 *          ECAP_CEVT2_INT   - Enable Capture Event 2 interrupt.\n
 *          ECAP_CEVT3_INT   - Enable Capture Event 3 interrupt.\n
 *          ECAP_CEVT4_INT   - Enable Capture Event 4 interrupt.\n
 *          ECAP_CNTOVF_INT  - Enable Counter Overflow interrupt.\n
 *          ECAP_PRDEQ_INT   - Enable Period equal interrupt.\n
 *          ECAP_CMPEQ_INT   - Enable Compare equal interrupt.\n
 *
 * \return  None.
 *
 **/
void ECAPIntDisable(uint32 baseAdd, uint32 flag)
{
    uint32 temp_addr = baseAdd + ECAP_ECEINT;
    uint32 value = HW_RD_REG32(temp_addr);
    uint16 value16;
    value16  = ((uint16)(value & ~flag));
    HW_WR_REG16(temp_addr, value16);
}

/**
 * \brief   This function returns the status specified interrups
 *
 * \param   baseAdd   It is the Memory address of the ECAP instance used.
 * \param   flag      It is the value which specifies the status of interrupts
 *                    to be returned.\n
 *
 *          flag can take following macros.
 *
 *          ECAP_CEVT1_INT   - Status of Capture Event 1 interrupt.\n
 *          ECAP_CEVT2_INT   - Status of Capture Event 2 interrupt.\n
 *          ECAP_CEVT3_INT   - Status of Capture Event 3 interrupt.\n
 *          ECAP_CEVT4_INT   - Status of Capture Event 4 interrupt.\n
 *          ECAP_CNTOVF_INT  - Status of Counter Overflow interrupt.\n
 *          ECAP_PRDEQ_INT   - Status of Period equal interrupt.\n
 *          ECAP_CMPEQ_INT   - Status of Compare equal interrupt.\n
 *          ECAP_GLOBAL_INT  - Global interrupt status.\n
 *
 * \returns Status of the specified interrupts.
 *
 **/
uint32 ECAPIntStatus(uint32 baseAdd, uint32 flag)
{
    uint32 temp_addr = baseAdd + ECAP_ECFLG;
    uint16 value = HW_RD_REG16(temp_addr);
    return (value & flag);
}

/**
 * \brief   This function clears of the status specified interrups
 *
 * \param   baseAdd   It is the Memory address of the ECAP instance used.
 * \param   flag      It is the value which specifies the status of interrupts
 *                    to be cleared.\n
 *
 *          flag can take following macros.
 *
 *          ECAP_CEVT1_INT   - Status of Capture Event 1 interrupt.\n
 *          ECAP_CEVT2_INT   - Status of Capture Event 2 interrupt.\n
 *          ECAP_CEVT3_INT   - Status of Capture Event 3 interrupt.\n
 *          ECAP_CEVT4_INT   - Status of Capture Event 4 interrupt.\n
 *          ECAP_CNTOVF_INT  - Status of Counter Overflow interrupt.\n
 *          ECAP_PRDEQ_INT   - Status of Period equal interrupt.\n
 *          ECAP_CMPEQ_INT   - Status of Compare equal interrupt.\n
 *
 * \return  None.
 *
 **/
void ECAPIntStatusClear(uint32 baseAdd, uint32 flag)
{
    uint32 temp_addr = baseAdd + ECAP_ECFLG;
    uint16 value = HW_RD_REG16(temp_addr);
    temp_addr = baseAdd + ECAP_ECCLR;
    HW_WR_REG16(temp_addr,  value  & ((uint16)flag));
}
