#include "math/Transform.hpp"

#include "cglm/affine.h"

using namespace vt;

Transform& Transform::translate(const Vec3& offset) {
	m_position += offset;
	m_update_transform = true;

	return *this;
}

Transform& Transform::scale(const Vec2& factor) {
	m_scale *= factor;
	m_update_transform = true;

	return *this;
}

Transform& Transform::rotate(f32 angle) {
	m_rotation += angle;
	m_update_transform = true;

	return *this;
}

void Transform::set_origin(const Vec2& origin) {
	m_origin = origin;
	m_update_transform = true;
}

void Transform::set_position(const Vec3& position) {
	m_position = position;
	m_update_transform = true;
}

void Transform::set_rotation(f32 angle) {
	m_rotation = angle;
	m_update_transform = true;
}

void Transform::set_scale(const Vec2& scale) {
	m_scale = scale;
	m_update_transform = true;
}

[[nodiscard]] const vt::Vec2& Transform::get_origin() const {
	return m_origin;
}

[[nodiscard]] const vt::Vec3& Transform::get_position() const {
	return m_position;
}

[[nodiscard]] f32 Transform::get_rotation() const {
	return m_rotation;
}

[[nodiscard]] const vt::Vec2& Transform::get_scale() const {
	return m_scale;
}

[[nodiscard]] const vt::Matrix& Transform::get_matrix() const {
	if (!m_update_transform) {
		return m_transform;
	}

	// Transformation: T + R + S -> M
	mat4 m = GLM_MAT4_IDENTITY_INIT;
	glm_translate(m, (f32 *)m_position.raw);
	glm_rotate_at(
		m, vec3 { m_origin.x, m_origin.y, 0.0 }, m_rotation, vec3 { 0.0, 0.0, 1.0 }
	);
	glm_scale(m, vec3 { m_scale.x, m_scale.y, 1.0 });

	m_update_transform = false;
	m_transform = Matrix { (f32 *)m };
	return m_transform;
}
