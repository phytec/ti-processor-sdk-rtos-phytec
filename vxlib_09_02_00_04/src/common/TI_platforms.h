#ifndef TI_PLATFORMS_H__
#define TI_PLATFORMS_H__

#ifdef _HOST_BUILD
#include <c7x.h>
#include <assert.h>
#define restrict
#define _nassert assert
#define __int40_t long long
#ifdef _MSC_VER /* If using MS Visual Studio Compiler */
#define inline __inline
#define memalign(x, y) _aligned_malloc(y, x)
#if !defined(__cplusplus)
#if (_MSC_VER < 1800) /* Before MS Visual Studio 2013 (v12), C99 was not supported */
typedef unsigned int _Bool;
#define bool _Bool
#define true 1
#define false 0
#define round(x) ((x) + 0.5)
#else
#include <stdbool.h>
#endif
#endif
#else
#include <stdbool.h>
#endif

#else /* not _HOST_BUILD */

#include <c7x.h>
// #include <c7x_strm.h>
// #include <c7x_luthist.h>
#if defined(C6X_MIGRATION)
#include <c6x_migration.h>
#endif
#include <stdbool.h>

#endif /* _HOST_BUILD */

#endif /* ! TI_PLATFORMS_H__ */
