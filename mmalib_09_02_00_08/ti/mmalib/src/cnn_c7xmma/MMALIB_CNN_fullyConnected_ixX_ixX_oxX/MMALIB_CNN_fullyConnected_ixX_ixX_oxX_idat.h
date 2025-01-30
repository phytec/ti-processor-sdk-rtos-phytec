/*******************************************************************************
 **+--------------------------------------------------------------------------+**
 **|                            ****                                          |**
 **|                            ****                                          |**
 **|                            ******o***                                    |**
 **|                      ********_///_****                                   |**
 **|                      ***** /_//_/ ****                                   |**
 **|                       ** ** (__/ ****                                    |**
 **|                           *********                                      |**
 **|                            ****                                          |**
 **|                            ***                                           |**
 **|                                                                          |**
 **|         Copyright (c) 2016 Texas Instruments Incorporated                |**
 **|                        ALL RIGHTS RESERVED                               |**
 **|                                                                          |**
 **| Permission to use, copy, modify, or distribute this software,            |**
 **| whether in part or in whole, for any purpose is forbidden without        |**
 **| a signed licensing agreement and NDA from Texas Instruments              |**
 **| Incorporated (TI).                                                       |**
 **|                                                                          |**
 **| TI makes no representation or warranties with respect to              |**
 **| performance of this computer program, and specifically disclaims         |**
 **| any responsibility for any damages, special or consequential,            |**
 **| connected with  use of this program.                                  |**
 **|                                                                          |**
 **+--------------------------------------------------------------------------+**
 *******************************************************************************/

#ifndef MMALIB_CNN_FULLYCONNECTED_IXX_IXX_OXX_IDAT_H

#define MMALIB_CNN_FULLYCONNECTED_IXX_IXX_OXX_IDAT_H

#include <mmalib.h>

// include test infrastructure provided by MMALIB
#include "../../test/MMALIB_test.h"

#define MULTIPASS_TRUE (1)
#define MULTIPASS_FALSE (0)
#define NUM_K_MULTIPASS (128)

/*!
 *  @struct     MMALIB_CNN_fullyConnected_ixX_ixX_oxX_testParams_t
 *  @brief      Structure to hold paramters of test case
 *
 */

typedef struct {
   /*! @brief test pattern type */
   uint8_t testPattern; /* 0: constant, 1: sequential, 2: random, 3: static array, 4:
                           file, etc */
   /*! @brief static array with input feature map*/
   void *staticIn0; // feature map
   /*! @brief datatype of input feature map*/
   uint32_t dataTypeIn0;
   /*! @brief static array with kernel coefficients*/
   void *staticIn1; // kernel matrix
   /*! @brief datatype of kernel coefficients*/
   uint32_t dataTypeIn1;
   /*! @brief static array with kernel coefficients  */
   void *staticOut;
   /*! @brief datatype of ouput matrix*/
   uint32_t dataTypeOut;
   /*! @brief height of input feature map or batch size*/
   uint32_t heightIn0; // height of feature map
   /*! @brief width of input feature map or number of input channels, \f$(N_i)\f$ */
   uint32_t widthIn0; // width of feature map
   /*! @brief stride of input feature map, i.e., number of bytes between the
     first element in a given row to first element in next row. */
   uint32_t strideIn0; // stride of feature map
   /*! @brief height of kernel matrix including bias rows, i.e., \f$(N_i+Bias Rows)\f$ */
   uint32_t heightIn1; // height of kernel matrix
   /*! @brief width of kernel or number of output channels, \f$(N_o)\f$ */
   uint32_t widthIn1; // width of kernel matrix
   /*! @brief stride of kernel matrix, i.e., number of bytes between the
     first element in a given row to first element in next row. */
   uint32_t strideIn1; // stride of kernel matrix
   /*! @brief height of output matrix, typically batch size */
   uint32_t heightOut;
   /*! @brief width of output matrix, typically \f$(N_o)\f$ */
   uint32_t widthOut;
   /*! @brief stride of output matrix, i.e., number of bytes between the
     first element in a given row to first element in next row. */
   uint32_t strideOut;
   /*! @brief Values by which elements in output matrix are scaled; \f$(2^shiftOut)\f$ is
     vaule by which elements are scaled */
   uint16_t shiftOut; // scale value for output
   /*! @brief Bias value of constant matrix that is appended to feature matrix
     on-the-fly via SE0's DECDIM feature and perdicate register */
   int32_t biasB; // value of constant matrix that needs to be augmented to feature map
   /*! @brief Activation type for output matrix */
   int8_t activationType; // saturate outputs or ReLU
   int32_t expectedStatusCode; //expected Status code
   /*! @brief Memory to which output matrix is stored
    *         - 0 -> HEAP
    *         - 1 -> MSMC
    */
   int32_t outputDataLocation; // 0 -> HEAP (probably L2SRAM), 1 -> MSMC
   /*! @brief Enable multi-pass-style processing for this testcase as feature matrix is
    *         too large to be fit into L2
    *         - 0 -> DISABLE
    *         - 1 -> ENABLE
    */
   bool multiPassX; // 0 -> FALSE; 1 -> TRUE; to support large kernel matrices
   /*! @brief Enable multi-pass-style processing for this testcase as kernel matrix is too
    *         large to be fit into L2
    *         - 0 -> DISABLE
    *         - 1 -> ENABLE
    */
   bool multiPassH; // 0 -> FALSE; 1 -> TRUE; to support large kernel matrices
   /*! @brief Number of times that this test needs to repeated; 1 for once! */
   uint32_t numReps;
   /*! @brief Flag to signify if kernel matrix supplied by this test cases needs
    *         pre-processing (re-ordering of data to alleviate L2 bank conflicts) */
   bool preProcessFlag;
   /*! @brief Testcase ID */
   uint32_t testID;
} MMALIB_CNN_fullyConnected_ixX_ixX_oxX_testParams_t;

/**
 * @func         MMALIB_CNN_fullyConnected_ixX_ixX_oxX_getTestParams
 * @brief        Send the test parameter structure and number of tests
 *
 * @param [in]   params   Pointer to array of structures with test parameters
 * @param [in]   numTests Total number of tests
 */
void MMALIB_CNN_fullyConnected_ixX_ixX_oxX_getTestParams (
    MMALIB_CNN_fullyConnected_ixX_ixX_oxX_testParams_t **params,
    int32_t *                                 numTests);

#endif /* define MMALIB_CNN_FULLYCONNECTED_IXX_IXX_OXX_IDAT_H */

/* ======================================================================== */
/*  End of file:  MMALIB_CNN_fullyConnected_ixX_ixX_oxX_idat.h              */
/* ======================================================================== */
