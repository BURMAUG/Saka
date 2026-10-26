#include "store.h"

#include <unistd.h>
#include <string.h>

int
read_fd_to_string(int fd, char *buf, size_t cap)
{
    if (fd < 0 || buf == NULL || cap == 0)
        return -1;

    size_t total = 0;
    while (total + 1 < cap) {
        ssize_t n = read(fd, buf + total, cap - total - 1);
        if (n < 0)
            return -1;
        if (n == 0)
            break;
        total += (size_t)n;
    }
    buf[total] = '\0';
    return (int)total;
}

int
write_string_fd(int fd, const char *buf)
{
    if (fd < 0 || buf == NULL)
        return -1;

    size_t len = strlen(buf);
    size_t written = 0;
    while (written < len) {
        ssize_t n = write(fd, buf + written, len - written);
        if (n < 0)
            return -1;
        written += (size_t)n;
    }
    return 0;
}