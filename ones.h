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

#ifdef _WIN32
    #define ONES_LINE_END "\r\n"
#else
    #define ONES_LINE_END "\n"
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

#define ONES_SV_FMT "%.*s"
#define ONES_SV_ARG(sv) (int)(sv).size, (sv).data

ones_str_view ones_sv_from_parts(const char *str, size_t n);
ones_str_view ones_sv_from_c_str(const char *s);

#define ONES_SV_FROM_C_STR(s) ones_sv_from_c_str((s))
#define ONES_SV_FROM_PARTS(s, n) ones_sv_from_parts((s), (n))
#define ONES_SV_FROM_SB(sb) ones_sv_from_parts((sb)->c_str, (sb)->size)

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

void ones_sb_push_fmt(ones_str_builder *sb, const char *format, ...);

#define ONES_SB_PUSH_FMT(sb, format, ...) ones_sb_push_fmt((sb), format, __VA_ARGS__)

#define ONES_SB_TERMINATE(sb) \
    do { \
        ONES_DYN_ARR_RESERVE((sb)->c_str, (sb)->capa, (sb)->size + 1); \
        (sb)->c_str[(sb)->size] = '\0'; \
    } while (0)

#define ONES_SB_PUSH(sb, c) ONES_DYN_ARR_PUSH((sb)->c_str, (sb)->capa, (sb)->size, (c))

#define ONES_SB_FREE(sb) do { free((sb)->c_str); (sb)->c_str = NULL; (sb)->size = 0; (sb)->capa = 0; } while (0)

bool ones_sv_is_equal(ones_str_view a, ones_str_view b);

size_t ones_sv_find(ones_str_view sv, ones_str_view sub);

ones_str_view ones_sv_chop_while(ones_str_view *sv, int (*p)(int x));
ones_str_view ones_sv_chop_by_delim(ones_str_view *sv, char delim);
ones_str_view ones_sv_chop_by_sv(ones_str_view *sv, ones_str_view sub);

ones_str_view ones_sv_chop_left(ones_str_view *sv, size_t n);
ones_str_view ones_sv_chop_right(ones_str_view *sv, size_t n);

bool ones_sv_start_with_sv(ones_str_view sv, ones_str_view prefix);
bool ones_sv_end_with_sv(ones_str_view sv, ones_str_view suffix);

bool ones_sv_chop_prefix(ones_str_view *sv, ones_str_view prefix);
bool ones_sv_chop_suffix(ones_str_view *sv, ones_str_view suffix);

ones_str_view ones_sv_trim_left(ones_str_view sv);
ones_str_view ones_sv_trim_right(ones_str_view sv);
ones_str_view ones_sv_trim(ones_str_view sv);

/* string */

/* io */

void ones_stdin_clear_line(void);

bool ones_read_line(ones_str_builder *sb);

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

/* string */

ones_str_view ones_sv_from_parts(const char *str, size_t n) {
    ones_str_view v;
    v.data = str;
    v.size = n;
    return v;
}

ones_str_view ones_sv_from_c_str(const char *s) {
    ONES_ASSERT(s != NULL, "null c string");
    return ones_sv_from_parts(s, strlen(s));
}

void ones_sb_push_fmt(ones_str_builder *sb, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int needed = vsnprintf(NULL, 0, format, args);
    va_end(args);

    ONES_ASSERT(needed >= 0, "vsnprintf error");

    ONES_DYN_ARR_RESERVE(sb->c_str, sb->capa, sb->size + (size_t)needed + 1);

    va_start(args, format);
    vsnprintf(sb->c_str + sb->size, (size_t)needed + 1, format, args);
    va_end(args);

    sb->size += (size_t)needed;
}

bool ones_sv_is_equal(ones_str_view a, ones_str_view b) {
    if (a.size != b.size) return false;
    if (0 == a.size) return true;
    return memcmp(a.data, b.data, a.size) == 0;
}

size_t ones_sv_find(ones_str_view sv, ones_str_view sub) {
    if (sub.size == 0 || sub.size > sv.size) return sv.size;
    if (sub.size <= 1024) {
        for (size_t i = 0; i + sub.size <= sv.size; ++i) {
            if (memcmp(sv.data + i, sub.data, sub.size) == 0) return i;
        }
        return sv.size;
    } else {
        size_t *next = NULL;
        size_t next_capa = 0;
        ONES_DYN_ARR_RESERVE(next, next_capa, sub.size);
        next[0] = 0;
        size_t j = 0;
        for (size_t i = 1; i < sub.size; ++i) {
            while (j > 0 && sub.data[i] != sub.data[j]) {
                j = next[j - 1];
            }
            if (sub.data[i] == sub.data[j]) {
                ++j;
            }
            next[i] = j;
        }

        j = 0;
        for (size_t i = 0; i < sv.size; ++i) {
            while (j > 0 && sv.data[i] != sub.data[j]) {
                j = next[j - 1];
            }
            if (sv.data[i] == sub.data[j]) {
                ++j;
            }
            if (j == sub.size) {
                ONES_DYN_ARR_FREE(next);
                return i - sub.size + 1;

            }
        }

        ONES_DYN_ARR_FREE(next);
        return sv.size;
    }
}

