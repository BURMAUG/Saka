#ifndef TINGONE_STORE_H
#define TINGONE_STORE_H

#include <stddef.h>

/* Read from a file descriptor into buf (capacity cap). Returns bytes read,
 * 0 on EOF, or -1 on error. The buffer is always NUL-terminated when cap>0. */
int read_fd_to_string(int fd, char *buf, size_t cap);

/* Write a NUL-terminated string to a file descriptor. Returns 0 on success,
 * -1 on error. */
int write_string_fd(int fd, const char *buf);

#endif /* TINGONE_STORE_H */