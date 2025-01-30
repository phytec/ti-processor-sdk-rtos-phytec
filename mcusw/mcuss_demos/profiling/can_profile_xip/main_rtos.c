/*
*
* Copyright (C) 2024-2021 Texas Instruments Incorporated
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
 *  \file main_rtos.c
 *
 *  \brief Main file for RTOS build
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <ti/csl/arch/csl_arch.h>
#include <ti/csl/soc.h>
#include <ti/csl/cslr.h>

#include <ti/osal/osal.h>
#include <ti/osal/TaskP.h>
#include <ti/board/board.h>

#include "CanApp_Startup.h"
#include "can_profile.h"
#include "CanIf.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Test application stack size */
#define APP_TASK_STACK                  (10U * 1024U)
/**< Stack required for the stack */
#define CAN_PROFILE_DEMO_TASK_NAME      ("CAN PROFILE")
/**< Task name */

/** \brief WKUP Domain CTRL base address + offset to beginning of PAD CONFIG
    section */
#define CAN_XIP_WKUP_PMUX_CTRL	(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1C000U)
/** \brief MAIN Domain CTRL base address + offset to beginning of PAD CONFIG
    section */
#define CAN_XIP_MAIN_PMUX_CTRL	(CSL_CTRL_MMR0_CFG0_BASE + 0x1C000U)

#if defined (SOC_J721E)
#if defined (BUILD_MCU1_0)
/** \brief MCU_UART_TXD pad offset */
#define CAN_XIP_PIN_MCU_UART_TXD   (0x00E0U)
/** \brief MCU_UART_RXD pad offset */
#define CAN_XIP_PIN_MCU_UART_RXD   (0x00E4U)
/** \brief MCU_UART_CTS pad offset */
#define CAN_XIP_PIN_MCU_UART_CTS   (0x00E8U)
/** \brief MCU_UART_RTS pad offset */
#define CAN_XIP_PIN_MCU_UART_RTS   (0x00ECU)

#elif defined (BUILD_MCU2_1)

/** \brief MAIN_UART_RXD pad offset */
#define CAN_XIP_PIN_UART0_RXD     (0x1E8U)
/** \brief MAIN_UART_TXD pad offset */
#define CAN_XIP_PIN_UART0_TXD     (0x1ECU)
/** \brief MAIN_UART_CTS pad offset */
#define CAN_XIP_PIN_UART0_CTSN    (0x1F0U)
/** \brief MAIN_UART_RTS pad offset */
#define CAN_XIP_PIN_UART0_RTSN    (0x1F4U)

#endif /* (BUILD_MCU2_1) */

#elif defined (SOC_J7200)
#if defined (BUILD_MCU1_0)
/** \brief MCU_UART_TXD pad offset */
#define CAN_XIP_PIN_MCU_UART_TXD   (0x00F0U)
/** \brief MCU_UART_RXD pad offset */
#define CAN_XIP_PIN_MCU_UART_RXD   (0x00F4U)
/** \brief MCU_UART_CTS pad offset */
#define CAN_XIP_PIN_MCU_UART_CTS   (0x00F8U)
/** \brief MCU_UART_RTS pad offset */
#define CAN_XIP_PIN_MCU_UART_RTS   (0x00FCU)

#elif defined (BUILD_MCU2_1)

/** \brief MAIN_UART_RXD pad offset */
#define CAN_XIP_PIN_UART0_RXD     (0x0B0U)
/** \brief MAIN_UART_TXD pad offset */
#define CAN_XIP_PIN_UART0_TXD     (0x0B4U)
/** \brief MAIN_UART_CTS pad offset */
#define CAN_XIP_PIN_UART0_CTSN    (0x0C0U)
/** \brief MAIN_UART_RTS pad offset */
#define CAN_XIP_PIN_UART0_RTSN    (0x0C4U)

#endif /* (BUILD_MCU2_1) */
#endif /* (SOC_J7200) */

/** \brief Pin Mode Enable */
#define CAN_XIP_PIN_MODE(mode)	   (mode)

