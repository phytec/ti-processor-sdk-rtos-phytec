/*
 * R5F-CPU Example Application
 *
 *
 *  Copyright (c) Texas Instruments Incorporated 2022
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

#ifndef _INCLUDE_SDL_PROFILE_H_
#define _INCLUDE_SDL_PROFILE_H_

#include <stdint.h>
#define SDL_PROFILE_ONESHOT 0
#define SDL_MPU_SELF_TEST_MEM_BASE  (0x41C7EFE0U)
#define SDL_MPU_SELF_TEST_MEM_SIZE  (32U)

/*********************************************************************
* @fn      SDL_profileInit
*
* @brief   Profile point init routine
*
* @param   maxNumProfiles  Max Number of profile points
*
* @return    0 : Success; < 0 for failures
*/
int32_t SDL_profileInit(uint32_t maxNumProfiles);

/*********************************************************************
* @fn      SDL_profileBegin
*
* @brief   Profile point begin routine
*
* @param   profileNumber  Profile number
*
* @return  None
*/
void SDL_profileBegin(uint32_t profileNumber);

/*********************************************************************
* @fn      SDL_profileEnd
*
* @brief   Profile point end routine
*
* @param   profileNumber  Profile number
*
* @return  None
*/    
void SDL_profileEnd(uint32_t profileNumber);

/*********************************************************************
* @fn      SDL_profileDelta
*
* @brief   calculate delta time
*
* @param   profileNumber  Profile number
*
* @return  Time delta value for profile number
*/
uint32_t SDL_profileDelta(uint32_t profileNumber);

extern void SDL_profileTimerInit(void);
extern uint32_t SDL_profileTimerRead(void);

#endif /* _INCLUDE_SDL_PROFILE_H_ */
