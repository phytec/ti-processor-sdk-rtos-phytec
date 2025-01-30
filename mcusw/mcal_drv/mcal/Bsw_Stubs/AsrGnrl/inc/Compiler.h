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
 *  \file     Compiler.h
 *
 *  \brief    This file defines compiler types
 *
 */

#ifndef COMPILER_H_
#define COMPILER_H_

#ifdef __cplusplus
extern "C"{
#endif

/*=============================================================================
 |                       INCLUDE FILES                                        |
 =============================================================================*/

#include "Compiler_Cfg.h"


/*=============================================================================
|                       EXPORTED CONSTANTS and MACROS                         |
=============================================================================*/
/* [SWS_COMPILER_00010] */
/** \brief COMPILER Symbol */
#define _TEXAS_INSTRUMENTS_C_ARM_

/* [SWS_BSW_00059] Published information elements. */
/** \brief Compiler Vendor ID */
#define COMPILER_VENDOR_ID  (44U)
/** \brief Module ID */
#define COMPILER_MODULE_ID  (198U)

/* [SWS_COMPILER_00030] */
/** \brief Compiler Version Major Number */
#define COMPILER_SW_MAJOR_VERSION     (20U)
/** \brief Compiler Version Minor Number */
#define COMPILER_SW_MINOR_VERSION     (2U)
/** \brief Compiler Version Revision Number */
#define COMPILER_SW_PATCH_VERSION     (0U)

#if defined (AUTOSAR_421)
/** \brief AUTOSAR Version Major Number */
#define COMPILER_AR_RELEASE_MAJOR_VERSION     (4U)
/** \brief AUTOSAR Version Minor Number */
#define COMPILER_AR_RELEASE_MINOR_VERSION     (2U)
/** \brief AUTOSAR Version Revision Number */
#define COMPILER_AR_RELEASE_REVISION_VERSION  (1U)

/* End of 4.2.1 specifics */

#elif defined (AUTOSAR_431)

/** \brief AUTOSAR Version Major Number */
#define COMPILER_AR_RELEASE_MAJOR_VERSION     (4U)
/** \brief AUTOSAR Version Minor Number */
#define COMPILER_AR_RELEASE_MINOR_VERSION     (3U)
/** \brief AUTOSAR Version Revision Number */
#define COMPILER_AR_RELEASE_REVISION_VERSION  (1U)

/* End of 4.3.1 specifics */

#else
    #error UnSupported AUTOSAR Version
#endif

/* [SWS_COMPILER_00046] */
/** \brief Used for declaration of local pointers */
#define AUTOMATIC

/* [SWS_COMPILER_00059] */
/** \brief The memory class TYPEDEF shall be provided with empty definition */
#define TYPEDEF

/* [SWS_COMPILER_00051] */
/** \brief NULL_PTR as void pointer with value of 0 */
#ifndef NULL_PTR
#define NULL_PTR ((void *)0)
#endif

/* [SWS_COMPILER_00057] */
/** \brief Indirection for inline keyword */
#define INLINE inline

/* [SWS_COMPILER_00060] */
/** \brief Indirection for local inline keyword */
#define LOCAL_INLINE static inline

/* [SWS_COMPILER_00001] */
/** \brief Indirection for function declaration */
#define FUNC(rettype, memclass) rettype memclass

/* [SWS_COMPILER_00061] */
/** \brief Indirection for function returning pointer to constant */
#define FUNC_P2CONST(rettype, ptrclass, memclass) const rettype * memclass

/* [SWS_COMPILER_00063] */
/** \brief Indirection for function returning pointer to variable */
#define FUNC_P2VAR(rettype, ptrclass, memclass) rettype * memclass

/* [SWS_COMPILER_00006] */
/** \brief Indirection for pointer to variable */
#define P2VAR(ptrtype, memclass, ptrclass) ptrtype * memclass

/* [SWS_COMPILER_00013] */
/** \brief Indirection for pointer to constant */
#define P2CONST(ptrtype, memclass, ptrclass) const ptrtype * memclass

/* [SWS_COMPILER_00031] */
/** \brief Indirection for constant pointer to variable */
#define CONSTP2VAR(ptrtype, memclass, ptrclass) ptrtype * const memclass

/* [SWS_COMPILER_00032] */
/** \brief Indirection for constant pointer to constant */
#define CONSTP2CONST(ptrtype, memclass, ptrclass) const ptrtype * const memclass

/* [SWS_COMPILER_00039] */
/** \brief Indirection for pointer to function */
#define P2FUNC(rettype, ptrclass, fctname) rettype (*fctname)

/* [SWS_COMPILER_00065] */
/** \brief Indirection for constant pointer to function */
#define CONSTP2FUNC(rettype, ptrclass, fctname) rettype (* const fctname)

/* [SWS_COMPILER_00023] */
/** \brief Indirection for constant */
#define CONST(consttype, memclass) const consttype memclass

/* [SWS_COMPILER_00026] */
/** \brief Indirection for variable */
#define VAR(vartype, memclass) vartype memclass


/*=============================================================================
|                       VERSION CHECK                                         |
=============================================================================*/
/* SW version check of *.h/c of the own module */
#if ((COMPILER_SW_MAJOR_VERSION != COMPILER_SW_MAJOR_VERSION_CFG) || \
     (COMPILER_SW_MINOR_VERSION != COMPILER_SW_MINOR_VERSION_CFG) || \
     (COMPILER_SW_PATCH_VERSION != COMPILER_SW_PATCH_VERSION_CFG))
 #error "The SW version number of Compiler.h and Compiler_Cfg.h are different."
#endif

#ifdef __cplusplus
}
#endif

#endif /* COMPILER_H_ */
