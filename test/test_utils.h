#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_tests_passed = 0;
static int g_tests_failed = 0;

#define TEST_ASSERT(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL: %s:%d: assertion '%s' failed\n", __FILE__, __LINE__, #cond); \
        g_tests_failed++; \
    } else { \
        g_tests_passed++; \
    } \
} while(0)

#define TEST_ASSERT_INT_EQ(expected, actual) do { \
    int _e = (expected); \
    int _a = (actual); \
    if (_e != _a) { \
        fprintf(stderr, "FAIL: %s:%d: %s == %d, but expected %d\n", \
                __FILE__, __LINE__, #actual, _a, _e); \
        g_tests_failed++; \
    } else { \
        g_tests_passed++; \
    } \
} while(0)

#define TEST_ASSERT_SIZE_T_EQ(expected, actual) do { \
    size_t _e = (expected); \
    size_t _a = (actual); \
    if (_e != _a) { \
        fprintf(stderr, "FAIL: %s:%d: %s == %zu, but expected %zu\n", \
                __FILE__, __LINE__, #actual, _a, _e); \
        g_tests_failed++; \
    } else { \
        g_tests_passed++; \
    } \
} while(0)

#define TEST_ASSERT_DOUBLE_EQ(expected, actual) do { \
    double _e = (expected); \
    double _a = (actual); \
    if (_e != _a) { \
        fprintf(stderr, "FAIL: %s:%d: %s == %f, but expected %f\n", \
                __FILE__, __LINE__, #actual, _a, _e); \
        g_tests_failed++; \
    } else { \
        g_tests_passed++; \
    } \
} while(0)

#define TEST_ASSERT_STR_EQ(expected, actual) do { \
    if (strcmp((expected), (actual)) != 0) { \
        fprintf(stderr, "FAIL: %s:%d: %s == \"%s\", but expected \"%s\"\n", \
                __FILE__, __LINE__, #actual, (actual), (expected)); \
        g_tests_failed++; \
    } else { \
        g_tests_passed++; \
    } \
} while(0)

#define TEST_RUN(name) do { \
    printf("=== Running test: %s ===\n", name); \
} while(0)

#define TEST_SUMMARY() do { \
    printf("\n=== Test Summary ===\n"); \
    printf("Passed: %d\n", g_tests_passed); \
    printf("Failed: %d\n", g_tests_failed); \
    if (g_tests_failed > 0) { \
        printf("RESULT: FAILED\n"); \
        return 1; \
    } else { \
        printf("RESULT: PASSED\n"); \
        return 0; \
    } \
} while(0)

#endif /* TEST_UTILS_H */