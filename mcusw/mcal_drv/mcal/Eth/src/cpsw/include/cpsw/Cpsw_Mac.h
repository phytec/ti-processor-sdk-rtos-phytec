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
 *  \file     Cpsw_Mac.h
 *
 *  \brief    This file contains the structure definitions and function
 *            prototypes of the MAC module.
 */

/**
 *  \addtogroup CPSW_MAC_API
 *  @{
 */

#ifndef CPSWMCAL_MAC_H_
#define CPSWMCAL_MAC_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "Eth_Types.h"
#include "cslr_xge_cpsw.h"
#include "csl_cpsw.h"
#include "Cpsw_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */

/**
 *  \anchor MAC_ConfigFlags
 *  \name MAC configuration flags
 *
 *  Configuration flags that can be passed to the MAC module when the module
 *  is being opened.
 *
 *  @{
 */

/** \brief Loopback enable flag */
#define MAC_FLAG_MACLOOPBACK            (0x0001U)

/** \brief Receive flow control enable flag */
#define MAC_FLAG_RXFLOWCNTL             (0x0002U)

/** \brief Transmit flow control enable flag */
#define MAC_FLAG_TXFLOWCNTL             (0x0004U)

/** \brief Transmit pacing enable flag */
#define MAC_FLAG_TXPACE                 (0x0008U)

/** \brief Command idle flag */
#define MAC_FLAG_CMDIDLE                (0x0010U)

/** \brief Castagnoli CRC type flag */
#define MAC_FLAG_CASTAGNOLI_CRC         (0x0020U)

/** \brief External receive flow control enable flag */
#define MAC_FLAG_EXTRXFLOWCNTL          (0x0040U)

/** \brief External transmit flow control enable flag */
#define MAC_FLAG_EXTTXFLOWCNTL          (0x0080U)

/** \brief Transmit short grap limit enable flag */
#define MAC_FLAG_TXSHORTGAPEN           (0x0100U)

/** \brief Pass short and error frames enable flag */
#define MAC_FLAG_PASSERROR              (0x0200U)

/** \brief Pass MAC control frames enable flag */
#define MAC_FLAG_PASSCONTROL            (0x0400U)

/*! @} */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *  \brief MAC connection type
 *
 *  Type of the MII connection
 */
typedef enum
{
    CPSWMCAL_MAC_CONN_TYPE_RMII_10                 = 0x01U,
    /**< RMII mode, 10Mbps */
    CPSWMCAL_MAC_CONN_TYPE_RMII_100                = 0x02U,
    /**< RMII mode, 100Mbps */
    CPSWMCAL_MAC_CONN_TYPE_RGMII_FORCE_100_HALF    = 0x03U,
    /**< RGMII mode, 100Mbps, half-duplex */
    CPSWMCAL_MAC_CONN_TYPE_RGMII_FORCE_100_FULL    = 0x04U,
    /**< RGMII mode, 100Mbps, full-duplex */
    CPSWMCAL_MAC_CONN_TYPE_RGMII_FORCE_1000_FULL   = 0x05U,
    /**< RGMII force mode, 1Gbps, full-duplex */
    CPSWMCAL_MAC_CONN_TYPE_RGMII_DETECT_INBAND     = 0x06U,
    /**< RGMII mode, in-band detect */
    CPSWMCAL_MAC_CONN_TYPE_INVALID                 = 0x00U
    /**< Invalid MII type */
} CpswMac_ConnType;

/**
 *  \brief MAC configuration
 *
 *  Configuration information for a MAC port.
 */
typedef struct
{
    uint32 flags;
    /**< MAC configuration flags \ref MAC_ConfigFlags */
    uint32 pktMTU;
    /**< Packet MTU */
    CpswMac_ConnType connType;
    /**< MAC connection type */
} CpswMac_Config;

/**
 *  \brief MAC module
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
    CpswMac_Config cfg;
    /**< Saved configuration information */
} CpswMac;

/* ========================================================================== */
/*                         Global Variables Declarations                      */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief MAC open function
 *
 *  Opens the MAC module for a given port and performs an initial configuration
 *  according to the config structure being passed.
 *
 *  This function must be called before any other CpswMac_*() function.
 *
 *  \param num      MAC port number
 *  \param cfg      Configuration information
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType CpswMac_open(uint32 num, Eth_MacConnectionType connType, uint32 flags);

/**
 *  \brief MAC close function
 *
 *  Closes the MAC module on the port it was originally opened.
 *
 *  No CpswMac_*() functions must be called after this function.
 *
 *  \param num      MAC port number
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType CpswMac_close(uint32 num);

/**
 *  \brief Set packet MTU
 *
 *  Sets the packet MTU for a given port.
 *
 *  \param num      MAC port number
 *  \param len      MTU (in bytes)
 */
void CpswMac_setMaxLen(uint32 num, uint32 len);

/**
 *  \brief Set loopback mode for a given port
 *
 *  Set loopback mode for a given port
 *
 *  \param num      MAC port number
 *
 */
void CpswMac_enableLoopback(uint32 num);


/* ========================================================================== */
/*                        Deprecated Function Declarations                    */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif /* CPSWMCAL_MAC_H_ */

/*! @} */
