#include "gfx/RenderBatcher.hpp"

#include "core/Window.hpp"
#include "gfx/Drawable.hpp"
#include "log.hpp"

#include <cstring>

using namespace vt;

bool RenderBatcher::init(u32 max_vertices, u32 max_commands) {
	m_vertices.resize(max_vertices > 0 ? max_vertices : _DEFAULT_MAX_VERTICES);
	m_commands.resize(max_commands > 0 ? max_commands : _DEFAULT_MAX_COMMANDS);

	sg_buffer_desc bufdesc;
	bufdesc.size = m_vertices.capacity() * sizeof(Vertex);
	bufdesc.type = SG_BUFFERTYPE_VERTEXBUFFER;
	bufdesc.usage = SG_USAGE_STREAM;
	bufdesc.label = "vt_render_batcher.vertex_buffer";

	m_vertex_buf = sg_make_buffer(bufdesc);
	if (sg_query_buffer_state(m_vertex_buf) != SG_RESOURCESTATE_VALID) {
		vt::log::error("[GFX] | RenderBatcher > Failed to make vertex buffer handler");
		return false;
	}

	m_is_valid = true;
	return true;
}

void RenderBatcher::terminate() {
	if (sg_query_buffer_state(m_vertex_buf) != SG_RESOURCESTATE_INVALID) {
		sg_destroy_buffer(m_vertex_buf);
	}
}

void RenderBatcher::draw(const Drawable& drawable) {
	assert(m_is_valid);

	if (drawable.m_vertices.empty()) {
		return;
	}

	u32 vertex_idx = m_cur_vertex;
	u32 vertex_count = drawable.m_vertices.size();
	auto vertices = _get_vertices(vertex_count);
	if (vertices.empty()) {
		return;
	}

	const Matrix& model = drawable.get_matrix();
	const Matrix& view = m_state.view.get_transform();
	Matrix mvp = m_state.proj * view * model;

	Rect region { FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX };

	for (u32 i = 0; i < vertex_count; i += 1) {
		const auto& vertex = drawable.m_vertices[i];

		Vec3 position = mvp * vertex.position;
		vertices[i].position = position;
		vertices[i].color = vertex.color;
		vertices[i].texcoord = vertex.texcoord;

		// Update region area the rendering takes
		region.x1 = std::min(region.x1, vertices[i].position.x);
		region.y1 = std::min(region.y1, vertices[i].position.y);
		region.x2 = std::max(region.x2, vertices[i].position.x);
		region.y2 = std::max(region.y2, vertices[i].position.y);
	}

	DrawCommand draw;
	draw.region = region;
	draw.textures = drawable.m_textures;
	draw.vertex_idx = vertex_idx;
	draw.vertex_count = vertex_count;

	// Override pipeline if state has set one
	if (m_state.pipeline.id != SG_INVALID_ID) {
		draw.pipeline = m_state.pipeline;
		draw.uniform = m_state.uniform;
	} else {
		draw.pipeline = vt::make_pipeline(drawable.m_primitive);
		draw.uniform = UniformBuffer {};
	}

	if (sg_query_pipeline_state(draw.pipeline) != SG_RESOURCESTATE_VALID) {
		m_cur_vertex -= vertex_count; // Rewind vertices
		return;
	}

	// Try to merge command with any previous command
	if (drawable.m_primitive != SG_PRIMITIVETYPE_LINE_STRIP
		&& drawable.m_primitive != SG_PRIMITIVETYPE_TRIANGLE_STRIP
		&& _try_merge_command(draw)) {
		return; // Succefully merged
	}

	BatchCommand *cmd = _next_command();
	if (!cmd) {
		m_cur_vertex -= vertex_count; // Rewind vertices
		return;
	}

	std::memset(cmd, 0, sizeof(BatchCommand));
	cmd->type = BatchCommandType::Draw;
	cmd->args.draw = draw;
}

void RenderBatcher::set_target(const Window& window) {
	assert(m_is_valid);

	m_state.framesize = window.get_size();
	const ContextSettings& settings = window.get_context_settings();

	m_cur_pass.swapchain.width = m_state.framesize.w;
	m_cur_pass.swapchain.height = m_state.framesize.h;
	m_cur_pass.swapchain.sample_count = settings.samples;
	m_cur_pass.swapchain.color_format = settings.pixel_format;
	m_cur_pass.swapchain.depth_format = settings.depth_format;
	m_cur_pass.attachments.id = SG_INVALID_ID;

	m_state.view = View {};
	m_state.proj = Matrix::ortho(0.0, m_state.framesize.w, m_state.framesize.h, 0.0);
	apply_viewport(0.0, 0.0, m_state.framesize.w, m_state.framesize.h);
	apply_scissor(0.0, 0.0, -1.0, -1.0);
}

