// Code adapted from:
// https://github.com/edubart/sokol_gp/blob/master/sokol_gp.h

#include "gpu/renderer.h"
#include "cglm/struct/cam.h"
#include "cglm/struct/mat3.h"

#include <stdlib.h>

#define _VT_DEFAULT_MAX_VERTICES 65536
#define _VT_DEFAULT_MAX_COMMANDS 16384
#define _VT_MAX_MOVE_VERTICES	 128
#define _VT_MAX_STACK_DEPTH		 64

enum {
	_VT_COMMANDTYPE_NONE = 0,
	_VT_COMMANDTYPE_VIEWPORT,
	_VT_COMMANDTYPE_SCISSOR,
	_VT_COMMANDTYPE_DRAW,
};

typedef struct VT_DrawCall {
	sg_pipeline pipeline;
	mat4s mvp;
	u32 vertex_count;
	u32 vertex_idx;
	u32 uniform_idx;
} VT_DrawCall;

typedef struct VT_RenderCommand {
	u32 type;

	union {
		irect viewport;
		irect scissor;
		VT_DrawCall draw;
	};
} VT_RenderCommand;

struct VT_Renderer {
	u32 cur_vertex;
	u32 cur_command;
	u32 vertices_count;
	u32 commands_count;

	u32 cur_state;
	u32 cur_transform;
	VT_RenderState state_stack[_VT_MAX_STACK_DEPTH];
	mat4s transform_stack[_VT_MAX_STACK_DEPTH];

	VT_RenderState state;

	sg_pipeline pipelines[_VT_PRIMITIVETYPE_COUNT];
	sg_buffer vertex_buf;
	VT_Vertex *vertices;
	VT_RenderCommand *commands;
	VT_UniformData *uniforms;

	bool has_error;
};

static sg_pipeline _vt_lookup_pipeline(
	VT_Renderer *render, VT_PrimitiveType primitive_type
) {
	u32 pip_idx = primitive_type;
	if (render->pipelines[pip_idx].id != SG_INVALID_ID) {
		return render->pipelines[pip_idx];
	}

	sg_pipeline pip = vt_gpu_make_pipeline(vt_gpu_get_common_shader(), primitive_type);
	if (pip.id != SG_INVALID_ID) {
		render->pipelines[pip_idx] = pip;
	}

	return pip;
}

VT_Renderer *vt_create_renderer(void) {
	VT_Renderer *render = calloc(1, sizeof(VT_Renderer));
	if (render == nullptr) {
		LOG_ERROR("[RENDER] > Failed to alloc memory");
		return nullptr;
	}

	render->vertices_count = _VT_DEFAULT_MAX_VERTICES;
	render->commands_count = _VT_DEFAULT_MAX_COMMANDS;

	// Alloc vertices memory, commands queue and uniform memory
	render->vertices = calloc(render->vertices_count, sizeof(VT_Vertex));
	render->commands = calloc(render->commands_count, sizeof(VT_RenderCommand));
	render->uniforms = calloc(render->commands_count, sizeof(VT_UniformData));
	if (!render->vertices || !render->commands || !render->uniforms) {
		LOG_ERROR("[RENDER] > Failed to alloc resources memory");
		vt_destroy_renderer(render);
		return nullptr;
	}

	// Check if all primitive pipelines are valid
	for (u32 i = 0; i < _VT_PRIMITIVETYPE_COUNT; i += 1) {
		sg_pipeline pip = _vt_lookup_pipeline(render, i);
		if (pip.id == SG_INVALID_ID) {
			LOG_ERROR("[RENDER] > Failed to make common pipelines");
			vt_destroy_renderer(render);
			return nullptr;
		}
	}

	// Setup buffers
	sg_buffer_desc bufdesc = {
		.size = render->vertices_count * sizeof(VT_Vertex),
		.type = SG_BUFFERTYPE_VERTEXBUFFER,
		.usage = SG_USAGE_STREAM,
		.label = "vt_render.vertex_buffer",
	};

	render->vertex_buf = sg_make_buffer(&bufdesc);
	if (sg_query_buffer_state(render->vertex_buf) != SG_RESOURCESTATE_VALID) {
		LOG_ERROR("[RENDER] > Unable to make vertex buffer");
		vt_destroy_renderer(render);
		return nullptr;
	}

	return render;
}

void vt_destroy_renderer(VT_Renderer *render) {
	if (render == nullptr) {
		return;
	}

	for (u32 i = 0; i < _VT_PRIMITIVETYPE_COUNT; i += 1) {
		sg_pipeline pip = render->pipelines[i];
		if (sg_query_pipeline_state(pip) != SG_RESOURCESTATE_INVALID) {
			sg_destroy_pipeline(pip);
		}
	}
	if (sg_query_buffer_state(render->vertex_buf) != SG_RESOURCESTATE_INVALID) {
		sg_destroy_buffer(render->vertex_buf);
	}

	if (render->vertices != nullptr) {
		free(render->vertices);
	}
	if (render->commands != nullptr) {
		free(render->commands);
	}
	if (render->uniforms != nullptr) {
		for (u32 i = 0; i < render->commands_count; i += 1) {
			void *ptr = render->uniforms[i].ptr;
			if (ptr != nullptr) {
				free(ptr);
			}
		}
		free(render->uniforms);
	}

	free(render);
}

