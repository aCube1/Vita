#include "math/Rect.hpp"

using namespace vt;

f32 Rect::get_area() const {
	return w * h;
}

Vec2 Rect::get_center() const {
	return Vec2(x, y) + (Vec2(w, h) / 2.0f);
}

bool Rect::has_point(const Vec2& point) const {
	return point.x >= x		 //
		&& point.y >= y		 //
		&& point.x <= x + w	 //
		&& point.y <= y + h; //
}

bool Rect::intersects(const Rect& rect, bool edges) const {
	if (edges) {
		return x1 <= rect.x2 && rect.x1 <= x2 //
			&& y1 <= rect.y2 && rect.y1 <= y2;
	}

	return x1 < rect.x2 && rect.x1 < x2 //
		&& y1 < rect.y2 && rect.y1 < y2;
}
