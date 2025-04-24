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
	void apply_transform(const Transform& transform);
	void set_texture(u32 slot, const Texture& texture);

private:
	sg_primitive_type m_primitive;
	std::vector<Vertex> m_vertices;
	Transform m_transform;
	TexturesUniform m_textures;

	friend class RenderBatcher;
};

} // namespace vt::gfx

#endif
