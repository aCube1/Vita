#include "log.h"

#include "config.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static const char *_log_name[] = {
	"DEBUG", "INFO", "WARN", "ERROR", "FATAL",
};

static const char *_log_color[] = {
	"\x1b[32m", "\x1b[36m", "\x1b[33m", "\x1b[31m", "\x1b[35m",
};

void vt_log_message(VT_LogLevel level, const char *file, i32 line, const char *msg, ...) {
	time_t timer = time(nullptr);
	struct tm *ltime = localtime(&timer);

	char buf[16] = { 0 };
	buf[strftime(buf, 16, "%H:%M:%S", ltime)] = '\0';

#ifndef NDEBUG
	fprintf(
		stderr, "%s \x1b[90m%s:%d\x1b[0m %s[%s]\x1b[0m - ", buf, file, line,
		_log_color[level], _log_name[level]
	);
#else
	VT_UNUSED(file);
	VT_UNUSED(line);
	fprintf(stderr, "%s %s[%s]\x1b[0m - ", buf, _log_color[level], _log_name[level]);
#endif

	va_list args;
	va_start(args, msg);
	vfprintf(stderr, msg, args);
	fprintf(stderr, "\n");
	va_end(args);

	if (level == VT_LOG_FATAL) {
		abort();
	}
}

void vt_slog_callback(
	const char *tag,
	u32 level,
	u32 item,
	const char *msg,
	u32 line,
	const char *file,
	void *usrptr
) {
	VT_UNUSED(usrptr);

	u32 loglevel;
	switch (level) {
	case 0: loglevel = VT_LOG_FATAL; break;
	case 1: loglevel = VT_LOG_ERROR; break;
	case 2: loglevel = VT_LOG_WARN; break;
	default: loglevel = VT_LOG_INFO; break;
	}

	// The log message will never be longer than 512 bytes
	static char log[512] = {};

	u32 size = snprintf(nullptr, 0, "[%s](%d)", tag, item);
	u32 end = snprintf(log, size + 1, "[%s](%d)", tag, item);
	if (msg) {
		size = snprintf(nullptr, 0, " > %s", msg);
		snprintf(log + end, size + 1, " > %s", msg);
	}

	vt_log_message(loglevel, file, line, log);
}
