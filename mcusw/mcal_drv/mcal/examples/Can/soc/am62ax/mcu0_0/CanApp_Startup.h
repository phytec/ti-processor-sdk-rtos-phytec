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
#ifndef CANAPP_STARTUP_H_
#define CANAPP_STARTUP_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "Std_Types.h"
#include "Det.h"
#include "Dem.h"
#include "Os.h"
#include "Can.h"
#include "CanIf_Cbk.h"
#include "EcuM_Cbk.h"

#include "app_utils.h"

#include <hw_include/soc.h>
#include <hw_include/lldr.h>
#include <hw_include/arch/lld_arch.h>
#include <hw_include/soc/am62ax/src/lldr_soc_ctrl_mmr.h>

#include <drivers/sciclient/include/sciclient_pm.h>
#include <drivers/sciclient.h>

#include <examples/Can/soc/am62ax/Generated_Dpl/ti_drivers_config.h>
#include "IntRtr_Cfg.h"
/* Defines the cfg for interrupt router, for this example */
#include <drivers/sciclient/include/tisci/am62ax/tisci_clocks.h>
#include <drivers/sciclient/include/tisci/am62ax/tisci_devices.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define MSG_NORMAL              (APP_UTILS_PRINT_MSG_NORMAL)
/**< Message type */
#define MSG_STATUS              (APP_UTILS_PRINT_MSG_STATUS)
/**< Message type */

#define TCA6424_CMD_AUTO_INC    ((uint8) 0x80U)

/* Input status register */
#define TCA6424_REG_INPUT0      ((UInt8) 0x00U)
#define TCA6424_REG_INPUT1      ((UInt8) 0x01U)
#define TCA6424_REG_INPUT2      ((UInt8) 0x02U)

/* Output register to change state of output BIT set to 1, output set HIGH */
#define TCA6424_REG_OUTPUT0     ((uint8) 0x04U)
#define TCA6424_REG_OUTPUT1     ((uint8) 0x05U)
#define TCA6424_REG_OUTPUT2     ((uint8) 0x06U)

/* Configuration register. BIT = '1' sets port to input, BIT = '0' sets
 * port to output */
#define TCA6424_REG_CONFIG0     ((uint8) 0x0CU)
#define TCA6424_REG_CONFIG1     ((uint8) 0x0DU)
#define TCA6424_REG_CONFIG2     ((uint8) 0x0EU)

/* This instance name corresponds to design instance name: mcu1_0 */
#define APP_MCU_MCAN_0_INT0     (CSLR_MCU_R5FSS0_CORE0_INTR_MCU_MCAN0_MCANSS_MCAN_LVL_INT_0)
#define APP_MCU_MCAN_1_INT0     (CSLR_MCU_R5FSS0_CORE0_INTR_MCU_MCAN1_MCANSS_MCAN_LVL_INT_0)
#define APP_MCAN_0_INT0         (CSLR_MAIN2MCU_LVL_INTRTR0_IN_MCAN0_MCANSS_MCAN_LVL_INT_0)
#if defined (SOC_J721E) || defined (SOC_J7200)
#define APP_MCAN_4_INT0         (CSLR_MAIN2MCU_LVL_INTRTR0_IN_MCAN4_MCANSS_MCAN_LVL_INT_0)
#define APP_MCAN_9_INT0         (CSLR_MAIN2MCU_LVL_INTRTR0_IN_MCAN9_MCANSS_MCAN_LVL_INT_0)
#define APP_MCAN_11_INT0        (CSLR_MAIN2MCU_LVL_INTRTR0_IN_MCAN11_MCANSS_MCAN_LVL_INT_0)
#endif
void CanApp_CanXIsr(void *CanPtr);
typedef void (*CanApp_IsrType)(void);

/* ========================================================================== */
/*              Internal Function Declarations                                */
/* ========================================================================== */
void CanApp_Startup(void);
void CanApp_BuildIntList(void);
void Can_delay (uint32 count);
void CanApp_PowerAndClkSrc(void);
void CanApp_PlatformInit();
void CanApp_EnableTransceivers(void);
void CanApp_InterruptConfig(void);
void CanApp_SetupCanFrame(uint32 ctlr_cnt, uint8 *hth, uint32 *mask);
#endif /* CANAPP_STARTUP_H_ */
