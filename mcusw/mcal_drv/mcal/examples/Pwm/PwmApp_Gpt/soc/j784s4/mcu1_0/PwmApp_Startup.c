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
 *  \file     PwmApp_Startup.c
 *
 *  \brief    This file contains the PWM test example setup
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "PwmApp.h"
#include <hw_include/lld_types.h>
/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define MSG_NORMAL      (APP_UTILS_PRINT_MSG_NORMAL)
/**< Message type */
#define MSG_STATUS      (APP_UTILS_PRINT_MSG_STATUS)
/**< Message type */
/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */
#if (STD_ON == PWM_NOTIFICATION_SUPPORTED)
static void PwmApp_InterruptConfig(void);
#endif
static void PwmApp_ClearAnyOperationalTimers(void);
static void PwmApp_SetClockSource4Timers(void);

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/** \brief Start up sequence : Program the interrupt muxes / priorities */
void PwmApp_Startup(void)
{
    PwmApp_ClearAnyOperationalTimers(); /* Clear MAIN TIMER only */
    PwmApp_SetClockSource4Timers();
#if (STD_ON == PWM_NOTIFICATION_SUPPORTED)
    PwmApp_InterruptConfig();
#endif
	/* OSAL being generic might not record the correct frequency and hence we
     * are setting it explicitly. Osal_delay function uses PMU counter which
     * runs at CPU speed and uses OSAL_CPU_FREQ_KHZ_DEFAULT to get the CPU
     * frequency. By default OSAL_CPU_FREQ_KHZ_DEFAULT is set to 400MHZ.
	 * In case of J721E this parameter needs to be set explicitly to 1GHZ
     * to get the correct delay functionality.
     */
	AppUtils_SetCpuFrequency();
}

/* ========================================================================== */
/*                        Application Local Functions                         */
/* ========================================================================== */
#if (STD_ON == PWM_NOTIFICATION_SUPPORTED)
static void PwmApp_InterruptConfig(void)
{
    Intc_Init();    /* Interrupt handler initialized, here as other functions
                     * can use APIs to clear pending interrupts if any
                     */
    /*interrupt config*/
    OsalRegisterIntrParams_t    intrPrms;
    OsalInterruptRetCode_e      osalRetVal;
    HwiP_Handle hwiHandle;
    struct tisci_msg_rm_irq_set_req     rmIrqReq;
    struct tisci_msg_rm_irq_set_resp    rmIrqResp;
    Int32 retVal;

    /* Set the destination interrupt */
    rmIrqReq.valid_params           = TISCI_MSG_VALUE_RM_DST_ID_VALID;
    rmIrqReq.valid_params          |= TISCI_MSG_VALUE_RM_DST_HOST_IRQ_VALID;
    rmIrqReq.src_id                 = TISCI_DEV_TIMER0;
    rmIrqReq.global_event           = 0U;
    rmIrqReq.src_index              = 0U;
    rmIrqReq.dst_id                 = TISCI_DEV_MCU_R5FSS0_CORE0;
    rmIrqReq.dst_host_irq           = INTRTR_CFG_MAIN_DOMAIN_TIMER_1_OUT_INT_NO +
                                        CSLR_MCU_R5FSS0_CORE0_INTR_MAIN2MCU_LVL_INTRTR0_OUTL_0;
    rmIrqReq.ia_id                  = 0U;
    rmIrqReq.vint                   = 0U;
    rmIrqReq.vint_status_bit_index  = 0U;
    rmIrqReq.secondary_host         = TISCI_MSG_VALUE_RM_UNUSED_SECONDARY_HOST;
    retVal = Sciclient_rmIrqSet(
                 &rmIrqReq, &rmIrqResp, APP_SCICLIENT_TIMEOUT);
    if(LLD_PASS != retVal)
    {
        AppUtils_Printf(MSG_NORMAL,APP_NAME
        ": Error in SciClient Interrupt Params Configuration!!!\n");
    }

    /* We will have to use the routed interrupt number
    subsequently */
    Osal_RegisterInterrupt_initParams(&intrPrms);

    intrPrms.corepacConfig.arg          = (uintptr_t)Pwm_Ch11Isr;
    intrPrms.corepacConfig.isrRoutine   = (void (*)(uintptr_t))Pwm_Ch11Isr;
    intrPrms.corepacConfig.priority     = 1U;
    intrPrms.corepacConfig.corepacEventNum = 0U; /* NOT USED  */
    intrPrms.corepacConfig.intVecNum = INTRTR_CFG_MAIN_DOMAIN_TIMER_1_OUT_INT_NO +
                                    (CSLR_MCU_R5FSS0_CORE0_INTR_MAIN2MCU_LVL_INTRTR0_OUTL_0);
    osalRetVal = Osal_RegisterInterrupt(&intrPrms, &hwiHandle);
    if(OSAL_INT_SUCCESS != osalRetVal)
    {
        AppUtils_Printf(MSG_NORMAL,APP_NAME
            ": Error in OSAL Interrupt Registration !!!\n");
    }
}
#endif

