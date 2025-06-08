#include "core/Window.hpp"

#include "log.hpp"

#include <SDL3/SDL_events.h>
#include <string>

using namespace vt;

bool Window::create(i32 width, i32 height, const std::string& title) {
	if (m_handle) {
		SDL_DestroyWindow(m_handle);
		m_handle = nullptr;
	}

	m_settings.version_major = 4;
	m_settings.version_minor = 1;
	m_settings.pixel_format = SG_PIXELFORMAT_RGBA8;
	m_settings.depth_format = SG_PIXELFORMAT_DEPTH;
	m_settings.samples = 1;

	SDL_GL_ResetAttributes();
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, m_settings.version_major);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, m_settings.version_minor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#if VT_PLATFORM_MACOS
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif

	SDL_WindowFlags flags = SDL_WINDOW_OPENGL;

	m_handle = SDL_CreateWindow(std::string(title).c_str(), width, height, flags);
	if (!m_handle) {
		vt::log::error("[WINDOW] > Failed to create Window: {}", SDL_GetError());
		close();
		return false;
	}

	m_gl_context = SDL_GL_CreateContext(m_handle);
	if (!m_gl_context) {
		vt::log::error("[WINDOW] > Failed to create OpenGL context: {}", SDL_GetError());
		close();
		return false;
	}

	SDL_GL_MakeCurrent(m_handle, m_gl_context);

	SDL_AddEventWatch(_event_watcher, this);

	m_size.w = width;
	m_size.h = height;

	return true;
}

void Window::close() {
	if (m_gl_context) {
		SDL_GL_DestroyContext(m_gl_context);
	}

	if (m_handle) {
		SDL_DestroyWindow(m_handle);
	}
}

void Window::present() {
	if (!m_handle) {
		return;
	}

	sg_commit();
	SDL_GL_SwapWindow(m_handle);
}

[[nodiscard]] const Vec2& Window::get_size() const {
	return m_size;
}

[[nodiscard]] const ContextSettings& Window::get_context_settings() const {
	return m_settings;
}

bool Window::_event_watcher(void *usrdata, SDL_Event *event) {
	if (event->type < SDL_EVENT_WINDOW_FIRST || event->type > SDL_EVENT_WINDOW_LAST) {
		return true;
	}

	if (event->type == SDL_EVENT_WINDOW_RESIZED) {
		auto *window = (Window *)usrdata;
		window->m_size.w = event->window.data1;
		window->m_size.h = event->window.data2;
	}

	return false;
}
