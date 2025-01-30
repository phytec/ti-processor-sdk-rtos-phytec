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
 *   @file  sdl/lbist/soc/j721s2/sdl_lbist.h
 *
 *   @brief This file contains the SoC-specific SDL LBIST API's and
 *          definitions.
 *
 *   @defgroup SDL_LBIST_API Logical Built-In Test (LBIST)
 *
 *   Provides the APIs for LBIST.
 */

#ifndef SDL_LBIST_SOC_H_
#define SDL_LBIST_SOC_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
@defgroup SDL_LBIST_DATASTRUCT  LBIST Data Structures
@ingroup SDL_LBIST_API
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
 *  \brief LBIST instance
 *
 *  This enum defines the LBIST instances supported by the SDL_LBIST_selfTest API.
 */
typedef enum {
    /*!
     * Main Pulsar Instance 0
     */
    SDL_LBIST_INST_MAINR5F0,
    /*!
     * Main Pulsar Instance 1
     */
    SDL_LBIST_INST_MAINR5F1,
    /*!
     * C7X0 Instance
     */
    SDL_LBIST_INST_C7X0,
    /*!
     * C7X1 Instance
     */
    SDL_LBIST_INST_C7X1,
    /*!
     * VPAC0 Instance
     */
    SDL_LBIST_INST_VPAC0,
    /*!
     * DMPAC Instance
     */
    SDL_LBIST_INST_DMPAC,
    /*!
     * A72_0 Instance
     */
    SDL_LBIST_INST_A72,
} SDL_LBIST_inst;

typedef enum {
    /*!
     * MCU Instance
     */
    SDL_LBIST_HWPOST_INST_MCU,
    /*!
     * SMS Instance
     */
    SDL_LBIST_HWPOST_INST_SMS
} SDL_LBIST_hwpostInst;

typedef enum {
    /*!
     * The HW POST LBIST completed and the test passed
     */
    SDL_LBIST_POST_COMPLETED_SUCCESS,
    /*!
     * The HW POST LBIST completed and the test failed
     */
    SDL_LBIST_POST_COMPLETED_FAILURE,
    /*!
     * The HW POST LBIST was run but timed out
     */
    SDL_LBIST_POST_TIMEOUT,
    /*!
     * The HW POST LBIST was not performed on this device
     */
    SDL_LBIST_POST_NOT_RUN
} SDL_LBIST_postStatus;

/** @} */

#define SDL_LBIST_NUM_INSTANCES 7U

#define SDL_LBIST_HWPOST_NUM_INSTANCES 2U

/**
 *  @addtogroup SDL_LBIST_DATASTRUCT
    @{
 *
 */

typedef struct {
    /*!
     * HW POST LBIST status for MCU
     */
    SDL_LBIST_postStatus mcuPostStatus;
    /*!
     * HW POST LBIST status for SMS
     */
    SDL_LBIST_postStatus smsPostStatus;
} SDL_LBIST_postResult;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* SDL_LBIST_SOC_H_ */
