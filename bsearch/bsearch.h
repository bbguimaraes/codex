#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

bool lsearch(int x, const int *p, const int *e) {
    assert(p <= e);
    for(; p != e; ++p)
        if(x == *p)
            return true;
    return false;
}

static inline const int *bsearch_middle(const int *p, const int *e) {
    assert(p <= e);
    return p + ((e - p) >> 1);
}

bool bsearch0(int x, const int *p, const int *e) {
    const ptrdiff_t n = e - p;
    if(n <= 0)
        return false;
    const int *m = bsearch_middle(p, e);
    if(m == p)
        return *m == x;
    if(x < *m)
        return bsearch0(x, p, m);
    if(*m < x)
        return bsearch0(x, m + 1, e);
    return true;
}

bool bsearch1(int x, const int *p, const int *e) {
    while(p != e) {
        const int *m = bsearch_middle(p, e);
        if(m == p)
            return *m == x;
        if(x < *m)
            e = m;
        else if(*m < x)
            p = m + 1;
        else
            return true;
    }
    return false;
}

bool bsearch2(int x, const int *p, const int *e) {
    while(p != e) {
        const int *m = bsearch_middle(p, e);
        if(*m < x)
            p = m + 1;
        else if(x < *m)
            e = m;
        else
            return true;
    }
    return false;
}

const int *lower_bound(int x, const int *p, const int *ie) {
    for(const int *e = ie; p != e;) {
        const int *m = bsearch_middle(p, e);
        if(*m < x)
            p = m + 1;
        else
            e = m;
    }
    return p;
}

bool bsearch3(int x, const int *p, const int *e) {
    p = lower_bound(x, p, e);
    return p != e && !(x < *p);
}
