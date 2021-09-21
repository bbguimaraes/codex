#include "thread1.h"

#include <stdlib.h>

#include "common.h"

static void queue1_destroy(struct queue1 *q) {
    cnd_destroy(&q->cnd);
    mtx_destroy(&q->mtx);
}

static bool queue1_init(struct queue1 *q, struct task *tasks, size_t size) {
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

static bool queue1_push(struct queue1 *q, struct task t) {
    if(!CHECK(mtx_lock, &q->mtx))
        return false;
    bool ret = false;
    while(queue_full(q->r, q->w, q->size))
        if(!CHECK(cnd_wait, &q->cnd, &q->mtx))
            goto end;
    q->tasks[q->w++ % q->size] = t;
    ret = CHECK(cnd_signal, &q->cnd);
end:
    ret = CHECK(mtx_unlock, &q->mtx) && ret;
    return ret;
}

static bool queue1_pop(struct queue1 *q, struct task *t) {
    if(!CHECK(mtx_lock, &q->mtx))
        return false;
    bool ret = false;
    while(queue_empty(q->r, q->w))
        if(!CHECK(cnd_wait, &q->cnd, &q->mtx))
            goto end;
    const struct task tr = q->tasks[q->r++ % q->size];
    ret = CHECK(cnd_signal, &q->cnd);
end:
    ret = CHECK(mtx_unlock, &q->mtx) && ret;
    if(ret)
        *t = tr;
    return ret;
}

static int pool1_thread(void *data) {
    struct queue1 *const q = data;
    for(;;) {
        struct task j = {0};
        if(!queue1_pop(q, &j))
            return 1;
        if(!j.f)
            break;
        j.f(j.data);
    }
    return 0;
}

bool pool1_init(struct pool1 *p, size_t size, size_t queue_size) {
    p->threads = calloc(size, sizeof(*p->threads));
    p->workers = calloc(size, sizeof(*p->workers));
    p->tasks = calloc(queue_size * size, sizeof(*p->tasks));
    size_t i = 0;
    for(; i < size; ++i) {
        struct queue1 *const q = &p->workers[i].q;
        if(!queue1_init(q, p->tasks + queue_size * i, queue_size))
            goto e0;
        switch(thrd_create(p->threads + i, pool1_thread, q)) {
        case thrd_success: break;
        default: perror("thrd_create"); goto e0;
        }
    }
    p->size = size;
    return true;
e0:
    p->size = i;
    pool1_destroy(p);
    return false;
}

bool pool1_destroy(struct pool1 *p) {
    if(!p->size)
        return true;
    bool ret = true;
    for(size_t i = 0, n = p->size; i < n; ++i)
        ret = pool1_enqueue(p, (struct task){0}) && ret;
    for(size_t i = 0, n = p->size; i < n; ++i) {
        int ret_i = 0;
        thrd_join(p->threads[i], &ret_i);
        ret &= !ret_i;
        queue1_destroy(&p->workers[i].q);
    }
    free(p->threads);
    free(p->workers);
    free(p->tasks);
    return ret;
}

bool pool1_enqueue(struct pool1 *p, struct task t) {
    const size_t i = p->cur_queue;
    bool ret = queue1_push(&p->workers[i].q, t);
    p->cur_queue = (i + 1) % p->size;
    return ret;
}
