#include "ones.h"
#include "test_utils.h"

ONES_DECL_ARR(int)
ONES_DECL_ARR(double)

int main(void) {
    TEST_RUN("dynamic array");

    ones_arr_int arr;
    ones_arr_init_int(&arr);

    TEST_ASSERT(arr.data == NULL);
    TEST_ASSERT_SIZE_T_EQ(0, arr.size);
    TEST_ASSERT_SIZE_T_EQ(0, arr.capa);

    ones_arr_push_back_int(&arr, 1);
    ones_arr_push_back_int(&arr, 4);
    ones_arr_push_back_int(&arr, 2);
    ones_arr_push_back_int(&arr, 5);

    TEST_ASSERT_SIZE_T_EQ(4, arr.size);
    TEST_ASSERT_INT_EQ(1, arr.data[0]);
    TEST_ASSERT_INT_EQ(4, arr.data[1]);
    TEST_ASSERT_INT_EQ(2, arr.data[2]);
    TEST_ASSERT_INT_EQ(5, arr.data[3]);

    ones_arr_pop_back_int(&arr);
    TEST_ASSERT_SIZE_T_EQ(3, arr.size);
    TEST_ASSERT_INT_EQ(2, arr.data[2]);

    ones_arr_pop_back_int(&arr);
    ones_arr_pop_back_int(&arr);
    ones_arr_pop_back_int(&arr);
    TEST_ASSERT_SIZE_T_EQ(0, arr.size);

    ones_arr_deinit_int(&arr);
    TEST_ASSERT(arr.data == NULL);
    TEST_ASSERT_SIZE_T_EQ(0, arr.size);
    TEST_ASSERT_SIZE_T_EQ(0, arr.capa);

    ones_arr_double *darr = ones_arr_create_double();
    TEST_ASSERT(darr != NULL);
    TEST_ASSERT(darr->data == NULL);
    TEST_ASSERT_SIZE_T_EQ(0, darr->size);

    ones_arr_push_back_double(darr, 1.0);
    ones_arr_push_back_double(darr, 4.0);
    ones_arr_push_back_double(darr, 2.0);
    ones_arr_push_back_double(darr, 5.0);

    TEST_ASSERT_SIZE_T_EQ(4, darr->size);
    TEST_ASSERT_DOUBLE_EQ(1.0, darr->data[0]);
    TEST_ASSERT_DOUBLE_EQ(4.0, darr->data[1]);
    TEST_ASSERT_DOUBLE_EQ(2.0, darr->data[2]);
    TEST_ASSERT_DOUBLE_EQ(5.0, darr->data[3]);

    ones_arr_pop_back_double(darr);
    ones_arr_pop_back_double(darr);
    TEST_ASSERT_SIZE_T_EQ(2, darr->size);

    ones_arr_destroy_double(darr);

    TEST_SUMMARY();
}