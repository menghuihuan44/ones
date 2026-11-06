#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif


#include "ones.h"
#include "bench_utils.h"

#include <stdlib.h>
#include <string.h>

int main(void) {
    /* ---- find: small needle, repeated search ---- */
    {
        const size_t H = 100000u;
        char *hay = (char *)malloc(H + 1u);
        if (hay == NULL) {
            return 1;
        }
        memset(hay, 'a', H);
        memcpy(hay + H - 6u, "needle", 6u);
        hay[H] = '\0';

        ones_str_view sv = ONES_SV_FROM_C_STR(hay);
        ones_str_view nd = ONES_SV_FROM_C_STR("needle");

        volatile size_t sink = 0;
        const size_t iters = 1000u;

        double t0 = bench_now();
        for (size_t i = 0; i < iters; ++i) {
            sink += ones_sv_find(sv, nd);
        }
        double t1 = bench_now();

        bench_report("sv find small needle", iters, t1 - t0);
        free(hay);
        (void)sink;
    }

    /* ---- find: large needle (>1024, exercises KMP path) ---- */
    {
        const size_t H = 1u << 20;
        const size_t P = 2048u;
        char *hay = (char *)malloc(H);
        char *pat = (char *)malloc(P);
        if (hay == NULL || pat == NULL) {
            free(hay);
            free(pat);
            return 1;
        }
        memset(hay, 'a', H);
        for (size_t i = 0; i < P; ++i) {
            pat[i] = (char)('a' + (i % 26));
        }
        memcpy(hay + H - P, pat, P);

        ones_str_view sv = ONES_SV_FROM_PARTS(hay, H);
        ones_str_view nd = ONES_SV_FROM_PARTS(pat, P);

        volatile size_t sink = 0;
        const size_t iters = 100u;

        double t0 = bench_now();
        for (size_t i = 0; i < iters; ++i) {
            sink += ones_sv_find(sv, nd);
        }
        double t1 = bench_now();

        bench_report("sv find large needle (KMP)", iters, t1 - t0);
        free(hay);
        free(pat);
        (void)sink;
    }

    /* ---- trim ---- */
    {
        const size_t N = 100000u;
        char *buf = (char *)malloc(N + 1u);
        if (buf == NULL) {
            return 1;
        }
        memset(buf, ' ', N / 4u);
        memset(buf + N / 4u, 'x', N / 2u);
        memset(buf + N / 4u + N / 2u, ' ', N / 4u);
        buf[N] = '\0';

        ones_str_view sv = ONES_SV_FROM_PARTS(buf, N);

        volatile size_t sink = 0;
        const size_t iters = 1000u;

        double t0 = bench_now();
        for (size_t i = 0; i < iters; ++i) {
            ones_str_view t = ones_sv_trim(sv);
            sink += t.size;
        }
        double t1 = bench_now();

        bench_report("sv trim", iters, t1 - t0);
        free(buf);
        (void)sink;
    }

    /* ---- chop_by_delim over a comma list ---- */
    {
        const size_t N = 100000u;
        char *buf = (char *)malloc(N + 1u);
        if (buf == NULL) {
            return 1;
        }
        for (size_t i = 0; i < N; ++i) {
            buf[i] = (i % 27 == 26) ? ',' : 'a';
        }
        buf[N] = '\0';

        volatile size_t sink = 0;
        const size_t iters = 100u;

        double t0 = bench_now();
        for (size_t r = 0; r < iters; ++r) {
            ones_str_view sv = ONES_SV_FROM_PARTS(buf, N);
            while (sv.size > 0u) {
                ones_str_view part = ones_sv_chop_by_delim(&sv, ',');
                sink += part.size;
            }
        }
        double t1 = bench_now();

        bench_report("sv chop_by_delim", iters, t1 - t0);
        free(buf);
        (void)sink;
    }

    return 0;
}
