#ifndef _VT_GFX_COMMON_HPP
#define _VT_GFX_COMMON_HPP

#include "gfx/Color.hpp"
#include "math/Vec2.hpp"
#include "math/Vec3.hpp"

#include <array>
#include <sokol/sokol_gfx.h>

namespace vt::gfx {

constexpr u32 MAX_TEXTURES_SLOTS = SG_MAX_IMAGE_SAMPLER_PAIRS;

enum VertexAttr : u8 {
	Pos = 0,
	UV = 1,
	Color = 2,
};

struct Vertex {
	Vec3 position;
	Vec2 texcoord;
	struct Color color { Color::White };
};

struct Texture {
	sg_image img;
	sg_sampler smp;

	bool operator==(const Texture& other) const {
		return img.id == other.img.id && smp.id == other.smp.id;
	}
};

using TexturesUniform = std::array<Texture, MAX_TEXTURES_SLOTS>;

sg_pipeline_desc init_pipeline_desc(sg_primitive_type primitive, sg_shader shdr);
sg_pipeline make_pipeline(sg_primitive_type primitive);

sg_shader make_common_shader();
Texture make_common_texture();

} // namespace vt::gfx

#endif
