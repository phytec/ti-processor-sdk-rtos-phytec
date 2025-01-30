/*******************************************************************************
 **+--------------------------------------------------------------------------+**
 **|                            **** |**
 **|                            **** |**
 **|                            ******o*** |**
 **|                      ********_///_**** |**
 **|                      ***** /_//_/ **** |**
 **|                       ** ** (__/ **** |**
 **|                           ********* |**
 **|                            **** |**
 **|                            *** |**
 **| |**
 **|         Copyright (c) 2016 Texas Instruments Incorporated |**
 **|                        ALL RIGHTS RESERVED |**
 **| |**
 **| Permission to use, copy, modify, or distribute this software, |**
 **| whether in part or in whole, for any purpose is forbidden without |**
 **| a signed licensing agreement and NDA from Texas Instruments |**
 **| Incorporated (TI). |**
 **| |**
 **| TI makes no representation or warranties with respect to              |**
 **| performance of this computer program, and specifically disclaims |**
 **| any responsibility for any damages, special or consequential, |**
 **| connected with  use of this program.                                  |**
 **| |**
 **+--------------------------------------------------------------------------+**
 *******************************************************************************/

#ifndef CONVERT_IXX_OXX_IDAT_H

#define CONVERT_IXX_OXX_IDAT_H

#include <mmalib.h>

// include test infrastructure provided by MMALIB
#include "../../test/MMALIB_test.h"

/*!
 *  @struct     MMALIB_CNN_tensor_convert_ixX_oxX_testParams_t
 *  @brief      Structure to hold paramters of test case
 *
 */

typedef struct {
   /*! @brief test pattern type */
   uint8_t testPattern; /* 0: constant, 1: sequential, 2: random, 3: static
                           array, 4: file, etc */
   /*! @brief static array with input feature map*/
   void *staticIn; // feature map
   /*! @brief datatype of input feature map*/
   uint32_t dataTypeIn;
   void *   staticScale;
   uint32_t dataTypeScale;
   uint8_t *staticShift;
   void *   staticQ1;
   void *   staticQ2;
   void *   staticOut;
   /*! @brief datatype of ouput matrix*/
   uint32_t dataTypeOut;
   int32_t expectedStatusCode;
   /*! @brief width of input feature map or batch size*/
   uint32_t width;
   /*! @brief height of input feature map */
   uint32_t                height; // width of feature map
   uint32_t                numChannels;
   uint32_t                strideInY;  // line pitch of input feature map
   uint32_t                strideInZ;  // channel pitch of input feature map
   uint32_t                strideOutY; // line pitch of output feature map
   uint32_t                strideOutZ; // channel pitch of output feature map
   uint32_t                inPad;
   uint32_t                outPad;
   uint32_t                numRowsQuant;
   MMALIB_tensor_format    tensorFormatIn;
   MMALIB_tensor_format    tensorFormatOut;
   MMALIB_quantMethod_type quantMethod;
   /*! @brief Memory to which output matrix is stored
    *         - 0 -> HEAP
    *         - 1 -> MSMC
    */
   int32_t outputDataLocation; // 0 -> HEAP (probably L2SRAM), 1 -> MSMC
   /*! @brief Enable multi-pass-style processing for this testcase as feature
    * matrix is too large to be fit into L2
    *         - 0 -> DISABLE
    *         - 1 -> ENABLE
    */
   /*! @brief Number of times that this test needs to repeated; 1 for once! */
   uint32_t numReps;
   /*! @brief Testcase ID */
   uint32_t testID;
} MMALIB_CNN_tensor_convert_ixX_oxX_testParams_t;

/**
 * @func         MMALIB_CNN_tensor_convert_ixX_oxX_getTestParams
 * @brief        Send the test parameter structure and number of tests
 *
 * @param [in]   params   Pointer to array of structures with test parameters
 * @param [in]   numTests Total number of tests
 */
void MMALIB_CNN_tensor_convert_ixX_oxX_getTestParams (
    MMALIB_CNN_tensor_convert_ixX_oxX_testParams_t **params,
    int32_t *                                        numTests);

#endif /* define CONVERT_IXX_OXX_IDAT_H */

/* ======================================================================== */
/*  End of file:  MMALIB_CNN_tensor_convert_ixX_oxX_idat.h              */
/* ======================================================================== */
