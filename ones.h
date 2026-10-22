#ifndef ONES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/* panic */

void ones_panic(const char *format, ...) {
    fprintf(stderr, "panic at %s:%d: ", __FILE__, __LINE__);
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);
    abort();
}

/* panic */

/* move */

#define ONES_MOVE(des, src) \
    do { \
        *(des) = (src); \
        (src) = NULL; \
    } while(0)

/* move */

/* dynamic array */

#define ONES_DECL_ARR(type) \
    typedef struct { \
        type *data; \
        size_t size; \
        size_t capa; \
    } ones_arr_##type; \
    \
    void ones_arr_init_##type(ones_arr_##type *arr) { \
        if (!arr) { \
            ones_panic("invalid arr"); \
            return; \
        } \
        \
        arr->data = NULL; \
        arr->size = 0; \
        arr->capa = 0; \
    } \
    \
    void ones_arr_deinit_##type(ones_arr_##type *arr) { \
        if (!arr) { \
            ones_panic("invalid arr"); \
            return; \
        } \
        \
        free(arr->data); \
        arr->data = NULL; \
        arr->size = 0; \
        arr->capa = 0; \
    } \
    \
    ones_arr_##type *ones_arr_create_##type(void) { \
        ones_arr_##type *arr = (ones_arr_##type *)malloc(sizeof(ones_arr_##type)); \
        if (!arr) { \
            ones_panic("run out of memory"); \
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
    void ones_arr_destroy_##type(ones_arr_##type *arr) { \
        if (!arr) { \
            ones_panic("invalid arr"); \
            return; \
        } \
        \
        free(arr->data); \
        free(arr); \
        arr = NULL; \
    } \
    \
    bool ones_arr_push_back_##type(ones_arr_##type *arr, type value) { \
        if (!arr) { \
            ones_panic("invalid arr"); \
            return false; \
        } \
        \
        if (arr->size + 1 > arr->capa) { \
            arr->capa = arr->capa == 0 ? 4 : arr->capa << 1; \
            type *new_data = (type *)realloc(arr->data, sizeof(type) * arr->capa); \
            if (!new_data) { \
                ones_panic("run out of memory"); \
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
    bool ones_arr_pop_back_##type(ones_arr_##type *arr) { \
        if (!arr) { \
            ones_panic("invalid arr"); \
            return false; \
        } \
        if (!arr->size) { \
            ones_panic("arr's size is already zero"); \
            return false; \
        } \
        \
        --arr->size; \
        return true; \
    } \

/* dynamic array */

#ifdef __cplusplus
}
#endif

#endif /* ONES_H */