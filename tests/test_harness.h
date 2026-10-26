#ifndef TINGONE_TEST_HARNESS_H
#define TINGONE_TEST_HARNESS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int th_tests_run = 0;
static int th_tests_failed = 0;

#define ASSERT_TRUE(cond) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
            th_tests_failed++; \
        } \
    } while (0)

#define ASSERT_INT_EQ(a, b) \
    do { \
        if ((a) != (b)) { \
            fprintf(stderr, "FAIL %s:%d: %s == %s (%d != %d)\n", \
                    __FILE__, __LINE__, #a, #b, (int)(a), (int)(b)); \
            th_tests_failed++; \
        } \
    } while (0)

#define ASSERT_STR_EQ(a, b) \
    do { \
        if (strcmp((a), (b)) != 0) { \
            fprintf(stderr, "FAIL %s:%d: \"%s\" != \"%s\"\n", \
                    __FILE__, __LINE__, (a), (b)); \
            th_tests_failed++; \
        } \
    } while (0)

#define RUN_TEST(fn) \
    do { \
        th_tests_run++; \
        printf("Running %s...\n", #fn); \
        fn(); \
    } while (0)

#define TEST_SUMMARY() \
    do { \
        printf("\n%d/%d tests passed\n", th_tests_run - th_tests_failed, \
               th_tests_run); \
        return th_tests_failed == 0 ? 0 : 1; \
    } while (0)

#endif /* TINGONE_TEST_HARNESS_H */