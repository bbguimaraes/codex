#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>

#define N 1024

struct queue {
    atomic_uint i;
    int v[N];
};

int *advance(struct queue *q) {
    const memory_order rlx = memory_order_relaxed;
    for(unsigned i = atomic_load_explicit(&q->i, rlx); i != N;)
        if(atomic_compare_exchange_weak_explicit(&q->i, &i, i + 1, rlx, rlx))
            return q->v + i;
    return NULL;
}

bool push(struct queue *q, int i) {
    int *const p = advance(q);
    return p && (*p = i, true);
}

bool pop(struct queue *q, int *i) {
    const int *const p = advance(q);
    return p && (*i = *p, true);
}

bool is_full(const struct queue *q) {
    return atomic_load_explicit(&q->i, memory_order_relaxed) == N;
}
