#ifndef BENCH_UTILS_H
#define BENCH_UTILS_H

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

#if defined(_WIN32)
#include <windows.h>
static double bench_now(void) {
    LARGE_INTEGER freq;
    LARGE_INTEGER counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / (double)freq.QuadPart;
}
#else
static double bench_now(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return (double)clock() / (double)CLOCKS_PER_SEC;
    }
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1.0e9;
}
#endif

static void bench_report(const char *name, size_t iters, double seconds) {
    double ns = (iters > 0) ? (seconds * 1.0e9 / (double)iters) : 0.0;
    (void)printf("%-42s iters=%10zu time=%.6f s  %10.2f ns/op\n",
                 name, iters, seconds, ns);
}

#endif /* BENCH_UTILS_H */
