#ifndef VT_WINDOW_H
#define VT_WINDOW_H

#include "cglm/types-struct.h"
#include "utils.h"

typedef struct VT_Window VT_Window;

typedef struct VT_WindowAttributes {
	const char *title;
	ivec2s size;
	ivec2s framesize;
} VT_WindowAttribues;

VT_Window *vt_create_window(i32 w, i32 h, const char *title);
void vt_destroy_window(VT_Window *win);

void vt_window_query_attributes(const VT_Window *win, VT_WindowAttribues *attrs);

#endif
