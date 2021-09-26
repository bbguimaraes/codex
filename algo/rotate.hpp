#ifndef CODEX_ALGO_ROTATE_H
#define CODEX_ALGO_ROTATE_H

#include <iterator>

namespace codex {

template<std::permutable I, std::sentinel_for<I> S>
void rotate_rec(I b, I n, S e) {
    if(b == n || n == e)
        return;
    for(auto i = n;;) {
        std::iter_swap(b++, i++);
        if(b == n)
            return rotate_rec(b, i, e);
        if(i == e)
            return rotate_rec(b, n, e);
    }
}

template<std::permutable I, std::sentinel_for<I> S>
void rotate(I b, I n, S e) {
    if(b == n || n == e)
        return;
    for(auto i = n;;) {
        std::iter_swap(b++, i++);
        if(i == e) {
            if(b == n)
               return;
            i = n;
        } else if(b == n)
            n = i;
    }
}

}

#endif
