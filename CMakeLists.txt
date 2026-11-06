cmake_minimum_required(VERSION 3.10)
project(ones C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

enable_testing()
include(CTest)

# ---------------------------------------------------------------------------
# Unified test mode: OFF | ON | ASAN | UBSAN | COVERAGE | STRICT
#   OFF   - skip building tests
#   ON    - default warnings only
#   ASAN  - AddressSanitizer        (GCC/Clang only)
#   UBSAN - UndefinedBehaviorSanitizer (GCC/Clang only)
#   COVERAGE - gcov coverage        (GCC/Clang only)
#   STRICT - treat warnings as errors (-Werror / /WX)
# ---------------------------------------------------------------------------
set(ONES_TEST_MODE "ON" CACHE STRING "Test build mode: OFF=skip, ON=default, ASAN=AddressSanitizer, UBSAN=UndefinedBehaviorSanitizer, COVERAGE=gcov coverage, STRICT=-Werror")

set(ONES_BUILD_EXAMPLES ON CACHE BOOL "Build the main.c example")

set(ONES_BUILD_BENCHMARKS ON CACHE BOOL "Build the benchmark programs")

# ---------------------------------------------------------------------------
# Common warning flags
# ---------------------------------------------------------------------------
if(MSVC)
    set(ONES_TEST_WARNING_FLAGS /W4)
else()
    set(ONES_TEST_WARNING_FLAGS -Wall -Wextra)
endif()

# ---------------------------------------------------------------------------
# Mode-specific compile/link flags (GCC/Clang only; MSVC sanitizer/coverage skipped)
# ---------------------------------------------------------------------------
set(ONES_MODE_COMPILE_FLAGS "")
set(ONES_MODE_LINK_FLAGS "")

if(NOT MSVC)
    if(ONES_TEST_MODE STREQUAL "ASAN")
        set(ONES_MODE_COMPILE_FLAGS "-fsanitize=address -fno-omit-frame-pointer")
        set(ONES_MODE_LINK_FLAGS "-fsanitize=address")
    elseif(ONES_TEST_MODE STREQUAL "UBSAN")
        set(ONES_MODE_COMPILE_FLAGS "-fsanitize=undefined -fno-omit-frame-pointer")
        set(ONES_MODE_LINK_FLAGS "-fsanitize=undefined")
    elseif(ONES_TEST_MODE STREQUAL "COVERAGE")
        set(ONES_MODE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
        set(ONES_MODE_LINK_FLAGS "-fprofile-arcs -ftest-coverage")
    endif()
endif()

if(ONES_TEST_MODE STREQUAL "STRICT")
    if(MSVC)
        list(APPEND ONES_TEST_WARNING_FLAGS "/WX")
    else()
        list(APPEND ONES_TEST_WARNING_FLAGS "-Werror")
    endif()
endif()

# ---------------------------------------------------------------------------
# Helper: add a test executable + ctest entry
# ---------------------------------------------------------------------------
macro(add_ones_test name src)
    add_executable(${name} test/${src})
    target_include_directories(${name} PRIVATE .)
    # Each test target defines ONES_IMPLEMENTATION so ones_panic_at etc. are
    # linked into that single executable. Different executables are independent,
    # so there is no duplicate-symbol problem between them.
    target_compile_definitions(${name} PRIVATE ONES_IMPLEMENTATION)
    target_compile_options(${name} PRIVATE ${ONES_TEST_WARNING_FLAGS})
    if(ONES_MODE_COMPILE_FLAGS)
        target_compile_options(${name} PRIVATE ${ONES_MODE_COMPILE_FLAGS})
    endif()
    if(ONES_MODE_LINK_FLAGS)
        target_link_options(${name} PRIVATE ${ONES_MODE_LINK_FLAGS})
    endif()
    add_test(NAME ${name} COMMAND ${name})
endmacro()

# ---------------------------------------------------------------------------
# Helper: add a benchmark executable
# ---------------------------------------------------------------------------
macro(add_ones_bench name src)
    add_executable(${name} bench/${src})
    target_include_directories(${name} PRIVATE . bench)
    target_compile_definitions(${name} PRIVATE ONES_IMPLEMENTATION)
    target_compile_options(${name} PRIVATE ${ONES_TEST_WARNING_FLAGS})
    if(ONES_MODE_COMPILE_FLAGS)
        target_compile_options(${name} PRIVATE ${ONES_MODE_COMPILE_FLAGS})
    endif()
    if(ONES_MODE_LINK_FLAGS)
        target_link_options(${name} PRIVATE ${ONES_MODE_LINK_FLAGS})
    endif()
    add_test(NAME ${name} COMMAND ${name})
    set_tests_properties(${name} PROPERTIES LABELS "benchmark")
endmacro()

# ---------------------------------------------------------------------------
# Test targets
# ---------------------------------------------------------------------------
if(NOT ONES_TEST_MODE STREQUAL "OFF")
    add_ones_test(test_panic test_panic.c)
    set_tests_properties(test_panic PROPERTIES WILL_FAIL TRUE)

    add_ones_test(test_arr test_arr.c)
    add_ones_test(test_move test_move.c)
    add_ones_test(test_str test_str.c)
    add_ones_test(test_sv test_sv.c)
    add_ones_test(test_sb test_sb.c)
    add_ones_test(test_macros test_macros.c)
endif()

# ---------------------------------------------------------------------------
# Example
# ---------------------------------------------------------------------------
if(ONES_BUILD_EXAMPLES)
    add_executable(ones_example main.c)
    target_include_directories(ones_example PRIVATE .)
endif()
# ---------------------------------------------------------------------------
# Benchmarks
# ---------------------------------------------------------------------------
if(ONES_BUILD_BENCHMARKS)
    add_ones_bench(bench_arr bench_arr.c)
    add_ones_bench(bench_sv bench_sv.c)
    add_ones_bench(bench_sb bench_sb.c)
    add_ones_bench(bench_macros bench_macros.c)

    add_custom_target(ones_benchmarks
        DEPENDS bench_arr bench_sv bench_sb bench_macros)
endif()
