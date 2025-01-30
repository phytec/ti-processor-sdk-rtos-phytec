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
 *  \file     app_utils.c
 *
 *  \brief    Implements the common utility functions
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "Std_Types.h"
#include "stdlib.h"
#include "app_utils.h"

#include "app_timer.h"  /* Cores / Soc Independent timer implementation */

#if defined (SOC_J721E) || defined (SOC_J7200)|| defined (SOC_J721S2) || defined (SOC_J784S4)
#include <ti/drv/uart/UART_stdio.h>
#include <ti/drv/uart/UART.h>
#include <ti/board/board.h>
#include <ti/drv/pm/pmlib.h>
#include <ti/drv/sciclient/sciclient.h>
#include <ti/osal/osal.h>
#endif

#if defined (SOC_AM62X)
#include <drivers/hw_include/am62x/cslr_soc_baseaddress.h>
#include <drivers/sciclient/include/sciclient_pm.h>
#include <drivers/sciclient/include/tisci/am62x/tisci_devices.h>
#include <drivers/sciclient/include/tisci/am62x/tisci_clocks.h>
#elif defined (SOC_AM62AX)
#include <hw_include/soc.h>
#include <drivers/hw_include/am62ax/cslr_soc_baseaddress.h>
#include <drivers/sciclient/include/sciclient_pm.h>
#include <drivers/sciclient/include/tisci/am62ax/tisci_devices.h>
#include <drivers/sciclient/include/tisci/am62ax/tisci_clocks.h>
#elif defined (SOC_AM62PX)
#include <hw_include/soc.h>
#include <drivers/hw_include/am62px/cslr_soc_baseaddress.h>
#include <drivers/sciclient/include/sciclient_pm.h>
#include <drivers/sciclient/include/tisci/am62px/tisci_devices.h>
#include <drivers/sciclient/include/tisci/am62px/tisci_clocks.h>
#elif defined (SOC_J722S)
#include <hw_include/soc.h>
#include <drivers/hw_include/j722s/cslr_soc_baseaddress.h>
#include <drivers/sciclient/include/sciclient_pm.h>
#include <drivers/sciclient/include/tisci/j722s/tisci_devices.h>
#include <drivers/sciclient/include/tisci/j722s/tisci_clocks.h>
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/** \brief WKUP Domain CTRL base address + offset to beginning of PAD CONFIG
    section */
#define WKUP_PMUX_CTRL	(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1C000U)
/** \brief MAIN Domain CTRL base address + offset to beginning of PAD CONFIG
    section */
#define MAIN_PMUX_CTRL	(CSL_CTRL_MMR0_CFG0_BASE + 0x1C000U)

#if defined (SOC_J721E)
#if defined (BUILD_MCU1_0)
/** \brief MCU_UART_TXD pad offset */
#define PIN_MCU_UART_TXD   (0x00E0U)
/** \brief MCU_UART_RXD pad offset */
#define PIN_MCU_UART_RXD   (0x00E4U)
/** \brief MCU_UART_CTS pad offset */
#define PIN_MCU_UART_CTS   (0x00E8U)
/** \brief MCU_UART_RTS pad offset */
#define PIN_MCU_UART_RTS   (0x00ECU)

#elif defined (BUILD_MCU2_1)

/** \brief MAIN_UART_RXD pad offset */
#define PIN_UART0_RXD     (0x1E8U)
/** \brief MAIN_UART_TXD pad offset */
#define PIN_UART0_TXD     (0x1ECU)
/** \brief MAIN_UART_CTS pad offset */
#define PIN_UART0_CTSN    (0x1F0U)
/** \brief MAIN_UART_RTS pad offset */
#define PIN_UART0_RTSN    (0x1F4U)

#endif /* (BUILD_MCU2_1) */

#elif defined (SOC_J7200)
#if defined (BUILD_MCU1_0)
/** \brief MCU_UART_TXD pad offset */
#define PIN_MCU_UART_TXD   (0x00F0U)
/** \brief MCU_UART_RXD pad offset */
#define PIN_MCU_UART_RXD   (0x00F4U)
/** \brief MCU_UART_CTS pad offset */
#define PIN_MCU_UART_CTS   (0x00F8U)
/** \brief MCU_UART_RTS pad offset */
#define PIN_MCU_UART_RTS   (0x00FCU)