void RenderBatcher::set_target(const sg_attachments& attachments) {
	assert(m_is_valid);
	assert(attachments.id != SG_INVALID_ID);

	m_cur_pass.attachments = attachments;
	m_cur_pass.swapchain = sg_swapchain {};
}

void RenderBatcher::apply_view(const View& view) {
	assert(m_is_valid);
	m_state.view = view;
}

void RenderBatcher::apply_viewport(f32 x, f32 y, f32 w, f32 h) {
	assert(m_is_valid);

	Rect viewport { x, y, w, h };

	// Skip if nothing has changed
	if (m_state.viewport == viewport) {
		return;
	}

	// Try to reuse previous command
	auto *cmd = _prev_command(1);
	if (!cmd || cmd->type != BatchCommandType::Viewport) {
		cmd = _next_command();
		if (!cmd) {
			return;
		}
	}

	std::memset(cmd, 0, sizeof(BatchCommand));
	cmd->type = BatchCommandType::Viewport;
	cmd->args.viewport = viewport;

	// Adjust state scissor offset relative to the new viewport
	if (m_state.scissor.w > 0 && m_state.scissor.h > 0) {
		m_state.scissor.x += viewport.x - m_state.viewport.x;
		m_state.scissor.y += viewport.y - m_state.viewport.y;
	}

	m_state.viewport = viewport;

	// Reset projection
	m_state.proj = Matrix::ortho(0.0, viewport.w, viewport.h, 0.0);
}

void RenderBatcher::apply_scissor(f32 x, f32 y, f32 w, f32 h) {
	assert(m_is_valid);

	Rect scissor { x, y, w, h };

	// Skip if nothing has changed
	if (m_state.scissor == scissor) {
		return;
	}

	// Try to reuse previous command
	auto *cmd = _prev_command(1);
	if (!cmd || cmd->type != BatchCommandType::Scissor) {
		cmd = _next_command();
		if (!cmd) {
			return;
		}
	}

	// Adjust scissor offset relative to the current viewport
	scissor.x += m_state.viewport.x;
	scissor.y += m_state.viewport.y;

	// Reset scissor if invalid
	if (w < 0.0 && h < 0.0) {
		scissor = Rect { 0.0, 0.0, (f32)m_state.framesize.w, (f32)m_state.framesize.h };
	}

	std::memset(cmd, 0, sizeof(BatchCommand));
	cmd->type = BatchCommandType::Scissor;
	cmd->args.scissor = scissor;

	m_state.scissor = scissor;
}

void RenderBatcher::reset() {
	m_state.view = View {};
	m_state.proj = Matrix::ortho(0.0, m_state.framesize.w, m_state.framesize.h, 0.0);
	m_state.pipeline.id = SG_INVALID_ID;
	m_state.uniform = UniformBuffer {};

	apply_viewport(0.0, 0.0, m_state.framesize.w, m_state.framesize.h);
	apply_scissor(0.0, 0.0, -1.0, -1.0);
}

