/**
 * @file  csl_cpts.c
 *
 * @brief
 *  C implementation file for Ethernet Time Synchronization module CSL.
 *
 *  Contains the different control command and status query functions definations
 *
 *  \par
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

/**
 * \brief This is to disable HW_SYNC_BARRIER for J7 due to performance
 *        requirement
 */
#if defined (SOC_J721E) || defined (SOC_J7200)
#define MEM_BARRIER_DISABLE
#endif

#include "csl_cpts.h"

#if defined (SOC_AM273X) || defined (SOC_AWR294X) || defined(SOC_AM263X)
#define CSL_CPTS_NUM_TS_GENF    (1U)
#define CSL_CPTS_NUM_TS_ESTF    (1U)
#else
#define CSL_CPTS_NUM_TS_GENF    (sizeof(pCptsRegs->TS_GENF) / sizeof(CSL_cptsRegs_TS_GENF))
#define CSL_CPTS_NUM_TS_ESTF    (sizeof(pCptsRegs->TS_ESTF) / sizeof(CSL_cptsRegs_TS_ESTF))
#endif

void CSL_CPTS_enableCpts (
    CSL_cptsRegs    *pCptsRegs
)
{
    CSL_REG32_FINS (&pCptsRegs->CONTROL_REG, CPTS_CONTROL_REG_CPTS_EN, 1);
    return;
}

void CSL_CPTS_disableCpts (
    CSL_cptsRegs    *pCptsRegs
)
{
    CSL_REG32_FINS (&pCptsRegs->CONTROL_REG, CPTS_CONTROL_REG_CPTS_EN, 0);
    return;
}

void CSL_CPTS_setCntlReg (
    CSL_cptsRegs        *pCptsRegs,
    const CSL_CPTS_CONTROL*   pCntlCfg
)
{
    uint32 value = 0;

    CSL_FINS (value,  CPTS_CONTROL_REG_CPTS_EN, pCntlCfg->cptsEn);
    CSL_FINS (value,  CPTS_CONTROL_REG_INT_TEST, pCntlCfg->intTest);
    CSL_FINS (value,  CPTS_CONTROL_REG_TS_COMP_POLARITY, pCntlCfg->tsCompPolarity);
    CSL_FINS (value,  CPTS_CONTROL_REG_TSTAMP_EN, pCntlCfg->tstampEn);
    CSL_FINS (value,  CPTS_CONTROL_REG_SEQUENCE_EN, pCntlCfg->seqEn);
    CSL_FINS (value,  CPTS_CONTROL_REG_MODE, pCntlCfg->ts64bMode);
    CSL_FINS (value,  CPTS_CONTROL_REG_TS_COMP_TOG, pCntlCfg->tsCompToggle);
    CSL_FINS (value,  CPTS_CONTROL_REG_HW1_TS_PUSH_EN, pCntlCfg->tsHwPushEn[0]);
    CSL_FINS (value,  CPTS_CONTROL_REG_HW2_TS_PUSH_EN, pCntlCfg->tsHwPushEn[1]);
    CSL_FINS (value,  CPTS_CONTROL_REG_HW3_TS_PUSH_EN, pCntlCfg->tsHwPushEn[2]);
    CSL_FINS (value,  CPTS_CONTROL_REG_HW4_TS_PUSH_EN, pCntlCfg->tsHwPushEn[3]);
    CSL_FINS (value,  CPTS_CONTROL_REG_HW5_TS_PUSH_EN, pCntlCfg->tsHwPushEn[4]);
    CSL_FINS (value,  CPTS_CONTROL_REG_HW6_TS_PUSH_EN, pCntlCfg->tsHwPushEn[5]);
    CSL_FINS (value,  CPTS_CONTROL_REG_HW7_TS_PUSH_EN, pCntlCfg->tsHwPushEn[6]);
    CSL_FINS (value,  CPTS_CONTROL_REG_HW8_TS_PUSH_EN, pCntlCfg->tsHwPushEn[7]);
    CSL_FINS (value,  CPTS_CONTROL_REG_TS_SYNC_SEL, (uint32)pCntlCfg->tsOutputBitSel);
#ifdef CSL_CPTS_CONTROL_REG_TS_RX_NO_EVENT_MASK
    CSL_FINS (value,  CPTS_CONTROL_REG_TS_RX_NO_EVENT, pCntlCfg->tsDisableRxEvents);
#endif
#ifdef CSL_CPTS_CONTROL_REG_TS_GENF_CLR_EN_MASK
    CSL_FINS (value,  CPTS_CONTROL_REG_TS_GENF_CLR_EN, pCntlCfg->tsGenfClrEn);
#endif
    CSL_REG32_WR( &pCptsRegs->CONTROL_REG, value );
    return;
}

void CSL_CPTS_TSEventPush (
    CSL_cptsRegs        *pCptsRegs
)
{
    CSL_REG32_FINS (&pCptsRegs->TS_PUSH_REG, CPTS_TS_PUSH_REG_TS_PUSH, 1);
    return;
}

void CSL_CPTS_setTSAddVal (
    CSL_cptsRegs        *pCptsRegs,
    uint32            tsAddVal
)
{
    if( CSL_REG32_FEXT (&pCptsRegs->CONTROL_REG, CPTS_CONTROL_REG_MODE) == 1U )
    {
        CSL_REG32_FINS (&pCptsRegs->TS_ADD_VAL_REG, CPTS_TS_ADD_VAL_REG_ADD_VAL, tsAddVal);
    }
    else
    {
        CSL_REG32_FINS (&pCptsRegs->TS_ADD_VAL_REG, CPTS_TS_ADD_VAL_REG_ADD_VAL, 0);
    }
    return;
}

uint32 CSL_CPTS_isRawInterruptStatusBitSet (
    const CSL_cptsRegs  *pCptsRegs
)
{
    return CSL_REG32_FEXT (&pCptsRegs->INTSTAT_RAW_REG, CPTS_INTSTAT_RAW_REG_TS_PEND_RAW);
}

void CSL_CPTS_popEvent (
    CSL_cptsRegs    *pCptsRegs
)
{
    CSL_REG32_FINS (&pCptsRegs->EVENT_POP_REG, CPTS_EVENT_POP_REG_EVENT_POP, 1);
    return;
}

void CSL_CPTS_getEventInfo (
    const CSL_cptsRegs  *pCptsRegs,
    CSL_CPTS_EVENTINFO* pEventInfo
)
{
    uint32 value = CSL_REG32_RD( &pCptsRegs->EVENT_1_REG );

    pEventInfo->timeStamp   =   CSL_REG32_FEXT (&pCptsRegs->EVENT_0_REG, CPTS_EVENT_0_REG_TIME_STAMP);
    pEventInfo->seqId       =   CSL_FEXT (value, CPTS_EVENT_1_REG_SEQUENCE_ID);
    pEventInfo->msgType     =   CSL_FEXT (value, CPTS_EVENT_1_REG_MESSAGE_TYPE);
    pEventInfo->eventType   =   CSL_FEXT (value, CPTS_EVENT_1_REG_EVENT_TYPE);
    pEventInfo->portNo      =   CSL_FEXT (value, CPTS_EVENT_1_REG_PORT_NUMBER);
    pEventInfo->domain      =   CSL_REG32_FEXT (&pCptsRegs->EVENT_2_REG, CPTS_EVENT_2_REG_DOMAIN);
    pEventInfo->timeStampHi =   CSL_REG32_FEXT (&pCptsRegs->EVENT_3_REG, CPTS_EVENT_3_REG_TIME_STAMP);
    return;
}

