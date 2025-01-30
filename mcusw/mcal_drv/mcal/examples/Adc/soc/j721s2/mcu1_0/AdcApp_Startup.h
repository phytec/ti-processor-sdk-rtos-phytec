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

/* Protection against multiple inclusion */
#ifndef ADCAPP_STARTUP_H_
#define ADCAPP_STARTUP_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "Std_Types.h"
#include "string.h"
#include "Det.h"
#include "Dem.h"
#include "Adc_Cfg.h"
#include "Adc.h"
#include "Adc_Dbg.h"
#include "SchM_Adc.h"
#include <hw_include/arch/lld_arch.h>
#include <hw_include/soc.h>
#include <hw_include/lldr.h>
#include <hw_include/arch/c66x/interrupt.h>
#include "Os.h"
/* SCI Client */
#include <ti/drv/sciclient/sciclient.h>
/* Interrupt Registrations */
#include <ti/osal/osal.h>
#include "app_utils.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define MSG_NORMAL              (APP_UTILS_PRINT_MSG_NORMAL)
/**< Message type */
#define MSG_STATUS              (APP_UTILS_PRINT_MSG_STATUS)
/**< Message type */
/* This instance name corresponds to design instance name: mcu1_0 */
#define APP_ADC_0_INT0         (CSLR_MCU_R5FSS0_CORE0_INTR_MCU_ADC12FCC0_GEN_LEVEL_0)
#define APP_ADC_1_INT0         (CSLR_MCU_R5FSS0_CORE0_INTR_MCU_ADC12FCC1_GEN_LEVEL_0)

void AdcApp_AdcXIsr(uintptr_t AdcPtr);
typedef void (*AdcApp_IsrType)(void);
void AdcApp_Startup(void);
void AdcApp_PlatformInit(void);
void AdcApp_PowerAndClkSrc(void);

#endif /* ADCAPP_STARTUP_H_ */
