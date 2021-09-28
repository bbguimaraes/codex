#ifndef CODEX_LIST_H
#define CODEX_LIST_H

#include <stddef.h>

struct node {
    struct node *next;
    int i;
};

struct node *list_reverse(struct node *n) {
    struct node *last = NULL;
    while(n) {
        struct node *const next = n->next;
        n->next = last, last = n, n = next;
    }
    return last;
}

struct node *list_remove(struct node **n, int i) {
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
