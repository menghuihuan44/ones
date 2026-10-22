#include "ones.h"
#include "test_utils.h"

int main(void) {
    TEST_RUN("panic");

    TEST_ASSERT(ones_panic != NULL);

    ones_panic("test panic message");

    TEST_SUMMARY();
}