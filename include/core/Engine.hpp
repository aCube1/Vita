#ifndef _VT_CORE_ENGINE_HPP
#define _VT_CORE_ENGINE_HPP

#include "core/Display.hpp"
#include "gfx/RenderBatcher.hpp"
#include "types.hpp"

namespace vt::core {

class Engine final {
public:
	Engine();
	~Engine();

	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	void run();

private:
	Display m_display;
	gfx::RenderBatcher m_render;
	bool m_is_active { true };
};

} // namespace vt::core

#endif
