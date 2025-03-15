#ifndef _VT_VITA_GPU_SHADER_H
#define _VT_VITA_GPU_SHADER_H

#include "cglm/types-struct.h"
#include "common.h"
#include "sokol/sokol_gfx.h"
#include "vita/gpu/color.h"

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

typedef struct vt_vertex {
	vec3s position;
	vec2s texcoord;
	u8color color;
} vt_vertex;

vt_error vt_init_shader_resources(void);
void vt_clean_shader_resources(void);

sg_shader vt_get_common_shader(void);

sg_pipeline vt_make_shader_pipeline(sg_shader shdr, vt_primitive_type primitive);

#endif
