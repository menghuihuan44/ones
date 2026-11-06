# ones

**ones** is a lightweight, single-header C utility library that provides panic handling, pointer move semantics, and type-generic dynamic arrays. Just drop `ones.h` into your project and start coding — no build step, no external dependencies, no linking required.

## Table of Contents

- [Features](#features)
- [Requirements](#requirements)
- [Quick Start](#quick-start)
  - [1. Add `ones.h` to Your Project](#1-add-onesh-to-your-project)
  - [2. Include and Use](#2-include-and-use)
  - [3. Compile Your Program](#3-compile-your-program)
- [API Reference](#api-reference)
  - [Panic & Assert](#panic--assert)
  - [Pointer Move](#pointer-move)
  - [Dynamic Array Macros](#dynamic-array-macros)
  - [String Builder Macros](#string-builder-macros)
- [Code Examples](#code-examples)
  - [Example 1: Dynamic Array of Integers](#example-1-dynamic-array-of-integers)
  - [Example 2: Dynamic Array of Doubles](#example-2-dynamic-array-of-doubles)
  - [Example 3: Pointer Move](#example-3-pointer-move)
  - [Example 4: Panic on Invalid Input](#example-4-panic-on-invalid-input)
- [Build and Run Tests](#build-and-run-tests)
- [Project Structure](#project-structure)
- [License](#license)
- [Contributing](#contributing)

## Features

| Feature | Description |
|---------|-------------|
| **Single Header** | Everything lives in `ones.h`. Copy it into your project and `#include "ones.h"`. No `.c` files to compile, no libraries to link. |
| **Panic Handling** | `ONES_PANIC()` prints a descriptive error message (with file name and line number) to `stderr` and aborts the program. |
| **Pointer Move** | `ONES_MOVE_PTR(des, src)` safely transfers pointer ownership and sets the source to `NULL`, preventing double-free and dangling pointer bugs. |
| **Type-Generic Dynamic Array** | `ONES_DYN_ARR_*` macros provide low-level dynamic array operations (reserve, push, pop, free, etc.) for any pointer/type. Works with `int`, `double`, `struct`, etc. |
| **C99 Compliant** | Written in standard C99, works with GCC, Clang, and MSVC. |
| **Built-in Tests** | Includes CMake + CTest unit tests for panic, dynamic arrays, and pointer move. |

## Requirements

- A C99-compatible compiler (GCC, Clang, or MSVC)
- CMake 3.10+ (only needed if you want to build the included tests)
- Standard C library (`stdio.h`, `stdlib.h`, `stdbool.h`, `stdarg.h`)

## Quick Start

### 1. Add `ones.h` to Your Project

Copy the file `ones.h` from this repository into your project's source or include directory:

```bash
# Example: copy into your project's include folder
cp path/to/ones/ones.h your_project/include/
```

### 2. Include and Use

In your C source file:

```c
#include "ones.h"
```

If you need dynamic arrays, use the `ONES_DYN_ARR_*` macros directly on a pointer/`size_t`/`size_t` triple (`data`, `capa`, `size`):

```c
int *data = NULL;
size_t capa = 0, size = 0;
ONES_DYN_ARR_PUSH(data, capa, size, 42);
ONES_DYN_ARR_PUSH(data, capa, size, 7);
ONES_DYN_ARR_FREE(data);
```

### 3. Compile Your Program

Because `ones.h` is header-only, you compile your source file directly:

**GCC:**
```bash
gcc -std=c99 -Wall -Wextra your_program.c -o your_program
```

**Clang:**
```bash
clang -std=c99 -Wall -Wextra your_program.c -o your_program
```

**MSVC (Developer Command Prompt):**
```cmd
cl /std:c11 /W4 your_program.c
```

No extra include paths are needed as long as `ones.h` is in the same directory as your source file (or in your include path).

---

## API Reference

### Panic & Assert

#### `ONES_PANIC(...)`

```c
#define ONES_PANIC(...) ones_panic_at(__FILE__, __LINE__, __VA_ARGS__)
```

Prints a panic message to `stderr` with file name and line number, then calls `abort()`.

**Example:**
```c
if (ptr == NULL) {
    ONES_PANIC("memory allocation failed for size %zu", size);
}
```

#### `ONES_ASSERT(cond, ...)`

```c
#define ONES_ASSERT(cond, ...) ((cond) ? (void)0 : (void)ones_panic_at(__FILE__, __LINE__, "" __VA_ARGS__))
```

Conditional panic: evaluates `cond`; if false, calls `ones_panic_at` with the given message and aborts.

---

### Pointer Move

#### `ONES_MOVE_PTR(des, src)`

```c
#define ONES_MOVE_PTR(des, src) \
    do { \
        *(des) = (src); \
        (src) = NULL; \
    } while(0)
```

Atomically moves the pointer value from `src` to `des`, then sets `src` to `NULL`. Useful for transferring ownership of dynamically allocated memory and avoiding double-free bugs.

**Example:**
```c
int *p = malloc(sizeof(int));
int *d = NULL;
int *c = p;              /* save original address for verification */

ONES_MOVE_PTR(&d, p);

/* After this:
   p == NULL
   d == c               (d now owns the memory originally held by p)
*/
```

---

### Dynamic Array Macros

Low-level dynamic array operations on a `T *data`, `size_t capa`, `size_t size` triple.

| Macro | Description |
|-------|-------------|
| `ONES_DYN_ARR_RESERVE(data, capa, expected_capa)` | Grow `capa` (doubling from `ONES_ARR_MIN_CAP` = 4) until it fits `expected_capa`, reallocing `data`. |
| `ONES_DYN_ARR_FREE(data)` | `free(data)` and set `data = NULL`. |
| `ONES_DYN_ARR_RESIZE(data, capa, size, new_size)` | Reserve capacity then set `size = new_size`. |
| `ONES_DYN_ARR_CLEAR(size)` | Set `size = 0`. |
| `ONES_DYN_ARR_PUSH(data, capa, size, value)` | Reserve then append `value`, increment `size`. |
| `ONES_DYN_ARR_PUSHX(data, capa, size, values, n)` | Reserve then copy `n` elements from `values`. |
| `ONES_DYN_ARR_POP(data, size)` | Decrement `size` and return the popped element (panics if empty). |
| `ONES_DYN_ARR_FIRST(data, size)` | Return first element (panics if empty). |
| `ONES_DYN_ARR_LAST(data, size)` | Return last element (panics if empty). |
| `ONES_DYN_REMOVE_UNORDERED(data, size, i)` | Remove element at `i` by moving the last element into its place. |

**Example:**
```c
int *data = NULL;
size_t capa = 0, size = 0;
ONES_DYN_ARR_PUSH(data, capa, size, 42);
ONES_DYN_ARR_PUSH(data, capa, size, 7);
ONES_DYN_ARR_FREE(data);
```

### String Builder Macros

Low-level string builder operations on a `ones_str_builder` (`char *c_str`, `size_t size`, `size_t capa`).

| Macro | Description |
|-------|-------------|
| `ONES_SB_PUSH(sb, c)` | Push a single character. |
| `ONES_SB_PUSH_C_STR(sb, c_str)` | Push a null-terminated string. |
| `ONES_SB_PUSH_BUF(sb, buf, buf_size)` | Push a raw buffer of `buf_size` bytes. |
| `ONES_SB_PUSH_SV(sb, sv)` | Push a `ones_str_view`. |
| `ONES_SB_FREE(sb)` | Free buffer and reset builder. |

**Example:**
```c
ones_str_builder b;
b.c_str = NULL; b.size = 0; b.capa = 0;
ONES_SB_PUSH(&b, 'a');
ONES_SB_PUSH_C_STR(&b, "bc");
ONES_SB_FREE(&b);
```

### Types

#### `ones_str_builder`

```c
typedef struct {
    char *c_str;
    size_t size;
    size_t capa;
} ones_str_builder;
```

#### `ones_str_view`

```c
typedef struct {
    const char *data;
    size_t size;
} ones_str_view;
```

``## Code Examples

### Example 1: Dynamic Array of Integers (ONES_DYN_ARR_*)

Save as example_arr.c:

\\\c
#include <stdio.h>
#include "ones.h"

int main(void) {
    int *data = NULL;
    size_t capa = 0, size = 0;

    ONES_DYN_ARR_PUSH(data, capa, size, 1);
    ONES_DYN_ARR_PUSH(data, capa, size, 4);
    ONES_DYN_ARR_PUSH(data, capa, size, 2);
    ONES_DYN_ARR_PUSH(data, capa, size, 5);

    printf("Array contents: ");
    for (size_t i = 0; i < size; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");

    ONES_DYN_ARR_POP(data, size); /* removes 5 */
    ONES_DYN_ARR_POP(data, size); /* removes 2 */

    printf("After two pops: ");
    for (size_t i = 0; i < size; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");

    ONES_DYN_ARR_FREE(data);
    return 0;
}
\\\

Compile and run:

\\\ash
gcc -std=c99 example_arr.c -o example_arr
./example_arr
\\\

Expected output:

\\\
Array contents: 1 4 2 5 
After two pops: 1 4 
\\\

---

### Example 2: Dynamic Array of Doubles (ONES_DYN_ARR_*)

\\\c
#include <stdio.h>
#include "ones.h"

int main(void) {
    double *ddata = NULL;
    size_t dcapa = 0, dsize = 0;

    ONES_DYN_ARR_PUSH(ddata, dcapa, dsize, 1.5);
    ONES_DYN_ARR_PUSH(ddata, dcapa, dsize, 2.5);

    for (size_t i = 0; i < dsize; i++) {
        printf("%.1f ", ddata[i]);
    }
    printf("\n");

    ONES_DYN_ARR_FREE(ddata);
    return 0;
}
\\\

---

### Example 3: Pointer Move (ONES_MOVE_PTR)

\\\c
#include <stdio.h>
#include <stdlib.h>
#include "ones.h"

int main(void) {
    int *p = (int *)malloc(sizeof(int));
    *p = 42;

    int *d = NULL;
    int *c = p;  /* backup for comparison */

    ONES_MOVE_PTR(&d, p);

    printf("p is %s\n", p == NULL ? "NULL" : "not NULL");
    printf("d == c: %s\n", d == c ? "true" : "false");
    printf("*d = %d\n", *d);

    free(d);
    return 0;
}
\\\

Expected output:

\\\
p is NULL
d == c: true
*d = 42
\\\

---

### Example 4: Panic on Invalid Input (ONES_PANIC)

\\\c
#include <stdio.h>
#include "ones.h"

int divide(int a, int b) {
    if (b == 0) {
        ONES_PANIC("division by zero: %d / %d", a, b);
    }
    return a / b;
}

int main(void) {
    printf("%d\n", divide(10, 2));
    printf("%d\n", divide(10, 0));  /* this will panic and abort */
    return 0;
}
\\\

Expected output:

\\\
panic at example_panic.c:8: division by zero: 10 / 0
\\\
## Build and Run Tests

This project includes CMake-based unit tests under `test/`. You do **not** need to build them to use `ones.h` in your own projects, but they are useful for verifying correctness and contributing changes.

### Build with CMake

```bash
# Create a build directory
mkdir build
cd build

# Configure (choose your generator if needed, e.g. -G "Visual Studio 17 2022" for MSVC)
cmake ..

# Build
cmake --build .

# Run all tests
ctest --output-on-failure
```

### CTest test mode (`ONES_TEST_MODE`)

A single option replaces several individual flags:

| Value | Meaning |
|-------|---------|
| `ON` (default) | Build tests with common warnings only (`-Wall -Wextra` / `/W4`) |
| `OFF` | Skip building tests entirely |
| `ASAN` | Build with AddressSanitizer (`-fsanitize=address`, GCC/Clang only) |
| `UBSAN` | Build with UndefinedBehaviorSanitizer (`-fsanitize=undefined`, GCC/Clang only) |
| `COVERAGE` | Build with gcov coverage flags (`-fprofile-arcs -ftest-coverage`, GCC/Clang only) |
| `STRICT` | Treat warnings as errors (`-Werror` / `/WX`) |

MSVC does not support ASAN/UBSAN/COVERAGE; those values fall back to warning-only builds on MSVC.

Examples:

```bash
# Default test build
cmake -S . -B build && cmake --build build && ctest --test-dir build --output-on-failure

# AddressSanitizer one-line
cmake -S . -B build-asan -DONES_TEST_MODE=ASAN && cmake --build build-asan && ctest --test-dir build-asan --output-on-failure

# UndefinedBehaviorSanitizer one-line
cmake -S . -B build-ubsan -DONES_TEST_MODE=UBSAN && cmake --build build-ubsan && ctest --test-dir build-ubsan --output-on-failure

# Coverage one-line (needs lcov/genhtml for HTML report)
cmake -S . -B build-cov -DONES_TEST_MODE=COVERAGE && cmake --build build-cov && ctest --test-dir build-cov --output-on-failure

# Strict warnings (warnings as errors)
cmake -S . -B build-strict -DONES_TEST_MODE=STRICT && cmake --build build-strict && ctest --test-dir build-strict --output-on-failure

# Skip tests
cmake -S . -B build -DONES_TEST_MODE=OFF
```

### Available Tests

| Test Name | Description |
|-----------|-------------|
| `test_panic` | Verifies `ONES_PANIC` / `ONES_ASSERT` exist and are callable (expected to abort; marked as `WILL_FAIL` in CTest). |
| `test_arr` | Tests `ONES_DYN_ARR_*` macros for `int` and `double`: push, pop, first, last, reserve, clear, free. |
| `test_move` | Tests `ONES_MOVE_PTR`: verifies source becomes `NULL` and destination receives the original pointer. |
| `test_str` | Tests `ONES_SB_*` string builder macros: push char/c_str/buf, str view, free & reuse. |
| `test_sv` | Tests `ones_sv_*` string view functions: from_parts/c_str, is_equal, find, chop_while/delim/sv, chop_left/right, start/end_with, chop_prefix/suffix, trim, ONES_SV_FMT. |
| `test_sb` | Tests extended `ONES_SB_*` string builder: `PUSH_FMT`, `PUSH_SV`, `PUSH_BUF`, build-without-terminate, free & reuse. |
| `test_macros` | Tests `ONES_MAX/MIN/SWAP_T`, `ONES_ARR_LEN/AT`, `ONES_DYN_REMOVE_UNORDERED`. |

### Available Benchmarks

| Benchmark Name | Description |
|----------------|-------------|
| `bench_arr` | Benchmarks dynamic array push, bulk push (`PUSHX`), and reserve operations. |
| `bench_sv` | Benchmarks string view `find` (small and large needles), `trim`, and `chop_by_delim`. |
| `bench_sb` | Benchmarks string builder push char, C string, formatted text, and string view. |
| `bench_macros` | Benchmarks `ONES_MAX/MIN`, `ONES_SWAP_T`, `ONES_ARR_AT`, and unordered removal. |

Build and run benchmarks:

```bash
cmake -S . -B build && cmake --build build --target ones_benchmarks
./build/bench_arr
./build/bench_sv
./build/bench_sb
./build/bench_macros
```

On Windows (MSVC), executables are placed under `build/Debug/`.

Run only benchmarks via CTest:

```bash
ctest --test-dir build -L benchmark --output-on-failure
```

### One-line build and test

```bash
cmake -S . -B build && cmake --build build && ctest --test-dir build --output-on-failure
```

---

## Project Structure

```
ones/
├── ones.h             # Single-header library (all public API)
├── main.c             # Example program (ignored by Git)
├── CMakeLists.txt     # CMake build configuration with CTest
├── README.md          # This file
├── LICENSE            # MIT License
├── .gitignore         # Ignores build artifacts and IDE files
├── test/              # Unit tests (built when ONES_TEST_MODE != OFF)
    ├── test_utils.h   # Minimal test assertions used by the test suite
    ├── test_panic.c   # Unit tests for ONES_PANIC / ONES_ASSERT
    ├── test_arr.c     # Unit tests for ONES_DYN_ARR_* dynamic arrays
    ├── test_move.c    # Unit tests for ONES_MOVE_PTR
    ├── test_str.c     # Unit tests for ONES_SB_* string builder
    ├── test_sv.c      # Unit tests for ones_sv_* string view
    ├── test_sb.c      # Unit tests for extended ONES_SB_* string builder
    └── test_macros.c  # Unit tests for ONES_MAX/MIN/SWAP_T/ARR_*/DYN_REMOVE_UNORDERED
└── bench/             # Benchmarks (built when ONES_BUILD_BENCHMARKS is ON)
    ├── bench_utils.h  # Benchmark timing helpers
    ├── bench_arr.c    # Benchmarks for dynamic arrays
    ├── bench_sv.c     # Benchmarks for string views
    ├── bench_sb.c     # Benchmarks for string builders
    └── bench_macros.c # Benchmarks for utility macros
```

Build directories (`build/`, `build-asan/`, `build-ubsan/`, `build-cov/`, `build-strict/`, `build-clang/`, `build-gcc/`, `build-msvc/`) are generated locally and ignored by Git.

---

## License

This project is licensed under the [MIT License](LICENSE).

```
MIT License

Copyright (c) 2026 menghuihuan44

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## Contributing

Contributions are welcome! Feel free to:

1. Open an issue for bug reports or feature requests.
2. Fork the repository, make your changes, and submit a pull request.
3. Improve documentation, add examples, or extend the test suite.

When contributing code, please follow the existing C99 style and keep changes minimal and focused.

