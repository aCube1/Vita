#ifndef _VT_COMMON_H
#define _VT_COMMON_H

#include <stddef.h>
#include <stdint.h>

// Default colors
#define VT_COLOR_BLACK ((u8color) { { 0x00, 0x00, 0x00, 0xff } })
#define VT_COLOR_WHITE ((u8color) { { 0xff, 0xff, 0xff, 0xff } })
#define VT_COLOR_RED   ((u8color) { { 0xff, 0x00, 0x00, 0xff } })
#define VT_COLOR_GREEN ((u8color) { { 0x00, 0xff, 0x00, 0xff } })
#define VT_COLOR_BLUE  ((u8color) { { 0x00, 0x00, 0xff, 0xff } })

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

typedef union u8color {
	u8 raw[4];

	struct {
		u8 r, g, b, a;
	};
} u8color;

#endif
