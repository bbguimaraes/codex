// g++ -std=c++20 -masm=intel -fno-stack-protector -fno-exceptions -fno-rtti -fno-asynchronous-unwind-tables %
#include <stdio.h>

#define assert_equal(e, m) assert_equal0(e, #m, m)
#define assert_equal0(e, s, m) printf("%s: %s\n", (s), #m)

#define y(x) x + x
#define x(x) y(x)

int main(int argc, char **argv, char **env) {
    assert_equal(, x(0));
}
