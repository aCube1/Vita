#include "gpu.h"

#include <SDL3/SDL_video.h>
#include <string.h>

#define SOKOL_GLCORE
#define SOKOL_EXTERNAL_GL_LOADER
#define SOKOL_IMPL
#include "sokol_gfx.h"

struct VT_GPU {
	bool was_init;

	sg_image white_img;
	sg_sampler nearest_smp;
	sg_shader common_shdr;
};

static struct VT_GPU _gpu = {0};

bool vt_gpu_setup(void) {
	if (_gpu.was_init) {
		return true;
	}
	memset(&_gpu, 0, sizeof(struct VT_GPU));
	_gpu.was_init = true;

	i32 version = gladLoadGL(SDL_GL_GetProcAddress);
	if (version == 0) {
		LOG_ERROR("[GPU] > Failed to create OpenGL context");
		vt_gpu_shutdown();
		return false;
	}
	LOG_INFO(
		"[GPU] > Loaded API: OpenGL %d.%d", GLAD_VERSION_MAJOR(version),
		GLAD_VERSION_MINOR(version)
	);

	sg_desc sgdesc = {0};
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

	if (sg_isvalid()) {
		sg_shutdown();
	}

	_gpu.was_init = false;
}

sg_image vt_gpu_get_white_image(void) {
	if (sg_query_image_state(_gpu.white_img) == SG_RESOURCESTATE_VALID) {
		return _gpu.white_img;
	}

	u32 pixels[4] = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};

	sg_image_desc imgdesc = {0};
	imgdesc.width = 2;
	imgdesc.height = 2;
	imgdesc.pixel_format = SG_PIXELFORMAT_RGBA8;
	imgdesc.data.subimage[0][0] = SG_RANGE(pixels);
	imgdesc.label = "vt_gpu.white_img";

	_gpu.white_img = sg_make_image(&imgdesc);
	if (sg_query_image_state(_gpu.white_img) != SG_RESOURCESTATE_VALID) {
		_gpu.white_img.id = 0;
		LOG_WARN("[GPU] > Failed to make default white image");
	}

	return _gpu.white_img;
}

sg_sampler vt_gpu_get_nearest_sampler(void) {
	if (sg_query_sampler_state(_gpu.nearest_smp) == SG_RESOURCESTATE_VALID) {
		return _gpu.nearest_smp;
	}

	sg_sampler_desc smpdesc = {0};
	smpdesc.min_filter = SG_FILTER_NEAREST;
	smpdesc.mag_filter = SG_FILTER_NEAREST;
	smpdesc.mipmap_filter = SG_FILTER_NEAREST;
	smpdesc.label = "vt_gpu.nearest_smp";

	_gpu.nearest_smp = sg_make_sampler(&smpdesc);
	if (sg_query_sampler_state(_gpu.nearest_smp) != SG_RESOURCESTATE_VALID) {
		_gpu.nearest_smp.id = 0;
		LOG_WARN("[GPU] > Failed to make default nearest sampler");
	}

	return _gpu.nearest_smp;
}
