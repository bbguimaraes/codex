#ifndef CODEX_C_WRAPPER_H
#define CODEX_C_WRAPPER_H

#include <cassert>
#include <utility>

namespace codex {

/**
 * Wrapper for C-style `void*` + function pointer pair.
 */
template<auto F> struct CWrapper;

/**
 * Wraps a C-style function taking `void*` and calls it with `T*` instead.
 */
template<typename T, typename R, typename ...Args, R (T::*f)(Args...)>
struct CWrapper<f> {
    static R wrapper(void *p, Args ...args) {
        assert(p);
        return (static_cast<T*>(p)->*f)(std::forward<Args>(args)...);
    }
    (*operator R())(void*, Args...) { return &CWrapper::wrapper; }
};

}

#endif
