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
 *  \file     Cpsw_Types.h
 *
 *  \brief    This file contains the generic definitions and structure
 *            definitions of the CPSW driver.
 */

/**
 *  \addtogroup CPSW_MAIN_API
 *  @{
 */

#ifndef CPSWMCAL_TYPES_H_
#define CPSWMCAL_TYPES_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Eth_Cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum Eth_Enet_Type_e
{
    /*! "Null" peripheral - for testing only */
    ETH_ENET_NULL,

    /*! ICSSG_MAC: 1 MAC port */
    ETH_ENET_ICSSG_DUALMAC,

    /*! ICSSG: 1 host port + 2 MAC ports */
    ETH_ENET_ICSSG_SWITCH,

    /*! GMAC_3G: 1 host port + 2 MAC ports */
    ETH_ENET_GMAC_3G,

    /*! CPSW_2G: 1 host port + 1 MAC port */
    ETH_ENET_CPSW_2G,

    /*! CPSW_3G: 1 host port + 2 MAC port */
    ETH_ENET_CPSW_3G,

    /*! CPSW_5G: 1 host port + 4 MAC ports */
    ETH_ENET_CPSW_5G,

    /*! CPSW_9G: 1 host port + 8 MAC ports */
    ETH_ENET_CPSW_9G,

    /*! Number of Ethernet Peripheral types - For internal use only */
    ETH_ENET_TYPE_COUNT,
} Eth_Enet_Type;


/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */

/**
 *  \anchor Cpsw_ErrorCodes
 *  \name CPSW Error Codes
 *
 *  Error codes returned by the CPSW driver APIs
 *
 *  @{
 */
/* For simplicity, some CPSW error codes mirror the UDMA's */
/** \brief Success */
#define CPSW_SOK                        (0u)

/** \brief Generic failure error condition (typically caused by hardware) */
#define CPSW_EFAIL                      (1u)

/** \brief Bad arguments (i.e. NULL pointer) */
#define CPSW_EBADARGS                   (2u)

/** \brief Invalid parameters (i.e. value out-of-range) */
#define CPSW_EINVALID_PARAMS            (3u)

/** \brief Time out while waiting for a given condition to happen */
#define CPSW_ETIMEOUT                   (4u)

/** \brief Allocation failure */
#define CPSW_EALLOC                     (5u)

/** \brief Unexpected condition occurred (sometimes unrecoverable) */
#define CPSW_EUNEXPECTED                (CPSW_EALLOC - 1u)

/** \brief The resource is currently busy performing an operation */
#define CPSW_EBUSY                      (CPSW_EALLOC - 2u)

/*! @} */

/** \brief CPSW memory alignment */
#define CPSW_MEM_ALIGNMENT              (128u)

/** \brief MAC address length (in bytes) */
#define CPSW_MAC_ADDR_LEN               (6U)

/**
 *  \brief CPSW Port number
 *
 *  CPSW Port number.
 */
typedef enum
{
    CPSW_PORT0     = 0U,
    /**< CPSW Port 0 (host port) */
    CPSW_PORT1     = 1U,
    /**< CPSW Port 1 (MAC port) */
    CPSW_PORT2     = 2U,
    /**< CPSW Port 1 (MAC port) */
    CPSW_PORT_MAX  = 3U
    /**< Max number of ports */
} CpswPort_Num;

/**
 *  \brief CPSW MAC port number
 *
 *  CPSW MAC port number.
 */
typedef enum
{
    CPSW_PORT1_MAC = 0U,
    /**< CPSW MAC port 0 */
    CPSW_PORT2_MAC = 1U,
    /**< CPSW MAC port 1 */
    CPSW_MAC_MAX   = 2U
    /**< Max number of MAC ports */
} CpswMac_Num;

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Global Variables Declarations                      */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                        Deprecated Function Declarations                    */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif /* CPSWMCAL_TYPES_H_ */

/*! @} */
