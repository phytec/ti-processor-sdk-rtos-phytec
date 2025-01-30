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
 *  \file  Gpt_Gptimer.c
 *
 *  \brief This file contains GPT MCAL driver functions for the General purpose
 *         timers.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <Gpt_Cfg.h>
#include "Gpt.h"
#include "Gpt_Priv.h"

#include "stdint.h"

/* There are static inline functions in hw_types.h file. Map them as well */
#define GPT_START_SEC_CODE
#include "Gpt_MemMap.h"
#include <hw_include/hw_types.h>
#define GPT_STOP_SEC_CODE
#include "Gpt_MemMap.h"

#include "Dem.h"
#include "Os.h"

/* CSL from PDK */
#include <hw_include/soc.h>
#include <hw_include/lldr_timer.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/*
 * Values that can be passed to TIMERModeConfigure as timerMode.
 */
/* Value used to enable the timer in one-shot and compare mode */
#define GPTIMER_ONESHOT_CMP_ENABLE          (TIMER_TCLR_CE_MASK)

/* Value used to enable the timer only in one-shot mode */
#define GPTIMER_ONESHOT_NOCMP_ENABLE        (0x0000000U)

/* Value used to enable the timer in auto-reload and compare mode */
#define GPTIMER_AUTORLD_CMP_ENABLE          (TIMER_TCLR_AR_MASK | \
                                             TIMER_TCLR_CE_MASK)

/* Value used to enable the timer only in auto-reload mode */
#define GPTIMER_AUTORLD_NOCMP_ENABLE        (TIMER_TCLR_AR_MASK)

/******************************************************************************/
/*
 * Values that can be passed to GPTIMERGPOConfigure as gpoCfg.
 */
/* Value used to drive 0 on PORGPOCFG pin */
#define GPTIMER_GPO_CFG_0         (TIMER_TCLR_GPO_CFG_GPO_CFG_0)

/* Value used to drive 1 on PORGPOCFG pin */
#define GPTIMER_GPO_CFG_1         (TIMER_TCLR_GPO_CFG_GPO_CFG_1 << \
                                   TIMER_TCLR_GPO_CFG_SHIFT)

/******************************************************************************/
/*
 * Values that can be passed to GPTIMERIntStatusClear/GPTIMERIntRawStatusSet/
 * as intFlags. Also these values can be used while checking the status got from
 * GPTIMERIntRawStatusGet/GPTIMERIntStatusGet.
 * Any combination is also followed.
 * Example- (GPTIMER_INT_TCAR_IT_FLAG | GPTIMER_INT_OVF_IT_FLAG)
 */
/* Value used for capture event of Timer */
#define GPTIMER_INT_TCAR_IT_FLAG             ( \
        TIMER_IRQSTATUS_RAW_TCAR_IT_FLAG_MASK)

/* Value used for overflow event of Timer */
#define GPTIMER_INT_OVF_IT_FLAG              ( \
        TIMER_IRQSTATUS_RAW_OVF_IT_FLAG_MASK)

/* Value used for Match event of Timer */
#define GPTIMER_INT_MAT_IT_FLAG              ( \
        TIMER_IRQSTATUS_RAW_MAT_IT_FLAG_MASK)

/******************************************************************************/
/*
 * Values that can be passed to GPTIMERResetConfigure as rstOption.
 */
/* Value used to enable software reset for Timer */
#define GPTIMER_SFT_RESET_ENABLE             (TIMER_TSICR_SFT_MASK)

/* Value used to disable software reset for Timer */
#define GPTIMER_SFT_RESET_DISABLE            (TIMER_TSICR_SFT_SFT_0)

/******************************************************************************/
/*
 * Values that can be used while checking the status received from
 * GPTIMERIsResetDone.
 */
/* Value used to check whether reset is done */
#define GPTIMER_IS_RESET_DONE                ( \
        TIMER_TIOCP_CFG_SOFTRESET_SOFTRESET_VALUE_0)

/* Value used to check whether reset is ongoing */
#define GPTIMER_IS_RESET_ONGOING             ( \
        TIMER_TIOCP_CFG_SOFTRESET_SOFTRESET_VALUE_1)

/******************************************************************************/
/*
 * Values that can be passed to TIMERPostedModeConfig as postMode.
 */
/* Value used to enable the posted mode of operation */
#define GPTIMER_POSTED                       (TIMER_TSICR_POSTED_MASK)

/* Value used to disable the posted mode of operation */
/* Non-posted mode is where CPU is stalled untill write completion response
 * to indicate success or failure of the transaction */
#define GPTIMER_NONPOSTED                    (TIMER_TSICR_POSTED_POSTED_VALUE_0)

/******************************************************************************/

/******************************************************************************/
/*
 * Values that can be passed to GPTIMERIdleModeConfigure as idleModeOption.
 */
