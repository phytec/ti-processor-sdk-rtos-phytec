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
 *  \file     Fls.h
 *
 *  \brief    This file contains interface header for Fls MCAL driver
 *
 */

/**
 *  \defgroup MCAL_FLS_API FLS Driver API
 *
 *  The flash driver provides services for reading, writing and erasing
 *  flash memory.<br>
 *  The Flash Driver implements a standardized interface specified in the
 *  AUTOSAR_SWS_FlashDriver document.<br>
 *
 *  Only the following combinations of modes are supported:
 *      - Direct Access Mode, Double Transfer Rate
 *      - Direct Access Mode, Single Transfer Rate
 *      - Indirect Access Mode, Double Tranfer Rate
 *      - Indirect Access Mode, Single Transfer Rate
 *      - Direct Access Mode, Poll Mode
 *      - Indirect Access Mode, Poll Mode
 *      - Indirect Access Mode, Interrupt Mode
 *      - Direct Access Mode, XIP mode
 *
 *  Following combinations of modes are not supported:
 *      - Direct Access Mode with Interrupt Mode, Cancel API
 *
 *  @sa MCAL_FLS_CFG
 *  @{
 */

/*
 * Below are the global requirements which are met by this FLS
 * driver which can't be mapped to a particular source ID
 */
/*
 * Design : MCAL-7370,MCAL-7251,MCAL-7373,MCAL-7357,MCAL-7387,MCAL-7461,MCAL-7446,MCAL-7391,MCAL-7277,MCAL-7323,MCAL-7272,MCAL-7475,MCAL-7363.MCAL-7347,MCAL-7352,MCAL-7444,MCAL-7373,MCAL-7405,MCAL-7338,MCAL-7347,MCAL-7461
 */

 #ifndef FLS_H_
 #define FLS_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */


#include "Std_Types.h"
#include "Os.h"
#include "Fls_Cfg.h"
#include "Fls_Irq.h"
#include "Fls_Cbk.h"
#include "MemIf_Types.h"
#include <SchM_Fls.h>

