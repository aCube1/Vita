#include "window.h"

#include "gpu.h"
#include <assert.h>
#include <stdlib.h>

typedef struct VT_Window {
	SDL_Window *handle;
	VT_GPU *gpu;
	VT_WindowAttribues attrs;
} VT_Window;

static VT_Window *_primary_window = NULL;

VT_Window *vt_create_window(i32 w, i32 h, const char *title) {
	if (_primary_window != NULL) {
		LOG_WARN("[WINDOW] > Multi-window is not supported");
		return _primary_window;
	}

	VT_Window *win = calloc(1, sizeof(VT_Window));
	if (win == NULL) {
		LOG_ERROR("[WINDOW] > Failed to allocate memory");
		return NULL;
	}

#ifdef VT_GFX_GLCORE
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
#endif

	SDL_WindowFlags flags = SDL_WINDOW_OPENGL;
	win->handle = SDL_CreateWindow(title, w, h, flags);
	if (win->handle == NULL) {
		LOG_ERROR("[WINDOW] > Unable to create SDL3 window handler");
		vt_destroy_window(win);
		return NULL;
	}

	win->gpu = vt_gpu_init(win->handle, VT_GPU_NOFLAGS);
	if (win->gpu == NULL) {
		LOG_ERROR("[WINDOW] > Unable to create window gpu handler");
		vt_destroy_window(win);
		return NULL;
	}

	return win;
}

void vt_destroy_window(VT_Window *win) {
	assert(win != NULL);

	if (win->handle != NULL) {
		SDL_DestroyWindow(win->handle);
	}

	if (win->gpu != NULL) {
		vt_gpu_quit(win->gpu);
	}

	free(win);
}

void vt_window_query_attributes(const VT_Window *win, VT_WindowAttribues *attrs) {
	assert(attrs != NULL);
	*attrs = win->attrs;
}
