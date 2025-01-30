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
#include "Det.h"
#include "app_utils.h"

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
extern Std_ReturnType utils_Det_ReportError(uint16 ModuleId,
                               uint8  InstanceId,
                               uint8  ApiId,
                               uint8  ErrorId);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
#define DET_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Det_MemMap.h"
volatile VAR(uint16, DET_VAR_CLEARED) Det_ModuleId;
volatile VAR(uint8, DET_VAR_CLEARED) Det_InstanceId;
volatile VAR(uint8, DET_VAR_CLEARED) Det_ApiId;
volatile VAR(uint8, DET_VAR_CLEARED) Det_ErrorId;
volatile VAR(uint16, DET_VAR_CLEARED) Det_ModuleIdRunErr;
volatile VAR(uint8, DET_VAR_CLEARED) Det_InstanceIdRunErr;
volatile VAR(uint8, DET_VAR_CLEARED) Det_ApiIdRunErr;
volatile VAR(uint8, DET_VAR_CLEARED) Det_ErrorIdRunErr;
volatile VAR(uint16, DET_VAR_CLEARED) Det_ModuleIdTransFlt;
volatile VAR(uint8, DET_VAR_CLEARED) Det_InstanceIdTransFlt;
volatile VAR(uint8, DET_VAR_CLEARED) Det_ApiIdTransFlt;
volatile VAR(uint8, DET_VAR_CLEARED) Det_FaultIdTransFlt;
#define DET_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Det_MemMap.h"

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
#define DET_START_SEC_CODE
#include "Det_MemMap.h"

/* ========================================================================== */
/*              Service Name : Det_ReportError                                */
/* ========================================================================== */
FUNC(Std_ReturnType, DET_CODE) Det_ReportError(VAR(uint16, AUTOMATIC) ModuleId,
                                               VAR(uint8, AUTOMATIC) InstanceId,
                                               VAR(uint8, AUTOMATIC) ApiId,
                                               VAR(uint8, AUTOMATIC) ErrorId)
{
    Det_ModuleId = ModuleId;
    Det_InstanceId = InstanceId;
    Det_ApiId = ApiId;
    Det_ErrorId = ErrorId;
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL, 
                "DET Error reported. Module ID:%d  Api ID:%d  Error ID:%d \n",
                ModuleId, ApiId, ErrorId);
    return (E_OK);
}

/* ========================================================================== */
/*              Service Name : Det_ReportRuntimeError                         */
/* ========================================================================== */
FUNC(Std_ReturnType, DET_CODE) Det_ReportRuntimeError(
                                               VAR(uint16, AUTOMATIC) ModuleId,
                                               VAR(uint8, AUTOMATIC) InstanceId,
                                               VAR(uint8, AUTOMATIC) ApiId,
                                               VAR(uint8, AUTOMATIC) ErrorId)
{
    Det_ModuleIdRunErr = ModuleId;
    Det_InstanceIdRunErr = InstanceId;
    Det_ApiIdRunErr = ApiId;
    Det_ErrorIdRunErr = ErrorId;
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL, 
                    "Det Error reported\n"
                    "   Module ID       %d\n"
                    "   Instance ID     %d\n"
                    "   SID             %d\n"
                    "   Error ID        %d\n",
                    ModuleId, InstanceId, ApiId, ErrorId);
    return (E_OK);
}

/* ========================================================================== */
/*              Service Name : Det_ReportTransientFault                       */
/* ========================================================================== */
FUNC(Std_ReturnType, DET_CODE) Det_ReportTransientFault(
                                               VAR(uint16, AUTOMATIC) ModuleId,
                                               VAR(uint8, AUTOMATIC) InstanceId,
                                               VAR(uint8, AUTOMATIC) ApiId,
                                               VAR(uint8, AUTOMATIC) FaultId)
{
    Det_ModuleIdTransFlt = ModuleId;
    Det_InstanceIdTransFlt = InstanceId;
    Det_ApiIdTransFlt = ApiId;
    Det_FaultIdTransFlt = FaultId;

    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL, 
                    "Det Transient Error reported\n"
                    "   Module ID       %d\n"
                    "   Instance ID     %d\n"
                    "   SID             %d\n"
                    "   Error ID        %d\n",
                    ModuleId, InstanceId, ApiId, FaultId);
    return (E_OK);
}

#define DET_STOP_SEC_CODE
#include "Det_MemMap.h"

#ifdef __cplusplus
}
#endif

