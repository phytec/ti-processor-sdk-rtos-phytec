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
 *  \file     Spi.h
 *
 *  \brief    This file contains interface header for SPI MCAL driver
 *
 */

/**
 *  \defgroup MCAL_SPIHANDLER_API SPI Handler and Driver API
 *
 *  The SPI Driver MCAL provides low level access to SPI peripheral
 *  present on Device.<br>
 *  The SPI Driver implements a standardized interface specified in the
 *  AUTOSAR_SWS_SPIHandlerDriver document.<br>
 *
 *  Limitations of the driver:
 *      - A job could belong to several sequences but can't be active at the
 *        same time i.e. a job queued in a sequence cannot be queued via
 *        another sequence. This is a design limitation to reduce driver
 *        complexity.
 *      - A channel could belong to several sequences or jobs but can't be
 *        active at the same time i.e. a channel in a job in a sequence
 *        cannot be part of another active job or sequeuence.
 *        This is a design limitation to reduce driver complexity.
 *      - Non-Interruptible sequences applies only within a HW unit. If a
 *        sequence is started, another high priority job belonging to another
 *        sequence cannot interrupt the job belonging to the same hardware unit.
 *        But the job can be scheduled ahead of another pending job of the
 *        started sequence if it belongs to another HW queue. This is
 *        illustrated in below example
 *
 *        Example of non-interruptible sequence across HW units:
 *              SEQ1 - JOB1 (HW1, P0)
 *              SEQ2 - JOB2 (HW2, P0), JOB3 (HW1, P0)
 *              SEQ3 - JOB4 (HW2, P3)
 *
 *        Consider the above sequence of calls happening back to back at
 *        time T1. The job schedule for this case will be
 *
 *              Time T1 - JOB1 and JOB2     (Since different HW)
 *
 *              Time T2 - JOB4              (could interrupts SEQ2 JOB3 if JOB1
 *                                           takes more time that JOB2)
 *
 *              Time T3 - JOB3
 *
 *  \sa MCAL_SPI_CFG
 *  \sa MCAL_SPI_IRQ_API
 *  \sa MCAL_SPI_DBG_API
 *
 *  @{
 */

/*
 * Below are the global design requirements which are met by this SPI handler
 * driver which can't be mapped to a particular source ID
 */
/*
 * Design: MCAL-6422,MCAL-6412,MCAL-6690,MCAL-6487,MCAL-6429,MCAL-6683,MCAL-6384,MCAL-6478,MCAL-6383,MCAL-6593,MCAL-6573,MCAL-6381,MCAL-6492,MCAL-6610,MCAL-6658,MCAL-6451,MCAL-6449,MCAL-6448,MCAL-6581,MCAL-6710,MCAL-6527,MCAL-6642,MCAL-6458,MCAL-6544
 */

/*
 * Below are the SPI's module environment design requirements which can't be mapped
 * to this driver.
 */
/*
 * Design: MCAL-6719,MCAL-6486,MCAL-6415,MCAL-6685,MCAL-6421,MCAL-6390,MCAL-6608,MCAL-6670,MCAL-6643
 */