/* Value used to set Timer in Force-idle mode*/
#define GPTIMER_FORCE_IDLE            (TIMER_TIOCP_CFG_IDLEMODE_IDLE_MODE_0X0)

/* Value used to set Timer in No-idle mode*/
#define GPTIMER_NO_IDLE               (TIMER_TIOCP_CFG_IDLEMODE_IDLE_MODE_0X1)

/* Value used to set Timer in Smart-idle mode*/
#define GPTIMER_SMART_IDLE            (TIMER_TIOCP_CFG_IDLEMODE_IDLE_MODE_0X2)

/* Value used to set Timer in Smart-idle wake-up-capable mode*/
#define GPTIMER_SMART_IDLE_WAKEUP     (TIMER_TIOCP_CFG_IDLEMODE_IDLE_MODE_0X3)

/******************************************************************************/
/*
 * Values that can be passed to GPTIMEREmuModeConfigure as emuModeOption.
 */
/* Value used to freeze Timer in emulation mode*/
#define GPTIMER_FROZEN            (TIMER_TIOCP_CFG_EMUFREE_TIMER_FROZEN)

/* Value used to free run Timer in emulation mode  */
#define GPTIMER_FREE              (TIMER_TIOCP_CFG_EMUFREE_TIMER_FREE)

/**
 * \brief  GP Timer interrupt flags.
 */
#define GPTIMER_INT_TCAR_EN_FLAG    (TIMER_IRQENABLE_SET_TCAR_EN_FLAG_MASK)
/**< Timer capture event interrupt mask */
#define GPTIMER_INT_OVF_EN_FLAG     (TIMER_IRQENABLE_SET_OVF_EN_FLAG_MASK)
/**< Timer overflow event interrupt mask */
#define GPTIMER_INT_MAT_EN_FLAG     (TIMER_IRQENABLE_SET_MAT_EN_FLAG_MASK)
/**< Timer Match event interrupt mask */
#define GPTIMER_INT_ALL_EN_FLAG     (TIMER_IRQENABLE_SET_TCAR_EN_FLAG_MASK | \
                                     TIMER_IRQENABLE_SET_OVF_EN_FLAG_MASK |  \
                                     TIMER_IRQENABLE_SET_MAT_EN_FLAG_MASK)
/**< Macro to control all the above interrupt status */

/**
 * \brief  GP Timer wakeup flags.
 */
#define GPTIMER_WAKE_TCAR_FLAG      (TIMER_IRQWAKEEN_TCAR_WUP_ENA_MASK)
/**< Timer capture event wakeup mask */
#define GPTIMER_WAKE_OVF_FLAG       (TIMER_IRQWAKEEN_OVF_WUP_ENA_MASK)
/**< Timer overflow event wakeup mask */
#define GPTIMER_WAKE_MAT_FLAG       (TIMER_IRQWAKEEN_MAT_WUP_ENA_MASK)
/**< Timer Match event wakeup mask */
#define GPTIMER_WAKE_ALL_EN_FLAG    (TIMER_IRQWAKEEN_TCAR_WUP_ENA_MASK | \
                                     TIMER_IRQWAKEEN_OVF_WUP_ENA_MASK |  \
                                     TIMER_IRQWAKEEN_MAT_WUP_ENA_MASK)
/**< Macro to control all the above wakeup status */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

static void Gpt_gptimerStartCount(uint32 baseAddr);
static void Gpt_gptimerStopCount(uint32 baseAddr);
static void Gpt_gptimerModeConfigure(uint32 baseAddr, uint32 timerMode);
static void Gpt_gptimerPreScalerClkEnable(uint32 baseAddr, uint32 ptv);
static void Gpt_gptimerPreScalerClkDisable(uint32 baseAddr);
static void Gpt_gptimerCounterSet(uint32 baseAddr, uint32 counter);
static void Gpt_gptimerReloadSet(uint32 baseAddr, uint32 reload);
static void Gpt_gptimerIntStatusClear(uint32 baseAddr, uint32 intFlags);
static void Gpt_gptimerIntEnable(uint32 baseAddr, uint32 intFlags);
static void Gpt_gptimerIntDisable(uint32 baseAddr, uint32 intFlags);
static void Gpt_gptimerWakeEnable(uint32 baseAddr, uint32 wakeFlags);
static void Gpt_gptimerWakeDisable(uint32 baseAddr, uint32 wakeFlags);
static void Gpt_gptimerResetConfigure(uint32 baseAddr, uint32 rstOption);
static void Gpt_gptimerReset(uint32 baseAddr);
static void Gpt_gptimerPostedModeConfig(uint32 baseAddr, uint32 postMode);
static void Gpt_gptimerEmuModeConfigure(uint32 baseAddr, uint32 emuModeOption);
static void Gpt_gptimerCounterInit(const Gpt_ChannelHwObjType *pHwUnitObj,
                                   uint32                      baseAddr);
