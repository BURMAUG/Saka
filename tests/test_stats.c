#include "stats.h"
#include "test_harness.h"

static void
test_frequency_increments(void)
{
    map entry;
    memset(&entry, 0, sizeof(entry));
    entry.path = "/tmp/tingone";

    ASSERT_INT_EQ(frequency(&entry), 1);
    ASSERT_INT_EQ(frequency(&entry), 2);
    ASSERT_INT_EQ(frequency(&entry), 3);
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