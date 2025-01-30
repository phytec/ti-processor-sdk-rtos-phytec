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
 *  \file     Eth_IrqPacing.c
 *
 *  \brief    This file contains the Eth Irq pacing feature implementation
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <Std_Types.h>

#include <hw_include/soc.h>
#include <hw_include/hw_types.h>
#include <hw_include/lldr_timer.h>

#include "Eth_Cfg.h"
#include "Eth_IrqPacing.h"
#include "SchM_Eth.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**< \brief Maximum timer resolution */
#define ETH_IRQPACING_MAX_TIMER_COUNTER      (0xFFFFFFFFu)

/**< \brief Max Tick Value */
#define ETH_IRQPACING_MAX_TICK_VALUE_MAX     (0xF0000000u)

/**< \brief  Tick Value */
#define ETH_IRQPACING_TICK_VALUE_MAX         (0x0FFFFFFFu)

/**< \brief Max Wait counter Value */
#define ETH_IRQPACING_HW_WAIT_COUNT_MAX      (10000u)

/**< \brief Timer Match event interrupt mask */
#define ETH_IRQPACING_HW_INT_ALL_EN_FLAG     (TIMER_IRQENABLE_SET_TCAR_EN_FLAG_MASK | \
                                              TIMER_IRQENABLE_SET_OVF_EN_FLAG_MASK |  \
                                              TIMER_IRQENABLE_SET_MAT_EN_FLAG_MASK)

/**< \brief Timer disable event interrupt mask */
#define ETH_IRQPACING_HW_INT_ALL_DIS_FLAG    ((TIMER_IRQENABLE_SET_TCAR_EN_FLAG_MASK |  \
                                               TIMER_IRQENABLE_SET_OVF_EN_FLAG_MASK  |  \
                                               TIMER_IRQENABLE_SET_MAT_EN_FLAG_MASK) &  \
                                              (TIMER_IRQENABLE_CLR_TCAR_EN_FLAG_MASK |  \
                                               TIMER_IRQENABLE_CLR_OVF_EN_FLAG_MASK  |  \
                                               TIMER_IRQENABLE_CLR_MAT_EN_FLAG_MASK))

/**< \brief Timer enable overflow interrupt mask */
#define ETH_IRQPACING_HW_INT_OVF_EN_FLAG     ((TIMER_IRQENABLE_SET_OVF_EN_FLAG_MASK) & \
                                              (TIMER_IRQENABLE_CLR_TCAR_EN_FLAG_MASK | \
                                               TIMER_IRQENABLE_CLR_OVF_EN_FLAG_MASK |  \
                                               TIMER_IRQENABLE_CLR_MAT_EN_FLAG_MASK))


/**< \brief Timer clear overflow interrupt status */
#define ETH_IRQPACING_HW_INT_OVF_INT_ST_FLAG ((TIMER_IRQSTATUS_RAW_OVF_IT_FLAG_MASK) &  \
                                              (TIMER_IRQSTATUS_TCAR_IT_FLAG_MASK     |  \
                                               TIMER_IRQSTATUS_OVF_IT_FLAG_MASK      |  \
                                               TIMER_IRQSTATUS_MAT_IT_FLAG_MASK))

/**< \brief Timer Match event wakeup mask */
#define ETH_IRQPACING_HW_WAKE_ALL_DIS_FLAG   ((TIMER_IRQWAKEEN_TCAR_WUP_ENA_MASK     |   \
                                               TIMER_IRQWAKEEN_OVF_WUP_ENA_MASK      |   \
                                               TIMER_IRQWAKEEN_MAT_WUP_ENA_MASK)     &   \
                                              (TIMER_IRQWAKEEN_TCAR_WUP_ENA_MASK     |   \
                                               TIMER_IRQWAKEEN_OVF_WUP_ENA_MASK      |   \
                                               TIMER_IRQWAKEEN_MAT_WUP_ENA_MASK))

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief Init hardware timer
 *
 * \param ctrlIdx     Eth controller idx
 * \param timerIdx    Eth hardware timer idx
 *
 *  \return E_OK init successfully else E_NOT_OK
 */
static Std_ReturnType Eth_IrqPacingHwTimerInit(uint8 ctrlIdx, uint8 timerIdx);

