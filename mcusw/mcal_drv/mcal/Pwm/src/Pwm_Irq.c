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
 *  \file     Pwm_Irq.c
 *
 *  \brief    This file contains the ISR implementation of PWM MCAL driver
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "Pwm_Priv.h"
#include "Pwm_Irq.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

#define PWM_START_SEC_ISR_CODE
#include "Pwm_MemMap.h"

#if (STD_ON == PWM_NOTIFICATION_SUPPORTED)

#ifdef PWM_USE_GPT

FUNC(void, PWM_CODE_FAST) Pwm_IntISR_Fun(Pwm_ChannelType pwmChannel)
{
    if (PWM_STATUS_INIT != Pwm_DrvStatus)
    {
        /* Should not come here. Only possible case is MCU registers ISR and
         * stray/old interrupts are pending for channel */
        Pwm_IntrClear(pwmChannel);
        Pwm_IntDisableAll(pwmChannel);
    }
    else
    {
        Pwm_ChObjType *chObj = &Pwm_ChObj[pwmChannel];
        if (PWM_STATUS_INIT == chObj->channelState)
        {
            Pwm_ChannelNotificationISR(pwmChannel);
        }
    }
}
#endif /*PWM_USE_GPT*/

#ifdef PWM_USE_EPWM

FUNC(void, PWM_CODE_FAST) Pwm_IntISR_epwm_Fun(Pwm_ChannelType pwmChannel)
{
#if (STD_ON == PWM_DEV_ERROR_DETECT)
    /*TI_INSPECTED 91 D : MISRAC_2012_R.17.7
     * "Reason - Tool Issue, baseAddr is used in this function only " */
    uint32 baseAddr = Pwm_GetBaseAddr(pwmChannel);
    if (PWM_STATUS_INIT != Pwm_DrvStatus)
    {
        /* Disable and Clear Interrupt */
        Pwm_EtIntrDisable_epwm(baseAddr);
        Pwm_EtIntrClear_epwm(baseAddr);
    }
    else
#endif
    {
        Pwm_ChannelNotificationISR_epwm(pwmChannel);
    }
}

#endif /*PWM_USE_EPWM*/

#endif /* PWM_NOTIFICATION_SUPPORTED */

#define PWM_STOP_SEC_ISR_CODE
#include "Pwm_MemMap.h"
