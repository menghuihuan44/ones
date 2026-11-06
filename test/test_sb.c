#include "ones.h"
#include "test_utils.h"

#include <string.h>

int main(void) {
    TEST_RUN("string builder extended (ONES_SB_PUSH_FMT / PUSH_SV / PUSH_BUF / free & reuse)");

    /* ---- format, sv, buf ---- */
    {
        ones_str_builder sb;
        sb.c_str = NULL;
        sb.size = 0;
        sb.capa = 0;

        ones_sb_push_fmt(&sb, "count = %d", 7);
        ONES_SB_TERMINATE(&sb);
        TEST_ASSERT_STR_EQ("count = 7", sb.c_str);
        TEST_ASSERT_SIZE_T_EQ(9u, sb.size);

        ones_sb_push_fmt(&sb, " neg = %d", -3);
        ONES_SB_TERMINATE(&sb);
        TEST_ASSERT_STR_EQ("count = 7 neg = -3", sb.c_str);

        ones_str_view sv = ONES_SV_FROM_C_STR(" [sv]");
        ONES_SB_PUSH_SV(&sb, sv);
        ONES_SB_TERMINATE(&sb);
        TEST_ASSERT_STR_EQ("count = 7 neg = -3 [sv]", sb.c_str);
        TEST_ASSERT_SIZE_T_EQ(23u, sb.size);

        /* push zero-length buffer is a no-op */
        ONES_SB_PUSH_BUF(&sb, "", 0);
        ONES_SB_TERMINATE(&sb);
        TEST_ASSERT_SIZE_T_EQ(23u, sb.size);

        /* push fmt with multiple specifiers */
        ones_str_builder sb2;
        sb2.c_str = NULL;
        sb2.size = 0;
        sb2.capa = 0;
        ones_sb_push_fmt(&sb2, "%d %s %f", 1, "two", 3.0);
        ONES_SB_TERMINATE(&sb2);
        TEST_ASSERT_STR_EQ("1 two 3.000000", sb2.c_str);
        ONES_SB_FREE(&sb2);

        /* ---- build without terminate ---- */
        ones_str_builder raw;
        raw.c_str = NULL;
        raw.size = 0;
        raw.capa = 0;
        ONES_SB_PUSH(&raw, 'h');
        ONES_SB_PUSH(&raw, 'i');
        ones_str_view view = { .data = raw.c_str, .size = raw.size };
        TEST_ASSERT_SIZE_T_EQ(2u, view.size);
        TEST_ASSERT_MEM_EQ("hi", view.data, 2u);
        ONES_SB_FREE(&raw);

        /* ---- free & reuse ---- */
        ONES_SB_FREE(&sb);
        TEST_ASSERT_PTR_EQ(NULL, sb.c_str);
        TEST_ASSERT_SIZE_T_EQ(0u, sb.size);
        TEST_ASSERT_SIZE_T_EQ(0u, sb.capa);

        ONES_SB_PUSH_C_STR(&sb, "reused");
        ONES_SB_TERMINATE(&sb);
        TEST_ASSERT_STR_EQ("reused", sb.c_str);
        TEST_ASSERT_SIZE_T_EQ(6u, sb.size);
        ONES_SB_FREE(&sb);
    }

    TEST_SUMMARY();
}
