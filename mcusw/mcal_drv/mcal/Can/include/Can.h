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
 *  \file     Can.h
 *
 *  \brief    This file contains interface header for CAN MCAL driver
 *
 */

/**
 *
 * \defgroup MCAL_CAN_API CAN Driver API
 *
 * The Can Driver MCAL provides low level access to onbaord MCAN peripheral
 * present on Device.<br>
 * The Can Driver implements a standardized interface specified in the
 * AUTOSAR_SWS_CANDriver document.<br>
 * The Can Driver is part of the Communication Stack in the AUTOSAR Basic
 * Software (BSW)<br>
 * The CAN Hardware Unit cntrolled by CAN Driver combines one or several CAN
 * controllers, <br>
 * which may be located on-chip or as external standalone devices of the same
 * type, with common or separate Hardware Objects<br>
 * @sa MCAL_CAN_CFG
 *  @{
 */
/*
 * Below are the global design requirements which are met by this CAN
 * driver which can't be mapped to a particular source ID
 */

/*
 * Design: MCAL-6059,MCAL-5955,MCAL-6022,MCAL-5977,MCAL-6046,MCAL-6127,MCAL-6081,MCAL-5921,MCAL-5987,MCAL-6005,MCAL-5970
 */

#ifndef CAN_H_
#define CAN_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
/*
 * Design: MCAL-5993,MCAL-6004,MCAL-6075,MCAL-5904,MCAL-5980,MCAL-6098
 */
#include "Std_Types.h"
#include "ComStack_Types.h"
#include "Can_GeneralTypes.h"
#include "Can_Cfg.h"

/*
 * Design: MCAL-6106,MCAL-6144
 */
