/*****************************************************************************
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

/******************************************************************************/
/*                                                                            */
/* Includes                                                                   */
/*                                                                            */
/******************************************************************************/

#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "MATHLIB_profile.h"
#include "MATHLIB_test.h"

// test parameter to set verbose test feedback level
uint8_t MATHLIB_Verbose_Test_Level = 1;

// current test index
int test_index;

// type of kernel under test
int act_kernel;

// keep track of number of test cases
int32_t test_cases = 1;

// global test description string variables used by all tests
char desc[512];

// global character array used for storing print strings
char MATHLIB_test_print_buffer[512];

// test pattern type
char testPatternString[16];

/******************************************************************************/
/*                                                                            */
/* MATHLIB_print_float                                                          */
/*                                                                            */
/******************************************************************************/

void MATHLIB_test_print_float(float num, int printSign)
{
   int digit;
   int i;

   /* Explicitly print sign and remove from num afterwards */
   if (num < 0) {
      num = -num;
      sprintf(MATHLIB_test_print_buffer, "- ");
      MATHLIB_test_print_string(MATHLIB_test_print_buffer);
   }
   else if (printSign) {
      sprintf(MATHLIB_test_print_buffer, "+ ");
      MATHLIB_test_print_string(MATHLIB_test_print_buffer);
   }

   /* Print integer part (always) */
   digit = (int) num;
   num   = (num - (float) digit) * 10;
   sprintf(MATHLIB_test_print_buffer, "%d.", digit);
   MATHLIB_test_print_string(MATHLIB_test_print_buffer);

   /* Print each decimal place until zero remainder */
   for (i = 0; i < ti_PROFILE_SIG_DIGIT; i++) {
      /* Get next digit and display */
      digit = (int) num;
      num   = (num - (float) digit) * 10;
      sprintf(MATHLIB_test_print_buffer, "%d", digit);
      MATHLIB_test_print_string(MATHLIB_test_print_buffer);

      /* Break if remainder is zero */
      if (num == 0) {
         break;
      }
   }
}

/* Common Test Utility:
 *
 *    MATHLIB_error_handler
 *
 */
void MATHLIB_test_error_handler(void)
{
   /* For now just exit */
   exit(-1);
}

/* Standardized assertion utility */
void MATHLIB_test_assert(int assertion, char *err_string)
{
   if (!assertion) {
      /* Display start of error message */
      sprintf(MATHLIB_test_print_buffer, "Assertion Error");
      MATHLIB_test_print_string(MATHLIB_test_print_buffer);

      /* Display test if relevant */
      if (test_index > 0) {
         sprintf(MATHLIB_test_print_buffer, " (Test %d)", test_index);
         MATHLIB_test_print_string(MATHLIB_test_print_buffer);
      }

      /* Display remainder of error message */
      sprintf(MATHLIB_test_print_buffer, ": %s\n", err_string);
      MATHLIB_test_print_string(MATHLIB_test_print_buffer);

      /* Call error handler */
      MATHLIB_test_error_handler();
   }
}

template <typename T>
void MATHLIB_test_print_data(T       *pStaticIn,
                             T       *pDst,
                             T       *pStaticOut,
                             uint32_t pType,
                             uint32_t pTestId,
                             size_t   length,
                             char    *kernel_name)
{
   std::fstream outputData;
   std::string  filename =
       "../../test/" + std::string(kernel_name) + "/test_data/outputdata" + std::to_string(pTestId) + ".csv";
   outputData.open(filename, std::ios::trunc | std::ios::out | std::ios::in);
   if (pType == MATHLIB_FLOAT32) {
      for (uint32_t i = 0; i < length; i++) {
         outputData << std::setprecision(std::numeric_limits<float>::max_digits10) << pStaticIn[i] << ", ";
      }
      outputData << "\n";
      for (uint32_t i = 0; i < length; i++) {
         outputData << std::setprecision(std::numeric_limits<float>::max_digits10) << pDst[i] << ", ";
      }
      outputData << "\n";
      for (uint32_t i = 0; i < length; i++) {
         outputData << std::setprecision(std::numeric_limits<float>::max_digits10) << pStaticOut[i] << ", ";
      }
   }
   else if (pType == MATHLIB_FLOAT64) {
      for (uint32_t i = 0; i < length; i++) {
         outputData << std::setprecision(std::numeric_limits<double>::max_digits10) << pStaticIn[i] << ", ";
      }
      outputData << "\n";
      for (uint32_t i = 0; i < length; i++) {
         outputData << std::setprecision(std::numeric_limits<double>::max_digits10) << pDst[i] << ", ";
      }
      outputData << "\n";
      for (uint32_t i = 0; i < length; i++) {
         outputData << std::setprecision(std::numeric_limits<double>::max_digits10) << pStaticOut[i] << ", ";
      }
   }
   outputData.close();
}

