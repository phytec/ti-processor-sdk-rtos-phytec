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

/*
 * Auto generated file - DO NOT MODIFY
 */
#include "ti_drivers_config.h"
#include <drivers/pinmux/am62x/pinmux.h>

static Pinmux_PerCfg_t gPinMuxMainDomainCfg[] = {
                            /* SPI0 pin config */
    /* SPI0_CLK -> SPI0_CLK (A14) */
    {
        PIN_SPI0_CLK,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* SPI0_D0 -> SPI0_D0 (B13) */
    {
        PIN_SPI0_D0,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* SPI0_D1 -> SPI0_D1 (B14) */
    {
        PIN_SPI0_D1,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
            /* SPI1 pin config */
    /* SPI1_CLK -> MMC0_DAT2 (AA3) */
    {
        PIN_MMC0_DAT2,
        ( PIN_MODE(5) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* SPI1_D0 -> MMC0_DAT7 (AC2) */
    {
        PIN_MMC0_DAT7,
        ( PIN_MODE(5) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* SPI1_D1 -> MMC0_DAT6 (AD2) */
    {
        PIN_MMC0_DAT6,
        ( PIN_MODE(5) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
            /* SPI2 pin config */
    /* SPI2_CLK -> MMC0_DAT0 (AA2) */
    {
        PIN_MMC0_DAT0,
        ( PIN_MODE(6) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* SPI2_D0 -> MMC0_DAT5 (AC1) */
    {
        PIN_MMC0_DAT5,
        ( PIN_MODE(6) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* SPI2_D1 -> MMC0_DAT4 (AB2) */
    {
        PIN_MMC0_DAT4,
        ( PIN_MODE(6) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },

                            /* SPI0_CS0 pin config */
    /* SPI0_CS0 -> SPI0_CS0 (A13) */
    {
        PIN_SPI0_CS0,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
            /* SPI1_CS0 pin config */
    /* SPI1_CS0 -> MMC0_DAT3 (Y4) */
    {
        PIN_MMC0_DAT3,
        ( PIN_MODE(5) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
            /* SPI2_CS0 pin config */
    /* SPI2_CS0 -> MMC0_DAT1 (AA1) */
    {
        PIN_MMC0_DAT1,
        ( PIN_MODE(6) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },

    {PINMUX_END, PINMUX_END}
};

static Pinmux_PerCfg_t gPinMuxMcuDomainCfg[] = {
	 /* MCU_SPI0 pin config */
    /* MCU_SPI0_CLK -> MCU_SPI0_CLK (A7) */
    {
        PIN_MCU_SPI0_CLK,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* MCU_SPI0_D0 -> MCU_SPI0_D0 (D9) */
    {
        PIN_MCU_SPI0_D0,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* MCU_SPI0_D1 -> MCU_SPI0_D1 (C9) */
    {
        PIN_MCU_SPI0_D1,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
            /* MCU_SPI1 pin config */
    /* MCU_SPI1_CLK -> WKUP_UART0_RTSn (A4) */
    {
        PIN_WKUP_UART0_RTSN,
        ( PIN_MODE(3) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* MCU_SPI1_D0 -> MCU_UART0_CTSn (A6) */
    {
        PIN_MCU_UART0_CTSN,
        ( PIN_MODE(3) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* MCU_SPI1_D1 -> MCU_UART0_RTSn (B6) */
    {
        PIN_MCU_UART0_RTSN,
        ( PIN_MODE(3) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },

            /* MCU_SPI0_CS0 pin config */
    /* MCU_SPI0_CS0 -> MCU_SPI0_CS0 (E8) */
    {
        PIN_MCU_SPI0_CS0,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
            /* MCU_SPI1_CS0 pin config */
    /* MCU_SPI1_CS0 -> WKUP_UART0_CTSn (C6) */
    {
        PIN_WKUP_UART0_CTSN,
        ( PIN_MODE(3) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },

    {PINMUX_END, PINMUX_END}
};

/*
 * Pinmux
 */
void Pinmux_init(void)
{
    Pinmux_config(gPinMuxMainDomainCfg, PINMUX_DOMAIN_ID_MAIN);
    Pinmux_config(gPinMuxMcuDomainCfg, PINMUX_DOMAIN_ID_MCU);
}

