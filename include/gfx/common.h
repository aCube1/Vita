// clang-format Language: C
#ifndef _VT_GFX_COMMON_H
#define _VT_GFX_COMMON_H

#include "cglm/types-struct.h"
#include "sokol/sokol_gfx.h"
#include "types.h"

enum {
	VT_GFX_MAX_TEXTURE_SLOTS = SG_MAX_IMAGE_SAMPLER_PAIRS,
	VT_GFX_MAX_UNIFORM_SLOTS = SG_MAX_UNIFORMBLOCK_MEMBERS,
	VT_GFX_MAX_UNIFORMBLOCKS_BINDS = SG_MAX_UNIFORMBLOCK_BINDSLOTS,
};

// Shader attribute names and locations
enum {
	VT_SHADER_ATTR_POS = 0,
	VT_SHADER_ATTR_UV,
	VT_SHADER_ATTR_COLOR,
};

typedef struct vt_vertex {
	vec3s position;
	vec2s texcoord;
	u8color color;
} vt_vertex;

sg_shader_desc vt_init_shader_desc(const char *vs_src, const char *fs_src);
sg_pipeline_desc vt_init_pipeline_desc(sg_primitive_type primitive, sg_shader shdr);

sg_shader vt_make_gfx_common_shader(void);
vt_error vt_make_default_texture(sg_image *img, sg_sampler *smp);

#endif
