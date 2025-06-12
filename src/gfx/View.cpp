#include "gfx/View.hpp"

#include <cglm/affine2d.h>

using namespace vt;

View& View::translate(const Vec2& offset) {
	m_center += offset;
	m_update_transform = true;

	return *this;
}

View& View::rotate(f32 angle) {
	m_rotation += angle;
	m_update_transform = true;

	return *this;
}

View& View::zoom(f32 factor) {
	m_zoom *= factor;
	m_update_transform = true;

	return *this;
}

void View::set_center(const Vec2& center) {
	m_center = center;
	m_update_transform = true;
}

void View::set_rotation(f32 angle) {
	m_rotation = angle;
	m_update_transform = true;
}

void View::set_zoom(f32 scale) {
	m_zoom = scale;
	m_update_transform = true;
}

[[nodiscard]] const Vec2& View::get_center() const {
	return m_center;
}

[[nodiscard]] f32 View::get_rotation() const {
	return m_rotation;
}

[[nodiscard]] f32 View::get_zoom() const {
	return m_zoom;
}

[[nodiscard]] const Mat4& View::get_transform() const {
	if (!m_update_transform) {
		return m_transform;
	}

	// Transformation: O * R * S * (-O) -> M
	mat3 m = GLM_MAT3_IDENTITY_INIT;

	auto center = m_center;
	glm_translate2d(m, center.raw);
	glm_rotate2d(m, m_rotation);
	glm_scale2d_uni(m, m_zoom);

	center = -m_center;
	glm_translate2d(m, center.raw);

	m_transform = Mat4 {};
	glm_mat4_ins3(m, m_transform.raw);

	m_update_transform = false;
	return m_transform;
}