static void Gpt_gptimerDisableNotifn(uint32 baseAddr);
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                  Internal Function Definitions                             */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

#define GPT_START_SEC_CODE

#include "Gpt_MemMap.h"

/**
 * \brief   This API will enable/disable GPTIMER wake up events.
 *
 * \param   baseAddr       Base Address of the Timer Module Register.
 *
 * \return  None.
 **/
void
    Gpt_gptimerStart(const Gpt_ChannelHwObjType *pHwUnitObj, uint32  countValue,
                     uint32 enableInt)
{
    Gpt_ChannelMode channelMode;
    uint32          baseAddr;

    baseAddr = pHwUnitObj->baseAddr;

    channelMode = pHwUnitObj->channelCfg.channelMode;

    if (GPT_CH_MODE_ONESHOT == channelMode)
    {
        Gpt_gptimerModeConfigure(baseAddr, GPTIMER_ONESHOT_NOCMP_ENABLE);
    }
    else
    {
        Gpt_gptimerModeConfigure(baseAddr, GPTIMER_AUTORLD_NOCMP_ENABLE);
    }


    /*
     * Design : MCAL-6194,MCAL-6346,MCAL-6231
     */

    /* Configure reload count even though this is one shot mode as it would
     * be used while returning elapsed time in Gpt_getElapsedTime function  */
    Gpt_gptimerReloadSet(baseAddr,
                         (GPTIMER_MAX_RESOLUTION - countValue));
    /* Calculate timer start value so after countValue ticks overflow occurs */
    Gpt_gptimerCounterSet(
        baseAddr,
        (GPTIMER_MAX_RESOLUTION - countValue));
    if ((uint32)GPT_PRESCALER_NO_PRESCALE != pHwUnitObj->channelCfg.prescale)
    {
        Gpt_gptimerPreScalerClkEnable(baseAddr, pHwUnitObj->channelCfg.prescale);
    }


    /*
     * Design : MCAL-6254
     */

    if ((uint32)FALSE != enableInt)
    {
        /* Enable notification interrupt */
        Gpt_gptimerIntEnable(baseAddr, GPTIMER_INT_OVF_EN_FLAG);
    }


     /*
      * Design : MCAL-6309
      */

    /* Start timer counting */
    Gpt_gptimerStartCount(baseAddr);

    return;
}

/**
 * \brief   This API will enable/disable GPTIMER wake up events.
 *
 * \param   baseAddr       Base Address of the Timer Module Register.
 *
 * \return  None.
 **/
void Gpt_gptimerStop(
                                        const Gpt_ChannelHwObjType *pHwUnitObj)
{
    /* stop timer counting */
    Gpt_gptimerStopCount(pHwUnitObj->baseAddr);
    return;
}

/**
 * \brief   This API will enable/disable GPTIMER wake up events.
 *
 * \param   baseAddr       Base Address of the Timer Module Register.
 *
 * \return  None.
 **/
void  Gpt_gptimerEnableNotify(
    const Gpt_ChannelHwObjType *pHwUnitObj)
{

    /*
     * Design : MCAL-6338,MCAL-6306
     */
    Gpt_gptimerIntEnable(pHwUnitObj->baseAddr, GPTIMER_INT_OVF_EN_FLAG);
    return;
}

/**
 * \brief   This API will enable/disable GPTIMER wake up events.
 *
 * \param   baseAddr       Base Address of the Timer Module Register.
 *
 * \return  None.
 **/
void Gpt_gptimerDisableNotify(
    const Gpt_ChannelHwObjType *pHwUnitObj)
{
    /* Disable interrupts */
    Gpt_gptimerIntDisable(pHwUnitObj->baseAddr, GPTIMER_INT_OVF_EN_FLAG);
    return;
}


/**
 * \brief   This API will enable GPTIMER wake up events.
 *
 * \param   baseAddr       Base Address of the Timer Module Register.
 *
 * \return  None.
 **/

 /*
  * Design : MCAL-6307
  */
void Gpt_gptimerEnableWakeup(
    const Gpt_ChannelHwObjType *pHwUnitObj)
{
    /* GPT driver supports only overflow as of now */
    Gpt_gptimerWakeEnable(pHwUnitObj->baseAddr,
                          TIMER_IRQWAKEEN_OVF_WUP_ENA_MASK);
    /* Need to enable interrupt as it is mapped as wakeup interrupt */
    Gpt_gptimerIntEnable(pHwUnitObj->baseAddr, GPTIMER_INT_OVF_EN_FLAG);
    return;
}

