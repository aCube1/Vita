#ifndef VT_GPU_H
#define VT_GPU_H

#include "utils.h"
#include <SDL3/SDL_video.h>

enum {
	VT_GPU_NONE,
	VT_GPU_VSYNC,
};

typedef struct VT_GPU VT_GPU;

VT_GPU *vt_gpu_init(SDL_Window *win, u32 flags);
void vt_gpu_quit(VT_GPU *gpu);

void vt_gpu_set_vsync(i32 vsync);

#endif
