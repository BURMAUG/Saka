#include "setup/setup.h"
#include "stats/stats.h"
#include "test_harness.h"
#include "log/log.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

extern entry entries[MAX_ENTRIES];
extern size_t entry_count;
void save_entries(int fd);

static void
reset_state(void)
{
	entry_count = 0;
	memset(entries, 0, sizeof(entries));
	memset(log_buffer, 0, sizeof(log_buffer));
}

static char *
make_temp_file(void)
{
	char tmpl[] = "/tmp/tingone-setup-XXXXXX";
	int fd = mkstemp(tmpl);
	if (fd < 0)
		return NULL;
	close(fd);
	return strdup(tmpl);
}

static void
write_file(const char *path, const char *content)
{
	FILE *f = fopen(path, "w");
	ASSERT_TRUE(f != NULL);
	if (f)
	{
		fputs(content, f);
		fclose(f);
	}
}

static char *
read_file(const char *path)
{
	FILE *f = fopen(path, "r");
	if (!f)
		return NULL;
	fseek(f, 0, SEEK_END);
	long n = ftell(f);
	fseek(f, 0, SEEK_SET);
	char *buf = malloc((size_t)n + 1);
	if (!buf)
	{
		fclose(f);
		return NULL;
	}
	size_t r = fread(buf, 1, (size_t)n, f);
	buf[r] = '\0';
	fclose(f);
	return buf;
}

static void
test_setup_target_path_creates_and_tracks(void)
{
	reset_state();
	char *path = make_temp_file();
	ASSERT_TRUE(path != NULL);
	unlink(path); /* let setup_target_path open/create it */

	int fd = setup_target_path("/tmp/work", path, 1);
	/* setup_target_path closes fd and returns it */
	ASSERT_TRUE(fd >= 0);

	char *content = read_file(path);
	ASSERT_TRUE(content != NULL);
	ASSERT_TRUE(strstr(content, "/tmp/work") != NULL);
	ASSERT_TRUE(strstr(content, ",1,") != NULL || strstr(content, ",1\n") != NULL);
	free(content);
	unlink(path);
	free(path);
}

static void
test_setup_target_path_existing_store(void)
{
	reset_state();
	char *path = make_temp_file();
	ASSERT_TRUE(path != NULL);
	write_file(path, "/home,2,10,0,1000\n/tmp/scratchpad,1,5,0,0\n");

	int fd = setup_target_path("/home", path, 1);
	ASSERT_TRUE(fd >= 0);

	char *content = read_file(path);
	ASSERT_TRUE(content != NULL);
	/* /home should be present and frequency incremented to 3 */
	ASSERT_TRUE(strstr(content, "/home,3,") != NULL);
	ASSERT_TRUE(strstr(content, "/tmp/scratchpad,1,5,0,0") != NULL);
	free(content);
	unlink(path);
	free(path);
}

static void
test_setup_target_path_null_target(void)
{
	reset_state();
	char *path = make_temp_file();
	ASSERT_TRUE(path != NULL);
	write_file(path, "/home,2,10,0,1000\n");

	int fd = setup_target_path(NULL, path, 1);
	ASSERT_TRUE(fd >= 0);

	char *content = read_file(path);
	ASSERT_TRUE(content != NULL);
	/* null target should not add a new entry */
	ASSERT_TRUE(strstr(content, "/home,2,10,0,1000") != NULL);
	free(content);
	unlink(path);
	free(path);
}

static void
test_setup_target_path_open_failure(void)
{
	reset_state();
	/* directory path as file should fail to open as regular rw in some cases;
	 * use a path in a non-existent directory */
	int fd = setup_target_path("/tmp/work", "/no/such/dir/tingone-log", 1);
	ASSERT_INT_EQ(fd, -1);
}

