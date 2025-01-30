/**
 ******************************************************************************
 *  @file
 *
 *  @brief    File to hold common structure, enums, macros and functions for
 *            VXLIB
 *
 *  @version  0.1 - Jan 2018 : Initial Version
 *
 *******************************************************************************
 */

#ifndef COMMON_VXLIB_TYPES_H_
#define COMMON_VXLIB_TYPES_H_ 1

/* This header is needed to be included in OpenCL programs which link
 * against VXLIB, but OpenCL doesn't need the following headers */
#ifndef __OPENCL_VERSION__
#include <inttypes.h>
#include <stddef.h> // for NULL
#include <stdint.h>
#include <stdio.h> // for printf
#include <stdlib.h>
#endif
#include <c7x.h> // for streaming engine, streaming address gen.

#include "TI_platforms.h"
#include "VXLIB_bufParams.h"

#include "c71/VXLIB_defines.h"

#include "c7120/VXLIB_defines.h"
#include "c7120/VXLIB_types.h"

///* ---------------------------------------------------------------- */
///* Desire C namespace for these defines/includes                    */
///* ---------------------------------------------------------------- */
#if !defined(VXLIB_DEBUGPRINT)
#define VXLIB_DEBUGPRINT 0 //!< Enable debug printf statements
#endif

#if (defined(_HOST_BUILD) && (VXLIB_COMPILER_VERSION < 1003999))
#define VXLIB_const
#else
#define VXLIB_const const
#endif

#define __PRAGMA(x) _Pragma(#x)
#ifdef __cplusplus
#define __INLINE_FUNC(x) __PRAGMA(FUNC_ALWAYS_INLINE)
#else
#define __INLINE_FUNC(x) __PRAGMA(FUNC_ALWAYS_INLINE(x))
#endif

/* ---------------------------------------------------------------- */
/*  MISRAC Rule 4.9(DEFINE.FUNC) Deviation: The following two       */
/*  function-like macros do not have equivalent function            */
/*  implementations.                                                */
/* ---------------------------------------------------------------- */
#if defined(EVM_TEST)
#define VXLIB_PRINTF(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)

#if VXLIB_DEBUGPRINT > 0
#define VXLIB_DEBUGPRINTFN(N, fmt, ...)                                                                                \
   do {                                                                                                                \
      if (VXLIB_DEBUGPRINT >= (N)) {                                                                                   \
         fprintf(stdout, "VXLIB debug %s - %d: " fmt, __FUNCTION__, __LINE__, __VA_ARGS__);                            \
      }                                                                                                                \
   } while (0)

#else // VXLIB_DEBUGPRINT == 0

#define VXLIB_DEBUGPRINTFN(N, fmt, ...)
// #define VXLIB_PRINTF(fmt, ...)
#endif // #if VXLIB_DEBUGPRINT

#else

#if VXLIB_DEBUGPRINT > 0
#define VXLIB_DEBUGPRINTFN(N, fmt, ...)                                                                                \
   if (VXLIB_DEBUGPRINT >= (N)) {                                                                                      \
      fprintf(stderr, "VXLIB debug %s - %d: " fmt, __FUNCTION__, __LINE__, __VA_ARGS__);                               \
   }

#define VXLIB_PRINTF(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)

#else // VXLIB_DEBUGPRINT == 0

#define VXLIB_DEBUGPRINTFN(N, fmt, ...)
#define VXLIB_PRINTF(fmt, ...)

#endif // #if VXLIB_DEBUGPRINT
#endif // #if defined(EVM_TEST)

#if defined(VXLIB_MESSAGES)
// if enabled, display messages to the library user
// VXLIB_MESSAGES should only be defined when TARGET_BUILD=debug or
// CHECKPARAMS=1
#define VXLIB_MESG(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
#else
#define VXLIB_MESG(fmt, ...)
#endif // #if defined(VXLIB_MESSAGES)

