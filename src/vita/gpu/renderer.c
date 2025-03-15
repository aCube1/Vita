// Code adapted from:
// https://github.com/edubart/sokol_gp/blob/master/sokol_gp.h

#include "vita/gpu/renderer.h"

#include "cglm/struct/cam.h"
#include "cglm/struct/mat3.h"
#include "cglm/struct/mat4.h"
#include "log.h"
#include <stdlib.h>
#include <string.h>

enum {
	_VT_DEFAULT_MAX_VERTICES = 65536,
	_VT_DEFAULT_MAX_COMMANDS = 16384,
	_VT_MAX_MOVE_VERTICES = 128,
	_VT_MAX_STACK_DEPTH = 64,
	_VT_BATCH_MERGING_DEPTH = 8,
};

enum {
	_VT_COMMANDTYPE_NONE = 0,
	_VT_COMMANDTYPE_VIEWPORT,
	_VT_COMMANDTYPE_SCISSOR,
	_VT_COMMANDTYPE_DRAW,
};

typedef struct _vt_drawcall {
	sg_pipeline pipeline;
	vt_uniform uniform_blocks[SG_MAX_UNIFORMBLOCK_BINDSLOTS];
	frect region; // Screen region to be drawing to
	u32 vertex_count;
	u32 vertex_idx;
} _vt_drawcall;

typedef struct _vt_render_command {
	u32 type;

	union {
		irect viewport;
		irect scissor;
		_vt_drawcall draw;
	};
} _vt_render_command;

struct vt_renderer {
	vt_error error;
	u32 cur_vertex;
	u32 cur_command;
	u32 cur_uniform;
	u32 vertices_count;
	u32 commands_count;
	u32 uniforms_count;
	vt_vertex *vertices;
	_vt_render_command *commands;
	vt_uniforms_pool uniforms;

	u32 cur_state;
	u32 cur_transform;
	vt_batchstate state_stack[_VT_MAX_STACK_DEPTH];
	mat4s transform_stack[_VT_MAX_STACK_DEPTH];

	vt_batchstate state;
	sg_pipeline pipelines[_VT_PRIMITIVETYPE_COUNT];
	sg_buffer vertex_buf;
};

static sg_pipeline _vt_lookup_pipeline(
	vt_renderer *render, vt_primitive_type primitive_type
) {
	u32 pip_idx = primitive_type;
	if (render->pipelines[pip_idx].id != SG_INVALID_ID) {
		return render->pipelines[pip_idx];
	}

	sg_pipeline pip = vt_make_shader_pipeline(vt_get_common_shader(), primitive_type);
	if (pip.id != SG_INVALID_ID) {
		render->pipelines[pip_idx] = pip;
	}

	return pip;
}

