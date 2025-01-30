/*
 * SDL MPU
 *
 * Software Diagnostics Library module for MPU module
 *
 *  Copyright (c) Texas Instruments Incorporated 2019-2021
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
 * @file  sdl_mpu.h
 *
 * @brief
 *  Header file contains enumerations, structure definitions and function
 *  declarations for SDL MPU interface.
 *  ============================================================================
 */

#ifndef INCLUDE_SDL_MPU_H_
#define INCLUDE_SDL_MPU_H_

#include "sdl_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/** ===========================================================================
 *
 * @ingroup SDL_MODULE
 * @defgroup SDL_MPU_API SDL MPU API
 *
 * The SDL MPU module provides API for MPU Diagnostics
 *
 * ============================================================================
 */
/**
@defgroup SDL_MPU_DATASTRUCT  SDL MPU Data Structures
@ingroup SDL_MPU_API
*/
/**
@defgroup SDL_MPU_FUNCTION  SDL MPU Functions
@ingroup SDL_MPU_API
*/
/**
@defgroup SDL_MPU_ENUM SDL MPU Enumerated Data Types
@ingroup SDL_MPU_API
*/

/**
@defgroup SDL_MPU_MACROS SDL MPU Macro defines
@ingroup SDL_MPU_API
*/

/** ===========================================================================
 *  @addtogroup SDL_MPU_ENUM
    @{
 * ============================================================================
 */

/** ---------------------------------------------------------------------------
 * \brief This enumerator defines the type of Memory access to be used
 *        to trigger MPU exception
 * ----------------------------------------------------------------------------
 */
typedef enum {
    SDL_MPU_DATA_READ_ACCESS = 1,
    /**<  Use DATA Read access for MPU violation */
    SDL_MPU_DATA_WRITE_ACCESS = 2,
    /**<  Use DATA Write access for MPU violation */
} SDL_MPU_memAccessType;

/* @} */

/**
 *  \addtogroup SDL_MPU_MACROS
 *  @{
 */


/* @} */

/**
 *  \addtogroup SDL_MPU_DATASTRUCT
 *  @{
 */

/** ---------------------------------------------------------------------------
 * \brief MPU Data configuration
 *
 * This structure defines the elements of the MPU Data configuration
 * Please refer to ARM R5 Technical Reference Manual for details on MPU
 * The self test code assumes a free memory region ID to be provided which
 * configures the test memory location sub region to trigger the intended
 * MPU permission fault.
 * ----------------------------------------------------------------------------
 */
typedef struct SDL_MPU_memConfig_s
{
    uint32_t *pMemLocation;
    /**< Pointer to 16 byte aligned 32 byte memory block for self test
         example location as 0x41C7EFE0 */
    SDL_MPU_memAccessType memAccessType;
    /**< Memory access type to be used to generate exception */
    uint32_t  mpuRegionId;
    /**< MPU Region ID to be used for MPU Self Test */
} SDL_MPU_memConfig_t;

/* @} */

/**
 *  \addtogroup SDL_MPU_FUNCTION
 *  @{
 */

/** ============================================================================
 *
 * \brief   Self Test API for MPU module
 *         NOTE: Need to call SDL_MPU_init function before calling this API
 *
 * \param   pMemConfig: Pointer to memory configuration to be used for MPU
 *                        exception

 * \param   loopCount: Based on this count the diagnostics will wait in a loop
 *                     value of 0 indicates WAIT_FOREVER
 *
 * \return  SDL_RETURN_PASS : Success; SDL_RETURN_FAIL for failures
 */
int32_t SDL_MPU_selfTest (const SDL_MPU_memConfig_t *pMemConfig,
                             uint32_t loopCount);

/* @} */

#ifdef __cplusplus
}
#endif  /* extern "C" */

#endif /* INCLUDE_SDL_MPU_H_ */
