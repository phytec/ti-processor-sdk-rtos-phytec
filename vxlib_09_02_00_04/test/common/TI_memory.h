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
**|        Copyright (c) 2007 - 2017 Texas Instruments Incorporated          |**
**|                                                                          |**
**|              All rights reserved not granted herein.                     |**
**|                                                                          |**
**|                         Limited License.                                 |**
**|                                                                          |**
**|  Texas Instruments Incorporated grants a world-wide, royalty-free,       |**
**|  non-exclusive license under copyrights and patents it now or            |**
**|  hereafter owns or controls to make, have made, use, import, offer to    |**
**|  sell and sell ("Utilize") this software subject to the terms herein.    |**
**|  With respect to the foregoing patent license, such license is granted   |**
**|  solely to the extent that any such patent is necessary to Utilize the   |**
**|  software alone.  The patent license shall not apply to any              |**
**|  combinations which include this software, other than combinations       |**
**|  with devices manufactured by or for TI ("TI Devices").  No hardware     |**
**|  patent is licensed hereunder.                                           |**
**|                                                                          |**
**|  Redistributions must preserve existing copyright notices and            |**
**|  reproduce this license (including the above copyright notice and the    |**
**|  disclaimer and (if applicable) source code license limitations below)   |**
**|  in the documentation and/or other materials provided with the           |**
**|  distribution                                                            |**
**|                                                                          |**
**|  Redistribution and use in binary form, without modification, are        |**
**|  permitted provided that the following conditions are met:               |**
**|                                                                          |**
**|    *  No reverse engineering, decompilation, or disassembly of this      |**
**|  software is permitted with respect to any software provided in binary   |**
**|  form.                                                                   |**
**|                                                                          |**
**|    *  any redistribution and use are licensed by TI for use only with    |**
**|  TI Devices.                                                             |**
**|                                                                          |**
**|    *  Nothing shall obligate TI to provide you with source code for      |**
**|  the software licensed and provided to you in object code.               |**
**|                                                                          |**
**|  If software source code is provided to you, modification and            |**
**|  redistribution of the source code are permitted provided that the       |**
**|  following conditions are met:                                           |**
**|                                                                          |**
**|    *  any redistribution and use of the source code, including any       |**
**|  resulting derivative works, are licensed by TI for use only with TI     |**
**|  Devices.                                                                |**
**|                                                                          |**
**|    *  any redistribution and use of any object code compiled from the    |**
**|  source code and any resulting derivative works, are licensed by TI      |**
**|  for use only with TI Devices.                                           |**
**|                                                                          |**
**|  Neither the name of Texas Instruments Incorporated nor the names of     |**
**|  its suppliers may be used to endorse or promote products derived from   |**
**|  this software without specific prior written permission.                |**
**|                                                                          |**
**|  DISCLAIMER.                                                             |**
**|                                                                          |**
**|  THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY      |**
**|  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE       |**
**|  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR      |**
**|  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL TI AND TI'S LICENSORS BE      |**
**|  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR     |**
**|  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF    |**
**|  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR         |**
**|  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,   |**
**|  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE    |**
**|  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,       |**
**|  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                      |**
**+--------------------------------------------------------------------------+**
*******************************************************************************/

#ifndef COMMON_TI_MEMORY_H_
#define COMMON_TI_MEMORY_H_ 1

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "TI_profile.h"

/* Handle COFF/ELF for linker cmd file symbols */
#if defined(__TI_EABI__)
#define kernel_size _kernel_size
#define data_size _data_size
#endif

#ifdef _HOST_BUILD
#ifdef _MSC_VER /* If using MS Visual Studio Compiler */
#define align_free _aligned_free
#else
#define align_free (free)
#endif
#else /* _HOST_BUILD */
#define align_free (free)
#endif /* _HOST_BUILD */

#define VXLIB_L3_RESULTS_BUFFER_SIZE 3 * 1024 * 1024

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Defines for types of test sequence data */
#define CONSTANT 0
#define SEQUENTIAL 1
#define SEQUENTIAL_NEGATIVE 2
#define RANDOM 3
#define RANDOM_SIGNED 4
#define STATIC 5
#define CUSTOM 6
#define RANDOM_DIMENSION 7
#define RANDOM_FAST 8
#define RANDOM_SMALL 9

/* Used in linker CMD file to determine code/data size */
extern uint32_t kernel_size;
extern uint32_t data_size;

