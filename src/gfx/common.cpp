#include "gfx/common.hpp"

#include <glad/gl.h>

#define SOKOL_IMPL
#define SOKOL_GLCORE
#define SOKOL_EXTERNAL_GL_LOADER
#include <sokol/sokol_gfx.h>

struct GfxResources {
	sg_pipeline primitive_pips[_SG_PRIMITIVETYPE_NUM];
	sg_shader common_shdr;
	sg_image white_img;
	sg_sampler nearest_smp;
};

static GfxResources _gfx = {};

static const char _common_vs_source[] =
	"#version 410 core \n"
	"in vec3 a_pos;    \n"
	"in vec2 a_uv;     \n"
	"in vec4 a_color;  \n"
	"out vec4 f_color; \n"
	"out vec2 f_uv;    \n"
	"void main() {     \n"
	"    f_color = a_color; \n"
	"    f_uv = a_uv;       \n"
	"    gl_Position = vec4(a_pos, 1.0); \n"
	"}";

static const char _common_fs_source[] =
	"#version 410 core         \n"
	"uniform sampler2D u_tex0; \n"
	"in vec4 f_color;          \n"
	"in vec2 f_uv;             \n"
	"out vec4 out_color;       \n"
	"void main() {             \n"
	"    out_color = texture(u_tex0, f_uv) * f_color; \n"
	"}";

sg_pipeline_desc vt::init_pipeline_desc(sg_primitive_type primitive, sg_shader shdr) {
	sg_pipeline_desc desc {};
	desc.shader = shdr;
	desc.layout.buffers[0].stride = sizeof(vt::Vertex);
	desc.layout.attrs[(i32)vt::VertexAttr::Pos] = {
		.buffer_index = 0,
		.offset = offsetof(vt::Vertex, position),
		.format = SG_VERTEXFORMAT_FLOAT3,
	};
	desc.layout.attrs[(i32)vt::VertexAttr::UV] = {
		.buffer_index = 0,
		.offset = offsetof(vt::Vertex, texcoord),
		.format = SG_VERTEXFORMAT_FLOAT2,
	};
	desc.layout.attrs[(i32)vt::VertexAttr::Color] = {
		.buffer_index = 0,
		.offset = offsetof(vt::Vertex, color),
		.format = SG_VERTEXFORMAT_UBYTE4N,
	};
	desc.primitive_type = primitive;
	desc.label = "vt_gfx_resources.pipeline";

	return desc;
}

sg_pipeline vt::make_pipeline(sg_primitive_type primitive) {
	u32 pip_idx = primitive;
	if (_gfx.primitive_pips[pip_idx].id != SG_INVALID_ID) {
		return _gfx.primitive_pips[pip_idx];
	}

	sg_shader shdr = vt::make_common_shader();

	sg_pipeline_desc pipdesc = init_pipeline_desc(primitive, shdr);
	sg_pipeline pip = sg_make_pipeline(&pipdesc);
	if (sg_query_pipeline_state(pip) != SG_RESOURCESTATE_VALID) {
		sg_destroy_pipeline(pip);
		return sg_pipeline { SG_INVALID_ID };
	}

	_gfx.primitive_pips[pip_idx] = pip;
	return pip;
}

sg_shader vt::make_common_shader() {
	if (sg_query_shader_state(_gfx.common_shdr) == SG_RESOURCESTATE_VALID) {
		return _gfx.common_shdr;
	}

	sg_shader_desc desc {};
	desc.vertex_func.source = _common_vs_source;
	desc.vertex_func.entry = "main";
	desc.fragment_func.source = _common_fs_source;
	desc.fragment_func.entry = "main";
	desc.attrs[(i32)vt::VertexAttr::Pos].glsl_name = "a_pos";
	desc.attrs[(i32)vt::VertexAttr::UV].glsl_name = "a_uv";
	desc.attrs[(i32)vt::VertexAttr::Color].glsl_name = "a_color";
	desc.images[0].stage = SG_SHADERSTAGE_FRAGMENT;
	desc.images[0].image_type = SG_IMAGETYPE_2D;
	desc.images[0].sample_type = SG_IMAGESAMPLETYPE_FLOAT;
	desc.samplers[0].stage = SG_SHADERSTAGE_FRAGMENT;
	desc.samplers[0].sampler_type = SG_SAMPLERTYPE_FILTERING;
	desc.image_sampler_pairs[0].stage = SG_SHADERSTAGE_FRAGMENT;
	desc.image_sampler_pairs[0].image_slot = 0;
	desc.image_sampler_pairs[0].sampler_slot = 0;
	desc.image_sampler_pairs[0].glsl_name = "u_tex0";
	desc.label = "vt_gfx_resources.shader";

	sg_shader shdr = sg_make_shader(&desc);
	if (sg_query_shader_state(shdr) != SG_RESOURCESTATE_VALID) {
		sg_destroy_shader(shdr);
		shdr.id = SG_INVALID_ID;
	}

	_gfx.common_shdr = shdr;
	return _gfx.common_shdr;
}

vt::Texture vt::make_common_texture() {
	if (sg_query_image_state(_gfx.white_img) != SG_RESOURCESTATE_VALID) {
		u32 pixels[4] = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff };

		sg_image_desc desc {};
		desc.width = 2;
		desc.height = 2;
		desc.usage.immutable = true;
		desc.pixel_format = SG_PIXELFORMAT_RGBA8;
		desc.data.subimage[0][0] = SG_RANGE(pixels);
		desc.label = "vt_gfx_resources.white_image";

		_gfx.white_img = sg_make_image(&desc);
		if (sg_query_image_state(_gfx.white_img) != SG_RESOURCESTATE_VALID) {
			sg_destroy_image(_gfx.white_img);
			_gfx.white_img.id = SG_INVALID_ID;
		}
	}

	if (sg_query_sampler_state(_gfx.nearest_smp) != SG_RESOURCESTATE_VALID) {
		sg_sampler_desc desc {};
		desc.min_filter = SG_FILTER_NEAREST;
		desc.mag_filter = SG_FILTER_NEAREST;
		desc.mipmap_filter = SG_FILTER_NEAREST;
		desc.label = "vt_gfx_resources.nearest_smp";

		_gfx.nearest_smp = sg_make_sampler(&desc);
		if (sg_query_sampler_state(_gfx.nearest_smp) != SG_RESOURCESTATE_VALID) {
			sg_destroy_sampler(_gfx.nearest_smp);
			_gfx.nearest_smp.id = SG_INVALID_ID;
		}
	}

	return vt::Texture {
		.img = _gfx.white_img,
		.smp = _gfx.nearest_smp,
	};
}
