#ifndef _VT_COMMON_H
#define _VT_COMMON_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#if defined(WIN32) || defined(_WIN32)
#	define VT_TARGET_WINDOWS 1
#elif defined(__APPLE__)
#	include <TargetConditionals.h>
#	if defined(TARGET_OS_MAC)
#		define VT_TARGET_MACOS 1
#	endif
#elif defined(__linux__) || defined(__unix__)
#	define VT_TARGET_LINUX 1
#endif

#if !defined(VT_TARGET_WINDOWS) && !defined(VT_TARGET_LINUX) && !defined(VT_TARGET_MACOS)
#	error Could not detect target platform
#endif

// Utility macros
#define VT_UNUSED(x) ((void)x)

#define VT_MIN(a, b) ((a) < (b) ? (a) : (b))
#define VT_MAX(a, b) ((a) > (b) ? (a) : (b))

#define VT_DEF(a, b) ((a) > 0 ? (a) : (b))

typedef enum vt_error {
	VT_ERROR_NONE,
	VT_ERROR_GENERIC,
	VT_ERROR_OUT_OF_MEMORY,
	VT_ERROR_MEM_OVERFLOW,
} vt_error;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

typedef size_t usize;

typedef union irect {
	u32 raw[4];

	struct {
		u32 x, y;
		u32 w, h;
	};
} irect;

typedef union frect {
	f32 raw[4];

	struct {
		f32 x, y;
		f32 w, h;
	};

	struct {
		f32 x1, y1;
		f32 x2, y2;
	};
} frect;

#endif
