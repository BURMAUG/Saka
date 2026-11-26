#include "setup/setup.h"
#include "stats/stats.h"
#include "stdlib.h"
#include "store/store.h"
#include "string.h"
#include "log/log.h"
#include <stddef.h>
#include <stdio.h>
#if __has_include("color.h")
//#include "color.h"
#else
static inline const char *
color_reset(void)
{
	return "";
}


static inline const char *
color_green(void)
{
	return "";
}


static inline const char *
color_red(void)
{
	return "";
}


static inline const char *
color_cyan(void)
{
	return "";
}


static inline const char *
color_yellow(void)
{
	return "";
}
#endif

#include <unistd.h>
#include <fcntl.h>
//#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

char log_buffer[BUFF_SIZE];

entry entries[MAX_ENTRIES];
size_t entry_count = 0;

int look_up_storage_for_active_path(int fd, const char *target_path);
void load_entries();


void
print_entry(const entry *e)
{
	LOG_INFO("path=%s freq=%lu duration=%llu active=%u start=%lld\n", e->path, e->frequency,
	         e->duration, e->is_active, (long long)e->start_t);
}


void
save_entries(int fd)
{
	if (lseek(fd, 0, SEEK_SET) < 0 || ftruncate(fd, 0) < 0)
	{
		LOG_ERROR("unable to rewrite store with fd: %i\n", fd);
		return;
	}

	for (size_t i = 0; i < entry_count; i++)
	{
		const entry *e = &entries[i];
		dprintf(fd, "%s,%lu,%llu,%u,%lld\n", e->path, e->frequency, e->duration,
		        e->is_active, (long long)e->start_t);
	}
}


int
setup_target_path(const char *target_path, char *rumbu, int is_active)
{
	int fd = open(rumbu, O_RDWR, 0644); // read and write now
	LOG_INFO("STORE_FILE:[%s] created/open with fd: %i\n", rumbu, fd);
	if (fd < 0)
	{
		LOG_ERROR("unable to created/open with fd: %i\n", fd);
		return 1;
	}
	//
	//int read =
	look_up_storage_for_active_path(fd, target_path);
	save_entries(fd);
	close(fd);

	LOG_DEBUG("path: %s\n", target_path);

	return fd;
}


int
look_up_storage_for_active_path(int fd, const char *target_path)
{
	if (target_path == NULL)
	{
		return -1;
	}

	read_fd_to_string(fd, log_buffer, BUFF_SIZE);
	// even after read_fd_to_string is complete and popped off the program stack
	// buffer is still populated with the values read;
	LOG_DEBUG("in_buffer_values: %s", log_buffer);
	load_entries();

	time_t now = time(NULL);
	entry *target = NULL;

	for (size_t i = 0; i < entry_count; i++)
	{
		entry *e = &entries[i];

		if (e->is_active)
		{
			if (e->start_t > 0 && now > e->start_t)
			{
				e->duration += (unsigned long long)(now - e->start_t);
			}
			e->is_active = 0;
			e->start_t = 0;
		}

		if (strcmp(target_path, e->path) == 0)
		{
			LOG_INFO("found_pile");
			target = e;
		}
		else
		{
			print_entry(e);
		}
	}

	if (target == NULL && entry_count < MAX_ENTRIES)
	{
		target = &entries[entry_count++];
		target->path = target_path;
		target->frequency = 0;
		target->duration = 0;
	}

	if (target != NULL)
	{
		++target->frequency;
		target->is_active = 1;
		target->start_t = now;
		print_entry(target);
	}
	else
	{
		LOG_ERROR("store is full, not tracking: %s\n", target_path);
	}

	// find target_path in the current buffer and split into tokens
	// update the target to active
	// start the intial time
	return fd;
}


void
load_entries()
{
	char *save = NULL;
	char *line = strtok_r(log_buffer, "\n", &save);

	while (line != NULL && entry_count < MAX_ENTRIES)
	{
		char *field = NULL;
		char *path = strtok_r(line, ",", &field);
		char *freq = strtok_r(NULL, ",", &field);
		char *duration = strtok_r(NULL, ",", &field);
		char *is_active = strtok_r(NULL, ",", &field);
		char *start_t = strtok_r(NULL, ",", &field);

		if (path != NULL && freq != NULL && duration != NULL && is_active != NULL)
		{
			entry *e = &entries[entry_count++];
			e->path = path;
			e->frequency = strtoul(freq, NULL, 10);
			e->duration = strtoull(duration, NULL, 10);
			e->is_active = (unsigned int)strtoul(is_active, NULL, 10);
			e->start_t = start_t ? (time_t)strtoll(start_t, NULL, 10) : 0;
		}

		line = strtok_r(NULL, "\n", &save);
	}
}
