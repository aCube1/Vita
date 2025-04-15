#pragma once
#include "Vec2.hpp"

namespace vt {

[[nodiscard]] inline Vec2 operator-(const Vec2& lh) {
	return Vec2 {
		-lh.raw[0],
		-lh.raw[1],
	};
}

[[nodiscard]] inline Vec2 operator+(const Vec2& lh, const Vec2& rh) {
	return Vec2 {
		lh.raw[0] + rh.raw[0],
		lh.raw[1] + rh.raw[1],
	};
}

[[nodiscard]] inline Vec2 operator-(const Vec2& lh, const Vec2& rh) {
	return Vec2 {
		lh.raw[0] - rh.raw[0],
		lh.raw[1] - rh.raw[1],
	};
}

[[nodiscard]] inline Vec2 operator*(const Vec2& lh, const Vec2& rh) {
	return Vec2 {
		lh.raw[0] * rh.raw[0],
		lh.raw[1] * rh.raw[1],
	};
}

[[nodiscard]] inline Vec2 operator/(const Vec2& lh, const Vec2& rh) {
	return Vec2 {
		lh.raw[0] / rh.raw[0],
		lh.raw[1] / rh.raw[1],
	};
}

[[nodiscard]] inline bool operator==(const Vec2& lh, const Vec2& rh) {
	return lh.raw[0] == rh.raw[0] && lh.raw[1] == rh.raw[1];
}

inline Vec2& operator+=(Vec2& lh, const Vec2& rh) {
	lh.raw[0] += rh.raw[0];
	lh.raw[1] += rh.raw[1];

	return lh;
}

inline Vec2& operator-=(Vec2& lh, const Vec2& rh) {
	lh.raw[0] -= rh.raw[0];
	lh.raw[1] -= rh.raw[1];

	return lh;
}

inline Vec2& operator*=(Vec2& lh, const Vec2& rh) {
	lh.raw[0] *= rh.raw[0];
	lh.raw[1] *= rh.raw[1];

	return lh;
}

inline Vec2& operator/=(Vec2& lh, const Vec2& rh) {
	lh.raw[0] /= rh.raw[0];
	lh.raw[1] /= rh.raw[1];

	return lh;
}

template <typename T>
[[nodiscard]] inline Vec2 operator+(const Vec2& lh, T rh) {
	return Vec2 {
		lh.raw[0] + rh,
		lh.raw[1] + rh,
	};
}

template <typename T>
[[nodiscard]] inline Vec2 operator-(const Vec2& lh, T rh) {
	return Vec2 {
		lh.raw[0] - rh,
		lh.raw[1] - rh,
	};
}

template <typename T>
[[nodiscard]] inline Vec2 operator*(const Vec2& lh, T rh) {
	return Vec2 {
		lh.raw[0] * rh,
		lh.raw[1] * rh,
	};
}

template <typename T>
[[nodiscard]] inline Vec2 operator/(const Vec2& lh, T rh) {
	return Vec2 {
		lh.raw[0] / rh,
		lh.raw[1] / rh,
	};
}

template <typename T>
inline Vec2& operator+=(Vec2& lh, T rh) {
	lh.raw[0] += rh;
	lh.raw[1] += rh;

	return lh;
}

template <typename T>
inline Vec2& operator-=(Vec2& lh, T rh) {
	lh.raw[0] -= rh;
	lh.raw[1] -= rh;

	return lh;
}

template <typename T>
inline Vec2& operator*=(Vec2& lh, T rh) {
	lh.raw[0] *= rh;
	lh.raw[1] *= rh;

	return lh;
}

template <typename T>
inline Vec2& operator/=(Vec2& lh, T rh) {
	lh.raw[0] /= rh;
	lh.raw[1] /= rh;

	return lh;
}

} // namespace vt
