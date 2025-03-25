#ifndef _VT_VITA_GPU_RENDERER_H
#define _VT_VITA_GPU_RENDERER_H

#include "cglm/types-struct.h"
#include "types.h"
#include "vita/graphics/common.h"

typedef struct vt_uniform_block {
	usize size;
	usize offset;
} vt_uniform_block;

typedef struct vt_render_state {
	sg_pass pass; // Used on sg_render_flush
	ivec2s framesize;
	irect viewport;
	irect scissor;
	mat4s projection;
	mat4s transform;
	u8color color;
	sg_pipeline pipeline;
	vt_texture_uniform textures[VT_MAX_TEXTURE_UNIFORM_SLOTS];
	vt_uniform_block uniforms[VT_MAX_UNIFORMBLOCK_SLOTS];

	u32 _base_vertex;
	u32 _base_command;
	u32 _base_uniform;
} vt_render_state;

typedef struct vt_render_geometry_desc {
	vt_primitive_type primitive;
	vt_vertex *vertices;
	u32 vertex_count;
} vt_render_geometry_desc;

typedef struct vt_renderer vt_renderer;

vt_renderer *vt_create_renderer(void);
void vt_destroy_renderer(vt_renderer *render);

void vt_render_begin(vt_renderer *render, const sg_pass *pass);
void vt_render_end(vt_renderer *render);
void vt_render_flush(vt_renderer *render);

void vt_render_geometry(vt_renderer *render, const vt_render_geometry_desc *desc);

void vt_push_render_transform(vt_renderer *render, mat3s transform);
void vt_pop_render_transform(vt_renderer *render);

void vt_render_projection(vt_renderer *render, mat4s projection);
void vt_render_viewport(vt_renderer *render, irect viewport);
void vt_render_scissor(vt_renderer *render, irect scissor);

void vt_set_render_pipeline(vt_renderer *render, sg_pipeline pipeline);
void vt_set_render_texture(vt_renderer *render, i32 slot, vt_texture_uniform texture);
void vt_set_render_uniform(vt_renderer *render, i32 ubslot, const sg_range *data);
void vt_set_render_color(vt_renderer *render, u8color color);

void vt_query_render_state(vt_renderer *render, vt_render_state *state);

#endif
