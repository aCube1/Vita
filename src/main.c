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
	if (app == NULL) {
		LOG_FATAL("[MAIN] > Failed to allocate AppState");
		return SDL_APP_FAILURE;
	}

	if (!SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO)) {
		LOG_FATAL("[MAIN] > Failed to initialize SDL3 Subsystems");
		return SDL_APP_FAILURE;
	}

	app->window = vt_create_window(WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT, "Vitae");
	if (app->window == NULL) {
		LOG_FATAL("[MAIN] > Unable to create primary window");
		return SDL_APP_FAILURE;
	}

	*appstate = app;
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
	vtAppState *app = (vtAppState *)appstate;

	(void)app;

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
	vtAppState *app = (vtAppState *)appstate;
	(void)app;

	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS;
	}

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
	(void)result;

	vtAppState *app = (vtAppState *)appstate;

	if (app->window != NULL) {
		vt_destroy_window(app->window);
	}

	free(app);
}
