#ifndef _VT_VITA_GPU_RENDERER_H
#define _VT_VITA_GPU_RENDERER_H

#include "cglm/types-struct.h"
#include "common.h"
#include "vita/gpu.h"

typedef struct vt_batchstate {
	ivec2s framesize;
	irect viewport;
	mat4s projview;
	mat3s transform;
	mat4s mvp;
	u8color color;
	sg_pipeline pipeline;
	sg_pass render_pass;

	vt_uniform uniform;

	u32 _base_vertex;
	u32 _base_command;
	u32 _base_uniform;
} vt_batchstate;

typedef struct vt_renderer vt_renderer;

vt_renderer *vt_create_renderer(void);
void vt_destroy_renderer(vt_renderer *render);

vt_error vt_get_render_error(vt_renderer *render);

void vt_render_begin(vt_renderer *render, ivec2s framesize);
void vt_render_end(vt_renderer *render);
void vt_render_flush(vt_renderer *render);

void vt_set_render_pipeline(vt_renderer *render, sg_pipeline pip);
void vt_set_render_uniform(
	vt_renderer *render,
	const void *vs_data,
	usize vs_size,
	const void *fs_data,
	usize fs_size
);

void vt_render_geometry(
	vt_renderer *render, vt_primitive_type primitive, const vt_vertex *vertices, u32 count
);

#endif
