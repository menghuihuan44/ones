cmake_minimum_required(VERSION 3.10)
project(ones C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

enable_testing()
include(CTest)

# Common compiler warnings for all test targets
if(MSVC)
    set(ONES_TEST_WARNING_FLAGS "/W4")
else()
    set(ONES_TEST_WARNING_FLAGS "-Wall -Wextra")
endif()

add_executable(test_panic test_panic.c)
target_include_directories(test_panic PRIVATE .)
target_compile_options(test_panic PRIVATE ${ONES_TEST_WARNING_FLAGS})
add_test(NAME test_panic COMMAND test_panic)
set_tests_properties(test_panic PROPERTIES WILL_FAIL TRUE)

add_executable(test_arr test_arr.c)
target_include_directories(test_arr PRIVATE .)
target_compile_options(test_arr PRIVATE ${ONES_TEST_WARNING_FLAGS})
add_test(NAME test_arr COMMAND test_arr)

add_executable(test_move test_move.c)
target_include_directories(test_move PRIVATE .)
target_compile_options(test_move PRIVATE ${ONES_TEST_WARNING_FLAGS})
add_test(NAME test_move COMMAND test_move)

add_executable(test_str test_str.c)
target_include_directories(test_str PRIVATE .)
target_compile_options(test_str PRIVATE ${ONES_TEST_WARNING_FLAGS})
add_test(NAME test_str COMMAND test_str)