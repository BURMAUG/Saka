#define _XOPEN_SOURCE 600
#include "colour/color.h"
#include "test_harness.h"

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined(__linux__)
#include <pty.h>
#endif

static void
test_no_color_env(void)
{
	setenv("NO_COLOR", "1", 1);
	ASSERT_INT_EQ(colors_enabled(), 0);
	ASSERT_STR_EQ(color_reset(), "");
	ASSERT_STR_EQ(color_green(), "");
	ASSERT_STR_EQ(color_red(), "");
	ASSERT_STR_EQ(color_cyan(), "");
	ASSERT_STR_EQ(color_yellow(), "");
	/* cached path */
	ASSERT_INT_EQ(colors_enabled(), 0);
}

static void
test_default_no_tty(void)
{
	unsetenv("NO_COLOR");
	/* stdout is typically not a tty under meson test; force it */
	int fd = open("/dev/null", O_WRONLY);
	ASSERT_TRUE(fd >= 0);
	int saved = dup(fileno(stdout));
	ASSERT_TRUE(saved >= 0);
	ASSERT_INT_EQ(dup2(fd, fileno(stdout)), fileno(stdout));
	close(fd);

	ASSERT_INT_EQ(colors_enabled(), 0);
	ASSERT_STR_EQ(color_reset(), "");
	ASSERT_STR_EQ(color_green(), "");
	ASSERT_STR_EQ(color_red(), "");
	ASSERT_STR_EQ(color_cyan(), "");
	ASSERT_STR_EQ(color_yellow(), "");
	ASSERT_INT_EQ(colors_enabled(), 0);

	dup2(saved, fileno(stdout));
	close(saved);
}

static void
test_tty_colors(void)
{
	unsetenv("NO_COLOR");
#if defined(__linux__)
	int master = -1;
	int slave = -1;
	if (openpty(&master, &slave, NULL, NULL, NULL) != 0)
	{
		/* can't get pty, skip but don't fail */
		printf("openpty failed, skipping tty branch\n");
		return;
	}
	int saved = dup(fileno(stdout));
	ASSERT_TRUE(saved >= 0);
	ASSERT_INT_EQ(dup2(slave, fileno(stdout)), fileno(stdout));
	close(slave);

	ASSERT_INT_EQ(colors_enabled(), 1);
	ASSERT_STR_EQ(color_reset(), "\033[0m");
	ASSERT_STR_EQ(color_green(), "\033[32m");
	ASSERT_STR_EQ(color_red(), "\033[31m");
	ASSERT_STR_EQ(color_cyan(), "\033[36m");
	ASSERT_STR_EQ(color_yellow(), "\033[33m");
	ASSERT_INT_EQ(colors_enabled(), 1);

	dup2(saved, fileno(stdout));
	close(saved);
	close(master);
#else
	printf("no pty support, skipping tty branch\n");
#endif
}

int
main(int argc, char **argv)
{
	const char *mode = argc > 1 ? argv[1] : "default";
	if (strcmp(mode, "no_color") == 0)
	{
		RUN_TEST(test_no_color_env);
	}
	else if (strcmp(mode, "tty") == 0)
	{
		RUN_TEST(test_tty_colors);
	}
	else
	{
		RUN_TEST(test_default_no_tty);
	}
	TEST_SUMMARY();
}
