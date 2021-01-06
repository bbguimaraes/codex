AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I@srcdir@/..
AM_CXXFLAGS = -std=c++2a -O2 -Wall -Wextra -pedantic

TESTS = $(check_PROGRAMS)
if ENABLE_TESTS
check_PROGRAMS = c_wrapper_test
endif

noinst_HEADERS = c_wrapper.hpp c_wrapper_test.hpp

check_CXXFLAGS = -fPIC -fsanitize=address,undefined

c_wrapper_test_CXXFLAGS = $(AM_CXXFLAGS) $(TEST_DEPS_CFLAGS) $(check_CXXFLAGS)
c_wrapper_test_LDFLAGS = $(AM_LDFLAGS) $(TEST_DEPS_LIBS)
c_wrapper_test_SOURCES = c_wrapper.cpp c_wrapper_test.cpp c_wrapper_test.moc.cpp

%.moc.cpp: %.hpp
	moc -o $@ $^
