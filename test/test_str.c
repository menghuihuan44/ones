#include "ones.h"
#include "test_utils.h"

#include <string.h>

int main(void) {
    TEST_RUN("string builder (ONES_SB_*)");

    /* ---- stack-allocated builder ---- */
    ones_str_builder b;
    b.c_str = NULL;
    b.size  = 0;
    b.capa  = 0;

    TEST_ASSERT(b.c_str == NULL);
    TEST_ASSERT_SIZE_T_EQ(0, b.size);
    TEST_ASSERT_SIZE_T_EQ(0, b.capa);

    ONES_SB_PUSH(&b, 'a');
    ONES_SB_PUSH(&b, 'b');
    ONES_SB_PUSH(&b, 'c');
    ONES_SB_SET_NULL(&b);
    TEST_ASSERT_STR_EQ("abc", b.c_str);
    TEST_ASSERT_SIZE_T_EQ(3, b.size);

    ONES_SB_PUSH_C_STR(&b, ", ");
    ONES_SB_SET_NULL(&b);
    TEST_ASSERT_STR_EQ("abc, ", b.c_str);
    TEST_ASSERT_SIZE_T_EQ(5, b.size);

    ONES_SB_PUSH_C_STR(&b, "world");
    ONES_SB_SET_NULL(&b);
    TEST_ASSERT_STR_EQ("abc, world", b.c_str);
    TEST_ASSERT_SIZE_T_EQ(10, b.size);

    /* push raw buffer */
    const char *extra = "!!!";
    ONES_SB_PUSH_BUF(&b, extra, strlen(extra));
    ONES_SB_SET_NULL(&b);
    TEST_ASSERT_STR_EQ("abc, world!!!", b.c_str);
    TEST_ASSERT_SIZE_T_EQ(13, b.size);

    /* view over current contents (not NUL-terminated, use size) */
    ones_str_view sv = { .data = b.c_str, .size = b.size };
    TEST_ASSERT_SIZE_T_EQ(13, sv.size);
    TEST_ASSERT(memcmp(sv.data, "abc, world!!!", 13) == 0);

    /* empty string push is a no-op via ONES_SB_PUSH_C_STR (strlen("") == 0) */
    ONES_SB_PUSH_C_STR(&b, "");
    ONES_SB_SET_NULL(&b);
    TEST_ASSERT_SIZE_T_EQ(13, b.size);

    /* ---- free & reuse ---- */
    ONES_SB_FREE(&b);
    TEST_ASSERT(b.c_str == NULL);
    TEST_ASSERT_SIZE_T_EQ(0, b.size);
    TEST_ASSERT_SIZE_T_EQ(0, b.capa);

    /* after free, push should reallocate from scratch */
    ONES_SB_PUSH(&b, 'x');
    ONES_SB_SET_NULL(&b);
    TEST_ASSERT_STR_EQ("x", b.c_str);
    TEST_ASSERT_SIZE_T_EQ(1, b.size);

    ONES_SB_FREE(&b);

    TEST_SUMMARY();
}