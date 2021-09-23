AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I@srcdir@/..
AM_CFLAGS = -std=c11 -O2 -Wall -Wextra -pedantic
AM_CXXFLAGS = \
	-std=c++2a -O2 \
	-Wall -Wextra -pedantic -Wno-missing-field-initializers
AM_LDFLAGS = -lpthread

if ENABLE_BENCHMARKS
noinst_PROGRAMS = bench
endif

TESTS = $(check_PROGRAMS)
if ENABLE_TESTS
check_PROGRAMS = test
endif

noinst_HEADERS = \
	common.h \
	thread.h \
	thread0.h \
	thread1.h \
	thread2.h \
	thread3.h \
	thread4.h \
	bench.hpp \
	test.hpp

check_CFLAGS = -fPIC -fsanitize=address,undefined
check_CXXFLAGS = $(check_CFLAGS)

test_CXXFLAGS = $(AM_CXXFLAGS) $(TEST_DEPS_CFLAGS) $(check_CXXFLAGS)
test_LDFLAGS = $(AM_LDFLAGS) $(TEST_DEPS_LIBS)
test_SOURCES = \
	test.cpp \
	test.moc.cpp \
	thread.c \
	thread0.c \
	thread1.c \
	thread2.c \
	thread3.c \
	thread4.c

bench_CXXFLAGS = $(AM_CXXFLAGS) $(TEST_DEPS_CFLAGS) -fPIC
bench_LDFLAGS = $(AM_LDFLAGS) $(TEST_DEPS_LIBS)
bench_SOURCES = \
	bench.cpp \
	bench.moc.cpp \
	thread.c \
	thread0.c \
	thread1.c \
	thread2.c \
	thread3.c \
	thread4.c

%.moc.cpp: %.hpp
	moc -o $@ $^
