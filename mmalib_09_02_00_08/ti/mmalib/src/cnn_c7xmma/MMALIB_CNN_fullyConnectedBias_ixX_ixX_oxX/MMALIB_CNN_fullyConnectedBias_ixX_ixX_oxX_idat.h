/******************************************************************************/
/*!
 * \file MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_idat.h
 */
/* Copyright (C) 2017 Texas Instruments Incorporated - http://www.ti.com/
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

#ifndef MMALIB_CNN_FULLYCONNECTED_IXX_IXX_OXX_IDAT_H

#define MMALIB_CNN_FULLYCONNECTED_IXX_IXX_OXX_IDAT_H

#include <mmalib.h>

// include test infrastructure provided by MMALIB
#include "../../test/MMALIB_test.h"

#define MULTIPASS_TRUE (1)
#define MULTIPASS_FALSE (0)
#define NUM_K_MULTIPASS (128)

/*!
 *  @struct     MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_testParams_t
 *  @brief      Structure to hold paramters of test case
 *
 */

typedef struct {
   /*! @brief test pattern type */
   uint8_t testPattern; /* 0: constant, 1: sequential, 2: random, 3: static
                           array, 4: file, etc */
   /*! @brief Static array with input feature map*/
   void *staticIn0; // feature map
   /*! @brief datatype of input feature map*/
   uint32_t dataTypeIn0;
   /*! @brief Static array with kernel coefficients*/
   void *staticIn1; // kernel matrix
   /*! @brief Datatype of kernel coefficients*/
   uint32_t dataTypeIn1;
   /*! @brief Bias vector added  to output matrix*/
   void *staticIn2; // bias vector
   /*! @brief Datatype of input bias vector*/
   uint32_t dataTypeIn2;
   /*! @brief Scale vector/scalar value(s)*/
   void *staticScale;
   /*! @brief Datatype of scale vector*/
   uint32_t dataTypeIn3;
   /*! @brief Shift vector/scalar value(s)*/
   void *staticShift;
   /*! @brief Static array with kernel coefficients  */
   void *staticOut;
   /*! @brief Datatype of ouput matrix*/
   uint32_t dataTypeOut;
   /*! @brief Minimum value of dynamic range*/
   void *staticInDynamicRangeMin;
   /*! @brief Maximum value of dynamic range*/
   void *staticInDynamicRangeMax;
   /*! @brief Height of input feature map or batch size*/
   uint32_t heightIn0; // height of feature map
   /*! @brief Width of input feature map or number of input channels,
    *         \f$(N_i)\f$ */
   uint32_t widthIn0; // width of feature map
   /*! @brief Stride of input feature map, i.e., number of bytes between the
    *         first element in a given row to first element in next row. */
   uint32_t strideIn0; // stride of feature map
   /*! @brief Height of kernel matrix including bias rows, i.e.,
    *         \f$(N_i+Bias * Rows)\f$ */
   uint32_t heightIn1; // height of kernel matrix
   /*! @brief Width of kernel or number of output channels, \f$(N_o)\f$ */
   uint32_t widthIn1; // width of kernel matrix
   /*! @brief Stride of kernel matrix, i.e., number of bytes between the
    *         first element in a given row to first element in next row. */
   uint32_t strideIn1; // stride of kernel matrix
   /*! @brief Height of output matrix, typically batch size */
   uint32_t heightOut;
   /*! @brief Width of output matrix, typically \f$(N_o)\f$ */
   uint32_t widthOut;
   /*! @brief Stride of output matrix, i.e., number of bytes between the
    *         first element in a given row to first element in next row. */
   uint32_t strideOut;
   /*! @brief Activation type for output matrix */
   int8_t activationType; // saturate outputs or ReLU or PSAT
   /*! @brief Min value for parameterized saturation */
   int32_t pSatMin;
   /*! @brief Max value for parameterized saturation */
   int32_t                        pSatMax;
   int32_t       expectedStatusCode;
   MMALIB_enableDynamicRange_type enableDynamicRange;
   MMALIB_initDynamicRange_type   initDynamicRange;
   /*! @brief Quantization type, per-kernel or per-channel basis */
   MMALIB_quantMethod_type quantMethod;
   /*! @brief Memory to which output matrix is stored
    *         - 0 -> HEAP
    *         - 1 -> MSMC
    */
   int32_t outputDataLocation; // 0 -> HEAP (probably L2SRAM), 1 -> MSMC
   /*! @brief Enable multi-pass-style processing for this testcase as feature
    *         matrix is too large to be fit into L2
    *         - 0 -> DISABLE
    *         - 1 -> ENABLE
    */
   bool multiPassX; // 0 -> FALSE; 1 -> TRUE; to support large kernel matrices
   /*! @brief Enable multi-pass-style processing for this testcase as kernel
    *         matrix is too large to be fit into L2
    *         - 0 -> DISABLE
    *         - 1 -> ENABLE
    */
   bool multiPassH; // 0 -> FALSE; 1 -> TRUE; to support large kernel matrices
   /*! @brief Number of times that this test needs to repeated; 1 for once! */
   uint32_t numReps;
   /*! @brief Flag to signify if kernel matrix supplied by this test cases needs
    *         pre-processing (re-ordering of data to alleviate L2 bank
    *         conflicts) */
   bool preProcessFlag;
   /*! @brief Testcase ID */
   uint32_t testID;
} MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_testParams_t;

/**
 * @func         MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_getTestParams
 * @brief        Send the test parameter structure and number of tests
 *
 * @param [in]   params   Pointer to array of structures with test parameters
 * @param [in]   numTests Total number of tests
 */
void MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_getTestParams (
    MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_testParams_t **params,
    int32_t *                                     numTests);

#endif /* define MMALIB_CNN_FULLYCONNECTED_IXX_IXX_OXX_IDAT_H */

/* ======================================================================== */
/*  End of file:  MMALIB_CNN_fullyConnectedBias_ixX_ixX_oxX_idat.h */
/* ======================================================================== */
