#ifndef _VT_MATH_TRANSFORM_HPP
#define _VT_MATH_TRANSFORM_HPP

#include "math/Matrix.hpp"
#include "math/Vec3.hpp"

namespace vt {

class Transform {
public:
	Transform() = default;

	Transform& translate(const Vec3& offset);
	Transform& rotate(f32 angle);
	Transform& scale(const Vec2& factor);

	void set_origin(const Vec2& origin);
	void set_position(const Vec3& position);
	void set_rotation(f32 angle);
	void set_scale(const Vec2& scale);

	[[nodiscard]] const Vec2& get_origin() const;
	[[nodiscard]] const Vec3& get_position() const;
	[[nodiscard]] f32 get_rotation() const;
	[[nodiscard]] const Vec2& get_scale() const;

	[[nodiscard]] const Matrix& get_matrix() const;

private:
	Vec2 m_origin;
	Vec3 m_position;
	f32 m_rotation {};
	Vec2 m_scale { 1.0, 1.0 };
	mutable Matrix m_transform;
	mutable bool m_update_transform { true };
};

} // namespace vt

#endif
