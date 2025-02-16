#ifndef VT_WINDOW_H
#define VT_WINDOW_H

#include "common.h"
#include <SDL3/SDL_video.h>

typedef struct VT_Window VT_Window;

VT_Window *vt_create_window(i32 w, i32 h, const char *title);
void vt_destroy_window(VT_Window *win);

bool vt_window_should_close(const VT_Window *win);

#endif
