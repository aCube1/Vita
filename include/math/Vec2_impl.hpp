#pragma once
#include "math/Vec2.hpp"

namespace vt {

constexpr Vec2& Vec2::operator+=(const Vec2& other) {
	raw[0] += other.raw[0];
	raw[1] += other.raw[1];

	return *this;
}

constexpr Vec2& Vec2::operator-=(const Vec2& other) {
	raw[0] -= other.raw[0];
	raw[1] -= other.raw[1];

	return *this;
}

constexpr Vec2& Vec2::operator*=(const Vec2& other) {
	raw[0] *= other.raw[0];
	raw[1] *= other.raw[1];

	return *this;
}

constexpr Vec2& Vec2::operator/=(const Vec2& other) {
	raw[0] /= other.raw[0];
	raw[1] /= other.raw[1];

	return *this;
}

constexpr bool Vec2::operator==(const Vec2& other) const {
	return raw[0] == other.raw[0] && raw[1] == other.raw[1];
}

template <typename T>
Vec2 Vec2::operator+(T scalar) const {
	return Vec2 {
		raw[0] + static_cast<f32>(scalar),
		raw[1] + static_cast<f32>(scalar),
	};
}

template <typename T>
Vec2 Vec2::operator-(T scalar) const {
	return Vec2 {
		raw[0] - static_cast<f32>(scalar),
		raw[1] - static_cast<f32>(scalar),
	};
}

template <typename T>
Vec2 Vec2::operator*(T scalar) const {
	return Vec2 {
		raw[0] * static_cast<f32>(scalar),
		raw[1] * static_cast<f32>(scalar),
	};
}

template <typename T>
Vec2 Vec2::operator/(T scalar) const {
	return Vec2 {
		raw[0] / static_cast<f32>(scalar),
		raw[1] / static_cast<f32>(scalar),
	};
}

template <typename T>
constexpr Vec2& Vec2::operator+=(T scalar) {
	raw[0] += static_cast<f32>(scalar);
	raw[1] += static_cast<f32>(scalar);

	return *this;
}

template <typename T>
constexpr Vec2& Vec2::operator-=(T scalar) {
	raw[0] -= static_cast<f32>(scalar);
	raw[1] -= static_cast<f32>(scalar);

	return *this;
}

template <typename T>
constexpr Vec2& Vec2::operator*=(T scalar) {
	raw[0] *= static_cast<f32>(scalar);
	raw[1] *= static_cast<f32>(scalar);

	return *this;
}

template <typename T>
constexpr Vec2& Vec2::operator/=(T scalar) {
	raw[0] /= static_cast<f32>(scalar);
	raw[1] /= static_cast<f32>(scalar);

	return *this;
}

} // namespace vt
