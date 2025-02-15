#include "gpu.h"
#include "utils.h"
#include "window.h"
#include <stdlib.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

#define WINDOW_DEFAULT_WIDTH  960
#define WINDOW_DEFAULT_HEIGHT 540

typedef struct vtAppState {
	VT_Window *window;
} vtAppState;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
	(void)argc;
	(void)argv;

	vtAppState *app = calloc(1, sizeof(vtAppState));
	if (app == nullptr) {
		LOG_FATAL("[MAIN] > Failed to allocate AppState");
		return SDL_APP_FAILURE;
	}
	*appstate = app;

	if (!SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO)) {
		LOG_FATAL("[MAIN] > Failed to initialize SDL3 Subsystems");
		return SDL_APP_FAILURE;
	}
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	app->window = vt_create_window(WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT, "Vitae");
	if (app->window == nullptr) {
		LOG_FATAL("[MAIN] > Unable to create primary window");
		return SDL_APP_FAILURE;
	}

	if (!vt_gpu_setup()) {
		LOG_FATAL("[MAIN] > Unable to setup gpu handler");
		return SDL_APP_FAILURE;
	}

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
	vtAppState *app = (vtAppState *)appstate;

	if (vt_window_should_close(app->window)) {
		return SDL_APP_SUCCESS;
	}
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
	vtAppState *app = (vtAppState *)appstate;
	(void)app;
	(void)event;

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
	if (appstate == nullptr) {
		return;
	}
	(void)result;

	vtAppState *app = (vtAppState *)appstate;

	if (app->window != nullptr) {
		vt_destroy_window(app->window);
	}

	vt_gpu_shutdown();
	free(app);
}
