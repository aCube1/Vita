#ifndef VT_GPU_RENDERER_H
#define VT_GPU_RENDERER_H

#include "cglm/types-struct.h"
#include "common.h"
#include "gpu.h"

typedef struct VT_RenderState {
	ivec2s framesize;
	irect viewport;
	mat4s projview;
	mat3s transform;
	u8color color;
	sg_pipeline pipeline;
	sg_pass render_pass;
	VT_UniformData *uniform;

	u32 _base_vertex;
	u32 _base_command;
} VT_RenderState;

typedef struct VT_Renderer VT_Renderer;

VT_Renderer *vt_create_renderer(void);
void vt_destroy_renderer(VT_Renderer *render);

void vt_render_begin(VT_Renderer *render, ivec2s framesize);
void vt_render_end(VT_Renderer *render);
void vt_render_flush(VT_Renderer *render);

void vt_render_draw(VT_Renderer *render, const VT_Vertex *vertices, u32 count);

#endif
