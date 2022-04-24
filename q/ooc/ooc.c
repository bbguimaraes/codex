#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define get_method(C, c, m) (((const struct C*)c)->m)
#define call_method(C, m, o, ...) \
    (get_method(C, get_class(o), m)(o __VA_OPT__(,) __VA_ARGS__))
#define call_super_method(SC, c, m, o, ...) \
    (get_method(SC, get_super(c), m)(o __VA_OPT__(,) __VA_ARGS__))

struct Class {
    const struct Class *super;
    const char *name;
    size_t size;
    void *(*new)(void *self, va_list *args);
    void *(*delete)(void *self);
    int (*cmp)(const void *self, const void *p);
    void (*dump)(const void *self, FILE *f);
};

struct Object { struct Class *class; };

struct AClass {
    struct Class class;
    int (*a)(const void *self);
};

struct BClass {
//    struct Class a;
    struct AClass a;
    int (*b)(const void *self);
};

struct CClass {
//    struct AClass a;
    struct BClass b;
    int (*c)(const void *self);
};

const void *get_class(const void *self) {
    const struct Object *obj = self;
    assert(obj);
    const void *ret = obj->class;
    assert(ret);
    return ret;
}

const void *get_super(const void *self) {
    const struct Class *class = self;
    assert(class);
    const void *ret = class->super;
    assert(ret);
    return ret;
}

void *new(void *class_p, ...) {
    struct Class *class = class_p;
    assert(class);
    assert(class->size);
    struct Object *obj = calloc(1, class->size);
    assert(obj);
    obj->class = class;
    va_list args;
    va_start(args, class_p);
    assert(class->new);
    obj = class->new(obj, &args);
    va_end(args);
    return obj;
}

void delete(void *self) {
    if(self)
        free(call_method(Class, delete, self));
}

int cmp(const void *self, const void *p) {
    return call_method(Class, cmp, self, p);
}

void dump(const void *self, FILE *f) {
    call_method(Class, dump, self, f);
}

void *object_new(void *self, va_list*) { return self; }
void *object_delete(void *self) { return self; }

int object_cmp(const void *self, const void *p) {
    return self < p ? -1
        : self > p ? 1
        : 0;
}

void object_dump(const void *self, FILE *f) {
    const struct Class *class = get_class(self);
    fprintf(f, "%s at %p", class->name, self);
}

void *class_delete(void*) { return NULL; }

void *class_new(void *self_p, va_list *args) {
    struct Class *self = self_p;
    self->name = va_arg(*args, const char*);
    self->super = va_arg(*args, const void*);
    self->size = va_arg(*args, size_t);
    assert(self->super);
    size_t off = offsetof(struct Class, new);
    memcpy(&self->new, &self->super->new, sizeof(struct Class) - off);
    typedef void(voidf)();
    const voidf *sel;
    while((sel = va_arg(*args, const voidf*))) {
        voidf *fp = va_arg(*args, voidf*);
        if(sel == (voidf*)new)
            *(voidf**)&self->new = fp;
        else if(sel == (voidf*)delete)
            *(voidf**)&self->delete = fp;
        else if(sel == (voidf*)cmp)
            *(voidf**)&self->cmp = fp;
        else if(sel == (voidf*)dump)
            *(voidf**)&self->dump = fp;
    }
    return self;
}

static struct Class base[2] = {{
    .super = base,
    .name = "Object",
    .size = sizeof(struct Object),
    .new = object_new,
    .delete = object_delete,
    .cmp = object_cmp,
    .dump = object_dump,
}, {
    .super = base,
    .name = "Class",
    .size = sizeof(struct Class),
    .new = class_new,
    .delete = class_delete,
    .cmp = object_cmp,
    .dump = object_dump,
}};

static struct Class *Object = base;
static struct Class *Class = base + 1;

struct A { struct Object obj; int a; };
struct B { struct A a; int b; };
struct C { struct B b; int c; };

void *AClass, *A, *BClass, *B, *CClass, *C;

void *a_new(void *self, va_list *args) {
    struct A *a = call_super_method(Class, A, new, self, args);
    a->a = va_arg(*args, int);
    return a;
}

void a_dump(const void *self, FILE *f) {
    const struct A *a = self;
    call_super_method(Class, A, dump, self, f);
    fprintf(f, " a = %d", a->a);
}

int a_a(const void *self) {
    return ((const struct A*)self)->a;
}

void *a_class_new(void *self, va_list *args) {
    struct AClass *a = call_super_method(Class, AClass, new, self, args);
    // TODO selector
    a->a = a_a;
    return a;
}

void *b_new(void *self, va_list *args) {
    struct B *b = call_super_method(Class, B, new, self, args);
    b->b = va_arg(*args, int);
    return b;
}

void b_dump(const void *self, FILE *f) {
    const struct B *b = self;
    call_super_method(Class, B, dump, self, f);
    fprintf(f, " b = %d", b->b);
}

int b_b(const void *self) {
    return ((struct B*)self)->b;
}

void *b_class_new(void *self, va_list *args) {
    struct BClass *b = call_super_method(Class, BClass, new, self, args);
    // TODO selector
    b->b = b_b;
    return b;
}

void *c_new(void *self, va_list *args) {
    struct C *c = call_super_method(Class, C, new, self, args);
    c->c = va_arg(*args, int);
    return c;
}

void c_dump(const void *self, FILE *f) {
    const struct C *c = self;
    call_super_method(Class, C, dump, self, f);
    fprintf(f, " c = %d", c->c);
}

int c_c(const void *self) {
    return ((struct C*)self)->c;
}

void *c_class_new(void *self, va_list *args) {
    struct CClass *c = call_super_method(Class, CClass, new, self, args);
    // TODO selector
    c->c = c_c;
    return c;
}

int main(int argc, char **argv, char **env) {
    AClass = new(
        Class, "AClass", Class, sizeof(struct AClass),
        new, a_class_new, 0);
    BClass = new(
        Class, "BClass", AClass, sizeof(struct BClass),
        new, b_class_new, 0);
    CClass = new(
        Class, "CClass", BClass, sizeof(struct CClass),
        new, c_class_new, 0);
    dump(AClass, stderr), fputc('\n', stderr);
    dump(BClass, stderr), fputc('\n', stderr);
    dump(CClass, stderr), fputc('\n', stderr);
    A = new(
        AClass, "A", Object, sizeof(struct A),
        new, a_new, dump, a_dump, 0);
    B = new(
        BClass, "B", A, sizeof(struct B),
        new, b_new, dump, b_dump, 0);
    C = new(
        CClass, "C", B, sizeof(struct C),
        new, c_new, dump, c_dump, 0);
    void *a = new(A, 1);
    void *b = new(B, 1, 2);
    void *c = new(C, 1, 2, 3);
    dump(a, stderr), fputc('\n', stderr);
    dump(b, stderr), fputc('\n', stderr);
    dump(c, stderr), fputc('\n', stderr);
    printf("%d\n", call_method(AClass, a, a));
    printf("%d\n", call_method(AClass, a, b));
    printf("%d\n", call_method(AClass, a, c));
    printf("%d\n", call_method(BClass, b, b));
    printf("%d\n", call_method(BClass, b, c));
    printf("%d\n", call_method(CClass, c, c));
    delete(c);
    delete(b);
    delete(a);
    delete(CClass);
    delete(BClass);
    delete(AClass);
}
