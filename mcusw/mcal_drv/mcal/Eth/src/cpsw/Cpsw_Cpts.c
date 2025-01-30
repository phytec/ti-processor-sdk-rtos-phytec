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
 *  \file     Cpsw_Cpts.c
 *
 *  \brief    This file contains CPTS related
 *            functionality.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <Std_Types.h>
#include "csl_cpts.h"
#include "cslr_xge_cpsw_ss_s.h"
#include "cpsw/Cpsw_Types.h"
#include "cpsw/Cpsw_Cpts.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**< \brief CPST hardward push event number . */
#define CPSW_CPTS_HWPUSH_EVEN_NUM		(8u)

/**< \brief CPST FFT clock frequency . */
#define CPSW_CPTS_RFTCLK_FREQ_200MHZ	(4u)

/**< \brief CPST max time retrive . */
#define CPSW_CPTS_RETRIVE_TS_LOOP		(10000u)

/**< \brief CPST cache size . */
#define CPSW_CPTS_EVENT_CACHE_SIZE      (8u)

/*! \brief Minimum PPM value. */
#define CPSW_CPTS_PPM_MIN_VAL           (0x400U)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *  \brief CPTS event type
 *
 *  Type of the CPTS event.
 */
typedef enum eCpswCpts_EventType
{
    CPSW_CPTS_EVENTTYPE_TS_PUSH = 0U,
    /**< CPTS software time stamp push event */
    CPSW_CPTS_EVENTTYPE_TS_ROLLOVER,
     /**< CPTS time stamp rollover event (32-bit mode only) */
    CPSW_CPTS_EVENTTYPE_TS_HALFROLLOVER,
     /**< CPTS time stamp half rollover event (32-bit mode only) */
    CPSW_CPTS_EVENTTYPE_HW_TS_PUSH,
    /**< CPTS hardware time stamp push event */
    CPSW_CPTS_EVENTTYPE_ETH_RECEIVE,
    /**< CPTS Ethernet receive event */
    CPSW_CPTS_EVENTTYPE_ETH_TRANSMIT,
    /**< CPTS Ethernet transmit event */
    CPSW_CPTS_EVENTTYPE_TS_COMP,
    /**< CPTS time stamp compare event */
    CPSW_CPTS_EVENTTYPE_TS_HOST_TX,
    /**< CPTS time stamp HOTS TX event */
    CPSW_CPTS_EVENTTYPE_MAX,
    /**< Invalid event */
} CpswCpts_EventType;

/**
 *  \brief CPTS event entry type
 *
 *  Type of the CPTS event entry.
 */
typedef struct sCpswCpts_Event
{
    uint64 tsVal;
    /**< Time stamp value when the event occurred */
    CpswCpts_EventType eventType;
     /**< CPTS event type information */
    uint32 msgType;
    /**< CPTS event message type */
    uint32 seqId;
     /**< Sequence id of the event */
    uint32 portNum;
    /**< MAC port number corresponding to the CPTS event */
    uint32 domain;
    /**< Domain number of the event */
    uint8 hwPushNum;
    /**< Hardware push instance number which triggered the event */
    boolean valid;
    /**< This event is valid or invalid */
} CpswCpts_Event;


/* ========================================================================== */
/*                            Local  Variables                                */
/* ========================================================================== */
/**< \brief CPST cache for cpts even . */
static CpswCpts_Event cptsCacheEvent[CPSW_CPTS_EVENTTYPE_MAX][CPSW_CPTS_EVENT_CACHE_SIZE];

/**< \brief CPST current free cache index for cpts even . */
static uint8 cptsEventCacheCurrentIdx[CPSW_CPTS_EVENTTYPE_MAX];

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief Get base address of CPSW CPTS register set
 *
 *  \return  Base address of CPSW CPTS register set
 */
static inline CSL_cptsRegs* CpswCpts_getRegAddr(void);

/**
 *  \brief Init Cpts cache
 *
 *  Init Cpts cache
 *
 *  \return None
 */
static void CpswCpts_cacheInit(void);

/**
 *  \brief Lookup cpts event in cache
 *
 *  Lookup cpts event in cache
 *
 *  \param EventIdx cache index
 *  \param DomainId domain id
 *  \param msgType  message type
 *  \param seqId  sequence id
 *  \param timeStampPtr egress timestamp value
 *
 *  \return \ref Std_ReturnType
 */
