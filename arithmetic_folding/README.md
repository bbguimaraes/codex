arithmetic_folding
==================

Given two functions:

```cpp
vec3 rotate(const vec3 &v, float sin, float cos, const vec3 &n);
vec3 rotate_y(const vec3 &v, float sin, float cos);

constexpr auto a = 45.0f / 180.0f * std::numbers::pi_v<float>;
constexpr auto sin = std::numbers::sqrt2_v<float> / 2.0f, cos = sin;

vec3 f(const vec3 &v, float sin, float cos)
    { return rotate(v, sin, cos, {0, 1, 0}); }
vec3 g(const vec3 &v, float sin, float cos)
    { return rotate_y(v, sin, cos); }
```

`rotate` uses the traditional vector rotation algorithm, which involves a dot
and cross product and several arithmetic operations.  `rotate_y` is a
specialized version which does a much simpler 2D rotation.  In principle, both
perform the same mathematical operation.  Indeed, if the compiler is instructed
to apply sufficient optimization (`-O2`), disregard IEEE-754 non-commutativity
(`-funsafe-math-optimizations`), and ignore certain special floating-point
values (`-ffinite-math-only`), the exact same or very similar machine code is
generated for both:

```x86masm
; g++ -std=c++20 -O2
0000000000000000 <f(vec3 const&)>:
   0:   53                      push   rbx
   1:   48 89 fb                mov    rbx,rdi
   4:   48 83 ec 30             sub    rsp,0x30
   8:   f3 0f 10 05 00 00 00    movss  xmm0,DWORD PTR [rip+0x0]
   f:   00
  10:   48 8d 7c 24 1c          lea    rdi,[rsp+0x1c]
  15:   48 8d 74 24 18          lea    rsi,[rsp+0x18]
  1a:   e8 00 00 00 00          call   1f <f(vec3 const&)+0x1f>
  1f:   f3 0f 10 3b             movss  xmm7,DWORD PTR [rbx]
  23:   66 0f ef d2             pxor   xmm2,xmm2
  27:   f3 0f 10 73 08          movss  xmm6,DWORD PTR [rbx+0x8]
  2c:   f3 0f 10 6b 04          movss  xmm5,DWORD PTR [rbx+0x4]
  31:   f3 44 0f 10 44 24 18    movss  xmm8,DWORD PTR [rsp+0x18]
  38:   0f 28 c7                movaps xmm0,xmm7
  3b:   0f 28 e6                movaps xmm4,xmm6
  3e:   f3 44 0f 10 4c 24 1c    movss  xmm9,DWORD PTR [rsp+0x1c]
  45:   f3 0f 59 c2             mulss  xmm0,xmm2
  49:   45 0f 28 d0             movaps xmm10,xmm8
  4d:   f3 0f 59 e2             mulss  xmm4,xmm2
  51:   f3 0f 58 c5             addss  xmm0,xmm5
  55:   f3 0f 58 e0             addss  xmm4,xmm0
  59:   0f 28 cc                movaps xmm1,xmm4
  5c:   f3 0f 5c ec             subss  xmm5,xmm4
  60:   f3 0f 59 ca             mulss  xmm1,xmm2
  64:   0f 28 dd                movaps xmm3,xmm5
  67:   f3 0f 59 da             mulss  xmm3,xmm2
  6b:   f3 41 0f 59 e8          mulss  xmm5,xmm8
  70:   f3 0f 5c f1             subss  xmm6,xmm1
  74:   f3 0f 5c f9             subss  xmm7,xmm1
  78:   f3 44 0f 59 d7          mulss  xmm10,xmm7
  7d:   0f 28 c6                movaps xmm0,xmm6
  80:   f3 0f 5c c3             subss  xmm0,xmm3
  84:   f3 0f 5c df             subss  xmm3,xmm7
  88:   f3 41 0f 59 c1          mulss  xmm0,xmm9
  8d:   f3 41 0f 59 d9          mulss  xmm3,xmm9
  92:   f3 41 0f 58 c2          addss  xmm0,xmm10
  97:   f3 0f 58 c1             addss  xmm0,xmm1
  9b:   f3 0f 11 44 24 24       movss  DWORD PTR [rsp+0x24],xmm0
  a1:   0f 28 c7                movaps xmm0,xmm7
  a4:   f3 0f 59 c2             mulss  xmm0,xmm2
  a8:   f3 0f 59 d6             mulss  xmm2,xmm6
  ac:   f3 41 0f 59 f0          mulss  xmm6,xmm8
  b1:   f3 0f 5c c2             subss  xmm0,xmm2
  b5:   f3 0f 58 de             addss  xmm3,xmm6
  b9:   0f 28 d0                movaps xmm2,xmm0
  bc:   0f 28 c5                movaps xmm0,xmm5
  bf:   f3 41 0f 59 d1          mulss  xmm2,xmm9
  c4:   f3 0f 58 cb             addss  xmm1,xmm3
  c8:   f3 0f 58 c2             addss  xmm0,xmm2
  cc:   f3 0f 58 c4             addss  xmm0,xmm4
  d0:   f3 0f 11 44 24 28       movss  DWORD PTR [rsp+0x28],xmm0
  d6:   f3 0f 7e 44 24 24       movq   xmm0,QWORD PTR [rsp+0x24]
  dc:   48 83 c4 30             add    rsp,0x30
  e0:   5b                      pop    rbx
  e1:   c3                      ret
```

```x86masm
; g++ -std=c++20 -O2 -funsafe-math-optimizations -ffinite-math-only
0000000000000000 <f(vec3 const&)>:
   0:   53                      push   rbx
   1:   48 89 fb                mov    rbx,rdi
   4:   48 83 ec 30             sub    rsp,0x30
   8:   f3 0f 10 05 00 00 00    movss  xmm0,DWORD PTR [rip+0x0]
   f:   00
  10:   48 8d 7c 24 1c          lea    rdi,[rsp+0x1c]
  15:   48 8d 74 24 18          lea    rsi,[rsp+0x18]
  1a:   e8 00 00 00 00          call   1f <f(vec3 const&)+0x1f>
  1f:   f3 0f 10 03             movss  xmm0,DWORD PTR [rbx]
  23:   f3 0f 10 4b 08          movss  xmm1,DWORD PTR [rbx+0x8]
  28:   f3 0f 10 64 24 18       movss  xmm4,DWORD PTR [rsp+0x18]
  2e:   f3 0f 10 54 24 1c       movss  xmm2,DWORD PTR [rsp+0x1c]
  34:   0f 28 d9                movaps xmm3,xmm1
  37:   0f 28 f0                movaps xmm6,xmm0
  3a:   f3 0f 10 6b 04          movss  xmm5,DWORD PTR [rbx+0x4]
  3f:   f3 0f 59 da             mulss  xmm3,xmm2
  43:   f3 0f 59 f4             mulss  xmm6,xmm4
  47:   f3 0f 11 6c 24 28       movss  DWORD PTR [rsp+0x28],xmm5
  4d:   f3 0f 59 d0             mulss  xmm2,xmm0
  51:   f3 0f 59 cc             mulss  xmm1,xmm4
  55:   f3 0f 58 de             addss  xmm3,xmm6
  59:   f3 0f 5c ca             subss  xmm1,xmm2
  5d:   f3 0f 11 5c 24 24       movss  DWORD PTR [rsp+0x24],xmm3
  63:   f3 0f 7e 44 24 24       movq   xmm0,QWORD PTR [rsp+0x24]
  69:   48 83 c4 30             add    rsp,0x30
  6d:   5b                      pop    rbx
  6e:   c3                      ret
```
