#include "gpu.h"

#include <stdlib.h>
#include <string.h>

#ifdef VT_GFX_GLCORE
#	include "gpu/gl.h"
#endif

struct VT_GPU {
	u32 flags;
	SDL_Window *win;

#ifdef VT_GFX_GLCORE
	struct {
		SDL_GLContext ctx;
	} gl;
#endif
};

VT_GPU *vt_gpu_init(SDL_Window *win, u32 flags) {
	assert(win != NULL);

	VT_GPU *gpu = calloc(1, sizeof(VT_GPU));
	if (gpu == NULL) {
		LOG_ERROR("[GPU] > Failed to allocate GPU context handler");
		return NULL;
	}

	gpu->flags = flags;
	gpu->win = win;

#ifdef VT_GFX_GLCORE
	gpu->gl.ctx = SDL_GL_CreateContext(win);
	i32 version = gladLoadGL(SDL_GL_GetProcAddress);
	if (gpu->gl.ctx == NULL || version == 0) {
		LOG_ERROR("[GPU] > Failed to create OpenGL context");
		vt_gpu_quit(gpu);
		return NULL;
	}
	LOG_INFO(
		"[GPU] > Loaded API: OpenGL %d.%d", GLAD_VERSION_MAJOR(version),
		GLAD_VERSION_MINOR(version)
	);

	SDL_GL_MakeCurrent(gpu->win, gpu->gl.ctx);

	if ((flags & VT_GPU_VSYNC) != 0) {
		SDL_GL_SetSwapInterval(1);
	}
#endif

	return gpu;
}

void vt_gpu_quit(VT_GPU *gpu) {
	if (gpu == NULL) {
		return;
	}

#ifdef VT_GFX_GLCORE
	if (gpu->gl.ctx != NULL) {
		SDL_GL_DestroyContext(gpu->gl.ctx);
	}
#endif

	free(gpu);
}

void vt_gpu_set_vsync(i32 vsync) {
#ifdef VT_GFX_GLCORE
	if (!SDL_GL_SetSwapInterval(vsync)) {
		SDL_GL_SetSwapInterval(0);
		LOG_WARN("[GPU] > VSync option not supported, disabling it...");
	}
#endif
}
