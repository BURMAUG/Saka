#include "setup.h"
#include "test_harness.h"

#include <string.h>

static void
test_parse_basic(void)
{
    const char *data = "/home,2,10,1\n/tmp/scratchpad,1,5,0\n";
    store_record_t records[8];
    size_t count = 0;

    ASSERT_INT_EQ(parse_storage_buffer(data, strlen(data), records, 8, &count), 0);
    ASSERT_INT_EQ((int)count, 2);
    ASSERT_STR_EQ(records[0].path, "/home");
    ASSERT_INT_EQ((int)records[0].frequency, 2);
    ASSERT_INT_EQ((int)records[0].duration, 10);
    ASSERT_INT_EQ((int)records[0].is_active, 1);
    ASSERT_STR_EQ(records[1].path, "/tmp/scratchpad");
    ASSERT_INT_EQ((int)records[1].frequency, 1);
}

static void
test_parse_empty(void)
{
    store_record_t records[4];
    size_t count = 99;
    ASSERT_INT_EQ(parse_storage_buffer("", 0, records, 4, &count), 0);
    ASSERT_INT_EQ((int)count, 0);

    const char *only_nl = "\n\n";
    ASSERT_INT_EQ(parse_storage_buffer(only_nl, strlen(only_nl), records, 4, &count), 0);
    ASSERT_INT_EQ((int)count, 0);
}

static void
test_parse_crlf_and_path_with_comma(void)
{
    /* Path contains a comma; trailing numeric fields are parsed from the right. */
    const char *data = "/a,b,3,7,0\r\n";
    store_record_t records[4];
    size_t count = 0;
    ASSERT_INT_EQ(parse_storage_buffer(data, strlen(data), records, 4, &count), 0);
    ASSERT_INT_EQ((int)count, 1);
    ASSERT_STR_EQ(records[0].path, "/a,b");
    ASSERT_INT_EQ((int)records[0].frequency, 3);
    ASSERT_INT_EQ((int)records[0].duration, 7);
    ASSERT_INT_EQ((int)records[0].is_active, 0);
}

static void
test_lookup(void)
{
    store_record_t records[4];
    memset(records, 0, sizeof(records));
    snprintf(records[0].path, MAX_PATH_LEN, "%s", "/home");
    records[0].frequency = 2;
    snprintf(records[1].path, MAX_PATH_LEN, "%s", "/tmp");
    records[1].frequency = 1;

    ASSERT_INT_EQ(lookup_record(records, 2, "/tmp"), 1);
    ASSERT_INT_EQ(lookup_record(records, 2, "/home"), 0);
    ASSERT_INT_EQ(lookup_record(records, 2, "/nope"), -1);
    ASSERT_INT_EQ(lookup_record(NULL, 2, "/tmp"), -1);
}

static void
test_parse_null_args(void)
{
    size_t count = 0;
    store_record_t records[1];
    ASSERT_INT_EQ(parse_storage_buffer(NULL, 0, records, 1, &count), -1);
    ASSERT_INT_EQ(parse_storage_buffer("x", 1, NULL, 1, &count), -1);
    ASSERT_INT_EQ(parse_storage_buffer("x", 1, records, 1, NULL), -1);
}

int
main(void)
{
    RUN_TEST(test_parse_basic);
    RUN_TEST(test_parse_empty);
    RUN_TEST(test_parse_crlf_and_path_with_comma);
    RUN_TEST(test_lookup);
    RUN_TEST(test_parse_null_args);
    TEST_SUMMARY();
}