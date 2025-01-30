/*
 * SDL MPU
 *
 * Software Diagnostics Reference Test for MPU module
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

#include <stdint.h>
#include<main.h>
#include <sdl_mpu.h>

/* SDL MPU SelfTest local defines */

#define SDL_MPU_SELF_TEST_MEM_BASE  (0x41C7EFE0U)
#define SDL_MPU_SELF_TEST_MEM_SIZE  (32U)

/*********************************************************************
 * @fn      SDL_runMPUSelfTest
 *
 * @brief   Execute MPU Self test
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t SDL_runMPUSelfTest(void)
{
    int32_t  result;
    SDL_MPU_memConfig_t  triggerFaultMemRd, triggerFaultMemWr;
    uint32_t             loopCnt = 10U;
    int32_t              retVal=0;

    /* Initialize the MPU for the test */
    triggerFaultMemWr.pMemLocation  = (uint32_t *) SDL_MPU_SELF_TEST_MEM_BASE;
    triggerFaultMemWr.memAccessType = SDL_MPU_DATA_WRITE_ACCESS;
    triggerFaultMemWr.mpuRegionId   = 8;

    triggerFaultMemRd.pMemLocation  = (uint32_t *) SDL_MPU_SELF_TEST_MEM_BASE;
    triggerFaultMemRd.memAccessType = SDL_MPU_DATA_READ_ACCESS;
    triggerFaultMemRd.mpuRegionId   = 8;

    /* Initialize the memory for triggering MPU exception */
    sdlApp_print("\n MPU self test for Mem Write Access Fault: starting");   
    result = SDL_MPU_selfTest(&triggerFaultMemWr, loopCnt);

    if (result != SDL_PASS ) {
        sdlApp_print("\n MPU self test for Mem Write Access Fault failed");
        retVal = -1;
    } else {
        sdlApp_print("\n MPU self test for Mem Write Access Fault complete");
      
    }

    sdlApp_print("\n MPU self test for Mem Read Access Fault: starting");
    result = SDL_MPU_selfTest(&triggerFaultMemRd, loopCnt);

    if (result != SDL_PASS ) {
        sdlApp_print("\n MPU self test for Mem Read Access Fault failed");
        retVal = -1;
    } else {
        sdlApp_print("\n MPU self test for Mem Read Access Fault complete");
    }


    sdlApp_print("\n MPU self test for Mem Write Access Fault (Wait for long timeout): starting");
    result = SDL_MPU_selfTest(&triggerFaultMemWr, 0U);

    if (result != SDL_PASS ) {
        sdlApp_print("\n MPU self test for Mem Write Access Fault (Wait for long timeout) failed");
        retVal = -1;
    } else {
        sdlApp_print("\n MPU self test for Mem Write Access Fault  (Wait for long timeout) complete");
    }

    sdlApp_print("\n MPU self test for Mem Read Access Fault  (Wait for long timeout): starting");
    result = SDL_MPU_selfTest(&triggerFaultMemRd, 0U);
	
    if (result != SDL_PASS ) {
        sdlApp_print("\n MPU self test for Mem Read Access Fault (Wait for long timeout) failed");
        retVal = -1;
    } else {
        sdlApp_print("\n MPU self test for Mem Read Access Fault  (Wait for long timeout) complete");
    
    }

    return retVal;
}
