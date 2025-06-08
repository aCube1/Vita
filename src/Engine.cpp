#include "Engine.hpp"

#include "gfx/Drawable.hpp"
#include "log.hpp"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

using namespace vt;

static Engine *s_engine { nullptr };

bool Engine::init() noexcept {
	assert(!s_engine);

	s_engine = new (std::nothrow) Engine;
	if (!s_engine) {
		return false;
	}

	return true;
}

void Engine::quit() {
	delete s_engine;
	s_engine = nullptr;
}

Engine& Engine::get() {
	assert(s_engine);
	return *s_engine;
}

Engine::Engine() {
	if (!SDL_InitSubSystem(SDL_INIT_EVENTS | SDL_INIT_VIDEO)) {
		vt::log::fatal("[ENGINE] > Failed to initialize Events and Video subsystems");
		return; // [[noreturn]]
	}

	if (!m_window.create(960, 540, "Vitae")) {
		vt::log::fatal("[ENGINE] > Failed to create Window");
		return; // [[noreturn]]
	}

	if (!_init_graphics_driver()) {
		vt::log::fatal("[ENGINE] > Failed to initialize Graphics Driver");
		return; // [[noreturn]]
	}

	if (!m_render.init()) {
		vt::log::fatal("[ENGINE] > Failed to initialize Batch Renderer");
		return; // [[noreturn]]
	}

	m_is_valid = true;
}

Engine::~Engine() {
	m_render.terminate();
	_terminate_graphics_driver();
	m_window.close();

	SDL_Quit();
}

void Engine::run() {
	assert(m_is_valid);
	bool should_quit = false;

	while (!should_quit) {
		_do_update();

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				should_quit = true;
			}
		}
	}
}

void Engine::_do_update() {
	auto rect = Drawable::make_rect(DrawMode::ModeFill, 128, 128, 32, 32);

	m_render.set_target(m_window);
	m_render.draw(rect);

	m_render.flush();
	m_window.present();
}

bool Engine::_init_graphics_driver() {
	i32 version = gladLoadGL(SDL_GL_GetProcAddress);
	if (version == 0) {
		vt::log::error("[GFX] | GL > Failed to initialize OpenGL context");
		return false;
	}
	vt::log::info(
		"[GFX] | GL > Loaded version: {}.{}", GLAD_VERSION_MAJOR(version),
		GLAD_VERSION_MINOR(version)
	);

	const auto& context_settings = m_window.get_context_settings();
	sg_desc desc {};
	desc.environment.defaults = {
		.color_format = context_settings.pixel_format,
		.depth_format = context_settings.depth_format,
		.sample_count = context_settings.samples,
	};
#if !defined(NDEBUG)
	desc.logger.func = vt::log::slog_callback;
#endif

	sg_setup(desc);
	if (!sg_isvalid()) {
		vt::log::error("[GFX] | Renderer > Failed to setup Sokol");
		return false;
	}

	return true;
}

void Engine::_terminate_graphics_driver() {
	sg_shutdown();
}
