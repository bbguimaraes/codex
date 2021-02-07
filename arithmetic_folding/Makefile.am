AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I@srcdir@/..
AM_CXXFLAGS = \
	-std=c++2a -O2 -Wall -Wextra -pedantic \
	-funsafe-math-optimizations -ffinite-math-only

TESTS = $(check_PROGRAMS)
if ENABLE_TESTS
check_PROGRAMS = rotate_test
endif

noinst_HEADERS = rotate.hpp rotate_test.hpp

check_CXXFLAGS = -fPIC -fsanitize=address,undefined

rotate_test_CXXFLAGS = $(AM_CXXFLAGS) $(TEST_DEPS_CFLAGS) $(check_CXXFLAGS)
rotate_test_LDFLAGS = $(AM_LDFLAGS) $(TEST_DEPS_LIBS)
rotate_test_SOURCES = rotate.cpp rotate_test.cpp rotate_test.moc.cpp

%.moc.cpp: %.hpp
	moc -o $@ $^
