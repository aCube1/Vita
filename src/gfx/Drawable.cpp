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

void Drawable::translate(const Vec3& offset) {
	m_position += offset;
	m_update_transform = true;
}

void Drawable::rotate(f32 angle) {
	m_rotation += angle;
	m_update_transform = true;
}

void Drawable::scale(const Vec2& factor) {
	m_scale *= factor;
	m_update_transform = true;
}

void Drawable::set_texture(u32 slot, const Texture& texture) {
	if (slot >= MAX_TEXTURES_SLOTS) {
		vt::log::warn("[GFX] | Drawable > Cannot assign texture to slot: {}", slot);
		return;
	}

	m_textures[slot] = texture;
}

void Drawable::set_origin(const Vec2& origin) {
	m_origin = origin;
	m_update_transform = true;
}

void Drawable::set_position(const Vec3& position) {
	m_position = position;
	m_update_transform = true;
}

void Drawable::set_angle(f32 angle) {
	m_rotation = angle;
	m_update_transform = true;
}

void Drawable::set_scale(const Vec2& scale) {
	m_scale = scale;
	m_update_transform = true;
}

[[nodiscard]] const vt::Vec2& Drawable::get_origin() const {
	return m_origin;
}

[[nodiscard]] const vt::Vec3& Drawable::get_position() const {
	return m_position;
}

[[nodiscard]] f32 Drawable::get_rotation() const {
	return m_rotation;
}

[[nodiscard]] const vt::Vec2& Drawable::get_scale() const {
	return m_scale;
}

[[nodiscard]] const vt::Transform& Drawable::get_transform() const {
	if (!m_update_transform) {
		return m_transform;
	}

	// Transformation: T + R + S -> M
	m_transform = Transform {};
	m_transform.translate(m_position - m_origin);
	m_transform.rotate(m_rotation);
	m_transform.translate(m_position);
	m_transform.scale(m_scale);

	m_update_transform = false;
	return m_transform;
}
