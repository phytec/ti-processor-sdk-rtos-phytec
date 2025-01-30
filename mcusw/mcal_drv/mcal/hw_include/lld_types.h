/*  ============================================================================
 *  Copyright (c) Texas Instruments Incorporated 2002, 2003, 2004, 2005, 2006, 2008
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

/**
 *   \file  lld_types.h
 *
 *   \brief  This file contains the Register Desciptions for LLD types
 */

#ifndef LLD_TYPES_H
#define LLD_TYPES_H

#include <hw_include/tistdtypes.h>

/**
 *  \anchor CSL_Type_t
 *  \name CSL Types
 *
 *  CSL data types.
 *
 *  @{
 */
#ifndef TRUE
#define TRUE		((Bool) 1)
#define FALSE		((Bool) 0)
#endif

typedef Int16           LLD_Uid;
typedef Int16           LLD_ModuleId;
typedef Uint32          LLD_Xio;

typedef Uint8           LLD_BitMask8;
typedef Uint16          LLD_BitMask16;
typedef Uint32          LLD_BitMask32;

typedef volatile Uint8  LLD_Reg8;
typedef volatile Uint16 LLD_Reg16;
typedef volatile Uint32 LLD_Reg32;

typedef Int16           LLD_Status;
typedef Int16           LLD_InstNum;
typedef Int16           LLD_ChaNum;

typedef unsigned long long int  LLD_Uint64;
/* @} */

/** \brief LLD open mode */
typedef enum {
  LLD_EXCLUSIVE = 0,
  LLD_SHARED    = 1
} LLD_OpenMode;

/** \brief LLD error type */
typedef int32_t LLD_ErrType_t;

/**
 *  \anchor CSL_ErrType_t
 *  \name CSL Error Types
 *
 *  CSL function return error codes.
 *
 *  @{
 */
#define LLD_PASS                        ( (int32_t) (0))
#define LLD_EFAIL                       (-(int32_t) (1))
#define LLD_EBADARGS                    (-(int32_t) (2))
#define LLD_EINVALID_PARAMS             (-(int32_t) (3))
#define LLD_ETIMEOUT                    (-(int32_t) (4))
#define LLD_EOUT_OF_RANGE               (-(int32_t) (5))
#define LLD_EUNSUPPORTED_CMD            (-(int32_t) (6))
#define LLD_EUNSUPPORTED_OPS            (-(int32_t) (7))
#define LLD_EALLOC                      (-(int32_t) (8))
/* @} */

/** \brief Define NULL if not defined */
#ifndef NULL
#define NULL            ((void*)0)
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


/**
 *  \anchor STW_ErrType_t
 *  \name STW Error Types
 *
 *  CSL function return error codes meant for backward compatibility.
 *
 *  @{
 */
#define LLD_STW_SOK                         (LLD_PASS)
#define LLD_STW_EFAIL                       (LLD_EFAIL)
#define LLD_STW_EBADARGS                    (LLD_EBADARGS)
#define LLD_STW_EINVALID_PARAMS             (LLD_EINVALID_PARAMS)
#define LLD_STW_ETIMEOUT                    (LLD_ETIMEOUT)
#define LLD_STW_EOUT_OF_RANGE               (LLD_EOUT_OF_RANGE)
#define LLD_STW_EUNSUPPORTED_CMD            (LLD_EUNSUPPORTED_CMD)
#define LLD_STW_EUNSUPPORTED_OPS            (LLD_EUNSUPPORTED_OPS)
/* @} */

#endif /* LLD_TYPES_H */
