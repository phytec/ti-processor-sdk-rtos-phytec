/**
 *  \file       ecap.h
 *
 *  \brief      This file contains the function prototypes for the device
 *              abstraction layer for ECAP. It also contains some
 *              related macro definitions and some files to be included.
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


#ifndef ECAP_H
#define ECAP_H

#include "hw_ecap.h"
#ifdef __cplusplus
extern "C" {
#endif
/****************************************************************************/
/*
** Values that can be passed to ECAPOperatingModeSelect API as modeSelect
** to select type operating mode.
*/

#define  ECAP_CAPTURE_MODE               (1)
#define  ECAP_APWM_MODE                  (0)

/****************************************************************************/
/*
** Values that can be passed to ECAPTimeStampRead API as capEvtFlag
** to determine for which capture event time-stamp has to be returned
*/

#define  ECAP_CAPTURE_EVENT_1            ((uint32)0x08U)
#define  ECAP_CAPTURE_EVENT_2            ((uint32)0x0cU)
#define  ECAP_CAPTURE_EVENT_3            ((uint32)0x10U)
#define  ECAP_CAPTURE_EVENT_4            ((uint32)0x14U)

/****************************************************************************/
/*
** Values that can be passed to ECAPOneShotModeConfig API as stopVal
** which determines number of captures allowed to occur before
** Capture register(1-4) frozen.\n
*/

#define  ECAP_CAPTURE_EVENT1_STOP        (((uint32)0x00U) << ECAP_ECCTL2_STOP_WRAP_SHIFT)
#define  ECAP_CAPTURE_EVENT2_STOP        (((uint32)0x01U) << ECAP_ECCTL2_STOP_WRAP_SHIFT)
#define  ECAP_CAPTURE_EVENT3_STOP        (((uint32)0x02U) << ECAP_ECCTL2_STOP_WRAP_SHIFT)
#define  ECAP_CAPTURE_EVENT4_STOP        (((uint32)0x03U) << ECAP_ECCTL2_STOP_WRAP_SHIFT)

/****************************************************************************/
/*
** Values that can be passed to ECAPAPWMPolarityConfig API as flag
** which determines the output polarity for APWM output
*/

#define  ECAP_APWM_ACTIVE_HIGH           (0)
#define  ECAP_APWM_ACTIVE_LOW            (1)

/****************************************************************************/
/*
** Values that can be passed to ECAPCounterControl API  as flag
** which determines whether counter to be configured  to stop
** or free running
*/

#define  ECAP_COUNTER_STOP               (0)
#define  ECAP_COUNTER_FREE_RUNNING       (1)

/****************************************************************************/
/*
** Values that can be passed to ECAPSyncInOutSelect API  as syncIn
** to disable syncIn or to enable counter to be loaded from
** CNTPHS register upon a SYNCI signal
*/

#define  ECAP_SYNC_IN_DISABLE            (((uint32)0U) << ECAP_ECCTL2_SYNCI_EN_SHIFT)
#define  ECAP_ENABLE_Counter             (((uint32)1U) << ECAP_ECCTL2_SYNCI_EN_SHIFT)

/****************************************************************************/
/*
** Values that can be passed to ECAPSyncInOutSelect API  as syncOut
** to select syncIn event to be the Sync-Out signal or select
** PRD_eq event to be Sync-Out signal or to disaqble syncOut
** signal
*/

#define  ECAP_SYNC_IN                    (((uint32)0x0U) << ECAP_ECCTL2_SYNCO_SEL_SHIFT)
#define  ECAP_PRD_EQ                     (((uint32)0x1U) << ECAP_ECCTL2_SYNCO_SEL_SHIFT)
#define  ECAP_SYNC_OUT_DISABLE           (((uint32)0x2U) << ECAP_ECCTL2_SYNCO_SEL_SHIFT)

/****************************************************************************/
/*
** Values that can be passed to ECAPIntEnable/Disable  API
** to enable or disable interrupt or it can be passed
** ECAPIntStatus to get the interrupt or it can be passed
** to ECAPIntStatusClear to clear the interrupt status
*/

#define  ECAP_CEVT1_INT                  (ECAP_ECEINT_CEVT1)
#define  ECAP_CEVT2_INT                  (ECAP_ECEINT_CEVT2)
#define  ECAP_CEVT3_INT                  (ECAP_ECEINT_CEVT3)
#define  ECAP_CEVT4_INT                  (ECAP_ECEINT_CEVT4)
#define  ECAP_CNTOVF_INT                 (ECAP_ECEINT_CTROVF)
#define  ECAP_PRDEQ_INT                  (ECAP_ECEINT_CTR_PRD)
#define  ECAP_CMPEQ_INT                  (ECAP_ECEINT_CTR_CMP)

/* It can be passed to ECAPIntStatusClear to clear global interrupt flag */
#define ECAP_GLOBAL_INT                   (ECAP_ECFLG_INT)

/****************************************************************************/
/*
** Values that can be passed to ECAPStandByModeConfig API
** to configure ECAP module in different stand by mode.
**
*/
#define     ECAP_SMART_STAND_BY_WAKE_UP     (3)
#define     ECAP_FORCE_STAND_BY             (0)
#define     ECAP_SMART_STAND_BY             (2)
#define     ECAP_NO_STAND_BY                (1)
/****************************************************************************/
/*
** Values that can be passed to ECAPIdleModeConfig API
** to configure ECAP module in different Idle mode.
**
*/
#define     ECAP_SMART_IDLE_WAKE_UP        (3)
#define     ECAP_SMART_IDLE_MODE           (2)
#define     ECAP_FORCE_IDLE_MODE           (0)
#define     ECAP_NO_IDLE_MODE              (1)

/*
** Structure to save the ECAP context
*/
typedef struct ecapContext {
    uint16 ecctl2;
    uint16 ecclr;
    uint16 eceint;
    uint32 pwm0ssclkconfig;
    uint32 tsctr;
    uint32 cap1;
    uint32 cap2;
}ECAPCONTEXT;

void ECAPGlobalIntEnable(uint32 baseAdd);
void ECAPContinousModeConfig(uint32 baseAdd);
void ECAPCaptureLoadingEnable(uint32 baseAdd);
void ECAPCaptureLoadingDisable(uint32 baseAdd);
void ECAPIntEnable(uint32 baseAdd, uint32 flag);
void ECAPIntDisable(uint32 baseAdd, uint32 flag);
void ECAPIntStatusClear(uint32 baseAdd, uint32 flag);
void ECAPCounterControl(uint32 baseAdd, uint32 flag);
uint32 ECAPIntStatus(uint32 baseAdd, uint32 flag);
void ECAPSyncInOutSelect(uint32 baseAdd, uint32 syncIn,
                         uint32 syncOut);
void ECAPPrescaleConfig(uint32 baseAdd, uint32 prescale);
void ECAPOperatingModeSelect(uint32 baseAdd, uint32 modeSelect);
uint32 ECAPTimeStampRead(uint32 baseAdd, uint32 capEvtFlag);
void ECAPCapeEvtPolarityConfig(uint32 baseAdd, uint32 capEvt1pol,
                               uint32 capEvt2pol, uint32 capEvt3pol,
                               uint32 capEvt4pol);
void ECAPCaptureEvtCntrRstConfig(uint32 baseAdd, uint32 CounterRst1,
                                 uint32 CounterRst2,uint32 CounterRst3,
                                 uint32 CounterRst4);


#ifdef __cplusplus
}
#endif

#endif