/* Original implementation that had the benefit of always being compiled and
thus receiving error checking.  However, the fprintf generates a MISRA-C
violation.
#define VXLIB_DEBUGPRINTFN(N, fmt, ...)                                       \
  do {                                                                         \
    if (VXLIB_DEBUGPRINT >= (N)) {                                            \
      fprintf(stderr, "VXLIB debug %s - %d: " fmt, __FUNCTION__, __LINE__,    \
              __VA_ARGS__);                                                    \
    }                                                                          \
  } while (0)

#define VXLIB_PRINTF(fmt, ...)                                                \
   do {                                                                        \
       fprintf(stderr, fmt, __VA_ARGS__);                                      \
   } while (0)
#endif
 */

// switch to enable or disable static inline for VXLIB functions defined in .c
// files (so not many functions)
#define VXLIB_STATIC_INLINE static inline

#ifdef __cplusplus
#include <c7x_scalable.h> // for device scalability

extern const c7x::uchar_vec  VXLIB_zeroVect_uchar_vec;
extern const c7x::ushort_vec VXLIB_zeroVect_ushort_vec;
extern const c7x::uchar_vec  VXLIB_vperm_data_0_63;

template <int32_t dType> struct VXLIB_element_type {
   using type = uint8_t;
};

template <> struct VXLIB_element_type<VXLIB_UINT8> {
   using type = uint8_t;
};

template <> struct VXLIB_element_type<VXLIB_INT8> {
   using type = int8_t;
};

template <> struct VXLIB_element_type<VXLIB_UINT16> {
   using type = uint16_t;
};

template <> struct VXLIB_element_type<VXLIB_INT16> {
   using type = int16_t;
};

template <> struct VXLIB_element_type<VXLIB_UINT32> {
   using type = uint32_t;
};

template <> struct VXLIB_element_type<VXLIB_INT32> {
   using type = int32_t;
};

template <int32_t dType> struct VXLIB_max_simd {
   static constexpr size_t value = c7x::max_simd<typename VXLIB_element_type<dType>::type>::value;
};

template <int32_t dType> struct VXLIB_vec_type {
   using type = c7x::char_vec;
};

template <> struct VXLIB_vec_type<VXLIB_UINT8> {
   using type = c7x::uchar_vec;
};

template <> struct VXLIB_vec_type<VXLIB_INT8> {
   using type = c7x::char_vec;
};

template <> struct VXLIB_vec_type<VXLIB_UINT16> {
   using type = c7x::ushort_vec;
};

template <> struct VXLIB_vec_type<VXLIB_INT16> {
   using type = c7x::ushort_vec;
};

template <> struct VXLIB_vec_type<VXLIB_UINT32> {
   using type = c7x::uint_vec;
};

template <> struct VXLIB_vec_type<VXLIB_INT32> {
   using type = c7x::int_vec;
};

extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup VXLIB_COMMON Common definitions
 * \brief This module consists of definitions (macros, structures, utility
 *        functions) that are commonly applicable to all VXLIB kernels.
 * \details
 */

/* @{ */

/*******************************************************************************
 *
 * TYPEDEFS
 *
 ******************************************************************************/

typedef double VXLIB_D64; //!< Double precision floating point
typedef float  VXLIB_F32; //!< Single precision floating point