#include "Os.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \name Can Driver Module SW Version Info
 *
 *  Defines for CAN Driver version used for compatibility checks
 *  @{
 */
/** \brief  Driver Implementation Major Version.Used for version compatibiltiy
 *check */
#define CAN_SW_MAJOR_VERSION             (9U)
/** \brief  Driver Implementation Minor Version.Used for version compatibiltiy
 *check */
#define CAN_SW_MINOR_VERSION             (1U)
/** \brief  Driver Implementation Patch Version */
/* @} */
#define CAN_SW_PATCH_VERSION             (0U)

/**
 *  \name Can Driver Module AUTOSAR Version Info
 *
 *  Defines for CAN Driver AUTOSAR version used for compatibility checks
 *  @{
 */
/** \brief  AUTOSAR Major version specification implemented by CanDriver*/
#define CAN_AR_RELEASE_MAJOR_VERSION     (4U)
/** \brief  AUTOSAR Minor  version specification implemented by CanDriver */
#define CAN_AR_RELEASE_MINOR_VERSION     (3U)
/** \brief  AUTOSAR Patch version specification implemented by CanDriver*/
/* @} */
#define CAN_AR_RELEASE_REVISION_VERSION  (1U)

/* Global Macros */
/**
 *  \name Can Driver ID Info
 *  @{
 */
/** \brief  Texas Instruments Vendor ID */
#define CAN_VENDOR_ID   (44U)
/** \brief  Can Driver Module ID       */
#define CAN_MODULE_ID   (80U)
/** \brief CAN Driver Instance ID */
#define CAN_INSTANCE_ID (0U)
/* @} */

/**
 *  \name CAN Driver states.
 *  @{
 */
/** \brief  CAN Driver is NOT initialized */
#define CAN_UNINIT   ((uint8)0U)
/** \brief CAN Driver is initialized */
#define CAN_READY    ((uint8)1U)
/* @} */

/**
 *  \name Can Driver Service Id
 *
 *  The Service Id is one of the argument to Det_ReportError function and is
 *  used to identify the source of the error
 *  @{
 */
/** \brief  Can_Init() */
#define CAN_INIT_ID                  (0x00U)
/** \brief  Can_MainFunction_Write()  */
#define CAN_MAINFCT_WRITE_ID         (0x01U)
/** \brief  Can_SetControllerMode()  */
#define CAN_SETCTR_ID                (0x03U)
/** \brief  Can_DisableControllerInterrupts() */
#define CAN_DIINT_ID                 (0x04U)
/** \brief  Can_EnableControllerInterrupts() */
#define CAN_ENINT_ID                 (0x05U)
/** \brief  Can_Write()  */
#define CAN_WRITE_ID                 (0x06U)
/** \brief  Can_GetVersionInfo() */
#define CAN_VERSION_ID               (0x07U)
/** \brief  Can_MainFunction_Read() */
#define CAN_MAINFCT_READ_ID          (0x08U)
/** \brief  Can_MainFunction_BusOff()  */
#define CAN_MAINFCT_BO_ID            (0x09U)
/** \brief  Can_MainFunction_Wakeup() */
#define CAN_MAINFCT_WU_ID            (0x0AU)
/** \brief  Can_CheckWakeup()         */
#define CAN_CKWAKEUP_ID              (0x0BU)
/** \brief  Can_MainFunction_Mode()   */
#define CAN_MAINFCT_MODE_ID          (0x0CU)
/** \brief  Can_SetBaudrate()         */
#define CAN_SETBAUDRATE_ID           (0x0FU)
/** \brief  Can_DeInit()              */
#define CAN_DEINIT_ID                (0x10U)
/** \brief  Can_GetControllerErrorState() */
#define CAN_GETCTRERRST_ID           (0x11U)
/** \brief  Can_GetControllerMode() */
#define CAN_GETCTRMODE_ID            (0x12U)
/** \brief  Can_TestLoopBackModeEnable() */
#define CAN_LOOPBACK_ID              (0x20U)
/** \brief  Can_RxProcess()           */
#define CAN_RXPROCESS_ID             (0x21U)

/* @} */

/**
 *  \name Can Error Codes
 *
 *  Error codes returned by Can functions
 *  @{
 */
#ifndef CAN_E_PARAM_POINTER
#define CAN_E_PARAM_POINTER          (0x01U)
/** \brief  ERROR:NULL_PTR passed as parameter */
#endif
#ifndef CAN_E_PARAM_HANDLE
#define CAN_E_PARAM_HANDLE           (0x02U)
/** \brief  ERROR:Invalid Controller Handle */
#endif
#ifndef CAN_E_PARAM_DLC
#define CAN_E_PARAM_DLC              (0x03U)
/** \brief  ERROR:Invalid length of msg */
#endif
#ifndef CAN_E_PARAM_CONTROLLER
#define CAN_E_PARAM_CONTROLLER       (0x04U)
/** \brief  ERROR:Invalid controller id */
#endif
#ifndef CAN_E_UNINIT
#define CAN_E_UNINIT                 (0x05U)
/** \brief  ERROR:Eror due to API invoked without  performing Can_Init() */
#endif
#ifndef CAN_E_TRANSITION
#define CAN_E_TRANSITION             (0x06U)
/** \brief  ERROR:Eror during Controller state  transition */
#endif
#ifndef CAN_E_DATALOST
#define CAN_E_DATALOST               (0x07U)
/** \brief  ERROR:Data lost/dropped */
#endif
#ifndef CAN_E_PARAM_BAUDRATE
#define CAN_E_PARAM_BAUDRATE         (0x08U)
/** \brief  ERROR:Timeout occured */
#endif
#ifndef CAN_E_ICOM_CONFIG_INVALID
#define CAN_E_ICOM_CONFIG_INVALID    (0x09U)
/** \brief  ERROR:Invalid Baudrate configuration */
#endif
#ifndef CAN_E_INIT_FAILED
#define CAN_E_INIT_FAILED            (0x0AU)
/** \brief  ERROR:Invalid configuration set selection */
#endif
/* @} */

/**
 *  \name CAN Config Ids
 *
 *  The Config Ids used for different
 *  CAN Configuration builds
 *  @{
 */
/** \brief Config 0 (All configurations ON) */
#define  CAN_CFG_ID_0     (0x01U)
/** \brief Config 1 (All configurations OFF).
 *         CAN_CFG_ID_1 is used only for compile check */
#define  CAN_CFG_ID_1     (0x02U)
/** \brief Config 2 (All configurations ON except DET OFF) */
#define  CAN_CFG_ID_2     (0x04U)
/** \brief Config 3 (Configurator auto generated file) */
#define  CAN_CFG_ID_3     (0x08U)
/** \brief Config 4 (Configurator auto generated file MCAN instead
  *                  of DCAN2 with pre-compile variant ON) */
#define  CAN_CFG_ID_4     (0x10U)
/** \brief Config 5 (All configurations ON but used only for testing
  *                   WakeUp by external HW event and Performance tests) */
#define  CAN_CFG_ID_5     (0x20U)
/** \brief Config 6 (Used for MCAN Testing) */
#define  CAN_CFG_ID_6     (0x40U)
/* @} */

/**
 *  \name CanIdType.
 *  @{
 */
/** \brief  Mixed Mode MB Id type */
#define  MIXED_MODE_MB_ID     (2U)
/* @} */

#if (STD_ON == CAN_SAFETY_DIAGNOSTIC)
/**
 *  \name Can Error Types
 *
 *  Error Types returned by Can functions
 *  @{
 */
 /** \brief  ERROR:Protocol Error in Arbitration Phase */
#define MCAN_ERR_TYPE_PEA_ERROR    (1U)
/** \brief  ERROR:Time out Error */
#define MCAN_ERR_TYPE_TOO_ERROR    (2U)
/* @} */

/**
 *  \ local macro.
 *  @{
 */
/** \brief  Check lsb bit Set or not */
#define MCAN_LSB_BIT_SET           (1U)
/* @} */
#endif /* (STD_ON == CAN_SAFETY_DIAGNOSTIC) */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */
/**
 *  \addtogroup MCAL_CAN_CFG Can Configuration
 *  @{
 */

/**
 * \brief Can Mailbox direction enum
 */
typedef enum
{
    CAN_MAILBOX_DIRECTION_RX = 0x0U,
    /**< Mailbox is for receive */
    CAN_MAILBOX_DIRECTION_TX = 0x1U
    /**< Mailbox is for transmit */
} Can_MailBoxDirectionType;

/**
 * \brief Can Handle Type enum
 */
typedef enum
{
    CAN_FULL = 0x0U,
    /**< For only one L-PDU (identifier) is handled by the hardware object */
    CAN_BASIC = 0x1U
    /**< For several L-PDUs are handled by the hardware object */
} Can_HandleType;

/**
 * \brief Can Tx/Rx processing enum
 */
typedef enum
{
    CAN_TX_RX_PROCESSING_INTERRUPT = 0x0U,
    /**< Interrupt Mode of operation */
    CAN_TX_RX_PROCESSING_MIXED = 0x1U,
    /**< Mixed Mode of operation */
    CAN_TX_RX_PROCESSING_POLLING = 0x2U
    /**< Polling Mode of operation */
} Can_TxRxProcessingType;

#if (STD_ON == CAN_SAFETY_DIAGNOSTIC)
/**
 * \brief CAN IRQ status enum
 */
typedef enum
{
    CAN_IRQ_NO_INTR_EVENT  = 0U,
    /**< No Interrupt Event occured */
    CAN_IRQ_INTR_STUFF_EVENT = 1U,
    /**< Stuff Error Event occured */
    CAN_IRQ_INTR_FORM_EVENT  = 2U,
    /**< Form Error Event occured */
    CAN_IRQ_INTR_ACK_EVENT   = 3U,
    /**< Acknowldgement Error Event occured */
    CAN_IRQ_INTR_BIT1_EVENT  = 4U,
    /**< Bit-1 Error Event occured */
    CAN_IRQ_INTR_BIT0_EVENT  = 5U,
    /**< Bit-0 Error Event occured */
    CAN_IRQ_INTR_CRC_EVENT = 6U,
    /**< Cyclic Redundancy Error Event occured */
    CAN_IRQ_INTR_TOO_EVENT = 7U,
    /**< Time Out Error Event occured */
    CAN_IRQ_STATUS_READ_FAIL = 8U,
    /**< Interrupt status reading fail */
} Can_IrqStatusType;
#endif /* (STD_ON == CAN_SAFETY_DIAGNOSTIC) */

/**
 * \brief Can Controller State enum type
 */
typedef uint8 CanControllerState_Type;

/**
 * \brief Can Interrupt Mask type
 */
typedef uint32 Can_InterruptMask_Type;

/**
 * \brief Structure defining the filter mask to be used
 */
typedef struct Can_MaskStruct
{
    uint32 MaskValue;
/**< Filter mask value. */
}Can_MaskType;

/**
 * \brief Structure defining the HW filter to be used
 */
typedef struct Can_HwFilterStruct
{
    Can_IdType               IDValue;
    /**< ID value extended will have MSB set */
    uint32                   Mask;
    /**< Mask for the RX filtering */
}Can_HwFilterType;

/**
 * \brief Structure defining the CAN FD data phase baud rate configuration
 */
typedef struct Can_FdBaudConfigStruct
{
    uint16 Baud;
    /**< In Kbps */
    uint8  PropSeg;
    /**< Prop Segement value */
    uint8  Pseg1;
    /**< Phase Segment 1 */
    uint8  Pseg2;
    /**< Phase Segment 2 */
    uint8  Sjw;
    /**< Sync jump width */
    uint16 TimingValues;
    /**< Sum of all timing parameters * */
    uint16 BrpValue;
    /**< Controller BRP value for Baud **/
    uint16 TrcvCompDelay;
    /**< Specifies the Transceiver Delay Compensation Offset in ns **/
    boolean BrsSwitch;
    /**< Specifies if the bit rate switching shall be used for transmissions.**/
}Can_FdBaudConfigType;

/**
 * \brief Structure defining the CAN baud rate configuration
 */
typedef struct Can_BaudConfigStruct
{
    uint16 Baud;
    /**< In Kbps */
    uint8  PropSeg;
    /**< Prop Segement value */
    uint8  Pseg1;
    /**< Phase Segment 1 */
    uint8  Pseg2;
    /**< Phase Segment 2 */
    uint8  Sjw;
    /**< Sync jump width */
    uint16  TimingValues;
    /**< Sum of all timing parameters */
    uint16 BrpValue;
    /**< Controller BRP value for Baud */
    Can_FdBaudConfigType  BaudFdRateConfig;
    /**< List of available Data phase Baud rates for CAN FD */
}Can_BaudConfigType;

/**
 * \brief Can Controller Configuration definition
 */
typedef struct Can_ControllerStruct
{
    Can_BaudConfigType    *DefaultBaud;
    /**< pointer to default Baud structure */
    Can_BaudConfigType  **BaudRateConfigList;
    /**< List of available Baud rates */
}Can_ControllerType;

/**
 * \brief Can Controller Pre Compile Configuration definition
 */
typedef struct Can_ControllerStruct_PC
{
    uint8                     ControllerId;
    /**< Id as provided by GUI **/
    boolean                   CntrActive;
    /**< Contoller is used=1 or not_used=0*/
    uint32                    CntrAddr;
    /**< Pointer to CanReg structure */
    Can_TxRxProcessingType    RxProcessingType;
    /**< Type can be Interrupt, Polling or Mixed */
    Can_TxRxProcessingType    TxProcessingType;
    /**< Type can be Interrupt, Polling or Mixed */
    boolean                   BusOffProcessingInterrupt;
    /**< TRUE = Interrupt FALSE = Polling */
    Can_ControllerInstance    CanControllerInst;
    /**< Can Controller Instance **/
    boolean                   CanFDModeEnabled;
    /**< Controller is in CAN FD Mode or not */
}Can_ControllerType_PC;

/**
 * \brief Can mailox configuration definition
 */
typedef struct Can_MailboxStruct
{
    uint8                    CanHandleType;
    /**< CanHandleType 0=Full, *1=Basic */
    uint32                   MBIdType;
    /**< CanIdType 0=standard 1=Extended 2= Mixed */
    Can_HwHandleType         HwHandle;
    /**< Mailbox - Hw object in the controller*/
    uint32                   CanHwObjectCount;
    /**< Number of hardware objects used to implement one HOH*/
    Can_MailBoxDirectionType MBDir;
    /**< CanObjectType - Direction of Mailbox @sa Can_MailBoxDirectionType */
    const Can_ControllerType_PC    *Controller;
    /**< Reference to CAN Controller to which the HOH is associated to. */
    Can_HwFilterType         **HwFilterList;
    /**< List of HW Filter */
    uint32                   HwFilterCnt;
    /**< HW Filter Count */
    uint8                    CanFdPaddingValue;
    /**< If PduInfo->SduLength does not match possible DLC values
     *   CanDrv will use the next higher valid DLC for transmission with
     *   initialization of unused bytes to the value of
     *   the corresponding CanFdPaddingValue.
     */
    boolean                  CanHardwareObjectUsesPolling;
    /**< TRUE = Polling, FALSE = Interrupt */
}Can_MailboxType;

/**
 * \brief Can mailox Pre compile configuration definition
 */
typedef struct Can_MailboxStruct_PC
{
    uint16                   CanObjectId;
    /**< CanObjectId - Holds handle Id */
}Can_MailboxType_PC;

/**
 * \brief Dummy structure will be used if DMA support is required
 */
typedef struct
{
    uint32  Reserved;
    /**< Reserved for Future use, currently unused parameter */
}Can_DmaPrms;

/*
 * Design: MCAL-5980
 */
/**
 * \brief Can MCAL root configuration structure.
 */
typedef struct Can_ConfigType_s
{
    Can_ControllerType **CanControllerList;
    /**< List of enabled Controllers */
    uint8                CanMaxControllerCount;
    /**< MaxCount of Controller in Controller List */
    Can_MailboxType    **MailBoxList;
    /**< MB array for all controllers */
    uint32               MaxMbCnt;
    /**< MaxMbCount in MB list in all controller */
    uint32               MaxBaudConfigID[CAN_MAX_CONTROLLER];
    /**< Max Baud Config Index in BaudRateConfigList in all controller */
    Can_DmaPrms          *DmaPrms;
    /**< Reserved for Future use, currently unused parameter */
}Can_ConfigType;

#if (STD_ON == CAN_REGISTER_READBACK_API)
/**
 * \brief  Register Readback Structure
 *
 * Structure to read back key CAN control registers.
 *
 * The Safety API to readback CAN register vales will populate this structure
 */
typedef struct
{
    uint32 CanReadBackRegPID;
    /**< MCANSS PID Register  */
    uint32 CanReadBackRegSTAT;
    /**< MCANSS STAT Register  */
    uint32 CanReadBackRegCREL;
    /**< Auto Bus On Time Register  */
    uint32 CanReadBackRegENDN;
    /**< Endianess MCAN_TEST value Constant "0x8765 4321" */
    uint32 CanReadBackRegCTRL;
    /**< MCAN CTRL Register */
    uint32 CanReadBackRegDBTP;
    /**< MCAN DBTB Register */
    uint32 CanReadBackRegRWD;
    /**< MCAN RWD Register */
    uint32 CanReadBackRegCCCR;
    /**< MCAN CCCR Register */
    uint32 CanReadBackRegNBTP;
    /**< MCAN NBTP Register */
    uint32 CanReadBackRegTDCR;
    /**< MCAN TDCR Register */
    uint32 CanReadBackRegGFC;
    /**< MCAN GFC Register */
    uint32 CanReadBackRegSIDFC;
    /**< MCAN SIDFC Register */
    uint32 CanReadBackRegXIDFC;
    /**< MCAN XIDFC Register */
    uint32 CanReadBackRegRXF0C;
    /**< MCAN RXF0C Register */
    uint32 CanReadBackRegRXBC;
    /**< MCAN RXBC Register */
    uint32 CanReadBackRegRXF1C;
    /**< MCAN RXF1C Register */
    uint32 CanReadBackRegTXBC;
    /**< MCAN TXBC Register */
    uint32 CanReadBackRegTXESC;
    /**< MCAN TXESC Register */
    uint32 CanReadBackRegTXEFC;
    /**< MCAN TXEFC Register */
} Can_RegisterReadbackType;
#endif /* (STD_ON == CAN_REGISTER_READBACK_API) */

/* @} */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

#if (CAN_VERSION_INFO_API == STD_ON)
/**
 * \brief Function returns the version information of this module.
 *
 * \verbatim
 * Service name    : Can_GetVersionInfo
 * Syntax          : void Can_GetVersionInfo(Std_VersionInfoType* versioninfo)
 * Mode            : User Mode (Non-Privileged Mode)
 * Service ID[hex] : 0x07
 * Sync/Async      : Synchronous
 * Reentrancy      : Reentrant
 * Parameters (in) : None
 * Parameters (inout): None
 * Parameters (out)  : versioninfo
 *                     Pointer to where to store the version information of this
 *                     module.
 * Return value      : None
 * Description       : This function returns the version information of this
 *                     module
 * \endverbatim
 */
FUNC(void,
     CAN_CODE) Can_GetVersionInfo(P2VAR(Std_VersionInfoType, AUTOMATIC,
                                        CAN_APPL_DATA) VersionInfo);
#endif

/**
 * \brief This function initializes the module.
 *
 * \verbatim
 * Service name      : Can_Init
 * Syntax            : void Can_Init(const Can_ConfigType* CfgPtr)
 * Mode              : Supervisor Mode (Privileged Mode)
 * Service ID[hex]   : 0x00
 * Sync/Async        : Synchronous
 * Reentrancy        : Non Reentrant
 * Parameters (in)   : CfgPtr
 *                     Pointer to driver configuration.
 * Parameters (inout): None
 * Parameters (out)  : None
 * Return value      : None
 * Description       : This function initializes the module.
 * \endverbatim
 */
FUNC(void, CAN_CODE) Can_Init(P2CONST(Can_ConfigType, AUTOMATIC,
                                    CAN_PBCFG) CfgPtr);

/**
 * \brief This function performs software triggered state transitions of the CAN
 *                     controller State machine.
 *
 * \verbatim
 * Service name: Can_SetControllerMode
 * Syntax: Std_ReturnType Can_SetControllerMode(uint8 Controller,
 *                     Can_ControllerStateType Transition)
 * Mode              : Supervisor Mode (Privileged Mode)
 * Service ID[hex]: 0x03
 * Sync/Async: Asynchronous
 * Reentrancy: Non Reentrant
 * Parameters (in): Controller
 *                  - CAN controller for which the status shall be changed
 *                   Transition
 *                  - Transition value to request new CAN controller state
 * Parameters (inout): None
 * Parameters (out):   None
 * Return value: Std_ReturnType
 *               - E_OK: request accepted
 *               - E_NOT_OK: request not accepted, a development error
 *                     occurred
 * Description:  This function performs software triggered state transitions of
 *                     the CAN controller State machine.
 * \endverbatim
 */

FUNC(Std_ReturnType, CAN_CODE) Can_SetControllerMode(
    uint8 Controller, Can_ControllerStateType Transition);

/**
 * \brief This function is called by CanIf to pass a CAN message to CanDrv for
 *                     transmission.
 *
 * \verbatim
 * Service name: Can_Write
 * Syntax      : Std_ReturnType Can_Write(Can_HwHandleType Hth, const
 *                     Can_PduType* PduInfo)
 * Mode              : Supervisor Mode (Privileged Mode)
 * Service ID[hex]: 0x06
 * Sync/Async     : Synchronous
 * Reentrancy     : Reentrant (thread-safe)
 * Parameters (in): Hth
 *                  - Information which HW-transmit handle shall be used for
 *                    transmit. Implicitly this is also the information about
 *                    the controller to use because the Hth numbers are unique
 *                    inside one hardware unit.
 *                  PduInfo
 *                  - Pointer to SDU user memory, DLC and Identifier.
 * Parameters (inout): None
 * Parameters (out)  : None
 * Return value: Std_ReturnType
 *               - E_OK: Write command has been accepted
 *               - E_NOT_OK: development error occurred
 *               - CAN_BUSY: No TX hardware buffer available or pre-emptive call
 *                      of Can_Write that can't be implemented re-entrant
 * Description: This function is called by CanIf to pass a CAN message to CanDrv
 *                     for transmission.
 * \endverbatim
 */
FUNC(Std_ReturnType, CAN_CODE) Can_Write(Can_HwHandleType Hth,
	const Can_PduType *PduInfo);

/**
 * \brief This function disables all interrupts for this CAN controller
 *
 * \verbatim
 * Service name      : Can_DisableControllerInterrupts
 * Syntax            : void Can_DisableControllerInterrupts(uint8 Controller)
 * Mode              : Supervisor Mode (Privileged Mode)
 * Service ID[hex]   : 0x04
 * Sync/Async        : Synchronous
 * Reentrancy        : Reentrant
 * Parameters (in)   : Controller
 *                     - CAN controller for which interrupts shall be disabled.
 * Parameters (inout): None
 * Parameters (out)  : None
 * Return value      : None
 * Description       : This function disables all interrupts for this CAN
 *                     controller
 *
 * \endverbatim
 */
FUNC(void, CAN_CODE) Can_DisableControllerInterrupts(uint8 Controller);

/**
 * \brief This function enables all allowed interrupts
 *
 * \verbatim
 * Service name      : Can_EnableControllerInterrupts
 * Syntax            : void Can_EnableControllerInterrupts(uint8 Controller)
 * Mode              : Supervisor Mode (Privileged Mode)
 * Service ID[hex]   : 0x05
 * Sync/Async        : Synchronous
 * Reentrancy        : Reentrant
 * Parameters (in)   : Controller
 *                     - CAN controller for which interrupts shall be re-enabled.
 * Parameters (inout): None
 * Parameters (out)  : None
 * Return value      : None
 * Description       : This function enables all allowed interrupts
 *
 * \endverbatim
 */
FUNC(void, CAN_CODE) Can_EnableControllerInterrupts(uint8 Controller);

/* polling functions (always available for non polled systems also)*/

/**
 * \brief This function performs the polling of TX confirmation when
 *                     CAN_TX_PROCESSING is set to POLLING.
 *
 * \verbatim
 * Service name   : Can_MainFunction_Write
 * Syntax         : void Can_MainFunction_Write(void)
 * Mode           : Supervisor Mode (Privileged Mode)
 * Service ID[hex]: 0x01
 * Description    : This function performs the polling of TX confirmation when
 *                     CAN_TX_PROCESSING is set to POLLING.
 *
 * \endverbatim
 */
FUNC(void, CAN_CODE) Can_MainFunction_Write(void);

/**
 * \brief This function performs the polling of bus-off events that are
 *                     configured statically as 'to be polled'
 *
 * \verbatim
 * Service name   : Can_MainFunction_BusOff
 * Syntax         : void Can_MainFunction_BusOff(void)
 * Mode           : Supervisor Mode (Privileged Mode)
 * Service ID[hex]: 0x09
 * Description    : This function performs the polling of bus-off events that are
 *                     configured statically as 'to be polled'.
 *
 * \endverbatim
 */
FUNC(void, CAN_CODE) Can_MainFunction_BusOff(void);

/**
 * \brief This function performs the polling of RX indications when
 *                     CAN_RX_PROCESSING is set to POLLING
 *
 * \verbatim
 * Service name   : Can_MainFunction_Read
 * Syntax         : void Can_MainFunction_Read(void)
 * Mode           : Supervisor Mode (Privileged Mode)
 * Service ID[hex]: 0x08
 * Description    : This function performs the polling of RX indications when
 *                     CAN_RX_PROCESSING is set to POLLING.
 *
 * \endverbatim
 */
FUNC(void, CAN_CODE) Can_MainFunction_Read(void);

/**
 * \brief This function performs the polling of wake-up events that are
 *                     configured statically as 'to be polled'.
 *
 * \verbatim
 * Service name   : Can_MainFunction_Wakeup
 * Syntax         : void Can_MainFunction_Wakeup(void)
 * Mode           : Supervisor Mode (Privileged Mode)
 * Service ID[hex]: 0x0A
 * Description    : This function performs the polling of wake-up events that are
 *                     configured statically as 'to be polled'.
 *
 * \endverbatim
 */
FUNC(void, CAN_CODE) Can_MainFunction_Wakeup(void);

/**
 * \brief This function performs the polling of CAN controller mode transitions
 *
 * \verbatim
 * Service name   : Can_MainFunction_Mode
 * Syntax         : void Can_MainFunction_Mode(void)
 * Mode           : Supervisor Mode (Privileged Mode)
 * Service ID[hex]: 0x0C
 * Description    : This function performs the polling of CAN controller mode
 *                     transitions.
 *
 * \endverbatim
 */
FUNC(void, CAN_CODE) Can_MainFunction_Mode(void);

/* Baud rate functions */
/**
 * \brief This service shall set the baud rate configuration of the CAN
 *        controller. Depending on necessary baud rate modifications the
 *        controller might have to reset.
 *
 * \verbatim
 *
 *  Function name     : Can_SetBaudrate
 *  Syntax            : FUNC(Std_ReturnType, CAN_CODE) Can_SetBaudrate(uint8
 *                           Controller, uint16 BaudRateConfigID)
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x0f
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant for different Controllers.
 *                      Non reentrant for the same Controller.
 *  Parameters (in)   : None
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : This service shall set the baud rate configuration
 *                      of the CAN controller. Depending on necessary baud rate
 *                      modifications the controller might have to reset.
 *
 * \endverbatim
 */
FUNC(Std_ReturnType, CAN_CODE) Can_SetBaudrate(uint8 Controller,
                                               uint16 BaudRateConfigID);

#if (CAN_LOOPBACK_ENABLE == STD_ON)
/**
 * \brief This service will enable CAN loopback mode.
 *
 * \verbatim
 *
 *  Service name      : Can_TestLoopBackModeEnable
 *  Syntax            : Std_ReturnType Can_TestLoopBackModeEnable(uint8
 *                      Controller, uint8 mode)
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : N/A
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : Controller
 *                      - CAN Controller to enable loopback mode
 *                      mode
 *                      - Loopback mode
 *                      -# 0 - Digital Loopback
 *                      -# 1 - Analog  Loopback
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Std_ReturnType
 *                      - E_OK: Baudrate supported by the CAN Controller
 *                      - E_NOT_OK: Baudrate not supported / invalid CAN
 *  Description       : This service will enable CAN loopback mode.
 * \endverbatim
 */
FUNC(Std_ReturnType, CAN_CODE) Can_TestLoopBackModeEnable(uint8 Controller,
                                                          uint8 Mode);

/**
 * \brief This service will disable CAN loopback mode.
 *
 * \verbatim
 *
 *  Service name      : Can_TestLoopBackModeDisable
 *  Syntax            : Std_ReturnType Can_TestLoopBackModeDisable(uint8
 *                      Controller, uint8 Mode)
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : N/A
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : Controller
 *                      - CAN Controller to enable loopback mode
 *                      mode
 *                      Loopback mode
 *                      -# 0 - Digital Loopback
 *                      -# 1 - Analog  Loopback
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Std_ReturnType
 *                      - E_OK: Loopback mode enabled in the CAN Controller
 *                      - E_NOT_OK: Loopback mode enable failed
 *  Description       : This service will disable CAN loopback mode.
 * \endverbatim
 */
FUNC(Std_ReturnType, CAN_CODE) Can_TestLoopBackModeDisable(uint8 Controller,
                                                           uint8 Mode);
#endif

#if (STD_ON == CAN_REGISTER_READBACK_API)
/**
 * \brief This service will readback CAN registers
 *
 * \verbatim
 *
 *  Service name      : Can_RegisterReadback
 *  Syntax            : Std_ReturnType Can_RegisterReadback(uint8 Controller)
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : N/A
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : Controller
 *                      - CAN Controller to readback registers configuration.
 *  Parameters (inout): RegRbPtr - Pointer to where to store the readback
 *                      values. If this pointer is NULL_PTR, then the API
 *                      will return E_NOT_OK.
 *  Parameters (out)  : None
 *  Return value      : Std_ReturnType
 *                      - E_OK: Register readback successful.
 *                      - E_NOT_OK: Register readback failed.
 *  Description       : This service will readback CAN registers.
 * \endverbatim
 */
Std_ReturnType Can_RegisterReadback(VAR(uint8, AUTOMATIC)Controller,
                    P2VAR(Can_RegisterReadbackType, AUTOMATIC,
                                    CAN_APPL_DATA) RegRbPtr);
#endif

#if (STD_ON == CAN_SAFETY_DIAGNOSTIC)
/**
 * \brief This service will Enable the interrupts
 *
 * \verbatim
 *
 *  Service name      : Can_EnableIntr
 *  Syntax            : Std_ReturnType Can_EnableIntr(
 *                        VAR(uint8, AUTOMATIC)Controller,
 *                        VAR(uint8, AUTOMATIC)CanErrVar);
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : N/A
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : Controller
 *                      - Controller for which interrupt errors has
 *                        to be enabled.
 *                      - CanErrVar - CAN Error Type.
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Std_ReturnType
 *                      - E_OK: Enabling Interrupts is success.
 *                      - E_NOT_OK: Enabling Interrupts failed.
 *  Description       : This service will Enable the interrupts.
 * \endverbatim
 */
Std_ReturnType Can_EnableIntr(VAR(uint8, AUTOMATIC)Controller,
                    VAR(uint8, AUTOMATIC)CanErrVar);

/**
 * \brief This service will Disable the Interrupts.
 *
 * \verbatim
 *
 *  Service name      : Can_DisableIntr
 *  Syntax            : Std_ReturnType Can_DisableIntr(
 *                        VAR(uint8, AUTOMATIC)Controller,
 *                        VAR(uint8, AUTOMATIC)CanErrVar)
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : N/A
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : Controller
 *                      - Controller for which interrupt errors has to
 *                        be Disabled.
 *                      - CanErrVar - CAN Error Type.
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Std_ReturnType
 *                      - E_OK: Disabling Interrupts is success.
 *                      - E_NOT_OK: Disabling Interrupts failed.
 *  Description       : This service will Disable the Interrupts.
 * \endverbatim
 */
Std_ReturnType Can_DisableIntr(VAR(uint8, AUTOMATIC)Controller,
                    VAR(uint8, AUTOMATIC)CanErrVar);

/**
 * \brief This service will provide the status of the interrupt.
 *
 * \verbatim
 *
 *  Service name      : Can_GetIntrStatus
 *  Syntax            : Can_IrqStatusType Can_GetIntrStatus(
 *                        VAR(uint8, AUTOMATIC)Controller)
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : N/A
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : Controller
 *                      - Controller for which it reads the interrupt error
 *                        status.
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Can_IrqStatusType
 *                      - CAN_IRQ_NO_INTR_EVENT: No Interrupt Event occured.
 *                      - CAN_IRQ_INTR_STUFF_EVENT: STUFF Error Event occured.
 *                      - CAN_IRQ_INTR_FORM_EVENT: FORM Error Event occured.
 *                      - CAN_IRQ_INTR_ACK_EVENT: ACK Error Event occured.
 *                      - CAN_IRQ_INTR_BIT1_EVENT: BIT1 Error Event occured.
 *                      - CAN_IRQ_INTR_BIT0_EVENT: BIT0` Error Event occured.
 *                      - CAN_IRQ_INTR_CRC_EVENT: CRC Error Event occured.
 *                      - CAN_IRQ_INTR_TOO_EVENT: TOO  Error Event occured.
 *                      - CAN_IRQ_STATUS_READ_FAIL: Interrupt status read fail.
 *  Description       : This service will provide the status of the interrupt.
 * \endverbatim
 */
Can_IrqStatusType Can_GetIntrStatus(VAR(uint8, AUTOMATIC)Controller);

/**
 * \brief This service will clear the interrupt status.
 *
 * \verbatim
 *
 *  Service name      : Can_ClearIntrStatus
 *  Syntax            : Std_ReturnType Can_ClearIntrStatus(
 *                        VAR(uint8, AUTOMATIC)Controller)
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : N/A
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : Controller
 *                      - Controller for which it clears the interrupt
 *                        error status.
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Std_ReturnType
 *                      - E_OK: Clearing Interrupt Status is Success.
 *                      - E_NOT_OK: Clearing Interrupt Status Failed.
 *  Description       : This service will clear the interrupt status.
 * \endverbatim
 */
Std_ReturnType Can_ClearIntrStatus(VAR(uint8, AUTOMATIC)Controller);
#endif /* (STD_ON == CAN_SAFETY_DIAGNOSTIC) */
/**
 * \brief This function is the ISR for CAN controller 0/INT 0
 *
 * \verbatim
 *
 *  Function name     : Can_IntISR_Fun
 *  Syntax            : void Can_IntISR_Fun(Can_ControllerInstance CanInstanceID)
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : N/A
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non-Reentrant
 *  Parameters (in)   : None
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : This function is the ISR for CAN controller 0/INT 0 <br>
 *                      This function should be invoked either by OS Interrupt
 *                      frame handler <br>
 *                      CAN Interrupt frame handler for Category 1 ISR
 * \endverbatim
 */
FUNC(void, CAN_CODE) Can_IntISR_Fun(Can_ControllerInstance CanInstanceID);


/**
 * \brief This function is the ISR for CAN controller 0/INT 0
 *
 * \verbatim
 *
 *  Function name     : Can_IntISR_Fun
 *  Syntax            : void Can_IntISR_Fun(Can_ControllerInstance CanInstanceID)
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : N/A
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non-Reentrant
 *  Parameters (in)   : None
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : This function is the ISR for CAN controller 0/INT 0 <br>
 *                      This function should be invoked either by OS Interrupt
 *                      frame handler <br>
 *                      CAN Interrupt frame handler for Category 1 ISR
 * \endverbatim
 */
FUNC(void, CAN_CODE) Can_IntISR_Function(Can_ControllerInstance CanInstanceID,uint32 baseAddr);

/**
 *  \brief This function de-initializes the module.
 *
 *  \verbatim
 *  Service name      : Can_DeInit
 *  Syntax            : void Can_DeInit(void)
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non-Reentrant
 *  Parameters (in)   : None
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : This function de-initializes the module.
 *  \endverbatim
 */
FUNC(void, CAN_CODE) Can_DeInit(void);

/**
 * \brief This service reports about the current status of the requested
 *        CAN controller.
 *
 * \verbatim
 * Service name: Can_GetControllerMode
 * Syntax: Std_ReturnType Can_GetControllerMode(uint8 Controller,
 *                     Can_ControllerStateType* ControllerModePtr)
 * Mode            : User Mode (Non-Privileged Mode)
 * Service ID[hex]: 0x12
 * Sync/Async: Synchronous
 * Reentrancy: Non Reentrant
 * Parameters (in): Controller
 *                  - CAN controller for which the status shall be changed
 * Parameters (out): ControllerModePtr
 *                  - Pointer to a memory location,
 *                    where the current mode of the CAN controller will be stored.
 * Parameters (inout):  None
 * Return value: Std_ReturnType
 *               - E_OK: request accepted
 *               - E_NOT_OK: request not accepted.
 * Description:  This service reports about the current status of the requested
 *               CAN controller.
 * \endverbatim
 */

FUNC(Std_ReturnType, CAN_CODE) Can_GetControllerMode(uint8 Controller,
    Can_ControllerStateType* ControllerModePtr);

/**
 * \brief This service obtains the error state of the CAN controller.
 *
 * \verbatim
 * Service name: Can_GetControllerErrorState
 * Syntax: Std_ReturnType Can_GetControllerErrorState(uint8 Controller,
 *                     Can_ErrorStateType* ErrorStatePtr)
 * Mode           : Supervisor Mode (Privileged Mode)
 * Service ID[hex]: 0x11
 * Sync/Async: Synchronous
 * Reentrancy: Non Reentrant
 * Parameters (in): Controller
 *                  - CAN controller for which the status shall be changed
 * Parameters (out): ErrorStatePtr
 *                  - Pointer to a memory location,
 *                    where the error state of the CAN controller will be stored.
 * Parameters (inout):  None
 * Return value: Std_ReturnType
 *               - E_OK: request accepted
 *               - E_NOT_OK: request not accepted
 * Description:  This service obtains the error state of the CAN controller.
 * \endverbatim
 */

FUNC(Std_ReturnType, CAN_CODE) Can_GetControllerErrorState
    (uint8 ControllerId, Can_ErrorStateType* ErrorStatePtr);

#ifdef __cplusplus
}
#endif

#endif /* CAN_H_ end file*/
/*  End of File: Can.h */

/* @} */
