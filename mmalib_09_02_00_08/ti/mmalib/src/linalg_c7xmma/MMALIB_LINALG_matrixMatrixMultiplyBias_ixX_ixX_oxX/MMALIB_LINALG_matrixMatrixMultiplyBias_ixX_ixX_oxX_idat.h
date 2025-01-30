/******************************************************************************/
/*!
 * \file MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_idat.h
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

#ifndef MMALIB_LINALG_matrixMatrixMultiply_IXX_IXX_OXX_IDAT_H
#define MMALIB_LINALG_matrixMatrixMultiply_IXX_IXX_OXX_IDAT_H

#include <mmalib.h>

// include test infrastructure provided by MMALIB
#include "../../test/MMALIB_test.h"

/*!
 *  @struct     matrixMatrixMultiplyBias_ixX_ixX_oxX_testParams_t
 *  @brief      Structure to hold paramters of test case
 *
 */

typedef struct {
   /*! @brief test pattern type */
   uint8_t testPattern; /* 0: constant, 1: sequential, 2: random, 3: static
                           array, 4: file, etc */
   /*! @brief Static A matrix */
   void *staticIn0;
   /*! @brief Datatype of A matrix*/
   uint32_t dataTypeIn0;
   /*! @brief Static B matrix */
   void *staticIn1;
   /*! @brief Datatype of B matrix*/
   uint32_t dataTypeIn1;
   /*! @brief Bias vector added to output matrix*/
   void *staticIn2;
   /*! @brief Datatype of input bias vector*/
   uint32_t dataTypeIn2;
   /*! @brief Scale vector/scalar value(s)*/
   void *staticScale;
   /*! @brief Datatype of scale vector*/
   uint32_t dataTypeIn3;
   /*! @brief Shift vector/scalar value(s)*/
   void *staticShift;
   /*! @brief Static C matrix */
   void *staticOut;
   /*! @brief Datatype of ouput matrix */
   uint32_t dataTypeOut;
   /*! @brief Number of channels */
   uint32_t numChannels;
   /*! @brief Height of A matrix */
   uint32_t heightIn0;
   /*! @brief Width of A matrix */
   uint32_t widthIn0;
   /*! @brief Stride of A matrix, i.e., number of bytes between the
    *         first element in a given row to first element in next row. */
   uint32_t strideIn0;
   /*! @brief Height of B matrix */
   uint32_t heightIn1;
   /*! @brief Width of B matrix */
   uint32_t widthIn1;
   /*! @brief Stride of B matrix, i.e., number of bytes between the
    *         first element in a given row to first element in next row. */
   uint32_t strideIn1;
   /*! @brief Height of output matrix */
   uint32_t heightOut;
   /*! @brief Width of output matrix */
   uint32_t widthOut;
   /*! @brief Stride of output matrix, i.e., number of bytes between the
    *         first element in a given row to first element in next row. */
   uint32_t strideOut;
   /*! @brief Activation type for output matrix */
   int8_t activationType;
   /*! @brief Min value for parameterized saturation */
   int32_t pSatMin;
   /*! @brief Max value for parameterized saturation */
   int32_t pSatMax;
   /*! @brief flag to indicate whther B transposed or not */
   int8_t bTranspose;   // 0 -> B normal, 1 -> B transpose
   /*! @brief flag to indicate bias order (row vector or column vector) 
              @ref MMALIB_LINALG_MATRIXMATRIXMULTIPLYBIAS_IXX_IXX_OXX_BIAS_ORDER*/
   int8_t biasOrder;
   /*! @brief flag to indicate scale and shift order (row vector or column vector) 
              @ref MMALIB_LINALG_MATRIXMATRIXMULTIPLYBIAS_IXX_IXX_OXX_SCALE_SHIFT_ORDER*/
   int8_t scaleShiftOrder;
   /*! @brief flag to indicate whether scale and shift enabled or not
              @ref MMALIB_LINALG_MATRIXMATRIXMULTIPLYBIAS_IXX_IXX_OXX_SCALE_SHIFT_ENABLE_STATUS */
   int8_t scaleShiftEnable;
   /*! @brief Memory to which output matrix is stored
    *         - 0 -> HEAP
    *         - 1 -> MSMC
    */
   int32_t outputDataLocation; // 0 -> HEAP (probably L2SRAM), 1 -> MSMC
   /*! @brief Number of times that this test needs to repeated; 1 for once! */
   uint32_t numReps;
   /*! @brief Testcase ID */
   uint32_t testID;
} matrixMatrixMultiplyBias_ixX_ixX_oxX_testParams_t;

/**
 * @func         matrixMatrixMultiplyBias_ixX_ixX_oxX_getTestParams
 * @brief        Send the test parameter structure and number of tests
 *
 * @param [in]   params   Pointer to array of structures with test parameters
 * @param [in]   numTests Total number of tests
 */
void matrixMatrixMultiplyBias_ixX_ixX_oxX_getTestParams (
    matrixMatrixMultiplyBias_ixX_ixX_oxX_testParams_t **params,
    int32_t *                                     numTests);

#endif /* define MMALIB_LINALG_matrixMatrixMultiply_IXX_IXX_OXX_IDAT_H */

/* ======================================================================== */
/*  End of file:  MMALIB_LINALG_matrixMatrixMultiplyBias_ixX_ixX_oxX_idat.h */
/* ======================================================================== */
