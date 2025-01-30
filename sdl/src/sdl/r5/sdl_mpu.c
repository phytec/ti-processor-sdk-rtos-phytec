/*
 * SDL MPU
 *
 * Software Diagnostics Library module for MPU
 *
 *  Copyright (c) Texas Instruments Incorporated 2018-2021
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

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <sdl_types.h>
#include <sdl_common.h>
#include <sdl_mpu.h>
#include <sdl_exception.h>
#include <sdl_mpuCfg.h>

#include <soc.h>

#include "r5/sdl_r5_utils.h"

/** ---------------------------------------------------------------------------
 * @brief This enumerator defines the values for mpu self test flag
 * ----------------------------------------------------------------------------
 */
typedef enum {
    SDL_MPU_SELF_TEST_FLAG_NONE=0,
    SDL_MPU_SELF_TEST_FLAG_STARTING=0x55555555U,
    SDL_MPU_SELF_TEST_FLAG_TRIGGERED=0xAAAAAAAAU,

} SDL_mpuSelfTestFlag;

/** ---------------------------------------------------------------------------
 * @brief This structure defines the data used for tracking the current
 *        self test
 * ----------------------------------------------------------------------------
 */
typedef struct SDL_MpuSelfTest_s
{
    volatile SDL_mpuSelfTestFlag    mpuSelfTestState;
    /**< MPU self test state in progress */
    uint32_t               regId;
    /**< MPU self test memory configuration region ID */
    uint32_t               faultAddr;
    /**< MPU self test fault address */
    SDL_MPU_memAccessType  faultType;
    /**< MPU self test fault type */
}  SDL_MpuSelfTest_t;

/* static Global objects */
static SDL_MpuSelfTest_t gSDL_MPUSelfTest;

/* Global Objects */
/* None */

/* Local functions */
static void  SDL_trigger_fault(const SDL_MPU_memConfig_t *pMemConfig);
bool SDL_MPU_exceptionChkSelfTest(SDL_MPU_memAccessType faultType, uint32_t faultAddr);

static void  SDL_trigger_fault(const SDL_MPU_memConfig_t *pMemConfig)
{
    uint32_t      *pMemLocation = pMemConfig->pMemLocation;
    volatile       uint32_t       dummy = 1U;

    if (pMemConfig->memAccessType == SDL_MPU_DATA_WRITE_ACCESS) {
        /* write to the memory to trigger the exception */
        *pMemLocation = dummy;
    }
    else {
        /* read from the memory to trigger the exception */
        dummy = *pMemLocation;
    }
    return;
}

/* Global Functions */

/** ============================================================================*
 *
 * \brief   MPU SELF TEST Check Handler (NOT AN API for SDL MPU)
 *          corrects any mpu self test faults
 *
 * \return  true : if it is in self test; false otherwise
 */
bool SDL_MPU_exceptionChkSelfTest(SDL_MPU_memAccessType faultType, uint32_t faultAddr)
{
    bool    selfTestFlag = (bool)false;

    if ( (gSDL_MPUSelfTest.mpuSelfTestState == SDL_MPU_SELF_TEST_FLAG_STARTING) &&
         (gSDL_MPUSelfTest.faultAddr        == faultAddr) &&
         (gSDL_MPUSelfTest.faultType        == faultType) )
    {
        /* It is in self test flag */
        selfTestFlag = (bool)true;

        /* indicate self test exception trigger flag */
        gSDL_MPUSelfTest.mpuSelfTestState = SDL_MPU_SELF_TEST_FLAG_TRIGGERED;

        /* Provide the necessary privillage for the MPU exception to resolve */
        SDL_MPUResolve(gSDL_MPUSelfTest.faultAddr, gSDL_MPUSelfTest.regId);
    }
    return (selfTestFlag);
}

/** ============================================================================
 *
 * \brief   Runs self test by injecting a fault and monitor response
 *          Assumes MPU Settings from higher layer is already enabled.
 *
 * \param1  pMemConfig: pointer to memory configuration to be used for exception
 * \param2  loopCount: Loopcount for diagnostics to wait in a loop
 *
 * \return  SDL_PASS : Success; SDL_EFAIL for failures
 */
int32_t SDL_MPU_selfTest(const SDL_MPU_memConfig_t *pMemConfig,
                            uint32_t             loopCount)
{
    int32_t retVal = SDL_EFAIL;
    uint32_t   loopCnt = loopCount;
	uint64_t   waitForLong = 0x7FFFFFFFFFFFFFFFULL;

    /* Reset self test flag */
    gSDL_MPUSelfTest.mpuSelfTestState = SDL_MPU_SELF_TEST_FLAG_NONE;

    if ((pMemConfig != NULL) &&
        (pMemConfig->mpuRegionId < 16u)) {

        /* start self test for MPU */
        gSDL_MPUSelfTest.mpuSelfTestState = SDL_MPU_SELF_TEST_FLAG_STARTING;
        /* Copy the mpu memory configuration */
        gSDL_MPUSelfTest.regId  = pMemConfig->mpuRegionId;
        /* Record the mpu fault addresss */
        gSDL_MPUSelfTest.faultAddr = (uint32_t) (uintptr_t) pMemConfig->pMemLocation;
        /* Record the mpu fault type */
        gSDL_MPUSelfTest.faultType = pMemConfig->memAccessType;

        /* Add the MPU entry for the fault trigger */
        SDL_MPUCfgAddRegion(pMemConfig);

        /* trigger the mpu self test */
        SDL_trigger_fault(pMemConfig);

        /* wait until loopCount (> 0) for exception to trigger */
        if (loopCnt > 0U) {
            while(loopCnt != 0U)  {
                loopCnt--;
                /* If exception is triggered, self test is successful */
                if (gSDL_MPUSelfTest.mpuSelfTestState == SDL_MPU_SELF_TEST_FLAG_TRIGGERED)
                {
                    retVal = SDL_PASS;
                    break;
                }
            }
       } else { /* Wait forever configuration */
            while (waitForLong != 0U)  {
				waitForLong--;
                /* If exception is triggered, self test is successful */
                if (gSDL_MPUSelfTest.mpuSelfTestState == SDL_MPU_SELF_TEST_FLAG_TRIGGERED)
                {
                    retVal = SDL_PASS;
                    break;
                }
           }
       }
       /* Remove the MPU entry for the fault trigger */
       SDL_MPUCfgRemoveRegion(pMemConfig);

       /* Reset self test flag */
       gSDL_MPUSelfTest.mpuSelfTestState = SDL_MPU_SELF_TEST_FLAG_NONE;
    }
    return retVal;
}

/* Nothing past this point */
