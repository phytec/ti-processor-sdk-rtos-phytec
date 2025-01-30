/*
 * Copyright (C) 2005-2017 Texas Instruments Incorporated.
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
/*
 *  ======== tistdtypes.h ========
 */

/*
 * These types are also defined by other TI components.  They are bracketed
 * with _TI_STD_TYPES to avoid warnings for duplicate definition.
 *
 * You may get warnings about duplicate type definitions when using this
 * header file with earlier versions of DSP/BIOS and CSL.
 *
 * You can use the '-pds303' compiler option to suppress these warnings.
 */
#ifndef _TI_STD_TYPES_ETH
#define _TI_STD_TYPES_ETH

/* If the CSL header files are getting included through the MAKEFILE builds we don't want
 * to include XDC TARGETS at all. */

#ifdef MAKEFILE_BUILD
#undef xdc_target_types__
#endif

#ifndef xdc_target_types__
/*
 * This '#ifndef STD_' is needed to protect from duplicate definitions
 * of Int, Uns, etc. in DSP/BIOS v4.x (e.g. 4.90, 4.80) since these versions
 * of DSP/BIOS or linux/include/ti/ipc/Std.h did not contain the
 * '#ifndef_TI_STD_TYPES' logic.
 */
#if !defined(STD_) && !defined(STD_H)

/* Define TRUE/FALSE to go with Bool */
#ifndef TRUE

#define TRUE		((boolean) 1)
#define FALSE		((boolean) 0)

#endif

/* Define CSL_TRUE/CSL_FALSE to go with Uint32 */
#ifndef CSL_TRUE
#define CSL_TRUE    (1U)
#define CSL_FALSE   (0U)
#endif

#endif /* STD_ */

#ifndef NULL
#define NULL (0)
#endif

#ifndef NULL_PTR
#if defined (HOST_EMULATION)
/* Host emulation compilation throws below error when void * is used. So
 * retain as normal 0 comparision.
 *
 * error: invalid conversion from 'void*' to 'xxx *' [-fpermissive]
 */
#define NULL_PTR (NULL)
#else
#define NULL_PTR ((void *)0)
#endif
#endif

#else
    #include <xdc/std.h>
#endif /* xdc_target_types__ */

#endif  /* _TI_STD_TYPES */

#ifndef TI_TYPES_SOC_ALIASES
#define TI_TYPES_SOC_ALIASES
    /* SOC Alias Defines for backward compatibility */
    #if defined(DEVICE_K2K)
    #ifndef SOC_K2K
    #define SOC_K2K
    #endif
    #elif defined(DEVICE_K2H)
    #ifndef SOC_K2H
    #define SOC_K2H
    #endif
    #elif defined(DEVICE_K2E)
    #ifndef SOC_K2E
    #define SOC_K2E
    #endif
    #elif defined(DEVICE_K2L)
    #ifndef SOC_K2L
    #define SOC_K2L
    #endif
    /* Temporary to be removed */
    #elif defined(DEVICE_K2G)
    #ifndef SOC_K2G
    #define SOC_K2G
    #endif
    #elif defined(DEVICE_AM574x)
    #ifndef SOC_AM574x
    #define SOC_AM574x
    #endif
    #elif defined(DEVICE_AM572x)
    #ifndef SOC_AM572x
    #define SOC_AM572x
    #endif
    #elif defined(DEVICE_AM571x)
    #ifndef SOC_AM571x
    #define SOC_AM571x
    #endif
    #elif defined(DEVICE_C6678)
    #ifndef SOC_C6678
    #define SOC_C6678
    #endif
    #elif defined(DEVICE_C6657)
    #ifndef SOC_C6657
    #define SOC_C6657
    #endif

    #endif /* DEVICE_XXXXX */
#endif
