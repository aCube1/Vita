// Code adapted from:
// https://github.com/edubart/sokol_gp/blob/master/sokol_gp.h

#include "gfx/renderer.h"

#include "assert.h"
#include "cglm/struct/cam.h"
#include "cglm/struct/mat4.h"
#include "log.h"

#include <stdlib.h>
#include <string.h>
#include <utils.h>

#define _VT_UNIFORMBLOCKS_SIZE (sizeof(vt_uniform_block) * VT_GFX_MAX_UNIFORM_SLOTS)

enum {
	_VT_DEFAULT_MAX_VERTICES = 65536,
	_VT_DEFAULT_MAX_COMMANDS = 16384,
	_VT_DEFAULT_MIN_UNIFORMBUFFER_SIZE = 4 * 1024 * 1024,
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

typedef struct _vt_texture_bind {
	sg_image img;
	sg_sampler smp;
} _vt_texture_bind;

typedef struct _vt_draw_command {
	sg_pipeline pipeline;
	_vt_texture_bind textures[VT_GFX_MAX_TEXTURE_SLOTS];
	vt_uniform_block uniforms[VT_GFX_MAX_UNIFORM_SLOTS];
	frect region; // Screen region to be drawing to
	u32 vertex_count;
	u32 vertex_idx;
} _vt_draw_command;

typedef struct _vt_render_command {
	u32 type;

	union {
		irect viewport;
		irect scissor;
		_vt_draw_command draw;
	};
} _vt_render_command;

typedef struct _vt_uniform_buffer {
	usize size;
	usize lenght;
	u8 *data;
} _vt_uniform_buffer;

struct vt_renderer {
	u32 cur_vertex;
	u32 cur_command;
	u32 vertices_count;
	u32 commands_count;
	vt_vertex *vertices;
	_vt_render_command *commands;
	_vt_uniform_buffer uniformbuf;

	u32 cur_state;
	u32 cur_transform;
	vt_render_state state_stack[_VT_MAX_STACK_DEPTH];
	mat4s transform_stack[_VT_MAX_STACK_DEPTH];

	sg_pipeline pipelines[_SG_PRIMITIVETYPE_NUM];
	sg_buffer vertex_buf;
	sg_shader common_shdr;
	sg_image white_img;
	sg_sampler nearest_smp;

