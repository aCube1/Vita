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

static VT_Window *_primary_window = NULL;

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
	if (_primary_window != NULL) {
		LOG_WARN("[WINDOW] > Multi-window is not supported");
		return _primary_window;
	}

	VT_Window *win = calloc(1, sizeof(VT_Window));
	if (win == NULL) {
		LOG_ERROR("[WINDOW] > Failed to allocate memory");
		return NULL;
	}
	win->size.x = w;
	win->size.y = h;
	win->framesize = win->size;
	win->should_close = false;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_WindowFlags winflags = SDL_WINDOW_OPENGL;
	win->handle = SDL_CreateWindow(title, w, h, winflags);
	if (win->handle == NULL) {
		LOG_ERROR("[WINDOW] > Unable to create SDL3 window handler: %s", SDL_GetError());
		vt_destroy_window(win);
		return NULL;
	}

	win->glctx = SDL_GL_CreateContext(win->handle);
	if (win->glctx == NULL) {
		LOG_ERROR("[GPU] > Failed to create OpenGL context: %s", SDL_GetError());
		vt_destroy_window(win);
		return NULL;
	}
	SDL_GL_MakeCurrent(win->handle, win->glctx);

	SDL_AddEventWatch(_vt_window_event, win);
	return win;
}

void vt_destroy_window(VT_Window *win) {
	assert(win != NULL);

	if (win->handle != NULL) {
		SDL_DestroyWindow(win->handle);
	}

	if (win->glctx != NULL) {
		SDL_GL_DestroyContext(win->glctx);
	}

	free(win);
	_primary_window = NULL;
}

bool vt_window_should_close(const VT_Window *win) {
	assert(win != NULL);
	return win->should_close;
}