void RenderBatcher::flush() {
	assert(m_is_valid);

	u32 vertex_count = m_cur_vertex;
	u32 command_count = m_cur_command;

	// Rewind indexes
	m_cur_vertex = 0;
	m_cur_command = 0;
	m_cur_uniform = 0;

	// Check if there's any command in this state
	if (command_count == 0) {
		return;
	}

	sg_range vertices_range = {
		.ptr = m_vertices.data(),
		.size = vertex_count * sizeof(Vertex),
	};
	u32 offset = sg_append_buffer(m_vertex_buf, vertices_range);
	if (sg_query_buffer_overflow(m_vertex_buf)) {
		vt::log::error("[GFX] | RenderBatcher > Vertex buffer overflow");
		return;
	}

	sg_pipeline cur_pipeline {};
	sg_image cur_images[MAX_TEXTURES_SLOTS] = {};
	UniformBuffer cur_uniform_buffer {
		.offset = 0,
		.size = 0,
	};

	sg_bindings binds {};
	binds.vertex_buffers[0] = m_vertex_buf;
	binds.vertex_buffer_offsets[0] = offset;

	sg_begin_pass(m_cur_pass);
	auto commands = std::span(m_commands.begin(), command_count);
	for (const auto& cmd : commands) {
		switch (cmd.type) {
		case BatchCommandType::Viewport: {
			const Rect& viewport = cmd.args.viewport;
			sg_apply_viewport(viewport.x, viewport.y, viewport.w, viewport.h, true);
		} break;

		case BatchCommandType::Scissor: {
			const Rect& scissor = cmd.args.scissor;
			sg_apply_scissor_rect(scissor.x, scissor.y, scissor.w, scissor.h, true);
		} break;

		case BatchCommandType::Draw: {
			DrawCommand draw = cmd.args.draw;
			if (draw.vertex_count == 0) {
				continue; // Nothing to do
			}

			bool apply_bindings = false;
			bool apply_uniforms = false;

			if (draw.pipeline.id != cur_pipeline.id) {
				sg_apply_pipeline(draw.pipeline);
				cur_pipeline = draw.pipeline;
				apply_bindings = true;
			}

			u32 slot = 0;
			for (const auto& texture : draw.textures) {
				sg_image img = texture.img;
				sg_sampler smp {};
				if (texture.img.id != SG_INVALID_ID) {
					smp = texture.smp;
				}

				if (cur_images[slot].id != img.id) {
					cur_images[slot] = img;
					binds.images[slot] = img;
					binds.samplers[slot] = smp;
					apply_bindings = true;
				}

				slot += 1;
			}

			if (apply_bindings) {
				sg_apply_bindings(binds);
				apply_uniforms = true;
			}

			if (cur_uniform_buffer != draw.uniform) {
				cur_uniform_buffer = draw.uniform;
				apply_uniforms = true;
			}

			if (apply_uniforms) {
				const auto& uniform = draw.uniform;

				// Vertex uniform
				if (draw.uniform.size > 0) {
					sg_range range = {
						.ptr = m_uniform_buffer.data() + uniform.offset,
						.size = uniform.size,
					};
					sg_apply_uniforms(0, range);
				}
			}

			sg_draw(draw.vertex_idx, draw.vertex_count, 1);
		} break;

		case BatchCommandType::None: break; // Command was merged
		}
	}

	sg_end_pass();
	reset();
}