	vt_render_state state;
};

static sg_pipeline
_vt_lookup_pipeline(vt_renderer *render, sg_primitive_type primitive_type) {
	u32 pip_idx = primitive_type;
	if (render->pipelines[pip_idx].id != SG_INVALID_ID) {
		return render->pipelines[pip_idx];
	}

	sg_shader shdr = vt_make_gfx_common_shader();

	sg_pipeline_desc pipdesc = vt_init_pipeline_desc(primitive_type, shdr);
	sg_pipeline pip = sg_make_pipeline(&pipdesc);
	if (sg_query_pipeline_state(pip) != SG_RESOURCESTATE_VALID) {
		sg_destroy_pipeline(pip);
		return (sg_pipeline) { SG_INVALID_ID };
	}

	render->pipelines[pip_idx] = pip;
	return pip;
}

vt_renderer *vt_create_renderer(void) {
	vt_renderer *render = calloc(1, sizeof(vt_renderer));
	if (!render) {
		LOG_FATAL("[GPU] > Renderer | Failed to alloc memory");
		return nullptr;
	}

	render->vertices_count = _VT_DEFAULT_MAX_VERTICES;
	render->commands_count = _VT_DEFAULT_MAX_COMMANDS;

	render->vertices = calloc(render->vertices_count, sizeof(vt_vertex));
	render->commands = calloc(render->commands_count, sizeof(_vt_render_command));
	if (!render->vertices || !render->commands) {
		LOG_FATAL("[GPU] > Renderer | Failed to alloc resources memory");
		vt_destroy_renderer(render);
		return nullptr;
	}

	// Check if all primitive pipelines are valid
	for (u32 i = 0; i < _SG_PRIMITIVETYPE_NUM; i += 1) {
		sg_pipeline pip = _vt_lookup_pipeline(render, i);
		if (pip.id == SG_INVALID_ID) {
			LOG_FATAL("[GPU] > Renderer | Failed to make primitive pipelines");
			vt_destroy_renderer(render);
			return nullptr;
		}
	}

	// Setup buffers
	sg_buffer_desc bufdesc = {
		.size = render->vertices_count * sizeof(vt_vertex),
		.type = SG_BUFFERTYPE_VERTEXBUFFER,
		.usage = SG_USAGE_STREAM,
		.label = "vt_renderer.vertex_buffer",
	};

	render->vertex_buf = sg_make_buffer(&bufdesc);
	if (sg_query_buffer_state(render->vertex_buf) != SG_RESOURCESTATE_VALID) {
		LOG_ERROR("[GPU] > Renderer | Unable to make vertex buffer");
		vt_destroy_renderer(render);
		return nullptr;
	}

	// Initialize uniform buffer
	render->uniformbuf.size = _VT_DEFAULT_MIN_UNIFORMBUFFER_SIZE;
	render->uniformbuf.lenght = 0;
	render->uniformbuf.data = malloc(render->uniformbuf.size * sizeof(u8));
	if (!render->uniformbuf.data) {
		LOG_FATAL("[GPU] > Renderer | Failed initialize uniforms buffer");
		vt_destroy_renderer(render);
		return nullptr;
	}

	// First element of the transform stack must be a matrix identity
	render->transform_stack[1] = GLMS_MAT4_IDENTITY;

	return render;
}

void vt_destroy_renderer(vt_renderer *render) {
	if (!render) {
		return;
	}

	for (u32 i = 0; i < _SG_PRIMITIVETYPE_NUM; i += 1) {
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

	if (render->uniformbuf.data) {
		free(render->uniformbuf.data);
	}

	free(render);
}

void vt_render_begin(vt_renderer *render, const sg_pass *pass) {
	assert(render && pass);

	if (render->cur_state >= _VT_MAX_STACK_DEPTH) {
		LOG_FATAL("[GPU] > Renderer | State stack overflow");
		return;
	}

	// Store current state
	render->state_stack[render->cur_state] = render->state;
	render->cur_state += 1;

	// Setup new state
	vt_clear_mem(render->state.uniforms, _VT_UNIFORMBLOCKS_SIZE);
	vt_clear_mem(render->state.textures, sizeof(vt_texture) * VT_GFX_MAX_TEXTURE_SLOTS);

	// Query render target size to fill default rendering area
	render->state.pass = *pass;
	ivec2s framesize;
	if (pass->attachments.id != SG_INVALID_ID) {
		sg_attachments_desc attachments = sg_query_attachments_desc(pass->attachments);
		framesize.x = sg_query_image_width(attachments.colors[0].image);
		framesize.y = sg_query_image_height(attachments.colors[0].image);
	} else {
		framesize.x = pass->swapchain.width;
		framesize.y = pass->swapchain.height;
	}
	render->state.framesize = framesize;

	mat4s proj = glms_ortho(0.0, framesize.x, framesize.y, 0.0, -1.0, 1.0);
	render->state.projection = proj;
	render->state.viewport = (irect) { { 0, 0, framesize.x, framesize.y } };
	render->state.scissor = render->state.viewport;
	render->state.transform = GLMS_MAT4_IDENTITY;
	render->state.color = VT_COLOR_WHITE;
	render->state.pipeline.id = SG_INVALID_ID;
	render->state._base_vertex = render->cur_vertex;
	render->state._base_command = render->cur_command;
	render->state._base_command = render->uniformbuf.lenght;

	vt_make_default_texture(
		&render->state.textures[0].img, &render->state.textures[0].smp
	);
}

void vt_render_end(vt_renderer *render) {
	assert(render);

	if (render->cur_state <= 0) {
		LOG_FATAL("[GPU] > Renderer | State stack underflow");
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
	render->uniformbuf.lenght = render->state._base_uniform;

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
		LOG_FATAL("[RENDER] > Vertex buffer overflowed");
		return;
	}

	// TODO: Add texture support
	sg_bindings binds = {
		.vertex_buffers[0] = render->vertex_buf,
		.vertex_buffer_offsets[0] = offset,
	};

	u32 cur_pipeline = SG_INVALID_ID;
	u32 cur_imgs[VT_GFX_MAX_TEXTURE_SLOTS] = {};

	// Begin current state pass
	sg_begin_pass(&render->state.pass);

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
			_vt_draw_command *draw = &cmd->draw;
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
			for (u32 slot = 0; slot < VT_GFX_MAX_TEXTURE_SLOTS; slot += 1) {
				sg_image img = draw->textures[slot].img;
				sg_sampler smp = { SG_INVALID_ID };
				if (img.id != SG_INVALID_ID) {
					smp = draw->textures[slot].smp;
				}

				// Reapply bindings when image changes
				if (cur_imgs[slot] != img.id) {
					cur_imgs[slot] = img.id;
					binds.images[slot] = img;
					binds.samplers[slot] = smp;
					apply_bindings = true;
				}
			}

			if (apply_bindings) {
				sg_apply_bindings(&binds);
				apply_uniforms = true;
			}

			// Apply all uniforms
			if (apply_uniforms) {
				for (i32 ubslot = 0; ubslot < VT_GFX_MAX_UNIFORMBLOCKS_BINDS;
					 ubslot += 1) {
					vt_uniform_block *block = &render->state.uniforms[ubslot];
					if (block->size > 0) {
						sg_range data_range = {
							render->uniformbuf.data + block->offset,
							block->size,
						};
						sg_apply_uniforms(ubslot, &data_range);
					}
				}
			}

			sg_draw(draw->vertex_idx - render->state._base_vertex, draw->vertex_count, 1);
		}
	}

