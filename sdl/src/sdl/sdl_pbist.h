/********************************************************************
 * Copyright (C) 2021 Texas Instruments Incorporated.
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
 *   @file  sdl/sdl_pbist.h
 *
 *   @brief This file contains the SDL PBIST API's.
 *
 *   @defgroup SDL_PBIST_API Built-In Test (PBIST)
 *
 *   Provides the APIs for PBIST.
 */

#ifndef SDL_PBIST_H_
#define SDL_PBIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <src/ip/sdl_ip_pbist.h>
#if defined (SOC_J721E)
#include <src/sdl/pbist/soc/j721e/sdl_pbist.h>
#endif
#if defined (SOC_J7200)
#include <src/sdl/pbist/soc/j7200/sdl_pbist.h>
#endif
#if defined (SOC_J721S2)
#include <src/sdl/pbist/soc/j721s2/sdl_pbist.h>
#endif
#if defined (SOC_J784S4)
#include <src/sdl/pbist/soc/j784s4/sdl_pbist.h>
#endif

/**
@ingroup SDL_MODULE
@defgroup SDL_PBIST_API Built-In Test (PBIST)
*/

/**
@defgroup SDL_PBIST_FUNCTION  PBIST Functions
@ingroup SDL_PBIST_API
*/

/**
@defgroup SDL_PBIST_ENUM PBIST Enuerated Data Types
@ingroup SDL_PBIST_API
*/

/**
 *  @addtogroup SDL_PBIST_ENUM
    @{
 *
 */

/**
 *  \brief PBIST self test type
 *
 *  This enum defines the test types supported by the SDL_PBIST_selfTest API.
 */
typedef enum {
    /*!
     * PBIST Test
     * Performs the PBIST test for a the instance specified.
     * When a valid pConfig is passed, those values will be used to execute the
     * test. If pConfig is NULL, then the default "golden vectors" will be used
     * to execute the test.
     */
    SDL_PBIST_TEST,
    /*!
     * PBIST Negative Test
     * Performs the PBIST negative test to verify the PBIST logic for the
     * instance specified
     */
    SDL_PBIST_NEG_TEST,
     /*!
     * PBIST Test of ROM
     * Performs the PBIST test of ROM.
     */
    SDL_PBIST_TEST_OF_ROM

} SDL_PBIST_testType;

/** @} */

/**
 *  @addtogroup SDL_PBIST_FUNCTION
    @{
 *
 */

/**
 *  \brief PBIST self test
 *
 *  This function executes a PBIST self test of the specified type for a
 *  specific PBIST instance. The test types supported are defined in
 *  SDL_PBIST_testType.
 *  PBIST is a feature that is used for self test of the memory regions
 *  in the SoC. Multiple instances may be supported.
 *  Note: The PBIST test is destructive to the cores/IP which are being
 *  tested. Therefore it should be executed from a different core than
 *  the one under test.
 *  It is important to note that execution of PBIST self-tests require
 *  preparation of the IPS under test by bringing them to a certain
 *  power and reset state before executing the test. The application
 *  must take care of this sequence before executing the self-test.
 *  The SDL examples give a reference for the necessary sequences.
 *  The PBIST instances supported are specific to the SoC, and are
 *  defined in the soc-specific header file.
 *
 *  \param instance         [IN]  PBIST instance
 *  \param testType         [IN]  PBIST test type to run.
 *  \param pConfig          [IN]  Pointer to PBIST test configuration (optional)
 *  \param pResult          [OUT] Pointer to variable to indicate result
 *
 *  \return The SDL error code for the API.
 *                                 If pResult is NULL: SDL_EBADARGS
 *                                 If testType is invalid: SDL_EBADARGS
 *                                 If other error happened: SDL_EFAIL
 *                                 Success: SDL_PASS
 */
int32_t SDL_PBIST_selfTest(SDL_PBIST_inst instance, SDL_PBIST_testType testType,
                           const SDL_PBIST_config *pConfig, bool *pResult);

/**
 *  \brief PBIST POST result status
 *
 *  This function retrieves the status of the PBIST POST test(s) for all
 *  the supported PBIST instances which have POST support on the device.
 *  PBIST is a feature that is used for self test of the memory regions
 *  in the SoC. Multiple instances may be supported.
 *
 *  If supported, HW POST may execute for one or more instances. This
 *  function will check if the test was executed, and if it was executed,
 *  what was the result of the test.
 *
 *  See the definition of SDL_PBIST_postResult for more information.
 *
 *  \param pResult          [OUT] Pointer to variable to indicate the result
 */
int32_t SDL_PBIST_getPOSTStatus(SDL_PBIST_postResult *pResult);

/**
 *  \brief Application-provided API to give the PBIST module a clec output
 *         event to use upon request for a PBIST test.
 *
 *  \param clecOutputEvt    [OUT] Pointer to clecOuput event
 *
 *  \return The SDL error code for the API.
 *                                 If failed to give a CLEC output event: SDL_EFAIL
 *                                 Success: SDL_PASS
 */
int32_t SDL_OSAL_getClecOutputEvent(uint32_t *clecOutputEvt);

/**
 *  \brief Application-provided API to allow PBIST module to release a clec event
 *
 *  \param clecOutputEvt    [IN]  CLEC event to release
 *
 *  \return The SDL error code for the API.
 *                                 If failed to release: SDL_EFAIL
 *                                 Success: SDL_PASS
 */
int32_t SDL_OSAL_releaseClecOutputEvent(uint32_t clecOutputEvt);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* SDL_PBIST_H_ */
