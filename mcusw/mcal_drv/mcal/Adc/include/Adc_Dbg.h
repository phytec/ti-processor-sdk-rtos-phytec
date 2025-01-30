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
 *  \file     Adc_Dbg.h
 *
 *  \brief    This file contains debug variable declaration for ADC MCAL driver
 *
 */

/**
 *  \ingroup MCAL_ADC_API
 *  @{
 */

#ifndef ADC_DBG_H_
#define ADC_DBG_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Adc.h"

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

#if (STD_ON == ADC_GROUP_LOG)
/**
 *  \brief ADC log structure per group entry
 */
typedef struct
{
    uint32                  timeStamp;
    /**< Timestamp based on 32-kHz Synchronized Timer (COUNTER_32K) at which
     *   the group is scheduled */
    Adc_GroupType           groupId;
    /**< Group ID of the scheduled group */
    Adc_HWUnitType          hwUnitId;
    /**< HWUnit associated with the group */
    uint32                  isReScheduled;
    /**< This will be set to TRUE is a group is rescheduled when a higher
     *   priority task pre-empts this group */
} Adc_GroupLogEntryType;

/**
 *  \brief ADC group log structure
 */
typedef struct
{
    uint32                  logIndex;
    /**< Current log index. This -1 points to the last log entry */
    uint32                  totalLog;
    /**< Total log including wrapper around */
    Adc_GroupLogEntryType   logEntry[ADC_MAX_GROUP_LOG];
    /**< Log entries */
} Adc_GroupLogType;
#endif  /* #if (STD_ON == ADC_GROUP_LOG) */

#if (STD_ON == ADC_FIFO_ERR_LOG)
/**
 *  \brief ADC log structure per FIFO error entry
 */
typedef struct
{
    uint32                  timeStamp;
    /**< Timestamp based on 32-kHz Synchronized Timer (COUNTER_32K) at which
     *   the FIFO error occurred */
    Adc_GroupType           groupId;
    /**< Group ID which caused FIFO error */
    Adc_HWUnitType          hwUnitId;
    /**< HWUnit associated with the group */
} Adc_FifoErrLogEntryType;

/**
 *  \brief ADC FIFO error log structure
 */
typedef struct
{
    uint32                  logIndex;
    /**< Current log index. This -1 points to the last log entry */
    uint32                  totalLog;
    /**< Total log including wrapper around */
    Adc_FifoErrLogEntryType logEntry[ADC_MAX_FIFO_ERR_LOG];
    /**< Log entries */
} Adc_FifoErrLogType;
#endif  /* #if (STD_ON == ADC_FIFO_ERR_LOG) */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

#if (STD_ON == ADC_GROUP_LOG)
/** \brief ADC group log object */
extern Adc_GroupLogType Adc_GroupLogObj;
#endif  /* #if (STD_ON == ADC_GROUP_LOG) */

#if (STD_ON == ADC_FIFO_ERR_LOG)
/** \brief ADC FIFO error log object */
extern Adc_FifoErrLogType Adc_FifoErrLogObj;
#endif  /* #if (STD_ON == ADC_FIFO_ERR_LOG) */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef ADC_DBG_H_ */

/* @} */