#ifndef SPI_H_
#define SPI_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "Std_Types.h"
#include "Spi_Cfg.h"
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
#include <ti/drv/udma/udma.h>
#endif
#include "Spi/mcspi_hw/V0/mcspi.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \name SPI Driver Module SW Version Info
 *
 *  Defines for SPI Driver version used for compatibility checks
 *  @{
 */
/** \brief Driver Implementation Major Version */
#define SPI_SW_MAJOR_VERSION            (9U)
/** \brief Driver Implementation Minor Version */
#define SPI_SW_MINOR_VERSION            (0U)
/** \brief Driver Implementation Patch Version */
#define SPI_SW_PATCH_VERSION            (1U)
/* @} */

/**
 *  \name SPI Driver Module AUTOSAR Version Info
 *
 *  Defines for SPI Driver AUTOSAR version used for compatibility checks
 *  @{
 */
/** \brief AUTOSAR Major version specification implemented by SPI Driver */
#define SPI_AR_RELEASE_MAJOR_VERSION    (4U)
/** \brief AUTOSAR Minor  version specification implemented by SPI Driver */
#define SPI_AR_RELEASE_MINOR_VERSION    (3U)
/** \brief AUTOSAR Patch version specification implemented by SPI Driver */
#define SPI_AR_RELEASE_REVISION_VERSION (1U)
/* @} */

/**
 *  \name SPI Driver ID Info
 *  @{
 */
/** \brief Texas Instruments Vendor ID */
#define SPI_VENDOR_ID                   ((uint16) 44U)
/** \brief SPI Driver Module ID */
#define SPI_MODULE_ID                   ((uint16) 83U)
/** \brief SPI Driver Instance ID */
#define SPI_INSTANCE_ID                 ((uint8) 0U)
/* @} */

/**
 *  \name SPI Buffer Definition
 *  @{
 */
/** \brief Buffer definitions IB - internal buffers */
#define SPI_IB                          (0U)
/** \brief Buffer definitions EB - external buffers. Only this is supported */
#define SPI_EB                          (1U)
/** \brief Buffer definitions IB/EB - internal/external buffers */
#define SPI_IB_EB                       (2U)
/* @} */


/** \brief Type of application data buffer elements */
/*
 * Design: MCAL-6691,MCAL-6394,MCAL-6615
 */
typedef uint8 Spi_DataBufferType;
/**
 *  \brief Type for defining the number of data elements of the type
 *  Spi_DataBufferType to send and / or receive by Channel
 */
/*
 * Design: MCAL-6625,MCAL-6669
 */
typedef uint16 Spi_NumberOfDataType;
/** \brief Specifies the identification (ID) for a Channel */
/*
 * Design: MCAL-6528,MCAL-6493,MCAL-6687
 */
typedef uint8 Spi_ChannelType;
/** \brief Specifies the identification (ID) for a Job */
/*
 * Design: MCAL-6457,MCAL-6689,MCAL-6728
 */
typedef uint16 Spi_JobType;
/** \brief Specifies the identification (ID) for a sequence of jobs */
/*
 * Design: MCAL-6639,MCAL-6505,MCAL-6729
 */
typedef uint8 Spi_SequenceType;
/**
 *  \brief Specifies the identification (ID) for a SPI Hardware micro controller
 *  peripheral (unit)
 */
/*
 * Design: MCAL-6655,MCAL-6682,MCAL-6707
 */
typedef uint8 Spi_HWUnitType;

/**
 *  \name SPI Error Codes
 *
 *  Error codes returned by SPI functions
 *  @{
 */
#ifndef SPI_E_PARAM_CHANNEL
/** \brief API service called with wrong parameter */
#define SPI_E_PARAM_CHANNEL             ((uint8) 0x0AU)
#endif
#ifndef SPI_E_PARAM_JOB
/** \brief API service called with wrong parameter */
#define SPI_E_PARAM_JOB                 ((uint8) 0x0BU)
#endif
#ifndef SPI_E_PARAM_SEQ
/** \brief API service called with wrong parameter */
#define SPI_E_PARAM_SEQ                 ((uint8) 0x0CU)
#endif
#ifndef SPI_E_PARAM_LENGTH
/** \brief API service called with wrong parameter */
#define SPI_E_PARAM_LENGTH              ((uint8) 0x0DU)
#endif
#ifndef SPI_E_PARAM_UNIT
/** \brief API service called with wrong parameter */
#define SPI_E_PARAM_UNIT                ((uint8) 0x0EU)
#endif
#ifndef SPI_E_PARAM_POINTER
/** \brief APIs called with a Null Pointer */
#define SPI_E_PARAM_POINTER             ((uint8) 0x10U)
#endif
#ifndef SPI_E_UNINIT
/** \brief API service used without module initiali-zation */
#define SPI_E_UNINIT                    ((uint8) 0x1AU)
#endif
#ifndef SPI_E_SEQ_PENDING
/** \brief Services called in a wrong sequence */
#define SPI_E_SEQ_PENDING               ((uint8) 0x2AU)
#endif
#ifndef SPI_E_SEQ_IN_PROCESS
/** \brief Synchronous transmission service called at wrong time */
#define SPI_E_SEQ_IN_PROCESS            ((uint8) 0x3AU)
#endif
#ifndef SPI_E_ALREADY_INITIALIZED
/**
 *  \brief API Spi_Init service called while the SPI driver has already been
 *  initialized
 */
#define SPI_E_ALREADY_INITIALIZED       ((uint8) 0x4AU)
#endif
#ifndef SPI_E_SEQUENCE_NOT_OK
/** \brief Sequence did not complete successfully */
#define SPI_E_SEQUENCE_NOT_OK           ((uint8) 0x5AU)
#endif

/* @} */

/**
 *  \name SPI Service Ids
 *
 *  The Service Id is one of the argument to Det_ReportError function and
 *  is used to identify the source of the error
 *  @{
 */
/** \brief Spi_Init() API Service ID */
#define SPI_SID_INIT                    ((uint8) 0x00U)
/** \brief Spi_DeInit() API Service ID */
#define SPI_SID_DEINIT                  ((uint8) 0x01U)
/** \brief Spi_WriteIB() API Service ID */
#define SPI_SID_WRITE_IB                ((uint8) 0x02U)
/** \brief Spi_AsyncTransmit() API Service ID */
#define SPI_SID_ASYNC_TRANSMIT          ((uint8) 0x03U)
/** \brief Spi_ReadIB() API Service ID */
#define SPI_SID_READ_IB                 ((uint8) 0x04U)
/** \brief Spi_SetupEB() API Service ID */
#define SPI_SID_SETUP_EB                ((uint8) 0x05U)
/** \brief Spi_GetStatus() API Service ID */
#define SPI_SID_GET_STATUS              ((uint8) 0x06U)
/** \brief Spi_GetJobResult() API Service ID */
#define SPI_SID_GET_JOB_RESULT          ((uint8) 0x07U)
/** \brief Spi_GetSequenceResult() API Service ID */
#define SPI_SID_GET_SEQ_RESULT          ((uint8) 0x08U)
/** \brief Spi_GetVersionInfo() API Service ID */
#define SPI_SID_GET_VERSION_INFO        ((uint8) 0x09U)
/** \brief Spi_SyncTransmit() API Service ID */
#define SPI_SID_SYNC_TRANSMIT           ((uint8) 0x0AU)
/** \brief Spi_GetHWUnitStatus() API Service ID */
#define SPI_SID_GET_HW_UNIT_STATUS      ((uint8) 0x0BU)
/** \brief Spi_Cancel() API Service ID */
#define SPI_SID_CANCEL                  ((uint8) 0x0CU)
/** \brief Spi_SetAsyncMode() API Service ID */
#define SPI_SID_SET_ASYNC_MODE          ((uint8) 0x0DU)
/** \brief Spi_MainFunction_Handling() API Service ID */
#define SPI_SID_MAINFUNCTION_HANDLING   ((uint8) 0x10U)
/* @} */

/**
 *  \brief MCSPI functional clock input in Hz.
 *   This clock is used to generate the serial clock output after the divider.
 *
 *   Note: This is given here for reference and can't be changed.
 */
/*
 * Design: MCAL-6699
 */
#define SPI_MCSPI_FCLK                  (48000000U)

/**
 *  \name SPI Config Ids
 *
 *  The Config Ids used for different
 *  SPI Configuration builds
 *  @{
 */
/** \brief Config 0 (All configurations ON), Scaleability level 2) */
#define  SPI_CFG_ID_0     (0x01U)
/** \brief Config 1 (All configurations OFF).
 *         SPI_CFG_ID_1 is used only for compile check */
#define  SPI_CFG_ID_1     (0x02U)
/** \brief Config 2 (All configurations ON except DET OFF) */
#define  SPI_CFG_ID_2     (0x04U)
/** \brief Config 3 (Configurator auto generated file) */
#define  SPI_CFG_ID_3     (0x08U)
/** \brief Config 4 (Config is used only by Unit Test, Scaleability level 0) */
#define  SPI_CFG_ID_4     (0x10U)
/** \brief Config 5 (Config is used only by Unit Test for DMA  */
#define  SPI_CFG_ID_5     (0x20U)
/** \brief Config 6 (Config is used only by Unit Test for DMA */
#define  SPI_CFG_ID_6     (0x40U)
/** \brief Config 7 (Performance testing only, level 2, asynch, no DMA)*/
#define  SPI_CFG_ID_7     (0x80U)
/* @} */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *  \addtogroup MCAL_SPI_CFG SPI Configuration
 *  @{
 */

/**
 *  \brief This type defines a range of specific status for SPI Handler/Driver
 */
/*
 * Design: MCAL-6531,MCAL-6648,MCAL-6537,MCAL-6574
 */
typedef enum
{
    SPI_UNINIT = 0U,
    /**< The SPI Handler/Driver is not initialized or not usable */
    SPI_IDLE = 1U,
    /**< The SPI Handler/Driver is not currently transmitting any Job */
    SPI_BUSY = 2U
    /**< The SPI Handler/Driver is performing a SPI Job (transmit) */
} Spi_StatusType;

/**
 *  \brief This type defines a range of specific Jobs status for SPI
 *  Handler/Driver
 */
/*
 * Design: MCAL-6703,MCAL-6425,MCAL-6430
 */
typedef enum
{
    SPI_JOB_OK = 0U,
    /**< The last transmission of the Job has been finished successfully */
    SPI_JOB_PENDING = 1U,
    /**< The SPI Handler/Driver is performing a SPI Job.
     *   The meaning of this status is equal to SPI_BUSY */
    SPI_JOB_FAILED = 2U,
    /**< The last transmission of the Job has failed */
    SPI_JOB_QUEUED = 3U
    /**< An asynchronous transmit Job has been accepted, while
     * actual transmission for this Job has not started yet */
} Spi_JobResultType;

/**
 *  \brief This type defines a range of specific Sequences status for SPI
 *  Handler/Driver
 */
/*
 * Design: MCAL-6512,MCAL-6607,MCAL-6686
 */
typedef enum
{
    SPI_SEQ_OK = 0U,
    /**< The last transmission of the Sequence has been finished successfully */
    SPI_SEQ_PENDING = 1U,
    /**< The SPI Handler/Driver is performing a SPI Sequence. The meaning of
     *   this status is equal to SPI_BUSY */
    SPI_SEQ_FAILED = 2U,
    /**< The last transmission of the Sequence has failed */
    SPI_SEQ_CANCELLED = 3U
    /**< The last transmission of the Sequence has been canceled by user */
} Spi_SeqResultType;

/**
 *  \brief This type defines a range of specific HW unit status for SPI
 *  Handler/Driver
 */
typedef enum
{
    SPI_HW_UNIT_OK = 0U,
    /**< HW unit result is ok */
    SPI_HW_UNIT_PENDING = 1U,
    /**< HW unit result is pending */
    SPI_HW_UNIT_FAILED = 2U
    /**< HW unit result is failed */
} Spi_HwUnitResultType;

/**
 *  \brief Specifies the asynchronous mechanism mode for SPI busses handled
 *  asynchronously in LEVEL 2
 */
/*
 * Design: MCAL-6502,MCAL-6659,MCAL-6420,MCAL-6475,MCAL-6419,MCAL-6517,MCAL-6641
 */
typedef enum
{
    SPI_POLLING_MODE = 0U,
    /**< The asynchronous mechanism is ensured by polling, so interrupts
     *   related to SPI busses handled asynchronously are disabled */
    SPI_INTERRUPT_MODE = 1U
    /**< The asynchronous mechanism is ensured by
     * interrupt, so interrupts related to SPI busses
     * handled asynchronously are enabled */
} Spi_AsyncModeType;

/**
 *  \brief Word transfer order - MSB first or LSB first
 */
typedef enum
{
    SPI_MSB = 0U,
    /**< MSB is transferred first. Only this is supported */
    SPI_LSB = 1U
    /**< LSB is transferred first. This is not supported */
} Spi_TransferType;

/**
 *  \brief Type for SPI Chip Select Polarity and Clock Idle Level.
 */
typedef enum
{
    SPI_LOW = STD_LOW,
    /**< Low clock or chip select */
    SPI_HIGH
    /**< High clock or chip select */
} Spi_LevelType;

/**
 *  \brief SPI Chip Select Pin.
 */
typedef enum
{
    SPI_CS0 = 0U,
    /**< Chip select 0 */
    SPI_CS1,
    /**< Chip select 1 */
    SPI_CS2,
    /**< Chip select 2 */
    SPI_CS3
    /**< Chip select 3 */
} Spi_CsPinType;

/**
 *  \brief SPI Clock Mode - sets the clock polarity and phase.
 *   Note: These values are a direct register mapping. So don't change value.
 */
typedef enum
{
    SPI_CLK_MODE_0 = 0x00U,
    /**< SPI Clock Phase = 0 (rising edge latch),  Polarity = 0 (Active HIGH) */
    SPI_CLK_MODE_1 = 0x01U,
    /**< SPI Clock Phase = 1 (falling edge latch), Polarity = 0 (Active HIGH) */
    SPI_CLK_MODE_2 = 0x02U,
    /**< SPI Clock Phase = 0 (rising edge latch),  Polarity = 1 (Active LOW) */
    SPI_CLK_MODE_3 = 0x03U,
    /**< SPI Clock Phase = 1 (falling edge latch), Polarity = 1 (Active LOW) */
} Spi_ClkMode;

/**
 *  \brief SPI TX/RX Mode.
 *
 *   Note:
 *      1. These values are a direct register mapping. So don't change value.
 *      2. RX only mode doesn't make sense in master mode because to receive
 *         data the master has to generate clock, which means it should
 *         transmit. Hence this mode is not supported. The user can
 *         alternatively set the TX buffer pointer to NULL and set the
 *         default TX value (defaultTxData) to make TX data line at the desired
 *         level.
 */
typedef enum
{
    SPI_TX_RX_MODE_BOTH = 0x00U,
    /**< Both TX and RX are enabled */
    SPI_TX_RX_MODE_TX_ONLY = 0x02U,
    /**< Only TX is enabled */
} Spi_TxRxMode;

/**
 *  \brief SPI Job Priority.
 */
/*
 * Design: MCAL-6597
 */
typedef enum
{
    SPI_JOB_PRIORITY_0 = 0U,
    /**< Job priority 0 - low */
    SPI_JOB_PRIORITY_1,
    /**< Job priority 1*/
    SPI_JOB_PRIORITY_2,
    /**< Job priority 2 */
    SPI_JOB_PRIORITY_3
    /**< Job priority 3 - High */
} Spi_JobPriorityType;

/**
 *  \brief SPI Chip Select Mode.
 */
typedef enum
{
    SPI_SINGLE = 0U,
    /**< Chip select mode - single. Active only when transfer is on. */
    SPI_CONTINUOUS = 1U
                     /**< Chip select mode - continuous. Active throughout. */
} Spi_CsModeType;

/**
 *  \brief Spi_DataDelayType defines the number of interface clock cycles between
 *         CS toggling and first or last edge of MCSPI clock.
 */
typedef enum
{
    SPI_DATADELAY_0 = 0U,
    /**< 0.5 clock cycles delay */
    SPI_DATADELAY_1 = 1U,
    /**< 1.5 clock cycles delay */
    SPI_DATADELAY_2 = 2U,
    /**< 2.5 clock cycles delay */
    SPI_DATADELAY_3 = 3U,
    /**< 3.5 clock cycles delay */
} Spi_DataDelayType;

/**
 *  \brief Spi_DataLineReceiveType defines the lines selected for reception
 */
typedef enum
{
    DATA_LINE_0_RECEPTION = 0U,
    /**< Data line 0 (SPIDAT[0]) selected for reception */
    DATA_LINE_1_RECEPTION = 1U,
    /**< Data line 1 (SPIDAT[1]) selected for reception */
} Spi_DataLineReceiveType;

/**
 *  \brief Spi_DataLineTransmitType defines the lines selected for transmission
 */
typedef enum
{
    DATA_LINE_NO_TRANSMISSION = 0x3U,
    /**< No transmission on data lines */
    DATA_LINE_0_TRANSMISSION = 0x2U,
    /**< Data line 0 (SPIDAT[0]) selected for transmission */
    DATA_LINE_1_TRANSMISSION = 0x1U,
    /**< Data line 1 (SPIDAT[1]) selected for transmission */
    DATA_LINE_BOTH_TRANSMISSION = 0x0U,
    /**< Data line 0 and 1 (SPIDAT[0] & SPIDAT[1]) selected for transmission */
} Spi_DataLineTransmitType;

/**
 * \brief Irq status and std return type
 */
typedef enum
{
    SPI_NO_EVENT = 0U,
    /**< No event ocuurs in Overflow/Under flow */
    SPI_EVENT_PENDING = 1U,
    /**< Event occurs in Overflow/Under flow */
    SPI_STATUS_READ_FAIL = 2U,
    /**< The status reading fails */
} Mcspi_IrqStatusType;

/**
 *  \brief Cache write-back invalidate function
 *
 *  Pointer to a function that performs the cache write-back invalidate
 *  operation.  This function is to be called on TX buffers before they
 *  are given to the Spi controller hardware.
 */
typedef void (*Spi_CacheWbInv)(uint8 *BufPtr,
                               uint16 LenByte);

/**
 *  \brief Cache write-back function
 *
 *  Pointer to a function that performs the cache write-back operation.
 *  This function is to be called on TX buffers before they are given to
 *  the Spi controller hardware.
 */
typedef void (*Spi_CacheWb)(uint8 *BufPtr,
                            uint16 LenByte);

/**
 *  \brief Cache invalidate function
 *
 *  Pointer to a function that performs the cache invalidate operation.
 *  This function is to be called on RX buffers after they have been
 *  retrieved from the Spi controller hardware.
 */
typedef void (*Spi_CacheInv)(uint8 *BufPtr,
                             uint16 LenByte);

/**
 *  \brief SPI Channel configuration structure.
 */
/*
 * Design: MCAL-6529,MCAL-6519,MCAL-6649,MCAL-6716,MCAL-6619
 */
typedef struct
{
    uint8                 channelBufType;
    /**< Buffer Type IB/EB */
    uint8                 dataWidth;
    /**< Width of clock frame in bits.
     *   In case of McSPI, valid values are from 1 to 32 bits. */
    uint32                defaultTxData;
    /**< Default transmit value when TX buffer is NULL */
    Spi_NumberOfDataType  maxBufLength;
    /**< Max data length for external or internal buffer in SPI words.
     *   In case of internal buffers, this represents the number of words to
     *   copy from application buffer to internal buffer in case of
     *   Spi_WriteIB() API or from internal buffer to application buffer
     *   in case of Spi_ReadIB().
     *   The value of this should be less than or equal to the SPI
     *   configuration SPI_IB_MAX_LENGTH as this macro is is used for buffer
     *   allocation.
      */
    Spi_TransferType      transferType;
    /**< Start with MSB or LSB.
     *   Only MSB is supported.  */
} Spi_ChannelConfigType;

/**
 *  \brief SPI Job configuration structure specific to McSPI peripheral.
 */
typedef struct
{
    uint16            csEnable;
    /**< Chip select functionality on/off */
    Spi_CsModeType    csMode;
    /**< Select single or continuous mode
     *   Note: Applicable only for McSPI; */
    Spi_LevelType     csPolarity;
    /**< Chip select pin polarity high or low */
    Spi_DataDelayType csIdleTime;
    /**< CS idle time (Timing between clock and chip select) if single mode
     *   is chosen. Values in case of McSPI
     *                  McSPI
     *   0x00   -   0.5 clock cycles
     *   0x01   -   1.5 clock cycles
     *   0x02   -   2.5 clock cycles
     *   0x03   -   3.5 clock cycles */
    uint32            clkDivider;
    /**< Clock divider. This is used to derive the required baudrate from
     *   the McSPI functional clock. This value should be 1 less
     *   than the actual divider value. So a value of 0 means the divider is 1.
     *
     *   Maximum allowed value of divider is 4095(12 bit register field) */
    Spi_ClkMode       clkMode;
    /**< Mode 0 = {0=CPOL,0=CPHA}; Mode 1={0,1}; Mode 2={1,0} Mode 3={1,1} */
    Spi_TxRxMode      txRxMode;
    /**< TX and RX mode */
    uint16            startBitEnable;
    /**< Start bit D/CX added before SPI transfer. Polarity is defined by
     *   start bit level (below). */
    Spi_LevelType     startBitLevel;
    /**< Start-bit polarity used when startBitEnable is TRUE. */
    Spi_DataLineReceiveType     receptionLineEnable;
    /**< Defines the data lines selected for reception. */
    Spi_DataLineTransmitType    transmissionLineEnable;
    /**< Defines the data lines selected for transmission. */
} Spi_McspiExternalDeviceConfigType;

/**
 *  \brief SPI external device specific configuration structure .
 */
typedef struct
{
    Spi_McspiExternalDeviceConfigType mcspi;
    /**< MCSPI HW specific external device config. Should be populated only
     * if hwUnitId is MCSPI */
} Spi_ExternalDeviceConfigType;

/**
 *  \brief SPI Job configuration structure.
 */
/*
 * Design: MCAL-6692,MCAL-6437,MCAL-6684,MCAL-6522,MCAL-6572,MCAL-6406,MCAL-6632,MCAL-6476
 */
typedef struct
{
    Spi_JobPriorityType  jobPriority;
    /**< Job priority */
    Spi_HWUnitType       hwUnitId;
    /**< HWUnit associated with this job */
    Spi_JobEndNotifyType Spi_JobEndNotification;
    /**< Job end notification callback fxn pointer */
    uint32               channelPerJob;
    /**< Number of channels for this job.
     *   Should not be more than SPI_MAX_CHANNELS_PER_JOB */
    Spi_ChannelType      channelList[SPI_MAX_CHANNELS_PER_JOB];
    /**< Channel index list */
} Spi_JobConfigType;

/**
 *  \brief SPI Sequence configuration structure.
 */
/*
 * Design: MCAL-6496,MCAL-6536,MCAL-6742,MCAL-6413
 */
typedef struct
{
    uint8                seqInterruptible;
    /**< Sequence interruptible or not (TRUE/FALSE) */
    Spi_SeqEndNotifyType Spi_SequenceEndNotification;
    /**< Sequence end notification callback fxn pointer */
    uint32               jobPerSeq;
    /**< Number of jobs for this sequence.
     *   Should not be more than SPI_MAX_JOBS_PER_SEQ */
    Spi_JobType          jobList[SPI_MAX_JOBS_PER_SEQ];
    /**< Job index list */
} Spi_SeqConfigType;

/**
 *  \brief SPI Hardware unit configuration structure.
 */
typedef struct
{
    Spi_HWUnitType hwUnitId;
    /**< SPI HW unit to use */
    boolean        enabledmaMode;
    /**< Enable SPI DMA Support per instance */
    uint32         dmaTxChIntrNum;
    /**< DMA TX channel interrupt number */
    uint32         dmaRxChIntrNum;
    /**< DMA RX channel interrupt number */
} Spi_HwUnitConfigType;

/**
 *  \brief SPI config structure
 */
/*
 * Design: MCAL-6570,MCAL-6423,MCAL-6588,MCAL-6485,MCAL-6733
 */
typedef struct Spi_ConfigType_s
{
    uint8                        maxChannels;
    /**< Maximum number of channels.
     *   Should not be more than SPI_MAX_CHANNELS */
    uint8                        maxJobs;
    /**< Maximum number of jobs
     *   Should not be more than SPI_MAX_JOBS */
    uint8                        maxSeq;
    /**< Maximum number of sequences
     *   Should not be more than SPI_MAX_SEQ */
    uint8                        maxHwUnit;
    /**< Maximum number of HW unit
     *   Should not be more than SPI_MAX_HW_UNIT */
    uint8                        maxExtDevCfg;
    /**< Maximum number of external device configurations
     *   Should not be more than SPI_MAX_EXT_DEV */
    uint32                       udmaInstId;
    /**< UDMA Instance Id. Based on core in which the SpiDriver is running
     *  Udma Instance should be chosen
     *  - UDMA_INST_ID_MCU_0 for cores in MCU domain
     *  - UDMA_INST_ID_MAIN_0 for cores in Main domain
     */
    Spi_CacheWbInv               cacheWbInv;
    /**< Cache write-back invalidate function */
    Spi_CacheWb                  cacheWb;
    /**< Cache write-back function */
    Spi_CacheInv                 cacheInv;
    /**< Cache invalidate function */
    Spi_ChannelConfigType        channelCfg[SPI_MAX_CHANNELS];
    /**< Channel configurations */
    Spi_JobConfigType            jobCfg[SPI_MAX_JOBS];
    /**< Job configurations */
    Spi_SeqConfigType            seqCfg[SPI_MAX_SEQ];
    /**< Sequence configurations */
    Spi_HwUnitConfigType         hwUnitCfg[SPI_MAX_HW_UNIT];
    /**< HW Unit configurations */
    Spi_ExternalDeviceConfigType extDevCfg[SPI_MAX_EXT_DEV];
    /**< External HW device config */
} Spi_ConfigType;

/**
 *  \brief SPI channel config structure parameters Pre-Compile only
 */
typedef struct Spi_ChannelConfigType_PC_s
{
    Spi_ChannelType      channelId;
    /**< Channel ID */
} Spi_ChannelConfigType_PC;

/*
 * Design: MCAL-6717,MCAL-6650
 */
/**
 *  \brief SPI job config structure parameters Pre-Compile only
 */
typedef struct Spi_JobConfigType_PC_s
{
    Spi_JobType          jobId;
    /**< Job ID */
    Spi_CsPinType        csPin;
    /**< Chip select pin to use */
    uint8                externalDeviceCfgId;
    /**< index into SpiConfig.extDevCfg[] indicating the devCfg associated
     *   with the job */
} Spi_JobConfigType_PC;

/**
 *  \brief SPI sequence config structure parameters Pre-Compile only
 */
typedef struct Spi_SeqConfigType_PC_s
{
    Spi_SequenceType     seqId;
    /**< Sequence ID */
} Spi_SeqConfigType_PC;

#if (STD_ON == SPI_REGISTER_READBACK_API)
/**
 *  \brief SPI register readback structure
 *
 */
typedef struct
{
    /*
     * McSPI related registers
     */
    uint32       mcspiHlRev;
    /**< IP revision identifier */
    uint32       mcspiHlHwInfo;
    /**< Information about the IP module's hardware configuration */
    uint32       mcspiHlSysConfig;
    /**< Clock management configuration */
    uint32       mcspiRev;
    /**< IP revision number */
    uint32       mcspiSysStatus;
    /**< IP status information */
    uint32       mcspiSyst;
    /**< System config */
    uint32       mcspiModulctrl;
    /**< module ctrl register MCSPI_MODULCTRL */
    uint32       mcspiSysConfig;
    /**< sysconfig register MCSPI_SYSCONFIG */
    uint32       mcspiCh0config;
    uint32       mcspiCh1config;
    uint32       mcspiCh2config;
    uint32       mcspiCh3config;
    /**< module Channel config Register MCSPI_CHCONF_0/1/2/3 */
    uint32 	     mcspiIrqenable;
    /**< Irqenable MCSPI_IRQENABLE */
} Spi_RegisterReadbackType;
#endif  /* #if (STD_ON == SPI_REGISTER_READBACK_API) */
/* @} */
/* @} */
/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief Service for SPI initialization.
 *
 *  \verbatim
 *  Service name        : Spi_Init
 *  Syntax              : void Spi_Init( const Spi_ConfigType* CfgPtr )
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x00
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Non Reentrant
 *  Parameters (in)     : CfgPtr - Pointer to configuration set
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : Service for SPI initialization
 *  \endverbatim
 */
FUNC(void, SPI_CODE) Spi_Init(
    P2CONST(Spi_ConfigType, AUTOMATIC, SPI_CONFIG_DATA) CfgPtr);

/**
 *  \brief Service for SPI de-initialization.
 *
 *  \verbatim
 *  Service name        : Spi_DeInit
 *  Syntax              : Std_ReturnType Spi_DeInit( void )
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x01
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Non Reentrant
 *  Parameters (in)     : None
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : Std_ReturnType
 *                        E_OK: de-initialisation command has been accepted
 *                        E_NOT_OK: de-initialisation command has not been
 *                                  accepted
 *  Description         : Service for SPI de-initialization
 *  \endverbatim
 */
FUNC(Std_ReturnType, SPI_CODE) Spi_DeInit(void);

/**
 *  \brief Service returns the SPI Handler/Driver software module status.
 *
 *  \verbatim
 *  Service name        : Spi_GetStatus
 *  Syntax              : Spi_StatusType Spi_GetStatus( void )
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x06
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : None
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : Spi_StatusType - Spi_StatusType
 *  Description         : Service for SPI de-initialization
 *  \endverbatim
 */
FUNC(Spi_StatusType, SPI_CODE) Spi_GetStatus(void);

/**
 *  \brief This service returns the last transmission result of the specified
 *         Job.
 *
 *  \verbatim
 *  Service name        : Spi_GetJobResult
 *  Syntax              : Spi_JobResultType Spi_GetJobResult( Spi_JobType Job )
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x07
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Job - Job ID. An invalid job ID will return an
 *                        undefined result
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : Spi_JobResultType - Spi_JobResultType
 *  Description         : Service for SPI de-initialization
 *  \endverbatim
 */
FUNC(Spi_JobResultType, SPI_CODE) Spi_GetJobResult(Spi_JobType Job);

/**
 *  \brief This service returns the last transmission result of the specified
 *         Sequence.
 *
 *  \verbatim
 *  Service name        : Spi_GetSequenceResult
 *  Syntax              : Spi_SeqResultType Spi_GetSequenceResult(
 *                                                   Spi_SequenceType Sequence )
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x08
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Sequence - Sequence ID. An invalid sequence ID will
 *                        return an undefined result
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : Spi_SeqResultType - Spi_SeqResultType
 *  Description         : This service returns the last transmission result of
 *                        the specified Sequence
 *  \endverbatim
 */
FUNC(Spi_SeqResultType, SPI_CODE) Spi_GetSequenceResult(
    Spi_SequenceType Sequence);

#if (STD_ON == SPI_VERSION_INFO_API)
/**
 *  \brief This service returns the version information of this module.
 *
 *  \verbatim
 *  Service name        : Spi_GetVersionInfo
 *  Syntax              : void Spi_GetVersionInfo( Std_VersionInfoType*
 *                                                                 versioninfo )
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x09
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
FUNC(void, SPI_CODE) Spi_GetVersionInfo(
    P2VAR(Std_VersionInfoType, AUTOMATIC, SPI_APPL_DATA) versioninfo);
#endif  /* #if (STD_ON == SPI_VERSION_INFO_API) */

#if (STD_ON == SPI_HW_STATUS_API)
/**
 *  \brief This service returns the status of the specified SPI Hardware
 *         microcontroller peripheral.
 *
 *  \verbatim
 *  Service name        : Spi_GetHWUnitStatus
 *  Syntax              : Spi_StatusType Spi_GetHWUnitStatus( Spi_HWUnitType
 *                                                                      HWUnit )
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x0b
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : HWUnit - SPI Hardware microcontroller peripheral
 *                        unit ID.
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : Spi_StatusType - Spi_StatusType
 *  Description         : This service returns the status of the specified SPI
 *                        Hardware microcontroller peripheral
 *  \endverbatim
 */
FUNC(Spi_StatusType, SPI_CODE) Spi_GetHWUnitStatus(Spi_HWUnitType HWUnit);
#endif  /* #if (STD_ON == SPI_HW_STATUS_API) */

#if ((SPI_CHANNELBUFFERS == SPI_IB) || (SPI_CHANNELBUFFERS == SPI_IB_EB))
/**
 *  \brief Service for writing one or more data to an IB SPI Handler/Driver
 *         Channel specified by parameter.
 *
 *  \verbatim
 *  Service name        : Spi_WriteIB
 *  Syntax              : Std_ReturnType Spi_WriteIB(
 *                              Spi_ChannelType Channel,
 *                              const Spi_DataBufferType* DataBufferPtr )
 *
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x02
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Channel - Channel ID
 *                        DataBufferPtr - Pointer to source data buffer.
 *                        If this pointer is null, it is assumed that the data
 *                        to be transmitted is not relevant and the default
 *                        transmit value of this channel will be used instead.
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : Std_ReturnType
 *                        E_OK: Write command has been accepted
 *                        E_NOT_OK: Write command has not been accepted
 *  Description         : Service for writing one or more data to an IB SPI
 *                        Handler/Driver Channel specified by parameter
 *  \endverbatim
 */
FUNC(Std_ReturnType, SPI_CODE) Spi_WriteIB(
    Spi_ChannelType Channel,
    P2CONST(Spi_DataBufferType, AUTOMATIC, SPI_APPL_DATA) DataBufferPtr);

/**
 *  \brief Service for reading synchronously one or more data
 *         from an IB SPI Handler/Driver Channel specified by parameter
 *
 *  \verbatim
 *  Service name        : Spi_ReadIB
 *  Syntax              : Std_ReturnType Spi_ReadIB(
 *                              Spi_ChannelType Channel,
 *                              Spi_DataBufferType* DataBufferPointer )
 *
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x04
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Channel - Channel ID
 *  Parameters (inout)  : None
 *  Parameters (out)    : DataBufferPointer - Pointer to destination data buffer.
 *  Return value        : Std_ReturnType
 *                        E_OK: Read command has been accepted
 *                        E_NOT_OK: Read command has not been accepted
 *  Description         : Service for reading synchronously one or more data
 *                        from an IB SPI Handler/Driver Channel specified by
 *                        parameter
 *  \endverbatim
 */
FUNC(Std_ReturnType, SPI_CODE) Spi_ReadIB(
    Spi_ChannelType Channel,
    P2VAR(Spi_DataBufferType, AUTOMATIC, SPI_APPL_DATA) DataBufferPointer);
#endif  /* #if SPI_IB || SPI_IB_EB */

#if ((SPI_CHANNELBUFFERS == SPI_EB) || (SPI_CHANNELBUFFERS == SPI_IB_EB))
/**
 *  \brief Service to setup the buffers and the length of data for the EB SPI
 *         Handler/Driver Channel specified.
 *
 *  \verbatim
 *  Service name        : Spi_SetupEB
 *  Syntax              : Std_ReturnType Spi_SetupEB( Spi_ChannelType Channel,
 *                                  const Spi_DataBufferType* SrcDataBufferPtr,
 *                                  Spi_DataBufferType* DesDataBufferPtr,
 *                                   Spi_NumberOfDataType Length )
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x05
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Channel - Channel ID
 *                        SrcDataBufferPtr - Pointer to source data buffer
 *                        DesDataBufferPtr - Pointer to destination data buffer
 *                                           in RAM.
 *                        Length - Length (number of data elements) of the data
 *                        to be transmitted from SrcDataBufferPtr and/or
 *                        received
 *                        from DesDataBufferPtr Min.: 1 Max.: Max of data
 *                        specified at configuration for this channel
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : Std_ReturnType
 *                        E_OK: Setup command has been accepted
 *                        E_NOT_OK: Setup command has not been accepted
 *  Description         : Service to setup the buffers and the length of data
 *                        for the EB SPI Handler/Driver Channel specified
 *  \endverbatim
 */
FUNC(Std_ReturnType, SPI_CODE) Spi_SetupEB(
    Spi_ChannelType Channel,
    P2CONST(Spi_DataBufferType, AUTOMATIC, SPI_APPL_DATA) SrcDataBufferPtr,
    P2VAR(Spi_DataBufferType, AUTOMATIC, SPI_APPL_DATA) DesDataBufferPtr,
    Spi_NumberOfDataType Length);
#endif  /* #if ((SPI_CHANNELBUFFERS == SPI_EB) || (SPI_CHANNELBUFFERS ==
         *SPI_IB_EB)) */

