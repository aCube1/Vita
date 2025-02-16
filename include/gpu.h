#ifndef VT_GPU_H
#define VT_GPU_H

#include "cglm/types-struct.h"
#include "sokol_gfx.h"
#include <glad/gl.h>

#define VT_GPU_ATTR_POS		   0
#define VT_GPU_ATTR_UV		   1
#define VT_GPU_ATTR_COLOR	   2

#define VT_GPU_UB_VERTEXPARAMS 0
#define VT_GPU_UB_TEX0		   0

typedef struct VT_VertexParams {
	mat4s mvp;
} VT_VertexParams;

bool vt_gpu_setup(void);
void vt_gpu_shutdown(void);

sg_image vt_gpu_get_white_image(void);
sg_sampler vt_gpu_get_nearest_sampler(void);
sg_shader vt_gpu_get_common_shader(void);

#endif
