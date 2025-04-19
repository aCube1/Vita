#include "math/Transform.hpp"

#include <cglm/cam.h>

using namespace vt;

Transform Transform::ortho(
	f32 left, f32 right, f32 bottom, f32 top, f32 near_z, f32 far_z
) {
	mat4 ortho;
	glm_ortho(left, right, bottom, top, near_z, far_z, ortho);

	return Transform { (f32 *)ortho };
}

Transform Transform::operator*(const Transform& other) const {
	mat4 out;
	glm_mat4_mul(const_cast<vec4 *>(raw), const_cast<vec4 *>(other.raw), out);

	return Transform { (float *)out };
}
