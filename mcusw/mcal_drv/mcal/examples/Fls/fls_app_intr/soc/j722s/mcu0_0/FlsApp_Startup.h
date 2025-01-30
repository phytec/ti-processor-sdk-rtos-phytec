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
 *  \file     FlsApp_Startup.c
 *
 *  \brief    This file contains interrupt and clock configuration.
 */
/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */


#include "Fls.h"

#include <hw_include/arch/lld_arch.h>
#include <hw_include/soc.h>
#include <hw_include/lldr.h>
#include <hw_include/lld_types.h>
#include <Fls/fls_hw/ospi/V0/lldr_ospi.h>
#include <Fls/fls_hw/ospi/V0/lld_ospi.h>
#include <stdint.h>
#include <hw_include/soc.h>
#include <hw_include/hw_types.h>
#include <hw_include/lldr.h>
#include <hw_include/arch/lld_arch.h>
#include <drivers/sciclient.h>


#include <kernel/dpl/HwiP.h>
#include <examples/Fls/soc/j722s/Generated_Dpl/ti_drivers_config.h>

#include <drivers/sciclient/include/tisci/j722s/tisci_clocks.h>
#include <drivers/sciclient/include/tisci/j722s/tisci_devices.h>
/* Interrupt Registrations */
#include "app_utils.h"

#include <examples_config/IntRtr_Cfg/soc/j722s/mcu0_0/IntRtr_Cfg.h>


#define MSG_NORMAL              (APP_UTILS_PRINT_MSG_NORMAL)
/**< Message type */
#define MSG_STATUS              (APP_UTILS_PRINT_MSG_STATUS)


/**< Message type */
/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

//#define XIP_TEST
#define APP_FLS_INT   CSLR_WKUP_R5FSS0_CORE0_INTR_FSS0_OSPI_0_OSPI_LVL_INTR_0


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */


Int32 Fls_OSPI_configClk(uint64 clk_input);
void FlsApp_InterruptConfig(void);