/** \brief The enumeration of all status codes. */
typedef enum {
   VXLIB_SUCCESS                      = 0, /**< 0 => No error */
   VXLIB_ERR_FAILURE                  = 1, /**< 1 => Unspecified error */
   VXLIB_ERR_INVALID_VALUE            = 2, /**< 2 => Invalid parameter value */
   VXLIB_ERR_INVALID_TYPE             = 3, /**< 3 => Invalid parameter type (VXLIB_data_type_e data_type) */
   VXLIB_ERR_INVALID_DIMENSION        = 4, /**< 4 => Dimension parameter (width/height) is too big/small */
   VXLIB_ERR_NULL_POINTER             = 5, /**< 5 => Unsupported null pointer condition */
   VXLIB_ERR_NOT_IMPLEMENTED          = 6, /**< 6 => Parameter configuration is not supported/implemented */
   VXLIB_ERR_NOT_EQUAL_WIDTH_STRIDE   = 7, /**< 7 => Stride should be equal to width * element size */
   VXLIB_ERR_NOT_ALIGNED_PTRS_STRIDES = 8, /**< 8 => Pointers and stride values are not aligned to documented
                                               value  */
   VXLIB_ERR_NOT_ALIGNED_WIDTHS = 9,       /**< 9 => Width values are not aligned to documented value */
   VXLIB_ERR_BUFFER_TOO_SMALL   = 10,      /**< 10 => Buffer size not large enough */
   VXLIB_ERR_INVALID_ACTIVATION = 11,      /**< 11 => Activation selection incompatible with data type */
   VXLIB_ERR_INVALID_SHIFT      = 12,      /**< 12 => Requested shift amount is not valid */
   VXLIB_ERROR_MAX              = 13,

   // VXLIB_convolve
   VXLIB_ERR_CONVOLVE_PADDED_NOT_IMPLEMENTED   = 1000, /**< 1000 => Padded implementation not supported */
   VXLIB_ERR_CONVOLVE_INVALID_TYPE_COMBINATION = 1001, /**< 1001 => data type combination is not supported */
   VXLIB_ERR_CONVOLVE_INVALID_DIMENSION = 1002, /**< 1002 => dimension combinations are invalid for given method */
   VXLIB_ERR_CONVOLVE_INVALID_SHIFT     = 1003, /**< 1003 => shift value is out of bounds */

} VXLIB_STATUS_NAME;

typedef VXLIB_STATUS_NAME VXLIB_STATUS; //!< Return value for VXLIB functions

typedef void *VXLIB_kernelHandle; //!< Handle type for VXLIB operations

/** \brief  Enumeration for the style of function implementation. */
typedef enum {
   VXLIB_FUNCTION_NATC = 0,  /**< Natural C implementation of the function */
   VXLIB_FUNCTION_OPTIMIZED, /**< Optimized C implementation of the function for
                                  C7x architecture*/
   VXLIB_FUNCTION_MAX = 128
} VXLIB_FUNCTION_STYLE;

/** \brief  Enumeration for the overflow policy. */
typedef enum {
   VXLIB_WRAP = 0, /**< Wrap the result of the operation */
   VXLIB_SATURATE, /**< Saturate the result of the operation */
} VXLIB_OVERFLOW_POLICY;

/*******************************************************************************
 *
 * DEFINES
 *
 ******************************************************************************/

#define VXLIB_SE_PARAM_SIZE sizeof(__SE_TEMPLATE_v1) //!< Parameter structure size in bytes
#define VXLIB_SA_PARAM_SIZE sizeof(__SA_TEMPLATE_v1) //!< Parameter structure size in bytes

/*******************************************************************************
 *
 * Macros
 *
 ******************************************************************************/

#define VXLIB_ALIGN_SHIFT_64BYTES 6  //!< Number of bits to shift for 64-byte memory alignment
#define VXLIB_ALIGN_SHIFT_128BYTES 7 //!< Number of bits to shift for 128-byte memory alignment
#define VXLIB_ALIGN_SHIFT_256BYTES 8 //!< Number of bits to shift for 256-byte memory alignment

#define VXLIB_ALIGN_64BYTES (1 << VXLIB_ALIGN_SHIFT_64BYTES)   //!< Align by 64-byte memory alignment
#define VXLIB_ALIGN_128BYTES (1 << VXLIB_ALIGN_SHIFT_128BYTES) //!< Align by 128-byte memory alignment
#define VXLIB_ALIGN_256BYTES (1 << VXLIB_ALIGN_SHIFT_256BYTES) //!< Align by 256-byte memory alignment

#define VXLIB_L2DATA_ALIGN_SHIFT VXLIB_ALIGN_SHIFT_64BYTES //!< Set the default L2 data alignment

/** @brief Macro that specifies the alignment of data buffers in L2 memory for
 * optimal performance */
#define VXLIB_L2DATA_ALIGNMENT (((uint32_t) 1) << ((uint32_t) VXLIB_L2DATA_ALIGN_SHIFT))

