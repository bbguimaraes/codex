#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

bool is_sorted(const int *b, const int *e);
bool lsearch(int x, const int *b, const int *e);

bool bsearch(int x, const int *b, const int *e) {
    assert(is_sorted(b, e));
    const int *const ib = b, *const ie = e;
    while(b != e) {
        assert(ib <= b && b <= e && e <= ie);
        assert(!lsearch(x, ib, b) && !lsearch(x, e, ie));
        assert(b == ib || b[-1] < x);
        assert(e == ie || x < *e);
        const int *const m = b + (size_t)(e - b) / 2;
        assert(b <= m && m < e);
        if(*m == x)
            return true;
        else if(*m < x)
            b = m + 1;
        else
            e = m;
    }
    assert(ib <= b && b == e && b <= ie);
    assert(!lsearch(x, ib, ie));
    assert(b == ib || b[-1] < x);
    assert(b == ie || x < *b);
    return false;
}
