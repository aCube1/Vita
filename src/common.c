#include "common.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static const char *_log_name[] = {
	"DEBUG", "INFO", "WARN", "ERROR", "FATAL",
};

static const char *_log_color[] = {
	"\x1b[32m", "\x1b[36m", "\x1b[33m", "\x1b[31m", "\x1b[35m",
};

void log_message(enum LogLevel level, const char *file, i32 line, const char *msg, ...) {
	time_t timer = time(nullptr);
	struct tm *ltime = localtime(&timer);

	char buf[16] = { 0 };
	buf[strftime(buf, 16, "%H:%M:%S", ltime)] = '\0';

#ifdef _DEBUG
	fprintf(
		stderr, "%s \x1b[90m%s:%d\x1b[0m %s[%s]\x1b[0m - ", buf, file, line,
		_log_color[level], _log_name[level]
	);
#else
	(void)file;
	(void)line;
	fprintf(stderr, "%s %s[%s]\x1b[0m - ", buf, _log_color[level], _log_name[level]);
#endif

	va_list args;
	va_start(args, msg);
	vfprintf(stderr, msg, args);
	fprintf(stderr, "\n");
	va_end(args);
}

void slog_callback(
	const char *tag,
	u32 level,
	u32 item,
	const char *msg,
	u32 line,
	const char *file,
	void *usrdata
) {
	(void)usrdata;

	u32 loglevel;
	switch (level) {
	case 0: loglevel = LOGL_FATAL; break;
	case 1: loglevel = LOGL_ERROR; break;
	case 2: loglevel = LOGL_WARN; break;
	default: loglevel = LOGL_INFO; break;
	}

	char log[1024]; // The log message will never be longer than 1024 bytes
	u32 size = 0;
	u32 end = 0;

	if (tag) {
		size = snprintf(nullptr, 0, "[%s]", tag);
		end = snprintf(log, size + 1, "[%s]", tag);
	}
	size = snprintf(nullptr, 0, "(%d) > %s", item, msg);
	snprintf(log + end, size + 1, "(%d) > %s", item, msg);

	log_message(loglevel, file, line, log);
}
