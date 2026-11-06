#include "ones.h"
#include "test_utils.h"

#include <stdlib.h>

int main(void) {
    TEST_RUN("pointer move (ONES_MOVE_PTR)");

    /* Basic move: ownership transfers and source becomes NULL. */
    {
        int *p = (int *)malloc(sizeof(int));
        TEST_ASSERT(p != NULL);
        *p = 42;

        int *d = NULL;
        int *c = p;

        ONES_MOVE_PTR(&d, p);

        TEST_ASSERT_PTR_EQ(NULL, p);
        TEST_ASSERT_PTR_EQ(c, d);
        TEST_ASSERT_INT_EQ(42, *d);

        free(d);
    }

    /* Move from NULL leaves both pointers NULL. */
    {
        int *src = NULL;
        int *dst = (int *)0x1; /* non-NULL sentinel, will be overwritten */
        (void)dst;

        int *actual_dst = NULL;
        ONES_MOVE_PTR(&actual_dst, src);

        TEST_ASSERT_PTR_EQ(NULL, src);
        TEST_ASSERT_PTR_EQ(NULL, actual_dst);
    }

    /* Moving into an existing pointer overwrites it (caller owns cleanup). */
    {
        int *old_dst = (int *)malloc(sizeof(int));
        int *src = (int *)malloc(sizeof(int));
        TEST_ASSERT(old_dst != NULL);
        TEST_ASSERT(src != NULL);

        *src = 7;
        ONES_MOVE_PTR(&old_dst, src);

        TEST_ASSERT_PTR_EQ(NULL, src);
        TEST_ASSERT_INT_EQ(7, *old_dst);

        free(old_dst);
    }

    TEST_SUMMARY();
}
