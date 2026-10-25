#include "ones.h"
#include "test_utils.h"

int main(void) {
    TEST_RUN("string");

    /* --- ones_str_view_triml / trimr / trim --- */

    ones_str_view v = ones_str_view_triml((ones_str_view){.data = "   hello", .size = 8});
    TEST_ASSERT_SIZE_T_EQ(5, v.size);
    TEST_ASSERT(memcmp(v.data, "hello", 5) == 0);

    v = ones_str_view_trimr((ones_str_view){.data = "hello   ", .size = 8});
    TEST_ASSERT_SIZE_T_EQ(5, v.size);
    TEST_ASSERT(memcmp(v.data, "hello", 5) == 0);

    v = ones_str_view_trim((ones_str_view){.data = "  hello world  ", .size = 15});
    TEST_ASSERT_SIZE_T_EQ(11, v.size);
    TEST_ASSERT(memcmp(v.data, "hello world", 11) == 0);

    /* empty / all-space view */
    v = ones_str_view_trim((ones_str_view){.data = "   ", .size = 3});
    TEST_ASSERT_SIZE_T_EQ(0, v.size);

    v = ones_str_view_trim((ones_str_view){.data = "", .size = 0});
    TEST_ASSERT_SIZE_T_EQ(0, v.size);

    /* --- ones_str_builder_init / deinit --- */

    ones_str_builder b;
    ones_str_builder_init(&b);
    TEST_ASSERT(b.c_str == NULL);
    TEST_ASSERT_SIZE_T_EQ(0, b.size);
    TEST_ASSERT_SIZE_T_EQ(0, b.capa);

    ones_str_builder_push_char(&b, 'a');
    ones_str_builder_push_char(&b, 'b');
    ones_str_builder_push_char(&b, 'c');
    TEST_ASSERT_STR_EQ("abc", b.c_str);
    TEST_ASSERT_SIZE_T_EQ(3, b.size);

    ones_str_builder_deinit(&b);
    TEST_ASSERT(b.c_str == NULL);
    TEST_ASSERT_SIZE_T_EQ(0, b.size);
    TEST_ASSERT_SIZE_T_EQ(0, b.capa);

    /* --- ones_str_builder_create / destroy --- */

    ones_str_builder *pb = ones_str_builder_create();
    TEST_ASSERT(pb != NULL);
    TEST_ASSERT(pb->c_str == NULL);
    TEST_ASSERT_SIZE_T_EQ(0, pb->size);
    TEST_ASSERT_SIZE_T_EQ(0, pb->capa);

    ones_str_builder_push_char(pb, 'H');
    ones_str_builder_push_char(pb, 'i');
    ones_str_builder_push_str(pb, ", ");
    ones_str_builder_push_str(pb, "world");
    ones_str_builder_push_char(pb, '!');
    TEST_ASSERT_STR_EQ("Hi, world!", pb->c_str);
    TEST_ASSERT_SIZE_T_EQ(10, pb->size);

    ones_str_view sv = ones_str_builder_as_str_view(pb);
    TEST_ASSERT_STR_EQ("Hi, world!", sv.data);
    TEST_ASSERT_SIZE_T_EQ(10, sv.size);

    /* empty string push is a no-op */
    ones_str_builder_push_str(pb, "");
    TEST_ASSERT_SIZE_T_EQ(10, pb->size);

    ones_str_builder_destroy(pb);

    TEST_SUMMARY();
}