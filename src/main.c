#include "common.h"
#include "log.h"
#include "vita/gpu.h"
#include "vita/gpu/renderer.h"
#include "vita/window.h"
#include <stdlib.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define WINDOW_DEFAULT_WIDTH  960
#define WINDOW_DEFAULT_HEIGHT 540

#define _DEBUG_RECT_COUNTS	  2040

typedef enum VT_AppStatus {
	VT_APP_FAILURE,
	VT_APP_SUCCESS,
	VT_APP_CONTINUE,
} VT_AppStatus;

typedef struct VT_AppState {
	VT_Window *window;
	VT_Renderer *render;
	vec2s rects[_DEBUG_RECT_COUNTS];

	VT_AppStatus status;
} VT_AppState;

static void _vt_glfw_err_callback(i32 err, const char *desc) {
	LOG_ERROR("[VT] > GLFW Error - (%d) %s", err, desc);
}

static VT_AppStatus _vt_init(VT_AppState *app) {
	if (!glfwInit()) {
		LOG_ERROR("[VT] > Failed to initialize GLFW library");
		return VT_APP_FAILURE;
	}

	app->window = vt_create_window(WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT, "Vita");
	if (!app->window) {
		LOG_ERROR("[VT] > Unable to create primary window");
		return VT_APP_FAILURE;
	}

	if (!vt_gpu_setup()) {
		LOG_ERROR("[VT] > Unable to setup GPU handler");
		return VT_APP_FAILURE;
	}

	app->render = vt_create_renderer();
	if (!app->render) {
		LOG_ERROR("[VT] > Unable to create main 2D renderer");
		return VT_APP_FAILURE;
	}

	u32 y = 0;
	u32 x = 0;
	for (u32 i = 0; i < _DEBUG_RECT_COUNTS; i += 1) {
		app->rects[i].x = x;
		app->rects[i].y = y;

		if (x + 16 >= 960) {
			y += 16;
			x = 0;
		} else {
			x += 16;
		}
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

// TODO: Function just for testing, remove later
static void _vt_draw_quad(VT_Renderer *render, f32 x, f32 y, f32 w, f32 h) {
	vec3s quad[4] = {
		{ { x, y, 0.0 } },		   // Top left
		{ { x + w, y, 0.0 } },	   // Top right
		{ { x + w, y + h, 0.0 } }, // Bottom right
		{ { x, y + h, 0.0 } },	   // Bottom left
	};

	const vec2s quad_uv[4] = {
		{ { 0.0f, 0.0f } }, // Top left
		{ { 1.0f, 0.0f } }, // Top right
		{ { 1.0f, 1.0f } }, // Bottom right
		{ { 0.0f, 1.0f } }, // Bottom left
	};

	VT_Vertex vertices[6] = {
		{ quad[0], quad_uv[0], VT_COLOR_RED },	 // Top left
		{ quad[1], quad_uv[1], VT_COLOR_GREEN }, // Top right
		{ quad[2], quad_uv[2], VT_COLOR_BLUE },	 // Bottom right

		{ quad[0], quad_uv[0], VT_COLOR_RED },	 // Top left
		{ quad[2], quad_uv[2], VT_COLOR_BLUE },	 // Bottom right
		{ quad[3], quad_uv[3], VT_COLOR_WHITE }, // Bottom left
	};

	vt_render_geometry(render, VT_PRIMITIVETYPE_TRIANGLES, vertices, 6);
}

static VT_AppStatus _vt_iterate(VT_AppState *app) {
	vt_render_begin(app->render, vt_get_window_framesize(app->window));

	for (u32 i = 0; i < _DEBUG_RECT_COUNTS; i += 1) {
		_vt_draw_quad(app->render, app->rects[i].x, app->rects[i].y, 16, 16);
	}

	vt_render_flush(app->render);
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
		LOG_ERROR("[VT] > App exited with a critical failure");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
