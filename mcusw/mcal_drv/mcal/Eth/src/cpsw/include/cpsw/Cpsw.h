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
 *  \file     Cpsw.h
 *
 *  \brief    This file contains the main structure definitions and function
 *            prototypes of the CPSW driver.
 */

/**
 *  \defgroup CPSW_API Gigabit Ethernet Switch (CPSW) low-level APIs
 *
 *  The Gigabit Ethernet Switch (CPSW) is present in the Jacinto 7 device
 *  family.  This CPSW driver implementation supports the MCU_CPSW0 instance
 *  which is part of the MCU domain and provides two ports: an Ethernet port
 *  (port 1) and an internal CPPI port (port 0 or host port).
 *
 *  The Ethernet port supports RMII and RGMII interfaces at 100 and 1000
 *  Mbps.  The CPSW also includes an Address Lookup Engine (ALE) and an
 *  Ethernet Statistics submodule.
 *
 *  @{
 */

/**
 *  \defgroup CPSW_MAIN_API CPSW Global API
 */

/**
 *  \defgroup ETH_CPSW_ALE_API CPSW Address Lookup Engine (ALE) API
 */

/**
 *  \defgroup CPSW_DMA_API CPSW DMA API
 */

/**
 *  \defgroup CPSW_MAC_API CPSW MAC API
 */

/**
 *  \defgroup CPSW_MDIO_API CPSW Management Data I/O (MDIO) API
 */

/**
 *  \defgroup CPSW_PORT_API CPSW Port API
 */

/**
 *  \defgroup CPSW_STATS_API CPSW Statistics API
 */

/*! @} */

/**
 *  \addtogroup CPSW_MAIN_API
 *  @{
 */

#ifndef CPSWMCAL_H_
#define CPSWMCAL_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <Std_Types.h>
#include "Cpsw_Types.h"
#include "Cpsw_Ale.h"
#include "Cpsw_Dma.h"
#include "Cpsw_Mac.h"
#include "Cpsw_Mdio.h"
#include "Cpsw_Port.h"
#include "Cpsw_Stats.h"
#include "Cpsw_Cpts.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* ========================================================================== */
/*                         Global Variables Declarations                      */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* ========================================================================== */
/*                        Deprecated Function Declarations                    */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif /* CPSWMCAL_H_ */

/*! @} */
