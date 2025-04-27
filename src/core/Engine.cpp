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

static void _draw_rect(Display& display, f32 x, f32 y, f32 w, f32 h) {
	vt::Vec3 quad[4] = {
		vt::Vec3(0.0, 0.0, 0.0), // Top Left
		vt::Vec3(w, 0.0, 0.0),	 // Top Right
		vt::Vec3(w, h, 0.0),	 // Bottom Right
		vt::Vec3(0.0, h, 0.0),	 // Bottom Left
	};

	vt::Vec2 quad_uv[4] = {
		vt::Vec2(0.0, 0.0), // Top Left
		vt::Vec2(1.0, 0.0), // Top Right
		vt::Vec2(1.0, 1.0), // Bottom Right
		vt::Vec2(0.0, 1.0), // Bottom Left
	};

	std::vector<vt::gfx::Vertex> vertices {
		vt::gfx::Vertex(quad[0], quad_uv[0], vt::gfx::Color::Red),
		vt::gfx::Vertex(quad[1], quad_uv[1], vt::gfx::Color::Green),
		vt::gfx::Vertex(quad[2], quad_uv[2], vt::gfx::Color::Blue),

		vt::gfx::Vertex(quad[0], quad_uv[0], vt::gfx::Color::Red),
		vt::gfx::Vertex(quad[2], quad_uv[2], vt::gfx::Color::Blue),
		vt::gfx::Vertex(quad[3], quad_uv[3], vt::gfx::Color::White),
	};

	vt::gfx::Drawable obj { SG_PRIMITIVETYPE_TRIANGLES, vertices };
	vt::Transform transform;
	transform.set_origin(vt::Vec2(w / 2, h / 2));
	transform.translate(vt::Vec3 { x, y, 0.0 });
	transform.rotate(glfwGetTime());

	display.draw(obj, transform);
}

void Engine::run() {
	while (m_is_active) {
		m_display.begin();
		_draw_rect(m_display, 960 / 2, 540 / 2, 128, 128);
		_draw_rect(m_display, 480 + 128, 270 + 128, 16, 16);
		m_display.end();

		m_display.begin();
		_draw_rect(m_display, 0, 0, 128, 128);

		{
			m_display.begin();
			_draw_rect(m_display, 0, 270, 32, 32);
			m_display.end();
		}
		_draw_rect(m_display, 480, 0, 16, 16);
		m_display.end();

		m_display.present();
		glfwPollEvents();

		if (!m_display.is_open()) {
			m_is_active = false;
		}
	}
}
