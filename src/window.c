#include "window.h"

#include "cglm/types-struct.h"
#include <SDL3/SDL_events.h>
#include <assert.h>
#include <stdlib.h>

typedef struct VT_Window {
	SDL_Window *handle;
	SDL_GLContext glctx;

	ivec2s size;
	ivec2s framesize;
	bool should_close;
} VT_Window;

static VT_Window *_primary_window = nullptr;

bool _vt_window_event(void *usrdata, SDL_Event *event) {
	VT_Window *win = usrdata;
	switch (event->type) {
	case SDL_EVENT_WINDOW_RESIZED:
		win->size.x = event->window.data1;
		win->size.y = event->window.data2;
		break;
	case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
		win->framesize.x = event->window.data1;
		win->framesize.y = event->window.data2;
		break;
	}

	if (event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
		win->should_close = true;
	}

	return true;
}

VT_Window *vt_create_window(i32 w, i32 h, const char *title) {
	if (_primary_window != nullptr) {
		LOG_WARN("[WINDOW] > Multi-window is not supported");
		return _primary_window;
	}

	VT_Window *win = calloc(1, sizeof(VT_Window));
	if (win == nullptr) {
		LOG_ERROR("[WINDOW] > Failed to allocate memory");
		return nullptr;
	}
	win->size.x = w;
	win->size.y = h;
	win->framesize = win->size;
	win->should_close = false;

	SDL_WindowFlags winflags = SDL_WINDOW_OPENGL;
	win->handle = SDL_CreateWindow(title, w, h, winflags);
	if (win->handle == nullptr) {
		LOG_ERROR("[WINDOW] > Unable to create SDL3 window handler: %s", SDL_GetError());
		vt_destroy_window(win);
		return nullptr;
	}

	win->glctx = SDL_GL_CreateContext(win->handle);
	if (win->glctx == nullptr) {
		LOG_ERROR("[GPU] > Failed to create OpenGL context: %s", SDL_GetError());
		vt_destroy_window(win);
		return nullptr;
	}
	SDL_GL_MakeCurrent(win->handle, win->glctx);

	SDL_AddEventWatch(_vt_window_event, win);
	return win;
}

void vt_destroy_window(VT_Window *win) {
	assert(win != nullptr);

	if (win->handle) {
		SDL_DestroyWindow(win->handle);
	}

	if (win->glctx) {
		SDL_GL_DestroyContext(win->glctx);
	}

	free(win);
	_primary_window = nullptr;
}

bool vt_window_should_close(const VT_Window *win) {
	assert(win != nullptr);
	return win->should_close;
}
