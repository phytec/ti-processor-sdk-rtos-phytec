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
#include "CanIf_Cbk.h"

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
#define CANIF_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "CanIf_MemMap.h"

/* Global variables used in callback functions */
volatile VAR(uint8, CANIF_VAR_CLEARED) CanIf_ControllerId;
volatile VAR(PduIdType, CANIF_VAR_CLEARED) CanIf_CanTxPduId;
#if defined (AUTOSAR_421)
volatile VAR(CanIf_ControllerModeType, CANIF_VAR_CLEARED) CanIf_ControllerMode;
#else
volatile VAR(Can_ControllerStateType, CANIF_VAR_CLEARED) CanIf_ControllerMode;
#endif
volatile P2VAR(Can_HwType, CANIF_VAR_CLEARED, CANIF_APPL_DATA) CanIf_Mailbox;
volatile VAR(Can_HwType, CANIF_VAR_CLEARED) CanIf_MailBuf;
volatile P2CONST(PduInfoType, CANIF_VAR_CLEARED, CANIF_APPL_DATA) 
															CanIf_PduInfoPtr;
volatile VAR(uint8, CANIF_VAR_CLEARED) CanIf_RxConfirmationCount;
volatile VAR(uint8, CANIF_VAR_CLEARED) CanIf_TxConfirmationCount;
volatile VAR(PduIdType, CANIF_VAR_CLEARED) CanIf_GblTxPduIdStore[32];
volatile VAR(uint8, CANIF_VAR_CLEARED) CanIf_TxCounter;

#define CANIF_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "CanIf_MemMap.h"
/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
#define CANIF_START_SEC_CODE
#include "CanIf_MemMap.h"

void CanIf_TxConfirmation(PduIdType CanTxPduId)
{
    CanIf_TxConfirmationCount++;
	CanIf_CanTxPduId = CanTxPduId;
	CanIf_GblTxPduIdStore[CanIf_TxCounter++] = CanIf_CanTxPduId;
}

void CanIf_RxIndication(const Can_HwType* Mailbox, 
    const PduInfoType* PduInfoPtr)
{
    CanIf_RxConfirmationCount++;
	CanIf_Mailbox = &CanIf_MailBuf;
    CanIf_Mailbox->CanId = Mailbox->CanId;
    CanIf_Mailbox->Hoh = Mailbox->Hoh;
    CanIf_Mailbox->ControllerId = Mailbox->ControllerId;
    CanIf_PduInfoPtr = PduInfoPtr;
}

void CanIf_ControllerBusOff(uint8 ControllerId)
{
    CanIf_ControllerId = ControllerId;
}

#if defined (AUTOSAR_421)
void CanIf_ControllerModeIndication(uint8 ControllerId,
                                    CanIf_ControllerModeType ControllerMode)
{
    CanIf_ControllerId = ControllerId;
}
#else
void CanIf_ControllerModeIndication(uint8 ControllerId,
                                    Can_ControllerStateType ControllerMode)
{
    CanIf_ControllerId = ControllerId;
}
#endif
#define CANIF_STOP_SEC_CODE
#include "CanIf_MemMap.h"

#ifdef __cplusplus
}
#endif