#if ((SPI_SCALEABILITY == SPI_LEVEL_1) || (SPI_SCALEABILITY == \
                                                   SPI_LEVEL_2))
/**
 *  \brief Service to transmit data on the SPI bus.
 *
 *  \verbatim
 *  Service name        : Spi_AsyncTransmit
 *  Syntax              : Std_ReturnType Spi_AsyncTransmit( Spi_SequenceType
 *                                                              Sequence )
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x03
 *  Sync/Async          : Asynchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Sequence - Sequence ID
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : Std_ReturnType
 *                        E_OK: Transmission command has been accepted
 *                        E_NOT_OK: Transmission command has not been accepted
 *  Description         : Service to transmit data on the SPI bus
 *  \endverbatim
 */
FUNC(Std_ReturnType, SPI_CODE) Spi_AsyncTransmit(Spi_SequenceType Sequence);
#endif  /* #if ((SPI_SCALEABILITY == SPI_LEVEL_1) ||
         *(SPI_SCALEABILITY == SPI_LEVEL_2)) */

#if (STD_ON == SPI_CANCEL_API)
/**
 *  \brief Service cancels the specified on-going sequence transmission.
 *
 *  \verbatim
 *  Service name        : Spi_Cancel
 *  Syntax              : void Spi_Cancel( Spi_SequenceType Sequence )
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x0c
 *  Sync/Async          : Asynchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Sequence - Sequence ID
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : Service cancels the specified on-going sequence
 *                        transmission
 *  \endverbatim
 */
