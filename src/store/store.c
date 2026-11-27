#include "store.h"
#include "log/log.h"

#include <unistd.h>
#include <string.h>


int
read_fd_to_string(int fd, char *buf, size_t cap)
{
	if (fd < 0 || buf == NULL || cap == 0)
		return -1;

	ssize_t n = read(fd, buf, cap - 1);
	if (n < 0)
	{
		buf[0] = '\0';
		return -1;
	}

	buf[n] = '\0';
	LOG_DEBUG("lookup_vals: %s\nwhat is r_fd: %zd\nwhat is fd: %d", buf, n, fd);
	return (int)n;
}


// this is where I'd have to update or marking it active
// start the time
// end the time
// increament frequency
int
write_string_fd(int fd, const char *buf)
{
	if (fd < 0 || buf == NULL)
		return -1;

	size_t len = strlen(buf);
	size_t off = 0;
	while (off < len)
	{
		ssize_t w = write(fd, buf + off, len - off);
		if (w <= 0)
			return -1;
		off += (size_t)w;
	}
	return 0;
}
