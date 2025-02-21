#include "window.h"

#include "cglm/types-struct.h"
#include "sokol_gfx.h"
#include <GLFW/glfw3.h>
#include <assert.h>
#include <stdlib.h>

typedef struct VT_Window {
	GLFWwindow *handle;

	ivec2s size;
	ivec2s framesize;
	bool should_close;
} VT_Window;

static VT_Window *_primary_window = nullptr;

static void _vt_window_close_callback(GLFWwindow *handle) {
	VT_Window *win = glfwGetWindowUserPointer(handle);

	win->should_close = true;
}

static void _vt_window_size_callback(GLFWwindow *handle, i32 w, i32 h) {
	VT_Window *win = glfwGetWindowUserPointer(handle);
	win->size.x = w;
	win->size.y = h;
}

static void _vt_framebuffer_size_callback(GLFWwindow *handle, i32 w, i32 h) {
	VT_Window *win = glfwGetWindowUserPointer(handle);
	win->framesize.x = w;
	win->framesize.y = h;
}

VT_Window *vt_create_window(i32 w, i32 h, const char *title) {
	if (_primary_window) {
		LOG_WARN("[WINDOW] > Multi-window is not supported");
		return _primary_window;
	}

	VT_Window *win = calloc(1, sizeof(VT_Window));
	if (!win) {
		LOG_ERROR("[WINDOW] > Failed to alloc memory");
		return nullptr;
	}
	win->size.x = w;
	win->size.y = h;
	win->framesize = win->size;
	win->should_close = false;

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	win->handle = glfwCreateWindow(w, h, title, nullptr, nullptr);
	if (!win->handle) {
		LOG_ERROR("[WINDOW] > Unable to create GLFW window handler");
		vt_destroy_window(win);
		return nullptr;
	}
	glfwMakeContextCurrent(win->handle);

	glfwSetWindowUserPointer(win->handle, win);
	glfwSetWindowCloseCallback(win->handle, _vt_window_close_callback);
	glfwSetWindowSizeCallback(win->handle, _vt_window_size_callback);
	glfwSetFramebufferSizeCallback(win->handle, _vt_framebuffer_size_callback);

	return win;
}

void vt_destroy_window(VT_Window *win) {
	assert(win);

	if (win->handle) {
		glfwDestroyWindow(win->handle);
	}

	free(win);
	_primary_window = nullptr;
}

void vt_window_update(VT_Window *win) {
	assert(win);
	sg_commit();
	glfwSwapBuffers(win->handle);
	glfwPollEvents();
}

bool vt_window_should_close(const VT_Window *win) {
	assert(win);
	return win->should_close;
}

ivec2s vt_get_window_size(const VT_Window *win) {
	assert(win);
	return win->size;
}

ivec2s vt_get_window_framesize(const VT_Window *win) {
	assert(win);
	return win->framesize;
}
