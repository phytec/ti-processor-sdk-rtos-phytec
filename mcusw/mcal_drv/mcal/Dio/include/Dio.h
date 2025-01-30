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
 *  \file     Dio.h
 *
 *  \brief    This file contains interface header for DIO MCAL driver
 *              to be used by other AUTOSAR MCAL Layers
 *
 */

/**
 *  \defgroup MCAL_DIO_API DIO Driver API
 *
 *  The DIO Driver provides services for reading and writing to/from
 *      -  DIO Channels (GPIO Pins)
 *      -  DIO Ports    (GPIO whole Registers)
 *      -  DIO Channel Groups (GPIO register submask)
 *
 *  This specification is applicable to drivers only for on chip DIO pins and
 *  ports <br>
 *  This module works on pins and ports which are configured by the PORT driver
 *  for this purpose.<br>
 *  For this reason, there is no configuration and initialization of this port
 *  structure in the DIO Driver <br>
 *  @sa MCAL_DIO_CFG
 *  @{
 */


/*
 * Below are the global requirements which are met by this DIO
 * driver which can't be mapped to a particular source ID
 */
/*
 * Design: MCAL-5468,MCAL-5428
 */
#ifndef DIO_H_
#define DIO_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/*
 * Design: MCAL-5509
 */
#include "Std_Types.h"
/*
 * Design: MCAL-5492,MCAL-5461
 */
