/*
 * SDL ESM
 *
 * Software Diagnostics Reference module for Error Signaling Module
 *
 *  Copyright (c) Texas Instruments Incorporated 2021
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

#include <stdbool.h>
#include <stddef.h>

#include <sdl_esm.h>
#include <sdl_esm_core.h>
#include "sdl_esm_priv.h"

#if defined (SOC_J721E)
#include "src/sdl/esm/soc/j721e/sdl_esm_soc.h"
#endif /* SOC_J721E */

#if defined (SOC_J7200)
#include "src/sdl/esm/soc/j7200/sdl_esm_soc.h"
#endif /* SOC_J7200 */

#if defined (SOC_J721S2)
#include "src/sdl/esm/soc/j721s2/sdl_esm_soc.h"
#endif /* SOC_J721S2 */

#if defined (SOC_J784S4)
#include "src/sdl/esm/soc/j784s4/sdl_esm_soc.h"
#endif /* SOC_J784S4 */

/*
 * Design: PROC_SDL-1068
 */
static SDL_ESM_Instance_t SDL_ESM_instance_MCU;
static SDL_ESM_Instance_t SDL_ESM_instance_WKUP;
static SDL_ESM_Instance_t SDL_ESM_instance_MAIN;

/** ================================================================================
 *
 * \brief        Check that ESM instance type is valid for this device, and fill the
 *               ESM base address
 *
 * \param [in]   esmInstType: ESM instance type
 * \param [out]  esmBaseAddr: Base address for ESM instance's registers
 *
 * \return       true: if valid instance type; false if not valid instance type
 */
bool SDL_ESM_getBaseAddr(const SDL_ESM_Inst esmInstType, uint32_t *esmBaseAddr)
{
    bool instValid = ((bool)false);

    if (esmBaseAddr != NULL)
    {
        switch(esmInstType)
        {
            case SDL_ESM_INST_MCU_ESM0:
                instValid = ((bool)true);
                *esmBaseAddr = SOC_MCU_ESM_BASE;
                break;

            case SDL_ESM_INST_WKUP_ESM0:
                instValid = ((bool)true);
                *esmBaseAddr = SOC_WKUP_ESM_BASE;
                break;

            case SDL_ESM_INST_MAIN_ESM0:
                instValid = ((bool)true);
                *esmBaseAddr = SOC_MAIN_ESM_BASE;
                break;

            default:
                break;
        }
    }

    return (instValid);
}

/** ================================================================================
 *
 * \brief        Get the max number of ESM events supported for a given ESM instance
 *
 * \param [in]   esmInstType: ESM instance type
 * \param [out]  esmMaxNumEvents: Maximum number of ESM events supported
 *
 * \return       true: if valid instance type; false if not valid instance type
 */
bool SDL_ESM_getMaxNumEvents(const SDL_ESM_Inst esmInstType,
                             uint32_t *esmMaxNumEvents)
{
    bool instValid = ((bool)false);

    if (esmMaxNumEvents != NULL)
    {
        switch(esmInstType)
        {
            case SDL_ESM_INST_MCU_ESM0:
                instValid = ((bool)true);
                *esmMaxNumEvents = SOC_MCU_ESM_MAX_NUM_EVENTS;
                break;

            case SDL_ESM_INST_WKUP_ESM0:
                instValid = ((bool)true);
                *esmMaxNumEvents = SOC_WKUP_ESM_MAX_NUM_EVENTS;
                break;

            case SDL_ESM_INST_MAIN_ESM0:
                instValid = ((bool)true);
                *esmMaxNumEvents = SOC_MAIN_ESM_MAX_NUM_EVENTS;
                break;

            default:
                break;
        }
    }

    return (instValid);
}

/** ================================================================================
 *
 * \brief        Check that ESM instance type or ESM base address is valid for this
 *               device, and fill the SDL_ESM instance
 *
 * \param [in]   esmInstType:     ESM instance type
 * \param [out]  pEsmInstancePtr: Pointer to location of ESM instance structure
 *
 * \return       true: if valid instance type; false if not valid instance type
 */
bool SDL_ESM_selectEsmInst(const SDL_ESM_Inst esmInstType,
                           SDL_ESM_Instance_t **pEsmInstancePtr)
{
    bool instValid = ((bool)true);

    switch(esmInstType)
    {
        case SDL_ESM_INST_MCU_ESM0:
            *pEsmInstancePtr = (SDL_ESM_Instance_t *)(&SDL_ESM_instance_MCU);
            break;

        case SDL_ESM_INST_WKUP_ESM0:
            *pEsmInstancePtr = (SDL_ESM_Instance_t *)(&SDL_ESM_instance_WKUP);
            break;

        case SDL_ESM_INST_MAIN_ESM0:
            *pEsmInstancePtr = (SDL_ESM_Instance_t *)(&SDL_ESM_instance_MAIN);
            break;

        default:
            /* Invalid instance input parameter */
            instValid = ((bool)false);
            break;
    }

    return (instValid);
}

/** ================================================================================
 *
 * \brief        Check that ESM instance type or ESM base address is valid for this
 *               device, and fill the SDL_ESM instance
 *
 * \param [in]   esmInstBaseAddr: Base address for ESM instance's registers.
 *                                Function fills pointer to instance for this
 *                                set of registers
 * \param [out]  pEsmInstType:    Pointer to ESM instance type
 * \param [out]  pEsmInstancePtr: Pointer to location of ESM instance structure
 *
 * \return       true: if valid base address; false if not valid base address
 */
void SDL_ESM_selectEsmInstFromAddr(uint32_t esmInstBaseAddr,
                                   SDL_ESM_Inst *pEsmInstType,
                                   SDL_ESM_Instance_t **pEsmInstancePtr)
{
    switch(esmInstBaseAddr)
    {
        case SOC_MCU_ESM_BASE:
            *pEsmInstancePtr = &SDL_ESM_instance_MCU;
            *pEsmInstType = SDL_ESM_INST_MCU_ESM0;
            break;

        case SOC_WKUP_ESM_BASE:
            *pEsmInstancePtr = &SDL_ESM_instance_WKUP;
            *pEsmInstType = SDL_ESM_INST_WKUP_ESM0;
            break;

        case SOC_MAIN_ESM_BASE:
            *pEsmInstancePtr = &SDL_ESM_instance_MAIN;
            *pEsmInstType = SDL_ESM_INST_MAIN_ESM0;
            break;

        default:
            break;
    }
}

