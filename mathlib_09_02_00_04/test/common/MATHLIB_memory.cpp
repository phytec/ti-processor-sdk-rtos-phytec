/******************************************************************************/
/*!
 * \file MATHLIB_memory.cpp
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

#include "MATHLIB_memory.h"
#include "MATHLIB_profile.h"
#include "MATHLIB_test.h"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iostream>

#if defined(_HOST_BUILD)
#include <malloc.h>
#endif

/* Heap global variables */
#if !defined(_HOST_BUILD)

/* Handle COFF/ELF for linker cmd file symbols */
#if defined(__TI_EABI__)
#define sram_start _sram_start
#endif

/* These extern variables are created by the linker command files */
extern uint32_t sram_start;

static void *fast_heap_start;
static void *fast_heap_end;

#if defined(HEAP_L2SRAM)
static uint32_t fast_heap_size;
static void    *fast_heap_front;
#endif

/* The linker will place this section in L2SRAM after linked section, if any
 * Therefore, the address of this variable indicates the first available
 * address in the L2SRAM for the fast heap to use */
#pragma DATA_SECTION(".sram_start")
static uint8_t sram_start_placeholder;

#endif /* !defined(_HOST_BUILD) */

uint32_t MATHLIB_meminit(uint32_t size)
{
   uint32_t status = 0;

#if !defined(_HOST_BUILD)
   fast_heap_start = (void *) (size_t) &sram_start_placeholder;
   fast_heap_end   = (void *) ((size_t) &sram_start + size * 1024);
   if (fast_heap_start > fast_heap_end) {
      status = 1; /* ERROR: There is no room in the L2SRAM. */
   }
#if defined(HEAP_L2SRAM)
   else {
      fast_heap_size  = (size_t) fast_heap_end - (size_t) fast_heap_start;
      fast_heap_front = fast_heap_start;
   }
#endif
#endif
   return (status);
}

void *MATHLIB_memalign(size_t alignment, size_t size)
{
#if !defined(_HOST_BUILD) && defined(HEAP_L2SRAM)
   size_t temp_start = ((size_t) fast_heap_front + (alignment - 1)) & ~(size_t) (alignment - 1);

   if ((temp_start + size) > (size_t) fast_heap_end) {
      temp_start = 0;
   }
   else {
      fast_heap_front = (void *) (temp_start + size);
   }
   return ((void *) temp_start);
#else
#ifdef WIN32
   return (_aligned_malloc(size, alignment));
#else
   return (memalign(alignment, size));
#endif
#endif
}

void *MATHLIB_malloc(size_t size)
{
#if !defined(_HOST_BUILD) && defined(HEAP_L2SRAM)
   size_t temp_start = ((size_t) fast_heap_front + (sizeof(long double) - 1)) & ~(size_t) (sizeof(long double) - 1);

   if ((temp_start + size) > (size_t) fast_heap_end) {
      temp_start = 0;
   }
   else {
      fast_heap_front = (void *) (temp_start + size);
   }
   return ((void *) temp_start);
#else
   return (malloc(size));
#endif
}

void MATHLIB_align_free(void *ptr)
{
#if !defined(_HOST_BUILD) && defined(HEAP_L2SRAM)
   fast_heap_front = fast_heap_start;
#else
   align_free(ptr);
#endif
}

void MATHLIB_free(void *ptr)
{
#if !defined(_HOST_BUILD) && defined(HEAP_L2SRAM)
   fast_heap_front = fast_heap_start;
#else
   free(ptr);
#endif
}

int32_t MATHLIB_get_heap_size(void)
{
#if !defined(_HOST_BUILD) && defined(HEAP_L2SRAM)
   return (fast_heap_size);
#else
   return (0);
#endif
}

void MATHLIB_memError(char *fn)
{
#if !defined(NO_PRINTF)
   sprintf(MATHLIB_test_print_buffer,
           "%s:\n    ERROR: This test can not run given the build-time "
           "configuration of L2SRAM.\n",
           fn);
   MATHLIB_test_print_string(MATHLIB_test_print_buffer);
#endif
}

template <typename T>
int32_t MATHLIB_compare_mem(T *a, T *b, size_t length, double tolerance, MATHLIB_TestMetric testMetric)
{
   size_t col = 0;
   if (testMetric == MATHLIB_ABSOLUTE_TOLERANCE) {
      double absDiff;
      for (col = 0; col < length; col++) {
         if (a[col] != b[col]) {
            absDiff = fabs(a[col] - b[col]);
            if (absDiff > tolerance) {
               std::cout << "Failure at column: " << col << " val1= " << a[col] << " val2 = " << b[col] << "\n";
               return (int32_t) MATHLIB_TEST_KERNEL_FAIL;
            }
            else {
               /* std::cout << "Warning at column: " << col << " val1= " << a[col]
                */
               /*           << " val2 = " << b[col] << "\n"; */
            }
         }
      }
   }
   else if (testMetric == MATHLIB_PERCENT_ERROR) {
      double percentError;
      for (col = 0; col < length; col++) {
         if (a[col] != b[col]) {
            percentError = fabs((a[col] - b[col]) / b[col]) * 100;
            if (percentError > tolerance) {
               std::cout << "Failure at column: " << col << " val1= " << a[col] << " val2 = " << b[col] << "\n";
               return (int32_t) MATHLIB_TEST_KERNEL_FAIL;
            }
            else {
               /* std::cout << "Warning at column: " << col << " val1= " << a[col]
                */
               /*           << " val2 = " << b[col] << "\n"; */
            }
         }
      }
   }

   // return PASS/FAIL
   return ((int32_t) MATHLIB_TEST_KERNEL_PASS);
}

