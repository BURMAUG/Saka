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
}

int
main(void)
{
    RUN_TEST(test_round_trip);
    RUN_TEST(test_empty_read);
    RUN_TEST(test_null_args);
    TEST_SUMMARY();
}