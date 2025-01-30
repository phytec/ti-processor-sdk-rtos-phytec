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
 *  \file     Cpsw_Ale.c
 *
 *  \brief    This file contains Address Lookup Engine (ALE) related
 *            functionality.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "csl_cpsw.h"
#include "cpsw/Cpsw_Types.h"
#include "cpsw/Cpsw_Ale.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *  \brief ALE entry type
 *
 *  The type of an ALE table entry.
 */
typedef union
{
    ETH_CSL_CPSW_ALE_MCASTADDR_ENTRY mcast;
    /**< Multicast address entry */
    ETH_CSL_CPSW_ALE_VLANMCASTADDR_ENTRY vlanMcast;
    /**< VLAN/Multicast address entry */
    ETH_CSL_CPSW_ALE_UNICASTADDR_ENTRY ucast;
    /**< Unicast address entry */
    ETH_CSL_CPSW_ALE_OUIADDR_ENTRY oui;
    /**< OUI unicast address entry */
    ETH_CSL_CPSW_ALE_VLANUNICASTADDR_ENTRY vlanUcast;
    /**< VLAN/Unicast address entry */
    ETH_CSL_CPSW_ALE_VLAN_ENTRY vlan;
    /**< VLAN entry */
} CpswAle_Entry;


/* ========================================================================== */
/*                            Internal Variables                              */
/* ========================================================================== */

static ETH_CSL_CPSW_ALETABLE_TYPE aleTableType = ETH_CSL_CPSW_ALETABLE_TYPE_4PORT;

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief Get base address of CPSW ALE register set
 *
 *  \return  Base address of CPSW ALE register set
 */
static inline CSL_AleRegs* CpswAle_getRegAddr(void);
/**
 *  \brief Find a free entry
 *
 *  Finds a free entry in the ALE table
 *
 *  \return         Entry index if found, otherwise ETH_CPSW_ALE_TABLE_SIZE
 */
static uint32 CpswAle_findFreeEntry(void);

/**
 *  \brief Find an entry
 *
 *  Finds a entry in the ALE table matching the given address and VLAN
 *
 *  \param addr     MAC address
 *  \param vlanId   VLAN id
 *
 *  \return         Entry index if found, otherwise ETH_CPSW_ALE_TABLE_SIZE
 */
static uint32 CpswAle_findEntry(const uint8 *addr, uint32 vlanId);

/**
 *  \brief Convert Eth MCAL Port State to CSL Port State
 *
 *  \param ethMcalPortState Eth MCAL Ale Port State
 *
 *  \return         CSL ALE Port State
 */
static ETH_CSL_CPSW_ALE_PORTSTATE CpswAle_getCslPortState(
                    Eth_CpswMcalAle_PortState ethMcalPortState);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

void CpswAle_open(void)
{
    uint32 val;

    /* Clear ALE table */
    ETH_CSL_CPSW_clearAleTable(CpswAle_getRegAddr());

    val = ETH_CSL_CPSW_getAleControlReg(CpswAle_getRegAddr());
    val &= ~(CSL_ALE_ALE_CONTROL_ENABLE_BYPASS_MASK |
             CSL_ALE_ALE_CONTROL_ALE_VLAN_AWARE_MASK |
             CSL_ALE_ALE_CONTROL_ENABLE_ALE_MASK);

    val |= CSL_ALE_ALE_CONTROL_ENABLE_ALE_MASK;
    val |= CSL_ALE_ALE_CONTROL_ALE_VLAN_AWARE_MASK;

    ETH_CSL_CPSW_setAleControlReg(CpswAle_getRegAddr(), val);
}

void CpswAle_close(void)
{
    /* Clear table and disable ALE */
    ETH_CSL_CPSW_clearAleTable(CpswAle_getRegAddr());
    ETH_CSL_CPSW_disableAle(CpswAle_getRegAddr());
}

void CpswAle_setBypass(boolean enable)
{
    if (TRUE == enable)
    {
        ETH_CSL_CPSW_enableAleBypass(CpswAle_getRegAddr());
    }
    else
    {
        ETH_CSL_CPSW_disableAleBypass(CpswAle_getRegAddr());
    }
}


boolean CpswAle_getBypass(void)
{
    return (boolean)(CSL_CPSW_isAleBypassEnabled(CpswAle_getRegAddr()));
}

void CpswAle_clearTable(void)
{
    ETH_CSL_CPSW_clearAleTable(CpswAle_getRegAddr());
}

void CpswAle_setPortState(CpswPort_Num port,
                          Eth_CpswMcalAle_PortState state)
{
    ETH_CSL_CPSW_ALE_PORTCONTROL control;
    ETH_CSL_CPSW_ALE_PORTSTATE cslPortState = CpswAle_getCslPortState(state);
    uint32 portNum = (uint32)port;

    ETH_CSL_CPSW_getAlePortControlReg(CpswAle_getRegAddr(), portNum, &control);
    control.portState = cslPortState;
    control.macOnlyEnable = (uint32)TRUE;
    ETH_CSL_CPSW_setAlePortControlReg(CpswAle_getRegAddr(), portNum, &control);
}

