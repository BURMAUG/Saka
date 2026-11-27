#ifdef main
#undef main
#endif

#include "test_harness.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

int ting_main(int argc, char **argv);

static void
test_main_no_args(void)
{
	char *argv[] = {"ting", NULL};
	int r = ting_main(1, argv);
	ASSERT_INT_EQ(r, 0);
}

static void
test_main_cd_missing_path(void)
{
	char *argv[] = {"ting", "cd", NULL};
	int r = ting_main(2, argv);
	ASSERT_INT_EQ(r, 0);
}

static void
test_main_cd_success(void)
{
	char tmpl[] = "/tmp/tingone-home-XXXXXX";
	char *home = mkdtemp(tmpl);
	ASSERT_TRUE(home != NULL);

	char store_dir[512];
	snprintf(store_dir, sizeof(store_dir), "%s/.local/share/ting", home);
	char cmd[1024];
	snprintf(cmd, sizeof(cmd), "mkdir -p '%s'", store_dir);
	ASSERT_INT_EQ(system(cmd), 0);

	char *old_home = getenv("HOME");
	setenv("HOME", home, 1);

	char *argv[] = {"ting", "cd", "/tmp", NULL};
	int r = ting_main(3, argv);
	ASSERT_INT_EQ(r, 0);

	if (old_home)
		setenv("HOME", old_home, 1);
	else
		unsetenv("HOME");

	snprintf(cmd, sizeof(cmd), "rm -rf '%s'", home);
	system(cmd);
}

static void
test_main_cd_open_failure(void)
{
	char tmpl[] = "/tmp/tingone-home-XXXXXX";
	char *home = mkdtemp(tmpl);
	ASSERT_TRUE(home != NULL);
	/* make .local/share/ting/log a directory so open(O_RDWR) fails */
	char store_dir[512];
	snprintf(store_dir, sizeof(store_dir), "%s/.local/share/ting/log", home);
	char cmd[1024];
	snprintf(cmd, sizeof(cmd), "mkdir -p '%s'", store_dir);
	ASSERT_INT_EQ(system(cmd), 0);

	char *old_home = getenv("HOME");
	setenv("HOME", home, 1);

	char *argv[] = {"ting", "cd", "/tmp", NULL};
	int r = ting_main(3, argv);
	ASSERT_INT_EQ(r, -1);

	if (old_home)
		setenv("HOME", old_home, 1);
	else
		unsetenv("HOME");

	snprintf(cmd, sizeof(cmd), "rm -rf '%s'", home);
	system(cmd);
}

static void
test_main_other_command(void)
{
	char *argv[] = {"ting", "ls", "/tmp", NULL};
	int r = ting_main(3, argv);
	ASSERT_INT_EQ(r, 0);
}

int
main(void)
{
	RUN_TEST(test_main_no_args);
	RUN_TEST(test_main_cd_missing_path);
	RUN_TEST(test_main_cd_success);
	RUN_TEST(test_main_cd_open_failure);
	RUN_TEST(test_main_other_command);
	TEST_SUMMARY();
}
