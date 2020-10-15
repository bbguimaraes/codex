base64
======

A Base64 (RFC 4648) encoder and decoder.  The implementation uses a pair of
look-up and reverse look-up tables generated automatically at compile time.

benchmarks
----------

The `bench` directory compares the performance against both the reference
implementation included in RFC 4648 and the `coreutils` version usually present
in GNU/Linux systems:

![](https://bbguimaraes.com/codex/base64/bench.svg)

`base64`, `rfc`, and `system` in these graphs are the `codex`, RFC 4648, and
`coreutils` versions, respectively.  All of them are single-threaded
implementations.  `encode` and `decode` measure in-memory encoding and decoding
of random byte arrays.  `exec` measures both encoding and decoding of `tmpfs`
files containing random bytes by executing the compiled binaries of each
implementation, and demonstrates that the performance of all is mostly dominated
by file I/O.  In all cases, the number above the graph indicates the input size.
