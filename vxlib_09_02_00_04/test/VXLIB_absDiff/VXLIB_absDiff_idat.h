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
#ifndef VXLIB_absDiff_IXX_IXX_OXX_IDAT_H

#define VXLIB_absDiff_IXX_IXX_OXX_IDAT_H

#include <vxlib.h>

// include test infrastructure provided by VXLIB
#include "../common/VXLIB_test.h"

/**
 * @brief Structure containing the parameters to run test cases for kernel
 */
typedef struct {
   /** @brief Type of test sequence data
    * 0: constant
    * 1: sequential
    * 2: sequential negative
    * 3: random
    * 4: random signed
    * 5: static
    * 6: custom
    * 7: random dimension
    * 8: random fast
    * 9: random small
    */
   uint8_t testPattern;
   /** @brief Pointer to input data 0 */
   void *staticIn0;
   /** @brief Pointer to input data 1 */
   void *staticIn1;
   /** @brief Pointer to output data */
   void *staticOut;
   /** @brief Data type of input and output data */
   uint32_t dTypeIn0;
   /** @brief Height of input images */
   uint32_t height;
   /** @brief Width of input images */
   uint32_t width;
   /** @brief Stride of input image 0 */
   uint32_t strideIn0;
   /** @brief Stride of input image 1 */
   uint32_t strideIn1;
   /** @brief Stride of output image */
   uint32_t strideOut;
   /** @brief Output location of data
    * 0: Heap (most likely L2SRAM)
    * 1: MSMC
    */
   uint32_t outputDataLocation;
   /** @brief Number of repetitions */
   uint32_t numReps;
   /** @brief Test ID */
   uint32_t testID;
} VXLIB_absDiff_testParams_t;

void VXLIB_absDiff_getTestParams(VXLIB_absDiff_testParams_t **params, int32_t *numTests);

#endif /* define VXLIB_absDiff_IXX_IXX_OXX_IDAT_H */
