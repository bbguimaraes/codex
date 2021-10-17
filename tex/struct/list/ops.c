#include <stddef.h>

struct value;
void use(struct value*);

struct node {
    struct node *next;
};

#define container_of(p, t, m) ((t*)((char*)(p) - offsetof(t, m)))
#define list_entry(p, t, f) ((void*)container_of(p, t, f))
#define list_for_each(h, v) for(struct node *v = (h); v; v = v->next)

void list_push_front(struct node **l, struct node *n) {
    n->next = *l;
    *l = n;
}

void list_pop_front(struct node **l) {
    *l = (*l)->next;
}

struct container {
    int i;
    float f;
    struct node *head;
};

struct value {
    char *s;
    struct node *next;
};

void f(struct container *c) {
    struct node n = {0};
    list_push_front(&c->head, &n);
    list_for_each(c->head, n) {
        struct value *v = list_entry(n, struct value, next);
        use(v);
    }
    list_pop_front(&c->head);
}
