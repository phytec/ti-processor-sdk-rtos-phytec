/*******************************************************************************
**+--------------------------------------------------------------------------+**
**|                            ****                                          |**
**|                            ****                                          |**
**|                            ******o***                                    |**
**|                      ********_///_****                                   |**
**|                      ***** /_//_/ ****                                   |**
**|                       ** ** (__/ ****                                    |**
**|                           *********                                      |**
**|                            ****                                          |**
**|                            ***                                           |**
**|                                                                          |**
**|         Copyright (c) 2007-2017 Texas Instruments Incorporated           |**
**|                        ALL RIGHTS RESERVED                               |**
**|                                                                          |**
**| Permission to use, copy, modify, or distribute this software,            |**
**| whether in part or in whole, for any purpose is forbidden without        |**
**| a signed licensing agreement and NDA from Texas Instruments              |**
**| Incorporated (TI).                                                       |**
**|                                                                          |**
**| TI makes no representation or warranties with respect to the             |**
**| performance of this computer program, and specifically disclaims         |**
**| any responsibility for any damages, special or consequential,            |**
**| connected with the use of this program.                                  |**
**|                                                                          |**
**+--------------------------------------------------------------------------+**
*******************************************************************************/

#include "../TI_memory.h"

// TODO: find a better place for #defines
#define FFTLIB_DMC_L1DWBINV_WBINV_MASK  (0x00000001U)
#define FFTLIB_DMC_L1DWBINV_WBINV_SHIFT (0U)

#define FFTLIB_DMC_L1DINV_INV_MASK  (0x00000001U)
#define FFTLIB_DMC_L1DINV_INV_SHIFT (0U)

/* TODO: Configure DSP L1 and L2 cache registers, and call TI_meminit() */
/*       Check c6x implementation                                       */
int32_t TI_cache_init (void) { return (0); }

/* TODO: Invalidate caches */
int32_t TI_cache_inval (void) { return (0); }
#if !defined(_HOST_BUILD)
void FFTLIB_c7xCleaninvalidateL1DCache ()
{
   volatile uint64_t wbinv;
   wbinv = FFTLIB_c7xGetL1DWBINV () & ~FFTLIB_DMC_L1DWBINV_WBINV_MASK;
   wbinv |= (0x1U << FFTLIB_DMC_L1DWBINV_WBINV_SHIFT) &
            FFTLIB_DMC_L1DWBINV_WBINV_MASK;
   FFTLIB_c7xSetL1DWBINV (wbinv);
   return;
}

void FFTLIB_c7xInvalidateL1DCache ()
{
   volatile uint64_t wbinv;
   wbinv = FFTLIB_c7xGetL1DINV () & ~FFTLIB_DMC_L1DINV_INV_MASK;
   wbinv |= (0x1U << FFTLIB_DMC_L1DINV_INV_SHIFT) & FFTLIB_DMC_L1DINV_INV_MASK;
   FFTLIB_c7xSetL1DINV (wbinv);
   return;
}
#endif
