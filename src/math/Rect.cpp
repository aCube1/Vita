#include "math/Rect.hpp"

using namespace vt;

f32 Rect::get_area() const {
	return size.w * size.h;
}

Vec2 Rect::get_center() const {
	return pos + (size / 2.0f);
}

bool Rect::has_point(const Vec2& point) const {
	return point.x >= pos.x			  //
		&& point.y >= pos.y			  //
		&& point.x <= pos.x + size.w  //
		&& point.y <= pos.y + size.h; //
}

bool Rect::intersects(const Rect& rect, bool edges) const {
	if (edges) {
		return pos.x <= rect.end.x && rect.pos.x <= end.x //
			&& pos.y <= rect.end.y && rect.pos.y <= end.y;
	}

	return pos.x < rect.end.x && rect.pos.x < end.x //
		&& pos.y < rect.end.y && rect.pos.y < end.y;
}
