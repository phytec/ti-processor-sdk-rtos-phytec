/*
 *  Copyright (C) 2020 Texas Instruments Incorporated
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
#ifndef SDLR_SOC_IN_H_
#define SDLR_SOC_IN_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <sdl_psilcfg_thread_map.h>
#include <sdl_soc_isc.h>
#include <sdl_soc_qos.h>
#include <sdlr_soc_intr.h>
#include <sdlr_navss_defines.h>
#include <sdlr_soc_baseaddress.h>
#include <sdlr_soc_defines.h>
#include <sdlr_mcu_r5fss0_baseaddress.h>
#include <sdlr_r5fss0_baseaddress.h>
#include <sdlr_main_ctrl_mmr.h>
#include <sdlr_main_sec_mmr.h>
#include <sdlr_mcu_pll_mmr.h>
#include <sdlr_main_pll_mmr.h>
#include <sdlr_mcu_ctrl_mmr.h>
#include <sdlr_mcu_sec_mmr.h>
#include <sdlr_wkup_ctrl_mmr.h>
#include <sdl_soc_firewalls.h>

 
 /* Manual changes to support LBIST base Addresses */
#define SDL_SMS_LBIST_BASE               (SDL_WKUP_CTRL_MMR0_CFG0_BASE + SDL_WKUP_CTRL_MMR_CFG0_SMS_LBIST_CTRL)
#define SDL_MCU_LBIST_BASE               (SDL_MCU_CTRL_MMR0_CFG0_BASE + SDL_MCU_CTRL_MMR_CFG0_MCU_LBIST_CTRL)
#define SDL_MAIN_R5F0_LBIST_BASE         (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MCU0_LBIST_CTRL)      
#define SDL_MAIN_R5F1_LBIST_BASE         (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MCU1_LBIST_CTRL)      
#define SDL_DMPAC_LBIST_BASE             (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_DMPAC_LBIST_CTRL)     
#define SDL_VPAC0_LBIST_BASE             (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_VPAC0_LBIST_CTRL)     
#define SDL_C7X0_LBIST_BASE              (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_DSP0_LBIST_CTRL)      
#define SDL_C7X1_LBIST_BASE              (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_DSP1_LBIST_CTRL)      
#define SDL_A72_0_LBIST_BASE             (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MPU0_LBIST_CTRL)      

#define SDL_SMS_LBIST_SIG               (SDL_WKUP_CTRL_MMR0_CFG0_BASE + SDL_WKUP_CTRL_MMR_CFG0_SMS_LBIST_SIG)
#define SDL_MCU_LBIST_SIG               (SDL_MCU_CTRL_MMR0_CFG0_BASE + SDL_MCU_CTRL_MMR_CFG0_MCU_LBIST_SIG)
#define SDL_MAIN_R5F0_LBIST_SIG         (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MCU0_LBIST_SIG)      
#define SDL_MAIN_R5F1_LBIST_SIG         (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MCU1_LBIST_SIG)      
#define SDL_DMPAC_LBIST_SIG             (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_DMPAC_LBIST_SIG)     
#define SDL_VPAC0_LBIST_SIG             (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_VPAC0_LBIST_SIG)     
#define SDL_C7X0_LBIST_SIG              (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_DSP0_LBIST_SIG)      
#define SDL_C7X1_LBIST_SIG              (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_DSP1_LBIST_SIG)      
#define SDL_A72_0_LBIST_SIG             (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MPU0_LBIST_SIG)      

#ifdef __cplusplus
}
#endif
#endif /* SDLR_SOC_IN_H_ */
