#ifndef _VT_MATH_VEC2I_HPP
#define _VT_MATH_VEC2I_HPP

#include "math/Vec2.hpp"
#include "types.hpp"
#include "utils.hpp"

#include <cglm/ivec2.h>
#include <cglm/types.h>

#ifdef VT_COMPILER_CLANG
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#	pragma clang diagnostic ignored "-Wnested-anon-types"
#endif

namespace vt {

struct [[nodiscard]] Vec2i {
	static const Vec2i Zero;  // x: 0.0 | y: 0.0
	static const Vec2i One;	  // x: 1.0 | y: 1.0
	static const Vec2i Up;	  // x: 0.0 | y:-1.0
	static const Vec2i Down;  // x: 0.0 | y: 1.0
	static const Vec2i Left;  // x:-1.0 | y: 0.0
	static const Vec2i Right; // x: 1.0 | y: 0.0

	union {
		ivec2 raw = { 0, 0 };

		struct {
			i32 x;
			i32 y;
		};

		struct {
			i32 w;
			i32 h;
		};

		struct {
			i32 u;
			i32 v;
		};
	};

	Vec2i() = default;
	Vec2i(const Vec2& vec)
		: x { static_cast<i32>(vec.x) }, y { static_cast<i32>(vec.y) } { };

	Vec2i(i32 fill)
		: x { fill }, y { fill } { }

	Vec2i(i32 x_, i32 y_)
		: x { x_ }, y { y_ } { }

	Vec2i(const i32 *v) {
		raw[0] = v[0];
		raw[1] = v[1];
	}

	Vec2i clamp(const Vec2i& max, const Vec2i& min) const;
	Vec2i clamp(i32 min, i32 max) const;
	i32 cross(const Vec2i& other) const;
	i32 distance_to(const Vec2i& other) const;
	i32 dot(const Vec2i& other) const;
	i32 lenght() const;
	Vec2i max(const Vec2i& other) const;
	Vec2i max(i32 max) const;
	Vec2i min(const Vec2i& other) const;
	Vec2i min(i32 min) const;

	Vec2i operator-() const;
	Vec2i operator+(const Vec2i& other) const;
	Vec2i operator-(const Vec2i& other) const;
	Vec2i operator*(const Vec2i& other) const;
	Vec2i operator/(const Vec2i& other) const;

	constexpr Vec2i& operator+=(const Vec2i& other);
	constexpr Vec2i& operator-=(const Vec2i& other);
	constexpr Vec2i& operator*=(const Vec2i& other);
	constexpr Vec2i& operator/=(const Vec2i& other);
	constexpr bool operator==(const Vec2i& other) const;

	template <AsNumeric T>
	Vec2i operator+(T scalar) const;

	template <AsNumeric T>
	Vec2i operator-(T scalar) const;

	template <AsNumeric T>
	Vec2i operator*(T scalar) const;

	template <AsNumeric T>
	Vec2i operator/(T scalar) const;

	template <AsNumeric T>
	constexpr Vec2i& operator+=(T scalar);

	template <AsNumeric T>
	constexpr Vec2i& operator-=(T scalar);

	template <AsNumeric T>
	constexpr Vec2i& operator*=(T scalar);

	template <AsNumeric T>
	constexpr Vec2i& operator/=(T scalar);
};

using Point = Vec2i;

} // namespace vt

#include "Vec2i_impl.hpp"

#endif
