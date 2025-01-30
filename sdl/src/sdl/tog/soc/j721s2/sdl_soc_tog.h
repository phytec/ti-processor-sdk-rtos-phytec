/********************************************************************
*
* SOC TOG PROPERTIES. header file
*
* Copyright (C) 2015-2020 Texas Instruments Incorporated.
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
#ifndef SDL_AEP_SOC_TOG_H_
#define SDL_AEP_SOC_TOG_H_

#include <sdlr.h>
#include <tistdtypes.h>
#include <include/soc.h>
#ifdef __cplusplus
extern "C"
{
#endif

/*
* SDL definitions for SoC TOG Instances:
*/

 /** ---------------------------------------------------------------------------
 * @brief This enumerator defines the possible TOG instance
 *
 *  \anchor SDL_TOG_Inst
 *  \name TOG instance
 *
 *  @{
 * ----------------------------------------------------------------------------
 */

typedef uint8_t SDL_TOG_Inst;
	/** NAVSS_TO_AC */
#define SDL_TOG_INSTANCE_STOG0	           0U
	/** AC_CFG */
#define SDL_TOG_INSTANCE_STOG1  	       1U
	/** AC_CFG_TO_AC_CFG */
#define SDL_TOG_INSTANCE_STOG2      	   2U
	/** RC_TO_HC2 */
#define SDL_TOG_INSTANCE_STOG3    		   3U
	/** RC_TO_HC2_*/
#define SDL_TOG_INSTANCE_STOG4             4U
	/** HC2_TO_HC */
#define SDL_TOG_INSTANCE_STOG5             5U
	/** RC_TO_RC */
#define SDL_TOG_INSTANCE_STOG6             6U
	/** IPPHY_TO_RTI */
#define SDL_TOG_INSTANCE_STOG7             7U
	/** IPPHY_TO_IPPHY */
#define SDL_TOG_INSTANCE_STOG8             8U
	/** MAIN_INFRA */
#define SDL_TOG_INSTANCE_STOG9 		       9U
	/** PVU0_CFG */
#define SDL_TOG_INSTANCE_STOG10            10U
    /** WKUP_VDC_INFRA */
#define SDL_TOG_INSTANCE_STOG11            11U
    /** TIMEOUT_64B2 */
#define SDL_TOG_INSTANCE_STOG12            12U
    /** TIMEOUT_64B3 */
#define SDL_TOG_INSTANCE_STOG13            13U
    /** TIMEOUT_64B4 */
#define SDL_TOG_INSTANCE_STOG14            14U
    /** MCU_VDC_SOC */
#define SDL_TOG_INSTANCE_STOG15            15U
    /** MCU_VDC_INFRA */
#define SDL_TOG_INSTANCE_STOG16            16U
	/** Maximum */
#define SDL_TOG_MAX_INSTANCE               SDL_TOG_INSTANCE_STOG16

static uint32_t SDL_TOG_baseAddress[SDL_TOG_MAX_INSTANCE+1U] =
{
	SDL_AM_NAVSS_TO_AC_NON_SAFE_STOG4_CFG_BASE,
	SDL_AM_AC_CFG_TO_AC_CFG_NON_SAFE_STOG2_CFG_BASE,
	SDL_AM_AC_CFG_TO_AC_CFG_NON_SAFE_STOG9_CFG_BASE,
	SDL_AM_RC_TO_HC2_STOG6_CFG_BASE,
	SDL_AM_RC_TO_HC2_STOG7_CFG_BASE,
	SDL_AM_HC2_TO_HC_CFG_STOG5_CFG_BASE,
	SDL_AM_RC_TO_RC_CFG_STOG3_CFG_BASE,
	SDL_AM_IPPHY_TO_RTI_GPU_STOG8_CFG_BASE,
	SDL_AM_IPPHY_TO_IPPHY_STOG1_CFG_BASE,
	SDL_AM_MAIN_INFRA_TO_MAIN_INFRA_STOG0_CFG_BASE,
	SDL_NAVSS0_PVU0_CFG_TOG_CFG_BASE,
	SDL_WKUP_VDC_INFRA_VBUSP_32B_SRC_SAFEG0_CFG_BASE, 
	SDL_MCU_TIMEOUT_64B2_CFG_BASE,
	SDL_MCU_TIMEOUT_64B3_CFG_BASE, 
	SDL_MCU_TIMEOUT_64B4_CFG_BASE,
	SDL_MCU_VDC_SOC_FW_VBUSP_32B_SRC_SAFEG1_CFG_BASE, 
	SDL_MCU_VDC_INFRA_VBUSP_32B_SRC_SAFEG0_CFG_BASE, 
};

/* Properties of TOG instances in: TOG */

#ifdef __cplusplus
}
#endif

#endif /* SDL_SOC_TOG_H_ */

