#ifndef _VT_GFX_DRAWABLE_HPP
#define _VT_GFX_DRAWABLE_HPP

#include "gfx/common.hpp"
#include "math/Rect.hpp"
#include "math/Transform.hpp"

#include <span>
#include <vector>

namespace vt::gfx {

enum class DrawMode : u8 {
	ModeFill = 0,
	ModeLines,
};

class Drawable : public Transform {
public:
	Drawable() = default;
	Drawable(sg_primitive_type primitive, const std::span<Vertex>& vertices = {});

	static Drawable make_rect(
		DrawMode mode, f32 x, f32 y, f32 w, f32 h, const Color& color = Color::White
	);
	static Drawable make_rect(
		DrawMode mode, const Rect& rect, const Color& color = Color::White
	);

	void append_vertices(const std::span<Vertex>& vertices);

	void set_texture(u32 slot, const Texture& texture);

private:
	sg_primitive_type m_primitive = SG_PRIMITIVETYPE_TRIANGLES;
	std::vector<Vertex> m_vertices;
	TexturesUniform m_textures;

	friend class RenderBatcher;
};

} // namespace vt::gfx

#endif
