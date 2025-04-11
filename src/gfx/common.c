#include "gfx/common.h"

#include <assert.h>

struct _gfx_data {
	sg_shader common_shdr;
	sg_image white_img;
	sg_sampler nearest_smp;
};

static struct _gfx_data _gfx = {};

sg_shader_desc vt_init_shader_desc(const char *vs_src, const char *fs_src) {
	assert(vs_src && fs_src);

	return (sg_shader_desc) {
		.vertex_func.source = vs_src,
		.vertex_func.entry = "main",
		.fragment_func.source = fs_src,
		.fragment_func.entry = "main",
		.attrs[VT_SHADER_ATTR_POS].glsl_name = "a_pos",
		.attrs[VT_SHADER_ATTR_UV].glsl_name = "a_uv",
		.attrs[VT_SHADER_ATTR_COLOR].glsl_name = "a_color",
		.label = "vt_graphics_resources.common_shader",
	};
}

sg_pipeline_desc vt_init_pipeline_desc(sg_primitive_type primitive, sg_shader shdr) {
	return (sg_pipeline_desc) {
		.shader = shdr,
		.layout = {
			.buffers[0].stride = sizeof(vt_vertex),
			.attrs[VT_SHADER_ATTR_POS] = {
				.offset = offsetof(vt_vertex, position),
				.format = SG_VERTEXFORMAT_FLOAT3,
			},
			.attrs[VT_SHADER_ATTR_UV] = {
				.offset = offsetof(vt_vertex, texcoord),
				.format = SG_VERTEXFORMAT_FLOAT2,
			},
			.attrs[VT_SHADER_ATTR_COLOR] =	{
				.offset = offsetof(vt_vertex, color),
				.format = SG_VERTEXFORMAT_UBYTE4N,
			},
		},
		.primitive_type = primitive,
		.label = "vt_graphics_resources.common_pipeline",
	};
}

sg_shader vt_make_gfx_common_shader(void) {
	static const char _common_vs_source[] = {
#embed "../assets/shaders/common.vert.glsl"
		, '\0'
	};

	static const char _common_fs_source[] = {
#embed "../assets/shaders/common.frag.glsl"
		, '\0'
	};

	if (sg_query_shader_state(_gfx.common_shdr) == SG_RESOURCESTATE_VALID) {
		return _gfx.common_shdr;
	}

	sg_shader_desc shdr_desc = vt_init_shader_desc(_common_vs_source, _common_fs_source);
	shdr_desc.images[0] = (sg_shader_image) {
		.stage = SG_SHADERSTAGE_FRAGMENT,
		.image_type = SG_IMAGETYPE_2D,
		.sample_type = SG_IMAGESAMPLETYPE_FLOAT,
	};
	shdr_desc.samplers[0] = (sg_shader_sampler) {
		.stage = SG_SHADERSTAGE_FRAGMENT,
		.sampler_type = SG_SAMPLERTYPE_FILTERING,
	};
	shdr_desc.image_sampler_pairs[0] = (sg_shader_image_sampler_pair) {
		.stage = SG_SHADERSTAGE_FRAGMENT,
		.image_slot = 0,
		.sampler_slot = 0,
		.glsl_name = "u_tex0",
	};

	sg_shader shdr = sg_make_shader(&shdr_desc);
	if (sg_query_shader_state(shdr) != SG_RESOURCESTATE_VALID) {
		sg_destroy_shader(shdr);
		shdr.id = SG_INVALID_ID;
	}

	_gfx.common_shdr = shdr;
	return _gfx.common_shdr;
}

vt_error vt_make_default_texture(sg_image *img, sg_sampler *smp) {
	if (sg_query_image_state(_gfx.white_img) != SG_RESOURCESTATE_VALID) {
		u32 pixels[4] = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff };
		sg_image_desc img_desc = {
			.width = 2,
			.height = 2,
			.pixel_format = SG_PIXELFORMAT_RGBA8,
			.data.subimage[0][0] = SG_RANGE(pixels),
			.label = "vt_graphics_resources.white_image",
		};

		_gfx.white_img = sg_make_image(&img_desc);
		if (sg_query_image_state(_gfx.white_img) != SG_RESOURCESTATE_VALID) {
			sg_destroy_image(_gfx.white_img);
			return VT_ERROR_GENERIC;
		}
	}

	if (sg_query_sampler_state(_gfx.nearest_smp) != SG_RESOURCESTATE_VALID) {
		sg_sampler_desc smp_desc = {
			.min_filter = SG_FILTER_NEAREST,
			.mag_filter = SG_FILTER_NEAREST,
			.mipmap_filter = SG_FILTER_NEAREST,
			.label = "vt_graphics_resources.nearest_smp",

		};

		_gfx.nearest_smp = sg_make_sampler(&smp_desc);
		if (sg_query_sampler_state(_gfx.nearest_smp) != SG_RESOURCESTATE_VALID) {
			sg_destroy_sampler(_gfx.nearest_smp);
			return VT_ERROR_GENERIC;
		}
	}

	if (img) {
		*img = _gfx.white_img;
	}
	if (smp) {
		*smp = _gfx.nearest_smp;
	}

	return VT_ERROR_NONE;
}
