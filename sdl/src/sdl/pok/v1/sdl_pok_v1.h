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
 *  \file    sdl_pok_v1.h
 *
 *  \brief    This file contains the prototypes of the APIs present in the
 *            device abstraction layer file of POK.
 *            This also contains some related macros.
 */

#ifndef SDL_POK_V1_H_
#define SDL_POK_V1_H_
 
#include <src/ip/sdl_ip_pok.h>
#include <src/ip/sdl_ip_esm.h>
#include <osal/sdl_osal.h>

#if defined (SOC_J7200)
#include <include/soc/j7200/sdlr_soc_baseaddress.h>
#include <include/soc/j7200/sdlr_wkup_ctrl_mmr.h>
#endif

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

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/** ===========================================================================
 *  @addtogroup SDL_POK_DATASTRUCT
    @{
 * ============================================================================
 */

/** ---------------------------------------------------------------------------
 * \brief POK Static Registers structure
 *
 * This structure defines the POK static configuration registers
 * ----------------------------------------------------------------------------
 */
typedef struct SDL_Pok_Static_Reg_read{
    uint8_t hystCtrl;
    /**< Hysteresis control value */
    uint8_t hystCtrlOV;
    /**< Hysteresis control for OV */
    uint8_t voltDetMode;
    /**< Voltage Detection Mode value */
    uint8_t trim;
    /**< trim value */
    uint8_t trimOV;
    /**< trim value for OV */
    uint8_t deglitch;
    /**< deglitch value */
    bool porBGapOK;
    /**< POR Band Gap OK status */
    uint8_t porModuleStatus;
    /**< POR Module Status */
}SDL_POK_staticRegs;

/** @} */

/** ============================================================================
 *
 * \brief   POK Function to enable or diable ping/pong mode for a specified PRG.
 *          Only supported for v1 version of the IP.
 *
 * \param   instance: Instance of PRG
 * \param   enable: Enable or disable the ping/pong mode
 *
 * \return  SDL_PASS for success. SDL Error Code for failure.
 */
int32_t SDL_POK_enablePP(SDL_PRG_Inst instance, bool enable);

#ifdef __cplusplus
}
#endif
#endif /* SDL_SOC_POK_H_ */

