AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I@srcdir@/..
AM_CXXFLAGS = -std=c++2a -O2 -Wall -Wextra -pedantic

TESTS = $(check_PROGRAMS)
if ENABLE_TESTS
check_PROGRAMS = soa
endif

noinst_HEADERS = \
	detail/fields.hpp \
	detail/soa.hpp \
	fields.hpp \
	soa.hpp \
	utils.hpp

check_CFLAGS = -fPIC -fsanitize=address,undefined
check_CXXFLAGS = $(check_CFLAGS)

soa_CXXFLAGS = $(AM_CXXFLAGS) $(TEST_DEPS_CFLAGS) $(check_CXXFLAGS)
soa_LDFLAGS = $(AM_LDFLAGS) $(TEST_DEPS_LIBS)
soa_SOURCES = \
	detail/fields.cpp \
	detail/soa.cpp \
	fields.cpp \
	soa.cpp \
	utils.cpp
