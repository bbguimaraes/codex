#include "thread0.h"

#include <stdlib.h>

#include "common.h"

static void queue0_destroy(struct queue0 *q) {
    free(q->tasks);
    cnd_destroy(&q->cnd);
    mtx_destroy(&q->mtx);
}

static bool queue0_init(struct queue0 *q, size_t size) {
    if(!CHECK(mtx_init, &q->mtx, mtx_plain))
        return false;
    if(!CHECK(cnd_init, &q->cnd))
        goto e0;
    q->size = size;
    q->tasks = calloc(size, sizeof(*q->tasks));
    return true;
e0:
    mtx_destroy(&q->mtx);
    return false;
}

static bool queue0_push(struct queue0 *q, struct task t) {
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

static bool queue0_pop(struct queue0 *q, struct task *t) {
    if(!CHECK(mtx_lock, &q->mtx))
        return false;
    while(queue_empty(q->r, q->w))
        if(!CHECK(cnd_wait, &q->cnd, &q->mtx))
            goto e0;
    bool ret = true;
    if(queue_full(q->r, q->w, q->size))
        ret = CHECK(cnd_broadcast, &q->cnd) && ret;
    const struct task tr = q->tasks[q->r++ % q->size];
    ret = CHECK(mtx_unlock, &q->mtx) && ret;
    *t = tr;
    return ret;
e0:
    CHECK(mtx_unlock, &q->mtx);
    return false;
}

static int pool0_thread(void *data) {
    struct queue0 *const q = data;
    for(;;) {
        struct task t = {0};
        if(!queue0_pop(q, &t))
            return 1;
        if(!t.f)
            break;
        t.f(t.data);
    }
    return 0;
}

bool pool0_init(struct pool0 *p, size_t size, size_t queue_size) {
    p->threads = calloc(size, sizeof(*p->threads));
    size_t i = 0;
    if(!queue0_init(&p->q, queue_size))
        goto e0;
    for(; i < size; ++i) {
        switch(thrd_create(p->threads + i, pool0_thread, &p->q)) {
        case thrd_success: break;
        default: perror("thrd_create"); goto e0;
        }
    }
    p->size = size;
    return true;
e0:
    p->size = i;
    pool0_destroy(p);
    return false;
}

bool pool0_destroy(struct pool0 *p) {
    bool ret = true;
    for(size_t i = 0, n = p->size; i < n; ++i)
        ret = pool0_enqueue(p, (struct task){0}) && ret;
    for(size_t i = 0, n = p->size; i < n; ++i) {
        int ret_i = 0;
        thrd_join(p->threads[i], &ret_i);
        ret &= !ret_i;
    }
    queue0_destroy(&p->q);
    free(p->threads);
    return true;
}

bool pool0_enqueue(struct pool0 *p, struct task t) {
    return queue0_push(&p->q, t);
}
