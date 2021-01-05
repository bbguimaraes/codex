AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I@srcdir@/..
AM_CXXFLAGS = -std=c++2a -O2 -Wall -Wextra -pedantic

TESTS = $(check_PROGRAMS)
if ENABLE_TESTS
check_PROGRAMS = mutable_test
endif

noinst_HEADERS = mutable.hpp mutable_test.hpp

check_CXXFLAGS = -fPIC -fsanitize=address,undefined

mutable_test_CXXFLAGS = $(AM_CXXFLAGS) $(TEST_DEPS_CFLAGS) $(check_CXXFLAGS)
mutable_test_LDFLAGS = $(AM_LDFLAGS) $(TEST_DEPS_LIBS)
mutable_test_SOURCES = mutable_test.cpp mutable_test.moc.cpp

%.moc.cpp: %.hpp
	moc -o $@ $^
