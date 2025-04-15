#ifndef _VT_MATH_VEC2_HPP
#define _VT_MATH_VEC2_HPP

#include "types.hpp"

#include <cglm/types.h>
#include <cglm/vec2.h>
#include <cstring>

#ifdef VT_COMPILER_CLANG
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#	pragma clang diagnostic ignored "-Wnested-anon-types"
#endif

namespace vt {

struct Vec2 {
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

	[[nodiscard]] Vec2 clamp(const Vec2& max, const Vec2& min) const;
	[[nodiscard]] Vec2 clamp(f32 min, f32 max) const;
	[[nodiscard]] f32 cross(const Vec2& other) const;
	[[nodiscard]] f32 distance_to(const Vec2& other) const;
	[[nodiscard]] f32 dot(const Vec2& other) const;
	[[nodiscard]] f32 lenght() const;
	[[nodiscard]] Vec2 lerp(const Vec2& to, f32 weight) const;
	[[nodiscard]] Vec2 max(const Vec2& other) const;
	[[nodiscard]] Vec2 max(f32 max) const;
	[[nodiscard]] Vec2 min(const Vec2& other) const;
	[[nodiscard]] Vec2 min(f32 min) const;
	[[nodiscard]] Vec2 normalized() const;
};

using Point = Vec2;

[[nodiscard]] inline Vec2 operator-(const Vec2& lh);
[[nodiscard]] inline Vec2 operator+(const Vec2& lh, const Vec2& rh);
[[nodiscard]] inline Vec2 operator-(const Vec2& lh, const Vec2& rh);
[[nodiscard]] inline Vec2 operator*(const Vec2& lh, const Vec2& rh);
[[nodiscard]] inline Vec2 operator/(const Vec2& lh, const Vec2& rh);
[[nodiscard]] inline bool operator==(const Vec2& lh, const Vec2& rh);

inline Vec2& operator+=(Vec2& lh, const Vec2& rh);
inline Vec2& operator-=(Vec2& lh, const Vec2& rh);
inline Vec2& operator*=(Vec2& lh, const Vec2& rh);
inline Vec2& operator/=(Vec2& lh, const Vec2& rh);

template <typename T>
[[nodiscard]] inline Vec2 operator+(const Vec2& lh, T rh);

template <typename T>
[[nodiscard]] inline Vec2 operator-(const Vec2& lh, T rh);

template <typename T>
[[nodiscard]] inline Vec2 operator*(const Vec2& lh, T rh);

template <typename T>
[[nodiscard]] inline Vec2 operator/(const Vec2& lh, T rh);

template <typename T>
inline Vec2& operator+=(Vec2& lh, T rh);

template <typename T>
inline Vec2& operator-=(Vec2& lh, T rh);

template <typename T>
inline Vec2& operator*=(Vec2& lh, T rh);

template <typename T>
inline Vec2& operator/=(Vec2& lh, T rh);

} // namespace vt

#include "Vec2_impl.hpp"

#endif
