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
 *  \file     Cpsw_Mac.c
 *
 *  \brief    This file contains MAC configuration related functionality.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "csl_cpsw.h"
#include "csl_cpgmac_sl.h"
#include "cpsw/Cpsw_Types.h"
#include "cpsw/Cpsw_Mac.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define CPSW_MAC_MAX_TIMEOUT 10000u

/* None */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief Get base address of CPSW MAC register set
 *
 *  \return  Base address of CPSW MAC register set
 */
static inline CSL_Xge_cpswRegs* CpswMac_getRegAddr(void);

/**
 *  \brief Set flags to MACCONTROL register value
 *
 *  Set the value to be written to the MACCONTROL register according to the
 *  flags passed.
 *
 *  \param val      MACCONTROL value
 *  \param flags    MAC configuration flags \ref MAC_ConfigFlags
 */
static void CpswMac_setControl(uint32 *val, uint32 flags);

/**
 *  \brief Set connection type to MACCONTROL register value
 *
 *  Set the value to be written to the MACCONTROL register according to the
 *  connection type.
 *
 *  \param val      MACCONTROL value
 *  \param type     Connection type
 */
static void CpswMac_updateConnType(uint32 *val, Eth_MacConnectionType type);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

Std_ReturnType CpswMac_open(uint32 num, Eth_MacConnectionType connType, uint32 flags)
{
    uint32 val;
    uint32 counter = 0u;
    Std_ReturnType retVal = E_OK;

    /* Soft reset the MAC */
    CSL_CPGMAC_SL_resetMac(CpswMac_getRegAddr(), num);

    while (CSL_CPGMAC_SL_isMACResetDone(CpswMac_getRegAddr(), num) == FALSE)
    {
        counter++;
        if(counter > CPSW_MAC_MAX_TIMEOUT)
        {
            retVal = E_NOT_OK;
            break;
        }
    }

    if (retVal == E_OK)
    {
        /* Set MAC control register with configuration flags and selected MII mode */
        val = CSL_CPGMAC_SL_getMacControlReg(CpswMac_getRegAddr(), num);
        CpswMac_setControl(&val, flags);
        CpswMac_updateConnType(&val, connType);
        CSL_CPGMAC_SL_setMacControlReg(CpswMac_getRegAddr(), num, val);
        CSL_CPGMAC_SL_enableGMII(CpswMac_getRegAddr(), num);
    }

    return retVal;
}

Std_ReturnType CpswMac_close(uint32 num)
{
    Std_ReturnType retVal = E_OK;
    uint32 counter = 0u;

    /* Soft reset the MAC */
    CSL_CPGMAC_SL_resetMac(CpswMac_getRegAddr(), num);
    while (CSL_CPGMAC_SL_isMACResetDone(CpswMac_getRegAddr(), num) == FALSE)
    {
        counter++;
        if(counter > CPSW_MAC_MAX_TIMEOUT)
        {
            retVal = E_NOT_OK;
            break;
        }
    }

    return retVal;
}

void CpswMac_setMaxLen(uint32 num, uint32 len)
{
    CSL_CPGMAC_SL_setRxMaxLen(CpswMac_getRegAddr(), num, len);
}

void CpswMac_enableLoopback(uint32 num)
{
    CSL_CPGMAC_SL_enableLoopback(CpswMac_getRegAddr(), num);
}

/* ========================================================================== */
/*                  Internal Function Definitions                             */
/* ========================================================================== */

static inline CSL_Xge_cpswRegs* CpswMac_getRegAddr(void)
{
    return (CSL_Xge_cpswRegs*)Eth_Cpsw_GetCtrlRegAddr();
}

