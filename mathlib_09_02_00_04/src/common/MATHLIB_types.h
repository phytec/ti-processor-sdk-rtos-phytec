/******************************************************************************
 * Copyright (C) 2023 Texas Instruments Incorporated - http://www.ti.com/
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

#ifndef MATHLIB_TYPES_H_
#define MATHLIB_TYPES_H_ 1

/******************************************************************************/
/*                                                                            */
/* Includes                                                                   */
/*                                                                            */
/******************************************************************************/

#include <c7x.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * \defgroup MATHLIB_COMMON Common
 * \brief This module consists of definitions (macros, structures, utility
 *        functions) that are commonly applicable to all MATHLIB kernels.
 * \details
 */

/******************************************************************************/
/*                                                                            */
/* Defines                                                                    */
/*                                                                            */
/******************************************************************************/

#define MATHLIB_TEST_OUTPUT_HEAP 0
#if defined(_HOST_BUILD)
// Valgrind works better when output is in the heap (it
// can't track statically allocated memory), so
// in host emulation mode, place test outputs in the heap rather than statically
// allocated MSMC
#define MATHLIB_TEST_OUTPUT_MSMC MATHLIB_TEST_OUTPUT_HEAP
#else
#define MATHLIB_TEST_OUTPUT_MSMC 1
#endif

#ifdef _HOST_BUILD
#define restrict
#endif

// debug print macro
#if !defined(MATHLIB_DEBUGPRINT)
#define MATHLIB_DEBUGPRINT 0 //!< Enable debug printf statements
#endif

#define MATHLIB_PRINTF(fmt, ...)                                                                                       \
   do {                                                                                                                \
      fprintf(stdout, fmt, __VA_ARGS__);                                                                               \
   } while (0)

#if MATHLIB_DEBUGPRINT > 0
#define MATHLIB_DEBUGPRINTFN(N, fmt, ...)                                                                              \
   do {                                                                                                                \
      if (MATHLIB_DEBUGPRINT >= (N)) {                                                                                 \
         fprintf(stdout, "MATHLIB debug %s - %d: " fmt, __FUNCTION__, __LINE__, __VA_ARGS__);                          \
      }                                                                                                                \
   } while (0)

#else // MATHLIB_DEBUGPRINT == 0

#define MATHLIB_DEBUGPRINTFN(N, fmt, ...)

#endif // #if MATHLIB_DEBUGPRINT

#ifdef __cplusplus
#include <c7x_scalable.h> // for device scalability
extern "C" {
#endif /* __cplusplus */

/**
 * \ingroup MATHLIB_COMMON
 */

/**
 * \defgroup MATHLIB_TYPES Common typedefs
 * \brief This module consists of common typefeds to all MATHLIB kernels.
 */

/**@{*/

/******************************************************************************/
/*                                                                            */
/* Typedes                                                                    */
/*                                                                            */
/******************************************************************************/

/**
 @brief The enumeration of all status codes.
*/

typedef enum {
   MATHLIB_SUCCESS                      = 0, /*!< 0 => No error */
   MATHLIB_ERR_FAILURE                  = 1, /*!< 1 => Unspecified error */
   MATHLIB_ERR_INVALID_VALUE            = 2, /*!< 2 => Invalid parameter value */
   MATHLIB_ERR_INVALID_TYPE             = 3, /*!< 3 => Invalid parameter type (MATHLIB_data_type_e data_type) */
   MATHLIB_ERR_INVALID_DIMENSION        = 4, /*!< 4 => Dimension parameter (width/height) is too big/small */
   MATHLIB_ERR_NULL_POINTER             = 5, /*!< 5 => Unsupported null pointer condition */
   MATHLIB_ERR_NOT_IMPLEMENTED          = 6, /*!< 6 => Parameter configuration is not supported/implemented */
   MATHLIB_ERR_NOT_EQUAL_WIDTH_STRIDE   = 7, /*!< 7 => Stride should be equal to width * element size */
   MATHLIB_ERR_NOT_ALIGNED_PTRS_STRIDES = 8, /*!< 8 => Pointers and stride values are not aligned to documented
                                               value  */
   MATHLIB_ERR_NOT_ALIGNED_WIDTHS = 9,       /*!< 9 => Width values are not aligned to documented value */
   MATHLIB_ERR_BUFFER_TOO_SMALL   = 10,      /*!< 10 => Buffer size not large enough */
   MATHLIB_ERR_INVALID_ACTIVATION = 11,      /*!< 11 => Activation selection incompatible with data type */
   MATHLIB_ERR_INVALID_SHIFT      = 12,      /*!< 12 => Requested shift amount is not valid */
   MATHLIB_ERROR_MAX              = 13,
} MATHLIB_STATUS_NAME;

typedef MATHLIB_STATUS_NAME MATHLIB_STATUS; //!< Return value for MATHLIB functions

/**
 @struct MATHLIB_SESA1DSequential_t
 @brief Structure containing the initialization parameters for kernels involving 1D sequential access
*/

typedef struct {
   bool             callCheckParams = false; ///< flag to call the checkparams function of given kernel
   bool             callInit        = true;  ///< flag to call the initialization function of given kernel
   __SE_TEMPLATE_v1 se0Params;               ///< struct to hold SE0 parameters
   __SE_TEMPLATE_v1 se1Params;               ///< struct to hold SE1 parameters
   __SA_TEMPLATE_v1 sa0Params;               ///< struct to hold SA0 parameters
   size_t           blockSize = 0;           ///<  placeholder for iteration count of compute loop
} MATHLIB_SESA1DSequential_t;

/**
 @enum MATHLIB_data_type_e
 @brief Enum of all datatypes
*/

typedef enum _MATHLIB_data_type_e {

   // standard fixed-point integer datatypes
   MATHLIB_INT8   = 0, /*!<  Signed 8-bit integer */
   MATHLIB_INT16  = 1, /*!<  Signed 16-bit integer */
   MATHLIB_INT32  = 2, /*!<  Signed 32-bit integer */
   MATHLIB_INT64  = 3, /*!<  Signed 64-bit integer */
   MATHLIB_INT128 = 4, /*!<  Signed 128-bit integer */

   // standard fixed-point unsigned integer datatypes
   MATHLIB_UINT8   = 5,  /*!<  Unsigned 8-bit integer */
   MATHLIB_UINT16  = 6,  /*!<  Unsigned 16-bit integer */
   MATHLIB_UINT24  = 7,  /*!<  Unsigned 24-bit integer */
   MATHLIB_UINT32  = 8,  /*!<  Unsigned 32-bit integer */
   MATHLIB_UINT64  = 9,  /*!<  Unsigned 64-bit integer */
   MATHLIB_UINT128 = 10, /*!<  Unsigned 128-bit integer */

   // standard floating-point datatypes
   MATHLIB_FLOAT16 = 11, /*!<  16-bit floating point number */
   MATHLIB_FLOAT32 = 12, /*!<  32-bit floating point number */
   MATHLIB_FLOAT64 = 13  /*!<  64-bit floating point number */

} MATHLIB_data_type_e;

/** @}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* MATHLIB_TYPES_H_ */