/**
 *  \brief Start hardware timer
 *
 * \param ctrlIdx     Eth controller idx
 * \param timerIdx    Eth hardware timer idx
 *
 *  \return None
 */
static void Eth_IrqPacingStartHwTimer(uint8 ctrlIdx, uint8 timerIdx);

/**
 *  \brief Stop hardware timer
 *
 * \param ctrlIdx     Eth controller idx
 * \param timerIdx    Eth hardware timer idx
 *
 *  \return None
 */
static void Eth_IrqPacingStopHwTimer(uint8 ctrlIdx, uint8 timerIdx);

/**
 *  \brief clear  hardware timer interrupt
 *
 * \param ctrlIdx     Eth controller idx
 * \param timerIdx    Eth hardware timer idx
 *
 *  \return None
 */
static void Eth_IrqPacingHwTimerClearInt(uint8 ctrlIdx, uint8 timerIdx);

/* ========================================================================== */
/*                 Global Function Definitions                                */
/* ========================================================================== */

void Eth_IrqPacingRestartHwTimer(uint8 ctrlIdx, uint8 timerIdx, boolean isRestart)
{
    if(TRUE == Eth_GetHwTimerDynRunningState(ctrlIdx, timerIdx))
    {
        Eth_IrqPacingHwTimerClearInt(ctrlIdx, timerIdx);

        if(TRUE == isRestart)
        {
            Eth_IrqPacingStartHwTimer(ctrlIdx, timerIdx);
        }
        else
        {
            Eth_SetHwTimerDynRunningState(ctrlIdx, timerIdx, FALSE);
        }
    }
}

Std_ReturnType Eth_IrqPacingInit(uint8 ctrlIdx)
{
    Std_ReturnType retVal = E_OK;
    uint8 timerIdx;

    for(timerIdx = 0u; timerIdx < Eth_GetHwTimerTotalNum(ctrlIdx); ++timerIdx)
    {
        Eth_SetHwTimerDynRunningState(ctrlIdx, timerIdx, FALSE);
        retVal |= Eth_IrqPacingHwTimerInit(ctrlIdx, timerIdx);
    }

    return retVal;
}

void Eth_IrqPacingStopTimer(uint8 ctrlIdx)
{
    uint8 timerIdx;

    SchM_Enter_Eth_ETH_EXCLUSIVE_AREA_0();

    for(timerIdx = 0u; timerIdx < Eth_GetHwTimerTotalNum(ctrlIdx); ++timerIdx)
    {
        if(TRUE == Eth_GetHwTimerDynRunningState(ctrlIdx, timerIdx))
        {
            Eth_IrqPacingStopHwTimer(ctrlIdx, timerIdx);
            Eth_SetHwTimerDynRunningState(ctrlIdx, timerIdx, FALSE);
        }
    }
    SchM_Exit_Eth_ETH_EXCLUSIVE_AREA_0();
}

void Eth_IrqPacingStartTimer(uint8 ctrlIdx, uint8 timerIdx)
{
    SchM_Enter_Eth_ETH_EXCLUSIVE_AREA_0();

    if(FALSE == Eth_GetHwTimerDynRunningState(ctrlIdx, timerIdx))
    {
        Eth_SetHwTimerDynRunningState(ctrlIdx, timerIdx, TRUE);
        Eth_IrqPacingStartHwTimer(ctrlIdx, timerIdx);
    }

    SchM_Exit_Eth_ETH_EXCLUSIVE_AREA_0();
}

