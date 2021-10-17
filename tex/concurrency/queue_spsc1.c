#include <stdatomic.h>
#include <stdbool.h>

#define N 1024

struct queue {
    atomic_uint w, r;
    int v[N];
};

bool try_push(struct queue *q, int i) {
    const unsigned w = q->w;
    if(w >= N)
        return false;
    q->v[w] = i;
    atomic_store_explicit(
        &q->w, w + 1,
        memory_order_release);
    return true;
}

bool try_pop(struct queue *q, int *i) {
    const unsigned w =
        atomic_load_explicit(
            &q->w,
            memory_order_relaxed);
    const unsigned r = q->r;
    if(r >= w)
        return false;
    atomic_thread_fence(
        memory_order_acquire);
    *i = q->v[r];
    q->r = r + 1;
    return true;
}
