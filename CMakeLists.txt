cmake_minimum_required(VERSION 3.10)
project(ones C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

enable_testing()
include(CTest)

add_executable(test_panic test_panic.c)
target_include_directories(test_panic PRIVATE .)
add_test(NAME test_panic COMMAND test_panic)
set_tests_properties(test_panic PROPERTIES WILL_FAIL TRUE)

add_executable(test_arr test_arr.c)
target_include_directories(test_arr PRIVATE .)
add_test(NAME test_arr COMMAND test_arr)

add_executable(test_move test_move.c)
target_include_directories(test_move PRIVATE .)
add_test(NAME test_move COMMAND test_move)