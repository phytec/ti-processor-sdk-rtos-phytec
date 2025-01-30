/**
 * @file  sdl_rat.h
 *
 * @brief
 *  Header file containing various enumerations, structure definitions and function
 *  declarations for the KSBUS RAT Ip.
 *      =
 *  @n   (C) Copyright 2016-2020, Texas Instruments, Inc.
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
#ifndef SDL_RAT_H_
#define SDL_RAT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <src/ip/sdl_ip_rat.h>
#include <sdl_types.h>
#include <tistdtypes.h>
#include <stdint.h>
#include <stdbool.h>
#include <src/sdl/hw_types.h>


#if defined (SOC_J721E)
#include <include/soc/j721e/sdlr_soc_baseaddress.h>
#endif


/**
 * @ingroup SDL_MODULE
 * @defgroup SDL_RAT_API RAT API
 */

/**
 * @defgroup SDL_RAT_FUNCTION RAT Functions
 * @ingroup SDL_RAT_API
 */

/**     
 *  @addtogroup SDL_RAT_FUNCTION
    @{
 * 
 */


 /**
 * \brief   This function returns the enable status of the specified region address
 *          translation
 *
 * \param   pRatRegs       - A pointer (of type SDL_ratRegs*) to the base
 *                           address of the RAT module
 *
 * \param   regionIndex    - The index of the region to configure. This value must
 *                           be in the range 0..(SDL_RAT_getMaxRegions()-1) inclusive.
 */
int32_t SDL_RAT_isRegionTranslationEnabled(const SDL_ratRegs *pRatRegs, uint32_t regionIndex);

 /**
 * \brief   This function is used to set the destination ID for error messages.
 *
 * \param   pRatRegs   - A pointer (of type SDL_ratRegs*) to the base
 *                      address of the RAT module
 *
 * \param   destId     - The destination ID for future error messages
 */
int32_t SDL_RAT_setErrMsgDestId(SDL_ratRegs *pRatRegs, uint32_t destId);

 /**
 * \brief   This function is used to read the destination ID for error messages.
 *
 * \param   pRatRegs   - A pointer (of type SDL_ratRegs*) to the base
 *                        address of the RAT module
 *
 * \param   pDestId    - A pointer to hold the destination ID register value
 */
int32_t SDL_RAT_getErrMsgDestId(const SDL_ratRegs *pRatRegs, uint32_t *pDestId);
 
 
 /**
 * \brief   This function enables the exception logging capability. It also enables
 *      	exception interrupt generation. The module's exception interrupt signal
 *      	is asserted if the exception logging capability is enabled and the
 *      	exception interrupt source is enabled by calling the
 *
 * \param   pRatRegs       - A pointer (of type SDL_ratRegs*) to the base
 *                        address of the RAT module
 */
int32_t SDL_RAT_enableExceptionLog(SDL_ratRegs *pRatRegs);

 
 /**
 * \brief   This function disables the exception logging capability. It also
 *      	disables the module's exception interrupt signal generation, regardless
 *      	of whether the exception interrupt source is enabled or disabled.
 *
 * \param   pRatRegs       - A pointer (of type SDL_ratRegs*) to the base
 *                           address of the RAT module
 */
int32_t SDL_RAT_disableExceptionLog(SDL_ratRegs *pRatRegs);
 
 /**
 * \brief   This function returns the pending interrupt status of the specified
 *      	interrupt source.
 *
 * \param   pRatRegs       - A pointer (of type SDL_ratRegs*) to the base
 *                           address of the RAT module
 *
 * \param   intrSrc    - The interrupt source of type \ref SDL_RAT_intrSrcTypes
 */
int32_t SDL_RAT_isIntrPending(const SDL_ratRegs *pRatRegs, SDL_RAT_intrSrcTypes intrSrc);
 
 /**
 * \brief   This function is used to forceably set the pending status of the
 *      	specified interrupt source.
 *
 * \param   pRatRegs       - A pointer (of type SDL_ratRegs*) to the base
 *                           address of the RAT module
 *
 * \param   intrSrc    - The interrupt source of type \ref SDL_RAT_intrSrcTypes
 */
int32_t SDL_RAT_setIntrPending(SDL_ratRegs *pRatRegs, SDL_RAT_intrSrcTypes intrSrc);
 
 /**
 * \brief   This function is used to clear the pending status of the
 *      	specified interrupt source.
 *
 * \param   pRatRegs       - A pointer (of type SDL_ratRegs*) to the base
 *                           address of the RAT module
 *
 * \param   intrSrc    - The interrupt source of type \ref SDL_RAT_intrSrcTypes
 */
int32_t SDL_RAT_clrIntrPending(SDL_ratRegs *pRatRegs, SDL_RAT_intrSrcTypes intrSrc);
 
 /**
 * \brief   This function is used to acknowledge the specified interrupt source.
 *
 * \param   pRatRegs       - A pointer (of type SDL_ratRegs*) to the base
 *                           address of the RAT module
 *
 * \param   intrSrc    - The interrupt source of type \ref SDL_RAT_intrSrcTypes
 */
