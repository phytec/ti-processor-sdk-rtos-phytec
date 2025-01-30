/* =============================================================================
 *   Copyright (c) Texas Instruments Incorporated 2022
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

#ifndef ESM_APP_R5_H
#define ESM_APP_R5_H

#ifdef __cplusplus
extern "C" {
#endif

#include <include/soc.h>
#include <src/ip/sdl_ip_esm.h>

#define BITS_PER_WORD                       (32u)

typedef struct SDL_esm_app_R5_cfg_s {
    uint32_t    hi_pri_evt;
    uint32_t    lo_pri_evt;
}SDL_esm_app_R5_cfg;

/* ESM Base Addresses */

#define SDL_TEST_ESM_EN_KEY_ENBALE_VAL             (0xFU)
#define BITS_PERWORD                               (32u)
#define GROUP_NUMBER_BIT_SHIFT                     (5u)
#define NO_EVENT_VALUE                             (0xffffu)

#define SDL_APP_INT_NUM_INVALID                    (0xffffffffU)

#if defined(SOC_J7200) || defined(SOC_J721S2) || defined(SOC_J784S4)
    #define DDRSS_CFG_BASE                  (SDL_COMPUTE_CLUSTER0_SS_CFG_BASE)
    #if defined(BUILD_MPU)
        #define ESM_CFG_BASE                (SDL_ESM0_CFG_BASE)
        #define ESM_LO_INT                  (SDLR_COMPUTE_CLUSTER0_GIC500SS_SPI_ESM0_ESM_INT_LOW_LVL_0)
        #define ESM_HI_INT                  (SDLR_COMPUTE_CLUSTER0_GIC500SS_SPI_ESM0_ESM_INT_HI_LVL_0)
        #define ESM_CFG_ERR_INT             (SDLR_COMPUTE_CLUSTER0_GIC500SS_SPI_ESM0_ESM_INT_CFG_LVL_0)

    #else
        #define ESM_CFG_BASE                (SDL_ESM0_CFG_BASE)
        #define ESM_LO_INT                  (SDLR_MCU_R5FSS0_CORE0_INTR_ESM0_ESM_INT_LOW_LVL_0)
        #define ESM_HI_INT                  (SDLR_MCU_R5FSS0_CORE0_INTR_ESM0_ESM_INT_HI_LVL_0)
        #define ESM_CFG_ERR_INT             (SDLR_MCU_R5FSS0_CORE0_INTR_ESM0_ESM_INT_CFG_LVL_0)
        #define ESM_MAX_EVENT_MAP_NUM_WORDS (4u)

        #define MCU_ESM_CFG_BASE                (SDL_MCU_ESM0_CFG_BASE)
        #define MCU_ESM_LO_INT                  (SDLR_MCU_R5FSS0_CORE0_INTR_MCU_ESM0_ESM_INT_LOW_LVL_0)
        #define MCU_ESM_HI_INT                  (SDLR_MCU_R5FSS0_CORE0_INTR_MCU_ESM0_ESM_INT_HI_LVL_0)
        #define MCU_ESM_CFG_ERR_INT             (SDLR_MCU_R5FSS0_CORE0_INTR_MCU_ESM0_ESM_INT_CFG_LVL_0)
        #define MCU_ESM_MAX_EVENT_MAP_NUM_WORDS (4u)
    #endif
#endif
typedef void (*ESM_handlerPtr)(uint32_t esmEvent);

int32_t sdlAppEsmSetup(void);
int32_t sdlAppEsmConfig(uint32_t  baseAddr, uint32_t esmEventNumber,
                        uint32_t priority, ESM_handlerPtr pESMHandler,
                        uint32_t parameter);

int32_t sdlAppEsmDisable(uint32_t  baseAddr, uint32_t esmEventNumber);

int32_t sdlAppEsmRegisterIntr(void);

/* exteran variable references */
extern uint32_t gUartBaseAddr;

#ifdef __cplusplus
}
#endif

#endif

