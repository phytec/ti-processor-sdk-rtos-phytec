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
 *  \file     Cpsw_Port.c
 *
 *  \brief    This file contains the Port module related functionality.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "cslr_xge_cpsw.h"
#include "csl_cpsw.h"
#include "cpsw/Cpsw_Types.h"
#include "cpsw/Cpsw_Port.h"
#include "Eth_Types.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \brief Port ratelimit dvide
 */
#define CPSW_PORT_RATELIM_DIV               (32768U)

/**
 *  \brief Port ratelimit roundup formular
 */
#define CPSW_PORT_DIV_ROUNDUP(val, div)     (((val) + (div) - 1) / (div))

/**
 *  \brief Port ratelimit max counter value
 */
#define CPSW_PORT_BANDWIDTH_CNT_MAX         (0x0FFFFFFFULL)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                  Internal Function Declarations                            */
/* ========================================================================== */

/**
 *  \brief Get base address of CPSW PORT register set
 *
 *  \return  Base address of CPSW PORT register set
 */
static inline CSL_Xge_cpswRegs* CpswPort_getRegAddr(void);

#if (STD_ON == ETH_TRAFFIC_SHAPING_API)
/**
 *  \brief Convert rate in bps to counter
 *
 *  \return  Counter coresponding to rate in bps
 */
static uint64 CpswPort_mapBwToCnt(uint64 rateInBps, uint32 cppiClkFreqHz);

/**
 *  \brief Convert counter to rate
 *
 *  \return  Counter coresponding to rate in bps
 */
static uint64 CpswPort_mapCntToBw(uint32 cntVal, uint32 cppiClkFreqHz);
#endif /* (STD_ON == ETH_TRAFFIC_SHAPING_API) */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

void CpswPort_hostPortopen(void)
{
    ETH_CSL_CPSW_CONTROL control;

    ETH_CSL_CPSW_getCpswControlReg(CpswPort_getRegAddr(), &control);

    control.vlanAware     = FALSE;
    control.p0TxCrcRemove = TRUE;
    control.p0RxPad       = TRUE;

    ETH_CSL_CPSW_setCpswControlReg(CpswPort_getRegAddr(), &control);

    CpswPort_enableHostPortPrioControl();
}

void CpswPort_enable(CpswPort_Num num)
{
    /* Only port 0 can be enabled/disabled, NOP for others */
    if (CPSW_PORT0 == num)
    {
        ETH_CSL_CPSW_enablePort0(CpswPort_getRegAddr());
    }
}

void CpswPort_disable(CpswPort_Num num)
{
    /* Only port 0 can be enabled/disabled, NOP for others */
    if (CPSW_PORT0 == num)
    {
        ETH_CSL_CPSW_disablePort0(CpswPort_getRegAddr());
    }
}

Std_ReturnType CpswPort_setMacAddr(CpswPort_Num num, const uint8 *macAddr)
{
    Std_ReturnType retVal = E_OK;

    /* Only MAC ports have a MAC address */
    if ((CPSW_PORT0 < num) && (CPSW_PORT_MAX > num))
    {
        ETH_CSL_CPSW_setPortMACAddress(CpswPort_getRegAddr(), ((uint32)num), (uint8 *)macAddr);
    }
    else
    {
        retVal = E_NOT_OK;
    }

    return retVal;
}

void CpswPort_setMaxLen(CpswPort_Num num, uint32 len)
{
    /* For MAC ports, this is equivalent to CpswMac_setMaxLen() */
    if (CPSW_PORT_MAX > num)
    {
        ETH_CSL_CPSW_setPortRxMaxLen(CpswPort_getRegAddr(), ((uint32)num), len);
    }
}

void CpswPort_enableHostPortPrioControl(void)
{
    ETH_CSL_CPSW_CPPI_P0_CONTROL control;

    control.p0RxRemapDscpIpv6 = TRUE;
    control.p0RxRemapDscpIpv4 = TRUE;
    control.p0RxRemapVlan = TRUE;
    control.p0DscpIpv6En = TRUE;
    control.p0DscpIpv4En = TRUE;
    control.p0RxEccErrEn = FALSE;
    control.p0TxEccErrEn = FALSE;
#if ((STD_ON == ETH_CTRL_ENABLE_OFFLOAD_CHECKSUM_TCP) || \
     (STD_ON == ETH_CTRL_ENABLE_OFFLOAD_CHECKSUM_UDP))
    control.p0RxChksumEn = TRUE;
#else
    control.p0RxChksumEn = FALSE;
#endif

    ETH_CSL_CPSW_setCppiP0Control(CpswPort_getRegAddr(), &control);
}

