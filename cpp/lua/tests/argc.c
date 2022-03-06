#define ARGC(...) ARGC0(__VA_ARGS__ __VA_OPT__(,) 7, 6, 5, 4, 3, 2, 1, 0)
#define ARGC0(_0, _1, _2, _3, _4, _5, _6, _7, ...) _7
ARGC()
ARGC(_0)
ARGC(_0, _1)
