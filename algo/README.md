Algorithms
==========

`rotate`
--------

```cpp
template< std::permutable I, std::sentinel_for<I> S>
constexpr ranges::subrange<I> rotate(I first, I middle, S last);
```

> Performs a left rotation on a range of elements.  Specifically, `rotate` swaps
> the elements in the range `[first, last)` in such a way that the element
> `*middle` becomes the first element of the new range and `*(middle - 1)`
> becomes the last element.

```cpp
extern std::size_t n;
std::array a = {0, 1, 2, 3, 4, 5, 6, 7};
rotate(begin(a), begin(a) + n, end(a));
```

```
n = 1
|0 1 2 3 4 5 6 7|  1|0 2 3 4 5 6 7|  1 2|0 3 4 5 6 7|  …  1 2 3 4 5 6|0 7|
 i n                 i n                 i n                          i n
|1 0 2 3 4 5 6 7|  1|2 0 3 4 5 6 7|  1 2|0 3 4 5 6 7|      1 2 3 4 5 6|7 0|
   i n                 i n                 i n                           i n
```

```
n = 2
|0 1 2 3 4 5 6 7|  2 3|0 1 4 5 6 7|  2 3 4 5|0 1 6 7|  2 3 4 5 6 7|0 1|
 i   n                 i   n                 i   n                 i   n
|2 1 0 3 4 5 6 7|  2 3|4 1 0 5 6 7|  2 3 4 5|6 1 0 7|
   i   n                 i   n                 i   n
|2 3 0 1 4 5 6 7|  2 3|4 5 0 1 6 7|  2 3 4 5|6 7 0 1|
     i   n                 i   n                 i   n
```

```
n = 5
|0 1 2 3 4 5 6 7|  5 6 7|3 4 0 1 2|  5 6 7 0 1|3 4 2|  5 6 7 0 1 2|4 3|
 i         n             i   n                 i   n               i n
|5 1 2 3 4 0 6 7|  5 6 7|0 4 3 1 2|  5 6 7 0 1|2 4 3|  5 6 7 0 1 2|3 4|
   i         n             i   n                 i   n
|5 6 2 3 4 0 1 7|  5 6 7|0 1 3 4 2|
     i         n             i   n
|5 6 7 3 4 0 1 2|
       i         n
```