Std_ReturnType CpswAle_addUcastAddr(CpswPort_Num port,
                                    const uint8 *addr,
                                    uint32 vlanId,
                                    uint32 block,
                                    uint32 secure)
{
    Std_ReturnType retVal = E_OK;
    CpswAle_Entry entry;
    uint32 idx;

    /* Use existing entry or find a new free one */
    idx = CpswAle_findEntry(addr, vlanId);
    if (ETH_CPSW_ALE_TABLE_SIZE == idx)
    {
        idx = CpswAle_findFreeEntry();
    }

    /* Add the unicast entry */
    if (ETH_CPSW_ALE_TABLE_SIZE > idx)
    {
        if (0U == vlanId)
        {
            entry.ucast.macAddress[0u] = addr[0u];
            entry.ucast.macAddress[1u] = addr[1u];
            entry.ucast.macAddress[2u] = addr[2u];
            entry.ucast.macAddress[3u] = addr[3u];
            entry.ucast.macAddress[4u] = addr[4u];
            entry.ucast.macAddress[5u] = addr[5u];
            entry.ucast.ageable = 0U;
            entry.ucast.secureEnable = secure;
            entry.ucast.blockEnable = block;
            entry.ucast.portNumber = (uint32)port;
            entry.ucast.trunkFlag = 0U;
            ETH_CSL_CPSW_setAleUnicastAddrEntry(CpswAle_getRegAddr(), idx, &entry.ucast, aleTableType);
        }
        else
        {
            entry.vlanUcast.macAddress[0u] = addr[0u];
            entry.vlanUcast.macAddress[1u] = addr[1u];
            entry.vlanUcast.macAddress[2u] = addr[2u];
            entry.vlanUcast.macAddress[3u] = addr[3u];
            entry.vlanUcast.macAddress[4u] = addr[4u];
            entry.vlanUcast.macAddress[5u] = addr[5u];
            entry.vlanUcast.vlanId = vlanId;
            entry.vlanUcast.ageable = 0U;
            entry.vlanUcast.secureEnable = secure;
            entry.vlanUcast.blockEnable = block;
            entry.vlanUcast.portNumber = (uint32)port;
            entry.vlanUcast.trunkFlag = 0U;
            ETH_CSL_CPSW_setAleVlanUnicastAddrEntry(CpswAle_getRegAddr(), idx, &entry.vlanUcast, aleTableType);
        }
    }
    else
    {
        retVal = E_NOT_OK;
    }

    return retVal;
}

Std_ReturnType CpswAle_removeUcastAddr(const uint8 *addr,
                                       uint32 vlanId)
{
    return CpswAle_removeAddr(addr, vlanId);
}

Std_ReturnType CpswAle_addMcastAddr(const uint8 *addr,
                                    uint32 vlanIdParam,
                                    uint32 portMask,
                                    uint32 super,
                                    Eth_CpswMcalAle_PortState state)
{
    Std_ReturnType retVal = E_OK;
    CpswAle_Entry entry;
    boolean isVlanAware;
    uint32 vlanId = vlanIdParam;
    uint32 mask = 0U;
    uint32 idx;

    isVlanAware = (boolean) ETH_CSL_CPSW_isAleVlanAwareEnabled(CpswAle_getRegAddr());
    if (FALSE == isVlanAware)
    {
        vlanId = 0U;
    }

    /* Use existing entry or find a new free one */
    idx = CpswAle_findEntry(addr, vlanId);
    if (ETH_CPSW_ALE_TABLE_SIZE == idx)
    {
        idx = CpswAle_findFreeEntry();
        entry.mcast.macAddress[0u] = addr[0u];
        entry.mcast.macAddress[1u] = addr[1u];
        entry.mcast.macAddress[2u] = addr[2u];
        entry.mcast.macAddress[3u] = addr[3u];
        entry.mcast.macAddress[4u] = addr[4u];
        entry.mcast.macAddress[5u] = addr[5u];
        mask = portMask;
    }
    else
    {
        ETH_CSL_CPSW_getAleVlanMcastAddrEntry(CpswAle_getRegAddr(), idx, &entry.vlanMcast, aleTableType);
        mask = entry.mcast.portMask | portMask;
    }

    /* Add the multicast entry */
    if (ETH_CPSW_ALE_TABLE_SIZE > idx)
    {
        entry.mcast.mcastFwdState = (uint32)state;
        entry.mcast.superEnable = super;
        entry.mcast.portMask = mask;

        if (0U == vlanId)
        {
            ETH_CSL_CPSW_setAleMcastAddrEntry(CpswAle_getRegAddr(), idx, &entry.mcast,
                                          aleTableType);
        }
        else
        {
            entry.vlanMcast.macAddress[0u] = addr[0u];
            entry.vlanMcast.macAddress[1u] = addr[1u];
            entry.vlanMcast.macAddress[2u] = addr[2u];
            entry.vlanMcast.macAddress[3u] = addr[3u];
            entry.vlanMcast.macAddress[4u] = addr[4u];
            entry.vlanMcast.macAddress[5u] = addr[5u];
            entry.vlanMcast.vlanId = vlanId;
            ETH_CSL_CPSW_setAleVlanMcastAddrEntry(CpswAle_getRegAddr(), idx, &entry.vlanMcast, aleTableType);
        }
    }
    else
    {
        retVal = E_NOT_OK;
    }

    return retVal;
}