#ifdef __cplusplus
extern "C"
{
#endif


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \name FLS Driver ISR category level
 *
 *  Defines for FLS Driver ISR ISR category level
 *  @{
 */
/** \brief void ISR type */
#define FLS_ISR_VOID                    (0x00U)
/** \brief Category 1 ISR type */
#define FLS_ISR_CAT1                    (0x01U)
/** \brief Category 2 ISR type */
#define FLS_ISR_CAT2                    (0x02U)
/* @} */

/**
 *  \name FLS Driver Module SW Version Info
 *
 *  Defines for FLS Driver version used for compatibility checks
 *  @{
 */
/** \brief Driver Implementation Major Version */
#define FLS_SW_MAJOR_VERSION            (10U)
/** \brief Driver Implementation Minor Version */
#define FLS_SW_MINOR_VERSION            (1U)
/** \brief Driver Implementation Patch Version */
#define FLS_SW_PATCH_VERSION            (0U)
/* @} */

/**
 *  \name FLS Driver Module AUTOSAR Version Info
 *
 *  Defines for FLS Driver AUTOSAR version used for compatibility checks
 *  @{
 */
/** \brief AUTOSAR Major version specification implemented by FLS Driver */
#define FLS_AR_RELEASE_MAJOR_VERSION    (4U)
/** \brief AUTOSAR Minor  version specification implemented by FLS Driver */
#define FLS_AR_RELEASE_MINOR_VERSION    (3U)
/** \brief AUTOSAR Patch version specification implemented by FLS Driver */
#define FLS_AR_RELEASE_REVISION_VERSION (1U)
/* @} */

/**
 *  \name FLS Driver ID Info
 *  @{
 */
/** \brief Texas Instruments Vendor ID */
#define FLS_VENDOR_ID                   ((uint16) 44U)
/** \brief FLS Driver Module ID */
#define FLS_MODULE_ID                   ((uint16) 92U)
/** \brief FLS Driver Instance ID */
#define FLS_INSTANCE_ID                 ((uint8) 0U)
/* @} */


/** \brief Type of address type */
/*
 * Design : MCAL-7457
 */
typedef uint32 Fls_AddressType;
typedef uint32 Fls_LengthType;


/**
 *  \name FLS Error Codes
 *
 *  Error codes returned by FLS functions
 *  @{
 */
/*
 * Design : MCAL-7275,MCAL-7369,MCAL-7308,MCAL-7267,MCAL-7427,MCAL-7269,MCAL-7394,MCAL-7332,MCAL-7394,MCAL-7257,MCAL-7269,MCAL-7308,MCAL-7455,MCAL-7358
 */
#ifndef FLS_E_PARAM_CONFIG
/** \brief API service called with wrong parameter */
#define FLS_E_PARAM_CONFIG                      ((uint8) 0x01U)
#endif
#ifndef FLS_E_PARAM_ADDRESS
/** \brief API service called with wrong parameter */
#define FLS_E_PARAM_ADDRESS                     ((uint8) 0x02U)
#endif
#ifndef FLS_E_PARAM_LENGTH
/** \brief API service called with wrong parameter */
#define FLS_E_PARAM_LENGTH                      ((uint8) 0x03U)
#endif
#ifndef FLS_E_PARAM_DATA
/** \brief API service called with wrong parameter */
#define FLS_E_PARAM_DATA                        ((uint8) 0x04U)
#endif
#ifndef FLS_E_UNINIT
/** \brief API service used without module initiali-zation */
#define FLS_E_UNINIT                            ((uint8) 0x05U)
#endif
#ifndef FLS_E_BUSY
/** \brief APIs called when module is busy */
#define FLS_E_BUSY                              ((uint8) 0x06U)
#endif
#ifndef FLS_E_PARAM_POINTER
/** \brief APIs called with a Null Pointer */
#define FLS_E_PARAM_POINTER                     ((uint8) 0x0AU)
#endif
#ifndef FLS_E_VERIFY_ERASE_FAILED
/** \brief APIs service Erase Verification (BlankCheck) failed */
#define FLS_E_VERIFY_ERASE_FAILED               ((uint8) 0x07U)
#endif
#ifndef FLS_E_VERIFY_WRITE_FAILED
/** \brief APIs serice Write Verification (Compare) failed */
#define FLS_E_VERIFY_WRITE_FAILED               ((uint8) 0x08U)
#endif
#ifndef FLS_E_TIMEOUT
/** \brief APIs Timeout Exceeded*/
#define FLS_E_TIMEOUT                           ((uint8) 0x09U)
#endif


/*TRANSIENT FAULTS WITH HW*/
#ifndef FLS_E_ERASE_FAILED
/** \brief Flash Erase Failed in HW*/
#define FLS_E_ERASE_FAILED                      ((uint8) 0x01U)
#endif
#ifndef FLS_E_WRITE_FAILED
/** \brief Flash Write Failed in HW*/
#define FLS_E_WRITE_FAILED                      ((uint8) 0x02U)
#endif
#ifndef FLS_E_READ_FAILED
/** \brief Flash Read Failed in HW */
#define FLS_E_READ_FAILED                       ((uint8) 0x03U)
#endif
#ifndef FLS_E_COMPARE_FAILED
/** \brief Flash Compare Failediin HW */
#define FLS_E_COMPARE_FAILED                    ((uint8) 0x04U)
#endif
#ifndef FLS_E_UNEXPECTED_FLASH_ID
/** \brief Expected HW ID not matched */
#define FLS_E_UNEXPECTED_FLASH_ID               ((uint8) 0x05U)
#endif


/**
 *  \name FLS Service Ids
 *
 *  The Service Id is one of the argument to Det_ReportError function and
 *  is used to identify the source of the error
 *  @{
 */
/** \brief FLS_Init() API Service ID */
#define FLS_SID_INIT                            ((uint8) 0x00U)
/** \brief FLS_Erase() API Service ID */
#define FLS_SID_ERASE                           ((uint8) 0x01U)
/** \brief FLS_Write() API Service ID */
#define FLS_SID_WRITE                           ((uint8) 0x02U)
/** \brief FLS_Cancel() API Service ID */
#define FLS_SID_CANCEL                          ((uint8) 0x03U)
/** \brief FLS_GetStatus() API Service ID */
#define FLS_SID_GET_STATUS                      ((uint8) 0x04U)
/** \brief Fls_GetJobResult() API Service ID */
#define FLS_SID_GET_JOB_RESULT                  ((uint8) 0x05U)
/** \brief FLS_Read() API Service ID */
#define FLS_SID_READ                            ((uint8) 0x07U)
/** \brief FLS_Compare() API Service ID */
#define FLS_SID_COMPARE                         ((uint8) 0x08U)
/** \brief FLS_SetMode() API Service ID */
#define FLS_SID_SET_MODE                        ((uint8) 0x09U)
/** \brief Fls_GetVersionInfo() API Service ID */
#define FLS_SID_GET_VERSION_INFO                ((uint8) 0x10U)
/** \brief FLS_BlankCheck() API Service ID */
#define FLS_SID_BLANK_CHECK                     ((uint8) 0x0AU)
/** \brief FLS_MainFunction() API Service ID */
#define FLS_SID_MAIN_FUNCTION                   ((uint8) 0x06U)

/* @} */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *  \addtogroup MCAL_FLS_CFG FLS Configuration
 *  @{
 */

/**
 *  \brief FLS Module Flashable Sectors and pages
 */
/*
 * Design : MCAL-7339,MCAL-7328,MCAL-7248,MCAL-7298,MCAL-7415,MCAL-7411,MCAL-7431,MCAL-7460,MCAL-7434
 */
typedef struct
{
        Fls_LengthType        numberOfSectors;
        /**< Number of total Sectors in the Flash Device */
        Fls_LengthType        sectorPageSize;
        /**< Page size of the Flash Device */
        Fls_LengthType        sectorSize;
        /**< Size of each sector in the Flash Device */
        Fls_AddressType       sectorStartaddress;
        /**< Start address of first sector of the Flash Device */
} Fls_SectorType;

/**
 *  \brief  Number of lines used for OSPI read/write transaction
 */
typedef enum
{
    OSPI_XFER_LINES_SINGLE = 0U,
    /**< Single line is used */
    OSPI_XFER_LINES_DUAL,
    /**< Two lines are used */
    OSPI_XFER_LINES_QUAD,
    /**< Four lines are used */
    OSPI_XFER_LINES_OCTAL
    /**< Four lines are used */
} OSPI_xferLines;
/**
 *  \brief FLS Module Configuration Structure
 */
/*
 * Design : MCAL-7343,MCAL-7355,MCAL-7302,MCAL-7404,MCAL-7290,MCAL-7414,MCAL-7273,MCAL-7396,MCAL-7323,MCAL-7348,MCAL-7364,MCAL-7360,MCAL-7287,MCAL-7317,MCAL-7365,MCAL-7291,MCAL-7262
 */
typedef struct Fls_ConfigType_s
{
        Fls_JobEndNotifyType            Fls_JobEndNotification;
        /**<    Mapped to the job end notification routine provided by some upper layer
         *      module, typically the Fee module. */
        Fls_JobErrorNotifyType          Fls_JobErrorNotification;
        /**<    Mapped to the job error notification routine provided by some upper layer
         *      module, typically the Fee module. */
        uint32                          maxReadNormalMode;
        /**<    The maximum number of bytes to read or compare in one cycle of the
         *      flash driver's job processing function in normal mode. */
        uint32                          maxWriteNormalMode;
        /**<    The maximum number of bytes to write in one cycle of the
         *      flash driver's job processing function in normal mode. */
	    Fls_SectorType                  sectorList[1];
        /**<     The maximum number of bytes to write in one cycle of the flash driver's job
         *       processing function in normal mode.
         *       Using Fls Info Structure instead of this for now. */
        boolean                            dacEnable;
        /**<    Enable Direct Access Mode of Transfer in OSPI Flash */
        boolean                            xipEnable;
        /**<    Enable XIP mode in OSPI Flash */
        uint64                          ospiClkSpeed;
        /**<    OSPI Clock Speed */
        boolean                            dtrEnable;
        /**<    Enable Double transfer rate protocol for OSPI. */
        boolean                         phyEnable;
        /**<    Enable PHY high-speed mode for DAC mode */
		OSPI_xferLines                  ospixferlines;
		/**<    Select the OSPI read/write transaction Lines */
		uint32                          flsBaudRateDiv;
		/**<    To select the FLS baudrate Divider value */
		uint32                          blockSize;
		/**<    To select OSPI block size based on variants */
} Fls_ConfigType;

/* @} */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief Switch Mode of the OSPI Driver
 *  Parameters (in)     : boolean dacEnable - should DAC mode be enaled or disabled
 *  Parameters (in)     : boolean xipEnable - should XIP mode be enaled or disabled
 *  Mode                : Supervisor Mode (Privileged Mode)
 */
FUNC(void, FLS_CODE) Fls_SwitchMode(boolean dacEnable, boolean xipEnable);

/**
 *  \brief Initializes the Flash Driver.
 *
 *  \verbatim
 *  Service name        : Fls_Init
 *  Syntax              : void Fls_Init( const Fls_ConfigType* ConfigPtr )
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x00
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Non Reentrant
 *  Parameters (in)     : ConfigPtr - Pointer to flash driver configuration set
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : Initializes the Flash Driver
 *  \endverbatim
 */
FUNC(void, FLS_CODE) Fls_Init(
    P2CONST(Fls_ConfigType, AUTOMATIC, FLS_CONFIG_DATA) ConfigPtr);

/**
 *  \brief Erases flash sector(s).
 *
 *  \verbatim
 *  Service name        : Fls_Erase
 *  Syntax              : Std_ReturnType Fls_Erase(Fls_AddressType TargetAddress
 *                        ,Fls_LengthType Length)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x01
 *  Sync/Async          : Asynchronous
 *  Reentrancy          : Non Reentrant
 *  Parameters (in)     : TargetAddress - Target address in flash memory.
 *                        This address offset will be added to the flash memory
 *                        base address.
 *                        Length - Number of bytes to erase
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : Std_ReturnType -
 *                        E_OK: erase command has been accepted
 *                        E_NOT_OK: erase command has not been accepted
 *  Description         : Erases flash sector(s).
 *  \endverbatim
 */
FUNC(Std_ReturnType, FLS_CODE) Fls_Erase(Fls_AddressType TargetAddress, Fls_LengthType Length);

/**
 *  \brief  Writes one or more complete flash pages.
 *
 *  \verbatim
 *  Service name        : Fls_Write
 *  Syntax              : Std_ReturnType Fls_Write(Fls_AddressType TargetAddress
 *                        ,const uint8* SourceAddressPtr,
 *                        Fls_LengthType Length)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x02
 *  Sync/Async          : Asynchronous
 *  Reentrancy          : Non Reentrant
 *  Parameters (in)     : TargetAddress - Target address in flash memory.
 *                        This address offset will be added to the flash memory
 *                        base address.
 *                        SourceAddressPtr - Pointer to source data buffer
 *                        Length - Number of bytes to erase
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : Std_ReturnType -
 *                        E_OK: write command has been accepted
 *                        E_NOT_OK: write command has not been accepted
 *  Description         : Writes one or more complete flash pages.
 *  \endverbatim
 */
FUNC(Std_ReturnType, FLS_CODE) Fls_Write(Fls_AddressType TargetAddress,
    const uint8 *SourceAddressPtr, Fls_LengthType Length);

/**
 *  \brief Reads from flash memory..
 *
 *  \verbatim
 *  Service name        : Fls_Read
 *  Syntax              : Std_ReturnType Fls_Read((Fls_AddressType SourceAddress
 *                        ,const uint8* TargetAddressPtr,
 *                        Fls_LengthType Length)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x07
 *  Sync/Async          : Asynchronous
 *  Reentrancy          : Non Reentrant
 *  Parameters (in)     : SourceAddress - source address in flash memory.
 *                        This address offset will be added to the flash memory
 *                        base address.
 *                        TargetAddressPtr - Pointer to source data buffer
 *                        Length - Number of bytes to erase
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : Std_ReturnType -
 *                        E_OK: read command has been accepted
 *                        E_NOT_OK: read command has not been accepted
 *  Description         : Reads from flash memory.
 *  \endverbatim
 */
FUNC(Std_ReturnType, FLS_CODE) Fls_Read(Fls_AddressType SourceAddress,
                    uint8 *TargetAddressPtr,
                    Fls_LengthType Length);

/**
 *  \brief Compares the contents of an area of flash memory with
 *  that of an application data buffer.
 *
 *  \verbatim
 *  Service name        : Fls_Compare
 *  Syntax              : Std_ReturnType Fls_Compare
 *                        (Fls_AddressType SourceAddress,
 *                        const uint8* TargetAddressPtr,
 *                        Fls_LengthType Length)
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x08
 *  Sync/Async          : Asynchronous
 *  Reentrancy          : Non Reentrant
 *  Parameters (in)     : SourceAddress - Target address in flash memory.
 *                        This address offset will be added to the flash memory
 *                        base address.
 *                        TargetAddressPtr - Pointer to source data buffer
 *                        Length - Number of bytes to erase
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : Std_ReturnType -
 *                        E_OK: compare command has been accepted
 *                        E_NOT_OK: compare command has not been accepted
 *  Description         : Compares the contents of an area of flash memory
 *                        with that of an application data buffer.
 *  \endverbatim
 */
#if (STD_ON == FLS_COMPARE_API)
FUNC(Std_ReturnType, FLS_CODE) Fls_Compare(Fls_AddressType SourceAddress, const uint8 *TargetAddressPtr, Fls_LengthType Length);
#endif

/**
 *  \brief The function Fls_BlankCheck shall verify,
 *         whether a given memory area has been
 *         erased but not (yet) programmed.
 *
 *  \verbatim
 *  Service name        : Fls_BlankCheck
 *  Syntax              : Std_ReturnType Fls_Compare
 *                        (Fls_AddressType TargetAddress,,
 *                        Fls_LengthType Length)
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x0a
 *  Sync/Async          : Asynchronous
 *  Reentrancy          : Non Reentrant
 *  Parameters (in)     : TargetAddress - Target address in flash memory.
 *                        This address offset will be added to the flash memory
 *                        base address.
 *                        SourceAddressPtr - Pointer to source data buffer
 *                        Length - Number of bytes to erase
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : Std_ReturnType -
 *                        E_OK: BlankCheck command has been accepted
 *                        E_NOT_OK: BlankCheck command has not been accepted
 *  Description         : The function Fls_BlankCheck shall verify,
 *                        whether a given memory area has been
 *                        erased but not (yet) programmed.
 *  \endverbatim
 */
#if (STD_ON == FLS_BLANK_CHECK_API)
FUNC(Std_ReturnType, FLS_CODE) Fls_BlankCheck(Fls_AddressType Address, Fls_LengthType Length);
#endif

/**
 *  \brief Returns the driver state.
 *
 *  \verbatim
 *  Service name        : Fls_GetStatus
 *  Syntax              : MemIf_StatusType Fls_GetStatus( void )
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x04
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : None
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : MemIf_StatusType
 *  Description         : Returns the driver state.
 *  \endverbatim
 */
#if (STD_ON == FLS_GET_STATUS_API)
FUNC(MemIf_StatusType, FLS_CODE) Fls_GetStatus( void );
#endif

/**
 *  \brief This service returns the version information of this module.
 *
 *  \verbatim
 *  Service name        : Fls_GetVersionInfo
 *  Syntax              : void Fls_GetVersionInfo( Std_VersionInfoType*
 *                                                                 versioninfo )
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x10
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : None
 *  Parameters (inout)  : versioninfo - Pointer to where to store the version
 *                        information of this module
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : This service returns the version information of this
 *                        module
 *  \endverbatim
 */
#if (STD_ON == FLS_VERSION_INFO_API)
FUNC(Std_ReturnType, FLS_CODE) Fls_GetVersionInfo(Std_VersionInfoType *versioninfo);
#endif

/**
 *  \brief Returns the result of the last job.
 *
 *  \verbatim
 *  Service name        : Fls_GetJobResult
 *  Syntax              : MemIf_JobResultType Fls_GetJobResult(void)
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x05
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : None
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : MemIf_JobResultType
 *  Description         : Returns the result of the last job.
 *  \endverbatim
 */
#if (STD_ON == FLS_GET_JOB_RESULT_API)
FUNC(MemIf_JobResultType, FLS_CODE) Fls_GetJobResult( void );
#endif

/**
 *  \brief Performs the processing of jobs.
 *
 *  \verbatim
 *  Service name        : Fls_MainFunction
 *  Syntax              : void Fls_MainFunction(void)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x06
 *  Description         : Performs the processing of jobs.
 *  \endverbatim
 */
FUNC(void, FLS_CODE) Fls_MainFunction(void);

/**
 *  \brief Cancels an ongoing job.
 *
 *  \verbatim
 *  Service name        : Fls_Cancel
 *  Syntax              : void Fls_Cancel(void)
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x03
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Non Reentrant
 *  Parameters (in)     : None
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : Cancels an ongoing job.
 *  \endverbatim
 */
#if ( FLS_CANCEL_API == STD_ON )
FUNC(void, FLS_CODE) Fls_Cancel(void);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef FLS_H_ */

/* @} */
