cpp
===

An implementation of the macro expansion facility of the C preprocessor in Lua.
Detailed information is output at each step of the expansions.

The goal of this project is to explore macro expansion, not write a robust
parser, so it is based on Lua patterns (which are not regular) and very
primitive.  For these reasons, notably missing is support for:

- comments
- string literals (but the `#` and `##` operators are supported)
- parentheses in macro calls
- idempotent redefinitions (i.e. without a previous `#undef`)
- inclusion of subsequent tokens when the replacement text of a macro is
  recursively expanded
- escaped new-line characters not surrounded by white space (e.g. in the middle
  of a macro identifier)
- most source code validations
- probably much more

Code is generally assumed to be correct.  Make sure to both validate the input
and verify the output using your favorite C compiler and preprocessor (GCC's
`cpp` and Clang's `clang-cpp` are used as a reference).  The `tests` directory
contains an extensive test suite with examples from the C standard and the GCC
documentation.

Lexer
-----

The lexer can be executed independently with the `-l`/`--lexer-only` argument:

```console
$ cat tests/0.c
#define X
X
$ ./cpp.lua --lexer-only tests/0.c
1:1 DEFINE "#define"
1:8 SPACE " "
1:9 IDENTIFIER "X"
2:1 NEW_LINE "\\n"
2:2 IDENTIFIER "X"
3:1 NEW_LINE "\\n"
```

This output can also appear at the beginning of the execution with the `--lexer`
argument.

```console
$ cat tests/1.c
#define X x
X
$ ./cpp.lua --lexer tests/1.c
1:1 DEFINE "#define"
1:8 SPACE " "
1:9 IDENTIFIER "X"
1:10 SPACE " "
1:11 IDENTIFIER "x"
2:1 NEW_LINE "\\n"
2:2 IDENTIFIER "X"
3:1 NEW_LINE "\\n"
x
```

Example
-------

```console
$ cat tests/std0.c
#define hash_hash # ## #
#define str(x) #x
#define in_between(x) str(x)
#define join(x, y) in_between(x hash_hash y)
join(x, y)
$ lua cpp.lua -v --no-color tests/std0.c
tests/std0.c

=== macros ===

function:
  "str":
    parameters:
    - "x"
    replacement: "str(x)"
  "join":
    parameters:
    - "x"
    - "y"
    replacement: "in_between(x hash_hash y)"
  "str":
    parameters:
    - "x"
    replacement: "#x"

object:
  "hash_hash":
    replacement: "# ## #"

=== steps ===

processing directive line "#define hash_hash # ## #"

added object-like macro "hash_hash"

processing source block

   | join(x, y)
   |

replacing invocation of macro "join"

   | join(x, y) |

replaced macro "join" with replacement text

   | in_between(x hash_hash y) |

replacing parameter "x" with argument

  in_between( | x |  hash_hash y)

[…]

replaced function-like macro "str" with "\"x ## y\""

   | "x ## y" |

macro "in_between" recursively expanded to "\"x ## y\""

macro "join" recursively expanded to "\"x ## y\""


=== input ===

#define hash_hash # ## #
#define str(x) #x
#define in_between(x) str(x)
#define join(x, y) in_between(x hash_hash y)
join(x, y)

=== output ===

"x ## y"
```
