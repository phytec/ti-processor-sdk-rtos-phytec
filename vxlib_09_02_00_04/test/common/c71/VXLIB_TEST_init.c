/******************************************************************************/
/*!
 * \file VXLIB_utility.c
 */
/* Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

/*******************************************************************************
 *
 * INCLUDES
 *
 ******************************************************************************/
#include "VXLIB_test.h"

#if !defined(_HOST_BUILD)
extern const uint64_t pte_lvl0[512];
#endif

void VXLIB_TEST_init()
{

#if __C7X_VEC_SIZE_BITS__ == 512
   // Known silicon bug, errata: TBD
   __sa_set_cr(0, __sa_get_cr(1));
#endif

#if !defined(_HOST_BUILD)
   // enable MMU
   enable_cache_mmu((uint64_t) pte_lvl0);

   // invalidate TLB
   invalidate_tlb();
#endif

#ifndef VXLIB_FIXED_SEED
   time_t t;
   time(&t);
   VXLIB_DEBUGPRINTFN(1, "rand seed = %u\n", (unsigned) t);
   srand((unsigned) t);
#else
   VXLIB_DEBUGPRINTFN(1, "fixed seed = %u\n", (unsigned) VXLIB_FIXED_SEED);
   srand((unsigned) VXLIB_FIXED_SEED);
#endif

#if !defined(DEMO)
#if defined(_HOST_BUILD)
#if defined(VXLIB_DEBUGPRINT)
   streaming_engine::set_debug_level(VXLIB_DEBUGPRINT);
   sa_generator::set_debug_level(VXLIB_DEBUGPRINT);
#endif
#endif
#endif
}
