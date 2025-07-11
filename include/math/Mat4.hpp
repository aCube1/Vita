#ifndef _VT_MATH_MAT4_HPP
#define _VT_MATH_MAT4_HPP

#include "math/Vec3.hpp"
#include "types.hpp"
#include "utils.hpp"

#include <cglm/mat4.h>
#include <cglm/types.h>

namespace vt {

struct [[nodiscard]] Mat4 {
	union {
		mat4 raw = GLM_MAT4_IDENTITY_INIT;

		struct {
			f32 m00, m01, m02, m03;
			f32 m10, m11, m12, m13;
			f32 m20, m21, m22, m23;
			f32 m30, m31, m32, m33;
		};
	};

	Mat4() = default;
	Mat4(const f32 *m) {
		glm_mat4_make(m, raw);
	}

	static Mat4 ortho(
		f32 left, f32 right, f32 bottom, f32 top, f32 near_z = -1.0, f32 far_z = 1.0
	);

	Mat4 operator*(const Mat4& other) const;
	Vec3 operator*(const Vec3& other) const;

	Mat4& operator*=(const Mat4& other) {
		glm_mat4_mul(raw, const_cast<vec4 *>(other.raw), raw);
		return *this;
	}

	constexpr bool operator==(const Mat4& other) const {
		return raw[0][0] == other.raw[0][0] && raw[1][1] == other.raw[1][1] //
			&& raw[2][2] == other.raw[2][2] && raw[3][3] == other.raw[3][3];
	}
};

} // namespace vt

#endif
