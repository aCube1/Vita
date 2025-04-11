// clang-format Language: C
#ifndef _VT_GRAPHICS_RENDERER_H
#define _VT_GRAPHICS_RENDERER_H

#include "cglm/types-struct.h"
#include "gfx/common.h"
#include "sokol/sokol_gfx.h"
#include "types.h"

typedef struct vt_uniform_block {
	usize size;
	usize offset;
} vt_uniform_block;

typedef struct vt_texture {
	sg_image img;
	sg_sampler smp;
} vt_texture;

typedef struct vt_render_state {
	sg_pass pass; // Used on sg_render_flush
	ivec2s framesize;
	irect viewport;
	irect scissor;
	mat4s projection;
	mat4s transform;
	u8color color;
	sg_pipeline pipeline;
	vt_texture textures[VT_GFX_MAX_TEXTURE_SLOTS];
	vt_uniform_block uniforms[VT_GFX_MAX_UNIFORM_SLOTS];

	u32 _base_vertex;
	u32 _base_command;
	u32 _base_uniform;
} vt_render_state;

typedef struct vt_render_geometry_desc {
	sg_primitive_type primitive;
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
void vt_set_render_image(vt_renderer *render, i32 slot, sg_image img);
void vt_set_render_sample(vt_renderer *render, i32 slot, sg_sampler smp);
void vt_set_render_uniform(vt_renderer *render, i32 ubslot, const sg_range *data);
void vt_set_render_color(vt_renderer *render, u8color color);

void vt_query_render_state(vt_renderer *render, vt_render_state *state);

#endif