int32_t SDL_RAT_ackIntr(SDL_ratRegs *pRatRegs, SDL_RAT_intrSrcTypes intrSrc);
 
 /**
 * \brief   This function enables the specified interrupt source. It also calls
 *      	\ref SDL_RAT_enableExceptionLog to enable exception logging as there
 *      	is no valid reason to enable the exception interrupt source and not log
 *      	the exception information and assert the module's exception interrupt
 *      	signal. 
 *
 * \param   pRatRegs       - A pointer (of type SDL_ratRegs*) to the base
 *                           address of the RAT module
 *
 * \param   intrSrc    - The interrupt source of type \ref SDL_RAT_intrSrcTypes
 */
int32_t SDL_RAT_enableIntr(SDL_ratRegs *pRatRegs, SDL_RAT_intrSrcTypes intrSrc);
 
 /**
 * \brief   This function disables the specified interrupt source. This prevents
 *      	the module's exception interrupt signal from being asserted. Note that
 *      	software can still call \ref SDL_RAT_isIntrPending to poll for an
 *      	exception event and call \ref SDL_RAT_getExceptionInfo to read exception
 *      	event information if exception logging is enabled.
 *
 * \param   pRatRegs       - A pointer (of type SDL_ratRegs*) to the base
 *                           address of the RAT module
 *
 * \param   intrSrc    - The interrupt source of type \ref SDL_RAT_intrSrcTypes
 */
int32_t SDL_RAT_disableIntr(SDL_ratRegs *pRatRegs, SDL_RAT_intrSrcTypes intrSrc);
 
 /**
 * \brief   This function is used to get information for the last boundary crossing
 *      	transaction exception captured. The log is automatically cleared by this
 *      	function so that a future exception can be captured.
 *
 * \param   pRatRegs       - A pointer (of type SDL_ratRegs*) to the base
 *                           address of the RAT module
 *
 * \param   pExceptionInfo - A pointer to a SDL_ratExceptionInfo structure containing
 *                        returned exception information
 */
int32_t SDL_RAT_getExceptionInfo(SDL_ratRegs *pRatRegs, SDL_ratExceptionInfo *pExceptionInfo);
 
 /**
 * \brief   This function enables address translation for the specified region.
 *
 * \param   pRatRegs       - A pointer (of type SDL_ratRegs*) to the base
 *                           address of the RAT module
 *
 * \param   regionIndex    - The index of the region to configure. This value must
 *                           be in the range 0..(SDL_RAT_getMaxRegions()-1) inclusive.
 */
int32_t SDL_RAT_enableRegionTranslation(SDL_ratRegs *pRatRegs, uint32_t regionIndex);
 
 /**
 * \brief   This function disables address translation for the specified region.
 *
 * \param   pRatRegs       - A pointer (of type SDL_ratRegs*) to the base
 *                           address of the RAT module
 *
 * \param   regionIndex    - The index of the region to configure. This value must
 *                           be in the range 0..(SDL_RAT_getMaxRegions()-1) inclusive.
 */
int32_t SDL_RAT_disableRegionTranslation(SDL_ratRegs *pRatRegs, uint32_t regionIndex);
 
 /**
 * \brief   This function is used to configure a region address translation for the
 *      specified region index. The translation is automatically enabled after
 *      it is configured.
 *
 * \param   pRatRegs        - A pointer (of type SDL_ratRegs*) to the base
 *                            address of the RAT module
 *
 * \param   regionIndex     - The index of the region to configure. This value must
 *                            be in the range 0..(SDL_RAT_getMaxRegions()-1) inclusive.
 *
 * \param   pTranslationCfg - A pointer of type SDL_ratTranslationCfgInfo containing
 *                            configuration information
 */
int32_t SDL_RAT_configRegionTranslation(SDL_ratRegs *pRatRegs, uint32_t regionIndex,
                                        SDL_ratTranslationCfgInfo * pTranslationCfg);
 
 /**
 * \brief   This function is used to configure a region address translation for the
 *      specified region index. The translation is automatically enabled after
 *      it is configured.
 *
 * \param   pRatRegs        - A pointer (of type SDL_ratRegs*) to the base
 *                            address of the RAT module
 *
 * \param   regionIndex     - The index of the region to configure. This value must
 *                            be in the range 0..(SDL_RAT_getMaxRegions()-1) inclusive.
 *
 * \param   pTranslationCfg - A pointer of type SDL_ratTranslationCfgInfo containing
 *                            configuration information
 */
int32_t SDL_RAT_verifyConfigRegionTranslation(const SDL_ratRegs *pRatRegs, uint32_t regionIndex,
                                              SDL_ratTranslationCfgInfo *pTranslationCfg);

/**
 * \brief   This function returns the enable status of the specified region address
 *          translation
 *
 * \param   pRatRegs       - A pointer (of type SDL_ratRegs*) to the base
 *                           address of the RAT module
 *
 * \param   regionIndex    - The index of the region to configure. This value must
 *                            be in the range 0..(SDL_RAT_getMaxRegions()-1) inclusive. 
 *
 * \param   pStaticRegs    - A pointer to the static registers structure.
 */ 
int32_t SDL_RAT_getStaticRegs(SDL_ratRegs *pRatRegs, uint32_t regionIndex, SDL_ratStaticRegs * pStaticRegs);											  

/** @} */

#ifdef __cplusplus
}
#endif

#endif