#elif defined (BUILD_MCU2_1)

/** \brief MAIN_UART_RXD pad offset */
#define PIN_UART0_RXD     (0x0B0U)
/** \brief MAIN_UART_TXD pad offset */
#define PIN_UART0_TXD     (0x0B4U)
/** \brief MAIN_UART_CTS pad offset */
#define PIN_UART0_CTSN    (0x0C0U)
/** \brief MAIN_UART_RTS pad offset */
#define PIN_UART0_RTSN    (0x0C4U)

#endif /* (BUILD_MCU2_1) */

#elif defined (SOC_J784S4)
#if defined (BUILD_MCU1_0)
/** \brief MCU_UART_TXD pad offset - PAD Configuration Register 56*/
#define PIN_MCU_UART_TXD   (0x00F0U)
/** \brief MCU_UART_RXD pad offset - PAD Configuration Register 57 */
#define PIN_MCU_UART_RXD   (0x00F4U)
/** \brief MCU_UART_CTS pad offset  - PAD Configuration Register 58*/
#define PIN_MCU_UART_CTS   (0x00F8U)
/** \brief MCU_UART_RTS pad offset - PAD Configuration Register 59 */
#define PIN_MCU_UART_RTS   (0x00FCU)

#elif defined (BUILD_MCU2_1)

/** \brief MAIN_UART_RXD pad offset - PAD Configuration Register 122 */
#define PIN_UART0_RXD     (0x0B0U)
/** \brief MAIN_UART_TXD pad offset - PAD Configuration Register 123  */
#define PIN_UART0_TXD     (0x0B4U)
/** \brief MAIN_UART_CTS pad offset - PAD Configuration Register 124 */
#define PIN_UART0_CTSN    (0x0C0U)
/** \brief MAIN_UART_RTS pad offset - PAD Configuration Register 125 */
#define PIN_UART0_RTSN    (0x0C4U)

#endif /* (BUILD_MCU2_1) */

#elif defined (SOC_J721S2)
#if defined (BUILD_MCU1_0)
/** \brief MCU_UART_TXD pad offset */
#define PIN_WKUP_GPIO0_12 (0x00F0U)
/** \brief MCU_UART_RXD pad offset */
#define PIN_WKUP_GPIO0_13 (0x00F4U)
/** \brief MCU_UART_CTS pad offset */
#define PIN_WKUP_GPIO0_14 (0x00F8U)
/** \brief MCU_UART_RTS pad offset */
#define PIN_WKUP_GPIO0_15 (0x00FCU)

#elif defined (BUILD_MCU2_1)

/** \brief MAIN_UART_RXD pad offset */
#define PIN_SPI0_CS1 (0x0D0U)
/** \brief MAIN_UART_TXD pad offset */
#define PIN_SPI0_CLK (0x0D4U)
/** \brief MAIN_UART_CTS pad offset */
#define PIN_MCASP0_AXR0 (0x040U)
/** \brief MAIN_UART_RTS pad offset */
#define PIN_MCASP0_AXR1 (0x044U)

#endif /* (BUILD_MCU2_1) */
#endif /* (SOC_J721S2) */

/** \brief Pin Mode Enable */
#define PIN_MODE(mode)	   (mode)

/** \brief Active mode configurations */
/** \brief Resistor disable */
#define PIN_PULL_DISABLE                (0x1U << 16U)
/** \brief Pull direction */
#define	PIN_PULL_DIRECTION              (0x1U << 17U)
/** \brief Receiver enable */
#define	PIN_INPUT_ENABLE                (0x1U << 18U)
/** \brief Driver disable */
#define	PIN_OUTPUT_DISABLE              (0x1U << 21U)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */


/* ========================================================================== */
/*                  Internal Function Declarations                            */
/* ========================================================================== */
/** \brief Utility used to check any memory corruption occured in
 *   stack and memory sections initialized by AppUtils_SectionInit. */
static uint32 AppUtils_CheckSectionCorruption(uint32 secStartAddr);
/** \brief Utility used to initialize memory sections word wise */
static void AppUtils_MemsetWord(uint32 *memPtr, uint32 pattern, uint32 length);
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
/** \brief Stores the 32bit result
 *         APP_UTILS_STATUS_PASS or APP_UTILS_STATUS_FAIL
 */
