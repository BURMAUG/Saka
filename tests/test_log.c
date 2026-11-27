#include "log/log.h"
#include "test_harness.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

static char *
capture_stream(FILE *stream, int fd, void (*fn)(void))
{
	char tmpl[] = "/tmp/tingone-log-XXXXXX";
	int tmp = mkstemp(tmpl);
	ASSERT_TRUE(tmp >= 0);

	int saved = dup(fd);
	ASSERT_TRUE(saved >= 0);
	fflush(stream);
	ASSERT_INT_EQ(dup2(tmp, fd), fd);

	fn();

	fflush(stream);
	dup2(saved, fd);
	close(saved);

	off_t end = lseek(tmp, 0, SEEK_END);
	char *buf = NULL;
	if (end >= 0)
	{
		buf = malloc((size_t)end + 1);
		lseek(tmp, 0, SEEK_SET);
		ssize_t r = read(tmp, buf, (size_t)end);
		if (r < 0)
			r = 0;
		buf[r] = '\0';
	}
	close(tmp);
	unlink(tmpl);
	return buf;
}

static void
emit_info(void)
{
	LOG_INFO("hello %s", "world");
}

static void
emit_debug(void)
{
	LOG_DEBUG("dbg");
}

static void
emit_warn(void)
{
	LOG_WARN("warn\n");
}

static void
emit_error(void)
{
	LOG_ERROR("err");
}

static void
emit_null(void)
{
	log_message(LOG_INFO, NULL);
}

static void
emit_unknown(void)
{
	log_message((log_level_t)99, "unknown");
}

static void
emit_newline(void)
{
	log_message(LOG_INFO, "with newline\n");
}

static void
test_log_info_stdout(void)
{
	log_set_level(LOG_DEBUG);
	char *out = capture_stream(stdout, fileno(stdout), emit_info);
	ASSERT_TRUE(out != NULL);
	ASSERT_TRUE(strstr(out, "[INFO]") != NULL);
	ASSERT_TRUE(strstr(out, "hello world") != NULL);
	free(out);
}

static void
test_log_filtered(void)
{
	log_set_level(LOG_ERROR);
	char *out = capture_stream(stdout, fileno(stdout), emit_debug);
	ASSERT_TRUE(out != NULL);
	ASSERT_STR_EQ(out, "");
	free(out);
}

static void
test_log_warn_stderr(void)
{
	log_set_level(LOG_DEBUG);
	char *out = capture_stream(stderr, fileno(stderr), emit_warn);
	ASSERT_TRUE(out != NULL);
	ASSERT_TRUE(strstr(out, "[WARN]") != NULL);
	ASSERT_TRUE(strstr(out, "warn") != NULL);
	free(out);
}

static void
test_log_error_stderr(void)
{
	log_set_level(LOG_DEBUG);
	char *out = capture_stream(stderr, fileno(stderr), emit_error);
	ASSERT_TRUE(out != NULL);
	ASSERT_TRUE(strstr(out, "[ERROR]") != NULL);
	ASSERT_TRUE(strstr(out, "err") != NULL);
	free(out);
}

static void
test_log_null_fmt(void)
{
	log_set_level(LOG_DEBUG);
	char *out = capture_stream(stdout, fileno(stdout), emit_null);
	ASSERT_TRUE(out != NULL);
	ASSERT_STR_EQ(out, "");
	free(out);
}

static void
test_log_unknown_level(void)
{
	log_set_level(LOG_DEBUG);
	char *out = capture_stream(stderr, fileno(stderr), emit_unknown);
	ASSERT_TRUE(out != NULL);
	ASSERT_TRUE(strstr(out, "[UNKNOWN]") != NULL);
	ASSERT_TRUE(strstr(out, "unknown") != NULL);
	free(out);
}

static void
test_log_with_newline(void)
{
	log_set_level(LOG_DEBUG);
	char *out = capture_stream(stdout, fileno(stdout), emit_newline);
	ASSERT_TRUE(out != NULL);
	ASSERT_TRUE(strstr(out, "with newline\n") != NULL);
	/* should not add extra blank line beyond the one in message+fputc? 
	 * message ends with \n so no extra fputc */
	free(out);
}

static time_t wrap_time_value = 0;
static int wrap_time_enabled = 0;

time_t
__real_time(time_t *t);

time_t
__wrap_time(time_t *t)
{
	if (!wrap_time_enabled)
		return __real_time(t);
	if (t)
		*t = wrap_time_value;
	return wrap_time_value;
}

static void
emit_tm_null(void)
{
	log_message(LOG_INFO, "tmnull");
}

static void
test_log_tm_null(void)
{
	log_set_level(LOG_DEBUG);
	wrap_time_enabled = 1;
	wrap_time_value = (time_t)0x7fffffffffffffffLL;
	char *out = capture_stream(stdout, fileno(stdout), emit_tm_null);
	wrap_time_enabled = 0;
	ASSERT_TRUE(out != NULL);
	ASSERT_TRUE(strstr(out, "[INFO]") != NULL);
	ASSERT_TRUE(strstr(out, "tmnull") != NULL);
	free(out);
}

static void
emit_empty(void)
{
	log_message(LOG_INFO, "");
}

static void
test_log_empty_fmt(void)
{
	log_set_level(LOG_DEBUG);
	char *out = capture_stream(stdout, fileno(stdout), emit_empty);
	ASSERT_TRUE(out != NULL);
	ASSERT_TRUE(strstr(out, "[INFO]") != NULL);
	free(out);
}

int
main(void)
{
	RUN_TEST(test_log_info_stdout);
	RUN_TEST(test_log_filtered);
	RUN_TEST(test_log_warn_stderr);
	RUN_TEST(test_log_error_stderr);
	RUN_TEST(test_log_null_fmt);
	RUN_TEST(test_log_unknown_level);
	RUN_TEST(test_log_with_newline);
	RUN_TEST(test_log_tm_null);
	RUN_TEST(test_log_empty_fmt);
	TEST_SUMMARY();
}
