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

#include <stdlib.h>
#include <stdbool.h>
/*=== SDK/PDK includes ===*/
#include <drivers/hw_include/cslr_soc.h>
#include <drivers/sciclient.h>
#include <drivers/pinmux.h>
#include <kernel/dpl/DebugP.h>
#include <kernel/dpl/SemaphoreP.h>
#include <kernel/dpl/HwiP.h>
#include <kernel/dpl/ClockP.h>
#include <kernel/dpl/CacheP.h>
#include <kernel/nortos/dpl/r5/HwiP_armv7r_vim.h>
/*=== Generated DPL includes ===*/
#include <ti_drivers_config.h>
#include <ti_board_config.h>
#include <ti_drivers_open_close.h>
#include <ti_board_open_close.h>
/*=== AUTOSAR Standard includes ===*/
#include <Std_Types.h>
#include "board_utils.h"
/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief MMR0 ENET1 control register address */
#define CTRL_MMR0_CFG0_ENET1_CTRL           (0x000104044U)

/** \brief MMR0 ENET2 control register address */
#define CTRL_MMR0_CFG0_ENET2_CTRL           (0x000104048U)

/* \brief ENET offset value for enet control register */
#define CTRL_MMR0_CFG0_ENET_OFFSET(macPort) (((macPort)-1)*0x4u)

/** \brief LOCK1_KICK0 register address  */
#define CFG0_CTRL_MMR_LOCK1_KICK0           (0x000105008U)

/** \brief LOCK1_KICK1 register address  */
#define CFG0_CTRL_MMR_LOCK1_KICK1           (0x00010500CU)

/** \brief CPSW (CPTS) Clock selection register */
#define CFG0_CTRL_MMR_CPSW_CLKSEL_REG       (0x00108140U)

/** \brief CPSW (CPTS) Clock selection value to be set
 *   Valid Values:
 *   - 3'b000 -> MAIN_PLL2_HSDIV5_CLKOUT
 *   - 3'b001 -> MAIN_PLL0_HSDIV6_CLKOUT
 *   - 3'b010 -> CP_GEMAC_CPTS0_RFT_CLK (Pin)
 *   - 3'b011 -> Reserved
 *   - 3'b100 -> MCU_EXT_REFCLK0 (Pin)
 *   - 3'b101 -> EXT_REFCLK1 (Pin)
 *   - 3'b110 -> MCU_SYSCLK0
 *   - 3'b111 -> MAIN_SYSCLK0
 */
#define MAIN_SYSCLK0                        (7u)

/** \brief Unlock value for lock kick 0 */
#define MMR_KICK0_UNLOCK_VAL                (0x68EF3490U)

/** \brief Unlock value for lock kick 1 */
#define MMR_KICK1_UNLOCK_VAL                (0xD172BC5AU)

/* ========================================================================== */
/*                           External Variables                               */
/* ========================================================================== */

extern ClockP_Config gClockConfig;

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

static Pinmux_PerCfg_t MDIOPinMuxMainDomainCfg[] = {
    /* RGMII_MDIO -> MDIO0_MDC -> AD24 */
    {
        PIN_MDIO0_MDC, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE) & (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE))
    },
    /* RGMII_MDIO -> MDIO0_MDIO -> AB22 */
    {
        PIN_MDIO0_MDIO, PIN_MODE(0) | \
        ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) & (~PIN_PULL_DIRECTION))
    },
    {PINMUX_END, PINMUX_END}
};

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/*
 * Board info
 */
void BoardUtils_cpswMuxSel(void)
{
    /* MDIO0 pin config */
    Pinmux_config(MDIOPinMuxMainDomainCfg, PINMUX_DOMAIN_ID_MAIN);
    return;
}

/*
 * borrow CPTS setup code from ENET driver
 */