#if defined (BUILD_MCU)
__attribute__((section("EG_TEST_RESULT_32_SECTION")))
uint32 AppUtils_TestResult;
#endif
#if defined (BUILD_A53)
__attribute__ ((section ("EG_TEST_RESULT_32_SECTION")))
uint32 AppUtils_TestResult;
#endif

/** \brief Global variables of stack */
extern uint32 __stack, __STACK_SIZE;
/* ========================================================================== */
/*                          Function Implementation                           */
/* ========================================================================== */
/** \brief Utility used to get stack usage.
 *   Prerequisite - Known pattern 0xFEAA55EFU is written to the entire
 *                  stack size in linker command file
 *   In this API, stack size is known from the compiler macro __STACK_SIZE and
 *   stack start pointer from __stack.
 *   Pattern(0xFEAA55EFU) matching is done from the start of the stack and
 *   it is continued until pattern does not matches. The Stack
 *   region not consumed is updated in a variable freeEntry.
 *   Max stack used is calculated by (__STACK_SIZE - freeEntry).
 */
uint32 AppUtils_GetStackUsage(void)
{
    uint32  freeEntry = 0U, stackSize = 0U;
    uint32 *stackStart = &__stack;
    uint32  fillPattern = APP_UTILS_LINKER_FILL_PATTERN;

    stackSize = ((uint32) & __STACK_SIZE);
    /* __stack points to the ending stack address (__STACK_END) as the
       stack grows downwards. So stack usage is checked starting
       from __stack +1
    */
    stackStart++;
    while (1)
    {
        if (*stackStart != fillPattern)
        {
            break;
        }
        stackStart++;
        freeEntry += sizeof (uint32);
    }

    return (stackSize - freeEntry);
}

/** \brief Utility used to check any memory corruption occured in
 *   stack and memory sections initialized by AppUtils_SectionInit.
 *   In this API, both stack and memory sections initialized by
 *   AppUtils_SectionInit are checked.
 *   Stack size is known from the compiler macro __STACK_SIZE and stack size
 *   used is calculated from AppUtils_GetStackUsage. If stack size used is
 *   greater than __STACK_SIZE then stack corruption occured and return E_NOT_OK
 *   Similarly Pattern(0xFEAA55EFU) matching is done from the start of the each
 *   memory section initialized by AppUtils_SectionInit and it is continued
 *   till end of the section. If there is a pattern mismatch, breaks and
 *   returns E_NOT_OK otherwise returns E_OK.
 *
 *   This API expects AppUtils_SectionInit should be called in start up code
 *   before calling this.
 */
uint32 AppUtils_CheckStackAndSectionCorruption(
    uint32 *pAppUtils_SecHoleAddrList, uint32 secCnt)
{
    uint32 retVal = E_OK;
    uint32 index;
    uint32 stackUsed, stackSize = ((uint32) & __STACK_SIZE);

    stackUsed = AppUtils_GetStackUsage();
    if (stackUsed >= stackSize)
    {
        retVal = E_NOT_OK;
    }

    if (E_OK == retVal)
    {
        for (index = 0U; index < secCnt; index++)
        {
            retVal = AppUtils_CheckSectionCorruption(
                pAppUtils_SecHoleAddrList[index]);
            if (E_OK != retVal)
            {
                break;
            }
        }
    }

    return (retVal);
}

/** \brief Initializes the core internal timer (PMU in case of R5F)
 *
 *          This should be called before any profiling API's are called. (i.e.
 *          AppUtils_GetTimeStamp (), AppUtils_GetTimeStampFreq(),
 *          AppUtils_GetCycles()
 */
void AppUtils_ProfileInit(uint32 unUsedForNow)
{
    AppUtils_TimerInit (unUsedForNow);
}

/** \brief Provides clock frequency used for the counter
 */
uint32 AppUtils_ProfileGetClkFreq (void)
{
    return (AppUtils_TimerGetClkFreq ());
}

/** \brief Provides the current tick of the counter
 *
 */
uint32 AppUtils_ProfileGetCycles(void)
{
    return (AppUtils_TimerGetCurrTick());
}

/** \brief Elapsed count
 *
 */