static void
test_look_up_finalizes_active(void)
{
	reset_state();
	char *path = make_temp_file();
	ASSERT_TRUE(path != NULL);
	time_t now = time(NULL);
	char buf[256];
	snprintf(buf, sizeof(buf), "/home,2,10,1,%lld\n", (long long)(now - 5));
	write_file(path, buf);

	int fd = open(path, O_RDWR);
	ASSERT_TRUE(fd >= 0);
	int r = look_up_storage_for_active_path(fd, "/tmp");
	ASSERT_TRUE(r >= 0);
	close(fd);

	ASSERT_TRUE(entry_count >= 2);
	/* /home was active, should be finalized */
	int found_home = 0;
	int found_tmp = 0;
	for (size_t i = 0; i < entry_count; i++)
	{
		if (strcmp(entries[i].path, "/home") == 0)
		{
			found_home = 1;
			ASSERT_INT_EQ((int)entries[i].is_active, 0);
			ASSERT_TRUE(entries[i].duration >= 15);
		}
		if (strcmp(entries[i].path, "/tmp") == 0)
		{
			found_tmp = 1;
			ASSERT_INT_EQ((int)entries[i].is_active, 1);
			ASSERT_INT_EQ((int)entries[i].frequency, 1);
		}
	}
	ASSERT_TRUE(found_home);
	ASSERT_TRUE(found_tmp);

	unlink(path);
	free(path);
}

static void
test_look_up_same_path_reactivates(void)
{
	reset_state();
	char *path = make_temp_file();
	ASSERT_TRUE(path != NULL);
	time_t now = time(NULL);
	char buf[256];
	snprintf(buf, sizeof(buf), "/home,2,10,1,%lld\n", (long long)(now - 2));
	write_file(path, buf);

	int fd = open(path, O_RDWR);
	ASSERT_TRUE(fd >= 0);
	look_up_storage_for_active_path(fd, "/home");
	close(fd);

	ASSERT_INT_EQ((int)entry_count, 1);
	ASSERT_STR_EQ(entries[0].path, "/home");
	ASSERT_INT_EQ((int)entries[0].frequency, 3);
	ASSERT_INT_EQ((int)entries[0].is_active, 1);
	ASSERT_TRUE(entries[0].duration >= 12);

	unlink(path);
	free(path);
}

static void
test_look_up_null_target(void)
{
	reset_state();
	int r = look_up_storage_for_active_path(-1, NULL);
	ASSERT_INT_EQ(r, -1);
}

static void
test_load_entries_legacy_format(void)
{
	reset_state();
	char *path = make_temp_file();
	ASSERT_TRUE(path != NULL);
	write_file(path, "/home,2,10,1\n");

	int fd = open(path, O_RDWR);
	ASSERT_TRUE(fd >= 0);
	look_up_storage_for_active_path(fd, "/other");
	close(fd);

	int found = 0;
	for (size_t i = 0; i < entry_count; i++)
	{
		if (strcmp(entries[i].path, "/home") == 0)
		{
			found = 1;
			ASSERT_INT_EQ((int)entries[i].frequency, 2);
			ASSERT_INT_EQ((int)entries[i].duration, 10);
			ASSERT_INT_EQ((int)entries[i].is_active, 0); /* finalized by lookup */
			ASSERT_INT_EQ((int)entries[i].start_t, 0);
		}
	}
	ASSERT_TRUE(found);

	unlink(path);
	free(path);
}

static void
test_load_entries_skips_malformed(void)
{
	reset_state();
	char *path = make_temp_file();
	ASSERT_TRUE(path != NULL);
	write_file(path, "badline\n/home,2,10,0,0\n/also,bad\n/a,1,2\n");

	int fd = open(path, O_RDWR);
	ASSERT_TRUE(fd >= 0);
	look_up_storage_for_active_path(fd, "/new");
	close(fd);

	int found_home = 0;
	for (size_t i = 0; i < entry_count; i++)
	{
		if (strcmp(entries[i].path, "/home") == 0)
			found_home = 1;
	}
	ASSERT_TRUE(found_home);

	unlink(path);
	free(path);
}

