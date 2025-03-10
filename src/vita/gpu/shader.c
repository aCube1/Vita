#include "vita/gpu/shader.h"
#include "log.h"

typedef struct _vt_shader_context {
	sg_shader common_shdr;
} _vt_shader_context;

static _vt_shader_context _shdr;

static const char _common_vs_source[] = {
#embed "../assets/shaders/common.vert.glsl"
	, '\0'
};

static const char _common_fs_source[] = {
#embed "../assets/shaders/common.frag.glsl"
	, '\0'
};

vt_error vt_init_shader_resources(void) {
	_shdr.common_shdr = vt_get_common_shader();
	if (_shdr.common_shdr.id == SG_INVALID_ID) {
		LOG_ERROR("[SHADER] > Failed to initialize common shader resource");
		return VT_ERROR_GENERIC;
	}

	return VT_ERROR_NONE;
}

void vt_clean_shader_resources(void) {
	if (sg_query_shader_state(_shdr.common_shdr) != SG_RESOURCESTATE_INVALID) {
		sg_destroy_shader(_shdr.common_shdr);
	}
}

sg_shader vt_get_common_shader(void) {
	if (sg_query_shader_state(_shdr.common_shdr) == SG_RESOURCESTATE_VALID) {
		return _shdr.common_shdr;
	}

	sg_shader_desc shdrdesc = {
		.vertex_func.source = _common_vs_source,
		.vertex_func.entry = "main",
		.fragment_func.source = _common_fs_source,
		.fragment_func.entry = "main",
		.attrs[VT_SHADER_ATTR_POS].glsl_name = "a_pos",
		.attrs[VT_SHADER_ATTR_UV].glsl_name = "a_uv",
		.attrs[VT_SHADER_ATTR_COLOR].glsl_name = "a_color",
		.images[0] = {
			.stage = SG_SHADERSTAGE_FRAGMENT,
			.image_type = SG_IMAGETYPE_2D,
			.sample_type = SG_IMAGESAMPLETYPE_FLOAT,
			.multisampled = false,
		},
		.samplers[0] = {
			.stage = SG_SHADERSTAGE_FRAGMENT,
			.sampler_type = SG_SAMPLERTYPE_FILTERING,
		},
		.image_sampler_pairs[VT_SHADER_UB_TEX0] = {
			.stage = SG_SHADERSTAGE_FRAGMENT,
			.image_slot = 0,
			.sampler_slot = 0,
			.glsl_name = "u_tex0",
		},
		.label = "vt_gpu.common_shdr",
	};

	_shdr.common_shdr = sg_make_shader(&shdrdesc);
	if (sg_query_shader_state(_shdr.common_shdr) != SG_RESOURCESTATE_VALID) {
		sg_destroy_shader(_shdr.common_shdr);
		_shdr.common_shdr.id = SG_INVALID_ID;
	}

	return _shdr.common_shdr;
}

sg_pipeline vt_make_shader_pipeline(sg_shader shdr, vt_primitive_type primitive) {
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
