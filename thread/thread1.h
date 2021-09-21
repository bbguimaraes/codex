#ifndef CODEX_THREAD_THREAD1_H
#define CODEX_THREAD_THREAD1_H

#include <stdalign.h>
#include <stdbool.h>
#include <threads.h>

#include "thread.h"

struct queue1 {
    size_t r, w, size;
    struct task *tasks;
    mtx_t mtx;
    cnd_t cnd;
};

struct worker1 {
    union {
        struct queue1 q;
        alignas(64) char cache_line[64];
    };
};

struct pool1 {
    thrd_t *threads;
    struct worker1 *workers;
    struct task *tasks;
    size_t size, cur_queue;
};

bool pool1_init(struct pool1 *p, size_t size, size_t queue_size);
bool pool1_destroy(struct pool1 *p);
bool pool1_enqueue(struct pool1 *p, struct task t);

#endif
