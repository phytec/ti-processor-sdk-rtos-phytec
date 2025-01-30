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
 *  \file     mcu_timer_multicore.c
 *
 *  \brief    This file implements profiling timer setup used by all cores.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Std_Types.h"

#include <ti/csl/csl_timer.h>

/* PM Lib */
#include <ti/drv/pm/include/pm_types.h>
#include <ti/drv/pm/include/dmsc/pmlib_sysconfig.h>
#include <ti/drv/pm/include/dmsc/pmlib_clkrate.h>

#include "app_utils.h"
#include "mcu_timer_multicore.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define CTRLMMR_MCU_TIMER0_CTRL	  (CSL_MCU_CTRL_MMR0_CFG0_BASE + 0x4200)
#define CTRLMMR_MCU_TIMER1_CTRL	  (CSL_MCU_CTRL_MMR0_CFG0_BASE + 0x4204)
#define CTRLMMR_MCU_TIMER2_CTRL	  (CSL_MCU_CTRL_MMR0_CFG0_BASE + 0x4208)
#define CTRLMMR_MCU_TIMER3_CTRL	  (CSL_MCU_CTRL_MMR0_CFG0_BASE + 0x420C)
#define CTRLMMR_MCU_TIMER0_CLKSEL (CSL_MCU_CTRL_MMR0_CFG0_BASE + 0x8100)
#define CTRLMMR_MCU_TIMER1_CLKSEL (CSL_MCU_CTRL_MMR0_CFG0_BASE + 0x8104)
#define CTRLMMR_MCU_TIMER2_CLKSEL (CSL_MCU_CTRL_MMR0_CFG0_BASE + 0x8108)
#define CTRLMMR_MCU_TIMER3_CLKSEL (CSL_MCU_CTRL_MMR0_CFG0_BASE + 0x810C)
#define MCU_TIMER_LOAD_VAL	      0

#define writel(x,y)               (*((uint64_t *)(y))=(x))
#define readl(x)                  (*((uint64_t *)(x)))

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

static struct mcu_timer *timer_base = (struct mcu_timer *)MCU_PROFILING_TIMER_BASE;

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */


/*
 * Start a counter
 */
int32_t mcu_timer_init(void)
{
    int32_t  retVal = 0;
    uint32_t timerId;
    uint32_t timerClkSet;
    uint64_t rcvdClkRate;
    uint64_t desiredClkRate;

    timerId        = TISCI_DEV_MCU_TIMER2;
    timerClkSet    = TISCI_DEV_MCU_TIMER2_TIMER_TCLK_CLK;
    desiredClkRate = (uint64_t)CLK_200M_RC_DEFAULT_FREQ;
    /* Set MCU Timer 2 to desiredClkRate (clock rates defined in mcu_timer_freq.h) */
    if (PM_SUCCESS != PMLIBClkRateSet(timerId, timerClkSet,
                                      desiredClkRate))
    {
        AppUtils_Printf(MSG_NORMAL,
                        "Could not set the clock source !!!\n");
        retVal = -1;
    }
    /* Check that Timer 2 clock is set to correct value */
    if (PM_SUCCESS != PMLIBClkRateGet(timerId, timerClkSet,
                                      &rcvdClkRate))
    {
        AppUtils_Printf(MSG_NORMAL,
                        "Could not get the Timer 2 clock source rate !!!\n");
        retVal = -1;
    }
    else
    {
        if (rcvdClkRate != desiredClkRate)
        {
            AppUtils_Printf(MSG_NORMAL,
                            "Timer 2 source rate of %jd does not match %jd !!!\n",
                            rcvdClkRate,
                            desiredClkRate);
            retVal = -1;
        }
    }

    if (retVal == 0)
    {
        /* configure timer for posted writes */
        writel(TSICR_POST, &timer_base->tsicr);
        /* start the counter ticking up, reload value on overflow */
        writel(MCU_TIMER_LOAD_VAL, &timer_base->tldr);
        /* enable timer */
        writel((MCU_TIMER_PTV << 2) | TCLR_PRE | TCLR_AR | TCLR_ST,
               &timer_base->tclr);
    }

	return retVal;
}

/*
 * Get timestamp in microseconds
 */
uint64_t get_usec_timestamp(void)
{
	uint64_t timestamp;
    uint64_t output;
	unsigned long count = readl(&timer_base->tcrr);

	/* Use correct multiplier to get resulting timestamp in microseconds */
	timestamp = (uint64_t)count * 1000000ULL;
    output = timestamp/MCU_TIMER_CLOCK;

	return output;
}