FUNC(void, SPI_CODE) Spi_Cancel(Spi_SequenceType Sequence);
#endif  /* #if (STD_ON == SPI_CANCEL_API) */

#if ((SPI_SCALEABILITY == SPI_LEVEL_0) || (SPI_SCALEABILITY == \
                                                   SPI_LEVEL_2))
/**
 *  \brief Service to transmit data on the SPI bus.
 *
 *  \verbatim
 *  Service name        : Spi_SyncTransmit
 *  Syntax              : Std_ReturnType Spi_SyncTransmit( Spi_SequenceType
 *                                                                  Sequence)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x0A
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Sequence - Sequence ID
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : Std_ReturnType
 *                        E_OK: Transmission command has been completed
 *                        E_NOT_OK: Transmission command has not been completed
 *  Description         : Service to transmit data on the SPI bus
 *  \endverbatim
 */
FUNC(Std_ReturnType, SPI_CODE) Spi_SyncTransmit(Spi_SequenceType Sequence);
#endif  /* #if ((SPI_SCALEABILITY == SPI_LEVEL_0) ||
         *(SPI_SCALEABILITY == SPI_LEVEL_2)) */

#if (SPI_SCALEABILITY == SPI_LEVEL_2)
/**
 *  \brief Service to set the asynchronous mechanism mode for SPI busses
 *  handled asynchronously.
 *
 *  \verbatim
 *  Service name        : Spi_SetAsyncMode
 *  Syntax              : Std_ReturnType Spi_SetAsyncMode( Spi_AsyncModeType
 *                                                                      Mode )
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x0D
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Non Reentrant
 *  Parameters (in)     : Mode - New mode required.
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : Std_ReturnType
 *                        E_OK: Setting command has been done
 *                        E_NOT_OK: Setting command has not been accepted
 *  Description         : Service to set the asynchronous mechanism mode for
 *                        SPI busses handled asynchronously.
 *  \endverbatim
 */
