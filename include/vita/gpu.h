#ifndef _VT_VITA_GPU_H
#define _VT_VITA_GPU_H

#include "cglm/types-struct.h"
#include "glad/gl.h"
#include "gpu/color.h"
#include "sokol/sokol_gfx.h"

#define VT_GPU_ATTR_POS		 0
#define VT_GPU_ATTR_UV		 1
#define VT_GPU_ATTR_COLOR	 2
#define VT_GPU_UB_TEX0		 0

#define VT_GPU_VERTEX_SLOT	 0
#define VT_GPU_FRAGMENT_SLOT 1

typedef enum VT_PrimitiveType {
	VT_PRIMITIVETYPE_POINTS = 0,
	VT_PRIMITIVETYPE_LINES,
	VT_PRIMITIVETYPE_LINE_STRIP,
	VT_PRIMITIVETYPE_TRIANGLES,
	VT_PRIMITIVETYPE_TRIANGLE_STRIP,
	_VT_PRIMITIVETYPE_COUNT,
} VT_PrimitiveType;

typedef struct VT_Vertex {
	vec3s position;
	vec2s texcoord;
	u8color color;
} VT_Vertex;

typedef struct VT_Uniform {
	usize vs_size;
	usize fs_size;
	void *ptr;
} VT_Uniform;

vt_error vt_gpu_init_resources(void);
void vt_gpu_clean_resources(void);

sg_image vt_gpu_get_white_image(void);
sg_sampler vt_gpu_get_nearest_sampler(void);
sg_shader vt_gpu_get_common_shader(void);

sg_pipeline vt_gpu_make_pipeline(sg_shader shdr, VT_PrimitiveType primitive);

#endif
