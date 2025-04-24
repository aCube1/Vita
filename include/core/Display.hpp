#ifndef _VT_CORE_DISPLAY_HPP
#define _VT_CORE_DISPLAY_HPP

#include "gfx/RenderBatcher.hpp"
#include "types.hpp"

#include <string_view>

struct GLFWwindow;

namespace vt::core {

class Display : public gfx::RenderBatcher {
public:
	Display() = default;
	~Display() = default;

	Display(const Display&) = delete;
	Display& operator=(const Display&) = delete;

	bool create(i32 width, i32 height, std::string_view title);
	void close();

	void begin() override;
	void end() override;

	void present();

	// TODO: Window should communicate with the Main Event Bus to expose its events
	[[nodiscard]] bool is_open();

private:
	struct ContextSettings {
		u32 version_major;
		u32 version_minor;
		sg_pixel_format pixel_format;
		sg_pixel_format depth_format;
		i32 samples;
	};

	GLFWwindow *m_window { nullptr };
	Point m_framesize;
	ContextSettings m_context;
};

} // namespace vt::core

#endif
