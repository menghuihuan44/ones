#include "ones.h"
#include "test_utils.h"

#include <string.h>

int main(void) {
    TEST_RUN("string builder (ONES_SB_*)");

    /* ---- stack-allocated builder ---- */
    {
        ones_str_builder b;
        b.c_str = NULL;
        b.size = 0;
        b.capa = 0;

        TEST_ASSERT_PTR_EQ(NULL, b.c_str);
        TEST_ASSERT_SIZE_T_EQ(0u, b.size);
        TEST_ASSERT_SIZE_T_EQ(0u, b.capa);

        ONES_SB_PUSH(&b, 'a');
        ONES_SB_PUSH(&b, 'b');
        ONES_SB_PUSH(&b, 'c');
        ONES_SB_TERMINATE(&b);
        TEST_ASSERT_STR_EQ("abc", b.c_str);
        TEST_ASSERT_SIZE_T_EQ(3u, b.size);

        ONES_SB_PUSH_C_STR(&b, ", ");
        ONES_SB_TERMINATE(&b);
        TEST_ASSERT_STR_EQ("abc, ", b.c_str);
        TEST_ASSERT_SIZE_T_EQ(5u, b.size);

        ONES_SB_PUSH_C_STR(&b, "world");
        ONES_SB_TERMINATE(&b);
        TEST_ASSERT_STR_EQ("abc, world", b.c_str);
        TEST_ASSERT_SIZE_T_EQ(10u, b.size);

        /* push raw buffer */
        const char *extra = "!!!";
        ONES_SB_PUSH_BUF(&b, extra, strlen(extra));
        ONES_SB_TERMINATE(&b);
        TEST_ASSERT_STR_EQ("abc, world!!!", b.c_str);
        TEST_ASSERT_SIZE_T_EQ(13u, b.size);

        /* view over current contents */
        ones_str_view sv = { .data = b.c_str, .size = b.size };
        TEST_ASSERT_SIZE_T_EQ(13u, sv.size);
        TEST_ASSERT_MEM_EQ("abc, world!!!", sv.data, 13u);

        /* empty string push is a no-op */
        ONES_SB_PUSH_C_STR(&b, "");
        ONES_SB_TERMINATE(&b);
        TEST_ASSERT_SIZE_T_EQ(13u, b.size);

        /* zero-length buffer push is a no-op */
        ONES_SB_PUSH_BUF(&b, "ignored", 0);
        ONES_SB_TERMINATE(&b);
        TEST_ASSERT_SIZE_T_EQ(13u, b.size);

        /* ---- free & reuse ---- */
        ONES_SB_FREE(&b);
        TEST_ASSERT_PTR_EQ(NULL, b.c_str);
        TEST_ASSERT_SIZE_T_EQ(0u, b.size);
        TEST_ASSERT_SIZE_T_EQ(0u, b.capa);

        ONES_SB_PUSH(&b, 'x');
        ONES_SB_TERMINATE(&b);
        TEST_ASSERT_STR_EQ("x", b.c_str);
        TEST_ASSERT_SIZE_T_EQ(1u, b.size);

        ONES_SB_FREE(&b);
    }

    /* ---- ONES_SB_FROM? view via ONES_SV_FROM_SB ---- */
    {
        ones_str_builder b;
        b.c_str = NULL;
        b.size = 0;
        b.capa = 0;

        ONES_SB_PUSH_C_STR(&b, "hello");
        ones_str_view sv = ONES_SV_FROM_SB(&b);
        TEST_ASSERT_SIZE_T_EQ(5u, sv.size);
        TEST_ASSERT_MEM_EQ("hello", sv.data, 5u);

        ONES_SB_FREE(&b);
    }

    TEST_SUMMARY();
}
