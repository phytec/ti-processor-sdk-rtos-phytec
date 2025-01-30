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
 *  \file     Cpsw_Port.h
 *
 *  \brief    This file contains the structure definitions and function
 *            prototypes of the Port module.
 */

/**
 *  \addtogroup CPSW_PORT_API
 *  @{
 */

#ifndef CPSWMCAL_PORT_H_
#define CPSWMCAL_PORT_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "csl_cpsw.h"
#include "Cpsw_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */

/**
 *  \anchor Port_ConfigFlags
 *  \name Port configuration flags
 *
 *  Configuration flags that can be passed to the Port module when the module
 *  is being opened.
 *
 *  @{
 */

/** \brief Port 0 VLAN aware enable flag */
#define PORT_FLAG_VLANAWARE             (0x0001U)

/** \brief Port 0 pass priority tagged packets enable flag */
#define PORT_FLAG_P0PASSPRITAGGED       (0x0002U)

/** \brief Port 0 TX CRC remove flag */
#define PORT_FLAG_TXCRCREMOVE           (0x0004U)

/** \brief Port 0 RX short packet padding enable flag */
#define PORT_FLAG_P0RXPAD               (0x0008U)

/** \brief Port 0 pass CRC error packets enable flag */
#define PORT_FLAG_P0RXPASSCRCERR        (0x0010U)

/** \brief Port 1 pass priority tagged packets enable flag */
#define PORT_FLAG_P1PASSPRITAGGED       (0x0020U)

/*! @} */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *  \brief Port configuration
 *
 *  Configuration information for a Port
 */
typedef struct
{
    uint32 flags;
    /**< Port configuration flags \ref Port_ConfigFlags */
    uint32 pktMTU;
    /**< Packet MTU */
    uint8 macAddr[CPSW_MAC_ADDR_LEN];
    /**< Port MAC address (not applicable for Port 0) */
} CpswPort_Config;

/**
 *  \brief Port module
 *
 *  This is an internal/private driver structure and should not be used
 *  or modified by the caller.
 */
typedef struct
{
    CSL_Xge_cpswRegs *regs;
    /**< CPSW register overlay */
    uint32 num;
    /**< MAC port number */
    boolean isInit;
    /**< Module initialization state */
    CpswPort_Config cfg;
    /**< Saved configuration information */
} CpswPort;

/* ========================================================================== */
/*                         Global Variables Declarations                      */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief Host port open function
 *
 *  Opens the host Port module and performs an initial
 *  configuration according to the config structure being passed.
 *
 *  This function must be called before any other CpswPort_*() function.
 *
 *  \return \ref Cpsw_ErrorCodes
 */
void CpswPort_hostPortopen(void);

/**
 *  \brief Enable host port
 *
 *  Enables the host port.  It's applicable only to port 0, no action
 *  takes place if called on other ports.
 *
 *  \param num      Port number
 */
void CpswPort_enable(CpswPort_Num num);

/**
 *  \brief Disable host port
 *
 *  Disables the host port.  It's applicable only to port 0, no action
 *  takes place if called on other ports.
 *
 *  \param num      Port number
 *
 *  \return None
 */
void CpswPort_disable(CpswPort_Num num);

/**
 *  \brief Set address for MAC ports
 *
 *  Sets the MAC address of a port.  It's applicable only to MAC ports,
 *  no action takes place if called on host port.
 *
 *  \param num      Port number
 *  \param macAddr  MAC address
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType CpswPort_setMacAddr(CpswPort_Num num, const uint8 *macAddr);

/**
 *  \brief Set packet MTU
 *
 *  Sets the packet MTU for a given port.  It's equivalent to
 *  CpswMac_setMaxLen() if called on MAC ports.
 *
 *  \param num      Port number
 *  \param len      MTU (in bytes)
 */
void CpswPort_setMaxLen(CpswPort_Num num, uint32 len);

/**
 *  \brief Enable host port vlan/dscp priority control
 *
 *  Enable host port vlan/dscp priority control
 *
 *  \return None
 */
void CpswPort_enableHostPortPrioControl(void);

/**
 *  \brief Enable CPTS
 *
 *   Enable CPTS for a given port.
 *
 *  \param macPort     macport number
 *
 *  \return None
 */
void CpswPort_enableCpts(CpswPort_Num macPort);

/**
 *  \brief Disable CPTS
 *
 *   Disable CPTS for a given port.
 *
 *  \param macPort     macport number
 *
 *  \return None
 */
void CpswPort_disableCpts(CpswPort_Num macPort);

#if (STD_ON == ETH_TRAFFIC_SHAPING_API)
/**
 *  \brief Set limit bandwidth
 *
 *   Set limit bandwidth for a given port and given traffic priority
 *
 *  \param macPort                  macport number
 *  \param priority                 traffic priority
 *  \param bandwithLimitBitPerSec   bandwidth limit
 *
 *  \return None
 */
void CpswPort_setBandwidthLimit(uint32 macPort,
    uint8  priority,
    uint32 bandwithLimitBitPerSec);

/**
 *  \brief Get limit bandwidth
 *
 *   Get limit bandwidth for a given port and given traffic priority
 *
 *  \param macPort                      macport number
 *  \param priority                     traffic priority
 *  \param bandwithLimitBitPerSecPtr    bandwidth limit
 *
 *  \return None
 */
void CpswPort_getBandwidthLimit(uint32 macPort,
    uint8 priority,
    uint32 *bandwithLimitBitPerSecPtr);
#endif /* (STD_ON == ETH_TRAFFIC_SHAPING_API) */

/* ========================================================================== */
/*                        Deprecated Function Declarations                    */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif /* CPSWMCAL_PORT_H_ */

/*! @} */
