/******************************************************************************/
/*!
 * \file
 */
/* Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
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

#ifndef C71_FFTLIB_DEBUG_H_
#define C71_FFTLIB_DEBUG_H_ 1

/*******************************************************************************
 *
 * INCLUDES
 *
 ******************************************************************************/

#include <float.h>  // for max float, double values
#include <limits.h> // for min, max integer values

#include "../FFTLIB_bufParams.h"
#include "../FFTLIB_types.h"

#include "FFTLIB_debugPrintVector.h"

/*******************************************************************************
 *
 * Function prototypes
 *
 ******************************************************************************/

#ifdef __cplusplus

extern "C" {
#endif /* __cplusplus */

void FFTLIB_debugPrintMatrix1D(void *matrix, const FFTLIB_bufParams1D_t *params);
void FFTLIB_debugPrintMatrix(void *matrix, const FFTLIB_bufParams2D_t *params);
void FFTLIB_debugPrintMatrix3D(void *matrix, const FFTLIB_bufParams3D_t *params);

void FFTLIB_debugPrint3DVReg(void *matrix, const FFTLIB_bufParams3D_t *params);
void FFTLIB_debugPrintBufParams1D(const FFTLIB_bufParams1D_t *params);
void FFTLIB_debugPrintBufParams(const FFTLIB_bufParams2D_t *params);
void FFTLIB_debugPrintBufParams3D(const FFTLIB_bufParams3D_t *params);

void FFTLIB_debugPrintMMAReg(const int32_t regNumber);
   

/******************************************************************************/

//#if defined(_HOST_BUILD)
//   // x86 GCC assembly code for printing the stack pointer
//   // this version defines the function in asm, which avoid all compiler warnings
//   // if this stops working, a simpler version is commented below
//   unsigned long *get_stack_ptr(void);
//   __asm__(
//          "get_stack_ptr:\n"
//          "mov %rsp, %rax\n"
//          "ret\n"
//          );
//
//   static inline unsigned long *get_stack_ptr(void) {
//      __asm__(
//              "mov %rsp, %rax"
//              );
//   }
//#define FFTLIB_DEBUGPRINT_STACK_PTR FFTLIB_DEBUGPRINTFN(1, "Stack Pointer: %p\n", get_stack_ptr())
//#else
//#define FFTLIB_DEBUGPRINT_STACK_PTR
//#endif  // #if defined(_HOST_BUILD)
   
//   static inline void FFTLIB_debugPrintStackPtr(void){
//#if defined(_HOST_BUILD)
//      unsigned long *stack_ptr = get_stack_ptr();
//      FFTLIB_DEBUGPRINTFN(0, "Stack Pointer: %p\n", stack_ptr);
//#endif
//   }

   
#ifdef __cplusplus
}
#endif /* __cplusplus */

// C++ linkage only
#ifdef __cplusplus

static inline uint8_t FFTLIB_firstElement(c7x::uchar_vec vector){
   return *((uint8_t*)(&vector));
}

static inline int8_t FFTLIB_firstElement(c7x::char_vec vector){
   return *((int8_t*)(&vector));
}

static inline uint16_t FFTLIB_firstElement(c7x::ushort_vec vector){
   return *((uint16_t*)(&vector));
}

static inline int16_t FFTLIB_firstElement(c7x::short_vec vector){
   return *((int16_t*)(&vector));
}


#endif // #ifdef __cplusplus

#endif /* C71_FFTLIB_DEBUG_H_ */
