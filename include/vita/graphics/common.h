#ifndef _VT_VITA_GRAPHICS_COMMON_H
#define _VT_VITA_GRAPHICS_COMMON_H

#include "cglm/types-struct.h"
#include "sokol/sokol_gfx.h"
#include "types.h"

enum {
	VT_MAX_TEXTURE_UNIFORM_SLOTS = SG_MAX_IMAGE_SAMPLER_PAIRS,
	VT_MAX_UNIFORMBLOCK_SLOTS = SG_MAX_UNIFORMBLOCK_BINDSLOTS,
};

// Shader attribute names and locations
enum {
	VT_SHADER_ATTR_POS = 0,
	VT_SHADER_ATTR_UV,
	VT_SHADER_ATTR_COLOR,
};

// Shader uniform block names and bindings
enum {
	VT_SHADER_UB_TEX0 = 0,
};

typedef enum vt_primitive_type {
	VT_PRIMITIVETYPE_POINTS = 0,
	VT_PRIMITIVETYPE_LINES,
	VT_PRIMITIVETYPE_LINE_STRIP,
	VT_PRIMITIVETYPE_TRIANGLES,
	VT_PRIMITIVETYPE_TRIANGLE_STRIP,
	_VT_PRIMITIVETYPE_COUNT,
} vt_primitive_type;

typedef struct vt_shader_texture_desc {
	sg_shader_stage stage;
	sg_image_type image_type;
	sg_sampler_type sampler_type;
	sg_image_sample_type image_sample;
	bool multisampled;
	const char *glsl_name;
} vt_shader_texture_desc;

typedef struct vt_shader_desc {
	const char *vs_src;
	const char *fs_src;
	vt_shader_texture_desc textures[VT_MAX_TEXTURE_UNIFORM_SLOTS];
	sg_shader_uniform_block uniform_blocks[VT_MAX_UNIFORMBLOCK_SLOTS];
} vt_shader_desc;

typedef struct vt_vertex {
	vec3s position;
	vec2s texcoord;
	u8color color;
} vt_vertex;

typedef struct vt_texture_uniform {
	sg_image img;
	sg_sampler smp;
} vt_texture_uniform;

sg_shader vt_make_gpu_shader(const vt_shader_desc *desc);
sg_pipeline vt_make_gpu_pipeline(vt_primitive_type primitive, sg_shader shdr);

sg_shader vt_get_gpu_common_shader(void);
sg_image vt_get_gpu_white_image(void);
sg_sampler vt_get_gpu_nearest_sampler(void);

#endif