Std_ReturnType CpswAle_removeMcastAddr(const uint8 *addr,
                                       uint32 vlanId,
                                       uint32 portMask)
{
    Std_ReturnType retVal = E_OK;
    CpswAle_Entry entry;
    uint32 idx;

    /* Clear the port bit from the mask if an ALE entry is found */
    idx = CpswAle_findEntry(addr, vlanId);
    if (ETH_CPSW_ALE_TABLE_SIZE != idx)
    {
        if (0U == vlanId)
        {
            ETH_CSL_CPSW_getAleMcastAddrEntry(CpswAle_getRegAddr(), idx, &entry.mcast, aleTableType);
            entry.mcast.portMask &= ~portMask;
            if (0U == entry.mcast.portMask)
            {
                ETH_CSL_CPSW_clearAleEntry(CpswAle_getRegAddr(), idx);
            }
            else
            {
                ETH_CSL_CPSW_setAleMcastAddrEntry(CpswAle_getRegAddr(), idx, &entry.mcast, aleTableType);
            }
        }
        else
        {
            ETH_CSL_CPSW_getAleVlanMcastAddrEntry(CpswAle_getRegAddr(), idx, &entry.vlanMcast, aleTableType);
            entry.vlanMcast.portMask &= ~portMask;
            if (0U == entry.vlanMcast.portMask)
            {
                ETH_CSL_CPSW_clearAleEntry(CpswAle_getRegAddr(), idx);
            }
            else
            {
                ETH_CSL_CPSW_setAleVlanMcastAddrEntry(CpswAle_getRegAddr(), idx, &entry.vlanMcast, aleTableType);
            }
        }
    }
    else
    {
        retVal = E_NOT_OK;
    }

    return retVal;
}

Std_ReturnType CpswAle_removeAddr(const uint8 *addr,
                                  uint32 vlanId)
{
    Std_ReturnType retVal = E_OK;
    uint32 idx;

    /* Clear the ALE entry if a matching one was found */
    idx = CpswAle_findEntry(addr, vlanId);
    if (ETH_CPSW_ALE_TABLE_SIZE != idx)
    {
        ETH_CSL_CPSW_clearAleEntry(CpswAle_getRegAddr(), idx);
    }
    else
    {
        retVal = E_NOT_OK;
    }

    return retVal;
}

void CpswAle_setUnknownVlan(uint32 portMask)
{
    uint32 vlanList;
    uint32 mcastMask;
    uint32 regMcastMask;
    uint32 forceUntagged;

    ETH_CSL_CPSW_getAleUnkownVlanReg(CpswAle_getRegAddr(),
                                 &vlanList,
                                 &mcastMask,
                                 &regMcastMask,
                                 &forceUntagged);

    vlanList = portMask;

    ETH_CSL_CPSW_setAleUnkownVlanReg(CpswAle_getRegAddr(),
                                 vlanList,
                                 mcastMask,
                                 regMcastMask,
                                 forceUntagged);
}

void CpswAle_setUnknownRegMcastFlood(uint32 portMask)
{
    uint32 vlanList;
    uint32 mcastMask;
    uint32 regMcastMask;
    uint32 forceUntagged;

    ETH_CSL_CPSW_getAleUnkownVlanReg(CpswAle_getRegAddr(),
                                 &vlanList,
                                 &mcastMask,
                                 &regMcastMask,
                                 &forceUntagged);

    regMcastMask = portMask;

    ETH_CSL_CPSW_setAleUnkownVlanReg(CpswAle_getRegAddr(),
                                 vlanList,
                                 mcastMask,
                                 regMcastMask,
                                 forceUntagged);
}

