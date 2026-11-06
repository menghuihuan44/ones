#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif


#include "ones.h"
#include "bench_utils.h"

int main(void) {
    const size_t N = 10000000u;

    /* ---- ONES_MAX / ONES_MIN ---- */
    {
        volatile int sink = 0;
        double t0 = bench_now();
        for (size_t i = 0; i < N; ++i) {
            sink += ONES_MAX((int)i, (int)(i + 1u));
            sink += ONES_MIN((int)i, (int)(i + 1u));
        }
        double t1 = bench_now();
        bench_report("ONES_MAX/MIN (int)", N * 2u, t1 - t0);
        (void)sink;
    }

    /* ---- ONES_SWAP_T ---- */
    {
        int a = 1;
        int b = 2;
        volatile int sink = 0;

        double t0 = bench_now();
        for (size_t i = 0; i < N; ++i) {
            ONES_SWAP_T(int, a, b);
            sink += a + b;
        }
        double t1 = bench_now();

        bench_report("ONES_SWAP_T (int)", N, t1 - t0);
        (void)sink;
        (void)a;
        (void)b;
    }

    /* ---- ONES_ARR_AT ---- */
    {
        int arr[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
        volatile int sink = 0;

        double t0 = bench_now();
        for (size_t i = 0; i < N; ++i) {
            sink += ONES_ARR_AT(arr, 8u, i % 8u);
        }
        double t1 = bench_now();

        bench_report("ONES_ARR_AT", N, t1 - t0);
        (void)sink;
    }

    /* ---- ONES_DYN_REMOVE_UNORDERED ---- */
    {
        int *data = NULL;
        size_t capa = 0;
        size_t size = 0;
        for (size_t i = 0; i < 1024u; ++i) {
            ONES_DYN_ARR_PUSH(data, capa, size, (int)i);
        }

        double t0 = bench_now();
        size_t removed = 0;
        while (size > 0u) {
            ONES_DYN_REMOVE_UNORDERED(data, size, 0u);
            ++removed;
        }
        double t1 = bench_now();

        bench_report("dyn_remove_unordered", removed, t1 - t0);
        ONES_DYN_ARR_FREE(data);
    }

    return 0;
}
