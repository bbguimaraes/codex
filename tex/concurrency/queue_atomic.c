#include <stdbool.h>
#include <stdatomic.h>

#define N 1024

struct queue {
    atomic_uint i;
    int v[N];
};

int *advance(struct queue *q) {
    return q->v + atomic_fetch_add_explicit(&q->i, 1, memory_order_relaxed);
}

void push(struct queue *q, int i) {
    *advance(q) = i;
}

void pop(struct queue *q, int *i) {
    *i = *advance(q);
}

bool is_full(const struct queue *q) {
    return atomic_load_explicit(&q->i, memory_order_relaxed) == N;
}
