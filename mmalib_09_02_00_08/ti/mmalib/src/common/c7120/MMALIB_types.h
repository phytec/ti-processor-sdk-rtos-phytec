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
#ifndef C7120_MMALIB_TYPES_H_
#define C7120_MMALIB_TYPES_H_ 1

#include <c7x.h> // for streaming engine, streaming address gen.

///* ---------------------------------------------------------------- */
///* Desire C namespace for these defines/includes                    */
///* ---------------------------------------------------------------- */



#ifdef __cplusplus
#include <c7x_scalable.h> // for device scalability

/******************************************************************************
 *
 * PREDEFINED VECTORS
 *
 ******************************************************************************/

extern const c7x::uchar_vec MMALIB_vperm_convolve_col_scale_No_1;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_scale_No_2;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_scale_No_3;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_scale_No_4;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_scale_No_5;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_scale_No_6;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_scale_No_7;

// permutation register values for 32-bit bias
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_biasVec0_32bit_No_1;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_biasVec1_32bit_No_1;

extern const c7x::uchar_vec MMALIB_vperm_convolve_col_biasVec0_32bit_No_2;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_biasVec1_32bit_No_2;

extern const c7x::uchar_vec MMALIB_vperm_convolve_col_biasVec0_32bit_No_3;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_biasVec1_32bit_No_3;

extern const c7x::uchar_vec MMALIB_vperm_convolve_col_biasVec0_32bit_No_4;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_biasVec1_32bit_No_4;

extern const c7x::uchar_vec MMALIB_vperm_convolve_col_biasVec0_32bit_No_5;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_biasVec1_32bit_No_5;

extern const c7x::uchar_vec MMALIB_vperm_convolve_col_biasVec0_32bit_No_6;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_biasVec1_32bit_No_6;

extern const c7x::uchar_vec MMALIB_vperm_convolve_col_biasVec0_32bit_No_7;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_biasVec1_32bit_No_7;

// permutation register values for 64-bit bias
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_biasVec0_64bit_No_1;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_biasVec1_64bit_No_1;

extern const c7x::uchar_vec MMALIB_vperm_convolve_col_biasVec0_64bit_No_2;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_biasVec1_64bit_No_2;

extern const c7x::uchar_vec MMALIB_vperm_convolve_col_biasVec0_64bit_No_3;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_biasVec1_64bit_No_3;

// split groups
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_splitGroups_scale_No_1;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_splitGroups_scale_No_2;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_splitGroups_scale_No_3;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_splitGroups_scale_No_4;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_splitGroups_scale_No_5;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_splitGroups_scale_No_6;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_splitGroups_scale_No_7;

// permutation register values for 32-bit bias, split groups
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_splitGroups_biasVec0_32bit_No_1;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_splitGroups_biasVec1_32bit_No_1;

extern const c7x::uchar_vec MMALIB_vperm_convolve_col_splitGroups_biasVec0_32bit_No_2;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_splitGroups_biasVec1_32bit_No_2;

extern const c7x::uchar_vec MMALIB_vperm_convolve_col_splitGroups_biasVec0_32bit_No_3;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_splitGroups_biasVec1_32bit_No_3;

extern const c7x::uchar_vec MMALIB_vperm_convolve_col_splitGroups_biasVec0_32bit_No_4;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_splitGroups_biasVec1_32bit_No_4;

extern const c7x::uchar_vec MMALIB_vperm_convolve_col_splitGroups_biasVec0_32bit_No_5;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_splitGroups_biasVec1_32bit_No_5;

extern const c7x::uchar_vec MMALIB_vperm_convolve_col_splitGroups_biasVec0_32bit_No_6;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_splitGroups_biasVec1_32bit_No_6;

extern const c7x::uchar_vec MMALIB_vperm_convolve_col_splitGroups_biasVec0_32bit_No_7;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_splitGroups_biasVec1_32bit_No_7;

// permutation register values for 64-bit bias, split groups
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_splitGroups_biasVec0_64bit_No_1;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_splitGroups_biasVec1_64bit_No_1;

extern const c7x::uchar_vec MMALIB_vperm_convolve_col_splitGroups_biasVec0_64bit_No_2;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_splitGroups_biasVec1_64bit_No_2;

extern const c7x::uchar_vec MMALIB_vperm_convolve_col_splitGroups_biasVec0_64bit_No_3;
extern const c7x::uchar_vec MMALIB_vperm_convolve_col_splitGroups_biasVec1_64bit_No_3;

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