vt_renderer *vt_create_renderer(void) {
	vt_renderer *render = calloc(1, sizeof(vt_renderer));
	if (!render) {
		LOG_ERROR("[RENDER] > Failed to alloc memory");
		return nullptr;
	}
	render->error = VT_ERROR_NONE;

	render->vertices_count = _VT_DEFAULT_MAX_VERTICES;
	render->commands_count = _VT_DEFAULT_MAX_COMMANDS;
	render->uniforms_count = _VT_DEFAULT_MAX_COMMANDS;

	render->vertices = calloc(render->vertices_count, sizeof(vt_vertex));
	render->commands = calloc(render->commands_count, sizeof(_vt_render_command));
	if (!render->vertices || !render->commands) {
		LOG_ERROR("[RENDER] > Failed to alloc resources memory");
		vt_destroy_renderer(render);
		return nullptr;
	}

	render->error = vt_init_uniforms_pool(
		&render->uniforms, render->commands_count * VT_UNIFORM_BINDSLOTS_COUNT
	);
	if (render->error != VT_ERROR_NONE) {
		LOG_ERROR("[RENDER] > Failed to initialize uniforms pool");
		vt_destroy_renderer(render);
		return nullptr;
	}

	// Check if all primitive pipelines are valid
	for (u32 i = 0; i < _VT_PRIMITIVETYPE_COUNT; i += 1) {
		sg_pipeline pip = _vt_lookup_pipeline(render, i);
		if (pip.id == SG_INVALID_ID) {
			LOG_ERROR("[RENDER] > Failed to make primitive pipelines");
			vt_destroy_renderer(render);
			return nullptr;
		}
	}

	// Setup buffers
	sg_buffer_desc bufdesc = {
		.size = render->vertices_count * sizeof(vt_vertex),
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

void vt_destroy_renderer(vt_renderer *render) {
	if (!render) {
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

	if (render->vertices) {
		free(render->vertices);
	}
	if (render->commands) {
		free(render->commands);
	}

	vt_destroy_uniforms_pool(&render->uniforms);
	free(render);
}

vt_error vt_get_render_error(vt_renderer *render) {
	assert(render);
	return render->error;
}

void vt_render_begin(vt_renderer *render, ivec2s framesize) {
	assert(render);

	if (render->cur_state >= _VT_MAX_STACK_DEPTH) {
		LOG_WARN("[RENDER] > State's stack has overflow");
		render->error = VT_ERROR_MEM_OVERFLOW;
		return;
	}

	// Reset error state
	render->error = VT_ERROR_NONE;

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
	render->state.mvp = render->state.projview;
	render->state.color = VT_COLOR_WHITE;
	render->state._base_vertex = render->cur_vertex;
	render->state._base_command = render->cur_command;
	render->state._base_uniform = render->cur_uniform;

	memset(
		&render->state.uniform_blocks, 0, sizeof(vt_uniform) * VT_UNIFORM_BINDSLOTS_COUNT
	);

	render->state.render_pass = (sg_pass) {
		.action.colors[0] = {
			.load_action = SG_LOADACTION_CLEAR,
			.store_action = SG_STOREACTION_STORE,
			.clear_value = { 0.25, 0.25, 0.25, 1.0 },
		},
		.swapchain = {
			.width = framesize.x,
			.height = framesize.y,
		},
		.label = "vt_render.default_pass",
	};
}

void vt_render_end(vt_renderer *render) {
	assert(render);

	if (render->cur_state <= 0) {
		LOG_WARN("[RENDER] > State's stack underflow");
		return;
	}

	// Restore previous state
	render->cur_state -= 1;
	render->state = render->state_stack[render->cur_state];
}

void vt_render_flush(vt_renderer *render) {
	assert(render && render->cur_state > 0);

	u32 end_vertex = render->cur_vertex;
	u32 end_cmd = render->cur_command;

	// Rewind indexes
	render->cur_vertex = render->state._base_vertex;
	render->cur_command = render->state._base_command;

	// Skip drawing if any error happened
	if (render->error != VT_ERROR_NONE) {
		return;
	}

	// Check there's some drawing to do
	if (end_cmd <= render->cur_command) {
		return;
	}

	sg_range vertex_range = {
		.ptr = &render->vertices[render->state._base_vertex],
		.size = (end_vertex - render->state._base_vertex) * sizeof(vt_vertex),
	};
	u32 offset = sg_append_buffer(render->vertex_buf, &vertex_range);
	if (sg_query_buffer_overflow(render->vertex_buf)) {
		LOG_WARN("[RENDER] > Vertex buffer overflowed");
		render->error = VT_ERROR_MEM_OVERFLOW;
		return;
	}

	// TODO: Add texture support
	sg_bindings binds = {
		.vertex_buffers[0] = render->vertex_buf,
		.vertex_buffer_offsets[0] = offset,
		.images[0] = vt_gpu_get_white_image(),
		.samplers[0] = vt_gpu_get_nearest_sampler(),
	};

	u32 cur_pipeline = SG_INVALID_ID;

	// Begin state render pass before drawing
	sg_begin_pass(&render->state.render_pass);

	// Flush all commands
	for (u32 i = render->state._base_command; i < end_cmd; i += 1) {
		_vt_render_command *cmd = &render->commands[i];

		if (cmd->type == _VT_COMMANDTYPE_VIEWPORT) {
			irect *viewport = &cmd->viewport;
			sg_apply_viewport(viewport->x, viewport->y, viewport->w, viewport->h, true);
		}

		if (cmd->type == _VT_COMMANDTYPE_SCISSOR) {
			irect *scissor = &cmd->scissor;
			sg_apply_scissor_rect(scissor->x, scissor->y, scissor->w, scissor->h, true);
		}

		if (cmd->type == _VT_COMMANDTYPE_DRAW) {
			_vt_drawcall *draw = &cmd->draw;
			if (draw->vertex_count == 0) {
				continue; // Ignore empty drawcalls
			}

			bool apply_bindings = false;
			bool apply_uniforms = false;

			// Reapply bindings if pipeline is different from current
			if (draw->pipeline.id != cur_pipeline) {
				sg_apply_pipeline(draw->pipeline);
				cur_pipeline = draw->pipeline.id;
				apply_bindings = true;
			}

			// Apply bindings
			if (apply_bindings) {
				sg_apply_bindings(&binds);
				apply_uniforms = true;
			}

			// Apply all uniforms
			if (apply_uniforms) {
				for (i32 i = 0; i < VT_UNIFORM_BINDSLOTS_COUNT; i += 1) {
					vt_apply_uniform(&render->uniforms, i, draw->uniform_blocks[i]);
				}
			}

			sg_draw(draw->vertex_idx - render->state._base_vertex, draw->vertex_count, 1);
		}
	}
	sg_end_pass();
	vt_reset_uniforms_pool(&render->uniforms);
}

void vt_set_render_pipeline(vt_renderer *render, sg_pipeline pip) {
	assert(render);
	render->state.pipeline = pip;

	// Reset uniforms
	memset(
		&render->state.uniform_blocks, 0, sizeof(vt_uniform) * VT_UNIFORM_BINDSLOTS_COUNT
	);
}

void vt_set_render_uniform(vt_renderer *render, i32 ubslot, const sg_range *data) {
	assert(render);

	// Reset slot if data is null
	if (!data) {
		render->state.uniform_blocks[ubslot].id = VT_INVALID_SLOT_ID;
		return;
	}

	vt_uniform uniform = vt_create_uniform(&render->uniforms, data);
	if (uniform.id == VT_INVALID_SLOT_ID) {
		LOG_ERROR("[RENDER] > Failed to set uniform on slot: %d", ubslot);
		render->error = VT_ERROR_GENERIC;
		return;
	}

	render->state.uniform_blocks[ubslot] = uniform;
}

static vt_vertex *_vt_get_vertices(vt_renderer *render, u32 count) {
	if (render->cur_vertex + count > render->vertices_count) {
		LOG_ERROR("[RENDER] > Vertices buffer overflowed");
		render->error = VT_ERROR_MEM_OVERFLOW;
		return nullptr;
	}

	vt_vertex *vertices = &render->vertices[render->cur_vertex];
	render->cur_vertex += count;
	return vertices;
}

static _vt_render_command *_vt_get_prev_command(vt_renderer *render, u32 count) {
	// Don't underflow command queue
	if (render->cur_command - render->state._base_command < count) {
		return nullptr;
	}

	return &render->commands[render->cur_command - count];
}

static _vt_render_command *_vt_get_next_command(vt_renderer *render) {
	if (render->cur_command == render->commands_count) {
		return nullptr;
	}

	_vt_render_command *cmd = &render->commands[render->cur_command];
	render->cur_command += 1;
	return cmd;
}

static inline bool _vt_do_region_overlap(frect ra, frect rb) {
	// return !(ra.x2 <= rb.x1 || rb.x2 <= ra.x1 || ra.y2 <= rb.y1 || rb.y2 <= ra.y1);
	return ra.x1 <= rb.x2 && rb.x1 <= ra.x2 && ra.y1 <= rb.y2 && rb.y2 <= ra.y1;
}

static bool _vt_try_merge_commands(
	vt_renderer *render,
	sg_pipeline pip,
	frect region,
	vt_uniform *uniform,
	u32 vertex_idx,
	u32 vertex_count
) {
	_vt_render_command *prev_cmd = nullptr;
	_vt_render_command *inter_cmds[_VT_BATCH_MERGING_DEPTH] = {};
	u32 inter_cmd_count = 0;

	u32 search_depth = _VT_BATCH_MERGING_DEPTH;
	for (u32 depth = 0; depth < search_depth; depth += 1) {
		_vt_render_command *cmd = _vt_get_prev_command(render, depth + 1);

		// Don't proceed if the command isn't a drawing command, or doesn't exist
		if (!cmd || cmd->type != _VT_COMMANDTYPE_DRAW) {
			break;
		}

		// Check if command was merged
		if (cmd->type == _VT_COMMANDTYPE_NONE) {
			search_depth += 1; // Search deeper
			continue;
		}

		if (cmd->draw.pipeline.id == pip.id || uniform) {
			vt_uniform *cmd_uniform = cmd->draw.uniform_blocks;
			bool is_uniform_eq = !memcmp(
				uniform, cmd_uniform, sizeof(vt_uniform) * VT_UNIFORM_BINDSLOTS_COUNT
			);

			if (is_uniform_eq) {
				prev_cmd = cmd;
				break;
			}
		}

		inter_cmds[inter_cmd_count] = cmd;
		inter_cmd_count += 1;
	}

	if (!prev_cmd) {
		return false; // Failed to merge command
	}

	// Only allow merging if current or previows draw is not touch by
	// any intermidiate command
	bool overlaps_next = false;
	bool overlaps_prev = false;
	frect prev_region = prev_cmd->draw.region;

	for (u32 i = 0; i < inter_cmd_count; i += 1) {
		frect inter_region = inter_cmds[i]->draw.region;
		if (_vt_do_region_overlap(region, inter_region)) {
			overlaps_next = true;
			if (overlaps_prev) {
				return false;
			}
		}

		if (_vt_do_region_overlap(prev_region, region)) {
			overlaps_prev = true;
			if (overlaps_next) {
				return false;
			}
		}
	}

	if (!overlaps_next) { // Merge with the previous draw command
		if (inter_cmd_count > 0) {
			// Check if there's enough vertices space
			if (render->cur_vertex + vertex_count > render->vertices_count) {
				return false;
			}

			u32 prev_end_vertex = prev_cmd->draw.vertex_idx + prev_cmd->draw.vertex_count;
			u32 prev_vertex_count = render->cur_vertex - prev_end_vertex;

			// Avoid moving too many vertices at once
			if (prev_vertex_count > _VT_MAX_MOVE_VERTICES) {
				return false;
			}

			// Insert vertices after previous vertices
			memmove(
				&render->vertices[prev_end_vertex + prev_vertex_count],
				&render->vertices[prev_end_vertex], prev_vertex_count * sizeof(vt_vertex)
			);
			memcpy(
				&render->vertices[prev_end_vertex],
				&render->vertices[vertex_idx + vertex_count],
				vertex_count * sizeof(vt_vertex)
			);

			// Offset vertices index of intermediate commands
			for (u32 i = 0; i < inter_cmd_count; i += 1) {
				inter_cmds[i]->draw.vertex_idx += vertex_count;
			}
		}

		// Update render region and vertices
		prev_region.x1 = VT_MIN(prev_region.x1, region.x1);
		prev_region.y1 = VT_MIN(prev_region.y1, region.y1);
		prev_region.x2 = VT_MAX(prev_region.x2, region.x2);
		prev_region.y2 = VT_MAX(prev_region.y2, region.y2);
		prev_cmd->draw.vertex_count += vertex_count;
		prev_cmd->draw.region = prev_region;
	} else { // Merge with the next draw command
		assert(inter_cmd_count > 0);

		// Append a new draw command
		_vt_render_command *cmd = _vt_get_next_command(render);
		if (!cmd) {
			return false;
		}
		u32 prev_vertex_count = prev_cmd->draw.vertex_count;

		// Check if there's enough vertices space
		if (render->cur_vertex + prev_vertex_count > render->vertices_count) {
			return false;
		}

		// Avoid moving too many vertices at once
		if (prev_vertex_count > _VT_MAX_MOVE_VERTICES) {
			return false;
		}

		// Batch previous and current vertices in the same memory region
		memmove(
			&render->vertices[vertex_idx + prev_vertex_count],
			&render->vertices[vertex_idx], vertex_count * sizeof(vt_vertex)
		);
		memcpy(
			&render->vertices[vertex_idx], &render->vertices[prev_cmd->draw.vertex_idx],
			prev_vertex_count * sizeof(vt_vertex)
		);

		// Update render region and vertices
		prev_region.x1 = VT_MIN(prev_region.x1, region.x1);
		prev_region.y1 = VT_MIN(prev_region.y1, region.y1);
		prev_region.x2 = VT_MAX(prev_region.x2, region.x2);
		prev_region.y2 = VT_MAX(prev_region.y2, region.y2);
		render->cur_vertex += prev_vertex_count;
		vertex_count += prev_vertex_count;

		// Setup new draw command
		cmd->type = _VT_COMMANDTYPE_DRAW;
		cmd->draw.pipeline = pip;
		cmd->draw.region = prev_region;
		cmd->draw.vertex_idx = vertex_idx;
		cmd->draw.vertex_count = vertex_count;
		memcpy(
			cmd->draw.uniform_blocks, prev_cmd->draw.uniform_blocks,
			sizeof(vt_uniform) * VT_UNIFORM_BINDSLOTS_COUNT
		);

		// Skip previous draw command
		prev_cmd->type = _VT_COMMANDTYPE_NONE;
	}

	return true;
}

static void _vt_queue_draw(
	vt_renderer *render,
	sg_pipeline pip,
	frect region,
	u32 vertex_idx,
	u32 vertex_count,
	vt_primitive_type primitive
) {
	// Override pipeline if the state has set one
	vt_uniform *uniforms = nullptr;
	if (render->state.pipeline.id != SG_INVALID_ID) {
		pip = render->state.pipeline;
		uniforms = render->state.uniform_blocks;
	}

	if (sg_query_pipeline_state(pip) != SG_RESOURCESTATE_VALID) {
		render->cur_vertex -= vertex_count; // Rewind vertices
		return;
	}

	// Try to merge current command with the previous ones
	if (primitive != VT_PRIMITIVETYPE_LINE_STRIP
		&& primitive != VT_PRIMITIVETYPE_TRIANGLE_STRIP
		&& _vt_try_merge_commands(
			render, pip, region, uniforms, vertex_idx, vertex_count
		)) {
		return;
	}

	// Register new draw command
	_vt_render_command *cmd = _vt_get_next_command(render);
	if (!cmd) {
		render->cur_vertex -= vertex_count; // Rewind vertices
		return;
	}
	cmd->type = _VT_COMMANDTYPE_DRAW;
	cmd->draw.region = region;
	cmd->draw.pipeline = pip;
	cmd->draw.vertex_count = vertex_count;
	cmd->draw.vertex_idx = vertex_idx;

	if (uniforms) {
		memcpy(
			cmd->draw.uniform_blocks, uniforms,
			sizeof(vt_uniform) * VT_UNIFORM_BINDSLOTS_COUNT
		);
	}
}

void vt_render_geometry(
	vt_renderer *render, vt_primitive_type primitive, const vt_vertex *vertices, u32 count
) {
	assert(render && vertices);
	assert(render->cur_state > 0);

	if (count == 0) {
		return;
	}

	u32 vertex_idx = render->cur_vertex;
	vt_vertex *v = _vt_get_vertices(render, count);
	if (!v) {
		return;
	}

	// Copy vertices to memory
	frect region = { { FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX } };
	mat4s mvp = render->state.mvp;
	for (u32 i = 0; i < count; i += 1) {
		vec3s pos = glms_mat4_mulv3(mvp, vertices[i].position, 1.0);
		v[i].position = pos;
		v[i].texcoord = vertices[i].texcoord;
		v[i].color = vertices[i].color;

		// Update screen region the render take
		region.x1 = VT_MIN(region.x1, pos.x);
		region.y1 = VT_MIN(region.y1, pos.y);
		region.x2 = VT_MAX(region.x2, pos.x);
		region.y2 = VT_MAX(region.y2, pos.y);
	}

	// Send draw command to the queue
	sg_pipeline pip = _vt_lookup_pipeline(render, primitive);
	_vt_queue_draw(render, pip, region, vertex_idx, count, primitive);
}
