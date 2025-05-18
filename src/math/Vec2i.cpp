#include "math/Vec2i.hpp"

#include <algorithm>

using namespace vt;

const Vec2i Vec2i::Zero { 0, 0 };
const Vec2i Vec2i::One { 1, 1 };
const Vec2i Vec2i::Up { 0, -1 };
const Vec2i Vec2i::Down { 0, 1 };
const Vec2i Vec2i::Left { -1, 0 };
const Vec2i Vec2i::Right { 1, 0 };

Vec2i Vec2i::clamp(const Vec2i& max, const Vec2i& min) const {
	return Vec2i {
		std::clamp(raw[0], min.raw[0], max.raw[0]),
		std::clamp(raw[1], min.raw[1], max.raw[1]),
	};
}

Vec2i Vec2i::clamp(i32 min, i32 max) const {
	return Vec2i {
		std::clamp(raw[0], min, max),
		std::clamp(raw[1], min, max),
	};
}

i32 Vec2i::cross(const Vec2i& other) const {
	auto& a = const_cast<ivec2&>(raw);
	auto& b = const_cast<ivec2&>(other.raw);

	return glm_ivec2_cross(a, b);
}

i32 Vec2i::distance_to(const Vec2i& other) const {
	auto& a = const_cast<ivec2&>(raw);
	auto& b = const_cast<ivec2&>(other.raw);

	return glm_ivec2_distance(a, b);
}

i32 Vec2i::dot(const Vec2i& other) const {
	auto& a = const_cast<ivec2&>(raw);
	auto& b = const_cast<ivec2&>(other.raw);

	return glm_ivec2_dot(a, b);
}

i32 Vec2i::lenght() const {
	return dot(*this);
}

Vec2i Vec2i::max(const Vec2i& other) const {
	return Vec2i {
		std::max(raw[0], other.raw[0]),
		std::max(raw[1], other.raw[1]),
	};
}

Vec2i Vec2i::max(i32 max) const {
	return Vec2i {
		std::max(raw[0], max),
		std::max(raw[1], max),
	};
}

Vec2i Vec2i::min(const Vec2i& other) const {
	return Vec2i {
		std::min(raw[0], other.raw[0]),
		std::min(raw[1], other.raw[1]),
	};
}

Vec2i Vec2i::min(i32 min) const {
	return Vec2i {
		std::min(raw[0], min),
		std::min(raw[1], min),
	};
}

Vec2i Vec2i::operator-() const {
	return Vec2i {
		-raw[0],
		-raw[1],
	};
}

Vec2i Vec2i::operator+(const Vec2i& other) const {
	return Vec2i {
		raw[0] + other.raw[0],
		raw[1] + other.raw[1],
	};
}

Vec2i Vec2i::operator-(const Vec2i& other) const {
	return Vec2i {
		raw[0] - other.raw[0],
		raw[1] - other.raw[1],
	};
}

Vec2i Vec2i::operator*(const Vec2i& other) const {
	return Vec2i {
		raw[0] * other.raw[0],
		raw[1] * other.raw[1],
	};
}

Vec2i Vec2i::operator/(const Vec2i& other) const {
	return Vec2i {
		raw[0] / other.raw[0],
		raw[1] / other.raw[1],
	};
}