static Std_ReturnType CpswCpts_cacheLookUp(uint8 EventIdx,
    uint32 Domain,
    uint32 msgType,
    uint32 SeqId,
    uint64 *timeStampPtr);

/**
 *  \brief Update Cpts event to cache
 *
 *  Lookup cpts event in cache
 *
 *  \param eventInfo cpst event infor
 *
 *  \return \ref Std_ReturnType
 */
static void CpswCpts_cacheUpdate(CSL_CPTS_EVENTINFO *eventInfo);

/**
 *  \brief Process pending cpts event in cache
 *
 *  Proccess pending cpts event in cache
 *
 *  \param EventIdx cache index
 *  \param DomainId domain id
 *  \param msgType  message type
 *  \param seqId  sequence id
 *  \param timeStampPtr egress timestamp value
 *
 *  \return \ref Std_ReturnType
 */
static Std_ReturnType CpswCpts_processPendingFifoEvent(CpswCpts_EventType EventId,
    uint32 DomainId,
    uint32 msgType,
    uint32 seqId,
    uint64 *timeStampPtr);

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
void CpswCpts_open(void)
{
    uint16 idx;
    CSL_CPTS_CONTROL cptsCtrl;

    cptsCtrl.cptsEn            = TRUE;
    cptsCtrl.intTest           = FALSE;
    cptsCtrl.ts64bMode    	   = TRUE;
    cptsCtrl.tsOutputBitSel    = 0u;
    cptsCtrl.seqEn        	   = FALSE;
    cptsCtrl.tsCompToggle 	   = FALSE;
    cptsCtrl.tsCompPolarity    = TRUE;
    cptsCtrl.tsDisableRxEvents = FALSE;
    cptsCtrl.tsGenfClrEn       = FALSE;
    cptsCtrl.tstampEn          = TRUE;

    for(idx = 0u; idx < CPSW_CPTS_HWPUSH_EVEN_NUM; ++idx)
    {
        cptsCtrl.tsHwPushEn[idx] = TRUE;
    }

    CSL_CPTS_disableCpts(CpswCpts_getRegAddr());
    CSL_CPTS_setCntlReg(CpswCpts_getRegAddr(), &cptsCtrl);
    CSL_CPTS_enableCpts(CpswCpts_getRegAddr());
    CSL_CPTS_setTSAddVal(CpswCpts_getRegAddr(), Eth_Cpsw_GetCptsRefClockFreq());

    CpswCpts_cacheInit();
}

/*
 * Internal comment:
 * caller must disable irq
 */
Std_ReturnType CpswCpts_getCurrentTimeStamp(uint64 *timeStampPtr)
{
    Std_ReturnType retVal = E_NOT_OK;

    CSL_CPTS_TSEventPush(CpswCpts_getRegAddr());
    retVal = CpswCpts_processPendingFifoEvent(CPSW_CPTS_EVENTTYPE_TS_PUSH, 0u, 0u, 0u, timeStampPtr);

    return retVal;
}

/*
 * Internal comment:
 * This function is called inside function TxConfirmation callback, which is already Disable IRQ
 * So no need to disable IRQ.
 */
Std_ReturnType CpswCpts_getHostEgressTimeStamp(uint32 domainId,
    uint32 msgType,
    uint32 seqId,
    uint64 *timeStampPtr)
{
    Std_ReturnType retVal = E_NOT_OK;

    retVal = CpswCpts_cacheLookUp(CPSW_CPTS_EVENTTYPE_TS_HOST_TX, domainId, msgType, seqId, timeStampPtr);
    if(E_NOT_OK == retVal)
    {
        retVal = CpswCpts_processPendingFifoEvent(CPSW_CPTS_EVENTTYPE_TS_HOST_TX, domainId, msgType, seqId, timeStampPtr);
    }

    return retVal;
}

/* ========================================================================== */
/*                 Internal Function Definition                               */
/* ========================================================================== */
static inline CSL_cptsRegs* CpswCpts_getRegAddr(void)
{
    return (CSL_cptsRegs*)Eth_Cpsw_GetCptsRegAddr();
}

