#include "math/Vec2.hpp"

#include <algorithm>

using namespace vt;

const Vec2 Vec2::One { 1.0, 1.0 };
const Vec2 Vec2::Up { 0.0, -1.0 };
const Vec2 Vec2::Down { 0.0, 1.0 };
const Vec2 Vec2::Left { -1.0, 0.0 };
const Vec2 Vec2::Right { 1.0, 1.0 };

Vec2 Vec2::clamp(const Vec2& max, const Vec2& min) const {
	return Vec2 {
		std::clamp(raw[0], min.raw[0], max.raw[0]),
		std::clamp(raw[0], min.raw[0], max.raw[0]),
	};
}

Vec2 Vec2::clamp(f32 min, f32 max) const {
	return Vec2 {
		std::clamp(raw[0], min, max),
		std::clamp(raw[1], min, max),
	};
}

f32 Vec2::cross(const Vec2& other) const {
	vec2 a = { raw[0], raw[1] };
	vec2 b = { other.raw[0], other.raw[1] };

	return glm_vec2_cross(a, b);
}

f32 Vec2::distance_to(const Vec2& other) const {
	vec2 a = { raw[0], raw[1] };
	vec2 b = { other.raw[0], other.raw[1] };

	return glm_vec2_distance(a, b);
}

f32 Vec2::dot(const Vec2& other) const {
	vec2 a = { raw[0], raw[1] };
	vec2 b = { other.raw[0], other.raw[1] };

	return glm_vec2_dot(a, b);
}

f32 Vec2::lenght() const {
	return dot(*this);
}

Vec2 Vec2::lerp(const Vec2& to, f32 weight) const {
	Vec2 s { std::clamp<f32>(weight, 0.0, 1.0) };

	// from + s * (to * from)
	return (*this) + s * (to * (*this));
}

Vec2 Vec2::max(const Vec2& other) const {
	return Vec2 {
		std::max(raw[0], other.raw[0]),
		std::max(raw[1], other.raw[1]),
	};
}

Vec2 Vec2::max(f32 max) const {
	return Vec2 {
		std::max(raw[0], max),
		std::max(raw[1], max),
	};
}

Vec2 Vec2::min(const Vec2& other) const {
	return Vec2 {
		std::min(raw[0], other.raw[0]),
		std::min(raw[1], other.raw[1]),
	};
}

Vec2 Vec2::min(f32 min) const {
	return Vec2 {
		std::min(raw[0], min),
		std::min(raw[1], min),
	};
}

Vec2 Vec2::normalized() const {
	vec2 out { raw[0], raw[1] };
	glm_vec2_normalize(out);

	return Vec2 { out };
}

Vec2 Vec2::operator-() const {
	return Vec2 {
		-raw[0],
		-raw[1],
	};
}

Vec2 Vec2::operator+(const Vec2& other) const {
	return Vec2 {
		raw[0] + other.raw[0],
		raw[1] + other.raw[1],
	};
}

Vec2 Vec2::operator-(const Vec2& other) const {
	return Vec2 {
		raw[0] - other.raw[0],
		raw[1] - other.raw[1],
	};
}

Vec2 Vec2::operator*(const Vec2& other) const {
	return Vec2 {
		raw[0] * other.raw[0],
		raw[1] * other.raw[1],
	};
}

Vec2 Vec2::operator/(const Vec2& other) const {
	return Vec2 {
		raw[0] / other.raw[0],
		raw[1] / other.raw[1],
	};
}