ones_str_view ones_sv_chop_while(ones_str_view *sv, int (*p)(int x)) {
    size_t i = 0;
    while (i < sv->size && p((unsigned char)sv->data[i])) {
        ++i;
    }

    ones_str_view result = ONES_SV_FROM_PARTS(sv->data, i);
    sv->data += i;
    sv->size -= i;

    return result;
}

ones_str_view ones_sv_chop_by_delim(ones_str_view *sv, char delim) {
    size_t i = 0;
    while (i < sv->size && sv->data[i] != delim) {
        ++i;
    }

    ones_str_view result = ONES_SV_FROM_PARTS(sv->data, i);

    if (i < sv->size) {
        sv->data += i + 1;
        sv->size -= i + 1;
    } else {
        sv->data += sv->size;
        sv->size = 0;
    }

    return result;
}

ones_str_view ones_sv_chop_by_sv(ones_str_view *sv, ones_str_view sub){
    size_t index = ones_sv_find(*sv, sub);

    ones_str_view result = ONES_SV_FROM_PARTS(sv->data, index);

    if (index + sub.size < sv->size) {
        sv->data += index + sub.size;
        sv->size -= index + sub.size;
    } else {
        sv->data += sv->size;
        sv->size = 0;
    }

    return result;
}

ones_str_view ones_sv_chop_left(ones_str_view *sv, size_t n) {
    if (n > sv->size) {
        n = sv->size;
    }

    ones_str_view result = ONES_SV_FROM_PARTS(sv->data, n);

    sv->data += n;
    sv->size -= n;

    return result;
}

ones_str_view ones_sv_chop_right(ones_str_view *sv, size_t n) {
    if (n > sv->size) {
        n = sv->size;
    }

    ones_str_view result = ONES_SV_FROM_PARTS(sv->data + sv->size - n, n);

    sv->size -= n;

    return result;
}

bool ones_sv_start_with_sv(ones_str_view sv, ones_str_view prefix) {
    if (prefix.size <= sv.size) {
        ones_str_view parts = ONES_SV_FROM_PARTS(sv.data, prefix.size);
        return ones_sv_is_equal(parts, prefix);
    }

    return false;
}

bool ones_sv_end_with_sv(ones_str_view sv, ones_str_view suffix) {
    if (suffix.size <= sv.size) {
        ones_str_view parts = ONES_SV_FROM_PARTS(sv.data + sv.size - suffix.size, suffix.size);
        return ones_sv_is_equal(parts, suffix);
    }

    return false;
}

bool ones_sv_chop_prefix(ones_str_view *sv, ones_str_view prefix) {
    if (ones_sv_start_with_sv(*sv, prefix)) {
        sv->data += prefix.size;
        sv->size -= prefix.size;
        return true;
    }

    return false;
}

bool ones_sv_chop_suffix(ones_str_view *sv, ones_str_view suffix) {
    if (ones_sv_end_with_sv(*sv, suffix)) {
        sv->size -= suffix.size;
        return true;
    }

    return false;
}

ones_str_view ones_sv_trim_left(ones_str_view sv) {
    size_t i = 0;
    while (i < sv.size && isspace((unsigned char)sv.data[i])) {
        ++i;
    }

    return ONES_SV_FROM_PARTS(sv.data + i, sv.size - i);
}

ones_str_view ones_sv_trim_right(ones_str_view sv) {
    size_t i = 0;
    while (i < sv.size && isspace((unsigned char)sv.data[sv.size - i - 1])) {
        ++i;
    }

    return ONES_SV_FROM_PARTS(sv.data, sv.size - i);
}

ones_str_view ones_sv_trim(ones_str_view sv) {
    return ones_sv_trim_left(ones_sv_trim_right(sv));
}

/* string */

/* io */

void ones_stdin_clear_line(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

bool ones_read_line(ones_str_builder *sb) {
    int c = getchar();
    if (c == EOF) return false;
    while (c != '\n' && c != EOF) {
        ONES_SB_PUSH(sb, (char)c);
        c = getchar();
    }
    return true;
}

/* io */

#endif

#ifdef __cplusplus
}
#endif

#endif /* ONES_H */