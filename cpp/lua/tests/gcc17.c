#define foo (a,b)
#define bar(x) lose(x)
#define lose(x) (1 + (x))
bar(foo)

#undef bar
#define bar(x) lose((x))
bar(foo)