/**
 * \brief   This API will enable GPTIMER wake up events.
 *
 * \param   baseAddr       Base Address of the Timer Module Register.
 *
 * \return  None.
 **/

/*
 * Design : MCAL-6293
 */
void Gpt_gptimerDisableWakeup(
    const Gpt_ChannelHwObjType *pHwUnitObj)
{
    /* GPT driver supports only overflow as of now */
    Gpt_gptimerWakeDisable(pHwUnitObj->baseAddr,
                           TIMER_IRQWAKEEN_OVF_WUP_ENA_MASK);
    Gpt_gptimerIntDisable(pHwUnitObj->baseAddr, GPTIMER_INT_OVF_EN_FLAG);

    return;
}

/**
 * \brief   This API will initialize the timer.
 *
 * \param   baseAddr       Base Address of the Timer Module Register.
 * \param   pHwUnitObj     Pointer to Timer HW object to store initialized
 *                          value.
 *
 * \return  None.
 **/
void Gpt_gptimerInit(uint32   gptChannel,
                      Gpt_ChannelHwObjType *pHwUnitObj)
{
    uint32 baseAddr;

    baseAddr = Gpt_TimerBaseAddr[gptChannel];
    /*
     * Init the HW instance
     */
    pHwUnitObj->baseAddr = baseAddr;

    /* Reset timer configuration */
    Gpt_gptimerResetConfigure(baseAddr, GPTIMER_SFT_RESET_ENABLE);
    /* Reset timer module */
    Gpt_gptimerReset(baseAddr);
    /* Set timer EMU mode to Emulation free */
    Gpt_gptimerEmuModeConfigure(baseAddr, GPTIMER_FREE);
    /* Keep channel disabled till explicitly enable by Gpt_StartTimer */
    Gpt_gptimerStopCount(baseAddr);
    /* Enable not posted mode of timer operation. */
    Gpt_gptimerPostedModeConfig(baseAddr, GPTIMER_NONPOSTED);
    /* To implement Auto-reload/continuous timer as TCRR is reloaded
     * with the Timer Load Register (TLDR) value after a counting overflow
     **/
    Gpt_gptimerCounterInit(pHwUnitObj, baseAddr);

    return;
}

static void Gpt_gptimerDisableNotifn(uint32 baseAddr)
{
    Gpt_gptimerIntDisable(baseAddr, GPTIMER_INT_ALL_EN_FLAG);
    Gpt_gptimerWakeDisable(baseAddr, GPTIMER_WAKE_ALL_EN_FLAG);

    return;
}

/**
 * \brief   This API will deinitialize the timer.
 *
 * \param   pHwUnitObj     Pointer to Timer HW object.
 *
 * \return  None.
 **/
void Gpt_gptimerDeInit(
    const Gpt_ChannelHwObjType *pHwUnitObj)
{
    uint32 baseAddr = pHwUnitObj->baseAddr;


    /*
     * Design : MCAL-6224
     */
    /* Reset timer configuration */
    Gpt_gptimerResetConfigure(baseAddr, GPTIMER_SFT_RESET_ENABLE);
    /* Reset timer module */
    Gpt_gptimerReset(baseAddr);
    Gpt_gptimerStopCount(baseAddr);
    Gpt_gptimerCounterSet(baseAddr, 0U);
    Gpt_gptimerReloadSet(baseAddr, 0U);
    Gpt_gptimerPreScalerClkDisable(baseAddr);


  /*
   * Design : MCAL-6211
   */
    Gpt_gptimerDisableNotifn(baseAddr);

    return;
}

/**
 * \brief   Get/Read the counter value from the counter register.
 *
 * \param   baseAddr       Base Address of the Timer Module Register.
 *
 * \return  This API returns the count value present in the Timer Counter
 *          register.
 *
 * \note:   Value can be read from the counter register when the counter is
 *          stopped or when it is running.
 **/
uint32 Gpt_gptimerCounterGet(uint32 baseAddr)
{
    /* Read the counter value from TCRR */
    return (HW_RD_REG32(baseAddr + TIMER_TCRR));
}

/**
 * \brief   Get the match register contents.
 *
 * \param   baseAddr       Base Address of the Timer Module Register.
 *
 * \return  This API returns the match register contents.
 *
 **/
uint32 Gpt_gptimerReloadGet(uint32 baseAddr)
{
    /* Return the TLDR value */
    return (HW_RD_REG32(baseAddr + TIMER_TLDR));
}

/* ========================================================================== */
/*                 Internal Function Definitions                              */
/* ========================================================================== */

/**
 * \brief   This API will start the timer.
 *
 * \param   baseAddr       Base Address of the Timer Module Register.
 *
 * \return  None.
 *
 * \note    The timer must be configured before it is started/enabled.
 *
 **/