bool RenderBatcher::_try_merge_command(const RenderBatcher::DrawCommand& draw) {
	BatchCommand *prev_cmd = nullptr;
	std::vector<BatchCommand *> inter_cmds;

	u32 search_depth = _BATCH_MERGE_DEPTH;
	for (u32 depth = 0; depth < search_depth; depth += 1) {
		BatchCommand *cmd = _prev_command(depth + 1);
		if (!cmd) {
			break;
		}

		// Check if command was already merged
		if (cmd->type == BatchCommandType::None) {
			search_depth += 1; // Search even deeper
			continue;
		}

		// Stop if command isn't a drawing command
		if (cmd->type != BatchCommandType::Draw) {
			break;
		}

		// Check if command has the same data
		if (cmd->args.draw.pipeline.id == draw.pipeline.id
			&& cmd->args.draw.uniform == draw.uniform
			&& cmd->args.draw.textures == draw.textures) {
			prev_cmd = cmd;
			break;
		}

		inter_cmds.push_back(cmd);
	}

	if (!prev_cmd) {
		return false; // Failed to merge
	}

	// Only allow merging if current or previous draw command doesn't touch
	// any intermediate commands
	bool overlaps_next = false;
	bool overlaps_prev = false;
	Rect prev_region = prev_cmd->args.draw.region;

	for (const auto& cmd : inter_cmds) {
		const Rect& region = cmd->args.draw.region;
		if (region.intersects(draw.region)) {
			overlaps_next = true;
			if (overlaps_prev) {
				return false; // Cannot overlap with previous region
			}
		}

		if (prev_region.intersects(draw.region)) {
			overlaps_prev = true;
			if (overlaps_next) {
				return false; // Cannot overlap with next region
			}
		}
	}

	if (!overlaps_next) { // Merge with previous command
		if (!inter_cmds.empty()) {
			// Check if there's enough vertices space
			if (m_cur_vertex + draw.vertex_count >= m_vertices.capacity()) {
				return false;
			}

			u32 end_vertex = prev_cmd->args.draw.vertex_idx
						   + prev_cmd->args.draw.vertex_count;
			u32 vertex_count = m_cur_vertex - end_vertex;

			// Avoid moving large chunks of memory
			if (vertex_count > _MAX_MOVE_VERTICES) {
				return false;
			}

			// Insert vertices after previous command vertices
			// NOTE: Don't use `std::vector.insert()` since we don't want the
			// vector's size to change
			std::memmove(
				&m_vertices[end_vertex + vertex_count], &m_vertices[end_vertex],
				vertex_count * sizeof(Vertex)
			);
			std::memcpy(
				&m_vertices[end_vertex], &m_vertices[draw.vertex_idx + draw.vertex_count],
				draw.vertex_count * sizeof(Vertex)
			);

			// Offset indexes from intermediate commands
			for (auto& cmd : inter_cmds) {
				cmd->args.draw.vertex_idx += draw.vertex_count;
			}
		}

		// Update render region
		prev_region.x1 = std::min(prev_region.x1, draw.region.x1);
		prev_region.y1 = std::min(prev_region.y1, draw.region.y1);
		prev_region.x2 = std::max(prev_region.x2, draw.region.x2);
		prev_region.y2 = std::max(prev_region.y2, draw.region.y2);

		prev_cmd->args.draw.vertex_count += draw.vertex_count;
		prev_cmd->args.draw.region = prev_region;
	} else { // Merge with next command
		assert(!inter_cmds.empty());

		// Push a new command
		BatchCommand *cmd = _next_command();
		if (!cmd) {
			return false;
		}

		u32 vertex_count = prev_cmd->args.draw.vertex_count;

		// Check if there's enough vertices space
		if (m_cur_vertex + vertex_count >= m_vertices.capacity()) {
			return false;
		}

		// Avoid moving large chunks of memory
		if (vertex_count > _MAX_MOVE_VERTICES) {
			return false;
		}

		// Merge previous and current vertices
		// NOTE: Don't use `std::vector.insert()` since we don't want the
		// vector's size to change
		std::memmove(
			&m_vertices[draw.vertex_idx + vertex_count], &m_vertices[draw.vertex_idx],
			vertex_count * sizeof(Vertex)
		);
		std::memcpy(
			&m_vertices[draw.vertex_idx], &m_vertices[prev_cmd->args.draw.vertex_idx],
			vertex_count * sizeof(Vertex)
		);

		m_cur_vertex += draw.vertex_count;

		// Update render region
		prev_region.x1 = std::min(prev_region.x1, draw.region.x1);
		prev_region.y1 = std::min(prev_region.y1, draw.region.y1);
		prev_region.x2 = std::max(prev_region.x2, draw.region.x2);
		prev_region.y2 = std::max(prev_region.y2, draw.region.y2);

		// Setup new command
		std::memset(cmd, 0, sizeof(BatchCommand));
		cmd->type = BatchCommandType::Draw;
		cmd->args.draw.pipeline = draw.pipeline;
		cmd->args.draw.region = prev_region;
		cmd->args.draw.vertex_idx = draw.vertex_idx;
		cmd->args.draw.vertex_count = draw.vertex_count + vertex_count;

		prev_cmd->type = BatchCommandType::None; // Skip previous command
	}

	return true;
}

std::span<Vertex> RenderBatcher::_get_vertices(u32 count) {
	if (m_cur_vertex + count >= m_vertices.capacity()) {
		vt::log::error("[GFX] | RenderBatcher > Vertex buffer overflow");
		return std::span<Vertex> {};
	}

	std::span<Vertex> vertices { m_vertices.begin() + m_cur_vertex, count };
	m_cur_vertex += count;
	return vertices;
}

RenderBatcher::BatchCommand *RenderBatcher::_next_command() {
	if (m_cur_command >= m_commands.capacity()) {
		vt::log::error("[GFX] | RenderBatcher > Commands array is full");
		return nullptr;
	}

	auto *cmd = &m_commands[m_cur_command];
	m_cur_command += 1;
	return cmd;
}

RenderBatcher::BatchCommand *RenderBatcher::_prev_command(u32 depth) {
	if (m_cur_command < depth) {
		return nullptr;
	}

	return &m_commands[m_cur_command - depth];
}
