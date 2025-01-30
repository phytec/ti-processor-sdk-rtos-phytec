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
 *  \file     Std_Types.h
 *
 *  \brief    This file defines standard types
 *
 */
/* [SWS_Std_00014] */
#ifndef STD_TYPES_H_
#define STD_TYPES_H_

#ifdef __cplusplus
extern "C"{
#endif
/* [SWS_Std_00019] */
#include "Platform_Types.h"
#include "Compiler.h"


/*******************************************************************************
                            EXPORTED CONSTANTS and MACROS
*******************************************************************************/
/* [SWS_BSW_00059] Published information elements. */
#define STD_VENDOR_ID   (44U)
#define STD_MODULE_ID   (197U)

#if defined (AUTOSAR_421)

#define STD_AR_RELEASE_MAJOR_VERSION    (4U)
#define STD_AR_RELEASE_MINOR_VERSION    (2U)
#define STD_AR_RELEASE_REVISION_VERSION (1U)

#else

#define STD_AR_RELEASE_MAJOR_VERSION    (4U)
#define STD_AR_RELEASE_MINOR_VERSION    (3U)
#define STD_AR_RELEASE_REVISION_VERSION (1U)

#endif

#define STD_SW_MAJOR_VERSION    (9U)
#define STD_SW_MINOR_VERSION    (0U)
#define STD_SW_PATCH_VERSION    (0U)

/*******************************************************************************
                                Types
*******************************************************************************/

/* [SWS_Std_00005] */
/* 4.2.1 or 4.3.1, Valid range is 0x02 to 0x3F */
typedef uint8 Std_ReturnType;

/* [SWS_Std_00015] */
typedef struct
{
    uint16 vendorID;
    uint16 moduleID;
    uint8  sw_major_version;
    uint8  sw_minor_version;
    uint8  sw_patch_version;
} Std_VersionInfoType;

/* [SWS_Std_00006] */
#ifndef STATUSTYPEDEFINED
#define STATUSTYPEDEFINED
    #define E_OK    (0x00U)
    typedef unsigned char StatusType; /* OSEK compliance */
#endif
#define E_NOT_OK    (0x01U)

/* [SWS_Std_00007] */
/* Physical state 5V or 3.3V */
#define STD_HIGH    (0x01U)
/* Physical state 0V */
#define STD_LOW     (0x00U)

/* [SWS_Std_00013] */
/* Logical state active */
#define STD_ACTIVE  (0x01U)
/* Logical state idle */
#define STD_IDLE    (0x00U)

/* [SWS_Std_00010] */
#define STD_ON  (0x01U)
#define STD_OFF (0x00U)

/* [SWS_BSW_00036] Inter module version check. */
#if ((STD_AR_RELEASE_MAJOR_VERSION != PLATFORM_AR_RELEASE_MAJOR_VERSION) || \
     (STD_AR_RELEASE_MINOR_VERSION != PLATFORM_AR_RELEASE_MINOR_VERSION))
    #error "Autosar version numbers of Std_Types.h and Platform_Types.h are \
different."
#endif

#if ((STD_AR_RELEASE_MAJOR_VERSION != COMPILER_AR_RELEASE_MAJOR_VERSION) || \
     (STD_AR_RELEASE_MINOR_VERSION != COMPILER_AR_RELEASE_MINOR_VERSION))
   #error "Autosar version numbers of Std_Types.h and Compiler.h are different."
#endif


#ifdef __cplusplus
}
#endif

#endif /* STD_TYPES_H */
