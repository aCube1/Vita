#ifndef _VT_GFX_COLOR_HPP
#define _VT_GFX_COLOR_HPP

#include "types.hpp"

#include <sokol/sokol_gfx.h>

#ifdef VT_COMPILER_CLANG
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#	pragma clang diagnostic ignored "-Wnested-anon-types"
#endif

namespace vt {

struct [[nodiscard]] Color {
	static const Color Black; // RGBA: 0x00'00'00'ff
	static const Color White; // RGBA: 0xff'ff'ff'ff
	static const Color Red;	  // RGBA: 0xff'00'00'ff
	static const Color Green; // RGBA: 0x00'ff'00'ff
	static const Color Blue;  // RGBA: 0x00'00'ff'ff

	union {
		u8 raw[4] = { 0x00, 0x00, 0x00, 0xff };

		struct {
			u8 r, g, b, a;
		};
	};

	constexpr Color() = default;
	constexpr Color(u8 red, u8 green, u8 blue, u8 alpha = 0xff)
		: r { red }, g { green }, b { blue }, a { alpha } { }
	constexpr explicit Color(u32 rgba)
		: r { static_cast<u8>((rgba & 0xff'00'00'00) >> 24) },
		  g { static_cast<u8>((rgba & 0x00'ff'00'00) >> 16) },
		  b { static_cast<u8>((rgba & 0x00'00'ff'00) >> 8) },
		  a { static_cast<u8>((rgba & 0x00'00'00'ff) >> 0) }

	{ }

	constexpr u32 as_u32() const {
		return static_cast<u32>((r << 24) | (g << 16) | (b << 16) | a);
	}

	constexpr bool operator==(const Color& other) const {
		return r == other.r && g == other.g && b == other.b && a == other.a;
	}
};

inline const Color Color::Black { 0, 0, 0 };
inline const Color Color::White { 255, 255, 255 };
inline const Color Color::Red { 255, 0, 0 };
inline const Color Color::Green { 0, 255, 0 };
inline const Color Color::Blue { 0, 0, 255 };

} // namespace vt

#endif
