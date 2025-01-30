/********************************************************************
*
* SOC MTOG PROPERTIES. header file
*
* Copyright (C) 2022 Texas Instruments Incorporated.
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
#ifndef SDL_AEP_SOC_MTOG_H_
#define SDL_AEP_SOC_MTOG_H_

#include <sdlr.h>
#include <tistdtypes.h>
#include <include/soc.h>
#ifdef __cplusplus
extern "C"
{
#endif

/*
* SDL definitions for SoC MTOG Instances:
*/

 /** ---------------------------------------------------------------------------
 * @brief This enumerator defines the possible MTOG instance
 *
 *  \anchor SDL_MTOG_Inst
 *  \name MTOG instance
 *
 *  @{
 * ----------------------------------------------------------------------------
 */
typedef uint8_t SDL_MTOG_Inst;

    /** MAIN_MTOG0 */                 
#define SDL_INSTANCE_MAIN_MTOG0               0U
    /** MAIN_MTOG1 */                 
#define SDL_INSTANCE_MAIN_MTOG1               1U
    /** MAIN_MTOG4 */                 
#define SDL_INSTANCE_MAIN_MTOG4               2U
    /** MAIN_MTOG5 */                 
#define SDL_INSTANCE_MAIN_MTOG5               3U
    /** MAIN_MTOG14 */                 
#define SDL_INSTANCE_MAIN_MTOG14              4U
    /** MAIN_MTOG24 */                 
#define SDL_INSTANCE_MAIN_MTOG24              5U
    /** MAIN_MTOG25 */                 
#define SDL_INSTANCE_MAIN_MTOG25              6U
    /** MAIN_MTOG32 */                 
#define SDL_INSTANCE_MAIN_MTOG32              7U
    /** MAIN_MTOG33 */                 
#define SDL_INSTANCE_MAIN_MTOG33              8U
    /** MAIN_MTOG34 */                 
#define SDL_INSTANCE_MAIN_MTOG34              9U
    /** MAIN_MTOG35 */                 
#define SDL_INSTANCE_MAIN_MTOG35              10U
    /** MAIN_MTOG36 */                 
#define SDL_INSTANCE_MAIN_MTOG36              11U
    /** MAIN_MTOG37 */                 
#define SDL_INSTANCE_MAIN_MTOG37              12U
    /** MAIN_MTOG38 */                 
#define SDL_INSTANCE_MAIN_MTOG38              13U
    /** MAIN_MTOG39 */                 
#define SDL_INSTANCE_MAIN_MTOG39              14U
    /** MCU_MTOG0 */
#define SDL_INSTANCE_MCU_MTOG0                15U
    /** MCU_MTOG16 */                 
#define SDL_INSTANCE_MCU_MTOG16               16U
    /** MCU_MTOG17 */                 
#define SDL_INSTANCE_MCU_MTOG17               17U
    /** MCU_MTOG18 */                 
#define SDL_INSTANCE_MCU_MTOG18               18U
    /** MCU_MTOG19 */                 
#define SDL_INSTANCE_MCU_MTOG19               19U
    /** MCU_MTOG20 */                 
#define SDL_INSTANCE_MCU_MTOG20               20U
    /** MCU_MTOG21 */                 
#define SDL_INSTANCE_MCU_MTOG21               21U
    /** MCU_MTOG22 */                 
#define SDL_INSTANCE_MCU_MTOG22               22U
    /** MCU_MTOG23 */                 
#define SDL_INSTANCE_MCU_MTOG23               23U
    /** MAX Instance MTOG */                 
#define SDL_INSTANCE_MTOG_MAX                 SDL_INSTANCE_MCU_MTOG23 

static uint32_t SDL_MTOG_baseAddress[SDL_INSTANCE_MTOG_MAX+1U] =
{
    (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MAIN_MTOG0_CTRL ),
    (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MAIN_MTOG1_CTRL ),
    (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MAIN_MTOG4_CTRL ),
    (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MAIN_MTOG5_CTRL ),
    (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MAIN_MTOG14_CTRL),  
    (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MAIN_MTOG24_CTRL),
    (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MAIN_MTOG25_CTRL),
    (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MAIN_MTOG32_CTRL),
    (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MAIN_MTOG33_CTRL),
    (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MAIN_MTOG34_CTRL),
    (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MAIN_MTOG35_CTRL),
    (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MAIN_MTOG36_CTRL),
    (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MAIN_MTOG37_CTRL),
    (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MAIN_MTOG38_CTRL),
    (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MAIN_MTOG39_CTRL),
    (SDL_MCU_CTRL_MMR0_CFG0_BASE +SDL_MCU_CTRL_MMR_CFG0_MCU_MTOG0_CTRL),
    (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MAIN_MTOG16_CTRL ),
    (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MAIN_MTOG17_CTRL),
    (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MAIN_MTOG18_CTRL),
    (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MAIN_MTOG19_CTRL),
    (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MAIN_MTOG20_CTRL),
    (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MAIN_MTOG21_CTRL),
    (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MAIN_MTOG22_CTRL),
    (SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_MAIN_MTOG23_CTRL),
};

/* Properties of MTOG instances in: MTOG */

#ifdef __cplusplus
}
#endif

#endif /* SDL_AEP_SOC_MTOG_H_ */

