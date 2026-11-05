#ifndef ONES_H
#define ONES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ONES_API static inline

#ifdef _WIN32
    #define ONES_LINE_END "\r\n"
#else
    #define ONES_LINE_END "\n"
#endif

#ifdef __cplusplus
    #define ONES_DECLTYPE_CAST(T) (decltype(T))
#else
    #define ONES_DECLTYPE_CAST(T)
#endif

#if defined(__cplusplus)
    #define ONES_NORETURN [[noreturn]]
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
    #define ONES_NORETURN [[noreturn]]
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    #define ONES_NORETURN _Noreturn
#elif defined(__GNUC__) || defined(__clang__)
    #define ONES_NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
    #define ONES_NORETURN __declspec(noreturn)
#else
    #define ONES_NORETURN
#endif

#define ONES_MAX(a, b) ((a) > (b) ? (a) : (b))
#define ONES_MIN(a, b) ((a) < (b) ? (a) : (b))

#define ONES_SWAP_T(T, a, b) do { T t = (a); (a) = (b); (b) = t; } while (0)

/* panic && assert */

ONES_NORETURN void ones_panic_at(const char *file, int line, const char *format, ...);

#define ONES_PANIC(...) ones_panic_at(__FILE__, __LINE__, __VA_ARGS__)
#define ONES_ASSERT(cond, ...) ((cond) ? (void)0 : (void)ones_panic_at(__FILE__, __LINE__, "" __VA_ARGS__))

/* panic && assert */

/* move */

#define ONES_MOVE_PTR(des, src) \
    do { \
        *(des) = (src); \
        (src) = NULL; \
    } while(0)

/* move */

/* array */

#define ONES_ARR_LEN(array) (sizeof(array) / sizeof(array[0]))
#define ONES_ARR_AT(array, len, ind) (ONES_ASSERT((size_t)(ind) < (size_t)(len), "index out of range"), array[(size_t)ind])

#define ONES_ARR_MIN_CAP (4)

#define ONES_DYN_ARR_RESERVE(data, capa, expected_capa) \
    do { \
        if ((expected_capa) > (capa)) { \
            if (0 == (capa)) { \
                (capa) = ONES_ARR_MIN_CAP; \
            } \
            while ((expected_capa) > (capa)) { \
                ONES_ASSERT((capa) <= SIZE_MAX / 2, "capacity overflow"); \
                (capa) <<= 1; \
            } \
            void *tmp = realloc((data), (capa) * sizeof(*(data))); \
            ONES_ASSERT(tmp != NULL, "no space to alloc"); \
            (data) = tmp; \
        } \
    } while (0)

#define ONES_DYN_ARR_FREE(data) do { free((data)); (data) = NULL; } while (0)

#define ONES_DYN_ARR_RESIZE(data, capa, size, new_size) \
    do { \
        ONES_DYN_ARR_RESERVE((data), (capa), (new_size)); \
        (size) = (new_size); \
    } while (0)

#define ONES_DYN_ARR_CLEAR(size) do { (size) = 0; } while (0)

#define ONES_DYN_ARR_PUSH(data, capa, size, value) \
    do { \
        ONES_DYN_ARR_RESERVE((data), (capa), (size) + 1); \
        (data)[(size)++] = (value); \
    } while (0)

#define ONES_DYN_ARR_PUSHX(data, capa, size, values, n) \
    do { \
        ONES_DYN_ARR_RESERVE((data), (capa), (size) + (n)); \
        memcpy((data) + (size), (values), (n) * sizeof(*(data))); \
        (size) += (n); \
    } while (0)

#define ONES_DYN_ARR_POP(data, size) (data)[(ONES_ASSERT((size) > 0, "out of range"), --(size))]

#define ONES_DYN_ARR_FIRST(data, size) (data)[(ONES_ASSERT((size) > 0, "out of range"), 0)]

#define ONES_DYN_ARR_LAST(data, size) (data)[(ONES_ASSERT((size) > 0, "out of range"), (size) - 1)]

#define ONES_DYN_REMOVE_UNORDERED(data, size, i) \
    do { \
        size_t j = (i); \
        ONES_ASSERT(j < (size), "out of range"); \
        (data)[j] = (data)[--(size)]; \
    } while (0)

/* array */

/* string */

typedef struct {
    char *c_str;
    size_t size;
    size_t capa;
} ones_str_builder;

typedef struct {
    const char *data;
    size_t size;
} ones_str_view;

#define ONES_SB_PUSH_BUF(sb, buf, buf_size) ONES_DYN_ARR_PUSHX((sb)->c_str, (sb)->capa, (sb)->size, (buf), (buf_size))

#define ONES_SB_PUSH_SV(sb, sv) \
    do { \
        ones_str_view v = (sv); \
        ONES_SB_PUSH_BUF((sb), v.data, v.size); \
    } while (0)

#define ONES_SB_PUSH_C_STR(sb, c_str) \
    do { \
        const char *s = (c_str); \
        size_t n = strlen(s); \
        ONES_SB_PUSH_BUF((sb), (s), (n)); \
    } while (0)

#define ONES_SB_SET_NULL(sb) \
    do { \
        ONES_DYN_ARR_RESERVE((sb)->c_str, (sb)->capa, (sb)->size + 1); \
        (sb)->c_str[(sb)->size] = '\0'; \
    } while (0)

#define ONES_SB_PUSH(sb, c) ONES_DYN_ARR_PUSH((sb)->c_str, (sb)->capa, (sb)->size, (c))

#define ONES_SB_FREE(sb) do { free((sb)->c_str); (sb)->c_str = NULL; (sb)->size = 0; (sb)->capa = 0; } while (0)

/* string */

/* io */

void ones_stdin_clear_line(void);

/* io */

#ifdef ONES_IMPLEMENTATION

/* panic && assert */

ONES_NORETURN void ones_panic_at(const char *file, int line, const char *format, ...) {
    fprintf(stderr, "panic at %s:%d: ", file, line);
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs(ONES_LINE_END, stderr);
    abort();
}

/* panic && assert */

/* io */

void ones_stdin_clear_line(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

/* io */

#endif

#ifdef __cplusplus
}
#endif

#endif /* ONES_H */