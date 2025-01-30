/******************************************************************************
 *                                                                             *
 * module name       :MMALIB                                                   *
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
 *            MMALIB
 *
 *  @version  0.1 - Jan 2018 : Initial Version
 *
 *******************************************************************************
 */
#ifndef COMMON_MMALIB_TYPES_H_
#define COMMON_MMALIB_TYPES_H_ 1

/* This header is needed to be included in OpenCL programs which link
 * against MMALIB, but OpenCL doesn't need the following headers */
#ifndef __OPENCL_VERSION__
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stddef.h> // for NULL
#include <stdio.h>  // for printf
#endif
#include <c7x.h> // for streaming engine, streaming address gen.


#include "MMALIB_bufParams.h"
#include "TI_platforms.h"

#include "c71/MMALIB_defines.h"

#include "c7120/MMALIB_types.h"
#include "c7120/MMALIB_defines.h"

///* ---------------------------------------------------------------- */
///* Desire C namespace for these defines/includes                    */
///* ---------------------------------------------------------------- */
#if !defined(MMALIB_DEBUGPRINT)
#define MMALIB_DEBUGPRINT 0 //!< Enable debug printf statements
#endif

#if (defined(_HOST_BUILD) && (MMALIB_COMPILER_VERSION < 1003999))
#define MMALIB_const
#else
#define MMALIB_const const
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
#define MMALIB_PRINTF(fmt,...)                                                 \
   do {                                                                        \
      fprintf(stdout, fmt, __VA_ARGS__);                                       \
   } while (0)

#if MMALIB_DEBUGPRINT > 0
#define MMALIB_DEBUGPRINTFN(N, fmt, ...)                                       \
  do {                                                                         \
    if (MMALIB_DEBUGPRINT >= (N)) {                                            \
      fprintf(stdout, "MMALIB debug %s - %d: " fmt, __FUNCTION__, __LINE__,    \
              __VA_ARGS__);                                                    \
    }                                                                          \
  } while (0)

#else // MMALIB_DEBUGPRINT == 0

#define MMALIB_DEBUGPRINTFN(N, fmt, ...)
//#define MMALIB_PRINTF(fmt, ...)
#endif // #if MMALIB_DEBUGPRINT

#if defined(MMALIB_MESSAGES)
// if enabled, display messages to the library user
// MMALIB_MESSAGES should only be defined when TARGET_BUILD=debug or CHECKPARAMS=1
#define MMALIB_MESG(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
#else
#define MMALIB_MESG(fmt, ...)
#endif //#if defined(MMALIB_MESSAGES)

/* Original implementation that had the benefit of always being compiled and thus
 receiving error checking.  However, the fprintf generates a MISRA-C violation.
#define MMALIB_DEBUGPRINTFN(N, fmt, ...)                                       \
  do {                                                                         \
    if (MMALIB_DEBUGPRINT >= (N)) {                                            \
      fprintf(stderr, "MMALIB debug %s - %d: " fmt, __FUNCTION__, __LINE__,    \
              __VA_ARGS__);                                                    \
    }                                                                          \
  } while (0)

#define MMALIB_PRINTF(fmt, ...)                                                \
   do {                                                                        \
       fprintf(stderr, fmt, __VA_ARGS__);                                      \
   } while (0)
#endif
 */


#define MMALIB_SOFT_MMA_RESET 0

// switch to enable or disable static inline for MMALIB functions defined in .c files (so not many functions)
#define MMALIB_STATIC_INLINE static inline


#ifdef __cplusplus
#include <c7x_scalable.h> // for device scalability

extern const c7x::uchar_vec MMALIB_zeroVect_uchar_vec;
extern const c7x::ushort_vec MMALIB_zeroVect_ushort_vec;
extern const c7x::uchar_vec MMALIB_vperm_data_0_63;