FUNC(Std_ReturnType, SPI_CODE) Spi_SetAsyncMode(Spi_AsyncModeType Mode);
#endif  /* #if (SPI_SCALEABILITY == SPI_LEVEL_2) */

/**
 *  \brief This function polls the SPI interrupts linked to HW Units allocated
 *  to the transmission of SPI sequences to enable the evolution of
 *  transmission state machine.
 *
 *  \verbatim
 *  Service name        : Spi_MainFunction_Handling
 *  Syntax              : void Spi_MainFunction_Handling( void )
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x10
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Non Reentrant
 *  Parameters (in)     : None
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : This function polls the SPI interrupts linked to
 *                        HW Units allocated to the transmission of SPI
 *                        sequences to enable the evolution of transmission
 *                        state machine.
 *  \endverbatim
 */
FUNC(void, SPI_CODE) Spi_MainFunction_Handling(void);

#if (STD_ON == SPI_REGISTER_READBACK_API)
/**
 *  \brief This function reads the important registers of the hardware unit
 *  and returns the value in the structure.
 *
 *  This API should be called after Spi_Init is called. Otherwise this API will
 *  return E_NOT_OK.
 *
 *  This API could be used to readback the register contents after Spi_Init
 *  and then the readback value could be compared during SPI execution to
 *  check the correctness of the HW unit. Since this API is used for this
 *  purpose, the register returned are the ones which doesn't change after init
 *  based on job or channel config.
 *
 *  \verbatim
 *  Service name        : Spi_RegisterReadback
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : HWUnit - SPI Hardware microcontroller peripheral
 *                        unit ID. If this is invalid, then the API will
 *                        return E_NOT_OK.
 *  Parameters (inout)  : RegRbPtr - Pointer to where to store the readback
 *                        values. If this pointer is NULL_PTR, then the API
 *                        will return E_NOT_OK.
 *  Return value        : Std_ReturnType
 *                        E_OK: Register read back has been done
 *                        E_NOT_OK: Register read back failed
 *  \endverbatim
 */
