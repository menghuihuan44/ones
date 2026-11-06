#include "ones.h"
#include "test_utils.h"

#include <ctype.h>
#include <string.h>

static int is_space_pred(int x) {
    return isspace(x);
}

int main(void) {
    TEST_RUN("string view (ones_sv_*)");

    /* ---- from_parts / from_c_str ---- */
    ones_str_view a = ones_sv_from_parts("hello", 5);
    TEST_ASSERT_SIZE_T_EQ(5, a.size);
    TEST_ASSERT(memcmp(a.data, "hello", 5) == 0);

    ones_str_view b = ONES_SV_FROM_C_STR("world");
    TEST_ASSERT_SIZE_T_EQ(5, b.size);
    TEST_ASSERT(memcmp(b.data, "world", 5) == 0);

    ones_str_view empty = ONES_SV_FROM_PARTS("", 0);
    TEST_ASSERT_SIZE_T_EQ(0, empty.size);
    TEST_ASSERT(empty.data != NULL);

    /* ---- is_equal ---- */
    TEST_ASSERT(ones_sv_is_equal(a, ONES_SV_FROM_PARTS("hello", 5)));
    TEST_ASSERT(!ones_sv_is_equal(a, b));
    TEST_ASSERT(ones_sv_is_equal(empty, empty));
    TEST_ASSERT(!ones_sv_is_equal(a, empty));

    /* ---- find ---- */
    ones_str_view hay = ONES_SV_FROM_C_STR("hello world, hello ctest");
    TEST_ASSERT_SIZE_T_EQ(6, ones_sv_find(hay, ONES_SV_FROM_C_STR("world")));
    TEST_ASSERT_SIZE_T_EQ(0, ones_sv_find(hay, ONES_SV_FROM_C_STR("hello")));
    TEST_ASSERT_SIZE_T_EQ(hay.size, ones_sv_find(hay, ONES_SV_FROM_C_STR("zzz")));
    TEST_ASSERT_SIZE_T_EQ(hay.size, ones_sv_find(hay, ONES_SV_FROM_C_STR("")));
    TEST_ASSERT_SIZE_T_EQ(0, ones_sv_find(empty, empty));

    /* ---- chop_while ---- */
    ones_str_view s = ONES_SV_FROM_C_STR("   trim me");
    ones_str_view lead = ones_sv_chop_while(&s, is_space_pred);
    TEST_ASSERT_SIZE_T_EQ(3, lead.size);
    TEST_ASSERT(memcmp(lead.data, "   ", 3) == 0);
    TEST_ASSERT_SIZE_T_EQ(7, s.size);
    TEST_ASSERT(memcmp(s.data, "trim me", 7) == 0);

    ones_str_view no_space = ONES_SV_FROM_C_STR("notrim");
    ones_str_view lead2 = ones_sv_chop_while(&no_space, is_space_pred);
    TEST_ASSERT_SIZE_T_EQ(0, lead2.size);
    TEST_ASSERT_SIZE_T_EQ(6, no_space.size);

    /* ---- chop_by_delim ---- */
    ones_str_view list = ONES_SV_FROM_C_STR("a,b,c");
    ones_str_view first = ones_sv_chop_by_delim(&list, ',');
    TEST_ASSERT_SIZE_T_EQ(1, first.size);
    TEST_ASSERT(first.data[0] == 'a');
    TEST_ASSERT_SIZE_T_EQ(3, list.size);
    TEST_ASSERT(memcmp(list.data, "b,c", 3) == 0);

    ones_str_view second = ones_sv_chop_by_delim(&list, ',');
    TEST_ASSERT_SIZE_T_EQ(1, second.size);
    TEST_ASSERT(second.data[0] == 'b');

    ones_str_view no_delim = ONES_SV_FROM_C_STR("nodelimiter");
    ones_str_view part = ones_sv_chop_by_delim(&no_delim, ',');
    TEST_ASSERT_SIZE_T_EQ(11, part.size);
    TEST_ASSERT_SIZE_T_EQ(0, no_delim.size);

    /* ---- chop_by_sv ---- */
    ones_str_view kv = ONES_SV_FROM_C_STR("key=value");
    ones_str_view key = ones_sv_chop_by_sv(&kv, ONES_SV_FROM_C_STR("="));
    TEST_ASSERT_SIZE_T_EQ(3, key.size);
    TEST_ASSERT(memcmp(key.data, "key", 3) == 0);
    TEST_ASSERT_SIZE_T_EQ(5, kv.size);
    TEST_ASSERT(memcmp(kv.data, "value", 5) == 0);

    ones_str_view noeq = ONES_SV_FROM_C_STR("nodelimiter");
    ones_str_view whole = ones_sv_chop_by_sv(&noeq, ONES_SV_FROM_C_STR("="));
    TEST_ASSERT_SIZE_T_EQ(11, whole.size);
    TEST_ASSERT_SIZE_T_EQ(0, noeq.size);

    /* ---- chop_left / chop_right ---- */
    ones_str_view lr = ONES_SV_FROM_C_STR("abcdef");
    ones_str_view left = ones_sv_chop_left(&lr, 2);
    TEST_ASSERT_SIZE_T_EQ(2, left.size);
    TEST_ASSERT(memcmp(left.data, "ab", 2) == 0);
    TEST_ASSERT_SIZE_T_EQ(4, lr.size);
    TEST_ASSERT(memcmp(lr.data, "cdef", 4) == 0);

    ones_str_view right = ones_sv_chop_right(&lr, 2);
    TEST_ASSERT_SIZE_T_EQ(2, right.size);
    TEST_ASSERT(memcmp(right.data, "ef", 2) == 0);
    TEST_ASSERT_SIZE_T_EQ(2, lr.size);
    TEST_ASSERT(memcmp(lr.data, "cd", 2) == 0);

    ones_str_view over = ONES_SV_FROM_C_STR("hi");
    ones_str_view l2 = ones_sv_chop_left(&over, 10);
    TEST_ASSERT_SIZE_T_EQ(2, l2.size);
    TEST_ASSERT_SIZE_T_EQ(0, over.size);

    /* ---- start_with / end_with ---- */
    TEST_ASSERT(ones_sv_start_with_sv(hay, ONES_SV_FROM_C_STR("hello")));
    TEST_ASSERT(!ones_sv_start_with_sv(hay, ONES_SV_FROM_C_STR("world")));
    TEST_ASSERT(ones_sv_end_with_sv(hay, ONES_SV_FROM_C_STR("ctest")));
    TEST_ASSERT(!ones_sv_end_with_sv(hay, ONES_SV_FROM_C_STR("ctests")));
    TEST_ASSERT(ones_sv_start_with_sv(empty, empty));
    TEST_ASSERT(ones_sv_end_with_sv(empty, empty));

    /* ---- chop_prefix / chop_suffix ---- */
    ones_str_view pref = ONES_SV_FROM_C_STR("prefix_rest");
    TEST_ASSERT(ones_sv_chop_prefix(&pref, ONES_SV_FROM_C_STR("prefix_")));
    TEST_ASSERT_SIZE_T_EQ(4, pref.size);
    TEST_ASSERT(memcmp(pref.data, "rest", 4) == 0);

    ones_str_view pref_fail = ONES_SV_FROM_C_STR("prefix_rest");
    TEST_ASSERT(!ones_sv_chop_prefix(&pref_fail, ONES_SV_FROM_C_STR("suffix_")));
    TEST_ASSERT_SIZE_T_EQ(11, pref_fail.size);

    ones_str_view suff = ONES_SV_FROM_C_STR("rest_suffix");
    TEST_ASSERT(ones_sv_chop_suffix(&suff, ONES_SV_FROM_C_STR("_suffix")));
    TEST_ASSERT_SIZE_T_EQ(4, suff.size);
    TEST_ASSERT(memcmp(suff.data, "rest", 4) == 0);

    ones_str_view suff_fail = ONES_SV_FROM_C_STR("rest_suffix");
    TEST_ASSERT(!ones_sv_chop_suffix(&suff_fail, ONES_SV_FROM_C_STR("_prefix")));
    TEST_ASSERT_SIZE_T_EQ(11, suff_fail.size);

    TEST_ASSERT(!ones_sv_chop_suffix(&suff, ONES_SV_FROM_C_STR("_suffix")));
    TEST_ASSERT_SIZE_T_EQ(4, suff.size);
    TEST_ASSERT(memcmp(suff.data, "rest", 4) == 0);

    /* ---- trim ---- */
    ones_str_view t = ONES_SV_FROM_C_STR("  hello world  ");
    TEST_ASSERT_SIZE_T_EQ(13, ones_sv_trim_left(t).size);
    TEST_ASSERT_SIZE_T_EQ(13, ones_sv_trim_right(t).size);
    TEST_ASSERT_SIZE_T_EQ(11, ones_sv_trim(t).size);
    TEST_ASSERT(memcmp(ones_sv_trim(t).data, "hello world", 11) == 0);
    TEST_ASSERT_SIZE_T_EQ(0, ones_sv_trim(ONES_SV_FROM_C_STR(" \t\n")).size);
    TEST_ASSERT_SIZE_T_EQ(0, ones_sv_trim(empty).size);

    /* ---- ONES_SV_FMT / ONES_SV_ARG ---- */
    char buf[64];
    int n = snprintf(buf, sizeof(buf), ONES_SV_FMT, ONES_SV_ARG(a));
    TEST_ASSERT_INT_EQ(5, n);
    TEST_ASSERT(memcmp(buf, "hello", 5) == 0);

    TEST_SUMMARY();
}