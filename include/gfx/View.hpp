#ifndef _VT_GFX_VIEW_HPP
#define _VT_GFX_VIEW_HPP

#include "math/Mat4.hpp"
#include "math/Vec2.hpp"

namespace vt {

/**
 * Holds a 2D camera
 */
class View {
public:
	View() = default;
	View(const Vec2& center)
		: m_center { center } { }

	View& translate(const Vec2& offset);
	View& rotate(f32 angle);
	View& zoom(f32 factor);

	void set_center(const Vec2& center);
	void set_rotation(f32 angle);
	void set_zoom(f32 scale);

	[[nodiscard]] const Vec2& get_center() const;
	[[nodiscard]] f32 get_rotation() const;
	[[nodiscard]] f32 get_zoom() const;

	[[nodiscard]] const Mat4& get_transform() const;

private:
	Vec2 m_center;
	f32 m_rotation;
	f32 m_zoom { 1.0 };

	mutable Mat4 m_transform;
	mutable bool m_update_transform { true };

	friend class RenderBatcher;
};

} // namespace vt

#endif
