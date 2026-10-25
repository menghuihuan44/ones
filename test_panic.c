#include "ones.h"
#include "test_utils.h"

int main(void) {
    TEST_RUN("panic");

    TEST_ASSERT(ones_panic_at != NULL);

    ONES_PANIC("test panic message");

    TEST_SUMMARY();
}