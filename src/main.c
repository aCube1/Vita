#include "common.h"
#include "vita/gpu.h"
#include "vita/gpu/renderer.h"
#include "vita/window.h"
#include <stdlib.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define WINDOW_DEFAULT_WIDTH  960
#define WINDOW_DEFAULT_HEIGHT 540

typedef enum VT_AppStatus {
	VT_APP_FAILURE,
	VT_APP_SUCCESS,
	VT_APP_CONTINUE,
} VT_AppStatus;

typedef struct VT_AppState {
	VT_Window *window;
	VT_Renderer *render;

	VT_AppStatus status;
} VT_AppState;

static void _vt_glfw_err_callback(i32 err, const char *desc) {
	LOG_ERROR("[VT] > GLFW Error - (%d) %s", err, desc);
}

static VT_AppStatus _vt_init(VT_AppState *app) {
	if (!glfwInit()) {
		LOG_FATAL("[VT] > Failed to initialize GLFW library");
		return VT_APP_FAILURE;
	}

	app->window = vt_create_window(WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT, "Vita");
	if (!app->window) {
		LOG_FATAL("[VT] > Unable to create primary window");
		return VT_APP_FAILURE;
	}

	if (!vt_gpu_setup()) {
		LOG_FATAL("[VT] > Unable to setup GPU handler");
		return VT_APP_FAILURE;
	}

	app->render = vt_create_renderer();
	if (!app->render) {
		LOG_FATAL("[VT] > Unable to create main 2D renderer");
		return VT_APP_FAILURE;
	}

	return VT_APP_CONTINUE;
}

static void _vt_quit(VT_AppState *app) {
	if (app->window) {
		vt_destroy_window(app->window);
	}
	if (app->render) {
		vt_destroy_renderer(app->render);
	}

	vt_gpu_shutdown();
	glfwTerminate();
}

static VT_AppStatus _vt_iterate(VT_AppState *app) {
	vt_render_begin(app->render, vt_get_window_framesize(app->window));
	vt_render_end(app->render);
	vt_window_update(app->window);

	if (vt_window_should_close(app->window)) {
		return VT_APP_SUCCESS;
	}
	return VT_APP_CONTINUE;
}

int main(void) {
	glfwSetErrorCallback(_vt_glfw_err_callback);

	VT_AppState app = {};
	app.status = _vt_init(&app);

	while (app.status == VT_APP_CONTINUE) {
		app.status = _vt_iterate(&app);
	}
	_vt_quit(&app);

	if (app.status == VT_APP_FAILURE) {
		LOG_FATAL("[VT] > App exited with a critical failure");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
