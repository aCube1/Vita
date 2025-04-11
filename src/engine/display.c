#include "engine/display.h"

#include "gfx/renderer.h"
#include "log.h"
#include "utils.h"

#include <assert.h>

#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"

#include <GLFW/glfw3.h>

#define SOKOL_GLCORE
#define SOKOL_EXTERNAL_GL_LOADER
#define SOKOL_IMPL
#include "sokol/sokol_gfx.h"

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

vt_error vt_init_display(vt_display *display, const vt_display_desc *desc) {
	assert(display && desc);
	assert(desc->window.size.x > 0 && desc->window.size.y > 0);

	vt_clear_mem(display, sizeof(vt_display));
	display->desc = *desc;

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#ifdef VT_TARGET_MACOS
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif

	display->window = glfwCreateWindow(
		desc->window.size.x, desc->window.size.y, desc->window.title, nullptr, nullptr
	);
	if (!display->window) {
		LOG_ERROR("[DISPLAY] | Window > Failed to create Window");
		vt_deinit_display(display);
		return VT_ERROR_GENERIC;
	}

	glfwSetWindowUserPointer(display->window, display);
	glfwMakeContextCurrent(display->window);

	if (_vt_load_gl() != VT_ERROR_NONE) {
		LOG_ERROR("[DISPLAY] | GL > Failed to initialize OpenGL context");
		vt_deinit_display(display);
		return VT_ERROR_GENERIC;
	}

	if (!sg_isvalid()) {
		sg_desc sgdesc = {};
#ifndef NDEBUG
		sgdesc.logger.func = vt_slog_callback;
#endif

		sg_setup(&sgdesc);
		if (!sg_isvalid()) {
			LOG_ERROR("[DISPLAY] | Renderer > Failed to setup SokolGFX");
			vt_deinit_display(display);
			return false;
		}
	}

	display->renderer = vt_create_renderer();
	if (!display->renderer) {
		LOG_ERROR("[DISPLAY] | Renderer > Failed to create Renderer");
		vt_deinit_display(display);
		return VT_ERROR_GENERIC;
	}

	return VT_ERROR_NONE;
}

void vt_deinit_display(vt_display *display) {
	if (!display) {
		return;
	}

	if (display->renderer) {
		vt_destroy_renderer(display->renderer);
	}

	if (sg_isvalid()) {
		sg_shutdown();
	}

	if (display->window) {
		glfwDestroyWindow(display->window);
	}
}

void vt_update_display(const vt_display *display) {
	assert(display);

	glfwSwapBuffers(display->window);
	glfwPollEvents();
}
