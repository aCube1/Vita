#ifndef _VT_MATH_VEC2_HPP
#define _VT_MATH_VEC2_HPP

#include "types.hpp"
#include "utils.hpp"

#include <cglm/types.h>
#include <cglm/vec2.h>

#ifdef VT_COMPILER_CLANG
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#	pragma clang diagnostic ignored "-Wnested-anon-types"
#endif

namespace vt {

struct [[nodiscard]] Vec2 {
	static const Vec2 Zero;	 // x: 0.0 | y: 0.0
	static const Vec2 One;	 // x: 1.0 | y: 1.0
	static const Vec2 Up;	 // x: 0.0 | y:-1.0
	static const Vec2 Down;	 // x: 0.0 | y: 1.0
	static const Vec2 Left;	 // x:-1.0 | y: 0.0
	static const Vec2 Right; // x: 1.0 | y: 0.0

	union {
		vec2 raw = { 0.0, 0.0 };

		struct {
			f32 x;
			f32 y;
		};

		struct {
			f32 w;
			f32 h;
		};

		struct {
			f32 u;
			f32 v;
		};
	};

	Vec2() = default;
	Vec2(f32 fill) : x { fill }, y { fill } { }
	Vec2(f32 x_, f32 y_) : x { x_ }, y { y_ } { }

	Vec2(const f32 *v) {
		raw[0] = v[0];
		raw[1] = v[1];
	}

	Vec2 clamp(const Vec2& max, const Vec2& min) const;
	Vec2 clamp(f32 min, f32 max) const;
	f32 cross(const Vec2& other) const;
	f32 distance_to(const Vec2& other) const;
	f32 dot(const Vec2& other) const;
	f32 lenght() const;
	Vec2 lerp(const Vec2& to, f32 weight) const;
	Vec2 max(const Vec2& other) const;
	Vec2 max(f32 max) const;
	Vec2 min(const Vec2& other) const;
	Vec2 min(f32 min) const;
	Vec2 normalized() const;

	Vec2 operator-() const;
	Vec2 operator+(const Vec2& other) const;
	Vec2 operator-(const Vec2& other) const;
	Vec2 operator*(const Vec2& other) const;
	Vec2 operator/(const Vec2& other) const;

	constexpr Vec2& operator+=(const Vec2& other);
	constexpr Vec2& operator-=(const Vec2& other);
	constexpr Vec2& operator*=(const Vec2& other);
	constexpr Vec2& operator/=(const Vec2& other);
	constexpr bool operator==(const Vec2& other) const;

	template <AsNumeric T>
	Vec2 operator+(T scalar) const;

	template <AsNumeric T>
	Vec2 operator-(T scalar) const;

	template <AsNumeric T>
	Vec2 operator*(T scalar) const;

	template <AsNumeric T>
	Vec2 operator/(T scalar) const;

	template <AsNumeric T>
	constexpr Vec2& operator+=(T scalar);

	template <AsNumeric T>
	constexpr Vec2& operator-=(T scalar);

	template <AsNumeric T>
	constexpr Vec2& operator*=(T scalar);

	template <AsNumeric T>
	constexpr Vec2& operator/=(T scalar);
};

using Point = Vec2;

} // namespace vt

#include "Vec2_impl.hpp"

#endif
