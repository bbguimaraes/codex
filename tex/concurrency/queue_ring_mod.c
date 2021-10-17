#include <stdatomic.h>
#include <stdbool.h>

#define N 1024

struct queue {
    atomic_uint i;
    int v[N];
};

int *advance(struct queue *q) {
    const unsigned i =
        atomic_fetch_add_explicit(
            &q->i, 1,
            memory_order_relaxed);
    return q->v + i % N;
}

void push(struct queue *q, int i) {
    *advance(q) = i;
}

void pop(struct queue *q, int *i) {
    *i = *advance(q);
}
