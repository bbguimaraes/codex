#include "thread3.h"

#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>

#include "util/def.h"

#include "common.h"

static bool sleep_ns(u64 ns) {
    struct timespec t = {
        .tv_sec = ns / (uint64_t)1e9,
        .tv_nsec = ns % (uint64_t)1e9,
    };
    for(struct timespec r = {0};; t = r)
        switch(thrd_sleep(&t, &r)) {
            case -1: break;
            case 0: return true;
            default: perror("thrd_sleep"); return false;
        }
}

static const u64 sleep_dur_ns = 1000 * 1000;

static bool queue3_push(struct queue3 *q, struct task t) {
    const size_t w = (q->w + 1) % q->size;
    while(atomic_load_explicit(&q->r, memory_order_acquire) == w)
        if(!sleep_ns(sleep_dur_ns))
            return false;
    q->tasks[q->w] = t;
    atomic_store_explicit(&q->w, w, memory_order_release);
    return true;
}

static bool queue3_pop(struct queue3 *q, struct task *t) {
    while(atomic_load_explicit(&q->w, memory_order_acquire) == q->r)
        if(!sleep_ns(sleep_dur_ns))
            false;
    const struct task ret = q->tasks[q->r];
    atomic_store_explicit(&q->r, (q->r + 1) % q->size, memory_order_release);
    *t = ret;
    return true;
}

static int pool3_thread(void *data) {
    struct queue3 *const q = data;
    for(;;) {
        struct task t = {0};
        if(!queue3_pop(q, &t))
            return 1;
        if(!t.f)
            break;
        t.f(t.data);
    }
    return 0;
}

bool pool3_init(struct pool3 *p, size_t size, size_t queue_size) {
    p->threads = calloc(size, sizeof(*p->threads));
    p->workers = calloc(size, sizeof(*p->workers));
    p->tasks = calloc(queue_size * size, sizeof(*p->tasks));
    size_t i = 0;
    for(; i < size; ++i) {
        struct queue3 *const q = &p->workers[i].q;
        q->size = queue_size;
        q->tasks = p->tasks + queue_size * i;
        switch(thrd_create(p->threads + i, pool3_thread, q)) {
        case thrd_success: break;
        default: perror("thrd_create"); goto e0;
        }
    }
    p->size = size;
    return true;
e0:
    p->size = i;
    pool3_destroy(p);
    return false;
}

bool pool3_destroy(struct pool3 *p) {
    if(!p->size)
        return true;
    bool ret = true;
    for(size_t i = 0, n = p->size; i < n; ++i)
        ret = pool3_enqueue(p, (struct task){0}) && ret;
    for(size_t i = 0, n = p->size; i < n; ++i)
        thrd_join(p->threads[i], NULL);
    free(p->threads);
    free(p->workers);
    free(p->tasks);
    return true;
}

bool pool3_enqueue(struct pool3 *p, struct task t) {
    const size_t i = p->cur_queue;
    bool ret = queue3_push(&p->workers[i].q, t);
    p->cur_queue = (i + 1) % p->size;
    return ret;
}
