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
  - [`ones_panic`](#ones_panic)
  - [`ONES_MOVE`](#ones_move)
  - [`ONES_DECL_ARR(type)`](#ones_decl_arrtype)
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
| **Panic Handling** | `ones_panic()` prints a descriptive error message (with file name and line number) to `stderr` and aborts the program. |
| **Pointer Move** | `ONES_MOVE(des, src)` safely transfers pointer ownership and sets the source to `NULL`, preventing double-free and dangling pointer bugs. |
| **Type-Generic Dynamic Array** | `ONES_DECL_ARR(type)` generates a complete dynamic array API for any type (`int`, `double`, `struct`, etc.), including init, deinit, create, destroy, push back, and pop back. |
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

If you need dynamic arrays, declare the array type with the macro:

```c
ONES_DECL_ARR(int)        /* generates ones_arr_int and related functions */
ONES_DECL_ARR(double)     /* generates ones_arr_double and related functions */
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

### `ones_panic`

```c
void ones_panic(const char *format, ...);
```

**Description:**  
Prints a panic message to `stderr` with the format:
```
panic at <file>:<line>: <formatted message>
```
Then calls `abort()` to terminate the program immediately.

**Parameters:**
- `format` — `printf`-style format string.
- `...` — Variable arguments matching the format string.

**Example:**
```c
if (ptr == NULL) {
    ones_panic("memory allocation failed for size %zu", size);
}
```

**Output:**
```
panic at main.c:42: memory allocation failed for size 1024
```

---

### `ONES_MOVE`

```c
#define ONES_MOVE(des, src)
```

**Description:**  
Atomically moves the pointer value from `src` to `des`, then sets `src` to `NULL`. This is useful for transferring ownership of dynamically allocated memory and avoiding double-free bugs.

**Parameters:**
- `des` — Destination pointer (must be an lvalue pointer).
- `src` — Source pointer (must be an lvalue pointer; will be set to `NULL` after the move).

**Example:**
```c
int *p = malloc(sizeof(int));
int *d = NULL;
int *c = p;              /* save original address for verification */

ONES_MOVE(&d, p);

/* After this:
   p == NULL
   d == c               (d now owns the memory originally held by p)
*/
```

---

### `ONES_DECL_ARR(type)`

```c
#define ONES_DECL_ARR(type)
```

**Description:**  
Generates a dynamic array structure and a full set of functions for the given `type`. The generated struct name is `ones_arr_##type`, and all functions are prefixed with `ones_arr_##type_`.

**Generated struct:**
```c
typedef struct {
    type *data;   /* pointer to the heap-allocated element array */
    size_t size;  /* number of elements currently stored */
    size_t capa;  /* current allocated capacity */
} ones_arr_##type;
```

**Generated functions:**

| Function | Signature | Description |
|----------|-----------|-------------|
| `ones_arr_init_##type` | `void ones_arr_init_##type(ones_arr_##type *arr)` | Initialize an array to empty state (`data = NULL`, `size = 0`, `capa = 0`). |
| `ones_arr_deinit_##type` | `void ones_arr_deinit_##type(ones_arr_##type *arr)` | Free the element buffer and reset the array to empty state. |
| `ones_arr_create_##type` | `ones_arr_##type *ones_arr_create_##type(void)` | Allocate a new array struct on the heap and return its pointer. Returns `NULL` on allocation failure (triggers `ones_panic`). |
| `ones_arr_destroy_##type` | `void ones_arr_destroy_##type(ones_arr_##type *arr)` | Free the element buffer, then free the array struct itself. |
| `ones_arr_push_back_##type` | `bool ones_arr_push_back_##type(ones_arr_##type *arr, type value)` | Append `value` to the end. Automatically grows capacity (starting at 4, doubling when full). Returns `true` on success. |
| `ones_arr_pop_back_##type` | `bool ones_arr_pop_back_##type(ones_arr_##type *arr)` | Remove the last element (decrements `size`). Does not free memory. Returns `true` on success, panics if array is empty. |

**Example usage for `int`:**
```c
ONES_DECL_ARR(int)

ones_arr_int arr;
ones_arr_init_int(&arr);

ones_arr_push_back_int(&arr, 10);
ones_arr_push_back_int(&arr, 20);
ones_arr_push_back_int(&arr, 30);

printf("%zu elements\n", arr.size);   /* prints: 3 elements */
printf("%d\n", arr.data[0]);          /* prints: 10 */

ones_arr_deinit_int(&arr);
```

---

## Code Examples

### Example 1: Dynamic Array of Integers

Save as `example_arr.c`:

```c
#include <stdio.h>
#include "ones.h"

ONES_DECL_ARR(int)

int main(void) {
    ones_arr_int arr;
    ones_arr_init_int(&arr);

    ones_arr_push_back_int(&arr, 1);
    ones_arr_push_back_int(&arr, 4);
    ones_arr_push_back_int(&arr, 2);
    ones_arr_push_back_int(&arr, 5);

    printf("Array contents: ");
    for (size_t i = 0; i < arr.size; i++) {
        printf("%d ", arr.data[i]);
    }
    printf("\n");

    ones_arr_pop_back_int(&arr);  /* removes 5 */
    ones_arr_pop_back_int(&arr);  /* removes 2 */

    printf("After two pops: ");
    for (size_t i = 0; i < arr.size; i++) {
        printf("%d ", arr.data[i]);
    }
    printf("\n");

    ones_arr_deinit_int(&arr);
    return 0;
}
```

Compile and run:

```bash
gcc -std=c99 example_arr.c -o example_arr
./example_arr
```

Expected output:

```
Array contents: 1 4 2 5 
After two pops: 1 4 
```

---

### Example 2: Dynamic Array of Doubles

```c
#include <stdio.h>
#include "ones.h"

ONES_DECL_ARR(double)

int main(void) {
    ones_arr_double *darr = ones_arr_create_double();

    ones_arr_push_back_double(darr, 1.5);
    ones_arr_push_back_double(darr, 2.5);
    ones_arr_push_back_double(darr, 3.5);

    for (size_t i = 0; i < darr->size; i++) {
        printf("%.1f ", darr->data[i]);
    }
    printf("\n");

    ones_arr_destroy_double(darr);
    return 0;
}
```

---

### Example 3: Pointer Move

```c
#include <stdio.h>
#include <stdlib.h>
#include "ones.h"

int main(void) {
    int *p = (int *)malloc(sizeof(int));
    *p = 42;

    int *d = NULL;
    int *c = p;  /* backup for comparison */

    ONES_MOVE(&d, p);

    printf("p is %s\n", p == NULL ? "NULL" : "not NULL");
    printf("d == c: %s\n", d == c ? "true" : "false");
    printf("*d = %d\n", *d);

    free(d);
    return 0;
}
```

Expected output:

```
p is NULL
d == c: true
*d = 42
```

---

### Example 4: Panic on Invalid Input

```c
#include <stdio.h>
#include "ones.h"

int divide(int a, int b) {
    if (b == 0) {
        ones_panic("division by zero: %d / %d", a, b);
    }
    return a / b;
}

int main(void) {
    printf("%d\n", divide(10, 2));
    printf("%d\n", divide(10, 0));  /* this will panic and abort */
    return 0;
}
```

Expected output:

```
panic at example_panic.c:8: division by zero: 10 / 0
```

---

## Build and Run Tests

This project includes CMake-based unit tests. You do **not** need to build them to use `ones.h` in your own projects, but they are useful for verifying correctness and contributing changes.

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

### Available Tests

| Test Name | Description |
|-----------|-------------|
| `test_panic` | Verifies `ones_panic` exists and is callable (expected to abort; marked as `WILL_FAIL` in CTest). |
| `test_arr` | Tests dynamic arrays for `int` and `double`: init, push_back, pop_back, deinit, create, destroy. |
| `test_move` | Tests `ONES_MOVE`: verifies source becomes `NULL` and destination receives the original pointer. |

### One-line build and test

```bash
cmake -S . -B build && cmake --build build && ctest --test-dir build --output-on-failure
```

---

## Project Structure

```
ones/
├── ones.h            # Single-header library (all public API)
├── test_utils.h      # Minimal test assertions used by the test suite
├── test_arr.c        # Unit tests for dynamic arrays
├── test_move.c       # Unit tests for ONES_MOVE
├── test_panic.c      # Unit tests for ones_panic
├── CMakeLists.txt    # CMake build configuration with CTest
├── README.md         # This file
├── LICENSE           # MIT License
└── .gitignore        # Ignores build artifacts and IDE files
```

Build directories (`build-clang/`, `build-gcc/`, `build-msvc/`) are generated locally and ignored by Git.

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