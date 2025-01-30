/*
 * SDL ESM
 *
 * Software Diagnostics Reference module for ESM
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

#ifndef INCLUDE_SDL_ESM_PRIV_H_
#define INCLUDE_SDL_ESM_PRIV_H_

#include <stddef.h>
#include <stdbool.h>

#include <src/ip/sdl_ip_esm.h>
#include "sdl_esm.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
     /**< selfTest Flag */
     volatile bool selfTestFlag;
     /**< Store the ECC callback function arg */
     void *eccCallBackFunctionArg;
     /**< Store the CCM callback function arg */
     void *ccmCallBackFunctionArg;
     /**< Store application argument */
     void *arg;
     uint32_t eccenableBitmap[SDL_ESM_MAX_EVENT_MAP_NUM_WORDS];
     uint32_t ccmenableBitmap[SDL_ESM_MAX_EVENT_MAP_NUM_WORDS];
     SDL_ESM_config esmInitConfig;
     /**< Store ECC callback function */
     SDL_ESM_applicationCallback eccCallBackFunction;
     /**< Store the CCM callback function */
     SDL_ESM_applicationCallback CCMCallBackFunction;
     /**< Store applicataion callback function */
     SDL_ESM_applicationCallback callback;
}SDL_ESM_Instance_t;


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
bool SDL_ESM_getBaseAddr(SDL_ESM_Inst esmInstType, uint32_t *esmBaseAddr);

/** ================================================================================
 *
 * \brief        Check that ESM instance type is valid for this device, and fill the
 *               SDL_ESM instance
 *
 * \param [in]   esmInstType: ESM instance type
 * \param [out]  pEsmInstancePtr: Pointer to location of ESM instance structure
 *
 * \return       true: if valid instance type; false if not valid instance type
 */
bool SDL_ESM_selectEsmInst(SDL_ESM_Inst esmInstType,
                           SDL_ESM_Instance_t **pEsmInstancePtr);

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
                                   SDL_ESM_Instance_t **pEsmInstancePtr);

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
                             uint32_t *esmMaxNumEvents);
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_SDL_ESM_PRIV_H_ */
