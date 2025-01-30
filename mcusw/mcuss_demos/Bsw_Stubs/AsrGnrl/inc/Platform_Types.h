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
 *  \file     Platform_Types.h
 *
 *  \brief    This file defines platform dependent types and symbols
 *
 */

#ifndef PLATFORM_TYPES_H_
#define PLATFORM_TYPES_H_

#ifdef __cplusplus
extern "C"{
#endif



/*=============================================================================
|                       EXPORTED CONSTANTS and MACROS                         |
=============================================================================*/
/* [SWS_BSW_00059] Published information elements. */
/** \brief Vendor ID */
#define PLATFORM_VENDOR_ID  (44U)
/** \brief Module ID */
#define PLATFORM_MODULE_ID  (199U)

#if defined (AUTOSAR_421)

/** \brief AUTOSAR Version Major Number */
#define PLATFORM_AR_RELEASE_MAJOR_VERSION       (4U)
/** \brief AUTOSAR Version Minor Number */
#define PLATFORM_AR_RELEASE_MINOR_VERSION       (2U)
/** \brief AUTOSAR Version Revision Number */
#define PLATFORM_AR_RELEASE_REVISION_VERSION    (1U)

#else

/** \brief AUTOSAR Version Major Number */
#define PLATFORM_AR_RELEASE_MAJOR_VERSION       (4U)
/** \brief AUTOSAR Version Minor Number */
#define PLATFORM_AR_RELEASE_MINOR_VERSION       (3U)
/** \brief AUTOSAR Version Revision Number */
#define PLATFORM_AR_RELEASE_REVISION_VERSION    (1U)

#endif

/** \brief Version Major Number */
#define PLATFORM_SW_MAJOR_VERSION   (9U)
/** \brief Version Minor Number */
#define PLATFORM_SW_MINOR_VERSION   (0U)
/** \brief Version Revision Number */
#define PLATFORM_SW_PATCH_VERSION   (0U)


/*****************************************************************************/
/* GLOBAL DEFINITIONS                                                        */
/*****************************************************************************/
/* [SWS_Platform_00057] */
#define CPU_TYPE_8          (8)
#define CPU_TYPE_16         (16)
#define CPU_TYPE_32         (32)

#if defined (AUTOSAR_431)
#define CPU_TYPE_64         (64)
#endif

#define MSB_FIRST           (0)
#define LSB_FIRST           (1)

#define HIGH_BYTE_FIRST     (0)
#define LOW_BYTE_FIRST      (1)


/* [SWS_Platform_00064], [SWS_Platform_00044] and [SWS_Platform_00045] */
#define CPU_TYPE        (CPU_TYPE_32)

/* [SWS_Platform_00038], [SWS_Platform_00043] and [SWS_Platform_00048] */
#define CPU_BIT_ORDER   (LSB_FIRST)

/* [SWS_Platform_00039], [SWS_Platform_00046] and [SWS_Platform_00050] */
#define CPU_BYTE_ORDER  (LOW_BYTE_FIRST)

/* [SWS_Platform_00056] */
#ifndef TRUE
    #define TRUE        (1U)
#endif

#ifndef FALSE
    #define FALSE       (0U)
#endif

/*****************************************************************************/
/* TYPE DEFINITIONS                                                          */
/*****************************************************************************/
/* [SWS_Platform_00026] */
typedef unsigned char       boolean;

/* [SWS_Platform_00013] */
typedef unsigned char       uint8;
/* [SWS_Platform_00014] */
typedef unsigned short      uint16;
/* [SWS_Platform_00015] */
typedef unsigned int        uint32;
/* [SWS_Platform_00066] */
/* On R5F, long is 32 bits wide, long long is 64bits */
typedef unsigned long long  uint64;

/* [SWS_Platform_00016] */
typedef signed char         sint8;
/* [SWS_Platform_00017] */
typedef signed short        sint16;
/* [SWS_Platform_00018] */
typedef signed int          sint32;
/* [SWS_Platform_00067] */
typedef signed long long sint64;

/* [SWS_Platform_00020] */
typedef unsigned int        uint8_least;
/* [SWS_Platform_00021] */
typedef unsigned int        uint16_least;
/* [SWS_Platform_00022] */
typedef unsigned int        uint32_least;
/* [SWS_Platform_00023] */
typedef signed int          sint8_least;
/* [SWS_Platform_00024] */
typedef signed int          sint16_least;
/* [SWS_Platform_00025] */
typedef signed int          sint32_least;

/* [SWS_Platform_00041] */
typedef float               float32;
/* [SWS_Platform_00042] */
typedef double              float64;

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_TYPES_H_ */
