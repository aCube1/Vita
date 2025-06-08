#include "math/Mat4.hpp"

#include <cglm/affine.h>
#include <cglm/cam.h>

using namespace vt;

Mat4 Mat4::ortho(f32 left, f32 right, f32 bottom, f32 top, f32 near_z, f32 far_z) {
	mat4 ortho;
	glm_ortho(left, right, bottom, top, near_z, far_z, ortho);

	return Mat4 { (f32 *)ortho };
}

Mat4 Mat4::operator*(const Mat4& other) const {
	mat4 out;
	glm_mat4_mul(const_cast<vec4 *>(raw), const_cast<vec4 *>(other.raw), out);

	return Mat4 { (f32 *)out };
}

Vec3 Mat4::operator*(const Vec3& other) const {
	vec3 out;
	glm_mat4_mulv3(const_cast<vec4 *>(raw), const_cast<f32 *>(other.raw), 1.0, out);

	return Vec3 { (f32 *)out };
}
