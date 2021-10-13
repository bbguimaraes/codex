AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I@srcdir@/..
AM_CXXFLAGS = -std=c++2a -O2 -Wall -Wextra -pedantic

if ENABLE_BENCHMARKS
#noinst_PROGRAMS = bench
endif

TESTS = $(check_PROGRAMS)
if ENABLE_TESTS
check_PROGRAMS = test
endif

noinst_HEADERS = \
	selection.hpp \
	test.hpp \
	test.cpp

check_CXXFLAGS = -fPIC -fsanitize=address,undefined

test_CXXFLAGS = $(AM_CXXFLAGS) $(TEST_DEPS_CFLAGS) $(check_CXXFLAGS)
test_LDFLAGS = $(AM_LDFLAGS) $(TEST_DEPS_LIBS)
test_SOURCES = \
	selection.cpp \
	test.cpp \
	test.moc.cpp

#bench_CXXFLAGS = $(AM_CXXFLAGS) $(TEST_DEPS_CFLAGS) -fPIC
#bench_LDFLAGS = $(AM_LDFLAGS) $(TEST_DEPS_LIBS)
#bench_SOURCES = \
#	bench.cpp \
#	bench.moc.cpp

%.moc.cpp: %.hpp
	moc -o $@ $^
