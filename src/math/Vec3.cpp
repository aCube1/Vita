#include "math/Vec3.hpp"

#include <algorithm>

using namespace vt;

const Vec3 Vec3::Zero { 0.0, 0.0, 0.0 };
const Vec3 Vec3::One { 1.0, 1.0, 1.0 };
const Vec3 Vec3::Up { 0.0, -1.0, 0.0 };
const Vec3 Vec3::Down { 0.0, 1.0, 0.0 };
const Vec3 Vec3::Left { -1.0, 0.0, 0.0 };
const Vec3 Vec3::Right { 1.0, 0.0, 0.0 };
const Vec3 Vec3::Front { 0.0, 0.0, 1.0 };
const Vec3 Vec3::Back { 0.0, 0.0, -1.0 };

Vec3 Vec3::clamp(const Vec3& max, const Vec3& min) const {
	return Vec3 {
		std::clamp(raw[0], min.raw[0], max.raw[0]),
		std::clamp(raw[1], min.raw[1], max.raw[1]),
		std::clamp(raw[2], min.raw[2], max.raw[2]),
	};
}

Vec3 Vec3::clamp(f32 min, f32 max) const {
	return Vec3 {
		std::clamp(raw[0], min, max),
		std::clamp(raw[1], min, max),
		std::clamp(raw[2], min, max),
	};
}

Vec3 Vec3::cross(const Vec3& other) const {
	vec3& a = const_cast<vec3&>(raw);
	vec3& b = const_cast<vec3&>(other.raw);
	vec3 out;
	glm_vec3_cross(a, b, out);

	return Vec3 { out };
}

f32 Vec3::distance_to(const Vec3& other) const {
	vec3& a = const_cast<vec3&>(raw);
	vec3& b = const_cast<vec3&>(other.raw);

	return glm_vec3_distance(a, b);
}

f32 Vec3::dot(const Vec3& other) const {
	vec3& a = const_cast<vec3&>(raw);
	vec3& b = const_cast<vec3&>(other.raw);

	return glm_vec3_dot(a, b);
}

Vec3 Vec3::lerp(const Vec3& to, f32 weight) const {
	vec3& from = const_cast<vec3&>(raw);
	vec3& to_ = const_cast<vec3&>(to.raw);
	vec3 out;
	glm_vec3_lerp(from, to_, weight, out);

	return Vec3 { out };
}

Vec3 Vec3::max(const Vec3& other) const {
	return Vec3 {
		std::max(raw[0], other.raw[0]),
		std::max(raw[1], other.raw[1]),
		std::max(raw[2], other.raw[2]),
	};
}

Vec3 Vec3::max(f32 max) const {
	return Vec3 {
		std::max(raw[0], max),
		std::max(raw[1], max),
		std::max(raw[2], max),
	};
}

Vec3 Vec3::min(const Vec3& other) const {
	return Vec3 {
		std::min(raw[0], other.raw[0]),
		std::min(raw[1], other.raw[1]),
		std::min(raw[2], other.raw[2]),
	};
}

Vec3 Vec3::min(f32 min) const {
	return Vec3 {
		std::min(raw[0], min),
		std::min(raw[1], min),
		std::min(raw[2], min),
	};
}

Vec3 Vec3::normalized() const {
	vec3& out = const_cast<vec3&>(raw);
	glm_vec3_normalize(out);

	return Vec3 { out };
}

Vec3 Vec3::operator-() const {
	return Vec3 {
		-raw[0],
		-raw[1],
		-raw[2],
	};
}

Vec3 Vec3::operator+(const Vec3& other) const {
	return Vec3 {
		raw[0] + other.raw[0],
		raw[1] + other.raw[1],
		raw[2] + other.raw[2],
	};
}

Vec3 Vec3::operator-(const Vec3& other) const {
	return Vec3 {
		raw[0] - other.raw[0],
		raw[1] - other.raw[1],
		raw[2] - other.raw[2],
	};
}

Vec3 Vec3::operator*(const Vec3& other) const {
	return Vec3 {
		raw[0] * other.raw[0],
		raw[1] * other.raw[1],
		raw[2] * other.raw[2],
	};
}

Vec3 Vec3::operator/(const Vec3& other) const {
	return Vec3 {
		raw[0] / other.raw[0],
		raw[1] / other.raw[1],
		raw[2] / other.raw[2],
	};
}
