#ifndef CODEX_THREAD_THREAD4_H
#define CODEX_THREAD_THREAD4_H

#include <stdalign.h>
#include <stdbool.h>
#include <threads.h>

#include "thread.h"

struct queue4 {
    size_t r, w, size;
    struct task *tasks;
    mtx_t mtx;
    cnd_t cnd;
    bool done;
};

struct pool4;

struct worker4_data {
    struct queue4 *queues;
    size_t i, n_queues;
};

struct worker4 {
    union {
        struct worker4_data d;
        alignas(64) char cache_line[64];
    };
};

struct pool4 {
    thrd_t *threads;
    struct worker4 *workers;
    struct queue4 *queues;
    struct task *tasks;
    size_t size, cur_queue;
};

bool pool4_init(struct pool4 *p, size_t size, size_t queue_size);
bool pool4_destroy(struct pool4 *p);
bool pool4_enqueue(struct pool4 *p, struct task t);

#endif
