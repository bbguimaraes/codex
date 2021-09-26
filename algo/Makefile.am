AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I@srcdir@/..
AM_CFLAGS = -std=c11 -O2 -Wall -Wextra -pedantic
AM_CXXFLAGS = \
	-std=c++2a -O2 \
	-Wall -Wextra -pedantic -Wno-missing-field-initializers

TESTS = $(check_PROGRAMS)
if ENABLE_TESTS
check_PROGRAMS = test
endif

noinst_HEADERS = \
	rotate.hpp \
	test.hpp

check_CFLAGS = -fPIC -fsanitize=address,undefined
check_CXXFLAGS = $(check_CFLAGS)

test_CXXFLAGS = $(AM_CXXFLAGS) $(TEST_DEPS_CFLAGS) $(check_CXXFLAGS)
test_LDFLAGS = $(AM_LDFLAGS) $(TEST_DEPS_LIBS)
test_SOURCES = \
	test.cpp \
	test.moc.cpp

%.moc.cpp: %.hpp
	moc -o $@ $^
