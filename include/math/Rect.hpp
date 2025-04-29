#ifndef _VT_MATH_RECT_HPP
#define _VT_MATH_RECT_HPP

#include "math/Vec2.hpp"

#ifdef VT_COMPILER_CLANG
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#	pragma clang diagnostic ignored "-Wnested-anon-types"
#endif

namespace vt {

struct [[nodiscard]] Rect {
	union {
		struct {
			f32 x, y;
			f32 w, h;
		};
		struct {
			f32 x1, y1;
			f32 x2, y2;
		};
	};

	Rect() = default;
	Rect(f32 x_, f32 y_, f32 w_, f32 h_) : x { x_ }, y { y_ }, w { w_ }, h { h_ } { }

	f32 get_area() const;
	Vec2 get_center() const;
	bool has_point(const Vec2& point) const;
	bool intersects(const Rect& rect, bool edges = false) const;

	constexpr bool operator==(const Rect& other) const {
		return x == other.x && y == other.y && w == other.w && h == other.h;
	}
};

} // namespace vt

#endif
