#include <stddef.h>

#define vptr_shared(b, d) \
    union { \
        struct b b; \
        const struct d ## _vtbl *vptr; \
    }

#define offset_from_base(d, b) (-(ptrdiff_t)offsetof(d, b))
#define call(t, vf, p) call_impl((p)->t.vptr->vf, (p))
#define vcall(vf, p) call_impl((p)->vptr->vf, (p))
#define call_impl(vf, p) (((vf).f)(voff(&(vf), (p))))

struct vfn { void (*f)(); ptrdiff_t off; };

void *voff(const struct vfn *v, void *p) {
    return (char*)p + v->off;
}

struct a_vtbl { const struct vfn af0, af1; };
struct b_vtbl { const struct vfn bf0, bf1; };
struct c_vtbl { const struct vfn cf0, cf1; };
struct d_vtbl { const struct a_vtbl a; const struct vfn df; };

struct a { const struct a_vtbl *vptr; const char *data; };
struct b { const struct b_vtbl *vptr; const char *data; };
struct c { const struct c_vtbl *vptr; const char *data; };
struct d { vptr_shared(a, d); struct b b; struct c c; const char *data; };

void af0(struct a*), bf0(struct b*), cf0(struct c*);
void af1(struct a*), bf1(struct b*), cf1(struct c*);

const struct a_vtbl a_vtbl = {.af0 = {.f = af0}, .af1 = {.f = af1}};
const struct b_vtbl b_vtbl = {.bf0 = {.f = bf0}, .bf1 = {.f = bf1}};
const struct c_vtbl c_vtbl = {.cf0 = {.f = cf0}, .cf1 = {.f = cf1}};

void d_af1(struct d*), d_bf1(struct d*), d_cf1(struct d*), df(struct d*);

const struct d_vtbl d_vtbl = {
    .a = {
        .af0 = {.f = af0},
        .af1 = {.f = d_af1},
    },
    .df = {.f = df},
};

const struct b_vtbl db_vtbl = {
    .bf0 = {.f = bf0},
    .bf1 = {.f = d_bf1, .off = offset_from_base(struct d, b)},
};

const struct c_vtbl dc_vtbl = {
    .cf0 = {.f = cf0},
    .cf1 = {.f = d_cf1, .off = offset_from_base(struct d, c)},
};

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
    vcall(af0, &a), vcall(af1, &a);
    vcall(bf0, &b), vcall(bf1, &b);
    vcall(cf0, &c), vcall(cf1, &c);
    vcall(df, &d);
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
