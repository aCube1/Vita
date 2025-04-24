#ifndef _VT_GFX_UNIFORMBLOCK_HPP
#define _VT_GFX_UNIFORMBLOCK_HPP

#include "Pool.hpp"

#include <vector>

namespace vt::gfx {

struct Uniform {
	slot_id id;
};

class UniformBuffer {
public:
	template <typename T>
	[[nodiscard]] Uniform create(T object);
	void discard(Uniform uniform);

	bool apply(i32 slot, Uniform uniform);

private:
	struct UniformBlock {
		usize offset;
		usize size;
	};

	std::vector<u8> m_data;
	Pool m_indexes;
};

} // namespace vt::gfx

#endif
