#include "gpu.h"

#include "common.h"
#include <GLFW/glfw3.h>
#include <string.h>

#define SOKOL_EXTERNAL_GL_LOADER
#define SOKOL_GLCORE
#define SOKOL_IMPL
#include "sokol_gfx.h"

struct VT_GPU {
	bool was_init;

	sg_image white_img;
	sg_sampler nearest_smp;
	sg_shader common_shdr;
};

static struct VT_GPU _gpu = { 0 };

static const char _common_vs_source[] = {
#embed "../assets/shaders/common.vert.glsl"
	, '\0'
};

static const char _common_fs_source[] = {
#embed "../assets/shaders/common.frag.glsl"
	, '\0'
};

bool vt_gpu_setup(void) {
	if (_gpu.was_init) {
		return true;
	}
	memset(&_gpu, 0, sizeof(struct VT_GPU));
	_gpu.was_init = true;

	i32 version = gladLoadGL(glfwGetProcAddress);
	if (version == 0) {
		LOG_ERROR("[GPU] > Failed to create OpenGL context");
		vt_gpu_shutdown();
		return false;
	}
	LOG_INFO(
		"[GPU] > Loaded API: OpenGL %d.%d", GLAD_VERSION_MAJOR(version),
		GLAD_VERSION_MINOR(version)
	);

	sg_desc sgdesc = { 0 };
	sgdesc.logger.func = slog_callback;
	sg_setup(&sgdesc);
	if (!sg_isvalid()) {
		LOG_ERROR("[GPU] > Failed to setup SokolGFX");
		vt_gpu_shutdown();
		return false;
	}

	return true;
}

void vt_gpu_shutdown(void) {
	if (!_gpu.was_init) {
		return;
	}

	if (sg_query_image_state(_gpu.white_img) != SG_RESOURCESTATE_INVALID) {
		sg_destroy_image(_gpu.white_img);
	}
	if (sg_query_sampler_state(_gpu.nearest_smp) != SG_RESOURCESTATE_INVALID) {
		sg_destroy_sampler(_gpu.nearest_smp);
	}
	if (sg_query_shader_state(_gpu.common_shdr) != SG_RESOURCESTATE_INVALID) {
		sg_destroy_shader(_gpu.common_shdr);
	}

	if (sg_isvalid()) {
		sg_shutdown();
	}

	_gpu.was_init = false;
}

sg_image vt_gpu_get_white_image(void) {
	if (sg_query_image_state(_gpu.white_img) == SG_RESOURCESTATE_VALID) {
		return _gpu.white_img;
	}

	u32 pixels[4] = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff };
	sg_image_desc imgdesc = { 0 };
	imgdesc.width = 2;
	imgdesc.height = 2;
	imgdesc.pixel_format = SG_PIXELFORMAT_RGBA8;
	imgdesc.data.subimage[0][0] = SG_RANGE(pixels);
	imgdesc.label = "vt_gpu.white_img";

	_gpu.white_img = sg_make_image(&imgdesc);
	if (sg_query_image_state(_gpu.white_img) != SG_RESOURCESTATE_VALID) {
		LOG_WARN("[GPU] > Failed to make default white image");
		sg_destroy_image(_gpu.white_img);
		_gpu.white_img.id = SG_INVALID_ID;
	}

	return _gpu.white_img;
}

sg_sampler vt_gpu_get_nearest_sampler(void) {
	if (sg_query_sampler_state(_gpu.nearest_smp) == SG_RESOURCESTATE_VALID) {
		return _gpu.nearest_smp;
	}

	sg_sampler_desc smpdesc = { 0 };
	smpdesc.min_filter = SG_FILTER_NEAREST;
	smpdesc.mag_filter = SG_FILTER_NEAREST;
	smpdesc.mipmap_filter = SG_FILTER_NEAREST;
	smpdesc.label = "vt_gpu.nearest_smp";

	_gpu.nearest_smp = sg_make_sampler(&smpdesc);
	if (sg_query_sampler_state(_gpu.nearest_smp) != SG_RESOURCESTATE_VALID) {
		LOG_WARN("[GPU] > Failed to make default nearest sampler");
		sg_destroy_sampler(_gpu.nearest_smp);
		_gpu.nearest_smp.id = SG_INVALID_ID;
	}

	return _gpu.nearest_smp;
}

sg_shader vt_gpu_get_common_shader(void) {
	if (sg_query_shader_state(_gpu.common_shdr) == SG_RESOURCESTATE_VALID) {
		return _gpu.common_shdr;
	}

	sg_shader_desc shdrdesc = {
		.vertex_func.source = _common_vs_source,
		.vertex_func.entry = "main",
		.fragment_func.source = _common_fs_source,
		.fragment_func.entry = "main",
		.attrs[VT_GPU_ATTR_POS].glsl_name = "a_pos",
		.attrs[VT_GPU_ATTR_UV].glsl_name = "a_uv",
		.attrs[VT_GPU_ATTR_COLOR].glsl_name = "a_color",
		.uniform_blocks[VT_GPU_UB_VERTEXPARAMS] = {
			.stage = SG_SHADERSTAGE_VERTEX,
			.layout = SG_UNIFORMLAYOUT_STD140,
			.size = sizeof(VT_VertexParams),
			.glsl_uniforms[0] = {
				.type = SG_UNIFORMTYPE_MAT4,
				.array_count = 1,
				.glsl_name = "VT_VertexParams"
			},
		},
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
		.image_sampler_pairs[VT_GPU_UB_TEX0] = {
			.stage = SG_SHADERSTAGE_FRAGMENT,
			.image_slot = 0,
			.sampler_slot = 0,
			.glsl_name = "u_tex0",
		},
		.label = "vt_gpu.common_shdr",
	};

	_gpu.common_shdr = sg_make_shader(&shdrdesc);
	if (sg_query_shader_state(_gpu.common_shdr) != SG_RESOURCESTATE_VALID) {
		LOG_WARN("[GPU] > Failed to make common shader");
		sg_destroy_shader(_gpu.common_shdr);
		_gpu.common_shdr.id = SG_INVALID_ID;
	}

	return _gpu.common_shdr;
}

sg_pipeline vt_gpu_make_pipeline(sg_shader shdr, VT_PrimitiveType primtype) {
	sg_primitive_type primitive_type;
	switch (primtype) {
	case VT_PRIMITIVETYPE_POINTS: primitive_type = SG_PRIMITIVETYPE_POINTS; break;
	case VT_PRIMITIVETYPE_LINES: primitive_type = SG_PRIMITIVETYPE_LINES; break;
	case VT_PRIMITIVETYPE_TRIANGLES:
	default: primitive_type = SG_PRIMITIVETYPE_TRIANGLES; break;
	}

	sg_pipeline_desc pipdesc = {
		.shader = shdr,
		.layout = {
			.buffers[0].stride = sizeof(VT_Vertex),
			.attrs[VT_GPU_ATTR_POS] = {
				.offset = offsetof(VT_Vertex, position),
				.format = SG_VERTEXFORMAT_FLOAT2,
			},
			.attrs[VT_GPU_ATTR_UV] = {
				.offset = offsetof(VT_Vertex, texcoord),
				.format = SG_VERTEXFORMAT_FLOAT2,
			},
			.attrs[VT_GPU_ATTR_COLOR] = {
				.offset = offsetof(VT_Vertex, color),
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
