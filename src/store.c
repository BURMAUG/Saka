#include "store.h"
#include "log.h"

#include <unistd.h>
#include <string.h>

void
read_fd_to_string(int fd, char *buf, size_t cap)
{
	size_t r_fd = read(fd, buf, cap - 1);
	LOG_DEBUG("lookup_vals: %s\nwhat is r_fd: %d\nwhat is fd: %d", buf, r_fd, fd);
	fflush(stdout);
	close(r_fd);
}

// this is where I'd have to update or marking it active
// start the time
// end the time
// increament frequency
int
write_string_fd(int fd, const char *buf)
{
	return fd;
}
