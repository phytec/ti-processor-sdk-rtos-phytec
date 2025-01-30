/******************************************************************************
 *                                                                             *
 * module name       :FFTLIB                                                   *
 *                                                                             *
 * module descripton :Matrix Multiply Accelerator Library module for C7x+MMA   *
 *                                                                             *
 * Copyright (C) 2017-2018 Texas Instruments Incorporated - http://www.ti.com/ *
 * ALL RIGHTS RESERVED                                                         *
 *                                                                             *
 ******************************************************************************/

/**
 ******************************************************************************
 *  @file
 *
 *  @brief    File to hold common structure, enums, macros and functions for
 *            FFTLIB
 *
 *  @version  0.1 - Jan 2018 : Initial Version
 *
 *******************************************************************************
 */
#ifndef COMMON_FFTLIB_TYPES_H_
#define COMMON_FFTLIB_TYPES_H_ 1

/* This header is needed to be included in OpenCL programs which link
 * against FFTLIB, but OpenCL doesn't need the following headers */
#ifndef __OPENCL_VERSION__
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stddef.h> // for NULL
#include <stdio.h>  // for printf
#endif
#include <c7x.h> // for streaming engine, streaming address gen.


#include "FFTLIB_bufParams.h"
#include "TI_platforms.h"

#include "c71/FFTLIB_defines.h"

#include "c7120/FFTLIB_types.h"
#include "c7120/FFTLIB_defines.h"

///* ---------------------------------------------------------------- */
///* Desire C namespace for these defines/includes                    */
///* ---------------------------------------------------------------- */
#if !defined(FFTLIB_DEBUGPRINT)
#define FFTLIB_DEBUGPRINT 0 //!< Enable debug printf statements
#endif

#if (defined(_HOST_BUILD) && (FFTLIB_COMPILER_VERSION < 1003999))
#define FFTLIB_const
#else
#define FFTLIB_const const
#endif

#define __PRAGMA(x) _Pragma(#x)
#ifdef __cplusplus
#  define __INLINE_FUNC(x) __PRAGMA(FUNC_ALWAYS_INLINE)
#else
#  define __INLINE_FUNC(x) __PRAGMA(FUNC_ALWAYS_INLINE(x))
#endif

/* ---------------------------------------------------------------- */
/*  MISRAC Rule 4.9(DEFINE.FUNC) Deviation: The following two       */
/*  function-like macros do not have equivalent function            */
/*  implementations.                                                */
/* ---------------------------------------------------------------- */
#if defined(EVM_TEST)
#define FFTLIB_PRINTF(fmt,...)                                                 \
   do {                                                                        \
      fprintf(stdout, fmt, __VA_ARGS__);                                       \
   } while (0)

#if FFTLIB_DEBUGPRINT > 0
#define FFTLIB_DEBUGPRINTFN(N, fmt, ...)                                       \
  do {                                                                         \
    if (FFTLIB_DEBUGPRINT >= (N)) {                                            \
      fprintf(stdout, "FFTLIB debug %s - %d: " fmt, __FUNCTION__, __LINE__,    \
              __VA_ARGS__);                                                    \
    }                                                                          \
  } while (0)

#else // FFTLIB_DEBUGPRINT == 0

#define FFTLIB_DEBUGPRINTFN(N, fmt, ...)
//#define FFTLIB_PRINTF(fmt, ...)
#endif // #if FFTLIB_DEBUGPRINT

#else

#if FFTLIB_DEBUGPRINT > 0
#define FFTLIB_DEBUGPRINTFN(N, fmt, ...)                                       \
    if (FFTLIB_DEBUGPRINT >= (N)) {                                            \
      fprintf(stderr, "FFTLIB debug %s - %d: " fmt, __FUNCTION__, __LINE__,    \
              __VA_ARGS__);                                                    \
    }                                                                          \

#define FFTLIB_PRINTF(fmt, ...)                                                \
   fprintf(stderr, fmt, __VA_ARGS__)

#else // FFTLIB_DEBUGPRINT == 0

