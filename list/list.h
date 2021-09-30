#ifndef CODEX_LIST_H
#define CODEX_LIST_H

#include <stddef.h>

struct node {
    struct node *next;
    int i;
};

static inline void list_push_front(struct node **l, struct node *n) {
    n->next = *l;
    *l = n;
}

static inline struct node *list_reverse(struct node *n) {
    struct node *ret = NULL;
    for(struct node *next = NULL; n; n = next) {
        next = n->next;
        list_push_front(&ret, n);
    }
    return ret;
}

static inline struct node *list_remove(struct node **n, int i) {
    for(; *n; n = &(*n)->next) {
        if((*n)->i != i)
            continue;
        struct node *const ret = *n;
        *n = ret->next;
        return ret;
    }
    return NULL;
}

#endif