template int32_t
MATHLIB_compare_mem<float>(float *a, float *b, size_t length, double tolerance, MATHLIB_TestMetric testMetric);
template int32_t
MATHLIB_compare_mem<double>(double *a, double *b, size_t length, double tolerance, MATHLIB_TestMetric testMetric);

template <typename T> int32_t MATHLIB_compare_mem(T *a, T *b, size_t width, size_t height, double tolerance)
{
   size_t row = 0, col = 0;
   size_t offset = 0;
   size_t stride = width;
   double absDiff;

   for (row = 0; row < height; row++) {
      for (col = 0; col < width; col++) {
         offset = (row * (stride) + col);
         if (a[offset] != b[offset]) {
            absDiff = fabs(a[offset] - b[offset]);
            if (absDiff > tolerance) {
               std::cout << "Failure at row: " << row << " column: " << col << " val1= " << a[offset]
                         << " val2 = " << b[offset] << "\n";
               return (int32_t) MATHLIB_TEST_KERNEL_FAIL;
            }
            else {
               std::cout << "Warning at row: " << row << " column: " << col << " val1= " << a[offset]
                         << " val2 = " << b[offset] << "\n";
            }
         }
      }
   }

   /* Return PASS/FAIL */
   return ((int32_t) MATHLIB_TEST_KERNEL_PASS);
}

template int32_t MATHLIB_compare_mem<float>(float *a, float *b, size_t width, size_t height, double tolerance);
template int32_t MATHLIB_compare_mem<double>(double *a, double *b, size_t width, size_t height, double tolerance);

template <typename T> int32_t MATHLIB_compare_mem(T *a, T *b, size_t width, size_t height)
{
   size_t row = 0, col = 0;
   size_t offset = 0;
   size_t stride = width;

   for (row = 0; row < height; row++) {
      for (col = 0; col < width; col++) {
         offset = (row * (stride) + col);
         if (a[offset] != b[offset]) {
            std::cout << "Failure at row: " << row << " column: " << col << " val1= " << static_cast<int32_t>(a[offset])
                      << " val2 = " << static_cast<int32_t>(b[offset]) << "\n";
            return (int32_t) MATHLIB_TEST_KERNEL_FAIL;
         }
      }
   }

   /* Return PASS/FAIL */
   return ((int32_t) MATHLIB_TEST_KERNEL_PASS);
}

template int32_t MATHLIB_compare_mem<int8_t>(int8_t *a, int8_t *b, size_t width, size_t height);
template int32_t MATHLIB_compare_mem<int16_t>(int16_t *a, int16_t *b, size_t width, size_t height);
template int32_t MATHLIB_compare_mem<int32_t>(int32_t *a, int32_t *b, size_t width, size_t height);

template <typename T>
void MATHLIB_copyRoiDualStride(T     *arraySrc,
                               T     *arrayDst,
                               size_t width,
                               size_t height,
                               size_t strideSrc,
                               size_t strideDst)
{
   size_t row, col;

   T *restrict dst = (T *) arrayDst;
   T *restrict src = (T *) arraySrc;

   for (row = 0; row < height; row++) {
      for (col = 0; col < width; col++) {
         dst[row * strideDst + col] = src[row * strideSrc + col];
      }
   }
}

template <typename T> void MATHLIB_fillBuffer(T *src, T *dst, size_t length)
{

   MATHLIB_copyRoiDualStride(src, dst, length, 1, 1, 1);
}

template void MATHLIB_fillBuffer<int8_t>(int8_t *src, int8_t *dst, size_t length);
template void MATHLIB_fillBuffer<int16_t>(int16_t *src, int16_t *dst, size_t length);
template void MATHLIB_fillBuffer<int32_t>(int32_t *src, int32_t *dst, size_t length);
template void MATHLIB_fillBuffer<float>(float *src, float *dst, size_t length);
template void MATHLIB_fillBuffer<double>(double *src, double *dst, size_t length);

template <typename T> void MATHLIB_fillBuffer(T *src, T *dst, size_t width, size_t height)
{

   MATHLIB_copyRoiDualStride(src, dst, width, height, width, width);
}

template void MATHLIB_fillBuffer<int8_t>(int8_t *src, int8_t *dst, size_t width, size_t height);

template void MATHLIB_fillBuffer<int16_t>(int16_t *src, int16_t *dst, size_t width, size_t height);
template void MATHLIB_fillBuffer<int32_t>(int32_t *src, int32_t *dst, size_t width, size_t height);
template void MATHLIB_fillBuffer<float>(float *src, float *dst, size_t width, size_t height);
template void MATHLIB_fillBuffer<double>(double *src, double *dst, size_t width, size_t height);

template <typename T>
void MATHLIB_fillBuffer(T *src, T *dst, size_t width, size_t height, size_t strideSrc, size_t strideDst)
{

   MATHLIB_copyRoiDualStride(src, dst, width, height, strideSrc, strideDst);
}

template void
MATHLIB_fillBuffer<int8_t>(int8_t *src, int8_t *dst, size_t width, size_t height, size_t strideSrc, size_t strideDst);

template void MATHLIB_fillBuffer<int16_t>(int16_t *src,
                                          int16_t *dst,
                                          size_t   width,
                                          size_t   height,
                                          size_t   strideSrc,
                                          size_t   strideDst);

template void MATHLIB_fillBuffer<int32_t>(int32_t *src,
                                          int32_t *dst,
                                          size_t   width,
                                          size_t   height,
                                          size_t   strideSrc,
                                          size_t   strideDst);
template void
MATHLIB_fillBuffer<float>(float *src, float *dst, size_t width, size_t height, size_t strideSrc, size_t strideDst);
template void
MATHLIB_fillBuffer<double>(double *src, double *dst, size_t width, size_t height, size_t strideSrc, size_t strideDst);
