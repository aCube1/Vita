#include "core/Display.hpp"

#define GLFW_INCLUDE_NONE
#define GLAD_GL_IMPLEMENTATION
#define SOKOL_IMPL
#define SOKOL_GLCORE
#define SOKOL_EXTERNAL_GL_LOADER

#include "log.hpp"

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <sokol/sokol_gfx.h>
#include <string>

using namespace vt::core;

static void _gl_init() {
	static i32 version = 0;
	if (version != 0) {
		return;
	}

	version = gladLoadGL(glfwGetProcAddress);
	if (version == 0) {
		vt::log::fatal("[DISPLAY] | GL > Failed to initialize OpenGL context");
		return; // [[noreturn]]
	}
	vt::log::info(
		"[DISPLAY] | GL > Loaded version: {}.{}", GLAD_VERSION_MAJOR(version),
		GLAD_VERSION_MINOR(version)
	);
}

bool Display::create(i32 width, i32 height, std::string_view title) {
	if (m_window) {
		glfwDestroyWindow(m_window);
		m_window = nullptr;
	}

	m_context.version_major = 4;
	m_context.version_minor = 1;

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_context.version_major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_context.version_minor);
#ifdef VT_PLATFORM_MACOS
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif

	m_window = glfwCreateWindow(
		width, height, std::string(title).c_str(), nullptr, nullptr
	);
	if (!m_window) {
		vt::log::error("[DISPLAY] | Window > Failed to create GLFW window handler");
		close();
		return false;
	}

	glfwSetWindowUserPointer(m_window, this);

	glfwMakeContextCurrent(m_window);
	_gl_init();

	sg_desc desc;
#if !defined(NDEBUG)
	desc.logger.func = vt::log::slog_callback;
#endif
	sg_setup(desc);
	if (!sg_isvalid()) {
		vt::log::error("[DISPLAY] | Renderer > Failed to setup Sokol");
		close();
		return false;
	}

	return true;
}

void Display::close() {
	sg_shutdown();

	if (m_window) {
		glfwDestroyWindow(m_window);
	}
}

void Display::present() {
	sg_commit();

	if (m_window) {
		glfwSwapBuffers(m_window);
	}
}

bool Display::is_open() {
	return m_window && !glfwWindowShouldClose(m_window);
}
