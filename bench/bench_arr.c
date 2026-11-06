#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif


#include "ones.h"
#include "bench_utils.h"

int main(void) {
    const size_t N = 1000000u;

    /* ---- push one element at a time ---- */
    {
        int *data = NULL;
        size_t capa = 0;
        size_t size = 0;
        volatile int sink = 0;

        double t0 = bench_now();
        for (size_t i = 0; i < N; ++i) {
            ONES_DYN_ARR_PUSH(data, capa, size, (int)i);
            sink += data[size - 1u];
        }
        double t1 = bench_now();

        bench_report("dyn_arr push (int)", N, t1 - t0);
        ONES_DYN_ARR_FREE(data);
        (void)sink;
        (void)size;
    }

    /* ---- push a fixed-size buffer in batches ---- */
    {
        int buf[1024];
        for (int i = 0; i < 1024; ++i) {
            buf[i] = i;
        }

        int *data = NULL;
        size_t capa = 0;
        size_t size = 0;
        const size_t batches = N / 1024u;

        double t0 = bench_now();
        for (size_t b = 0; b < batches; ++b) {
            ONES_DYN_ARR_PUSHX(data, capa, size, buf, 1024u);
        }
        double t1 = bench_now();

        bench_report("dyn_arr pushx 1024 (int)", batches * 1024u, t1 - t0);
        ONES_DYN_ARR_FREE(data);
        (void)size;
    }

    /* ---- reserve capacity up front ---- */
    {
        int *data = NULL;
        size_t capa = 0;
        size_t size = 0;

        double t0 = bench_now();
        ONES_DYN_ARR_RESERVE(data, capa, N);
        size = N;
        double t1 = bench_now();

        bench_report("dyn_arr reserve N (int)", N, t1 - t0);
        ONES_DYN_ARR_FREE(data);
        (void)size;
    }

    return 0;
}
