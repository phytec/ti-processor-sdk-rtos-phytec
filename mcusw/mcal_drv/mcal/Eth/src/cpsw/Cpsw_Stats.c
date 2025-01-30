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
 *  \file     Cpsw_Stats.c
 *
 *  \brief    This file contains statistics related functionality.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "cpsw/Cpsw_Types.h"
#include "cpsw/Cpsw_Stats.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \brief Maximum stats block element in 4 bytes
 */
#define CPSW_STATS_BLOCK_ELEM_NUM             (128U)
/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================= */
/*                            Internal Variables                              */
/* ========================================================================== */

/**
 *  \brief Use when retrieve stats from hardware
 *
 */
static ETH_CSL_CPSW_STATS portStats[CPSW_PORT_MAX];

/* ========================================================================== */
/*                  Internal Function Declarations                            */
/* ========================================================================== */

/**
 *  \brief Get base address of CPSW STATS register set
 *
 *  \return  Base address of CPSW STATS register set
 */
static inline CSL_Xge_cpswRegs* CpswStats_getRegAddr(void);

/**
 *  \brief Reset statistic variable
 *
 *  \return none
 */
static void CpswStats_resetStats(ETH_CSL_CPSW_STATS *stats);

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

void CpswStats_open(void)
{
    ETH_CSL_CPSW_PORTSTAT statEnable;
    uint32 Idx;

    /* Reset port stats */
    for(Idx = 0u; Idx < (uint32)CPSW_PORT_MAX; Idx++)
    {
        ETH_CSL_CPSW_getPortStats(CpswStats_getRegAddr(), Idx, &portStats[Idx]);
        CpswStats_resetStats(&portStats[Idx]);
    }

    /* initialized statEnable with the current port stats value */
    ETH_CSL_CPSW_getPortStatsEnableReg(CpswStats_getRegAddr(), &statEnable);

    /* Enable statistics for host port */
    statEnable.p0StatEnable = 1U;

    ETH_CSL_CPSW_setPortStatsEnableReg(CpswStats_getRegAddr(), &statEnable);
}

void CpswStats_close(void)
{
    ETH_CSL_CPSW_PORTSTAT statEnable;

    /* initialized statEnable with the current port stats value */
    ETH_CSL_CPSW_getPortStatsEnableReg(CpswStats_getRegAddr(), &statEnable);

    /* Disable statistics for host port */
    statEnable.p0StatEnable = 0U;
    ETH_CSL_CPSW_setPortStatsEnableReg(CpswStats_getRegAddr(), &statEnable);
}

void CpswStats_getStats(CpswPort_Num port, CpswStats_Stats *val)
{
    uint32 *readStats;
    uint32 *accStats;
    uint32 num = (uint32)port;
    uint32 i;

    if (CPSW_PORT_MAX > port)
    {
        readStats = (uint32 *)val;
        accStats = (uint32 *)&portStats[num];

        ETH_CSL_CPSW_getPortStats(CpswStats_getRegAddr(), num, (ETH_CSL_CPSW_STATS*)readStats);

        for (i = 0U; i < CPSW_STATS_BLOCK_ELEM_NUM; i++)
        {
            accStats[i] += readStats[i];
            readStats[i] = accStats[i];
        }
    }
}

Std_ReturnType CpswStats_setPort(CpswPort_Num port, boolean value)
{
    Std_ReturnType ret = E_OK;
    ETH_CSL_CPSW_PORTSTAT statEnable;

    ETH_CSL_CPSW_getPortStatsEnableReg(CpswStats_getRegAddr(), &statEnable);

    switch (port)
    {
    case CPSW_PORT0:
        statEnable.p0StatEnable = value;
        break;

    case CPSW_PORT1:
        statEnable.p1StatEnable = value;
        break;

    case CPSW_PORT2:
        statEnable.p2StatEnable = value;
        break;

    default:
        ret = E_NOT_OK;
        break;
    }

    if (E_OK == ret)
    {
        ETH_CSL_CPSW_setPortStatsEnableReg(CpswStats_getRegAddr(), &statEnable);
    }

    return  ret;
}

/* ========================================================================== */
/*                  Internal Function Definitions                             */
/* ========================================================================== */
static inline CSL_Xge_cpswRegs* CpswStats_getRegAddr(void)
{
    return (CSL_Xge_cpswRegs*)Eth_Cpsw_GetCtrlRegAddr();
}

static void CpswStats_resetStats(ETH_CSL_CPSW_STATS *stats)
{
    uint32 *un32StatsPtr = (uint32*)stats;
    uint32 Idx;

    for(Idx = 0; Idx < CPSW_STATS_BLOCK_ELEM_NUM; ++Idx)
    {
        un32StatsPtr[Idx] = 0u;
    }
}
