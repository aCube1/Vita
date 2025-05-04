#include "math/Matrix.hpp"

#include <cglm/affine.h>
#include <cglm/cam.h>

using namespace vt;

Matrix Matrix::ortho(f32 left, f32 right, f32 bottom, f32 top, f32 near_z, f32 far_z) {
	mat4 ortho;
	glm_ortho(left, right, bottom, top, near_z, far_z, ortho);

	return Matrix { (f32 *)ortho };
}

Matrix Matrix::operator*(const Matrix& other) const {
	mat4 out;
	glm_mat4_mul(const_cast<vec4 *>(raw), const_cast<vec4 *>(other.raw), out);

	return Matrix { (f32 *)out };
}

Vec3 Matrix::operator*(const Vec3& other) const {
	vec3 out;
	glm_mat4_mulv3(const_cast<vec4 *>(raw), const_cast<f32 *>(other.raw), 1.0, out);

	return Vec3 { (f32 *)out };
}
