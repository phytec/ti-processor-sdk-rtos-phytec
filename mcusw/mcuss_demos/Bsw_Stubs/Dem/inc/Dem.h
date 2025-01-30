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
 *  \file     Dem.h
 *
 *  \brief    This is a sample Diagnostic Event Manager interface header file.
 *            This file should be replaced by original Dem.h that is part of
 *            Integration AUTOSAR stack.
 */

#ifndef DEM_H
#define DEM_H

#ifdef __cplusplus
extern "C"{
#endif

/* ========================================================================== */
/*                           Include Files                                    */
/* ========================================================================== */
#include "Dem_Types.h"

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

#define DEM_START_SEC_CODE
#include "Dem_MemMap.h"
extern FUNC(void, DEM_CODE) Dem_Init(
                P2CONST(Dem_ConfigType, AUTOMATIC, DEM_APPL_DATA) ConfigPtr);
extern FUNC(void, DEM_CODE) Dem_PreInit(void);
#if defined (AUTOSAR_421)
extern FUNC(void, DEM_CODE) Dem_ReportErrorStatus(
                    VAR(Dem_EventIdType, AUTOMATIC) EventId,
                    VAR(Dem_EventStatusType, AUTOMATIC) EventStatus);
#endif
extern FUNC(Std_ReturnType, DEM_CODE) Dem_SetEventStatus(
            VAR(Dem_EventIdType, AUTOMATIC) EventId,
            VAR(Dem_EventStatusType, AUTOMATIC) EventStatus);
extern FUNC(void, DEM_CODE) Dem_GetVersionInfo(
            P2VAR(Std_VersionInfoType, AUTOMATIC, DEM_APPL_DATA) versioninfo);
#define DEM_STOP_SEC_CODE
#include "Dem_MemMap.h"

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
#define DEM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Dem_MemMap.h"
extern volatile VAR(Dem_EventIdType, DEM_VAR_CLEARED) Dem_EventId;
extern volatile VAR(Dem_EventStatusType, DEM_VAR_CLEARED) Dem_EventStatus;
#define DEM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Dem_MemMap.h"

/*
 * Ethernet MCAL driver error return
 */
/* Requirements : MCAL-1658 */
/** \brief Ethernet Controller Access Failure */
#define DemConf_DemEventParameter_ETH_E_ACCESS              (100U)

/* Requirements : MCAL-1659 */
/** \brief Ethernet Frames Lost */
#define DemConf_DemEventParameter_ETH_E_RX_FRAMES_LOST      (101U)

/** \brief Monitors invalid Ethernet frames during reception */
/* Requirements : MCAL-1704 */
#define DemConf_DemEventParameter_ETH_E_CRC                 (102U)

/** \brief Frame Size Underflow */
/* Requirements : MCAL-1705 */
#define DemConf_DemEventParameter_ETH_E_UNDERSIZEFRAME      (103U)

/** \brief Frame Size Overflow */
/* Requirements : MCAL-1706 */
#define DemConf_DemEventParameter_ETH_E_OVERSIZEFRAME       (104U)

/** \brief Frame Alignment Error */
/* Requirements : MCAL-1707 */
#define DemConf_DemEventParameter_ETH_E_ALIGNMENT           (105U)

/** \brief Single Frame Collision */
/* Requirements : MCAL-1708 */
#define DemConf_DemEventParameter_ETH_E_SINGLECOLLISION     (106U)

/** \brief Multiple Frame Collision */
/* Requirements : MCAL-1709 */
#define DemConf_DemEventParameter_ETH_E_MULTIPLECOLLISION   (107U)

/** \brief Late Frame Collision */
/* Requirements : MCAL-1710 */
#define DemConf_DemEventParameter_ETH_E_LATECOLLISION       (108U)

/**
 *  \brief An hardware error occurred during asynchronous or synchronous SPI
 *  transmit.
 *  This Extended Production Error shall be issued when any error bit inside
 *  the SPI hardware transmit status register is raised.
 */
#define DemConf_DemEventParameter_ETH_E_HARDWARE_ERROR      (109U)

/** \brief TX packet descriptor returned by UDMA is corrupted.
 *         Invalid data pointer in descriptor */
#define DemConf_DemEventParameter_ETH_E_TX_INTERNAL         (110U)

/*
 * Ethernet Transceiver MCAL driver error return
 */
/* Requirements : MCAL-1834 */
/** \brief Ethernet Transceiver Controller Access Failure */
#define DemConf_DemEventParameter_ETHTRCV_E_ACCESS          (300U)

/**
 *  This Extended Production Error shall be issued when DIO
 *  Write channel register setting fails
 */
#define DemConf_DemEventParameter_DIO_WRITE_CHANNEL_EVENT_ID   (65520U)

/**
 *  This Extended Production Error shall be issued when DIO
 *  register port register setting fails
 */
#define DemConf_DemEventParameter_DIO_WRITE_PORT_EVENT_ID      (65519U)

/**
 *  \brief An hardware error occurred during asynchronous or synchronous SPI
 *  transmit.
 *  This Extended Production Error shall be issued when any error bit inside
 *  the SPI hardware transmit status register is raised.
 */
#define DemConf_DemEventParameter_SPI_E_HARDWARE_ERROR         (65518U)

/**
 *  This Extended Production Error shall be issued when CAN
 *  register setting timeout occurs
 */
#define DemConf_DemEventParameter_CAN_E_HARDWARE_ERROR         (65517U)

/**
 *  This Extended Production Error shall be issued when GPT
 *  Timer register reset fails.
 */
#define DemConf_DemEventParameter_GPT_E_HARDWARE_ERROR         (65516U)

/**
 *  This Extended Production Error shall be issued when WDG
 *  initialization or mode switch failed because it would disable the watchdog.
 */
#define DemConf_DemEventParameter_WDG_E_DISABLE_REJECTED       (65515U)

/**
 *  This Extended Production Error shall be issued when WDG
 *  setting a watchdog mode failed (during initialization or mode switch).
 */
#define DemConf_DemEventParameter_WDG_E_MODE_FAILED            (65514U)

/**
 *  This Extended Production Error shall be issued when ADC
 *  register setting timeout occurs
 */
#define DemConf_DemEventParameter_ADC_E_HARDWARE_ERROR         (65513U)

/**
 *  This Extended Production Error shall be issued when CDD encounters
 *  internal error
 */
#define DemConf_DemEventParameter_CDD_IPC_E_HARDWARE_ERROR     (65512U)

#if defined (AUTOSAR_421)
void Dem_ReportErrorStatus(Dem_EventIdType     eventId,
                           Dem_EventStatusType eventStatus);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* DEM_H_ */
