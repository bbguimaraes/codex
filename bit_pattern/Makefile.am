AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I@srcdir@/..
AM_CXXFLAGS = -std=c++2a -O2 -Wall -Wextra -pedantic

TESTS = $(check_PROGRAMS)
if ENABLE_TESTS
check_PROGRAMS = bit_pattern_test
endif

noinst_HEADERS = bit_pattern_test.hpp

check_CXXFLAGS = -fPIC -fsanitize=address,undefined

bit_pattern_test_CXXFLAGS = $(AM_CXXFLAGS) $(TEST_DEPS_CFLAGS) $(check_CXXFLAGS)
bit_pattern_test_LDFLAGS = $(AM_LDFLAGS) $(TEST_DEPS_LIBS)
bit_pattern_test_SOURCES = bit_pattern_test.cpp bit_pattern_test.moc.cpp

%.moc.cpp: %.hpp
	moc -o $@ $^
