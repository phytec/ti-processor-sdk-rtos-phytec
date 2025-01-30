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
 *  \file     IcuApp_Startup.c
 *
 *  \brief    This file contains the ICU test example
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "IcuApp.h"
#include "IcuApp_Startup.h"
#include <hw_include/lld_types.h>
/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define MSG_NORMAL      (APP_UTILS_PRINT_MSG_NORMAL)
/**< Message type */
#define MSG_STATUS      (APP_UTILS_PRINT_MSG_STATUS)
/**< Message type */

#define KICK0_UNLOCK_VAL 0x68EF3490
#define KICK1_UNLOCK_VAL 0xD172BC5A
#define ICU_SCICLIENT_TIMEOUT       0xffffffffu

#define APP_MAIN_CTRL_MMR_CFG0_EPWM0_CTRL_TB_CLKEN_EN (0x1U)
#define APP_MAIN_CTRL_MMR_CFG0_EPWM1_CTRL_TB_CLKEN_EN (0x1U)
#define MMR_KEY0_UNLOCK     (0x68EF3490U)
#define MMR_KEY1_UNLOCK     (0xD172BC5AU)

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

static void IcuApp_ClockEnable(void);
static void IcuApp_BoardInit(void);
static void IcuApp_IntrConfig(void);

#if (STD_ON == PWM_NOTIFICATION_SUPPORTED)
static void PwmApp_InterruptConfig(void);
#endif

#if (PWM_INDEX == 0U) /*EPWM*/
static void Pwm_ClockEnable_MmrUnlock(void);
static void Pwm_PinMux(uint32 offset, uint32 muxmode);
static void PwmApp_unlockMmrRegisters(uint32 lockKey0, uint32 lockKey1);
#endif /*EPWM*/


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/** \brief Start up sequence : Program the interrupt muxes / priorities */
void IcuApp_Startup(void)
{
    IcuApp_BoardInit();

    /* Unlock Main Control MMR */
    if ((CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_LOCK1_KICK0) & 0x1U) != 0x1U)
    {
        CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_LOCK0_KICK0, KICK0_UNLOCK_VAL);
        CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_LOCK0_KICK1, KICK1_UNLOCK_VAL);

    }
    while ((CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_LOCK1_KICK0) & 0x1U) != 0x1U)
    {
        /* Wait till the MMR is unlocked. */
    }

    /* Configure PADCONFIG 58 to mux mode 1 for ECAP2_IN_APWM_OUT and input pin. */
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_PADCONFIG58, 0x08050001);

    IcuApp_ClockEnable();

    IcuApp_IntrConfig();
}

void PwmApp_Startup(void)
{
#if (STD_ON == PWM_NOTIFICATION_SUPPORTED)
        PwmApp_InterruptConfig();
#endif

#if (PWM_INDEX == 0U) /*EPWM*/
    Pwm_ClockEnable_MmrUnlock();

    Pwm_PinMux(CSL_MAIN_CTRL_MMR_CFG0_PADCONFIG12, 0x08050009);/*EHRPWM1_A*/
    Pwm_PinMux(CSL_MAIN_CTRL_MMR_CFG0_PADCONFIG17, 0x08050009);/*EHRPWM1_B*/
#endif /*EPWM*/

    /* OSAL being generic might not record the correct frequency and hence we
     * are setting it explicitly. Osal_delay function uses PMU counter which
     * runs at CPU speed and uses OSAL_CPU_FREQ_KHZ_DEFAULT to get the CPU
     * frequency. By default OSAL_CPU_FREQ_KHZ_DEFAULT is set to 400MHZ.
	 * In case of J721E this parameter needs to be set explicitly to 1GHZ
     * to get the correct delay functionality.
     */
	AppUtils_SetCpuFrequency();
}

static void IcuApp_ClockEnable(void)
{
    sint32 retVal;
    /* Mcu module, when included will replace this operation */
    retVal = AppUtils_ModuleClockEnable(TISCI_DEV_ECAP2);
     if(retVal != 0)
    {
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                          ": Error could not enable module !!!\n");
    }
    return;
}

/* Initialize board */
static void IcuApp_BoardInit(void)
{
    Board_initCfg boardCfg;

    boardCfg = BOARD_INIT_UNLOCK_MMR |
               BOARD_INIT_MODULE_CLOCK |
               BOARD_INIT_PINMUX_CONFIG;
            
    Board_init(boardCfg);
}

