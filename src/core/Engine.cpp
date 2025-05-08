#include "core/Engine.hpp"

#include "log.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace vt::core;
using namespace vt::gfx;

static void _glfw_log_error_callback(i32 err, const char *msg) {
	vt::log::error("[ENGINE] | GLFW {} > {}", err, msg);
}

Engine::Engine() {
	glfwSetErrorCallback(_glfw_log_error_callback);

	if (!glfwInit()) {
		vt::log::fatal("[ENGINE] > Failed to initialize GLFW library");
		return; // [[noreturn]]
	}

	bool success = m_display.create(960, 540, "Vitae");
	if (!success) {
		vt::log::fatal("[ENGINE] > Failed to create Display");
		return; // [[noreturn]]
	}

	success = m_render.init();
	if (!success) {
		vt::log::fatal("[ENGINE] > Failed to initialize Batch Renderer");
		return; // [[noreturn]]
	}
}

Engine::~Engine() {
	m_display.close();
	glfwTerminate();
}

void Engine::run() {
	View camera { { 480.0, 270.0 } };

	while (m_is_active) {
		m_render.begin_frame(m_display.get_pass());
		{
			Drawable rect;

			m_render.begin(camera);

			rect = Drawable::make_rect(
				DrawMode::ModeFill, 480 - 32, 270 - 32, 64, 64, Color::Blue
			);
			rect.set_rotation(glfwGetTime());
			m_render.draw(rect);

			rect = Drawable::make_rect(
				DrawMode::ModeLines, 480 - 46, 270 - 46, 92, 92, Color::Green
			);
			rect.set_rotation(glfwGetTime() * -1.0);
			rect.scale({ 2.0 });
			m_render.draw(rect);

			m_render.end();
		}
		m_render.end_frame();

		m_display.present();
		glfwPollEvents();

		if (!m_display.is_open()) {
			m_is_active = false;
		}
	}
}