/* ========================================================================== */
/*                 Internal Function Definition                               */
/* ========================================================================== */
static Std_ReturnType Eth_IrqPacingHwTimerInit(uint8 ctrlIdx, uint8 timerIdx)
{
    Std_ReturnType retVal = E_OK;
    uint32 hwLoopCnt = 0u;
    uint32 regVal;
    uint32 baseAddr = Eth_GetHwTimerBaseAddr(ctrlIdx, timerIdx);

    HW_WR_FIELD32(baseAddr + TIMER_TSICR, TIMER_TSICR_SFT, (TIMER_TSICR_SFT_MASK >> TIMER_TSICR_SFT_SHIFT));
    HW_WR_FIELD32(baseAddr + TIMER_TIOCP_CFG, TIMER_TIOCP_CFG_SOFTRESET, TIMER_TIOCP_CFG_SOFTRESET_SOFTRESET_VALUE_1);

    while(HW_RD_FIELD32(baseAddr + TIMER_TIOCP_CFG, TIMER_TIOCP_CFG_SOFTRESET) ==
             (uint32)TIMER_TIOCP_CFG_SOFTRESET_SOFTRESET_VALUE_1)
    {
        hwLoopCnt++;
        if(hwLoopCnt == ETH_IRQPACING_HW_WAIT_COUNT_MAX)
        {
            retVal = E_NOT_OK;
            break;
        }
    }

    if(retVal == E_OK)
    {
        HW_WR_FIELD32(baseAddr + TIMER_TIOCP_CFG, TIMER_TIOCP_CFG_EMUFREE, TIMER_TIOCP_CFG_EMUFREE_TIMER_FREE);
        HW_WR_FIELD32(baseAddr + TIMER_TCLR, TIMER_TCLR_ST, TIMER_TCLR_ST_ST_VALUE_0);
        HW_WR_FIELD32(baseAddr + TIMER_TSICR, TIMER_TSICR_POSTED, 0U);

        HW_WR_REG32(baseAddr + TIMER_TCRR, ETH_IRQPACING_TICK_VALUE_MAX);
        HW_WR_REG32(baseAddr + TIMER_TLDR, ETH_IRQPACING_TICK_VALUE_MAX);

        HW_WR_REG32(baseAddr + TIMER_IRQENABLE_CLR, ETH_IRQPACING_HW_INT_ALL_DIS_FLAG);

        regVal = HW_RD_REG32(baseAddr + TIMER_IRQWAKEEN) & ~((uint32)ETH_IRQPACING_HW_WAKE_ALL_DIS_FLAG);
        HW_WR_REG32(baseAddr + TIMER_IRQWAKEEN, regVal);

        HW_WR_FIELD32(baseAddr + TIMER_TCLR, TIMER_TCLR_PRE, TIMER_TCLR_PRE_PRE_VALUE_0);
    }

    return retVal;
}

static void Eth_IrqPacingHwTimerClearInt(uint8 ctrlIdx, uint8 timerIdx)
{
    uint32 baseAddr = Eth_GetHwTimerBaseAddr(ctrlIdx, timerIdx);
    HW_WR_REG32((baseAddr + TIMER_IRQSTATUS), ETH_IRQPACING_HW_INT_OVF_INT_ST_FLAG);
}

static void Eth_IrqPacingStartHwTimer(uint8 ctrlIdx, uint8 timerIdx)
{
    uint32 baseAddr = Eth_GetHwTimerBaseAddr(ctrlIdx, timerIdx);

    HW_WR_FIELD32(baseAddr + TIMER_TCLR, TIMER_TCLR_AR, TIMER_TCLR_AR_AR_VALUE_0);
    HW_WR_FIELD32(baseAddr + TIMER_TCLR, TIMER_TCLR_CE, TIMER_TCLR_CE_CE_VALUE_0);

    HW_WR_REG32(baseAddr + TIMER_TLDR, (ETH_IRQPACING_MAX_TIMER_COUNTER - Eth_GetHwTimerCounter(ctrlIdx, timerIdx)));
    HW_WR_REG32(baseAddr + TIMER_TCRR, (ETH_IRQPACING_MAX_TIMER_COUNTER - Eth_GetHwTimerCounter(ctrlIdx, timerIdx)));

    HW_WR_REG32(baseAddr + TIMER_IRQENABLE_SET,ETH_IRQPACING_HW_INT_OVF_EN_FLAG);

    HW_WR_FIELD32(baseAddr + TIMER_TCLR, TIMER_TCLR_ST, TIMER_TCLR_ST_ST_VALUE_1);
}

static void Eth_IrqPacingStopHwTimer(uint8 ctrlIdx, uint8 timerIdx)
{
    uint32 baseAddr = Eth_GetHwTimerBaseAddr(ctrlIdx, timerIdx);
    HW_WR_FIELD32(baseAddr + TIMER_TCLR, TIMER_TCLR_ST, TIMER_TCLR_ST_ST_VALUE_0);
}