static void  Gpt_gptimerStartCount(uint32 baseAddr)
{
    /* Start the timer */
    HW_WR_FIELD32(
        baseAddr + TIMER_TCLR,
        TIMER_TCLR_ST,
        TIMER_TCLR_ST_ST_VALUE_1);
}

/**
 * \brief   This API will stop the timer.
 *
 * \param   baseAddr      Base Address of the Timer Module Register.
 *
 * \return  None.
 *
 **/
static void Gpt_gptimerStopCount(uint32 baseAddr)
{
    /* Stop the timer */
    HW_WR_FIELD32(
        baseAddr + TIMER_TCLR,
        TIMER_TCLR_ST,
        TIMER_TCLR_ST_ST_VALUE_0);
}

/**
 * \brief   This API will configure the timer in combinations of
 *          'One Shot timer' and 'Compare' Mode or 'Auto-reload timer'
 *          and 'Compare' Mode.
 *
 * \param   baseAddr      Base Address of the Timer Module Register.
 * \param   timerMode    Mode for enabling the timer.
 *
 * 'timerMode' can take the following values \n
 *    GPTIMER_ONESHOT_CMP_ENABLE - One shot and compare mode enabled \n
 *    TIMER_ONESHOT_NOCMP_ENABLE - One shot enabled, compare mode disabled \n
 *    GPTIMER_AUTORLD_CMP_ENABLE - Auto-reload and compare mode enabled \n
 *    TIMER_AUTORLD_NOCMP_ENABLE - Auto-reload enabled, compare mode
 *                                   disabled \n
 *
 * \return  None.
 *
 **/
static void Gpt_gptimerModeConfigure(uint32 baseAddr,
                                      uint32 timerMode)
{
    uint32 regVal;

    /* Clear the AR and CE field of TCLR */
    HW_WR_FIELD32(baseAddr + TIMER_TCLR,
                  TIMER_TCLR_AR,
                  TIMER_TCLR_AR_AR_VALUE_0);

    HW_WR_FIELD32(baseAddr + TIMER_TCLR,
                  TIMER_TCLR_CE,
                  TIMER_TCLR_CE_CE_VALUE_0);

    /* Set the timer mode in TCLR register */
    regVal = HW_RD_REG32(baseAddr + TIMER_TCLR);

    regVal |= (timerMode & (TIMER_TCLR_AR_MASK | TIMER_TCLR_CE_MASK));

    HW_WR_REG32(baseAddr + TIMER_TCLR, regVal);
}

/**
 * \brief   This API will configure and enable the pre-scaler clock.
 *
 * \param   baseAddr      Base Address of the Timer Module Register.
 * \param   ptv          Pre-scale clock Timer value.
 *
 * 'ptv' can take the following values \n
 *    GPTIMER_PRESCALER_CLK_DIV_BY_2 - Timer clock divide by 2 \n
 *    GPTIMER_PRESCALER_CLK_DIV_BY_4 - Timer clock divide by 4 \n
 *    GPTIMER_PRESCALER_CLK_DIV_BY_8 - Timer clock divide by 8 \n
 *    GPTIMER_PRESCALER_CLK_DIV_BY_16 - Timer clock divide by 16 \n
 *    GPTIMER_PRESCALER_CLK_DIV_BY_32 - Timer clock divide by 32 \n
 *    GPTIMER_PRESCALER_CLK_DIV_BY_64 - Timer clock divide by 64 \n
 *    GPTIMER_PRESCALER_CLK_DIV_BY_128 - Timer clock divide by 128 \n
 *    GPTIMER_PRESCALER_CLK_DIV_BY_256 - Timer clock divide by 256 \n
 *
 * \return  None.
 *
 **/
static void Gpt_gptimerPreScalerClkEnable(uint32 baseAddr,
                                           uint32 ptv)
{
    uint32 regVal;

    /* Clear the PTV field of TCLR */
    HW_WR_FIELD32(baseAddr + TIMER_TCLR, TIMER_TCLR_PTV, 0U);

    /* Set the PTV field and enable the pre-scaler clock */
    regVal = HW_RD_REG32(baseAddr + TIMER_TCLR);

    /* Enable Prescale mode */
    HW_SET_FIELD32(regVal, TIMER_TCLR_PRE, TIMER_TCLR_PRE_PRE_VALUE_1);

    /* Configure prescale value */
    HW_SET_FIELD32(regVal, TIMER_TCLR_PTV, ptv);

    HW_WR_REG32(baseAddr + TIMER_TCLR, regVal);
}

/**
 * \brief   This API will disable the pre-scaler clock.
 *
 * \param   baseAddr      Base Address of the Timer Module Register.
 *
 * \return  None.
 *
 **/
