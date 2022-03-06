extern void foo(void);
#define foo() optimized inline version

foo();
funcptr = foo;
