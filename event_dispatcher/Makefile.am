AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I@srcdir@/..
AM_CXXFLAGS = -std=c++2a -O2 -Wall -Wextra -pedantic

TESTS = $(check_PROGRAMS)
if ENABLE_TESTS
check_PROGRAMS = event_dispatcher_test
endif

noinst_HEADERS = event_dispatcher.hpp event_dispatcher_test.hpp

check_CXXFLAGS = -fPIC -fsanitize=address,undefined

event_dispatcher_test_CXXFLAGS = \
	$(AM_CXXFLAGS) $(TEST_DEPS_CFLAGS) $(check_CXXFLAGS)
event_dispatcher_test_LDFLAGS = $(AM_LDFLAGS) $(TEST_DEPS_LIBS)
event_dispatcher_test_SOURCES = \
	event_dispatcher.cpp event_dispatcher_test.cpp event_dispatcher_test.moc.cpp

%.moc.cpp: %.hpp
	moc -o $@ $^