static void Gpt_gptimerPreScalerClkDisable(
    uint32 baseAddr)
{
    /* Disable Pre-scaler clock */
    HW_WR_FIELD32(baseAddr + TIMER_TCLR,
                  TIMER_TCLR_PRE,
                  TIMER_TCLR_PRE_PRE_VALUE_0);
}

/**
 * \brief   Set/Write the Counter register with the counter value.
 *
 * \param   baseAddr       Base Address of the Timer Module Register.
 * \param   counter       Count value for the timer.
 *
 * \return  None.
 *
 * \note    Value can be loaded into the counter register when the counter is
 *          stopped or when it is running.
 **/
static void Gpt_gptimerCounterSet(uint32 baseAddr,
                                   uint32 counter)
{
    /* Set the counter value */
    HW_WR_REG32(baseAddr + TIMER_TCRR, counter);
}

/**
 * \brief   Set the reload count value in the timer load register.
 *
 * \param   baseAddr       Base Address of the Timer Module Register.
 * \param   reload        The reload count value of the timer.
 *
 * \return  None.
 *
 * \note:   It is recommended to not use reload value as 0xFFFFFFFF as it can
 *          lead to undesired results.
 **/
static void Gpt_gptimerReloadSet(uint32 baseAddr,
                                  uint32 reload)
{
    /* Load the register with the re-load value */
    HW_WR_REG32(baseAddr + TIMER_TLDR, reload);
}

/**
 * \brief   Clear the status of interrupt events.
 *
 * \param   baseAddr       Base Address of the Timer Module Register.
 * \param   intFlags      Variable used to clear the events.
 *
 * 'intFlags' can take the following values \n
 *    TIMER_INT_TCAR_IT_FLAG - Clear IRQ status for capture \n
 *    TIMER_INT_OVF_IT_FLAG - Clear IRQ status for overflow \n
 *    TIMER_INT_MAT_IT_FLAG - Clear IRQ status for match \n
 *
 * \return  None.
 *
 **/
static void Gpt_gptimerIntStatusClear(uint32 baseAddr,
                                       uint32 intFlags)
{
    /* Clear the interrupt status from IRQSTATUS register */
    HW_WR_REG32(baseAddr + TIMER_IRQSTATUS,
                intFlags & (TIMER_IRQSTATUS_TCAR_IT_FLAG_MASK |
                            TIMER_IRQSTATUS_OVF_IT_FLAG_MASK |
                            TIMER_IRQSTATUS_MAT_IT_FLAG_MASK));
}

/**
 * \brief   Enable the Timer interrupts.
 *
 * \param   baseAddr       Base Address of the Timer Module Register.
 * \param   intFlags      Variable used to enable the interrupts.
 *
 * 'intFlags' can take the following values \n
 *    TIMER_INT_TCAR_EN_FLAG - IRQ enable for capture \n
 *    GPTIMER_INT_OVF_EN_FLAG  - IRQ enable for overflow \n
 *    TIMER_INT_MAT_EN_FLAG - IRQ enable for match \n
 *
 * \return  None.
 *
 **/
static void Gpt_gptimerIntEnable(uint32 baseAddr,
                                  uint32 intFlags)
{
    /* Enable the Timer interrupts represented by intFlags */
    HW_WR_REG32(baseAddr + TIMER_IRQENABLE_SET,
                intFlags & (TIMER_IRQENABLE_SET_TCAR_EN_FLAG_MASK |
                            TIMER_IRQENABLE_SET_OVF_EN_FLAG_MASK |
                            TIMER_IRQENABLE_SET_MAT_EN_FLAG_MASK));
}

/**
 * \brief   Disable the Timer interrupts.
 *
 * \param   baseAddr       Base Address of the Timer Module Register.
 * \param   intFlags      Variable used to disable the interrupts.
 *
 * 'intFlags' can take the following values \n
 *    TIMER_INT_TCAR_EN_FLAG - IRQ disable for capture \n
 *    GPTIMER_INT_OVF_EN_FLAG  - IRQ disable for overflow \n
 *    TIMER_INT_MAT_EN_FLAG - IRQ disable for match \n
 *
 * \return  None.
 *
 **/
static void Gpt_gptimerIntDisable(uint32 baseAddr,
                                   uint32 intFlags)
{
    /* Disable the Timer interrupts represented by intFlags */
    HW_WR_REG32(baseAddr + TIMER_IRQENABLE_CLR,
                intFlags & (TIMER_IRQENABLE_CLR_TCAR_EN_FLAG_MASK |
                            TIMER_IRQENABLE_CLR_OVF_EN_FLAG_MASK |
                            TIMER_IRQENABLE_CLR_MAT_EN_FLAG_MASK));
}