	sg_end_pass();
}

static vt_vertex *_vt_get_vertices(vt_renderer *render, u32 count) {
	if (render->cur_vertex + count > render->vertices_count) {
		LOG_ERROR("[GPU] > Renderer | Vertices buffer overflow");
		return nullptr;
	}

	vt_vertex *vertices = &render->vertices[render->cur_vertex];
	render->cur_vertex += count;
	return vertices;
}

static _vt_render_command *_vt_get_prev_command(vt_renderer *render, u32 count) {
	// Don't underflow command queue
	if (render->cur_command < count) {
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
	// return !(ra.x2 <= rb.x1 || rb.x2 <= ra.x1 || ra.y2 <= rb.y1 || rb.y2 <=
	// ra.y1);
	return ra.x1 <= rb.x2 && rb.x1 <= ra.x2 && ra.y1 <= rb.y2 && rb.y2 <= ra.y1;
}

static bool _vt_try_merge_commands(vt_renderer *render, const _vt_draw_command *merge) {
	_vt_render_command *prev_cmd = nullptr;
	_vt_render_command *inter_cmds[_VT_BATCH_MERGING_DEPTH] = {};
	u32 inter_cmd_count = 0;

	u32 search_depth = _VT_BATCH_MERGING_DEPTH;
	for (u32 depth = 0; depth < search_depth; depth += 1) {
		_vt_render_command *cmd = _vt_get_prev_command(render, depth + 1);
		if (!cmd) {
			break;
		}

		// Check if command was merged
		if (cmd->type == _VT_COMMANDTYPE_NONE) {
			search_depth += 1; // Search deeper
			continue;
		}

		// Don't proceed if the command isn't a drawing command
		if (cmd->type != _VT_COMMANDTYPE_DRAW) {
			break;
		}

		// NOTE: We can compare the pointers since they point to the same memory
		// object
		if (cmd->draw.pipeline.id == merge->pipeline.id
			&& !memcmp(
				merge->textures, cmd->draw.textures,
				sizeof(vt_texture) * VT_GFX_MAX_TEXTURE_SLOTS
			)
			&& !memcmp(merge->uniforms, cmd->draw.uniforms, _VT_UNIFORMBLOCKS_SIZE)) {
			prev_cmd = cmd;
			break;
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
		if (_vt_do_region_overlap(merge->region, inter_region)) {
			overlaps_next = true;
			if (overlaps_prev) {
				return false;
			}
		}

		if (_vt_do_region_overlap(prev_region, merge->region)) {
			overlaps_prev = true;
			if (overlaps_next) {
				return false;
			}
		}
	}

	if (!overlaps_next) { // Merge with the previous draw command
		if (inter_cmd_count > 0) {
			// Check if there's enough vertices space
			if (render->cur_vertex + merge->vertex_count > render->vertices_count) {
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
				&render->vertices[merge->vertex_idx + merge->vertex_count],
				merge->vertex_count * sizeof(vt_vertex)
			);

			// Offset vertices index of intermediate commands
			for (u32 i = 0; i < inter_cmd_count; i += 1) {
				inter_cmds[i]->draw.vertex_idx += merge->vertex_count;
			}
		}

		// Update render region and vertices
		prev_region.x1 = VT_MIN(prev_region.x1, merge->region.x1);
		prev_region.y1 = VT_MIN(prev_region.y1, merge->region.y1);
		prev_region.x2 = VT_MAX(prev_region.x2, merge->region.x2);
		prev_region.y2 = VT_MAX(prev_region.y2, merge->region.y2);
		prev_cmd->draw.vertex_count += merge->vertex_count;
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
			&render->vertices[merge->vertex_idx + prev_vertex_count],
			&render->vertices[merge->vertex_idx], merge->vertex_count * sizeof(vt_vertex)
		);
		memcpy(
			&render->vertices[merge->vertex_idx],
			&render->vertices[prev_cmd->draw.vertex_idx],
			prev_vertex_count * sizeof(vt_vertex)
		);

		// Update render region and vertices
		prev_region.x1 = VT_MIN(prev_region.x1, merge->region.x1);
		prev_region.y1 = VT_MIN(prev_region.y1, merge->region.y1);
		prev_region.x2 = VT_MAX(prev_region.x2, merge->region.x2);
		prev_region.y2 = VT_MAX(prev_region.y2, merge->region.y2);
		render->cur_vertex += prev_vertex_count;

		// Setup new draw command
		cmd->type = _VT_COMMANDTYPE_DRAW;
		cmd->draw.pipeline = merge->pipeline;
		cmd->draw.region = prev_region;
		cmd->draw.vertex_idx = merge->vertex_idx;
		cmd->draw.vertex_count = merge->vertex_count + prev_vertex_count;

		memcpy(
			cmd->draw.textures, prev_cmd->draw.textures,
			sizeof(vt_texture) * VT_GFX_MAX_TEXTURE_SLOTS
		);
		memcpy(cmd->draw.uniforms, prev_cmd->draw.uniforms, _VT_UNIFORMBLOCKS_SIZE);

		// Skip previous draw command
		prev_cmd->type = _VT_COMMANDTYPE_NONE;
	}

	return true;
}

void vt_render_geometry(vt_renderer *render, const vt_render_geometry_desc *desc) {
	assert(render);
	assert(render->cur_state > 0);
	assert(desc && desc->vertices);

	if (desc->vertex_count == 0) {
		return;
	}

	u32 vertex_idx = render->cur_vertex;
	vt_vertex *verts = _vt_get_vertices(render, desc->vertex_count);
	if (!verts) {
		return;
	}

	// Calculate ModelViewProjection matrix
	mat4s mvp = glms_mat4_mul(render->state.projection, render->state.transform);

	// Copy vertices to memory
	frect region = { { FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX } };
	for (u32 i = 0; i < desc->vertex_count; i += 1) {
		const vt_vertex *vertex = &desc->vertices[i];
		vec3s position = glms_mat4_mulv3(mvp, vertex->position, 1.0);

		// Copy data
		verts[i].position = position;
		verts[i].texcoord = vertex->texcoord;
		verts[i].color = vertex->color;

		// Update screen region the render takes
		region.x1 = VT_MIN(region.x1, position.x);
		region.y1 = VT_MIN(region.y1, position.y);
		region.x2 = VT_MAX(region.x2, position.x);
		region.y2 = VT_MAX(region.y2, position.y);
	}

	// Override pipeline if the state has set one
	sg_pipeline pipeline;
	const vt_uniform_block *uniforms = nullptr;
	if (render->state.pipeline.id != SG_INVALID_ID) {
		pipeline = render->state.pipeline;
		uniforms = render->state.uniforms;
	} else {
		pipeline = _vt_lookup_pipeline(render, desc->primitive);
	}

	if (sg_query_pipeline_state(pipeline) != SG_RESOURCESTATE_VALID) {
		render->cur_vertex -= desc->vertex_count; // Rewind vertices
		return;
	}

	_vt_draw_command draw_cmd = {
		.pipeline = pipeline,
		.region = region,
		.vertex_idx = vertex_idx,
		.vertex_count = desc->vertex_count,
	};

	memcpy(
		draw_cmd.textures, render->state.textures,
		sizeof(vt_texture) * VT_GFX_MAX_TEXTURE_SLOTS
	);
	if (uniforms) {
		memcpy(draw_cmd.uniforms, uniforms, _VT_UNIFORMBLOCKS_SIZE);
	}

	// Try to merge current command with the previous ones
	if (desc->primitive != SG_PRIMITIVETYPE_LINE_STRIP
		&& desc->primitive != SG_PRIMITIVETYPE_TRIANGLE_STRIP
		&& _vt_try_merge_commands(render, &draw_cmd)) {
		return;
	}

	// Register new draw command
	_vt_render_command *cmd = _vt_get_next_command(render);
	if (!cmd) {
		render->cur_vertex -= desc->vertex_count; // Rewind vertices
		return;
	}
	cmd->type = _VT_COMMANDTYPE_DRAW;
	cmd->draw = draw_cmd;
}

void vt_push_render_transform(vt_renderer *render, mat3s transform) {
	assert(render);
	assert(render->cur_state > 0);

	if (render->cur_transform >= _VT_MAX_STACK_DEPTH) {
		LOG_FATAL("[GPU] > Renderer | Transform stack overflow");
		return;
	}

	float new_transform[] = {
		transform.m00, transform.m01, transform.m02, 0.0,
		transform.m10, transform.m11, transform.m12, 0.0,
		transform.m20, transform.m21, transform.m22, 0.0,
		0.0,		   0.0,			  0.0,			 1.0,
	};

	render->state.transform = glms_mat4_make(new_transform);
	render->transform_stack[render->cur_transform] = render->state.transform;
	render->cur_transform += 1;
}

void vt_pop_render_transform(vt_renderer *render) {
	assert(render);
	assert(render->cur_state > 0);

	if (render->cur_transform >= _VT_MAX_STACK_DEPTH) {
		LOG_FATAL("[GPU] > Renderer | Transform stack underflow");
		return;
	}

	render->state.transform = render->transform_stack[render->cur_transform];
	render->cur_transform -= 1;
}

void vt_render_projection(vt_renderer *render, mat4s projection) {
	assert(render);
	assert(render->cur_state > 0);

	render->state.projection = projection;
}

void vt_render_viewport(vt_renderer *render, irect viewport) {
	assert(render);
	assert(render->cur_state > 0);

	// Skip if nothing was changed
	if (render->state.viewport.x == viewport.x && render->state.viewport.y == viewport.y
		&& render->state.viewport.w == viewport.w
		&& render->state.viewport.h == viewport.h) {
		return;
	}

	// Try to reuse previous command
	_vt_render_command *cmd = _vt_get_prev_command(render, 1);
	if (!cmd || cmd->type != _VT_COMMANDTYPE_VIEWPORT) {
		cmd = _vt_get_next_command(render);
		if (!cmd) {
			return;
		}
	}
	vt_clear_mem(cmd, sizeof(_vt_render_command));
	cmd->type = _VT_COMMANDTYPE_VIEWPORT;
	cmd->viewport = viewport;

	// Adjust current scissor offset relative to the viewport
	if (render->state.scissor.w > 0 && render->state.scissor.h > 0) {
		render->state.scissor.x += viewport.x - render->state.viewport.x;
		render->state.scissor.y += viewport.y - render->state.viewport.y;
	}

	render->state.viewport = viewport;
	// Reset projection
	mat4s projection = glms_ortho(0.0, viewport.x, viewport.y, 0.0, 1.0, 1.0);
	render->state.projection = projection;
}

void vt_render_scissor(vt_renderer *render, irect scissor) {
	assert(render);
	assert(render->cur_state > 0);

	// Skip if nothing was changed
	if (render->state.scissor.x == scissor.x && render->state.scissor.y == scissor.y
		&& render->state.scissor.w == scissor.w && render->state.scissor.h == scissor.h) {
		return;
	}

	// Try to reuse previous command
	_vt_render_command *cmd = _vt_get_prev_command(render, 1);
	if (!cmd || cmd->type != _VT_COMMANDTYPE_SCISSOR) {
		cmd = _vt_get_next_command(render);
		if (!cmd) {
			return;
		}
	}

	// Adjust scissor offset relative to the current viewport
	irect subscissor = {
		.x = render->state.viewport.x + scissor.x,
		.y = render->state.viewport.y + scissor.y,
		.w = scissor.w,
		.h = scissor.h,
	};

	// Reset scissor if it has an invalid subspace
	if (scissor.w < 0 && scissor.h < 0) {
		subscissor = (irect) {
			.x = 0, .y = 0, .w = render->state.framesize.x, .h = render->state.framesize.y
		};
	}

	vt_clear_mem(cmd, sizeof(_vt_render_command));
	cmd->type = _VT_COMMANDTYPE_SCISSOR;
	cmd->scissor = subscissor;

	render->state.scissor = scissor;
}

void vt_set_render_pipeline(vt_renderer *render, sg_pipeline pipeline) {
	assert(render);
	assert(render->cur_state > 0);

	render->state.pipeline = pipeline;

	// Reset uniform slots
	memset(render->state.uniforms, 0, _VT_UNIFORMBLOCKS_SIZE);
}

void vt_set_render_image(vt_renderer *render, i32 slot, sg_image img) {
	assert(render);
	assert(render->cur_state > 0);
	assert(slot >= 0 && slot <= VT_GFX_MAX_TEXTURE_SLOTS);

	render->state.textures[slot].img = img;
}

void vt_set_render_sample(vt_renderer *render, i32 slot, sg_sampler smp) {
	assert(render);
	assert(render->cur_state > 0);
	assert(slot >= 0 && slot <= VT_GFX_MAX_TEXTURE_SLOTS);

	render->state.textures[slot].smp = smp;
}

void vt_set_render_uniform(vt_renderer *render, i32 ubslot, const sg_range *data) {
	assert(render);
	assert(render->cur_state > 0);
	assert(ubslot >= 0 && ubslot <= VT_GFX_MAX_UNIFORMBLOCKS_BINDS);

	// Disable slot if data is empty
	if (!data || !data->ptr || data->size == 0) {
		vt_clear_mem(&render->state.uniforms[ubslot], sizeof(vt_uniform_block));
		return;
	}

	// If uniforms memory overflow, alloc a bigger memory block
	if (render->uniformbuf.lenght + data->size >= render->uniformbuf.size) {
		render->uniformbuf.size *= 2;
		void *tmp = realloc(render->uniformbuf.data, render->uniformbuf.size);
		if (!tmp) {
			abort(); // This should be unreachable
		}
		render->uniformbuf.data = tmp;
	}

	// Append uniform memory
	vt_uniform_block block = { .offset = render->uniformbuf.lenght, .size = data->size };
	memcpy(render->uniformbuf.data + block.offset, data->ptr, data->size);
	render->uniformbuf.lenght += data->size;

	// Register block on current state
	render->state.uniforms[ubslot] = block;
}

void vt_set_render_color(vt_renderer *render, u8color color) {
	assert(render);
	assert(render->cur_state > 0);

	render->state.color = color;
}

void vt_query_render_state(vt_renderer *render, vt_render_state *state) {
	assert(render);
	assert(render->cur_state > 0);
	assert(state);

	memcpy(state, &render->state, sizeof(vt_render_state));
}
