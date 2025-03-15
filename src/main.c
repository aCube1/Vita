#include "cglm/struct/affine-pre.h"
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

typedef enum vt_status {
	VT_STATUS_FAILURE,
	VT_STATUS_SUCCESS,
	VT_STATUS_CONTINUE,
} vt_status;

typedef struct vt_appstate {
	vt_window window;
	vt_renderer *render;
	vt_status status;

	sg_pipeline test_pip;
} vt_appstate;

static void _vt_glfw_err_callback(i32 err, const char *desc) {
	LOG_ERROR("[VT] > GLFW Error - (%d) %s", err, desc);
}

static vt_status _vt_init(vt_appstate *app) {
	if (!glfwInit()) {
		LOG_ERROR("[VT] > Failed to initialize GLFW library");
		return VT_STATUS_FAILURE;
	}
	vt_error err;

	err = vt_create_window(
		&app->window, WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT, "Vita"
	);
	if (err != VT_ERROR_NONE) {
		LOG_ERROR("[VT] > Unable to create primary window");
		return VT_STATUS_FAILURE;
	}

	err = vt_gpu_init_resources();
	if (err != VT_ERROR_NONE) {
		LOG_ERROR("[GPU] > Failed to alloc default resources");
		return VT_STATUS_FAILURE;
	}

	app->render = vt_create_renderer();
	if (!app->render) {
		LOG_ERROR("[VT] > Unable to create main 2D renderer");
		return VT_STATUS_FAILURE;
	}

	static const char _test_vs_source[] = {
#embed "../assets/shaders/test.vert.glsl"
		, '\0'
	};

	static const char _common_fs_source[] = {
#embed "../assets/shaders/common.frag.glsl"
		, '\0'
	};

	sg_shader_desc shdrdesc = {
		.vertex_func.source = _test_vs_source,
		.vertex_func.entry = "main",
		.fragment_func.source = _common_fs_source,
		.fragment_func.entry = "main",
		.attrs[VT_SHADER_ATTR_POS].glsl_name = "a_pos",
		.attrs[VT_SHADER_ATTR_UV].glsl_name = "a_uv",
		.attrs[VT_SHADER_ATTR_COLOR].glsl_name = "a_color",
		.uniform_blocks[0] = {
			.stage = SG_SHADERSTAGE_VERTEX,
			.size = 128,
			.layout = SG_UNIFORMLAYOUT_STD140,
			.glsl_uniforms[0] = {
				.type = SG_UNIFORMTYPE_MAT4,
				.array_count = 1,
				.glsl_name = "rot",
			},
			.glsl_uniforms[1] = {
				.type = SG_UNIFORMTYPE_MAT4,
				.array_count = 1,
				.glsl_name = "spin",
			},
		},
		.images[0] = {
			.stage = SG_SHADERSTAGE_FRAGMENT,
			.image_type = SG_IMAGETYPE_2D,
			.sample_type = SG_IMAGESAMPLETYPE_FLOAT,
			.multisampled = false,
		},
		.samplers[0] = {
			.stage = SG_SHADERSTAGE_FRAGMENT,
			.sampler_type = SG_SAMPLERTYPE_FILTERING,
		},
		.image_sampler_pairs[VT_SHADER_UB_TEX0] = {
			.stage = SG_SHADERSTAGE_FRAGMENT,
			.image_slot = 0,
			.sampler_slot = 0,
			.glsl_name = "u_tex0",
		},
		.label = "vt_gpu.common_shdr",
	};

	sg_shader shdr = sg_make_shader(&shdrdesc);
	if (sg_query_shader_state(shdr) != SG_RESOURCESTATE_VALID) {
		sg_destroy_shader(shdr);
	}

	app->test_pip = vt_make_shader_pipeline(shdr, VT_PRIMITIVETYPE_TRIANGLES);
	if (app->test_pip.id == SG_INVALID_ID) {
		return VT_STATUS_FAILURE;
	}

	return VT_STATUS_CONTINUE;
}

static void _vt_quit(vt_appstate *app) {
	if (app->render) {
		vt_destroy_renderer(app->render);
	}

	sg_destroy_pipeline(app->test_pip);

	vt_gpu_clean_resources();
	vt_destroy_window(&app->window);
	glfwTerminate();
}

// TODO: Function just for testing, remove later
static void _vt_draw_quad(vt_renderer *render, f32 x, f32 y, f32 w, f32 h) {
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

	vt_vertex vertices[6] = {
		{ quad[0], quad_uv[0], VT_COLOR_RED },	 // Top left
		{ quad[1], quad_uv[1], VT_COLOR_GREEN }, // Top right
		{ quad[2], quad_uv[2], VT_COLOR_BLUE },	 // Bottom right

		{ quad[0], quad_uv[0], VT_COLOR_RED },	 // Top left
		{ quad[2], quad_uv[2], VT_COLOR_BLUE },	 // Bottom right
		{ quad[3], quad_uv[3], VT_COLOR_WHITE }, // Bottom left
	};

	vt_render_geometry(render, VT_PRIMITIVETYPE_TRIANGLES, vertices, 6);
}

static vt_status _vt_iterate(vt_appstate *app) {
	struct uniform_data {
		mat4s rot;
		mat4s spin;
	} data = {
		.rot = glms_rotate_at(
			GLMS_MAT4_IDENTITY, (vec3s) { { 64, 64, 0.0 } }, glfwGetTime(), GLMS_ZUP
		),
		.spin = glms_spin(GLMS_MAT4_IDENTITY, glfwGetTime(), GLMS_ZUP),

		// .rot = GLMS_MAT4_IDENTITY,
		// .spin = GLMS_MAT4_IDENTITY,
	};

	vt_render_begin(app->render, app->window.framesize);
	vt_set_render_pipeline(app->render, app->test_pip);
	vt_set_render_uniform(app->render, 0, &(sg_range) { .ptr = &data, sizeof(data) });
	_vt_draw_quad(app->render, 960 / 2, 540 / 2, 128, 128);
	vt_render_flush(app->render);
	vt_render_end(app->render);

	vt_window_update(&app->window);

	if (app->window.should_close) {
		return VT_STATUS_SUCCESS;
	}
	return VT_STATUS_CONTINUE;
}

int main(void) {
	glfwSetErrorCallback(_vt_glfw_err_callback);

	vt_appstate app = {};
	app.status = _vt_init(&app);

	while (app.status == VT_STATUS_CONTINUE) {
		app.status = _vt_iterate(&app);
	}
	_vt_quit(&app);

	if (app.status == VT_STATUS_FAILURE) {
		LOG_ERROR("[VT] > App exited with a critical failure");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
