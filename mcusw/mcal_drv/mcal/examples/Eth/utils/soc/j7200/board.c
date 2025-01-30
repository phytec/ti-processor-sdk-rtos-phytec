/*
 *  Copyright (C) 2024 Texas Instruments Incorporated
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
 */

/**
 *  \file     board.c
 *
 *  \brief    This file contains the board specific implementations
 *            needed by the Eth application.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
/*=== SDK/PDK includes ===*/
#include <ti/csl/arch/csl_arch.h>
#include <ti/csl/soc.h>
#include <ti/csl/hw_types.h>
#include <ti/osal/osal.h>
/*=== AUTOSAR Standard includes ===*/
#include <Std_Types.h>
#include <app_utils.h>
#include "board_utils.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Pinmux setup macros */
#define PIN_MODE(mode)                      ((mode) & 0xFU)
#define PIN_PULL_DISABLE                    (0x1U << 16U)
#define PIN_PULL_DIRECTION                  (0x1U << 17U)
#define PIN_INPUT_ENABLE                    (0x1U << 18U)
#define PIN_OUTPUT_DISABLE                  (0x1U << 21U)
#define PIN_WAKEUP_ENABLE                   (0x1U << 29U)
#define PIN_INPUT                           (PIN_PULL_DISABLE | PIN_INPUT_ENABLE)
#define PIN_OUTPUT                          (PIN_PULL_DISABLE)

/** \brief Unlock value for lock kick 0 */
#define MMR_KICK0_UNLOCK_VAL                (0x68EF3490U)

/** \brief Unlock value for lock kick 1 */
#define MMR_KICK1_UNLOCK_VAL                (0xD172BC5AU)

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

void BoardUtils_setCpsw0Pinmux(void)
{
    CSL_wkup_ctrl_mmr_cfg0Regs *regs =
        (CSL_wkup_ctrl_mmr_cfg0Regs *)(uintptr_t)CSL_WKUP_CTRL_MMR0_CFG0_BASE;

    CSL_REG32_WR(&regs->LOCK2_KICK0, MMR_KICK0_UNLOCK_VAL);
    CSL_REG32_WR(&regs->LOCK2_KICK1, MMR_KICK1_UNLOCK_VAL);
    CSL_REG32_WR(&regs->LOCK7_KICK0, MMR_KICK0_UNLOCK_VAL);
    CSL_REG32_WR(&regs->LOCK7_KICK1, MMR_KICK1_UNLOCK_VAL);

    /* MCU_CPSW -> MCU_RGMII1_TX_CTL -> X (J7200) */
    CSL_REG32_WR(&regs->PADCONFIG26, PIN_MODE(0U) | PIN_OUTPUT);

    /* MCU_CPSW -> MCU_RGMII1_RX_CTL -> X (J7200) */
    CSL_REG32_WR(&regs->PADCONFIG27, PIN_MODE(0U) | PIN_INPUT);

    /* MCU_CPSW -> MCU_RGMII1_TD3 -> X (J7200) */
    CSL_REG32_WR(&regs->PADCONFIG28, PIN_MODE(0U) | PIN_OUTPUT);

    /* MCU_CPSW -> MCU_RGMII1_TD2 -> X (J7200) */
    CSL_REG32_WR(&regs->PADCONFIG29, PIN_MODE(0U) | PIN_OUTPUT);

    /* MCU_CPSW -> MCU_RGMII1_TD1 -> X (J7200) */
    CSL_REG32_WR(&regs->PADCONFIG30, PIN_MODE(0U) | PIN_OUTPUT);

    /* MCU_CPSW -> MCU_RGMII1_TD0 -> X (J7200) */
    CSL_REG32_WR(&regs->PADCONFIG31, PIN_MODE(0U) | PIN_OUTPUT);

    /* MCU_CPSW -> MCU_RGMII1_TXC -> X (J7200) */
    CSL_REG32_WR(&regs->PADCONFIG32, PIN_MODE(0U) | PIN_OUTPUT);

    /* MCU_CPSW -> MCU_RGMII1_RXC -> X (J7200) */
    CSL_REG32_WR(&regs->PADCONFIG33, PIN_MODE(0U) | PIN_INPUT);

    /* MCU_CPSW -> MCU_RGMII1_RD3 -> X (J7200) */
    CSL_REG32_WR(&regs->PADCONFIG34, PIN_MODE(0U) | PIN_INPUT);

    /* MCU_CPSW -> MCU_RGMII1_RD2 -> X (J7200) */
    CSL_REG32_WR(&regs->PADCONFIG35, PIN_MODE(0U) | PIN_INPUT);

    /* MCU_CPSW -> MCU_RGMII1_RD1 -> X (J7200) */
    CSL_REG32_WR(&regs->PADCONFIG36, PIN_MODE(0U) | PIN_INPUT);

    /* MCU_CPSW -> MCU_RGMII1_RD0 -> X (J7200) */
    CSL_REG32_WR(&regs->PADCONFIG37, PIN_MODE(0U) | PIN_INPUT);

    /* MCU_MDIO0 -> MCU_MDIO0_MDIO -> X (J7200) */
    CSL_REG32_WR(&regs->PADCONFIG38, PIN_MODE(0U) | PIN_INPUT);

    /* MCU_MDIO0 -> MCU_MDIO0_MDC -> X (J7200) */
    CSL_REG32_WR(&regs->PADCONFIG39, PIN_MODE(0U) | PIN_OUTPUT);

    /* WKUP_GPIO0 -> WKUP_GPIO0_3 -> X (J7200) */
    CSL_REG32_WR(&regs->PADCONFIG51, PIN_MODE(7U) | PIN_INPUT);
}