uint32 AppUtils_ProfileGetElapsedTimeInMs(uint32 cycleCnt)
{
    uint32 currTick, rtnVal, clkFreq;
    /* Steps
        1. Get current count
        2. Check for overflow
        3. Compute elapsed time in MS
        4. if overflow
        4.1. reset the counter
        4.2. return fixed value of 0xFFFFFFFF */
    currTick = AppUtils_TimerGetCurrTick();
    if (cycleCnt < currTick)
    {
        clkFreq = AppUtils_TimerGetClkFreq ();
        /* clkFreq = ticks per second
            ticksPerMsecond = clkFreq / 1000
            elapsed value = old value - new value / ticksPerMsecond */
        rtnVal = ((currTick - cycleCnt) / (clkFreq / 1000U));
    }
    else
    {
        /* Overflow ? Tbd to reset and others */
        rtnVal = 0xFFFFFFFFU;
    }

    return rtnVal;
}

/**
 *  \brief Stores the provided 32bit value in global variable.
 *
 *      The variable 'AppUtils_TestResult' will updated to reflect the test
 *      result.
 */
void AppUtils_LogResult (uint32 testResult)
{
    AppUtils_TestResult = testResult;
    return;
}

/**
 *  \brief App Utils Init for print utility
 *
 */
 void AppUtils_Init(void)
{
#if defined (SOC_J721E) || defined (SOC_J7200)|| defined (SOC_J721S2) || defined (SOC_J784S4)
    Board_initCfg   boardCfg;
	/* Configure MCU UART0 and MAIN UART0 module for serial console prints. */
    boardCfg = BOARD_INIT_UART_STDIO;
#endif
    uint32 regVal;

#if defined (SOC_J721E) || defined (SOC_J7200)|| defined (SOC_J721S2) || defined (SOC_J784S4)
	Board_init(boardCfg);
#endif

    /* Unlock lock key registers for Partition 7: IO PAD
       configuration registers in WKUP_CTRL_MMR */
    /* write Partition 7 Lock Key 0 Register */
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE +
        CSL_WKUP_CTRL_MMR_CFG0_LOCK7_KICK0, 0x68EF3490U);
    /* write Partition 7 Lock Key 1 Register */
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE +
        CSL_WKUP_CTRL_MMR_CFG0_LOCK7_KICK1, 0xD172BC5AU);
    /* Check for unlock */
    regVal = CSL_REG32_RD(CSL_WKUP_CTRL_MMR0_CFG0_BASE +
        CSL_WKUP_CTRL_MMR_CFG0_LOCK7_KICK0);
    while ((regVal & 0x1U) != 0x1U)
    {
        regVal = CSL_REG32_RD(CSL_WKUP_CTRL_MMR0_CFG0_BASE +
            CSL_WKUP_CTRL_MMR_CFG0_LOCK7_KICK0);
    }

    /* Below code will be replaced by Port module in further releases */
    /* UART Pin Mux */
#if defined (SOC_J721E)

#if defined (BUILD_MCU1_0)
    /* MCU_UART0 -> MCU_UART0_RXD */
    regVal = PIN_MODE(0U) | ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) &
            (~PIN_PULL_DIRECTION));
    CSL_REG32_WR(WKUP_PMUX_CTRL + PIN_MCU_UART_RXD, regVal);
    /* MCU_UART0 -> MCU_UART0_TXD */
    regVal = PIN_MODE(0U) | ((PIN_PULL_DISABLE) &
            (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE));
    CSL_REG32_WR(WKUP_PMUX_CTRL + PIN_MCU_UART_TXD, regVal);
    /* MCU_UART0 -> MCU_UART0_CTS */
    regVal = PIN_MODE(0U) | ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) &
            (~PIN_PULL_DIRECTION));
    CSL_REG32_WR(WKUP_PMUX_CTRL + PIN_MCU_UART_CTS, regVal);
    /* MCU_UART0 -> MCU_UART0_RTS */
    regVal = PIN_MODE(0U) | ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) &
            (~PIN_PULL_DIRECTION));
    CSL_REG32_WR(WKUP_PMUX_CTRL + PIN_MCU_UART_RTS, regVal);

