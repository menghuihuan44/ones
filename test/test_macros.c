#include "ones.h"
#include "test_utils.h"

#include <string.h>

int main(void) {
    TEST_RUN("utility macros (ONES_MAX/MIN/SWAP_T/ARR_*/DYN_REMOVE_UNORDERED)");

    TEST_ASSERT_INT_EQ(7, ONES_MAX(3, 7));
    TEST_ASSERT_INT_EQ(3, ONES_MIN(3, 7));
    TEST_ASSERT_INT_EQ(7, ONES_MAX(7, 7));
    TEST_ASSERT_INT_EQ(-5, ONES_MIN(-5, -2));
    TEST_ASSERT_INT_EQ(3, ONES_MAX(-3, 3));
    TEST_ASSERT_INT_EQ(-3, ONES_MIN(-3, 3));

    int x = 1, y = 2;
    ONES_SWAP_T(int, x, y);
    TEST_ASSERT_INT_EQ(2, x);
    TEST_ASSERT_INT_EQ(1, y);

    int arr[6];
    TEST_ASSERT_SIZE_T_EQ(6, ONES_ARR_LEN(arr));

    char *str_arr[3];
    TEST_ASSERT_SIZE_T_EQ(3, ONES_ARR_LEN(str_arr));

    int arr2[3] = {10, 20, 30};
    TEST_ASSERT_INT_EQ(10, ONES_ARR_AT(arr2, 3, 0));
    TEST_ASSERT_INT_EQ(20, ONES_ARR_AT(arr2, 3, 1));
    TEST_ASSERT_INT_EQ(30, ONES_ARR_AT(arr2, 3, 2));

    /* ONES_DYN_REMOVE_UNORDERED: remove index 1, last element fills the hole */
    int *data = NULL;
    size_t capa = 0, size = 0;
    ONES_DYN_ARR_PUSH(data, capa, size, 1);
    ONES_DYN_ARR_PUSH(data, capa, size, 2);
    ONES_DYN_ARR_PUSH(data, capa, size, 3);
    ONES_DYN_ARR_PUSH(data, capa, size, 4);
    TEST_ASSERT_SIZE_T_EQ(4, size);
    ONES_DYN_REMOVE_UNORDERED(data, size, 1);
    TEST_ASSERT_SIZE_T_EQ(3, size);
    TEST_ASSERT_INT_EQ(1, data[0]);
    TEST_ASSERT_INT_EQ(4, data[1]); /* unordered: tail moved here */
    TEST_ASSERT_INT_EQ(3, data[2]);
    ONES_DYN_ARR_FREE(data);
    TEST_ASSERT(data == NULL);

    /* remove last element */
    int *d2 = NULL;
    size_t c2 = 0, s2 = 0;
    ONES_DYN_ARR_PUSH(d2, c2, s2, 100);
    ONES_DYN_ARR_PUSH(d2, c2, s2, 200);
    ONES_DYN_REMOVE_UNORDERED(d2, s2, 1);
    TEST_ASSERT_SIZE_T_EQ(1, s2);
    TEST_ASSERT_INT_EQ(100, d2[0]);
    ONES_DYN_ARR_FREE(d2);

    TEST_SUMMARY();
}