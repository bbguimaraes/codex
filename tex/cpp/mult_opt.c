#include <stddef.h>

#define container_of(p, t, m) ((t*)((char*)(p) - offsetof(t, m)))

#define vptr_shared(b, d) \
    union { \
        struct b b; \
        const struct d ## _vtbl *vptr; \
    }

#define call(t, vf, p) (((p)->t.vptr->vf)((p)))
#define vcall(vf, p) (((p)->vptr->vf)((p)))

struct a_vtbl { void (*af0)(), (*af1)(); };
struct b_vtbl { void (*bf0)(), (*bf1)(); };
struct c_vtbl { void (*cf0)(), (*cf1)(); };
struct d_vtbl { const struct a_vtbl a; void (*df)(); };

struct a { const struct a_vtbl *vptr; const char *data; };
struct b { const struct b_vtbl *vptr; const char *data; };
struct c { const struct c_vtbl *vptr; const char *data; };
struct d { vptr_shared(a, d); struct b b; struct c c; const char *data; };

void af0(struct a*), bf0(struct b*), cf0(struct c*);
void af1(struct a*), bf1(struct b*), cf1(struct c*);

const struct a_vtbl a_vtbl = {.af0 = af0, .af1 = af1};
const struct b_vtbl b_vtbl = {.bf0 = bf0, .bf1 = bf1};
const struct c_vtbl c_vtbl = {.cf0 = cf0, .cf1 = cf1};

void d_af1(struct d*), d_bf1(struct d*), d_cf1(struct d*), df(struct d*);
void d_pre_af1(struct d *p) { d_af1(container_of(p, struct d, a)); }
void d_pre_bf1(struct d *p) { d_bf1(container_of(p, struct d, b)); }
void d_pre_cf1(struct d *p) { d_cf1(container_of(p, struct d, c)); }

const struct d_vtbl d_vtbl = {
    .a = {.af0 = af0, .af1 = d_pre_af1},
    .df = df,
};

const struct b_vtbl db_vtbl = {.bf0 = bf0, .bf1 = d_pre_bf1};
const struct c_vtbl dc_vtbl = {.cf0 = cf0, .cf1 = d_pre_cf1};

int main(void) {
    struct d d = {
        .a = {.vptr = &d_vtbl.a, .data = "da"},
        .b = {.vptr = &db_vtbl, .data = "db"},
        .c = {.vptr = &dc_vtbl, .data = "dc"},
        .data = "d",
    };
    struct a a = {.vptr = &a_vtbl, .data = "a"}, *ap = &d.a;
    struct b b = {.vptr = &b_vtbl, .data = "b"}, *bp = &d.b;
    struct c c = {.vptr = &c_vtbl, .data = "c"}, *cp = &d.c;
    struct d *dp = &d;
    af0(&a), af1(&a);
    bf0(&b), bf1(&b);
    cf0(&c), cf1(&c);
    df(&d);
    vcall(af0, ap), call(a, af0, dp);
    vcall(af1, ap), call(a, af1, dp);
    vcall(bf0, bp), call(b, bf0, dp),
    vcall(bf1, bp), call(b, bf1, dp);
    vcall(cf0, cp), call(c, cf0, dp),
    vcall(cf1, cp), call(c, cf1, dp);
    vcall(df, dp);
}

#include <stdio.h>
#define X(n, t, m) void n(t *p) { printf("%-5s %s\n", __func__, p->m data); }
X(af0, struct a,)
X(bf0, struct b,)
X(cf0, struct c,)
X(af1, struct a,)
X(bf1, struct b,)
X(cf1, struct c,)
X(df, struct d,)
X(d_af1, struct d, a.)
X(d_bf1, struct d, b.)
X(d_cf1, struct d, c.)
#undef X