/** \brief Active mode configurations */
/** \brief Resistor disable */
#define CAN_XIP_PIN_PULL_DISABLE                (0x1U << 16U)
/** \brief Pull direction */
#define	CAN_XIP_PIN_PULL_DIRECTION              (0x1U << 17U)
/** \brief Receiver enable */
#define	CAN_XIP_PIN_INPUT_ENABLE                (0x1U << 18U)
/** \brief Driver disable */
#define	CAN_XIP_PIN_OUTPUT_DISABLE              (0x1U << 21U)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static void CanApp_TaskFxn(void* a0, void* a1);
static void CanApp_Shutdown(void);
static void AppUtils_XIP_Init(void);
Board_STATUS Board_uartStdioInit(void);
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
/* application stack */
static uint8_t CanApp_TaskStack[APP_TASK_STACK] __attribute__((aligned(32)));
/**< Stack for the task */
static canAppTaskObj_t  CanApp_TestPrms;
/**< Test parameters */
static HwiP_Handle      CanApp_IsrHndls[CAN_MAX_CONTROLLER];
/**< Stores the ISR handles */
extern SemaphoreP_Handle CanIf_TxConfirmationSemaphore;
/**< TX Confirmation semaphore, would be posted when TX completes */
extern SemaphoreP_Handle CanIf_RxConfirmationSemaphore;
/**< Rx Confirmation semaphore, would be posted when TX completes */
/* ========================================================================== */
/*                            External Variables                              */
/* ========================================================================== */
extern uint32 CanIf_DrvStatus;
/**< CAN IF Driver Status, defined in CanIf.c */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int main(void)
{
    TaskP_Handle task;
    TaskP_Params taskParams;

    /* Configure MCU UART0 and MAIN UART0 module for serial console prints. */
    AppUtils_XIP_Init();
    OS_init();

    CanApp_Startup();
    CanApp_PowerAndClkSrc();
    CanApp_PlatformInit();

    /* Initialize dummy CAN IF */
    CanIf_Init(NULL);

    if (CANIF_DRV_INITIALIZED == CanIf_DrvStatus)
    {
        /* Initialize the task params */
        TaskP_Params_init(&taskParams);
        // taskParams.instance->name = CAN_PROFILE_DEMO_TASK_NAME;
        /* Set the task priority higher than the default priority (1) */
        taskParams.priority     = 2;
        taskParams.stack        = CanApp_TaskStack;
        taskParams.stacksize    = sizeof (CanApp_TaskStack);

        task = TaskP_create(&CanApp_TaskFxn, &taskParams);
        if(NULL == task)
        {
            OS_stop();
        }
        OS_start();    /* does not return */
    }
    else
    {
        AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                " Error : CAN IF not initialized !!!\n");
    }

    return(0);
}

static void CanApp_TaskFxn(void* a0, void* a1)
{
    Utils_prfInit();

    Utils_prfLoadRegister (TaskP_self(), CAN_PROFILE_DEMO_TASK_NAME);

    CanApp_TestPrms.numTxPackets = APP_NUM_MSG_PER_ITERATION;
    CanApp_TestPrms.numIterations = APP_NUM_ITERATION;

    TaskP_yield();

    CanApp_ProfileTest(&CanApp_TestPrms);

    CanApp_Shutdown();

    Utils_prfLoadUnRegister (TaskP_self());
    Utils_prfDeInit();
    return;
}

/** \brief Application tear down functions */
static void CanApp_Shutdown(void)
{
    uint32 idx;

    for (idx = 0U; idx < CAN_MAX_CONTROLLER; idx++)
    {
        if (NULL != CanApp_IsrHndls[idx])
        {
            if (HwiP_OK != HwiP_delete(CanApp_IsrHndls[idx]))
            {
                AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                                " Error!!! Could not De register"
                                " the ISR for instance %d!!!\n", idx);
                break;
            }
        }
    }

    if (NULL != CanIf_TxConfirmationSemaphore)
    {
        SemaphoreP_delete(CanIf_TxConfirmationSemaphore);
        if (NULL != CanIf_RxConfirmationSemaphore)
        {
            SemaphoreP_delete(CanIf_RxConfirmationSemaphore);
        }
    }
    return;
}

