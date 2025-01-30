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
 *  \file     WdgApp.h
 *
 *  \brief    WDG example header file
 *
 */

#ifndef WDG_APP_STARTUP_H_
#define WDG_APP_STARTUP_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#ifdef __cplusplus
extern "C"
{
#endif

#include "Std_Types.h"
#include "Wdg.h"
#include "SchM_Wdg.h"
#include "app_utils.h"
#include "stdint.h"
#include <hw_include/soc.h>
#include <hw_include/hw_types.h>
#include <hw_include/lld_rti.h>
#include <hw_include/lldr.h>
/* Design : MCAL-5597 */
#include <hw_include/lld_esm.h>
/* Interrupt Registrations */
#include <ti/osal/osal.h>
#include "WdgApp.h"
#include <examples/utils/rti.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
//#define TISCI_DEV_RTI29_RTI_CLK_PARENT_J7_WAKEUP_16FF_WKUP_0_WKUP_RCOSC_12P5M_CLK  (0)
//
#define MAIN_ESM_INSTANCE    (CSL_ESM0_CFG_BASE)
/* ESM_INSTANCE */
#define MAIN_ESM_INTID       (CSLR_R5FSS0_CORE1_INTR_ESM0_ESM_INT_HI_LVL_0)
/* ESM Interrupt ID to the core */
#define MAIN_ESM_ERR_SIG     (CSLR_ESM0_ESM_PLS_EVENT0_RTI29_INTR_WWD_0)

/*
* To change the ESM Instance as desired, use the following definitions:
* - ESM_INSTANCE
* - ESM_INTID
* - ESM_ERR_SIG
*/
#define ESM_INSTANCE        (MAIN_ESM_INSTANCE)
#define ESM_INTID           (MAIN_ESM_INTID)
#define ESM_ERR_SIG         (MAIN_ESM_ERR_SIG)


/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */
void WdgApp_InterruptConfig(void);
void WdgApp_Startup(void);
void WdgApp_PowerAndClkSrc(void);
void WdgApp_ESMIsr(uintptr_t handle);
void WdgApp_ESMInit(void);
void WdgApp_EnableESMErrorPin(void);

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef WDG_APP_STARTUP_H_ */
