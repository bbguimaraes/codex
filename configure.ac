AC_PREREQ([2.69])
AC_INIT([codex], [0.0.1])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([base64/main.cpp])

ENABLE_TESTS=no
AC_ARG_ENABLE([tests],
    [AS_HELP_STRING(
        [--enable-tests],
        [build tests, requires Qt (default: no)])],
    [ENABLE_TESTS=$enableval])
AM_CONDITIONAL([ENABLE_TESTS], [test "x$ENABLE_TESTS" == xyes])

ENABLE_BENCHMARKS=no
AC_ARG_ENABLE([benchmarks],
    [AS_HELP_STRING(
        [--enable-benchmarks],
        [build benchmarks, requires Qt (default: no)])],
    [ENABLE_BENCHMARKS=$enableval])
AM_CONDITIONAL([ENABLE_BENCHMARKS], [test "x$ENABLE_BENCHMARKS" == xyes])

AC_PROG_CC
AC_PROG_CXX

AS_IF([test "$ENABLE_TESTS" != no],
    [PKG_CHECK_MODULES(
        [TEST_DEPS], [Qt5Core Qt5Test],
        [TEST_DEPS_CFLAGS=$(echo "$TEST_DEPS_CFLAGS" | sed s/-I/-isystem/g)])])

AC_CONFIG_FILES([
    Makefile
    arithmetic_folding/Makefile
    base64/Makefile
    bit_pattern/Makefile
    bsearch/Makefile
    c_wrapper/Makefile
    event_dispatcher/Makefile
    lambda/Makefile
    packet_reader/Makefile
    popcount/Makefile
    time/Makefile])
AC_OUTPUT
