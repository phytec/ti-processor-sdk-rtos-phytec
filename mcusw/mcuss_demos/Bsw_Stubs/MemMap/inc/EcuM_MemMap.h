/*
*
* Copyright (c) 2024 Texas Instruments Incorporated
*
* All rights reserved not granted herein.
*
* Limited License.
*
* Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive
* license under copyrights and patents it now or hereafter owns or controls to make,
* have made, use, import, offer to sell and sell ("Utilize") this software subject to the
* terms herein.  With respect to the foregoing patent license, such license is granted
* solely to the extent that any such patent is necessary to Utilize the software alone.
* The patent license shall not apply to any combinations which include this software,
* other than combinations with devices manufactured by or for TI ("TI Devices").
* No hardware patent is licensed hereunder.
*
* Redistributions must preserve existing copyright notices and reproduce this license
* (including the above copyright notice and the disclaimer and (if applicable) source
* code license limitations below) in the documentation and/or other materials provided
* with the distribution
*
* Redistribution and use in binary form, without modification, are permitted provided
* that the following conditions are met:
*
* *       No reverse engineering, decompilation, or disassembly of this software is
* permitted with respect to any software provided in binary form.
*
* *       any redistribution and use are licensed by TI for use only with TI Devices.
*
* *       Nothing shall obligate TI to provide you with source code for the software
* licensed and provided to you in object code.
*
* If software source code is provided to you, modification and redistribution of the
* source code are permitted provided that the following conditions are met:
*
* *       any redistribution and use of the source code, including any resulting derivative
* works, are licensed by TI for use only with TI Devices.
*
* *       any redistribution and use of any object code compiled from the source code
* and any resulting derivative works, are licensed by TI for use only with TI Devices.
*
* Neither the name of Texas Instruments Incorporated nor the names of its suppliers
*
* may be used to endorse or promote products derived from this software without
* specific prior written permission.
*
* DISCLAIMER.
*
* THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

/**
 *  \file     EcuM_MemMap.h
 *
 *  \brief    This file contains memory map section definition
 *
 */


