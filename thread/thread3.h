#ifndef CODEX_THREAD_THREAD3_H
#define CODEX_THREAD_THREAD3_H

#include <stdalign.h>
#include <stdbool.h>
#include <threads.h>

#include "thread.h"

struct queue3 {
    size_t r, w, size;
    struct task *tasks;
};

struct worker3 {
    union {
        struct queue3 q;
        alignas(64) char cache_line[64];
    };
};

struct pool3 {
    thrd_t *threads;
    struct worker3 *workers;
    struct task *tasks;
    size_t size, cur_queue;
};

bool pool3_init(struct pool3 *p, size_t size, size_t queue_size);
bool pool3_destroy(struct pool3 *p);
bool pool3_enqueue(struct pool3 *p, struct task t);

#endif