extern "C" {
#endif /* __cplusplus */

/*!
 * \defgroup MMALIB_COMMON Common definitions
 * \brief This module consists of definitions (macros, structures, utility
 *        functions) that are commonly applicable to all MMALIB kernels.
 * \details
 */

/**@{*/

/*******************************************************************************
 *
 * TYPEDEFS
 *
 ******************************************************************************/

typedef double MMALIB_D64; //!< Double precision floating point
typedef float MMALIB_F32;  //!< Single precision floating point

/*! \brief The enumeration of all status codes. */
typedef enum {
   MMALIB_STATUS_NOT_SET = 999,       /*!< 999 => Not Tested */
   MMALIB_SUCCESS = 0,           /*!< 0 => No error */
   MMALIB_ERR_FAILURE = 1,       /*!< 1 => Unspecified error */
   MMALIB_ERR_INVALID_VALUE = 2, /*!< 2 => Invalid parameter value */
   MMALIB_ERR_INVALID_TYPE  = 3, /*!< 3 => Invalid parameter type (MMALIB_data_type_e data_type) */
   MMALIB_ERR_INVALID_DIMENSION = 4, /*!< 4 => Dimension parameter (width/height) is too big/small */
   MMALIB_ERR_NULL_POINTER = 5, /*!< 5 => Unsupported null pointer condition */
   MMALIB_ERR_NOT_IMPLEMENTED = 6, /*!< 6 => Parameter configuration is not supported/implemented */
   MMALIB_ERR_NOT_EQUAL_WIDTH_STRIDE = 7, /*!< 7 => Stride should be equal to width * element size */
   MMALIB_ERR_NOT_ALIGNED_PTRS_STRIDES = 8, /*!< 8 => Pointers and stride values are not aligned to documented value  */
   MMALIB_ERR_NOT_ALIGNED_WIDTHS = 9, /*!< 9 => Width values are not aligned to documented value */
   MMALIB_ERR_BUFFER_TOO_SMALL = 10, /*!< 10 => Buffer size not large enough */
   MMALIB_ERR_INVALID_ACTIVATION = 11, /*!< 11 => Activation selection incompatible with data type */
   MMALIB_ERR_INVALID_SHIFT = 12,      /*!< 12 => Requested shift amount is not valid */
   MMALIB_ERROR_MAX = 13,
   // MMALIB_CNN_convolve_row
   MMALIB_ERR_CONVOLVE_ROW_INVALID_INCHOFFSET_CIRCULAR = 1000, /*!< 1000 => Invalid InChOffset value*/
   MMALIB_ERR_CONVOLVE_ROW_UNSUPPORTED_STRIDE  = 1001, /*!< 1001 => Unsupported stride value*/
   MMALIB_ERR_CONVOLVE_ROW_UNSUPPORTED_TYPE_COMBINATION = 1002, /*!< 1002 => Unsupported combination of weights, input and output data types */
   MMALIB_ERR_CONVOLVE_ROW_INVALID_PTR_STRIDE = 1003, /*!< 1003 => Invalid pointer stride */
   MMALIB_ERR_CONVOLVE_ROW_INVALID_SUBMCHANNELS_STRIDE = 1004,  /*!< 1004 => Invalid subMChannels strided */
   MMALIB_ERR_CONVOLVE_ROW_INVALID_SUBMCHANNELS = 1005,  /*!< 1005 => Invalid subMChannels and MChannels combination*/
   MMALIB_ERR_CONVOLVE_ROW_INVALID_BUFFER_DIMENSION = 1006,  /*!< 1006 => Invalid buffer dimension*/
   MMALIB_ERR_CONVOLVE_ROW_INVALID_ROWS_STRIDE = 1007, /*!< 1007 => Invalid Valid RowsIn Strided*/
   MMALIB_ERR_CONVOLVE_ROW_INVALID_INITARGS = 1008, /*!< 1008 => inWidth, validColsIn, inChOffset, Fc, Fr, dilationX, dilationY, strideX, strideY, or maxHeight*/
   MMALIB_ERR_CONVOLVE_ROW_INVALID_VALIDCOLSIN = 1009,  /*!< 1009 => Invalid Valid ColsIn*/
   MMALIB_ERR_CONVOLVE_ROW_INVALID_NUM_GROUPS_PER_KERNEL = 1010,  /*!< 1010 => Invalid numGroupsPerKernel*/
   MMALIB_ERR_CONVOLVE_ROW_INVALID_NUM_OUTPUT_CHANNELS_PER_GROUP = 1011,  /*!< 1011 => Invalid number of output channels per group*/
   MMALIB_ERR_CONVOLVE_ROW_INVALID_NUM_GROUPS_PER_KERNEL_STRIDED = 1012, /*!< 1012 => Invalid numGroupsPerKernel strided convolution*/
   MMALIB_ERR_CONVOLVE_ROW_INVALID_PAD_VALUES  = 1013, /*!< 1013 => Invalid combination of pad values */

   // MMALIB_CNN_convolve_col_smallNo
   MMALIB_ERR_CONVOLVE_COL_INVALID_SHIFTMETHOD = 2000, /*!< 2000 => Invalid shiftMethod selected */
   MMALIB_ERR_CONVOLVE_COL_INVALID_SHIFTVALUES = 2001, /*!< 2001 => Invalid shiftValues pointer in combination with shiftMethod value */
   MMALIB_ERR_CONVOLVE_COL_INVALID_PAD_VALUES  = 2002, /*!< 2002 => Invalid combination of pad values */
   MMALIB_ERR_CONVOLVE_COL_INVALID_NUMBIASVALS = 2003, /*!< 2003 => Invalid selection for numBiasVals, note combination restrictions with shiftMethod*/
   MMALIB_ERR_CONVOLVE_COL_INVALID_BIASBVALUES = 2004, /*!< 2004 => Invalid biasBValues pointer in combination with shiftMethod value */
   MMALIB_ERR_CONVOLVE_COL_INVALID_BIASB_TYPE  = 2005, /*!< 2005 => Invalid biasB data type, check that it matches input feature map type */
   MMALIB_ERR_CONVOLVE_COL_UNSUPPORTED_INPUT_FEATURE_MAP_TYPE  = 2006, /*!< 2006 => Unsupported input feature map data type */
   MMALIB_ERR_CONVOLVE_COL_UNSUPPORTED_WEIGHTS_TYPE            = 2007, /*!< 2007 => Unsupported filter weights data type */
   MMALIB_ERR_CONVOLVE_COL_UNSUPPORTED_WEIGHTS_DIMENSION       = 2008, /*!< 2008 => Unsupported filter weights dimension */
   MMALIB_ERR_CONVOLVE_COL_UNSUPPORTED_OUTPUT_FEATURE_MAP_TYPE = 2009, /*!< 2009 => Unsupported output feature map data type */
   MMALIB_ERR_CONVOLVE_COL_UNSUPPORTED_TYPE_COMBINATION        = 2010, /*!< 2010 => Unsupported combination of weights, input and output data types */
   MMALIB_ERR_CONVOLVE_COL_UNSUPPORTED_STRIDE  = 2011, /*!< 2011 => Unsupported stride value*/
   MMALIB_ERR_CONVOLVE_COL_UNSUPPORTED_GROUPING= 2012, /*!< 2012 => Unsupported input/output feature map grouping*/
   MMALIB_ERR_CONVOLVE_COL_INVALID_CASE        = 2013, /*!< 2013 => Unexpected error related to problem geometry*/
   MMALIB_ERR_CONVOLVE_COL_UNSUPPORTED_STRIDE_DILATION_COMBINATION  = 2014, /*!< 2014 => Unsupported combination of stride and dilation values*/

   // MMALIB_CNN_deconvolve_row
   MMALIB_ERR_DECONVOLVE_ROW_INVALID_KERNEL_SIZE = 3000, /*!< 3000 => Invalid kernel size, widht or height*/
   MMALIB_ERR_DECONVOLVE_ROW_INVALID_STRIDE = 3001, /*!< 3001 => Invalid stride values*/
   MMALIB_ERR_DECONVOLVE_ROW_INVALID_DILATION = 3002, /*!< 3002 => Invalid dilation values*/
   MMALIB_ERR_DECONVOLVE_ROW_INVALID_PAD = 3003, /*!< 3003 => Invalid pad value*/
   MMALIB_ERR_DECONVOLVE_ROW_INVALID_INIT_ARGS = 3004, /*!< 3004 => Invalid init args*/
   MMALIB_ERR_DECONVOLVE_ROW_INVALID_BUFFER_DIMENSION_WEIGHTS = 3005, /*!< 3005 => Invalid buffer dim for weights*/
   MMALIB_ERR_DECONVOLVE_ROW_INVALID_SUBMCHANNELS = 3006,  /*!< 3006 => Invalid subMChannels and MChannels combination*/
   MMALIB_ERR_DECONVOLVE_ROW_INVALID_BUFFER_SIZES = 3007,  /*!< 3007 => Invalid buffer size*/
   
   // MMALIB_CNN_fullyConnected
   MMALIB_ERR_FULLYCONNECTED_INVALID_TYPE_COMBINATION = 4000, /*!< 4000 => Invalid datatype combination*/
   MMALIB_ERR_FULLYCONNECTED_INVALID_DIMENSION_COMBINATION = 4001, /*!< 4001 => Invalid dimension combination*/
   MMALIB_ERR_FULLYCONNECTED_INVALID_TYPE_RELU_COMBINATION = 4002, /*!< 4002 => Invalid type and ReLU combination*/

   // MMALIB_CNN_pixelShuffle_row
   MMALIB_ERR_PIXELSHUFFLE_ROW_INVALID_INCHOFFSET_CIRCULAR = 5000, /*!< 5000 => Invalid InChOffset value*/
   MMALIB_ERR_PIXELSHUFFLE_ROW_UNSUPPORTED_STRIDE  = 5001, /*!< 5001 => Unsupported stride value*/
   MMALIB_ERR_PIXELSHUFFLE_ROW_UNSUPPORTED_TYPE_COMBINATION = 5002, /*!< 5002 => Unsupported combination of weights, input and output data types */
   MMALIB_ERR_PIXELSHUFFLE_ROW_INVALID_PTR_STRIDE = 5003, /*!< 5003 => Invalid pointer stride */
   MMALIB_ERR_PIXELSHUFFLE_ROW_INVALID_SUBMCHANNELS_STRIDE = 5004,  /*!< 5004 => Invalid subMChannels strided */
   MMALIB_ERR_PIXELSHUFFLE_ROW_INVALID_SUBMCHANNELS = 5005,  /*!< 5005 => Invalid subMChannels and MChannels combination*/
   MMALIB_ERR_PIXELSHUFFLE_ROW_INVALID_BUFFER_DIMENSION = 5006,  /*!< 5006 => Invalid buffer dimension*/
   MMALIB_ERR_PIXELSHUFFLE_ROW_INVALID_ROWS_STRIDE = 5007, /*!< 5007 => Invalid Valid RowsIn Strided*/
   MMALIB_ERR_PIXELSHUFFLE_ROW_INVALID_INITARGS = 5008, /*!< 5008 => inWidth, validColsIn, inChOffset, Fc, Fr, dilationX, dilationY, strideX, strideY, or maxHeight*/
   MMALIB_ERR_PIXELSHUFFLE_ROW_INVALID_VALIDCOLSIN = 5009,  /*!< 5009 => Invalid Valid ColsIn*/
   MMALIB_ERR_PIXELSHUFFLE_ROW_INVALID_UPSCALE_FACTOR = 5010,  /*!< 5010 => Invalid upscale factor*/
   MMALIB_ERR_PIXELSHUFFLE_ROW_INVALID_KDIM = 5011,  /*!< 5011 => Invalid kDim*/
   MMALIB_ERR_PIXELSHUFFLE_ROW_INVALID_NUMOFOUTPUTCHKERBUF = 5012,  /*!< 5012 => Invalid numOfOutputChKerBuf*/
   MMALIB_ERR_PIXELSHUFFLE_ROW_INVALID_PITCHA = 5013,  /*!< 5013 => Invalid pitchA*/
   

   // MMALIB_CNN_tensor_convert_ixX_oxX
   MMALIB_ERR_TENSOR_CONVERT_INVALID_TYPE_COMBINATION = 6000, /*!< 6000 => Invalid datatype combination*/
   MMALIB_ERR_TENSOR_CONVERT_INVALID_DIMENSION_COMBINATION = 6001, /*!< 6001 => Invalid dimension combination*/
   MMALIB_ERR_TENSOR_CONVERT_UNSUPPORTED_TYPE_COMBINATION = 6002, /*!< 6002 => Unsupported dimension combination*/
   MMALIB_ERR_TENSOR_CONVERT_UNSUPPORTED_CHANNEL_DIMENSION = 6003, /*!< 6003 => Unsupported channel dimension*/
   MMALIB_ERR_TENSOR_CONVERT_UNSUPPORTED_CONVRSION_FORMAT = 6004, /*!< 6004 => Unsupported conversion format*/
   MMALIB_ERR_TENSOR_CONVERT_UNSUPPORTED_PAD_DIMENSION = 6005, /*!< 6005 => Unsupported pad dimension*/


   // MMALIB_LINALG_matrixMatrixMultiply
   MMALIB_ERR_MATRIX_MATRIX_MULTIPLY_UNSUPPORTED_INPUT_TYPE  = 10000, /*!< 10000 => Unsupported input data type */
   MMALIB_ERR_MATRIX_MATRIX_MULTIPLY_UNSUPPORTED_OUTPUT_TYPE = 10001, /*!< 10001 => Unsupported output data type */
   MMALIB_ERR_MATRIX_MATRIX_MULTIPLY_UNSUPPORTED_TYPE_COMBINATION = 10002,  /*!< 10002 => Unsupported combination of input and output data type */
   // MMALIB_LINALG_matrixMatrixMultiplyAccumulate
   MMALIB_ERR_MATRIX_MATRIX_MULTIPLY_ACCUMULATE_UNSUPPORTED_INPUT_TYPE  = 11000, /*!< 11000 => Unsupported input data type */
   MMALIB_ERR_MATRIX_MATRIX_MULTIPLY_ACCUMULATE_UNSUPPORTED_OUTPUT_TYPE = 11001, /*!< 11001 => Unsupported output data type */
   MMALIB_ERR_MATRIX_MATRIX_MULTIPLY_ACCUMULATE_UNSUPPORTED_TYPE_COMBINATION = 11002,  /*!< 11002 => Unsupported combination of input and output data type */
   // MMALIB_LINALG_matrixTranspose
   MMALIB_ERR_MATRIX_TRANSPOSE_UNSUPPORTED_INPUT_TYPE  = 12000, /*!< 12000 => Unsupported input data type */
   MMALIB_ERR_MATRIX_TRANSPOSE_UNSUPPORTED_OUTPUT_TYPE = 12001, /*!< 12001 => Unsupported output data type */
   MMALIB_ERR_MATRIX_TRANSPOSE_UNSUPPORTED_TYPE_COMBINATION = 12002,  /*!< 12002 => Unsupported combination of input and output data type */
   // MMALIB_LINALG_pointwiseMatrixMatrixMultiply
   MMALIB_ERR_POINTWISE_MATRIX_MATRIX_MULTIPLY_UNSUPPORTED_INPUT_TYPE  = 13000, /*!<  13000 => Unsupported input data type */
   MMALIB_ERR_POINTWISE_MATRIX_MATRIX_MULTIPLY_UNSUPPORTED_OUTPUT_TYPE = 13001, /*!<  13001 => Unsupported output data type */
   MMALIB_ERR_POINTWISE_MATRIX_MATRIX_MULTIPLY_UNSUPPORTED_TYPE_COMBINATION = 13002,  /*!< 13002 => Unsupported combination of input and output data type */
} MMALIB_STATUS_NAME;

typedef MMALIB_STATUS_NAME MMALIB_STATUS; //!< Return value for MMALIB functions

typedef void *MMALIB_kernelHandle; //!< Handle type for MMALIB operations

/*! \brief  Enumeration for the style of function implementation. */
typedef enum {
  MMALIB_FUNCTION_NATC = 0,  /*!< Natural C implementation of the function */
  MMALIB_FUNCTION_OPTIMIZED, /*!< Optimized C implementation of the function for
                                the MMA + C7x architecture*/
  MMALIB_FUNCTION_MAX = 128
} MMALIB_FUNCTION_STYLE;

/*! \brief Buffer structure for input feature maps and coefficients of @ref
 * MMALIB_CNN_convolve_row_ixX_ixX_oxX */
typedef enum {
  MMALIB_LINEAR = 0,    //!< Linear buffer
  MMALIB_SE_CIRCULAR,   //!< Circular buffer for input feature map managed by
                        //!< streaming engine
  MMALIB_BUF_CIRCULAR   //!< Circular buffer for coefficients
} MMALIB_buffer_mode_e; // formerly eMMABufferMode;

 /*! \brief Tensor format */
typedef enum {
  MMALIB_CHW = 0,    //!< Used when tensor is in NCHW format
  MMALIB_HWC         //!< Used when tensor is in NHWC format
} MMALIB_tensor_format_e;

typedef MMALIB_tensor_format_e MMALIB_tensor_format;

/*! \brief The enumeration of possible look-up table (LUT) settings */
typedef enum {
   MMALIB_LUT_NONE = 0,          //!< No lookup table post processing
} MMALIB_LUT_type_e;

typedef MMALIB_LUT_type_e MMALIB_LUT_type;


/*******************************************************************************
 *
 * DEFINES
 *
 ******************************************************************************/

#define MMALIB_PARAM_SIZE 128 //!< Parameter structure size in bytes
#define MMALIB_BYTE_WIDTH __MMA_A_COLS(sizeof(int8_t))  //!< MMA width in bytes
#define MMALIB_MMA_ACCUMULATOR_BYTE_WIDTH_8_BIT  4  //!< MMA accumulator width in bytes for 8 bit output data width
#define MMALIB_MMA_ACCUMULATOR_BYTE_WIDTH_16_BIT 8  //!< MMA accumulator width in bytes for 16 bit output data width
#define MMALIB_MMA_ACCUMULATOR_BYTE_WIDTH_32_BIT 16 //!< MMA accumulator width in bytes for 32 bit output data width


/*******************************************************************************
 *
 * Macros
 *
 ******************************************************************************/

#define MMALIB_ALIGN_SHIFT_64BYTES                                             \
  6 //!< Number of bits to shift for 64-byte memory alignment
#define MMALIB_ALIGN_SHIFT_128BYTES                                            \
  7 //!< Number of bits to shift for 128-byte memory alignment
#define MMALIB_ALIGN_SHIFT_256BYTES                                            \
  8 //!< Number of bits to shift for 256-byte memory alignment

#define MMALIB_ALIGN_64BYTES                                                   \
  (1 << MMALIB_ALIGN_SHIFT_64BYTES) //!< Align by 64-byte memory alignment
#define MMALIB_ALIGN_128BYTES                                                  \
  (1 << MMALIB_ALIGN_SHIFT_128BYTES) //!< Align by 128-byte memory alignment
#define MMALIB_ALIGN_256BYTES                                                  \
  (1 << MMALIB_ALIGN_SHIFT_256BYTES) //!< Align by 256-byte memory alignment

#define MMALIB_L2DATA_ALIGN_SHIFT                                              \
  MMALIB_ALIGN_SHIFT_64BYTES //!< Set the default L2 data alignment

/*! @brief Macro that specifies the alignment of data buffers in L2 memory for
 * optimal performance */
#define MMALIB_L2DATA_ALIGNMENT (((uint32_t)1) << ((uint32_t)MMALIB_L2DATA_ALIGN_SHIFT))

/* ---------------------------------------------------------------- */
/*  MISRAC Rule 4.9(DEFINE.FUNC) Deviation: The advisory is not     */
/*  being addressed                                                 */
/* ---------------------------------------------------------------- */

/*! @brief MMA size as a function of precision */

#define MMALIB_MMA_SIZE_8_BIT   __MMA_A_COLS(sizeof(int8_t))
#define MMALIB_MMA_SIZE_16_BIT  __MMA_A_COLS(sizeof(int16_t))
#define MMALIB_MMA_SIZE_32_BIT  __MMA_A_COLS(sizeof(int32_t))


#define MMALIB_MMA_BIAS_SIZE_32_BIT  __MMA_A_COLS(sizeof(int32_t))
#define MMALIB_MMA_BIAS_SIZE_64_BIT  ((MMALIB_MMA_BIAS_SIZE_32_BIT) >> 1)

/**@}*/

/******************************************************************************
 *
 *  Do not document these in the User Guide
 ******************************************************************************/

/*!
 * @cond
 */
/**@{*/


/* ---------------------------------------------------------------- */
/*  MISRAC Rule 4.9(DEFINE.FUNC) Deviation: The advisory is not     */
/*  being addressed                                                 */
/* ---------------------------------------------------------------- */
// remove asm comments for Loki testing as they may add cycles
   #if defined(_HOST_BUILD)
      #define MMALIB_asm(string) ;
   #else
      #define MMALIB_asm(string); asm(string);
   #endif  // _HOST_BUILD

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
#define MMALIB_UNROLL(count)
#else
#define MMALIB_UNROLL(count) _Pragma(str(UNROLL(count)))
#endif
#else
#define MMALIB_UNROLL(COUNT) [[TI::unroll(COUNT)]]
#endif

#if defined(_HOST_BUILD)
#ifdef WIN32
#define __attribute__()
#define MMALIB_MUST_ITERATE(initial, max, multiple)
#else
#define MMALIB_MUST_ITERATE(initial, max, multiple) _Pragma(str(MUST_ITERATE(initial, max, multiple)))
#endif
#else
#define MMALIB_MUST_ITERATE(INTIAL, MAX, MULTIPLE)  [[TI::must_iterate(INTIAL, MAX, MULTIPLE)]]
#endif

#ifndef MMALIB_NUMBER_RANDOM_DIM_TESTS
#define MMALIB_MAX_TEST_PER_KERNEL 1024
#endif

#ifndef MMALIB_NUMBER_RANDOM_DIM_TESTS
#define MMALIB_NUMBER_RANDOM_DIM_TESTS 1
#endif

#define MMALIB_PERFORMANCE_TEST_PATTERN RANDOM_SIGNED

#define MMALIB_TEST_OUTPUT_HEAP 0
#if defined(_HOST_BUILD)
   // Valgrind works better when output is in the heap (it can't track statically allocated memory), so
   // in host emulation mode, place test outputs in the heap rather than statically allocated MSMC
   #define MMALIB_TEST_OUTPUT_MSMC MMALIB_TEST_OUTPUT_HEAP
#else
   #define MMALIB_TEST_OUTPUT_MSMC 1
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
#define MMALIB_min(x, y)                                                       \
  (((x) < (y)) ? (x) : (y)) /*!< A macro to return the minimum of 2 values. */
#define MMALIB_max(x, y)                                                       \
  (((x) < (y)) ? (y) : (x)) /*!< A macro to return the maximum of 2 values. */
#define MMALIB_ceilingDiv(x, y)                                                \
  (((x) + (y)-1) /                                                             \
   (y)) /*!< A macro to return the ceiling of the division of two integers. */


/******************************************************************************
 *
 * COMMON MMA CONIFGURATIONS
 *
 ******************************************************************************/

   extern const MMALIB_MMA_CONFIG_REG configRegisterStruct_i32f_i32f_o32f;
   extern const MMALIB_MMA_CONFIG_REG configRegisterStruct_i32s_i32s_o32s;

   extern const MMALIB_MMA_CONFIG_REG configRegisterStruct_i16s_i16s_o16s;
   extern const MMALIB_MMA_CONFIG_REG configRegisterStruct_i16s_i16s_o16u;
   extern const MMALIB_MMA_CONFIG_REG configRegisterStruct_i16u_i16s_o16s;
   extern const MMALIB_MMA_CONFIG_REG configRegisterStruct_i16u_i16s_o16u;

   extern const MMALIB_MMA_CONFIG_REG configRegisterStruct_i8s_i8s_o8s;
   extern const MMALIB_MMA_CONFIG_REG configRegisterStruct_i8s_i8s_o8u;
   extern const MMALIB_MMA_CONFIG_REG configRegisterStruct_i8u_i8s_o8s;
   extern const MMALIB_MMA_CONFIG_REG configRegisterStruct_i8u_i8s_o8u;
   extern const MMALIB_MMA_CONFIG_REG configRegisterStruct_i8s_i8u_o8s;
   extern const MMALIB_MMA_CONFIG_REG configRegisterStruct_i8u_i8u_o8s;
   extern const MMALIB_MMA_CONFIG_REG configRegisterStruct_i8u_i8u_o8u;

   extern const __HWA_OFFSET_REG offsetRegStruct_zeros;
   extern const __HWA_OFFSET_REG offsetRegStruct_diagonal_32bit;
   extern const __HWA_OFFSET_REG offsetRegStruct_diagonal_16bit;
   extern const __HWA_OFFSET_REG offsetRegStruct_diagonal_8bit;


/**@}*/
/** @endcond  */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* COMMON_MMALIB_TYPES_H_ */

/* ======================================================================== */
/*  End of file:  MMALIB_types.h                                             */
/* ======================================================================== */
