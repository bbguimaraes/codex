// https://stroustrup.com/wrapper.pdf
#include <cstdio>

struct S { void f(void) { std::puts("f"); } } s;
void prefix(S*) { std::puts("p"); }
void suffix(S*) { std::puts("s"); }

template<typename T>
struct wrap {
    auto operator->(void) {
        prefix(&this->t);
        struct proxy {
            ~proxy(void) { suffix(&this->t); }
            T *operator->(void) { return &this->t; }
            T &t;
        } ret{this->t};
        return ret;
    }
    T &t;
};

int main(void) { wrap{s}->f(); }
