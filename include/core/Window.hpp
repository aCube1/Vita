#ifndef _VT_CORE_WINDOW_HPP
#define _VT_CORE_WINDOW_HPP

#include "gfx/Color.hpp"
#include "types.hpp"

#include <SDL3/SDL_video.h>
#include <string>

namespace vt::core {

struct ContextSettings {
	u32 version_major;
	u32 version_minor;
	sg_pixel_format pixel_format;
	sg_pixel_format depth_format;
	i32 samples;
};

class Window {
public:
	Window() = default;
	~Window() = default;

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	bool create(i32 width, i32 height, const std::string& title);
	void close();

	void present();

	[[nodiscard]] const sg_pass& get_pass() const;
	[[nodiscard]] const ContextSettings& get_context_settings() const;

private:
	SDL_Window *m_handle { nullptr };
	SDL_GLContext m_gl_context { nullptr };
	ContextSettings m_settings;
	sg_pass m_pass;
};

} // namespace vt::core

#endif
