AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I@srcdir@/..
AM_CXXFLAGS = -std=c++2a -O2 -Wall -Wextra -pedantic

TESTS = $(check_PROGRAMS)
if ENABLE_TESTS
check_PROGRAMS = power_test
endif

noinst_HEADERS = \
	power.hpp \
	power_test.hpp

check_CXXFLAGS = -fPIC -fsanitize=address,undefined

power_test_CXXFLAGS = $(AM_CXXFLAGS) $(TEST_DEPS_CFLAGS) $(check_CXXFLAGS)
power_test_LDFLAGS = $(AM_LDFLAGS) $(TEST_DEPS_LIBS)
power_test_SOURCES = \
	power_test.cpp \
	power_test.moc.cpp \
	power.cpp

%.moc.cpp: %.hpp
	moc -o $@ $^
