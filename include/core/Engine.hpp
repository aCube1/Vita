#ifndef _VT_CORE_ENGINE_HPP
#define _VT_CORE_ENGINE_HPP

#include "core/Window.hpp"
#include "gfx/RenderBatcher.hpp"

union SDL_Event;

namespace vt::core {

class Engine final {
public:
	static bool init() noexcept;
	static void quit();
	static Engine& get();

	Engine();
	~Engine();

	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	void run_loop();

private:
	Window m_window;
	gfx::RenderBatcher m_render;
	bool m_is_running {};

	void _do_update();

	bool _init_graphics_driver();
	void _terminate_graphics_driver();
};

} // namespace vt::core

#endif