#define FFTLIB_DEBUGPRINTFN(N, fmt, ...)
#define FFTLIB_PRINTF(fmt, ...)

#endif // #if FFTLIB_DEBUGPRINT
#endif // #if defined(EVM_TEST)

#if defined(FFTLIB_MESSAGES)
// if enabled, display messages to the library user
// FFTLIB_MESSAGES should only be defined when TARGET_BUILD=debug or CHECKPARAMS=1
#define FFTLIB_MESG(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
#else
#define FFTLIB_MESG(fmt, ...)
#endif //#if defined(FFTLIB_MESSAGES)

/* Original implementation that had the benefit of always being compiled and thus
 receiving error checking.  However, the fprintf generates a MISRA-C violation.
#define FFTLIB_DEBUGPRINTFN(N, fmt, ...)                                       \
  do {                                                                         \
    if (FFTLIB_DEBUGPRINT >= (N)) {                                            \
      fprintf(stderr, "FFTLIB debug %s - %d: " fmt, __FUNCTION__, __LINE__,    \
              __VA_ARGS__);                                                    \
    }                                                                          \
  } while (0)

#define FFTLIB_PRINTF(fmt, ...)                                                \
   do {                                                                        \
       fprintf(stderr, fmt, __VA_ARGS__);                                      \
   } while (0)
#endif
 */


#define FFTLIB_SOFT_MMA_RESET 0

// switch to enable or disable static inline for FFTLIB functions defined in .c files (so not many functions)
#define FFTLIB_STATIC_INLINE static inline


#ifdef __cplusplus
#include <c7x_scalable.h> // for device scalability

extern const c7x::uchar_vec FFTLIB_zeroVect_uchar_vec;
extern const c7x::ushort_vec FFTLIB_zeroVect_ushort_vec;
extern const c7x::uchar_vec FFTLIB_vperm_data_0_63;

