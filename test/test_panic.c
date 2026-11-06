#include "ones.h"
#include "test_utils.h"

#ifdef _MSC_VER
#include <stdlib.h>
#endif

int main(void) {
    TEST_RUN("panic and assert");

    /* The panic entry point must be available. */
    TEST_ASSERT(ones_panic_at != NULL);

    /* ONES_ASSERT must be a no-op when its condition is true. */
    ONES_ASSERT(1 == 1, "this should not panic");
    ONES_ASSERT(true, "true condition");
    TEST_ASSERT(true);

#ifdef _MSC_VER
    /* Keep abort() from triggering the Windows fail-fast path in Release. */
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
#endif

    /* The remaining call aborts the process, so this test is registered
       with WILL_FAIL in CTest. */
    ONES_PANIC("intentional test panic");
}
