#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"

static void vlog(const char *level, const char *fmt, va_list ap)
{
	extern const char *__progname;

	if (level)
		fprintf(stderr, "%s: %s: ", __progname, level);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
}

void log_info(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vlog(NULL, fmt, ap);
	va_end(ap);
}

void log_warn(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vlog("warn", fmt, ap);
	va_end(ap);
}

void fatal(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vlog("fatal", fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}
