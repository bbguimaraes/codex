#ifndef CODEX_THREAD_THREAD0_H
#define CODEX_THREAD_THREAD0_H

#include <stdalign.h>
#include <stdbool.h>
#include <threads.h>

#include "thread.h"

struct queue0 {
    size_t r, w, size;
    struct task *tasks;
    mtx_t mtx;
    cnd_t cnd;
};

struct pool0 {
    thrd_t *threads;
    struct queue0 q;
    size_t size;
};

bool pool0_init(struct pool0 *p, size_t size, size_t queue_size);
bool pool0_destroy(struct pool0 *p);
bool pool0_enqueue(struct pool0 *p, struct task t);

#endif