FUNC(Std_ReturnType, SPI_CODE) Spi_RegisterReadback(
    Spi_HWUnitType HWUnit,
    P2VAR(Spi_RegisterReadbackType, AUTOMATIC, SPI_APPL_DATA) RegRbPtr);
#endif  /* #if (STD_ON == SPI_REGISTER_READBACK_API) */

#if (STD_ON == SPI_SAFETY_API)
/**
 *  \brief This function Enable  Under/Overflow Interupts of the hardware unit
 *  and returns the status.
 *
 *  \verbatim
 *  Service name        : Spi_dataOverflowUnderflowIntrEnable
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : HWUnit - SPI Hardware microcontroller peripheral
 *                        unit ID. If this is invalid, then the API will
 *                        return E_NOT_OK.
 *                      : intFlags - Mask value of MCSPI_IRQENABLE register
 *  Return value        : Std_ReturnType
 *                        E_OK: Interupt Enabled
 *                        E_NOT_OK: Interupt Enable failed
 *  \endverbatim
 */
FUNC(Std_ReturnType, SPI_CODE) Spi_dataOverflowUnderflowIntrEnable(
    Spi_HWUnitType HWUnit, uint32 intFlags );

/**
 *  \brief This function Disable Under/Overflow Interupts of the hardware unit
 *  and returns the status.
 *
 *  \verbatim
 *  Service name        : Spi_dataOverflowUnderflowIntrDisable
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : HWUnit - SPI Hardware microcontroller peripheral
 *                        unit ID. If this is invalid, then the API will
 *                        return E_NOT_OK.
 *                      : intFlags - Mask value of MCSPI_IRQSTATUS register
 *  Return value        : Std_ReturnType
 *                        E_OK: Interupt Disabled
 *                        E_NOT_OK: Interupt Disable failed
 *
 *  \endverbatim
 */