/* 2D Memory handling utilities in TI_memory.c */
int32_t TI_compare_mem(void *a, void *b, int32_t n);
int32_t TI_compare_mem_2D(void    *a,
                          void    *b,
                          int64_t  tolerance,
                          int32_t  wrap_exception,
                          uint32_t width,
                          uint32_t height,
                          uint32_t stride,
                          uint8_t  elementSize);
int32_t TI_compare_mem_roi2D(void    *a,
                             void    *b,
                             int32_t  tolerance,
                             int32_t  wrap_exception,
                             uint32_t width,
                             uint32_t height,
                             uint32_t stride,
                             uint8_t  elementSize);
int32_t TI_compare_mem_roi2DStatic(void    *a,
                                   void    *b,
                                   int32_t  tolerance,
                                   int32_t  wrap_exception,
                                   uint32_t width,
                                   uint32_t height,
                                   uint32_t stride,
                                   uint8_t  elementSize);

int32_t TI_compare_mem_2D_float(void    *a,
                                void    *b,
                                double   log10tolerance,
                                double   abstolerance,
                                uint32_t width,
                                uint32_t height,
                                uint32_t stride,
                                uint8_t  elementSize);
void    fillConst(uint32_t val, void *array, uint32_t width, uint32_t height, uint32_t stride, uint8_t elementSize);
void    fillSeq(void *array, uint32_t width, uint32_t height, uint32_t stride, uint8_t elementSize);
void    fillSeqNeg(void *array, uint32_t width, uint32_t height, uint32_t stride, uint8_t elementSize);
void    fillRand(void *array, uint32_t width, uint32_t height, uint32_t stride, uint8_t elementSize);
void    fillRandSigned(void *array, uint32_t width, uint32_t height, uint32_t stride, uint8_t elementSize);
// void fillRandFast(void *array, uint32_t width, uint32_t height, uint32_t
// stride, uint8_t elementSize);
void     copyRoi(void *arrayDst, void *arraySrc, uint32_t width, uint32_t height, uint32_t stride, uint8_t elementSize);
void     copyRoiDualStride(void    *arrayDst,
                           void    *arraySrc,
                           uint32_t width,
                           uint32_t height,
                           uint32_t strideDst,
                           uint32_t strideSrc,
                           uint8_t  elementSize);
void     TI_fillBuffer(uint8_t  testPattern,
                       uint8_t  constFill,
                       void    *dest,
                       void    *src,
                       uint32_t width,
                       uint32_t height,
                       uint32_t stride,
                       uint8_t  elementSize,
                       char    *testPatternType);
uint32_t compare(uint8_t *dsp_output, uint8_t *natc_output, uint32_t width, uint32_t height, uint32_t stride);
uint32_t TI_randomDimensionLessThan(uint32_t maxValue);

/* 3D Memory handling utilities in TI_memory.c */
int32_t TI_compare_mem_3D(void    *a,
                          void    *b,
                          int32_t  tolerance,
                          int32_t  wrap_exception,
                          uint32_t dim_x,
                          uint32_t dim_y,
                          uint32_t dim_z,
                          uint32_t stride_y,
                          uint32_t stride_z,
                          uint8_t  elementSize);
// int32_t TI_compare_mem_roi3D(void *a, void *b, int32_t tolerance, int32_t
// wrap_exception, uint32_t dim_x, uint32_t dim_y, uint32_t dim_z, uint32_t
// stride_y, uint32_t stride_z, uint8_t elementSize);
int32_t TI_compare_mem_roi3DStatic(void    *a,
                                   void    *b,
                                   int32_t  tolerance,
                                   int32_t  wrap_exception,
                                   uint32_t dim_x,
                                   uint32_t dim_y,
                                   uint32_t dim_z,
                                   uint32_t stride_y,
                                   uint32_t stride_z,
                                   uint8_t  elementSize);
int32_t TI_compare_mem_3D_float(void    *a,
                                void    *b,
                                double   log10tolerance,
                                double   abstolerance,
                                uint32_t dim_x,
                                uint32_t dim_y,
                                uint32_t dim_z,
                                uint32_t stride_y,
                                uint32_t stride_z,
                                uint8_t  elementSize);
void    fillConst3D(uint32_t val,
                    void    *array,
                    uint32_t dim_x,
                    uint32_t dim_y,
                    uint32_t dim_z,
                    uint32_t stride_y,
                    uint32_t stride_z,
                    uint8_t  elementSize);
