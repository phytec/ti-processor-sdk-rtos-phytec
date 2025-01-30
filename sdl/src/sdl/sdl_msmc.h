/********************************************************************
 * Copyright (C) 2022 Texas Instruments Incorporated.
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
 *
 *   @file  sdl_msmc.h
 *
 *   @brief This file contains the SDL-FL API's for MSMC Scrubbing Engine
 *
 *   This is the SDL-FL API documentation for the e Multicore Shared Memory 
 *   Controller (MSMC) module.
 *
 *
 *
 */

#ifndef SDL_IP_MSMC_H_
#define SDL_IP_MSMC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <sdl_types.h>
#include <sdlr64.h>
#if defined (SOC_J721E)
#include "src/sdl/misc/soc/j721e/sdl_soc_msmc.h"
#elif defined (SOC_J7200)
#include "src/sdl/misc/soc/j7200/sdl_soc_msmc.h"
#elif defined (SOC_J721S2)
#include "src/sdl/misc/soc/j721s2/sdl_soc_msmc.h"
#endif
#if defined (SOC_J784S4)
#include "src/sdl/misc/soc/j784s4/sdl_soc_msmc.h"
#endif

/**************************************************************************
* Register Macros
**************************************************************************/
#define SDL_MSMC_SCRUB_ENGINE_REGISTER                     (0x00003080UL)


/**************************************************************************
* Field Definition Macros
**************************************************************************/
/* SMEDCC */
#define SDL_MSMC_SCRUB_ENABLE_MASK                (0x80000000UL)
#define SDL_MSMC_SCRUB_ENABLE_SHIFT               (0x0000001FU)
#define SDL_MSMC_SCRUB_ENABLE_MAX                 (0x00000001UL)

/**
@ingroup SDL_MODULE
@defgroup SDL_MSMC_API MSMC API
*/

/**
@defgroup SDL_MSMC_FUNCTION  MSMC Functions
@ingroup SDL_MSMC_API
*/

/**
 *  @addtogroup SDL_MSMC_FUNCTION
    @{
 *
 */

/**
 *  \brief Checks enable status of MSMC Scrubbing Engine
 *
 *  This function returns enable status of MSMC Scrubbing Engine
 *
 * \return  true - if MSMC Scrubbing Engine enabled, else false
 *
 **/
bool SDL_MSMC_isScrubEnabled(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* SDL_IP_MSMC_H_ */
