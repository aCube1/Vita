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

static void _draw_rect(RenderBatcher& render, f32 x, f32 y, f32 w, f32 h) {
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

	render.draw(obj, transform);
}

static void _draw_rect_lines(RenderBatcher& render, f32 x, f32 y, f32 w, f32 h) {
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

	vt::gfx::Drawable obj { SG_PRIMITIVETYPE_LINE_STRIP, vertices };
	vt::Transform transform;
	transform.set_origin(vt::Vec2(w / 2, h / 2));
	transform.translate(vt::Vec3 { x, y, 0.0 });
	transform.scale(vt::Vec2 { 2.0 });

	render.draw(obj, transform);
}

void Engine::run() {
	View camera { { 480.0, 270.0 } };

	while (m_is_active) {
		m_render.begin_frame(m_display.get_pass());
		{
			m_render.begin(camera);
			_draw_rect_lines(m_render, 480 - 64, 270 - 64, 128, 128);
			_draw_rect(m_render, 480 - 46, 270 - 46, 92, 92);
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
