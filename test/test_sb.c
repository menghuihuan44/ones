#include "ones.h"
#include "test_utils.h"

#include <string.h>

int main(void) {
    TEST_RUN("string builder extended (format-via-buf / ONES_SB_PUSH_SV / ONES_SB_PUSH_BUF / free & reuse)");

    ones_str_builder sb;
    sb.c_str = NULL;
    sb.size = 0;
    sb.capa = 0;

    ones_sb_push_fmt(&sb, "count = %d", 7);
    ONES_SB_TERMINATE(&sb);
    TEST_ASSERT_STR_EQ("count = 7", sb.c_str);
    TEST_ASSERT_SIZE_T_EQ(9, sb.size);

    ones_sb_push_fmt(&sb, " neg = %d", -3);
    ONES_SB_TERMINATE(&sb);
    TEST_ASSERT_STR_EQ("count = 7 neg = -3", sb.c_str);

    ones_str_view sv = ONES_SV_FROM_C_STR(" [sv]");
    ONES_SB_PUSH_SV(&sb, sv);
    ONES_SB_TERMINATE(&sb);
    TEST_ASSERT_STR_EQ("count = 7 neg = -3 [sv]", sb.c_str);
    TEST_ASSERT_SIZE_T_EQ(23, sb.size);

    /* push zero-length buffer is a no-op */
    ONES_SB_PUSH_BUF(&sb, "", 0);
    ONES_SB_TERMINATE(&sb);
    TEST_ASSERT_SIZE_T_EQ(23, sb.size);

    /* build without terminate: size is the source of truth */
    ones_str_builder raw;
    raw.c_str = NULL;
    raw.size = 0;
    raw.capa = 0;
    ONES_SB_PUSH(&raw, 'h');
    ONES_SB_PUSH(&raw, 'i');
    ones_str_view view = { .data = raw.c_str, .size = raw.size };
    TEST_ASSERT_SIZE_T_EQ(2, view.size);
    TEST_ASSERT(memcmp(view.data, "hi", 2) == 0);
    ONES_SB_FREE(&raw);

    /* free & reuse */
    ONES_SB_FREE(&sb);
    TEST_ASSERT(sb.c_str == NULL);
    TEST_ASSERT_SIZE_T_EQ(0, sb.size);
    TEST_ASSERT_SIZE_T_EQ(0, sb.capa);

    ONES_SB_PUSH_C_STR(&sb, "reused");
    ONES_SB_TERMINATE(&sb);
    TEST_ASSERT_STR_EQ("reused", sb.c_str);
    TEST_ASSERT_SIZE_T_EQ(6, sb.size);
    ONES_SB_FREE(&sb);

    TEST_SUMMARY();
}