#include "utils.h"

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

void log_message(enum LogLevel level, const char *file, i32 line, const char *msg, ...) {
	time_t timer = time(NULL);
	struct tm *ltime = localtime(&timer);

	char buf[16] = {0};
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
