#include "gpu.h"

#include <SDL3/SDL_video.h>
#include <string.h>

#define SOKOL_GLCORE
#define SOKOL_EXTERNAL_GL_LOADER
#define SOKOL_IMPL
#include "sokol_gfx.h"

struct VT_GPU {
	bool was_init;
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