void BoardUtils_setEnetCtrl(uint32 macPortNum, uint32 val)
{
    CSL_mcu_ctrl_mmr_cfg0Regs *regs =
        (CSL_mcu_ctrl_mmr_cfg0Regs *)(uintptr_t)CSL_MCU_CTRL_MMR0_CFG0_BASE;

    CSL_REG32_WR(&regs->LOCK1_KICK0, MMR_KICK0_UNLOCK_VAL);
    CSL_REG32_WR(&regs->LOCK1_KICK1, MMR_KICK1_UNLOCK_VAL);
    CSL_REG32_WR(&regs->MCU_ENET_CTRL, val);
}

void BoardUtils_setClockPIsrPrio(uint32 priority)
{
    /* Do Nothing - Not needed on Jacinto Boards */
}

uint32 BoardUtils_getMdioIntrNum(void)
{
    return CSLR_MCU_R5FSS0_CORE0_INTR_MCU_CPSW0_MDIO_PEND_0;
}

Std_ReturnType BoardUtils_registerInterrupt(uint32 intrNum, void (*func)(void*))
{
    Osal_IsrRoutine isrHandler = (Osal_IsrRoutine)func;
    OsalRegisterIntrParams_t intrPrms;
    OsalInterruptRetCode_e oSalRet = OSAL_INT_SUCCESS;
    Std_ReturnType retVal = E_OK;
    HwiP_Handle hwiHandle;

    Osal_RegisterInterrupt_initParams(&intrPrms);
    intrPrms.corepacConfig.arg             = (uintptr_t)NULL_PTR;
    intrPrms.corepacConfig.isrRoutine      = isrHandler;
    intrPrms.corepacConfig.priority        = 1U;
    intrPrms.corepacConfig.corepacEventNum = 0U; /* not used? */
    intrPrms.corepacConfig.intVecNum       = intrNum;

    oSalRet = Osal_RegisterInterrupt(&intrPrms, &hwiHandle);
    if (OSAL_INT_SUCCESS != oSalRet)
    {
        retVal = E_NOT_OK;
    }

    return retVal;
}

void BoardUtils_platformInit(void)
{
#ifdef UART_ENABLED
    AppUtils_Init();
#endif
#if defined(BUILD_MCU1_0)
    BoardUtils_setCpsw0Pinmux();
#endif
    /* Equivalent to EcuM_AL_SetProgrammableInterrupts */
    Intc_Init();
    Intc_SystemEnable();
    return;
}

void BoardUtils_platformDeInit(void)
{
    /* Do Nothing */
}