template <typename T>
void MATHLIB_test_print_data(T       *pStaticIn0,
                             T       *pStaticIn1,
                             T       *pDst,
                             T       *pStaticOut,
                             uint32_t pType,
                             uint32_t pTestId,
                             size_t   length,
                             char    *kernel_name)
{
   std::fstream outputData;
   std::string  filename =
       "../../test/" + std::string(kernel_name) + "/test_data/outputdata" + std::to_string(pTestId) + ".csv";
   outputData.open(filename, std::ios::trunc | std::ios::out | std::ios::in);
   if (pType == MATHLIB_FLOAT32) {
      for (uint32_t i = 0; i < length; i++) {
         outputData << std::setprecision(std::numeric_limits<float>::max_digits10) << pStaticIn0[i] << ", ";
      }
      outputData << "\n";
      for (uint32_t i = 0; i < length; i++) {
         outputData << std::setprecision(std::numeric_limits<float>::max_digits10) << pStaticIn1[i] << ", ";
      }
      outputData << "\n";
      for (uint32_t i = 0; i < length; i++) {
         outputData << std::setprecision(std::numeric_limits<float>::max_digits10) << pDst[i] << ", ";
      }
      outputData << "\n";
      for (uint32_t i = 0; i < length; i++) {
         outputData << std::setprecision(std::numeric_limits<float>::max_digits10) << pStaticOut[i] << ", ";
      }
   }
   else if (pType == MATHLIB_FLOAT64) {
      for (uint32_t i = 0; i < length; i++) {
         outputData << std::setprecision(std::numeric_limits<double>::max_digits10) << pStaticIn0[i] << ", ";
      }
      outputData << "\n";
      for (uint32_t i = 0; i < length; i++) {
         outputData << std::setprecision(std::numeric_limits<double>::max_digits10) << pStaticIn1[i] << ", ";
      }
      outputData << "\n";
      for (uint32_t i = 0; i < length; i++) {
         outputData << std::setprecision(std::numeric_limits<double>::max_digits10) << pDst[i] << ", ";
      }
      outputData << "\n";
      for (uint32_t i = 0; i < length; i++) {
         outputData << std::setprecision(std::numeric_limits<double>::max_digits10) << pStaticOut[i] << ", ";
      }
   }
   outputData.close();
}

template void MATHLIB_test_print_data<float>(float   *pStaticIn,
                                             float   *pDst,
                                             float   *pStaticOut,
                                             uint32_t pType,
                                             uint32_t pTestId,
                                             size_t   length,
                                             char    *kernel_name);
template void MATHLIB_test_print_data<double>(double  *pStaticIn,
                                              double  *pDst,
                                              double  *pStaticOut,
                                              uint32_t pType,
                                              uint32_t pTestId,
                                              size_t   length,
                                              char    *kernel_name);

template void MATHLIB_test_print_data<float>(float   *pStaticIn0,
                                             float   *pStaticIn1,
                                             float   *pDst,
                                             float   *pStaticOut,
                                             uint32_t pType,
                                             uint32_t pTestId,
                                             size_t   length,
                                             char    *kernel_name);
template void MATHLIB_test_print_data<double>(double  *pStaticIn0,
                                              double  *pStaticIn1,
                                              double  *pDst,
                                              double  *pStaticOut,
                                              uint32_t pType,
                                              uint32_t pTestId,
                                              size_t   length,
                                              char    *kernel_name);