static void AppUtils_XIP_Init(void)
{
    uint32 regVal;

    /* Configure MCU UART0 and MAIN UART0 module for serial console prints. */
    Board_uartStdioInit();

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
    regVal = CAN_XIP_PIN_MODE(0U) | ((CAN_XIP_PIN_PULL_DISABLE | CAN_XIP_PIN_INPUT_ENABLE) &
            (~CAN_XIP_PIN_PULL_DIRECTION));
    CSL_REG32_WR(CAN_XIP_WKUP_PMUX_CTRL + CAN_XIP_PIN_MCU_UART_RXD, regVal);
    /* MCU_UART0 -> MCU_UART0_TXD */
    regVal = CAN_XIP_PIN_MODE(0U) | ((CAN_XIP_PIN_PULL_DISABLE) &
            (~CAN_XIP_PIN_PULL_DIRECTION & ~CAN_XIP_PIN_INPUT_ENABLE));
    CSL_REG32_WR(CAN_XIP_WKUP_PMUX_CTRL + CAN_XIP_PIN_MCU_UART_TXD, regVal);
    /* MCU_UART0 -> MCU_UART0_CTS */
    regVal = CAN_XIP_PIN_MODE(0U) | ((CAN_XIP_PIN_PULL_DISABLE | CAN_XIP_PIN_INPUT_ENABLE) &
            (~CAN_XIP_PIN_PULL_DIRECTION));
    CSL_REG32_WR(CAN_XIP_WKUP_PMUX_CTRL + CAN_XIP_PIN_MCU_UART_CTS, regVal);
    /* MCU_UART0 -> MCU_UART0_RTS */
    regVal = CAN_XIP_PIN_MODE(0U) | ((CAN_XIP_PIN_PULL_DISABLE | CAN_XIP_PIN_INPUT_ENABLE) &
            (~CAN_XIP_PIN_PULL_DIRECTION));
    CSL_REG32_WR(CAN_XIP_WKUP_PMUX_CTRL + CAN_XIP_PIN_MCU_UART_RTS, regVal);

#endif /* (BUILD_MCU1_0) */

#elif defined (SOC_J7200)

#if defined (BUILD_MCU1_0)
    /* MCU_UART0 -> MCU_UART0_RXD */
    regVal = CAN_XIP_PIN_MODE(0U) | ((CAN_XIP_PIN_PULL_DISABLE | CAN_XIP_PIN_INPUT_ENABLE) &
            (~CAN_XIP_PIN_PULL_DIRECTION));
    CSL_REG32_WR(CAN_XIP_WKUP_PMUX_CTRL + CAN_XIP_PIN_MCU_UART_RXD, regVal);
    /* MCU_UART0 -> MCU_UART0_TXD */
    regVal = CAN_XIP_PIN_MODE(0U) | ((CAN_XIP_PIN_PULL_DISABLE) &
            (~CAN_XIP_PIN_PULL_DIRECTION & ~CAN_XIP_PIN_INPUT_ENABLE));
    CSL_REG32_WR(CAN_XIP_WKUP_PMUX_CTRL + CAN_XIP_PIN_MCU_UART_TXD, regVal);
    /* MCU_UART0 -> MCU_UART0_CTS */
    regVal = CAN_XIP_PIN_MODE(0U) | ((CAN_XIP_PIN_PULL_DISABLE | CAN_XIP_PIN_INPUT_ENABLE) &
            (~CAN_XIP_PIN_PULL_DIRECTION));
    CSL_REG32_WR(CAN_XIP_WKUP_PMUX_CTRL + CAN_XIP_PIN_MCU_UART_CTS, regVal);
    /* MCU_UART0 -> MCU_UART0_RTS */
    regVal = CAN_XIP_PIN_MODE(0U) | ((CAN_XIP_PIN_PULL_DISABLE | CAN_XIP_PIN_INPUT_ENABLE) &
            (~CAN_XIP_PIN_PULL_DIRECTION));
    CSL_REG32_WR(CAN_XIP_WKUP_PMUX_CTRL + CAN_XIP_PIN_MCU_UART_RTS, regVal);

#endif /* (BUILD_MCU2_1) */

#endif /* (SOC_J7200) */

    return;
}