/*Interrupt Configuraiton*/
static void IcuApp_IntrConfig(void)
{
    OsalRegisterIntrParams_t    intrPrms;
    OsalInterruptRetCode_e      osalRetVal;
    HwiP_Handle hwiHandle;

    /* We will have to use the routed interrupt number
    subsequently */
    Osal_RegisterInterrupt_initParams(&intrPrms);

    intrPrms.corepacConfig.arg          = (uint32_t)NULL_PTR;
    intrPrms.corepacConfig.isrRoutine   = (void (*)(uintptr_t))Icu_ch2Notify;
    intrPrms.corepacConfig.priority     = 1U;
    intrPrms.corepacConfig.corepacEventNum = 0U; /* NOT USED  */
    intrPrms.corepacConfig.intVecNum = CSLR_R5FSS0_CORE1_INTR_ECAP2_ECAP_INT_0;// intNum;
    osalRetVal = Osal_RegisterInterrupt(&intrPrms, &hwiHandle);
    if(OSAL_INT_SUCCESS != osalRetVal)
    {
        AppUtils_Printf(MSG_NORMAL,APP_NAME
        ": Error in Osal Interrupt Registration Configuration!!!\n");
    }

}
/* ========================================================================== */
/*                        Application Local Functions                         */
/* ========================================================================== */
#if (STD_ON == PWM_NOTIFICATION_SUPPORTED)
static void PwmApp_InterruptConfig(void)
{
    /*interrupt config*/
    OsalRegisterIntrParams_t    intrPrms;
    OsalInterruptRetCode_e      osalRetVal;
    HwiP_Handle hwiHandle;

#if (PWM_INDEX == 0U) /*EPWM*/
    /* We will have to use the routed interrupt number
    subsequently */
    Osal_RegisterInterrupt_initParams(&intrPrms);

    intrPrms.corepacConfig.arg          = (uint32)NULL_PTR;
    intrPrms.corepacConfig.isrRoutine   = (void (*)(uintptr_t))Pwm_Ch2Notify;
    intrPrms.corepacConfig.priority     = 1U;
    intrPrms.corepacConfig.corepacEventNum = 0U; /* NOT USED  */
    intrPrms.corepacConfig.intVecNum = CSLR_R5FSS0_CORE1_INTR_EHRPWM1_EPWM_ETINT_0;
    osalRetVal = Osal_RegisterInterrupt(&intrPrms, &hwiHandle);
    if(OSAL_INT_SUCCESS != osalRetVal)
    {
        AppUtils_Printf(MSG_NORMAL,APP_NAME
            ": Error in OSAL Interrupt Registration !!!\n");
    }
#endif /*EPWM*/

}
#endif

#if (PWM_INDEX == 0U)/*EPWM*/
static void Pwm_ClockEnable_MmrUnlock(void)
{
    /* write Partition 0 Register */
    PwmApp_unlockMmrRegisters(CSL_MAIN_CTRL_MMR_CFG0_LOCK0_KICK0, CSL_MAIN_CTRL_MMR_CFG0_LOCK0_KICK1);
    /* write Partition 1 Register */
    PwmApp_unlockMmrRegisters(CSL_MAIN_CTRL_MMR_CFG0_LOCK1_KICK0, CSL_MAIN_CTRL_MMR_CFG0_LOCK1_KICK1);
    /* write Partition 2 Register */
    PwmApp_unlockMmrRegisters(CSL_MAIN_CTRL_MMR_CFG0_LOCK2_KICK0, CSL_MAIN_CTRL_MMR_CFG0_LOCK2_KICK1);
    /* write Partition 3 Register */
    PwmApp_unlockMmrRegisters(CSL_MAIN_CTRL_MMR_CFG0_LOCK3_KICK0, CSL_MAIN_CTRL_MMR_CFG0_LOCK3_KICK1);
    /* write Partition 4 Register */
    PwmApp_unlockMmrRegisters(CSL_MAIN_CTRL_MMR_CFG0_LOCK4_KICK0, CSL_MAIN_CTRL_MMR_CFG0_LOCK4_KICK1);
    /* write Partition 7 Register */
    PwmApp_unlockMmrRegisters(CSL_MAIN_CTRL_MMR_CFG0_LOCK7_KICK0, CSL_MAIN_CTRL_MMR_CFG0_LOCK7_KICK1);
    /*write 0x08050006 to LOCK 7*/

    /*enable Clock*/
    HW_WR_FIELD32(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_EPWM1_CTRL,
        CSL_MAIN_CTRL_MMR_CFG0_EPWM1_CTRL_TB_CLKEN,
        APP_MAIN_CTRL_MMR_CFG0_EPWM1_CTRL_TB_CLKEN_EN);

    /*enable eallow*/
    HW_WR_FIELD32(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_EPWM1_CTRL,
        CSL_MAIN_CTRL_MMR_CFG0_EPWM1_CTRL_EALLOW,
        1U);

}

static void PwmApp_unlockMmrRegisters(uint32 lockKey0, uint32 lockKey1)
{
    uint32 regVal = 0U;
    /* write Partition Lock Key 0 Register */
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + lockKey0, MMR_KEY0_UNLOCK);
    /* write Partition Lock Key 1 Register */
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + lockKey1, MMR_KEY1_UNLOCK);
    /* Check for unlock */
    regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + lockKey0);
    while ((regVal & 0x1) != 0x1U)
    {
        regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + lockKey0);
    }

    return;
}

static void Pwm_PinMux(uint32 offset, uint32 muxmode)
{
    uint32 regVal = 0U;
    regVal |= muxmode;
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + offset, regVal);
    return;
}

/** \brief None, SBL/GEL powers up the timers and clock sources */
void PwmApp_PowerAndClkSrc(void)
{
    sint32 retVal;
    /* Mcu module, when included will replace this operation */
    retVal = AppUtils_ModuleClockEnable(TISCI_DEV_EHRPWM1);
     if(retVal != 0)
    {
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                          ": Error could not enable module !!!\n");
    }
    return;
}
#endif /*EPWM*/

/*EoF*/
