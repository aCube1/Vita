#ifndef _VT_COMMON_H
#define _VT_COMMON_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

typedef enum VT_Error {
	VT_ERROR_NONE,
	VT_ERROR_OUT_OF_MEMORY,
	VT_ERROR_MEM_OVERFLOW,
} VT_Error;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef size_t usize;

typedef union irect {
	u32 raw[4];

	struct {
		u32 x, y;
		u32 w, h;
	};
} irect;

#endif