#elif defined (BUILD_MCU2_1)
    /* Unlock lock key registers for Partition 7: IO PAD
       configuration registers in MAIN_CTRL_MMR */
    /* write Partition 7 Lock Key 0 Register */
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1D008U, 0x68EF3490U);
    /* write Partition 7 Lock Key 1 Register */
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1D00CU, 0xD172BC5AU);
    /* Check for unlock */
    regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + 0x1D008U);
    while ((regVal & 0x1U) != 0x1U)
    {
        regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + 0x1D008U);
    }
    /* Main UART0 -> UART0_RXD -> AA4 */
    regVal = PIN_MODE(0U) | ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) &
            (~PIN_PULL_DIRECTION));
    CSL_REG32_WR(MAIN_PMUX_CTRL + PIN_UART0_RXD, regVal);
    /* Main UART0 -> UART0_TXD -> AB4 */
    regVal = PIN_MODE(0U) | ((PIN_PULL_DISABLE) &
            (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE));
    CSL_REG32_WR(MAIN_PMUX_CTRL + PIN_UART0_TXD, regVal);
    /* Main UART0 -> UART0_CTSn -> Y3 */
    regVal = PIN_MODE(0U) | ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) &
            (~PIN_PULL_DIRECTION));
    CSL_REG32_WR(MAIN_PMUX_CTRL + PIN_UART0_CTSN, regVal);
    /* Main UART0 -> UART0_RTSn -> AA2 */
    regVal = PIN_MODE(0U) | ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) &
            (~PIN_PULL_DIRECTION));
    CSL_REG32_WR(MAIN_PMUX_CTRL + PIN_UART0_RTSN, regVal);

#endif /* (BUILD_MCU2_1) */

#elif defined (SOC_J7200)

#if defined (BUILD_MCU1_0)
    /* MCU_UART0 -> MCU_UART0_RXD */
    regVal = PIN_MODE(0U) | ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) &
            (~PIN_PULL_DIRECTION));
    CSL_REG32_WR(WKUP_PMUX_CTRL + PIN_MCU_UART_RXD, regVal);
    /* MCU_UART0 -> MCU_UART0_TXD */
    regVal = PIN_MODE(0U) | ((PIN_PULL_DISABLE) &
            (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE));
    CSL_REG32_WR(WKUP_PMUX_CTRL + PIN_MCU_UART_TXD, regVal);
    /* MCU_UART0 -> MCU_UART0_CTS */
    regVal = PIN_MODE(0U) | ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) &
            (~PIN_PULL_DIRECTION));
    CSL_REG32_WR(WKUP_PMUX_CTRL + PIN_MCU_UART_CTS, regVal);
    /* MCU_UART0 -> MCU_UART0_RTS */
    regVal = PIN_MODE(0U) | ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) &
            (~PIN_PULL_DIRECTION));
    CSL_REG32_WR(WKUP_PMUX_CTRL + PIN_MCU_UART_RTS, regVal);

#elif defined (BUILD_MCU2_1)
    /* Unlock lock key registers for Partition 7: IO PAD
       configuration registers in MAIN_CTRL_MMR */
    /* write Partition 7 Lock Key 0 Register */
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1D008U, 0x68EF3490U);
    /* write Partition 7 Lock Key 1 Register */
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1D00CU, 0xD172BC5AU);
    /* Check for unlock */
    regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + 0x1D008U);
    while ((regVal & 0x1U) != 0x1U)
    {
        regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + 0x1D008U);
    }
    /* Main UART0 -> UART0_RXD -> AA4 */
    regVal = PIN_MODE(0U) | ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) &
            (~PIN_PULL_DIRECTION));
    CSL_REG32_WR(MAIN_PMUX_CTRL + PIN_UART0_RXD, regVal);
    /* Main UART0 -> UART0_TXD -> AB4 */
    regVal = PIN_MODE(0U) | ((PIN_PULL_DISABLE) &
            (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE));
    CSL_REG32_WR(MAIN_PMUX_CTRL + PIN_UART0_TXD, regVal);
    /* Main UART0 -> UART0_CTSn -> Y3 */
    regVal = PIN_MODE(2U) | ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) &
            (~PIN_PULL_DIRECTION));
    CSL_REG32_WR(MAIN_PMUX_CTRL + PIN_UART0_CTSN, regVal);
    /* Main UART0 -> UART0_RTSn -> AA2 */
    regVal = PIN_MODE(2U) | ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) &
            (~PIN_PULL_DIRECTION));
    CSL_REG32_WR(MAIN_PMUX_CTRL + PIN_UART0_RTSN, regVal);

#endif /* (BUILD_MCU2_1) */

#elif defined (SOC_J784S4)

