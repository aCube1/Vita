#ifndef _VT_VITA_WINDOW_H
#define _VT_VITA_WINDOW_H

#include "cglm/types-struct.h"
#include "common.h"

typedef struct GLFWwindow GLFWwindow;

typedef struct vt_window {
	GLFWwindow *handle;

	ivec2s size;
	ivec2s framesize;
	bool should_close;
} vt_window;

vt_error vt_create_window(vt_window *win, i32 w, i32 h, const char *title);
void vt_destroy_window(vt_window *win);

void vt_window_update(vt_window *win);

#endif