void vt_render_begin(VT_Renderer *render, ivec2s framesize) {
	assert(render != nullptr);

	if (render->cur_state >= _VT_MAX_STACK_DEPTH) {
		LOG_WARN("[RENDER] > State's stack has overflow");
		render->has_error = true;
		return;
	}
	render->has_error = false;

	// Store current state
	render->state_stack[render->cur_state] = render->state;
	render->cur_state += 1;

	// Setup new state
	render->state.framesize = framesize;
	render->state.viewport.x = 0;
	render->state.viewport.y = 0;
	render->state.viewport.w = framesize.x;
	render->state.viewport.h = framesize.y;
	render->state.projview = glms_ortho(0.0, framesize.x, framesize.y, 0.0, -1.0, 1.0);
	render->state.transform = GLMS_MAT3_IDENTITY;
	render->state.color = VT_COLOR_WHITE;
	render->state.uniform = nullptr;
	render->state._base_vertex = render->cur_vertex;
	render->state._base_command = render->cur_command;

	render->state.render_pass = (sg_pass) {
		.action = {
			.colors[0].clear_value = { 0.25, 0.25, 0.25, 1.0},
		},
		.label = "vt_gpu.render_pass",
	};
}

void vt_render_end(VT_Renderer *render) {
	assert(render != nullptr);

	if (render->cur_state <= 0) {
		LOG_WARN("[RENDER] > State's stack underflow");
		render->has_error = true;
		return;
	}

	// Restore previous state
	render->cur_state -= 1;
	render->state = render->state_stack[render->cur_state];
}

void _vt_flush_draw(
	VT_Renderer *render,
	VT_DrawCall *draw,
	sg_bindings *binds,
	u32 *cur_pipeline,
	u32 *cur_uniform
) {
	bool apply_bindings = false;
	bool apply_uniforms = false;

	// Reapply bindings if pipeline is different from current
	if (draw->pipeline.id != *cur_pipeline) {
		sg_apply_pipeline(draw->pipeline);
		*cur_pipeline = draw->pipeline.id;
		apply_bindings = true;
	}

	if (apply_bindings) {
		sg_apply_bindings(binds);
		apply_uniforms = true;
	}

	// Apply bindings
	if (draw->uniform_idx != *cur_uniform) {
		*cur_uniform = draw->uniform_idx;
		apply_uniforms = true;
	}

	if (apply_uniforms && *cur_uniform != UINT32_MAX) {
		VT_UniformData *uniform = &render->uniforms[*cur_uniform];
		if (uniform->stage != SG_SHADERSTAGE_NONE && uniform->size > 0) {
			sg_range uniform_range = { uniform->ptr, uniform->size };
			sg_apply_uniforms(uniform->stage, &uniform_range);
		}
	}

	sg_draw(draw->vertex_idx - render->state._base_vertex, draw->vertex_count, 1);
}

void vt_render_flush(VT_Renderer *render) {
	assert(render != nullptr && render->cur_state > 0);

	u32 end_vertex = render->cur_vertex;
	u32 end_cmd = render->cur_command;

	// Rewind indexes
	render->cur_vertex = render->state._base_vertex;
	render->cur_command = render->state._base_command;

	// Skip drawing if any error happened
	if (render->has_error) {
		return;
	}

	// Check there's some drawing to do
	if (end_cmd <= render->cur_command) {
		return;
	}

	sg_range vertex_range = {
		.ptr = &render->vertices[render->state._base_vertex],
		.size = (end_vertex - render->state._base_vertex) * sizeof(VT_Vertex),
	};
	u32 offset = sg_append_buffer(render->vertex_buf, &vertex_range);
	if (sg_query_buffer_overflow(render->vertex_buf)) {
		LOG_WARN("[RENDER] > Vertex buffer overflowed");
		render->has_error = true;
		return;
	}

	sg_bindings binds = {
		.vertex_buffers[0] = render->vertex_buf,
		.vertex_buffer_offsets[0] = offset,
	};

	u32 cur_pipeline = SG_INVALID_ID;
	u32 cur_uniform = UINT32_MAX;

	// Begin state render pass before drawing
	sg_begin_pass(&render->state.render_pass);

	// Flush all commands
	for (u32 i = render->state._base_command; i < end_cmd; i += 1) {
		VT_RenderCommand *cmd = &render->commands[i];

		switch (cmd->type) {
		case _VT_COMMANDTYPE_VIEWPORT: {
			irect *viewport = &cmd->viewport;
			sg_apply_viewport(viewport->x, viewport->y, viewport->w, viewport->h, true);
		} break;
		case _VT_COMMANDTYPE_SCISSOR: {
			irect *scissor = &cmd->scissor;
			sg_apply_scissor_rect(scissor->x, scissor->y, scissor->w, scissor->h, true);
		} break;
		case _VT_COMMANDTYPE_DRAW: {
			VT_DrawCall *draw = &cmd->draw;
			if (draw->vertex_count == 0) {
				continue; // Ignore empty drawcalls
			}

			_vt_flush_draw(render, draw, &binds, &cur_pipeline, &cur_uniform);
		} break;
		}
	}
	sg_end_pass();
}