void    fillSeq3D(void    *array,
                  uint32_t dim_x,
                  uint32_t dim_y,
                  uint32_t dim_z,
                  uint32_t stride_y,
                  uint32_t stride_z,
                  uint8_t  elementSize);
void    fillSeqNeg3D(void    *array,
                     uint32_t dim_x,
                     uint32_t dim_y,
                     uint32_t dim_z,
                     uint32_t stride_y,
                     uint32_t stride_z,
                     uint8_t  elementSize);
void    fillRand3D(void    *array,
                   uint32_t dim_x,
                   uint32_t dim_y,
                   uint32_t dim_z,
                   uint32_t stride_y,
                   uint32_t stride_z,
                   uint8_t  elementSize);
void    copyRoi3D(void    *arrayDst,
                  void    *arraySrc,
                  uint32_t dim_x,
                  uint32_t dim_y,
                  uint32_t dim_z,
                  uint32_t stride_y,
                  uint32_t stride_z,
                  uint8_t  elementSize);
void    TI_fillBuffer3D(uint8_t  testPattern,
                        uint8_t  constFill,
                        void    *dest,
                        void    *src,
                        uint32_t dim_x,
                        uint32_t dim_y,
                        uint32_t dim_z,
                        uint32_t stride_y,
                        uint32_t stride_z,
                        uint8_t  elementSize,
                        char    *testPatternType);
int32_t TI_compare_mem_3DDualStride(void    *a,
                                    void    *b,
                                    int32_t  tolerance,
                                    int32_t  wrap_exception,
                                    uint32_t dim_x,
                                    uint32_t dim_y,
                                    uint32_t dim_z,
                                    uint32_t strideA_y,
                                    uint32_t strideA_z,
                                    uint32_t strideB_y,
                                    uint32_t strideB_z,
                                    uint8_t  elementSize);

/* 2D Float Memory handling utilities in TI_memory.c */
void copyRoi_float(void    *arrayDst,
                   void    *arraySrc,
                   uint32_t width,
                   uint32_t height,
                   uint32_t stride,
                   uint8_t  elementSize);
void fillRand_float(void *array, uint32_t width, uint32_t height, uint32_t stride, uint8_t elementSize);
void TI_fillBuffer_float(uint8_t  testPattern,
                         void    *constFill,
                         void    *dest,
                         void    *src,
                         uint32_t width,
                         uint32_t height,
                         uint32_t stride,
                         uint8_t  elementSize,
                         char    *testPatternType);

/* Display memory reqirements */
static inline void TI_kernel_memory(void)
{
#if !defined(__ONESHOTTEST) && !defined(_HOST_BUILD)
//    uint32_t kern_size = (uint32_t)((uint64_t)&kernel_size & 0xFFFFFFFF);
//    uint32_t dat_size = (uint32_t)((uint64_t)&data_size & 0xFFFFFFFF);
#endif /* __ONESHOTTEST */

   sprintf(TI_test_print_buffer, "--------------------------------------------------------------------"
                                 "--------------------------------------------------------------------"
                                 "-----------------------------------------\n");
   TI_test_print_string(TI_test_print_buffer);
#if !defined(__ONESHOTTEST) && !defined(_HOST_BUILD)
   /* Provide memory information */
   sprintf(TI_test_print_buffer, "Memory Usage:\n");
   TI_test_print_string(TI_test_print_buffer);
   //    sprintf(TI_test_print_buffer, "  Code size:  %9u bytes\n", kern_size);
   TI_test_print_string(TI_test_print_buffer);
   //    sprintf(TI_test_print_buffer, "  Data size:  %9u bytes\n", dat_size);
   TI_test_print_string(TI_test_print_buffer);
#endif /* __ONESHOTTEST */
   TI_profile_stack_memory();
}

/* Run-time initialation of the fast heap for library
 * Called only if heap is linked into L2SRAM */
uint32_t TI_meminit(uint32_t size);

/* Following should be called to allocate buffers passed to kernel functions
 * for each test vector */
void *TI_memalign(size_t alignment, size_t size);
void *TI_malloc(size_t size);

/* Following should be called to de-allocate buffers passed to kernel functions
 * for each test vector */
void    TI_align_free(void *ptr);
void    TI_free(void *ptr);
int32_t TI_get_heap_size(void);
void    TI_memError(char *fn);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* COMMON_TI_MEMORY_H_ */

/* =========================================================================*/
/*  End of file:  TI_memory.h                                             */
/* =========================================================================*/
