#ifndef _VT_CONFIG_H
#define _VT_CONFIG_H

#if defined(__APPLE__)
#	include <TargetConditionals.h>
#	define GL_SILENCE_DEPRECATION
#	if defined(TARGET_OS_MAC)
#		define VT_TARGET_MACOS 1
#	endif
#elif defined(WIN32) || defined(_WIN32)
#	define VT_TARGET_WINDOWS 1
#elif defined(__linux__) || defined(__unix__)
#	define VT_TARGET_LINUX 1
#endif

#if !defined(VT_TARGET_WINDOWS) && !defined(VT_TARGET_LINUX) && !defined(VT_TARGET_MACOS)
#	error Could not detect target platform
#endif

#ifndef VT_UNUSED
#	define VT_UNUSED(x) ((void)x)
#endif

#endif
