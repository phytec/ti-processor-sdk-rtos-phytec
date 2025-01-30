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

#ifdef __cplusplus
extern "C"{
#endif

/* ========================================================================== */
/*                           Include Files                                    */
/* ========================================================================== */
#include "EcuM_Types.h"
#include "EcuM.h"

#include "app_utils.h"

#define UART_BASE_ADDR  (0U)
/**< Base address of uart instance that is used to print */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
#define ECUM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "EcuM_MemMap.h"
    volatile VAR(EcuM_WakeupSourceType, ECUM_VAR_INIT) EcuM_WakeupSource;
    /**< Wakeup source */
    volatile uint8 GptApp_WakeupSrcDetFlag = (uint8) FALSE;
    /**< Flag used for Wakeup status, used by Gpt Demo application */
#define ECUM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "EcuM_MemMap.h"

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
#define ECUM_START_SEC_CODE
#include "EcuM_MemMap.h"
/* ========================================================================== */
/*              Service Name : EcuM_CheckWakeup                               */
/* ========================================================================== */
FUNC(void, ECUM_CODE) EcuM_CheckWakeup(
                        VAR(EcuM_WakeupSourceType, AUTOMATIC) wakeupSource)
{
    EcuM_WakeupSource = wakeupSource;
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL, " EcuM "
        ": Wakeup event received for wakeupSource =%d !!!\n", wakeupSource);

    GptApp_WakeupSrcDetFlag = (uint8) TRUE;

}

/* ========================================================================== */
/*              Service Name : EcuM_SetWakeupEvent                            */
/* ========================================================================== */
FUNC(void, ECUM_CODE) EcuM_SetWakeupEvent(
                            VAR(EcuM_WakeupSourceType, AUTOMATIC) sources)
{
    EcuM_WakeupSource = sources;
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL, " EcuM "
        ": Wakeup event set for wakeupSource =%d !!!\n", sources);
}

/* ========================================================================== */
/*              Service Name : EcuM_EndCheckWakeup                            */
/* ========================================================================== */
FUNC(void, ECUM_CODE) EcuM_EndCheckWakeup(
                                VAR(EcuM_WakeupSourceType, AUTOMATIC) sources)
{
    EcuM_WakeupSource = sources;
}

#define ECUM_STOP_SEC_CODE
#include "EcuM_MemMap.h"

#ifdef __cplusplus
}
#endif

