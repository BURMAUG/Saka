#include "log.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static log_level_t current_level = LOG_INFO;

void
log_set_level(log_level_t level)
{
    current_level = level;
}

void
log_message(log_level_t level, const char *fmt, ...)
{
    if (level < current_level || fmt == NULL)
        return;

    static const char *names[] = { "DEBUG", "INFO", "WARN", "ERROR" };
    const char *name = (level >= LOG_DEBUG && level <= LOG_ERROR)
                           ? names[level]
                           : "UNKNOWN";

    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    char ts[32];
    if (tm != NULL)
        strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", tm);
    else
        snprintf(ts, sizeof(ts), "%ld", (long)now);

    FILE *out = (level >= LOG_WARN) ? stderr : stdout;
    fprintf(out, "%s [%s] ", ts, name);

    va_list args;
    va_start(args, fmt);
    vfprintf(out, fmt, args);
    va_end(args);

    size_t n = strlen(fmt);
    if (n == 0 || fmt[n - 1] != '\n')
        fputc('\n', out);
    fflush(out);
}