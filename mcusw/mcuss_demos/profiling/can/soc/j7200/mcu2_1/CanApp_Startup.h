/*
*
* Copyright (c) 2024-2020 Texas Instruments Incorporated
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
#include "Dio.h"

#include "app_utils.h"

#include <ti/csl/soc.h>
#include <ti/csl/cslr.h>
#include <ti/csl/arch/csl_arch.h>
#include <ti/csl/soc/cslr_soc_ctrl_mmr.h>
#include <ti/drv/i2c/I2C.h>
/* SCI Client */
#include <ti/drv/sciclient/sciclient.h>

/* Interrupt Registrations */
#include <ti/osal/osal.h>

//#include "IntRtr_Cfg.h"
/* Defines the cfg for interrupt router, for this example */

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define MSG_NORMAL              (APP_UTILS_PRINT_MSG_NORMAL)
/**< Message type */
#define MSG_STATUS              (APP_UTILS_PRINT_MSG_STATUS)
/**< Message type */

/* Input status register */
#define TCA6408A_REG_INPUT0      ((uint8) 0x00U)

/* Output register to change state of output BIT set to 1, output set HIGH */
#define TCA6408A_REG_OUTPUT0     ((uint8) 0x01U)

/* Configuration register. BIT = '1' sets port to input, BIT = '0' sets
 * port to output */
#define TCA6408A_REG_CONFIG0     ((uint8) 0x03U)

/* This instance name corresponds to design instance name: mcu2_1 */
#define APP_MCU_MCAN_0_INT0     (CSLR_R5FSS0_INTROUTER0_IN_MCU_MCAN0_MCANSS_MCAN_LVL_INT_0)
#define APP_MCU_MCAN_1_INT0     (CSLR_R5FSS0_INTROUTER0_IN_MCU_MCAN1_MCANSS_MCAN_LVL_INT_0)
#define APP_MCAN_0_INT0         (CSLR_R5FSS0_CORE1_INTR_MCAN0_MCANSS_MCAN_LVL_INT_0)
#define APP_MCAN_4_INT0         (CSLR_R5FSS0_CORE1_INTR_MCAN4_MCANSS_MCAN_LVL_INT_0)
#define APP_MCAN_8_INT0         (CSLR_R5FSS0_CORE1_INTR_MCAN8_MCANSS_MCAN_LVL_INT_0)
#define APP_MCAN_10_INT0        (CSLR_R5FSS0_CORE1_INTR_MCAN10_MCANSS_MCAN_LVL_INT_0)

/* Can App Isr to be registered */
void CanApp_CanXIsr(uintptr_t CanPtr);
typedef void (*CanApp_IsrType)(void);

/* ========================================================================== */
/*              Internal Function Declarations                                */
/* ========================================================================== */
/* Start up sequence : Program the interrupt muxes / priorities */
void CanApp_Startup(void);
/* Build Interrupt list for the enabled Can Instances */
void CanApp_BuildIntList(void);
/* Enable Clock Source, Currently done by SBL/GEL */
void CanApp_PowerAndClkSrc(void);
/* Enable Pin Mux Configuration */
void CanApp_PlatformInit();
/* Can Transceivers Enable */
void CanApp_EnableTransceivers(void);
/* Interrupt configuration for the enabled Can Instances */
void CanApp_InterruptConfig(void);
void SetupI2CTransfer(I2C_Handle handle,  uint32 slaveAddr,
                      uint8 *writeData, uint32 numWriteBytes,
                      uint8 *readData,  uint32 numReadBytes);
/* Set Up Can PDU Unit used for transmission */
void CanApp_SetupCanFrame(uint32 ctlr_cnt, uint8 *hth, uint32 *mask);
/* Set Up Can Controllers MCAN4 & MCAN8 for external loopback of MCAN4 -> MCAN8 */
Std_ReturnType CanApp_SetupControllersExternalLoopback(uint32 CanControllerTransition);
/* Set Up Can PDU Unit used for transmission for external loopback */
void CanApp_SetupCanFrameExternalLoopback(uint8 *hth, uint32 *mask);
#endif /* CANAPP_STARTUP_H_ */