/******************************************************************************
 *
 *  Do not document these in the User Guide
 ******************************************************************************/

/**
 * @cond
 */
/* @{ */

/* ---------------------------------------------------------------- */
/*  MISRAC Rule 4.9(DEFINE.FUNC) Deviation: The advisory is not     */
/*  being addressed                                                 */
/* ---------------------------------------------------------------- */
// remove asm comments for Loki testing as they may add cycles
#if defined(QT_TEST) || defined(RTL_TEST) || defined(EVM_TEST)
#if defined(_HOST_BUILD)
#define VXLIB_asm(string) ;
#else
#define VXLIB_asm(string)                                                                                              \
   ;                                                                                                                   \
   asm(string);
#endif // _HOST_BUILD
#else
#define VXLIB_asm(string) ;
#endif

/* ---------------------------------------------------------------- */
/*  MISRAC Rule 4.9(DEFINE.FUNC) Deviation: The advisory is not     */
/*  being addressed  so as not to lose portability across different */
/*  platforms.                                                      */
/* ---------------------------------------------------------------- */
// cl7x unroll attributes not recognized by gcc/gpp compiler and generate
// warnings. Remove them with macro
#if defined(_HOST_BUILD)
#define str(x) #x
#define xstr(x) str(x)
#ifdef WIN32
#define __attribute__()
#define VXLIB_UNROLL(count)
#else
#define VXLIB_UNROLL(count) _Pragma(str(UNROLL(count)))
#endif
#else
#define VXLIB_UNROLL(COUNT) [[TI::unroll(COUNT)]]
#endif

#if defined(_HOST_BUILD)
#ifdef WIN32
#define __attribute__()
#define VXLIB_MUST_ITERATE(initial, max, multiple)
#else
#define VXLIB_MUST_ITERATE(initial, max, multiple) _Pragma(str(MUST_ITERATE(initial, max, multiple)))
#endif
#else
#define VXLIB_MUST_ITERATE(INTIAL, MAX, MULTIPLE) [[TI::must_iterate(INTIAL, MAX, MULTIPLE)]]
#endif

#ifndef VXLIB_NUMBER_RANDOM_DIM_TESTS
#define VXLIB_NUMBER_RANDOM_DIM_TESTS 25
#endif

#if defined(PERFORMANCE_TEST)
#define VXLIB_PERFORMANCE_TEST_PATTERN SEQUENTIAL
#else
#define VXLIB_PERFORMANCE_TEST_PATTERN RANDOM_SIGNED
#endif

#define VXLIB_TEST_OUTPUT_HEAP 0
#if defined(_HOST_BUILD)
// Valgrind works better when output is in the heap (it can't track statically
// allocated memory), so in host emulation mode, place test outputs in the heap
// rather than statically allocated MSMC
#define VXLIB_TEST_OUTPUT_MSMC VXLIB_TEST_OUTPUT_HEAP
#else
#define VXLIB_TEST_OUTPUT_MSMC 1
#endif

#define STRIDE_OPT_4CYCLE 1

/*******************************************************************************
 *
 * MACROS
 *
 ******************************************************************************/

/* ---------------------------------------------------------------- */
/*  MISRAC Rule 4.9(DEFINE.FUNC) Deviation: The following           */
/*  function-like macros are intended to be used across different   */
/*  data types.                                                     */
/* ---------------------------------------------------------------- */
#define VXLIB_min(x, y) (((x) < (y)) ? (x) : (y)) /**< A macro to return the minimum of 2 values. */
#define VXLIB_max(x, y) (((x) < (y)) ? (y) : (x)) /**< A macro to return the maximum of 2 values. */
#define VXLIB_ceilingDiv(x, y)                                                                                         \
   (((x) + (y) -1) / (y)) /**< A macro to return the ceiling of the division of two integers. */

/* @} */
/** @endcond  */

#ifdef __cplusplus
}

#endif /* __cplusplus */

#endif /* COMMON_VXLIB_TYPES_H_ */

/* ======================================================================== */
/*  End of file:  VXLIB_types.h                                             */
/* ======================================================================== */
