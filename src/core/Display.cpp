#include "core/Display.hpp"

#include "gfx/common.hpp"
#include "log.hpp"

#include <string>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

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
	m_context.pixel_format = SG_PIXELFORMAT_RGBA8;
	m_context.depth_format = SG_PIXELFORMAT_DEPTH;
	m_context.samples = 1;

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
	desc.environment.defaults = {
		.color_format = m_context.pixel_format,
		.depth_format = m_context.depth_format,
		.sample_count = m_context.samples,
	};
#if !defined(NDEBUG)
	desc.logger.func = vt::log::slog_callback;
#endif
	sg_setup(desc);
	if (!sg_isvalid()) {
		vt::log::error("[DISPLAY] | Renderer > Failed to setup Sokol");
		close();
		return false;
	}

	_init();
	return true;
}

void Display::close() {
	_deinit();
	sg_shutdown();

	if (m_window) {
		glfwDestroyWindow(m_window);
	}
}

void Display::begin() {
	i32 framebuffer_width;
	i32 framebuffer_height;

	glfwGetFramebufferSize(m_window, &framebuffer_width, &framebuffer_height);
	m_render_pass.swapchain.width = framebuffer_width;
	m_render_pass.swapchain.height = framebuffer_height;
	m_render_pass.swapchain.sample_count = m_context.samples;
	m_render_pass.swapchain.color_format = m_context.pixel_format;
	m_render_pass.swapchain.depth_format = m_context.depth_format;
	m_render_pass.action.colors[0] = {
		.load_action = SG_LOADACTION_CLEAR,
		.store_action = SG_STOREACTION_STORE,
		.clear_value = { 0.2, 0.2, 0.2, 1.0 },
	};

	_begin_frame(m_render_pass);
}

void Display::end() {
	flush();
	_end_frame();
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
