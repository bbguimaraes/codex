AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I@srcdir@/..
AM_CXXFLAGS = -std=c++2a -O2 -Wall -Wextra -pedantic

TESTS = $(check_PROGRAMS)
if ENABLE_TESTS
check_PROGRAMS = packet_reader_test
endif

noinst_HEADERS = packet_reader.hpp packet_reader_test.hpp

check_CXXFLAGS = -fPIC -fsanitize=address,undefined

packet_reader_test_CXXFLAGS = \
	$(AM_CXXFLAGS) $(TEST_DEPS_CFLAGS) $(check_CXXFLAGS)
packet_reader_test_LDFLAGS = $(AM_LDFLAGS) $(TEST_DEPS_LIBS)
packet_reader_test_SOURCES = \
	packet_reader.cpp packet_reader_test.cpp packet_reader_test.moc.cpp

%.moc.cpp: %.hpp
	moc -o $@ $^