#if defined (BUILD_MCU1_0)
    /* MCU_UART0 -> MCU_UART0_RXD */
    regVal = PIN_MODE(0U) | ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) &
            (~PIN_PULL_DIRECTION));
    CSL_REG32_WR(WKUP_PMUX_CTRL + PIN_MCU_UART_RXD, regVal);
    /* MCU_UART0 -> MCU_UART0_TXD */
    regVal = PIN_MODE(0U) | ((PIN_PULL_DISABLE) &
            (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE));
    CSL_REG32_WR(WKUP_PMUX_CTRL + PIN_MCU_UART_TXD, regVal);
    /* MCU_UART0 -> MCU_UART0_CTS */
    regVal = PIN_MODE(0U) | ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) &
            (~PIN_PULL_DIRECTION));
    CSL_REG32_WR(WKUP_PMUX_CTRL + PIN_MCU_UART_CTS, regVal);
    /* MCU_UART0 -> MCU_UART0_RTS */
    regVal = PIN_MODE(0U) | ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) &
            (~PIN_PULL_DIRECTION));
    CSL_REG32_WR(WKUP_PMUX_CTRL + PIN_MCU_UART_RTS, regVal);

#elif defined (BUILD_MCU2_1)
    /* Unlock lock key registers for Partition 7: IO PAD
       configuration registers in MAIN_CTRL_MMR */
    /* write Partition 7 Lock Key 0 Register */
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1D008U, 0x68EF3490U);
    /* write Partition 7 Lock Key 1 Register */
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1D00CU, 0xD172BC5AU);
    /* Check for unlock */
    regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + 0x1D008U);
    while ((regVal & 0x1U) != 0x1U)
    {
        regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + 0x1D008U);
    }
    /* Main UART0 -> UART0_RXD -> AA4 */
    regVal = PIN_MODE(0U) | ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) &
            (~PIN_PULL_DIRECTION));
    CSL_REG32_WR(MAIN_PMUX_CTRL + PIN_UART0_RXD, regVal);
    /* Main UART0 -> UART0_TXD -> AB4 */
    regVal = PIN_MODE(0U) | ((PIN_PULL_DISABLE) &
            (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE));
    CSL_REG32_WR(MAIN_PMUX_CTRL + PIN_UART0_TXD, regVal);
    /* Main UART0 -> UART0_CTSn -> Y3 */
    regVal = PIN_MODE(2U) | ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) &
            (~PIN_PULL_DIRECTION));
    CSL_REG32_WR(MAIN_PMUX_CTRL + PIN_UART0_CTSN, regVal);
    /* Main UART0 -> UART0_RTSn -> AA2 */
    regVal = PIN_MODE(2U) | ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) &
            (~PIN_PULL_DIRECTION));
    CSL_REG32_WR(MAIN_PMUX_CTRL + PIN_UART0_RTSN, regVal);

#endif /* (BUILD_MCU2_1) */

#elif defined (SOC_J721S2)

#if defined (BUILD_MCU1_0)
    /* MCU_UART0 -> MCU_UART0_RXD ->C24 */
    regVal = PIN_MODE(0U) | ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) &
            (~PIN_PULL_DIRECTION));
    CSL_REG32_WR(WKUP_PMUX_CTRL + PIN_WKUP_GPIO0_13, regVal);
    /* MCU_UART0 -> MCU_UART0_TXD -> C25 */
    regVal = PIN_MODE(0U) | ((PIN_PULL_DISABLE) &
            (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE));
    CSL_REG32_WR(WKUP_PMUX_CTRL + PIN_WKUP_GPIO0_12, regVal);
    /* MCU_UART0 -> MCU_UART0_CTS -> B24*/
    regVal = PIN_MODE(0U) | ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) &
            (~PIN_PULL_DIRECTION));
    CSL_REG32_WR(WKUP_PMUX_CTRL + PIN_WKUP_GPIO0_14, regVal);
    /* MCU_UART0 -> MCU_UART0_RTS  -> D25*/
    regVal = PIN_MODE(0U) | ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) &
            (~PIN_PULL_DIRECTION));
    CSL_REG32_WR(WKUP_PMUX_CTRL + PIN_WKUP_GPIO0_15, regVal);

