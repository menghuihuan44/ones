#ifndef ONES_H
#define ONES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <ctype.h>

#define ONES_API static inline

/* panic */

ONES_API void ones_panic_at(const char *file, int line, const char *format, ...) {
    fprintf(stderr, "panic at %s:%d: ", file, line);
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);
    abort();
}

#define ONES_PANIC(...) ones_panic_at(__FILE__, __LINE__, __VA_ARGS__)

/* panic */

/* move */

#define ONES_MOVE_PTR(des, src) \
    do { \
        *(des) = (src); \
        (src) = NULL; \
    } while(0)

/* move */

/* dynamic array */

#define ONES_DECL_ARR(typename, type) \
    typedef struct { \
        type *data; \
        size_t size; \
        size_t capa; \
    } ones_arr_##typename; \
    \
    ONES_API void ones_arr_init_##typename(ones_arr_##typename *arr) { \
        if (!arr) { \
            ONES_PANIC("invalid arr"); \
            return; \
        } \
        \
        arr->data = NULL; \
        arr->size = 0; \
        arr->capa = 0; \
    } \
    \
    ONES_API void ones_arr_deinit_##typename(ones_arr_##typename *arr) { \
        if (!arr) { \
            ONES_PANIC("invalid arr"); \
            return; \
        } \
        \
        free(arr->data); \
        arr->data = NULL; \
        arr->size = 0; \
        arr->capa = 0; \
    } \
    \
    ONES_API ones_arr_##typename *ones_arr_create_##typename(void) { \
        ones_arr_##typename *arr = (ones_arr_##typename *)malloc(sizeof(ones_arr_##typename)); \
        if (!arr) { \
            ONES_PANIC("run out of memory"); \
            return NULL; \
        } \
        \
        arr->data = NULL; \
        arr->size = 0; \
        arr->capa = 0; \
        \
        return arr; \
    } \
    \
    ONES_API void ones_arr_destroy_##typename(ones_arr_##typename *arr) { \
        if (!arr) { \
            ONES_PANIC("invalid arr"); \
            return; \
        } \
        \
        free(arr->data); \
        free(arr); \
    } \
    \
    ONES_API bool ones_arr_push_back_##typename(ones_arr_##typename *arr, type value) { \
        if (!arr) { \
            ONES_PANIC("invalid arr"); \
            return false; \
        } \
        \
        if (arr->size >= arr->capa) { \
            arr->capa = arr->capa == 0 ? 4 : arr->capa << 1; \
            type *new_data = (type *)realloc(arr->data, sizeof(type) * arr->capa); \
            if (!new_data) { \
                ONES_PANIC("run out of memory"); \
                return false; \
            } \
            \
            arr->data = new_data; \
        } \
        \
        arr->data[arr->size++] = value; \
        return true; \
    } \
    \
    ONES_API bool ones_arr_pop_back_##typename(ones_arr_##typename *arr) { \
        if (!arr) { \
            ONES_PANIC("invalid arr"); \
            return false; \
        } \
        if (!arr->size) { \
            ONES_PANIC("arr's size is already zero"); \
            return false; \
        } \
        \
        --arr->size; \
        return true; \
    } \

/* dynamic array */

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

ONES_API void ones_str_builder_init(ones_str_builder *builder) {
    if (!builder) {
        ONES_PANIC("invalid string builder");
        return;
    }

    builder->c_str = NULL;
    builder->size = 0;
    builder->capa = 0;
}

ONES_API void ones_str_builder_deinit(ones_str_builder *builder) {
    if (!builder) {
        ONES_PANIC("invalid string builder");
        return;
    }

    free(builder->c_str);
    builder->c_str = NULL;
    builder->size = 0;
    builder->capa = 0;
}

ONES_API ones_str_builder *ones_str_builder_create(void) {
    ones_str_builder *builder = (ones_str_builder *)malloc(sizeof(ones_str_builder));
    if (!builder) {
        ONES_PANIC("run out of memory");
        return NULL;
    }

    builder->c_str = NULL;
    builder->size = 0;
    builder->capa = 0;
    return builder;
}

ONES_API void ones_str_builder_destroy(ones_str_builder *builder) {
    if (!builder) {
        ONES_PANIC("invalid string builder");
        return;
    }

    free(builder->c_str);
    free(builder);
}

ONES_API bool ones_str_builder_push_char(ones_str_builder *builder, char c) {
    if (!builder) {
        ONES_PANIC("invalid string builder");
        return false;
    }

    if (builder->size >= builder->capa) {
        builder->capa = builder->capa == 0 ? 4 : builder->capa << 1;
        char *new_c_str = (char *)realloc(builder->c_str, builder->capa + 1);
        if (!new_c_str) {
            ONES_PANIC("run out of memory");
            return false;
        }

        builder->c_str = new_c_str;
    }

    builder->c_str[builder->size++] = c;
    builder->c_str[builder->size] = '\0';
    return true;
}

ONES_API bool ones_str_builder_push_str(ones_str_builder *builder, const char *str) {
    if (!builder) {
        ONES_PANIC("invalid string builder");
        return false;
    }
    if (!str) {
        ONES_PANIC("invalid string");
        return false;
    }

    size_t str_size = strlen(str);
    if (str_size == 0) {
        return true;
    }

    bool need_grow = false;
    while (builder->size + str_size > builder->capa) {
        builder->capa = builder->capa == 0 ? 4 : builder->capa << 1;
        need_grow = true;
    }

    if (need_grow) {
        char *new_c_str = (char *)realloc(builder->c_str, builder->capa + 1);
        if (!new_c_str) {
            ONES_PANIC("run out of memory");
            return false;
        }
        builder->c_str = new_c_str;
    }


    memcpy(builder->c_str + builder->size, str, str_size);
    builder->size += str_size;
    builder->c_str[builder->size] = '\0';
    return true;
}

ONES_API ones_str_view ones_str_builder_as_str_view(ones_str_builder *builder) {
    if (!builder) {
        ONES_PANIC("invalid string builder");
        return (ones_str_view){0};
    }

    return (ones_str_view){.data = builder->c_str, .size = builder->size};
}

ONES_API ones_str_view ones_str_view_triml(ones_str_view view) {
    while (view.size != 0 && isspace((unsigned char)*view.data)) {
        ++view.data;
        --view.size;
    }

    return view;
}

ONES_API ones_str_view ones_str_view_trimr(ones_str_view view) {
    while (view.size != 0 && isspace((unsigned char)*(view.data + view.size - 1))) {
        --view.size;
    }

    return view;
}

ONES_API ones_str_view ones_str_view_trim(ones_str_view view) {
    return ones_str_view_triml(ones_str_view_trimr(view));
}

/* string */

/* io */

ONES_API void ones_stdin_clear_line(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

/* io */

#endif /* ONES_H */