extern "C" {
#endif /* __cplusplus */

/*!
 * \defgroup FFTLIB_COMMON Common definitions
 * \brief This module consists of definitions (macros, structures, utility
 *        functions) that are commonly applicable to all FFTLIB kernels.
 * \details
 */

/* @{ */

/*******************************************************************************
 *
 * TYPEDEFS
 *
 ******************************************************************************/

typedef double FFTLIB_D64; //!< Double precision floating point
typedef float FFTLIB_F32;  //!< Single precision floating point

/*! \brief The enumeration of all status codes. */
typedef enum {
   FFTLIB_SUCCESS = 0,           /*!< 0 => No error */
   FFTLIB_ERR_FAILURE = 1,       /*!< 1 => Unspecified error */
   FFTLIB_ERR_INVALID_VALUE = 2, /*!< 2 => Invalid parameter value */
   FFTLIB_ERR_INVALID_TYPE  = 3, /*!< 3 => Invalid parameter type (FFTLIB_data_type_e data_type) */
   FFTLIB_ERR_INVALID_DIMENSION = 4, /*!< 4 => Dimension parameter (width/height) is too big/small */
   FFTLIB_ERR_NULL_POINTER = 5, /*!< 5 => Unsupported null pointer condition */
   FFTLIB_ERR_NOT_IMPLEMENTED = 6, /*!< 6 => Parameter configuration is not supported/implemented */
   FFTLIB_ERR_NOT_EQUAL_WIDTH_STRIDE = 7, /*!< 7 => Stride should be equal to width * element size */
   FFTLIB_ERR_NOT_ALIGNED_PTRS_STRIDES = 8, /*!< 8 => Pointers and stride values are not aligned to documented value  */
   FFTLIB_ERR_NOT_ALIGNED_WIDTHS = 9, /*!< 9 => Width values are not aligned to documented value */
   FFTLIB_ERR_BUFFER_TOO_SMALL = 10, /*!< 10 => Buffer size not large enough */
   FFTLIB_ERR_INVALID_ACTIVATION = 11, /*!< 11 => Activation selection incompatible with data type */
   FFTLIB_ERR_INVALID_SHIFT = 12,      /*!< 12 => Requested shift amount is not valid */
   FFTLIB_ERROR_MAX = 13,

   // FFTLIB_LINALG_matrixMatrixMultiply
   FFTLIB_ERR_MATRIX_MATRIX_MULTIPLY_UNSUPPORTED_INPUT_TYPE  = 10000, /*!< 10000 => Unsupported input data type */
   FFTLIB_ERR_MATRIX_MATRIX_MULTIPLY_UNSUPPORTED_OUTPUT_TYPE = 10001, /*!< 10001 => Unsupported output data type */
   FFTLIB_ERR_MATRIX_MATRIX_MULTIPLY_UNSUPPORTED_TYPE_COMBINATION = 10002,  /*!< 10002 => Unsupported combination of input and output data type */
   // FFTLIB_LINALG_matrixMatrixMultiplyAccumulate
   FFTLIB_ERR_MATRIX_MATRIX_MULTIPLY_ACCUMULATE_UNSUPPORTED_INPUT_TYPE  = 11000, /*!< 11000 => Unsupported input data type */
   FFTLIB_ERR_MATRIX_MATRIX_MULTIPLY_ACCUMULATE_UNSUPPORTED_OUTPUT_TYPE = 11001, /*!< 11001 => Unsupported output data type */
   FFTLIB_ERR_MATRIX_MATRIX_MULTIPLY_ACCUMULATE_UNSUPPORTED_TYPE_COMBINATION = 11002,  /*!< 11002 => Unsupported combination of input and output data type */
   // FFTLIB_LINALG_matrixTranspose
   FFTLIB_ERR_MATRIX_TRANSPOSE_UNSUPPORTED_INPUT_TYPE  = 12000, /*!< 12000 => Unsupported input data type */
   FFTLIB_ERR_MATRIX_TRANSPOSE_UNSUPPORTED_OUTPUT_TYPE = 12001, /*!< 12001 => Unsupported output data type */
   FFTLIB_ERR_MATRIX_TRANSPOSE_UNSUPPORTED_TYPE_COMBINATION = 12002,  /*!< 12002 => Unsupported combination of input and output data type */
   // FFTLIB_LINALG_pointwiseMatrixMatrixMultiply
   FFTLIB_ERR_POINTWISE_MATRIX_MATRIX_MULTIPLY_UNSUPPORTED_INPUT_TYPE  = 13000, /*!<  13000 => Unsupported input data type */
   FFTLIB_ERR_POINTWISE_MATRIX_MATRIX_MULTIPLY_UNSUPPORTED_OUTPUT_TYPE = 13001, /*!<  13001 => Unsupported output data type */
   FFTLIB_ERR_POINTWISE_MATRIX_MATRIX_MULTIPLY_UNSUPPORTED_TYPE_COMBINATION = 13002,  /*!< 13002 => Unsupported combination of input and output data type */
} FFTLIB_STATUS_NAME;

typedef FFTLIB_STATUS_NAME FFTLIB_STATUS; //!< Return value for FFTLIB functions

typedef void *FFTLIB_kernelHandle; //!< Handle type for FFTLIB operations

/*! \brief  Enumeration for the style of function implementation. */
typedef enum {
  FFTLIB_FUNCTION_NATC = 0,  /*!< Natural C implementation of the function */
  FFTLIB_FUNCTION_OPTIMIZED, /*!< Optimized C implementation of the function for
                                the MMA + C7x architecture*/
  FFTLIB_FUNCTION_MAX = 128
} FFTLIB_FUNCTION_STYLE;

/*******************************************************************************
 *
 * DEFINES
 *
 ******************************************************************************/

#define FFTLIB_PARAM_SIZE 128 //!< Parameter structure size in bytes
//#define FFTLIB_BYTE_WIDTH 64  //!< MMA width in bytes
#define FFTLIB_MMA_ACCUMULATOR_BYTE_WIDTH_8_BIT  4  //!< MMA accumulator width in bytes for 8 bit output data width
#define FFTLIB_MMA_ACCUMULATOR_BYTE_WIDTH_16_BIT 8  //!< MMA accumulator width in bytes for 16 bit output data width
#define FFTLIB_MMA_ACCUMULATOR_BYTE_WIDTH_32_BIT 16 //!< MMA accumulator width in bytes for 32 bit output data width


/*******************************************************************************
 *
 * Macros
 *
 ******************************************************************************/

#define FFTLIB_ALIGN_SHIFT_64BYTES                                             \
  6 //!< Number of bits to shift for 64-byte memory alignment
#define FFTLIB_ALIGN_SHIFT_128BYTES                                            \
  7 //!< Number of bits to shift for 128-byte memory alignment
#define FFTLIB_ALIGN_SHIFT_256BYTES                                            \
  8 //!< Number of bits to shift for 256-byte memory alignment

#define FFTLIB_ALIGN_64BYTES                                                   \
  (1 << FFTLIB_ALIGN_SHIFT_64BYTES) //!< Align by 64-byte memory alignment
#define FFTLIB_ALIGN_128BYTES                                                  \
  (1 << FFTLIB_ALIGN_SHIFT_128BYTES) //!< Align by 128-byte memory alignment
#define FFTLIB_ALIGN_256BYTES                                                  \
  (1 << FFTLIB_ALIGN_SHIFT_256BYTES) //!< Align by 256-byte memory alignment

#define FFTLIB_L2DATA_ALIGN_SHIFT                                              \
  FFTLIB_ALIGN_SHIFT_64BYTES //!< Set the default L2 data alignment

/*! @brief Macro that specifies the alignment of data buffers in L2 memory for
 * optimal performance */
#define FFTLIB_L2DATA_ALIGNMENT (((uint32_t)1) << ((uint32_t)FFTLIB_L2DATA_ALIGN_SHIFT))

/* ---------------------------------------------------------------- */
/*  MISRAC Rule 4.9(DEFINE.FUNC) Deviation: The advisory is not     */
/*  being addressed                                                 */
/* ---------------------------------------------------------------- */
/*! @brief Macro function that computes the minimum stride for the specified
 * number of bytes and alignment */
/* #define FFTLIB_CALC_STRIDE(BYTES, ALIGN_SHIFT)                                 \ */
/*   (((((BYTES)-1) >> (ALIGN_SHIFT)) + 1) << (ALIGN_SHIFT)) */

/* /\*! @brief Macro for FFTLIB shift amounts *\/ */
/* #define FFTLIB_MMA_SIZE_8_BIT_SHIFT                                            \ */
/*   6 //!< Shift for MMA size when data type is 8-bit integers */
/* #define FFTLIB_MMA_SIZE_16_BIT_SHIFT                                           \ */
/*   5 //!< Shift for MMA size when data type is 16-bit integers */
/* #define FFTLIB_MMA_SIZE_32_BIT_SHIFT                                           \ */
/*   4 //!< Shift for MMA size when data type is 32-bit integers */

/* /\*! @brief MMA size as a function of precision *\/ */
/* //#define FFTLIB_MMA_SIZE_1_BYTE    1 */
/* #define FFTLIB_MMA_SIZE_8_BIT                                              \ */
/*   ((uint32_t)(((uint32_t)1)                                                     \ */
/*              << FFTLIB_MMA_SIZE_8_BIT_SHIFT)) //!< Shift for MMA size when data */
/*                                               //!< type is 8-bit integers */
/* #define FFTLIB_MMA_SIZE_16_BIT                                                 \ */
/*   ((uint32_t)(((uint32_t)1)                                                     \ */
/*              << FFTLIB_MMA_SIZE_16_BIT_SHIFT)) //!< Shift for MMA size when data */
/*                                                //!< type is 16-bit integers */
/* #define FFTLIB_MMA_SIZE_32_BIT                                                 \ */
/*   ((int32_t)(((uint32_t)1)                                                     \ */
/*              << FFTLIB_MMA_SIZE_32_BIT_SHIFT)) //!< Shift for MMA size when data */
/*                                                //!< type is 32-bit integers */

/* @} */

/******************************************************************************
 *
 *  Do not document these in the User Guide
 ******************************************************************************/

/*!
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
      #define FFTLIB_asm(string) ;
   #else
      #define FFTLIB_asm(string); asm(string);
   #endif  // _HOST_BUILD
#else
   #define FFTLIB_asm(string) ;
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
#define FFTLIB_UNROLL(count)
#else
#define FFTLIB_UNROLL(count) _Pragma(str(UNROLL(count)))
#endif
#else
#define FFTLIB_UNROLL(COUNT) [[TI::unroll(COUNT)]]
#endif

#if defined(_HOST_BUILD)
#ifdef WIN32
#define __attribute__()
#define FFTLIB_MUST_ITERATE(initial, max, multiple)
#else
#define FFTLIB_MUST_ITERATE(initial, max, multiple) _Pragma(str(MUST_ITERATE(initial, max, multiple)))
#endif
#else
#define FFTLIB_MUST_ITERATE(INTIAL, MAX, MULTIPLE)  [[TI::must_iterate(INTIAL, MAX, MULTIPLE)]]
#endif

#ifndef FFTLIB_NUMBER_RANDOM_DIM_TESTS
#define FFTLIB_NUMBER_RANDOM_DIM_TESTS 25
#endif

#if defined(PERFORMANCE_TEST)
#define FFTLIB_PERFORMANCE_TEST_PATTERN SEQUENTIAL
#else
#define FFTLIB_PERFORMANCE_TEST_PATTERN RANDOM_SIGNED
#endif

#define FFTLIB_TEST_OUTPUT_HEAP 0
#if defined(_HOST_BUILD)
   // Valgrind works better when output is in the heap (it can't track statically allocated memory), so
   // in host emulation mode, place test outputs in the heap rather than statically allocated MSMC
   #define FFTLIB_TEST_OUTPUT_MSMC FFTLIB_TEST_OUTPUT_HEAP
#else
   #define FFTLIB_TEST_OUTPUT_MSMC 1
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
#define FFTLIB_min(x, y)                                                       \
  (((x) < (y)) ? (x) : (y)) /*!< A macro to return the minimum of 2 values. */
#define FFTLIB_max(x, y)                                                       \
  (((x) < (y)) ? (y) : (x)) /*!< A macro to return the maximum of 2 values. */
#define FFTLIB_ceilingDiv(x, y)                                                \
  (((x) + (y)-1) /                                                             \
   (y)) /*!< A macro to return the ceiling of the division of two integers. */


/******************************************************************************
 *
 * COMMON MMA CONIFGURATIONS
 *
 ******************************************************************************/

   extern const FFTLIB_MMA_CONFIG_REG configRegisterStruct_i32s_i32s_o32s;

   extern const FFTLIB_MMA_CONFIG_REG configRegisterStruct_i16s_i16s_o16s;
   extern const FFTLIB_MMA_CONFIG_REG configRegisterStruct_i16s_i16s_o16u;
   extern const FFTLIB_MMA_CONFIG_REG configRegisterStruct_i16u_i16s_o16s;
   extern const FFTLIB_MMA_CONFIG_REG configRegisterStruct_i16u_i16s_o16u;

   extern const FFTLIB_MMA_CONFIG_REG configRegisterStruct_i8s_i8s_o8s;
   extern const FFTLIB_MMA_CONFIG_REG configRegisterStruct_i8s_i8s_o8u;
   extern const FFTLIB_MMA_CONFIG_REG configRegisterStruct_i8u_i8s_o8s;
   extern const FFTLIB_MMA_CONFIG_REG configRegisterStruct_i8u_i8s_o8u;

   extern const __HWA_OFFSET_REG offsetRegStruct_zeros;
   extern const __HWA_OFFSET_REG offsetRegStruct_diagonal_32bit;
   extern const __HWA_OFFSET_REG offsetRegStruct_diagonal_16bit;
   extern const __HWA_OFFSET_REG offsetRegStruct_diagonal_8bit;


/* @} */
/** @endcond  */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* COMMON_FFTLIB_TYPES_H_ */

/* ======================================================================== */
/*  End of file:  FFTLIB_types.h                                             */
/* ======================================================================== */
