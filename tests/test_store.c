#include "store.h"
#include "test_harness.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>


static void
test_round_trip(void)
{
	char tmpl[] = "/tmp/tingone-store-XXXXXX";
	int fd = mkstemp(tmpl);
	ASSERT_TRUE(fd >= 0);
	close(fd);

	fd = open(tmpl, O_WRONLY | O_TRUNC);
	ASSERT_TRUE(fd >= 0);
	ASSERT_INT_EQ(write_string_fd(fd, "hello world"), 0);
	close(fd);

	fd = open(tmpl, O_RDONLY);
	ASSERT_TRUE(fd >= 0);
	char buf[64];
	memset(buf, 0, sizeof(buf));
	ASSERT_INT_EQ(read_fd_to_string(fd, buf, sizeof(buf)), 11);
	ASSERT_STR_EQ(buf, "hello world");
	close(fd);

	unlink(tmpl);
}


static void
test_empty_read(void)
{
	char tmpl[] = "/tmp/tingone-empty-XXXXXX";
	int fd = mkstemp(tmpl);
	ASSERT_TRUE(fd >= 0);
	close(fd);

	fd = open(tmpl, O_RDONLY);
	ASSERT_TRUE(fd >= 0);
	char buf[16];
	memset(buf, 0, sizeof(buf));
	ASSERT_INT_EQ(read_fd_to_string(fd, buf, sizeof(buf)), 0);
	ASSERT_STR_EQ(buf, "");
	close(fd);

	unlink(tmpl);
}


static void
test_null_args(void)
{
	ASSERT_INT_EQ(read_fd_to_string(-1, NULL, 0), -1);
	ASSERT_INT_EQ(write_string_fd(-1, NULL), -1);
	ASSERT_INT_EQ(write_string_fd(1, NULL), -1);

	char buf[8];
	ASSERT_INT_EQ(read_fd_to_string(0, NULL, sizeof(buf)), -1);
	ASSERT_INT_EQ(read_fd_to_string(0, buf, 0), -1);
}


static void
test_read_error(void)
{
	int fd = open("/dev/null", O_WRONLY);
	ASSERT_TRUE(fd >= 0);
	char buf[8];
	buf[0] = 'x';
	ASSERT_INT_EQ(read_fd_to_string(fd, buf, sizeof(buf)), -1);
	close(fd);
}


static void
test_write_error(void)
{
	int fd = open("/dev/null", O_RDONLY);
	ASSERT_TRUE(fd >= 0);
	ASSERT_INT_EQ(write_string_fd(fd, "x"), -1);
	close(fd);
}


int
main(void)
{
	RUN_TEST(test_round_trip);
	RUN_TEST(test_empty_read);
	RUN_TEST(test_null_args);
	RUN_TEST(test_read_error);
	RUN_TEST(test_write_error);
	TEST_SUMMARY();
}