#ifndef _VT_GFX_DRAWABLE_HPP
#define _VT_GFX_DRAWABLE_HPP

#include "gfx/common.hpp"
#include "math/Transform.hpp"

#include <span>
#include <vector>

namespace vt::gfx {

class Drawable {
public:
	Drawable(sg_primitive_type primitive, const std::span<Vertex>& vertices);

	void append_vertices(const std::span<Vertex>& vertices);

	// NOTE: Probably is a good idea to separate a Drawable from a Transformable.
	// since a Transformable object can not be a Drawable, but a Drawable is
	// Transformable.
	void translate(const Vec3& offset);
	void rotate(f32 angle);
	void scale(const Vec2& factor);

	void set_texture(u32 slot, const Texture& texture);

	void set_origin(const Vec2& origin);
	void set_position(const Vec3& position);
	void set_angle(f32 angle);
	void set_scale(const Vec2& scale);

	[[nodiscard]] const Vec2& get_origin() const;
	[[nodiscard]] const Vec3& get_position() const;
	[[nodiscard]] f32 get_rotation() const;
	[[nodiscard]] const Vec2& get_scale() const;

	[[nodiscard]] const Transform& get_transform() const;

private:
	sg_primitive_type m_primitive;
	std::vector<Vertex> m_vertices;
	TexturesUniform m_textures;

	Vec2 m_origin;
	Vec3 m_position;
	f32 m_rotation {};
	Vec2 m_scale { 1.0, 1.0 };
	mutable Transform m_transform;
	mutable bool m_update_transform { true };

	friend class RenderBatcher;
};

} // namespace vt::gfx

#endif
