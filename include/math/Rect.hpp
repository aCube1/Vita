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
	Vec2 pos;
	Vec2 size;
	Vec2 end; // Should always be: pos + size

	Rect() = default;
	Rect(f32 x, f32 y, f32 w, f32 h) : pos { x, y }, size { w, h }, end { pos + size } { }
	Rect(const Vec2& pos_, const Vec2& size_)
		: pos { pos_ }, size { size_ }, end { pos_ + size_ } { }

	f32 get_area() const;
	Vec2 get_center() const;
	bool has_point(const Vec2& point) const;
	bool intersects(const Rect& rect, bool edges = false) const;

	constexpr bool operator==(const Rect& other) const {
		return pos == other.pos && size == other.size && end == other.end;
	}
};

} // namespace vt

#endif
