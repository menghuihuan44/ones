#include "ones.h"
#include "test_utils.h"

#include <ctype.h>
#include <string.h>

static int is_space_pred(int x) {
    return isspace(x);
}

static int is_digit_pred(int x) {
    return isdigit(x);
}

int main(void) {
    TEST_RUN("string view (ones_sv_*)");

    /* ---- from_parts / from_c_str ---- */
    {
        ones_str_view a = ones_sv_from_parts("hello", 5);
        TEST_ASSERT_SIZE_T_EQ(5u, a.size);
        TEST_ASSERT_MEM_EQ("hello", a.data, 5u);

        ones_str_view b = ONES_SV_FROM_C_STR("world");
        TEST_ASSERT_SIZE_T_EQ(5u, b.size);
        TEST_ASSERT_MEM_EQ("world", b.data, 5u);

        ones_str_view empty = ONES_SV_FROM_PARTS("", 0);
        TEST_ASSERT_SIZE_T_EQ(0u, empty.size);
        TEST_ASSERT(empty.data != NULL);
    }

    /* ---- is_equal ---- */
    {
        ones_str_view a = ONES_SV_FROM_C_STR("hello");
        TEST_ASSERT(ones_sv_is_equal(a, ONES_SV_FROM_PARTS("hello", 5)));
        TEST_ASSERT(!ones_sv_is_equal(a, ONES_SV_FROM_C_STR("world")));
        TEST_ASSERT(ones_sv_is_equal(ONES_SV_FROM_PARTS("", 0), ONES_SV_FROM_PARTS("", 0)));
        TEST_ASSERT(!ones_sv_is_equal(a, ONES_SV_FROM_PARTS("", 0)));
        TEST_ASSERT(!ones_sv_is_equal(ONES_SV_FROM_C_STR("abc"), ONES_SV_FROM_C_STR("abcd")));
    }

    /* ---- find ---- */
    {
        ones_str_view hay = ONES_SV_FROM_C_STR("hello world, hello ctest");
        TEST_ASSERT_SIZE_T_EQ(6u, ones_sv_find(hay, ONES_SV_FROM_C_STR("world")));
        TEST_ASSERT_SIZE_T_EQ(0u, ones_sv_find(hay, ONES_SV_FROM_C_STR("hello")));
        TEST_ASSERT_SIZE_T_EQ(hay.size, ones_sv_find(hay, ONES_SV_FROM_C_STR("zzz")));
        TEST_ASSERT_SIZE_T_EQ(hay.size, ones_sv_find(hay, ONES_SV_FROM_C_STR("")));
        TEST_ASSERT_SIZE_T_EQ(0u, ones_sv_find(ONES_SV_FROM_PARTS("", 0), ONES_SV_FROM_PARTS("", 0)));
        TEST_ASSERT_SIZE_T_EQ(hay.size, ones_sv_find(hay, ONES_SV_FROM_C_STR("this is way too long")));
        TEST_ASSERT_SIZE_T_EQ(13u, ones_sv_find(hay, ONES_SV_FROM_C_STR("hello ctest")));
    }

    /* ---- chop_while ---- */
    {
        ones_str_view s = ONES_SV_FROM_C_STR("   trim me");
        ones_str_view lead = ones_sv_chop_while(&s, is_space_pred);
        TEST_ASSERT_SIZE_T_EQ(3u, lead.size);
        TEST_ASSERT_MEM_EQ("   ", lead.data, 3u);
        TEST_ASSERT_SIZE_T_EQ(7u, s.size);
        TEST_ASSERT_MEM_EQ("trim me", s.data, 7u);

        ones_str_view no_space = ONES_SV_FROM_C_STR("notrim");
        ones_str_view lead2 = ones_sv_chop_while(&no_space, is_space_pred);
        TEST_ASSERT_SIZE_T_EQ(0u, lead2.size);
        TEST_ASSERT_SIZE_T_EQ(6u, no_space.size);

        ones_str_view digits = ONES_SV_FROM_C_STR("123abc");
        ones_str_view d = ones_sv_chop_while(&digits, is_digit_pred);
        TEST_ASSERT_SIZE_T_EQ(3u, d.size);
        TEST_ASSERT_MEM_EQ("123", d.data, 3u);
        TEST_ASSERT_SIZE_T_EQ(3u, digits.size);
        TEST_ASSERT_MEM_EQ("abc", digits.data, 3u);

        ones_str_view empty = ONES_SV_FROM_C_STR("");
        ones_str_view e = ones_sv_chop_while(&empty, is_space_pred);
        TEST_ASSERT_SIZE_T_EQ(0u, e.size);
        TEST_ASSERT_SIZE_T_EQ(0u, empty.size);
    }

    /* ---- chop_by_delim ---- */
    {
        ones_str_view list = ONES_SV_FROM_C_STR("a,b,c");
        ones_str_view first = ones_sv_chop_by_delim(&list, ',');
        TEST_ASSERT_SIZE_T_EQ(1u, first.size);
        TEST_ASSERT(first.data[0] == 'a');
        TEST_ASSERT_SIZE_T_EQ(3u, list.size);
        TEST_ASSERT_MEM_EQ("b,c", list.data, 3u);

        ones_str_view second = ones_sv_chop_by_delim(&list, ',');
        TEST_ASSERT_SIZE_T_EQ(1u, second.size);
        TEST_ASSERT(second.data[0] == 'b');

        ones_str_view no_delim = ONES_SV_FROM_C_STR("nodelimiter");
        ones_str_view part = ones_sv_chop_by_delim(&no_delim, ',');
        TEST_ASSERT_SIZE_T_EQ(11u, part.size);
        TEST_ASSERT_SIZE_T_EQ(0u, no_delim.size);

        ones_str_view empty = ONES_SV_FROM_C_STR("");
        ones_str_view ep = ones_sv_chop_by_delim(&empty, ',');
        TEST_ASSERT_SIZE_T_EQ(0u, ep.size);
        TEST_ASSERT_SIZE_T_EQ(0u, empty.size);
    }

    /* ---- chop_by_sv ---- */
    {
        ones_str_view kv = ONES_SV_FROM_C_STR("key=value");
        ones_str_view key = ones_sv_chop_by_sv(&kv, ONES_SV_FROM_C_STR("="));
        TEST_ASSERT_SIZE_T_EQ(3u, key.size);
        TEST_ASSERT_MEM_EQ("key", key.data, 3u);
        TEST_ASSERT_SIZE_T_EQ(5u, kv.size);
        TEST_ASSERT_MEM_EQ("value", kv.data, 5u);

        ones_str_view noeq = ONES_SV_FROM_C_STR("nodelimiter");
        ones_str_view whole = ones_sv_chop_by_sv(&noeq, ONES_SV_FROM_C_STR("="));
        TEST_ASSERT_SIZE_T_EQ(11u, whole.size);
        TEST_ASSERT_SIZE_T_EQ(0u, noeq.size);

        ones_str_view s = ONES_SV_FROM_C_STR("abc");
        ones_str_view w = ones_sv_chop_by_sv(&s, ONES_SV_FROM_C_STR(""));
        TEST_ASSERT_SIZE_T_EQ(3u, w.size);
        TEST_ASSERT_SIZE_T_EQ(0u, s.size);
    }
    /* ---- chop_left / chop_right ---- */
    {
        ones_str_view lr = ONES_SV_FROM_C_STR("abcdef");
        ones_str_view left = ones_sv_chop_left(&lr, 2);
        TEST_ASSERT_SIZE_T_EQ(2u, left.size);
        TEST_ASSERT_MEM_EQ("ab", left.data, 2u);
        TEST_ASSERT_SIZE_T_EQ(4u, lr.size);
        TEST_ASSERT_MEM_EQ("cdef", lr.data, 4u);

        ones_str_view right = ones_sv_chop_right(&lr, 2);
        TEST_ASSERT_SIZE_T_EQ(2u, right.size);
        TEST_ASSERT_MEM_EQ("ef", right.data, 2u);
        TEST_ASSERT_SIZE_T_EQ(2u, lr.size);
        TEST_ASSERT_MEM_EQ("cd", lr.data, 2u);

        ones_str_view z = ONES_SV_FROM_C_STR("hi");
        ones_str_view zl = ones_sv_chop_left(&z, 0);
        TEST_ASSERT_SIZE_T_EQ(0u, zl.size);
        TEST_ASSERT_SIZE_T_EQ(2u, z.size);

        ones_str_view over = ONES_SV_FROM_C_STR("hi");
        ones_str_view l2 = ones_sv_chop_left(&over, 10);
        TEST_ASSERT_SIZE_T_EQ(2u, l2.size);
        TEST_ASSERT_SIZE_T_EQ(0u, over.size);

        ones_str_view over2 = ONES_SV_FROM_C_STR("hi");
        ones_str_view r2 = ones_sv_chop_right(&over2, 10);
        TEST_ASSERT_SIZE_T_EQ(2u, r2.size);
        TEST_ASSERT_SIZE_T_EQ(0u, over2.size);
    }

    /* ---- start_with / end_with ---- */
    {
        ones_str_view hay = ONES_SV_FROM_C_STR("hello world, hello ctest");
        TEST_ASSERT(ones_sv_start_with_sv(hay, ONES_SV_FROM_C_STR("hello")));
        TEST_ASSERT(!ones_sv_start_with_sv(hay, ONES_SV_FROM_C_STR("world")));
        TEST_ASSERT(ones_sv_end_with_sv(hay, ONES_SV_FROM_C_STR("ctest")));
        TEST_ASSERT(!ones_sv_end_with_sv(hay, ONES_SV_FROM_C_STR("ctests")));

        ones_str_view empty = ONES_SV_FROM_PARTS("", 0);
        TEST_ASSERT(ones_sv_start_with_sv(empty, empty));
        TEST_ASSERT(ones_sv_end_with_sv(empty, empty));
        TEST_ASSERT(ones_sv_start_with_sv(hay, empty));
        TEST_ASSERT(ones_sv_end_with_sv(hay, empty));
    }

    /* ---- chop_prefix / chop_suffix ---- */
    {
        ones_str_view pref = ONES_SV_FROM_C_STR("prefix_rest");
        TEST_ASSERT(ones_sv_chop_prefix(&pref, ONES_SV_FROM_C_STR("prefix_")));
        TEST_ASSERT_SIZE_T_EQ(4u, pref.size);
        TEST_ASSERT_MEM_EQ("rest", pref.data, 4u);

        ones_str_view pref_fail = ONES_SV_FROM_C_STR("prefix_rest");
        TEST_ASSERT(!ones_sv_chop_prefix(&pref_fail, ONES_SV_FROM_C_STR("suffix_")));
        TEST_ASSERT_SIZE_T_EQ(11u, pref_fail.size);

        ones_str_view suff = ONES_SV_FROM_C_STR("rest_suffix");
        TEST_ASSERT(ones_sv_chop_suffix(&suff, ONES_SV_FROM_C_STR("_suffix")));
        TEST_ASSERT_SIZE_T_EQ(4u, suff.size);
        TEST_ASSERT_MEM_EQ("rest", suff.data, 4u);

        ones_str_view suff_fail = ONES_SV_FROM_C_STR("rest_suffix");
        TEST_ASSERT(!ones_sv_chop_suffix(&suff_fail, ONES_SV_FROM_C_STR("_prefix")));
        TEST_ASSERT_SIZE_T_EQ(11u, suff_fail.size);

        TEST_ASSERT(!ones_sv_chop_suffix(&suff, ONES_SV_FROM_C_STR("_suffix")));
        TEST_ASSERT_SIZE_T_EQ(4u, suff.size);

        ones_str_view ep = ONES_SV_FROM_C_STR("abc");
        TEST_ASSERT(ones_sv_chop_prefix(&ep, ONES_SV_FROM_PARTS("", 0)));
        TEST_ASSERT_SIZE_T_EQ(3u, ep.size);

        ones_str_view es = ONES_SV_FROM_C_STR("abc");
        TEST_ASSERT(ones_sv_chop_suffix(&es, ONES_SV_FROM_PARTS("", 0)));
        TEST_ASSERT_SIZE_T_EQ(3u, es.size);
    }

    /* ---- trim ---- */
    {
        ones_str_view t = ONES_SV_FROM_C_STR("  hello world  ");
        TEST_ASSERT_SIZE_T_EQ(13u, ones_sv_trim_left(t).size);
        TEST_ASSERT_SIZE_T_EQ(13u, ones_sv_trim_right(t).size);
        TEST_ASSERT_SIZE_T_EQ(11u, ones_sv_trim(t).size);
        TEST_ASSERT_MEM_EQ("hello world", ones_sv_trim(t).data, 11u);

        TEST_ASSERT_SIZE_T_EQ(0u, ones_sv_trim(ONES_SV_FROM_C_STR(" \t\n")).size);
        TEST_ASSERT_SIZE_T_EQ(0u, ones_sv_trim(ONES_SV_FROM_PARTS("", 0)).size);

        ones_str_view nw = ONES_SV_FROM_C_STR("abc");
        TEST_ASSERT_SIZE_T_EQ(3u, ones_sv_trim(nw).size);
        TEST_ASSERT_MEM_EQ("abc", ones_sv_trim(nw).data, 3u);
    }

    /* ---- ONES_SV_FMT / ONES_SV_ARG ---- */
    {
        ones_str_view a = ONES_SV_FROM_C_STR("hello");
        char buf[64];
        int n = snprintf(buf, sizeof(buf), ONES_SV_FMT, ONES_SV_ARG(a));
        TEST_ASSERT_INT_EQ(5, n);
        TEST_ASSERT_MEM_EQ("hello", buf, 5u);
    }

    /* ---- ONES_SV_FROM_SB ---- */
    {
        ones_str_builder b;
        b.c_str = NULL;
        b.size = 0;
        b.capa = 0;
        ONES_SB_PUSH_C_STR(&b, "fromsb");
        ones_str_view sv = ONES_SV_FROM_SB(&b);
        TEST_ASSERT_SIZE_T_EQ(6u, sv.size);
        TEST_ASSERT_MEM_EQ("fromsb", sv.data, 6u);
        ONES_SB_FREE(&b);
    }

    TEST_SUMMARY();
}

