#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define container_of(p, t, m) ((t*)((char*)(p) - offsetof(t, m)))

typedef uint32_t u32;

char *array_alloc(size_t n);
size_t array_size(char *p);
void array_destroy(char *p);

struct array {
    size_t n;
    u32 flags;
    char data[];
};
static_assert(sizeof(struct array) == 2 * sizeof(size_t));

char *array_alloc(size_t n) {
    struct array *const ret = malloc(sizeof(struct array) + n);
    *ret = (struct array){.n = n, /*.flags = ...*/};
    return ret->data;
}

size_t array_size(char *p) {
    return container_of(p, struct array, data)->n;
}

void array_destroy(char *p) {
    free(container_of(p, struct array, data));
}