void CpswPort_enableCpts(CpswPort_Num macPort)
{
    ETH_CSL_CPSW_TSCONFIG timeSyncCfg;
    uint32 portNum = ((uint32)macPort) - ((uint32)ETH_MAC_PORT_FIRST);

    timeSyncCfg.tsTxHostEnable   = TRUE;
    timeSyncCfg.tsRxAnnexDEnable = TRUE;
    timeSyncCfg.tsRxAnnexEEnable = TRUE;
    timeSyncCfg.tsRxAnnexFEnable = TRUE;
    timeSyncCfg.tsTxAnnexDEnable = TRUE;
    timeSyncCfg.tsTxAnnexEEnable = TRUE;
    timeSyncCfg.tsTxAnnexFEnable = TRUE;
    timeSyncCfg.tsMsgTypeEnable  = 0xFFFFu;
    timeSyncCfg.tsSeqIdOffset    = 30u;
    timeSyncCfg.tsDomainOffset   = 4u;
    timeSyncCfg.tsMcastTypeEnable = 0u;
    timeSyncCfg.tsTTLNonzeroEnable = TRUE;
    timeSyncCfg.tsUniEnable = FALSE;
    timeSyncCfg.ts107Enable = FALSE;
    timeSyncCfg.ts129Enable = FALSE;
    timeSyncCfg.ts130Enable = FALSE;
    timeSyncCfg.ts131Enable = FALSE;
    timeSyncCfg.ts132Enable = FALSE;
    timeSyncCfg.ts319Enable = TRUE;
    timeSyncCfg.ts320Enable = TRUE;
    timeSyncCfg.tsLType2Enable = FALSE;
    timeSyncCfg.tsLType1 = 0;
    timeSyncCfg.tsRxVlanLType1Enable = 0;
    timeSyncCfg.tsRxVlanLType2Enable = FALSE;
    timeSyncCfg.tsTxVlanLType1Enable = 0;
    timeSyncCfg.tsTxVlanLType2Enable = FALSE;
    timeSyncCfg.tsVlanLType1 = 0u;
    timeSyncCfg.tsVlanLType2 = 0u;

    ETH_CSL_CPSW_setPortTimeSyncConfig(CpswPort_getRegAddr(), portNum + 1U, &timeSyncCfg);
}

void CpswPort_disableCpts(CpswPort_Num macPort)
{
    ETH_CSL_CPSW_TSCONFIG timeSyncCfg;
    uint32 portNum = ((uint32)macPort) - ((uint32)ETH_MAC_PORT_FIRST);

    timeSyncCfg.tsTxHostEnable   = FALSE;
    timeSyncCfg.tsRxAnnexDEnable = FALSE;
    timeSyncCfg.tsRxAnnexEEnable = FALSE;
    timeSyncCfg.tsRxAnnexFEnable = FALSE;
    timeSyncCfg.tsTxAnnexDEnable = FALSE;
    timeSyncCfg.tsTxAnnexEEnable = FALSE;
    timeSyncCfg.tsTxAnnexFEnable = FALSE;
    timeSyncCfg.tsMsgTypeEnable  = FALSE;

    ETH_CSL_CPSW_setPortTimeSyncConfig(CpswPort_getRegAddr(), portNum + 1U, &timeSyncCfg);
}

#if (STD_ON == ETH_TRAFFIC_SHAPING_API)
void CpswPort_setBandwidthLimit(uint32 macPort,
    uint8  priority,
    uint32 bandwithLimitBitPerSec)
{
    uint64 cir;
    uint32 portNum = macPort - 1u;

    cir = CpswPort_mapBwToCnt(bandwithLimitBitPerSec, Eth_Cpsw_GetCppiClockFreq());
    if(cir < CPSW_PORT_BANDWIDTH_CNT_MAX)
    {
        ETH_CSL_CPSW_setPriCirEir(CpswPort_getRegAddr(), portNum, priority, (uint32)cir, (uint32)0u);
    }
}

void CpswPort_getBandwidthLimit(uint32 macPort,
    uint8 priority,
    uint32 *bandwithLimitBitPerSecPtr)
{
    uint32 cir;
    uint32 eir;
    uint32 portNum = macPort - 1u;

    ETH_CSL_CPSW_getPriCirEir(CpswPort_getRegAddr(), portNum, priority, &cir, &eir);
    *bandwithLimitBitPerSecPtr = CpswPort_mapCntToBw(cir, Eth_Cpsw_GetCppiClockFreq());
}
#endif /* (STD_ON == ETH_TRAFFIC_SHAPING_API) */

/* ========================================================================== */
/*                  Internal Function Definitions                             */
/* ========================================================================== */

static inline CSL_Xge_cpswRegs* CpswPort_getRegAddr(void)
{
    return (CSL_Xge_cpswRegs*)Eth_Cpsw_GetCtrlRegAddr();
}

#if (STD_ON == ETH_TRAFFIC_SHAPING_API)
static uint64 CpswPort_mapBwToCnt(uint64 rateInBps, uint32 cppiClkFreqHz)
{
    return CPSW_PORT_DIV_ROUNDUP(rateInBps * CPSW_PORT_RATELIM_DIV, cppiClkFreqHz);
}

static uint64 CpswPort_mapCntToBw(uint32 cntVal, uint32 cppiClkFreqHz)
{
    return ((uint64)cntVal * cppiClkFreqHz) / CPSW_PORT_RATELIM_DIV;
}
#endif /* (STD_ON == ETH_TRAFFIC_SHAPING_API) */

