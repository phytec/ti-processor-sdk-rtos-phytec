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
 *  \file     can_profile.h
 *
 *  \brief    Common header file for this application
 *
 */
/**
 * \defgroup MCUSS_APP_CAN_PROFILE Can profiling application
 *
 *  This application perform CAN transmission on all the enabled instances.
 *  By default the CAN peripheral is configured to operate in loop-back mode,
 *  i.e. transmit and receive.
 *
 *  TI RTOS is used and CAN tx/rx is performed in a task, The CAN IF stub is
 *  is used to indicate completion of transmission / reception by posting a
 *  semaphore.
 * @{
 */
#ifndef CAN_PROFILE_H_
#define CAN_PROFILE_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "string.h"
#include "Std_Types.h"
#include "Det.h"
#include "Dem.h"
#include "Os.h"
#include "Can.h"
#include "CanIf_Cbk.h"
#include "EcuM_Cbk.h"
#include "Dio.h"

#include "CanApp_Startup.h"
#include "app_utils.h" /* MCAL Example utilities */
#include "utils_prf.h" /* Demo utilities */
#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                       Application Configurations                           */
/* ========================================================================== */
#define APP_NUM_MSG_PER_ITERATION           (100U)
/**< Number of CAN messages that would be sent per iteration */
#define APP_NUM_ITERATION                   (5U)
/**< Number of iteration, total can messages sent would be
    APP_NUM_MSG_PER_ITERATION * APP_NUM_ITERATION */
#define APP_INSTANCE_1_INST_IN_CFG_ONLY     (STD_OFF)
/**< Enables application to use 1st instance configured, useful when profiling
        has to be done for 1 instance only
        STD_OFF Configures to use all instances specified in the configuration
        STD_ON Configures to use only the first instance of MCAN */
#define CAN_TX_ONLY_MODE                    (STD_ON)
/**< Enables application to run in Transmit only mode, if set to STD_OFF then
     application works in Recieve only mode */
#define CAN_INITIAL_PRINT_DISABLE_BEFORE_CAN_RESPONSE (STD_OFF)
/**< Disables prints before sending/receiving of CAN messages, this is done so
     that the time for printing doesn't get accounted for in the CAN resonse
     calculation */

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define MSG_NORMAL              (APP_UTILS_PRINT_MSG_NORMAL)
/**< Message type */
#define MSG_STATUS              (APP_UTILS_PRINT_MSG_STATUS)
/**< Message type */
#define MSG_APP_NAME            "CAN Profile XIP App:"

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

typedef struct canAppTaskObj
{
    uint32 numTxPackets;
    /**< Number of CAN packets to be transmitted */
    Can_PduType *pTxPkts;
    /**< Pointer to packets to be transmitted */
    uint32 numRxPackets;
    /**< Number of CAN packets to be received */
    Can_PduType *pRxPkts;
    /**< Pointer to hold received packets */
    uint32 numIterations;
    /**< Number of iterations */
}canAppTaskObj_t;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

sint32 CanApp_ProfileTest(canAppTaskObj_t *pTskObj);
void CanApp_CanXIsr(uintptr_t CanPtr);

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef CAN_PROFILE_H_ */
/* @} */