static void CpswMac_setControl(uint32 *val, uint32 flags)
{
    *val &= ~(CSL_CPGMAC_SL_MACCONTROL_RX_CSF_EN |
              CSL_CPGMAC_SL_MACCONTROL_RX_CEF_EN |
              CSL_CPGMAC_SL_MACCONTROL_RX_CMF_EN |
              CSL_CPGMAC_SL_MACCONTROL_TX_SHORT_GAP_EN |
              CSL_CPGMAC_SL_MACCONTROL_EXT_EN_TX_FLOW |
              CSL_CPGMAC_SL_MACCONTROL_EXT_EN_RX_FLOW |
              CSL_CPGMAC_SL_MACCONTROL_CMD_IDLE_EN |
              CSL_CPGMAC_SL_MACCONTROL_CASTAGNOLI_CRC |
              CSL_CPGMAC_SL_MACCONTROL_TX_PACE_EN |
              CSL_CPGMAC_SL_MACCONTROL_TX_FLOW_EN |
              CSL_CPGMAC_SL_MACCONTROL_RX_FLOW_EN |
              CSL_CPGMAC_SL_MACCONTROL_LOOPBACK_EN);

    /* RX Copy MAC Control Frames */
    if (0U != (flags & MAC_FLAG_PASSCONTROL))
    {
        *val |= CSL_CPGMAC_SL_MACCONTROL_RX_CSF_EN;
    }

    /* RX Copy Short and Error Frames */
    if (0U != (flags & MAC_FLAG_PASSERROR))
    {
        *val |= (CSL_CPGMAC_SL_MACCONTROL_RX_CEF_EN |
                 CSL_CPGMAC_SL_MACCONTROL_RX_CMF_EN);
    }

    /* Transmit Short Gap Enable */
    if (0U != (flags & MAC_FLAG_TXSHORTGAPEN))
    {
        *val |= CSL_CPGMAC_SL_MACCONTROL_TX_SHORT_GAP_EN;
    }

    /* External Transmit Flow Control */
    if (0U != (flags & MAC_FLAG_EXTTXFLOWCNTL))
    {
        *val |= CSL_CPGMAC_SL_MACCONTROL_EXT_EN_TX_FLOW;
    }

    /* External Receive Flow Control */
    if (0U != (flags & MAC_FLAG_EXTRXFLOWCNTL))
    {
        *val |= CSL_CPGMAC_SL_MACCONTROL_EXT_EN_RX_FLOW;
    }

    /* CRC type */
    if (0U != (flags & MAC_FLAG_CASTAGNOLI_CRC))
    {
        *val |= CSL_CPGMAC_SL_MACCONTROL_CASTAGNOLI_CRC;
    }

    /* Command Idle */
    if (0U != (flags & MAC_FLAG_CMDIDLE))
    {
        *val |= CSL_CPGMAC_SL_MACCONTROL_CMD_IDLE_EN;
    }

    /* Transmit Pacing */
    if (0U != (flags & MAC_FLAG_TXPACE))
    {
        *val |= CSL_CPGMAC_SL_MACCONTROL_TX_PACE_EN;
    }

    /* Transmit Flow Control */
    if (0U != (flags & MAC_FLAG_TXFLOWCNTL))
    {
        *val |= CSL_CPGMAC_SL_MACCONTROL_TX_FLOW_EN;
    }

    /* Receive Flow Control */
    if (0U != (flags & MAC_FLAG_RXFLOWCNTL))
    {
        *val |= CSL_CPGMAC_SL_MACCONTROL_RX_FLOW_EN;
    }

    /* Loop Back */
    if (0U != (flags & MAC_FLAG_MACLOOPBACK))
    {
        *val |= CSL_CPGMAC_SL_MACCONTROL_LOOPBACK_EN;
    }
}

static void CpswMac_updateConnType(uint32 *val, Eth_MacConnectionType type)
{
    *val &= ~(CSL_CPGMAC_SL_MACCONTROL_EXT_EN |
              CSL_CPGMAC_SL_MACCONTROL_GIG_FORCE_EN |
              CSL_CPGMAC_SL_MACCONTROL_IFCTL_A_EN |
              CSL_CPGMAC_SL_MACCONTROL_GIG_EN |
              CSL_CPGMAC_SL_MACCONTROL_FULLDUPLEX_EN);

    switch (type)
    {
    case ETH_MAC_CONN_TYPE_RMII_10:
        *val |= CSL_CPGMAC_SL_MACCONTROL_FULLDUPLEX_EN;
        break;
    case ETH_MAC_CONN_TYPE_RMII_100:
        *val |= (CSL_CPGMAC_SL_MACCONTROL_FULLDUPLEX_EN |
                 CSL_CPGMAC_SL_MACCONTROL_IFCTL_A_EN);
        break;
    case ETH_MAC_CONN_TYPE_RGMII_FORCE_100_HALF:
        *val |= CSL_CPGMAC_SL_MACCONTROL_GIG_FORCE_EN;
        break;
    case ETH_MAC_CONN_TYPE_RGMII_FORCE_100_FULL:
        *val |= (CSL_CPGMAC_SL_MACCONTROL_FULLDUPLEX_EN |
                 CSL_CPGMAC_SL_MACCONTROL_GIG_FORCE_EN);
        break;
    case ETH_MAC_CONN_TYPE_RGMII_FORCE_1000_FULL:
        *val |= (CSL_CPGMAC_SL_MACCONTROL_FULLDUPLEX_EN |
                 CSL_CPGMAC_SL_MACCONTROL_GIG_EN |
                 CSL_CPGMAC_SL_MACCONTROL_GIG_FORCE_EN);
        break;
    case ETH_MAC_CONN_TYPE_RGMII_DETECT_INBAND:
        *val |= CSL_CPGMAC_SL_MACCONTROL_EXT_EN;
        break;
    default:
        break;
    }
}
