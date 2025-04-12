#ifndef _VT_UTILS_HPP
#define _VT_UTILS_HPP

#define VT_UNUSED(x) ((void)x)

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

#endif
