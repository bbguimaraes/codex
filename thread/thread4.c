#include "thread4.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

static void queue4_destroy(struct queue4 *q) {
    cnd_destroy(&q->cnd);
    mtx_destroy(&q->mtx);
}

static bool queue4_init(struct queue4 *q, struct task *tasks, size_t size) {
    if(!CHECK(mtx_init, &q->mtx, mtx_plain))
        return false;
    if(!CHECK(cnd_init, &q->cnd))
        goto e0;
    q->size = size;
    q->tasks = tasks;
    return true;
e0:
    mtx_destroy(&q->mtx);
    return false;
}

static bool queue4_set_done(struct queue4 *q) {
    if(!CHECK(mtx_lock, &q->mtx))
        return false;
    q->done = true;
    bool ret = CHECK(cnd_signal, &q->cnd);
    ret = CHECK(mtx_unlock, &q->mtx) && ret;
    return ret;
}

static int queue4_try_push(struct queue4 *q, struct task t) {
    const int err = mtx_trylock(&q->mtx);
    switch(err) {
    case thrd_success: break;
    case thrd_busy: return 0;
    default: check(__func__, "mtx_trylock", err); return -1;
    }
    if(queue_full(q->r, q->w, q->size))
        return CHECK(mtx_unlock, &q->mtx) ? 0 : -1;
    q->tasks[q->w++ % q->size] = t;
    bool ret = CHECK(cnd_signal, &q->cnd);
    ret = CHECK(mtx_unlock, &q->mtx) && ret;
    return ret;
}

static bool queue4_push(struct queue4 *q, struct task t) {
    if(!CHECK(mtx_lock, &q->mtx))
        return false;
    while(queue_full(q->r, q->w, q->size))
        if(!CHECK(cnd_wait, &q->cnd, &q->mtx))
            goto e0;
    q->tasks[q->w++ % q->size] = t;
    bool ret = CHECK(cnd_signal, &q->cnd);
    ret = CHECK(mtx_unlock, &q->mtx) && ret;
    return ret;
e0:
    CHECK(mtx_unlock, &q->mtx);
    return false;
}

static int queue4_try_pop(struct queue4 *q, struct task *t) {
    const int err = mtx_trylock(&q->mtx);
    switch(err) {
    case thrd_success: break;
    case thrd_busy: return 0;
    default: check(__func__, "mtx_trylock", err); return -1;
    }
    if(queue_empty(q->r, q->w))
        return CHECK(mtx_unlock, &q->mtx) ? 0 : -1;
    const struct task tr = q->tasks[q->r++ % q->size];
    bool ret = CHECK(cnd_signal, &q->cnd);
    ret = CHECK(mtx_unlock, &q->mtx) && ret;
    if(ret)
        *t = tr;
    return ret;
}

static bool queue4_pop(struct queue4 *q, struct task *t) {
    if(!CHECK(mtx_lock, &q->mtx))
        return false;
    while(queue_empty(q->r, q->w)) {
        if(q->done)
            return true;
        if(!CHECK(cnd_wait, &q->cnd, &q->mtx))
            goto e0;
    }
    bool ret = true;
    if(queue_full(q->r, q->w, q->size))
        ret = CHECK(cnd_broadcast, &q->cnd) && ret;
    const struct task jr = q->tasks[q->r++ % q->size];
    ret = CHECK(mtx_unlock, &q->mtx) && ret;
    *t = jr;
    return ret;
e0:
    CHECK(mtx_unlock, &q->mtx);
    return false;
}

static int pool4_thread(void *data) {
    const struct worker4_data w = *(struct worker4_data*)data;
    for(;;) {
        struct task t = {0};
        for(size_t i = 0; i < w.n_queues; ++i) {
            switch(queue4_try_pop(w.queues + (w.i + i) % w.n_queues, &t)) {
            case 0: continue;
            case -1: return 1;
            default: goto l0;
            }
        }
        if(!queue4_pop(w.queues + w.i, &t))
            return 1;
l0:
        if(!t.f)
            return 0;
        t.f(t.data);
    }
}

bool pool4_init(struct pool4 *p, size_t size, size_t queue_size) {
    p->threads = calloc(size, sizeof(*p->threads));
    p->queues = calloc(size, sizeof(*p->queues));
    p->workers = calloc(size, sizeof(*p->workers));
    p->tasks = calloc(queue_size * size, sizeof(*p->tasks));
    size_t i = 0;
    for(; i < size; ++i)
        if(!queue4_init(p->queues + i, p->tasks + queue_size * i, queue_size))
            goto e0;
    for(i = 0; i < size; ++i) {
        struct worker4 *const w = p->workers + i;
        *w = (struct worker4){.d = {
            .queues = p->queues,
            .n_queues = size,
            .i = i,
        }};
        switch(thrd_create(p->threads + i, pool4_thread, &p->workers[i].d)) {
        case thrd_success: break;
        default: perror("thrd_create"); goto e1;
        }
    }
    p->size = size;
    return true;
e0:
    while(--i)
        queue4_destroy(p->queues + i);
    pool4_destroy(p);
    return false;
e1:
    p->size = i;
    pool4_destroy(p);
    return false;
}

bool pool4_destroy(struct pool4 *p) {
    const size_t n = p->size;
    if(!n)
        return true;
    for(size_t i = 0; i < n; ++i)
        if(!queue4_set_done(p->queues + i))
            return false;
    for(size_t i = 0; i < n; ++i)
        thrd_join(p->threads[i], NULL);
    free(p->tasks);
    free(p->workers);
    free(p->queues);
    free(p->threads);
    return true;
}

bool pool4_enqueue(struct pool4 *p, struct task t) {
    const size_t i = p->cur_queue;
    for(size_t q = 0; q < p->size; ++q)
        switch(queue4_try_push(p->queues + (i + q) % p->size, t)) {
        case 1: goto end;
        case -1: return false;
        }
    if(!queue4_push(&p->queues[i], t))
        return false;
end:
    p->cur_queue = (i + 1) % p->size;
    return true;
}
