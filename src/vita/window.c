#include "vita/window.h"

#include "log.h"
#include "sokol/sokol_gfx.h"
#include <assert.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

static vt_window *_primary_window = nullptr;

static void _vt_window_close_callback(GLFWwindow *handle) {
	if (!handle) {
		return;
	}

	vt_window *win = glfwGetWindowUserPointer(handle);
	win->should_close = true;
}

static void _vt_window_size_callback(GLFWwindow *handle, i32 w, i32 h) {
	if (!handle) {
		return;
	}

	vt_window *win = glfwGetWindowUserPointer(handle);
	win->size.x = w;
	win->size.y = h;
}

static void _vt_framebuffer_size_callback(GLFWwindow *handle, i32 w, i32 h) {
	if (!handle) {
		return;
	}

	vt_window *win = glfwGetWindowUserPointer(handle);
	win->framesize.x = w;
	win->framesize.y = h;
}

vt_error vt_create_window(vt_window *win, i32 w, i32 h, const char *title) {
	assert(win);

	if (_primary_window) {
		LOG_WARN("[WINDOW] > Multi-window is not supported");
		return VT_ERROR_GENERIC;
	}
	win->size.x = w;
	win->size.y = h;
	win->framesize = win->size;
	win->should_close = false;

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#ifdef VT_TARGET_MACOS
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif

	win->handle = glfwCreateWindow(w, h, title, nullptr, nullptr);
	if (!win->handle) {
		LOG_ERROR("[WINDOW] > Unable to create GLFW window handler");
		vt_destroy_window(win);
		return VT_ERROR_GENERIC;
	}
	glfwMakeContextCurrent(win->handle);

	glfwSetWindowUserPointer(win->handle, win);
	glfwSetWindowCloseCallback(win->handle, _vt_window_close_callback);
	glfwSetWindowSizeCallback(win->handle, _vt_window_size_callback);
	glfwSetFramebufferSizeCallback(win->handle, _vt_framebuffer_size_callback);

	_primary_window = win;
	return VT_ERROR_NONE;
}

void vt_destroy_window(vt_window *win) {
	assert(win);

	if (win->handle) {
		glfwDestroyWindow(win->handle);
		win->handle = nullptr;
	}

	_primary_window = nullptr;
}

void vt_window_update(vt_window *win) {
	assert(win && win->handle);

	sg_commit();
	glfwSwapBuffers(win->handle);
	glfwPollEvents();
}
