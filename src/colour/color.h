#ifndef TINGONE_COLOR_H
#define TINGONE_COLOR_H

/* Returns 1 when ANSI colors should be emitted (TTY + NO_COLOR unset). */
int colors_enabled(void);

/* Color escape sequences (empty strings when colors are disabled). */
const char *color_reset(void);
const char *color_green(void);
const char *color_red(void);
const char *color_cyan(void);
const char *color_yellow(void);

#endif /* TINGONE_COLOR_H */