void CpswAle_setPolicerPriorityThreadMapping(uint8 policerIdx,
                                             uint8 priority,
                                             uint32 threadId,
                                             uint8 portId)
{
    ETH_CSL_CPSW_ALE_POLICER_ENTRY polEntry;

    ETH_CSL_CPSW_getAleCOSPolicerEntry(CpswAle_getRegAddr(),
        policerIdx, &polEntry);

    polEntry.validBitmap = 0u;

    polEntry.validBitmap |= ETH_CSL_CPSW_ALE_POLICER_PRI_VALID;
    polEntry.validBitmap |= ETH_CSL_CPSW_ALE_POLICER_THREAD_VALID;
    polEntry.validBitmap |= ETH_CSL_CPSW_ALE_POLICER_PORT_VALID;
    polEntry.pri = priority;
    polEntry.thread = threadId;
    polEntry.port = portId;

    ETH_CSL_CPSW_setAleCOSPolicerEntry(CpswAle_getRegAddr(),
        policerIdx, &polEntry);
}

void CpswAle_deletePolicerThreadMapping(uint8 policerIdx)
{
    ETH_CSL_CPSW_ALE_POLICER_ENTRY polEntry;

    ETH_CSL_CPSW_getAleCOSPolicerEntry(CpswAle_getRegAddr(),
        policerIdx, &polEntry);

    polEntry.validBitmap = 0;

    ETH_CSL_CPSW_setAleCOSPolicerEntry(CpswAle_getRegAddr(),
        policerIdx, &polEntry);

    ETH_CSL_CPSW_disableAlePolicerThread(CpswAle_getRegAddr(),
        policerIdx);
}


/* ========================================================================== */
/*                  Internal Function Definitions                             */
/* ========================================================================== */

static inline CSL_AleRegs* CpswAle_getRegAddr(void)
{
    return (CSL_AleRegs *)Eth_Cpsw_GetAleRegAddr();
}

static uint32 CpswAle_findFreeEntry(void)
{
    uint32 idx = ETH_CPSW_ALE_TABLE_SIZE;
    ETH_CSL_CPSW_ALE_ENTRYTYPE type;
    uint32 i;

    for (i = 0U; i < ETH_CPSW_ALE_TABLE_SIZE; i++)
    {
        type = ETH_CSL_CPSW_getALEEntryType(CpswAle_getRegAddr(), i, aleTableType);
        if (ETH_CSL_ALE_ENTRYTYPE_FREE == type)
        {
            idx = i;
            break;
        }
    }

    return idx;
}

static uint32 CpswAle_findEntry(const uint8 *addr, uint32 vlanId)
{
    uint32 idx = ETH_CPSW_ALE_TABLE_SIZE;
    uint32 w0Addr, w1Addr;
    uint32 w2, w1, w0;
    uint32 type;
    uint32 i;

    w1Addr = (((uint32)addr[0U]) <<  8U) | ((uint32)addr[1U]);
    w0Addr = (((uint32)addr[2U]) << 24U) | (((uint32)addr[3U]) << 16U) |
             (((uint32)addr[4U]) <<  8U) | ((uint32)addr[5U]);

    for (i = 0U; i < ETH_CPSW_ALE_TABLE_SIZE; i++)
    {
        ETH_CSL_CPSW_getAleTableEntry(CpswAle_getRegAddr(), i, &w0, &w1, &w2);

        type = ((w1 >> 28U) & 0x3U);
        if ((((uint32)ETH_CSL_ALE_ENTRYTYPE_ADDRESS) == type) ||
            (((uint32)ETH_CSL_ALE_ENTRYTYPE_VLANADDRESS) == type))
        {
            /* Address and VLAN fields must match */
            if ((w1Addr == (w1 & 0x0000FFFFU)) &&
                (w0Addr == w0) &&
                (vlanId == ((w1 >> 16U) & 0xFFFU)))
            {
                idx = i;
                break;
            }
        }
    }

    return idx;
}

static ETH_CSL_CPSW_ALE_PORTSTATE CpswAle_getCslPortState(
                    Eth_CpswMcalAle_PortState ethMcalPortState)
{
    ETH_CSL_CPSW_ALE_PORTSTATE cslPortState = ETH_CSL_ALE_PORTSTATE_DISABLED;

    switch(ethMcalPortState)
    {
        case CPSWMCAL_ALE_PORTSTATE_DISABLED:
            cslPortState = ETH_CSL_ALE_PORTSTATE_DISABLED;
            break;
        case CPSWMCAL_ALE_PORTSTATE_BLOCKED:
            cslPortState = ETH_CSL_ALE_PORTSTATE_BLOCKED;
            break;
        case CPSWMCAL_ALE_PORTSTATE_LEARN:
            cslPortState = ETH_CSL_ALE_PORTSTATE_LEARN;
            break;
        case CPSWMCAL_ALE_PORTSTATE_FORWARD:
            cslPortState = ETH_CSL_ALE_PORTSTATE_FORWARD;
            break;
        default:
            break;
    }

    return cslPortState;
}

