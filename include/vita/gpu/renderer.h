#ifndef _VT_VITA_GPU_RENDERER_H
#define _VT_VITA_GPU_RENDERER_H

#include "cglm/types-struct.h"
#include "common.h"
#include "vita/gpu.h"
#include "vita/gpu/uniform.h"

typedef struct vt_batchstate {
	ivec2s framesize;
	irect viewport;
	mat4s projview;
	mat3s transform;
	mat4s mvp;
	u8color color;
	sg_pipeline pipeline;
	sg_pass render_pass;
	vt_uniform uniform_blocks[VT_MAX_UNIFORMBLOCK_SLOTS];
	vt_texture_uniform textures[VT_MAX_TEXTURE_UNIFORM_SLOTS];

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
void vt_set_render_uniform(vt_renderer *render, i32 ubslot, const sg_range *data);
void vt_set_render_image(vt_renderer *render, i32 slot, sg_image img, sg_sampler smp);

void vt_render_geometry(
	vt_renderer *render, vt_primitive_type primitive, const vt_vertex *vertices, u32 count
);

#endif
