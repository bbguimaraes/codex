#include <stdbool.h>
#include <threads.h>

#define N 1024

struct queue {
    mtx_t mtx;
    unsigned i;
    int v[N];
};

void init(struct queue *q) {
    mtx_init(&q->mtx, mtx_plain);
}

void push(struct queue *q, int i) {
    mtx_lock(&q->mtx);
    q->v[q->i++] = i;
    mtx_unlock(&q->mtx);
}

void pop(struct queue *q, int *i) {
    mtx_lock(&q->mtx);
    *i = q->v[q->i++];
    mtx_unlock(&q->mtx);
}

bool is_full(struct queue *q) {
    mtx_lock(&q->mtx);
    const unsigned i = q->i;
    mtx_unlock(&q->mtx);
    return i == N;
}
