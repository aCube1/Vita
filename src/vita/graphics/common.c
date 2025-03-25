#include "vita/graphics/common.h"

#include "log.h"
#include <assert.h>
#include <string.h>

struct _vt_gpu_resources {
	sg_shader common_shdr;
	sg_image white_img;
	sg_sampler nearest_smp;
};

static struct _vt_gpu_resources _gpu = {};

sg_shader vt_make_gpu_shader(const vt_shader_desc *desc) {
	assert(desc);

	sg_shader_desc shdrdesc = {
		.vertex_func.source = desc->vs_src,
		.vertex_func.entry = "main",
		.fragment_func.source = desc->fs_src,
		.fragment_func.entry = "main",
		.attrs[VT_SHADER_ATTR_POS].glsl_name = "a_pos",
		.attrs[VT_SHADER_ATTR_UV].glsl_name = "a_uv",
		.attrs[VT_SHADER_ATTR_COLOR].glsl_name = "a_color",
		.label = "vt_gpu_resources.common_shdr",
	};

	memcpy(
		shdrdesc.uniform_blocks, desc->uniform_blocks,
		sizeof(sg_shader_uniform_block) * VT_MAX_UNIFORMBLOCK_SLOTS
	);

	for (i32 i = 0; i < VT_MAX_TEXTURE_UNIFORM_SLOTS; i += 1) {
		vt_shader_texture_desc texdesc = desc->textures[i];

		shdrdesc.images[i] = (sg_shader_image) {
			.stage = texdesc.stage,
			.image_type = texdesc.image_type,
			.sample_type = texdesc.image_sample,
			.multisampled = texdesc.multisampled,
		};
		shdrdesc.samplers[i] = (sg_shader_sampler) {
			.stage = texdesc.stage,
			.sampler_type = texdesc.sampler_type,
		};
		shdrdesc.image_sampler_pairs[i] = (sg_shader_image_sampler_pair) {
			.stage = texdesc.stage,
			.image_slot = i,
			.sampler_slot = i,
			.glsl_name = texdesc.glsl_name,
		};
	}

	sg_shader shdr = sg_make_shader(&shdrdesc);
	if (sg_query_shader_state(shdr) != SG_RESOURCESTATE_VALID) {
		sg_destroy_shader(shdr);
		shdr.id = SG_INVALID_ID;
	}

	return shdr;
}

sg_pipeline vt_make_gpu_pipeline(vt_primitive_type primitive, sg_shader shdr) {
	sg_primitive_type primitive_type = primitive + 1;

	sg_pipeline_desc pipdesc = {
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
			.attrs[VT_SHADER_ATTR_COLOR] = {
				.offset = offsetof(vt_vertex, color),
				.format = SG_VERTEXFORMAT_UBYTE4N,
			},
		},
		.primitive_type = primitive_type,
		.label = "vt_gpu.pipeline",
	};

	sg_pipeline pip = sg_make_pipeline(&pipdesc);
	if (sg_query_pipeline_state(pip) != SG_RESOURCESTATE_VALID) {
		sg_destroy_pipeline(pip);
		pip.id = SG_INVALID_ID;
	}

	return pip;
}

sg_shader vt_get_gpu_common_shader(void) {
	static const char _common_vs_source[] = {
#embed "../assets/shaders/common.vert.glsl"
		, '\0'
	};

	static const char _common_fs_source[] = {
#embed "../assets/shaders/common.frag.glsl"
		, '\0'
	};

	if (sg_query_shader_state(_gpu.common_shdr) == SG_RESOURCESTATE_VALID) {
		return _gpu.common_shdr;
	}

	_gpu.common_shdr = vt_make_gpu_shader(&(vt_shader_desc) {
		.vs_src = _common_vs_source,
		.fs_src = _common_fs_source,
		.textures[0] = {
			.stage = SG_SHADERSTAGE_FRAGMENT,
			.image_type = SG_IMAGETYPE_2D,
			.sampler_type = SG_SAMPLERTYPE_FILTERING,
			.image_sample = SG_IMAGESAMPLETYPE_FLOAT,
			.multisampled = false,
			.glsl_name = "u_tex0",
		},
	});
	return _gpu.common_shdr;
}

sg_image vt_get_gpu_white_image(void) {
	if (sg_query_image_state(_gpu.white_img) == SG_RESOURCESTATE_VALID) {
		return _gpu.white_img;
	}

	u32 pixels[4] = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff };
	sg_image_desc imgdesc = { 0 };
	imgdesc.width = 2;
	imgdesc.height = 2;
	imgdesc.pixel_format = SG_PIXELFORMAT_RGBA8;
	imgdesc.data.subimage[0][0] = SG_RANGE(pixels);
	imgdesc.label = "vt_gpu_resources.white_img";

	_gpu.white_img = sg_make_image(&imgdesc);
	if (sg_query_image_state(_gpu.white_img) != SG_RESOURCESTATE_VALID) {
		LOG_WARN("[GPU] > Failed to make default white image");
		sg_destroy_image(_gpu.white_img);
		_gpu.white_img.id = SG_INVALID_ID;
	}

	return _gpu.white_img;
}

sg_sampler vt_get_gpu_nearest_sampler(void) {
	if (sg_query_sampler_state(_gpu.nearest_smp) == SG_RESOURCESTATE_VALID) {
		return _gpu.nearest_smp;
	}

	sg_sampler_desc smpdesc = { 0 };
	smpdesc.min_filter = SG_FILTER_NEAREST;
	smpdesc.mag_filter = SG_FILTER_NEAREST;
	smpdesc.mipmap_filter = SG_FILTER_NEAREST;
	smpdesc.label = "vt_gpu_resources.nearest_smp";

	_gpu.nearest_smp = sg_make_sampler(&smpdesc);
	if (sg_query_sampler_state(_gpu.nearest_smp) != SG_RESOURCESTATE_VALID) {
		LOG_WARN("[GPU] > Failed to make default nearest sampler");
		sg_destroy_sampler(_gpu.nearest_smp);
		_gpu.nearest_smp.id = SG_INVALID_ID;
	}

	return _gpu.nearest_smp;
}