static void CpswCpts_cacheInit(void)
{
    uint16 eventIdx;
    uint8 eventCacheIdx;

    for(eventIdx = 0u; eventIdx < (uint16)CPSW_CPTS_EVENTTYPE_MAX; ++eventIdx)
    {
        for(eventCacheIdx = 0u; eventCacheIdx < CPSW_CPTS_EVENT_CACHE_SIZE; ++eventCacheIdx)
        {
            cptsCacheEvent[eventIdx][eventCacheIdx].tsVal        = 0u;
            cptsCacheEvent[eventIdx][eventCacheIdx].eventType    = CPSW_CPTS_EVENTTYPE_MAX;
            cptsCacheEvent[eventIdx][eventCacheIdx].seqId        = 0u;
            cptsCacheEvent[eventIdx][eventCacheIdx].portNum      = 0u;
            cptsCacheEvent[eventIdx][eventCacheIdx].domain       = 0u;
            cptsCacheEvent[eventIdx][eventCacheIdx].hwPushNum    = 0u;
            cptsCacheEvent[eventIdx][eventCacheIdx].valid        = FALSE;
        }
        cptsEventCacheCurrentIdx[eventIdx] = 0u;
    }
}

static Std_ReturnType CpswCpts_cacheLookUp(uint8 EventIdx,
    uint32 Domain,
    uint32 msgType,
    uint32 SeqId,
    uint64 *timeStampPtr)
{
    uint8 eventCacheIdx;
    Std_ReturnType ret = E_NOT_OK;

    for(eventCacheIdx = 0u; eventCacheIdx < CPSW_CPTS_EVENT_CACHE_SIZE; ++eventCacheIdx)
    {
        if((cptsCacheEvent[EventIdx][eventCacheIdx].valid == TRUE) &&
           (cptsCacheEvent[EventIdx][eventCacheIdx].msgType == msgType) &&
           (cptsCacheEvent[EventIdx][eventCacheIdx].seqId == SeqId) &&
           (cptsCacheEvent[EventIdx][eventCacheIdx].domain == Domain))
        {
            *timeStampPtr = cptsCacheEvent[EventIdx][eventCacheIdx].tsVal;
            cptsCacheEvent[EventIdx][eventCacheIdx].valid = FALSE;
            ret = E_OK;
            break;
        }
    }

    return ret;
}

static void CpswCpts_cacheUpdate(CSL_CPTS_EVENTINFO *eventInfo)
{
    uint32 eventIdx;
    uint8 eventCacheIdx;

    if(eventInfo->eventType < (uint32)CPSW_CPTS_EVENTTYPE_MAX)
    {
        eventIdx = eventInfo->eventType;
        eventCacheIdx = cptsEventCacheCurrentIdx[eventIdx];

        cptsCacheEvent[eventIdx][eventCacheIdx].tsVal   = ((((uint64)(eventInfo->timeStampHi)) << 32U) | ((uint64)(eventInfo->timeStamp)));
        cptsCacheEvent[eventIdx][eventCacheIdx].msgType = eventInfo->msgType;
        cptsCacheEvent[eventIdx][eventCacheIdx].seqId   = eventInfo->seqId;
        cptsCacheEvent[eventIdx][eventCacheIdx].domain  = eventInfo->domain;
        cptsCacheEvent[eventIdx][eventCacheIdx].valid   = TRUE;

        cptsEventCacheCurrentIdx[eventIdx] = (cptsEventCacheCurrentIdx[eventIdx] + 1u) % CPSW_CPTS_EVENT_CACHE_SIZE;
    }
}

static Std_ReturnType CpswCpts_processPendingFifoEvent(CpswCpts_EventType eventId,
    uint32 domainId,
    uint32 msgType,
    uint32 seqId,
    uint64 *timeStampPtr)
{
    Std_ReturnType retVal = E_NOT_OK;
    CSL_CPTS_EVENTINFO eventInfo;
    uint32 loopCnt = 0u;

    while(loopCnt < CPSW_CPTS_RETRIVE_TS_LOOP)
    {
        if(TRUE == CSL_CPTS_isRawInterruptStatusBitSet(CpswCpts_getRegAddr()))
        {
            CSL_CPTS_getEventInfo(CpswCpts_getRegAddr(), &eventInfo);
            CSL_CPTS_popEvent(CpswCpts_getRegAddr());

            if((eventInfo.eventType == (uint32)eventId) &&
                (eventInfo.domain == domainId) &&
                (eventInfo.msgType == msgType) &&
                (eventInfo.seqId == seqId))
            {
                *timeStampPtr = ((((uint64)(eventInfo.timeStampHi)) << 32U) | ((uint64)(eventInfo.timeStamp)));
                retVal = E_OK;
                break;
            }
            else
            {
                CpswCpts_cacheUpdate(&eventInfo);
            }
        }
        else
        {
            loopCnt++;
        }
    }
    return retVal;
}
