#ifndef _VT_MATH_RECT_HPP
#define _VT_MATH_RECT_HPP

#include "Vec2.hpp"

#ifdef VT_COMPILER_CLANG
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#	pragma clang diagnostic ignored "-Wnested-anon-types"
#endif

namespace vt {

struct Rect {
	Vec2 pos;
	Vec2 size;
	Vec2 end; // Should always be: pos + size

	Rect() = default;
	Rect(f32 x, f32 y, f32 w, f32 h) : pos { x, y }, size { w, h }, end { pos + size } { }
	Rect(const Vec2& pos_, const Vec2& size_)
		: pos { pos_ }, size { size_ }, end { pos_ + size_ } { }

	[[nodiscard]] f32 get_area() const;
	[[nodiscard]] Vec2 get_center() const;
	[[nodiscard]] bool has_point(const Vec2& point) const;
	[[nodiscard]] bool intersects(const Rect& rect, bool edges = false) const;
};

[[nodiscard]] inline bool operator==(const Rect& lh, const Rect& rh) {
	return lh.pos == rh.pos && lh.size == rh.size && lh.end == rh.end;
}

} // namespace vt

#endif
