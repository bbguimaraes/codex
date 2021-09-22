#include "thread2.h"

#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>

#include "common.h"

static bool queue2_push(struct queue2 *q, struct task t) {
    const size_t w = (q->w + 1) % q->size;
    while(atomic_load(&q->r) == w)
        thrd_yield();
    q->tasks[q->w] = t;
    atomic_store(&q->w, w);
    return true;
}

static bool queue2_pop(struct queue2 *q, struct task *t) {
    while(atomic_load(&q->w) == q->r)
        thrd_yield();
    const struct task ret = q->tasks[q->r];
    atomic_store(&q->r, (q->r + 1) % q->size);
    *t = ret;
    return true;
}

static int pool2_thread(void *data) {
    struct queue2 *const q = data;
    for(;;) {
        struct task t = {0};
        if(!queue2_pop(q, &t))
            return 1;
        if(!t.f)
            break;
        t.f(t.data);
    }
    return 0;
}

bool pool2_init(struct pool2 *p, size_t size, size_t queue_size) {
    p->threads = calloc(size, sizeof(*p->threads));
    p->workers = calloc(size, sizeof(*p->workers));
    p->tasks = calloc(queue_size * size, sizeof(*p->tasks));
    size_t i = 0;
    for(; i < size; ++i) {
        struct queue2 *const q = &p->workers[i].q;
        q->size = queue_size;
        q->tasks = p->tasks + queue_size * i;
        switch(thrd_create(p->threads + i, pool2_thread, q)) {
        case thrd_success: break;
        default: perror("thrd_create"); goto e0;
        }
    }
    p->size = size;
    return true;
e0:
    p->size = i;
    pool2_destroy(p);
    return false;
}

bool pool2_destroy(struct pool2 *p) {
    if(!p->size)
        return true;
    bool ret = true;
    for(size_t i = 0, n = p->size; i < n; ++i)
        ret = pool2_enqueue(p, (struct task){0}) && ret;
    for(size_t i = 0, n = p->size; i < n; ++i)
        thrd_join(p->threads[i], NULL);
    free(p->threads);
    free(p->workers);
    free(p->tasks);
    return true;
}

bool pool2_enqueue(struct pool2 *p, struct task t) {
    const size_t i = p->cur_queue;
    bool ret = queue2_push(&p->workers[i].q, t);
    p->cur_queue = (i + 1) % p->size;
    return ret;
}
