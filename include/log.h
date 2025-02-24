#ifndef _VT_LOG_H
#define _VT_LOG_H

#include "common.h"

#if !defined(__FILE_NAME__)
#	define __FILE_NAME__ "<unknown>"
#endif

#define LOG_DEBUG(...) vt_log_message(VT_LOG_DEBUG, __FILE_NAME__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)  vt_log_message(VT_LOG_INFO, __FILE_NAME__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...)  vt_log_message(VT_LOG_WARN, __FILE_NAME__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) vt_log_message(VT_LOG_ERROR, __FILE_NAME__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...) vt_log_message(VT_LOG_FATAL, __FILE_NAME__, __LINE__, __VA_ARGS__)

typedef enum VT_LogLevel {
	VT_LOG_DEBUG,
	VT_LOG_INFO,
	VT_LOG_WARN,
	VT_LOG_ERROR,
	VT_LOG_FATAL,
} VT_LogLevel;

void vt_log_message(VT_LogLevel level, const char *file, i32 line, const char *msg, ...);

void vt_slog_callback(
	const char *tag,
	u32 level,
	u32 item,
	const char *msg,
	u32 line,
	const char *file,
	void *usrptr
);

#endif
