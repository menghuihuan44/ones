#include "ones.h"
#include "test_utils.h"

int main(void) {
    TEST_RUN("dynamic array (ONES_DYN_ARR_*)");

    /* ---- int array ---- */
    int *data = NULL;
    size_t capa = 0;
    size_t size = 0;

    TEST_ASSERT(data == NULL);
    TEST_ASSERT_SIZE_T_EQ(0, size);
    TEST_ASSERT_SIZE_T_EQ(0, capa);

    ONES_DYN_ARR_PUSH(data, capa, size, 1);
    ONES_DYN_ARR_PUSH(data, capa, size, 4);
    ONES_DYN_ARR_PUSH(data, capa, size, 2);
    ONES_DYN_ARR_PUSH(data, capa, size, 5);

    TEST_ASSERT_SIZE_T_EQ(4, size);
    TEST_ASSERT_INT_EQ(1, data[0]);
    TEST_ASSERT_INT_EQ(4, data[1]);
    TEST_ASSERT_INT_EQ(2, data[2]);
    TEST_ASSERT_INT_EQ(5, data[3]);

    TEST_ASSERT_INT_EQ(5, ONES_DYN_ARR_POP(data, size));
    TEST_ASSERT_SIZE_T_EQ(3, size);
    TEST_ASSERT_INT_EQ(2, ONES_DYN_ARR_LAST(data, size));
    TEST_ASSERT_INT_EQ(1, ONES_DYN_ARR_FIRST(data, size));

    /* grow beyond min cap to exercise reserve path */
    ONES_DYN_ARR_RESERVE(data, capa, 16);
    TEST_ASSERT(capa >= 16);

    ONES_DYN_ARR_CLEAR(size);
    TEST_ASSERT_SIZE_T_EQ(0, size);

    ONES_DYN_ARR_FREE(data);
    TEST_ASSERT(data == NULL);

    /* ---- double array ---- */
    double *ddata = NULL;
    size_t dcapa = 0;
    size_t dsize = 0;

    ONES_DYN_ARR_PUSH(ddata, dcapa, dsize, 1.0);
    ONES_DYN_ARR_PUSH(ddata, dcapa, dsize, 4.0);
    ONES_DYN_ARR_PUSH(ddata, dcapa, dsize, 2.0);
    ONES_DYN_ARR_PUSH(ddata, dcapa, dsize, 5.0);

    TEST_ASSERT_SIZE_T_EQ(4, dsize);
    TEST_ASSERT_DOUBLE_EQ(1.0, ddata[0]);
    TEST_ASSERT_DOUBLE_EQ(4.0, ddata[1]);
    TEST_ASSERT_DOUBLE_EQ(2.0, ddata[2]);
    TEST_ASSERT_DOUBLE_EQ(5.0, ddata[3]);

    ONES_DYN_ARR_POP(ddata, dsize);
    ONES_DYN_ARR_POP(ddata, dsize);
    TEST_ASSERT_SIZE_T_EQ(2, dsize);

    ONES_DYN_ARR_FREE(ddata);
    TEST_ASSERT(ddata == NULL);

    TEST_SUMMARY();
}