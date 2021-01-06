bit_pattern
===========

Compile-time bit pattern matcher based on
[C++ Weekly ep.  230](https://www.youtube.com/watch?v=-GqMLnWuHTU).

```cpp
static_assert(BitPattern("11xxx1").matches(0b110101u));
static_assert(!BitPattern("11xxx1").matches(0b110100u));
```