/*! \brief The enumeration of possible activation operations on the computation. */
typedef enum {
   MMALIB_NONE = 0,   //!< Return result as-is (allow wrapping)
   MMALIB_RELU,       //!< Return result after applying ReLU operation
   MMALIB_SATURATION, //!< Return result after saturating
   MMALIB_ROUNDING,   //!< Return result after rounding
   MMALIB_PSAT,       //!< Return result after parameterized saturation
} MMALIB_activation_type_e; // formerly eMMAActivationType;


   /*! \brief The enumeration of possible dynamic range enable settings */
   typedef enum {
      MMALIB_DYNAMIC_RANGE_DISABLE = 0,          //!< Disable dynamic range computation
      MMALIB_DYNAMIC_RANGE_ENABLE  = 1,          //!< Enable dynamic range computation
   } MMALIB_enableDynamicRange_type_e;

   /*! \brief The enumeration of possible dynamic range reset options */
   typedef enum {
      MMALIB_DYNAMIC_RANGE_NOINIT = 0,          //!< Do not reset dynamic range min/max at start of computation
      MMALIB_DYNAMIC_RANGE_INIT   = 1,          //!< Reset dynamic range min/max at start of computation
   } MMALIB_initDynamicRange_type_e;

   typedef MMALIB_enableDynamicRange_type_e MMALIB_enableDynamicRange_type; //!< Type for enableDynamicRange variables
   typedef MMALIB_initDynamicRange_type_e MMALIB_initDynamicRange_type; //!< Type for initDynamicRange variables

  /*! \brief The enumeration of possible quantization methods */
   typedef enum {
      MMALIB_QUANT_PER_KERNEL = 0,
      MMALIB_QUANT_PER_CHANNEL   = 1,
   } MMALIB_quantMethod_type_e;
   typedef MMALIB_quantMethod_type_e MMALIB_quantMethod_type; //!< Type for initDynamicRange variables
   /*******************************************************************************
    *
    * DEFINES
    *
    ******************************************************************************/

#define MMALIB_ILUT_SUPPORT 0       // !< No ILUT on this device
#define MMALIB_ILUT_NUM_BYTES 0     // !< No ILUT on this device


   /* ---------------------------------------------------------------- */
   /*  MISRAC Rule 4.9(DEFINE.FUNC) Deviation: The advisory is not     */
   /*  being addressed                                                 */
   /* ---------------------------------------------------------------- */
   /*! @brief Macro function that computes the minimum stride for the specified
    * number of bytes and alignment */
   #define MMALIB_CALC_STRIDE(BYTES, ALIGN_SHIFT)                                 \
     (((((BYTES)-1) >> (ALIGN_SHIFT)) + 1) << (ALIGN_SHIFT))

/*! @brief Macro for MMALIB shift amounts */
#define MMALIB_MMA_SIZE_8_BIT_SHIFT   6 //!< Shift for MMA size when data type is 8-bit integers
#define MMALIB_MMA_SIZE_16_BIT_SHIFT  5 //!< Shift for MMA size when data type is 16-bit integers
#define MMALIB_MMA_SIZE_32_BIT_SHIFT  4 //!< Shift for MMA size when data type is 32-bit integers

   /* /\*! @brief MMA size as a function of precision *\/ */
   /* //#define MMALIB_MMA_SIZE_1_BYTE    1 */
   /* #define MMALIB_MMA_SIZE_8_BIT                                              \ */
   /*   ((uint32_t)(((uint32_t)1)                                                     \ */
   /*              << MMALIB_MMA_SIZE_8_BIT_SHIFT)) //!< Shift for MMA size when data */
   /*                                               //!< type is 8-bit integers */
   /* #define MMALIB_MMA_SIZE_16_BIT                                                 \ */
   /*   ((uint32_t)(((uint32_t)1)                                                     \ */
   /*              << MMALIB_MMA_SIZE_16_BIT_SHIFT)) //!< Shift for MMA size when data */
   /*                                                //!< type is 16-bit integers */
   /* #define MMALIB_MMA_SIZE_32_BIT                                                 \ */
   /*   ((int32_t)(((uint32_t)1)                                                     \ */
   /*              << MMALIB_MMA_SIZE_32_BIT_SHIFT)) //!< Shift for MMA size when data */
   /*                                                //!< type is 32-bit integers */



/*******************************************************************************
 *
 * DEFINES
 *
 ******************************************************************************/


/*******************************************************************************
 *
 * Macros
 *
 ******************************************************************************/



/**@}*/



#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* C7120_MMALIB_TYPES_H_ */

/* ======================================================================== */
/*  End of file:  MMALIB_types.h                                             */
/* ======================================================================== */
