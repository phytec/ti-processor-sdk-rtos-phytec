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
 *  \file     EthTrcv_Types.h
 *
 *  \brief    This file contains EthTrcv type declarations.
 */

/**
 *  \ingroup MCAL_ETHTRCV_API
 *  @{
 */

#ifndef ETHTRCV_TYPES_H_
#define ETHTRCV_TYPES_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Std_Types.h"
#include "Eth_GeneralTypes.h"
#include "EthTrcv_Cfg.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *  \brief Advertisement configurations
 */
/* Requirements: MCAL-1764 */
typedef enum
{
    ETHERNET_ADV_CAPAB_10_HALF   = 0x01U,
    /**< 10 Mbps half operation */
    ETHERNET_ADV_CAPAB_10_FULL   = 0x02U,
    /**< 10 Mbps full operation */
    ETHERNET_ADV_CAPAB_100_HALF  = 0x04U,
    /**< 100 Mbps half operation */
    ETHERNET_ADV_CAPAB_100_FULL  = 0x08U,
    /**< 100 Mbps full operation */
    ETHERNET_ADV_CAPAB_1000_HALF = 0x10U,
    /**< 1000 Mbps half operation */
    ETHERNET_ADV_CAPAB_1000_FULL = 0x20U
    /**< 1000 Mbps full operation */
} EthTrcv_AdvCapabType;

/**
 *  \brief Implementation specific structure of the eth transceiver controller configuration.
 */
/* Requirements: MCAL-1827 */
typedef struct EthTrcv_ControllerConfigType_s
{
    uint8 ctrlIdx;
    /**< Index of the controller */
    uint8 trcvIdx;
    /**< Index of the transceiver */
    uint8 miiIdx;
    /**< PHY address */
    uint32 macType;
    /**< PHY interface type */
    boolean enableAutoNeg;
    /**< PHY auto-negotiation enable */
    uint32 advertiseCapab;
    /**< PHY advertisement capability */
    uint32 linkPartCapab;
    /**< Link partner capability */
    boolean isGigCapab;
    /**< PHY gigabit capability */
    EthTrcv_BaudRateType baudRate;
    /**< PHY operating speed */
    EthTrcv_DuplexModeType duplexMode;
    /**< PHY operating duplexity */
    boolean loopback;
    /**< PHY loopback enable. For test purpose only */
    uint32 phyId;
    /**< PHY identification as per IEEE registers PHYIDR1 and PHYIDR2.
     *   It's the concatenation of OUI, model and revision numbers */
    void *extendedConfig;
    /**< PHY specific configuration (chosen by driver based on PHY ID) */
} EthTrcv_ControllerConfigType;

/**
 *  \brief Implementation specific structure of the post build configuration.
 */
/* Requirements: MCAL-1827 */
typedef struct EthTrcv_ConfigType_s
{
    EthTrcv_ControllerConfigType *pController[ETHTRCV_MAX_CONTROLLER];
} EthTrcv_ConfigType;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif  /* ETHTRCV_TYPES_H_ */

/*! @} */
