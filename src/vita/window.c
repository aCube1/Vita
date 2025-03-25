#include "vita/window.h"

#include "log.h"
#include <assert.h>

#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"
#include <GLFW/glfw3.h>

#define SOKOL_GLCORE
#define SOKOL_EXTERNAL_GL_LOADER
#define SOKOL_IMPL
#include "sokol/sokol_gfx.h"

static vt_window *_primary_window = nullptr;

static void _vt_window_close_callback(GLFWwindow *handle) {
	vt_window *win = glfwGetWindowUserPointer(handle);
	win->should_close = true;
}

static void _vt_window_size_callback(GLFWwindow *handle, i32 w, i32 h) {
	vt_window *win = glfwGetWindowUserPointer(handle);
	win->size.x = w;
	win->size.y = h;
}

static void _vt_framebuffer_size_callback(GLFWwindow *handle, i32 w, i32 h) {
	vt_window *win = glfwGetWindowUserPointer(handle);
	win->framesize.x = w;
	win->framesize.y = h;
}

static vt_error _vt_load_gl(void) {
	static i32 version = 0;
	if (version != 0) {
		return VT_ERROR_NONE;
	}

	version = gladLoadGL(glfwGetProcAddress);
	if (version == 0) {
		LOG_ERROR("[GPU] > Failed to create OpenGL context");
		return VT_ERROR_GENERIC;
	}
	LOG_INFO(
		"[GPU] > Loaded API: OpenGL %d.%d", GLAD_VERSION_MAJOR(version),
		GLAD_VERSION_MINOR(version)
	);

	return VT_ERROR_NONE;
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

	if (_vt_load_gl() != VT_ERROR_NONE) {
		LOG_ERROR("[WINDOW] > Failed to initialize OpenGL context");
		vt_destroy_window(win);
		return VT_ERROR_GENERIC;
	}

	if (!sg_isvalid()) {
		sg_desc sgdesc = {};
#ifndef NDEBUG
		sgdesc.logger.func = vt_slog_callback;
#endif

		sg_setup(&sgdesc);
		if (!sg_isvalid()) {
			LOG_ERROR("[WINDOW] > Failed to setup SokolGFX");
			vt_destroy_window(win);
			return false;
		}
	}

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
	if (!_primary_window && sg_isvalid()) {
		sg_shutdown();
	}
}

void vt_window_update(vt_window *win) {
	assert(win && win->handle);

	glfwSwapBuffers(win->handle);
	glfwPollEvents();
}