#elif defined (BUILD_MCU2_1)
    /* Unlock lock key registers for Partition 7: IO PAD
       configuration registers in MAIN_CTRL_MMR */
    /* write Partition 7 Lock Key 0 Register */
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1D008U, 0x68EF3490U);
    /* write Partition 7 Lock Key 1 Register */
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1D00CU, 0xD172BC5AU);
    /* Check for unlock */
    regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + 0x1D008U);
    while ((regVal & 0x1U) != 0x1U)
    {
        regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + 0x1D008U);
    }
    /* Main UART8 -> UART8_RXD -> AF26 */
    regVal = PIN_MODE(11U) | ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) &
            (~PIN_PULL_DIRECTION));
    CSL_REG32_WR(MAIN_PMUX_CTRL + PIN_SPI0_CS1, regVal);
    /* Main UART8 -> UART8_TXD -> AH27 */
    regVal = PIN_MODE(11U) | ((PIN_PULL_DISABLE) &
            (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE));
    CSL_REG32_WR(MAIN_PMUX_CTRL + PIN_SPI0_CLK, regVal);
    /* Main UART8 -> UART8_CTSn -> AC28 */
    regVal = PIN_MODE(14U) | ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) &
            (~PIN_PULL_DIRECTION));
    CSL_REG32_WR(MAIN_PMUX_CTRL + PIN_MCASP0_AXR0, regVal);
    /* Main UART8 -> UART8_RTSn -> Y26 */
    regVal = PIN_MODE(14U) | ((PIN_PULL_DISABLE | PIN_INPUT_ENABLE) &
            (~PIN_PULL_DIRECTION));
    CSL_REG32_WR(MAIN_PMUX_CTRL + PIN_MCASP0_AXR1, regVal);

#endif /* (BUILD_MCU2_1) */

#endif /* (SOC_J721S2) */

    return;
}

/**
 *  \brief Printf utility
 *
 */
void AppUtils_Printf (uint32 type,const char *pcString, ...)
{
    char printBuffer[APP_UTILS_PRINT_MAX_STRING_SIZE];
    va_list arguments;

    /* Start the varargs processing. */
    va_start(arguments, pcString);
    vsnprintf (printBuffer, sizeof(printBuffer), pcString, arguments);

    if (type >= APP_UTILS_PRINT_MSG_LEVEL)
    {
#ifdef UART_ENABLED
         UART_printf("%s",printBuffer);
#else
        printf("%s",printBuffer);
#endif
    }
/* End the varargs processing. */
va_end(arguments);

    return;
}

char AppUtils_GetChar(void)
{
    char ch;

#ifdef UART_ENABLED
    ch = UART_getc();
#else
    scanf("%c", &ch);
#endif

    return (ch);
}

sint32 AppUtils_GetNum(void)
{
    sint32 num;

#ifdef UART_ENABLED
    UART_scanFmt("%d", &num);
#else
    scanf("%d", &num);
#endif

    return (num);
}

/** \brief Utility used to initialize memory sections.
 *   This API is used to initialize fixed size(APP_UTILS_LINKER_FILL_LENGTH)
 *   before start of each section and before end of each section. i.e
 *      .=align(4);
 *       __linker_spi_text_start = .;
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      *(SPI_TEXT_SECTION)
 *      .=align(4);
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      __linker_spi_text_end = .;
 *
 *   This API should be called during module start up code.
 */
void AppUtils_SectionInit(uint32 *pAppUtils_SecHoleAddrList, uint32 secCnt)
{
    uint32 index;

    for (index = 0U; index < secCnt; index++)
    {
        AppUtils_MemsetWord(
            ((uint32 *)pAppUtils_SecHoleAddrList[index]),
            APP_UTILS_LINKER_FILL_PATTERN,
            (APP_UTILS_LINKER_FILL_LENGTH / sizeof (uint32)));
    }

    return;
}

/** \brief Utility used to check any memory corruption occured in
 *   stack and memory sections initialized by AppUtils_SectionInit. */
static uint32 AppUtils_CheckSectionCorruption(uint32 secStartAddr)
{
    uint32  retVal      = E_OK, i;
    uint32 *secStartPtr = ((uint32 *) secStartAddr);
    uint32  fillPattern = APP_UTILS_LINKER_FILL_PATTERN;
    uint32  fillLength  = APP_UTILS_LINKER_FILL_LENGTH;

    i = 0U;
    while (i < fillLength)
    {
        if (*secStartPtr != fillPattern)
        {
            retVal = E_NOT_OK;
            break;
        }
        secStartPtr = secStartPtr + 1;
        i += sizeof (uint32);
    }

    return (retVal);
}