static void PwmApp_SetClockSource4Timers(void)
{
#ifdef MAIN_DOMAIN_TIMER_CLK_NOTFIREWALLED
    /* System clock frequency configured HFOSC0_CLK 25MHz)*/
    CSL_REG32_WR((CSL_CTRL_MMR0_CFG0_BASE+ CSL_MAIN_CTRL_MMR_CFG0_TIMER0_CLKSEL),
                  0x0U);
#endif

    return;
}

/** \brief Clear Main TIMER Timer */
static void PwmApp_ClearAnyOperationalTimers(void)
{
    /* Main domain TIMER is turned on by RBL and interrupts are enabled. We would
     *  require to disable the interrupt and clear Main domain timer, to ensure no
     *  un-expected interrupts are received
     *  Steps
     *  1. Disable the interrupt in TIMER
     *  2. Stop the timer
     *  3. Enable Soft Reset
     *  4. Reset the timer
     *  5. Reset the clock source
     */
    /* Steps 1 to 2 */
    CSL_REG32_WR((CSL_TIMER0_CFG_BASE + TIMER_IRQENABLE_CLR),
                    (TIMER_IRQENABLE_CLR_TCAR_EN_FLAG_MASK  |
                        TIMER_IRQENABLE_CLR_OVF_EN_FLAG_MASK   |
                        TIMER_IRQENABLE_CLR_MAT_EN_FLAG_MASK));
    CSL_REG32_WR((CSL_TIMER0_CFG_BASE + TIMER_TCLR), 0x0U);
    /* Steps 3 to 4 */
    CSL_REG32_WR((CSL_TIMER0_CFG_BASE + TIMER_TSICR),
                    TIMER_TSICR_SFT_MASK);
    CSL_REG32_WR((CSL_TIMER0_CFG_BASE + TIMER_TIOCP_CFG),
                    TIMER_TIOCP_CFG_SOFTRESET_MASK);
    /* Step 5. Reset the clock source also */
    /* Un Lock */
    uint32 regVal = 0U;
    /* Unlock lock key registers for Partition 7: IO PAD
       configuration registers in MAIN_CTRL_MMR */
    /* write Partition 7 Lock Key 0 Register */
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_LOCK7_KICK0,
                0x68EF3490);
    /* write Partition 7 Lock Key 1 Register */
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_LOCK7_KICK1,
                0xD172BC5A);
    /* Check for unlock */
    regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_LOCK7_KICK0);
    while ((regVal & 0x1) != 0x1U)
    {
		regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_LOCK7_KICK0);
    }
    /* Reset Clock source */
#ifdef MAIN_DOMAIN_TIMER_CLK_NOTFIREWALLED
    CSL_REG32_WR((CSL_TIMER0_CFG_BASE +
                        CSL_MAIN_CTRL_MMR_CFG0_TIMER0_CLKSEL),
                    CSL_MAIN_CTRL_MMR_CFG0_TIMER0_CLKSEL_RESETVAL);
#endif

    /* Enable the PIN MUX */

    /* Below code will be replaced by Port module in further releases */
    /* Pin mux configuration of Dio Pin 88 */
    regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_PADCONFIG58);
    regVal &= 0U;
    regVal |= 0x08050000U;
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_PADCONFIG58, regVal);
    /* Below code will be replaced by Port module in further releases */
    /* Controls Timer IO muxing. by default all timer pins on board are configured to timer0 */
    regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_TIMERIO0_CTRL);
    regVal &= 0U;
    regVal |= 0x0U;
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_TIMERIO0_CTRL, regVal);
}

/*EoF*/
