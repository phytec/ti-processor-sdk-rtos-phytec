/*
 *   Copyright (c) Texas Instruments Incorporated 2021
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
 *  \file    sdl_soc_pok.h
 *
 *  \brief    This file contains the prototypes of the APIs present in the
 *            device abstraction layer file of POK.
 *            This also contains some related macros.
 */

#ifndef SDL_SOC_POK_H_
#define SDL_SOC_POK_H_
 
#include <src/ip/sdl_ip_pok.h>
#include <src/ip/sdl_ip_esm.h>
#include <osal/sdl_osal.h>

#include <include/soc/j721s2/sdlr_soc_baseaddress.h>
#include <include/soc/j721s2/sdlr_wkup_ctrl_mmr.h>

#ifdef __cplusplus
extern "C" {
#endif

/** 
 *
 * @ingroup  SDL_MODULE
 * @defgroup SDL_POK_API Power OK(POK)
 *   Provides the APIs for POK.
 *
 */

/**
@defgroup SDL_POK_DATASTRUCT  POK Data Structures
@ingroup SDL_POK_API
*/
/**
@defgroup SDL_POK_FUNCTION  POK Functions
@ingroup SDL_POK_API
*/

/**************************************************************************
* Register Macros
**************************************************************************/
#define SDL_POK_MMR_BASE                (SDL_WKUP_CTRL_MMR0_CFG0_BASE)

#define WKUP_ESM_INSTANCE                     (SDL_WKUP_ESM0_CFG_BASE)
#define WKUP_ESM_INTID                        (SDLR_MCU_R5FSS0_CORE0_INTR_WKUP_ESM0_ESM_INT_HI_LVL_0)
#define WKUP_ESM_ERR_SIG_VDD_CORE_UV          (SDLR_WKUP_ESM0_ESM_PLS_EVENT0_WKUP_PRG0_POK_PGOOD_UV_OUT_N_TO_ESM_2)
#define WKUP_ESM_ERR_SIG_VDD_CORE_OV          (SDLR_WKUP_ESM0_ESM_PLS_EVENT0_WKUP_PRG0_POK_PGOOD_OV_OUT_N_TO_ESM_2)
#define WKUP_ESM_ERR_SIG_VDDR_CORE_UV         (SDLR_WKUP_ESM0_ESM_PLS_EVENT0_WKUP_PRG0_POK_PGOOD_UV_OUT_N_TO_ESM_4)
#define WKUP_ESM_ERR_SIG_VDDR_CORE_OV         (SDLR_WKUP_ESM0_ESM_PLS_EVENT0_WKUP_PRG0_POK_PGOOD_OV_OUT_N_TO_ESM_4)
#define WKUP_ESM_ERR_SIG_VDD_CPU_UV           (SDLR_WKUP_ESM0_ESM_PLS_EVENT0_WKUP_PRG0_POK_PGOOD_UV_OUT_N_TO_ESM_3)
#define WKUP_ESM_ERR_SIG_VDD_CPU_OV           (SDLR_WKUP_ESM0_ESM_PLS_EVENT0_WKUP_PRG0_POK_PGOOD_OV_OUT_N_TO_ESM_3)
#define WKUP_ESM_ERR_SIG_VMON_EXT_UV          (SDLR_WKUP_ESM0_ESM_PLS_EVENT0_WKUP_PRG0_POK_PGOOD_UV_OUT_N_TO_ESM_5)
#define WKUP_ESM_ERR_SIG_VMON_EXT_OV          (SDLR_WKUP_ESM0_ESM_PLS_EVENT0_WKUP_PRG0_POK_PGOOD_OV_OUT_N_TO_ESM_5)
#define WKUP_ESM_ERR_SIG_VMON_EXT_MAIN_1P8_OV (SDLR_WKUP_ESM0_ESM_PLS_EVENT0_WKUP_PRG0_POK_PGOOD_OV_OUT_N_TO_ESM_0)
#define WKUP_ESM_ERR_SIG_VMON_EXT_MAIN_1P8_UV (SDLR_WKUP_ESM0_ESM_PLS_EVENT0_WKUP_PRG0_POK_PGOOD_UV_OUT_N_TO_ESM_0)
#define WKUP_ESM_ERR_SIG_VMON_EXT_MAIN_3P3_OV (SDLR_WKUP_ESM0_ESM_PLS_EVENT0_WKUP_PRG0_POK_PGOOD_OV_OUT_N_TO_ESM_1)
#define WKUP_ESM_ERR_SIG_VMON_EXT_MAIN_3P3_UV (SDLR_WKUP_ESM0_ESM_PLS_EVENT0_WKUP_PRG0_POK_PGOOD_UV_OUT_N_TO_ESM_1)
#define WKUP_ESM_ERR_SIG_VDD_MCU_OV           (SDLR_WKUP_ESM0_ESM_PLS_EVENT0_WKUP_PRG_MCU0_POK_PGOOD_UV_OUT_N_TO_ESM_4)
#define WKUP_ESM_ERR_SIG_VDD_MCU_UV           (SDLR_WKUP_ESM0_ESM_PLS_EVENT0_WKUP_PRG_MCU0_POK_PGOOD_UV_OUT_N_TO_ESM_0)
#define WKUP_ESM_ERR_SIG_VDDR_MCU_UV          (SDLR_WKUP_ESM0_ESM_PLS_EVENT0_WKUP_PRG_MCU_3POKS0_POK_PGOOD_UV_OUT_N_TO_ESM_2)
#define WKUP_ESM_ERR_SIG_VDDR_MCU_OV          (SDLR_WKUP_ESM0_ESM_PLS_EVENT0_WKUP_PRG_MCU_3POKS0_POK_PGOOD_OV_OUT_N_TO_ESM_2)
#define WKUP_ESM_ERR_SIG_VDDSHV_WKUP_GEN_UV   (SDLR_WKUP_ESM0_ESM_PLS_EVENT0_WKUP_PRG_MCU_3POKS0_POK_PGOOD_UV_OUT_N_TO_ESM_1)
#define WKUP_ESM_ERR_SIG_VDDSHV_WKUP_GEN_OV   (SDLR_WKUP_ESM0_ESM_PLS_EVENT0_WKUP_PRG_MCU_3POKS0_POK_PGOOD_OV_OUT_N_TO_ESM_1)
#define WKUP_ESM_ERR_SIG_CAP_VDDS_MCU_GEN_UV  (SDLR_WKUP_ESM0_ESM_PLS_EVENT0_WKUP_PRG_MCU_3POKS0_POK_PGOOD_UV_OUT_N_TO_ESM_0)
#define WKUP_ESM_ERR_SIG_CAP_VDDS_MCU_GEN_OV  (SDLR_WKUP_ESM0_ESM_PLS_EVENT0_WKUP_PRG_MCU_3POKS0_POK_PGOOD_OV_OUT_N_TO_ESM_0)
#define WKUP_ESM_ERR_SIG_VDDA_PMIC_IN_UV      (SDLR_WKUP_ESM0_ESM_PLS_EVENT0_WKUP_PRG_MCU0_POK_PGOOD_UV_OUT_N_TO_ESM_3)
#define WKUP_ESM_ERR_SIG_VDDA_MCU_UV          (SDLR_WKUP_ESM0_ESM_PLS_EVENT0_WKUP_PRG_MCU0_POK_PGOOD_UV_OUT_N_TO_ESM_2)
#define WKUP_ESM_ERR_SIG_VDDA_MCU_OV          (SDLR_WKUP_ESM0_ESM_PLS_EVENT0_WKUP_PRG_MCU0_POK_PGOOD_UV_OUT_N_TO_ESM_1)

#define  ESM_INSTANCE                      WKUP_ESM_INSTANCE
#define  ESM_INTID                         WKUP_ESM_INTID

#ifdef __cplusplus
}
#endif
#endif /* SDL_SOC_POK_H_ */

