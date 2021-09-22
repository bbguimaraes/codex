#ifndef CODEX_THREAD_THREAD2_H
#define CODEX_THREAD_THREAD2_H

#include <stdalign.h>
#include <stdbool.h>
#include <threads.h>

#include "thread.h"

struct queue2 {
    size_t r, w, size;
    struct task *tasks;
};

struct worker2 {
    union {
        struct queue2 q;
        alignas(64) char cache_line[64];
    };
};

struct pool2 {
    thrd_t *threads;
    struct worker2 *workers;
    struct task *tasks;
    size_t size, cur_queue;
};

bool pool2_init(struct pool2 *p, size_t size, size_t queue_size);
bool pool2_destroy(struct pool2 *p);
bool pool2_enqueue(struct pool2 *p, struct task t);

#endif
