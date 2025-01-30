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
 *  \file     Spi_Dbg.h
 *
 *  \brief    This file contains debug variable declaration for SPI MCAL driver
 *
 */

/**
 *  \ingroup MCAL_SPIHANDLER_API
 *  \defgroup MCAL_SPI_DBG_API Defines variables that could be used to driver
 *                              status, job status, etc...
 *  @{
 */

#ifndef SPI_DBG_H_
#define SPI_DBG_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Spi.h"

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

#if (STD_ON == SPI_JOB_LOG)
/**
 *  \brief SPI log structure per job entry
 */
typedef struct
{
    uint32                  timeStamp;
    /**< Timestamp counter at which the job is scheduled */
    Spi_JobType             jobId;
    /**< Job ID of the scheduled job */
    Spi_SequenceType        seqId;
    /**< Sequence ID to which the job belongs */
    Spi_HWUnitType          hwUnitId;
    /**< HWUnit associated with the job */
} Spi_JobLogEntryType;

/**
 *  \brief SPI job log structure
 */
typedef struct
{
    uint32                  logIndex;
    /**< Current log index. This -1 points to the last log entry */
    uint32                  totalLog;
    /**< Total log including wrapper around */
    Spi_JobLogEntryType     logEntry[SPI_MAX_JOB_LOG];
    /**< Log entries */
} Spi_JobLogType;
#endif  /* #if (STD_ON == SPI_JOB_LOG) */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

#if (STD_ON == SPI_JOB_LOG)
/** \brief SPI job log object */
extern Spi_JobLogType Spi_JobLogObj;
#endif  /* #if (STD_ON == SPI_JOB_LOG) */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef SPI_DBG_H_ */

/* @} */
