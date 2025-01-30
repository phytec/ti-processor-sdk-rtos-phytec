//*****************************************************************************
//  Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//      Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//      Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the
//      distribution.
//
//      Neither the name of Texas Instruments Incorporated nor the names of
//      its contributors may be used to endorse or promote products derived
//      from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//*****************************************************************************

//*****************************************************************************
//  enable_cache_mmu.c
//
//  Description
//      This is the main program for DV running on ARM processors
//
//
//  Revision History:
//      14Jul15 - Removed address_defines.h
//      01Jan15 - Initial coding
//*****************************************************************************

/* ============================================================================
 * STANDARD INCLUDE FILES
 * =============================================================================
 */

/* ============================================================================
 * PROJECT SPECIFIC INCLUDE FILES
 * =============================================================================
 */
#include <stdint.h>
#include "enable_cache_mmu.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
   
   void enable_cache_mmu(uint64_t ttbr)
   {
#if !defined(_HOST_BUILD)
      uint64_t          tcr;
      uint64_t          mar;
      uint64_t          scr;
      
      tcr = 0x0000000000002a21U;
      scr = 0x80000000000000c1U;
      mar = 0x3D3D3D2915032A00U;

      DSPLIB_TEST_c7xSetTCR0(tcr);    //ECR785
      DSPLIB_TEST_c7xSetTBR0(ttbr);   //ECR787
      DSPLIB_TEST_c7xSetMAR(mar);     //ECR789
      DSPLIB_TEST_c7xSetSCR(scr);     //ECR784  *** This register has to be last ***
#endif
   }
   
#ifdef __cplusplus
}
#endif  // extern "C"