static void
test_load_entries_max_entries(void)
{
	reset_state();
	char *path = make_temp_file();
	ASSERT_TRUE(path != NULL);
	FILE *f = fopen(path, "w");
	ASSERT_TRUE(f != NULL);
	for (int i = 0; i < MAX_ENTRIES + 5; i++)
		fprintf(f, "/p%d,%d,0,0,0\n", i, i);
	fclose(f);

	int fd = open(path, O_RDWR);
	ASSERT_TRUE(fd >= 0);
	look_up_storage_for_active_path(fd, "/p0");
	close(fd);

	ASSERT_TRUE(entry_count <= MAX_ENTRIES);

	unlink(path);
	free(path);
}

static void
test_store_full_no_new_entry(void)
{
	reset_state();
	char *path = make_temp_file();
	ASSERT_TRUE(path != NULL);
	FILE *f = fopen(path, "w");
	ASSERT_TRUE(f != NULL);
	for (int i = 0; i < MAX_ENTRIES; i++)
		fprintf(f, "/p%d,1,0,0,0\n", i);
	fclose(f);

	int fd = open(path, O_RDWR);
	ASSERT_TRUE(fd >= 0);
	look_up_storage_for_active_path(fd, "/brand-new");
	close(fd);

	ASSERT_INT_EQ((int)entry_count, MAX_ENTRIES);
	int found = 0;
	for (size_t i = 0; i < entry_count; i++)
	{
		if (strcmp(entries[i].path, "/brand-new") == 0)
			found = 1;
	}
	ASSERT_TRUE(!found);

	unlink(path);
	free(path);
}

static void
test_save_entries_bad_fd(void)
{
	reset_state();
	save_entries(-1);
	ASSERT_TRUE(1);
}

static void
test_save_entries_ftruncate_fail(void)
{
	reset_state();
	char *path = make_temp_file();
	ASSERT_TRUE(path != NULL);
	int fd = open(path, O_RDONLY);
	ASSERT_TRUE(fd >= 0);
	save_entries(fd);
	close(fd);
	unlink(path);
	free(path);
}

static void
test_look_up_future_start(void)
{
	reset_state();
	char *path = make_temp_file();
	ASSERT_TRUE(path != NULL);
	time_t now = time(NULL);
	char buf[256];
	snprintf(buf, sizeof(buf), "/future,1,5,1,%lld\n", (long long)(now + 3600));
	write_file(path, buf);

	int fd = open(path, O_RDWR);
	ASSERT_TRUE(fd >= 0);
	look_up_storage_for_active_path(fd, "/other");
	close(fd);

	int found = 0;
	for (size_t i = 0; i < entry_count; i++)
	{
		if (strcmp(entries[i].path, "/future") == 0)
		{
			found = 1;
			ASSERT_INT_EQ((int)entries[i].duration, 5);
			ASSERT_INT_EQ((int)entries[i].is_active, 0);
		}
	}
	ASSERT_TRUE(found);
	unlink(path);
	free(path);
}

int
main(void)
{
	log_set_level(LOG_ERROR);
	RUN_TEST(test_setup_target_path_creates_and_tracks);
	RUN_TEST(test_setup_target_path_existing_store);
	RUN_TEST(test_setup_target_path_null_target);
	RUN_TEST(test_setup_target_path_open_failure);
	RUN_TEST(test_look_up_finalizes_active);
	RUN_TEST(test_look_up_same_path_reactivates);
	RUN_TEST(test_look_up_null_target);
	RUN_TEST(test_load_entries_legacy_format);
	RUN_TEST(test_load_entries_skips_malformed);
	RUN_TEST(test_load_entries_max_entries);
	RUN_TEST(test_store_full_no_new_entry);
	RUN_TEST(test_save_entries_bad_fd);
	RUN_TEST(test_save_entries_ftruncate_fail);
	RUN_TEST(test_look_up_future_start);
	TEST_SUMMARY();
}
