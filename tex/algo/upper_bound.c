#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

bool is_sorted(const int *b, const int *e);
bool lsearch(int x, const int *b, const int *e);

int *upper_bound(int x, int *b, int *e) {
    assert(is_sorted(b, e));
    int *const ib = b, *const ie = e;
    while(b != e) {
        assert(ib <= b && b <= e && e <= ie);
        assert(!lsearch(x, e, ie));
        assert(b == ib || !(x < b[-1]));
        assert(e == ie || x < *e);
        int *const m = b + (size_t)(e - b) / 2;
        if(!(x < *m))
            b = m + 1;
        else
            e = m;
    }
    assert(ib <= b && b == e && b <= ie);
    assert(b == ib || !(x < b[-1]));
    assert(b == ie || x < *e || !lsearch(x, ib, ie));
    return b;
}
