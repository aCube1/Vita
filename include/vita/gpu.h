#ifndef _VT_VITA_GPU_H
#define _VT_VITA_GPU_H

#include "common.h"
#include "glad/gl.h"
#include "sokol/sokol_gfx.h"

#define VT_GPU_VERTEX_SLOT	 0
#define VT_GPU_FRAGMENT_SLOT 1

vt_error vt_gpu_init_resources(void);
void vt_gpu_clean_resources(void);

sg_image vt_gpu_get_white_image(void);
sg_sampler vt_gpu_get_nearest_sampler(void);

#endif
