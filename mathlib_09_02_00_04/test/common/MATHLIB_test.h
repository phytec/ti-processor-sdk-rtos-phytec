/******************************************************************************/
/*!
 * \file MATHLIB_test.h
 */
/* Copyright (C) 2023 Texas Instruments Incorporated - http://www.ti.com/
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

#ifndef MATHLIB_TEST_H_
#define MATHLIB_TEST_H_ 1

#include "MATHLIB_enable_cache_mmu.h"
#include "MATHLIB_invalidate_tlb.h"
#include "mathlib.h"
#include <c7x.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void MATHLIB_test_init();
template <typename T>
void MATHLIB_test_print_data(T       *pStaticIn,
                             T       *pDst,
                             T       *pStaticOut,
                             uint32_t pType,
                             uint32_t pTestId,
                             size_t   length,
                             char    *kernel_name);
template <typename T>
void MATHLIB_test_print_data(T       *pStaticIn0,
                             T       *pStaticIn1,
                             T       *pDst,
                             T       *pStaticOut,
                             uint32_t pType,
                             uint32_t pTestId,
                             size_t   length,
                             char    *kernel_name);

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Number of significant digits in cycle formula display */
#define ti_PROFILE_SIG_DIGIT 5

/* Exclusive test parameters */
extern uint8_t MATHLIB_Verbose_Test_Level;
extern int32_t test_cases;
extern int32_t test_index;
extern int32_t act_kernel;
extern char    desc[];
extern char    MATHLIB_test_print_buffer[];
extern char    testPatternString[];

/* Kernel pass /fail enumeration */
enum { MATHLIB_TEST_KERNEL_FAIL = 0, MATHLIB_TEST_KERNEL_PASS = 1 };

/* Enum for percent error or absolute tolerance testing */
typedef enum { MATHLIB_PERCENT_ERROR, MATHLIB_ABSOLUTE_TOLERANCE } MATHLIB_TestMetric;

#define MATHLIB_EXP_PERCENT_ERROR 4e-4
#define MATHLIB_EXP2_PERCENT_ERROR 4e-2
#define MATHLIB_POW_PERCENT_ERROR 4e-2
#define MATHLIB_LOG2_PERCENT_ERROR 4e-4
#define MATHLIB_TANH_PERCENT_ERROR 4e-4
#define MATHLIB_COSH_PERCENT_ERROR 4e-4
#define MATHLIB_SINH_PERCENT_ERROR 4e-4

static inline void MATHLIB_test_print_string(char *s)
{
   /* #if !defined(NO_PRINTF) */
   /* #if defined(EVM_TEST) */
   fputs(s, stdout);
   /* #else */
   /*    // fprintf(stderr, "%s", s); */
   /*    fputs(s, stderr); */
   /* #endif */
   /* #endif */
}

/* Generic Test Utility Prototypes */
void MATHLIB_test_print_float(float num, int printSign);
void MATHLIB_test_error_handler(void);

/* Standardized assertion utility */
void MATHLIB_test_assert(int assertion, char *err_string);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MATHLIB_TEST_H_ */