FUNC(Std_ReturnType, SPI_CODE)  Spi_dataOverflowUnderflowIntrDisable(
    Spi_HWUnitType HWUnit, uint32 intFlags);

/**
 *  \brief This function status Under/Overflow Interupts of the hardware unit
 *  and returns the status.
 *
 *  \verbatim
 *  Service name        : Spi_dataOverflowUnderflowIntrGetStatus
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : HWUnit - SPI Hardware microcontroller peripheral
 *                        unit ID. If this is invalid, then the API will
 *                        return E_NOT_OK.
 *                        intFlags - Mask value of MCSPI_IRQSTATUS register
 *  Return value        : Mcspi_IrqStatusType
 *                        SPI_NO_EVENT: No underflow event
 *                        SPI_EVENT_PENDING: Underflow Event
 *                        SPI_STATUS_READ_FAIL: Status read fail
 *  \endverbatim
 */
FUNC(Mcspi_IrqStatusType, SPI_CODE) Spi_dataOverflowUnderflowIntrGetStatus(
    Spi_HWUnitType HWUnit, uint32 intFlags);

/**
 *  \brief This function status clear Under/Overflow Interupts of the hardware unit
 *  and returns the status.
 *
 *  \verbatim
 *  Service name        : Spi_dataOverflowUnderflowIntrStatusClear
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : HWUnit - SPI Hardware microcontroller peripheral
 *                        unit ID. If this is invalid, then the API will
 *                        return E_NOT_OK.
 *                        intFlags - Mask value of MCSPI_IRQSTATUS register
 *  Return value        : Std_ReturnType
 *                        E_OK: Interuptstatus cleares
 *                        E_NOT_OK: Interupt status clear failed
 *
 *  \endverbatim
 */
FUNC(Std_ReturnType, SPI_CODE)  Spi_dataOverflowUnderflowIntrStatusClear(
    Spi_HWUnitType HWUnit, uint32 intFlags);

#endif /* #if (STD_ON == SPI_SAFETY_API) */

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef SPI_H_ */

/* @} */
