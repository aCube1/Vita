#ifndef _VT_UTILS_H
#define _VT_UTILS_H

#include <assert.h>

#if defined(WIN32) || defined(_WIN32)
#	define VT_TARGET_WINDOWS 1
#elif defined(__APPLE__)
#	include <TargetConditionals.h>
#	if defined(TARGET_OS_MAC)
#		define VT_TARGET_MACOS 1
#	endif
#elif defined(__linux__) || defined(__unix__)
#	define VT_TARGET_LINUX 1
#endif

#if !defined(VT_TARGET_WINDOWS) && !defined(VT_TARGET_LINUX) && !defined(VT_TARGET_MACOS)
#	error Could not detect target platform
#endif

// Utility macros
#define VT_UNUSED(x)			((void)x)
#define VT_MIN(a, b)			((a) < (b) ? (a) : (b))
#define VT_MAX(a, b)			((a) > (b) ? (a) : (b))
#define VT_DEF(a, b)			((a) > 0 ? (a) : (b))
#define VT_CLEAR_MEM(ptr, size) memset((ptr), 0, (size))

#endif
