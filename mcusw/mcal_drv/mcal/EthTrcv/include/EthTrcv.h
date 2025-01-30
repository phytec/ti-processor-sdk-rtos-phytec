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
 *  \file     EthTrcv.h
 *
 *  \brief    This file contains interface header for Ethernet Transceiver
 *            MCAL driver.
 */

/**
 *
 * \defgroup MCAL_ETHTRCV_API Ethernet Transceiver Driver API
 *
 * The EthTrcv Driver MCAL provides low level access to Ethernet transceiver
 * present on Jacinto 7 device family EVMs.<br>
 * The EthTrcv Driver implements a standardized interface specified in the
 * AUTOSAR_SWS_EthernetTransceiverDriver document.<br>
 * The EthTrcv Driver is part of the Communication Stack in the AUTOSAR Basic
 * Software (BSW).<br>
 *
 *  \sa MCAL_ETHTRCV_CFG
 *  @{
 */

/*
 * Below are the global requirements which are met by this EthTrcv
 * driver which can't be mapped to a particular source ID
 */
/*
 * Requirements: MCAL-1751, MCAL-1754, MCAL-1755,
 *               MCAL-1756, MCAL-1757, MCAL-1818, MCAL-981
 */

#ifndef ETHTRCV_H_
#define ETHTRCV_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* Requirements: MCAL-1825 */
#include "EthTrcv_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \name EthTrcv Driver Module SW Version Info
 *
 *  Defines for ETHTRCV Driver version used for compatibility checks
 *  @{
 */
/*
 * Requirements: MCAL-1752
 */
/** \brief Driver Implementation Major Version */
#define ETHTRCV_SW_MAJOR_VERSION        (9U)
/** \brief Driver Implementation Minor Version */
#define ETHTRCV_SW_MINOR_VERSION        (2U)
/** \brief Driver Implementation Patch Version */
#define ETHTRCV_SW_PATCH_VERSION        (0U)
/*! @} */

/**
 *  \name EthTrcv Driver Module AUTOSAR Version Info
 *
 *  Defines for ETHTRCV Driver AUTOSAR version used for compatibility checks
 *  @{
 */
/*
 * Requirements: MCAL-1752
 */
/** \brief AUTOSAR Major Version implemented */
#define ETHTRCV_AR_RELEASE_MAJOR_VERSION      (4U)
/** \brief AUTOSAR Minor Version implemented */
#define ETHTRCV_AR_RELEASE_MINOR_VERSION      (3U)
/** \brief AUTOSAR patch Version implemented */
#define ETHTRCV_AR_RELEASE_REVISION_VERSION   (1U)
/*! @} */

/**
 *  \name EthTrcv Driver ID Info
 *  @{
 */
/** \brief Texas Instruments Vendor ID */
#define ETHTRCV_VENDOR_ID               (44U)
/** \brief EthTrcv Driver  ID */
#define ETHTRCV_MODULE_ID               (73U)
/** \brief EthTrcv Instace ID */
#define ETHTRCV_INSTANCE_ID             (0U)
/*! @} */

/**
 *  \addtogroup MCAL_ETHTRCV_CFG EthTrcv Configuration
 *  @{
 */

/**
 *  \name EthTrcv Driver Service Id
 *
 *  The Service Id is one of the argument to Det_ReportError function and is
 *  used to identify the source of the error
 *  @{
 */
/** \brief EthTrcv_Init() API Service ID */
#define ETHTRCV_INIT_ID                 ((uint8) 0x01U)

/** \brief EthTrcv_Init() API Service ID */
#define ETHTRCV_CONTROLLER_INIT_ID      ((uint8) 0x02U)

/** \brief EthTrcv_SetTransceiverMode() API Service ID */
#define ETHTRCV_SETTRCVMODE_ID          ((uint8) 0x03U)

/** \brief EthTrcv_GetTransceiverMode() API Service ID */
#define ETHTRCV_GETTRCVMODE_ID          ((uint8) 0x04U)

/** \brief EthTrcv_StartAutoNegotiation() API Service ID */
#define ETHTRCV_SRTAUTONEGOTIA_ID       ((uint8) 0x05U)

/** \brief EthTrcv_GetLinkState() API Service ID */
#define ETHTRCV_GETLNKSTATE_ID          ((uint8) 0x06U)

/** \brief EthTrcv_GetBaudRate() API Service ID */
#define ETHTRCV_GETBAUD_ID              ((uint8) 0x07U)

/** \brief EthTrcv_GetDuplexMode() API Service ID */
#define ETHTRCV_GETDPLXMODE_ID          ((uint8) 0x08U)

/** \brief EthTrcv_ReadMiiIndication callback() API Service ID */
#define ETHTRCV_READMIIIND_ID           ((uint8) 0x09U)

/** \brief EthTrcv_WriteMiiIndication callback() API Service ID */
#define ETHTRCV_WRITEMIIIND_ID          ((uint8) 0x0AU)

/** \brief EthTrcv_GetVersionInfo() API Service ID */
#define ETHTRCV_VERSION_ID              ((uint8) 0x0BU)

/** \brief EthTrcv_MainFunction() API Service ID */
#define ETHTRCV_MAINFCT_WRITE_ID        ((uint8) 0x0CU)

/** \brief EthTrcv_GetTransceiverWakeupMode() API Service ID */
#define ETHTRCV_GETTRCVWKUPMODE_ID      ((uint8) 0x0DU)

/** \brief  EthTrcv_SetTransceiverWakeupMode() API Service ID */
#define ETHTRCV_SETTRCVWKUPMODE_ID      ((uint8) 0x0EU)

/** \brief EthTrcv_CheckWakeup() API Service ID */
#define ETHTRCV_CHKWKUP_ID              ((uint8) 0x0FU)

/** \brief EthTrcv_SetPhyTestMode() API Service ID */
#define ETHTRCV_SETPHYTESTMODE_ID       ((uint8) 0x10U)

/** \brief EthTrcv_SetPhyLoopbackMode() API Service ID */
#define ETHTRCV_SETPHYLOOPBACKMODE_ID   ((uint8) 0x11U)

/** \brief EthTrcv_GetPhySignalQuality() API Service ID */
#define ETHTRCV_GETPHYSIGNALQUALITY_ID  ((uint8) 0x12U)

/** \brief EthTrcv_SetPhyTxMode() API Service ID */
#define ETHTRCV_SETPHYTXMODE_ID         ((uint8) 0x13U)

/** \brief EthTrcv_GetCableDiagnosticsResult() API Service ID */
#define ETHTRCV_GETCABLEDIAGNOSTICSRESULT_ID     ((uint8) 0x014U)

/** \brief EthTrcv_GetPhyIdentifier() API Service ID */
#define ETHTRCV_GETPHYIDENTIFIER_ID              ((uint8) 0x015U)

/*! @} */
/*! @} */

/**
 *  \name EthTrcv Error Codes
 *
 *  Error codes returned by EthTrcv functions
 *  @{
 */
/*
 * Requirements: MCAL-1763
 */

#ifndef ETHTRCV_E_INV_TRCV_IDX
/** \brief Invalid transceiver index */
#define ETHTRCV_E_INV_TRCV_IDX          ((uint8) 0x01U)
#endif

#ifndef ETHTRCV_E_UNINIT
/** \brief EthTrcv module was not initialized */
#define ETHTRCV_E_UNINIT       ((uint8) 0x02U)
#endif

#ifndef ETHTRCV_E_PARAM_POINTER
/**< brief Invalid pointer in parameter list */
#define ETHTRCV_E_PARAM_POINTER         ((uint8) 0x03U)
#endif

#ifndef ETHTRCV_E_INIT_FAILED
/**< brief Initialization failure */
#define ETHTRCV_E_INIT_FAILED           ((uint8) 0x04U)
#endif
/*! @} */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * \brief This function initializes the Ethernet Transceiver controller. All
 *        the required initialization for the transceiver driver is performed
 *        in this function.
 *
 * \verbatim
 *  Service name      : EthTrcv_Init
 *  Syntax            : void EthTrcv_Init(
 *                          const EthTrcv_ConfigType* ConfigPtr
 *                      )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x01
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : CfgPtr. Points to the implementation specific structure
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : Initializes the Ethernet Transceiver Driver.
 * \endverbatim
 */
FUNC(void, ETHTRCV_CODE)
EthTrcv_Init(P2CONST(EthTrcv_ConfigType, AUTOMATIC, ETHTRCV_PBCFG) CfgPtr);

/**
 * \brief This function initializes the Ethernet Transceiver controller. All
 *        the required initialization for the transceiver driver is performed
 *        in this function.
 *
 * \verbatim
 *  Service name      : EthTrcv_Init
 *  Syntax            : void EthTrcv_TransceiverInit(
 *                          uint8 TrcvIdx
 *                      )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x02
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : TrcvIdx tranceiver controller index
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Std_ReturnType
 *                        E_OK: Service accepted
 *                        E_NOT_OK: Service denied
 *  Description       : Initializes the Ethernet Transceiver Driver.
 * \endverbatim
 */
FUNC(Std_ReturnType, ETHTRCV_CODE)
EthTrcv_TransceiverInit(uint8 TrcvIdx);


#if (STD_ON == ETHTRCV_SETTRANSCEIVERMODE_API)
/**
 * \brief This function enables / disables the indexed transceiver
 *
 * \verbatim
 *  Service name      : EthTrcv_SetTransceiverMode
 *  Syntax            : Std_ReturnType EthTrcv_SetTransceiverMode(
 *                          uint8 TrcvIdx,
 *                          EthTrcv_ModeType CtrlMode
 *                      )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x03
 *  Sync/Async        : Asynchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : TrcvIdx. Index of the transceiver within the context
 *                               of the Ethernet Transceiver Driver
 *                      CtrlMode
 *                        ETHTRCV_MODE_DOWN: disable the transceiver
 *                        ETHTRCV_MODE_ACTIVE: enable the transceiver
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Std_ReturnType
 *                        E_OK: Service accepted
 *                        E_NOT_OK: Service denied
 *  Description       : Enables / disables the indexed transceiver.
 * \endverbatim
 */
FUNC(Std_ReturnType, ETHTRCV_CODE)
EthTrcv_SetTransceiverMode(uint8 TrcvIdx,
                           EthTrcv_ModeType CtrlMode);
#endif

#if (STD_ON == ETHTRCV_GETTRANSCEIVERMODE_API)
/**
 * \brief This function obtains the state of the indexed transceiver
 *
 * \verbatim
 *  Service name      : EthTrcv_GetTransceiverMode
 *  Syntax            : Std_ReturnType EthTrcv_GetTransceiverMode(
 *                          uint8 TrcvIdx,
 *                          EthTrcv_ModeType* TrcvModePtr
 *                      )
 *  Mode              : User Mode (Non-Privileged Mode)
 *  Service ID[hex]   : 0x04
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : TrcvIdx. Index of the transceiver within the context
 *                               of the Ethernet Transceiver Driver
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: transceiver could not be initialized
 *  Description       : Obtains the state of the indexed transceiver.
 * \endverbatim
 */
FUNC(Std_ReturnType, ETHTRCV_CODE)
EthTrcv_GetTransceiverMode(uint8 TrcvIdx,
                           EthTrcv_ModeType *TrcvModePtr);
#endif

#if (ETHTRCV_WAKEUPSUPPORT_API != ETHTRCV_WAKEUP_NOT_SUPPORTED)
/**
 * \brief This function enables / disables the wake-up mode or clear the
 *        wake-up reason of the indexed transceiver
 *
 * \verbatim
 *  Service name      : EthTrcv_SetTransceiverWakeupMode
 *  Syntax            : Std_ReturnType EthTrcv_SetTransceiverWakeupMode(
 *                          uint8 TrcvIdx,
 *                          EthTrcv_WakeupModeType TrcvWakeupMode
 *                      )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x0d
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : TrcvIdx. Index of the transceiver within the context
 *                               of the Ethernet Transceiver Driver
 *                      TrcvWakeupMode
 *                        ETHTRCV_WUM_DISABLE: disable transceiver wake up
 *                        ETHTRCV_WUM_ENABLE: enable transceiver wake up
 *                        ETHTRCV_WUM_CLEAR: clears transceiver wake up reason
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Std_ReturnType
 *                        E_OK: transceiver wake up mode has been changed
 *                        E_NOT_OK: transceiver wake up mode could not be
 *                                  changed or the wake-up reason could not be
 *                                  cleared
 *  Description       : Enables / disables the wake-up mode or clear the wake-up
 *                      reason of the indexed transceiver.
 * \endverbatim
 */
FUNC(Std_ReturnType, ETHTRCV_CODE)
EthTrcv_SetTransceiverWakeupMode(uint8 TrcvIdx,
                                 EthTrcv_WakeupModeType TrcvWakeupMode);
#endif

#if (ETHTRCV_WAKEUPSUPPORT_API != ETHTRCV_WAKEUP_NOT_SUPPORTED)
/**
 * \brief This function returns the wake up mode of the indexed transceiver
 *
 * \verbatim
 *  Service name      : EthTrcv_GetTransceiverWakeupMode
 *  Syntax            : Std_ReturnType EthTrcv_GetTransceiverWakeupMode(
 *                          uint8 TrcvIdx,
 *                          EthTrcv_WakeupModeType* TrcvWakeupModePtr
 *                      )
 *  Mode              : User Mode (Non-Privileged Mode)
 *  Service ID[hex]   : 0x0e
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : TrcvIdx. Index of the transceiver within the context
 *                               of the Ethernet Transceiver Driver
 *  Parameters (inout): None
 *  Parameters (out)  : TrcvWakeupModePtr
 *                        ETHTRCV_WUM_DISABLE: transceiver wake up is disabled
 *                        ETHTRCV_WUM_ENABLE: transceiver wake up is enabled
 *  Return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: transceiver wake up mode could not be
 *                                  obtained
 *  Description       : Returns the wake up mode of the indexed transceiver.
 * \endverbatim
 */
FUNC(Std_ReturnType, ETHTRCV_CODE)
EthTrcv_GetTransceiverWakeupMode(uint8 TrcvIdx,
                                 EthTrcv_WakeupModeType *TrcvWakeupModePtr);
#endif

#if (ETHTRCV_WAKEUPSUPPORT_API != ETHTRCV_WAKEUP_NOT_SUPPORTED)
/**
 * \brief This service is called by EthIf in case a wake-up interrupt is
 *        detected.
 *
 * \verbatim
 *  Service name      : EthTrcv_CheckWakeup
 *  Syntax            : Std_ReturnType EthTrcv_CheckWakeup(
 *                          uint8 TrcvIdx
 *                      )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x0f
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : TrcvIdx. Index of the transceiver within the context
 *                               of the Ethernet Transceiver Driver
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Std_ReturnType
 *                        E_OK: The function has been successfully executed
 *                        E_NOT_OK: The function could not be successfully
 *                                  executed
 *  Description       : Service is called by EthIf in case a wake-up interrupt
 *                      is detected.
 * \endverbatim
 */
FUNC(Std_ReturnType, ETHTRCV_CODE)
EthTrcv_CheckWakeup(uint8 TrcvIdx);
#endif

/* Requirements: MCAL-1820 */
/* The function is not required or called by an upper layer BSW software
 * component. So making internal */
#if (STD_ON == ETHTRCV_STARTAUTONEGOTIATION_API)
/**
 * \brief This function restarts the negotiation of the transmission parameters
 *        used by the indexed.
 *
 * \verbatim
 *  Service name      : EthTrcv_StartAutoNegotiation
 *  Syntax            : Std_ReturnType EthTrcv_StartAutoNegotiation(
 *                          uint8 TrcvIdx
 *                      )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x05
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : TrcvIdx. Index of the transceiver within the context
 *                              of the Ethernet Transceiver Driver
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: transceiver could not be initialized
 *  Description       : Restarts the negotiation of the transmission parameters
 *                      used by the indexed.
 * \endverbatim
 */
FUNC(Std_ReturnType, ETHTRCV_CODE)
EthTrcv_StartAutoNegotiation(uint8 TrcvIdx);
#endif

#if (STD_ON == ETHTRCV_GETLINKSTATE_API)
/**
 * \brief This function obtains the link state of the indexed transceiver
 *
 * \verbatim
 *  Service name      : EthTrcv_GetLinkState
 *  Syntax            : Std_ReturnType EthTrcv_GetLinkState(
 *                          uint8 TrcvIdx,
 *                          EthTrcv_LinkStateType* LinkStatePtr
 *                      )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x06
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : TrcvIdx. Index of the transceiver within the context
 *                               of the Ethernet Transceiver Driver
 *  Parameters (inout): None
 *  Parameters (out)  : LinkStatePtr
 *                        ETHTRCV_LINK_STATE_DOWN: transceiver is disconnected
 *                        ETHTRCV_LINK_STATE_ACTIVE: transceiver is connected
 *  Return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: transceiver could not be initialized
 *  Description       : Obtains the link state of the indexed transceiver.
 * \endverbatim
 */
FUNC(Std_ReturnType, ETHTRCV_CODE)
EthTrcv_GetLinkState(uint8 TrcvIdx,
                     EthTrcv_LinkStateType *LinkStatePtr);
#endif

/*
 * Requirements: MCAL-1821
 */
/* The function is not required or called by an upper layer BSW software
 * component. So making internal */
#if (STD_ON == ETHTRCV_GETBAUDRATE_API)
/**
 * \brief This function obtains the baud rate of the indexed transceiver
 *
 * \verbatim
 *  Service name      : EthTrcv_GetBaudRate
 *  Syntax            : Std_ReturnType EthTrcv_GetBaudRate(
 *                          uint8 TrcvIdx,
 *                          EthTrcv_BaudRateType* BaudRatePtr
 *                      )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x07
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : TrcvIdx. Index of the transceiver within the context
 *                               of the Ethernet Transceiver Driver
 *  Parameters (inout): None
 *  Parameters (out)  : BaudRatePtr
 *                        ETHTRCV_BAUD_RATE_10MBIT: 10MBit connection
 *                        ETHTRCV_BAUD_RATE_100MBIT: 100MBit connection
 *                        ETHTRCV_BAUD_RATE_1000MBIT: 1000MBit connection
 *  Return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: transceiver could not be initialized
 *  Description       : Obtains the baud rate of the indexed transceiver.
 * \endverbatim
 */
FUNC(Std_ReturnType, ETHTRCV_CODE)
EthTrcv_GetBaudRate(uint8 TrcvIdx,
                    EthTrcv_BaudRateType *BaudRatePtr);
#endif

#if (STD_ON == ETHTRCV_GETDUPLEXMODE_API)
/**
 * \brief This function obtains the duplex mode of the indexed transceiver
 *
 * \verbatim
 *  Service name      : EthTrcv_GetDuplexMode
 *  Syntax            : Std_ReturnType EthTrcv_GetDuplexMode(
 *                          uint8 TrcvIdx,
 *                          EthTrcv_DuplexModeType* DuplexModePtr
 *                      )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x08
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : TrcvIdx. Index of the transceiver within the context
 *                               of the Ethernet Transceiver Driver
 *  Parameters (inout): None
 *  Parameters (out)  : DuplexModePtr
 *                        ETHTRCV_DUPLEX_MODE_HALF: half duplex connections
 *                        ETHTRCV_DUPLEX_MODE_FULL: full duplex connection
 *  Return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: transceiver could not be initialized
 *  Description       : Obtains the duplex mode of the indexed transceiver.
 * \endverbatim
 */
FUNC(Std_ReturnType, ETHTRCV_CODE)
EthTrcv_GetDuplexMode(uint8 TrcvIdx,
                      EthTrcv_DuplexModeType *DuplexModePtr);
#endif

#if (STD_ON == ETHTRCV_SETPHYTESTMODE_API)
/**
 * \brief This function Activates a given test mode
 *
 * \verbatim
 *  Service name      : EthTrcv_SetPhyTestMode
 *  Syntax            : Std_ReturnType EthTrcv_SetPhyTestMode(
 *                          uint8 TrcvIdx,
 *                          EthTrcv_PhyTestModeType Mode)
 *
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   :0x11
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant for different TrcvIdx. Non reentrant for the same TrcvIdx
 *  Parameters (in)   : TrcvIdx. Index of the transceiver within the context
 *                               of the Ethernet Transceiver Driver
 *  Parameters (in)   : Mode. Test mode to be activated. Refer to the \ref EthTrcv_PhyTestModeType.
 *  Return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: transceiver could not be initialized
 *  Description       : Activates a given test mode
 * \endverbatim
 */
FUNC(Std_ReturnType, ETHTRCV_CODE)
EthTrcv_SetPhyTestMode(uint8 TrcvIdx,
                      EthTrcv_PhyTestModeType Mode);
#endif

#if (STD_ON == ETHTRCV_SETPHYLOOPBACKMODE_API)
/**
 * \brief This function Activates a given loopback mode
 *
 * \verbatim
 *  Service name      : EthTrcv_SetPhyLoopbackMode
 *  Syntax            : Std_ReturnType EthTrcv_SetPhyLoopbackMode(
 *                                      uint8 TrcvIdx,
 *                                      EthTrcv_PhyLoopbackModeType Mode
 *                                      )
 *
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   :0x12
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant for different TrcvIdx. Non reentrant for the same TrcvIdx
 *  Parameters (in)   : TrcvIdx. Index of the transceiver within the context
 *                               of the Ethernet Transceiver Driver
 *  Parameters (in)   : Mode. Refer to the \ref EthTrcv_PhyLoopbackModeType.
 *  Return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: transceiver could not be initialized
 *  Description       : Activates a given loopback mode.
 * \endverbatim
 */
FUNC(Std_ReturnType, ETHTRCV_CODE)
EthTrcv_SetPhyLoopbackMode(uint8 TrcvIdx,
                      EthTrcv_PhyLoopbackModeType Mode);
#endif

#if (STD_ON == ETHTRCV_GETPHYSIGNALQUALITY_API)
/**
 * \brief This function Obtains the current signal quality of the link of the indexed transceiver
 *
 * \verbatim
 *  Service name      : EthTrcv_GetPhySignalQuality
 *  Syntax            : Std_ReturnType EthTrcv_GetPhySignalQuality(
 *                                      uint8 TrcvIdx,
 *                                      uint32* SignalQualityPtr
 *                                      )
 *
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   :0x12
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant for different TrcvIdx. Non reentrant for the same TrcvIdx
 *  Parameters (in)   : TrcvIdx. Index of the transceiver within the context
 *                               of the Ethernet Transceiver Driver
 *  Parameters (Out)  : SignalQualityPtr Pointer to the memory where the signal
 *                      quality shall be stored.
 *  Return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: transceiver could not be initialized
 *  Description       : Obtains the current signal quality of the link of the indexed transceiver.
 * \endverbatim
 */
FUNC(Std_ReturnType, ETHTRCV_CODE)
EthTrcv_GetPhySignalQuality(uint8 TrcvIdx,
                      uint32* SignalQualityPtr);
#endif

#if (STD_ON == ETHTRCV_SETPHYTXMODE_API)
/**
 * \brief This function Activates a given transmission mode
 *
 * \verbatim
 *  Service name      :  EthTrcv_SetPhyTxMode
 *  Syntax            : Std_ReturnType  EthTrcv_SetPhyTxMode(
 *                                      uint8 TrcvIdx,
 *                                      EthTrcv_PhyTxModeType Mode
 *                                      )
 *
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   :0x13
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant for different TrcvIdx. Non reentrant for the same TrcvIdx
 *  Parameters (in)   : TrcvIdx. Index of the transceiver within the context
 *                               of the Ethernet Transceiver Driver
 *  Parameters (in)   : Mode Transmission mode to be activated. Refer to \ref EthTrcv_PhyTxModeType
 *  Return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: transceiver could not be initialized
 *  Description       : Activates a given transmission mode.
 * \endverbatim
 */
FUNC(Std_ReturnType, ETHTRCV_CODE)
 EthTrcv_SetPhyTxMode(uint8 TrcvIdx,
                      EthTrcv_PhyTxModeType Mode);
#endif

#if (STD_ON == ETHTRCV_GETCABLEDIAGNOSTICSRESULT_API)
/**
 * \brief This function Retrieves the cable diagnostics result of a given transceiver.
 *
 * \verbatim
 *  Service name      :  EthTrcv_GetCableDiagnosticsResult
 *  Syntax            : Std_ReturnType  EthTrcv_GetCableDiagnosticsResult(
 *                                      uint8 TrcvIdx,
 *                                      EthTrcv_CableDiagResultType* ResultPtr
 *                                      )
 *
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   :0x14
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant for different TrcvIdx. Non reentrant for the same TrcvIdx
 *  Parameters (in)   : TrcvIdx. Index of the transceiver within the context
 *                               of the Ethernet Transceiver Driver
 *  Parameters (out)  : ResultPtr. Transmission mode to be activated. Refer to \ref
 *                      EthTrcv_CableDiagResultType
 *  Return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: transceiver could not be initialized
 *  Description       : Retrieves the cable diagnostics result of a given transceiver..
 * \endverbatim
 */
FUNC(Std_ReturnType, ETHTRCV_CODE)
 EthTrcv_GetCableDiagnosticsResult(uint8 TrcvIdx,
                      EthTrcv_CableDiagResultType * ResultPtr);
#endif

#if (STD_ON == ETHTRCV_GETPHYIDENTIFIER_API)
/**
 * \brief This function Obtains the PHY identifier of the Ethernet Transceiver according to
 *        IEEE 802.3-2015 chapter 22.2.4.3.1 PHY Identifer.
 *
 * \verbatim
 *  Service name      :  EthTrcv_GetPhyIdentifier
 *  Syntax            : Std_ReturnType EthTrcv_GetPhyIdentifier(
 *                                      uint8 TrcvIdx,
 *                                      uint32* OrgUniqueIdPtr,
 *                                      uint8* ModelNrPtr,
 *                                      uint8* RevisionNrPtr
 *                                      )
 *
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x15
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant for different TrcvIdx. Non reentrant for the same TrcvIdx
 *  Parameters (in)   : TrcvIdx. Index of the transceiver within the context
 *                               of the Ethernet Transceiver Driver
 *  Parameters (in)   : OrgUniqueIdPtr Pointer to the memory where the Organizationally Unique
 *                      Identifier shall be stored.
 *  Parameters (in)   : ModelNrPtr Pointer to the memory where the Manufacturer's Model
 *                      Number shall be stored.
 *  Parameters (in)   : RevisionNrPtr Pointer to the memory where the Revision Number
 *                      shall be stored.
 *  Return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: transceiver could not be initialized
 *  Description       : Obtains the PHY identifier of the Ethernet Transceiver according to
 *                      IEEE 802.3-2015 chapter 22.2.4.3.1 PHY Identifer..
 * \endverbatim
 */
FUNC(Std_ReturnType, ETHTRCV_CODE)
Std_ReturnType EthTrcv_GetPhyIdentifier(uint8 TrcvIdx,
                                        uint32* OrgUniqueIdPtr,
                                        uint8* ModelNrPtr,
                                        uint8* RevisionNrPtr);
#endif

#if (STD_ON == ETHTRCV_VERSION_INFO_API)
/**
 * \brief Function returns the version information of this module.
 *
 * \verbatim
 *  Service name      : EthTrcv_GetVersionInfo
 *  Syntax            : void EthTrcv_GetVersionInfo(
 *                          Std_VersionInfoType* versioninfo
 *                      )
 *  Mode              : User Mode (Non-Privileged Mode)
 *  Service ID[hex]   : 0x07
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : None
 *  Parameters (inout): None
 *  Parameters (out)  : VersionInfoPtr. Pointer to where to store the version
 *                                      information of this module.
 *  Return value      : None
 *  Description       : This function returns the version information of this
 *                     module
 * \endverbatim
 */
FUNC(void, ETHTRCV_CODE)
EthTrcv_GetVersionInfo(P2VAR(Std_VersionInfoType, AUTOMATIC, ETHTRCV_APPL_DATA) VersionInfoPtr);
#endif

/**
 * \brief This function is called when information has been read out via MII
 *        interface. Triggered by previous Eth_ReadMii call. It can directly
 *        be called within Eth_ReadMii.
 *
 * \verbatim
 *  Service name      : EthTrcv_ReadMiiIndication
 *  Syntax            : void EthTrcv_ReadMiiIndication(
 *                          uint8 CtrlIdx,
 *                          uint8 TrcvIdx,
 *                          uint8 RegIdx,
 *                          uint8 RegVal
 *                      )
 *  Mode              : User Mode (Non-Privileged Mode)
 *  Service ID[hex]   : 0x09
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant for the same CtrlIdx, reentrant for
 *                      different
 *  Parameters (in)   : CtrlIdx. Index of the controller within the context
 *                               of the Ethernet Driver
 *                      TrcvIdx. Index of the transceiver on the MII
 *                      RegIdx. Index of the transceiver register on the MII
 *                      RegVal. Value contained in the indexed register
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : Called when information has been read out via MII interface.
 *                      Triggered by previous Eth_ReadMii call. Can directly be
 *                      called within Eth_ReadMii.
 * \endverbatim
 */
/* Important Note:
 * There is discrepancy in the Eth and EthTrcv specs. Data type for RegVal in
 * EthTrcv_ReadMiiIndication is uint8 as per Eth Trcv spec where as Eth_ReadMii
 * takes uint16 pointer RegValPtr. To avoid data loss we change RegVal data type
 * to uint16 */
FUNC(void, ETHTRCV_CODE)
EthTrcv_ReadMiiIndication(uint8 CtrlIdx,
                          uint8 TrcvIdx,
                          uint8 RegIdx,
                          uint16 RegVal);

/**
 * \brief This function is called when information has been written via MII
 *        interface. Triggered by previous Eth_WriteMii call. It can directly
 *        be called within Eth_WriteMii
 *
 * \verbatim
 *  Service name      : EthTrcv_WriteMiiIndication
 *  Syntax            : void EthTrcv_WriteMiiIndication(
 *                          uint8 CtrlIdx,
 *                          uint8 TrcvIdx,
 *                          uint8 RegId
 *                      )
 *  Mode              : User Mode (Non-Privileged Mode)
 *  Service ID[hex]   : 0x0a
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant for the same CtrlIdx, reentrant for
 *                      different
 *  Parameters (in)   : CtrlIdx. Index of the controller within the context
 *                               of the Ethernet Driver
 *                      TrcvIdx. Index of the transceiver on the MII
 *                      RegIdx. Index of the transceiver register on the MII
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : Called when information has been written via MII interface.
 *                      Triggered by previous Eth_WriteMii call. Can directly be
 *                      called within Eth_WriteMii.
 * \endverbatim
 */
FUNC(void, ETHTRCV_CODE)
EthTrcv_WriteMiiIndication(uint8 CtrlIdx,
                           uint8 TrcvIdx,
                           uint8 RegId);

/**
 * \brief This function is used for polling state changes and wakeup reasons.
 *        It calls EthIf_TrcvModeIndication when the transceiver mode changed.
 *        It stores wakeup events if EthTrcvWakeUpSupport is set to
 *        ETHTRCV_WAKEUP_BY_POLLING.
 *
 * \verbatim
 *  Service name      : EthTrcv_MainFunction
 *  Syntax            : void EthTrcv_MainFunction(
 *                          void
 *                      )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x0c
 *  Description       : Used for polling state changes and wakeup reasons. Calls
 *                      EthIf_TrcvModeIndication when the transceiver mode
 *                      changed. Stores wakeup events if EthTrcvWakeUpSupport is
 *                      set to ETHTRCV_WAKEUP_BY_POLLING.
 * \endverbatim
 */
FUNC(void, ETHTRCV_CODE)
EthTrcv_MainFunction(void);

#ifdef __cplusplus
}
#endif

#endif /* ETHTRCV_H_ */

/*! @} */
