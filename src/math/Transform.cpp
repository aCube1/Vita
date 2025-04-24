#include "math/Transform.hpp"

#include <cglm/affine.h>
#include <cglm/cam.h>

using namespace vt;

Transform Transform::ortho(
	f32 left, f32 right, f32 bottom, f32 top, f32 near_z, f32 far_z
) {
	mat4 ortho;
	glm_ortho(left, right, bottom, top, near_z, far_z, ortho);

	return Transform { (f32 *)ortho };
}

void Transform::translate(const Vec3& offset) {
	glm_translate(raw, (f32 *)offset.raw);
}

void Transform::scale(const Vec2& scale) {
	glm_scale(raw, vec3 { scale.x, scale.y, 1.0 });
}

void Transform::scale(f32 scale) {
	glm_scale_uni(raw, scale);
}

void Transform::rotate(f32 angle) {
	// Rotate around the Z axis
	glm_rotate(raw, angle, vec3 { 0.0, 0.0, 1.0 });
}

void Transform::rotate_at(const Vec2& pivot, f32 angle) {
	// Rotate at pivot around the Z axis
	glm_rotate_at(raw, vec3 { pivot.x, pivot.y, 0.0 }, angle, vec3 { 0.0, 0.0, 1.0 });
}

Transform Transform::operator*(const Transform& other) const {
	mat4 out;
	glm_mat4_mul(const_cast<vec4 *>(raw), const_cast<vec4 *>(other.raw), out);

	return Transform { (f32 *)out };
}

Vec3 Transform::operator*(const Vec3& other) const {
	vec3 out;
	glm_mat4_mulv3(const_cast<vec4 *>(raw), const_cast<f32 *>(other.raw), 1.0, out);

	return Vec3 { (f32 *)out };
}
