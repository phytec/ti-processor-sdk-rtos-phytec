/*
 *  Copyright (C) 2002-2020 Texas Instruments Incorporated
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
 *   \file  csl_types.h
 *
 *   \brief  This file contains the Register Desciptions for CSL types
 */

#ifndef CSL_TYPES_H
#define CSL_TYPES_H

/**
 *  \anchor CSL_Type_t
 *  \name CSL Types
 *
 *  CSL data types.
 *
 *  @{
 */
#ifndef TRUE
#define TRUE		(1)
#define FALSE		(0)
#endif

/* Define CSL_TRUE/CSL_FALSE to go with Uint32 */
#ifndef CSL_TRUE
#define CSL_TRUE    (1U)
#endif
#ifndef CSL_FALSE
#define CSL_FALSE   (0U)
#endif

/*! @} */


/** \brief CSL error type */
typedef sint32 CSL_ErrType_t;

/**
 *  \anchor CSL_ErrType_t
 *  \name CSL Error Types
 *
 *  CSL function return error codes.
 *
 *  @{
 */
#define CSL_PASS                        ( (sint32) (0))
#define CSL_EFAIL                       (-(sint32) (1))
#define CSL_EBADARGS                    (-(sint32) (2))
#define CSL_EINVALID_PARAMS             (-(sint32) (3))
#define CSL_ETIMEOUT                    (-(sint32) (4))
#define CSL_EOUT_OF_RANGE               (-(sint32) (5))
#define CSL_EUNSUPPORTED_CMD            (-(sint32) (6))
#define CSL_EUNSUPPORTED_OPS            (-(sint32) (7))
#define CSL_EALLOC                      (-(sint32) (8))
/*! @} */

/** \brief Define NULL if not defined */
#ifndef NULL
#define NULL            ((void*)0)
#endif

#ifndef NULL_PTR
#define NULL_PTR ((void *)0)
#endif

#endif /* CSL_TYPES_H */