/** \brief Utility used to initialize memory sections word wise */
static void AppUtils_MemsetWord(uint32 *memPtr, uint32 pattern, uint32 length)
{
    while (length > 0U)
    {
        *memPtr = pattern;
        memPtr++;
        length--;
    }

    return;
}

uint32 AppUtils_GetAdcVoltage(uint32 adcValue)
{
    uint32 refVoltage    = 1800U;
    uint32 maxResolution = 4095U;

    return ((adcValue * refVoltage) / maxResolution);
}

#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4) || defined (SOC_AM62X)
	/* OSAL being generic might not record the correct frequency and hence we
     * are setting it explicitly. Osal_delay function uses PMU counter which
     * runs at CPU speed and uses OSAL_CPU_FREQ_KHZ_DEFAULT to get the CPU
     * frequency. By default OSAL_CPU_FREQ_KHZ_DEFAULT is set to 400MHZ.
	 * In case of J721E this parameter needs to be set explicitly to 1GHZ
     * to get the correct delay functionality.
     */
void AppUtils_SetCpuFrequency(void)
{
	
	uint64 mcuClkFreq;
#if defined (SOC_AM62X)
	/* All R5F cores run at the same frequency, hence we just take R5FSS0_CORE0 */
     Sciclient_pmGetModuleClkFreq(TISCI_DEV_WKUP_R5FSS0_CORE0,
									   TISCI_DEV_WKUP_R5FSS0_CORE0_CPU_CLK,
									   &mcuClkFreq,
									   SCICLIENT_SERVICE_WAIT_FOREVER);
#endif

#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
	Int32  retVal;
	/* All R5F cores run at the same frequency, hence we just take R5FSS0_CORE0 */
	retVal = Sciclient_pmGetModuleClkFreq(TISCI_DEV_MCU_R5FSS0_CORE0,
									   TISCI_DEV_MCU_R5FSS0_CORE0_CPU_CLK,
									   &mcuClkFreq,
									   SCICLIENT_SERVICE_WAIT_FOREVER);
	if(retVal == 0)
	{
		Osal_HwAttrs  hwAttrs;
		uint32      ctrlBitmap;

		retVal = Osal_getHwAttrs(&hwAttrs);
		if(retVal == 0)
		{
			/*
			 * Change the PMU timer input clock frequency configuration
			   based on R5 CPU clock configured
			 */
			hwAttrs.cpuFreqKHz = (Int32)(mcuClkFreq/1000U);
			ctrlBitmap         = OSAL_HWATTR_SET_CPU_FREQ;
			retVal = Osal_setHwAttrs(ctrlBitmap, &hwAttrs);
		}
	}

	if(retVal != 0)
	{
		AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
			"Could not set CPU frequency !!!\n");
	}
#endif
	return;
}
#endif

sint32 AppUtils_ModuleClockEnable(uint32 moduleId)
{
    sint32 retVal = 0;
    sint32 status = -1;
    uint32 moduleState = 0U;
    uint32 resetState = 0U;
    uint32 contextLossState = 0U;

    /* Get the module state.
       No need to change the module state if it
       is already ON
     */
    status = Sciclient_pmGetModuleState(moduleId,
                                        &moduleState,
                                        &resetState,
                                        &contextLossState,
                                        SCICLIENT_SERVICE_WAIT_FOREVER);
    if(moduleState == TISCI_MSG_VALUE_DEVICE_HW_STATE_OFF)
    {
        status = Sciclient_pmSetModuleState(moduleId,
                                            TISCI_MSG_VALUE_DEVICE_SW_STATE_ON,
                                            (TISCI_MSG_FLAG_AOP |
                                             TISCI_MSG_FLAG_DEVICE_RESET_ISO),
                                             SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status == 0)
        {
            status = Sciclient_pmSetModuleRst (moduleId,
                                               0x0U,
                                               SCICLIENT_SERVICE_WAIT_FOREVER);
            if (status != 0)
            {
                retVal = -1;
            }
        }
        else
        {
            retVal = -1;
        }
    }

    return retVal;
}

/* ========================================================================== */
/*                             Local Functions                                */
/* ========================================================================== */
