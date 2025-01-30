#ifndef COMMON_MATHLIB_MEMORY_H_
#define COMMON_MATHLIB_MEMORY_H_ 1

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "MATHLIB_profile.h"

/* Handle COFF/ELF for linker cmd file symbols */
#if defined(__MATHLIB_EABI__)
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

#define MATHLIB_L3_RESULTS_BUFFER_SIZE 3 * 1024 * 1024

template <typename T>
int32_t MATHLIB_compare_mem(T *a, T *b, size_t length, double tolerance, MATHLIB_TestMetric testMetric);

template <typename T>
int32_t MATHLIB_compare_mem(T *a, T *b, size_t width, size_t height, size_t stride, double tolerance);

template <typename T> int32_t MATHLIB_compare_mem(T *a, T *b, size_t width, size_t height);

template <typename T> void MATHLIB_fillBuffer(T *src, T *dst, size_t length);
template <typename T> void MATHLIB_fillBuffer(T *src, T *dst, size_t width, size_t height);

template <typename T> void MATHLIB_fillBuffer(T *src, T *dst, size_t width, size_t height, size_t stride);

template <typename T>
void MATHLIB_fillBuffer(T *src, T *dst, size_t width, size_t height, size_t strideSrc, size_t strideDst);

#define MATHLIB_ALIGN_SHIFT_64BYTES 6  //!< Number of bits to shift for 64-byte memory alignment
#define MATHLIB_ALIGN_SHIFT_128BYTES 7 //!< Number of bits to shift for 128-byte memory alignment
#define MATHLIB_ALIGN_SHIFT_256BYTES 8 //!< Number of bits to shift for 256-byte memory alignment

#define MATHLIB_ALIGN_64BYTES (1 << MATHLIB_ALIGN_SHIFT_64BYTES)   //!< Align by 64-byte memory alignment
#define MATHLIB_ALIGN_128BYTES (1 << MATHLIB_ALIGN_SHIFT_128BYTES) //!< Align by 128-byte memory alignment
#define MATHLIB_ALIGN_256BYTES (1 << MATHLIB_ALIGN_SHIFT_256BYTES) //!< Align by 256-byte memory alignment

#define MATHLIB_L2DATA_ALIGN_SHIFT MATHLIB_ALIGN_SHIFT_64BYTES //!< Set the default L2 data alignment

/*! @brief Macro that specifies the alignment of data buffers in L2 memory for
 * optimal performance */
#define MATHLIB_L2DATA_ALIGNMENT (((uint32_t) 1) << ((uint32_t) MATHLIB_L2DATA_ALIGN_SHIFT))

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Used in linker CMD file to determine code/data size */
extern uint32_t kernel_size;
extern uint32_t data_size;

/* Display memory requirements */
static inline void MATHLIB_kernel_memory(void)
{
#if !defined(__ONESHOTTEST) && !defined(_HOST_BUILD)
//    uint32_t kern_size = (uint32_t)((uint64_t)&kernel_size & 0xFFFFFFFF);
//    uint32_t dat_size = (uint32_t)((uint64_t)&data_size & 0xFFFFFFFF);
#endif /* __ONESHOTTEST */

   sprintf(MATHLIB_test_print_buffer, "--------------------------------------------------------------------"
                                      "--------------------------------------------------------------------"
                                      "-----------------------------------------\n");
   MATHLIB_test_print_string(MATHLIB_test_print_buffer);
#if !defined(__ONESHOTTEST) && !defined(_HOST_BUILD)
   /* Provide memory information */
   sprintf(MATHLIB_test_print_buffer, "Memory Usage:\n");
   MATHLIB_test_print_string(MATHLIB_test_print_buffer);
   //    sprintf(MATHLIB_test_print_buffer, "  Code size:  %9u bytes\n",
   //    kern_size);
   MATHLIB_test_print_string(MATHLIB_test_print_buffer);
   //    sprintf(MATHLIB_test_print_buffer, "  Data size:  %9u bytes\n",
   //    dat_size);
   MATHLIB_test_print_string(MATHLIB_test_print_buffer);
#endif /* __ONESHOTTEST */
   MATHLIB_profile_stack_memory();
}

/* Run-time initialation of the fast heap for library
 * Called only if heap is linked into L2SRAM */
uint32_t MATHLIB_meminit(uint32_t size);

/* Following should be called to allocate buffers passed to kernel functions
 * for each test vector */
void *MATHLIB_memalign(size_t alignment, size_t size);
void *MATHLIB_malloc(size_t size);

/* Following should be called to de-allocate buffers passed to kernel functions
 * for each test vector */
void    MATHLIB_align_free(void *ptr);
void    MATHLIB_free(void *ptr);
int32_t MATHLIB_get_heap_size(void);
void    MATHLIB_memError(char *fn);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* COMMON_MATHLIB_MEMORY_H_ */
