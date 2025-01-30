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
 *  \file     Mcu_MemMap.h
 *
 *  \brief    This file contains memory map section definition
 *
 */

 #ifdef __cplusplus
 extern "C"
 {
 #endif


 /* START/STOP Macros are defined twice for each variables/function sections to
  * support MPU core. With this approach no change required in current M4 memmap
  * implementation. #pragma is replaced by __attribute__ for MPU. */

 #define MEMMAP_ERROR

 /* MCU */
 #if defined(MCU_START_SEC_VAR_INIT_UNSPECIFIED)
 #undef MCU_START_SEC_VAR_INIT_UNSPECIFIED
 #define START_SEC_COMMON_VAR_INIT_UNSPECIFIED

 #elif defined(MCU_START_SEC_VAR_INIT_32)
 #undef MCU_START_SEC_VAR_INIT_32
 #define START_SEC_COMMON_VAR_INIT_32

 #elif defined(MCU_START_SEC_VAR_INIT_16)
 #undef MCU_START_SEC_VAR_INIT_16
 #define START_SEC_COMMON_VAR_INIT_16

 #elif defined(MCU_START_SEC_VAR_INIT_8)
 #undef MCU_START_SEC_VAR_INIT_8
 #define START_SEC_COMMON_VAR_INIT_8

 #if !defined (AUTOSAR_421)
 #elif defined(MCU_START_SEC_VAR_INIT_PTR)
 #undef MCU_START_SEC_VAR_INIT_PTR
 #define START_SEC_COMMON_VAR_INIT_PTR
 #endif /* (AUTOSAR_421) */

 #elif defined(MCU_START_SEC_VAR_NO_INIT_UNSPECIFIED)
 #undef MCU_START_SEC_VAR_NO_INIT_UNSPECIFIED
 #define START_SEC_COMMON_VAR_NO_INIT_UNSPECIFIED

 #elif defined(MCU_START_SEC_VAR_NO_INIT_32)
 #undef MCU_START_SEC_VAR_NO_INIT_32
 #define START_SEC_COMMON_VAR_NO_INIT_32

 #elif defined(MCU_START_SEC_VAR_NO_INIT_16)
 #undef MCU_START_SEC_VAR_NO_INIT_16
 #define START_SEC_COMMON_VAR_NO_INIT_16

 #elif defined(MCU_START_SEC_VAR_NO_INIT_8)
 #undef MCU_START_SEC_VAR_NO_INIT_8
 #define START_SEC_COMMON_VAR_NO_INIT_8

 #if !defined (AUTOSAR_421)
 #elif defined(MCU_START_SEC_VAR_NO_INIT_PTR)
 #undef MCU_START_SEC_VAR_NO_INIT_PTR
 #define START_SEC_COMMON_VAR_NO_INIT_PTR
 #endif /* (AUTOSAR_421) */

 #elif defined(MCU_START_SEC_CONST_UNSPECIFIED)
 #undef MCU_START_SEC_CONST_UNSPECIFIED
 #define START_SEC_COMMON_CONST_UNSPECIFIED

 #elif defined(MCU_START_SEC_CONST_32)
 #undef MCU_START_SEC_CONST_32
 #define START_SEC_COMMON_CONST_32

 #elif defined(MCU_START_SEC_CONST_16)
 #undef MCU_START_SEC_CONST_16
 #define START_SEC_COMMON_CONST_16

 #elif defined(MCU_START_SEC_CONST_8)
 #undef MCU_START_SEC_CONST_8
 #define START_SEC_COMMON_CONST_8

 #if !defined (AUTOSAR_421)
 #elif defined(MCU_START_SEC_CONST_PTR)
 #undef MCU_START_SEC_CONST_PTR
 #define START_SEC_COMMON_CONST_PTR
 #endif /* (AUTOSAR_421) */

 #elif defined(MCU_START_SEC_CONFIG_DATA)
 #undef MCU_START_SEC_CONFIG_DATA
 #define START_SEC_COMMON_CONFIG_DATA

 #elif defined(MCU_START_SEC_CODE)
 #undef MCU_START_SEC_CODE
 #define START_SEC_COMMON_CODE

 #elif defined(MCU_START_SEC_ISR_CODE)
 #undef MCU_START_SEC_ISR_CODE
 #define START_SEC_COMMON_ISR_CODE

 #elif defined(MCU_START_SEC_CALLOUT_CODE)
 #undef MCU_START_SEC_CALLOUT_CODE
 #define START_SEC_COMMON_CALLOUT_CODE

 #endif

 #if defined(MCU_STOP_SEC_VAR_INIT_UNSPECIFIED)
 #undef MCU_STOP_SEC_VAR_INIT_UNSPECIFIED
 #define STOP_SEC_COMMON_VAR_INIT_UNSPECIFIED

 #elif defined(MCU_STOP_SEC_VAR_INIT_32)
 #undef MCU_STOP_SEC_VAR_INIT_32
 #define STOP_SEC_COMMON_VAR_INIT_32

 #elif defined(MCU_STOP_SEC_VAR_INIT_16)
 #undef MCU_STOP_SEC_VAR_INIT_16
 #define STOP_SEC_COMMON_VAR_INIT_16

 #elif defined(MCU_STOP_SEC_VAR_INIT_8)
 #undef MCU_STOP_SEC_VAR_INIT_8
 #define STOP_SEC_COMMON_VAR_INIT_8

 #if !defined (AUTOSAR_421)
 #elif defined(MCU_STOP_SEC_VAR_INIT_PTR)
 #undef MCU_STOP_SEC_VAR_INIT_PTR
 #define STOP_SEC_COMMON_VAR_INIT_PTR
 #endif /* (AUTOSAR_421) */

 #elif defined(MCU_STOP_SEC_VAR_NO_INIT_UNSPECIFIED)
 #undef MCU_STOP_SEC_VAR_NO_INIT_UNSPECIFIED
 #define STOP_SEC_COMMON_VAR_NO_INIT_UNSPECIFIED

 #elif defined(MCU_STOP_SEC_VAR_NO_INIT_32)
 #undef MCU_STOP_SEC_VAR_NO_INIT_32
 #define STOP_SEC_COMMON_VAR_NO_INIT_32

 #elif defined(MCU_STOP_SEC_VAR_NO_INIT_16)
 #undef MCU_STOP_SEC_VAR_NO_INIT_16
 #define STOP_SEC_COMMON_VAR_NO_INIT_16

 #elif defined(MCU_STOP_SEC_VAR_NO_INIT_8)
 #undef MCU_STOP_SEC_VAR_NO_INIT_8
 #define STOP_SEC_COMMON_VAR_NO_INIT_8

 #if !defined (AUTOSAR_421)
 #elif defined(MCU_STOP_SEC_VAR_NO_INIT_PTR)
 #undef MCU_STOP_SEC_VAR_NO_INIT_PTR
 #define STOP_SEC_COMMON_VAR_NO_INIT_PTR
 #endif /* (AUTOSAR_421) */

 #elif defined(MCU_STOP_SEC_CONST_UNSPECIFIED)
 #undef MCU_STOP_SEC_CONST_UNSPECIFIED
 #define STOP_SEC_COMMON_CONST_UNSPECIFIED

 #elif defined(MCU_STOP_SEC_CONST_32)
 #undef MCU_STOP_SEC_CONST_32
 #define STOP_SEC_COMMON_CONST_32

 #elif defined(MCU_STOP_SEC_CONST_16)
 #undef MCU_STOP_SEC_CONST_16
 #define STOP_SEC_COMMON_CONST_16

 #elif defined(MCU_STOP_SEC_CONST_8)
 #undef MCU_STOP_SEC_CONST_8
 #define STOP_SEC_COMMON_CONST_8

 #if !defined (AUTOSAR_421)
 #elif defined(MCU_STOP_SEC_CONST_PTR)
 #undef MCU_STOP_SEC_CONST_PTR
 #define STOP_SEC_COMMON_CONST_PTR
 #endif /* (AUTOSAR_421) */

 #elif defined(MCU_STOP_SEC_CONFIG_DATA)
 #undef MCU_STOP_SEC_CONFIG_DATA
 #define STOP_SEC_COMMON_CONFIG_DATA

 #elif defined(MCU_STOP_SEC_CODE)
 #undef MCU_STOP_SEC_CODE
 #define STOP_SEC_COMMON_CODE

 #elif defined(MCU_STOP_SEC_ISR_CODE)
 #undef MCU_STOP_SEC_ISR_CODE
 #define STOP_SEC_COMMON_ISR_CODE

 #elif defined(MCU_STOP_SEC_CALLOUT_CODE)
 #undef MCU_STOP_SEC_CALLOUT_CODE
 #define STOP_SEC_COMMON_CALLOUT_CODE

 #endif

 #if defined(START_SEC_COMMON_VAR_INIT_UNSPECIFIED)
 #pragma clang section data="MCU_DATA_INIT_UNSPECIFIED_SECTION"
 #undef START_SEC_COMMON_VAR_INIT_UNSPECIFIED
 #undef MEMMAP_ERROR
 #ifdef MEMMAP_ACTIVE_DATA_SECTION
 #error "SECTION start keyword not matching"
 #endif
 #define MEMMAP_ACTIVE_DATA_SECTION (VAR_INIT_UNSPECIFIED)
 #elif defined(START_SEC_COMMON_VAR_INIT_32)
 #pragma clang section data="MCU_DATA_INIT_32_SECTION"
 #undef START_SEC_COMMON_VAR_INIT_32
 #undef MEMMAP_ERROR
 #ifdef MEMMAP_ACTIVE_DATA_SECTION
 #error "SECTION start keyword not matching"
 #endif
 #define MEMMAP_ACTIVE_DATA_SECTION (VAR_INIT_32)

 #elif defined(START_SEC_COMMON_VAR_INIT_16)
 #pragma clang section data="MCU_DATA_INIT_16_SECTION"
 #undef START_SEC_COMMON_VAR_INIT_16
 #undef MEMMAP_ERROR
 #ifdef MEMMAP_ACTIVE_DATA_SECTION
 #error "SECTION start keyword not matching"
 #endif
 #define MEMMAP_ACTIVE_DATA_SECTION (VAR_INIT_16)

 #elif defined(START_SEC_COMMON_VAR_INIT_8)
 #pragma clang section data="MCU_DATA_INIT_8_SECTION"
 #undef START_SEC_COMMON_VAR_INIT_8
 #undef MEMMAP_ERROR
 #ifdef MEMMAP_ACTIVE_DATA_SECTION
 #error "SECTION start keyword not matching"
 #endif
 #define MEMMAP_ACTIVE_DATA_SECTION (VAR_INIT_8)

 #if !defined (AUTOSAR_421)
 #elif defined(START_SEC_COMMON_VAR_INIT_PTR)
 #pragma clang section data="MCU_DATA_INIT_PTR_SECTION"
 #undef START_SEC_COMMON_VAR_INIT_PTR
 #undef MEMMAP_ERROR
 #ifdef MEMMAP_ACTIVE_DATA_SECTION
 #error "SECTION start keyword not matching"
 #endif
 #define MEMMAP_ACTIVE_DATA_SECTION (VAR_INIT_PTR)
 #endif /* (AUTOSAR_421) */

 #elif defined(START_SEC_COMMON_VAR_NO_INIT_UNSPECIFIED)
 #pragma clang section data="MCU_DATA_NO_INIT_UNSPECIFIED_SECTION"
 #undef START_SEC_COMMON_VAR_NO_INIT_UNSPECIFIED
 #undef MEMMAP_ERROR
 #ifdef MEMMAP_ACTIVE_DATA_SECTION
 #error "SECTION start keyword not matching"
 #endif
 #define MEMMAP_ACTIVE_DATA_SECTION (VAR_NO_INIT_UNSPECIFIED)

 #elif defined(START_SEC_COMMON_VAR_NO_INIT_32)
 #pragma clang section data="MCU_DATA_NO_INIT_32_SECTION"
 #undef START_SEC_COMMON_VAR_NO_INIT_32
 #undef MEMMAP_ERROR
 #ifdef MEMMAP_ACTIVE_DATA_SECTION
 #error "SECTION start keyword not matching"
 #endif
 #define MEMMAP_ACTIVE_DATA_SECTION (VAR_NO_INIT_32)

 #elif defined(START_SEC_COMMON_VAR_NO_INIT_16)
 #pragma clang section data="MCU_DATA_NO_INIT_16_SECTION"
 #undef START_SEC_COMMON_VAR_NO_INIT_16
 #undef MEMMAP_ERROR
 #ifdef MEMMAP_ACTIVE_DATA_SECTION
 #error "SECTION start keyword not matching"
 #endif
 #define MEMMAP_ACTIVE_DATA_SECTION (VAR_NO_INIT_16)

 #elif defined(START_SEC_COMMON_VAR_NO_INIT_8)
 #pragma clang section data="MCU_DATA_NO_INIT_8_SECTION"
 #undef START_SEC_COMMON_VAR_NO_INIT_8
 #undef MEMMAP_ERROR
 #ifdef MEMMAP_ACTIVE_DATA_SECTION
 #error "SECTION start keyword not matching"
 #endif
 #define MEMMAP_ACTIVE_DATA_SECTION (VAR_NO_INIT_8)

 #if !defined (AUTOSAR_421)
 #elif defined(START_SEC_COMMON_VAR_NO_INIT_PTR)
 #pragma clang section data="MCU_DATA_NO_INIT_PTR_SECTION"
 #undef START_SEC_COMMON_VAR_NO_INIT_PTR
 #undef MEMMAP_ERROR
 #ifdef MEMMAP_ACTIVE_DATA_SECTION
 #error "SECTION start keyword not matching"
 #endif
 #define MEMMAP_ACTIVE_DATA_SECTION (VAR_NO_INIT_PTR)
 #endif /* (AUTOSAR_421) */

 #elif defined(START_SEC_COMMON_CONST_UNSPECIFIED)
 #pragma clang section data="MCU_CONST_UNSPECIFIED_SECTION"
 #undef START_SEC_COMMON_CONST_UNSPECIFIED
 #undef MEMMAP_ERROR
 #ifdef MEMMAP_ACTIVE_DATA_SECTION
 #error "SECTION start keyword not matching"
 #endif
 #define MEMMAP_ACTIVE_DATA_SECTION (VAR_CONST_UNSPECIFIED)

 #elif defined(START_SEC_COMMON_CONST_32)
 #pragma clang section data="MCU_CONST_32_SECTION"
 #undef START_SEC_COMMON_CONST_32
 #undef MEMMAP_ERROR
 #ifdef MEMMAP_ACTIVE_DATA_SECTION
 #error "SECTION start keyword not matching"
 #endif
 #define MEMMAP_ACTIVE_DATA_SECTION (VAR_CONST_32)

 #elif defined(START_SEC_COMMON_CONST_16)
 #pragma clang section data="MCU_CONST_16_SECTION"
 #undef START_SEC_COMMON_CONST_16
 #undef MEMMAP_ERROR
 #ifdef MEMMAP_ACTIVE_DATA_SECTION
 #error "SECTION start keyword not matching"
 #endif
 #define MEMMAP_ACTIVE_DATA_SECTION (VAR_CONST_16)

 #elif defined(START_SEC_COMMON_CONST_8)
 #pragma clang section data="MCU_CONST_8_SECTION"
 #undef START_SEC_COMMON_CONST_8
 #undef MEMMAP_ERROR
 #ifdef MEMMAP_ACTIVE_DATA_SECTION
 #error "SECTION start keyword not matching"
 #endif
 #define MEMMAP_ACTIVE_DATA_SECTION (VAR_CONST_8)

 #if !defined (AUTOSAR_421)
 #elif defined(START_SEC_COMMON_CONST_PTR)
 #pragma clang section data="MCU_CONST_PTR_SECTION"
 #undef START_SEC_COMMON_CONST_PTR
 #undef MEMMAP_ERROR
 #ifdef MEMMAP_ACTIVE_DATA_SECTION
 #error "SECTION start keyword not matching"
 #endif
 #define MEMMAP_ACTIVE_DATA_SECTION (VAR_CONST_PTR)
 #endif /* (AUTOSAR_421) */

 #elif defined(START_SEC_COMMON_CONFIG_DATA)
 #pragma clang section data="MCU_CONFIG_SECTION"
 #undef START_SEC_COMMON_CONFIG_DATA
 #undef MEMMAP_ERROR
 #ifdef MEMMAP_ACTIVE_DATA_SECTION
 #error "SECTION start keyword not matching"
 #endif
 #define MEMMAP_ACTIVE_DATA_SECTION (VAR_CONFIG_DATA)

 #elif defined(START_SEC_COMMON_CODE)
 #pragma clang section text="MCU_TEXT_SECTION"
 #undef START_SEC_COMMON_CODE
 #undef MEMMAP_ERROR
 #ifdef MEMMAP_ACTIVE_CODE_SECTION
 #error "SECTION start keyword not matching"
 #endif
 #define MEMMAP_ACTIVE_CODE_SECTION (CODE)

 #elif defined(START_SEC_COMMON_ISR_CODE)
 #pragma clang section text="MCU_ISR_TEXT_SECTION"
 #undef START_SEC_COMMON_ISR_CODE
 #undef MEMMAP_ERROR
 #ifdef MEMMAP_ACTIVE_CODE_SECTION
 #error "SECTION start keyword not matching"
 #endif
 #define MEMMAP_ACTIVE_CODE_SECTION (CODE_ISR)

 #elif defined(START_SEC_COMMON_CALLOUT_CODE)
 #pragma clang section text="MCU_CALLOUT_TEXT_SECTION"
 #undef START_SEC_COMMON_CALLOUT_CODE
 #undef MEMMAP_ERROR
 #ifdef MEMMAP_ACTIVE_CODE_SECTION
 #error "SECTION start keyword not matching"
 #endif
 #define MEMMAP_ACTIVE_CODE_SECTION (CODE_CALLOUT)

 #endif

 #if defined(STOP_SEC_COMMON_VAR_INIT_UNSPECIFIED)
 #pragma clang section data=""
 #undef STOP_SEC_COMMON_VAR_INIT_UNSPECIFIED
 #undef MEMMAP_ERROR
 #if (!defined(MEMMAP_ACTIVE_DATA_SECTION) || \
     (MEMMAP_ACTIVE_DATA_SECTION != VAR_INIT_UNSPECIFIED))
 #error "STOP keyword not matching start"
 #endif
 #undef MEMMAP_ACTIVE_DATA_SECTION

 #elif defined(STOP_SEC_COMMON_VAR_INIT_32)
 #pragma clang section data=""
 #undef STOP_SEC_COMMON_VAR_INIT_32
 #undef MEMMAP_ERROR
 #if (!defined(MEMMAP_ACTIVE_DATA_SECTION) || \
     (MEMMAP_ACTIVE_DATA_SECTION != VAR_INIT_32))
 #error "STOP keyword not matching start"
 #endif
 #undef MEMMAP_ACTIVE_DATA_SECTION

 #elif defined(STOP_SEC_COMMON_VAR_INIT_16)
 #pragma clang section data=""
 #undef STOP_SEC_COMMON_VAR_INIT_16
 #undef MEMMAP_ERROR
 #if (!defined(MEMMAP_ACTIVE_DATA_SECTION) || \
     (MEMMAP_ACTIVE_DATA_SECTION != VAR_INIT_16))
 #error "STOP keyword not matching start"
 #endif
 #undef MEMMAP_ACTIVE_DATA_SECTION

 #elif defined(STOP_SEC_COMMON_VAR_INIT_8)
 #pragma clang section data=""
 #undef STOP_SEC_COMMON_VAR_INIT_8
 #undef MEMMAP_ERROR
 #if (!defined(MEMMAP_ACTIVE_DATA_SECTION) || \
     (MEMMAP_ACTIVE_DATA_SECTION != VAR_INIT_8))
 #error "STOP keyword not matching start"
 #endif
 #undef MEMMAP_ACTIVE_DATA_SECTION

 #if !defined (AUTOSAR_421)
 #elif defined(STOP_SEC_COMMON_VAR_INIT_PTR)
 #pragma clang section data=""
 #undef STOP_SEC_COMMON_VAR_INIT_PTR
 #undef MEMMAP_ERROR
 #if (!defined(MEMMAP_ACTIVE_DATA_SECTION) || \
     (MEMMAP_ACTIVE_DATA_SECTION != VAR_INIT_PTR))
 #error "STOP keyword not matching start"
 #endif
 #undef MEMMAP_ACTIVE_DATA_SECTION
 #endif /* (AUTOSAR_421) */

 #elif defined(STOP_SEC_COMMON_VAR_NO_INIT_UNSPECIFIED)
 #pragma clang section data=""
 #undef STOP_SEC_COMMON_VAR_NO_INIT_UNSPECIFIED
 #undef MEMMAP_ERROR
 #if (!defined(MEMMAP_ACTIVE_DATA_SECTION) || \
     (MEMMAP_ACTIVE_DATA_SECTION != VAR_NO_INIT_UNSPECIFIED))
 #error "STOP keyword not matching start"
 #endif
 #undef MEMMAP_ACTIVE_DATA_SECTION

 #elif defined(STOP_SEC_COMMON_VAR_NO_INIT_32)
 #pragma clang section data=""
 #undef STOP_SEC_COMMON_VAR_NO_INIT_32
 #undef MEMMAP_ERROR
 #if (!defined(MEMMAP_ACTIVE_DATA_SECTION) || \
     (MEMMAP_ACTIVE_DATA_SECTION != VAR_NO_INIT_32))
 #error "STOP keyword not matching start"
 #endif
 #undef MEMMAP_ACTIVE_DATA_SECTION

 #elif defined(STOP_SEC_COMMON_VAR_NO_INIT_16)
 #pragma clang section data=""
 #undef STOP_SEC_COMMON_VAR_NO_INIT_16
 #undef MEMMAP_ERROR
 #if (!defined(MEMMAP_ACTIVE_DATA_SECTION) || \
     (MEMMAP_ACTIVE_DATA_SECTION != VAR_NO_INIT_16))
 #error "STOP keyword not matching start"
 #endif
 #undef MEMMAP_ACTIVE_DATA_SECTION

 #elif defined(STOP_SEC_COMMON_VAR_NO_INIT_8)
 #pragma clang section data=""
 #undef STOP_SEC_COMMON_VAR_NO_INIT_8
 #undef MEMMAP_ERROR
 #if (!defined(MEMMAP_ACTIVE_DATA_SECTION) || \
     (MEMMAP_ACTIVE_DATA_SECTION != VAR_NO_INIT_8))
 #error "STOP keyword not matching start"
 #endif
 #undef MEMMAP_ACTIVE_DATA_SECTION

 #if !defined (AUTOSAR_421)
 #elif defined(STOP_SEC_COMMON_VAR_NO_INIT_PTR)
 #pragma clang section data=""
 #undef STOP_SEC_COMMON_VAR_NO_INIT_PTR
 #undef MEMMAP_ERROR
 #if (!defined(MEMMAP_ACTIVE_DATA_SECTION) || \
     (MEMMAP_ACTIVE_DATA_SECTION != VAR_NO_INIT_PTR))
 #error "STOP keyword not matching start"
 #endif
 #undef MEMMAP_ACTIVE_DATA_SECTION
 #endif /* (AUTOSAR_421) */

 #elif defined(STOP_SEC_COMMON_CONST_UNSPECIFIED)
 #pragma clang section data=""
 #undef STOP_SEC_COMMON_CONST_UNSPECIFIED
 #undef MEMMAP_ERROR
 #if (!defined(MEMMAP_ACTIVE_DATA_SECTION) || \
     (MEMMAP_ACTIVE_DATA_SECTION != VAR_CONST_UNSPECIFIED))
 #error "STOP keyword not matching start"
 #endif
 #undef MEMMAP_ACTIVE_DATA_SECTION

 #elif defined(STOP_SEC_COMMON_CONST_32)
 #pragma clang section data=""
 #undef STOP_SEC_COMMON_CONST_32
 #undef MEMMAP_ERROR
 #if (!defined(MEMMAP_ACTIVE_DATA_SECTION) || \
     (MEMMAP_ACTIVE_DATA_SECTION != VAR_CONST_32))
 #error "STOP keyword not matching start"
 #endif
 #undef MEMMAP_ACTIVE_DATA_SECTION

 #elif defined(STOP_SEC_COMMON_CONST_16)
 #pragma clang section data=""
 #undef STOP_SEC_COMMON_CONST_16
 #undef MEMMAP_ERROR
 #if (!defined(MEMMAP_ACTIVE_DATA_SECTION) || \
     (MEMMAP_ACTIVE_DATA_SECTION != VAR_CONST_16))
 #error "STOP keyword not matching start"
 #endif
 #undef MEMMAP_ACTIVE_DATA_SECTION

 #elif defined(STOP_SEC_COMMON_CONST_8)
 #pragma clang section data=""
 #undef STOP_SEC_COMMON_CONST_8
 #undef MEMMAP_ERROR
 #if (!defined(MEMMAP_ACTIVE_DATA_SECTION) || \
     (MEMMAP_ACTIVE_DATA_SECTION != VAR_CONST_8))
 #error "STOP keyword not matching start"
 #endif
 #undef MEMMAP_ACTIVE_DATA_SECTION

 #if !defined (AUTOSAR_421)
 #elif defined(STOP_SEC_COMMON_CONST_PTR)
 #pragma clang section data=""
 #undef STOP_SEC_COMMON_CONST_PTR
 #undef MEMMAP_ERROR
 #if (!defined(MEMMAP_ACTIVE_DATA_SECTION) || \
     (MEMMAP_ACTIVE_DATA_SECTION != VAR_CONST_PTR))
 #error "STOP keyword not matching start"
 #endif
 #undef MEMMAP_ACTIVE_DATA_SECTION
 #endif /* (AUTOSAR_421) */

 #elif defined(STOP_SEC_COMMON_CONFIG_DATA)
 #pragma clang section data=""
 #undef STOP_SEC_COMMON_CONFIG_DATA
 #undef MEMMAP_ERROR
 #if (!defined(MEMMAP_ACTIVE_DATA_SECTION) || \
     (MEMMAP_ACTIVE_DATA_SECTION != VAR_CONFIG_DATA))
 #error "STOP keyword not matching start"
 #endif
 #undef MEMMAP_ACTIVE_DATA_SECTION

 #elif defined(STOP_SEC_COMMON_CODE)
 #pragma clang section text=""
 #undef STOP_SEC_COMMON_CODE
 #undef MEMMAP_ERROR
 #if (!defined(MEMMAP_ACTIVE_CODE_SECTION) || \
     (MEMMAP_ACTIVE_CODE_SECTION != CODE))
 #error "STOP keyword not matching start"
 #endif
 #undef MEMMAP_ACTIVE_CODE_SECTION

 #elif defined(STOP_SEC_COMMON_ISR_CODE)
 #pragma clang section text=""
 #undef STOP_SEC_COMMON_ISR_CODE
 #undef MEMMAP_ERROR
 #if (!defined(MEMMAP_ACTIVE_CODE_SECTION) || \
     (MEMMAP_ACTIVE_CODE_SECTION != CODE_ISR))
 #error "STOP keyword not matching start"
 #endif
 #undef MEMMAP_ACTIVE_CODE_SECTION

 #elif defined(STOP_SEC_COMMON_CALLOUT_CODE)
 #pragma clang section text=""
 #undef MEMMAP_ACTIVE_CODE_SECTION

 #endif

 #ifdef MEMMAP_ERROR
 #error "Mcu_Memmap.h pragma error"
 #endif


 #ifdef __cplusplus
 }
 #endif
