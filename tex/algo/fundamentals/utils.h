#include <stdbool.h>

bool is_sorted(const int *b, const int *e) {
    if(b == e)
        return true;
    while(++b != e)
        if(!(b[-1] < b[0]))
            return false;
    return true;
}

bool lsearch(const int *b, const int *e, int x) {
    for(; b != e; ++b)
        if(*b == x)
            return true;
    return false;
}
