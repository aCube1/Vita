#include "core/Engine.hpp"

#include "log.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace vt::core;

static void _glfw_log_error_callback(i32 err, const char *msg) {
	vt::log::error("[ENGINE] | GLFW {} > {}", err, msg);
}

Engine::Engine() {
	glfwSetErrorCallback(_glfw_log_error_callback);

	if (!glfwInit()) {
		vt::log::fatal("[ENGINE] > Failed to initialized GLFW library");
		return; // [[noreturn]]
	}

	bool success = m_display.create(960, 540, "Vitae");
	if (!success) {
		vt::log::fatal("[ENGINE] > Failed to initialized Display");
		return; // [[noreturn]]
	}
}

Engine::~Engine() {
	m_display.close();
	glfwTerminate();
}

void Engine::run() {
	while (m_is_active) {
		m_display.present();
		glfwPollEvents();

		if (!m_display.is_open()) {
			m_is_active = false;
		}
	}
}
