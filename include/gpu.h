#ifndef VT_GPU_H
#define VT_GPU_H

#include "utils.h"

#include "sokol_gfx.h"
#include <glad/gl.h>

bool vt_gpu_setup(void);
void vt_gpu_shutdown(void);

sg_image vt_gpu_get_white_image(void);
sg_sampler vt_gpu_get_nearest_sampler(void);

#endif
