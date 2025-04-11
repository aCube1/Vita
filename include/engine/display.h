// clang-format Language: C
#ifndef _VT_ENGINE_DISPLAY_H
#define _VT_ENGINE_DISPLAY_H

#include "cglm/types-struct.h"
#include "types.h"

typedef struct vt_display_window {
	const char *title;
	vec2s size;
} vt_display_window;

typedef struct vt_display_desc {
	vt_display_window window;
} vt_display_desc;

typedef struct vt_display {
	vt_display_desc desc;

	struct GLFWwindow *window;
	struct vt_renderer *renderer;
} vt_display;

vt_error vt_init_display(vt_display *display, const vt_display_desc *desc);
void vt_deinit_display(vt_display *display);

void vt_update_display(const vt_display *display);

#endif