/**
 * \brief   Enable/Disable the Timer Wakeup.
 *
 * \param   baseAddr       Base Address of the Timer Module Register.
 * \param   wakeFlags     Variable used to enable the Wakeup events.
 *
 * 'intFlags' can take the following values
 *    TIMER_IRQWAKEEN_TCAR_WUP_ENA_MASK - Wakeup enable for capture
 *    TIMER_IRQWAKEEN_OVF_WUP_ENA_MASK - Wakeup enable for overflow
 *    TIMER_IRQWAKEEN_MAT_WUP_ENA_MASK - Wakeup enable for match
 *
 * \return  None.
 *
 **/
static void Gpt_gptimerWakeEnable(uint32 baseAddr,
                                   uint32 wakeFlags)
{
    uint32 regVal;

    regVal  = HW_RD_REG32(baseAddr + TIMER_IRQWAKEEN);
    regVal |= wakeFlags & (TIMER_IRQWAKEEN_TCAR_WUP_ENA_MASK |
                           TIMER_IRQWAKEEN_OVF_WUP_ENA_MASK |
                           TIMER_IRQWAKEEN_MAT_WUP_ENA_MASK);
    /* Enable the Timer Wakeup events represented by wakeFlags */
    HW_WR_REG32(baseAddr + TIMER_IRQWAKEEN, regVal);
}

/**
 * \brief   Disable the Timer Wakeup.
 *
 * \param   baseAddr       Base Address of the Timer Module Register.
 * \param   wakeFlags     Variable used to disable the Wakeup events.
 *
 * 'intFlags' can take the following values
 *    TIMER_IRQWAKEEN_TCAR_WUP_ENA_MASK - Wakeup disable for capture
 *    TIMER_IRQWAKEEN_OVF_WUP_ENA_MASK - Wakeup disable for overflow
 *    TIMER_IRQWAKEEN_MAT_WUP_ENA_MASK - Wakeup disable for match
 *
 * \return  None.
 *
 **/
static void Gpt_gptimerWakeDisable(uint32 baseAddr,
                                    uint32 wakeFlags)
{
    uint32 regVal, mask;

    mask = ~(wakeFlags & (TIMER_IRQWAKEEN_TCAR_WUP_ENA_MASK |
                          TIMER_IRQWAKEEN_OVF_WUP_ENA_MASK |
                          TIMER_IRQWAKEEN_MAT_WUP_ENA_MASK));
    regVal = HW_RD_REG32(baseAddr + TIMER_IRQWAKEEN) & mask;
    /* Disable the Timer Wakeup events represented by wakeFlags */
    HW_WR_REG32(baseAddr + TIMER_IRQWAKEEN, regVal);
}

/**
 * \brief   Enable/Disable software reset for Timer module.
 *
 * \param   baseAddr       Base Address of the Timer Module Register.
 * \param   rstOption     Enable/Disable reset option for TIMER.
 *
 * 'rstOption' can take the following values \n
 *    GPTIMER_SFT_RESET_ENABLE - Software reset is enabled \n
 *    GPTIMER_SFT_RESET_DISABLE - Software reset is disabled \n
 *
 * \return  None.
 *
 **/
static void Gpt_gptimerResetConfigure(uint32 baseAddr,
                                       uint32 rstOption)
{
    /* Write the option sent by user to SFT field of TSICR */
    HW_WR_FIELD32(baseAddr + TIMER_TSICR,
                  TIMER_TSICR_SFT,
                  rstOption >> TIMER_TSICR_SFT_SHIFT);
}

/**
 * \brief   Reset the Timer module.
 *
 * \param   baseAddr       Base Address of the Timer Module Register.
 *
 * \return  None.
 *
 **/
static void Gpt_gptimerReset(uint32 baseAddr)
{
    StatusType status;
    TickType   startCount = 0U, tempCount = 0U, elaspsedCount = 0U;
    /* Reset the Timer module */
    HW_WR_FIELD32(baseAddr + TIMER_TIOCP_CFG,
                  TIMER_TIOCP_CFG_SOFTRESET,
                  TIMER_TIOCP_CFG_SOFTRESET_SOFTRESET_VALUE_1);

    status = GetCounterValue(GPT_OS_COUNTER_ID, &startCount);
    if (((StatusType) E_OK) == status)
    {
        /* Stay in the loop until reset is done. */
        do
        {
            /* Below API can change start time, so use temp variable */
            tempCount = startCount;
            status    = GetElapsedValue(
                GPT_OS_COUNTER_ID,
                &tempCount,
                &elaspsedCount);
            if ((((StatusType) E_OK) != status) ||
                (elaspsedCount >= GPT_TIMEOUT_DURATION))
            {
#if (GPT_E_HARDWARE_ERROR != GPT_DEM_NO_EVENT)
                (void)Dem_SetEventStatus(
                    GPT_E_HARDWARE_ERROR, DEM_EVENT_STATUS_FAILED);
#endif
                break;
            }

        } while (HW_RD_FIELD32(baseAddr + TIMER_TIOCP_CFG,
                               TIMER_TIOCP_CFG_SOFTRESET) ==
                 (uint32) TIMER_TIOCP_CFG_SOFTRESET_SOFTRESET_VALUE_1);
         /* GPT_CoverageGap_01: Dynamic branch coverage for True condition of while statement is not covered
          * because the soft reset ends before checking while condition.
          */
    }
}

