#ifndef _VT_VITA_GPU_COLOR_H
#define _VT_VITA_GPU_COLOR_H

#include "common.h"

#define VT_COLOR_BLACK ((u8color) { { 0x00, 0x00, 0x00, 0xff } })
#define VT_COLOR_WHITE ((u8color) { { 0xff, 0xff, 0xff, 0xff } })
#define VT_COLOR_RED   ((u8color) { { 0xff, 0x00, 0x00, 0xff } })
#define VT_COLOR_GREEN ((u8color) { { 0x00, 0xff, 0x00, 0xff } })
#define VT_COLOR_BLUE  ((u8color) { { 0x00, 0x00, 0xff, 0xff } })

typedef union u8color {
	u8 raw[4];

	struct {
		u8 r, g, b, a;
	};
} u8color;

#endif
