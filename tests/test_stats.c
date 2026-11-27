#include "stats/stats.h"
#include "test_harness.h"

#include <string.h>


static void
test_frequency_increments(void)
{
	entry e;
	memset(&e, 0, sizeof(e));
	e.path = "/tmp/tingone";

	ASSERT_INT_EQ(frequency(&e), 1);
	ASSERT_INT_EQ(frequency(&e), 2);
	ASSERT_INT_EQ(frequency(&e), 3);
}


static void
test_frequency_null(void)
{
	ASSERT_INT_EQ(frequency(NULL), -1);
}


int
main(void)
{
	RUN_TEST(test_frequency_increments);
	RUN_TEST(test_frequency_null);
	TEST_SUMMARY();
}
