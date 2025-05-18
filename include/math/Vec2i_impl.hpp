#pragma once
#include "math/Vec2i.hpp"

namespace vt {

constexpr Vec2i& Vec2i::operator+=(const Vec2i& other) {
	raw[0] += other.raw[0];
	raw[1] += other.raw[1];

	return *this;
}

constexpr Vec2i& Vec2i::operator-=(const Vec2i& other) {
	raw[0] -= other.raw[0];
	raw[1] -= other.raw[1];

	return *this;
}

constexpr Vec2i& Vec2i::operator*=(const Vec2i& other) {
	raw[0] *= other.raw[0];
	raw[1] *= other.raw[1];

	return *this;
}

constexpr Vec2i& Vec2i::operator/=(const Vec2i& other) {
	raw[0] /= other.raw[0];
	raw[1] /= other.raw[1];

	return *this;
}

constexpr bool Vec2i::operator==(const Vec2i& other) const {
	return raw[0] == other.raw[0] && raw[1] == other.raw[1];
}

template <AsNumeric T>
Vec2i Vec2i::operator+(T scalar) const {
	return Vec2i {
		raw[0] + static_cast<i32>(scalar),
		raw[1] + static_cast<i32>(scalar),
	};
}

template <AsNumeric T>
Vec2i Vec2i::operator-(T scalar) const {
	return Vec2i {
		raw[0] - static_cast<i32>(scalar),
		raw[1] - static_cast<i32>(scalar),
	};
}

template <AsNumeric T>
Vec2i Vec2i::operator*(T scalar) const {
	return Vec2i {
		raw[0] * static_cast<i32>(scalar),
		raw[1] * static_cast<i32>(scalar),
	};
}

template <AsNumeric T>
Vec2i Vec2i::operator/(T scalar) const {
	return Vec2i {
		raw[0] / static_cast<i32>(scalar),
		raw[1] / static_cast<i32>(scalar),
	};
}

template <AsNumeric T>
constexpr Vec2i& Vec2i::operator+=(T scalar) {
	raw[0] += static_cast<i32>(scalar);
	raw[1] += static_cast<i32>(scalar);

	return *this;
}

template <AsNumeric T>
constexpr Vec2i& Vec2i::operator-=(T scalar) {
	raw[0] -= static_cast<i32>(scalar);
	raw[1] -= static_cast<i32>(scalar);

	return *this;
}

template <AsNumeric T>
constexpr Vec2i& Vec2i::operator*=(T scalar) {
	raw[0] *= static_cast<i32>(scalar);
	raw[1] *= static_cast<i32>(scalar);

	return *this;
}

template <AsNumeric T>
constexpr Vec2i& Vec2i::operator/=(T scalar) {
	raw[0] /= static_cast<i32>(scalar);
	raw[1] /= static_cast<i32>(scalar);

	return *this;
}

} // namespace vt
