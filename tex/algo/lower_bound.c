#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

bool is_sorted(const int *b, const int *e);
bool lsearch(int x, const int *b, const int *e);

int *lower_bound(int x, int *b, int *e) {
    assert(is_sorted(b, e));
    int *const ib = b, *const ie = e;
    while(b != e) {
        assert(ib <= b && b <= e && e <= ie);
        assert(!lsearch(x, ib, b));
        assert(b == ib || b[-1] < x);
        assert(e == ie || !(*e < x));
        int *const m = b + (size_t)(e - b) / 2;
        if(*m < x)
            b = m + 1;
        else
            e = m;
    }
    assert(ib <= b && b == e && b <= ie);
    assert(b == ib || b[-1] < x);
    assert(b == ie || !(*b < x) || !lsearch(x, ib, ie));
    return b;
}
