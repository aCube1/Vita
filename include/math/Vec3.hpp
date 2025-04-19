#ifndef _VT_MATH_VEC3_HPP
#define _VT_MATH_VEC3_HPP

#include "math/Vec2.hpp"
#include "types.hpp"

#include <cglm/types.h>
#include <cglm/vec3.h>

#ifdef VT_COMPILER_CLANG
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#	pragma clang diagnostic ignored "-Wnested-anon-types"
#endif

namespace vt {

struct [[nodiscard]] Vec3 {
	static const Vec3 One;	 // x: 1.0 | y: 1.0 | z: 1.0
	static const Vec3 Up;	 // x: 0.0 | y:-1.0 | z: 0.0
	static const Vec3 Down;	 // x: 0.0 | y: 1.0 | z: 0.0
	static const Vec3 Left;	 // x:-1.0 | y: 0.0 | z: 0.0
	static const Vec3 Right; // x: 1.0 | y: 0.0 | z: 0.0
	static const Vec3 Front; // x: 0.0 | y: 0.0 | z: 1.0
	static const Vec3 Back;	 // x: 0.0 | y: 0.0 | z:-1.0

	union {
		vec3 raw = { 0.0, 0.0, 0.0 };

		struct {
			f32 x;
			f32 y;
			f32 z;
		};
	};

	Vec3() = default;
	Vec3(f32 fill) : x { fill }, y { fill }, z { fill } { }
	Vec3(f32 x_, f32 y_, f32 z_) : x { x_ }, y { y_ }, z { z_ } { }
	Vec3(Vec2 vec, f32 z_ = 0.0) : x { vec.x }, y { vec.y }, z { z_ } { }

	Vec3(const f32 *v) {
		raw[0] = v[0];
		raw[1] = v[1];
		raw[2] = v[2];
	}

	Vec3 clamp(const Vec3& max, const Vec3& min) const;
	Vec3 clamp(f32 min, f32 max) const;
	Vec3 cross(const Vec3& other) const;
	f32 distance_to(const Vec3& other) const;
	f32 dot(const Vec3& other) const;
	Vec3 lerp(const Vec3& to, f32 weight) const;
	Vec3 max(const Vec3& other) const;
	Vec3 max(f32 max) const;
	Vec3 min(const Vec3& other) const;
	Vec3 min(f32 min) const;
	Vec3 normalized() const;

	Vec3 operator-() const;
	Vec3 operator+(const Vec3& other) const;
	Vec3 operator-(const Vec3& other) const;
	Vec3 operator*(const Vec3& other) const;
	Vec3 operator/(const Vec3& other) const;

	constexpr Vec3& operator+=(const Vec3& other);
	constexpr Vec3& operator-=(const Vec3& other);
	constexpr Vec3& operator*=(const Vec3& other);
	constexpr Vec3& operator/=(const Vec3& other);
	constexpr bool operator==(const Vec3& other) const;

	template <typename T>
	Vec3 operator+(T scalar) const;

	template <typename T>
	Vec3 operator-(T scalar) const;

	template <typename T>
	Vec3 operator*(T scalar) const;

	template <typename T>
	Vec3 operator/(T scalar) const;

	template <typename T>
	constexpr Vec3& operator+=(T scalar);

	template <typename T>
	constexpr Vec3& operator-=(T scalar);

	template <typename T>
	constexpr Vec3& operator*=(T scalar);

	template <typename T>
	constexpr Vec3& operator/=(T scalar);
};

} // namespace vt

#include "Vec3_impl.hpp"

#endif
