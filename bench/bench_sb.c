#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif


#include "ones.h"
#include "bench_utils.h"

#include <stdlib.h>
#include <string.h>

int main(void) {
    const size_t N = 1000000u;

    /* ---- push single characters ---- */
    {
        ones_str_builder sb;
        sb.c_str = NULL;
        sb.size = 0u;
        sb.capa = 0u;

        double t0 = bench_now();
        for (size_t i = 0; i < N; ++i) {
            ONES_SB_PUSH(&sb, 'a');
        }
        double t1 = bench_now();

        bench_report("sb push char", N, t1 - t0);
        ONES_SB_FREE(&sb);
    }

    /* ---- push C strings ---- */
    {
        ones_str_builder sb;
        sb.c_str = NULL;
        sb.size = 0u;
        sb.capa = 0u;

        const size_t iters = N / 5u;
        double t0 = bench_now();
        for (size_t i = 0; i < iters; ++i) {
            ONES_SB_PUSH_C_STR(&sb, "hello");
        }
        double t1 = bench_now();

        bench_report("sb push c_str", iters, t1 - t0);
        ONES_SB_FREE(&sb);
    }

    /* ---- push formatted text ---- */
    {
        ones_str_builder sb;
        sb.c_str = NULL;
        sb.size = 0u;
        sb.capa = 0u;

        const size_t iters = N / 20u;
        double t0 = bench_now();
        for (size_t i = 0; i < iters; ++i) {
            ones_sb_push_fmt(&sb, "v=%zu ", i);
        }
        double t1 = bench_now();

        bench_report("sb push fmt", iters, t1 - t0);
        ONES_SB_FREE(&sb);
    }

    /* ---- push a string view ---- */
    {
        ones_str_builder sb;
        sb.c_str = NULL;
        sb.size = 0u;
        sb.capa = 0u;

        ones_str_view sv = ONES_SV_FROM_C_STR("view!");
        const size_t iters = N / 6u;

        double t0 = bench_now();
        for (size_t i = 0; i < iters; ++i) {
            ONES_SB_PUSH_SV(&sb, sv);
        }
        double t1 = bench_now();

        bench_report("sb push sv", iters, t1 - t0);
        ONES_SB_FREE(&sb);
    }

    return 0;
}
