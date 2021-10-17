#include <assert.h>
#include <stdlib.h>
#include <string.h>

enum { NAME_SIZE = 32 };
struct embedded0 { int i; };
struct embedded1 { float f; };

struct embedded {
    struct embedded0 s0;
    struct embedded1 s1;
    double d;
    char name[NAME_SIZE];
};

void embedded_init(struct embedded *p) {
    const char name[] = "embedded";
    *p = (struct embedded){.s0.i = 42, .s1.f = 43.0f, .d = 44.0};
    static_assert(sizeof(name) <= sizeof(p->name));
    strcpy(p->name, name);
}

struct embedded *embedded_alloc(void) {
    struct embedded *const ret = malloc(sizeof(struct embedded));
    embedded_init(ret);
    return ret;
}
