#ifndef _VT_MATH_TRANSFORM_HPP
#define _VT_MATH_TRANSFORM_HPP

#include "types.hpp"

#include <cglm/mat4.h>
#include <cglm/types.h>

#ifdef VT_COMPILER_CLANG
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#	pragma clang diagnostic ignored "-Wnested-anon-types"
#endif

namespace vt {

struct [[nodiscard]] Transform {
	union {
		mat4 raw = GLM_MAT4_IDENTITY_INIT;

		struct {
			f32 m00, m01, m02, m03;
			f32 m10, m11, m12, m13;
			f32 m20, m21, m22, m23;
			f32 m30, m31, m32, m33;
		};
	};

	Transform() = default;
	Transform(const f32 *m) {
		glm_mat4_make(m, raw);
	}

	static Transform ortho(
		f32 left, f32 right, f32 bottom, f32 top, f32 near_z, f32 far_z
	);

	Transform operator*(const Transform& other) const;

	constexpr Transform& operator*=(const Transform& other) {
		glm_mat4_mul(raw, const_cast<vec4 *>(other.raw), raw);
		return *this;
	}

	constexpr bool operator==(const Transform& other) const {
		return raw[0][0] == other.raw[0][0] && raw[1][1] == other.raw[1][1] //
			&& raw[2][2] == other.raw[2][2] && raw[3][3] == other.raw[3][3];
	}
};

} // namespace vt

#endif
