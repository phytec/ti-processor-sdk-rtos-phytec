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
 *   @file  sdl/pbist/soc/j721s2/sdl_pbist.h
 *
 *   @brief This file contains the SoC-specific SDL PBIST API's and
 *          definitions.
 *
 *   @defgroup SDL_PBIST_API Built-In Test (PBIST)
 *
 *   Provides the APIs for PBIST.
 */

#ifndef SDL_PBIST_SOC_H_
#define SDL_PBIST_SOC_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
@defgroup SDL_PBIST_FUNCTION  PBIST Functions
@ingroup SDL_PBIST_API
*/

/**
@defgroup SDL_PBIST_ENUM PBIST Enuerated Data Types
@ingroup SDL_PBIST_API
*/

/**
@defgroup SDL_PBIST_DATASTRUCT  PBIST Data Structures
@ingroup SDL_PBIST_API
*/

/**
 *  @addtogroup SDL_PBIST_ENUM
    @{
 *
 */

/**
 *  \brief PBIST instance
 *
 *  This enum defines the PBIST instances supported by the SDL_PBIST_selfTest API.
 */
typedef enum {
    /*!
     * VPAC Instance
     */
    SDL_PBIST_INST_VPAC_0,
    /*!
     * Main Infrastructure Instance 1
     */
    SDL_PBIST_INST_MAININFRA1,
    /*!
     * DMPAC Instance
     */
    SDL_PBIST_INST_DMPAC,
    /*!
     * Main Pulsar Instance 0
     */
    SDL_PBIST_INST_MAINR5F0,
    /*!
     * Main Pulsar Instance 1
     */
    SDL_PBIST_INST_MAINR5F1,
    /*!
     * Main Infrastructure Instance 0
     */
    SDL_PBIST_INST_MAININFRA0,
    /*!
     * HC Instance
     */
    SDL_PBIST_INST_HC,
    /*!
     * NAVSS Instance
     */
    SDL_PBIST_INST_NAVSS,
    /*!
     * Codec Instance
     */ 
    SDL_PBIST_INST_CODEC_1,
    /*!
     * GPU Instance
     */
    SDL_PBIST_INST_GPU,
    /*!
     * C7X_0 Instance
     */
    SDL_PBIST_INST_C7X_0,
    /*!
     * C7X_1 Instance
     */
    SDL_PBIST_INST_C7X_1,
    /*!
     * A72 Instance
     */
    SDL_PBIST_INST_A72,	
    /*
     * MSMC Instance
     */
    SDL_PBIST_INST_MSMC,
    /*!
     * MCU Pulsar Instance
     */	 
    SDL_PBIST_INST_MCUR5F0,
    /*!
     * MCU Instance 1
     */
    SDL_PBIST_INST_MCU_1,
    /*!
     * MCU Instance 0
     */
    SDL_PBIST_INST_MCU_0,
    /*!
     * DSS Instance
     */
    SDL_PBIST_INST_DSS,	
} SDL_PBIST_inst;

typedef enum {
    /*!
     * MCU Instance
     */
    SDL_PBIST_HWPOST_INST_MCU,
} SDL_PBIST_hwpostInst;

typedef enum {
    /*!
     * The HW POST PBIST completed and the test passed
     */
    SDL_PBIST_POST_COMPLETED_SUCCESS,
    /*!
     * The HW POST PBIST completed and the test failed
     */
    SDL_PBIST_POST_COMPLETED_FAILURE,
    /*!
     * The HW POST PBIST was run but timed out
     */
    SDL_PBIST_POST_TIMEOUT,
    /*!
     * The HW POST PBIST was not performed on this device
     */
    SDL_PBIST_POST_NOT_RUN
} SDL_PBIST_postStatus;

/** @} */

#define SDL_PBIST_NUM_INSTANCES 18U

#define SDL_PBIST_HWPOST_NUM_INSTANCES 1

/**
 *  @addtogroup SDL_PBIST_DATASTRUCT
    @{
 *
 */

typedef struct {
    /*!
     * HW POST PBIST status for MCU
     */
    SDL_PBIST_postStatus mcuPostStatus;
} SDL_PBIST_postResult;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* SDL_PBIST_SOC_H_ */
