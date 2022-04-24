AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I@srcdir@/..
AM_CXXFLAGS = -std=c++2a -O2 -Wall -Wextra -pedantic

bin_PROGRAMS = base64
noinst_PROGRAMS = rfc/base64
if ENABLE_BENCHMARKS
noinst_PROGRAMS += bench/bench
endif

TESTS = $(check_PROGRAMS)
if ENABLE_TESTS
check_PROGRAMS = test/test
endif

noinst_HEADERS = \
	base64.hpp \
	block.hpp \
	cmd.hpp \
	rfc/adaptor.hpp \
	rfc/base64.h \
	test/test.hpp \
	bench/bench.hpp

rfc_base64_SOURCES = rfc/base64.c rfc/main.cpp
base64_SOURCES = base64.cpp main.cpp

check_CXXFLAGS = -fPIC -fsanitize=address,undefined

test_test_CXXFLAGS = $(AM_CXXFLAGS) $(TEST_DEPS_CFLAGS) $(check_CXXFLAGS)
test_test_LDFLAGS = $(AM_LDFLAGS) $(TEST_DEPS_LIBS)
test_test_SOURCES = base64.cpp rfc/base64.c test/test.cpp test/test.moc.cpp

bench_bench_CXXFLAGS = $(AM_CXXFLAGS) $(TEST_DEPS_CFLAGS) -fPIC
bench_bench_LDFLAGS = $(AM_LDFLAGS) $(TEST_DEPS_LIBS)
bench_bench_SOURCES = \
	base64.cpp \
	rfc/base64.c \
	bench/bench.cpp bench/bench.moc.cpp

%.moc.cpp: %.hpp
	moc -o $@ $^
