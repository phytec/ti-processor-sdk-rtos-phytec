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
 *  \file     Mcu_Priv.h
 *
 *  \brief    This file contains the common declarations and macros used across
 *            all the MCU MCAL driver files.
 *
 */

#ifndef MCU_PRIV_H_
#define MCU_PRIV_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */


#include "Std_Types.h"
#include <hw_include/lldr.h>
#include <hw_include/soc.h>
#include "Mcu.h"

#include <hw_include/arch/lld_arch.h>
#include <hw_include/soc/lldr_soc_ctrl_mmr.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */


/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */



typedef struct Mcu_DriverObjType_s
{
  Mcu_ModeType             Mcu_Mode;
  /**< Mcu mode to be used @sa  MCAL_MCU_API:McuModes */
  uint8                    Mcu_ResetMode;
  /**< Reset mode to be used @sa  MCAL_MCU_API:ResetModes */
  Mcu_ClockConfigPtrType   Mcu_ClockConfig;
  /**< Configuration of Module functional clock source */
  Mcu_DomainType           Domain;
  /**< Domain type */
  uint8                    Mcu_NumberOfClockConfig;
  /**< Number of entries in Mcu_ClockConfig */
  Mcu_CBKFunctionPtrType   Mcu_CBK_Function;
} Mcu_DriverObjType;



/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
/*None*/

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

Mcu_ResetType Mcu_GetRawResetReason(Mcu_RawResetType reset_raw);
void Mcu_PerformSoftSysReset(uint32 resetVal);
uint32 Mcu_GetSystemRstCause(void);
FUNC(Mcu_ResetType, MCU_CODE)Mcu_InterpretRawResetStatus(
    Mcu_RawResetType reset_raw);
FUNC(void, MCU_CODE) Mcu_ClearResetReason(void);

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef MCU_PRIV_H_ */
