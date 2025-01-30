/*
 *  Copyright (C) 2024 Texas Instruments Incorporated.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/**
 *  \ingroup LLD_IP_MODULE
 *  \defgroup LLD_HRPWM HRPWM
 *
 *  @{
 */
/**
 *  \file  csl_hrpwm.h
 *
 *  \brief This file contains declarations of LLD APIs
 *         corresponding to the HRPWM module. This also contains necessary
 *         structure, enum and macro definitions and APIs are consolidated to
 *         provide more functional APIs.
 *
 *  \details Please refer to programming sequence of EPWM
 */

#ifndef LLD_HRPWM_H_
#define LLD_HRPWM_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "Std_Types.h"
#include <Pwm/epwm_hw/V0_1/lldr_hrpwm.h>
#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */
/* None */

/* ========================================================================== */
/*                         Global Variables Declarations                      */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * \brief   This API loads Counter-Comparator A high resolution value and also
 *          configure the pulse select bits, that select which pulse to use for
 *          timing events in the HRPWM module.
 *
 * \param   pwmBaseAddr               Base Address of PWM instance used.
 * \param   hrBaseAddr                Base address of the HRPWM register
 *                                    NULL if not applicable for the SOC
 *                                    Please refer to the Technical Reference
 *                                    Manual (TRM) for the details
 * \param   compAHighResVal           Counter-comparator A high resolution value
 * \param   ShadowToActiveLoadTrigger Condition when the active reg to be loaded.
 *          This parameter can take values from the following enum
 *          - #LLD_EpwmHrRegActLoad_t;
 *
 * \retval  LLD_PASS for success
 * \retval  LLD_EUNSUPPORTED_CMD for unsupported API on this SOC
 *
 * \note    For SoCS that do not have HRCTL register, ShadowToActiveLoadTrigger
 *          value is not don't care and is not used.
 *
 */
int32_t CSL_epwmHrLoadCmpAHrValue2(uint32  pwmBaseAddr, uint32 hrBaseAddr,
                               uint32 compAHighResVal,
                               uint32 ShadowToActiveLoadTrigger);

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef LLD_EPWM_H_ */
 /** @} */
