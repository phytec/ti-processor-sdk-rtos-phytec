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
#include <stdlib.h>
#include "app_utils.h"
#include "app_timer.h"

#include <hw_include/arch/r5/lld_arm_r5.h>
#include <hw_include/arch/r5/lld_arm_r5_pmu.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4) || defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)

/** \brief R5F Clock, platform/SoC dependent (1 GHz!!!) */
#define RF5_DEFAULT_CLOCK_IN_MHZ    (1000U * (1000U * 1000U))

#else

#error "Un Supported Soc"

#endif

#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4) || defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
/** \brief 1 = Divider is 64 and 0 = disabled */
#define R5F_COUNTER_TICK_DIVIDER    (0U)
#endif /* SOC_J72x */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */
    /* None */
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
uint32 AppUtils_IsCtrEnabled;

/* ========================================================================== */
/*                     External Function Implementation                       */
/* ========================================================================== */

#if defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
extern uint32_t ClockP_getTicks();
#endif

/* ========================================================================== */
/*                          Function Implementation                           */
/* ========================================================================== */

/** \brief Initializes the core internal timer (PMU in case of R5F)
 *
 *          This should be called before any profiling API's are called. (i.e.
 *          AppUtils_GetTimeStamp (), AppUtils_GetTimeStampFreq(),
 *          AppUtils_GetCycles()
 */
void AppUtils_TimerInit(uint32 unUsedForNow)
{
    #if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
    uint32 val;
    /*  cycleCntDiv : Divide ticks by 64, i.e.count every 64th clock is:
                                                        R5F_COUNTER_TICK_DIVIDER
        exportEvents : Export events to monitoring block of R5F : Disabled
                                                        0U
        userEnable  : Enabled user mode access : Enabled
                                                        1U
    */
    CSL_armR5PmuCfg(R5F_COUNTER_TICK_DIVIDER, 0U, 1);

    /* Clear the overflow */
    val = CSL_armR5PmuReadCntrOverflowStatus();
    val &= 0x80000000;
    CSL_armR5PmuClearCntrOverflowStatus(val);

    CSL_armR5PmuCfgCntr(
        CSL_ARM_R5_PMU_CYCLE_COUNTER_NUM, /* Counter ID for counting cycles */
        CSL_ARM_R5_PMU_EVENT_TYPE_CYCLE_CNT); /* Cfg for counting cycles */

    /* Clear the counter CSL_ARM_R5_PMU_CYCLE_COUNTER_NUM */
    CSL_armR5PmuEnableAllCntrs(1);

    AppUtils_IsCtrEnabled = 0U;
    #endif
}

uint32 AppUtils_TimerGetClkFreq (void)
{
    uint32 rtnVal = RF5_DEFAULT_CLOCK_IN_MHZ;
    #if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)

    if (1U == R5F_COUNTER_TICK_DIVIDER)
    {
        /* 64 is specific to R5F core and divider is not configurable */
        rtnVal = rtnVal / 64U;
    }
    #endif

    return (rtnVal);
}

uint32  AppUtils_TimerGetCurrTick (void)
{
    uint32 rtnVal = 0;;
    #if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)

    if (0U == AppUtils_IsCtrEnabled)
    {
        CSL_armR5PmuEnableCntr(CSL_ARM_R5_PMU_CYCLE_COUNTER_NUM, 1);
        AppUtils_IsCtrEnabled = 1U;
    }

    rtnVal = CSL_armR5PmuReadCntr(CSL_ARM_R5_PMU_CYCLE_COUNTER_NUM);
    #elif defined (SOC_AM62X) || defined (SOC_AM62AX)  || defined (SOC_AM62PX) || defined (SOC_J722S)

    rtnVal = ClockP_getTicks();

    #endif

    return (rtnVal);
}

/* ========================================================================== */
/*                             Local Functions                                */
/* ========================================================================== */