#include "Dio_Cfg.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \name DIO Driver Module SW Version Info
 *
 *  Defines for DIO Driver version used for compatibility checks
 *  @{
 */
/** \brief Driver Implementation Major Version */
#define DIO_SW_MAJOR_VERSION            (10U)
/** \brief Driver Implementation Minor Version */
#define DIO_SW_MINOR_VERSION            (0U)
/** \brief Driver Implementation Patch Version */
#define DIO_SW_PATCH_VERSION            (0U)
/* @} */

/**
 *  \name DIO Driver Module AUTOSAR Version Info
 *
 *  Defines for DIO Driver AUTOSAR version used for compatibility checks
 *  @{
 */
/** \brief AUTOSAR Major version specification implemented by DIO Driver */
#define DIO_AR_RELEASE_MAJOR_VERSION    (4U)
/** \brief AUTOSAR Minor  version specification implemented by DIO Driver */
#define DIO_AR_RELEASE_MINOR_VERSION    (3U)
/** \brief AUTOSAR Patch version specification implemented by DIO Driver */
#define DIO_AR_RELEASE_REVISION_VERSION (1U)
/* @} */

/**
 *  \name DIO Driver ID Info
 *  @{
 */
/** \brief Texas Instruments Vendor ID */
#define DIO_VENDOR_ID                   ((uint16) 44U)
/** \brief DIO Driver Module ID */
#define DIO_MODULE_ID                   ((uint16) 120U)
/* @} */

/**
 *  \name Dio Error Codes
 *
 *  Error codes returned by Driver functions
 *  @{
 */
/*
 * Design: MCAL-5443
 */
#ifndef DIO_E_PARAM_INVALID_CHANNEL_ID
/** \brief ERROR:API service called with invalid channel ID */
#define DIO_E_PARAM_INVALID_CHANNEL_ID  (0x0AU)
#endif
/*
 * Design: MCAL-5441
 */
#ifndef DIO_E_PARAM_INVALID_PORT_ID
/** \brief ERROR:API service called with invalid port ID */
#define DIO_E_PARAM_INVALID_PORT_ID     (0x14U)
#endif
/*
 * Design: MCAL-5493
 */
#ifndef DIO_E_PARAM_INVALID_GROUP
/** \brief ERROR:API service called with invalid channel group ID */
#define DIO_E_PARAM_INVALID_GROUP       (0x1FU)
#endif
/*
 * Design: MCAL-5425
 */
#ifndef DIO_E_PARAM_POINTER
/** \brief ERROR:Dio_GetVersionInfo() called with NULL_PTR as parameter */
#define DIO_E_PARAM_POINTER             (0x20U)
#endif
/* @} */

/**
 *  \name Dio Service Ids
 *
 *  The Service Id is one of the argument to Det_ReportError function and is
 *  used to identify the source of the error
 *  @{
 */
/* DIO API service identification */
/** \brief Dio_ReadChannel() */
#define DIO_SID_READ_CHANNEL            (0U)
/** \brief Dio_WriteChannel() */
#define DIO_SID_WRITE_CHANNEL           (1U)
/** \brief Dio_ReadPort() */
#define DIO_SID_READ_PORT               (2U)
/** \brief Dio_WritePort() */
#define DIO_SID_WRITE_PORT              (3U)
/** \brief Dio_ReadChannelGroup() */
#define DIO_SID_READ_CHL_GROUP          (4U)
/** \brief Dio_WriteChannelGroup() */
#define DIO_SID_WRITE_CHL_GROUP         (5U)
/** \brief Dio_FlipChannel() */
#define DIO_SID_FLIP_CHANNEL            (7U)
/** \brief Dio_GetVersionInfo() */
#define DIO_SID_GET_VERSION_INFO        (18U)
/* @} */

/*
 * Design: MCAL-5447
 */
/** \brief Width of GPIO register in this platform
 *   not to be modified */
#define DIO_PORT_WIDTH                  (32U)

/** \brief Read back issue in Maxwell is resolved or not */
#define DIO_MCAL_2139_FIXED             (0U)
/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *  \addtogroup MCAL_DIO_CFG DIO Configuration
 *  @{
 */

/**
 *  \brief type definition for identifying DIO channels (meaning single port
 *  pins)
 */
/*
 * Design: MCAL-5503,MCAL-5436,MCAL-5446,MCAL-5449
 */
typedef uint32 Dio_ChannelType;

/** \brief type definition for identifying ports */
/*
 * Design: MCAL-5533,MCAL-5521,MCAL-5504,MCAL-5516
 */
typedef uint32 Dio_PortType;

/*
 * Design: MCAL-5486,MCAL-5484
 */
/**
 *  \brief type definition for setting port level values
 *
 *  This is a bitmask, where each bit with value '1' sets the corresponding port
 *  pin to HIGH and '0' sets the pin to LOW)
 */
typedef uint32 Dio_PortLevelType;
/*
 * Design: MCAL-5484,MCAL-5526,MCAL-5518
 */
/**
 *  \brief  Type definition for identifying channel groups (meaning a set of pins in one port)
 */
typedef struct
{
    Dio_PortType      port;
    /**< port ID of a channel group */
    Dio_PortLevelType mask;
    /**< bitmask where a '1' represents a pin handled by this channel group */
    uint8             offset;
    /**< shifts channel group values, so it is always "right" aligned */
} Dio_ChannelGroupType;

/*
 * Design: MCAL-5519,MCAL-5517
 */
/**
 *  \brief type definition for setting DIO channnel level values (allowed
 *  values: STD_HIGH and STD_LOW)
 */

typedef uint8 Dio_LevelType;

/*
** \name Values used to configure the direction of GPIO pins.
* @{
*/
typedef enum {
    DIO_DIR_INPUT  = 0x1U,
    DIO_DIR_OUTPUT = 0x0U
} Dio_DirectionType;
/* @} */

#if (STD_ON == DIO_REGISTER_READBACK_API)
typedef struct
{
    /*
     * DIO related registers
     */
    uint32      pid;
    /**< DIO Peripheral ID */
    Dio_DirectionType Dio_Direction;
    /**< Direction of the Pin */

} Dio_RegisterReadbackType;
#endif  /* #if (STD_ON == DIO_REGISTER_READBACK_API) */



/* @} */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief Returns the value of the specified DIO channel.
 *
 *  \verbatim
 *  Service name      : Dio_ReadChannel
 *  Syntax            : Dio_LevelType Dio_ReadChannel(
 *                          Dio_ChannelType ChannelId )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x00
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : ChannelId
 *                      ID of DIO channel
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Dio_LevelType - STD_HIGH The physical level of the
 *                      corresponding Pin is STD_HIGH
 *                      - STD_LOW The physical level of the corresponding Pin
 *                      is STD_LOW
 *  Description       : Returns the value of the specified DIO channel.
 *  \endverbatim
 */
FUNC(Dio_LevelType, DIO_CODE) Dio_ReadChannel(Dio_ChannelType ChannelId);

/**
 *  \brief Service to set a level of a channel.
 *
 *  \verbatim
 *  Service name      : Dio_WriteChannel
 *  Syntax            : void Dio_WriteChannel( Dio_ChannelType ChannelId,
 *                                             Dio_LevelType Level )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x01
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : ChannelId - ID of DIO channel
 *                      Level - Value to be written
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : Service to set a level of a channel.
 *  \endverbatim
 */
FUNC(void, DIO_CODE) Dio_WriteChannel(
    Dio_ChannelType ChannelId, Dio_LevelType Level);

/**
 *  \brief Returns the level of all channels of that port.
 *
 *  \verbatim
 *  Service name      : Dio_ReadPort
 *  Syntax            : Dio_PortLevelType Dio_ReadPort( Dio_PortType PortId )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x02
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : PortId - ID of DIO Port
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Dio_PortLevelType - Level of all channels of that port
 *  Description       : Returns the level of all channels of that port.
 *  \endverbatim
 */
FUNC(Dio_PortLevelType, DIO_CODE) Dio_ReadPort(Dio_PortType PortId);

/**
 *  \brief Service to set a value of the port.
 *
 *  \verbatim
 *  Service name      : Dio_WritePort
 *  Syntax            : void Dio_WritePort( Dio_PortType PortId,
 *                                          Dio_PortLevelType Level )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x03
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : PortId - ID of DIO Port
 *                      Level - Value to be written
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : Service to set a value of the port.
 *  \endverbatim
 */
FUNC(void, DIO_CODE) Dio_WritePort(
    Dio_PortType PortId, Dio_PortLevelType Level);

/**
 *  \brief This Service reads a subset of the adjoining bits of a port
 *
 *  \verbatim
 *  Service name      : Dio_ReadChannelGroup
 *  Syntax            : Dio_PortLevelType Dio_ReadChannelGroup(
 *                              const Dio_ChannelGroupType* ChannelGroupIdPtr )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x04
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : ChannelGroupIdPtr - Pointer to ChannelGroup
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Dio_PortLevelType - Level of a subset of the adjoining
 *                      bits of a port
 *  Description       : This Service read a subset of the adjoining bits of a
 *                      port.
 *  \endverbatim
 */
FUNC(Dio_PortLevelType, DIO_CODE) Dio_ReadChannelGroup(
    const Dio_ChannelGroupType *ChannelGroupIdPtr);

/**
 *  \brief Service to set a subset of the adjoining bits of a port to a
 *  specified level
 *
 *  \verbatim
 *  Service name      : Dio_WriteChannelGroup
 *  Syntax            : void Dio_WriteChannelGroup( const Dio_ChannelGroupType*
 *                              ChannelGroupIdPtr, Dio_PortLevelType Level )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x05
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : ChannelGroupIdPtr - Pointer to ChannelGroup
 *                      Level - Value to be written
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : Service to set a subset of the adjoining bits of a port
 *                      to a specified level
 *  \endverbatim
 */
FUNC(void, DIO_CODE) Dio_WriteChannelGroup(
    const Dio_ChannelGroupType *ChannelGroupIdPtr, Dio_PortLevelType Level);

#if (STD_ON == DIO_VERSION_INFO_API)
/**
 *  \brief Service to get the version information of this module
 *
 *  \verbatim
 *  Service name      : Dio_GetVersionInfo
 *  Syntax            : void Dio_GetVersionInfo(
 *                          Std_VersionInfoType* versioninfo )
 *  Mode              : User Mode (Non-Privileged Mode)
 *  Service ID[hex]   : 0x12
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : None
 *  Parameters (inout): None
 *  Parameters (out)  : versioninfo - Pointer to where to store the version
 *                      information of this module.
 *  Return value      : None
 *  Description       : Service to get the version information of this module
 *  \endverbatim
 */
FUNC(void, DIO_CODE) Dio_GetVersionInfo(
    P2VAR(Std_VersionInfoType, AUTOMATIC, DIO_APPL_DATA) versioninfo);
#endif

#if (STD_ON == DIO_FLIP_CHANNEL_API)
/**
 *  \brief Service to flip (change from 1 to 0 or from 0 to 1) the
 *   level of a channel and return the level of the channel
 *
 *  \verbatim
 *  Service name      : Dio_FlipChannel
 *  Syntax            : Dio_LevelType Dio_FlipChannel(
 *                              Dio_ChannelType ChannelId )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x11
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : ChannelId
 *                      - ID of DIO channel
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Dio_LevelType
 *  Return value      : Dio_LevelType - STD_HIGH The physical level of the
 *                      corresponding Pin is STD_HIGH
 *                      - STD_LOW The physical level of the corresponding Pin is
 *                      STD_LOW
 *  Description       : Service to flip (change from 1 to 0 or from 0 to 1) the
 *                      level of a channel and return the level of the channel
 *                      after flip
 *  \endverbatim
 */
FUNC(Dio_LevelType, DIO_CODE) Dio_FlipChannel(Dio_ChannelType ChannelId);
#endif

#if (STD_ON == DIO_REGISTER_READBACK_API)
/**
 *  \brief This function reads the important registers of the hardware unit
 *  and returns the value in the structure.
 *
 *  \verbatim
 *  This API should be called after DIO initialization in the app.
 *
 *  This API could be used to readback the register contents
 *  and then the readback value could be compared during DIO execution to
 *  check the correctness of the HW unit. Since this API is used for this
 *  purpose, the register returned are the ones which doesn't
 *  change during execution.
 *
 *  \verbatim
 *  Service name        : Dio_RegisterReadback
 *  Syntax              : uint32 Dio_RegisterReadback(
 *                              Dio_ChannelType ChannelId )
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : ChannelId
 *                       - ID of DIO channel
 *  Parameters (inout)  : None
 *  Parameters (out)    : Value of the PID register
 *  Return value        : uint32
 *  \endverbatim
 */
FUNC(Std_ReturnType, DIO_CODE) Dio_RegisterReadback(Dio_ChannelType ChannelId, Dio_RegisterReadbackType *DioRegRbPtr);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef DIO_H_ */

/* @} */
