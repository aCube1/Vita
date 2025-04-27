#include "gfx/Drawable.hpp"

#include "log.hpp"

using namespace vt::gfx;

Drawable::Drawable(sg_primitive_type primitive, const std::span<Vertex>& vertices) {
	m_primitive = primitive;
	m_vertices.assign(vertices.begin(), vertices.end());
	m_textures[0] = make_common_texture();
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
