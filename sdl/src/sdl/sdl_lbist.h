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
 *   @file  sdl/sdl_lbist.h
 *
 *   @brief This file contains the SDL LBIST API's.
 *
 *   @defgroup SDL_LBIST_API Logical Built-In Test (LBIST)
 *
 *   Provides the APIs for LBIST.
 */

#ifndef SDL_LBIST_H_
#define SDL_LBIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <src/ip/sdl_ip_lbist.h>
#if defined (SOC_J721E)
#include <src/sdl/lbist/soc/j721e/sdl_lbist.h>
#endif
#if defined (SOC_J7200)
#include <src/sdl/lbist/soc/j7200/sdl_lbist.h>
#endif
#if defined (SOC_J721S2)
#include <src/sdl/lbist/soc/j721s2/sdl_lbist.h>
#endif
#if defined (SOC_J784S4)
#include <src/sdl/lbist/soc/j784s4/sdl_lbist.h>
#endif
/**
@defgroup SDL_LBIST_API Logical Built-In Test (LBIST)
@ingroup SDL_MODULE
*/

/**
@defgroup SDL_LBIST_FUNCTION  LBIST Functions
@ingroup SDL_LBIST_API
*/

/**
@defgroup SDL_LBIST_ENUM LBIST Enumerated Data Types
@ingroup SDL_LBIST_API
*/

/**
 *  @addtogroup SDL_LBIST_ENUM
    @{
 *
 */

/**
 *  \brief LBIST self test type
 *
 *  This enum defines the test types supported by the SDL_LBIST_selfTest API.
 */
typedef enum {
    /*!
     * LBIST Test
     * Performs the LBIST test for a the instance specified.
     */
    SDL_LBIST_TEST,
    /*!
     * Release the LBIST core under test from Isolation
     * This should be called after running selfTest for SDL_LBIST_TEST and
     * then performing the sequence to switch off the cores
     */
    SDL_LBIST_TEST_RELEASE
} SDL_LBIST_testType;

/** @} */

/**
 *  @addtogroup SDL_LBIST_FUNCTION
    @{
 *
 */

/**
 *  \brief LBIST self test
 *
 *  This function executes an LBIST self test of the specified type for a
 *  specific LBIST instance. The test types supported are defined in
 *  SDL_LBIST_testType.
 *  LBIST is a feature that is used for self test of the logic circuitry
 *  in the SoC. Multiple instances may be supported.
 *  Note: The LBIST test is destructive to the cores/IP which are being
 *  tested. Therefore it should be executed from a different core than
 *  the one under test.
 *  It is important to note that execution of LBIST self-tests require
 *  preparation of the IPS under test by bringing them to a certain
 *  power and reset state before executing the test. The application
 *  must take care of this sequence before executing the self-test.
 *  The SDL examples give a reference for the necessary sequences.
 *  The LBIST instances supported are specific to the SoC, and are
 *  defined in the soc-specific header file.
 *
 *  \param instance         [IN]  LBIST instance
 *  \param testType         [IN]  LBIST test type to run.
 *  \param pResult          [OUT] Pointer to variable to indicate result
 *
 *  \return The SDL error code for the API.
 *                                 If pResult is NULL: SDL_EBADARGS
 *                                 If testType is invalid: SDL_EBADARGS
 *                                 If other error happened: SDL_EFAIL
 *                                 Success: SDL_PASS
 */
int32_t SDL_LBIST_selfTest(SDL_LBIST_inst instance, SDL_LBIST_testType testType,
                           bool *pResult);

/**
 *  \brief LBIST POST result status
 *
 *  This function retrieves the status of the LBIST POST test(s) for all
 *  the supported LBIST instances which have POST support on the device.
 *  LBIST is a feature that is used for self test of the logic circuitry
 *  in the SoC. Multiple instances may be supported.
 *
 *  If supported, HW POST may execute for one or more instances. This
 *  function will check if the test was executed, and if it was executed,
 *  what was the result of the test.
 *
 *  See the definition of SDL_LBIST_postResult for more information.
 *
 *  \param pResult          [OUT] Pointer to variable to indicate the result
 */
int32_t SDL_LBIST_getPOSTStatus(SDL_LBIST_postResult *pResult);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* SDL_LBIST_H_ */
