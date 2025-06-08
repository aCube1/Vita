#ifndef _VT_ENGINE_HPP
#define _VT_ENGINE_HPP

#include "core/Window.hpp"
#include "gfx/RenderBatcher.hpp"

namespace vt {

class Engine final {
public:
	static bool init() noexcept;
	static void quit();
	static Engine& get();

	Engine();
	~Engine();

	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	void run();

private:
	Window m_window;
	RenderBatcher m_render;
	bool m_is_valid {};

	void _do_update();

	bool _init_graphics_driver();
	void _terminate_graphics_driver();
};

} // namespace vt

#endif
