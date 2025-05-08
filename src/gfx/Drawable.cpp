#include "gfx/Drawable.hpp"

#include "log.hpp"

#include <utility>

using namespace vt::gfx;

Drawable::Drawable(sg_primitive_type primitive, const std::span<Vertex>& vertices) {
	m_primitive = primitive;
	m_vertices.assign(vertices.begin(), vertices.end());
	m_textures[0] = make_common_texture();
}

Drawable Drawable::make_rect(
	DrawMode mode, f32 x, f32 y, f32 w, f32 h, const Color& color
) {
	static Vec2 quad_uv[4] = {
		Vec2(0.0, 0.0), // Top Left
		Vec2(1.0, 0.0), // Top Right
		Vec2(1.0, 1.0), // Bottom Right
		Vec2(0.0, 1.0), // Bottom Left
	};

	vt::Vec3 quad[4] = {
		Vec3(0.0, 0.0, 0.0), // Top Left
		Vec3(w, 0.0, 0.0),	 // Top Right
		Vec3(w, h, 0.0),	 // Bottom Right
		Vec3(0.0, h, 0.0),	 // Bottom Left
	};

	std::vector<vt::gfx::Vertex> vertices {
		Vertex(quad[0], quad_uv[0], color), // Top Left
		Vertex(quad[1], quad_uv[1], color), // Top Right
		Vertex(quad[2], quad_uv[2], color), // Bottom Right

		Vertex(quad[0], quad_uv[0], color), // Top Left
		Vertex(quad[2], quad_uv[2], color), // Bottom Right
		Vertex(quad[3], quad_uv[3], color), // Bottom Left
	};

	sg_primitive_type primitive;
	switch (mode) {
	case DrawMode::ModeFill:  primitive = SG_PRIMITIVETYPE_TRIANGLES; break;
	case DrawMode::ModeLines: primitive = SG_PRIMITIVETYPE_LINES; break;
	default:				  assert(false);
	}

	Drawable drawable { primitive, vertices };
	drawable.set_origin({ w / 2, h / 2 });
	drawable.set_position({ x, y, 0.0 });

	return drawable;
}

Drawable Drawable::make_rect(DrawMode mode, const Rect& rect, const Color& color) {
	return make_rect(mode, rect.x, rect.y, rect.w, rect.h, color);
}

void Drawable::append_vertices(const std::span<Vertex>& vertices) {
	if (vertices.empty()) {
		return;
	}

	m_vertices.insert(m_vertices.cend(), vertices.begin(), vertices.end());
}

void Drawable::set_texture(u32 slot, const Texture& texture) {
	if (slot >= MAX_TEXTURES_SLOTS) {
		vt::log::warn("[GFX] | Drawable > Cannot assign texture to slot: {}", slot);
		return;
	}

	m_textures[slot] = texture;
}
