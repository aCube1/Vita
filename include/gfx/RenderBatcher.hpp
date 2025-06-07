#ifndef _VT_GFX_RENDERBATCHER_HPP
#define _VT_GFX_RENDERBATCHER_HPP

#include "gfx/Color.hpp"
#include "gfx/Drawable.hpp"
#include "gfx/View.hpp"
#include "gfx/common.hpp"
#include "math/Matrix.hpp"
#include "math/Rect.hpp"
#include "math/Vec2i.hpp"

#include <sokol/sokol_gfx.h>
#include <span>
#include <stack>
#include <vector>

namespace vt {

struct UniformBuffer {
	u32 offset;
	u16 size;

	constexpr bool operator==(const UniformBuffer& other) const {
		return offset == other.offset && size == other.size;
	}
};

struct BatchState {
	Point framesize;
	Rect viewport;
	Rect scissor;
	Matrix proj;
	View view;
	sg_pipeline pipeline;
	UniformBuffer uniform;

private:
	u32 _base_vertex;
	u32 _base_command;
	u32 _base_uniform;

	friend class RenderBatcher;
};

class RenderBatcher {
public:
	RenderBatcher() = default;

	RenderBatcher(const RenderBatcher&) = delete;
	RenderBatcher& operator=(const RenderBatcher&) = delete;

	bool init(u32 max_vertices = 0, u32 max_commands = 0);
	void terminate();

	// Render pass manipulation
	void begin_frame(const sg_pass& pass);
	void end_frame();

	// Drawing state manipulation
	void begin(const View& view);
	void end();
	void flush();

	void draw(const Drawable& drawable);

	void set_pipeline(sg_pipeline pip);
	void set_uniform(i32 bindslot, const void *data, usize size);

	// State view manipulation
	void apply_viewport(f32 x, f32 y, f32 w, f32 h);
	void apply_scissor(f32 x, f32 y, f32 w, f32 h);

private:
	enum {
		_DEFAULT_MAX_VERTICES = 65536,
		_DEFAULT_MAX_COMMANDS = 16384,
		_MAX_MOVE_VERTICES = 512,
		_MAX_STACK_DEPTH = 64,
		_BATCH_MERGE_DEPTH = 8,
	};

	enum BatchCommandType : u8 {
		None = 0,
		Viewport,
		Scissor,
		Draw,
	};

	struct DrawCommand {
		sg_pipeline pipeline;
		Rect region;
		UniformBuffer uniform;
		TexturesUniform textures;
		u32 vertex_idx;
		u32 vertex_count;
	};

	struct BatchCommand {
		BatchCommandType type;
		union {
			Rect viewport;
			Rect scissor;
			DrawCommand draw;
		} args;
	};

	struct RenderPass {
		bool in_pass = false;
		Point framesize;
	};

	bool m_is_valid = false;
	RenderPass m_cur_pass {};
	BatchState m_state {};
	sg_buffer m_vertex_buf;

	u32 m_cur_vertex {};
	u32 m_cur_command {};
	u32 m_cur_uniform {};
	std::vector<Vertex> m_vertices;
	std::vector<BatchCommand> m_commands;
	std::vector<u8> m_uniform_buffer;

	std::stack<BatchState> m_state_stack;

	bool _try_merge_command(const DrawCommand& draw);

	std::span<Vertex> _get_vertices(u32 count);
	BatchCommand *_next_command();
	BatchCommand *_prev_command(u32 depth);
};

} // namespace vt

#endif