#ifdef __cplusplus
extern "C"{
#endif

/* Memory map */
#define MEMMAP_ERROR

/* Sections */
#ifdef ECUM_STOP_SEC_CODE
    #undef ECUM_STOP_SEC_CODE
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_CODE
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_CODE
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_CODE
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_CODE_FAST
    #undef ECUM_STOP_SEC_CODE_FAST
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_CODE_FAST
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_CODE_FAST
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_CODE_FAST
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_CODE_SLOW
    #undef ECUM_STOP_SEC_CODE_SLOW
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_CODE_SLOW
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_CODE_SLOW
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_CODE_SLOW
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_CALLOUT_CODE
    #undef ECUM_STOP_SEC_CALLOUT_CODE
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_CALLOUT_CODE
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_CALLOUT_CODE
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_CALLOUT_CODE
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_NO_INIT_BOOLEAN
    #undef ECUM_STOP_SEC_VAR_NO_INIT_BOOLEAN
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_NO_INIT_BOOLEAN
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_NO_INIT_BOOLEAN
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_NO_INIT_BOOLEAN
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_NO_INIT_8
    #undef ECUM_STOP_SEC_VAR_NO_INIT_8
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_NO_INIT_8
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_NO_INIT_8
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_NO_INIT_8
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_NO_INIT_16
    #undef ECUM_STOP_SEC_VAR_NO_INIT_16
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_NO_INIT_16
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_NO_INIT_16
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_NO_INIT_16
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_NO_INIT_32
    #undef ECUM_STOP_SEC_VAR_NO_INIT_32
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_NO_INIT_32
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_NO_INIT_32
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_NO_INIT_32
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#if !defined (AUTOSAR_421)
#elif defined ECUM_STOP_SEC_VAR_NO_INIT_PTR
    #undef ECUM_STOP_SEC_VAR_NO_INIT_PTR
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_NO_INIT_PTR
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_NO_INIT_PTR
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_NO_INIT_PTR
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#endif /* (AUTOSAR_421) */
#elif defined ECUM_STOP_SEC_VAR_NO_INIT_UNSPECIFIED
    #undef ECUM_STOP_SEC_VAR_NO_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_NO_INIT_UNSPECIFIED
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_NO_INIT_UNSPECIFIED
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_NO_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_CLEARED_BOOLEAN
    #undef ECUM_STOP_SEC_VAR_CLEARED_BOOLEAN
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_CLEARED_BOOLEAN
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_CLEARED_BOOLEAN
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_CLEARED_BOOLEAN
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_CLEARED_8
    #undef ECUM_STOP_SEC_VAR_CLEARED_8
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_CLEARED_8
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_CLEARED_8
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_CLEARED_8
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_CLEARED_16
    #undef ECUM_STOP_SEC_VAR_CLEARED_16
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_CLEARED_16
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_CLEARED_16
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_CLEARED_16
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_CLEARED_32
    #undef ECUM_STOP_SEC_VAR_CLEARED_32
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_CLEARED_32
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_CLEARED_32
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_CLEARED_32
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#if !defined (AUTOSAR_421)
#elif defined ECUM_STOP_SEC_VAR_CLEARED_PTR
    #undef ECUM_STOP_SEC_VAR_CLEARED_PTR
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_CLEARED_PTR
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_CLEARED_PTR
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_CLEARED_PTR
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#endif /* (AUTOSAR_421) */
#elif defined ECUM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
    #undef ECUM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_CLEARED_UNSPECIFIED
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_CLEARED_UNSPECIFIED
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_CLEARED_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_POWER_ON_CLEARED_BOOLEAN
    #undef ECUM_STOP_SEC_VAR_POWER_ON_CLEARED_BOOLEAN
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_POWER_ON_CLEARED_BOOLEAN
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_POWER_ON_CLEARED_BOOLEAN
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_POWER_ON_CLEARED_BOOLEAN
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_POWER_ON_CLEARED_8
    #undef ECUM_STOP_SEC_VAR_POWER_ON_CLEARED_8
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_POWER_ON_CLEARED_8
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_POWER_ON_CLEARED_8
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_POWER_ON_CLEARED_8
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_POWER_ON_CLEARED_16
    #undef ECUM_STOP_SEC_VAR_POWER_ON_CLEARED_16
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_POWER_ON_CLEARED_16
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_POWER_ON_CLEARED_16
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_POWER_ON_CLEARED_16
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_POWER_ON_CLEARED_32
    #undef ECUM_STOP_SEC_VAR_POWER_ON_CLEARED_32
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_POWER_ON_CLEARED_32
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_POWER_ON_CLEARED_32
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_POWER_ON_CLEARED_32
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#if !defined (AUTOSAR_421)
#elif defined ECUM_STOP_SEC_VAR_POWER_ON_CLEARED_PTR
    #undef ECUM_STOP_SEC_VAR_POWER_ON_CLEARED_PTR
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_POWER_ON_CLEARED_PTR
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_POWER_ON_CLEARED_PTR
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_POWER_ON_CLEARED_PTR
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#endif /* (AUTOSAR_421) */
#elif defined ECUM_STOP_SEC_VAR_POWER_ON_CLEARED_UNSPECIFIED
    #undef ECUM_STOP_SEC_VAR_POWER_ON_CLEARED_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_POWER_ON_CLEARED_UNSPECIFIED
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_POWER_ON_CLEARED_UNSPECIFIED
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_POWER_ON_CLEARED_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_INIT_BOOLEAN
    #undef ECUM_STOP_SEC_VAR_INIT_BOOLEAN
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_INIT_BOOLEAN
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_INIT_BOOLEAN
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_INIT_BOOLEAN
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_INIT_8
    #undef ECUM_STOP_SEC_VAR_INIT_8
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_INIT_8
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_INIT_8
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_INIT_8
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_INIT_16
    #undef ECUM_STOP_SEC_VAR_INIT_16
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_INIT_16
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_INIT_16
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_INIT_16
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_INIT_32
    #undef ECUM_STOP_SEC_VAR_INIT_32
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_INIT_32
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_INIT_32
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_INIT_32
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#if !defined (AUTOSAR_421)
#elif defined ECUM_STOP_SEC_VAR_INIT_PTR
    #undef ECUM_STOP_SEC_VAR_INIT_PTR
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_INIT_PTR
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_INIT_PTR
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_INIT_PTR
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#endif /* (AUTOSAR_421) */
#elif defined ECUM_STOP_SEC_VAR_INIT_UNSPECIFIED
    #undef ECUM_STOP_SEC_VAR_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_INIT_UNSPECIFIED
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_INIT_UNSPECIFIED
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_POWER_ON_INIT_BOOLEAN
    #undef ECUM_STOP_SEC_VAR_POWER_ON_INIT_BOOLEAN
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_POWER_ON_INIT_BOOLEAN
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_POWER_ON_INIT_BOOLEAN
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_POWER_ON_INIT_BOOLEAN
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_POWER_ON_INIT_8
    #undef ECUM_STOP_SEC_VAR_POWER_ON_INIT_8
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_POWER_ON_INIT_8
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_POWER_ON_INIT_8
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_POWER_ON_INIT_8
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_POWER_ON_INIT_16
    #undef ECUM_STOP_SEC_VAR_POWER_ON_INIT_16
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_POWER_ON_INIT_16
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_POWER_ON_INIT_16
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_POWER_ON_INIT_16
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_POWER_ON_INIT_32
    #undef ECUM_STOP_SEC_VAR_POWER_ON_INIT_32
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_POWER_ON_INIT_32
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_POWER_ON_INIT_32
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_POWER_ON_INIT_32
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#if !defined (AUTOSAR_421)
#elif defined ECUM_STOP_SEC_VAR_POWER_ON_INIT_PTR
    #undef ECUM_STOP_SEC_VAR_POWER_ON_INIT_PTR
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_POWER_ON_INIT_PTR
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_POWER_ON_INIT_PTR
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_POWER_ON_INIT_PTR
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#endif /* (AUTOSAR_421) */
#elif defined ECUM_STOP_SEC_VAR_POWER_ON_INIT_UNSPECIFIED
    #undef ECUM_STOP_SEC_VAR_POWER_ON_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_POWER_ON_INIT_UNSPECIFIED
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_POWER_ON_INIT_UNSPECIFIED
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_POWER_ON_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_FAST_NO_INIT_BOOLEAN
    #undef ECUM_STOP_SEC_VAR_FAST_NO_INIT_BOOLEAN
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_NO_INIT_BOOLEAN
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_NO_INIT_BOOLEAN
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_NO_INIT_BOOLEAN
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_FAST_NO_INIT_8
    #undef ECUM_STOP_SEC_VAR_FAST_NO_INIT_8
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_NO_INIT_8
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_NO_INIT_8
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_NO_INIT_8
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_FAST_NO_INIT_16
    #undef ECUM_STOP_SEC_VAR_FAST_NO_INIT_16
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_NO_INIT_16
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_NO_INIT_16
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_NO_INIT_16
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_FAST_NO_INIT_32
    #undef ECUM_STOP_SEC_VAR_FAST_NO_INIT_32
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_NO_INIT_32
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_NO_INIT_32
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_NO_INIT_32
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#if !defined (AUTOSAR_421)
#elif defined ECUM_STOP_SEC_VAR_FAST_NO_INIT_PTR
    #undef ECUM_STOP_SEC_VAR_FAST_NO_INIT_PTR
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_NO_INIT_PTR
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_NO_INIT_PTR
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_NO_INIT_PTR
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#endif /* (AUTOSAR_421) */
#elif defined ECUM_STOP_SEC_VAR_FAST_NO_INIT_UNSPECIFIED
    #undef ECUM_STOP_SEC_VAR_FAST_NO_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_NO_INIT_UNSPECIFIED
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_NO_INIT_UNSPECIFIED
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_NO_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_FAST_CLEARED_BOOLEAN
    #undef ECUM_STOP_SEC_VAR_FAST_CLEARED_BOOLEAN
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_CLEARED_BOOLEAN
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_CLEARED_BOOLEAN
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_CLEARED_BOOLEAN
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_FAST_CLEARED_8
    #undef ECUM_STOP_SEC_VAR_FAST_CLEARED_8
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_CLEARED_8
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_CLEARED_8
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_CLEARED_8
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_FAST_CLEARED_16
    #undef ECUM_STOP_SEC_VAR_FAST_CLEARED_16
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_CLEARED_16
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_CLEARED_16
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_CLEARED_16
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_FAST_CLEARED_32
    #undef ECUM_STOP_SEC_VAR_FAST_CLEARED_32
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_CLEARED_32
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_CLEARED_32
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_CLEARED_32
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#if !defined (AUTOSAR_421)
#elif defined ECUM_STOP_SEC_VAR_FAST_CLEARED_PTR
    #undef ECUM_STOP_SEC_VAR_FAST_CLEARED_PTR
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_CLEARED_PTR
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_CLEARED_PTR
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_CLEARED_PTR
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#endif /* (AUTOSAR_421) */
#elif defined ECUM_STOP_SEC_VAR_FAST_CLEARED_UNSPECIFIED
    #undef ECUM_STOP_SEC_VAR_FAST_CLEARED_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_CLEARED_UNSPECIFIED
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_CLEARED_UNSPECIFIED
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_CLEARED_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_FAST_POWER_ON_CLEARED_BOOLEAN
    #undef ECUM_STOP_SEC_VAR_FAST_POWER_ON_CLEARED_BOOLEAN
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_POWER_ON_CLEARED_BOOLEAN
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_POWER_ON_CLEARED_BOOLEAN
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_POWER_ON_CLEARED_BOOLEAN
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_FAST_POWER_ON_CLEARED_8
    #undef ECUM_STOP_SEC_VAR_FAST_POWER_ON_CLEARED_8
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_POWER_ON_CLEARED_8
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_POWER_ON_CLEARED_8
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_POWER_ON_CLEARED_8
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_FAST_POWER_ON_CLEARED_16
    #undef ECUM_STOP_SEC_VAR_FAST_POWER_ON_CLEARED_16
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_POWER_ON_CLEARED_16
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_POWER_ON_CLEARED_16
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_POWER_ON_CLEARED_16
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_FAST_POWER_ON_CLEARED_32
    #undef ECUM_STOP_SEC_VAR_FAST_POWER_ON_CLEARED_32
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_POWER_ON_CLEARED_32
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_POWER_ON_CLEARED_32
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_POWER_ON_CLEARED_32
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#if !defined (AUTOSAR_421)
#elif defined ECUM_STOP_SEC_VAR_FAST_POWER_ON_CLEARED_PTR
    #undef ECUM_STOP_SEC_VAR_FAST_POWER_ON_CLEARED_PTR
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_POWER_ON_CLEARED_PTR
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_POWER_ON_CLEARED_PTR
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_POWER_ON_CLEARED_PTR
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#endif /* (AUTOSAR_421) */
#elif defined ECUM_STOP_SEC_VAR_FAST_POWER_ON_CLEARED_UNSPECIFIED
    #undef ECUM_STOP_SEC_VAR_FAST_POWER_ON_CLEARED_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_POWER_ON_CLEARED_UNSPECIFIED
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_POWER_ON_CLEARED_UNSPECIFIED
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_POWER_ON_CLEARED_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_FAST_INIT_BOOLEAN
    #undef ECUM_STOP_SEC_VAR_FAST_INIT_BOOLEAN
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_INIT_BOOLEAN
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_INIT_BOOLEAN
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_INIT_BOOLEAN
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_FAST_INIT_8
    #undef ECUM_STOP_SEC_VAR_FAST_INIT_8
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_INIT_8
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_INIT_8
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_INIT_8
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_FAST_INIT_16
    #undef ECUM_STOP_SEC_VAR_FAST_INIT_16
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_INIT_16
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_INIT_16
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_INIT_16
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_FAST_INIT_32
    #undef ECUM_STOP_SEC_VAR_FAST_INIT_32
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_INIT_32
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_INIT_32
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_INIT_32
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#if !defined (AUTOSAR_421)
#elif defined ECUM_STOP_SEC_VAR_FAST_INIT_PTR
    #undef ECUM_STOP_SEC_VAR_FAST_INIT_PTR
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_INIT_PTR
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_INIT_PTR
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_INIT_PTR
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#endif /* (AUTOSAR_421) */
#elif defined ECUM_STOP_SEC_VAR_FAST_INIT_UNSPECIFIED
    #undef ECUM_STOP_SEC_VAR_FAST_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_INIT_UNSPECIFIED
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_INIT_UNSPECIFIED
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_FAST_POWER_ON_INIT_BOOLEAN
    #undef ECUM_STOP_SEC_VAR_FAST_POWER_ON_INIT_BOOLEAN
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_POWER_ON_INIT_BOOLEAN
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_POWER_ON_INIT_BOOLEAN
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_POWER_ON_INIT_BOOLEAN
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_FAST_POWER_ON_INIT_8
    #undef ECUM_STOP_SEC_VAR_FAST_POWER_ON_INIT_8
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_POWER_ON_INIT_8
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_POWER_ON_INIT_8
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_POWER_ON_INIT_8
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_FAST_POWER_ON_INIT_16
    #undef ECUM_STOP_SEC_VAR_FAST_POWER_ON_INIT_16
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_POWER_ON_INIT_16
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_POWER_ON_INIT_16
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_POWER_ON_INIT_16
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_FAST_POWER_ON_INIT_32
    #undef ECUM_STOP_SEC_VAR_FAST_POWER_ON_INIT_32
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_POWER_ON_INIT_32
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_POWER_ON_INIT_32
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_POWER_ON_INIT_32
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#if !defined (AUTOSAR_421)
#elif defined ECUM_STOP_SEC_VAR_FAST_POWER_ON_INIT_PTR
    #undef ECUM_STOP_SEC_VAR_FAST_POWER_ON_INIT_PTR
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_POWER_ON_INIT_PTR
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_POWER_ON_INIT_PTR
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_POWER_ON_INIT_PTR
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#endif /* (AUTOSAR_421) */
#elif defined ECUM_STOP_SEC_VAR_FAST_POWER_ON_INIT_UNSPECIFIED
    #undef ECUM_STOP_SEC_VAR_FAST_POWER_ON_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_FAST_POWER_ON_INIT_UNSPECIFIED
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_FAST_POWER_ON_INIT_UNSPECIFIED
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_FAST_POWER_ON_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_SLOW_NO_INIT_BOOLEAN
    #undef ECUM_STOP_SEC_VAR_SLOW_NO_INIT_BOOLEAN
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_NO_INIT_BOOLEAN
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_NO_INIT_BOOLEAN
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_NO_INIT_BOOLEAN
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_SLOW_NO_INIT_8
    #undef ECUM_STOP_SEC_VAR_SLOW_NO_INIT_8
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_NO_INIT_8
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_NO_INIT_8
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_NO_INIT_8
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_SLOW_NO_INIT_16
    #undef ECUM_STOP_SEC_VAR_SLOW_NO_INIT_16
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_NO_INIT_16
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_NO_INIT_16
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_NO_INIT_16
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_SLOW_NO_INIT_32
    #undef ECUM_STOP_SEC_VAR_SLOW_NO_INIT_32
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_NO_INIT_32
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_NO_INIT_32
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_NO_INIT_32
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#if !defined (AUTOSAR_421)
#elif defined ECUM_STOP_SEC_VAR_SLOW_NO_INIT_PTR
    #undef ECUM_STOP_SEC_VAR_SLOW_NO_INIT_PTR
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_NO_INIT_PTR
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_NO_INIT_PTR
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_NO_INIT_PTR
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#endif /* (AUTOSAR_421) */
#elif defined ECUM_STOP_SEC_VAR_SLOW_NO_INIT_UNSPECIFIED
    #undef ECUM_STOP_SEC_VAR_SLOW_NO_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_NO_INIT_UNSPECIFIED
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_NO_INIT_UNSPECIFIED
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_NO_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_SLOW_CLEARED_BOOLEAN
    #undef ECUM_STOP_SEC_VAR_SLOW_CLEARED_BOOLEAN
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_CLEARED_BOOLEAN
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_CLEARED_BOOLEAN
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_CLEARED_BOOLEAN
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_SLOW_CLEARED_8
    #undef ECUM_STOP_SEC_VAR_SLOW_CLEARED_8
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_CLEARED_8
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_CLEARED_8
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_CLEARED_8
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_SLOW_CLEARED_16
    #undef ECUM_STOP_SEC_VAR_SLOW_CLEARED_16
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_CLEARED_16
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_CLEARED_16
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_CLEARED_16
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_SLOW_CLEARED_32
    #undef ECUM_STOP_SEC_VAR_SLOW_CLEARED_32
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_CLEARED_32
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_CLEARED_32
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_CLEARED_32
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#if !defined (AUTOSAR_421)
#elif defined ECUM_STOP_SEC_VAR_SLOW_CLEARED_PTR
    #undef ECUM_STOP_SEC_VAR_SLOW_CLEARED_PTR
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_CLEARED_PTR
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_CLEARED_PTR
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_CLEARED_PTR
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#endif /* (AUTOSAR_421) */
#elif defined ECUM_STOP_SEC_VAR_SLOW_CLEARED_UNSPECIFIED
    #undef ECUM_STOP_SEC_VAR_SLOW_CLEARED_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_CLEARED_UNSPECIFIED
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_CLEARED_UNSPECIFIED
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_CLEARED_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_SLOW_POWER_ON_CLEARED_BOOLEAN
    #undef ECUM_STOP_SEC_VAR_SLOW_POWER_ON_CLEARED_BOOLEAN
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_POWER_ON_CLEARED_BOOLEAN
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_POWER_ON_CLEARED_BOOLEAN
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_POWER_ON_CLEARED_BOOLEAN
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_SLOW_POWER_ON_CLEARED_8
    #undef ECUM_STOP_SEC_VAR_SLOW_POWER_ON_CLEARED_8
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_POWER_ON_CLEARED_8
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_POWER_ON_CLEARED_8
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_POWER_ON_CLEARED_8
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_SLOW_POWER_ON_CLEARED_16
    #undef ECUM_STOP_SEC_VAR_SLOW_POWER_ON_CLEARED_16
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_POWER_ON_CLEARED_16
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_POWER_ON_CLEARED_16
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_POWER_ON_CLEARED_16
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_SLOW_POWER_ON_CLEARED_32
    #undef ECUM_STOP_SEC_VAR_SLOW_POWER_ON_CLEARED_32
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_POWER_ON_CLEARED_32
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_POWER_ON_CLEARED_32
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_POWER_ON_CLEARED_32
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#if !defined (AUTOSAR_421)
#elif defined ECUM_STOP_SEC_VAR_SLOW_POWER_ON_CLEARED_PTR
    #undef ECUM_STOP_SEC_VAR_SLOW_POWER_ON_CLEARED_PTR
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_POWER_ON_CLEARED_PTR
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_POWER_ON_CLEARED_PTR
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_POWER_ON_CLEARED_PTR
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#endif /* (AUTOSAR_421) */
#elif defined ECUM_STOP_SEC_VAR_SLOW_POWER_ON_CLEARED_UNSPECIFIED
    #undef ECUM_STOP_SEC_VAR_SLOW_POWER_ON_CLEARED_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_POWER_ON_CLEARED_UNSPECIFIED
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_POWER_ON_CLEARED_UNSPECIFIED
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_POWER_ON_CLEARED_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_SLOW_INIT_BOOLEAN
    #undef ECUM_STOP_SEC_VAR_SLOW_INIT_BOOLEAN
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_INIT_BOOLEAN
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_INIT_BOOLEAN
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_INIT_BOOLEAN
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_SLOW_INIT_8
    #undef ECUM_STOP_SEC_VAR_SLOW_INIT_8
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_INIT_8
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_INIT_8
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_INIT_8
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_SLOW_INIT_16
    #undef ECUM_STOP_SEC_VAR_SLOW_INIT_16
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_INIT_16
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_INIT_16
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_INIT_16
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_SLOW_INIT_32
    #undef ECUM_STOP_SEC_VAR_SLOW_INIT_32
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_INIT_32
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_INIT_32
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_INIT_32
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#if !defined (AUTOSAR_421)
#elif defined ECUM_STOP_SEC_VAR_SLOW_INIT_PTR
    #undef ECUM_STOP_SEC_VAR_SLOW_INIT_PTR
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_INIT_PTR
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_INIT_PTR
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_INIT_PTR
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#endif /* (AUTOSAR_421) */
#elif defined ECUM_STOP_SEC_VAR_SLOW_INIT_UNSPECIFIED
    #undef ECUM_STOP_SEC_VAR_SLOW_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_INIT_UNSPECIFIED
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_INIT_UNSPECIFIED
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_SLOW_POWER_ON_INIT_BOOLEAN
    #undef ECUM_STOP_SEC_VAR_SLOW_POWER_ON_INIT_BOOLEAN
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_POWER_ON_INIT_BOOLEAN
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_POWER_ON_INIT_BOOLEAN
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_POWER_ON_INIT_BOOLEAN
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_SLOW_POWER_ON_INIT_8
    #undef ECUM_STOP_SEC_VAR_SLOW_POWER_ON_INIT_8
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_POWER_ON_INIT_8
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_POWER_ON_INIT_8
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_POWER_ON_INIT_8
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_SLOW_POWER_ON_INIT_16
    #undef ECUM_STOP_SEC_VAR_SLOW_POWER_ON_INIT_16
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_POWER_ON_INIT_16
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_POWER_ON_INIT_16
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_POWER_ON_INIT_16
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_VAR_SLOW_POWER_ON_INIT_32
    #undef ECUM_STOP_SEC_VAR_SLOW_POWER_ON_INIT_32
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_POWER_ON_INIT_32
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_POWER_ON_INIT_32
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_POWER_ON_INIT_32
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#if !defined (AUTOSAR_421)
#elif defined ECUM_STOP_SEC_VAR_SLOW_POWER_ON_INIT_PTR
    #undef ECUM_STOP_SEC_VAR_SLOW_POWER_ON_INIT_PTR
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_POWER_ON_INIT_PTR
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_POWER_ON_INIT_PTR
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_POWER_ON_INIT_PTR
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#endif /* (AUTOSAR_421) */
#elif defined ECUM_STOP_SEC_VAR_SLOW_POWER_ON_INIT_UNSPECIFIED
    #undef ECUM_STOP_SEC_VAR_SLOW_POWER_ON_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_VAR_SLOW_POWER_ON_INIT_UNSPECIFIED
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_VAR_SLOW_POWER_ON_INIT_UNSPECIFIED
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_VAR_SLOW_POWER_ON_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_NO_INIT_BOOLEAN
    #undef ECUM_STOP_SEC_INTERNAL_VAR_NO_INIT_BOOLEAN
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_NO_INIT_BOOLEAN
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_NO_INIT_BOOLEAN
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_NO_INIT_BOOLEAN
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_NO_INIT_8
    #undef ECUM_STOP_SEC_INTERNAL_VAR_NO_INIT_8
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_NO_INIT_8
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_NO_INIT_8
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_NO_INIT_8
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_NO_INIT_16
    #undef ECUM_STOP_SEC_INTERNAL_VAR_NO_INIT_16
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_NO_INIT_16
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_NO_INIT_16
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_NO_INIT_16
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_NO_INIT_32
    #undef ECUM_STOP_SEC_INTERNAL_VAR_NO_INIT_32
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_NO_INIT_32
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_NO_INIT_32
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_NO_INIT_32
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#if !defined (AUTOSAR_421)
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_NO_INIT_PTR
    #undef ECUM_STOP_SEC_INTERNAL_VAR_NO_INIT_PTR
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_NO_INIT_PTR
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_NO_INIT_PTR
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_NO_INIT_PTR
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#endif /* (AUTOSAR_421) */
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_NO_INIT_UNSPECIFIED
    #undef ECUM_STOP_SEC_INTERNAL_VAR_NO_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_NO_INIT_UNSPECIFIED
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_NO_INIT_UNSPECIFIED
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_NO_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_CLEARED_BOOLEAN
    #undef ECUM_STOP_SEC_INTERNAL_VAR_CLEARED_BOOLEAN
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_CLEARED_BOOLEAN
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_CLEARED_BOOLEAN
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_CLEARED_BOOLEAN
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_CLEARED_8
    #undef ECUM_STOP_SEC_INTERNAL_VAR_CLEARED_8
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_CLEARED_8
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_CLEARED_8
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_CLEARED_8
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_CLEARED_16
    #undef ECUM_STOP_SEC_INTERNAL_VAR_CLEARED_16
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_CLEARED_16
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_CLEARED_16
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_CLEARED_16
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_CLEARED_32
    #undef ECUM_STOP_SEC_INTERNAL_VAR_CLEARED_32
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_CLEARED_32
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_CLEARED_32
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_CLEARED_32
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#if !defined (AUTOSAR_421)
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_CLEARED_PTR
    #undef ECUM_STOP_SEC_INTERNAL_VAR_CLEARED_PTR
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_CLEARED_PTR
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_CLEARED_PTR
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_CLEARED_PTR
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#endif /* (AUTOSAR_421) */
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_CLEARED_UNSPECIFIED
    #undef ECUM_STOP_SEC_INTERNAL_VAR_CLEARED_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_CLEARED_UNSPECIFIED
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_CLEARED_UNSPECIFIED
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_CLEARED_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_POWER_ON_CLEARED_BOOLEAN
    #undef ECUM_STOP_SEC_INTERNAL_VAR_POWER_ON_CLEARED_BOOLEAN
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_POWER_ON_CLEARED_BOOLEAN
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_POWER_ON_CLEARED_BOOLEAN
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_POWER_ON_CLEARED_BOOLEAN
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_POWER_ON_CLEARED_8
    #undef ECUM_STOP_SEC_INTERNAL_VAR_POWER_ON_CLEARED_8
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_POWER_ON_CLEARED_8
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_POWER_ON_CLEARED_8
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_POWER_ON_CLEARED_8
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_POWER_ON_CLEARED_16
    #undef ECUM_STOP_SEC_INTERNAL_VAR_POWER_ON_CLEARED_16
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_POWER_ON_CLEARED_16
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_POWER_ON_CLEARED_16
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_POWER_ON_CLEARED_16
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_POWER_ON_CLEARED_32
    #undef ECUM_STOP_SEC_INTERNAL_VAR_POWER_ON_CLEARED_32
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_POWER_ON_CLEARED_32
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_POWER_ON_CLEARED_32
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_POWER_ON_CLEARED_32
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#if !defined (AUTOSAR_421)
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_POWER_ON_CLEARED_PTR
    #undef ECUM_STOP_SEC_INTERNAL_VAR_POWER_ON_CLEARED_PTR
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_POWER_ON_CLEARED_PTR
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_POWER_ON_CLEARED_PTR
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_POWER_ON_CLEARED_PTR
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#endif /* (AUTOSAR_421) */
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_POWER_ON_CLEARED_UNSPECIFIED
    #undef ECUM_STOP_SEC_INTERNAL_VAR_POWER_ON_CLEARED_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_POWER_ON_CLEARED_UNSPECIFIED
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_POWER_ON_CLEARED_UNSPECIFIED
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_POWER_ON_CLEARED_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_INIT_BOOLEAN
    #undef ECUM_STOP_SEC_INTERNAL_VAR_INIT_BOOLEAN
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_INIT_BOOLEAN
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_INIT_BOOLEAN
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_INIT_BOOLEAN
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_INIT_8
    #undef ECUM_STOP_SEC_INTERNAL_VAR_INIT_8
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_INIT_8
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_INIT_8
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_INIT_8
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_INIT_16
    #undef ECUM_STOP_SEC_INTERNAL_VAR_INIT_16
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_INIT_16
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_INIT_16
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_INIT_16
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_INIT_32
    #undef ECUM_STOP_SEC_INTERNAL_VAR_INIT_32
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_INIT_32
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_INIT_32
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_INIT_32
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#if !defined (AUTOSAR_421)
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_INIT_PTR
    #undef ECUM_STOP_SEC_INTERNAL_VAR_INIT_PTR
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_INIT_PTR
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_INIT_PTR
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_INIT_PTR
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#endif /* (AUTOSAR_421) */
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_INIT_UNSPECIFIED
    #undef ECUM_STOP_SEC_INTERNAL_VAR_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_INIT_UNSPECIFIED
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_INIT_UNSPECIFIED
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_POWER_ON_INIT_BOOLEAN
    #undef ECUM_STOP_SEC_INTERNAL_VAR_POWER_ON_INIT_BOOLEAN
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_POWER_ON_INIT_BOOLEAN
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_POWER_ON_INIT_BOOLEAN
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_POWER_ON_INIT_BOOLEAN
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_POWER_ON_INIT_8
    #undef ECUM_STOP_SEC_INTERNAL_VAR_POWER_ON_INIT_8
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_POWER_ON_INIT_8
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_POWER_ON_INIT_8
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_POWER_ON_INIT_8
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_POWER_ON_INIT_16
    #undef ECUM_STOP_SEC_INTERNAL_VAR_POWER_ON_INIT_16
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_POWER_ON_INIT_16
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_POWER_ON_INIT_16
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_POWER_ON_INIT_16
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_POWER_ON_INIT_32
    #undef ECUM_STOP_SEC_INTERNAL_VAR_POWER_ON_INIT_32
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_POWER_ON_INIT_32
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_POWER_ON_INIT_32
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_POWER_ON_INIT_32
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#if !defined (AUTOSAR_421)
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_POWER_ON_INIT_PTR
    #undef ECUM_STOP_SEC_INTERNAL_VAR_POWER_ON_INIT_PTR
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_POWER_ON_INIT_PTR
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_POWER_ON_INIT_PTR
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_POWER_ON_INIT_PTR
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#endif /* (AUTOSAR_421) */
#elif defined ECUM_STOP_SEC_INTERNAL_VAR_POWER_ON_INIT_UNSPECIFIED
    #undef ECUM_STOP_SEC_INTERNAL_VAR_POWER_ON_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_INTERNAL_VAR_POWER_ON_INIT_UNSPECIFIED
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_INTERNAL_VAR_POWER_ON_INIT_UNSPECIFIED
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_INTERNAL_VAR_POWER_ON_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_CONST_BOOLEAN
    #undef ECUM_STOP_SEC_CONST_BOOLEAN
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_CONST_BOOLEAN
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_CONST_BOOLEAN
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_CONST_BOOLEAN
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_CONST_8
    #undef ECUM_STOP_SEC_CONST_8
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_CONST_8
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_CONST_8
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_CONST_8
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_CONST_16
    #undef ECUM_STOP_SEC_CONST_16
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_CONST_16
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_CONST_16
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_CONST_16
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_CONST_32
    #undef ECUM_STOP_SEC_CONST_32
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_CONST_32
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_CONST_32
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_CONST_32
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#if !defined (AUTOSAR_421)
#elif defined ECUM_STOP_SEC_CONST_PTR
    #undef ECUM_STOP_SEC_CONST_PTR
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_CONST_PTR
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_CONST_PTR
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_CONST_PTR
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#endif /* (AUTOSAR_421) */
#elif defined ECUM_STOP_SEC_CONST_UNSPECIFIED
    #undef ECUM_STOP_SEC_CONST_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_CONST_UNSPECIFIED
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_CONST_UNSPECIFIED
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_CONST_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_CALIB_BOOLEAN
    #undef ECUM_STOP_SEC_CALIB_BOOLEAN
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_CALIB_BOOLEAN
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_CALIB_BOOLEAN
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_CALIB_BOOLEAN
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_CALIB_8
    #undef ECUM_STOP_SEC_CALIB_8
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_CALIB_8
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_CALIB_8
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_CALIB_8
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_CALIB_16
    #undef ECUM_STOP_SEC_CALIB_16
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_CALIB_16
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_CALIB_16
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_CALIB_16
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_CALIB_32
    #undef ECUM_STOP_SEC_CALIB_32
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_CALIB_32
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_CALIB_32
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_CALIB_32
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#if !defined (AUTOSAR_421)
#elif defined ECUM_STOP_SEC_CALIB_PTR
    #undef ECUM_STOP_SEC_CALIB_PTR
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_CALIB_PTR
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_CALIB_PTR
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_CALIB_PTR
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#endif /* (AUTOSAR_421) */
#elif defined ECUM_STOP_SEC_CALIB_UNSPECIFIED
    #undef ECUM_STOP_SEC_CALIB_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_CALIB_UNSPECIFIED
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_CALIB_UNSPECIFIED
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_CALIB_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_CONFIG_DATA_BOOLEAN
    #undef ECUM_STOP_SEC_CONFIG_DATA_BOOLEAN
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_CONFIG_DATA_BOOLEAN
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_CONFIG_DATA_BOOLEAN
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_CONFIG_DATA_BOOLEAN
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_CONFIG_DATA_8
    #undef ECUM_STOP_SEC_CONFIG_DATA_8
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_CONFIG_DATA_8
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_CONFIG_DATA_8
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_CONFIG_DATA_8
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_CONFIG_DATA_16
    #undef ECUM_STOP_SEC_CONFIG_DATA_16
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_CONFIG_DATA_16
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_CONFIG_DATA_16
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_CONFIG_DATA_16
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#elif defined ECUM_STOP_SEC_CONFIG_DATA_32
    #undef ECUM_STOP_SEC_CONFIG_DATA_32
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_CONFIG_DATA_32
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_CONFIG_DATA_32
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_CONFIG_DATA_32
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#if !defined (AUTOSAR_421)
#elif defined ECUM_STOP_SEC_CONFIG_DATA_PTR
    #undef ECUM_STOP_SEC_CONFIG_DATA_PTR
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_CONFIG_DATA_PTR
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_CONFIG_DATA_PTR
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_CONFIG_DATA_PTR
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#endif /* (AUTOSAR_421) */
#elif defined ECUM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
    #undef ECUM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifdef ECUM_START_SEC_CONFIG_DATA_UNSPECIFIED
        #undef ECUM_MEMMAP_SECTION_STARTED
        #undef ECUM_START_SEC_CONFIG_DATA_UNSPECIFIED
    #else
        #error "No corresponding memory section STARTed before STOP."
    #endif
    /* No section end keyword. */
#elif defined ECUM_START_SEC_CONFIG_DATA_UNSPECIFIED
    #undef MEMMAP_ERROR
    #ifndef ECUM_MEMMAP_SECTION_STARTED
        #define ECUM_MEMMAP_SECTION_STARTED
    #else
      #error "Previous memory section should be STOPed before STARTing new one."
    #endif
#endif


#ifdef MEMMAP_ERROR
    #error "Valid memory mapping symbol is not defined."
#endif

#ifdef __cplusplus
}
#endif

