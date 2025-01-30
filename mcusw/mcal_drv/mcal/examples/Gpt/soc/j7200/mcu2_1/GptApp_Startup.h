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
#ifndef GPTAPP_STARTUP_H_
#define GPTAPP_STARTUP_H_

#include "Std_Types.h"
#include "string.h"
#include "Det.h"
#include "Dem.h"
#include "Gpt_Cfg.h"
#include "Gpt.h"
#include "Gpt_Irq.h"
#include "SchM_Gpt.h"

#include <hw_include/soc.h>
#include <hw_include/hw_types.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/** \brief Interrupt ID's for J7200 SoC
 *
 */
#define APP_GPT_TOTAL_TIMER_COUNT (30U)

#define APP_GPT_MCU_0_INT   (CSLR_R5FSS0_CORE1_INTR_MCU_TIMER0_INTR_PEND_0)
#define APP_GPT_MCU_1_INT   (CSLR_R5FSS0_CORE1_INTR_MCU_TIMER1_INTR_PEND_0)
#define APP_GPT_MCU_2_INT   (CSLR_R5FSS0_CORE1_INTR_MCU_TIMER2_INTR_PEND_0)
#define APP_GPT_MCU_3_INT   (CSLR_R5FSS0_CORE1_INTR_MCU_TIMER3_INTR_PEND_0)
#define APP_GPT_MCU_4_INT   (CSLR_R5FSS0_CORE1_INTR_MCU_TIMER4_INTR_PEND_0)
#define APP_GPT_MCU_5_INT   (CSLR_R5FSS0_CORE1_INTR_MCU_TIMER5_INTR_PEND_0)
#define APP_GPT_MCU_6_INT   (CSLR_R5FSS0_CORE1_INTR_MCU_TIMER6_INTR_PEND_0)
#define APP_GPT_MCU_7_INT   (CSLR_R5FSS0_CORE1_INTR_MCU_TIMER7_INTR_PEND_0)
#define APP_GPT_MCU_8_INT   (CSLR_R5FSS0_CORE1_INTR_MCU_TIMER8_INTR_PEND_0)
#define APP_GPT_MCU_9_INT   (CSLR_R5FSS0_CORE1_INTR_MCU_TIMER9_INTR_PEND_0)
#define APP_GPT_0_INT       (CSLR_R5FSS0_CORE1_INTR_TIMER0_INTR_PEND_0)
#define APP_GPT_1_INT       (CSLR_R5FSS0_CORE1_INTR_TIMER1_INTR_PEND_0)
#define APP_GPT_2_INT       (CSLR_R5FSS0_CORE1_INTR_TIMER2_INTR_PEND_0)
#define APP_GPT_3_INT       (CSLR_R5FSS0_CORE1_INTR_TIMER3_INTR_PEND_0)
#define APP_GPT_4_INT       (CSLR_R5FSS0_CORE1_INTR_TIMER4_INTR_PEND_0)
#define APP_GPT_5_INT       (CSLR_R5FSS0_CORE1_INTR_TIMER5_INTR_PEND_0)
#define APP_GPT_6_INT       (CSLR_R5FSS0_CORE1_INTR_TIMER6_INTR_PEND_0)
#define APP_GPT_7_INT       (CSLR_R5FSS0_CORE1_INTR_TIMER7_INTR_PEND_0)
#define APP_GPT_8_INT       (CSLR_R5FSS0_CORE1_INTR_TIMER8_INTR_PEND_0)
#define APP_GPT_9_INT       (CSLR_R5FSS0_CORE1_INTR_TIMER9_INTR_PEND_0)
#define APP_GPT_10_INT      (CSLR_R5FSS0_CORE1_INTR_TIMER10_INTR_PEND_0)
#define APP_GPT_11_INT      (CSLR_R5FSS0_CORE1_INTR_TIMER11_INTR_PEND_0)
#define APP_GPT_12_INT      (CSLR_R5FSS0_CORE1_INTR_TIMER12_INTR_PEND_0)
#define APP_GPT_13_INT      (CSLR_R5FSS0_CORE1_INTR_TIMER13_INTR_PEND_0)
#define APP_GPT_14_INT      (CSLR_R5FSS0_CORE1_INTR_TIMER14_INTR_PEND_0)
#define APP_GPT_15_INT      (CSLR_R5FSS0_CORE1_INTR_TIMER15_INTR_PEND_0)
#define APP_GPT_16_INT      (CSLR_R5FSS0_CORE1_INTR_TIMER16_INTR_PEND_0)
#define APP_GPT_17_INT      (CSLR_R5FSS0_CORE1_INTR_TIMER17_INTR_PEND_0)
#define APP_GPT_18_INT      (CSLR_R5FSS0_CORE1_INTR_TIMER18_INTR_PEND_0)
#define APP_GPT_19_INT      (CSLR_R5FSS0_CORE1_INTR_TIMER19_INTR_PEND_0)

#endif /* GPTAPP_STARTUP_H_ */
