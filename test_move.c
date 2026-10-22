#include "ones.h"
#include "test_utils.h"

int main(void) {
    TEST_RUN("move");

    int *p = (int *)malloc(sizeof(int));
    TEST_ASSERT(p != NULL);

    int *d = NULL, *c = p;
    ONES_MOVE(&d, p);
    TEST_ASSERT(p == NULL);
    TEST_ASSERT(d != NULL);
    TEST_ASSERT(d == c);

    TEST_SUMMARY();
}