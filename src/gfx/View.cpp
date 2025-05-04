#include "gfx/View.hpp"

#include <cglm/affine.h>

using namespace vt::gfx;
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

[[nodiscard]] const Matrix& View::get_matrix() const {
	if (!m_update_transform) {
		return m_transform;
	}

	// Transformation: O * R * S * (-O) -> M
	mat4 m = GLM_MAT4_IDENTITY_INIT;
	glm_translate(m, vec3 { m_center.x, m_center.y, 0.0 });
	glm_rotate_z(m, m_rotation, m);
	glm_scale_uni(m, m_zoom);
	glm_translate(m, vec3 { -m_center.x, -m_center.y, 0.0 });

	m_transform = Matrix { (f32 *)m };
	m_update_transform = false;
	return m_transform;
}
