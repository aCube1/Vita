#include "math/Transform.hpp"

#include "cglm/affine2d.h"

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

[[nodiscard]] const vt::Mat4& Transform::get_matrix() const {
	if (!m_update_transform) {
		return m_transform;
	}

	Vec2 position { m_position.x, m_position.y };
	auto origin = -m_origin;
	auto scale = m_scale;

	// Transformation: T * O * R * S * (-O) -> M
	mat3 m = GLM_MAT3_IDENTITY_INIT;
	glm_translate2d(m, position.raw);
	glm_translate2d(m, origin.raw);
	glm_rotate2d(m, m_rotation);
	glm_scale2d(m, scale.raw);

	origin = -m_origin;
	glm_translate2d(m, origin.raw);

	m_transform = Mat4 {};
	glm_mat4_ins3(m, m_transform.raw);

	m_update_transform = false;
	return m_transform;
}
