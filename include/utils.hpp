#ifndef _VT_UTILS_HPP
#define _VT_UTILS_HPP

#include <type_traits>

// Ignore nested anonymous structs
#ifdef VT_COMPILER_GCC
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wpedantic"
#endif
#ifdef VT_COMPILER_CLANG
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#	pragma clang diagnostic ignored "-Wnested-anon-types"
#endif

#if defined(WIN32) || defined(_WIN32)
#	define VT_PLATFORM_WINDOWS 1
#elif defined(__APPLE__)
#	include <TargetConditionals.h>
#	if defined(PLATFORM_OS_MAC)
#		define VT_PLATFORM_MACOS 1
#	endif
#elif defined(__linux__) || defined(__unix__)
#	define VT_PLATFORM_LINUX 1
#endif

#if !defined(VT_PLATFORM_WINDOWS) && !defined(VT_PLATFORM_LINUX) \
	&& !defined(VT_PLATFORM_MACOS)
#	error Could not detect target platform
#endif

#define VT_UNUSED(x) ((void)x)

namespace vt {

template <typename T>
concept AsNumeric = std::is_arithmetic_v<T>;

}

#endif
