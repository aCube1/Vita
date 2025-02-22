#ifndef _VT_COMMON_H
#define _VT_COMMON_H

#include <assert.h>
// #include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if !defined(__FILE_NAME__)
#	define __FILE_NAME__ "<unknown>"
#endif

#define LOG_DEBUG(...) log_message(LOGL_DEBUG, __FILE_NAME__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)  log_message(LOGL_INFO, __FILE_NAME__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...)  log_message(LOGL_WARN, __FILE_NAME__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) log_message(LOGL_ERROR, __FILE_NAME__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...) log_message(LOGL_FATAL, __FILE_NAME__, __LINE__, __VA_ARGS__)

enum LogLevel {
	LOGL_DEBUG,
	LOGL_INFO,
	LOGL_WARN,
	LOGL_ERROR,
	LOGL_FATAL,
};

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef size_t usize;

typedef union irect {
	u32 raw[4];

	struct {
		u32 x, y;
		u32 w, h;
	};
} irect;

void log_message(enum LogLevel level, const char *file, i32 line, const char *msg, ...);

void slog_callback(
	const char *tag,
	u32 level,
	u32 item,
	const char *msg,
	u32 line,
	const char *file,
	void *usrdata
);

#endif
