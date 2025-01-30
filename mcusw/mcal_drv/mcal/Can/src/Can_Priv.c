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
 *  \file     Can_Priv.c
 *
 *  \brief    This file contains CAN MCAL driver implementation.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "Can.h"
#include "Can_Priv.h"
#if (CAN_DEV_ERROR_DETECT == STD_ON)
#include "Det.h"
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
/* Memory mapping, done via MemMap.h: Code */
#define CAN_START_SEC_CODE
#include "Can_MemMap.h"

/*******************************************************************************
 * Can_hwUnitStart
 ******************************************************************************/
/*! \brief      This function will start the CAN node.
 *
 ******************************************************************************/
/*
 * Design: MCAL-6152
 */
boolean Can_hwUnitStart(
    Can_ControllerObjType *controllerObj)
{
    boolean status = (boolean) FALSE;

#if (CAN_DEV_ERROR_DETECT == STD_ON)
    if (controllerObj->canState != CAN_CS_STOPPED)
    {
        status = (boolean) FALSE;
        (void) Det_ReportError((uint16) CAN_MODULE_ID,
                               (uint8) CAN_INSTANCE_ID,
                               (uint8) CAN_SETCTR_ID,
                               (uint8) CAN_E_TRANSITION);
    }
    else
#endif
	{
		status = Can_mcanStart(controllerObj);
	}

	return status;
}

/*******************************************************************************
 * Can_hwUnitStop
 ******************************************************************************/
/*! \brief      This function will stop the CAN node.
 *
 ******************************************************************************/
/*
 * Design: MCAL-6073,MCAL-6050,MCAL-5978
 */
boolean Can_hwUnitStop(
    Can_ControllerObjType * controllerObj)
{
    boolean status = (boolean) FALSE;

    if (controllerObj->canState == CAN_CS_STOPPED)
    {
        CanIf_ControllerModeIndication(
                            controllerObj->canControllerConfig_PC.ControllerId,
                            CAN_CS_STOPPED);
        status = (boolean) TRUE;
    }
    else
    {
#if (CAN_DEV_ERROR_DETECT == STD_ON)

        /* CAN_CoverageGap_40: Dynamic coverage for the below 'if' statement is not covered.
         * The 'if' statement is added as an additional safety
         * mechanism to check wrong transition.
         */

        if ((controllerObj->canState != CAN_CS_STARTED) &&
                (controllerObj->canState != CAN_CS_SLEEP))
        {
            status = (boolean) FALSE;
            (void) Det_ReportError((uint16) CAN_MODULE_ID,
                                   (uint8) CAN_INSTANCE_ID,
                                   (uint8) CAN_SETCTR_ID,
                                   (uint8) CAN_E_TRANSITION);
        }
        else
#endif
        {
            status = Can_mcanStop(controllerObj);
        }
    }
    return status;
}

/*******************************************************************************
 * Can_hwUnitSleep
 ******************************************************************************/
/*! \brief      This function will sleep the CAN node.
 *
 ******************************************************************************/
/*
 * Design: MCAL-6087,MCAL-5982,MCAL-5940
 */
boolean Can_hwUnitSleep(
    Can_ControllerObjType *controllerObj)
{
    boolean status = (boolean) FALSE;

    if (controllerObj->canState == CAN_CS_SLEEP)
    {
        CanIf_ControllerModeIndication(
                            controllerObj->canControllerConfig_PC.ControllerId,
                            CAN_CS_SLEEP);
        status = (boolean) TRUE;
    }
    else
    {
#if (CAN_DEV_ERROR_DETECT == STD_ON)
        if (controllerObj->canState != CAN_CS_STOPPED)
        {
            status = (boolean) FALSE;
            (void) Det_ReportError((uint16) CAN_MODULE_ID,
                                   (uint8) CAN_INSTANCE_ID,
                                   (uint8) CAN_SETCTR_ID,
                                   (uint8) CAN_E_TRANSITION);
        }
        else
#endif
        {
            status = Can_mcanSleep(controllerObj);
        }
    }

    return status;
}

void Can_hwUnitDisableInterrupts(
    Can_ControllerObjType *controllerObj)
{
    Can_mcanDisableInterrupts(controllerObj);
}

void Can_hwUnitEnableInterrupts(
    Can_ControllerObjType *controllerObj)
{
    Can_mcanEnableInterrupts(controllerObj);
}

void Can_hwUnitSetBaudrate(uint32 baseAddr,
                           const Can_BaudConfigType *setBaud)
{
    Can_mcanSetBaudrate(baseAddr, setBaud);
}

/* CAN_CoverageGap_39: Dynamic coverage for the below function Can_MainFunction_BusOffProcess
 * is not covered. Since BusOff is not tested.
 */

void Can_MainFunction_BusOffProcess(
    Can_ControllerObjType *canController)
{
    Can_mcan_BusOffProcess(canController);
}

void Can_MainFunction_ModeProcess(
    Can_ControllerObjType *canController)
{
    Can_mcan_ModeProcess(canController);
}

#if (CAN_RX_POLLING == STD_ON)
void Can_MainFunction_ReadProcess(
    Can_DriverObjType *can_DriverObj)
{
    uint32 loopCnt;
    Can_ControllerObjType *controllerObj;
	Can_TxRxProcessingType rxProcessingType;

    for(loopCnt = 0U ;
        loopCnt < CAN_NUM_CONTROLLER;
        loopCnt++)
    {
        rxProcessingType = can_DriverObj->canController[loopCnt].
                        canControllerConfig_PC.RxProcessingType;
		/* only for the Tx Mailbox */
        if (rxProcessingType != CAN_TX_RX_PROCESSING_INTERRUPT)
        {
			if (can_DriverObj->canController[loopCnt].
						canControllerConfig_PC.CntrActive == (boolean) TRUE)
			{
				controllerObj = &can_DriverObj->canController[loopCnt];
				Can_mcan_ReadProcess(controllerObj,
						 (Can_MailboxObjType *)&can_DriverObj->canMailbox[0U],
						  can_DriverObj->maxMbCnt);
			}
		}
    }
}
#endif

Std_ReturnType Can_hwUnitTestLoopBackModeEnable(
    uint32 baseAddr, uint8 mode)
{
    VAR(Std_ReturnType, AUTOMATIC) retVal = (Std_ReturnType) E_NOT_OK;

    retVal = Can_mcanTestLoopBackModeEnable(baseAddr, mode);

    return retVal;
}

Std_ReturnType Can_hwUnitTestLoopBackModeDisable(
    uint32 baseAddr, uint8 mode)
{
    VAR(Std_ReturnType, AUTOMATIC) retVal = (Std_ReturnType) E_NOT_OK;

    retVal = Can_mcanTestLoopBackModeDisable(baseAddr, mode);

    return retVal;
}

void Can_hwDeInit(
    const Can_ControllerObjType *canController)
{
    Can_mcanHwDeInit(canController);
}

/* Memory mapping, done via MemMap.h: Code */
#define CAN_STOP_SEC_CODE
#include "Can_MemMap.h"
/*******************************************************************************
 *  End of File: Can_priv.c
 ******************************************************************************/
