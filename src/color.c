#include "color.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int
enabled(void)
{
    static int cached = -1;
    if (cached != -1)
        return cached;
    if (getenv("NO_COLOR") != NULL) {
        cached = 0;
        return cached;
    }
    cached = isatty(fileno(stdout)) ? 1 : 0;
    return cached;
}

int
colors_enabled(void)
{
    return enabled();
}

const char *
color_reset(void)
{
    return enabled() ? "\033[0m" : "";
}

const char *
color_green(void)
{
    return enabled() ? "\033[32m" : "";
}

const char *
color_red(void)
{
    return enabled() ? "\033[31m" : "";
}

const char *
color_cyan(void)
{
    return enabled() ? "\033[36m" : "";
}

const char *
color_yellow(void)
{
    return enabled() ? "\033[33m" : "";
}