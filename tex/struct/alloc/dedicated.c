#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct dedicated0 { int *i; };
struct dedicated1 { float *f; };

struct dedicated {
    struct dedicated0 *s0;
    struct dedicated1 *s1;
    double *d;
    char *name;
};

struct dedicated *dedicated_create(void) {
    const char name[] = "dedicated";
    struct dedicated *const ret = malloc(sizeof(struct dedicated));
    *ret = (struct dedicated){
        .s0 = malloc(sizeof(struct dedicated0)),
        .s1 = malloc(sizeof(struct dedicated1)),
        .d = malloc(sizeof(double)),
        .name = malloc(sizeof(name)),
    };
    *(ret->s0->i = malloc(sizeof(ret->s0->i))) = 42;
    *(ret->s1->f = malloc(sizeof(ret->s1->f))) = 43.0f;
    *ret->d = 44.0;
    strcpy(ret->name, name);
    return ret;
}
