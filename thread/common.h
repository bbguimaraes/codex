#ifndef CODEX_THREAD_COMMON_H
#define CODEX_THREAD_COMMON_H

#include <assert.h>
#include <stdio.h>
#include <threads.h>

#define CHECK(f, ...) (check(__func__, #f, f(__VA_ARGS__)))

static inline bool check(const char *f, const char *msg, int res) {
    switch(res) {
    case thrd_success: return true;
#define C(x) case x: fprintf(stderr, "%s: %s: " #x "\n", f, msg); return false;
    C(thrd_nomem)
    C(thrd_timedout)
    C(thrd_busy)
    C(thrd_error)
#undef C
    }
    abort();
}

static inline bool queue_empty(size_t r, size_t w) {
    return w == r;
}

static inline bool queue_full(size_t r, size_t w, size_t n) {
    return w - r == n;
}

#endif
