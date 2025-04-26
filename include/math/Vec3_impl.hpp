#pragma once
#include "math/Vec3.hpp"

namespace vt {

constexpr Vec3& Vec3::operator+=(const Vec3& other) {
	raw[0] += other.raw[0];
	raw[1] += other.raw[1];
	raw[2] += other.raw[2];

	return *this;
}

constexpr Vec3& Vec3::operator-=(const Vec3& other) {
	raw[0] -= other.raw[0];
	raw[1] -= other.raw[1];
	raw[2] -= other.raw[2];

	return *this;
}

constexpr Vec3& Vec3::operator*=(const Vec3& other) {
	raw[0] *= other.raw[0];
	raw[1] *= other.raw[1];
	raw[2] *= other.raw[2];

	return *this;
}

constexpr Vec3& Vec3::operator/=(const Vec3& other) {
	raw[0] /= other.raw[0];
	raw[1] /= other.raw[1];
	raw[2] /= other.raw[2];

	return *this;
}

constexpr bool Vec3::operator==(const Vec3& other) const {
	return raw[0] == other.raw[0] && raw[1] == other.raw[1] && raw[2] == other.raw[2];
}

template <AsNumeric T>
Vec3 Vec3::operator+(T scalar) const {
	return Vec3 {
		raw[0] + static_cast<f32>(scalar),
		raw[1] + static_cast<f32>(scalar),
		raw[2] + static_cast<f32>(scalar),
	};
}

template <AsNumeric T>
Vec3 Vec3::operator-(T scalar) const {
	return Vec3 {
		raw[0] - static_cast<f32>(scalar),
		raw[1] - static_cast<f32>(scalar),
		raw[2] - static_cast<f32>(scalar),
	};
}

template <AsNumeric T>
Vec3 Vec3::operator*(T scalar) const {
	return Vec3 {
		raw[0] * static_cast<f32>(scalar),
		raw[1] * static_cast<f32>(scalar),
		raw[2] * static_cast<f32>(scalar),
	};
}

template <AsNumeric T>
Vec3 Vec3::operator/(T scalar) const {
	return Vec3 {
		raw[0] / static_cast<f32>(scalar),
		raw[1] / static_cast<f32>(scalar),
		raw[2] / static_cast<f32>(scalar),
	};
}

template <AsNumeric T>
constexpr Vec3& Vec3::operator+=(T scalar) {
	raw[0] += static_cast<f32>(scalar);
	raw[1] += static_cast<f32>(scalar);
	raw[2] += static_cast<f32>(scalar);

	return *this;
}

template <AsNumeric T>
constexpr Vec3& Vec3::operator-=(T scalar) {
	raw[0] -= static_cast<f32>(scalar);
	raw[1] -= static_cast<f32>(scalar);
	raw[2] -= static_cast<f32>(scalar);

	return *this;
}

template <AsNumeric T>
constexpr Vec3& Vec3::operator*=(T scalar) {
	raw[0] *= static_cast<f32>(scalar);
	raw[1] *= static_cast<f32>(scalar);
	raw[2] *= static_cast<f32>(scalar);

	return *this;
}

template <AsNumeric T>
constexpr Vec3& Vec3::operator/=(T scalar) {
	raw[0] /= static_cast<f32>(scalar);
	raw[1] /= static_cast<f32>(scalar);
	raw[2] /= static_cast<f32>(scalar);

	return *this;
}

} // namespace vt
