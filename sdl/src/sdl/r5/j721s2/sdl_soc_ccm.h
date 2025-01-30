/********************************************************************
*
* SOC CCM PROPERTIES. header file
*
* Copyright (C) 2015-2021 Texas Instruments Incorporated.
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
#ifndef SDL_SOC_CCM_H_
#define SDL_SOC_CCM_H_

#include <sdlr.h>
#include <tistdtypes.h>
#include <include/soc.h>
#ifdef __cplusplus
extern "C"
{
#endif

#define SDL_ESM_INSTANCE_MCU_R5F0     (SDLR_MCU_ESM0_ESM_PLS_EVENT0_MCU_R5FSS0_CCM_COMPARE_STAT_PULSE_INTR_0)
#define SDL_ESM_INSTANCE_R5F0         (SDLR_ESM0_ESM_PLS_EVENT0_R5FSS0_CCM_COMPARE_STAT_PULSE_INTR_0)
#define SDL_ESM_INSTANCE_R5F1         (SDLR_ESM0_ESM_PLS_EVENT0_R5FSS1_CCM_COMPARE_STAT_PULSE_INTR_0)

/** ===========================================================================
 *
 * @ingroup  SDL_MODULE
 * @defgroup SDL_CCM_API CPU Compare Module(CCM)
 *
 * ============================================================================
 */

/**
@defgroup SDL_CCM_ENUM CCM Enumerated Data Types
@ingroup SDL_CCM_API
*/

/** ===========================================================================
 *  @addtogroup SDL_CCM_ENUM
    @{
 * ============================================================================
 */

 /** ---------------------------------------------------------------------------
 * @brief This enumerator defines the possible CCM instance
 *
 *  \anchor SDL_CCM_Inst
 *  \name CCM instance
 *
 *  @{
 * ----------------------------------------------------------------------------
 */


typedef uint32_t SDL_CCM_Inst;
	/** MCU R5F0 */
#define SDL_CCM_MCU_R5F0       0U
	/** MAIN R5F0 */
#define SDL_CCM_R5F0           1U
	/** MAIN R5F1 */
#define SDL_CCM_R5F1           2U
	/** Maximum */
#define SDL_CCM_MAX_INSTANCE   (SDL_CCM_R5F1 + 1U)

/* @} */

/** @} */

static uint32_t SDL_CCM_baseAddress[SDL_CCM_MAX_INSTANCE] =
{
	SDL_MCU_R5FSS0_COMPARE_CFG_BASE,
	SDL_R5FSS0_COMPARE_CFG_BASE,
	SDL_R5FSS1_COMPARE_CFG_BASE,
};

#ifdef __cplusplus
}
#endif
#endif /* SDL_SOC_CCM_H_ */

