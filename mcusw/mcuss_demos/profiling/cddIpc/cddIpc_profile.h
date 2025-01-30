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
 *  \file     cddIpc_profile.h
 *
 *  \brief    Common header file for this application
 *
 */
/**
 * \defgroup MCUSS_APP_CDD_IPC_PROFILE Can profiling application
 *
 *
 * @{
 */
#ifndef CDD_IPC_PROFILE_H_
#define CDD_IPC_PROFILE_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "string.h"
#include "Std_Types.h"
#include "Det.h"
#include "Dem.h"
#include "Os.h"

#include "Cdd_Ipc.h"
#include "Cdd_IpcIrq.h"

#include "Cdd_IpcCfg.h"

#include "app_utils.h" /* MCAL Example utilities */
#include "utils_prf.h" /* Demo utilities */
#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                       Application Configurations                           */
/* ========================================================================== */
#define APP_NUM_MSG_PER_ITERATION           (100000U)
/**< Number of IPC messages that would be sent per iteration */
#define APP_NUM_ITERATION                   (3U)
/**< Number of different message sizes that would be sent */
#define APP_NUM_ITERATION_0_SIZE            (4U)
/**< Size of TX buffer for iteration 0 */
#define APP_NUM_ITERATION_1_SIZE            (8U)
/**< Size of TX buffer for iteration 1, classic can msg size */
#define APP_NUM_ITERATION_2_SIZE            (64U)
/**< Size of TX buffer for iteration 2, can fd msg size */

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define MSG_NORMAL              (APP_UTILS_PRINT_MSG_NORMAL)
/**< Message type */
#define MSG_STATUS              (APP_UTILS_PRINT_MSG_STATUS)
/**< Message type */
#define MSG_APP_NAME            "IPC Profile App: "

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
/** \brief Test time taken for message transmission & reception */
uint32 Cdd_IpcProfileTest(void);
/** \brief Setsup the Platforms specific dependencies */
void Cdd_IpcProfile_Startup(void);

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef CDD_IPC_PROFILE_H_ */
/* @} */
