#include "ones.h"
#include "test_utils.h"

int main(void) {
    TEST_RUN("dynamic array (ONES_DYN_ARR_*)");

    /* ---- empty state ---- */
    {
        int *data = NULL;
        size_t capa = 0;
        size_t size = 0;

        TEST_ASSERT_PTR_EQ(NULL, data);
        TEST_ASSERT_SIZE_T_EQ(0u, size);
        TEST_ASSERT_SIZE_T_EQ(0u, capa);
    }

    /* ---- int array: push, pop, first, last ---- */
    {
        int *data = NULL;
        size_t capa = 0;
        size_t size = 0;

        ONES_DYN_ARR_PUSH(data, capa, size, 1);
        ONES_DYN_ARR_PUSH(data, capa, size, 4);
        ONES_DYN_ARR_PUSH(data, capa, size, 2);
        ONES_DYN_ARR_PUSH(data, capa, size, 5);

        TEST_ASSERT_SIZE_T_EQ(4u, size);
        TEST_ASSERT(capa >= size);
        TEST_ASSERT_INT_EQ(1, data[0]);
        TEST_ASSERT_INT_EQ(4, data[1]);
        TEST_ASSERT_INT_EQ(2, data[2]);
        TEST_ASSERT_INT_EQ(5, data[3]);

        TEST_ASSERT_INT_EQ(5, ONES_DYN_ARR_POP(data, size));
        TEST_ASSERT_SIZE_T_EQ(3u, size);
        TEST_ASSERT_INT_EQ(2, ONES_DYN_ARR_LAST(data, size));
        TEST_ASSERT_INT_EQ(1, ONES_DYN_ARR_FIRST(data, size));

        /* grow beyond the minimum capacity to exercise the reserve path */
        ONES_DYN_ARR_RESERVE(data, capa, 16);
        TEST_ASSERT(capa >= 16u);

        ONES_DYN_ARR_CLEAR(size);
        TEST_ASSERT_SIZE_T_EQ(0u, size);

        ONES_DYN_ARR_FREE(data);
        TEST_ASSERT_PTR_EQ(NULL, data);
    }

    /* ---- pushx: bulk append ---- */
    {
        int values[5] = { 10, 20, 30, 40, 50 };
        int *data = NULL;
        size_t capa = 0;
        size_t size = 0;

        ONES_DYN_ARR_PUSHX(data, capa, size, values, 5);
        TEST_ASSERT_SIZE_T_EQ(5u, size);
        TEST_ASSERT_INT_EQ(10, data[0]);
        TEST_ASSERT_INT_EQ(50, data[4]);

        /* zero-length push is a no-op */
        ONES_DYN_ARR_PUSHX(data, capa, size, values, 0);
        TEST_ASSERT_SIZE_T_EQ(5u, size);

        ONES_DYN_ARR_FREE(data);
    }

    /* ---- resize ---- */
    {
        int *data = NULL;
        size_t capa = 0;
        size_t size = 0;

        ONES_DYN_ARR_RESIZE(data, capa, size, 8);
        TEST_ASSERT_SIZE_T_EQ(8u, size);
        TEST_ASSERT(capa >= 8u);

        /* shrinking only changes size, capacity stays */
        ONES_DYN_ARR_RESIZE(data, capa, size, 3);
        TEST_ASSERT_SIZE_T_EQ(3u, size);
        TEST_ASSERT(capa >= 8u);

        ONES_DYN_ARR_CLEAR(size);
        TEST_ASSERT_SIZE_T_EQ(0u, size);

        ONES_DYN_ARR_FREE(data);
    }

    /* ---- remove unordered ---- */
    {
        int *data = NULL;
        size_t capa = 0;
        size_t size = 0;

        for (int i = 1; i <= 4; ++i) {
            ONES_DYN_ARR_PUSH(data, capa, size, i);
        }
        TEST_ASSERT_SIZE_T_EQ(4u, size);

        /* remove middle: last element fills the hole */
        ONES_DYN_REMOVE_UNORDERED(data, size, 1);
        TEST_ASSERT_SIZE_T_EQ(3u, size);
        TEST_ASSERT_INT_EQ(1, data[0]);
        TEST_ASSERT_INT_EQ(4, data[1]);
        TEST_ASSERT_INT_EQ(3, data[2]);

        /* remove last element */
        ONES_DYN_REMOVE_UNORDERED(data, size, size - 1u);
        TEST_ASSERT_SIZE_T_EQ(2u, size);
        TEST_ASSERT_INT_EQ(1, data[0]);
        TEST_ASSERT_INT_EQ(4, data[1]);

        /* remove first element */
        ONES_DYN_REMOVE_UNORDERED(data, size, 0);
        TEST_ASSERT_SIZE_T_EQ(1u, size);
        TEST_ASSERT_INT_EQ(4, data[0]);

        ONES_DYN_ARR_FREE(data);
    }

    /* ---- double array ---- */
    {
        double *ddata = NULL;
        size_t dcapa = 0;
        size_t dsize = 0;

        ONES_DYN_ARR_PUSH(ddata, dcapa, dsize, 1.0);
        ONES_DYN_ARR_PUSH(ddata, dcapa, dsize, 4.0);
        ONES_DYN_ARR_PUSH(ddata, dcapa, dsize, 2.0);
        ONES_DYN_ARR_PUSH(ddata, dcapa, dsize, 5.0);

        TEST_ASSERT_SIZE_T_EQ(4u, dsize);
        TEST_ASSERT_DOUBLE_EQ(1.0, ddata[0]);
        TEST_ASSERT_DOUBLE_EQ(4.0, ddata[1]);
        TEST_ASSERT_DOUBLE_EQ(2.0, ddata[2]);
        TEST_ASSERT_DOUBLE_EQ(5.0, ddata[3]);

        (void)ONES_DYN_ARR_POP(ddata, dsize);
        (void)ONES_DYN_ARR_POP(ddata, dsize);
        TEST_ASSERT_SIZE_T_EQ(2u, dsize);
        TEST_ASSERT_DOUBLE_EQ(4.0, ONES_DYN_ARR_LAST(ddata, dsize));
        TEST_ASSERT_DOUBLE_EQ(1.0, ONES_DYN_ARR_FIRST(ddata, dsize));

        ONES_DYN_ARR_FREE(ddata);
        TEST_ASSERT_PTR_EQ(NULL, ddata);
    }

    /* ---- reserve from zero with exact minimum ---- */
    {
        int *data = NULL;
        size_t capa = 0;
        size_t size = 0;

        ONES_DYN_ARR_RESERVE(data, capa, 1);
        TEST_ASSERT(capa >= 4u);
        TEST_ASSERT(data != NULL);
        (void)size;

        ONES_DYN_ARR_FREE(data);
    }

    TEST_SUMMARY();
}