void BoardUtils_setDeviceState(uint32_t moduleId,
                               uint32_t requiredState,
                               uint32_t appFlags)
{
    int32_t status;
    uint32_t moduleState      = 0U;
    uint32_t resetState       = 0U;
    uint32_t contextLossState = 0U;
    Bool turnOn;
    Bool turnOff;

    status = Sciclient_pmGetModuleState(moduleId,
                                        &moduleState,
                                        &resetState,
                                        &contextLossState,
                                        SystemP_WAIT_FOREVER);

    turnOn = (moduleState == TISCI_MSG_VALUE_DEVICE_HW_STATE_OFF) &&
             (requiredState == TISCI_MSG_VALUE_DEVICE_SW_STATE_ON);
    turnOff = (moduleState == TISCI_MSG_VALUE_DEVICE_HW_STATE_ON) &&
              (requiredState == TISCI_MSG_VALUE_DEVICE_SW_STATE_AUTO_OFF);

    if (turnOn || turnOff)
    {
        status = Sciclient_pmSetModuleState(moduleId,
                                            requiredState,
                                            (appFlags |
                                             TISCI_MSG_FLAG_AOP |
                                             TISCI_MSG_FLAG_DEVICE_RESET_ISO),
                                             SystemP_WAIT_FOREVER);
        if (requiredState == TISCI_MSG_VALUE_DEVICE_SW_STATE_ON)
        {
            /* Reset if changed state to enabled */
            status = Sciclient_pmSetModuleRst(moduleId,
                                              0x0U /*resetBit*/,
                                              SystemP_WAIT_FOREVER);
            (void)status;
        }
    }
}

void BoardUtils_selectCptsClock()
{
    volatile uint32_t *reg = (uint32_t*)CFG0_CTRL_MMR_CPSW_CLKSEL_REG;
    *reg = MAIN_SYSCLK0;
}

void BoardUtils_enableClocks()
{
    uint32_t moduleId = TISCI_DEV_CPSW0;
    uint32_t appFlags = 0U;

    BoardUtils_setDeviceState(moduleId, TISCI_MSG_VALUE_DEVICE_SW_STATE_ON, appFlags);
    BoardUtils_selectCptsClock();
}

void BoardUtils_setEnetCtrl(uint32 macPortNum, uint32 val)
{
    uint32 *enetRegs = (uint32*)(CTRL_MMR0_CFG0_ENET1_CTRL +
                                 CTRL_MMR0_CFG0_ENET_OFFSET(macPortNum));

    CSL_REG32_WR(CFG0_CTRL_MMR_LOCK1_KICK0, MMR_KICK0_UNLOCK_VAL);
    CSL_REG32_WR(CFG0_CTRL_MMR_LOCK1_KICK1, MMR_KICK1_UNLOCK_VAL);
    CSL_REG32_WR(enetRegs, val);
}

void BoardUtils_setClockPIsrPrio(uint32 priority)
{
    HwiP_setPri(gClockConfig.timerHwiIntNum, priority);
}

uint32 BoardUtils_getMdioIntrNum(void)
{
    return CSLR_WKUP_R5FSS0_CORE0_INTR_CPSW0_MDIO_PEND_0;
}

Std_ReturnType BoardUtils_registerInterrupt(uint32 intrNum, void (*func)(void*))
{
    HwiP_FxnCallback isrHandler = (HwiP_FxnCallback)func;
    HwiP_Object hwiPObject;
    HwiP_Params hwiParams;

    HwiP_Params_init(&hwiParams);
    hwiParams.intNum = intrNum;
    hwiParams.callback = isrHandler;
    hwiParams.isPulse = 0;
    hwiParams.priority = 1u;
    HwiP_construct(&hwiPObject, &hwiParams);
    return E_OK;
}

void BoardUtils_platformInit(void)
{
    /* init SOC specific modules */
    System_init();
    Board_init();

    Drivers_open();
    Board_driversOpen();

    BoardUtils_cpswMuxSel();
    BoardUtils_enableClocks();
    return;
}

void BoardUtils_platformDeInit(void)
{
    Board_deinit();
    System_deinit();
}

