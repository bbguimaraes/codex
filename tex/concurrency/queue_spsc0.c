#include <stdbool.h>

#define N 1024

struct queue {
    volatile unsigned w;
    unsigned r;
    int v[N];
};

bool push(struct queue *q, int i) {
    const unsigned w = q->w;
    if(w == N)
        return false;
    q->v[w] = i;
    q->w = w + 1;
    return true;
}

bool pop(struct queue *q, int *i) {
    const unsigned w = q->w;
    const unsigned r = q->r;
    if(r == w)
        return false;
    *i = q->v[r];
    q->r = r + 1;
    return true;
}