/**
 * \brief   Configure the posted mode of TIMER.
 *
 * \param   baseAddr       Base Address of the Timer Module Register.
 * \param   postMode      Posted mode configuration.
 *
 * 'postMode' can take the following values \n
 *    GPTIMER_NONPOSTED - Posted mode not used \n
 *    GPTIMER_POSTED - Posted mode used \n
 *
 * \return  None.
 *
 **/
static void Gpt_gptimerPostedModeConfig(uint32 baseAddr,
                                         uint32 postMode)
{
    /* Write to the POSTED field of TSICR */
    HW_WR_FIELD32(baseAddr + TIMER_TSICR,
                  TIMER_TSICR_POSTED,
                  postMode >> TIMER_TSICR_POSTED_SHIFT);
}

/**
 * \brief   Configure the timer status in emulation mode.
 *
 * \param   baseAddr         Base Address of the Timer Module Register.
 *
 * \param   emuModeOption   Emulation mode configuration.
 *
 * 'emuModeOption' can take the following values \n
 *    TIMER_FROZEN - Timer is frozen in emuMode \n
 *    TIMER_FREE - Timer is free in emuMode \n
 *
 * \return  None.
 *
 **/
static void Gpt_gptimerEmuModeConfigure(
    uint32 baseAddr, uint32 emuModeOption)
{
    /* Write to the EMUFREE field of TIOCP_CFG */
    HW_WR_FIELD32(baseAddr + TIMER_TIOCP_CFG,
                  TIMER_TIOCP_CFG_EMUFREE,
                  emuModeOption);
}

static void Gpt_gptimerCounterInit(
    const Gpt_ChannelHwObjType *pHwUnitObj, uint32 baseAddr)
{
    Gpt_gptimerCounterSet(
        baseAddr,
        (GPTIMER_MAX_RESOLUTION -
         pHwUnitObj->channelCfg.tickValueMax));
    Gpt_gptimerReloadSet(baseAddr,
                         (GPTIMER_MAX_RESOLUTION -
                          pHwUnitObj->channelCfg.tickValueMax));


    /*
     * Design : MCAL-6350
     */
    Gpt_gptimerIntDisable(baseAddr, GPTIMER_INT_ALL_EN_FLAG);

    /*
     * Design : MCAL-6326
     */
    Gpt_gptimerWakeDisable(baseAddr, GPTIMER_WAKE_ALL_EN_FLAG);

    if ((uint32)GPT_PRESCALER_NO_PRESCALE != pHwUnitObj->channelCfg.prescale)
    {
        Gpt_gptimerPreScalerClkEnable(baseAddr,
                                      pHwUnitObj->channelCfg.prescale);
    }
    else
    {
        Gpt_gptimerPreScalerClkDisable(baseAddr);
    }

    return;
}

#if (STD_ON == GPT_DEV_ERROR_DETECT)
void Gpt_reportDetError(uint8 apiId, uint8 errorId)
{
    (void) Det_ReportError(GPT_MODULE_ID, GPT_INSTANCE_ID, apiId, errorId);
    return;
}

#endif  /* #if (STD_ON == GPT_DEV_ERROR_DETECT) */

void Gpt_reportRuntimeError(uint8 apiId, uint8 errorId)
{
    (void) Det_ReportRuntimeError(GPT_MODULE_ID, GPT_INSTANCE_ID, apiId, errorId);
    return;
}

#define GPT_STOP_SEC_CODE
#include "Gpt_MemMap.h"

#define GPT_START_SEC_ISR_CODE
#include "Gpt_MemMap.h"

void Gpt_gptimerIntrClear(Gpt_ChannelType gptChannel)
{
    uint32 baseAddr = Gpt_TimerBaseAddr[gptChannel];

    Gpt_gptimerIntStatusClear(baseAddr, GPTIMER_INT_OVF_IT_FLAG);
}

void Gpt_gptimerIntDisableCh(Gpt_ChannelType gptChannel)
{
    uint32 baseAddr = Gpt_TimerBaseAddr[gptChannel];

    Gpt_gptimerDisableNotifn(baseAddr);
}

#define GPT_STOP_SEC_ISR_CODE
#include "Gpt_MemMap.h"
/* eof */
