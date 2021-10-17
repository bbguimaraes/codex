#include <stdbool.h>

#define N 1024

struct queue {
    unsigned i;
    int v[N];
};

void push(struct queue *q, int i) {
    q->v[q->i++] = i;
}

void pop(struct queue *q, int *i) {
    *i = q->v[q->i++];
}

bool is_full(const struct queue *q) {
    return q->i == N;
}
